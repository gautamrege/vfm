/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#include <common.h>
#include <stdarg.h>
#include <time.h>
#include <config.h>

/* Log file descriptor */
FILE *g_logfile = NULL;

/* Default log level = 0 i.e. no logs */
int g_loglevel = 0;

/* Database pointer */
sqlite3 *g_db = NULL;
char g_db_path[100];

char g_log_path[100];

extern int g_vfm_local;
extern int g_vfm_remote;
extern uint8_t g_vfm_protocol;
extern uint8_t g_bridge_mac[6];
extern uint8_t g_bridge_enc_mac[6];
extern uint8_t g_if_name[10];
extern uint8_t g_wwnn[8];
extern uint8_t g_wwpn[8];



/*
 * Parse configuration file.(vfm.config)
 *
 * Returns : vps_err
 */
/*
 * Configure database.
 *
 * Returns : vps_err
 *           database initialization error.
 */
vps_error
configure_database()
{
        int rc;
        vps_error err = VPS_SUCCESS;

        /* This will create the database if its does not exist */
        rc = sqlite3_open(g_db_path, &g_db);
        if (rc) {
                printf("Can't open database: %s\n", g_db_path);
                vps_trace(VPS_ERROR, "Can't open database: %s", g_db_path);
                err = VPS_ERROR_DB_INIT;
        }

        return err;
}

/* Initialization of Log File*/
void
init_log()
{
        time_t curr_time;
        struct tm *timestamp;
        char time_buff[30];

        /*Get the current time*/
        curr_time = time(NULL);

        /*Format the time , "ddd * mm-dd-yyyy hh:mm:ss"*/
        timestamp = localtime(&curr_time);
        strftime(time_buff, sizeof(time_buff), "%a %m-%d-%Y %H:%M:%S",
                        timestamp);

        printf("\t LOG Start Date : %s \n ", time_buff);
        if(strlen(g_log_path) > 0 )
        {
                g_logfile = fopen(g_log_path,"a");
                if(g_logfile)
                {
                        fprintf(g_logfile, "\t==========================="
                                        "===========================\n\n");
                        fprintf(g_logfile, "\t LOG Start Date : %s %\n\n ",
                                        time_buff);
                        fprintf(g_logfile, "\t==========================="
                                        "============================\n\n"); } 
                else
                                printf("Unable to open log file\n");
        }

}

/* Close Log file */
void
close_log()
{
        if(g_logfile)
                fclose(g_logfile);
}



void
vps_trace(int level, const char* format, ...)
{
        FILE *fp = stderr;
        va_list args;

        time_t curr_time;
        struct tm *timestamp;
        char time_buff[30];

        /* Ignore logging if log level is not set */
        if (g_loglevel == VPS_NO_LOGS)
                return;

        /*
         * If the desired level of logging is more than expected,
         * return from here.
         * Eg. if g_loglevel = 2 (i.e warnings and errors).
         * If and VPS_ENTRYEXIT (i.e. 4) is set, then it will return from here
         * directly.
         */
        if (level > g_loglevel)
                return;

        va_start(args, format);

        /* If the log file is not specified, then log to stderr by default */
        if (g_logfile)
                fp = g_logfile;


        /*Get the current time*/
        curr_time = time(NULL);

        /*Format the time , "ddd mm-dd-yyyy hh:mm:ss"*/
        timestamp = localtime(&curr_time);
        strftime(time_buff, sizeof(time_buff), "%a %m-%d-%Y %H:%M:%S",
                        timestamp);

        switch (level) {
                case VPS_ERROR:
                        fprintf(fp, "%s :ERROR: ", time_buff);
                        break;

                case VPS_WARNING:
                        fprintf(fp, "%s :WARNING: ", time_buff);
                        break;

                case VPS_INFO:
                        fprintf(fp, "%s :INFO: ", time_buff);
                        break;

                case VPS_ENTRYEXIT:
                        fprintf(fp, "%s :ENTRYEXIT: ", time_buff);
                        break;
        }

        vfprintf(fp, format, args);
        fprintf(fp, "\n");

        va_end(args);
}
