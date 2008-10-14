/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#include <net_util.h>

#define MULTICAST PACKET_MULTICAST
#define BROADCAST PACKET_BROADCAST

extern struct  sockaddr_ll g_socket_addr;
extern uint8_t g_local_mac[MAC_ADDR_LEN];
extern uint8_t g_if_index;
extern uint8_t g_if_mtu;

int send_sd = -1;
/* TODO: Current hard coded mac */
uint8_t g_bridge_mac[MAC_ADDR_LEN];
uint8_t g_bridge_enc_mac[MAC_ADDR_LEN];

/**
 * send buffer on socket.
 *
 * [IN] *sock_addr : Link layer 2 socket address.
 * [IN] send_type  : Multicast,Unicast etc.
 * [IN] *buff      : packet buffer.
 * [IN] length     : buffer length.
 */
vps_error
send_buffer(uint8_t send_type, 
        uint8_t *buff,
        int length)
{
    vps_error err = VPS_SUCCESS;  

    vps_trace(VPS_ENTRYEXIT, "Entering send_buffer");

    g_socket_addr.sll_pkttype = send_type;

    if(send_sd == -1)
    {
        if(VPS_SUCCESS != (err = open_socket(g_if_index, g_local_mac,
						&send_sd)))
        {
            vps_trace(VPS_ERROR, "Error in opening socket");
            goto out;
        }
    }              

    if(sendto(send_sd, buff, length, 0, 
            (struct sockaddr*)&g_socket_addr, sizeof(struct sockaddr_ll)) < 0)
    {
        vps_trace(VPS_ERROR, "Error at send buffer");
        err = VPS_ERROR_SEND_PK;
    }

out : 

    vps_trace(VPS_ENTRYEXIT, "Leaving send_buffer");
    return err;
}


/**
 * Fill ethernet header
 *
 * [IN] *buff      : Packet buffer
 * [IN] *dhost_mac : Destination host mac address.
 * [IN] *shost_mac : Source mac address.
 * [IN] *valntag   : Virutal lane tag.
 * [INOUT] *offset : Offset off packet buffer where to start copy.
 *
 * Return : error code.
 */
vps_error
fill_ether_hdr(uint8_t *buff,
                       uint8_t *dhost_mac,
                       uint8_t *shost_mac,
                       uint32_t *vlantag,
                       uint32_t *offset)
{
    vps_error err = VPS_SUCCESS;

    /*Fip version size is 4 bit.So shift with 12 bit*/
    uint16_t fip_version = htons((uint16_t)FIP_VERSION << 12); 
    uint16_t fip_eth_type = htons(FIP_ETH_TYPE);

    
    vps_trace(VPS_ENTRYEXIT, "Entering fill_eth_hdr");

    /*Copy destination host mac address and increament offset by MAC_ADDR_LEN */
    memcpy(buff + *offset, dhost_mac, MAC_ADDR_LEN);

    *offset += MAC_ADDR_LEN;

    
    /*Copy source host mac address and increament offset by MAC_ADDR_LEN */
    memcpy(buff + *offset, shost_mac, MAC_ADDR_LEN);    

    *offset += MAC_ADDR_LEN;

    /*If vlan not equal to 0 than add to buffer and increament offset 
     * by 4 bytes 
     */
    if(*vlantag != 0)
    {
        memcpy(buff + *offset, vlantag, sizeof(uint32_t));    

        *offset += sizeof(uint32_t);

        vps_trace(VPS_INFO, "Vlan tag added");
    }

    /*Copy fip ether tyep and increament offset by 2 bytes*/
    memcpy(buff + *offset, &fip_eth_type, sizeof(uint16_t));

    *offset += sizeof(uint16_t);

    /*Copy FIP version and increament offset by 2 bytes*/
    memcpy(buff + *offset, &fip_version , sizeof(uint16_t));

    *offset += sizeof(uint16_t);


    vps_trace(VPS_ENTRYEXIT, "Leaving fill_eth_hdr");

    return err;

}

/**
 * Copy tunnel header in to packet buffer.
 *
 * [IN] *buff      : Packet buffer
 * [IN] *tunnel_hdr: Tunnel header for add to packet.
 * [INOUT] *offset : Offset off packet buffer where to start copy.
 *
 * Return : error code.
 */
vps_error
fill_tunnel_hdr(uint8_t *buff,
        mlx_tunnel_hdr *tunnel_hdr, 
        int *offset)
{

    vps_error err = VPS_SUCCESS;

    vps_trace(VPS_ENTRYEXIT, "Entering fill_tunnel_hdr");


    tunnel_hdr->type   = TUNNEL_HDR_TYPE;
//    tunnel_hdr->length = (sizeof(mlx_tunnel_hdr) + payload_len)/DWORD;

    /*Copy Tunnel header data and increament offset by tunnel 
     * header length bytes
     */
    memcpy(buff + *offset, tunnel_hdr , sizeof(mlx_tunnel_hdr));

    *offset += sizeof(mlx_tunnel_hdr);
    
    vps_trace(VPS_ENTRYEXIT, "Leaving fill_tunnel_hdr");

    return err;
}



/**
 * Create packet for sending 
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

vps_error
send_packet(uint8_t tunnel_flag, 
        uint8_t *vfm_mac, 
        uint8_t *conx_mac, 
        uint8_t *bridge_mac,
        mlx_tunnel_hdr *tunnel_hdr,
        ctrl_hdr *control_hdr,
        uint8_t *desc_buff)
{
    vps_error err = VPS_SUCCESS;

    uint8_t buff[MAX_PK_LEN];
    uint32_t offset       = 0;
    uint32_t vlan_tag     = 0; 
    uint32_t en_footer    = 0; 
    uint32_t en_footer_fw = 0;
    uint16_t desc_length  = control_hdr->desc_list_length * DWORD;

       
    vps_trace(VPS_ENTRYEXIT, "Entering make_packet");


    /*-- 1.Add ethernet header.-- */
    /*fill ethernet haeader*/ 
    fill_ether_hdr(buff, g_bridge_mac, vfm_mac, &vlan_tag, &offset);

    /*--2.If tunnel flag true than  
     *Add tunnel header.(For host,connectX)
     *Add ethernet header to forword packet 
     */
    /*If tunnel header flag set than*/
    if(tunnel_flag == TRUE)
    {
        /*Fill tunnel header*/
        tunnel_hdr->length = (sizeof(mlx_tunnel_hdr) + 4*DWORD + 
                             sizeof(ctrl_hdr) + desc_length)/DWORD;
        fill_tunnel_hdr(buff, tunnel_hdr, &offset);

        /* TODO: Clean up this demo HACK */
        if(control_hdr->opcode == 2 && control_hdr->subcode == 1)
        {
            /* FLOGI FWED PACKET */
            fill_ether_hdr(buff, conx_mac, bridge_mac, &vlan_tag, &offset);
        }
        else
        {
            /*fill ethernet haeader*/ 
            fill_ether_hdr(buff, conx_mac, g_bridge_enc_mac, &vlan_tag,
			    &offset);
        }
    }

    /*-- 3. Add control header --*/     
    /*Control header*/
    control_hdr->opcode = htons(control_hdr->opcode);
    control_hdr->desc_list_length = htons(control_hdr->desc_list_length);
    control_hdr->flags = htons(control_hdr->flags);
    memcpy(buff + offset, control_hdr , sizeof(control_hdr));

    offset += sizeof(ctrl_hdr);

    /*--4.Add message descriptor.--*/
    /* Copy descriptor buffer */
    memcpy(buff + offset, desc_buff , desc_length);

    offset += desc_length;


    /*--5.Add forward ethernet footer.--*/
    /*If tunnel flag true then Copy Footer for forword */
    if(tunnel_flag == TRUE)
    {
        memcpy(buff + offset, &en_footer_fw , sizeof(uint32_t));

        offset += sizeof(uint32_t);
    }
    

    /*--7.Add ethernet footer.--*/
    memcpy(buff + offset, &en_footer , sizeof(uint32_t));

    offset += sizeof(uint32_t);

    /*--8.send data by socket*/
    if((err = send_buffer( MULTICAST, buff, offset ))!= VPS_SUCCESS)
    {
        vps_trace(VPS_ERROR, "Packet send err");
        err = VPS_ERROR_SEND_PK;
    }
     
out:

    vps_trace(VPS_ENTRYEXIT, "Leaving send packet");
    return err; 
}

/*
 * Send FC packet.
 *
 *[IN] *vfm_mac    : Mac addresss of VFM
 *[IN] *bridge_mac : Mac addresss of BridgeX.
 *[IN] *tunnel_hdr : Tunnel Header to add to packet.
 *[IN] *desc_buff  : Descriptor buffer to add to packet.
 *[IN] *desc_len   : Descriptor buffer length.
 *
 * Return : error code.
 *          Packet sending error. 
 */
vps_error send_fc_packet( uint8_t *vfm_mac,  
                          uint8_t *bridge_mac,
                          mlx_tunnel_hdr *tunnel_hdr,
                          uint8_t *desc_buff,
                          uint32_t desc_len)
{
 
    vps_error err = VPS_SUCCESS;

    uint8_t buff[MAX_PK_LEN];
    uint32_t offset       = 0;
    uint32_t vlan_tag     = 0; 
    uint32_t en_footer    = 0; 
    uint32_t fc_footer    = 0;
    uint32_t sof          = 0;


    vps_trace(VPS_ENTRYEXIT, "Entering send_fc_packet");


    /*-- 1.Add ethernet header.-- */
    fill_ether_hdr(buff, g_bridge_mac, vfm_mac, &vlan_tag, &offset);

    /*--2.Fill tunnel header */  
    /* Tunnel Header Length + SOF length + Descriptor Length +FC footer length + */
    tunnel_hdr->length = (sizeof(mlx_tunnel_hdr) + DWORD + desc_len + DWORD)/DWORD;
    fill_tunnel_hdr(buff, tunnel_hdr, &offset);

    /*--3.Add SOF to FC header.--*/
    memcpy(buff + offset, &sof , sizeof(uint32_t));
    offset += sizeof(uint32_t);

    /*--4.Add FC header and message descriptor.--*/
    memcpy(buff + offset, desc_buff , desc_len);
    offset += desc_len; 

    /*--5.Add  FC footer.--*/
    memcpy(buff + offset, &fc_footer , sizeof(uint32_t));
    offset += sizeof(uint32_t);

    /*--6.Add ethernet footer.--*/
    memcpy(buff + offset, &en_footer , sizeof(uint32_t));
    offset += sizeof(uint32_t);

    /*--7.send data by socket*/
    if((err = send_buffer( MULTICAST, buff, offset ))!= VPS_SUCCESS)
    {
        vps_trace(VPS_ERROR, "Packet send err");
        err = VPS_ERROR_SEND_PK;
    }
     
    vps_trace(VPS_ENTRYEXIT, "Leaving send_fc_packet");

    return err;
}
