/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 *
 * Parse and create IB header.
 */
#include<bxm_ib.h>
#include<ext/eoib-swgw/bx.h>
#include<bxm_queue.h>

#define GRH_PLUS_BTH_LEN 16  /* DWORDS */

#define GW_ADV_PAYLOAD_LEN 14

extern bxm_init_info_packet init_info_pack;

uint8_t mcast_gid[16] = {0xFF, 0x12, 0xE0, 0x1B, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0};
uint16_t mcast_lid;


uint8_t gw_adv[] = {0x0D, 0x03, 0x00, 0x00,
                    0x6D, 0x65, 0x6C, 0x6C,
                    0x61, 0x6E, 0x6F, 0x78,
                    0xF0, 0x07, 0x00, 0x00,
                    0x6D, 0x65, 0x6C, 0x6C,
                    0x61, 0x6E, 0x6F, 0x78,
                    0x00, 0xFF, 0xFF, 0xFF,
                    0x00, 0x01, 0x00, 0x06,
                    0x00, 0x02, 0xC9, 0x02,
                    0x00, 0x00, 0x76, 0x28,
                    0xF1, 0x04, 0x00, 0x00,
                    0x6D, 0x65, 0x6C, 0x6C,
                    0x61, 0x6E, 0x6F, 0x78,
                    0x10, 0x00, 0x00, 0x01};

/**
 * Parse IB header from buffer read by BridgeX API recv() call.
 *
 * [IN] *buff     : Input buffer.
 * [INOUT] *offset: Offset of buffer.
 * [OUT] *ib_hdr  : IB header.
 *
 * Return : err
 */
vps_error
parse_ib_header(uint *buff, uint32_t *offset, bxm_ib_hdr *ib_hdr)
{
        vps_error err = VPS_SUCCESS;

        /* Copy IB header from the input buffer */
        memcpy(ib_hdr, buff, sizeof(bxm_ib_hdr));
        *offset += sizeof(bxm_ib_hdr);

        /* Convert network byte order to host byte orderi */
        ib_hdr->dlid = ntohs(ib_hdr->dlid);
        ib_hdr->length = ntohs(ib_hdr->length);
        ib_hdr->slid = ntohs(ib_hdr->slid);
        ib_hdr->ipver_tclass_flowlabel =
                                ntohl(ib_hdr->ipver_tclass_flowlabel);
        ib_hdr->payload_length = ntohs(ib_hdr->payload_length);
        ib_hdr->pkey = ntohs(ib_hdr->pkey);
        ib_hdr->dqp  = ntohl(ib_hdr->dqp);
        ib_hdr->A_psn = ntohl(ib_hdr->A_psn);
        ib_hdr->queue_key = ntohl(ib_hdr->queue_key);
        ib_hdr->sqp = ntohl(ib_hdr->sqp);

        vps_trace(VPS_ENTRYEXIT, "Leaving read_ib_header");

        return err;

}

/**
 * Create IB header.
 *
 * [OUT] *ib_hdr : IB header.
 *
 */
vps_error
create_ib_header(bxm_ib_hdr *ib_hdr)
{
        vps_error err = VPS_SUCCESS;

        vps_trace(VPS_ENTRYEXIT, "Entering create_ib_header");

        /**
	 * Vlane and link version.
         * Vl << 4;
         * L-ver & 0x0F;
         */
        ib_hdr->vl_linkver = 0x00;

        /*
         * Serveice level. and link next header.
         * sl << 4 , LNH & 0xF.
         */
        ib_hdr->sl_lnh = 0x03;
        /* Destination local id*/
        /* TODO:  testing ib_hdr->dlid = htons(ib_hdr->dlid); */
        ib_hdr->dlid = htons(ib_hdr->dlid);
        ib_hdr->length = htons(ib_hdr->length);

        ib_hdr->slid = htons(init_info_pack.lid);

        /*** ---- GRH ----***/
        /*
         * IP version . >> 28.
         * Traffic class.  & 0x0FF00000 >> 20.
         * Flow label. & 0x000FFFFF.
         */
        ib_hdr->ipver_tclass_flowlabel = htonl(IPV6_VER << 28);

        ib_hdr->payload_length = htons(ib_hdr->payload_length);
        /* Next header */
        ib_hdr->next_header = 0x10;
        /* Hope  limit */
        ib_hdr->hop_limit = HOP_LIMIT;
        /* Source Global id */
        memcpy(ib_hdr->sgid, init_info_pack.gid, sizeof(init_info_pack.gid));

        /*** ---- BTH ---- ****/
        /* opration code :UD send only*/
        ib_hdr->opcode = UD_SEND_ONLY;
        /*
         * SE : solicited event.( >> 7)
         * M  : Migration status.( >> 6 & 0x01)
         * PC : Pad count (>> 4 & 0x03)
         * THV: Transport header version( & 0x0F)
         */
        ib_hdr->se_m_pc_thv = 0x40;
        /* partition key */
        ib_hdr->pkey = htons(DEF_PKEY);
        /* Dest  queue pair */
        ib_hdr->dqp = htonl(ib_hdr->dqp);
        /* Acknowledgement request (>> 31), PSN & 0x00FFFFFF*/
        ib_hdr->A_psn = 0x00;
        ib_hdr->queue_key = htonl(QK_EOIB);
        /* Source queue pair*/
        ib_hdr->sqp = htonl(init_info_pack.qp);
        /* version = 1*/
        ib_hdr->version = htonl(FIP_VERSION << 31);

        vps_trace(VPS_ENTRYEXIT, "Leaving create_ib_header");
        return err;
}


/*
 * Create MAD RMPP header.
 * [IN] *mad_hdr : MAD header.
 *
 * Return  :err
 */
vps_error
create_mad_header(bxm_mad_hdr *mad_hdr)
{
        vps_error err = VPS_SUCCESS;

        vps_trace(VPS_ENTRYEXIT, "Entering create_mad_header");

        mad_hdr->base_version  = BXM_MGMT_BASE_VERSION;
        mad_hdr->mgmt_class    = BXM_MGMT_SUBN_ADM_CLASS;
        mad_hdr->class_version = 0x02;
        mad_hdr->R_method      = INSERTF(mad_hdr->R_method, 0,
                                BXM_MGMT_SUBN_ADM_METHOD_SET, 0, 7);
        mad_hdr->status        = 0x0000;
        mad_hdr->class_specific= 0x00;
        mad_hdr->tid           = htonll((uint64_t)DEF_TRANS_ID);
        mad_hdr->attr_id       = htons(SUBN_ADM_ATTR_MC_MEMBER_RECORD);
        mad_hdr->reserved      = 0x00;
        mad_hdr->attr_mod      = 0x00;


        vps_trace(VPS_ENTRYEXIT, "Leaving create_mad_header");
        return err;
}

/*
 * Fill Subnet Admin data.
 * [IN] *pkey           : Partation key.
 * [IN] *port_gid       : Port GID.
 * [OUT] *mc_record     : MCMemberrecord.
 *
 * return: err
 */

vps_error
fill_mcmember_record(uint16_t pkey, uint8_t *port_gid,
                        bxm_mcmemeber_record *mc_record)
{
        vps_error err = VPS_SUCCESS;

        vps_trace(VPS_ENTRYEXIT, "Entering fill_mcmember_record");

        memcpy(mc_record->mgid, mcast_gid, 16);
        memcpy(mc_record->portgid, port_gid, 16);
        mc_record->q_key = htonl(MELLANOX_QKEY);
        mc_record->p_key = htons(pkey);
        mc_record->tclass = DEF_TCLASS;
#if 0
        mc_record->sl_flowlable[0] = INSERTF(mc_record->sl_flowlable[0],
                                        4, DEF_SL, 0, 4);
        mc_record->sl_flowlable[0] = INSERTF(mc_record->sl_flowlable[0], 0,
                                        DEF_FLOW_LABLE, 16, 4);
        mc_record->sl_flowlable[1] = INSERTF(mc_record->sl_flowlable[1], 0,
                                        DEF_FLOW_LABLE, 8, 8);
        mc_record->sl_flowlable[2] = INSERTF(mc_record->sl_flowlable[1], 0,
                                        DEF_FLOW_LABLE, 0, 8);
#endif
        mc_record->rateslector_rate = 0;
        mc_record->pklife_selector_time = 0;
        mc_record->sl_flowlable[0] = 0;
        mc_record->sl_flowlable[1] = 0;
        mc_record->sl_flowlable[2] = 0;
        mc_record->scope_joinstate = MCMEMBER_JOINSTATE_FULL_MEMBER;

        vps_trace(VPS_ENTRYEXIT, "Entering fill_mcmember_record");
        return err;

}


/*
 * Create Multicast MAD packet.
 *
 * [IN] *pkey          : Partation key.
 * [IN] *port_guid     : IB Port GUID.
 * [OUT]*sa_mad_packet : SA + MAD packet.
 *
 * Return : err;
 */
vps_error
create_mcast_mad_packet(uint16_t pkey, uint8_t *port_guid,
                        bxm_sa_mad_packet *sa_mad_packet)
{
        vps_error err  = VPS_SUCCESS;
        uint64_t comp_mask;
        bxm_mcmemeber_record mc_rec;

        vps_trace(VPS_ENTRYEXIT, "Entering create_mcast_mad_packe");

        create_mad_header(&(sa_mad_packet->mad_hdr));

        fill_mcmember_record(pkey, port_guid, &mc_rec);
        memcpy(sa_mad_packet->data, &mc_rec, sizeof(bxm_mcmemeber_record));

        comp_mask = SUBN_ADM_COMPMASK_MGID | SUBN_ADM_COMPMASK_PORT_GID |
                        SUBN_ADM_COMPMASK_Q_KEY | SUBN_ADM_COMPMASK_P_KEY |
                        SUBN_ADM_COMPMASK_TCLASS | SUBN_ADM_COMPMASK_SL |
                        SUBN_ADM_COMPMASK_FLOW_LABEL |
                        SUBN_ADM_COMPMASK_JOIN_STATE;

        sa_mad_packet->sa_hdr.comp_mask = htonll(comp_mask);


        vps_trace(VPS_ENTRYEXIT, "Leaving create_mcast_mad_packe");
        return err;

}

/* Send GW advertisements*/
vps_error
bxm_send_gw_adv()
{

        vps_error err = VPS_SUCCESS;
        ctrl_hdr control_hdr;
        bxm_sa_mad_packet mc_join;
        bxm_init_info_packet info_pack;
        uint8_t *buff;

        vps_trace(VPS_ENTRYEXIT, "Entering bxm_send_packet");

        memset(&mc_join, 0, sizeof(bxm_sa_mad_packet));

        /* Create Mcast Gid */
        create_mcast_mad_packet(DEF_PKEY, init_info_pack.gid, &mc_join);

        /* Send SA packet to join muticast group */
        mcast_lid = bx_send_sa((struct sa_mad_packet_t*)&mc_join);

        /* Set Destination information */
        info_pack.lid = mcast_lid;
        info_pack.qp  = MULTICAST_QPN;
        memcpy(info_pack.gid, mcast_gid, sizeof(mcast_gid));


        /* Fill control header infirmation */
        control_hdr.opcode  = htons(GW_ADV_OPCODE);
        control_hdr.subcode = GW_ADV_SUBCODE;
        control_hdr.reserved = 0;
        control_hdr.desc_list_length = GW_ADV_PAYLOAD_LEN;
        control_hdr.flags |= SET_FP;
        control_hdr.flags |= SET_SP;
        control_hdr.flags |= SET_F;

        control_hdr.flags = htons(control_hdr.flags);

        /* Create GW advertisement payload */
        prepare_eoib_vNIC_advertisement(&control_hdr, &init_info_pack, &buff);
        /* For multicast GW ADV set number of vlan = 0 */
//        buff[control_hdr.desc_list_length -1] = 0;
//        buff[control_hdr.desc_list_length] = 0;

        /* Send ib packet  */
        bxm_send_ib_packet(&info_pack, &control_hdr, buff);

        free(buff);

        vps_trace(VPS_ENTRYEXIT, "Entering bxm_send_packet");

        return err;
}


/* Send GW advertisements*/
vps_error
bxm_send_uincast_gw_adv(eoib_conx_vfm_adv *solicit_adv)
{

        vps_error err = VPS_SUCCESS;
        ctrl_hdr control_hdr;
        bxm_init_info_packet info_pack;
        uint8_t *buff;

        vps_trace(VPS_ENTRYEXIT, "Entering bxm_send_packet");

        /* Set Destination information */
        info_pack.lid = htons(solicit_adv->mlnx_240.lid);
        info_pack.qp  = htonl(solicit_adv->mlnx_240.qpn);
        memcpy(info_pack.gid, solicit_adv->mlnx_240.gw_guid,16);


        /* Fill control header infirmation */
        control_hdr.opcode  = htons(GW_ADV_OPCODE);
        control_hdr.subcode = GW_ADV_SUBCODE;
        control_hdr.reserved = 0;
        control_hdr.desc_list_length = GW_ADV_PAYLOAD_LEN;
        control_hdr.flags |= SET_FP;
        control_hdr.flags |= SET_SP;
        control_hdr.flags |= SET_F;
        control_hdr.flags |= SET_A;
        control_hdr.flags |= SET_S;

        control_hdr.flags = htons(control_hdr.flags);

        /* Create GW advertisement payload */
        prepare_eoib_vNIC_advertisement(&control_hdr, &init_info_pack, &buff);

        /* Send ib packet  */
        bxm_send_ib_packet(&info_pack, &control_hdr, buff);

        free(buff);

        vps_trace(VPS_ENTRYEXIT, "Entering bxm_send_packet");

        return err;
}

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
                   uint8_t *buff)
{
        vps_error err = VPS_SUCCESS;
        bxm_ib_hdr ib_hdr;
        uint8_t packet_buff[1500];
        uint32_t offset = 0;
        uint32_t payload_length = control_hdr->desc_list_length;

        vps_trace(VPS_ENTRYEXIT, "Entering bxm_send_packet");


        ib_hdr.dlid = info_pack->lid;
        memcpy(ib_hdr.dgid, info_pack->gid, sizeof(info_pack->gid));
        ib_hdr.dqp = info_pack->qp;

        ib_hdr.length =  control_hdr->desc_list_length + sizeof(ctrl_hdr) / 4 +
                                        GRH_PLUS_BTH_LEN;
        ib_hdr.payload_length = control_hdr->desc_list_length +
                sizeof(ctrl_hdr) / DWORD;

        /* Create IB header and copy to buffer*/
        create_ib_header(&ib_hdr);
        memcpy(packet_buff, &ib_hdr, sizeof(bxm_ib_hdr));
        offset += sizeof(bxm_ib_hdr);

        /* Copy control header*/
        control_hdr->desc_list_length= htons(control_hdr->desc_list_length); 
        memcpy(packet_buff + offset, control_hdr, sizeof(ctrl_hdr));
        offset += sizeof(ctrl_hdr);


        /* memcpy(packet_buff + offset, buff, payload_length * 4);*/
        memcpy(packet_buff + offset, buff, payload_length * DWORD);
        offset +=  payload_length * DWORD;

        /* Call BridgeX API send() */
        bx_send(packet_buff);

        vps_trace(VPS_ENTRYEXIT, "Leaving  bxm_send_packet");
        return err;
}

/**
 * Start IB sniffer.
 * recv packet and add to queue.
 *
 */
#if 0
void *
start_ib_sniffer(void *arg)
{
        vps_error err = VPS_SUCCESS;
        struct bx_recv_pack ib_packet;

        while (1)
        {
                // Call BridgeX API call recv
                if (bx_recv(bx, &ib_packet) == 0)
                {
                }
        }

}
#endif

/**
 * IB packet processing
 */
void *
start_ib_processor(void *arg)
{
        while (1)
        {
                /* get packet from queue and strat processing */
        }

}
