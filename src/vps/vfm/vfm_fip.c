/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#include<stdio.h>
#include<stdint.h>
#include<common.h>
#include<vfm_fip.h>

/** get_tlv
  * This function fills the TLV i.e the Type, length, value structure
  * 
  * mesg_desc (IN)  - It contains the copy of the packet received 
  *                    from the network.
  * tlv       (OUT) - It is a pointer to the TLV structure.
  *
  * Returns vps_error: 0 for success. The caller should free the tlv->value
  */

vps_error
get_tlv(uint8_t *msg_desc, vp_tlv *tlv)
{
    vps_error err = VPS_SUCCESS;
    uint32_t length;
    uint8_t *ptr = msg_desc;

    vps_trace(VPS_ENTRYEXIT, "Entering get_tlv" );

    /* Copy the type into tlv */
    memcpy(&(tlv->type), ptr, sizeof(tlv->type) );
    ptr += sizeof(tlv->type);

    /* Copy the length in dwords into the tlv */
    memcpy(&(tlv->length), ptr, sizeof(tlv->length));
    ptr += sizeof(tlv->length);

    
    /* The data in the tlv starts after the type and len values. Hence we need 
     * to allocate 2 bytes lesser.
     */
    length =((tlv->length * DWORD) - sizeof(tlv->length) - sizeof(tlv->type));

    /* Copy the data into the tlv */  
    /* TODO If len =< 0 , Then take some action to allocate memory*/
    tlv->value = malloc(length);
    
    memcpy(tlv->value, ptr, length);

    vps_trace(VPS_INFO, "TLV: type: %d, length: %d", tlv->type, tlv->length);
    vps_trace(VPS_ENTRYEXIT, "Leaving get_tlv");
    return err;
}



/** get_tlv_value
  * This function typecast/copies the values from the TLV into the 
  * predefined data structures depeding upon the TYPE specified.
  *
  * tlv    (IN)   - It is a pointer to the TLV data structure.
  * data   (OUT)  _ It will hold the actual value which is to be filled 
  *                 in the data structures.
  *
  * Returns vps_error : 0 for success.
  */

vps_error
get_tlv_value(vp_tlv *tlv, void *data)
{
    vps_error err = VPS_SUCCESS;
    vps_trace(VPS_ENTRYEXIT, "Entering get_tlv_value");
    uint32_t tmp_dword = 0;
    uint16_t tmp_word = 0;
    uint32_t i = 0;

    /* Special decls for TLV MLX_TLV_2 */
    mlx_bridgeX *bridgeX_ptr;
    mlx_gateway *gateway_ptr;
    void *tmp_ptr;

    uint32_t length;
    length = (tlv->length * DWORD) -
            sizeof(tlv->length) - sizeof(tlv->type);

    switch(tlv->type)
    {
        /* TODO: Change the case numbers to MACROS */
        case TLV_1 :
            /**TYPE :1 Priority (8 bits)
             * The lower 8 bits of tlv->value contain the priority value.
             * hence we need to remove the unwanted bits.
             */
            memcpy(data, tlv->value + sizeof(uint8_t), sizeof(uint8_t));
            break;

        case TLV_2 :
            /**TYPE :2 MAC address (48 bits)
             * The tlv->value contains the 48 bit(6 bytes) MAC address which we 
             * can directly copy into the output parameter.
             */
            memcpy(data, tlv->value, sizeof(uint8_t) * 6);
            break;

        case TLV_3 :
            /**TYPE :3 FC MAP (24 bits)
             * The lower 24 bits contain the FC MAP value.
             * Hence we first need to convert it from network byte order 
             * to Host byte order. And then assign it to the output parameter. 
             */
            tmp_dword = (ntohl(*((uint32_t*)tlv->value)));
            memcpy(data, &tmp_dword, sizeof(uint32_t));
            break;

        case TLV_4 :   
            /**TYPE :4 NODE NAME(64 bits)
             * The lower 64 bits contain the NODE NAME value.
             * Hence we first move to the location 2 bytes ahead to point to
             * the actual NODENAME and then copy it to the output parameter. 
             */
            memcpy(data, tlv->value + sizeof(uint16_t), sizeof(uint8_t) * 8);
            break;

        case TLV_5 :
            /**TYPE :5 FABRIC NAME(64 bits)
             * The lower 64 bits contain the FABRIC NAME value.
             * Hence we first move to the location 2 bytes ahead to point to
             * the actual FABRIC NAME and then copy it to the output parameter. 
             */
            memcpy(data, tlv->value + sizeof(uint16_t), sizeof(uint8_t) * 8);
            break;
            
        case TLV_6 : 
            /**TYPE :6 MAX RECEIVE SIZE (16 bits)
              * The 16 bits contain the  value of max receive size.
              * Hence we first need to convert it from network byte order 
              * to Host byte order. And then assign it to the output parameter. 
              */
            tmp_word = ntohs(*((uint16_t*)tlv->value));
            memcpy(data, &tmp_word, sizeof(uint32_t));
            break;
        case TLV_7 :  /* FLOGI */
        case TLV_8 :
        case TLV_9 :
        case TLV_10:
            break;
            
        case TLV_11:
            /** TYPE : 11 vHBA structure: It contains FCF Gateway MAC (48 bits),
              * vHBA port ID(24 bit), and vHBA Name.
              * The vHBA port ID is in network byte order, which needs to be 
              * converted to host byte order.
              */
            memcpy(data, tlv->value, sizeof(vHBA));
            ((vHBA*)data)->port_id = ntohl(((vHBA*)data)->port_id);
            break;
            
        case TLV_12:
            /**TYPE :12 FKA_ADVERTISEMENT PERIOD (32 bits)
             * The lower 32 bits of the tlv->value contains the value of
             * max receive size. Hence we first move to the tlv->value 
             * pointer 2 bytes ahead to point to the FKA_ADV_PERIOD. 
             * And also the value is in network byte order, hence we also 
             * need to convert it from network byte order to Host byte order. 
             * And then assign it to the output parameter. 
             */
            tmp_dword = (ntohl(*((uint32_t*)(tlv->value + sizeof(uint16_t)))));
            memcpy(data, &tmp_dword, sizeof(uint32_t));
            break;
    
        case MLX_TLV_2 :
            /**TYPE :MLX SPECIFIC MSG DESCRIPTOR FOR Bridge Info
			  * Len : 6, GW_num, Database ID (DB_ID), 
			  * MAC of the last associated VFM, Gateway Info.
			  */
            memcpy(data, tlv->value + sizeof(uint16_t), sizeof(mlx_bridgeX));

            /* Copy the bridge properties */
            bridgeX_ptr = (mlx_bridgeX*)data;

            bridgeX_ptr->gw_num = ntohs(bridgeX_ptr->gw_num);
			bridgeX_ptr->gw_num &= 0xf000;
			bridgeX_ptr->gw_num >>= 12;
           

            bridgeX_ptr->db_id = ntohs(bridgeX_ptr->db_id);

            /* Allocate the gateway memory */
            bridgeX_ptr->gw_arr = (mlx_gateway *)malloc(bridgeX_ptr->gw_num *
                                                       sizeof(mlx_gateway));
            gateway_ptr = bridgeX_ptr->gw_arr;
			
            /* Set the gateway pointer from the tlv value */
            tmp_ptr = tlv->value + sizeof(uint16_t) + (3 * sizeof(DWORD));

            /* Iterate over the tlv-data and populate the gateway information */
            for(i = 0; i < bridgeX_ptr->gw_num; i++)
            {
                memcpy(gateway_ptr + i, tmp_ptr, sizeof(mlx_gateway));
                gateway_ptr->gw_id = ntohs(gateway_ptr->gw_id);
				gateway_ptr->ports_stats = ntohl(gateway_ptr->ports_stats);

                /* Go to the next gateway data */
                tmp_ptr += sizeof(mlx_gateway);
            }
            break;	
    
    }
    /* TODO: For now we are freeing the value here. Ideally, we should pass an
     * OWNERSHIP flag to this method which determines whether the (IN)
     * parameter should be manipulated or not
     */
    free(tlv->value);

    vps_trace(VPS_ENTRYEXIT, "Leaving get_tlv_value");

    return err;
}    

/** fcoe_conx_discovery
  * This function first fills the TLV from the buffer using the 
  * get_tlv function. It then fills the predefined data structures 
  * for the ConnectX Solicitation from the TLV.
  * Sent from ConnectX(unicast) to VFM through BridgeX.
  *
  * mesg_desc (IN)    - It contains the copy of the packet received
  *                     from the network.
  * solicit   (OUT)   - Pointer to  the connectX_solication structure.
  *
  *  Returns vps_error: 0 for success.
  *
  */

vps_error
fcoe_conx_discovery(uint8_t *msg_desc, fcoe_conx_vfm_adv *solicit)
{
    vps_error err = VPS_SUCCESS;
    vps_trace(VPS_ENTRYEXIT, "Entering connectX_solication");
    
    vp_tlv data;
    uint8_t *ptr = msg_desc;
    
    /* Type = 2 */
    get_tlv(ptr, &data);
    get_tlv_value(&data, &(solicit->host_mac));
    ptr += data.length * DWORD;

    /* Type = 4 */
    get_tlv(ptr, &data);
    get_tlv_value(&data, &(solicit->node_name));
    ptr += (data.length * DWORD);
    
    /* Type = 6 */
    get_tlv(ptr, &data);
    get_tlv_value(&data, &(solicit->max_recv));
    ptr += (data.length * DWORD);
    
    vps_trace(VPS_ENTRYEXIT, "Leaving connectX_solication");
    return err;
}
 
/** fcoe_bridgeX_discovery
  * This function first fills the TLV from the buffer using the 
  * get_tlv function. It then fills the predefined data structures 
  * for the BridgeX Solicitation from the TLV. 
  * Sent from BridgeX(multicast) to VFM.
  *
  * mesg_desc (IN)    - It contains the copy of the packet received
  *                     from the network.
  * solicit   (OUT)   - Pointer to  the bridgeX_solication structure.
  *
  *  Returns vps_error: 0 for success.
  *
  */

vps_error
fcoe_bridgeX_discovery(uint8_t *msg_desc, fcoe_bridge_vfm_adv *adv)
{
    vps_error err = VPS_SUCCESS;
    vps_trace(VPS_ENTRYEXIT, "Entering fcoe_BridgeX_discovery");
    
    uint32_t length = 0;

	vp_tlv data;
    uint8_t *ptr = msg_desc;
    
    /* Type = 2 */
    get_tlv(ptr, &data);
    get_tlv_value(&data, &(adv->bridge_mac));
    ptr += (data.length * sizeof(uint32_t));
    
    /* Type = 4 */
    get_tlv(ptr, &data);
    get_tlv_value(&data, &(adv->node_name));
    ptr += (data.length * sizeof(uint32_t));

    /* Type = 6 */
    get_tlv(ptr, &data);
    get_tlv_value(&data, &(adv->max_recv));
    ptr += (data.length *sizeof(uint32_t));

   /* Type = MLNX ADV */
    get_tlv(ptr, &data);
    get_tlv_value(&data, &(adv->info));

    vps_trace(VPS_ENTRYEXIT, "Leaving fcoe_BridgeX_discovery");
    return err;
}    


/** fcoe_vHBA_keep_alive
 * This function first fills the TLV from the buffer using the
 * get_tlv function. It then fills the predefined data structures
 * for the vHBA alive from the TLV.
 *
 * Sent from ConnectX(unicast) to VFM.
 *
 * mesg_desc (IN)    - It contains the copy of the packet received
 *                     from the network.
 * solicit   (OUT)   - Pointer to  the vHBA Alive structure.
 *
 *  Returns vps_error: 0 for success.
 *
 */

vps_error
fcoe_vHBA_keep_alive(uint8_t *msg_desc, fcoe_vHBA_alive *alive)
{
    
    vps_error err = VPS_SUCCESS;
    vps_trace(VPS_ENTRYEXIT, "Entering fcoe_vHBA_keep_alive");
    
    vp_tlv data;
    uint8_t *ptr = msg_desc;
    
    /* Type = 2 */
    get_tlv(ptr, &data);
    get_tlv_value(&data, &alive->host_mac);
    ptr += (data.length * sizeof(uint32_t));
    
    /* Type = 4 /
    get_tlv(ptr, &data);
    get_tlv_value(&data, &alive->node_name);
    ptr += (data.length * sizeof(uint32_t));
    
    /* Type = 6 /
    get_tlv(ptr, &data);
    get_tlv_value(&data, &alive->max_recv);
    ptr += (data.length * sizeof(uint32_t));
    */ 
    /* Type = 11*/
    get_tlv(ptr, &data);
    get_tlv_value(&data, &alive->hba);
    
    vps_trace(VPS_ENTRYEXIT, "Leaving fcoe_vHBA_keep_alive");
    return err;
}


/** fcoe_vHBA_deregister
 * This function first fills the TLV from the buffer using the
 * get_tlv function. It then fills the predefined data structures
 * for the vHBA dereg from the TLV.
 * Sent from ConnectX (unicast) to VFM.
 * mesg_desc (IN)    - It contains the copy of the packet received
 *                     from the network.
 * solicit   (OUT)   - Pointer to  the vHBA Dereg structure.
 *
 *  Returns vps_error: 0 for success.
 *
 */
vps_error
fcoe_vHBA_deregister(uint8_t *msg_desc, fcoe_vHBA_dereg *dereg)
{
    
    vps_error err = VPS_SUCCESS;
    vps_trace(VPS_ENTRYEXIT, "Entering fcoe_vHBA_deregister");
    
    vp_tlv data;
    uint8_t *ptr = msg_desc;
    
    /* Type = 2 */
    get_tlv(ptr, &data);
    get_tlv_value(&data, &dereg->fcf_gw_mac);
    ptr += (data.length * sizeof(uint32_t));
    
    /* Type = 4 */
    get_tlv(ptr, &data);
    get_tlv_value(&data, &dereg->switch_name);
    ptr += (data.length * sizeof(uint32_t));
    
    /* Type = 11*/
    get_tlv(ptr, &data);
    get_tlv_value(&data, &dereg->hba);
    
    vps_trace(VPS_ENTRYEXIT, "Leaving fcoe_vHBA_deregister");
    return err;

}



#ifdef TEST
vps_error
display(fcoe_conx_vfm_adv *solicit)
{
    vps_error err = VPS_SUCCESS;
    int i =0;

    printf("\n\nCONX DISCOVERY PACKET");
    for(i=0; i< 5; i++)
       printf(" :%x ", solicit->host_mac[i]);
    for(i=0; i< 7; i++)
    printf("%x", solicit->node_name[i]);
    printf("\n%x", solicit->max_recv);
    
    printf("\nMAC ADDRESS:");
    for(i=0; i< 6; i++)
       printf(":%x ", solicit->host_mac[i]);

    printf("\nNODE NAME");
    for(i=0; i< 8; i++)
    printf("%x", solicit->node_name[i]);
    printf("\nMAX RECV%x", solicit->max_recv);
    return err;
}

vps_error
display1(fcoe_bridge_vfm_adv *solicit)
{
    vps_error err = VPS_SUCCESS;
    printf("\n\nBRIDGEX DISCOVERY PACKET");

    int i =0;
    for(i=0; i< 5; i++)
       printf(": %x ", solicit->bridge_mac[i]);
    printf("\nMAC ADDRESS:");
    for(i=0; i< 6; i++)
       printf(": %x ", solicit->bridge_mac[i]);
    for(i=0; i< 7;i++)
    printf(" %x", solicit->node_name[i]);
    printf("\n%x", solicit->max_recv);
    printf("\n%x", solicit->info);
    printf("\nNODE NAME:");  
    for(i=0; i< 8;i++)
    printf(" %x", solicit->node_name[i]);
    printf("\nMAX RECV %x", solicit->max_recv);

    printf("\n\nGATEWAY INFO:");
    printf("\n GW_NUMBER :%x", solicit->info.gw_num);
    printf("\n DB_ID :%x", solicit->info.db_id);
    printf("\nLAST BC MAC ADDRESS:");
    for(i=0; i< 6; i++)
    printf(":%x", solicit->info.last_bc_mac[i]);
  
    printf("\n EXT PORTS:%x", solicit->info.gw_arr->ext_ports);
    printf("\n FLAGS : %x", solicit->info.gw_arr->flags);
    printf("\n GW_ID: %x", solicit->info.gw_arr->gw_id);
    printf("\n PORT STATS%x", solicit->info.gw_arr->ports_stats);
    return err;

}

vps_error
display_alive(fcoe_vHBA_alive *alive)
{
    vps_error err = VPS_SUCCESS;
    int i =0;

    printf("\n\nCONX KEEP ALIVE");

    printf("\nMAC ADDRESS:");
    for(i=0; i< 6; i++)
       printf(":%x ", alive->host_mac[i]);

    printf("\nNODE NAME");
    for(i=0; i< 8; i++)
    printf("%x", alive->node_name[i]);
    printf("\nMAX RECV%x", alive->max_recv);
	
	
    printf("\nvHBA PORT INFO :");
    printf("\nFCF MAC ADDRESS:");
    for(i=0; i< 6; i++)
       printf(":%x ", alive->hba.fcf_mac[i]);

    printf("\nPORT ID%x", alive->hba.port_id);
    printf("\nPORT NAME");
    for(i=0; i< 8; i++)
    printf("%x", alive->hba.name[i]);
	
    return err;
}


int
packet_process()
{
    int a;
    fcoe_bridge_vfm_adv adv;
    fcoe_conx_vfm_adv solicit;
	fcoe_vHBA_alive alive;
    uint8_t msg[] = { 0x02, 0x02, 0xaa, 0xcc, 0xdd, 0xAA, 0xBB, 0xCC, 0x04, 
				      0x03, 0xf0, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xf0, 0xff,
				      0xff, 0xf0, 0x06, 0x01, 0x03, 0x03, 0x0e, 0x06, 0x00,
				      0x00, 0x10, 0x00, 0x99, 0x98, 0x00, 0x00, 0x12, 0x34,
				      0xa0, 0xb0, 0xc0, 0xd0, 0x01, 0x7f, 0x0c, 0xff, 0x00, 
				      0x00, 0x00, 0x03 };
    
	a = fcoe_conx_discovery(msg, &solicit);
    a = display(&solicit);
    a = fcoe_bridgeX_discovery(msg, &adv);
    a = display1(&adv);
    a = fcoe_vHBA_keep_alive(msg, &alive);
    a = display_alive(&alive);
    return 0;
}

#endif /*TEST*/



