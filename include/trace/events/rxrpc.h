/* SPDX-License-Identifier: GPL-2.0-or-later */
/* AF_RXRPC tracepoints
 *
 * Copyright (C) 2016 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM rxrpc

#if !defined(_TRACE_RXRPC_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_RXRPC_H

#include <linux/tracepoint.h>
#include <linux/errqueue.h>

/*
 * Declare tracing information enums and their string mappings for display.
 */
#define rxrpc_skb_traces \
	EM(rxrpc_skb_ack,			"ACK") \
	EM(rxrpc_skb_cleaned,			"CLN") \
	EM(rxrpc_skb_cloned_jumbo,		"CLJ") \
	EM(rxrpc_skb_freed,			"FRE") \
	EM(rxrpc_skb_got,			"GOT") \
	EM(rxrpc_skb_lost,			"*L*") \
	EM(rxrpc_skb_new,			"NEW") \
	EM(rxrpc_skb_purged,			"PUR") \
	EM(rxrpc_skb_received,			"RCV") \
	EM(rxrpc_skb_rotated,			"ROT") \
	EM(rxrpc_skb_seen,			"SEE") \
	EM(rxrpc_skb_unshared,			"UNS") \
	E_(rxrpc_skb_unshared_nomem,		"US0")

#define rxrpc_local_traces \
	EM(rxrpc_local_got,			"GOT") \
	EM(rxrpc_local_new,			"NEW") \
	EM(rxrpc_local_processing,		"PRO") \
	EM(rxrpc_local_put,			"PUT") \
	EM(rxrpc_local_queued,			"QUE") \
	E_(rxrpc_local_tx_ack,			"TAK")

#define rxrpc_peer_traces \
	EM(rxrpc_peer_got,			"GOT") \
	EM(rxrpc_peer_new,			"NEW") \
	EM(rxrpc_peer_processing,		"PRO") \
	E_(rxrpc_peer_put,			"PUT")

#define rxrpc_conn_traces \
	EM(rxrpc_conn_got,			"GOT") \
	EM(rxrpc_conn_new_client,		"NWc") \
	EM(rxrpc_conn_new_service,		"NWs") \
	EM(rxrpc_conn_put_client,		"PTc") \
	EM(rxrpc_conn_put_service,		"PTs") \
	EM(rxrpc_conn_queued,			"QUE") \
	EM(rxrpc_conn_reap_service,		"RPs") \
	E_(rxrpc_conn_seen,			"SEE")

#define rxrpc_client_traces \
	EM(rxrpc_client_activate_chans,		"Activa") \
	EM(rxrpc_client_alloc,			"Alloc ") \
	EM(rxrpc_client_chan_activate,		"ChActv") \
	EM(rxrpc_client_chan_disconnect,	"ChDisc") \
	EM(rxrpc_client_chan_pass,		"ChPass") \
	EM(rxrpc_client_chan_wait_failed,	"ChWtFl") \
	EM(rxrpc_client_cleanup,		"Clean ") \
	EM(rxrpc_client_discard,		"Discar") \
	EM(rxrpc_client_duplicate,		"Duplic") \
	EM(rxrpc_client_exposed,		"Expose") \
	EM(rxrpc_client_replace,		"Replac") \
	EM(rxrpc_client_to_active,		"->Actv") \
	E_(rxrpc_client_to_idle,		"->Idle")

#define rxrpc_call_traces \
	EM(rxrpc_call_connected,		"CON") \
	EM(rxrpc_call_error,			"*E*") \
	EM(rxrpc_call_got,			"GOT") \
	EM(rxrpc_call_got_kernel,		"Gke") \
	EM(rxrpc_call_got_timer,		"GTM") \
	EM(rxrpc_call_got_tx,			"Gtx") \
	EM(rxrpc_call_got_userid,		"Gus") \
	EM(rxrpc_call_new_client,		"NWc") \
	EM(rxrpc_call_new_service,		"NWs") \
	EM(rxrpc_call_put,			"PUT") \
	EM(rxrpc_call_put_kernel,		"Pke") \
	EM(rxrpc_call_put_noqueue,		"PnQ") \
	EM(rxrpc_call_put_notimer,		"PnT") \
	EM(rxrpc_call_put_timer,		"PTM") \
	EM(rxrpc_call_put_tx,			"Ptx") \
	EM(rxrpc_call_put_userid,		"Pus") \
	EM(rxrpc_call_queued,			"QUE") \
	EM(rxrpc_call_queued_ref,		"QUR") \
	EM(rxrpc_call_release,			"RLS") \
	E_(rxrpc_call_seen,			"SEE")

#define rxrpc_txqueue_traces \
	EM(rxrpc_txqueue_await_reply,		"AWR") \
	EM(rxrpc_txqueue_dequeue,		"DEQ") \
	EM(rxrpc_txqueue_end,			"END") \
	EM(rxrpc_txqueue_queue,			"QUE") \
	EM(rxrpc_txqueue_queue_last,		"QLS") \
	EM(rxrpc_txqueue_rotate,		"ROT") \
	EM(rxrpc_txqueue_rotate_last,		"RLS") \
	E_(rxrpc_txqueue_wait,			"WAI")

#define rxrpc_receive_traces \
	EM(rxrpc_receive_end,			"END") \
	EM(rxrpc_receive_front,			"FRN") \
	EM(rxrpc_receive_incoming,		"INC") \
	EM(rxrpc_receive_queue,			"QUE") \
	EM(rxrpc_receive_queue_last,		"QLS") \
	EM(rxrpc_receive_queue_oos,		"QUO") \
	EM(rxrpc_receive_queue_oos_last,	"QOL") \
	EM(rxrpc_receive_oos,			"OOS") \
	EM(rxrpc_receive_oos_last,		"OSL") \
	EM(rxrpc_receive_rotate,		"ROT") \
	E_(rxrpc_receive_rotate_last,		"RLS")

#define rxrpc_recvmsg_traces \
	EM(rxrpc_recvmsg_cont,			"CONT") \
	EM(rxrpc_recvmsg_data_return,		"DATA") \
	EM(rxrpc_recvmsg_dequeue,		"DEQU") \
	EM(rxrpc_recvmsg_enter,			"ENTR") \
	EM(rxrpc_recvmsg_full,			"FULL") \
	EM(rxrpc_recvmsg_hole,			"HOLE") \
	EM(rxrpc_recvmsg_next,			"NEXT") \
	EM(rxrpc_recvmsg_requeue,		"REQU") \
	EM(rxrpc_recvmsg_return,		"RETN") \
	EM(rxrpc_recvmsg_terminal,		"TERM") \
	EM(rxrpc_recvmsg_to_be_accepted,	"TBAC") \
	E_(rxrpc_recvmsg_wait,			"WAIT")

#define rxrpc_rtt_tx_traces \
	EM(rxrpc_rtt_tx_cancel,			"CNCE") \
	EM(rxrpc_rtt_tx_data,			"DATA") \
	EM(rxrpc_rtt_tx_no_slot,		"FULL") \
	E_(rxrpc_rtt_tx_ping,			"PING")

#define rxrpc_rtt_rx_traces \
	EM(rxrpc_rtt_rx_cancel,			"CNCL") \
	EM(rxrpc_rtt_rx_obsolete,		"OBSL") \
	EM(rxrpc_rtt_rx_lost,			"LOST") \
	EM(rxrpc_rtt_rx_ping_response,		"PONG") \
	E_(rxrpc_rtt_rx_requested_ack,		"RACK")

#define rxrpc_timer_traces \
	EM(rxrpc_timer_begin,			"Begin ") \
	EM(rxrpc_timer_exp_ack,			"ExpAck") \
	EM(rxrpc_timer_exp_hard,		"ExpHrd") \
	EM(rxrpc_timer_exp_idle,		"ExpIdl") \
	EM(rxrpc_timer_exp_keepalive,		"ExpKA ") \
	EM(rxrpc_timer_exp_lost_ack,		"ExpLoA") \
	EM(rxrpc_timer_exp_normal,		"ExpNml") \
	EM(rxrpc_timer_exp_ping,		"ExpPng") \
	EM(rxrpc_timer_exp_resend,		"ExpRsn") \
	EM(rxrpc_timer_init_for_reply,		"IniRpl") \
	EM(rxrpc_timer_init_for_send_reply,	"SndRpl") \
	EM(rxrpc_timer_restart,			"Restrt") \
	EM(rxrpc_timer_set_for_ack,		"SetAck") \
	EM(rxrpc_timer_set_for_hard,		"SetHrd") \
	EM(rxrpc_timer_set_for_idle,		"SetIdl") \
	EM(rxrpc_timer_set_for_keepalive,	"KeepAl") \
	EM(rxrpc_timer_set_for_lost_ack,	"SetLoA") \
	EM(rxrpc_timer_set_for_normal,		"SetNml") \
	EM(rxrpc_timer_set_for_ping,		"SetPng") \
	EM(rxrpc_timer_set_for_resend,		"SetRTx") \
	E_(rxrpc_timer_set_for_send,		"SetSnd")

#define rxrpc_propose_ack_traces \
	EM(rxrpc_propose_ack_client_tx_end,	"ClTxEnd") \
	EM(rxrpc_propose_ack_input_data,	"DataIn ") \
	EM(rxrpc_propose_ack_input_data_hole,	"DataInH") \
	EM(rxrpc_propose_ack_ping_for_check_life, "ChkLife") \
	EM(rxrpc_propose_ack_ping_for_keepalive, "KeepAlv") \
	EM(rxrpc_propose_ack_ping_for_lost_ack,	"LostAck") \
	EM(rxrpc_propose_ack_ping_for_lost_reply, "LostRpl") \
	EM(rxrpc_propose_ack_ping_for_params,	"Params ") \
	EM(rxrpc_propose_ack_processing_op,	"ProcOp ") \
	EM(rxrpc_propose_ack_respond_to_ack,	"Rsp2Ack") \
	EM(rxrpc_propose_ack_respond_to_ping,	"Rsp2Png") \
	EM(rxrpc_propose_ack_retry_tx,		"RetryTx") \
	EM(rxrpc_propose_ack_rotate_rx,		"RxAck  ") \
	E_(rxrpc_propose_ack_terminal_ack,	"ClTerm ")

#define rxrpc_congest_modes \
	EM(RXRPC_CALL_CONGEST_AVOIDANCE,	"CongAvoid") \
	EM(RXRPC_CALL_FAST_RETRANSMIT,		"FastReTx ") \
	EM(RXRPC_CALL_PACKET_LOSS,		"PktLoss  ") \
	E_(RXRPC_CALL_SLOW_START,		"SlowStart")

#define rxrpc_congest_changes \
	EM(rxrpc_cong_begin_retransmission,	" Retrans") \
	EM(rxrpc_cong_cleared_nacks,		" Cleared") \
	EM(rxrpc_cong_new_low_nack,		" NewLowN") \
	EM(rxrpc_cong_no_change,		" -") \
	EM(rxrpc_cong_progress,			" Progres") \
	EM(rxrpc_cong_idle_reset,		" IdleRes") \
	EM(rxrpc_cong_retransmit_again,		" ReTxAgn") \
	EM(rxrpc_cong_rtt_window_end,		" RttWinE") \
	E_(rxrpc_cong_saw_nack,			" SawNack")

#define rxrpc_pkts \
	EM(0,					"?00") \
	EM(RXRPC_PACKET_TYPE_DATA,		"DATA") \
	EM(RXRPC_PACKET_TYPE_ACK,		"ACK") \
	EM(RXRPC_PACKET_TYPE_BUSY,		"BUSY") \
	EM(RXRPC_PACKET_TYPE_ABORT,		"ABORT") \
	EM(RXRPC_PACKET_TYPE_ACKALL,		"ACKALL") \
	EM(RXRPC_PACKET_TYPE_CHALLENGE,		"CHALL") \
	EM(RXRPC_PACKET_TYPE_RESPONSE,		"RESP") \
	EM(RXRPC_PACKET_TYPE_DEBUG,		"DEBUG") \
	EM(9,					"?09") \
	EM(10,					"?10") \
	EM(11,					"?11") \
	EM(12,					"?12") \
	EM(RXRPC_PACKET_TYPE_VERSION,		"VERSION") \
	EM(14,					"?14") \
	E_(15,					"?15")

#define rxrpc_ack_names \
	EM(0,					"-0-") \
	EM(RXRPC_ACK_REQUESTED,			"REQ") \
	EM(RXRPC_ACK_DUPLICATE,			"DUP") \
	EM(RXRPC_ACK_OUT_OF_SEQUENCE,		"OOS") \
	EM(RXRPC_ACK_EXCEEDS_WINDOW,		"WIN") \
	EM(RXRPC_ACK_NOSPACE,			"MEM") \
	EM(RXRPC_ACK_PING,			"PNG") \
	EM(RXRPC_ACK_PING_RESPONSE,		"PNR") \
	EM(RXRPC_ACK_DELAY,			"DLY") \
	EM(RXRPC_ACK_IDLE,			"IDL") \
	E_(RXRPC_ACK__INVALID,			"-?-")

#define rxrpc_completions \
	EM(RXRPC_CALL_SUCCEEDED,		"Succeeded") \
	EM(RXRPC_CALL_REMOTELY_ABORTED,		"RemoteAbort") \
	EM(RXRPC_CALL_LOCALLY_ABORTED,		"LocalAbort") \
	EM(RXRPC_CALL_LOCAL_ERROR,		"LocalError") \
	E_(RXRPC_CALL_NETWORK_ERROR,		"NetError")

#define rxrpc_tx_points \
	EM(rxrpc_tx_point_call_abort,		"CallAbort") \
	EM(rxrpc_tx_point_call_ack,		"CallAck") \
	EM(rxrpc_tx_point_call_data_frag,	"CallDataFrag") \
	EM(rxrpc_tx_point_call_data_nofrag,	"CallDataNofrag") \
	EM(rxrpc_tx_point_call_final_resend,	"CallFinalResend") \
	EM(rxrpc_tx_point_conn_abort,		"ConnAbort") \
	EM(rxrpc_tx_point_reject,		"Reject") \
	EM(rxrpc_tx_point_rxkad_challenge,	"RxkadChall") \
	EM(rxrpc_tx_point_rxkad_response,	"RxkadResp") \
	EM(rxrpc_tx_point_version_keepalive,	"VerKeepalive") \
	E_(rxrpc_tx_point_version_reply,	"VerReply")

#define rxrpc_req_ack_traces \
	EM(rxrpc_reqack_ack_lost,		"ACK-LOST  ")	\
	EM(rxrpc_reqack_already_on,		"ALREADY-ON")	\
	EM(rxrpc_reqack_more_rtt,		"MORE-RTT  ")	\
	EM(rxrpc_reqack_no_srv_last,		"NO-SRVLAST")	\
	EM(rxrpc_reqack_old_rtt,		"OLD-RTT   ")	\
	EM(rxrpc_reqack_retrans,		"RETRANS   ")	\
	EM(rxrpc_reqack_slow_start,		"SLOW-START")	\
	E_(rxrpc_reqack_small_txwin,		"SMALL-TXWN")
/* ---- Must update size of stat_why_req_ack[] if more are added! */

#define rxrpc_txbuf_traces \
	EM(rxrpc_txbuf_alloc_ack,		"ALLOC ACK  ")	\
	EM(rxrpc_txbuf_alloc_data,		"ALLOC DATA ")	\
	EM(rxrpc_txbuf_free,			"FREE       ")	\
	EM(rxrpc_txbuf_get_buffer,		"GET BUFFER ")	\
	EM(rxrpc_txbuf_get_trans,		"GET TRANS  ")	\
	EM(rxrpc_txbuf_get_retrans,		"GET RETRANS")	\
	EM(rxrpc_txbuf_put_ack_tx,		"PUT ACK TX ")	\
	EM(rxrpc_txbuf_put_cleaned,		"PUT CLEANED")	\
	EM(rxrpc_txbuf_put_nomem,		"PUT NOMEM  ")	\
	EM(rxrpc_txbuf_put_rotated,		"PUT ROTATED")	\
	EM(rxrpc_txbuf_put_send_aborted,	"PUT SEND-X ")	\
	EM(rxrpc_txbuf_put_trans,		"PUT TRANS  ")	\
	EM(rxrpc_txbuf_see_send_more,		"SEE SEND+  ")	\
	E_(rxrpc_txbuf_see_unacked,		"SEE UNACKED")

/*
 * Generate enums for tracing information.
 */
#ifndef __NETFS_DECLARE_TRACE_ENUMS_ONCE_ONLY
#define __NETFS_DECLARE_TRACE_ENUMS_ONCE_ONLY

#undef EM
#undef E_
#define EM(a, b) a,
#define E_(a, b) a

enum rxrpc_call_trace		{ rxrpc_call_traces } __mode(byte);
enum rxrpc_client_trace		{ rxrpc_client_traces } __mode(byte);
enum rxrpc_congest_change	{ rxrpc_congest_changes } __mode(byte);
enum rxrpc_conn_trace		{ rxrpc_conn_traces } __mode(byte);
enum rxrpc_local_trace		{ rxrpc_local_traces } __mode(byte);
enum rxrpc_peer_trace		{ rxrpc_peer_traces } __mode(byte);
enum rxrpc_propose_ack_outcome	{ rxrpc_propose_ack_outcomes } __mode(byte);
enum rxrpc_propose_ack_trace	{ rxrpc_propose_ack_traces } __mode(byte);
enum rxrpc_receive_trace	{ rxrpc_receive_traces } __mode(byte);
enum rxrpc_recvmsg_trace	{ rxrpc_recvmsg_traces } __mode(byte);
enum rxrpc_req_ack_trace	{ rxrpc_req_ack_traces } __mode(byte);
enum rxrpc_rtt_rx_trace		{ rxrpc_rtt_rx_traces } __mode(byte);
enum rxrpc_rtt_tx_trace		{ rxrpc_rtt_tx_traces } __mode(byte);
enum rxrpc_skb_trace		{ rxrpc_skb_traces } __mode(byte);
enum rxrpc_timer_trace		{ rxrpc_timer_traces } __mode(byte);
enum rxrpc_tx_point		{ rxrpc_tx_points } __mode(byte);
enum rxrpc_txbuf_trace		{ rxrpc_txbuf_traces } __mode(byte);
enum rxrpc_txqueue_trace	{ rxrpc_txqueue_traces } __mode(byte);

#endif /* end __RXRPC_DECLARE_TRACE_ENUMS_ONCE_ONLY */

/*
 * Export enum symbols via userspace.
 */
#undef EM
#undef E_
#define EM(a, b) TRACE_DEFINE_ENUM(a);
#define E_(a, b) TRACE_DEFINE_ENUM(a);

rxrpc_call_traces;
rxrpc_client_traces;
rxrpc_congest_changes;
rxrpc_congest_modes;
rxrpc_conn_traces;
rxrpc_local_traces;
rxrpc_propose_ack_traces;
rxrpc_receive_traces;
rxrpc_recvmsg_traces;
rxrpc_req_ack_traces;
rxrpc_rtt_rx_traces;
rxrpc_rtt_tx_traces;
rxrpc_skb_traces;
rxrpc_timer_traces;
rxrpc_tx_points;
rxrpc_txbuf_traces;
rxrpc_txqueue_traces;

/*
 * Now redefine the EM() and E_() macros to map the enums to the strings that
 * will be printed in the output.
 */
#undef EM
#undef E_
#define EM(a, b)	{ a, b },
#define E_(a, b)	{ a, b }

TRACE_EVENT(rxrpc_local,
	    TP_PROTO(unsigned int local_debug_id, enum rxrpc_local_trace op,
		     int usage, const void *where),

	    TP_ARGS(local_debug_id, op, usage, where),

	    TP_STRUCT__entry(
		    __field(unsigned int,	local		)
		    __field(int,		op		)
		    __field(int,		usage		)
		    __field(const void *,	where		)
			     ),

	    TP_fast_assign(
		    __entry->local = local_debug_id;
		    __entry->op = op;
		    __entry->usage = usage;
		    __entry->where = where;
			   ),

	    TP_printk("L=%08x %s u=%d sp=%pSR",
		      __entry->local,
		      __print_symbolic(__entry->op, rxrpc_local_traces),
		      __entry->usage,
		      __entry->where)
	    );

TRACE_EVENT(rxrpc_peer,
	    TP_PROTO(unsigned int peer_debug_id, enum rxrpc_peer_trace op,
		     int usage, const void *where),

	    TP_ARGS(peer_debug_id, op, usage, where),

	    TP_STRUCT__entry(
		    __field(unsigned int,	peer		)
		    __field(int,		op		)
		    __field(int,		usage		)
		    __field(const void *,	where		)
			     ),

	    TP_fast_assign(
		    __entry->peer = peer_debug_id;
		    __entry->op = op;
		    __entry->usage = usage;
		    __entry->where = where;
			   ),

	    TP_printk("P=%08x %s u=%d sp=%pSR",
		      __entry->peer,
		      __print_symbolic(__entry->op, rxrpc_peer_traces),
		      __entry->usage,
		      __entry->where)
	    );

TRACE_EVENT(rxrpc_conn,
	    TP_PROTO(unsigned int conn_debug_id, enum rxrpc_conn_trace op,
		     int usage, const void *where),

	    TP_ARGS(conn_debug_id, op, usage, where),

	    TP_STRUCT__entry(
		    __field(unsigned int,	conn		)
		    __field(int,		op		)
		    __field(int,		usage		)
		    __field(const void *,	where		)
			     ),

	    TP_fast_assign(
		    __entry->conn = conn_debug_id;
		    __entry->op = op;
		    __entry->usage = usage;
		    __entry->where = where;
			   ),

	    TP_printk("C=%08x %s u=%d sp=%pSR",
		      __entry->conn,
		      __print_symbolic(__entry->op, rxrpc_conn_traces),
		      __entry->usage,
		      __entry->where)
	    );

TRACE_EVENT(rxrpc_client,
	    TP_PROTO(struct rxrpc_connection *conn, int channel,
		     enum rxrpc_client_trace op),

	    TP_ARGS(conn, channel, op),

	    TP_STRUCT__entry(
		    __field(unsigned int,		conn		)
		    __field(u32,			cid		)
		    __field(int,			channel		)
		    __field(int,			usage		)
		    __field(enum rxrpc_client_trace,	op		)
			     ),

	    TP_fast_assign(
		    __entry->conn = conn ? conn->debug_id : 0;
		    __entry->channel = channel;
		    __entry->usage = conn ? refcount_read(&conn->ref) : -2;
		    __entry->op = op;
		    __entry->cid = conn ? conn->proto.cid : 0;
			   ),

	    TP_printk("C=%08x h=%2d %s i=%08x u=%d",
		      __entry->conn,
		      __entry->channel,
		      __print_symbolic(__entry->op, rxrpc_client_traces),
		      __entry->cid,
		      __entry->usage)
	    );

TRACE_EVENT(rxrpc_call,
	    TP_PROTO(unsigned int call_debug_id, enum rxrpc_call_trace op,
		     int usage, const void *where, const void *aux),

	    TP_ARGS(call_debug_id, op, usage, where, aux),

	    TP_STRUCT__entry(
		    __field(unsigned int,		call		)
		    __field(int,			op		)
		    __field(int,			usage		)
		    __field(const void *,		where		)
		    __field(const void *,		aux		)
			     ),

	    TP_fast_assign(
		    __entry->call = call_debug_id;
		    __entry->op = op;
		    __entry->usage = usage;
		    __entry->where = where;
		    __entry->aux = aux;
			   ),

	    TP_printk("c=%08x %s u=%d sp=%pSR a=%p",
		      __entry->call,
		      __print_symbolic(__entry->op, rxrpc_call_traces),
		      __entry->usage,
		      __entry->where,
		      __entry->aux)
	    );

TRACE_EVENT(rxrpc_skb,
	    TP_PROTO(struct sk_buff *skb, enum rxrpc_skb_trace op,
		     int usage, int mod_count, const void *where),

	    TP_ARGS(skb, op, usage, mod_count, where),

	    TP_STRUCT__entry(
		    __field(struct sk_buff *,		skb		)
		    __field(enum rxrpc_skb_trace,	op		)
		    __field(int,			usage		)
		    __field(int,			mod_count	)
		    __field(const void *,		where		)
			     ),

	    TP_fast_assign(
		    __entry->skb = skb;
		    __entry->op = op;
		    __entry->usage = usage;
		    __entry->mod_count = mod_count;
		    __entry->where = where;
			   ),

	    TP_printk("s=%p Rx %s u=%d m=%d p=%pSR",
		      __entry->skb,
		      __print_symbolic(__entry->op, rxrpc_skb_traces),
		      __entry->usage,
		      __entry->mod_count,
		      __entry->where)
	    );

TRACE_EVENT(rxrpc_rx_packet,
	    TP_PROTO(struct rxrpc_skb_priv *sp),

	    TP_ARGS(sp),

	    TP_STRUCT__entry(
		    __field_struct(struct rxrpc_host_header,	hdr		)
			     ),

	    TP_fast_assign(
		    memcpy(&__entry->hdr, &sp->hdr, sizeof(__entry->hdr));
			   ),

	    TP_printk("%08x:%08x:%08x:%04x %08x %08x %02x %02x %s",
		      __entry->hdr.epoch, __entry->hdr.cid,
		      __entry->hdr.callNumber, __entry->hdr.serviceId,
		      __entry->hdr.serial, __entry->hdr.seq,
		      __entry->hdr.type, __entry->hdr.flags,
		      __entry->hdr.type <= 15 ?
		      __print_symbolic(__entry->hdr.type, rxrpc_pkts) : "?UNK")
	    );

TRACE_EVENT(rxrpc_rx_done,
	    TP_PROTO(int result, int abort_code),

	    TP_ARGS(result, abort_code),

	    TP_STRUCT__entry(
		    __field(int,			result		)
		    __field(int,			abort_code	)
			     ),

	    TP_fast_assign(
		    __entry->result = result;
		    __entry->abort_code = abort_code;
			   ),

	    TP_printk("r=%d a=%d", __entry->result, __entry->abort_code)
	    );

TRACE_EVENT(rxrpc_abort,
	    TP_PROTO(unsigned int call_nr, const char *why, u32 cid, u32 call_id,
		     rxrpc_seq_t seq, int abort_code, int error),

	    TP_ARGS(call_nr, why, cid, call_id, seq, abort_code, error),

	    TP_STRUCT__entry(
		    __field(unsigned int,		call_nr		)
		    __array(char,			why, 4		)
		    __field(u32,			cid		)
		    __field(u32,			call_id		)
		    __field(rxrpc_seq_t,		seq		)
		    __field(int,			abort_code	)
		    __field(int,			error		)
			     ),

	    TP_fast_assign(
		    memcpy(__entry->why, why, 4);
		    __entry->call_nr = call_nr;
		    __entry->cid = cid;
		    __entry->call_id = call_id;
		    __entry->abort_code = abort_code;
		    __entry->error = error;
		    __entry->seq = seq;
			   ),

	    TP_printk("c=%08x %08x:%08x s=%u a=%d e=%d %s",
		      __entry->call_nr,
		      __entry->cid, __entry->call_id, __entry->seq,
		      __entry->abort_code, __entry->error, __entry->why)
	    );

TRACE_EVENT(rxrpc_call_complete,
	    TP_PROTO(struct rxrpc_call *call),

	    TP_ARGS(call),

	    TP_STRUCT__entry(
		    __field(unsigned int,		call		)
		    __field(enum rxrpc_call_completion,	compl		)
		    __field(int,			error		)
		    __field(u32,			abort_code	)
			     ),

	    TP_fast_assign(
		    __entry->call = call->debug_id;
		    __entry->compl = call->completion;
		    __entry->error = call->error;
		    __entry->abort_code = call->abort_code;
			   ),

	    TP_printk("c=%08x %s r=%d ac=%d",
		      __entry->call,
		      __print_symbolic(__entry->compl, rxrpc_completions),
		      __entry->error,
		      __entry->abort_code)
	    );

TRACE_EVENT(rxrpc_txqueue,
	    TP_PROTO(struct rxrpc_call *call, enum rxrpc_txqueue_trace why),

	    TP_ARGS(call, why),

	    TP_STRUCT__entry(
		    __field(unsigned int,		call		)
		    __field(enum rxrpc_txqueue_trace,	why		)
		    __field(rxrpc_seq_t,		acks_hard_ack	)
		    __field(rxrpc_seq_t,		tx_bottom	)
		    __field(rxrpc_seq_t,		tx_top		)
		    __field(int,			tx_winsize	)
			     ),

	    TP_fast_assign(
		    __entry->call = call->debug_id;
		    __entry->why = why;
		    __entry->acks_hard_ack = call->acks_hard_ack;
		    __entry->tx_bottom = call->tx_bottom;
		    __entry->tx_top = call->tx_top;
		    __entry->tx_winsize = call->tx_winsize;
			   ),

	    TP_printk("c=%08x %s f=%08x h=%08x n=%u/%u/%u",
		      __entry->call,
		      __print_symbolic(__entry->why, rxrpc_txqueue_traces),
		      __entry->tx_bottom,
		      __entry->acks_hard_ack,
		      __entry->tx_top - __entry->tx_bottom,
		      __entry->tx_top - __entry->acks_hard_ack,
		      __entry->tx_winsize)
	    );

TRACE_EVENT(rxrpc_rx_data,
	    TP_PROTO(unsigned int call, rxrpc_seq_t seq,
		     rxrpc_serial_t serial, u8 flags),

	    TP_ARGS(call, seq, serial, flags),

	    TP_STRUCT__entry(
		    __field(unsigned int,		call		)
		    __field(rxrpc_seq_t,		seq		)
		    __field(rxrpc_serial_t,		serial		)
		    __field(u8,				flags		)
			     ),

	    TP_fast_assign(
		    __entry->call = call;
		    __entry->seq = seq;
		    __entry->serial = serial;
		    __entry->flags = flags;
			   ),

	    TP_printk("c=%08x DATA %08x q=%08x fl=%02x",
		      __entry->call,
		      __entry->serial,
		      __entry->seq,
		      __entry->flags)
	    );

TRACE_EVENT(rxrpc_rx_ack,
	    TP_PROTO(struct rxrpc_call *call,
		     rxrpc_serial_t serial, rxrpc_serial_t ack_serial,
		     rxrpc_seq_t first, rxrpc_seq_t prev, u8 reason, u8 n_acks),

	    TP_ARGS(call, serial, ack_serial, first, prev, reason, n_acks),

	    TP_STRUCT__entry(
		    __field(unsigned int,		call		)
		    __field(rxrpc_serial_t,		serial		)
		    __field(rxrpc_serial_t,		ack_serial	)
		    __field(rxrpc_seq_t,		first		)
		    __field(rxrpc_seq_t,		prev		)
		    __field(u8,				reason		)
		    __field(u8,				n_acks		)
			     ),

	    TP_fast_assign(
		    __entry->call = call->debug_id;
		    __entry->serial = serial;
		    __entry->ack_serial = ack_serial;
		    __entry->first = first;
		    __entry->prev = prev;
		    __entry->reason = reason;
		    __entry->n_acks = n_acks;
			   ),

	    TP_printk("c=%08x %08x %s r=%08x f=%08x p=%08x n=%u",
		      __entry->call,
		      __entry->serial,
		      __print_symbolic(__entry->reason, rxrpc_ack_names),
		      __entry->ack_serial,
		      __entry->first,
		      __entry->prev,
		      __entry->n_acks)
	    );

TRACE_EVENT(rxrpc_rx_abort,
	    TP_PROTO(struct rxrpc_call *call, rxrpc_serial_t serial,
		     u32 abort_code),

	    TP_ARGS(call, serial, abort_code),

	    TP_STRUCT__entry(
		    __field(unsigned int,		call		)
		    __field(rxrpc_serial_t,		serial		)
		    __field(u32,			abort_code	)
			     ),

	    TP_fast_assign(
		    __entry->call = call->debug_id;
		    __entry->serial = serial;
		    __entry->abort_code = abort_code;
			   ),

	    TP_printk("c=%08x ABORT %08x ac=%d",
		      __entry->call,
		      __entry->serial,
		      __entry->abort_code)
	    );

TRACE_EVENT(rxrpc_rx_rwind_change,
	    TP_PROTO(struct rxrpc_call *call, rxrpc_serial_t serial,
		     u32 rwind, bool wake),

	    TP_ARGS(call, serial, rwind, wake),

	    TP_STRUCT__entry(
		    __field(unsigned int,		call		)
		    __field(rxrpc_serial_t,		serial		)
		    __field(u32,			rwind		)
		    __field(bool,			wake		)
			     ),

	    TP_fast_assign(
		    __entry->call = call->debug_id;
		    __entry->serial = serial;
		    __entry->rwind = rwind;
		    __entry->wake = wake;
			   ),

	    TP_printk("c=%08x %08x rw=%u%s",
		      __entry->call,
		      __entry->serial,
		      __entry->rwind,
		      __entry->wake ? " wake" : "")
	    );

TRACE_EVENT(rxrpc_tx_packet,
	    TP_PROTO(unsigned int call_id, struct rxrpc_wire_header *whdr,
		     enum rxrpc_tx_point where),

	    TP_ARGS(call_id, whdr, where),

	    TP_STRUCT__entry(
		    __field(unsigned int,			call	)
		    __field(enum rxrpc_tx_point,		where	)
		    __field_struct(struct rxrpc_wire_header,	whdr	)
			     ),

	    TP_fast_assign(
		    __entry->call = call_id;
		    memcpy(&__entry->whdr, whdr, sizeof(__entry->whdr));
		    __entry->where = where;
			   ),

	    TP_printk("c=%08x %08x:%08x:%08x:%04x %08x %08x %02x %02x %s %s",
		      __entry->call,
		      ntohl(__entry->whdr.epoch),
		      ntohl(__entry->whdr.cid),
		      ntohl(__entry->whdr.callNumber),
		      ntohs(__entry->whdr.serviceId),
		      ntohl(__entry->whdr.serial),
		      ntohl(__entry->whdr.seq),
		      __entry->whdr.type, __entry->whdr.flags,
		      __entry->whdr.type <= 15 ?
		      __print_symbolic(__entry->whdr.type, rxrpc_pkts) : "?UNK",
		      __print_symbolic(__entry->where, rxrpc_tx_points))
	    );

TRACE_EVENT(rxrpc_tx_data,
	    TP_PROTO(struct rxrpc_call *call, rxrpc_seq_t seq,
		     rxrpc_serial_t serial, u8 flags, bool retrans, bool lose),

	    TP_ARGS(call, seq, serial, flags, retrans, lose),

	    TP_STRUCT__entry(
		    __field(unsigned int,		call		)
		    __field(rxrpc_seq_t,		seq		)
		    __field(rxrpc_serial_t,		serial		)
		    __field(u32,			cid		)
		    __field(u32,			call_id		)
		    __field(u8,				flags		)
		    __field(bool,			retrans		)
		    __field(bool,			lose		)
			     ),

	    TP_fast_assign(
		    __entry->call = call->debug_id;
		    __entry->cid = call->cid;
		    __entry->call_id = call->call_id;
		    __entry->seq = seq;
		    __entry->serial = serial;
		    __entry->flags = flags;
		    __entry->retrans = retrans;
		    __entry->lose = lose;
			   ),

	    TP_printk("c=%08x DATA %08x:%08x %08x q=%08x fl=%02x%s%s",
		      __entry->call,
		      __entry->cid,
		      __entry->call_id,
		      __entry->serial,
		      __entry->seq,
		      __entry->flags,
		      __entry->retrans ? " *RETRANS*" : "",
		      __entry->lose ? " *LOSE*" : "")
	    );

TRACE_EVENT(rxrpc_tx_ack,
	    TP_PROTO(unsigned int call, rxrpc_serial_t serial,
		     rxrpc_seq_t ack_first, rxrpc_serial_t ack_serial,
		     u8 reason, u8 n_acks),

	    TP_ARGS(call, serial, ack_first, ack_serial, reason, n_acks),

	    TP_STRUCT__entry(
		    __field(unsigned int,		call		)
		    __field(rxrpc_serial_t,		serial		)
		    __field(rxrpc_seq_t,		ack_first	)
		    __field(rxrpc_serial_t,		ack_serial	)
		    __field(u8,				reason		)
		    __field(u8,				n_acks		)
			     ),

	    TP_fast_assign(
		    __entry->call = call;
		    __entry->serial = serial;
		    __entry->ack_first = ack_first;
		    __entry->ack_serial = ack_serial;
		    __entry->reason = reason;
		    __entry->n_acks = n_acks;
			   ),

	    TP_printk(" c=%08x ACK  %08x %s f=%08x r=%08x n=%u",
		      __entry->call,
		      __entry->serial,
		      __print_symbolic(__entry->reason, rxrpc_ack_names),
		      __entry->ack_first,
		      __entry->ack_serial,
		      __entry->n_acks)
	    );

TRACE_EVENT(rxrpc_receive,
	    TP_PROTO(struct rxrpc_call *call, enum rxrpc_receive_trace why,
		     rxrpc_serial_t serial, rxrpc_seq_t seq),

	    TP_ARGS(call, why, serial, seq),

	    TP_STRUCT__entry(
		    __field(unsigned int,		call		)
		    __field(enum rxrpc_receive_trace,	why		)
		    __field(rxrpc_serial_t,		serial		)
		    __field(rxrpc_seq_t,		seq		)
		    __field(u64,			window		)
			     ),

	    TP_fast_assign(
		    __entry->call = call->debug_id;
		    __entry->why = why;
		    __entry->serial = serial;
		    __entry->seq = seq;
		    __entry->window = atomic64_read(&call->ackr_window);
			   ),

	    TP_printk("c=%08x %s r=%08x q=%08x w=%08x-%08x",
		      __entry->call,
		      __print_symbolic(__entry->why, rxrpc_receive_traces),
		      __entry->serial,
		      __entry->seq,
		      lower_32_bits(__entry->window),
		      upper_32_bits(__entry->window))
	    );

TRACE_EVENT(rxrpc_recvmsg,
	    TP_PROTO(struct rxrpc_call *call, enum rxrpc_recvmsg_trace why,
		     int ret),

	    TP_ARGS(call, why, ret),

	    TP_STRUCT__entry(
		    __field(unsigned int,		call		)
		    __field(enum rxrpc_recvmsg_trace,	why		)
		    __field(int,			ret		)
			     ),

	    TP_fast_assign(
		    __entry->call = call ? call->debug_id : 0;
		    __entry->why = why;
		    __entry->ret = ret;
			   ),

	    TP_printk("c=%08x %s ret=%d",
		      __entry->call,
		      __print_symbolic(__entry->why, rxrpc_recvmsg_traces),
		      __entry->ret)
	    );

TRACE_EVENT(rxrpc_recvdata,
	    TP_PROTO(struct rxrpc_call *call, enum rxrpc_recvmsg_trace why,
		     rxrpc_seq_t seq, unsigned int offset, unsigned int len,
		     int ret),

	    TP_ARGS(call, why, seq, offset, len, ret),

	    TP_STRUCT__entry(
		    __field(unsigned int,		call		)
		    __field(enum rxrpc_recvmsg_trace,	why		)
		    __field(rxrpc_seq_t,		seq		)
		    __field(unsigned int,		offset		)
		    __field(unsigned int,		len		)
		    __field(int,			ret		)
			     ),

	    TP_fast_assign(
		    __entry->call = call ? call->debug_id : 0;
		    __entry->why = why;
		    __entry->seq = seq;
		    __entry->offset = offset;
		    __entry->len = len;
		    __entry->ret = ret;
			   ),

	    TP_printk("c=%08x %s q=%08x o=%u l=%u ret=%d",
		      __entry->call,
		      __print_symbolic(__entry->why, rxrpc_recvmsg_traces),
		      __entry->seq,
		      __entry->offset,
		      __entry->len,
		      __entry->ret)
	    );

TRACE_EVENT(rxrpc_rtt_tx,
	    TP_PROTO(struct rxrpc_call *call, enum rxrpc_rtt_tx_trace why,
		     int slot, rxrpc_serial_t send_serial),

	    TP_ARGS(call, why, slot, send_serial),

	    TP_STRUCT__entry(
		    __field(unsigned int,		call		)
		    __field(enum rxrpc_rtt_tx_trace,	why		)
		    __field(int,			slot		)
		    __field(rxrpc_serial_t,		send_serial	)
			     ),

	    TP_fast_assign(
		    __entry->call = call->debug_id;
		    __entry->why = why;
		    __entry->slot = slot;
		    __entry->send_serial = send_serial;
			   ),

	    TP_printk("c=%08x [%d] %s sr=%08x",
		      __entry->call,
		      __entry->slot,
		      __print_symbolic(__entry->why, rxrpc_rtt_tx_traces),
		      __entry->send_serial)
	    );

TRACE_EVENT(rxrpc_rtt_rx,
	    TP_PROTO(struct rxrpc_call *call, enum rxrpc_rtt_rx_trace why,
		     int slot,
		     rxrpc_serial_t send_serial, rxrpc_serial_t resp_serial,
		     u32 rtt, u32 rto),

	    TP_ARGS(call, why, slot, send_serial, resp_serial, rtt, rto),

	    TP_STRUCT__entry(
		    __field(unsigned int,		call		)
		    __field(enum rxrpc_rtt_rx_trace,	why		)
		    __field(int,			slot		)
		    __field(rxrpc_serial_t,		send_serial	)
		    __field(rxrpc_serial_t,		resp_serial	)
		    __field(u32,			rtt		)
		    __field(u32,			rto		)
			     ),

	    TP_fast_assign(
		    __entry->call = call->debug_id;
		    __entry->why = why;
		    __entry->slot = slot;
		    __entry->send_serial = send_serial;
		    __entry->resp_serial = resp_serial;
		    __entry->rtt = rtt;
		    __entry->rto = rto;
			   ),

	    TP_printk("c=%08x [%d] %s sr=%08x rr=%08x rtt=%u rto=%u",
		      __entry->call,
		      __entry->slot,
		      __print_symbolic(__entry->why, rxrpc_rtt_rx_traces),
		      __entry->send_serial,
		      __entry->resp_serial,
		      __entry->rtt,
		      __entry->rto)
	    );

TRACE_EVENT(rxrpc_timer,
	    TP_PROTO(struct rxrpc_call *call, enum rxrpc_timer_trace why,
		     unsigned long now),

	    TP_ARGS(call, why, now),

	    TP_STRUCT__entry(
		    __field(unsigned int,			call		)
		    __field(enum rxrpc_timer_trace,		why		)
		    __field(long,				now		)
		    __field(long,				ack_at		)
		    __field(long,				ack_lost_at	)
		    __field(long,				resend_at	)
		    __field(long,				ping_at		)
		    __field(long,				expect_rx_by	)
		    __field(long,				expect_req_by	)
		    __field(long,				expect_term_by	)
		    __field(long,				timer		)
			     ),

	    TP_fast_assign(
		    __entry->call		= call->debug_id;
		    __entry->why		= why;
		    __entry->now		= now;
		    __entry->ack_at		= call->delay_ack_at;
		    __entry->ack_lost_at	= call->ack_lost_at;
		    __entry->resend_at		= call->resend_at;
		    __entry->expect_rx_by	= call->expect_rx_by;
		    __entry->expect_req_by	= call->expect_req_by;
		    __entry->expect_term_by	= call->expect_term_by;
		    __entry->timer		= call->timer.expires;
			   ),

	    TP_printk("c=%08x %s a=%ld la=%ld r=%ld xr=%ld xq=%ld xt=%ld t=%ld",
		      __entry->call,
		      __print_symbolic(__entry->why, rxrpc_timer_traces),
		      __entry->ack_at - __entry->now,
		      __entry->ack_lost_at - __entry->now,
		      __entry->resend_at - __entry->now,
		      __entry->expect_rx_by - __entry->now,
		      __entry->expect_req_by - __entry->now,
		      __entry->expect_term_by - __entry->now,
		      __entry->timer - __entry->now)
	    );

TRACE_EVENT(rxrpc_timer_expired,
	    TP_PROTO(struct rxrpc_call *call, unsigned long now),

	    TP_ARGS(call, now),

	    TP_STRUCT__entry(
		    __field(unsigned int,			call		)
		    __field(long,				now		)
		    __field(long,				ack_at		)
		    __field(long,				ack_lost_at	)
		    __field(long,				resend_at	)
		    __field(long,				ping_at		)
		    __field(long,				expect_rx_by	)
		    __field(long,				expect_req_by	)
		    __field(long,				expect_term_by	)
		    __field(long,				timer		)
			     ),

	    TP_fast_assign(
		    __entry->call		= call->debug_id;
		    __entry->now		= now;
		    __entry->ack_at		= call->delay_ack_at;
		    __entry->ack_lost_at	= call->ack_lost_at;
		    __entry->resend_at		= call->resend_at;
		    __entry->expect_rx_by	= call->expect_rx_by;
		    __entry->expect_req_by	= call->expect_req_by;
		    __entry->expect_term_by	= call->expect_term_by;
		    __entry->timer		= call->timer.expires;
			   ),

	    TP_printk("c=%08x EXPIRED a=%ld la=%ld r=%ld xr=%ld xq=%ld xt=%ld t=%ld",
		      __entry->call,
		      __entry->ack_at - __entry->now,
		      __entry->ack_lost_at - __entry->now,
		      __entry->resend_at - __entry->now,
		      __entry->expect_rx_by - __entry->now,
		      __entry->expect_req_by - __entry->now,
		      __entry->expect_term_by - __entry->now,
		      __entry->timer - __entry->now)
	    );

TRACE_EVENT(rxrpc_rx_lose,
	    TP_PROTO(struct rxrpc_skb_priv *sp),

	    TP_ARGS(sp),

	    TP_STRUCT__entry(
		    __field_struct(struct rxrpc_host_header,	hdr		)
			     ),

	    TP_fast_assign(
		    memcpy(&__entry->hdr, &sp->hdr, sizeof(__entry->hdr));
			   ),

	    TP_printk("%08x:%08x:%08x:%04x %08x %08x %02x %02x %s *LOSE*",
		      __entry->hdr.epoch, __entry->hdr.cid,
		      __entry->hdr.callNumber, __entry->hdr.serviceId,
		      __entry->hdr.serial, __entry->hdr.seq,
		      __entry->hdr.type, __entry->hdr.flags,
		      __entry->hdr.type <= 15 ?
		      __print_symbolic(__entry->hdr.type, rxrpc_pkts) : "?UNK")
	    );

TRACE_EVENT(rxrpc_propose_ack,
	    TP_PROTO(struct rxrpc_call *call, enum rxrpc_propose_ack_trace why,
		     u8 ack_reason, rxrpc_serial_t serial),

	    TP_ARGS(call, why, ack_reason, serial),

	    TP_STRUCT__entry(
		    __field(unsigned int,			call		)
		    __field(enum rxrpc_propose_ack_trace,	why		)
		    __field(rxrpc_serial_t,			serial		)
		    __field(u8,					ack_reason	)
			     ),

	    TP_fast_assign(
		    __entry->call	= call->debug_id;
		    __entry->why	= why;
		    __entry->serial	= serial;
		    __entry->ack_reason	= ack_reason;
			   ),

	    TP_printk("c=%08x %s %s r=%08x",
		      __entry->call,
		      __print_symbolic(__entry->why, rxrpc_propose_ack_traces),
		      __print_symbolic(__entry->ack_reason, rxrpc_ack_names),
		      __entry->serial)
	    );

TRACE_EVENT(rxrpc_send_ack,
	    TP_PROTO(struct rxrpc_call *call, enum rxrpc_propose_ack_trace why,
		     u8 ack_reason, rxrpc_serial_t serial),

	    TP_ARGS(call, why, ack_reason, serial),

	    TP_STRUCT__entry(
		    __field(unsigned int,			call		)
		    __field(enum rxrpc_propose_ack_trace,	why		)
		    __field(rxrpc_serial_t,			serial		)
		    __field(u8,					ack_reason	)
			     ),

	    TP_fast_assign(
		    __entry->call	= call->debug_id;
		    __entry->why	= why;
		    __entry->serial	= serial;
		    __entry->ack_reason	= ack_reason;
			   ),

	    TP_printk("c=%08x %s %s r=%08x",
		      __entry->call,
		      __print_symbolic(__entry->why, rxrpc_propose_ack_traces),
		      __print_symbolic(__entry->ack_reason, rxrpc_ack_names),
		      __entry->serial)
	    );

TRACE_EVENT(rxrpc_drop_ack,
	    TP_PROTO(struct rxrpc_call *call, enum rxrpc_propose_ack_trace why,
		     u8 ack_reason, rxrpc_serial_t serial, bool nobuf),

	    TP_ARGS(call, why, ack_reason, serial, nobuf),

	    TP_STRUCT__entry(
		    __field(unsigned int,			call		)
		    __field(enum rxrpc_propose_ack_trace,	why		)
		    __field(rxrpc_serial_t,			serial		)
		    __field(u8,					ack_reason	)
		    __field(bool,				nobuf		)
			     ),

	    TP_fast_assign(
		    __entry->call	= call->debug_id;
		    __entry->why	= why;
		    __entry->serial	= serial;
		    __entry->ack_reason	= ack_reason;
		    __entry->nobuf	= nobuf;
			   ),

	    TP_printk("c=%08x %s %s r=%08x nbf=%u",
		      __entry->call,
		      __print_symbolic(__entry->why, rxrpc_propose_ack_traces),
		      __print_symbolic(__entry->ack_reason, rxrpc_ack_names),
		      __entry->serial, __entry->nobuf)
	    );

TRACE_EVENT(rxrpc_retransmit,
	    TP_PROTO(struct rxrpc_call *call, rxrpc_seq_t seq, s64 expiry),

	    TP_ARGS(call, seq, expiry),

	    TP_STRUCT__entry(
		    __field(unsigned int,		call		)
		    __field(rxrpc_seq_t,		seq		)
		    __field(s64,			expiry		)
			     ),

	    TP_fast_assign(
		    __entry->call = call->debug_id;
		    __entry->seq = seq;
		    __entry->expiry = expiry;
			   ),

	    TP_printk("c=%08x q=%x xp=%lld",
		      __entry->call,
		      __entry->seq,
		      __entry->expiry)
	    );

TRACE_EVENT(rxrpc_congest,
	    TP_PROTO(struct rxrpc_call *call, struct rxrpc_ack_summary *summary,
		     rxrpc_serial_t ack_serial, enum rxrpc_congest_change change),

	    TP_ARGS(call, summary, ack_serial, change),

	    TP_STRUCT__entry(
		    __field(unsigned int,			call		)
		    __field(enum rxrpc_congest_change,		change		)
		    __field(rxrpc_seq_t,			hard_ack	)
		    __field(rxrpc_seq_t,			top		)
		    __field(rxrpc_seq_t,			lowest_nak	)
		    __field(rxrpc_serial_t,			ack_serial	)
		    __field_struct(struct rxrpc_ack_summary,	sum		)
			     ),

	    TP_fast_assign(
		    __entry->call	= call->debug_id;
		    __entry->change	= change;
		    __entry->hard_ack	= call->acks_hard_ack;
		    __entry->top	= call->tx_top;
		    __entry->lowest_nak	= call->acks_lowest_nak;
		    __entry->ack_serial	= ack_serial;
		    memcpy(&__entry->sum, summary, sizeof(__entry->sum));
			   ),

	    TP_printk("c=%08x r=%08x %s q=%08x %s cw=%u ss=%u nA=%u,%u+%u r=%u b=%u u=%u d=%u l=%x%s%s%s",
		      __entry->call,
		      __entry->ack_serial,
		      __print_symbolic(__entry->sum.ack_reason, rxrpc_ack_names),
		      __entry->hard_ack,
		      __print_symbolic(__entry->sum.mode, rxrpc_congest_modes),
		      __entry->sum.cwnd,
		      __entry->sum.ssthresh,
		      __entry->sum.nr_acks, __entry->sum.saw_nacks,
		      __entry->sum.nr_new_acks,
		      __entry->sum.nr_rot_new_acks,
		      __entry->top - __entry->hard_ack,
		      __entry->sum.cumulative_acks,
		      __entry->sum.dup_acks,
		      __entry->lowest_nak, __entry->sum.new_low_nack ? "!" : "",
		      __print_symbolic(__entry->change, rxrpc_congest_changes),
		      __entry->sum.retrans_timeo ? " rTxTo" : "")
	    );

TRACE_EVENT(rxrpc_disconnect_call,
	    TP_PROTO(struct rxrpc_call *call),

	    TP_ARGS(call),

	    TP_STRUCT__entry(
		    __field(unsigned int,		call		)
		    __field(u32,			abort_code	)
			     ),

	    TP_fast_assign(
		    __entry->call = call->debug_id;
		    __entry->abort_code = call->abort_code;
			   ),

	    TP_printk("c=%08x ab=%08x",
		      __entry->call,
		      __entry->abort_code)
	    );

TRACE_EVENT(rxrpc_improper_term,
	    TP_PROTO(struct rxrpc_call *call),

	    TP_ARGS(call),

	    TP_STRUCT__entry(
		    __field(unsigned int,		call		)
		    __field(u32,			abort_code	)
			     ),

	    TP_fast_assign(
		    __entry->call = call->debug_id;
		    __entry->abort_code = call->abort_code;
			   ),

	    TP_printk("c=%08x ab=%08x",
		      __entry->call,
		      __entry->abort_code)
	    );

TRACE_EVENT(rxrpc_rx_eproto,
	    TP_PROTO(struct rxrpc_call *call, rxrpc_serial_t serial,
		     const char *why),

	    TP_ARGS(call, serial, why),

	    TP_STRUCT__entry(
		    __field(unsigned int,		call		)
		    __field(rxrpc_serial_t,		serial		)
		    __field(const char *,		why		)
			     ),

	    TP_fast_assign(
		    __entry->call = call ? call->debug_id : 0;
		    __entry->serial = serial;
		    __entry->why = why;
			   ),

	    TP_printk("c=%08x EPROTO %08x %s",
		      __entry->call,
		      __entry->serial,
		      __entry->why)
	    );

TRACE_EVENT(rxrpc_connect_call,
	    TP_PROTO(struct rxrpc_call *call),

	    TP_ARGS(call),

	    TP_STRUCT__entry(
		    __field(unsigned int,		call		)
		    __field(unsigned long,		user_call_ID	)
		    __field(u32,			cid		)
		    __field(u32,			call_id		)
			     ),

	    TP_fast_assign(
		    __entry->call = call->debug_id;
		    __entry->user_call_ID = call->user_call_ID;
		    __entry->cid = call->cid;
		    __entry->call_id = call->call_id;
			   ),

	    TP_printk("c=%08x u=%p %08x:%08x",
		      __entry->call,
		      (void *)__entry->user_call_ID,
		      __entry->cid,
		      __entry->call_id)
	    );

TRACE_EVENT(rxrpc_resend,
	    TP_PROTO(struct rxrpc_call *call),

	    TP_ARGS(call),

	    TP_STRUCT__entry(
		    __field(unsigned int,		call		)
		    __field(rxrpc_seq_t,		seq		)
			     ),

	    TP_fast_assign(
		    __entry->call = call->debug_id;
		    __entry->seq = call->acks_hard_ack;
			   ),

	    TP_printk("c=%08x q=%x",
		      __entry->call,
		      __entry->seq)
	    );

TRACE_EVENT(rxrpc_rx_icmp,
	    TP_PROTO(struct rxrpc_peer *peer, struct sock_extended_err *ee,
		     struct sockaddr_rxrpc *srx),

	    TP_ARGS(peer, ee, srx),

	    TP_STRUCT__entry(
		    __field(unsigned int,			peer	)
		    __field_struct(struct sock_extended_err,	ee	)
		    __field_struct(struct sockaddr_rxrpc,	srx	)
			     ),

	    TP_fast_assign(
		    __entry->peer = peer->debug_id;
		    memcpy(&__entry->ee, ee, sizeof(__entry->ee));
		    memcpy(&__entry->srx, srx, sizeof(__entry->srx));
			   ),

	    TP_printk("P=%08x o=%u t=%u c=%u i=%u d=%u e=%d %pISp",
		      __entry->peer,
		      __entry->ee.ee_origin,
		      __entry->ee.ee_type,
		      __entry->ee.ee_code,
		      __entry->ee.ee_info,
		      __entry->ee.ee_data,
		      __entry->ee.ee_errno,
		      &__entry->srx.transport)
	    );

TRACE_EVENT(rxrpc_tx_fail,
	    TP_PROTO(unsigned int debug_id, rxrpc_serial_t serial, int ret,
		     enum rxrpc_tx_point where),

	    TP_ARGS(debug_id, serial, ret, where),

	    TP_STRUCT__entry(
		    __field(unsigned int,		debug_id	)
		    __field(rxrpc_serial_t,		serial		)
		    __field(int,			ret		)
		    __field(enum rxrpc_tx_point,	where		)
			     ),

	    TP_fast_assign(
		    __entry->debug_id = debug_id;
		    __entry->serial = serial;
		    __entry->ret = ret;
		    __entry->where = where;
			   ),

	    TP_printk("c=%08x r=%x ret=%d %s",
		      __entry->debug_id,
		      __entry->serial,
		      __entry->ret,
		      __print_symbolic(__entry->where, rxrpc_tx_points))
	    );

TRACE_EVENT(rxrpc_call_reset,
	    TP_PROTO(struct rxrpc_call *call),

	    TP_ARGS(call),

	    TP_STRUCT__entry(
		    __field(unsigned int,		debug_id	)
		    __field(u32,			cid		)
		    __field(u32,			call_id		)
		    __field(rxrpc_serial_t,		call_serial	)
		    __field(rxrpc_serial_t,		conn_serial	)
		    __field(rxrpc_seq_t,		tx_seq		)
		    __field(rxrpc_seq_t,		rx_seq		)
			     ),

	    TP_fast_assign(
		    __entry->debug_id = call->debug_id;
		    __entry->cid = call->cid;
		    __entry->call_id = call->call_id;
		    __entry->call_serial = call->rx_serial;
		    __entry->conn_serial = call->conn->hi_serial;
		    __entry->tx_seq = call->acks_hard_ack;
		    __entry->rx_seq = call->rx_highest_seq;
			   ),

	    TP_printk("c=%08x %08x:%08x r=%08x/%08x tx=%08x rx=%08x",
		      __entry->debug_id,
		      __entry->cid, __entry->call_id,
		      __entry->call_serial, __entry->conn_serial,
		      __entry->tx_seq, __entry->rx_seq)
	    );

TRACE_EVENT(rxrpc_notify_socket,
	    TP_PROTO(unsigned int debug_id, rxrpc_serial_t serial),

	    TP_ARGS(debug_id, serial),

	    TP_STRUCT__entry(
		    __field(unsigned int,		debug_id	)
		    __field(rxrpc_serial_t,		serial		)
			     ),

	    TP_fast_assign(
		    __entry->debug_id = debug_id;
		    __entry->serial = serial;
			   ),

	    TP_printk("c=%08x r=%08x",
		      __entry->debug_id,
		      __entry->serial)
	    );

TRACE_EVENT(rxrpc_rx_discard_ack,
	    TP_PROTO(unsigned int debug_id, rxrpc_serial_t serial,
		     rxrpc_seq_t first_soft_ack, rxrpc_seq_t call_ackr_first,
		     rxrpc_seq_t prev_pkt, rxrpc_seq_t call_ackr_prev),

	    TP_ARGS(debug_id, serial, first_soft_ack, call_ackr_first,
		    prev_pkt, call_ackr_prev),

	    TP_STRUCT__entry(
		    __field(unsigned int,	debug_id	)
		    __field(rxrpc_serial_t,	serial		)
		    __field(rxrpc_seq_t,	first_soft_ack)
		    __field(rxrpc_seq_t,	call_ackr_first)
		    __field(rxrpc_seq_t,	prev_pkt)
		    __field(rxrpc_seq_t,	call_ackr_prev)
			     ),

	    TP_fast_assign(
		    __entry->debug_id		= debug_id;
		    __entry->serial		= serial;
		    __entry->first_soft_ack	= first_soft_ack;
		    __entry->call_ackr_first	= call_ackr_first;
		    __entry->prev_pkt		= prev_pkt;
		    __entry->call_ackr_prev	= call_ackr_prev;
			   ),

	    TP_printk("c=%08x r=%08x %08x<%08x %08x<%08x",
		      __entry->debug_id,
		      __entry->serial,
		      __entry->first_soft_ack,
		      __entry->call_ackr_first,
		      __entry->prev_pkt,
		      __entry->call_ackr_prev)
	    );

TRACE_EVENT(rxrpc_req_ack,
	    TP_PROTO(unsigned int call_debug_id, rxrpc_seq_t seq,
		     enum rxrpc_req_ack_trace why),

	    TP_ARGS(call_debug_id, seq, why),

	    TP_STRUCT__entry(
		    __field(unsigned int,		call_debug_id	)
		    __field(rxrpc_seq_t,		seq		)
		    __field(enum rxrpc_req_ack_trace,	why		)
			     ),

	    TP_fast_assign(
		    __entry->call_debug_id = call_debug_id;
		    __entry->seq = seq;
		    __entry->why = why;
			   ),

	    TP_printk("c=%08x q=%08x REQ-%s",
		      __entry->call_debug_id,
		      __entry->seq,
		      __print_symbolic(__entry->why, rxrpc_req_ack_traces))
	    );

TRACE_EVENT(rxrpc_txbuf,
	    TP_PROTO(unsigned int debug_id,
		     unsigned int call_debug_id, rxrpc_seq_t seq,
		     int ref, enum rxrpc_txbuf_trace what),

	    TP_ARGS(debug_id, call_debug_id, seq, ref, what),

	    TP_STRUCT__entry(
		    __field(unsigned int,		debug_id	)
		    __field(unsigned int,		call_debug_id	)
		    __field(rxrpc_seq_t,		seq		)
		    __field(int,			ref		)
		    __field(enum rxrpc_txbuf_trace,	what		)
			     ),

	    TP_fast_assign(
		    __entry->debug_id = debug_id;
		    __entry->call_debug_id = call_debug_id;
		    __entry->seq = seq;
		    __entry->ref = ref;
		    __entry->what = what;
			   ),

	    TP_printk("B=%08x c=%08x q=%08x %s r=%d",
		      __entry->debug_id,
		      __entry->call_debug_id,
		      __entry->seq,
		      __print_symbolic(__entry->what, rxrpc_txbuf_traces),
		      __entry->ref)
	    );

#undef EM
#undef E_
#endif /* _TRACE_RXRPC_H */

/* This part must be outside protection */
#include <trace/define_trace.h>
