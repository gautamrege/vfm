/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#include <common.h>
#include <stdarg.h>
#include <time.h>

/* Log file descriptor */
FILE *g_logfile = NULL;

/* Default log level = 0 i.e. no logs */
int g_loglevel = 0;

/* Database pointer */
sqlite3 *g_db = NULL;

extern uint8_t g_bridge_mac[6];
extern uint8_t g_bridge_enc_mac[6];
extern uint8_t g_if_name[10];
extern uint8_t g_wwnn[8];
extern uint8_t g_wwpn[8];

#define LOGFILE "/vfm.log"
#define SQLITE3_DB "/vfm.db"

/*
 * Parse input buffer to address according to address length.
 * Mac or WWPN or WWNN address.
 * 
 * [IN] input_type : Name of address holder.
 * [IN] *buff      : Input buufer read from the file (Only one line).\
 * [IN] length     : Length of address.(Mac length = 6,WWPN and WWNN length=8).
 * [OUT]*out_buff  : Output address buffer.
 */
void 
parse_address(const char *input_type,
              uint8_t *buff,
              int length, 
              uint8_t *out_buff)
{
    int i =0,j=0;
    for(i =0; i < length*3-1; i++)
    {
        if (buff[i] == ':')
            continue;
        else if (buff[i] >= 'A' && buff[i] <= 'F')
            buff[i] -= 55;
        else if (buff[i] >= 'a' && buff[i] <= 'f')
            buff[i] -= 87;
        else if(buff[i] >= '0' && buff[i] <= '9')
            buff[i] -= 48;
        else
        {
            printf("ERROR: Incorrect (%s) specified\n",input_type);
            abort();
        }
    }
    for(i =0; i < length; i++, j+=3)
        out_buff[i] = buff[j]<<4 | buff[j+1];
}

/*
 * Parse configuration file.(vfm.config)
 *
 * Returns : vps_err
 */
vps_error
parse_configuration()
{
    /* Read a config file and populate the globals.
     * (Gautam) I agrued over having a global data struct instead of simple
     * globals because of simplicity.
     */

    vps_error err = VPS_SUCCESS;

    FILE *fp;
    uint8_t type[30];
    uint8_t value[30];
    uint8_t temp[30];
    
    if((fp = fopen("vfm.config","r"))!=NULL)
    {
       fscanf(fp, "%s %s", type, value);
       parse_address("gw_internal_mac", value, 6, g_bridge_enc_mac);

       printf("Bridge Internal mac : %0.2X:%0.2X:%0.2X:%0.2X:%0.2X:%0.2X\n",
               g_bridge_enc_mac[0], g_bridge_enc_mac[1], g_bridge_enc_mac[2],
               g_bridge_enc_mac[3], g_bridge_enc_mac[4], g_bridge_enc_mac[5]);

       fscanf(fp, "%s %s",type,value);
       parse_address("gw_mac", value, 6,g_bridge_mac);


       printf("Bridge  mac : %0.2X:%0.2X:%0.2X:%0.2X:%0.2X:%0.2X\n",
	   g_bridge_mac[0], g_bridge_mac[1], g_bridge_mac[2],
	   g_bridge_mac[3], g_bridge_mac[4], g_bridge_mac[5]);


       fscanf(fp, "%s %s", type, g_if_name);
       printf("VFM eth interface: %s\n", g_if_name);

       fscanf(fp, "%s %s", type, temp);
       g_loglevel = atoi(temp);
       printf("Log Level  : %d\n", g_loglevel);

       fscanf(fp, "%s %s",type,value);
       parse_address("wwnn", value, 8, g_wwnn);


       printf("WWNN : %0.2X:%0.2X:%0.2X:%0.2X:%0.2X:%0.2X:%0.2X:%0.2X\n",
	   g_wwnn[0], g_wwnn[1], g_wwnn[2],
           g_wwnn[3], g_wwnn[4], g_wwnn[5],
           g_wwnn[6], g_wwnn[7]);

       fscanf(fp, "%s %s",type,value);
       parse_address("wwpn", value, 8, g_wwpn);


       printf("WWPN: %0.2X:%0.2X:%0.2X:%0.2X:%0.2X:%0.2X:%0.2X:%0.2X\n",
	   g_wwpn[0], g_wwpn[1], g_wwpn[2],
           g_wwpn[3], g_wwpn[4], g_wwpn[5],
           g_wwpn[6], g_wwpn[7]);

    }
    else
    {

        /* For now, hard-code the global config */
        g_loglevel = 3;

        /* TODO: Implement file rotation so as not to get a HUGE logfile */
        g_logfile = fopen(LOGFILE, "a");

    }
    
    return err;
}

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
    rc = sqlite3_open(SQLITE3_DB, &g_db);
    if( rc ){
        vps_trace(VPS_ERROR, "Can't open database: %s", SQLITE3_DB);
        sqlite3_close(g_db);
        err = VPS_ERROR_DB_INIT;
    }

    return err;
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
    if (g_loglevel == VPS_NO_LOGS) return;

    /* If the desired level of logging is more than expected,
     * return from here. 
     * Eg. if g_loglevel = 2 (i.e warnings and errors).
     * If and VPS_ENTRYEXIT (i.e. 4) is set, then it will return from here
     * directly.
     */
    if (level > g_loglevel)
        return;


    va_start( args, format );

    /* If the log file is not specified, then log to stderr by default */
/*
    if (g_logfile)
        fp = g_logfile;
*/

    /*Get the current time*/
    curr_time = time(NULL);

    /*Format the time , "ddd mm-dd-yyyy hh:mm:ss"*/
    timestamp = localtime(&curr_time);
    strftime(time_buff, sizeof(time_buff), "%a %m-%d-%Y %H:%M:%S", timestamp);

    switch(level){

        case VPS_ERROR     : fprintf(fp, "%s :ERROR: ",time_buff );
                             break;

        case VPS_WARNING   : fprintf(fp, "%s :WARNING: ",time_buff);
                             break;

        case VPS_INFO      : fprintf(fp, "%s :INFO: ",time_buff ); 
                             break;
       
        case VPS_ENTRYEXIT : fprintf(fp, "%s :ENTRYEXIT: ",time_buff);
                             break;

    }
    
    vfprintf(fp, format, args );
    fprintf(fp, "\n" );

    va_end( args );
}
