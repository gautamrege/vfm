/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 *
 * Header File: Data Structures for IB header and packets
 */
#ifndef __VFM_IBOE_H_
#define __VFM_IBOE_H_

#include <common.h>
#include <vfm_fip.h>


/* VFM  Init info */
typedef struct __vfm_init_info_packet {
       uint16_t lid;
       uint8_t  gid[16];
       uint32_t qp;
       uint16_t pkey;
}vfm_init_info_packet;

/*
 * MELANOX SPECIFIC TLV 240, Stores GW info.
 */
typedef struct __mlx_spec_tlv_240 {
        uint8_t vendor_id[8];
        uint32_t  qpn;
        uint16_t sl_gw_port_id;
        uint16_t lid;
        uint8_t gw_guid[8];
}mlx_spec_tlv_240;


/*
 * MELANOX SPECIFIC TLV 240, Stores VLAN related info.
 */
typedef struct __mlx_spec_tlv_241 {
        uint8_t vendor_id[8];
        uint8_t h_bit;         /* First bit indicates VLANS accpted by GW */
        uint8_t reserved;
        uint16_t vlan_num;    /* Lower 12bits give number of VLANs */
}mlx_spec_tlv_241;

/*
 * ---GW Advertisement for EoIB structure ---
 */
typedef struct __eoib_vNIC_adv {
        uint8_t vendor_id[8];
        mlx_spec_tlv_240 mlnx_240;
        mlx_spec_tlv_241 mlnx_241;
}eoib_vNIC_adv;

/*--- ConnectX Discovery for EoIB--- */
typedef struct __eoib_conx_vfm_adv {
        uint8_t vendor_id[8];
        mlx_spec_tlv_240 mlnx_240;
}eoib_conx_vfm_adv;

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
                vfm_init_info_packet *init_info, uint8_t **msg_desc);

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
eoib_vNIC_advertisement(eoib_vNIC_adv *adv, uint8_t *msg_desc);

/*
 * eoib_conx_discovery
 * This function first fills the TLV from the buffer using the
 * get_tlv function. It then fills the predefined data structures
 * for the ConnectX Solicitation from the TLV.
 * Sent from ConnectX(unicast) to VFM through BridgeX.
 *
 * mesg_desc (IN)  - It contains the copy of the packet received
 *                   from the network.
 * solicit   (OUT) - Pointer to  the connectX_solication structure.
 *
 * Returns vps_error: 0 for success.
 *
 */
vps_error
eoib_conx_discovery(uint8_t *msg_desc, eoib_conx_vfm_adv *solicit);


#endif /* __VFM_IBOE_H_ */
