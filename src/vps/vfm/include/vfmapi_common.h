#ifndef VFM_COMMON_H
#define VFM_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <vfm_error.h>
#include <vfm_vadapter.h>
#include <vfm_vfabric.h>
#include <vfmdb.h>
#include <vfm_bridge_device.h>
/*
 * TODO: Header files to be included later
 * #include <vfm_common.h>
 * #include <vfm_gateway.h>
 * #include <vfm_port.h>
 * #include <vfm_table.h>
 * #include <vfm_state.h>
 * #include <vfm_buf.h>
 */

#define SERVER_PORT             10006
#define MAXBUFFLEN              500
#define BUFFER_SIZE             8192 /* TODO: Refer to bug# 56 */

/* Defining the Modules */
#define VFMAPI_IOMODULE         0x1
#define VFMAPI_GATEWAY          0x2
#define VFMAPI_VADAPTER         0x3
#define VFMAPI_BRIDGE_DEVICE    0x4
#define VFMAPI_VFABRIC          0x5

/* Defining the Operations */
#define VFM_CREATE              0x1
#define VFM_EDIT                0x2
#define VFM_QUERY               0x3
#define VFM_QUERY_INVENTORY     0x4
#define VFM_DESTROY             0x5
#define VFM_ONLINE              0x6
#define VFM_EDIT_PROTOCOL_ATTR  0x7
#define VFM_ERROR               0x8     
#define VFM_VA_ACTIVATE         0x11

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
#define Q_NAMED_PARAM           0x04 

/* Max number of arguments for the query */
#define MAX_ARGS                10

#define NAME_SIZE  sizeof(char[64])              
/* The size of Type & Length of TLV */
#define TLV_SIZE  2 * sizeof(uint32_t)
/*
 * Control header structure for the VFM_API messages for the api
 * implementation.
 * mod_id   : Gives the id of the module.
 * opcode   : code of the requested operation.
 * Reserved : For future use.
 * Length   : Length of whole packet.
 */
typedef struct __vfmapi_ctrl_hdr {
        uint8_t mod_id;
        uint8_t opcode;
        uint16_t reserved;
        uint32_t length;
}vfmapi_ctrl_hdr;

/*
 * Contains the API TLV's .
 * 32 bit length : Can go upto 4GB
 */
typedef struct __api_tlv {
        uint32_t type;
        uint32_t length;
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
vfm_error_t
process_request(uint32_t sockfd, uint8_t* ip_data, uint32_t ip_length,
                                res_packet *pack);

/*
 * This function creates the TCP/IP connection.
 */
vfm_error_t
create_connection(int *);

void *
cli_listener(void *arg);

void
stop_server_processes();

#endif /* VFM_COMMON_H */
