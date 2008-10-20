/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#include<stdio.h>
#include<stdint.h>
#include<net_util.h>

extern uint8_t g_local_mac[MAC_ADDR_LEN];
extern uint8_t g_bridge_enc_mac[MAC_ADDR_LEN];
extern uint8_t g_bridge_mac[MAC_ADDR_LEN];

uint8_t g_wwnn[8];
uint8_t g_wwpn[8];

/* To set the specific Flags */
#define SET_FP 0x8000
#define SET_SP 0x4000
#define SET_A  0x4
#define SET_S  0x2
#define SET_F  0x1

#define SET_E            0x80       /* to set the tunnel hdr E flag */ 

/* Size of FLOGI/FDISC request*/
#define FLOGI_SIZE 35


/**
 * create_tlv
 * This function fills the TLV i.e the Type, length, value structure
 * 
 * data  [IN]   : value to be filled in the tlv->value.
 *
 * tlv   [INOUT]: It is a pointer to the TLV structure.
 *
 * Returns vps_error: 0 for success. The caller should free the tlv->value
 */
vps_error
create_tlv( void *data,  vp_tlv *tlv)
{
    vps_error err = VPS_SUCCESS;
    uint32_t length;

    vps_trace(VPS_ENTRYEXIT, "Entering create_tlv" );

    /* Calculate actual length*/
    length =((tlv->length * sizeof(uint32_t))
                         -  sizeof(tlv->length) - sizeof(tlv->type));

    tlv->value = malloc(length);
    memset(tlv->value, 0, length);

       /* Copy data to the tlv . NOTE : Read inline comments for help*/        
    switch(tlv->type)
    {
        case TLV_1 :
            /**TYPE :1 Priority (8 bits)
             * The lower 8 bits of tlv->value contain the priority value.
             * hence we need to move the priority bits to the right position.
             */
            memcpy(tlv->value + sizeof(uint8_t), data, sizeof(uint8_t));
            break;
        case TLV_2 :
            /**TYPE :2 MAC address (48 bits)
             * The tlv->value contains the 48 bit(6 bytes) MAC address which we
             * have directly copied and does not need to be modified.
             */
            memcpy(tlv->value , data, sizeof(uint8_t) * 6 );
            break;
        case TLV_3 :
            /**TYPE :3 FC MAP (24 bits)
             * The lower 24 bits contain the FC MAP value.
             * Hence we need to shift it 24 bits left to put it to its right
             * position into the packet.
             */
            memcpy(tlv->value + (3 * sizeof(uint8_t)), 
                                            data, 3 * sizeof(uint8_t));
            break;
        case TLV_4 :
            /**TYPE :4 NODE NAME(64 bits)
             * The lower 64 bits contain the NODE NAME value.
             * Hence we first move to the location 2 bytes ahead to point to
             * the actual NODENAME and then copy it to the output parameter.
             */
            memcpy(tlv->value + sizeof(uint16_t), data, 2 * DWORD);
            break;
        case TLV_5 :
            /**TYPE :5 FABRIC NAME(64 bits)
             * The lower 64 bits contain the FABRIC NAME value.
             * Hence we first move to the location 2 bytes ahead to point to
             * the actual FABRIC NAME and then copy it to the output parameter.
             */
            memcpy(tlv->value + sizeof(uint16_t), data, 2 * DWORD);
            break;
        case TLV_6 :
            /**TYPE :6 MAX RECEIVE SIZE (16 bits)
             * The 16 bits contain the  value of max receive size.
             */
            memcpy(tlv->value, data, sizeof(uint16_t));
            break;
        case TLV_7 : /*TODO : FLOGI Request */
            break;
        case TLV_12:
            /**TYPE :12 FKA_ADVERTISEMENT PERIOD (32 bits)
             * The lower 32 bits of the tlv->value contains the value of
             * max receive size. Hence we first move to the tlv->value
             * pointer 2 bytes ahead to point to the FKA_ADV_PERIOD.
             * And then assign it to the output parameter.
             */
            memcpy(tlv->value + sizeof(uint16_t), data, DWORD);
            break;

        case MLX_TLV_1:
            /*MELLANOX SPECIFIC TLV for Associating VFM to BridgeX : Tab : 9 
            * Len : 6, GW_ID , Database ID (DB_ID), Cluster MAC, Gateway MAC.
            */
            memcpy(tlv->value + sizeof(uint16_t), data, 5 * DWORD);
            break;

    }
    
    
    vps_trace(VPS_ENTRYEXIT, "Leaving create_tlv");
    
}


/** fcoe_vHBA_advertisement
  * This function first creates the TLV's from the data structures
  * using the create_tlv function and fills the message descriptor
  * by adding the TLV's to it.
  * Sent from VFM to host (Also acts as Keep Alive from VFM to host).
  *
  * adv       [IN]   - Pointer to  the vHBA Advertisment structure.
  * mesg_desc [OUT]  - It contains the pointer to the message descriptor 
  *                    buffer i.e. the payload.
  *
  *  Returns vps_error: 0 for success.
  *
  */
vps_error
fcoe_vHBA_advertisement(fcoe_vHBA_adv *adv, uint8_t *msg_desc)
{

    vps_error err = VPS_SUCCESS;
    uint8_t *offset = msg_desc;
    vp_tlv tlv;


    vps_trace(VPS_ENTRYEXIT, "Entering fcoe_vHBA_advertisement");
    
    /* TODO : Write code for creating the FIP packet by reading the values 
     *        from the database and filling up the packet structure to send 
     *        it from the VFM to the host. 
     *
     */         


    /* TLV 1 : Priority , len = 1 */
    tlv.type   = 1;
    tlv.length = 1;
    create_tlv(&adv->priority, &tlv);
    memcpy(offset, &tlv, 2);
    offset += 2;
    memcpy(offset, tlv.value, tlv.length * DWORD - 2);
    offset += tlv.length * DWORD - 2;
    free(tlv.value);

    /* TLV 2 : FCF_GW_MAC , len = 2 */
    tlv.type   = 2;
    tlv.length = 2;
    create_tlv(adv->fcf_gw_mac, &tlv);
    memcpy(offset, &tlv, 2);
    offset += 2;
    memcpy(offset, tlv.value, tlv.length * DWORD - 2);
    offset += tlv.length * DWORD - 2;
    free(tlv.value);

    /* TLV 3 : FC_MAP , len = 2 */
    tlv.type   = 3;
    tlv.length = 2;
    create_tlv(&adv->fc_map, &tlv);
    memcpy(offset, &tlv, 2);
    offset += 2;
    memcpy(offset, tlv.value, tlv.length * DWORD - 2);
    offset += tlv.length * DWORD - 2;
    free(tlv.value);
    
    /* TLV 4 : SWITCH NAME , len = 3 */
    tlv.type   = 4;
    tlv.length = 3;
    create_tlv(adv->switch_name, &tlv);
    memcpy(offset, &tlv, 2);
    offset += 2;
    memcpy(offset, tlv.value, tlv.length * DWORD - 2);
    offset += tlv.length * DWORD - 2;
    free(tlv.value);
    
    /* TLV 5 : FABRIC NAME , len = 3 */
    tlv.type   = 5;
    tlv.length = 3;
    create_tlv(adv->fabric_name, &tlv);
    memcpy(offset, &tlv, 2);
    offset += 2;
    memcpy(offset, tlv.value, tlv.length * DWORD - 2);
    offset += tlv.length * DWORD - 2;
    free(tlv.value);
    

    /* TLV 12: FKA_ADVERTISEMENT PERIOD , len = 2 */
    tlv.type   = 12;
    tlv.length = 2;
    create_tlv(&adv->fka_adv_period, &tlv);
    memcpy(offset, &tlv, 2);
    offset += 2;
    memcpy(offset, tlv.value, tlv.length * DWORD - 2);
    offset += tlv.length * DWORD - 2;
    free(tlv.value);
    
    vps_trace(VPS_ENTRYEXIT, "Leaving fcoe_vHBA_advertisement");
    return err;
}  



/** fcoe_bridge_adv_res
 * This function first create the TLV from the database using the
 * create_tlv function. It then fills the message descriptor by 
 * by adding the TLV's to it.
 *
 * mesg_desc [OUT]: It contains the copy of the packet received
 *                  from the network.
 * adv_res   [IN] : Pointer to  the vHBA Advertisment structure.
 *
 *  Returns vps_error: 0 for success.
 *
 */
vps_error
fcoe_bridge_adv_res(fcoe_vfm_bridge_adv_res *adv_res, 
                uint8_t *msg_desc)
{

    uint8_t *offset = msg_desc;
    vp_tlv *tlv;
    vps_error err = VPS_SUCCESS;
    vps_trace(VPS_ENTRYEXIT, "Entering fcoe_bridge_adv_res");

    /* TODO : Write code for creating the FIP packet by reading the values
     *        from the database and filling up the packet structure to send
     *        it from the VFM to the BridgeX.
     *
     *        Type = 2 , Type = 4 , Type = 6, Type = MLNX_ADV_RES
     */


    /* TLV 2 : MAC ADDRESS, len = 2 */
    tlv->type   = 2;
    tlv->length = 2;
    create_tlv(&adv_res->vfm_mac, tlv);
    memcpy(offset, tlv, tlv->length);
    offset += tlv->length;

    
    /* TLV 4 : NODE NAME, len = 3 */
    tlv->type   = 4;
    tlv->length = 3;
    create_tlv(&adv_res->node_name, tlv);
    memcpy(offset, tlv, tlv->length);
    offset += tlv->length;
    
    
    /* TLV 6 : MAX RECV , len = 1 */
    tlv->type   = 6;
    tlv->length = 1;
    create_tlv(&adv_res->max_recv, tlv);
    memcpy(offset, tlv, tlv->length);
    offset += tlv->length;
    

    /* MLX_TLV_1 : Priority , len = 2 */
    tlv->type   = MLX_TLV_1;
    tlv->length = 6;
    create_tlv(&adv_res->res, tlv);
    memcpy(offset, tlv, tlv->length);
    offset += tlv->length;
    
    vps_trace(VPS_ENTRYEXIT, "Leaving fcoe_bridge_adv_res");
    return err;
}  


/** fcoe_vHBA_deregister From VFM to ConnectX.
 * TODO : Write code for creating the FIP packet by reading the values
 *        from the database and filling up the packet structure to send
 *        it from the VFM to the ConnectX.
 *
 */
vps_error
fcoe_vfm_vHBA_deregister(fcoe_vHBA_dereg *dereg, uint8_t *msg_desc)
{
}

vps_error
prepare_fcoe_vHBA_advertisement(ctrl_hdr *control_hdr, uint8_t **msg_desc)
{
    fcoe_vHBA_adv gw_adv;
    vps_error err = VPS_SUCCESS;
    vps_trace(VPS_ENTRYEXIT, "Entering prepare_fcoe_vHBA_advertisement");


    /* TODO :Hard Coding the Values in the Gateway Advertisement structure
     * for now. These values will be filled up from the database by
     * calling a routine to populate the structure. By Gautam.
     * 
     */
    gw_adv.priority = 45;                      /* Priority */
    /* FCF gateway mac address */
    memset(gw_adv.fcf_gw_mac, 0 , MAC_ADDR_LEN);
    gw_adv.fc_map = 123456;           /* FC-MAP TODO: 
                                         Where does it come from */
    memset(gw_adv.switch_name, 0 , NAME_LEN);  /* Switch Name */
    memset(gw_adv.fabric_name, 0 , NAME_LEN);  /* Fabric name: */  
    gw_adv.fka_adv_period = 50000; 
  
    /* TODO: bridge_enc_mac is used ONLY for te demo to work around SW GW 
     * bug. Please change bridge_enc_mac to bridge_mac later */
    memcpy(gw_adv.fcf_gw_mac, g_bridge_enc_mac, MAC_ADDR_LEN);
    gw_adv.fc_map = htonl(123456);             /* FC-MAP TODO: 
                                                  Where does it come from */
    memset(gw_adv.switch_name, 0 , NAME_LEN);  /* Switch Name */
    memset(gw_adv.fabric_name, 0 , NAME_LEN);  /* Fabric name: */  
    gw_adv.fka_adv_period = htonl(50000); 

    /* Allocate memory for the descriptor list or Payload */ 
    *msg_desc = (uint8_t *)malloc(control_hdr->desc_list_length * DWORD);
    memset(*msg_desc, 0, control_hdr->desc_list_length * DWORD);
   
    fcoe_vHBA_advertisement(&gw_adv, *msg_desc);
    
    vps_trace(VPS_ENTRYEXIT, "Leaving prepare_fcoe_vHBA_advertisement");
    return err;

}

/* To prepare the FC header */

vps_error
prepare_fc_hdr(fc_hdr *fc_header)
{

    vps_error err = VPS_SUCCESS;
    vps_trace(VPS_ENTRYEXIT, "Entering prepare_fc_hdr");


    fc_header->rt_ctrl_dest_id = htonl(0x22FFFFFE);
    fc_header->src_id = 0x0;
    fc_header->type_frame_ctrl = htonl(0x01290000);
    fc_header->seq_id = 0x0;
    fc_header->data_field_ctrl = 0x0;
    fc_header->seq_count = 0x0000;
    fc_header->org_id = htons(0x0001);
    fc_header->res_id = htons(0xffff);
    fc_header->parameter = 0x00000000;
    
    
    vps_trace(VPS_ENTRYEXIT, "Leaving prepare_fc_hdr");
    return err;
}







/** prepare_fdisc_request 
  * This function takes the payload of the flogi request from the Host
  * and then converts it into a FDISC request by changing the LS command code
  * It also creates the FC header and appends the FDISC payload to it and
  * forwards it to the Software GW( i.e. BridgeX).
  * 
  * *data [IN] : Contains the TLV of FLOGI header and FLOGI payload. 
  *
  */
vps_error 
prepare_fdisc_request(void *data)
{

    uint8_t *payload;
    mlx_tunnel_hdr tun_hdr;
    vps_error err = VPS_SUCCESS;

    vps_trace(VPS_ENTRYEXIT, "Entering prepare_fdisc_request");


    /* Prepare Default tunnel */
    memset(&tun_hdr, 0, sizeof(tun_hdr));

    tun_hdr.port_num |= SET_E; 
    /* T10 Vender ID = "Mellanox" */
    strcpy(tun_hdr.t10_vender_id, "MELLANOX");

    /* prepare FC Header and Payload */  
  
    payload = (uint8_t *)malloc(FLOGI_SIZE * DWORD );
    memset(payload, 0, FLOGI_SIZE * DWORD);

    data += DWORD;              /* Move data 1 DWORD ahead to FC Header */
    
    /* Copy Header and payload */
    memcpy(payload, data, FLOGI_SIZE * DWORD);

    /* Convert FLOGI to FDISC by changing the LS command code to 0x51*/
    payload[6 * DWORD] = 0x51;

   
    send_fc_packet(g_local_mac, g_bridge_mac, &tun_hdr, payload, FLOGI_SIZE * DWORD);

    vps_trace(VPS_ENTRYEXIT, "Leaving prepare_fdisc_request");
    return err;


}

vps_error 
create_vfm_flogi()
{

    uint8_t *msg_desc, *temp;
    mlx_tunnel_hdr tun_hdr;
    fc_hdr fc_header;
    vps_error err = VPS_SUCCESS;

    vps_trace(VPS_ENTRYEXIT, "Entering prepare_fdisc_request");

    /* Prepare Default tunnel */
    memset(&tun_hdr, 0, sizeof(tun_hdr));

    tun_hdr.port_num |= SET_E;
    /* T10 Vender ID = "Mellanox" */
    strcpy(tun_hdr.t10_vender_id, "MELLANOX");


    /* Create VFM FLOGI Request */
    prepare_fc_hdr(&fc_header);
    msg_desc = (uint8_t *)malloc(FLOGI_SIZE * DWORD);
    memset(msg_desc, 0 , FLOGI_SIZE * DWORD);

    temp = msg_desc;
    /* Copy the FC header */
    memcpy(temp, &fc_header, sizeof(fc_header));
    temp += sizeof(fc_header);

    /* Copy/Make the payload */
    temp[0] = 0x04;     /* LS Command code */
    temp[4] = 0x20;     /* Highest FC PH version*/
    temp[5] = 0x20;     /* Lowest FC PH version*/
    temp[7] = 0x0a;     /* Bufer to buffer credit LSB */
    temp[10] = 0x05;    /* Data Receive size */    
    temp[11] = 0xac;    /* Data Receive size */    

    memcpy(temp+20, g_wwpn, sizeof(g_wwpn));
    memcpy(temp+28, g_wwnn, sizeof(g_wwnn));
/*
    temp[20] = 0x20;    
    temp[22] = 0x00;
    temp[23] = 0x10;
    temp[24] = 0x86;
    temp[25] = 0x02;
    temp[26] = 0x09;
    temp[27] = 0x22;

    temp[28] = 0x10;   
    temp[30] = 0x00;
    temp[31] = 0x10;
    temp[32] = 0x86;
    temp[33] = 0x02;
    temp[34] = 0x09;
    temp[35] = 0x22;
*/
    temp[68] = 0x88;   /* Class 3 service options */

    /* To send the FC packet on the FC Plane */
    send_fc_packet(g_local_mac, g_bridge_mac, &tun_hdr, msg_desc, FLOGI_SIZE * DWORD);

    vps_trace(VPS_ENTRYEXIT, "Leaving prepare_fdisc_request");
    return err;
}

/** create_packet
  *
  * Working: Depending upon the type of packet that is to 
  * be created as specified by the "VFM CORE Logic" i.e which response 
  * to send for a particular request, the data structure required 
  * is filled up from the database. The pointer to structure is passed to this
  * function which is collected in 'data' and the opcode and Subopcode.
  * Now depending upon the Opcode and Subopcode the respective function is
  * called which then creates the 'TLV' or 'descriptor'structure for the packet.
  *
  *
  * This function will decides what type of the packet is to be created
  * depending upon the opcode and the subopcode.
  * And then call the create_tlv function to create the TLVs from the 
  * filled data structure sent from the database for the packet.
  *
  * op         [IN] : opcode in the control header of packet.
  * subop      [IN] : Subopcode
  * data       [IN] : Data structure that is filled from the database.
  *
  * Returns vps_error.
  */
vps_error
create_packet(uint16_t op,
              uint8_t subop,
              void *data)
{
    
    vps_error err = VPS_SUCCESS;
    uint8_t tunnel_flag;
    uint32_t desc_len;
    uint8_t *msg_desc;
    uint8_t conx_mac[MAC_ADDR_LEN];
    uint8_t broad_mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} ;
    ctrl_hdr control_hdr;
    mlx_tunnel_hdr tun_hdr;

    vps_trace(VPS_ENTRYEXIT, "Entering create_packet");

    /* Prepare Default tunnel */
    memset(&tun_hdr, 0, sizeof(tun_hdr));
    tunnel_flag = 1;           
    /* T10 Vender ID = "Mellanox" */
    strcpy(tun_hdr.t10_vender_id, "MELLANOX");
    /* TODO: Fill the other tunnel default values */


    /* Prepare Default Control header */
    memset(&control_hdr, 0, sizeof(control_hdr));

    /* VFM BridgeX Advertisement Respone i.e.Gateway Advertisement to Host */
    if(op == 1 && subop == 2)
    {
        control_hdr.opcode  = 1;
        control_hdr.subcode = 2;
        control_hdr.reserved = 0;
        control_hdr.desc_list_length = 13;
        control_hdr.flags |= SET_FP;
        control_hdr.flags |= SET_SP;
        control_hdr.flags |= SET_F;
        
        /* Copy Host MAC address which is sent in connectX Discovery*/
        memcpy(conx_mac, data, 6 * sizeof(uint8_t));

        if(!(is_multicast_ether_addr(conx_mac)))
        {      
            /* If and only if this is a unicast message to a conx
             * then set A = 1 & S = 1.
             */
            control_hdr.flags |= SET_A;
            control_hdr.flags |= SET_S;
        }

        prepare_fcoe_vHBA_advertisement(&control_hdr, &msg_desc);
        
        /* Call to the function which will take this payload and then send the
         * packet by putting the ethernet and tunnel header.
         */
        send_packet(tunnel_flag, g_local_mac, conx_mac, g_bridge_mac,
                                                        &tun_hdr, &control_hdr, msg_desc);

    } 

    /* Associating a VFM with a bridge i.e. VFM response to bridge adv */
    else if(op == 121 && subop == 1)
    {

        /*TODO Set Flags*/
        fcoe_bridge_adv_res(data, msg_desc);
    }

    /* Clear Host Link from VFM to Host */
    else if(op == 3 && subop == 2)
    {
        fcoe_vfm_vHBA_deregister(data, msg_desc);
        /*TODO Complete the whole function*/
    }
    
    else if(op == 2 && subop == 1)
    {
        prepare_fdisc_request(data);

    }
        
    /** Call to the function which will take this payload and then send the 
      * packet by putting the ethernet and tunnel header.
      */

    vps_trace(VPS_ENTRYEXIT, "Leaving create_packet ");
    return err;

}

