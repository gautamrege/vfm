/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#ifndef __NET_UTIL_H_
#define __NET_UTIL_H_

#include<common.h>
#include<vfm_fip.h>
#include<sys/ioctl.h>
#include<net/if.h>

#define PACKET_FAMILY PF_PACKET             /* Packet family */
#define PROTOCOL_TYPE htons(FIP_ETH_TYPE)   /* Packet type : FIP */


int is_multicast_ether_addr(uint8_t *);

#define MAX_PK_LEN       1500       /* Max packet length in bytes*/
#endif 
