/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/* vim: set ts=8 sts=4 sw=4 noexpandtab : */
/*
 *
 */



#include "mpid_nem_impl.h"
#include "tofu_impl.h"
#ifdef	NOTDEF
#include <llc/llc-tofu.h>
#endif	/* NOTDEF */

/* function prototypes */

static int	tofu_vc_init (MPIDI_VC_t *vc);


#undef FUNCNAME
#define FUNCNAME MPID_nem_tofu_vc_init
#undef FCNAME
#define FCNAME MPIDI_QUOTE(FUNCNAME)
int
MPID_nem_tofu_vc_init (MPIDI_VC_t *vc)
{
    int mpi_errno = MPI_SUCCESS;
    MPIDI_STATE_DECL(MPID_STATE_MPID_NEM_TOFU_VC_INIT);

    MPIDI_FUNC_ENTER(MPID_STATE_MPID_NEM_TOFU_VC_INIT);

    mpi_errno = tofu_vc_init (vc);
    if (mpi_errno) MPIU_ERR_POP(mpi_errno);

 fn_exit:
    MPIDI_FUNC_EXIT(MPID_STATE_MPID_NEM_TOFU_VC_INIT);
    return mpi_errno;
 fn_fail:
    goto fn_exit;
}

#undef FUNCNAME
#define FUNCNAME MPID_nem_tofu_vc_destroy
#undef FCNAME
#define FCNAME MPIDI_QUOTE(FUNCNAME)
int MPID_nem_tofu_vc_destroy(MPIDI_VC_t *vc)
{
    int mpi_errno = MPI_SUCCESS;
    MPIDI_STATE_DECL(MPID_STATE_MPID_NEM_TOFU_VC_DESTROY);

    MPIDI_FUNC_ENTER(MPID_STATE_MPID_NEM_TOFU_VC_DESTROY);
    /* free any resources associated with this VC here */

    {
#ifdef	NOTDEF
	int rc;
#endif	/* NOTDEF */
	MPID_nem_tofu_vc_area *vc_tofu = VC_TOFU(vc);

#ifdef	NOTDEF
	rc = llctofu_unbind(vc_tofu->endpoint);
	if (rc != 0 /* LLC_SUCCESS */) {
	    mpi_errno = MPI_ERR_OTHER;
	    MPIU_ERR_POP(mpi_errno);
	}
#endif	/* NOTDEF */
	vc_tofu->endpoint = 0;
    }

 fn_exit:
    MPIDI_FUNC_EXIT(MPID_STATE_MPID_NEM_TOFU_VC_DESTROY);
    return mpi_errno;
    //fn_fail:
    goto fn_exit;
}

#undef FUNCNAME
#define FUNCNAME MPID_nem_tofu_vc_terminate
#undef FCNAME
#define FCNAME MPIDI_QUOTE(FUNCNAME)
int MPID_nem_tofu_vc_terminate (MPIDI_VC_t *vc)
{
    int mpi_errno = MPI_SUCCESS;
    MPIDI_STATE_DECL(MPID_STATE_MPID_NEM_TOFU_VC_TERMINATE);

    MPIDI_FUNC_ENTER(MPID_STATE_MPID_NEM_TOFU_VC_TERMINATE);

    mpi_errno = MPIDI_CH3U_Handle_connection (vc, MPIDI_VC_EVENT_TERMINATED);

 fn_exit:
    MPIDI_FUNC_EXIT(MPID_STATE_MPID_NEM_TOFU_VC_TERMINATE);
    return mpi_errno;
    //fn_fail:
    goto fn_exit;
}

/* ============================================== */

#undef FUNCNAME
#define FUNCNAME tofu_vc_init
#undef FCNAME
#define FCNAME MPIDI_QUOTE(FUNCNAME)
static int tofu_vc_init (MPIDI_VC_t *vc)
{
    int mpi_errno = MPI_SUCCESS;

    {
	MPIU_DBG_MSG_D(CH3_CHANNEL, VERBOSE,
	    "MPID_NEM_VC_NETMOD_AREA_LEN = %d\n",
	    MPID_NEM_VC_NETMOD_AREA_LEN);
	MPIU_DBG_MSG_D(CH3_CHANNEL, VERBOSE,
	    "MPID_NEM_REQ_NETMOD_AREA_LEN = %d",
	    MPID_NEM_REQ_NETMOD_AREA_LEN);
	MPIU_DBG_MSG_D(CH3_CHANNEL, VERBOSE,
	    "MPID_nem_tofu_vc_area = %d\n",
	    (int) sizeof (MPID_nem_tofu_vc_area));
    }

    /* MPIDI_CH3I_VC: src/mpid/ch3/channels/nemesis/include/mpidi_ch3_pre.h */
    {
	MPIDI_CH3I_VC *vc_ch = &vc->ch;
	MPID_nem_tofu_vc_area *vc_tofu = VC_TOFU(vc);

#ifdef	NOTDEF
	vc_tofu->endpoint		= 0;
#else	/* NOTDEF */
	vc_tofu->endpoint		= vc;
#endif	/* NOTDEF */
	vc_tofu->remote_endpoint_addr	= vc->pg_rank;
	vc_tofu->send_queue.head	= 0; /* GENERIC_Q_DECL */
	vc_tofu->send_queue.tail	= 0; /* GENERIC_Q_DECL */
#ifdef	NOTDEF

	{
	    int rc;
	    rc = llctofu_bind(&vc_tofu->endpoint,
		    vc_tofu->remote_endpoint_addr, vc /* cba */);
	    if (rc != 0 /* LLC_SUCCESS */) {
		mpi_errno = MPI_ERR_OTHER;
		MPIU_ERR_POP(mpi_errno);
	    }
	}
#endif	/* NOTDEF */

	vc->eager_max_msg_sz	    = (12 * 1024);
	vc->ready_eager_max_msg_sz  = (12 * 1024);
	/* vc->rndvSend_fn = 0*/
	/* vc->rndvRecv_fn = 0*/
	vc->sendNoncontig_fn	    = MPID_nem_tofu_SendNoncontig;
#ifdef	ENABLE_COMM_OVERRIDES
	vc->comm_ops		    = 0 /* &comm_ops */;
#endif

	vc_ch->iStartContigMsg	= MPID_nem_tofu_iStartContigMsg;
	vc_ch->iSendContig	= MPID_nem_tofu_iSendContig;

#ifdef	ENABLE_CHECKPOINTING
	vc_ch->ckpt_pause_send_vc   = 0 /* MPID_nem_tofu_ckpt_pause_send_vc */;
	vc_ch->ckpt_continue_vc	    = 0 /* MPID_nem_tofu_ckpt_continue_vc */;
	vc_ch->ckpt_restart_vc	    = 0 /* = MPID_nem_tofu_ckpt_restart_vc */;
#endif

	MPIDI_CHANGE_VC_STATE(vc, ACTIVE);
    }

 fn_exit:
    return mpi_errno;
    //fn_fail:
    goto fn_exit;
}

#undef FUNCNAME
#define FUNCNAME MPID_nem_tofu_vc_prnt
#undef FCNAME
#define FCNAME MPIDI_QUOTE(FUNCNAME)
static int MPID_nem_tofu_vc_prnt(MPIDI_VC_t *vc)
{
    int mpi_errno = MPI_SUCCESS;

    /* MPIU_OBJECT_HEADER; */
	/* src/include/mpihandlemem.h */
	    /* int handle; */
	    /* MPIU_THREAD_OBJECT_HOOK */
	    /* MPIU_Handle_ref_count ref_count; */
    /* MPIDI_VC_State_t state; */
    /* struct MPIDI_PG *pg; */
    /* int pg_rank; */
    /* int lpid; */
    /* MPID_Node_id_t node_id; */
    /* int port_name_tag; */
    /* MPID_Seqnum_t seqnum_send; */ /* MPID_USE_SEQUENCE_NUMBERS */
    /* MPID_Seqnum_t seqnum_recv; */ /* MPID_CH3_MSGS_UNORDERED */
    /* MPIDI_CH3_Pkt_send_container_t *msg_reorder_queue; */
    /* int (*rndvSend_fn)(); */
    /* int (*rndvRecv_fn)(); */
    /* int eager_max_msg_sz; */
    /* int ready_eager_max_msg_sz; */
    /* int (*sendNonconfig_gn)(); */
    /* MPIDI_Comm_ops_t *comm_ops; */ /* ENABLE_COMM_OVERRIDES */
    /* MPIDI_CH3_VC_DECL */ /* MPIDI_CH3_VC_DECL */
	/* src/mpid/ch3/channels/nemesis/include/mpidi_ch3_pre.h */

    return mpi_errno;
}

