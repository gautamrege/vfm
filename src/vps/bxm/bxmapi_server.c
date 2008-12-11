/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */

#include <bxmapi_common.h>
#include <common.h>

#define MAX_CLIENT 50

int s_socket_id = 0;

/*
 * This function receives the message from the client. It is in the form of the
 * TLVs and then calls the unmarshall_request to process it.
 */
void *
process_client_request(void *argv)
{
        char buff[BUFFER_SIZE];
        int sock_fd = *((int *)argv);
        int recv_size = 0;
        res_packet pack;

        vps_trace(VPS_ENTRYEXIT, "Entering process_client_request");

        while (1) {
                /* get the message from the client*/
                if ((recv_size = recv(sock_fd, buff, sizeof(buff), 0)) > 0)
                        vps_trace(VPS_INFO, "Client Message : %s \n" , buff);
                else
                        return NULL;
                sleep(5);
                /*API Call */
                unmarshall_request(buff, recv_size, &pack);

                if (pack.size > 0) {
                        if (send(sock_fd, pack.data, pack.size, 0) > 0)
                               vps_trace(VPS_INFO, "Message sent to client");
                        else
                                return NULL;
                }
                free(pack.data);
                pack.size = 0;
        }
        vps_trace(VPS_ENTRYEXIT, "Leaving process_client_request");
}

/*
 * This function sends the message to the Client
 * SENDING DATA (NOT USING IT PRESENTLY)
 */
bxm_error_t
send_data(int sock_id, uint8_t* op_data, uint32_t op_length) {
        /* send the confirmation to client*/
        vps_error err = VPS_SUCCESS;

        if (send(sock_id, op_data, op_length, 0) > 0)
                vps_trace(VPS_INFO, "Message sent to client");
        else
                return err;
}



int
accept_req(int sock_id)
{
        struct sockaddr_in c_addr_info;
        int id;
        int addr_len = sizeof(struct sockaddr_in);
        /* accept the client request */
        if ((id = accept(sock_id, ((struct sockaddr *)&c_addr_info),
                                &addr_len)) == -1) {
               vps_trace(VPS_INFO, "Error in accepting the client request");
        }
        return id;
}


void
stop_server_processes()
{
        vps_trace(VPS_INFO, "Server is shutting down...");
        close(s_socket_id);
        s_socket_id = -1;
}


/*
 * This is the Listener thread that listens to the Request from the Clients.
 * When it receives a request, it creates a socket and binds the a thread to it
 * This thread then interacts with the clients for the further message transfer
 * The server continues to listen for other Client requests.
 */
void *
cli_listener(void *arg)
{
        int i;
        struct sockaddr_in s_addr_info;
        pthread_t req_tid[MAX_CLIENT];
        int client_no = 0;
        int c_sockfd[MAX_CLIENT];

        int val = 1;

        vps_trace(VPS_ENTRYEXIT, "Entering cli_listener");

        if ((s_socket_id = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
                vps_trace(VPS_INFO, "Error in creating a socket");
        }

        if (setsockopt(s_socket_id, SOL_SOCKET, SO_REUSEADDR, &val,
                                                        sizeof(val)) < 0){

                vps_trace(VPS_INFO, "Error in reuse socket");
                return;
        }


        vps_trace(VPS_INFO, "socket created at %d \n",  s_socket_id);

        s_addr_info.sin_family = AF_INET;
        s_addr_info.sin_port = htons(SERVER_PORT);
        s_addr_info.sin_addr.s_addr = INADDR_ANY;
        /* zero the rest of the memory */
        memset(&(s_addr_info.sin_zero), 0, sizeof(s_addr_info.sin_zero));

        if (bind(s_socket_id, (struct sockaddr *)&s_addr_info,
                                sizeof(struct sockaddr_in)) == -1) {
                vps_trace(VPS_INFO, "Error in the binding the socket");
                close(s_socket_id);
                return;
        }

        while (1) {

                if (s_socket_id == -1)
                   goto out;

                if (listen(s_socket_id, MAX_CLIENT) != -1) {
                        vps_trace(VPS_INFO, "listening.........");
                        if ((c_sockfd[client_no] = accept_req(s_socket_id)
                                                ) != -1) {

                                printf("Client request [%d] : %d \n" ,
                                           client_no, c_sockfd[client_no]);
                                /* create a thread per client */
                                pthread_create(&req_tid[client_no], NULL,
                                            process_client_request,
                                            ((void *)&c_sockfd[client_no]));

                                client_no++;
                        }
                }
                else {
                       goto out;
                }
        }

out:

        vps_trace(VPS_INFO, "Server Thread exit");

        close(s_socket_id);

        for (i = 0; i < MAX_CLIENT; i++) {
                pthread_join(req_tid[i], NULL);
        }

        vps_trace(VPS_ENTRYEXIT, "Leaving cli_listener");
        return 0;
}

