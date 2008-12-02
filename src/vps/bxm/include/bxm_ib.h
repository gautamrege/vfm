/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 *
 * Header File: Data Structures for IB header and packets
 */
#ifndef __BXM_IB_H_
#define __BXM_IB_H_

#include <common.h>
#include <bxm_iboe.h>
#include <vfm_fip.h>


#define GID_LENGTH  16  /* Global ID :16 bytes*/

/* Queue key */
#define QK_BXM_DISCOVERY      0x80020000
#define QK_BRIDGEX_DISCOVERY  0x80020000
#define QK_EOIB               0x80020002
#define QK_FCOIB              0x80020004

/* Management base version */
#define BXM_MGMT_BASE_VERSION  0x01


/* Management classes */
#define BXM_MGMT_SUBN_ADM_CLASS 0x03

/* Management methods */
#define BXM_MGMT_GET_METHOD             0x01
#define BXM_MGMT_SUBN_ADM_METHOD_SET    0x02
#define BXM_MGMT_SUBN_ADM_METHOD_DELETE 0x015


/*** definitions section for MADs ***/
#define SUBN_ADM_ATTR_MC_MEMBER_RECORD 0x38
/* Subnet Administration class */
#define MANAGMENT_CLASS_SUBN_ADM       0x03
#define MCMEMBER_JOINSTATE_FULL_MEMBER 0x1
/* the size of a MAD is 256 bytes */
#define MAD_SIZE                       256
/* Q_Key value of QP1 */
#define QP1_WELL_KNOWN_Q_KEY           0x8002002
/* TransactionID */
#define DEF_TRANS_ID                   0x12345678
#define DEF_TCLASS                     0
#define DEF_FLOW_LABLE                 0

/* poll CQ timeout in milisec */
#define MAX_POLL_CQ_TIMEOUT 10000
#define GRH_SIZE            40
#define DEF_PKEY_IDX        0
#define DEF_SL              0
#define DEF_SRC_PATH_BITS   0
#define DEF_QKEY            0x80020002
#define MULTICAST_QPN       0xFFFFFF
#define DEF_PKEY            0xFFFF

/* OPCODE AND SUBCODE*/
#define GW_ADV_OPCODE           0xFFF9
#define GW_ADV_SUBCODE          0x02
#define HOST_SOLICIT_SUBCODE    0x01

#define IPV6_VER                0x6
#define HOP_LIMIT               0x1

/* Send Type */
#define UD_SEND_ONLY            0x04

/* FIP Version */
#define FIP_VERSION             0x1


enum subn_adm_component_mask_t {
        SUBN_ADM_COMPMASK_MGID         = (1ULL << 0),
        SUBN_ADM_COMPMASK_PORT_GID     = (1ULL << 1),
        SUBN_ADM_COMPMASK_Q_KEY        = (1ULL << 2),
        SUBN_ADM_COMPMASK_P_KEY        = (1ULL << 7),
        SUBN_ADM_COMPMASK_TCLASS       = (1ULL << 6),
        SUBN_ADM_COMPMASK_SL           = (1ULL << 12),
        SUBN_ADM_COMPMASK_FLOW_LABEL   = (1ULL << 13),
        SUBN_ADM_COMPMASK_JOIN_STATE   = (1ULL << 16),
};

/* IB header */
typedef struct __bxm_ib_hdr {
        /*
         * LRH
         * Virtual Lane and Link version.
         * Vl << 4;
         * L-ver & 0x0F;
         */
        uint8_t vl_linkver;

        /*
         * Serveice level. and link next header.
         * sl << 4 , LNH & 0xF.
         */
        uint8_t sl_lnh;
        /* Destination local id*/
        uint16_t dlid;
        /* packet length .(11 bit)*/
        uint16_t length;
        /* Source local id */
        uint16_t slid;

        /*
         * ---- GRH ----
         * IP version . >> 28.
         * Traffic class.  & 0x0FF00000 >> 20.
         * Flow label. & 0x000FFFFF.
         */
        uint32_t ipver_tclass_flowlabel;
        /* Playload length */
        uint16_t payload_length;
        /* Next header */
        uint8_t next_header;
        /* Hope  limit */
        uint8_t hop_limit;
        /* Source global id */
        uint8_t sgid[GID_LENGTH];
        /* Destination global id */
        uint8_t dgid[GID_LENGTH];

        /*
         * ---- BTH ----
         * opration code :UD send only
         */
        uint8_t opcode;
        /*
         * SE : solicited event.( >> 7)
         * M  : Migration status.( >> 6 & 0x01)
         * PC : Pad count (>> 4 & 0x03)
         * THV: Transport header version( & 0x0F)
         */
        uint8_t se_m_pc_thv;
        /* partition key */
        uint16_t pkey;
        /* Dest queue pair */
        uint32_t dqp;
        /* Acknowledgement request (>> 31), PSN & 0x00FFFFFF*/
        uint32_t A_psn;
        uint32_t queue_key; /* Queue Key*/
        /* Source queue pair*/
        uint32_t sqp;
        /* version >> 24 */
        uint32_t  version;

}bxm_ib_hdr;

/* MAD header ( Table 112 : IB Spec 1.2.1) */
typedef struct __bxm_mad_hdr {
        /* Version of mad base format */
        uint8_t   base_version;
        /* Class of operation */
        uint8_t   mgmt_class;
        /* Version of MAD class */
        uint8_t   class_version;
        /* Response bit R(1 bit) and method(7 bit) */
        /* resonse bit and method : r >> 7, method & 0xEF */
        uint8_t   R_method;
        /* Status of opration  */
        uint16_t  status;
        uint16_t  class_specific;
        /* Transaction ID */
        uint64_t  tid;
        /* Attribute Id */
        uint16_t  attr_id;
        uint16_t  reserved;
        /* Attribute Modifier */
        uint32_t  attr_mod;
}bxm_mad_hdr;

/* RMPP header ( Table 170 : IB Spec 1.2.1 */
typedef struct __bxm_rmpp_hdr {
        /* RMPP version */
        uint8_t version;
        /* RMPP packet type */
        uint8_t type;
        uint8_t res_time_flags;
        uint8_t status;
/*
        uint32_t seg_num;
        uint32_t payload_length;
*/

}bxm_rmpp_hdr;


/* SA header ( Table 195 : IB Spec 1.2.1) */
typedef struct __bxm_sa_hdr {
        uint64_t sm_key;
        uint16_t attr_offset;
        uint16_t reserved;
        uint64_t comp_mask;

}bxm_sa_hdr;

/* MAD packet structure */
typedef struct __bxm_rmpp_packet {
       bxm_mad_hdr  mad_hdr;
       bxm_rmpp_hdr rmpp_hdr;
       uint8_t *data;
}bxm_mad_packet;

/* MCMemberrecord */
typedef struct __bxm_mcmemeber_record {
        uint8_t  mgid[GID_LENGTH];
        uint8_t  portgid[GID_LENGTH ];
        uint32_t q_key;
        uint16_t mlid;
        /* MTU selector (2bit), MTU(8 bit)*/
        uint8_t  mtuselector_mtu;
        uint8_t  tclass;
        uint16_t p_key;
        /* Rateselector (2 bit ) , Rate(6bit) */
        uint8_t  rateslector_rate;
        /* Packet time selector (2bit), packet life time(6 bit)*/
        uint8_t  pklife_selector_time;
        /* SL (4 bit ), Flow label (20 bit )*/
        uint8_t  sl_flowlable[3];
        uint8_t  hop_limit;
        /* scope (4 bit ), joinstat(4 bit) */
        uint8_t  scope_joinstate;
        /* Proxyjoin( 1 bit) ,reserved (23 bit )*/
        uint8_t proxyjoin_reserved[3];

}bxm_mcmemeber_record;


/* SA_MAD packet structure */
typedef struct __bxm_sa_mad_packet {
        bxm_mad_hdr  mad_hdr;
        bxm_rmpp_hdr rmpp_hdr;
        bxm_sa_hdr   sa_hdr;
        uint8_t data[200];
}bxm_sa_mad_packet;



/* htonll (for 64 bit)
uint64_t htonll(uint64_t n) {

        uint64_t temp;

        temp = ((uint64_t) htonl(n & 0xFFFFFFFFLLU)) << 32;
        temp |= htonl((n & 0xFFFFFFFF00000000LLU) >> 32);

        return temp;

}*/


/**
 * Send IB packet.
 *
 * [IN] *info_pack   : Info data( QPN, GID, LID).
 * [IN] *control_hdr : Control header
 *
 * Return : err.
 *
 */
vps_error
bxm_send_ib_packet(bxm_init_info_packet *info_pack, ctrl_hdr *control_hdr,
                   uint8_t *buff);

/* Send GW advertisements */
vps_error
bxm_send_uincast_gw_adv(eoib_conx_vfm_adv *solicit_adv);

/* Send GW advertisements */
vps_error
bxm_send_gw_adv();




#endif /* __BXM_IB_H_*/
