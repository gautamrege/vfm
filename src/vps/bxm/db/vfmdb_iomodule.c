/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */

#include <bxmapi_common.h>

/*
 * This function will search the database and the max value of the id.
 * Then it will increment the value and return .
 */
int
generate_io_module_id(void *data,
                int num_cols,
                uint8_t **values,
                char **cols)
{
        vpsdb_resource *rsc = (vpsdb_resource*)data;
        vpsdb_io_module_t *io_module = (vpsdb_io_module_t *)(rsc->data);
        /* For now we are using a simple auto-increment */
        if (values[0])
               io_module->id = atoi(values[0]) + 1;
        else
               io_module->id= 1;

        return 0;
}


/*
 * This function adds the io_module in the database(Table : bxm_io_modules)
 * 
 * [IN] *res_io_module : io module resource 
 *
 * Return : err 
 */
vps_error
add_io_module(vpsdb_resource *res_io_module)
{
        vps_error err = VPS_SUCCESS;
        vpsdb_io_module_t *io_module = (vpsdb_io_module_t*)res_io_module->data;
        void *stmt;
        void **args;
        char get_max_query[128] = "select max(id) from bxm_io_module_attr;";
        char insert_query[1024]; 

       
        vps_trace(VPS_ENTRYEXIT, "Entering add_io_module");

        memset(insert_query, 0 , sizeof(insert_query));
        /* Prepare the query for io module add */

        stmt = vfmdb_prepare_query(get_max_query, NULL, NULL);
        if (!stmt) {
                vps_trace(VPS_ERROR," Cannot prepare sqlite3 statement");
                err = VPS_DBERROR;
                goto out;
        }

        if (VPS_SUCCESS != (err = vfmdb_execute_query(stmt,
                                 generate_io_module_id,:/* call back function */
                                 res_io_module))) {
                vps_trace(VPS_ERROR, "Could not get max io module id");
                err = VPS_DBERROR;
                goto out;
        }
        
        args = (void**)malloc(3 * sizeof(void*));
                        
        args[0] = io_module->name;
        args[1] = io_module->mac;
        args[2] = io_module->guid;
        
        sprintf(insert_query, "insert into bxm_io_module_attr( id, name, "
                "type, mac, guid, num_vhba, num_vnic, "
                "slot, port, supported_speed) values ( %d, ?1, %d, ?2, ?3,"
                "%d, %d, %d, %d, %d )", io_module->id,io_module->type,
                io_module->num_vhba, io_module->num_vnic,
                io_module->slot, io_module->port,io_module->supported_speed);


        stmt = vfmdb_prepare_query(insert_query, "smg", args);
        if (!stmt) {
                vps_trace(VPS_ERROR," Cannot prepare sqlite3 statement");
                err = VPS_DBERROR;
                goto out;
        }

        if (VPS_SUCCESS != (err = vfmdb_execute_query(stmt, NULL, NULL))) {
                vps_trace(VPS_ERROR, "Error adding vadapter to datbase");
                goto out;
        }

        free(args);
        vps_trace(VPS_ENTRYEXIT, "Leaving  add_io_module");

out:        
        return err;
}

/*
 * This function will fill the values of the CNA from the database
 * The fields are not yet defined. So using the fields that are defined
 * in the database.
 */
        int
process_io_module(void *data, int num_cols, uint8_t **values, char **cols)
{
        vpsdb_resource *rsc = (vpsdb_resource*)data;
        vpsdb_io_module_t *io_module;
        uint32_t i;

        vps_trace(VPS_ENTRYEXIT, "Entering process_io_module");

        /* Read the existing resrouce count for re-allocation */
        if (NULL == (rsc->data = realloc(rsc->data,
                            sizeof(vpsdb_io_module_t) * (rsc->count + 1)))) {
                /*
                 * The block could not be realloc'ed. This can cause serious
                 * problems .Hence we return with a db_error
                 */
                vps_trace(VPS_ERROR, "Could not realloc memory: %d",
                                rsc->count + 1);
                return 1; /* This will propogate with SQL_ABORT */
        }

        /* Go the the io_module array offset */
        io_module = rsc->data + (sizeof(vpsdb_io_module_t) * rsc->count);

        /* Fill the io_module structure */
        for (i = 0; i < num_cols; i++) {
                if (strcmp(cols[i], "name") == 0)
                        memcpy(io_module->name, (values[i]), 8);
                else if (strcmp(cols[i], "type") == 0)
                        io_module->type = atoi(values[i]);
                else if (strcmp(cols[i], "mac") == 0)
                        memcpy(io_module->mac,(values[i]), 6);
                else if (strcmp(cols[i], "guid") == 0)
                        memcpy(io_module->guid, (values[i]), 8);
                else if (strcmp(cols[i], "num_vhba") == 0)
                        io_module->num_vhba = atoi(values[i]);
                else if (strcmp(cols[i], "num_vnic") == 0)
                        io_module->num_vnic = atoi(values[i]);
                else if (strcmp(cols[i], "slot") == 0)
                        io_module->slot = atoi(values[i]);
                else if (strcmp(cols[i], "port") == 0)
                        io_module->port = atoi(values[i]);
                else if (strcmp(cols[i], "supported_speed") == 0)
                        io_module->supported_speed = atoi(values[i]);
        }

        /* After the data is correctly populated, increment the bridge count */
        rsc->count++;

        vps_trace(VPS_ENTRYEXIT, "Leaving process_io_module");
        return 0; /* Callback must return 0 on success */
}










/*
 * Populate IO module from database.
 *
 */
vps_error
populate_iomodule(vpsdb_io_module_t *io_module)
{
        vps_error err = VPS_SUCCESS;
        char query[1024];
        vpsdb_resource rsc;

        vps_trace(VPS_ENTRYEXIT, "Entering populate_iomodule");

        if(!io_module) {
                vps_trace(VPS_ERROR, "IO Module not specified.");
                err = VPS_DBERROR_INVALID;
                goto out;
        }

        sprintf(query, "select * from bxm_io_modules_attr"
                       "where id = %d;", io_module->id);

        rsc.data = io_module;
        rsc.type = VPS_DB_IO_MODULE;

        if (VPS_SUCCESS != (err = vpsdb_read(query, process_io_module, 
                                        &rsc))) {
                vps_trace(VPS_ERROR, "Could not get io module information");
                err = VPS_DBERROR;
                goto out;
        }

        vps_trace(VPS_ENTRYEXIT, "Leaving  populate_iomodule");

}
