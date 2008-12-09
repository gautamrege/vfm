/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 *
 * The is the VFM server which has the following functionalities:
 * - runs as a daemon process
 * - sniffs FIP control packets for solicitations / heartbeat
 * - sends periodic advertisements for virtual interfaces
 * - sends vFabric udpates
 *
 * It also contains basic miscellaneous code for:
 * - reading from a config file
 * - logging debug statements and various trace-levels etc.
 */
#include <common.h>
#include <vfm_fip.h>
#include <db_access.h>
#include <crc32_le.h>
#include <map_util.h>
#include <bxm_ib.h>
#include <config.h>

extern sqlite3 *g_db;
extern FILE *g_logfile;
extern req_entry_map* g_req_map[REQ_ENTRY_MAP_LEN];


int  g_bxm_local;
int  g_bxm_remote;
uint8_t  g_bxm_protocol;
/*TODO : Remove after SW GW able to advertise its GW ports*/
uint8_t  g_local_mac[MAC_ADDR_LEN];
uint32_t g_if_index;
uint32_t g_if_mtu;
uint8_t  g_if_name[10];

/* For IB  */
bxm_init_info_packet init_info_pack;

/*
 * Pre-Initialization
 *  This function does the following:
 * - Read the configuration file.
 * - Configures / creates the sqlite database
 *
 * reserved - (IN) for future use
 *
 * Returns: 0 on success
 *                  non-zero error codes
 */
vps_error
pre_initialization(void * reserved)
{
        vps_error err  = VPS_SUCCESS;

        parse_configuration();
        configure_database();

        memset(g_req_map, 0, sizeof(g_req_map));
        vps_trace(VPS_INFO, "Configuration setup");
        return err;
}

/*
 * Initialization
 * This functions spawns the following threads.
 * - FIP packet sniffer.
 * - validator thread
 * - command thread (listening on a local pipe for server control commands).
 * - sync thread (which maintains a sync between master and slave VFM).(future)
 *
 * Returns: 0 on success
 */
vps_error initialization(void *reserved)
{
        vps_error err  = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering Initialization");

        if(g_bxm_protocol == BXM_EN_PROTOCOL)
        {
                /*TODO:Get interface name from *reserved */
                if ((err = get_inf_property(g_if_name, g_local_mac, 
                &g_if_index, &g_if_mtu)) != VPS_SUCCESS) {
                        vps_trace(VPS_ERROR,
                          "Unable to get local mac, interface index, mtu");
                }

                vps_trace(VPS_INFO, 
                            "local_mac: %0.2X:%0.2X:%0.2X:%0.2X:%0.2X:%0.2X\n"
                                "if_index: %d \n  mtu: %d",
                                g_local_mac[0], g_local_mac[1], g_local_mac[2],
                                g_local_mac[3], g_local_mac[4], g_local_mac[5],
                                g_if_index, g_if_mtu);
        }
        else if (g_bxm_protocol == BXM_IB_PROTOCOL) {
                /* Call BridgeX API init()*/
                bx_init(&init_info_pack);
                vps_trace(VPS_INFO,"SLID : %d", init_info_pack.lid);
                vps_trace(VPS_INFO,"SQP  : %d", init_info_pack.qp);
                vps_trace(VPS_INFO,"SGID : %0.2X:%0.2X:%0.2X:%0.2X:%0.2X:\
                %0.2X: %0.2X:%0.2X",
                init_info_pack.gid[0] ,init_info_pack.gid[1],
                init_info_pack.gid[2], init_info_pack.gid[3], 
                init_info_pack.gid[4], init_info_pack.gid[5],
                init_info_pack.gid[6], init_info_pack.gid[7]);

                if (init_info_pack.lid == 0){
                        vps_trace(VPS_ERROR,"SLID is Zero");
                }


        }
        vps_trace(VPS_ENTRYEXIT, "Exiting Initialization");
        return err;
}

/*
 * Post-Initialization
 * For future use. Always returs 1
 *
 * Returns: 0 on success
 *          non-zero error codes
 */
vps_error
post_initialization(void *reserved)
{
        vps_error err  = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering Post-initialization");

        vps_trace(VPS_ENTRYEXIT, "Exiting Post-initialization");
        return err;
}

/*
 * pre_shutdown
 * for future use.
 */
void
pre_shutdown(void *reserved)
{
        pthread_join(*(pthread_t*)reserved, NULL);
}

/*
 * pre_shutdown
 * This functions send a 'stop' signal to the threads, so that they can
 * cleanly shutdown.
 */
void
vps_shutdown(void *reserved)
{
}

/*
 * post_shutdown
 * This function closes the database and file handles (if any). This function
 * also syncs the database to the slave VFM (if needed).
 */
void
post_shutdown(void *reserved)
{
        fclose(g_logfile);
        sqlite3_close(g_db);
}


/*
 * process_options
 * If the server is NOT to be started (i.e. command line options are
 * specified), then this functions returns, otherwise it exits with the
 * return code: 0 for success and non-zero for errors
 *
 * argc, argv - (IN) the command line options if any
 *
 * returns void
 */
void
parse_options(int argc, char* argv[])
{
        /*
         * Parse command line options:
         * -v | --version  : version: <Major>.<Minor>-<build-number>. eg 0.1-10
         * -s | --shutdown : shutdown the server cleanly. (no force shutdown)
         * -h | --help :
         */
}

int
main(int argc, char* argv[])
{
        vps_error err = VPS_SUCCESS;
        pthread_t sniffer_tid;
        pthread_t lbc_sniffer_tid;
        pthread_t process_packet_tid1;
        pthread_t process_packet_tid2;
        pthread_t cli_listener_tid;

        /* Process Command line options */
        parse_options(argc, argv);

        /* Pre-Initialization Process */
        if (VPS_SUCCESS != pre_initialization(NULL)) {
                vps_trace(VPS_ERROR, "Pre-initialization failed");
                goto out;
        }

        /* Initialization Process */
        if (VPS_SUCCESS != initialization(NULL)) {
                vps_trace(VPS_ERROR, "Initialization failed");
                goto out;
        }

        /* Post-Initialization Process */
        if (VPS_SUCCESS != post_initialization(NULL)) {
                vps_trace(VPS_ERROR, "Post-initialization failed");
                goto out;
        }


#ifdef DB_TEST
        test_update_bridge(); /* update bridge to DB */
        get_bridge_info();    /* Get all Bridge information */
        get_all_gw();         /* Get all GW information */
        exit(0);
#endif
        /* Start packet processing thread */
        pthread_create(&process_packet_tid1, NULL, start_processor, NULL);
        pthread_create(&process_packet_tid2, NULL, start_processor, NULL);
        pthread_create(&cli_listener_tid, NULL, cli_listener, NULL);


        if (g_bxm_remote == 1 && g_bxm_protocol == BXM_EN_PROTOCOL) {

                /* Loop indefinitely checking for shutdown flag */
                pthread_create(&sniffer_tid, NULL, start_sniffer, NULL);

                /* Sleep for 1 second, for sniffer to start properly */
                sleep(1);

                /* Sending VFM FLOGI */
                vps_trace(VPS_ERROR, "--*** SENDING VFM FLOGI***--");
                create_vfm_flogi();

                /* TODO: Remove this after SWGW demo */
                /* Send gateway Advertisement */
                send_gw_ad();

        }
        else if (g_bxm_local == 1 && g_bxm_protocol == BXM_IB_PROTOCOL) {
               
                /* Send GW advertisement */ 
                bxm_send_gw_adv();
                
                /* Start Processor thread. */
                pthread_create(&lbc_sniffer_tid, NULL, start_lbc_sniffer, 
                                                                      NULL);

        }

#ifdef TEST
        packet_process();
#endif /* TEST */
        while (1) {
                sleep(60000);
        }

        /* Pre-Shutdown Process */
        pre_shutdown(&sniffer_tid);
        pthread_join(lbc_sniffer_tid, NULL);

        pthread_join(process_packet_tid1, NULL);
        pthread_join(process_packet_tid2, NULL);
        pthread_join(cli_listener_tid, NULL);
        /* Shutdown Process */
out:
        return err;
}

