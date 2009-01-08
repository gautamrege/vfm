/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#include<vfm_iboe.h>


uint8_t mellanox_str[NAME_LEN] = {'v','p','s','y','s','t','e','m'};
/*
 * eoib_conx_discovery
 * This function first fills the TLV from the buffer using the
 * get_tlv function. It then fills the predefined data structures
 * for the IO_Module Solicitation from the TLV.
 * Sent from IO_Module(unicast) to VFM through BridgeX.
 *
 * mesg_desc (IN)  - It contains the copy of the packet received
 *                   from the network.
 * solicit   (OUT) - Pointer to  the connectX_solication structure.
 *
 * Returns vps_error: 0 for success.
 *
 */
vps_error
eoib_conx_discovery(uint8_t *msg_desc, eoib_conx_vfm_adv *solicit)
{
        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering eoib_conx_discovery");

        vp_tlv data;
        uint8_t *ptr = msg_desc;

        /* Type = 13 */
        get_tlv(ptr, &data);
        get_tlv_value(&data, &(solicit->vendor_id));
        ptr += data.length * DWORD;

        /* Type = MLNX_240 */
        get_tlv(ptr, &data);
        get_tlv_value(&data, &(solicit->mlnx_240));

        solicit->mlnx_240.qpn = htonl(solicit->mlnx_240.qpn);
        solicit->mlnx_240.sl_gw_port_id = 
                             htons(solicit->mlnx_240.sl_gw_port_id);
        solicit->mlnx_240.lid = htons( solicit->mlnx_240.lid);
        ptr += (data.length * DWORD);

        vps_trace(VPS_ENTRYEXIT, "Leaving eoib_conx_discovery");
        return err;
}


/*
 * fcoe_vHBA_advertisement
 * This function first creates the TLV's from the data structures
 * using the create_tlv function and fills the message descriptor
 * by adding the TLV's to it.
 * Sent from VFM to host (Also acts as Keep Alive from VFM to host).
 *
 * adv       [IN]   - Pointer to  the vHBA Advertisment structure.
 * mesg_desc [OUT]  - It contains the pointer to the message descriptor
 *                                        buffer i.e. the payload.
 *
 *  Returns vps_error: 0 for success.
 *
 */
vps_error
eoib_vNIC_advertisement(eoib_vNIC_adv *adv, uint8_t *msg_desc)
{
        vps_error err = VPS_SUCCESS;
        uint8_t *offset = msg_desc;
        vp_tlv tlv;


        vps_trace(VPS_ENTRYEXIT, "Entering eoib_vNIC_advertisement");

        /*
         * TODO : Write code for creating the FIP packet by reading the values
         * from the database and filling up the packet structure to send
         * it from the VFM to the host.
         */


        /* TLV 13 : Vendor ID , len = 3 */
        tlv.type   = 13;
        tlv.length = 3;
        create_tlv(&adv->vendor_id, &tlv);
        memcpy(offset, &tlv, 2);
        offset += 2;
        memcpy(offset, tlv.value, tlv.length * DWORD - 2);
        offset += tlv.length * DWORD - 2;
        free(tlv.value);

        /* TLV MLNX_240 : len = 7 */
        tlv.type   = MLX_TLV_240;
        tlv.length = 7;
        create_tlv(&adv->mlnx_240, &tlv);
        memcpy(offset, &tlv, 2);
        offset += 2;
        memcpy(offset, tlv.value, tlv.length * DWORD - 2);
        offset += tlv.length * DWORD - 2;
        free(tlv.value);

        /* TLV MLNX_241 : len = 4 */
        tlv.type   = MLX_TLV_241;
        tlv.length = 4;
        create_tlv(&adv->mlnx_241, &tlv);
        memcpy(offset, &tlv, 2);
        offset += 2;
        memcpy(offset, tlv.value, tlv.length * DWORD - 2);
        offset += tlv.length * DWORD - 2;
        free(tlv.value);

        vps_trace(VPS_ENTRYEXIT, "Leaving eoib_vNIC_advertisement");
        return err;
}


/*
 * This function fills up the structure for sending a GW advertisement
 * to the Host for EoIB.
 *
 * control_hdr   [IN]     : Control header with the information.
 * *init_info    [IN]     : initialization info of device ,qpair ,lid.
 * msg_desc      [IN/OUT] : It contains the pointer to the packet.
 *                          Allocated in this function. Freed in the
 *                          function using the pointer.
 *
 */

vps_error
prepare_eoib_vNIC_advertisement(ctrl_hdr *control_hdr,
                vfm_init_info_packet *init_info, uint8_t **msg_desc)
{
        eoib_vNIC_adv gw_adv;
        vps_error err = VPS_SUCCESS;
        uint16_t length = htons(control_hdr->desc_list_length);
        vps_trace(VPS_ENTRYEXIT, "Entering prepare_eoib_vNIC_advertisement");

        vps_trace(VPS_INFO, "PREPARING GW ADV");
        /*
         * TODO :Hard Coding the Values in the Gateway Advertisement structure
         * for now. These values will be filled up from the database by
         * calling a routine to populate the structure. By Gautam.
         *
         */

        memcpy(gw_adv.vendor_id, mellanox_str , NAME_LEN);

        memcpy(gw_adv.mlnx_240.vendor_id, mellanox_str, NAME_LEN);
        gw_adv.mlnx_241.reserved = 0;

        gw_adv.mlnx_240.qpn = htonl(init_info->qp); 
        gw_adv.mlnx_240.sl_gw_port_id = htons(0); 
        gw_adv.mlnx_240.lid = htons(init_info->lid); 
        memcpy(gw_adv.mlnx_240.gw_guid,"1", NAME_LEN);

        memcpy(gw_adv.mlnx_241.vendor_id, mellanox_str, NAME_LEN);
        gw_adv.mlnx_241.h_bit = 0;
        gw_adv.mlnx_241.vlan_num = htons(0x80);


        /* Allocate memory for the descriptor list or Payload */
        *msg_desc = (uint8_t *)malloc(length * DWORD);
        memset(*msg_desc, 0, length * DWORD);

        eoib_vNIC_advertisement(&gw_adv, *msg_desc);

        /*
         * TODO: Call the function to send the packet on IB network
         * Presently freeing the buffer here only
         */
        vps_trace(VPS_ENTRYEXIT, "Leaving prepare_eoib_vNIC_advertisement");
        return err;
}

