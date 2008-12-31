/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */

#include<bxmapi_common.h>

#ifdef BXM_TEST
uint8_t server_addr[4] = {192, 168, 1, 100};
#endif

static int sockfd = -1;
/*
 * This function will send the data to the server and recieve the data
 * [IN]  sockfd     : socket id of the client
 * [IN]  *ip_data   : input data
 * [IN]  ip_length  : size of the input data
 * [OUT] *pack      : output struct which contains data and size.
 */
bxm_error_t
process_request(uint32_t sockfd, uint8_t* ip_data, uint32_t ip_length,
                res_packet *pack)
{
        pack->size = 0;
        uint8_t op_data[BUFFER_SIZE];

        memset(op_data, 0, BUFFER_SIZE);
        bxm_error_t err = BXM_SUCCESS;

        /*
         * Send the request to the client.
         */
        if (send(sockfd, ip_data, ip_length, 0) <= 0) {
                err = BXM_ERROR_SENDING;
                return err;
        }
        printf("\n sending data....\n");
	free(ip_data);
        /* recieve the message from server */
        if ((pack->size = recv(sockfd, op_data, BUFFER_SIZE, 0)) < 0) {
                err = BXM_ERROR_RECEIVE;
                printf("\n Error in receiving the data.. \n");
                return err;
        }

       pack->data = malloc(pack->size);
       memcpy(pack->data, op_data, pack->size);

       return err;
}

/*
 * This function will create a socket and establish a connection with
 * the server. If it is already created then return the socket fd.
 *
 * [IN/OUT] *sock_id : socket id of the connection.
 */
bxm_error_t
create_connection(int *sock_id)
{
        struct sockaddr_in add_info;
        char buff[BUFFER_SIZE];
        int count = 0;
        bxm_error_t err = BXM_SUCCESS;

        /* If the socket has already been created, return it */
        if (sockfd != -1) {
                *sock_id = sockfd;
                return err;
        }


        memset(buff, 0, BUFFER_SIZE);
        /* create a socket to connect to server */
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
                printf("Error in creating the socket \n");
                err = BXM_ERROR_CONNECT;
                return err;

        }
        add_info.sin_family = AF_INET;
        add_info.sin_port = htons(SERVER_PORT);
        add_info.sin_addr.s_addr = INADDR_ANY;

        /*
         * TODO: SERVER_PORT and ADDRESS should be configurable
         * memcpy(&add_info.sin_addr.s_addr, server_addr , 4);
         * inet_pton(AF_INET, "192.168.1.100", &(add_info.sin_aoddr.s_addr));
         */
        memset(&(add_info.sin_zero), '\0', 8);

        /* connect to the server using the socket descriptor */
        if (connect(sockfd, (struct sockaddr *)&add_info,
                             sizeof(struct sockaddr_in)) == -1) {
                printf("Error in connecting the server \n");
                err = BXM_ERROR_CONNECT;
                return err;
        }

        *sock_id = sockfd;

        return err;
}

