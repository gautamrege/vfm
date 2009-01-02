#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <sqlite3.h>

sqlite3 *g_db = NULL;


void* vfmdb_prepare_query(const char *query, const char *fmt, void **args)
{
        int i = 0, rc = 0, length = 0;
        sqlite3_stmt *stmt;
        char *str;

        rc = sqlite3_prepare(g_db, query, strlen(query), &stmt, NULL);
        if (SQLITE_OK != rc) {
                printf("SQL error: %s", sqlite3_errmsg(g_db));
                return NULL;
        }

        /* If there are no positional arguments, fmt will be NULL */
        if (!fmt)
                return stmt;

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
                                return stmt;
                }

                /* increment i since positional paramters start from 1 */
                rc = sqlite3_bind_text(stmt, i+1, str, length, SQLITE_STATIC);
                if (rc != SQLITE_OK) {
                        printf("SQL error: %s\n", sqlite3_errmsg(g_db));
                        return NULL;
                }
                *fmt++;
                i++;
        }
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
int vfmdb_execute_query(void *stmt,
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
                                break;
                        case SQLITE_ROW:
                                process_row(stmt, sql_cb, arg);
                                break;
                        default:
                                //  SQLITE_BUSY:  reset? to try again?
                                printf("sqlite error in switch:%s\n",
                                                sqlite3_errmsg(g_db));
                                process = 0;
                                rc = 1;
                                break;
                }
        }
        sqlite3_finalize(stmt);
        return rc;
}

test_insert_iomodule(int id)
{
        int ret;
        void *stmt;
        void **args;

        uint8_t host_mac[8] = {0x00, 0x30, 0x48, 0x68, 0x40, 0x75};
        char query[1024];

        args = (void**)malloc(sizeof(void*));
        args[0] = host_mac;

        sprintf(query, "insert into bxm_io_module_attr (id, name, "
                       "type, mac, num_vhba, num_vnic) values (%d, "
                       "'iomodul1', 1, ?1, 0, 0);", id);
        stmt = vfmdb_prepare_query(query, "m", args);
        if (!stmt) {
                printf("ERROR: Cannot prepare sqlite3 statement\n");
                return;
        }

        ret = vfmdb_execute_query(stmt, NULL, NULL);
        if (!ret) {
                printf("ERROR: Sqlite3 statement execution failed.\n");
        }
}

test_insert_gw_module(int id)
{
        int ret;
        void *stmt;
        void **args;

        uint8_t bridge_mac[8] = {0x00, 0x30, 0x48, 0x68, 0xB3, 0xDE, 0x00, 0x00};
        char query[1024];

        args = (void**)malloc(sizeof(void*));
        args[0] = bridge_mac;

        sprintf(query, "insert into bxm_gw_module_attr (gw_module_id,"
                       "bxm_bridge_guid) values(%d, ?1);", id);
        stmt = vfmdb_prepare_query(query, "g", args);
        if (!stmt) {
                printf("ERROR: Cannot prepare sqlite3 statement\n");
                return;
        }

        ret = vfmdb_execute_query(stmt, NULL, NULL);
        if (!ret) {
                printf("ERROR: Sqlite3 statement execution failed.\n");
        }
}

test_insert_gw(gwid)
{
        int ret;
        void *stmt;
        void **args;

        uint8_t bridge_guid[8] = {0x00, 0x30, 0x48, 0x68, 0xB3, 0xDE, 0x00, 0x00};
        char query[1024];

        args = (void**)malloc(sizeof(void*));
        args[0] = bridge_guid;

        sprintf(query, "insert into bxm_gateway_attr (gw_id, bxm_guid,"
                       "physical_index, desc, int_protocol, ext_protocol, "
                       "int_port, ext_port, "
                       "ingress_learning, ingress_secure, egress_secure, "
                       "flood, checksum_offload, component_mask) values ("
                       "%d, ?1, 'Gw1', 'Desc 1', 1, 1, 1, 2, 1, 1, 1, 1, 1, 1);",
                       gwid);
        stmt = vfmdb_prepare_query(query, "g", args);
        if (!stmt) {
                printf("ERROR: Cannot prepare sqlite3 statement\n");
                return;
        }

        ret = vfmdb_execute_query(stmt, NULL, NULL);
        if (!ret) {
                printf("ERROR: Sqlite3 statement execution failed.\n");
        }
}

test_insert_bridge()
{
        int ret;
        void *stmt;
        void **args;

        /* gw1 MAC */
        uint8_t guid[8] = {0x00, 0x30, 0x48, 0x68, 0xB3, 0xDE, 0x00, 0x00};

        /* bc2 MAC */
        uint8_t bxm_guid[8] = {0x00, 0x30, 0x48, 0x77, 0x67, 0xEE, 0x00, 0x00};

        char *query = "insert into bxm_bridge_device values(?1, ?2, ?3, ?4, ?5);";

        args = (void**)malloc(5 * sizeof(void*));
        args[0] = guid;
        args[1] = bxm_guid;
        args[2] = "bridge1";
        args[3] = "gw1 bridge";
        args[4] = "v1.0";

        stmt = vfmdb_prepare_query(query, "ggsss", args);
        if (!stmt) {
                printf("ERROR: Cannot prepare sqlite3 statement\n");
                return;
        }

        ret = vfmdb_execute_query(stmt, NULL, NULL);
        if (!ret) {
                printf("ERROR: Sqlite3 statement execution failed.\n");
        }
}

test_update_db()
{
        int ret;
        void *stmt;
        /* Dummy parameters */

        uint8_t guid[8] = { 0xFF, 0x10, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
        uint8_t bxm_guid[8] = { 0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7};

        char *query = "insert into bxm_bridge_device values(?1, ?2, ?3, ?4, ?5);";

        stmt = vfmdb_prepare_query_ex(query, "ggsss", guid, bxm_guid, "Bridge",
                        "Bridge1 Desc", "v1.1");
        if (!stmt) {
                printf("ERROR: Cannot prepare sqlite3 statement\n");
                return;
        }

        ret = vfmdb_execute_query(stmt, NULL, NULL);
        if (!ret) {
                printf("ERROR: Sqlite3 statement execution failed.\n");
        }
}

int print_bridge(void* arg, int nCols, uint8_t** values, char **cols)
{
        int i;
        for (i = 0; i < nCols; i++) {
                if (strcmp(cols[i], "guid") == 0) {
                        printf("guid : %0.2X:%0.2X:%0.2X:%0.2X:%0.2X:"
                               "%0.2X:%0.2X:%0.2X\n",
                               *values[i],     *(values[i]+1),
                               *(values[i]+2), *(values[i]+3), 
                               *(values[i]+4), *(values[i]+5),
                               *(values[i]+6), *(values[i]+7));
                }
                else if (strcmp(cols[i], "bxm_guid") == 0) {
                        printf("bxm_guid : %0.2X:%0.2X:%0.2X:%0.2X:%0.2X:"
                               "%0.2X:%0.2X:%0.2X\n",
                               *values[i],     *(values[i]+1),
                               *(values[i]+2), *(values[i]+3), 
                               *(values[i]+4), *(values[i]+5),
                               *(values[i]+6), *(values[i]+7));
                }
                else if (strcmp(cols[i], "desc") == 0) {
                        printf("desc:%s\n", values[i]);
                }
                else if (strcmp(cols[i], "name") == 0) {
                        printf("name:%s\n", values[i]);
                }
                else if (strcmp(cols[i], "firmware_version") == 0) {
                        printf("fw version:%s\n", values[i]);
                }
        }
        return 0;
}

void test_read_bridge()
{
        int ret;
        void *stmt;
        /* Dummy parameters */
        char *query = "select * from bxm_bridge_device;";

        stmt = vfmdb_prepare_query(query, NULL, NULL);
        if (!stmt) {
                printf("ERROR: Cannot prepare sqlite3 statement\n");
                return;
        }

        ret = vfmdb_execute_query(stmt, print_bridge, NULL);
        if (!ret) {
                printf("ERROR: Sqlite3 statement execution failed.\n");
        }
}

int print_io_module(void* arg, int nCols, uint8_t** values, char **cols)
{
        int i;
        for (i = 0; i < nCols; i++) {
                if (strcmp(cols[i], "mac") == 0) {
                        printf("mac : %0.2X:%0.2X:%0.2X:%0.2X:%0.2X:%0.2X\n",
                               *values[i],     *(values[i]+1),
                               *(values[i]+2), *(values[i]+3), 
                               *(values[i]+4), *(values[i]+5));
                }
        }
        return 0;
}

void test_read_io_module()
{
        int ret;
        void *stmt;
        /* Dummy parameters */
        char *query = "select * from bxm_io_module_attr;";

        stmt = vfmdb_prepare_query(query, NULL, NULL);
        if (!stmt) {
                printf("ERROR: Cannot prepare sqlite3 statement\n");
                return;
        }

        ret = vfmdb_execute_query(stmt, print_io_module, NULL);
        if (!ret) {
                printf("ERROR: Sqlite3 statement execution failed.\n");
        }
}

void main()
{
        int rc;

        /* initialize DB */
        rc = sqlite3_open("db/vfm.db", &g_db);
        if (rc) {
                printf("Can't open database: %s\n", sqlite3_errmsg(g_db));
                sqlite3_close(g_db);
                exit(1);
        }

        /*
        printf("Insert IOModule..\n");
        //test_insert_iomodule(1);
        //exit(1);

        printf("Inserting bridge..\n");
        test_insert_bridge();

        printf("Insert Gateway Modules..\n");
        test_insert_gw_module(1);
        test_insert_gw_module(2);

        printf("Insert Gateway..\n");
        test_insert_gw(1122);

        printf("Testing test_read_bridge\n");
        test_read_bridge();
        */

        printf("Testing test_read_io_module\n");
        test_read_io_module();
}
