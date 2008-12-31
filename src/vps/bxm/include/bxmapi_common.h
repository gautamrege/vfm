#ifndef BXM_COMMON_H
#define BXM_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <bxm_error.h>
#include <bxm_vadapter.h>
#include <bxm_vfabric.h>
#include <vfmdb.h>
#include <bxm_bridge_device.h>
/*
 * TODO: Header files to be included later
 * #include <bxm_common.h>
 * #include <bxm_gateway.h>
 * #include <bxm_port.h>
 * #include <bxm_table.h>
 * #include <bxm_state.h>
 * #include <bxm_buf.h>
 */

/*
 * TODO : Defining the ERROR codes
 * Later these will be moved in error.h
 */
#define BXM_SUCCESS             0
#define BXM_ERROR_SENDING       1
#define BXM_ERROR_RECEIVE       2
#define BXM_ERROR_CONNECT       3

#define SERVER_PORT             10006
#define MAXBUFFLEN              500
#define BUFFER_SIZE             1500 /* MTU will never go above this limit*/

/* Defining the Modules */
#define BXMAPI_IOMODULE         0x1
#define BXMAPI_GATEWAY          0x2
#define BXMAPI_VADAPTER         0x3
#define BXMAPI_BRIDGE_DEVICE    0x4
#define BXMAPI_VFABRIC          0x5

/* Defining the Operations */
#define BXM_CREATE              0x1
#define BXM_EDIT                0x2
#define BXM_QUERY               0x3
#define BXM_QUERY_INVENTORY     0x4
#define BXM_DESTROY             0x5
#define BXM_ONLINE              0x6
#define BXM_EDIT_PROTOCOL_ATTR  0x7
#define BXM_VA_ACTIVATE         0x11

/* Defining the TLV's */
#define TLV_INT                 0x1
#define TLV_CHAR                0x2
#define TLV_GUID                0x3

#define TLV_INT_ARR             0x9
#define TLV_BIT_MASK            0x10
#define TLV_VADP_ATTR           0x11
#define TLV_BD_ATTR             0x12
#define TLV_GATEWAY_ATTR        0x13
#define TLV_VFABRIC_ATTR_BITMASK 0x14
#define TLV_VFABRIC_ATTR         0x15

/* Define the data types to generate the query */
#define Q_UINT8                 0x01
#define Q_UINT32                0x02
#define Q_UINT64                0x03


#define NAME_SIZE  sizeof(char[64])              
/*
 * Control header structure for the BXM_API messages for the api
 * implementation.
 * mod_id   : Gives the id of the module.
 * opcode   : code of the requested operation.
 * Reserved : For future use.
 * Length   : Length of whole packet.
 */
typedef struct __bxmapi_ctrl_hdr {
        uint8_t mod_id;
        uint8_t opcode;
        uint16_t reserved;
        uint32_t length;
}bxmapi_ctrl_hdr;

/*
 * Contains the API TLV's .
 */
typedef struct __api_tlv {
        uint8_t type;
        uint8_t length;
        void *value;
}api_tlv;


/*
 * The response from the receive function is stored in this struct.
 * Contains the buffer, and the length.
 */
typedef struct __res_packet {
        void *data;
        int size;

}res_packet;

/*
 * This function sends the request from the client to server.
 * It also waits for the server to send response.
 */
bxm_error_t
process_request(uint32_t sockfd, uint8_t* ip_data, uint32_t ip_length,
                                res_packet *pack);

/*
 * This function creates the TCP/IP connection.
 */
bxm_error_t
create_connection(int *);

void *
cli_listener(void *arg);

void
stop_server_processes();

#endif /* BXM_COMMON_H */
