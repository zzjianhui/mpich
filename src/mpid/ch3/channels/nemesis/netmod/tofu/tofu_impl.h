/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/* vim: set ts=8 sts=4 sw=4 noexpandtab : */

#ifndef	TOFU_MODULE_IMPL_H
#define TOFU_MODULE_IMPL_H

#ifdef USE_PMI2_API
#include "pmi2.h"
#else
#include "pmi.h"
#endif
#include "mpid_nem_impl.h"
#include "llc.h"

/* The vc provides a generic buffer in which network modules can store
 *    private fields This removes all dependencies from the VC struction
 *       on the network module, facilitating dynamic module loading. */

/*
 * GENERIC_Q_*:
 *   src/mpid/ch3/channels/nemesis/include/mpid_nem_generic_queue.h
 */
typedef GENERIC_Q_DECL(struct MPID_Request) rque_t;
/*
typedef GENERIC_Q_DECL(struct MPID_Request) MPIDI_nem_tofu_request_queue_t;
 */

typedef struct
{
    uint64_t remote_endpoint_addr;
    void *endpoint;
    rque_t	send_queue; /* MPID_Request Queue */
} MPID_nem_tofu_vc_area;

/* macro for tofu private in VC */
#define VC_TOFU(vc) ((MPID_nem_tofu_vc_area *)(vc)->ch.netmod_area.padding)

typedef struct
{
    void *cmds;
    void *pack_buf;         /* to pack non-contiguous data */

} MPID_nem_tofu_req_area;

#define REQ_TOFU(req) \
    ((MPID_nem_tofu_req_area *)(&(req)->ch.netmod_area.padding))
#define REQ_FIELD(reqp, field) (((MPID_nem_tofu_req_area *)((reqp)->ch.netmod_area.padding))->field)

/* functions */
int MPID_nem_tofu_init (MPIDI_PG_t *pg_p, int pg_rank, char **bc_val_p, int *val_max_sz_p);
int MPID_nem_tofu_finalize (void);
int MPID_nem_tofu_poll(int in_blocking_progress);
int MPID_nem_tofu_get_business_card (int my_rank, char **bc_val_p, int *val_max_sz_p);
int MPID_nem_tofu_connect_to_root (const char *business_card, MPIDI_VC_t *new_vc);
int MPID_nem_tofu_vc_init (MPIDI_VC_t *vc);
int MPID_nem_tofu_vc_destroy(MPIDI_VC_t *vc);
int MPID_nem_tofu_vc_terminate (MPIDI_VC_t *vc);

int MPID_nem_tofu_anysource_iprobe(int tag, MPID_Comm *comm, int context_offset, int *flag, MPI_Status *status);
int MPID_nem_tofu_anysource_improbe(int tag, MPID_Comm *comm, int context_offset, int *flag,
				  MPID_Request **message,MPI_Status *status);

int MPID_nem_tofu_iSendContig(MPIDI_VC_t *vc, MPID_Request *sreq, void *hdr, MPIDI_msg_sz_t hdr_sz, void *data, MPIDI_msg_sz_t data_sz);
int MPID_nem_tofu_iStartContigMsg(MPIDI_VC_t *vc, void *hdr, MPIDI_msg_sz_t hdr_sz, void *data, MPIDI_msg_sz_t data_sz, MPID_Request **sreq_ptr);
int MPID_nem_tofu_SendNoncontig(MPIDI_VC_t *vc, MPID_Request *sreq, void *hdr, MPIDI_msg_sz_t hdr_sz);

int MPIDI_nem_tofu_Rqst_iov_update(MPID_Request *mreq, MPIDI_msg_sz_t consume);
int MPID_nem_tofu_send_queued(MPIDI_VC_t * vc, rque_t *send_queue);

int MPID_nem_tofu_isend(struct MPIDI_VC *vc, const void *buf, int count, MPI_Datatype datatype,
                        int dest, int tag, MPID_Comm *comm, int context_offset,
                        struct MPID_Request **request );
int MPID_nem_tofu_recv_posted(struct MPIDI_VC *vc, struct MPID_Request *req);

/*
 * temporary llctofu api
 */
typedef void (*llctofu_send_f)(void *cba, uint64_t *reqid);
typedef void (*llctofu_recv_f)
		(void *cba, uint64_t addr, void *buf, size_t bsz);

extern ssize_t llctofu_writev(void *endpt, uint64_t raddr,
    const struct iovec *iovs, int niov, void *cbarg, void **vpp_reqid);
extern int llctofu_bind(void **vpp_endpt, uint64_t raddr, void *cbarg);
extern int llctofu_unbind(void *endpt);

extern int llctofu_poll(int in_blocking_poll,
	    llctofu_send_f sfnc, llctofu_recv_f rfnc);

typedef struct MPID_nem_tofu_netmod_hdr {
    int initiator_pg_rank;
} MPID_nem_tofu_netmod_hdr_t;

#define MPID_nem_tofu_segv printf("%d\n", *(int32_t*)0);

#endif	/* TOFU_MODULE_IMPL_H */
