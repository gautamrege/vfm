/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 *
 * Header File: Data Structures for ALL FIP packets
 */
#ifndef __VFM_QUEUE_H_
#define __VFM_QUEUE_H_

#include <common.h>
#include <vfm_fip.h>

struct fip_packet {
        uint8_t type;    /*Packet from locla VFM or remote VFM*/
        uint8_t *data;
        int size;
        struct fip_packet  *next;
        struct fip_packet  *prev;
};

/*
 * Add packet data to queue.
 *
 * [IN] *data : Data read from sniffer.
 * [IN] size  : Packet size.
 * [IN] tyep  : VFM type - LBC or RBC
 *
 * Return : err
 *
 */
vps_error
add_packet_to_queue(uint8_t *data, int size, uint8_t type);

/*
 * Get packet from queue for processing and remove from queue.
 *
 * Return : packet.. THE CALLSER SHOULD FREEE THE PACKET !
 */
struct fip_packet *
get_packet_from_queue();

#endif /* __VFM_QUEUE_H_ */
