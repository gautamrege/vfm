/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#include <net_util.h>
#include <map_util.h>
#include <vfm_queue.h>
#include <vfm_iboe.h>

#ifdef VFM_EOIB
#include <bx.h>
#endif

extern uint8_t g_local_mac[MAC_ADDR_LEN];
extern uint8_t g_bridge_mac[MAC_ADDR_LEN];
extern uint32_t g_if_index;
extern uint8_t g_fc_map[3];
uint32_t g_gw_src_id;
uint32_t g_fcoe_t11;

/* byte wise VLAN Tag Postion in  EN header */
#define VLAN_TAG_POS 12

/* Get vlan tag */
#define GET_VLAN_TAG(buff) (uint16_t)(buff[VLAN_TAG_POS] << 8 | \
                buff[VLAN_TAG_POS+1])

#define FCOE_VER 0
#define FCOE_SOF 0xe
#define FCOE_EOF 0x42

#define FCOE_ENCAPS_LEN_SOF(len) \
         ((FCOE_VER << 14) | ((len & 0x3ff) << 4) | (FCOE_SOF & 0xf))

#define FIP_VERSION_MASK     0XF000         /* vesion size: 4 bits */
#define GW_ID_MASK           0X00FFFFFF     /* gateway id size: 24 bits */
#define VFM_ID_MASK          0X00FFFFFF     /* vfm id size: 24 bits */
#define SET_E                0x80           /* to set the tunnel hdr E flag */

/*Get First byte */
#define GET_FIRST_BYTE(dword) (uint8_t)((dword & 0xFF000000) >> 24)

/*Get Last 3 bytes */
#define GET_LAST_3_BYTES(dword) (dword & 0x00FFFFFF)

/*LOCAL or REMOTE vmf*/
#define LOCAL_VFM  0
#define REMOTE_VFM 1

#define FCOE_HDR_SIZE 4*DWORD
#define OPEN_FCOE_HDR_SIZE 7*DWORD
#define OPEN_FCOE_RESERVED 3*DWORD

/*
 * Read ethernet header and check for FIP packet.
 * [IN]  *buff          : buffer read from socket.
 * [IN]  size           : Read number of bytes read from socket.
 * [INOUT] *ret_pos   : Number of bytes processed.
 * [OUT] fip_eth_hdr: Ethernet header structure.
 *
 * Return : Error code.
 *          FIP packet or not.
 *          Incomplete packet.
 */
vps_error
read_eth_hdr(uint8_t *buff,
                uint32_t size,
                uint32_t *ret_pos,
                eth_hdr *fip_eth_hdr)
{
        vps_error err = VPS_SUCCESS;

        vps_trace(VPS_ENTRYEXIT, "Entering read_eth_hdr");

        /*
         * If receive bytes count less then ehter header size
         * then return.
         */
        if ((size - *ret_pos) < ETH_HDR_LEN) {
                vps_trace(VPS_INFO, "Incomplete packet");
                err = VPS_ERROR_INCOMPLETE_PK;
                goto out;
        }

        /*Copy dest, src hosts mac address and vlan tag from buffer */
        memcpy(fip_eth_hdr, buff + *ret_pos, sizeof(eth_hdr));
        *ret_pos += sizeof(eth_hdr);

        /* VLAN TAG not present in packet then*/
        if (GET_VLAN_TAG(buff) != VLAN_TAG) {
                /*
                 * reduce return pointer postion of buffer by 2 DWORD
                 * because vlan tag is not present.
                 */
                *ret_pos = *ret_pos - 2 * DWORD;

                /*
                 * Copied Vlan tag bytes are ether type, version, reserved.
                 * So copy vlan tag bytes to ether type, version , reserved.
                 */
                memcpy(&(fip_eth_hdr->ether_type), buff + *ret_pos,
                                sizeof(uint16_t));
                memcpy(&(fip_eth_hdr->version),
                                buff + *ret_pos + sizeof(uint16_t),
                                sizeof(uint16_t));

                fip_eth_hdr->vlan_tag = 0;
                *ret_pos += DWORD;
        }

        /* Convert network byte order to host byte order */
        fip_eth_hdr->ether_type = ntohs(fip_eth_hdr->ether_type);

        /* Convert network byte order to host byte order */
        /* To get last nibble because version size is 1 nibble(4 bits)*/
        fip_eth_hdr->version = ntohs(fip_eth_hdr->version) & FIP_VERSION_MASK;
        fip_eth_hdr->version >>= 12;

        vps_trace(VPS_INFO, "FIP Packet received");
out:
        vps_trace(VPS_ENTRYEXIT, "Leaving read_eth_hdr");
        return err;
}

/**
 * Read fiber channel header .
 * [IN]  *buff          : buffer read from socket.
 * [IN]  size           : Read number of bytes read from socket.
 * [INOUT] *ret_pos : Number of bytes processed.
 * [OUT] *fip_fc_hdr: FC header structure.
 *
 * Return : Error code.
 *                  FIP packet or not.
 *                  Incomplete packet.
 */
vps_error
read_fc_hdr(uint8_t *buff,
                uint32_t size,
                uint32_t *ret_pos,
                fc_hdr *fip_fc_hdr)
{
        vps_error err = VPS_SUCCESS;


        vps_trace(VPS_ENTRYEXIT, "Entering read_fc_hdr");

        /*
         * If receive bytes count less then FC header size
         * then return.
         */
        if ((size - *ret_pos) < sizeof(fc_hdr)) {
                vps_trace(VPS_WARNING, "Incomplete packet");
                err = VPS_ERROR_INCOMPLETE_PK;
                goto out;
        }

        /*Fill header from buffer*/
        memcpy(fip_fc_hdr, buff+ *ret_pos, sizeof(fc_hdr));

        /*Convert network to host byte order*/
        fip_fc_hdr->rt_ctrl_dest_id = ntohl(fip_fc_hdr->rt_ctrl_dest_id);

        fip_fc_hdr->src_id = ntohl(fip_fc_hdr->src_id) & 0x00FFFFFF;

        fip_fc_hdr->type_frame_ctrl = ntohl(fip_fc_hdr->type_frame_ctrl);
        fip_fc_hdr->seq_count =  ntohs(fip_fc_hdr->seq_count);
        fip_fc_hdr->ox_id  = ntohs(fip_fc_hdr->ox_id);
        fip_fc_hdr->res_id = ntohs(fip_fc_hdr->res_id);
        fip_fc_hdr->parameter = ntohl(fip_fc_hdr->parameter);

        /*Increment offset of buffer*/
        *ret_pos += sizeof(fc_hdr);

   out:
         vps_trace(VPS_ENTRYEXIT, "Leaving read_fc.hdr");
         return err;

}

/**
 * parse FC packet
 *  This method processes the FC packet.
 *
 * [IN]  *buff           : buffer read from socket.
 * [IN]  size                : Read number of bytes read from socket.
 * [INOUT] *ret_pos        : Number of bytes processed.
 * [IN] *tunnel_hdr : mellanox tunnel header structure.
 *
 * Return : error code.
 *                  incomplete packet.
 */
vps_error
process_fc_response(uint8_t *buff,
                uint32_t size,
                uint32_t *ret_pos,
                mlx_tunnel_hdr *tunnel_hdr)
{

        vps_error err = VPS_SUCCESS;
        fc_hdr fc_header;   /* FC header  */
        req_entry_map *entry;
        uint16_t ox_id;

        vps_trace(VPS_ENTRYEXIT, "Entering process_fc_response");

        /* Read the FC header and decide if its a request or a response */
        read_fc_hdr(buff, size, ret_pos, &fc_header);

        /* IF fc request coming from FC plane reject it */
         if(g_gw_src_id == (fc_header.rt_ctrl_dest_id & 0x00FFFFFF)) {
                vps_trace(VPS_INFO, "ELS request packet from FC to SWGW");
                create_reject_els(fc_header);
                goto out;
         }
        /* Response processing */
        /* Find the oxid from the FC header and lookup in the map. */
        /* Process the response */

        if (fc_header.src_id != 0x00FFFFFE) {
                /* process for els response*/
                process_fc_els_res(buff, size, ret_pos, tunnel_hdr, &fc_header,
                                   entry);
                goto out;
        }

        vps_trace(VPS_INFO, "OXID OF INCOMING PACKET:%d", fc_header.ox_id);
        ox_id = fc_header.ox_id;

        /* get entry from map*/
        entry =  get_entry_from_map(fc_header.ox_id);

        /* IMPORTANT: entry should NOT be NULL here */

        /* If map entry for VFM flogi then Free map entry and return*/
        if (entry->flag == 0x1) {
                if (buff[*ret_pos] == 0x01) {
                        /*
                         * TODO: We should NOT exit. We need to continue
                         * processing as  there could be other gateways too :D
                         */
                        vps_trace(VPS_INFO,
                                  "--** VFM FLOGI(RJT).. Aborting!! **--");
                        exit(1);
                }
                vps_trace(VPS_ERROR, "--** VFM FLOGI(ACC) response **--");
                g_gw_src_id = fc_header.rt_ctrl_dest_id & 0x00FFFFFF;
                remove_entry_from_map(ox_id);
                goto out;
        }
        /*
         * else if (entry->flag == 0x03) {
         *      process for plogi or other els response
         *
         *      process_fc_els_res(buff, size, ret_pos, tunnel_hdr, &fc_header,
         *                          entry);
         *      remove_entry_from_map(ox_id);
         *               goto out;
         * }
         */


        /*Check for LS command code*/
        switch (buff[*ret_pos]) {
                case 0x02:     /* FLOGI/FDISK ACC response */
                        prepare_fdisc_res(buff, ret_pos, entry,
                            &fc_header, 0x01); /* 0x01 = LS ACC */
                        vps_trace(VPS_ERROR,
                            "--** Sent FLOGI(ACC) response to IO_Module --**");
                        break;

                case 0x01:     /* FLOGI/FDISK REJECT response */
                        prepare_fdisc_res(buff, ret_pos, entry,
                            &fc_header, 0x00); /* 0x00 = LS RJT */
                        vps_trace(VPS_WARNING,
                            "--** Sent FLOGI(RJT) response to IO_Module --**");
                        break;

        }

        /*Free map entry*/
        remove_entry_from_map(ox_id);
out:

        vps_trace(VPS_ENTRYEXIT, "Leaving process_fc_response");
        return err;
}


/**
 * parse tunnel header
 * [IN]  *buff           : buffer read from socket.
 * [IN]  size                : Read number of bytes read from socket.
 * [INOUT] *ret_pos        : Number of bytes processed.
 * [OUT] tunnel_flag : it indicate tunnel header present or not
 * [OUT] *tunnel_hdr : mellanox tunnel header structure.
 *
 * Return : error code.
 *                  incomplete packet.
 */
vps_error
read_tunnel_hdr(uint8_t *buff,
                uint32_t size,
                uint32_t *ret_pos,
                uint8_t *tunnel_flag,
                mlx_tunnel_hdr *tunnel_hdr)
{
        vps_error err = VPS_SUCCESS;

        vps_trace(VPS_ENTRYEXIT, "Entering read_tunnel_hdr");

        if ((size - *ret_pos) < sizeof(mlx_tunnel_hdr)) {
                vps_trace(VPS_WARNING, "Incomplete packet");
                err = VPS_ERROR_INCOMPLETE_PK;
                goto out;
        }

        /* If tunnel header present in packet then parse it */
        if (buff[*ret_pos] == TUNNEL_HDR_TYPE) {
                vps_trace(VPS_INFO, "Tunneled packet..");

                /* copy bytes from buffer to tunnel header strucuter  */
                memcpy(tunnel_hdr, buff + *ret_pos, sizeof(mlx_tunnel_hdr));

                *ret_pos = *ret_pos + sizeof(mlx_tunnel_hdr);

                /*
                 * convert network byte order to host byte order
                 * Mask first byte because both gw_id and vfm_id are 24 bits
                 */
                tunnel_hdr->gw_id  = ntohl(tunnel_hdr->gw_id)  & GW_ID_MASK;
                tunnel_hdr->vfm_id = ntohl(tunnel_hdr->vfm_id) & VFM_ID_MASK;

                /*Set tunnel flag*/
                *tunnel_flag = TRUE;
        }
        else {
                *tunnel_flag = FALSE;
                vps_trace(VPS_INFO, "Not Tunneled packet..");
        }

out :

        vps_trace(VPS_ENTRYEXIT, "Leaving read_tunnel_hdr");
        return err;
}

/**
 * parse control header
 * [IN]  *buff           : buffer read from socket.
 * [IN]  size                : Read number of bytes read from socket.
 * [OUT] *ret_pos        : Number of bytes processed.
 * [OUT] fip_ctrl_hdr: FIP Control header structure.
 *
 * Returns : error code
 *                   Incomplete packet
 */
vps_error
read_ctrl_hdr(uint8_t *buff,
                uint32_t size,
                uint32_t *ret_pos,
                ctrl_hdr *fip_ctrl_hdr)
{
        vps_error err = VPS_SUCCESS;

        vps_trace(VPS_ENTRYEXIT, "Entering read_ctrl_hdr");

        /* If number of read bytes less than control header then return */
        if ((size - *ret_pos) < sizeof(ctrl_hdr)) {
                vps_trace(VPS_WARNING, "Incomplete packet");
                err = VPS_ERROR_INCOMPLETE_PK;
                goto out;
        }

        /*Copy control header*/
        memcpy(fip_ctrl_hdr, buff + *ret_pos, sizeof(ctrl_hdr));

        /*Convert network byte order to host byte order*/
        fip_ctrl_hdr->opcode = ntohs(fip_ctrl_hdr->opcode);
        fip_ctrl_hdr->desc_list_length = ntohs(fip_ctrl_hdr->desc_list_length);
        fip_ctrl_hdr->flags = ntohs(fip_ctrl_hdr->flags);

        *ret_pos = *ret_pos + sizeof(ctrl_hdr);

out:

        vps_trace(VPS_ENTRYEXIT, "Leaving read_ctrl_hdr");
        return err;

}

/**
 * This function will decides what type of the packet is under
 * process depending upon the opcode and the subopcode into the
 * packtet's control header. And then call the respective function
 * that would fill up the data structures for the packet.
 * It would also send a response if required for the received packet.
 *
 * *fip_eth_hdr_fw  [IN] : Forwarded Ethernet header Structure.
 * *t_hdr           [IN] : Tunnel Header Structure.
 * *c_hdr           [IN] : Control header Structure.
 * *desc_buff       [IN] : The buffer that contains the payload.
 *
 * Returns vps_error.
 */
vps_error
decide_packet(eth_hdr *fip_eth_hdr_fw, mlx_tunnel_hdr *tunnel_hdr,
                ctrl_hdr * control_hdr, uint8_t *desc_buff)
{

        /*
         * Reference:
         * Opcode = 0x1 Subcode = 0x1 IO_Module Solicitation Descriptor
         * Opcode = 0x1 Subcode = 0x2 Gateway Advertisement Descripto
         * Opcode = 0x2 Subcode = 0x1 IO_Module Login/Logout Descriptor
         * Opcode = 0x2 Subcode = 0x1 Gateway Login Response Descripto
         * Opcode = 0x3 Subcode = 0x1 IO_Module Keep Alive Descriptor – vHBA
         * over EN
         * Opcode = 0x3 Subcode = 0x2 IO_Module Clear Virtual Link Descriptor
         */
        vps_error err = VPS_SUCCESS;

        fcoe_conx_vfm_adv solicit;
        fcoe_bridge_vfm_adv adv;
        fcoe_vHBA_alive alive;
        fcoe_vHBA_dereg dereg;

        /*Multicast mac address define by FCoE standards */
        uint8_t multicast_mac[MAC_ADDR_LEN] = {
                0x01, 0x10, 0x18, 0x01, 0x00, 0x01
        };

        vps_trace(VPS_ENTRYEXIT, "Entering decide_packet");
        switch (control_hdr->opcode) {
                case 1 :       /* Discovery Packet */
                    switch (control_hdr->subcode) {
                           case 1 :   /* IO_Module Solicitation */
                                vps_trace(VPS_ERROR,
                                "--** IO_MODULE DISCOVERY RECEIVED **--");
                                fcoe_conx_discovery(desc_buff, &solicit);

                                add_en_io_module_to_db(&solicit);
                                /*  Unicast Gateway Advertisement 
                                create_packet(1, 2, &solicit);
                                vps_trace(VPS_ERROR,
                                "--*** SENT UNICAST GW ADVERTISEMENT***--"); */
                                break;
                    }
                    break;

                case 0xfffa:                  /* Gateway Advertisement */
                    switch (control_hdr->subcode) {
                            case 2:
                                /*
                                 * TODO: remove this comment later on
                                 * fcoe_bridgeX_discovery(desc_buff, &adv);
                                 */

                                  /* Sending Broadcast  GW Advertisement */
                                  /*  memset(multicast_mac, 0xFF, 6);*/

                                  create_packet(1, 2, multicast_mac);
                                  vps_trace(VPS_ERROR,
                                  "--** SENT MULTICAST GW ADVERTISEMENT **--");
                                  break;
                    }
                    break;

                case 2 :                           /* FLOGI / FDISK etc */
                    switch (control_hdr->subcode) {
                            case 1 :
                                  vps_trace(VPS_ERROR,
                                  "--** IO_MODULE FLOGI REQUEST RECEIVED **--");

                                  /* Convert Flogi to Fdisc and send */
                                  create_packet_ex(fip_eth_hdr_fw, tunnel_hdr,
                                                  control_hdr, desc_buff);

                                  vps_trace(VPS_ERROR,
                                  "--** FDISC REQUEST SENT TO BRIDGE **--");
                                  break;

                            case 2 :
                                  break;
                    }
                    break;

                case 3 :          /* Keep alive & Clear Link */
                    switch (control_hdr->subcode) {
                            case 1 :   /* IO_Module Keep Alive */
                                  vps_trace(VPS_ERROR,
                                  "--** Host Keep Alive Arrived **--");
                                  fcoe_vHBA_keep_alive(desc_buff, &alive);

                                  create_packet(3, 1, &alive);
                                  vps_trace(VPS_ERROR,
                                  "--** Sent Host Keep Alive Response **--");
                                  break;

                            case 2 :   /* IO_Module Clear Virtual Link */
                                  fcoe_vHBA_deregister(desc_buff, &dereg);
                                  break;
                    }
                    break;
        }
        vps_trace(VPS_ENTRYEXIT, "Leaving decide_packet ");
        return err;
}

/*
 * send_gw_ad
 * This function triggers the process by simulating the arrival of a
 * Gateway Discovery from the bridge.
 * TODO: This is temporary and will be removed when we get the actual
 * Gateway Discovery from bridge.
 */
void
send_gw_ad()
{
        ctrl_hdr c_hdr;

        c_hdr.opcode = 0xFFFA;
        c_hdr.subcode = 2;

        decide_packet(NULL, NULL, &c_hdr, NULL);

}

/*
 * FC ELS request processing
 *
 * [IN]  *buff        : buffer read from socket.
 * [IN]  size         : Read number of bytes read from socket.
 * [OUT] *ret_pos     : Number of bytes processed.
 * [IN]  *tunnel_hdr  : Tunnel header.
 * [IN]  *fcoe_eth_hdr: Ethernet header with ether type = 8906(FCoE)
 *
 * Returns : error code
 *                   Incomplete packet
 */
vps_error
process_fc_els_req(uint8_t *buff, uint32_t size, uint32_t *ret_pos,
                mlx_tunnel_hdr *tunnel_hdr,
                eth_hdr *fcoe_eth_hdr)
{

        vps_error err = VPS_SUCCESS;
        uint32_t fc_pk_len;
        uint32_t oxid_loc = 4 * DWORD;
        req_entry_map entry;
        uint16_t vfm_gen_oxid;

        vps_trace(VPS_ENTRYEXIT, "Entering process_fc_els_req");

        /*
         * Calulate FC payload length.
         * Minus tunnel header len + FCoE header length(4 dword)
         */
        fc_pk_len = tunnel_hdr->length * DWORD - sizeof(mlx_tunnel_hdr) -
                     4*DWORD;

        tunnel_hdr->port_num =  tunnel_hdr->port_num | 0x80;

        /*Set buffer pointer at start of FC frame*/
        buff += *ret_pos;

        /*
         * memcpy(entry.mac, fcoe_eth_hdr->dhost_mac, MAC_ADDR_LEN);
         * memcpy(entry.vhba_mac, fcoe_eth_hdr->shost_mac, MAC_ADDR_LEN);
         */

        /* For other els packet map entry flag = 3 */
        /* entry.flag = 0x03;*/

        /* Map the entry to the index in the map list */
        /* vfm_gen_oxid = htons(add_entry_to_map(&entry)); */

        /* Copy the VFM_generated OXID in the FDISC payload */
        /* memcpy(buff + oxid_loc, &vfm_gen_oxid, sizeof(uint16_t));*/

        send_fc_packet(g_local_mac, g_bridge_mac, tunnel_hdr, buff, fc_pk_len);

        vps_trace(VPS_ENTRYEXIT, "Leaving process_fc_els_req");
        return err;
}

/*
 * FC ELS response processing
 *
 * [IN]  *buff        : buffer read from socket.
 * [IN]  size         : Frame size.
 * [OUT] *ret_pos     : Number of bytes processed.
 * [IN]  *tunnel_hdr  : Tunnel header.
 * [IN]  *fc_hdr      : FC header.
 * [IN]  *entry       : Map entry.
 *
 * Returns : error code
 *                   Incomplete packet
 */
vps_error
process_fc_els_res(uint8_t *buff, uint32_t size, uint32_t *ret_pos,
                mlx_tunnel_hdr *tunnel_hdr, fc_hdr *fc_header,
                req_entry_map *entry)
{
        vps_error err = VPS_SUCCESS;
        uint32_t temp_did,length, crc, fc_pk_len;
        uint8_t *fcoe_buff, *temp, *fc_pack_offset;
        uint16_t len, fcoe_plen;

        vps_trace(VPS_ENTRYEXIT, "Entering process_fc_els_res");

        /* To send to ethernet plane make E flag =0 */
        tunnel_hdr->port_num =  tunnel_hdr->port_num & 0x7F;

        /* Calculate FC packet length */
        fc_pk_len = size - *ret_pos;
        len = sizeof(fc_hdr) + fc_pk_len;

        /*
         * Allocate memory for FCoE packet.
         * FCoE header length  + FC packet length + CRC + EOF
         */
        if (g_fcoe_t11 == 1) 
        length = (OPEN_FCOE_HDR_SIZE + len + 2 *DWORD);
        else
        length = (FCOE_HDR_SIZE + len + DWORD + sizeof(uint8_t));
        

        fcoe_buff = (uint8_t*)malloc(length);
        memset(fcoe_buff, 0x00, length);

        tunnel_hdr->length = (length + sizeof(mlx_tunnel_hdr))/DWORD;
        temp = fcoe_buff;

        /* Fill up FCoE header*/
        memcpy(temp, g_fc_map, sizeof(g_fc_map));
        temp +=3;

        temp_did = htonl(fc_header->rt_ctrl_dest_id << 8);
        memcpy(temp, &temp_did, 3);
        temp += 3;

        memcpy(temp, g_bridge_mac, MAC_ADDR_LEN);
        temp += MAC_ADDR_LEN;

        /* Copy FCoE ether type = 0x8906 */
        temp[0] = 0x89;
        temp[1] = 0x06;
        temp += sizeof(uint16_t);

        /* version Length  SOF */
        fcoe_plen = htons(FCOE_ENCAPS_LEN_SOF((len + DWORD)/DWORD));

        if (g_fcoe_t11 == 1) {
                temp += sizeof(uint16_t);
                temp += OPEN_FCOE_RESERVED - sizeof(uint8_t);
                temp[0] = 0x2e;
                temp += sizeof(uint8_t);
        }
        else {
                memcpy(temp, &fcoe_plen, sizeof(uint16_t));
                temp += sizeof(uint16_t);
        }

        /* Process FC header */
        fc_header->rt_ctrl_dest_id = htonl(fc_header->rt_ctrl_dest_id);
        fc_header->src_id = htonl(fc_header->src_id);
        fc_header->type_frame_ctrl = htonl(fc_header->type_frame_ctrl);
        fc_header->seq_count = htons(fc_header->seq_count);
        fc_header->res_id = htons(fc_header->res_id);
        fc_header->ox_id = htons(fc_header->ox_id);

        /* Replace OX_ID */
        /* fc_header->ox_id = htons(entry->oxid); */

        /* Pointing to the start of FC payload */
        fc_pack_offset = temp;

        /* Copy FC header */
        memcpy(temp, fc_header, sizeof(fc_hdr));
        temp += sizeof(fc_hdr);

        /* Copy FC packet paylaod*/
        memcpy(temp, (buff + *ret_pos), fc_pk_len);
        temp += fc_pk_len;

        /*--5. Calculate CRC */
        crc = ~0;
        crc = ~ether_crc32_le(fc_pack_offset, len);
        memcpy(temp, &crc , sizeof(uint32_t));
        temp += sizeof(uint32_t);

        /* EOF */
        temp[0] = FCOE_EOF;

        /* To send the FC packet to host*/
        send_fc_packet(g_local_mac, g_bridge_mac, tunnel_hdr,
                        fcoe_buff, length);

        if ((fc_header->rt_ctrl_dest_id & 0xFF000000) == 0x22000000)
                vps_trace(VPS_INFO, "**-- ELS REQUEST SENT TO HOST--**");

        else
                vps_trace(VPS_INFO, "**-- ELS RESPONSE SENT TO HOST--**");

        free(fcoe_buff);
        vps_trace(VPS_ENTRYEXIT, "Leaving  process_fc_els_res");
        return err;
}



/**
 * Read whole packet.
 * Update database and send response.
 *
 * Return : err
 *                  Invalid packet.
 *         
 */
vps_error
process_packet()
{
        vps_error err = VPS_SUCCESS;

        vfm_task *task = NULL;

        uint8_t *desc_buff;
        uint8_t tunnel_flag = FALSE;
        uint32_t recv_size= 0;
        uint32_t ret_pos  = 0;

        eth_hdr fip_eth_hdr;
        eth_hdr fip_eth_hdr_fw;         /* Forwarded message header */
        mlx_tunnel_hdr tunnel_hdr;
        ctrl_hdr fip_ctrl_hdr;
        uint8_t en_footer[DWORD];
        uint8_t en_footer_fw[DWORD];

        vps_trace(VPS_ENTRYEXIT, "Entering read_packet");

        /*-- 0.Read task from queue - blocking call --*/
        task = (vfm_task *)get_task_from_queue();

        /* TODO : Process local VFM packet.
         * If local VFM for FCoE return 
         */
        if(task->vfm_type == LOCAL_VFM)
        {
           vps_trace(VPS_INFO, "Local VFM type ");
           err = VPS_ERROR_IGNORE;
           goto out;
        }

        recv_size = task->size;

        /*-- 1.read ethernet header --*/
        if ((err = read_eth_hdr(task->data, recv_size, &ret_pos,
                                        &fip_eth_hdr)) != VPS_SUCCESS) {
                vps_trace(VPS_INFO, "Ignoring packet: %X",
                                fip_eth_hdr.ether_type);
                vps_trace(VPS_WARNING, " Read Packet : Ignore packet");
                err = VPS_ERROR_IGNORE;
                goto out;
        }

        /*-- 2. Read tunnel header --*/
        if ((err = read_tunnel_hdr(task->data, recv_size, &ret_pos,
                                &tunnel_flag, &tunnel_hdr)) != VPS_SUCCESS) {
                vps_trace(VPS_ERROR, "read_tunnel_hdr returned: %d", err);
                goto out;
        }

        /* 3. --IF tunnel header is not present, THERE IS PROBLEM --*/
        if (tunnel_flag != TRUE) {
                err = VPS_ERROR_IGNORE;
                vps_trace(VPS_ERROR, "Not a Tunneled packet.. ignore");
                goto out;
        }

        /* --If E=1, it means its an FC packet response.-- */
        if ((tunnel_hdr.port_num & 0x80)) {
                err = process_fc_response(task->data , recv_size,
                                &ret_pos, &tunnel_hdr);
                goto out;
        }

        /* Parse Ethernet packet */
        if ((err = read_eth_hdr(task->data, recv_size, &ret_pos,
                                &fip_eth_hdr_fw)) != VPS_SUCCESS) {
                vps_trace(VPS_INFO, "Ignoring packet: %X",
                                fip_eth_hdr.ether_type);
                vps_trace(VPS_ERROR, " Read Packet : Ignore packet");
                err = VPS_ERROR_IGNORE;
                goto out;
        }
        /*
         *  If plogi or other ELS packet receive(exclude FLOGI/FDISC)
         *  then process FCoE pay load.
         */
        if (fip_eth_hdr_fw.ether_type == FCOE_ETH_TYPE) {
                err = process_fc_els_req(task->data , recv_size,
                                &ret_pos, &tunnel_hdr, &fip_eth_hdr_fw);
                goto out;
        }

        /* 4.-- read control header --*/
        if ((err = read_ctrl_hdr(task->data, recv_size, &ret_pos,
                                &fip_ctrl_hdr)) != VPS_SUCCESS) {
                vps_trace(VPS_ERROR, "Socket read error for control header");
                goto out;
        }

        /* 5.-- read discriptor list. --*/
        if ((recv_size - ret_pos) < fip_ctrl_hdr.desc_list_length * DWORD) {
                vps_trace(VPS_ERROR, "Incomplete packet");
                err = VPS_ERROR_INCOMPLETE_PK;
                goto out;
        }

        /*
         * This function gives call the respective procedure to further
         * parse the payload depending upon the type of the packet which can
         * be found from the Opcode and the Subopcode in the control header.
         */
         err = decide_packet(&fip_eth_hdr_fw, &tunnel_hdr,
                                        &fip_ctrl_hdr, task->data + ret_pos);
         if (err) {
                 vps_trace(VPS_ERROR, "Error in descriptor parsing ");
         }

         /*-- 6. Copy en footer */
         /*If tunnel header present then copy en footer of forwarded packet*/
         if (tunnel_flag == TRUE) {
                 memcpy(en_footer_fw, task->data + ret_pos,
                                 sizeof(en_footer_fw));
                 ret_pos += sizeof(en_footer_fw);
         }

         /*Copy en footer of packet*/
         memcpy(en_footer, task->data + ret_pos, sizeof(en_footer));
         ret_pos += sizeof(en_footer);


out:
        /*Free desc buffer and packet , packet data */
        free(task->data);
        free(task);
        vps_trace(VPS_ENTRYEXIT, "Leaving read_packet");
        return err;
}






/*
 * Start sniffer
 *
 *  Returns : Error code.
 */
void *
start_sniffer(void *arg)
{
        vps_error err = VPS_SUCCESS;

        uint8_t buff[MAX_PK_LEN];
        int recv_size;
        int sd;


        vps_trace(VPS_ENTRYEXIT, "Entering start_sniffer");

        if (VPS_SUCCESS !=
                        (err = open_socket(g_if_index, g_local_mac , &sd))) {
                vps_trace(VPS_ERROR, "Error in opening socket");
                return NULL;
        }

        if (VPS_SUCCESS != (err = bind_socket(&sd))) {
                vps_trace(VPS_ERROR, "Error in binding socket");
                return NULL;
        }

        while (1) {
                /*-- Read data from socket and add packet to queue--*/
                if ((recv_size = recv(sd, buff, sizeof(buff), 0)) >  0) {
                        add_task_to_queue(buff, recv_size, FIP_TASK,
                                                               REMOTE_VFM);
                }
        };

        vps_trace(VPS_ENTRYEXIT, "Leaving start_sniffer");
        return NULL;
}

#ifdef VFM_EOIB
/**
 * Read whole packet.
 * Update database and send response.
 *
 * Return : err
 *                  Invalid packet.
 */
vps_error
process_lbc_packet()
{
        uint32_t length;
        uint8_t *packet;
        uint32_t offset = 0;
        eoib_conx_vfm_adv solicit;
        ctrl_hdr  fip_ctrl_hdr;
        vps_error err = VPS_SUCCESS;

        vps_trace(VPS_ENTRYEXIT, "Entering read_packet");

        /* Gateway API call */
        packet = bx_recv(&length);
       
        /* Read control header from the packet */
        read_ctrl_hdr(packet, length, &offset, &fip_ctrl_hdr);
       
        /* Solicit message from host */
        if (fip_ctrl_hdr.opcode == 0xFFF9 && 
                        fip_ctrl_hdr.subcode == 0x01) {

                 vps_trace(VPS_INFO, "Host solicit reserved");         
                /* Process Solicit message payloaad */
                eoib_conx_discovery(packet + offset, &solicit);

                /* Send uinicast GW advertisement */
                vfm_send_uincast_gw_adv(&solicit);
                vps_trace(VPS_INFO, "Unicast GW Advertisement sent");         
        }

out:
        vps_trace(VPS_ENTRYEXIT, "Leaving read_packet");
        return err;
}

/*
 * Packet processor thread.
 */
void *
start_lbc_sniffer(void *arg)
{
        vps_trace(VPS_ENTRYEXIT, "Entering start_lbc_sniffer");

        while (1) {
                process_lbc_packet();
        };

        vps_trace(VPS_ENTRYEXIT, "Leaving start_lbc_sniffer");
}

#endif

/*
 * Packet processor thread.
 */
void *
start_processor(void *arg)
{
        vps_trace(VPS_ENTRYEXIT, "Entering start_processor");

        while (1) {
                process_packet();
        };

        vps_trace(VPS_ENTRYEXIT, "Leaving start_processor");
}
