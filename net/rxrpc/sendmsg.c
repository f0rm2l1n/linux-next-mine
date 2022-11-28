// SPDX-License-Identifier: GPL-2.0-or-later
/* AF_RXRPC sendmsg() implementation.
 *
 * Copyright (C) 2007, 2016 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/net.h>
#include <linux/gfp.h>
#include <linux/skbuff.h>
#include <linux/export.h>
#include <linux/sched/signal.h>

#include <net/sock.h>
#include <net/af_rxrpc.h>
#include "ar-internal.h"

/*
 * Return true if there's sufficient Tx queue space.
 */
static bool rxrpc_check_tx_space(struct rxrpc_call *call, rxrpc_seq_t *_tx_win)
{
	unsigned int win_size;
	rxrpc_seq_t tx_win = smp_load_acquire(&call->acks_hard_ack);

	/* If we haven't transmitted anything for >1RTT, we should reset the
	 * congestion management state.
	 */
	if (ktime_before(ktime_add_us(call->tx_last_sent,
				      call->peer->srtt_us >> 3),
			 ktime_get_real())) {
		if (RXRPC_TX_SMSS > 2190)
			win_size = 2;
		else if (RXRPC_TX_SMSS > 1095)
			win_size = 3;
		else
			win_size = 4;
		win_size += call->cong_extra;
	} else {
		win_size = min_t(unsigned int, call->tx_winsize,
				 call->cong_cwnd + call->cong_extra);
	}

	if (_tx_win)
		*_tx_win = tx_win;
	return call->tx_top - tx_win < win_size;
}

/*
 * Wait for space to appear in the Tx queue or a signal to occur.
 */
static int rxrpc_wait_for_tx_window_intr(struct rxrpc_sock *rx,
					 struct rxrpc_call *call,
					 long *timeo)
{
	for (;;) {
		set_current_state(TASK_INTERRUPTIBLE);
		if (rxrpc_check_tx_space(call, NULL))
			return 0;

		if (call->state >= RXRPC_CALL_COMPLETE)
			return call->error;

		if (signal_pending(current))
			return sock_intr_errno(*timeo);

		if (READ_ONCE(call->acks_hard_ack) != call->tx_bottom) {
			rxrpc_shrink_call_tx_buffer(call);
			continue;
		}

		trace_rxrpc_txqueue(call, rxrpc_txqueue_wait);
		*timeo = schedule_timeout(*timeo);
	}
}

/*
 * Wait for space to appear in the Tx queue uninterruptibly, but with
 * a timeout of 2*RTT if no progress was made and a signal occurred.
 */
static int rxrpc_wait_for_tx_window_waitall(struct rxrpc_sock *rx,
					    struct rxrpc_call *call)
{
	rxrpc_seq_t tx_start, tx_win;
	signed long rtt, timeout;

	rtt = READ_ONCE(call->peer->srtt_us) >> 3;
	rtt = usecs_to_jiffies(rtt) * 2;
	if (rtt < 2)
		rtt = 2;

	timeout = rtt;
	tx_start = smp_load_acquire(&call->acks_hard_ack);

	for (;;) {
		set_current_state(TASK_UNINTERRUPTIBLE);

		if (rxrpc_check_tx_space(call, &tx_win))
			return 0;

		if (call->state >= RXRPC_CALL_COMPLETE)
			return call->error;

		if (timeout == 0 &&
		    tx_win == tx_start && signal_pending(current))
			return -EINTR;

		if (READ_ONCE(call->acks_hard_ack) != call->tx_bottom) {
			rxrpc_shrink_call_tx_buffer(call);
			continue;
		}

		if (tx_win != tx_start) {
			timeout = rtt;
			tx_start = tx_win;
		}

		trace_rxrpc_txqueue(call, rxrpc_txqueue_wait);
		timeout = schedule_timeout(timeout);
	}
}

/*
 * Wait for space to appear in the Tx queue uninterruptibly.
 */
static int rxrpc_wait_for_tx_window_nonintr(struct rxrpc_sock *rx,
					    struct rxrpc_call *call,
					    long *timeo)
{
	for (;;) {
		set_current_state(TASK_UNINTERRUPTIBLE);
		if (rxrpc_check_tx_space(call, NULL))
			return 0;

		if (call->state >= RXRPC_CALL_COMPLETE)
			return call->error;

		if (READ_ONCE(call->acks_hard_ack) != call->tx_bottom) {
			rxrpc_shrink_call_tx_buffer(call);
			continue;
		}

		trace_rxrpc_txqueue(call, rxrpc_txqueue_wait);
		*timeo = schedule_timeout(*timeo);
	}
}

/*
 * wait for space to appear in the transmit/ACK window
 * - caller holds the socket locked
 */
static int rxrpc_wait_for_tx_window(struct rxrpc_sock *rx,
				    struct rxrpc_call *call,
				    long *timeo,
				    bool waitall)
{
	DECLARE_WAITQUEUE(myself, current);
	int ret;

	_enter(",{%u,%u,%u,%u}",
	       call->tx_bottom, call->acks_hard_ack, call->tx_top, call->tx_winsize);

	add_wait_queue(&call->waitq, &myself);

	switch (call->interruptibility) {
	case RXRPC_INTERRUPTIBLE:
		if (waitall)
			ret = rxrpc_wait_for_tx_window_waitall(rx, call);
		else
			ret = rxrpc_wait_for_tx_window_intr(rx, call, timeo);
		break;
	case RXRPC_PREINTERRUPTIBLE:
	case RXRPC_UNINTERRUPTIBLE:
	default:
		ret = rxrpc_wait_for_tx_window_nonintr(rx, call, timeo);
		break;
	}

	remove_wait_queue(&call->waitq, &myself);
	set_current_state(TASK_RUNNING);
	_leave(" = %d", ret);
	return ret;
}

/*
 * Notify the owner of the call that the transmit phase is ended and the last
 * packet has been queued.
 */
static void rxrpc_notify_end_tx(struct rxrpc_sock *rx, struct rxrpc_call *call,
				rxrpc_notify_end_tx_t notify_end_tx)
{
	if (notify_end_tx)
		notify_end_tx(&rx->sk, call, call->user_call_ID);
}

/*
 * Queue a DATA packet for transmission, set the resend timeout and send
 * the packet immediately.  Returns the error from rxrpc_send_data_packet()
 * in case the caller wants to do something with it.
 */
static void rxrpc_queue_packet(struct rxrpc_sock *rx, struct rxrpc_call *call,
			       struct rxrpc_txbuf *txb,
			       rxrpc_notify_end_tx_t notify_end_tx)
{
	unsigned long now;
	rxrpc_seq_t seq = txb->seq;
	bool last = test_bit(RXRPC_TXBUF_LAST, &txb->flags);
	int ret;

	rxrpc_inc_stat(call->rxnet, stat_tx_data);

	ASSERTCMP(seq, ==, call->tx_top + 1);

	/* We have to set the timestamp before queueing as the retransmit
	 * algorithm can see the packet as soon as we queue it.
	 */
	txb->last_sent = ktime_get_real();

	/* Add the packet to the call's output buffer */
	rxrpc_get_txbuf(txb, rxrpc_txbuf_get_buffer);
	spin_lock(&call->tx_lock);
	list_add_tail(&txb->call_link, &call->tx_buffer);
	call->tx_top = seq;
	spin_unlock(&call->tx_lock);

	if (last)
		trace_rxrpc_txqueue(call, rxrpc_txqueue_queue_last);
	else
		trace_rxrpc_txqueue(call, rxrpc_txqueue_queue);

	if (last || call->state == RXRPC_CALL_SERVER_ACK_REQUEST) {
		_debug("________awaiting reply/ACK__________");
		write_lock_bh(&call->state_lock);
		switch (call->state) {
		case RXRPC_CALL_CLIENT_SEND_REQUEST:
			call->state = RXRPC_CALL_CLIENT_AWAIT_REPLY;
			rxrpc_notify_end_tx(rx, call, notify_end_tx);
			break;
		case RXRPC_CALL_SERVER_ACK_REQUEST:
			call->state = RXRPC_CALL_SERVER_SEND_REPLY;
			now = jiffies;
			WRITE_ONCE(call->delay_ack_at, now + MAX_JIFFY_OFFSET);
			if (call->ackr_reason == RXRPC_ACK_DELAY)
				call->ackr_reason = 0;
			trace_rxrpc_timer(call, rxrpc_timer_init_for_send_reply, now);
			if (!last)
				break;
			fallthrough;
		case RXRPC_CALL_SERVER_SEND_REPLY:
			call->state = RXRPC_CALL_SERVER_AWAIT_ACK;
			rxrpc_notify_end_tx(rx, call, notify_end_tx);
			break;
		default:
			break;
		}
		write_unlock_bh(&call->state_lock);
	}

	if (seq == 1 && rxrpc_is_client_call(call))
		rxrpc_expose_client_call(call);

	ret = rxrpc_send_data_packet(call, txb);
	if (ret < 0) {
		switch (ret) {
		case -ENETUNREACH:
		case -EHOSTUNREACH:
		case -ECONNREFUSED:
			rxrpc_set_call_completion(call, RXRPC_CALL_LOCAL_ERROR,
						  0, ret);
			goto out;
		}
	} else {
		unsigned long now = jiffies;
		unsigned long resend_at = now + call->peer->rto_j;

		WRITE_ONCE(call->resend_at, resend_at);
		rxrpc_reduce_call_timer(call, resend_at, now,
					rxrpc_timer_set_for_send);
	}

out:
	rxrpc_put_txbuf(txb, rxrpc_txbuf_put_trans);
}

/*
 * send data through a socket
 * - must be called in process context
 * - The caller holds the call user access mutex, but not the socket lock.
 */
static int rxrpc_send_data(struct rxrpc_sock *rx,
			   struct rxrpc_call *call,
			   struct msghdr *msg, size_t len,
			   rxrpc_notify_end_tx_t notify_end_tx,
			   bool *_dropped_lock)
{
	struct rxrpc_txbuf *txb;
	struct sock *sk = &rx->sk;
	enum rxrpc_call_state state;
	long timeo;
	bool more = msg->msg_flags & MSG_MORE;
	int ret, copied = 0;

	timeo = sock_sndtimeo(sk, msg->msg_flags & MSG_DONTWAIT);

	/* this should be in poll */
	sk_clear_bit(SOCKWQ_ASYNC_NOSPACE, sk);

reload:
	ret = -EPIPE;
	if (sk->sk_shutdown & SEND_SHUTDOWN)
		goto maybe_error;
	state = READ_ONCE(call->state);
	ret = -ESHUTDOWN;
	if (state >= RXRPC_CALL_COMPLETE)
		goto maybe_error;
	ret = -EPROTO;
	if (state != RXRPC_CALL_CLIENT_SEND_REQUEST &&
	    state != RXRPC_CALL_SERVER_ACK_REQUEST &&
	    state != RXRPC_CALL_SERVER_SEND_REPLY)
		goto maybe_error;

	ret = -EMSGSIZE;
	if (call->tx_total_len != -1) {
		if (len - copied > call->tx_total_len)
			goto maybe_error;
		if (!more && len - copied != call->tx_total_len)
			goto maybe_error;
	}

	txb = call->tx_pending;
	call->tx_pending = NULL;
	if (txb)
		rxrpc_see_txbuf(txb, rxrpc_txbuf_see_send_more);

	do {
		rxrpc_transmit_ack_packets(call->peer->local);

		if (!txb) {
			size_t remain, bufsize, chunk, offset;

			_debug("alloc");

			if (!rxrpc_check_tx_space(call, NULL))
				goto wait_for_space;

			/* Work out the maximum size of a packet.  Assume that
			 * the security header is going to be in the padded
			 * region (enc blocksize), but the trailer is not.
			 */
			remain = more ? INT_MAX : msg_data_left(msg);
			ret = call->conn->security->how_much_data(call, remain,
								  &bufsize, &chunk, &offset);
			if (ret < 0)
				goto maybe_error;

			_debug("SIZE: %zu/%zu @%zu", chunk, bufsize, offset);

			/* create a buffer that we can retain until it's ACK'd */
			ret = -ENOMEM;
			txb = rxrpc_alloc_txbuf(call, RXRPC_PACKET_TYPE_DATA,
						GFP_KERNEL);
			if (!txb)
				goto maybe_error;

			txb->offset = offset;
			txb->space -= offset;
			txb->space = min_t(size_t, chunk, txb->space);
		}

		_debug("append");

		/* append next segment of data to the current buffer */
		if (msg_data_left(msg) > 0) {
			size_t copy = min_t(size_t, txb->space, msg_data_left(msg));

			_debug("add %zu", copy);
			if (!copy_from_iter_full(txb->data + txb->offset, copy,
						 &msg->msg_iter))
				goto efault;
			_debug("added");
			txb->space -= copy;
			txb->len += copy;
			txb->offset += copy;
			copied += copy;
			if (call->tx_total_len != -1)
				call->tx_total_len -= copy;
		}

		/* check for the far side aborting the call or a network error
		 * occurring */
		if (call->state == RXRPC_CALL_COMPLETE)
			goto call_terminated;

		/* add the packet to the send queue if it's now full */
		if (!txb->space ||
		    (msg_data_left(msg) == 0 && !more)) {
			if (msg_data_left(msg) == 0 && !more) {
				txb->wire.flags |= RXRPC_LAST_PACKET;
				__set_bit(RXRPC_TXBUF_LAST, &txb->flags);
			}
			else if (call->tx_top - call->acks_hard_ack <
				 call->tx_winsize)
				txb->wire.flags |= RXRPC_MORE_PACKETS;

			ret = call->security->secure_packet(call, txb);
			if (ret < 0)
				goto out;

			rxrpc_queue_packet(rx, call, txb, notify_end_tx);
			txb = NULL;
		}
	} while (msg_data_left(msg) > 0);

success:
	ret = copied;
	if (READ_ONCE(call->state) == RXRPC_CALL_COMPLETE) {
		read_lock_bh(&call->state_lock);
		if (call->error < 0)
			ret = call->error;
		read_unlock_bh(&call->state_lock);
	}
out:
	call->tx_pending = txb;
	_leave(" = %d", ret);
	return ret;

call_terminated:
	rxrpc_put_txbuf(txb, rxrpc_txbuf_put_send_aborted);
	_leave(" = %d", call->error);
	return call->error;

maybe_error:
	if (copied)
		goto success;
	goto out;

efault:
	ret = -EFAULT;
	goto out;

wait_for_space:
	ret = -EAGAIN;
	if (msg->msg_flags & MSG_DONTWAIT)
		goto maybe_error;
	mutex_unlock(&call->user_mutex);
	*_dropped_lock = true;
	ret = rxrpc_wait_for_tx_window(rx, call, &timeo,
				       msg->msg_flags & MSG_WAITALL);
	if (ret < 0)
		goto maybe_error;
	if (call->interruptibility == RXRPC_INTERRUPTIBLE) {
		if (mutex_lock_interruptible(&call->user_mutex) < 0) {
			ret = sock_intr_errno(timeo);
			goto maybe_error;
		}
	} else {
		mutex_lock(&call->user_mutex);
	}
	*_dropped_lock = false;
	goto reload;
}

/*
 * extract control messages from the sendmsg() control buffer
 */
static int rxrpc_sendmsg_cmsg(struct msghdr *msg, struct rxrpc_send_params *p)
{
	struct cmsghdr *cmsg;
	bool got_user_ID = false;
	int len;

	if (msg->msg_controllen == 0)
		return -EINVAL;

	for_each_cmsghdr(cmsg, msg) {
		if (!CMSG_OK(msg, cmsg))
			return -EINVAL;

		len = cmsg->cmsg_len - sizeof(struct cmsghdr);
		_debug("CMSG %d, %d, %d",
		       cmsg->cmsg_level, cmsg->cmsg_type, len);

		if (cmsg->cmsg_level != SOL_RXRPC)
			continue;

		switch (cmsg->cmsg_type) {
		case RXRPC_USER_CALL_ID:
			if (msg->msg_flags & MSG_CMSG_COMPAT) {
				if (len != sizeof(u32))
					return -EINVAL;
				p->call.user_call_ID = *(u32 *)CMSG_DATA(cmsg);
			} else {
				if (len != sizeof(unsigned long))
					return -EINVAL;
				p->call.user_call_ID = *(unsigned long *)
					CMSG_DATA(cmsg);
			}
			got_user_ID = true;
			break;

		case RXRPC_ABORT:
			if (p->command != RXRPC_CMD_SEND_DATA)
				return -EINVAL;
			p->command = RXRPC_CMD_SEND_ABORT;
			if (len != sizeof(p->abort_code))
				return -EINVAL;
			p->abort_code = *(unsigned int *)CMSG_DATA(cmsg);
			if (p->abort_code == 0)
				return -EINVAL;
			break;

		case RXRPC_CHARGE_ACCEPT:
			if (p->command != RXRPC_CMD_SEND_DATA)
				return -EINVAL;
			p->command = RXRPC_CMD_CHARGE_ACCEPT;
			if (len != 0)
				return -EINVAL;
			break;

		case RXRPC_EXCLUSIVE_CALL:
			p->exclusive = true;
			if (len != 0)
				return -EINVAL;
			break;

		case RXRPC_UPGRADE_SERVICE:
			p->upgrade = true;
			if (len != 0)
				return -EINVAL;
			break;

		case RXRPC_TX_LENGTH:
			if (p->call.tx_total_len != -1 || len != sizeof(__s64))
				return -EINVAL;
			p->call.tx_total_len = *(__s64 *)CMSG_DATA(cmsg);
			if (p->call.tx_total_len < 0)
				return -EINVAL;
			break;

		case RXRPC_SET_CALL_TIMEOUT:
			if (len & 3 || len < 4 || len > 12)
				return -EINVAL;
			memcpy(&p->call.timeouts, CMSG_DATA(cmsg), len);
			p->call.nr_timeouts = len / 4;
			if (p->call.timeouts.hard > INT_MAX / HZ)
				return -ERANGE;
			if (p->call.nr_timeouts >= 2 && p->call.timeouts.idle > 60 * 60 * 1000)
				return -ERANGE;
			if (p->call.nr_timeouts >= 3 && p->call.timeouts.normal > 60 * 60 * 1000)
				return -ERANGE;
			break;

		default:
			return -EINVAL;
		}
	}

	if (!got_user_ID)
		return -EINVAL;
	if (p->call.tx_total_len != -1 && p->command != RXRPC_CMD_SEND_DATA)
		return -EINVAL;
	_leave(" = 0");
	return 0;
}

/*
 * Create a new client call for sendmsg().
 * - Called with the socket lock held, which it must release.
 * - If it returns a call, the call's lock will need releasing by the caller.
 */
static struct rxrpc_call *
rxrpc_new_client_call_for_sendmsg(struct rxrpc_sock *rx, struct msghdr *msg,
				  struct rxrpc_send_params *p)
	__releases(&rx->sk.sk_lock.slock)
	__acquires(&call->user_mutex)
{
	struct rxrpc_conn_parameters cp;
	struct rxrpc_call *call;
	struct key *key;

	DECLARE_SOCKADDR(struct sockaddr_rxrpc *, srx, msg->msg_name);

	_enter("");

	if (!msg->msg_name) {
		release_sock(&rx->sk);
		return ERR_PTR(-EDESTADDRREQ);
	}

	key = rx->key;
	if (key && !rx->key->payload.data[0])
		key = NULL;

	memset(&cp, 0, sizeof(cp));
	cp.local		= rx->local;
	cp.key			= rx->key;
	cp.security_level	= rx->min_sec_level;
	cp.exclusive		= rx->exclusive | p->exclusive;
	cp.upgrade		= p->upgrade;
	cp.service_id		= srx->srx_service;
	call = rxrpc_new_client_call(rx, &cp, srx, &p->call, GFP_KERNEL,
				     atomic_inc_return(&rxrpc_debug_id));
	/* The socket is now unlocked */

	rxrpc_put_peer(cp.peer);
	_leave(" = %p\n", call);
	return call;
}

/*
 * send a message forming part of a client call through an RxRPC socket
 * - caller holds the socket locked
 * - the socket may be either a client socket or a server socket
 */
int rxrpc_do_sendmsg(struct rxrpc_sock *rx, struct msghdr *msg, size_t len)
	__releases(&rx->sk.sk_lock.slock)
{
	enum rxrpc_call_state state;
	struct rxrpc_call *call;
	unsigned long now, j;
	bool dropped_lock = false;
	int ret;

	struct rxrpc_send_params p = {
		.call.tx_total_len	= -1,
		.call.user_call_ID	= 0,
		.call.nr_timeouts	= 0,
		.call.interruptibility	= RXRPC_INTERRUPTIBLE,
		.abort_code		= 0,
		.command		= RXRPC_CMD_SEND_DATA,
		.exclusive		= false,
		.upgrade		= false,
	};

	_enter("");

	ret = rxrpc_sendmsg_cmsg(msg, &p);
	if (ret < 0)
		goto error_release_sock;

	if (p.command == RXRPC_CMD_CHARGE_ACCEPT) {
		ret = -EINVAL;
		if (rx->sk.sk_state != RXRPC_SERVER_LISTENING)
			goto error_release_sock;
		ret = rxrpc_user_charge_accept(rx, p.call.user_call_ID);
		goto error_release_sock;
	}

	call = rxrpc_find_call_by_user_ID(rx, p.call.user_call_ID);
	if (!call) {
		ret = -EBADSLT;
		if (p.command != RXRPC_CMD_SEND_DATA)
			goto error_release_sock;
		call = rxrpc_new_client_call_for_sendmsg(rx, msg, &p);
		/* The socket is now unlocked... */
		if (IS_ERR(call))
			return PTR_ERR(call);
		/* ... and we have the call lock. */
		ret = 0;
		if (READ_ONCE(call->state) == RXRPC_CALL_COMPLETE)
			goto out_put_unlock;
	} else {
		switch (READ_ONCE(call->state)) {
		case RXRPC_CALL_UNINITIALISED:
		case RXRPC_CALL_CLIENT_AWAIT_CONN:
		case RXRPC_CALL_SERVER_PREALLOC:
		case RXRPC_CALL_SERVER_SECURING:
			rxrpc_put_call(call, rxrpc_call_put);
			ret = -EBUSY;
			goto error_release_sock;
		default:
			break;
		}

		ret = mutex_lock_interruptible(&call->user_mutex);
		release_sock(&rx->sk);
		if (ret < 0) {
			ret = -ERESTARTSYS;
			goto error_put;
		}

		if (p.call.tx_total_len != -1) {
			ret = -EINVAL;
			if (call->tx_total_len != -1 ||
			    call->tx_pending ||
			    call->tx_top != 0)
				goto error_put;
			call->tx_total_len = p.call.tx_total_len;
		}
	}

	switch (p.call.nr_timeouts) {
	case 3:
		j = msecs_to_jiffies(p.call.timeouts.normal);
		if (p.call.timeouts.normal > 0 && j == 0)
			j = 1;
		WRITE_ONCE(call->next_rx_timo, j);
		fallthrough;
	case 2:
		j = msecs_to_jiffies(p.call.timeouts.idle);
		if (p.call.timeouts.idle > 0 && j == 0)
			j = 1;
		WRITE_ONCE(call->next_req_timo, j);
		fallthrough;
	case 1:
		if (p.call.timeouts.hard > 0) {
			j = msecs_to_jiffies(p.call.timeouts.hard);
			now = jiffies;
			j += now;
			WRITE_ONCE(call->expect_term_by, j);
			rxrpc_reduce_call_timer(call, j, now,
						rxrpc_timer_set_for_hard);
		}
		break;
	}

	state = READ_ONCE(call->state);
	_debug("CALL %d USR %lx ST %d on CONN %p",
	       call->debug_id, call->user_call_ID, state, call->conn);

	if (state >= RXRPC_CALL_COMPLETE) {
		/* it's too late for this call */
		ret = -ESHUTDOWN;
	} else if (p.command == RXRPC_CMD_SEND_ABORT) {
		ret = 0;
		if (rxrpc_abort_call("CMD", call, 0, p.abort_code, -ECONNABORTED))
			ret = rxrpc_send_abort_packet(call);
	} else if (p.command != RXRPC_CMD_SEND_DATA) {
		ret = -EINVAL;
	} else {
		ret = rxrpc_send_data(rx, call, msg, len, NULL, &dropped_lock);
	}

out_put_unlock:
	if (!dropped_lock)
		mutex_unlock(&call->user_mutex);
error_put:
	rxrpc_put_call(call, rxrpc_call_put);
	_leave(" = %d", ret);
	return ret;

error_release_sock:
	release_sock(&rx->sk);
	return ret;
}

/**
 * rxrpc_kernel_send_data - Allow a kernel service to send data on a call
 * @sock: The socket the call is on
 * @call: The call to send data through
 * @msg: The data to send
 * @len: The amount of data to send
 * @notify_end_tx: Notification that the last packet is queued.
 *
 * Allow a kernel service to send data on a call.  The call must be in an state
 * appropriate to sending data.  No control data should be supplied in @msg,
 * nor should an address be supplied.  MSG_MORE should be flagged if there's
 * more data to come, otherwise this data will end the transmission phase.
 */
int rxrpc_kernel_send_data(struct socket *sock, struct rxrpc_call *call,
			   struct msghdr *msg, size_t len,
			   rxrpc_notify_end_tx_t notify_end_tx)
{
	bool dropped_lock = false;
	int ret;

	_enter("{%d,%s},", call->debug_id, rxrpc_call_states[call->state]);

	ASSERTCMP(msg->msg_name, ==, NULL);
	ASSERTCMP(msg->msg_control, ==, NULL);

	mutex_lock(&call->user_mutex);

	_debug("CALL %d USR %lx ST %d on CONN %p",
	       call->debug_id, call->user_call_ID, call->state, call->conn);

	switch (READ_ONCE(call->state)) {
	case RXRPC_CALL_CLIENT_SEND_REQUEST:
	case RXRPC_CALL_SERVER_ACK_REQUEST:
	case RXRPC_CALL_SERVER_SEND_REPLY:
		ret = rxrpc_send_data(rxrpc_sk(sock->sk), call, msg, len,
				      notify_end_tx, &dropped_lock);
		break;
	case RXRPC_CALL_COMPLETE:
		read_lock_bh(&call->state_lock);
		ret = call->error;
		read_unlock_bh(&call->state_lock);
		break;
	default:
		/* Request phase complete for this client call */
		trace_rxrpc_rx_eproto(call, 0, tracepoint_string("late_send"));
		ret = -EPROTO;
		break;
	}

	if (!dropped_lock)
		mutex_unlock(&call->user_mutex);
	_leave(" = %d", ret);
	return ret;
}
EXPORT_SYMBOL(rxrpc_kernel_send_data);

/**
 * rxrpc_kernel_abort_call - Allow a kernel service to abort a call
 * @sock: The socket the call is on
 * @call: The call to be aborted
 * @abort_code: The abort code to stick into the ABORT packet
 * @error: Local error value
 * @why: 3-char string indicating why.
 *
 * Allow a kernel service to abort a call, if it's still in an abortable state
 * and return true if the call was aborted, false if it was already complete.
 */
bool rxrpc_kernel_abort_call(struct socket *sock, struct rxrpc_call *call,
			     u32 abort_code, int error, const char *why)
{
	bool aborted;

	_enter("{%d},%d,%d,%s", call->debug_id, abort_code, error, why);

	mutex_lock(&call->user_mutex);

	aborted = rxrpc_abort_call(why, call, 0, abort_code, error);
	if (aborted)
		rxrpc_send_abort_packet(call);

	mutex_unlock(&call->user_mutex);
	return aborted;
}
EXPORT_SYMBOL(rxrpc_kernel_abort_call);

/**
 * rxrpc_kernel_set_tx_length - Set the total Tx length on a call
 * @sock: The socket the call is on
 * @call: The call to be informed
 * @tx_total_len: The amount of data to be transmitted for this call
 *
 * Allow a kernel service to set the total transmit length on a call.  This
 * allows buffer-to-packet encrypt-and-copy to be performed.
 *
 * This function is primarily for use for setting the reply length since the
 * request length can be set when beginning the call.
 */
void rxrpc_kernel_set_tx_length(struct socket *sock, struct rxrpc_call *call,
				s64 tx_total_len)
{
	WARN_ON(call->tx_total_len != -1);
	call->tx_total_len = tx_total_len;
}
EXPORT_SYMBOL(rxrpc_kernel_set_tx_length);
