/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#include <vfmapi_common.h>

/*
 * This function will search the database and the max value of the id.
 * Then it will increment the value and return .
 */
int
generate_vadapter_id(void *data,
                int num_cols,
                uint8_t **values,
                char **cols)
{
        vpsdb_resource *rsc = (vpsdb_resource*)data;
        vfm_vadapter_attr_t *vadapter = (vfm_vadapter_attr_t *)(rsc->data);
        /* For now we are using a simple auto-increment */
        if (values[0])
                vadapter->_vadapter_id = atoi(values[0]) + 1;
        else
                vadapter->_vadapter_id= 1;

        return 0;
}

/*
 * @brief This function will search the database and the count of the vadapters
 * Then it will return the value.
 */
int
get_vadapter_count(void *data,
                int num_cols,
                uint8_t **values,
                char **cols)
{
        uint32_t *count = (uint32_t *)(data);
        if (values[0])
                *count = atoi(values[0]);
}
/*
 * Query: insert into vfm_vadapter_attr_t (id, name, desc, init_type,
 *        protocol) values(...);
 * [in\out] info :Preallocated pointer by the caller. The new object id created
 *                will be returned into the same.
 */

vps_error
add_vadapter(vpsdb_resource *info)
{
        vps_error err = VPS_SUCCESS;
        vfm_vadapter_attr_t *vadapter = (vfm_vadapter_attr_t*)(info->data);
	void *stmt;
        char get_max_query[128] = "select max(id) from vfm_vadapter_attr;";
        char insert_query[1024] = "insert into vfm_vadapter_attr (id, name, "
                           "desc, protocol) values (";

        vps_trace(VPS_ENTRYEXIT, "Entering add_vadapter");

        /* Prepare the query for vadapter add */

        stmt = vfmdb_prepare_query(get_max_query, NULL, NULL);
        if (!stmt) {
                vps_trace(VPS_ERROR," Cannot prepare sqlite3 statement");
                err = VPS_DBERROR;
                goto out;
        }

        if (VPS_SUCCESS != (err = vfmdb_execute_query(stmt,
                              generate_vadapter_id, /* call back function */
                              info))) {
                vps_trace(VPS_ERROR, "Could not get max vadapter id");
                err = VPS_DBERROR;
                goto out;
        }

        /* Insert the vadapter into the table */
        sprintf(insert_query, "%s %d, '%s', '%s', %d);",
                        insert_query, vadapter->_vadapter_id,
                        vadapter->name, vadapter->desc, vadapter->protocol);

	stmt = vfmdb_prepare_query(insert_query, NULL, NULL);
        if (!stmt) {
                vps_trace(VPS_ERROR," Cannot prepare sqlite3 statement");
                err = VPS_DBERROR;
                goto out;
        }
        if (VPS_SUCCESS != (err = vfmdb_execute_query(stmt,
                                        NULL, NULL))) {
                vps_trace(VPS_ERROR, "Error adding vadapter to datbase");
                goto out;
        }

out:
        vps_trace(VPS_ENTRYEXIT, "Leaving add_vadapter");
        return err;
}


/*
 * This routine is called ONCE for each record from the result set - Gateways
 */
int
process_vadapter(void *data, int num_cols, uint8_t **values, char **cols)
{
        vpsdb_resource *rsc = (vpsdb_resource*)data;
        vfm_vadapter_attr_t *vadapter;
        uint8_t i;
        vps_trace(VPS_ENTRYEXIT, "Entering process_vadapter. Count: %d",
                rsc->count);
/* Now the memory is already allocated so no need to allocate it again.*/

        if (NULL == (rsc->data = realloc(rsc->data,
                     sizeof(vfm_vadapter_attr_t) * (rsc->count + 1)))) {
                /*
                 * The block could not be realloc'ed. This can cause serious
                 * problems. Hence we return with a db_error
                 */
                vps_trace(VPS_ERROR, "Could not realloc memory: %d",
                                rsc->count + 1);
                return 1; /* This will propogate with SQL_ABORT */
        }

        /* Go the the vadapter array offset */
        vadapter = rsc->data + (sizeof(vfm_vadapter_attr_t) * rsc->count);

        /* Fill the bridge structure */
        for (i = 0; i < num_cols; i++) {
                if (strcmp(cols[i], "id") == 0)
                        vadapter->_vadapter_id = atoi(values[i]);
                else if (strcmp(cols[i], "io_module_id") == 0) {
                        if(values[i]) /* Foreign keys may be NULL */
                                vadapter->io_module_id = atoi(values[i]);
                }
                else if (strcmp(cols[i], "vfabric_id") == 0) {
                        if(values[i]) /* Foreign keys may be NULL */
                                vadapter->vfabric_id = atoi(values[i]);
                }
                else if (strcmp(cols[i], "name") == 0) {
                        strcpy(vadapter->name, values[i]);
                }
                else if (strcmp(cols[i], "desc") == 0) {
                        strcpy(vadapter->desc, values[i]);
                }
                else if (strcmp(cols[i], "init_type") == 0)
                        vadapter->init_type = atoi(values[i]);
                else if (strcmp(cols[i], "protocol") == 0)
                        vadapter->protocol = atoi(values[i]);
        }
        /* After the data is correctly populated, increment the bridge count */
        rsc->count++;
        vps_trace(VPS_INFO, "Vadapter successfully read");
        vps_trace(VPS_ENTRYEXIT, "Leaving process_vadapter");
        return 0;
}


/* PROTOCOL ATTRIBUTES PROCESSING */
/* NOT TESTED */
int
process_vadapter_en(void *data, int num_cols, uint8_t **values, char **cols)
{
        uint32_t i;
        vfm_vadapter_attr_t *vadapter = (vfm_vadapter_attr_t*)data;

        /* Fill the bridge structure */
        for (i = 0; i < num_cols; i++) {
                if (strcmp(cols[i], "mac") == 0)
                        memcpy(vadapter->en_attr.mac, values[i], 6);
                else if (strcmp(cols[i], "promiscuous") == 0)
                        vadapter->en_attr.promiscuous_mode = atoi(values[i]);
                else if (strcmp(cols[i], "silent_listener") == 0)
                        vadapter->en_attr.silent_listener = atoi(values[i]);
                else if (strcmp(cols[i], "vlan") == 0)
                        vadapter->en_attr.vlan = atoi(values[i]);
        }
        return 0;
}

int
process_vadapter_fc(void *data, int num_cols, char **values, char **cols)
{
        uint32_t i;
        vfm_vadapter_attr_t *vadapter = (vfm_vadapter_attr_t*)data;

        /* Fill the bridge structure */
        for (i = 0; i < num_cols; i++) {
                if (strcmp(cols[i], "wwnn") == 0)
                        memcpy(&(vadapter->fc_attr.wwnn), values[i], 8);
                else if (strcmp(cols[i], "wwpn") == 0)
                        memcpy(&(vadapter->fc_attr.wwpn), values[i], 8);
                else if (strcmp(cols[i], "fcid") == 0)
                        memcpy(vadapter->fc_attr.fcid, values[i], 3);
                else if (strcmp(cols[i], "spma") == 0)
                        vadapter->fc_attr.spma = atoi(values[i]);
                else if (strcmp(cols[i], "fpma") == 0)
                        vadapter->fc_attr.fpma = atoi(values[i]);
        }
        return 0;
}


/*
 * This function populates the protocol attributes for the vadapter.
 * It takes the vadapter id as input and searches the database for the 
 * protocol attributes for that vadapter depending upon the protocol of 
 * the vadapter.
 * These attributes are filled in the given structure.
 * 
 * @param[in/out]  vadapter : vadapter attribute structure (preallocated).
 *
 * Returns success or error values.
 */
vps_error
populate_vadapter_ex(vfm_vadapter_attr_t *vadapter)
{
        vps_error err = VPS_SUCCESS;
        char query[1024];
        uint8_t i;
        char tmp_str[1024];
        vpsdb_resource rsc;

        /*
         * TODO: Modify this according to the prepare and execute
         * routines .
         */

        vps_trace(VPS_ENTRYEXIT, "Entering %s", __FUNCTION__);
        if (!vadapter) {
                vps_trace(VPS_ERROR, "Vadapter not specified.");
                err = VPS_DBERROR_INVALID;
                goto out;
        }

        if (vadapter->protocol == VFM_PROTOCOL_EN) {
                sprintf(query, "select * from vfm_vadapter_en_attr "
                           "where vadapter_id = %d;", vadapter->_vadapter_id);

                if (VPS_SUCCESS != (err = vpsdb_read(query,
                                          process_vadapter_en,
                                          (vpsdb_resource*)vadapter))) {
                        vps_trace(VPS_ERROR,
                                        "Could not get extra information");
                        err = VPS_DBERROR;
                        goto out;
                }
        }
        else if (vadapter->protocol == VFM_PROTOCOL_FC) {
                sprintf(query, "select * from vfm_vadapter_fc_attr "
                       "where vadapter_id = %d;", vadapter->_vadapter_id);

                if (VPS_SUCCESS != (err = vpsdb_read(query,
                                          process_vadapter_fc,
                                          (vpsdb_resource*)vadapter))) {
                        vps_trace(VPS_ERROR,
                                        "Could not get extra information");
                        err = VPS_DBERROR;
                        goto out;
                }
        }
out:
                vps_trace(VPS_ENTRYEXIT, "Leaving %s", __FUNCTION__);
}


/*
 * @brief This function will populate the properties of the vadapter.
 * 
 * @param[IN] bitmask : bit_mask Specifies an bit mask value. 
 *                      The bitmask indicates the specific properties of the
 *                      object to be retrieved.
 * @param[IN] *attr   : attr Pointer to the structure containing the attrs of the
 *                      vadapters that should match
 * @param[OUT] *rsc   : vps_resource pointer into which the data will be filled. 
 *
 * @return Returns 0 on success, or an error code on failure.
 */
vps_error
populate_vadapter_information(vfm_vadapter_attr_bitmask_t bitmask,
                              vfm_vadapter_attr_t *attr,
                              vpsdb_resource *rsc)
{
        char select_query[1024] = "select * from vfm_vadapter_attr ";
        char count_query[1024] = "select count(*) from vfm_vadapter_attr ";
        char query[1024];
        void *stmt;
        int count = 0 , i = 0, vadapter_count = 0;
        vps_error err = VPS_SUCCESS;

        vps_trace(VPS_ENTRYEXIT, "Entering %s", __FUNCTION__);

        memset(query, 0, sizeof(query));
        memset(rsc, 0, sizeof(vpsdb_resource));

        /* 
         * 1. Formulate the query according to the bitmask that is set.
         *    Take care of unicode values and its processing.
         *    Prepare final query and count query.
         * TODO :WORKING ON IT
         */

        if (bitmask.id) {
                add_query_parameters(query, count++, "_vadapter_id",
                               attr->_vadapter_id , Q_UINT32);
        }
        if (bitmask.desc) {
                add_query_parameters(query, count++, "`desc`",
                                attr->desc, Q_UINT8);
        }
        if (bitmask.name) {
                add_query_parameters(query, count++, "`name`",
                                attr->name, Q_UINT8);
        }
        if (bitmask.protocol) {
                add_query_parameters(query, count++, "protocol",
                                attr->protocol, Q_UINT32);
        }
        if (bitmask.io_module_id) {
                add_query_parameters(query, count++, "io_module_id",
                                attr->io_module_id, Q_UINT32);
        }
        query[strlen(query)] = ';' ; 

        /*
         * 2: Get the count of the vadapters from the databse
         */
        sprintf(count_query, "%s ", query); 
        stmt = vfmdb_prepare_query(count_query, NULL, NULL);
        if (!stmt) {
                vps_trace(VPS_ERROR," Cannot prepare sqlite3 statement");
                err = VPS_DBERROR;
                goto out;
        }

        if (VPS_SUCCESS != (err = vfmdb_execute_query(stmt,
                              get_vadapter_count, /* call back function */
                              (void *)&vadapter_count))) {
                vps_trace(VPS_ERROR, "Could not get count of vadapters");
                err = VPS_DBERROR;
                goto out;
        }


        /* 
         * 3. Allocate memory for them at once.
         *    The caller function is responsible for freeing this memory.
         */

        rsc->data = malloc(vadapter_count * sizeof(vfm_vadapter_attr_t));

        if(NULL == rsc->data) {
                vps_trace(VPS_ERROR, "Could not allocate memory");
                goto out;
        }
        
        /* 
         * 4. This fills up the vadapter information for all vadapters.
         *    This query is then given to prepare_query and execute.
         *    The result is collected in the vps_rsc previously allocated.
         */

        sprintf(select_query, "%s ", query); 

        stmt = vfmdb_prepare_query(select_query, NULL, NULL);
        if (!stmt) {
                vps_trace(VPS_ERROR," Cannot prepare sqlite3 statement");
                err = VPS_DBERROR;
                goto out;
        }
        if (VPS_SUCCESS != (err = vfmdb_execute_query(stmt,
                                        process_vadapter,
                                        rsc))) {
                vps_trace(VPS_ERROR, "Could not get EN properties ");
                err = VPS_DBERROR;
                goto out;
        }
        /* 
         * 5. Then for each vadapter call the populate_vadapter_ex to 
         *    populate its protocol information. by passing the pointer
         *    to the structure.
         */
       
        for (i = 0; i < vadapter_count; i ++) {
                                

        }
out :    
        vps_trace(VPS_ENTRYEXIT, "Leaving %s", __FUNCTION__);

}

