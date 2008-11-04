/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 * 
 * Header File: Data Structures for ALL FIP packets
 */
#ifndef __VFM_FIP_H_
#define __VFM_FIP_H_

#include <common.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <sys/socket.h>

#ifdef MELLANOX
#include <mlx_fip.h>

/*FIP version*/
#define FIP_VERSION 0x1

/* Definitions for Mellanox Header. */
#define MLX_HDR_OPCODE   0xFFF8     /* Mellanox header opcode */
#define MLX_HDR_SUBCODE  0x0000     /* Mellanox header sub opcode */

/**** Definitions for Mellanox Control Header. ****/
#define MLX_CTRL_HDR_OPCODE  0xFFFC /* Mellanox Control Header opcode */
#define MLX_CTRL_HDR_SUBCODE 0x02   /*Mellanox Control Header subopcode */
#endif /* MELLANOX */

#define FIP_ETH_TYPE   0x8914       /* Ether type for FIP packet*/
#define ETH_HDR_LEN    20           /* Ethernet header lenght in bytes*/ 

#define VLAN_TAG         0x8100     /* VLAN tag value */
#define TUNNEL_HDR_TYPE  244        /* Tunnel header type vlaue */

#define DWORD            4          /* 1 dword = 4 byte */
#define NAME_LEN         8          /* 8 bytes */

#define TLV_1            1
#define TLV_2            2
#define TLV_3            3
#define TLV_4            4
#define TLV_5            5
#define TLV_6            6
#define TLV_7            7
#define TLV_8            8
#define TLV_9            9
#define TLV_10           10
#define TLV_11           11
#define TLV_12           12

                                    /* MELLANOX SPECIFIC TLV's */
#define MLX_TLV_1        101        /* Table 9: VFM Solicitation Descriptor  for BridgeX */
#define MLX_TLV_2        102        /* Table 5: BridgeX Discovery Advertisement Descriptor */

#define GET_PK_TYPE(op, subop) ((uint32_t)(op) << 8 | (subop)) & 0x00ffffff

/* Mapping names to the opcode and subopcode of the packet*/
#define BX_DISC GET_PK_TYPE(0xFFFA, 0x2)
#define VFM_RES GET_PK_TYPE(0xFFFA, 0x1)
#define CX_DISC GET_PK_TYPE(   0x1, 0x1)
#define GW_ADV  GET_PK_TYPE(   0x1, 0x2)


/* Ethernet  header. */
typedef struct __eth_hdr{
    uint8_t  dhost_mac[MAC_ADDR_LEN]; /* Source host mac address */
    uint8_t  shost_mac[MAC_ADDR_LEN]; /* Destination host mac address */
    uint32_t vlan_tag;                /* VLAN Tag (IEEE 802.1q) It is optional*/
    uint16_t ether_type;              /* Ether type : For FIP 0x8914 */
    /* Version is the most significant 4-bits. last 12=bit is reserved */
    uint16_t version;
}eth_hdr;

/* FC Header*/
typedef struct __fc_hdr{
    /* link service 22h = Unsolocited control for request
       23h = Solocited control for request 
       and frame destination address 
       Route Control   = 0xFF000000 >> 24;
       Dest  identifier= 0x00FFFFFF; 
     */
    uint32_t rt_ctrl_dest_id;     
    /*reserved + Frame source address 
      Reserved          = 0xFF000000 >> 24;
      Source  identifier= 0x00FFFFFF; */
    uint32_t src_id; 

    /*01h = Extended Link Service 
      290000h = frame is from the originator of the exchange 
      990000h = frame is from the originator of the exchange  
      Type          = 0xFF000000 >> 24;
      Frame Control = 0x00FFFFFF; 
     */                                 
    uint32_t type_frame_ctrl; 
    uint8_t seq_id;                /* Uniquely identifies frames in a 
                                      non-streamed sequence or 
                                      when only one sequence is open */
    uint8_t data_field_ctrl;       /*Set to 00h to indicate that 
                                      no optional FC headers are used*/
    uint16_t seq_count;            /*Indicates the sequential order
                                      of frames within a sequence. 
                                      Start with 0000h */
    uint16_t ox_id;                /*Assigned by originator*/
    uint16_t res_id;               /*Assigned by originator*/   
    uint32_t parameter;            /* set 00000000h*/ 
}fc_hdr;

typedef struct __fcoe_flogi_paylaod
{
    uint32_t lscc;
    uint8_t hav;
    uint8_t lav;
    uint16_t b2bc;
    uint16_t common;
    uint16_t recv_data_size;
    uint32_t R_A_TOV;
    uint32_t E_D_TOV;
    uint8_t g_wwnn[8];
    uint8_t g_wwpn[8];
}fcoe_flogi_paylaod;


/* Standard Control Header structure.*/
typedef struct __ctrl_hdr {
    uint16_t opcode;              
    uint8_t  reserved;                 /* for future use */
    uint8_t  subcode;                
    uint16_t desc_list_length;      
    uint16_t flags;                    /* FP:1, SP:1, flags:11, A:1, S:1, F:1*/
}ctrl_hdr;

#ifdef MELLANOX
/* Mellanox Header structure.
 * opcode : 0xFFF8 
 * subcode: 0x0
 */  
typedef struct __mlx_hdr {
    ctrl_hdr header;
    uint8_t  t10_vendor_id[NAME_LEN];         /* T10 Vendor Id */
}mlx_hdr;       

/* Tunnel Header structure */
typedef struct __mlx_tunnel_hdr {
    uint8_t type;
    uint8_t length;
    uint16_t reserved1;
    uint8_t  t10_vender_id[NAME_LEN];  /* T10 Vender ID = "Mellanox" */
    uint8_t  group;
    uint8_t  port_num;        /* Port number MSB:1 = external, 0=internal */
    uint16_t reserved2;
    uint32_t gw_id;          /* Determine from gateway initial advertisement */ 
    uint32_t vfm_id;         /* VFM id - set during association with bridgex */
}mlx_tunnel_hdr;

#endif

/* Type-Length-Value Structure
 * - value should be allocated <length> bytes before being assigned. 
 */
typedef struct __vp_tlv {
    uint8_t type;                   /* FIP Descriptor type */
    uint8_t length;                 /* FIP Descriptor length */
    void   *value;                  /* FIP Descrtiprot value */
}vp_tlv;

/** --- BridgeX to VFM Advertisement  (Ref: Table 11) ---**/
typedef struct __fcoe_bridge_vfm_adv {
    uint8_t  bridge_mac[MAC_ADDR_LEN];       /* Bridgex mac address */  
    uint8_t  node_name[NAME_LEN];            /* Bridgex Node Name   */
    uint16_t max_recv;                       /* Max receive Size    */

    /* Gautam - Today this plugin support Mellanox. Tomorrow, if we want to a 
     * build specific Cisco plugin, we need to simply set the compile flag.
     * As an example, we are writing a CISCO macro for this.
     */
#ifdef MELLANOX
    mlx_bridgeX info;                        /* Mellanox gateway information */ 
#elif CISCO 
    cisco_bridge info;
#endif
}fcoe_bridge_vfm_adv;

/** --- VFM to BridgeX advertisement response (Ref: Table 15) --- **/
typedef struct __fcoe_vfm_bridge_adv_res{
    uint8_t vfm_mac[MAC_ADDR_LEN];  /* VFM mac address */
    uint8_t node_name[NAME_LEN];    /* VFM Node Name: From Bx Advertisement  */
    uint16_t max_recv;              /* Max Receive Size: From Bx Advertisement*/
#ifdef MELLANOX
    mlx_adv_res res;
#endif
}fcoe_vfm_bridge_adv_res;

/** --- ConnectX Solicitation Descriptor Structure (Ref: Table 18) --- **/
typedef struct __fcoe_conx_vfm_adv{
    uint8_t  host_mac[MAC_ADDR_LEN]; /* HOST mac address */
    uint8_t  node_name[NAME_LEN];    /* Node name */
    uint16_t max_recv;               /* Max Receive Size */  
}fcoe_conx_vfm_adv;    

/** --- vHBA Advertisement & Keep Alive- VFM to ConnectX (Ref: Table 24) --- **/
typedef struct __fcoe_vHBA_adv{
    uint8_t  priority;                  /* Priority */
    uint8_t  fcf_gw_mac[MAC_ADDR_LEN];  /* FCF gateway mac address */
    uint32_t fc_map;                    /* FC-MAP TODO: Where does it come from */
    uint8_t switch_name[NAME_LEN];      /* Switch Name: Where does it come from */
    uint8_t fabric_name[NAME_LEN];      /* Fabric name: Where does it come from */
    uint32_t fka_adv_period;            /* FKA_ADV_PERIOD-frequency of keep alive 
                                           adv TODO : Clarify for Length*/
}fcoe_vHBA_adv;

/** --- ConnectX vHBA keep Alive Descriptor (Ref: Table 44) --- **/
typedef struct __vHBA
{
    uint8_t  fcf_mac[MAC_ADDR_LEN];   /* FCF gateway mac address */
    uint32_t port_id;                 /* vHBA Port ID:Lower 23 bits are port id */
    uint8_t  name[NAME_LEN];          /* vHBA Name */
}vHBA;

/** --- ConnectX vHBA keep Alive Descriptor (Ref: Table 44) ---
 *  --- ConnectX Solicitation Descriptor Structure (Ref: Table 18) ---
 *
 * Note: When VFM sends a vHBA advertisement, the vHBA data structure need not
 * be filled. When the ConnectX sends a keep-alive packet, the vHBA will be 
 * updated by the VFM
 *
 **/
typedef struct __fcoe_vHBA_alive{
    uint8_t host_mac[MAC_ADDR_LEN]; /* HOST mac address */
    uint8_t node_name[NAME_LEN];    /* Node Name */
    uint16_t max_recv;              /* Max Receive Size */
    vHBA hba;                       /* VHBA info: ConnectX vHBA keep Alive */
}fcoe_vHBA_alive;

/** ---  Delete virtual HBA interface. (Ref: Table 52) --- **
 * NOTE: This can be sent by host or VFM.
 */
typedef struct __fcoe_vHBA_dereg{
    uint8_t fcf_gw_mac[MAC_ADDR_LEN];            /* Host mac address */
    uint8_t switch_name[NAME_LEN];               /* Switch Name      */
    vHBA hba;   
}fcoe_vHBA_dereg;

/** This is the context table entry.
 * type = 0 implies it is a vHBA entry and the fields 'fc_id' and 'host_mac'
 * are valid. A context table will have 1 entry of this kind per vHBA.
 *
 * type = 1 implies the Gateway internal address. 'gateway_mac' field applies 
 * for this type. A context table will only have 1 entry of this type.
 */
typedef struct __vHBA_ctx
{
    uint8_t  valid;                           /* If True, the entry is valid */
    uint32_t type;                            /* Type of Entry */
    uint32_t fc_id;                           /* vHBA FC ID (N_Port ID) */
    uint8_t  host_mac[MAC_ADDR_LEN];          /* MAC address of host */
    uint8_t  gateway_mac[MAC_ADDR_LEN];       /* MAC address of the Gateway */
}vHBA_ctx;


/* Sniffer thread */
void * start_sniffer(void * arg);

/** get_tlv
 * This function fills the TLV i.e the Type, length, value structure
 *
 * mesg_desc (IN)  - It contains the packet received from the network.
 * tlv       (OUT) - It is a pointer to the TLV structure.
 *                   Memory is allocated for Value in this function.          
 *
 * Returns vps_error: 0 for success.
 */
vps_error get_tlv(uint8_t *, vp_tlv *);


/** get_tlv_value
 * This function typecast the values from the TLV into the
 * predefined data structures depeding upon the TYPE specified.
 *
 * tlv    (IN)   - It is a pointer to the TLV data structure.
 * data   (OUT)  _ It is a preallocated pointer to a member of a structure.
 *
 * Returns vps_error : 0 for success.
 */

vps_error get_tlv_value(vp_tlv *, void  *);

/** fcoe_conx_discovery
 * This function first fills the TLV from the buffer using the
 * get_tlv function. It then fills the predefined data structures
 * for the ConnectX Solicitation from the TLV.
 * Sent from ConnectX(multicast) to VFM.
 *
 * mesg_desc (IN)    - It contains the packet received from the network.
 * solicit   (OUT)   - Pointer to  the preallocated connectX_solication 
 *                     structure.
 *
 *  Returns vps_error: 0 for success.
 *
 */

vps_error fcoe_conx_discovery(uint8_t *, fcoe_conx_vfm_adv *);

/** fcoe_bridgeX_discovery
 * This function first fills the TLV from the buffer using the
 * get_tlv function. It then fills the predefined data structures
 * for the BridgeX Solicitation from the TLV.
 * Sent from BridgeX(multicast) to VFM.
 *
 * mesg_desc (IN)    - It contains the packet received from the network.
 * solicit   (OUT)   - Pointer to the preallocated bridgeX_solication 
 *                     structure.
 *   
 *  Returns vps_error: 0 for success.
 */

vps_error fcoe_bridgeX_discovery(uint8_t *, fcoe_bridge_vfm_adv *);



/** fcoe_vHBA_keep_alive
 * This function first fills the TLV from the buffer using the
 * get_tlv function. It then fills the predefined data structures
 * for the vHBA alive from the TLV.
 * Sent from ConnectX(unicast) to VFM. If the VFM does not get this packet
 * within a specified time it will tear down the virtual interface.
 *
 * mesg_desc (IN)    - It contains the packet received from the network.
 * solicit   (OUT)   - Pointer to  the preallocated vHBA Alive structure.
 *
 *  Returns vps_error: 0 for success.
 *
 */

vps_error fcoe_vHBA_keep_alive(uint8_t *, fcoe_vHBA_alive *);


/** fcoe_vHBA_deregister
 * This function first fills the TLV from the buffer using the
 * get_tlv function. It then fills the predefined data structures
 * for the vHBA dereg from the TLV.
 * Sent from ConnectX (unicast) to VFM. The VFM will delete the virtual 
 * interfaces.
 *
 * mesg_desc (IN)    - It contains the packet received from the network.
 * solicit   (OUT)   - Pointer to  the preallocated vHBA Dereg structure.
 *
 *  Returns vps_error: 0 for success.
 *
 */

vps_error fcoe_vHBA_deregister(uint8_t *, fcoe_vHBA_dereg *);


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
vps_error create_packet(uint16_t op, uint8_t subop, void *data);



/*Create packet for sending
 *
 *[IN] tunnel_flag : Use for add tunnel header or not.
 *[IN] *vfm_mac    : Mac addresss of VFM
 *[IN] *conx_mac   : Mac addresss of ConnectX.
 *[IN] *bridge_mac : Mac addresss of BridgeX.
 *[IN] *tunnel_hdr : Tunnel Header to add to packet.
 *[IN] *control_hdr: Control Header to add to packet.
 *[IN] *desc_buff  : Descriptor buffer to add to packet.
 *
 * Return : error code.
 *          Packet sending error.
 */

vps_error send_packet(uint8_t tunnel_flag,
        uint8_t *vfm_mac,
        uint8_t *conx_mac,
        uint8_t *bridge_mac,
        mlx_tunnel_hdr *tunnel_hdr,
        ctrl_hdr *control_hdr,
        uint8_t *desc_buff,
        uint16_t max_recv);

/*Create packet for sending
 *
 *[IN] *vfm_mac    : Mac addresss of VFM
 *[IN] *bridge_mac : Mac addresss of BridgeX.
 *[IN] *tunnel_hdr : Tunnel Header to add to packet.
 *[IN] *desc_buff  : Descriptor buffer to add to packet.
 *[IN] desc_len
 *
 * Return : error code.
 *          Packet sending error.
 */

vps_error send_fc_packet(
        uint8_t *vfm_mac,
        uint8_t *bridge_mac,
        mlx_tunnel_hdr *tunnel_hdr,
        uint8_t *desc_buff,
        uint32_t desc_len);

#endif /* __VFM_FIP_H_ */
