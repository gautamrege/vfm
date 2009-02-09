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

        sprintf(query, "insert into vfm_io_module_attr (id, name, "
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


test_insert_gw(gwid)
{
        int ret;
        void *stmt;
        void **args;

        uint8_t bridge_guid[8] = {0x00, 0x30, 0x48, 0x68, 0xB3, 0xDE, 0x00, 0x00};
        char query[1024];

        args = (void**)malloc(sizeof(void*));
        args[0] = bridge_guid;

        sprintf(query, "insert into vfm_gateway_attr (gw_id, vfm_guid,"
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

test_insert_bridge(uint8_t *guid)
{
        int ret;
        void *stmt;
        void **args;

        /* bc2 MAC */
        uint8_t vfm_guid[8] = {0x01, 0x30, 0x48, 0x77, 0x67, 0xEE, 0x00, 0x00};

        char *query = "insert into vfm_bridge_device values(?1, ?2, ?3, ?4, ?5);";

        args = (void**)malloc(5 * sizeof(void*));
        args[0] = guid;
        args[1] = vfm_guid;
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
        free(args);
}

void 
test_insert_gw_module(uint8_t *bd_guid, uint32_t gw_id)
{
        int ret;
        void *stmt;
        void **args;
        char gw_query[512];

        sprintf(gw_query,"insert into vfm_gw_module_attr values(%d,?1);",
                                                                        gw_id);
        args = (void**)malloc(sizeof(void*));
        args[0] = bd_guid;

        stmt = vfmdb_prepare_query(gw_query, "g", args);

        if (!stmt) {
                printf("ERROR: Cannot prepare sqlite3 statement\n");
                return;
        } 
        ret = vfmdb_execute_query(stmt, NULL, NULL);
        if (!ret) {
                printf("ERROR: Sqlite3 statement execution failed.\n");
        }

        free(args);
}

test_update_db()
{
        int ret;
        void *stmt;
        /* Dummy parameters */

        uint8_t guid[8] = { 0xFF, 0x10, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
        uint8_t vfm_guid[8] = { 0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7};

        char *query = "insert into vfm_bridge_device values(?1, ?2, ?3, ?4, ?5);";

        stmt = vfmdb_prepare_query_ex(query, "ggsss", guid, vfm_guid, "Bridge",
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
                else if (strcmp(cols[i], "vfm_guid") == 0) {
                        printf("vfm_guid : %0.2X:%0.2X:%0.2X:%0.2X:%0.2X:"
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
        char *query = "select * from vfm_bridge_device;";

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

int print_gw_module(void* arg, int nCols, uint8_t** values, char **cols)
{
        int i;
        for (i = 0; i < nCols; i++) {
                if (strcmp(cols[i], "gw_module_id") == 0) {
                        printf("GW module id : %d",atoi(values[i]));
                }
        }
        return 0;
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

void test_read_gw_module()
{
        uint8_t bridge_guid[8] = 
                {0x01, 0x30, 0x48, 0x68, 0xB3, 0xDE, 0x00, 0x00};
        int ret, i = 0;
        void **args;
        void *stmt;
        /* Dummy parameters */
        char *query = "select * from vfm_gw_module_attr where vfm_bridge_guid = ?1;";
        
        for (i = 2 ; i < 5 ;i++) {

        bridge_guid[2]++;

        args = (void**)malloc(sizeof(void*));

        args[0] = bridge_guid;

        stmt = vfmdb_prepare_query(query, "g", args);
        if (!stmt) {
                printf("ERROR: Cannot prepare sqlite3 statement\n");
                return;
        }

        ret = vfmdb_execute_query(stmt, print_gw_module, NULL);
        if (!ret) {
                printf("ERROR: Sqlite3 statement execution failed.\n");
        }
        }
}
void test_read_io_module()
{
        int ret;
        void *stmt;
        /* Dummy parameters */
        char *query = "select * from vfm_io_module_attr;";

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

void create_vanilla_testdb(uint32_t count)
{
        uint32_t i, j =0;
        uint8_t bridge_guid[8] = 
                {0x01, 0x30, 0x48, 0x68, 0xB3, 0xDE, 0x03, 0x00};
        int gw_module_id = 1;

        for(i = 0; i < count; i++)
        {
                // Currently adding some randomness by changing 3rd byte
                bridge_guid[3]++;
                test_insert_bridge(bridge_guid);

                for (j = 0; j < 2; j++)
                        test_insert_gw_module(bridge_guid, gw_module_id++);
        }
}

void 
insert_vfabric_fc_attr(uint32_t vfabric_id)
{
        int ret;
        void *stmt;
        void **args;
        char query[512];
        int fcid = 1;
        uint8_t wwnn[8] = 
                {0x01, 0x30, 0x48, 0x68, 0xB3, 0xDE, 0x02, 0x3A};
        uint8_t wwpn[8] = 
                {0x0A, 0x20, 0xA8, 0x68, 0xC1, 0xDE, 0x12, 0x3A};

        sprintf(query,"insert into vfm_vfabric_fc_attr(vfabric_id, fcid,wwnn, wwpn) values(%d,%d,?1,?2);", vfabric_id,fcid);

        args = (void**)malloc(2 * sizeof(void*));
        args[0] = wwnn;
        args[1] = wwpn;

        stmt = vfmdb_prepare_query(query, "gg", args);

        if (!stmt) {
                printf("ERROR: Cannot prepare sqlite3 statement\n");
                return;
        }

        ret = vfmdb_execute_query(stmt, NULL, NULL);

        if (!ret) {
                printf("ERROR: Sqlite3 statement execution failed.\n");
        }
        free(args);
}

void 
insert_vfabric_en_attr(uint32_t vfabric_id)
{
        int ret;
        void *stmt;
        void **args;
        char query[512];
        int vlan_id = 1;
        uint8_t mac[6] = {0x01, 0x30, 0x4D, 0x78, 0x23, 0x3A};

        memset(query, 0, sizeof(query));

        sprintf(query,"insert into vfm_vfabric_en_attr(vfabric_id,vlan,mac) values(%d,%d,?1);", vfabric_id, vlan_id);

        args = (void**)malloc(sizeof(void*));

        args[0] = mac;

        stmt = vfmdb_prepare_query(query, "m", args);

        if (!stmt) {
                printf("ERROR: Cannot prepare sqlite3 statement\n");
                return;
        } 
        ret = vfmdb_execute_query(stmt, NULL, NULL);
        if (!ret) {
                printf("ERROR: Sqlite3 statement execution failed.\n");
        }

        free(args);

}


int process_vfabric(void* arg, int nCols, uint8_t** values, char **cols)
{
        int i;
        uint32_t vfabric_id, protocol;


        for (i = 0; i < nCols; i++) {
                if (strcmp(cols[i], "id") == 0) {
                        vfabric_id = atoi(values[i]);
                }
                if (strcmp(cols[i], "protocol") == 0) {
                        protocol = atoi(values[i]);
                }
        }
        if(protocol == 1) {
                insert_vfabric_en_attr(vfabric_id);
        }
        else {
                insert_vfabric_fc_attr(vfabric_id);
        }

        return 0;
}

void 
insert_vfabric_attr()
{
        int ret;
        void *stmt;
        /* Dummy parameters */
        char *query = "select * from vfm_vfabric_attr;";

        stmt = vfmdb_prepare_query(query, NULL, NULL);

        if (!stmt) {
                printf("ERROR: Cannot prepare sqlite3 statement\n");
                return;
        }

        ret = vfmdb_execute_query(stmt, process_vfabric, NULL);
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
        test_insert_iomodule(1);
        //exit(1);

        printf("Insert Gateway Modules..\n");
        test_insert_gw_module(1);
        test_insert_gw_module(2);

        printf("Insert Gateway..\n");
        test_insert_gw(1122);

        printf("Testing test_read_bridge\n");
        test_read_bridge();
        */

        //printf("Testing test_read_io_module\n");
        //test_read_io_module();

        //test_read_gw_module();
        //printf("Create vanilla test database..\n");
        //create_vanilla_testdb(2);
        insert_vfabric_attr();
}
