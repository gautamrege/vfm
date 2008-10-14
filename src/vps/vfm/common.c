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

#define LOGFILE "/vfm.log"
#define SQLITE3_DB "/vfm.db"

int
parse_configuration()
{
    /* Read a config file and populate the globals.
     * (Gautam) I agrued over having a global data struct instead of simple
     * globals because of simplicity.
     */

    /* For now, hard-code the global config */
    g_loglevel = 3;

    /* TODO: Implement file rotation so as not to get a HUGE logfile */
    g_logfile = fopen(LOGFILE, "a");
}

int
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
