/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#include <bxmapi_common.h>

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
        bxm_vadapter_attr_t *vadapter = (bxm_vadapter_attr_t *)(rsc->data);
        /* For now we are using a simple auto-increment */
        if (values[0])
                vadapter->_vadapter_id = atoi(values[0]) + 1;
        else
                vadapter->_vadapter_id= 1;

        return 0;
}

/*
 * Query: insert into bxm_vadapter_attr_t (id, name, desc, init_type,
 *        protocol) values(...);
 * [in\out] info :Preallocated pointer by the caller. The new object id created
 *                will be returned into the same.
 */

vps_error
add_vadapter(vpsdb_resource *info)
{
        vps_error err = VPS_SUCCESS;
        bxm_vadapter_attr_t *vadapter = (bxm_vadapter_attr_t*)(info->data);
	void *stmt;
        char get_max_query[128] = "select max(id) from bxm_vadapter_attr;";
        char insert_query[1024] = "insert into bxm_vadapter_attr (id, name, "
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
        bxm_vadapter_attr_t *vadapter;
        uint8_t i;
        vps_trace(VPS_ENTRYEXIT, "Entering process_vadapter. Count: %d",
                rsc->count);

        if (NULL == (rsc->data = realloc(rsc->data,
                     sizeof(bxm_vadapter_attr_t) * (rsc->count + 1)))) {
                /*
                 * The block could not be realloc'ed. This can cause serious
                 * problems. Hence we return with a db_error
                 */
                vps_trace(VPS_ERROR, "Could not realloc memory: %d",
                                rsc->count + 1);
                return 1; /* This will propogate with SQL_ABORT */
        }

        /* Go the the bridge array offset */
        vadapter = rsc->data + (sizeof(bxm_vadapter_attr_t) * rsc->count);

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
        bxm_vadapter_attr_t *vadapter = (bxm_vadapter_attr_t*)data;

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
        bxm_vadapter_attr_t *vadapter = (bxm_vadapter_attr_t*)data;

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


/* POPULATING VADAPTER PROTCOL ATTR INFO */
vps_error
populate_vadapter_ex(bxm_vadapter_attr_t *vadapter)
{
        vps_error err = VPS_SUCCESS;
        char query[1024];
        uint8_t i;
        char tmp_str[1024];
        vpsdb_resource rsc;

        vps_trace(VPS_ENTRYEXIT, "Entering %s", __FUNCTION__);
        if (!vadapter) {
                vps_trace(VPS_ERROR, "Vadapter not specified.");
                err = VPS_DBERROR_INVALID;
                goto out;
        }

        if (vadapter->protocol == BXM_PROTOCOL_EN) {
                sprintf(query, "select * from bxm_vadapter_en_attr "
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
        else if (vadapter->protocol == BXM_PROTOCOL_FC) {
                sprintf(query, "select * from bxm_vadapter_fc_attr "
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


vps_error
populate_vadapter_information(vpsdb_resource *rsc, const char* name)
{
        bxm_vadapter_attr_t *vadapter = (bxm_vadapter_attr_t*)rsc->data;
        vps_error err = VPS_SUCCESS;
        char query[1024] = "select * from bxm_vadapter_attr ";
        uint8_t i;

        /* If name exists, we need information only for that bridge */
        if (name)
                sprintf(query, "%s where id = \"%s\";", query, name);
        else
                strcat(query, ";");

        /* Update the resource information with the type */
        rsc->type = VPS_DB_VADAPTER;

        vps_trace(VPS_INFO, "populate_vadapter_information query: %s", query);
        /* Get the bridges */
        if (VPS_SUCCESS != (err = vpsdb_read(query,
                            process_vadapter,
                            rsc))) {
                vps_trace(VPS_ERROR, "Could not get gateway information");
                goto out;
        }

        /* For each gateway, get the external port status */
        for (i = 0; i < rsc->count; i++) {
                /*  For each vadaptor, populate its attributes */
                populate_vadapter_ex((bxm_vadapter_attr_t*)rsc->data + i);
        }
out:
        return err;
}

