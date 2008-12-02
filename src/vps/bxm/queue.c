/*
 *  Copyright (c) 2008  VirtualPlane Systems, Inc.
 */

#include<queue.h>


pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond =  PTHREAD_COND_INITIALIZER;

struct fip_packet *first_packet = NULL; /*Locate the first packet in queue*/
struct fip_packet *last_packet  = NULL; /*Locate the last packet in queue*/

int num_packet_in_queue = 0;

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
add_packet_to_queue(uint8_t *data, int size, uint8_t type)
{
        vps_error err = VPS_SUCCESS;

        struct fip_packet *new_packet;

        vps_trace(VPS_ENTRYEXIT, "Entering add_packet_to_queue");

        /*Allocate memory for queue element*/
        new_packet = (struct fip_packet*)malloc(sizeof(struct fip_packet));

        /*
         * Set VFM Type- If packet receive forn LBC then it is 0
         * If packet receive forn RBC then it is 1
         */
        new_packet->type = type;

        /*Set size of packet deta*/
        new_packet->size = size;

        /*Allocate memory for packet data and copy packet data*/
        new_packet->data = (uint8_t*)malloc(size);
        memcpy(new_packet->data, data, size);

        /*----- CRITICAL REGION START--- */

        pthread_mutex_lock(&queue_mutex);

        /*
         * IF no packet in queue then add packet and set it to first packet
         * and set previous packet pinter to  NULL.
         * ELSE add packet to queue and set previous packet to old last packet.
         */
        if (first_packet == NULL) {
                first_packet = new_packet;
                new_packet->prev = NULL;
        }
        else {
                last_packet->next = new_packet;
                new_packet->prev = last_packet;
        }

        /*Update last packet pointer to new added packet*/
        last_packet  = new_packet;

        /*Set next packet pointer to NULL for added new packet*/
        new_packet->next = NULL;

        /*Increment number of packet in queue for processing*/
        num_packet_in_queue++;

        pthread_cond_broadcast(&queue_cond);
        pthread_mutex_unlock(&queue_mutex);
        /*----- CRITICAL REGION END--- */

        vps_trace(VPS_ENTRYEXIT, "Leaving add_packet_to_queue");

        return err;
}

/*
 * Get packet from queue for processing and remove from queue.
 *
 * Return : packet.. THE CALLSER SHOULD FREEE THE PACKET !
 */
struct fip_packet *
get_packet_from_queue()
{
        vps_error err = VPS_SUCCESS;

        struct fip_packet *packet = NULL;

        vps_trace(VPS_ENTRYEXIT, "Entering get_packet_from_queue");

        /*----- CRITICAL REGION START--- */
        pthread_mutex_lock(&queue_mutex);

        while (num_packet_in_queue == 0)
          pthread_cond_wait(&queue_cond, &queue_mutex);

        packet = first_packet;

        if (first_packet && first_packet->next != NULL) {
           first_packet = first_packet->next;
           first_packet->prev = NULL;
        }
        else {
                last_packet = NULL;
                first_packet = NULL;
        }

        /*Decrement number of packets in queue*/
        num_packet_in_queue--;

        pthread_mutex_unlock(&queue_mutex);
        /*----- CRITICAL REGION END--- */

        vps_trace(VPS_ENTRYEXIT, "Leaving get_packet_from_queue");

        return packet;
}
