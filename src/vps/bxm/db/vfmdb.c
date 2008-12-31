/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#include <vfmdb.h>

/* The sqlite database pointer */
extern sqlite3 *g_db;


void* vfmdb_prepare_query(const char *query, const char *fmt, void **args)
{
        int i = 0, rc = 0, length = 0;
        sqlite3_stmt *stmt = NULL;
        char *str;
        long double ld;
        int integer;

#if SQLITE_VERSION_NUMBER < 3004001
        rc = sqlite3_prepare(g_db, query, strlen(query), &stmt, NULL);
#else
        rc = sqlite3_prepare_v2(g_db, query, strlen(query), &stmt, NULL);
#endif
        if (SQLITE_OK != rc) {
                printf("SQL error: %s", sqlite3_errmsg(g_db));
                goto out;
        }

        /* If there are no positional arguments, fmt will be NULL */
        if (!fmt)
		goto out;

        while(*fmt) {

                switch(*fmt) {
                        case 'g': /* guid */
                                str = (char*)args[i];
                                length = 8;
                                break;
                        case 'm': /* mac */
                                str = (char*)args[i];
                                length = 6;
                                break;
                        case 's': /* NULL terminated string */
                                str = (char*)args[i];
                                length = strlen(str);
                                break;
                        /* TODO: Future enhancements */
                        /*
                        case 'd': // integer
                                integer = va_arg(args, int);
                                str = (char*)&integer;
                                length = sizeof(int);
                                break;
                        case 'l': // long double
                                ld = va_arg(args, long double);
                                str = (char*)&ld;
                                length = sizeof(long double);
                                break;
                        */
                        default:
                                printf("Format error: %c", *fmt);
				goto out;
                }

                /* increment i since positional paramters start from 1 */
                rc = sqlite3_bind_text(stmt, i+1, str, length, SQLITE_STATIC);
                if (rc != SQLITE_OK) {
                        printf("SQL error: %s", sqlite3_errmsg(g_db));
			sqlite3_finalize(stmt);
			stmt = NULL;
                        goto out;
                }
                *fmt++;
                i++;
        }
out:
        return stmt;
}

/* API to prepare a query with positional paramters. If there is 
 * no need of any positional paramters, fmt can be NULL
 */
void* vfmdb_prepare_query_ex(const char *query, const char *fmt, ...)
{
        int i = 0;
        va_list args;
        sqlite3_stmt *stmt;
        void **nargs;

        if (!fmt)
                return vfmdb_prepare_query(query, NULL, NULL);

        va_start(args, fmt);

        nargs = (void**)malloc(strlen(fmt) * sizeof(void*));
        /*
         * Do not increment fmt here, since it is used inside 
         * vfmdb_prepare_query_ex
         */
        while(*(fmt+i)) {
                nargs[i] = va_arg(args, char*);
                i++;
        }
        va_end(args);

        stmt = vfmdb_prepare_query(query, fmt,nargs);
        free(nargs);
        return stmt;
}

int
process_row(sqlite3_stmt *stmt,
                int (*sql_cb)(void*, int, uint8_t**, char**),
                void *rsc)
{
        int err = 0, i = 0, len;
        int ncols = sqlite3_column_count(stmt);
        char **columns;
        uint8_t **values;
        const uint8_t *tmp;

        columns = (char**)malloc(ncols * sizeof(char*));
        values = (uint8_t**)malloc(ncols * sizeof(uint8_t*));

        /* Populate column names and values */
        for (i = 0; i < ncols; i++) {
                columns[i] = (char*)malloc(128);
                strcpy(columns[i], sqlite3_column_name(stmt, i));

                /*
                 * sqlite3_column_text should be called before 
                 * sqlite3_column_bytes, as the lenght may change depending
                 * on the encoding.
                 */
                tmp = sqlite3_column_text(stmt, i);
                len = sqlite3_column_bytes(stmt, i);

                values[i] = (char*)malloc(len+1);
                memset(values[i], 0, len+1);
                memcpy(values[i], tmp, len);
        }

        /* invoke the callback */
        if (0 != sql_cb(rsc, ncols, values, columns)) {
                printf("Callback failed.");
                err = 1; /* SQLITE Error code??? */
        }

        /* Clean up resources */
        for (i = 0; i < ncols; i++) {
                free(values[i]);
                free(columns[i]);
        }
        free(values);
        free(columns);

        return err;
}

/* API for executing the query. If there is no response processing
 * expected, set callback as NULL
 */
vps_error vfmdb_execute_query(void *stmt,
                int (*sql_cb)(void*, int, uint8_t**, char**),
                void *arg)
{
        int rc = 0;
        int process = 1;
        while (process) {
                rc = sqlite3_step(stmt); // Execute the statement.

                switch (rc) {
                        case SQLITE_DONE:
                                process = 0;
				rc = 0; /* success */
                                break;
                        case SQLITE_ROW:
                                process_row(stmt, sql_cb, arg);
                                break;
                        default:
                                //  SQLITE_BUSY:  reset? to try again?
                                printf("sqlite error in switch:%s\n",
                                                sqlite3_errmsg(g_db));
                                process = 0;
                                break;
                }
        }
        sqlite3_finalize(stmt);
        stmt = NULL;
        return rc;
}
