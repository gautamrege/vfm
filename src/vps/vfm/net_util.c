/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#include<net_util.h>

struct sockaddr_ll g_socket_addr;

/**
 * Determine if the Ethernet address is a multicast.
 * 
 * [IN] *addr: Ethernet address
 *
 * Return : true if the address is a multicast address.
 *
 * Info: By definition the broadcast address is also a multicast address.
 * i.e uinicast  = 00 : 50 : 56 : C0 : 00 : 01
 *     multicast = 10 : 50 : 56 : C0 : 00 : 01
 */
int
is_multicast_ether_addr(uint8_t *addr)
{
    return (0x01 & addr[0]);
}
 
/**
 * Determine if the Ethernet address is broadcast
 *
 * [IN] *addr: Ethernet address
 *
 * Return :  true if the address is the broadcast address.
 *
 * i.e FF : FF : FF : FF : FF : FF
 */
int
is_broadcast_ether_addr(uint8_t *addr)
{
    return (addr[0] & addr[1] & addr[2] & addr[3] & addr[4] & addr[5]) == 0xff;
}


/** 
 * This method get mac address , interfac index and 
 * max transmission uinit  of interface. 
 * 
 *  [IN]  *sd       : socket descriptor.
 *  [IN]  *if_name  : Network interface name(ethX)
 *  [OUT] *mac_addr : Mac address array.
 *  [OUT] *if_index : Interface index.
 *  [OUT] *if_mtu   : Interface max transmission unit.
 *
 *  Return : error code 
 *           error in open socket.
 *           error to get local mac.
 *           error to get interface index.    
 *           error to get interface MTU.
 */
vps_error
get_inf_property(char *if_name,
        uint8_t *mac_addr,
        uint8_t *if_index, 
        uint16_t *if_mtu)
{
    vps_error err = VPS_SUCCESS;
    struct ifreq ifr;
    int sd;

    vps_trace(VPS_ENTRYEXIT, "Entering get_inf_property");


    /* Open raw socket on OSI Layer-2 to receive for all type of packet. */
    if((sd =socket(PACKET_FAMILY, SOCK_RAW, PROTOCOL_TYPE))== -1)
    {
        vps_trace(VPS_ERROR, "Socket creation error");
        err = VPS_ERROR_SOCK_OPEN;
        goto out;
    }

    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, if_name);

    /*This system call fill up hardware property : mac address*/      
    if(ioctl(sd, SIOCGIFHWADDR, &ifr) !=0)
    {
        vps_trace(VPS_ERROR, "Error to get local mac");
        err = VPS_ERROR_GET_MAC;
        close(sd);   /*close socket */             
        goto out;
    }

    /*Copy mac address from the */
    memcpy(mac_addr, &ifr.ifr_hwaddr.sa_data,MAC_ADDR_LEN);

    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, if_name);
    /*This system call fill up hardware property : interface index*/
    if( ioctl(sd, SIOCGIFINDEX, &ifr) != 0 )
    {
        vps_trace(VPS_ERROR, "Error to interface index");
        err = VPS_ERROR_GET_IFINDEX;
        close(sd);                /*close socket*/
        goto out;
    }

    /*set interface index*/
    *if_index = ifr.ifr_ifindex;

    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, if_name);
    /*This system call fill up hardware property : interface mtu*/
    if( ioctl(sd, SIOCGIFMTU, &ifr) !=0 )
    {
        vps_trace(VPS_ERROR, "Error to get interface mtu");
        err = VPS_ERROR_GET_MTU;
        close(sd);                /*close socket*/
        goto out;
    }

    /*set mtu*/
    *if_mtu = ifr.ifr_mtu;

    close(sd);  /*close socket*/
out:
    vps_trace(VPS_ENTRYEXIT, "Leaving get_inf_property");
    return err;
}

/**
 * Open raw socket for receive all type of packet
 * on OSI layer-2 and bind with network interface.
 *
 * [IN] if_index : Networrk interface index.
 * [IN] mac_addr : Mac address network interface.
 * [OUT] *sd     : Socket descriptor.
 *
 * Returns : Error code.
 *           Error in open socket.
 */
vps_error
open_socket(uint8_t if_index, uint8_t *mac_addr, int *sd)
{
    vps_error err = VPS_SUCCESS;

    vps_trace(VPS_ENTRYEXIT, "Entering open_socket");

    memset(&g_socket_addr, 0, sizeof(struct sockaddr_ll));

    /* Open raw socket on OSI Layer-2 to receive for all type of packet. */
    if((*sd =socket(PACKET_FAMILY, SOCK_RAW, PROTOCOL_TYPE))== -1)
    {
        vps_trace(VPS_ERROR, "Socket creation error");
        err = VPS_ERROR_SOCK_OPEN;
        goto out;
    }

    g_socket_addr.sll_family  = PACKET_FAMILY;
    g_socket_addr.sll_protocol= PROTOCOL_TYPE;
    g_socket_addr.sll_ifindex = if_index;
    g_socket_addr.sll_halen =  MAC_ADDR_LEN;

    /* set mac address.
     * To only get  packets  from  a  specific  interface.
     */
    memcpy(g_socket_addr.sll_addr, mac_addr, MAC_ADDR_LEN);

out : 

    vps_trace(VPS_ENTRYEXIT, "Leaving open_socket");
    return err; 

}

/**
 * Bind socket to network interface for listen.
 *
 * [IN] *sd : socket descriptor.
 */
vps_error
bind_socket(int *sd)
{
    vps_error err = VPS_SUCCESS;

    vps_trace(VPS_ENTRYEXIT, "Leaving bind_socket");

    /* Bind socket to interface to listen  on perticular device*/
    if( bind(*sd ,(struct sockaddr *)&g_socket_addr, 
                sizeof(struct sockaddr_ll))== -1)
    {
        /* Bind fail error. */
        vps_trace(VPS_ERROR, "Socket bind error");
        err = VPS_ERROR_SOCK_BIND;
    }

    vps_trace(VPS_ENTRYEXIT, "Leaving bind_socket");

    return err;
}
