/*
 * Copyright (c) 2005 Topspin Communications.  All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* Copyright (c) 2008  VirtualPlane Systems, Inc. */

#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <malloc.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <time.h>

#include "bx.h"

int bx_send_ctr = 0;

enum {
	BX_RECV_WRID = 1,
	BX_SEND_WRID = 2,
};

static int page_size;

struct bx_context *ctx;
struct bx_dest my_dest;
struct bx_dest *rem_dest;
struct ibv_device **dev_list;

static uint16_t bx_get_local_lid(int port)
{
        struct ibv_port_attr attr;

        enib_debug("Enter: bx_get_local_lid");

        if (ibv_query_port(ctx->context, port, &attr))
                return 0;

        enib_debug("Exit: bx_get_local_lid");

        return attr.lid;
}


/* Connection parameters to connect to Host */
static int bx_connect_ctx(struct ib_header *ibh )
{
	
	enib_debug("Enter: bx_connect_ctx");

	struct ibv_ah_attr ah_attr = {
                .is_global     = 1,	/* GRH attributes are valid */
                .dlid          = ntohs((ibh->lrh).dlid),
                .grh.sgid_index = 0,       
                .sl =         (uint8_t) (ntohl((ibh->lrh).vl_lv_sl) & 0x00F0),
		.src_path_bits = 0,
		.port_num      = ctx->port
	};

        memcpy(&ah_attr.grh.dgid.raw[0],&(ibh->grh).dest_gid.raw[0], sizeof(union ibv_gid));

	enib_printf("*** Remote LID = 0x%x ***", ah_attr.dlid);
        enib_printf("*** SL = 0x%x ***",ah_attr.sl);        

	/* Change the state of queue pair to send/receive packets */
	struct ibv_qp_attr attr = {
		.qp_state		= IBV_QPS_RTR
	};

	if (ibv_modify_qp(ctx->qp, &attr, IBV_QP_STATE)) {
		fprintf(stderr, "Failed to modify QP to RTR\n");
		return 1;
	}

	attr.qp_state	    = IBV_QPS_RTS;
        attr.sq_psn         = (uint32_t) (ntohs((ibh->bth).misc_2) & 0x00FFFFFF) ;

	if (ibv_modify_qp(ctx->qp, &attr,
			  IBV_QP_STATE              |
			  IBV_QP_SQ_PSN)) {
		fprintf(stderr, "Failed to modify QP to RTS\n");
		return 1;
	}

	ctx->ah = ibv_create_ah(ctx->pd, &ah_attr);
	if (!ctx->ah) {
		fprintf(stderr, "Failed to create AH\n");
		return 1;
	}

	enib_debug("Enter: bx_connect_ctx");
	return 0;
}

/* Initialize the Infiniband environment */
static struct bx_context *bx_init_ctx(struct ibv_device *ib_dev, int size, int rx_depth, int port)
{
	
	enib_debug("Enter: bx_init_ctx");

	ctx->mr_size     = size;
	ctx->max_recv = rx_depth;
	ctx->port = port;

	ctx->send_buf = memalign(page_size, ctx->mr_size + GRH_SIZE);
	if (!ctx->send_buf) {
		fprintf(stderr, "Couldn't allocate work buf.\n");
		return NULL;
	}

	ctx->recv_buf = memalign(page_size, ctx->mr_size + GRH_SIZE);
        if (!ctx->recv_buf) {
                fprintf(stderr, "Couldn't allocate work buf.\n");
                return NULL;
        }

	memset(ctx->send_buf, 0, ctx->mr_size + GRH_SIZE);
	memset(ctx->recv_buf, 0, ctx->mr_size + GRH_SIZE);

	ctx->context = ibv_open_device(ib_dev);
	if (!ctx->context) {
		fprintf(stderr, "Couldn't get context for %s\n",
			ibv_get_device_name(ib_dev));
		return NULL;
	}

	ctx->pd = ibv_alloc_pd(ctx->context);
	if (!ctx->pd) {
		fprintf(stderr, "Couldn't allocate PD\n");
		return NULL;
	}

	ctx->send_mr = ibv_reg_mr(ctx->pd, ctx->send_buf, ctx->mr_size + GRH_SIZE, IBV_ACCESS_LOCAL_WRITE);
	if (!ctx->send_mr) {
		fprintf(stderr, "Couldn't register MR\n");
		return NULL;
	}

	ctx->recv_mr = ibv_reg_mr(ctx->pd, ctx->recv_buf, ctx->mr_size + GRH_SIZE, IBV_ACCESS_LOCAL_WRITE);
        if (!ctx->recv_mr) {
                fprintf(stderr, "Couldn't register MR\n");
                return NULL;
        }

	ctx->cq = ibv_create_cq(ctx->context, rx_depth + 1, NULL, NULL, 0);
	if (!ctx->cq) {
		fprintf(stderr, "Couldn't create CQ\n");
		return NULL;
	}

	{
		struct ibv_qp_init_attr attr = {
			.send_cq = ctx->cq,
			.recv_cq = ctx->cq,
			.cap     = {
				.max_send_wr  = 1,
				.max_recv_wr  = rx_depth,
				.max_send_sge = 1,
				.max_recv_sge = 1
			},
			.qp_type = IBV_QPT_UD,
		};

		ctx->qp = ibv_create_qp(ctx->pd, &attr);
		if (!ctx->qp)  {
			fprintf(stderr, "Couldn't create QP\n");
			return NULL;
		}
	}

	{
		struct ibv_qp_attr attr = {
			.qp_state        = IBV_QPS_INIT,
			.pkey_index      = 0,
			.port_num        = port,
			.qkey 		 = 0x80020002
		};

		if (ibv_modify_qp(ctx->qp, &attr,
				  IBV_QP_STATE              |
				  IBV_QP_PKEY_INDEX         |
				  IBV_QP_PORT               |
				  IBV_QP_QKEY)) {
			fprintf(stderr, "Failed to modify QP to INIT\n");
			return NULL;
		}
	}

	enib_debug("Exit: bx_init_ctx");
	return ctx;
}


int bx_close_ctx()
{
	enib_debug("Enter: bx_close_ctx");
	if (ibv_destroy_qp(ctx->qp)) {
		fprintf(stderr, "Couldn't destroy QP\n");
		return 1;
	}

	if (ibv_destroy_cq(ctx->cq)) {
		fprintf(stderr, "Couldn't destroy CQ\n");
		return 1;
	}

	if (ibv_dereg_mr(ctx->send_mr)) {
		fprintf(stderr, "Couldn't deregister Send  MR\n");
		return 1;
	}

	if (ibv_dereg_mr(ctx->recv_mr)) {
                fprintf(stderr, "Couldn't deregister Receive  MR\n");
                return 1;
        }

	if (ibv_destroy_ah(ctx->ah)) {
		fprintf(stderr, "Couldn't destroy AH\n");
		return 1;
	}

	if (ibv_dealloc_pd(ctx->pd)) {
		fprintf(stderr, "Couldn't deallocate PD\n");
		return 1;
	}

	if (ibv_close_device(ctx->context)) {
		fprintf(stderr, "Couldn't release context\n");
		return 1;
	}

	free(ctx->send_buf);
	free(ctx->recv_buf);
	free(ctx);
	
	enib_debug("Exit: bx_close_ctx");
	return 0;
}

/* Posts the specified number of requests to the receive queue */
static int bx_post_recv(int n)
{
	struct ibv_sge list = {
		.addr	= (uintptr_t) ctx->recv_buf,
		.length = ctx->mr_size + GRH_SIZE,
		.lkey	= ctx->recv_mr->lkey
	};
	struct ibv_recv_wr wr = {
		.wr_id	    = BX_RECV_WRID,
		.sg_list    = &list,
		.num_sge    = 1,
	};
	struct ibv_recv_wr *bad_wr;
	int i;

	enib_debug("Enter: bx_post_recv");

	for (i = 0; i < n; ++i)
		if (ibv_post_recv(ctx->qp, &wr, &bad_wr))
			break;
	enib_debug("Exit: bx_post_recv");

	return i;
}

/* Posts a send request to the send queue */
static int bx_post_send(struct ib_header * ibh)
{
        int ret;
	struct ibv_sge list = {
		.addr	= (uintptr_t) ctx->send_buf + GRH_SIZE,
		.length = (ntohs((ibh->lrh).length) - 16) * 4,
		.lkey	= ctx->send_mr->lkey
	};

         struct ibv_ah    *new_ah;
	/* Create an AH to send unicast messages. For unicast messages, is_global=0 */
         struct ibv_ah_attr       ah_attr = {
                 .is_global     = 0,	/* GRH attributes invalid */
                 .dlid     = ntohs((ibh->lrh).dlid),
                 .sl =         (uint8_t) (ntohl((ibh->lrh).vl_lv_sl) & 0x00F0),
                 .grh.sgid_index = 0,       
                 .src_path_bits = 0,
                 .port_num      = ctx->port
         };

	struct ibv_send_wr wr = {
		.wr_id	    = BX_SEND_WRID,
		.sg_list    = &list,
		.num_sge    = 1,
		.opcode     = IBV_WR_SEND,
		.send_flags = IBV_SEND_SIGNALED,
		.wr         = {
			.ud = {
				 .ah          = ctx->ah,
                                 .remote_qpn  = (ntohl((ibh->bth).dqp)  & 0x00FFFFFF),
                                 .remote_qkey = ntohl((ibh->deth).qkey)
			 }
		}
	};

	struct ibv_send_wr *bad_wr;
	int j;
        struct ibv_pd *pd;
       
	enib_debug("Enter: bx_post_send");       

	 if(bx_send_ctr > 1)
        {
                if( (new_ah = ibv_create_ah(ctx->pd, &ah_attr)) == NULL)
                {
                        enib_debug("Unable to CREATE AH-----------");
                        return -1;
                }
                wr.wr.ud.ah = new_ah;
        }
        
	/*
         * FIXME: Increment the counter to identify if the request is a
         * multicast or unicast solicitation.
         * This is a very bad hack. We should fix the code to be logically
         * correct ASAP. 
         */        
	bx_send_ctr++;

        enib_printf("*** Length of packet in bytes = %d***", list.length);
        enib_printf("** Remote Qkey = 0x%lx **", wr.wr.ud.remote_qkey);
	enib_printf("** Remote QPN = 0x%lx **", wr.wr.ud.remote_qpn);
	
	/* Copy the payload from the IB packet */
	memcpy(ctx->send_buf + GRH_SIZE, (char *)(ibh)+sizeof(struct ib_header), list.length);

	enib_printf("*** Sending the packet to Host ***");
        for (j = 0; j < list.length ; j++) {
		if ((j % 16) == 0)
                	printf("\n");
                printf(" 0x%0.2x", (unsigned char) (ctx->send_buf[j + GRH_SIZE] & 0xff));
        }
	printf("\n");
	
	ret = ibv_post_send(ctx->qp, &wr, &bad_wr);

/* FIXME: This can cause problems if messages are sent after destruction */
#if 0
        if(bx_send_ctr > 2)
        {
		enib_debug("ibv_destroy_ah(new_ah)");
                ibv_destroy_ah(new_ah);
        }
        else {
		enib_debug("ibv_destroy_ah(ctx->ah)");
                ibv_destroy_ah(ctx->ah);
        } 
#endif

        enib_debug("Exit: bx_post_send: %d", ret);

        return ret;
}

/* BXM should call this function before bx_send/bx_recv */
int bx_init(struct init_info_packet * info)
{
	struct timeval           start, end;
	char                    *ib_devname = NULL;
	int                      ib_port = 1;
	int                      size = 2048;
	int                      rx_depth = 2000;
	int                      iters = 1000;
	int                      use_event = 0;
	int                      rc;
	int                      rcnt, scnt;
	int                      num_cq_events = 0;
	int                      sl = 0;
	int j=0;

	enib_debug("Enter: bx_init");

	page_size = sysconf(_SC_PAGESIZE);

	ctx = malloc(sizeof *ctx);
        if (!ctx)
                return -1;


	dev_list = ibv_get_device_list(NULL);
	if (!dev_list) {
		fprintf(stderr, "No IB devices found\n");
		return 1;
	}

	if (!ib_devname) {
		ctx->ib_dev = *dev_list;
		if (!ctx->ib_dev) {
			fprintf(stderr, "No IB devices found\n");
			return 1;
		}
	} else {
		int i;
		for (i = 0; dev_list[i]; ++i)
			if (!strcmp(ibv_get_device_name(dev_list[i]), ib_devname))
				break;
		ctx->ib_dev = dev_list[i];
		if (!ctx->ib_dev) {
			fprintf(stderr, "IB device %s not found\n", ib_devname);
			return 1;
		}
	}
	
	/* Initialize the Infiniband context */	
	ctx = bx_init_ctx(ctx->ib_dev, size, rx_depth, ib_port);
	if (!ctx) {
		fprintf(stderr, "Couldn't initialize the context");
		exit(1);
	}

	/* Post receive requests so that we are ready to receive any message */
	rc = bx_post_recv(ctx->max_recv);
	
	if (rc < ctx->max_recv) {
		fprintf(stderr, "Couldn't post receive (%d)\n", rc);
		return 1;
	}

	/* Gather local IB port information */
	my_dest.lid = bx_get_local_lid(ctx->port);
	my_dest.qpn = ctx->qp->qp_num;
	if (!my_dest.lid) {
		fprintf(stderr, "Couldn't get local LID\n");
		return 1;
	}

	printf("\nlocal address:  LID 0x%04x, QPN 0x%06x\n",
	       my_dest.lid, my_dest.qpn);
          

        /*------- For SA MAD packet --------*/   
        rc = ibv_query_pkey(ctx->context, ctx->port, DEF_PKEY_IDX, &info->pkey);
        if (rc) {
                fprintf(stderr, "\nfailed to query PKey table \
                    of port %d with index %d\n", ctx->port, DEF_PKEY_IDX);
                return 1;
        }

        rc = ibv_query_gid(ctx->context, ctx->port, 0, info->sgid);
        if (rc) {
                fprintf(stderr, "\nfailed to query GID table of ib \
                port %d with index %d\n", ctx->port, 0);
                return 1;
        }

	info->slid = my_dest.lid;

#if 0
	// Set this value when required
	memset(info->sgid, 0, sizeof(info->sgid));
#endif

	info->sqp  = my_dest.qpn;

	enib_debug("Exit: bx_init");
}


/* BXM calls this to send an IB packet to host */
int bx_send(void *packet)
{
        struct ib_header* ibh;
        int j=0, len=0;

        enib_debug("Enter: bx_send()");

	ibh = (struct ib_header *) packet;

	/* Establish a connection with the other end with multicast parameters */
	if(bx_send_ctr == 0){
               bx_connect_ctx(ibh);
               bx_send_ctr++;
        }

       len = (ntohs((ibh->lrh).length) * 4) + sizeof(struct ib_header);

       enib_printf("*** BXM sent this packet on bx_send() ***");

       for (j = 0; j < len; j++) {
         if ((j % 16) == 0)
                printf("\n");
         printf(" %0.2x", (unsigned char)((*((char *)ibh + j)) & 0xff));
       }
       printf("\n");

        enib_debug("Exit: bx_send()");

        return bx_post_send(ibh);
}


/* BXM calls this function to receive an IB packet */
void * bx_recv(uint32_t *length)
{
	void * packet = NULL;
	enib_debug("Enter: bx_recv");
	do
	{
		{
			struct ibv_wc wc[2];
			int rc, i, j;

			do {
				rc = ibv_poll_cq(ctx->cq, 2, wc);
				sleep(2);
				if (rc < 0) {
					fprintf(stderr, "polling CQ failed %d\n", rc);
					return NULL;
				}

			} while (rc < 1);

			for (i = 0; i < rc; ++i) {
				if (wc[i].status != IBV_WC_SUCCESS) {
					fprintf(stderr, "Failed status (%d) for wr_id %d\n",
						wc[i].status, (int) wc[i].wr_id);
					return NULL;
				}
				
				switch ((int) wc[i].wr_id) {
				case BX_SEND_WRID:
                                        enib_printf("*** Packet sent successfully ***");
					break;

				case BX_RECV_WRID:
					printf("\n *** Incoming packet *** \n");
#if 0
					for (j = 0; j < wc[i].byte_len - GRH_SIZE; j++) {
        	                        	if ((j % 16) == 0)
                	                		printf("\n");
                        	        	printf(" 0x%02x", (unsigned char) (ctx->recv_buf[j+GRH_SIZE] & 0xff));
                                	}
#endif
                                        printf("\n");
					packet = malloc(wc[i].byte_len);
					memcpy(packet, ctx->recv_buf + GRH_SIZE, wc[i].byte_len);
                                        *length = wc[i].byte_len;
					bx_post_recv(1);	
					enib_debug("Exit: bx_recv");
					return packet;

				default:
					fprintf(stderr, "Completion for unknown wr_id %d\n",
						(int) wc[i].wr_id);
					return NULL;
				}

			}
	}		
  } while(1);
		
}

static void get_mlid_from_mad(
        struct sa_mad_packet_t *samad_packet,
        uint16_t *mlid)
{
        u_int8_t *ptr;

	enib_debug("Enter: get_mlid_from_mad");	

        ptr = samad_packet->SubnetAdminData;
        *mlid = ntohs(*(u_int16_t *)(ptr + 36));

	enib_debug("Exit: get_mlid_from_mad");
}

static int check_mad_status(struct sa_mad_packet_t *samad_packet)
{
        u_int8_t *ptr;
        u_int32_t user_trans_id;
        u_int16_t mad_header_status;    /* only 15 bits */

	enib_debug("Enter: check_mad_status");

        ptr = samad_packet->mad_header_buf;

	/* the upper 32 bits of TransactionID were set by the kernel */
        user_trans_id = ntohl(*(u_int32_t *)(ptr + 12)); 

        /* check the TransactionID to make sure this is the response for the 
	 * join/leave multicast group request we posted */
        if (user_trans_id != DEF_TRANS_ID) {
          fprintf(stderr, "\nreceived a mad with TransactionID 0x%x, when expecting 0x%x\n", (unsigned int)user_trans_id, (unsigned int)DEF_TRANS_ID);
                return 1;
        }

        mad_header_status = 0x0;
        mad_header_status = INSERTF(mad_header_status, 8, ptr[4], 0, 7); /* only 15 bits */
        mad_header_status = INSERTF(mad_header_status, 0, ptr[5], 0, 8);

        if (mad_header_status) {
         fprintf(stderr,"\nreceived UMAD with an error: 0x%x\n", mad_header_status);
                return 1;
        }

	enib_debug("Exit: check_mad_status");
        return 0;
}


static int bx_send_sa_mad(struct sa_mad_packet_t *pkt)
{
        void *umad_buff = NULL;
        void *mad = NULL;
        int portid = -1;
        int agentid = -1;
        int timeout_ms, num_retries;
        int length;
        int test_result = 1;
        int rc;
	uint8_t *ptr, *ptr1;
        int i = 0;

	enib_debug("Enter: bx_send_sa_mad");

        /* Print sa mad  */
        ptr1 = (uint8_t *)pkt;

        enib_printf("** MAD packet from BXM  **");
        
        for(i =0 ; i< sizeof(struct sa_mad_packet_t) ; i = i+16)
        {
                printf(" %0.2X %0.2X %0.2X %0.2X ",   
                         ptr1[i],ptr1[i+1],ptr1[i+2],ptr1[i+3]);
                printf(" %0.2X %0.2X %0.2X %0.2X ",  
                          ptr1[i+4],ptr1[i+5],ptr1[i+6],ptr1[i+7]);
                printf(" %0.2X %0.2X %0.2X %0.2X ",   
                          ptr1[i+8],ptr1[i+9],ptr1[i+10],ptr1[i+11]);
                printf(" %0.2X %0.2X %0.2X %0.2X \n", 
                          ptr1[i+12],ptr1[i+13],ptr1[i+14],ptr1[i+15]);
        }


        /* use casting to loose the "const char0 *" */
        portid = umad_open_port((char*) ibv_get_device_name(ctx->ib_dev) , ctx->port);
        if (portid < 0) {
            fprintf(stderr, "\nfailed to open UMAD port %d of device %s\n", ctx->port, ibv_get_device_name(ctx->ib_dev));
                goto cleanup;
        }
        fprintf(stdout, "\nUMAD port %d of device %s was opened\n", ctx->port, ibv_get_device_name(ctx->ib_dev));

	ptr = pkt->mad_header_buf;

        agentid = umad_register(portid, MANAGMENT_CLASS_SUBN_ADM , 2, 0, 0);
        if (agentid < 0) {
                fprintf(stderr, "\nfailed to register UMAD agent for MADs\n");
                goto cleanup;
        }

        fprintf(stdout, "\n*** UMAD agent was registered*** \n");

        umad_buff = umad_alloc(1, umad_size() + MAD_SIZE);
        if (!umad_buff) {
                fprintf(stderr, "\nfailed to allocate MAD buffer\n");
                goto cleanup;
        }

	 mad = umad_get_mad(umad_buff);

	memcpy((struct sa_mad_packet_t *)mad, pkt,sizeof(struct sa_mad_packet_t));
	
        rc = umad_set_addr(umad_buff, ctx->port_attr.sm_lid, 1, ctx->port_attr.sm_sl, MELLANOX_QKEY);
        if (rc < 0) {
                fprintf(stderr, "\nfailed to set the destination address of the SMP\n");
                goto cleanup;
        }
        timeout_ms = 100;
        num_retries = 5;

        rc = umad_send(portid, agentid, umad_buff, MAD_SIZE, timeout_ms, num_retries);
        if (rc < 0) {
                fprintf(stderr, "\nfailed to send MAD\n");
                goto cleanup;
        }
        length = MAD_SIZE;

        rc = umad_recv(portid, umad_buff, &length, (10 * timeout_ms * num_retries));
        if (rc < 0) {
                fprintf(stderr, "\nfailed to receive MAD response\n");
                goto cleanup;
        }

        /* make sure that the join\leave multicast group request was accepted */
        rc = check_mad_status((struct sa_mad_packet_t *)mad);
        if (rc) {
                fprintf(stderr, "\nfailed to get mlid from MAD\n");
                goto cleanup;
        }

        /* if a "join multicast group" message was sent */
         get_mlid_from_mad((struct sa_mad_packet_t *)mad, &ctx->mcast_data.mlid);

#if 0
	/* Copy the returned mad structure over original ib packet */
	memcpy(pkt,(struct sa_mad_packet_t *) mad,sizeof(struct sa_mad_packet_t));	
#endif

	fprintf(stdout, "\n ***node has joined multicast group, mlid = 0x%x*** \n", ctx->mcast_data.mlid);

        test_result = 0;

cleanup:
        if (umad_buff)
                umad_free(umad_buff);

        if (portid >= 0) {
                if (agentid >= 0) {
                        if (umad_unregister(portid, agentid)) {
		fprintf(stderr, "\nfailed to deregister UMAD agent  for MADs\n");
                                test_result = 1;
                        }
                }

                if (umad_close_port(portid)) {
                        fprintf(stderr, "\nfailed to close UMAD portid\n");
                        test_result = 1;
                }
        }

	enib_debug("Exit: bx_send_sa_mad");
        return test_result;
}

static int bx_join_multicast_group(struct sa_mad_packet_t* sa_mad_pkt)
{
        int rc;

	enib_debug("Enter: bx_join_multicast_group");

       /* query port properties */
       if (ibv_query_port(ctx->context, ctx->port, &ctx->port_attr)) {
               fprintf(stderr, "ibv_query_port on port %u failed\n", ctx->port);
             return 1;
        }

        rc = ibv_query_pkey(ctx->context, ctx->port, DEF_PKEY_IDX, &ctx->mcast_data.pkey);
        if (rc) {
                fprintf(stderr, "\nfailed to query PKey table of port %d with index %d\n", ctx->port, DEF_PKEY_IDX);
                return 1;
        }

        rc = ibv_query_gid(ctx->context, ctx->port, 0, &ctx->mcast_data.port_gid);
        if (rc) {
                fprintf(stderr, "\nfailed to query GID table of port %d with index %d\n", ctx->port, 0);
                return 1;
        }

        /* mlid will be assigned to the new LID after the join */
        if (umad_init() < 0) {
                fprintf(stderr, "failed to init the UMAD library\n");
                return 1;
        }

        /* join the multicast group */
        rc = bx_send_sa_mad(sa_mad_pkt);
        if (rc) {
                fprintf(stderr, "\nfailed to join the mcast group\n");
                return 1;
        }
	
	enib_debug("Exit: bx_join_multicast_group");

        return 0;
}

uint16_t bx_send_sa(struct sa_mad_packet_t* sa_mad_pkt)
{
	int rc = 0, i=0;
	union ibv_gid           mgid;
	
	enib_debug("Enter bx_send_sa");

	rc = bx_join_multicast_group(sa_mad_pkt);
	if(rc)
	{
		fprintf(stderr, "\n!!! Failed to join the multicast group !!!\n");
		return -1;
	}
	
#if 0
	enib_printf("*** MAD packet received ***");

        for (i = 0; i <  MAD_SIZE ; i++) {
                if ((i % 16) == 0)
                        printf("\n");
                printf(" %02x", (unsigned char) (*((char *)sa_mad_pkt + i)) & 0xff);
        }
        printf("\n");
#endif
	

        memset(&mgid.raw[0], 0, sizeof(mgid.raw));

        mgid.raw[0] = 0xFF;
        mgid.raw[1] = 0x12;
        mgid.raw[2] = 0xE0;
        mgid.raw[3] = 0x1b;

        rc = ibv_attach_mcast(ctx->qp, &mgid , ctx->mcast_data.mlid);
        if(rc) {
                fprintf(stderr, "\n Failed to attach the multicast qpn 0x%02x to multicast group\n", ctx->qp->qp_num, rc);
                perror("ibv_attach_mcast fails with message");
                return -1;
        }

        enib_printf("** Attached QP to Multicast Group ***");

	enib_debug("Exit bx_send_sa");
	return ctx->mcast_data.mlid;
}

int bx_exit()
{
	enib_debug("Enter: bx_exit");
	bx_close_ctx();
	ibv_free_device_list(dev_list);
	free(rem_dest);
	enib_debug("Exit: bx_exit");
}

