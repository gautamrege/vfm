/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#include <vfmdb.h>
/* The sqlite database pointer */
extern sqlite3 *g_db;

/*
 * vpsdb_update
 * This routine is used to update the database. It does need to
 * process any results.
 */
vps_error
vpsdb_update(const char* query)
{
        vps_error err = VPS_SUCCESS;
        char *zErrMsg = 0;
        int rc;

        vps_trace(VPS_ENTRYEXIT, "Entering vpsdb_update: %s\n", query);

        rc = sqlite3_exec(g_db, query, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK) {
                vps_trace(VPS_ERROR, "SQL error: %s", zErrMsg);
                /* Free zErrMsg ONLY during errors. NULL for success */
                sqlite3_free(zErrMsg);
                err = VPS_DBERROR;
        }

        vps_trace(VPS_ENTRYEXIT, "Leaving vpsdb_update");
        return err;
}

/*
 * This is a read-only function which processes select queries.
 * The callback function will be given the vpsdb_resource parameter, so its
 * the onus of that callback function to populate the data correctly
 */
vps_error
vpsdb_read(const char* query,
                int (*sql_cb)(void*, int, char**, char**),
                vpsdb_resource *rsc)
{
        vps_error err = VPS_SUCCESS;
        char *zErrMsg = 0;
        int rc;

        vps_trace(VPS_ENTRYEXIT, "Entering vspdb_read");

        rc = sqlite3_exec(g_db, query, sql_cb, rsc, &zErrMsg);
        if (rc != SQLITE_OK) {
                vps_trace(VPS_ERROR, "SQL error: %s", zErrMsg);
                /* Free zErrMsg ONLY during errors. NULL during success */
                sqlite3_free(zErrMsg);
                err = VPS_DBERROR;
        }
        else
                vps_trace(VPS_INFO,
                      "SQL query SUCCESS: Retrieved %d objects:", rsc->count);

        vps_trace(VPS_ENTRYEXIT, "Leaving vspdb_read");
        return err;
}

vps_error
validate_add_bridge(vpsdb_resource *info)
{
        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering validate_add_bridge");

        /* Validate that the info indeed contains the bridge */
        if (VPS_DB_BRIDGE != info->type) {
                vps_trace(VPS_ERROR, "Incorrect resource. Bridge expected");
                err = VPS_DBERROR_INVALID_RESOURCE;
        }

        /* TODO: Any other bridge validations? */

        vps_trace(VPS_ENTRYEXIT, "Leaving validate_add_bridge: %x", err);
        return err;
}

vps_error
add_external_ports(uint16_t gw_id, uint16_t *ext_ports)
{
        vps_error err = VPS_SUCCESS;
        char insert_external_ports[1024];
        uint32_t i;
        vps_trace(VPS_ENTRYEXIT, "Entering add_external_ports");

        /* There are 15 external ports always */
        for (i = 0; i < 15; i++) {
                strcpy(insert_external_ports, "insert into external_ports("
                                "gw_id, "
                                "id, "
                                "max_speed, "
                                "current_speed, "
                                "status) values (");
                sprintf(insert_external_ports, "%s%d",
                                insert_external_ports, gw_id);
                sprintf(insert_external_ports, "%s, %d",
                                insert_external_ports, i+1);
                /* TODO: max_speed, current_speed need to be set right */
                sprintf(insert_external_ports, "%s, %d, %d",
                                insert_external_ports, 0, 0);
                sprintf(insert_external_ports, "%s, %d);",
                                insert_external_ports,
                                ext_ports[i]);

                vps_trace(VPS_INFO, "insert external ports (%d): %s", i+1,
                                insert_external_ports);
                /** Execute the query to add external port into the DB ***/
                if (VPS_SUCCESS != (err = vpsdb_update(insert_external_ports)))
                {
                        vps_trace(VPS_ERROR, "Ignoring error in adding "
                                        "external port(%d) for gateway", i+1);
                        /* TODO: Ensure if this is correct. Ignore?? */
                        /* Reset the error */
                        err = VPS_SUCCESS;
                }
        }

        vps_trace(VPS_ENTRYEXIT, "Leaving add_external_ports: %x", err);
        return err;
}

vps_error
add_gateway(const char* bridge_id, bxm_gateway_attr_t *gw)
{
        vps_error err = VPS_SUCCESS;
        uint8_t tmp[32];
        char insert_gateway_query[1024] = "insert into gateways(bridge_id, "
                                          "gw_id, flag_ep, flag_sp, flag_se, "
                                          "flag_f, flag_es, flag_is, flag_l, "
                                          "flag_extra, ips, heartbeat, "
                                          "conn_speed) values (";

        vps_trace(VPS_ENTRYEXIT, "Entering add_gateway");

        /* bridge id */
        memset(tmp, 0, sizeof(tmp));
        memcpy(tmp, bridge_id, 6);
        sprintf(insert_gateway_query, "%s\"%s\"", insert_gateway_query, tmp);
        /* Set gw_id and all the flags */
/*        sprintf(insert_gateway_query,
                        "%s, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
                        insert_gateway_query, gw->_gw_id,
                        gw->_ext_protocol, gw->_vadapter_list,
			gw->_vfabric_list, gw->flood,
                        gw->egress_secure, gw->ingress_secure,
                        gw->l2_lookup, 0, gw->_int_port);*/
	/* TODO: set heartbeat correctly */
        sprintf(insert_gateway_query, "%s,\"%s\"", insert_gateway_query, "0");
        /* TODO: set connection speed later */
        sprintf(insert_gateway_query, "%s,\"%s\")", insert_gateway_query, "0");

        vps_trace(VPS_INFO, "add_gateway query: %s", insert_gateway_query);
        /** Execute the query to add_gateway into the DB ***/
        if (VPS_SUCCESS != (err = vpsdb_update(insert_gateway_query))) {
                vps_trace(VPS_ERROR, "Error in adding gateway for bridge");
                goto out;
        }
/*
        if (VPS_SUCCESS
                != (err = add_external_ports(gw->_gw_id, gw->_ext_port))) {
                vps_trace(VPS_ERROR, "Error in adding gateway to database");
                goto out;
        }
*/
out:
        vps_trace(VPS_ENTRYEXIT, "Leaving add_gateway: %x", err);
        return err;
}

/* TODO : Rewrite the function using new header file and Unicode correction */
vps_error
add_bridge(vpsdb_resource *info)
{
        vps_error err = VPS_SUCCESS;
        bxm_bd_attr_t *bridge = (bxm_bd_attr_t*)info->data;
        bxm_bd_attr_t *ptr = NULL;
        bxm_gateway_attr_t *gw = NULL;
        uint32_t i, j;
        uint8_t tmp[32];
out:
        vps_trace(VPS_ENTRYEXIT, "Leaving add_bridge");
        return err;
}

/*
 * This routine is called ONCE for each record from the result set
 * This is a special routine for external_ports. There is no resource type
 * called external ports, so we have to be careful. We use the resource
 * only as a temporary storage structure.
 */
int
process_external_ports(void *data,
                int num_cols,
                char **values,
                char **cols)
{
        vpsdb_resource *rsc = (vpsdb_resource*)data;
        uint16_t *external_ports = (uint16_t*)rsc->data;

        /*
         * According to the query, values[0] = id & values[1] = status
         * Ports start from 1 to 15, so we need to decrement the index
         */
        external_ports[atoi(values[0]) - 1] = (uint16_t)atoi(values[1]);
        return 0;
}


/*
 * This routine is called ONCE for each record from the result set - Gateways
 */
int
process_gateway_module(void *data, int num_cols, char **values, char **cols)
{
        vpsdb_resource *rsc = (vpsdb_resource*)data;
        uint32_t *gw_module;
        uint8_t i;
        vps_trace(VPS_ENTRYEXIT, "Entering process_gateway. Count: %d",
                rsc->count);

        if (NULL == (rsc->data = realloc(rsc->data,
                     sizeof(uint32_t) * (rsc->count + 1)))) {
                /*
                 * The block could not be realloc'ed. This can cause serious
                 * problems. Hence we return with a db_error
                 */
                vps_trace(VPS_ERROR, "Could not realloc memory: %d",
                                rsc->count + 1);
                return 1; /* This will propogate with SQL_ABORT */
        }

        /* Go the the bridge array offset */
        gw_module = rsc->data + (sizeof(uint32_t) * rsc->count);

        /* Fill the bridge structure */
        for (i = 0; i < num_cols; i++) {
                if (strcmp(cols[i], "gw_module_id") == 0)
                        *gw_module = atoi(values[i]);
        }
        /* After the data is correctly populated, increment the bridge count */
        rsc->count++;
        vps_trace(VPS_INFO, "Gateway successfully read");
        vps_trace(VPS_ENTRYEXIT, "Leaving processs_gateway.");
        return 0;
}


/*
 * This routine is called ONCE for each record from the result set - Gateways
 */
int
process_gateway(void *data, int num_cols, char **values, char **cols)
{
        vpsdb_resource *rsc = (vpsdb_resource*)data;
        bxm_gateway_attr_t *gateway;
        uint8_t i;
        vps_trace(VPS_ENTRYEXIT, "Entering process_gateway. Count: %d",
                rsc->count);

        if (NULL == (rsc->data = realloc(rsc->data,
                     sizeof(bxm_gateway_attr_t) * (rsc->count + 1)))) {
                /*
                 * The block could not be realloc'ed. This can cause serious
                 * problems. Hence we return with a db_error
                 */
                vps_trace(VPS_ERROR, "Could not realloc memory: %d",
                                rsc->count + 1);
                return 1; /* This will propogate with SQL_ABORT */
        }

        /* Go the the bridge array offset */
        gateway = rsc->data + (sizeof(bxm_gateway_attr_t) * rsc->count);

        /* Fill the bridge structure */
        for (i = 0; i < num_cols; i++) {
                if (strcmp(cols[i], "gw_id") == 0)
                        gateway->_gw_id = atoi(values[i]);
                else if (strcmp(cols[i], "physical_index") == 0)
                        memcpy(gateway->_physical_index, values[i], 8);
                else if (strcmp(cols[i], "desc") == 0)
                        memcpy(gateway->desc, values[i], 8);
                else if (strcmp(cols[i], "bxm_guid") == 0)
                        gateway->_bxm_guid = atoi(values[i]);
                else if (strcmp(cols[i], "flag_f") == 0)
                        gateway->flood = atoi(values[i]);
                else if (strcmp(cols[i], "flag_es") == 0)
                        gateway->egress_secure = atoi(values[i]);
                else if (strcmp(cols[i], "flag_is") == 0)
                        gateway->ingress_secure = atoi(values[i]);
        }
        /* After the data is correctly populated, increment the bridge count */
        rsc->count++;
        vps_trace(VPS_INFO, "Gateway successfully read");
        vps_trace(VPS_ENTRYEXIT, "Leaving processs_gateway.");
        return 0;
}

/*
 * This routine is called ONCE for each record from the result set - Bridge
 */
int
process_bridge(void *data, int num_cols, char **values, char **cols)
{
        vpsdb_resource *rsc = (vpsdb_resource*)data;
        bxm_bd_attr_t *bridge;
        uint32_t i;

        vps_trace(VPS_ENTRYEXIT, "Entering process_bridge. Count: %d",
                        rsc->count);

        /* Read the existing resrouce count for re-allocation */
        if (NULL == (rsc->data = realloc(rsc->data,
                     sizeof(bxm_bd_attr_t) * (rsc->count + 1)))) {
                /*
                 * The block could not be realloc'ed. This can cause serious
                 * problems .Hence we return with a db_error
                 */
                vps_trace(VPS_ERROR, "Could not realloc memory: %d",
                                rsc->count + 1);
                return 1; /* This will propogate with SQL_ABORT */
        }

        /* Go the the bridge array offset */
        bridge = rsc->data + (sizeof(bxm_bd_attr_t) * rsc->count);

        /* Fill the bridge structure */
        for (i = 0; i < num_cols; i++) {
                if (strcmp(cols[i], "guid") == 0)
                        bridge->_bd_guid = atoi(values[i]);
                else if (strcmp(cols[i], "bxm_guid") == 0)
                        bridge->_bxm_guid = atoi(values[i]);
                else if (strcmp(cols[i], "desc") == 0) {
                        memcpy(bridge->desc, values[i], 64);
                }
                else if (strcmp(cols[i], "firmware_version") == 0) {
                        memcpy(bridge->_firmware_version, values[i], 64);
                }
        }

        /* After the data is correctly populated, increment the bridge count */
        rsc->count++;
        vps_trace(VPS_INFO, "Bridge successfully read");
        vps_trace(VPS_ENTRYEXIT, "Leaving process_bridge");
        return 0; /* Callback must return 0 on success */
}

vps_error
populate_gw_external_ports(bxm_gateway_attr_t *gw)
{
        vps_error err = VPS_SUCCESS;
        char query[1024];
        uint8_t i;
        char tmp_str[1024];
        vpsdb_resource rsc;

        vps_trace(VPS_ENTRYEXIT, "Entering %s", __FUNCTION__);
        if (!gw) {
                vps_trace(VPS_ERROR, "External ports MUST be for a Gateway");
                err = VPS_DBERROR_INVALID;
                goto out;
        }
        sprintf(query,
        "select id, status from external_ports where gw_id = %d;", gw->_gw_id);

        vps_trace(VPS_INFO, "populate_gw_external_ports query: %s", query);

        /* We already know resource information. */
        rsc.count = 15;
        rsc.data = gw->_ext_port;

        /* Get the external_ports */
        if (VPS_SUCCESS != (err = vpsdb_read(query,
                              process_external_ports, /* call back function */
                              &rsc))) {
                vps_trace(VPS_ERROR, "Could not get ext port information");
                err = VPS_DBERROR;
                goto out;
        }
out:
        vps_trace(VPS_ENTRYEXIT, "Leaving %s", __FUNCTION__);
}

/* The name is the MAC address of the gateway */
vps_error
populate_gateway_information(vpsdb_resource *rsc, const char* where_clause)
{
        bxm_gateway_attr_t *gateway;
        vps_error err = VPS_SUCCESS;
        char query[1024] = "select * from bxm_gateway_attr ";
        uint8_t i;

        /* If name exists, we need information only for that bridge */
        if (*where_clause != '\0')
                sprintf(query, "%s where %s;", query, *where_clause);
        else
                strcat(query, ";");

        /* Update the resource information with the type */
        rsc->type = VPS_DB_GATEWAY;

        vps_trace(VPS_INFO, "populate_gateway_information query: %s", query);
        /* Get the bridges */
        if (VPS_SUCCESS != (err = vpsdb_read(query,
                            process_gateway, /* gateway call back function */
                            rsc))) {
                vps_trace(VPS_ERROR, "Could not get gateway information");
                goto out;
        }

        /* For each gateway, get the external port status */
        for (i = 0; i < rsc->count; i++) {
                /*
                 * For each gateway, the external port status is updated in
                 * the func.
                 */
                populate_gw_external_ports((bxm_gateway_attr_t*)rsc->data + i);
        }
out:
        return err;
}

/* Here name is the bxm_guid of the bridge device */
vps_error
populate_gateway_module_information(vpsdb_resource *rsc, const char* name)
{
        bxm_gw_module_attr_t *gateway;
        vps_error err = VPS_SUCCESS;
        char query[1024] = "select * from bxm_gw_module_attr ";
        uint8_t i;

        /* If name exists, we need information only for that bridge */
        if (name != '\0')
                sprintf(query, "%s where bxm_bridge_guid = \"%s\";",
                                                query, name);
        else
                strcat(query, ";");

        /* Update the resource information with the type */
        rsc->type = VPS_DB_GATEWAY;

        vps_trace(VPS_INFO, "populate_gateway_module_information query: %s",
                                                                        query);
        /* Get the bridges */
        if (VPS_SUCCESS != (err = vpsdb_read(query,
                            process_gateway_module,
                            /* gateway call back function */
                            rsc))) {
                vps_trace(VPS_ERROR, "Could not get gateway information");
                goto out;
        }
out:
        return err;
}


vps_error
populate_bridge_info(vpsdb_resource *rsc, const char *where_clause)
{
        bxm_bd_attr_t *bridge;
        vps_error err = VPS_SUCCESS;
        char query[1024] = "select * from bxm_bridge_device ";
        char tmp_str[1024];
        uint8_t i;
        vpsdb_resource gw_rsc;

        /* If name exists, we need information only for that bridge */
        if (*where_clause)
                sprintf(query, "%s where %s;", query, where_clause);
        else
                strcat(query, ";");

        /* Update the resource information with the type */
        rsc->type = VPS_DB_BRIDGE;

        /* Get the bridges */
        if (VPS_SUCCESS != (err = vpsdb_read(query,
                            process_bridge, /* bridge call back function */
                            rsc))) {
                vps_trace(VPS_ERROR, "Could not get bridge information");
                goto out;
        }

        /* For each bridge, get the gateway */
        for (i = 0; i < rsc->count; i++) {
                bridge = (bxm_bd_attr_t*)rsc->data + i;

                /* reset the gateway resource */
                memset(&gw_rsc, 0, sizeof(gw_rsc));

                /* 'name' parameter should be a NULL terminated string */
                memset(tmp_str, 0, sizeof(tmp_str));

                memcpy(tmp_str, &bridge->_bd_guid,
                                        sizeof(bridge->_bd_guid));

                sprintf(tmp_str,"%d",bridge->_bd_guid);
                /*
                 * For each gateway, the external port status is updated
                 * in the func.
                 */
                populate_gateway_module_information(&gw_rsc, tmp_str);
                /*
                 * Move data from the resource into the bridge. Pointer
                 * assignment. Now, the ownership of the gateway allocation
                 * is with the bridge structure.
                 */
                bridge->_num_gw_module = gw_rsc.count;
                bridge->_gw_module_index = gw_rsc.data;
        }
out:
        return err;
}

int
process_vfabric(void *data, int num_cols, uint8_t **values, char **cols)
{
        vpsdb_resource *rsc = (vpsdb_resource*)data;
	bxm_vfabric_attr_t *vfabric;
        uint32_t i;

        vps_trace(VPS_ENTRYEXIT, "Entering process_vfabric");

        /* Read the existing resrouce count for re-allocation */
        if (NULL == (rsc->data = realloc(rsc->data,
                     sizeof(bxm_vfabric_attr_t) * (rsc->count + 1)))) {
                /*
                 * The block could not be realloc'ed. This can cause serious
                 * problems .Hence we return with a db_error
                 */
                vps_trace(VPS_ERROR, "Could not realloc memory: %d",
                                rsc->count + 1);
                return 1; /* This will propogate with SQL_ABORT */
        }

        /* Go the the vfabric array offset */
        vfabric = rsc->data + (sizeof(bxm_vfabric_attr_t) * rsc->count);

        /* Fill the vfabric structure */
        for (i = 0; i < num_cols; i++) {
                if (strcmp(cols[i], "primary_gw_id") == 0)
                        vfabric->primary_gateway = atoi(values[i]);
		else if (strcmp(cols[i], "id") == 0)
                        vfabric->_vfabric_id = atoi(values[i]);
		else if (strcmp(cols[i], "backup_gw_id") == 0)
                        vfabric->backup_gateway = atoi(values[i]);
                else if (strcmp(cols[i], "name") == 0)
                        memcpy(vfabric->name,(values[i]), 6);
                else if (strcmp(cols[i], "desc") == 0)
                        memcpy(vfabric->desc,(values[i]), 6);
                else if (strcmp(cols[i], "ctx_table_id") == 0)
                        vfabric->_ctx_table_id = atoi(values[i]);
                else if (strcmp(cols[i], "protocol") == 0)
                        vfabric->protocol = atoi(values[i]);
                else if (strcmp(cols[i], "auto_failover") == 0)
                        vfabric->auto_failover = atoi(values[i]);
                else if (strcmp(cols[i], "auto_failback") == 0)
                        vfabric->auto_failback = atoi(values[i]);
        }

        /* After the data is correctly populated, increment the bridge count */
        rsc->count++;
        vps_trace(VPS_INFO, "Vfabric successfully read");

        vps_trace(VPS_ENTRYEXIT, "Leaving process_vfabric");
        return 0; /* Callback must return 0 on success */
}

/*
 * This function populates the vfabric info.
 * TODO It can be further optimized to be into the same calling function
 */

vps_error
populate_vfabric_info(vpsdb_resource *rsc, const char* where_clause)
{
        vps_error err = VPS_SUCCESS;
        bxm_vfabric_attr_t *vfabric;
        char query[1024] = "select * from bxm_vfabric_attr";
        char tmp_str[1024];
        uint8_t i;

        /* If name exists, we need information only for that vfabric */
        if (where_clause)
                sprintf(query, "%s where %s;", query, where_clause);
        else
                strcat(query, ";");

        /* Update the resource information with the type */
        rsc->type = VPS_DB_IO_MODULE;

        /* Get the bridges */
        if (VPS_SUCCESS != (err = vpsdb_read(query,
                            process_vfabric, /* call back function */
                            rsc))) {
                vps_trace(VPS_ERROR, "Could not get vfabric information");
        }

        return err;
}

/*
 * This function will search the database and the max value of the id.
 * Then it will increment the value and return .
 */
int
generate_vfabric_id(void *data,
                int num_cols,
                char **values,
                char **cols)
{
	vpsdb_resource *rsc = (vpsdb_resource*)data;
	bxm_vfabric_attr_t *vfabric = (bxm_vfabric_attr_t *)rsc->data;
        /* For now we are using a simple auto-increment */
        if (values[0])
                vfabric->_vfabric_id = atoi(values[0]) + 1;
        else
                vfabric->_vfabric_id = 1;

        return 0;
}

/*
 * Query: insert into bxm_vadapter_attr_t (id, name, desc, init_type,
 *        protocol) values(...);
 * [in\out] info :Preallocated pointer by the caller. The new object id created
 *                will be returned into the same.
 */
vps_error
add_vfabric(vpsdb_resource *info)
{
        vps_error err = VPS_SUCCESS;
        bxm_vfabric_attr_t *vfabric = (bxm_vfabric_attr_t*)(info->data);

        char get_max_query[128] = "select max(id) from bxm_vfabric_attr;";
        char insert_query[1024] = "insert into bxm_vfabric_attr (id, name, "
                           "desc, protocol, type) values (";

        vps_trace(VPS_ENTRYEXIT, "Entering add_vfabric");

        /* Prepare the query for vfabric add */
        if (VPS_SUCCESS != (err = vpsdb_read(get_max_query,
                              generate_vfabric_id, /* call back function */
                              info))) {
                vps_trace(VPS_ERROR, "Could not get max vfabric id");
                err = VPS_DBERROR;
                goto out;
        }

        /* Insert the vfabric into the table */
        sprintf(insert_query, "%s %d, '%s', '%s', %d, %d);",
			insert_query, vfabric->_vfabric_id,
                        vfabric->name, vfabric->desc, vfabric->protocol, 1);
        if (VPS_SUCCESS != (err = vpsdb_update(insert_query))) {
                vps_trace(VPS_ERROR, "Error adding vfabric to datbase");
                goto out;
        }

out:
        vps_trace(VPS_ENTRYEXIT, "Leaving add_vfabric");
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
        vps_trace(VPS_INFO, "IO Module successfully read");

        vps_trace(VPS_ENTRYEXIT, "Leaving process_io_module");
        return 0; /* Callback must return 0 on success */
}
/*
 * This function populates the io_module struct
 */
vps_error
populate_io_module_info(vpsdb_resource *rsc, const char* where_clause)
{
        vps_error err = VPS_SUCCESS;
        vpsdb_io_module_t *io_module;
        char query[1024] = "select * from bxm_io_modules";
        char tmp_str[1024];
        uint8_t i;

        /* If name exists, we need information only for that io_module */
        if (*where_clause)
                sprintf(query, "%s where %s;", query, where_clause);
        else
                strcat(query, ";");

        /* Update the resource information with the type */
        rsc->type = VPS_DB_IO_MODULE;

        /* Get the bridges */
        if (VPS_SUCCESS != (err = vpsdb_read(query,
                            process_io_module, /* call back function */
                            rsc))) {
                vps_trace(VPS_ERROR, "Could not get io_module information");
        }

        return err;
}

/*
 * This function adds the io_module struct to the database
 * TODO :NOT TESTED
 */

vps_error
add_io_module(vpsdb_resource *info)
{
        vps_error err = VPS_SUCCESS;
        vpsdb_io_module_t *io_module = (vpsdb_io_module_t*)info->data;
        vpsdb_io_module_t *ptr = NULL;
        uint32_t i, j;
        uint8_t tmp[32];
        char insert_io_module_query[1024] = "insert into bxm_io_modules(name, "
		"type, mac, guid, num_vhba, num_vnic, "
		"slot, port, supported_port) values (";

        vps_trace(VPS_ENTRYEXIT, "Entering add_io_module");

	/* Iterate each io_module info */
	for (i = 0; i < info->count; i++) {
		/* Set the correct io_module data-structure offset */
		ptr = io_module + i;

		/* Formulate the query to insert the io_module into the DB */
		/* node name */
		memset(tmp, 0, sizeof(tmp));
		memcpy(tmp, ptr->name, 8);
		sprintf(insert_io_module_query, "%s\"%s\"",
				insert_io_module_query, tmp);

		/* Type */
		sprintf(insert_io_module_query, "%s, %d",
				insert_io_module_query, ptr->type);

		/* Mac address */
		memset(tmp, 0, sizeof(tmp));
		memcpy(tmp, ptr->mac, 6);
		sprintf(insert_io_module_query, "%s, \"%s\"",
				insert_io_module_query, tmp);

		/* node name */
		memset(tmp, 0, sizeof(tmp));
		memcpy(tmp, ptr->guid, 8);
		sprintf(insert_io_module_query, "%s\"%s\"",
				insert_io_module_query, tmp);

		/* Num of vhba */
		sprintf(insert_io_module_query, "%s, %d",
				insert_io_module_query, ptr->num_vhba);

		/* Num of vnic*/
		sprintf(insert_io_module_query, "%s, %d",
				insert_io_module_query, ptr->num_vnic);

		/* Slot */
		sprintf(insert_io_module_query, "%s, %d",
				insert_io_module_query, ptr->slot);
		/* Port */
		sprintf(insert_io_module_query, "%s, %d",
				insert_io_module_query, ptr->port);
		/* Supported speed  */
		sprintf(insert_io_module_query, "%s, %d",
				insert_io_module_query, ptr->supported_speed);



		strcat(insert_io_module_query, ");");

		vps_trace(VPS_INFO, "insert io_module query: %s",
				insert_io_module_query);
	}
	/** Execute the query to insert io_module into the DB ***/
	if (VPS_SUCCESS != (err = vpsdb_update(insert_io_module_query))) {
		vps_trace(VPS_ERROR,
				"Error in adding io_module to the database");
	}


	vps_trace(VPS_ENTRYEXIT, "Leaving add_io_module: %x", err);
	return err;

}

vps_error
validate_add_io_module(vpsdb_resource *host)
{
	vps_error err = VPS_SUCCESS;
	vps_trace(VPS_ENTRYEXIT, "Entering validate_add_io_module");

	/* Validate that the info indeed contains the host */
	if (VPS_DB_VADAPTER == host->type) {
                vps_trace(VPS_ERROR, "Incorrect resource. Host expected");
                err = VPS_DBERROR_INVALID_RESOURCE;
        }

        /* TODO: Any other host validations? */

        vps_trace(VPS_ENTRYEXIT, "Leaving validate_add_io_module: %x", err);
        return err;
}

/*
 * vpsdb_get_resource
 *
 * This routine gets data from the Sqlite database and populate the
 * respective data structures.
 */
vps_error
vpsdb_get_resource(uint32_t type, vpsdb_resource *info, const char *name)
{
        vps_error err = VPS_SUCCESS;

        vps_trace(VPS_ENTRYEXIT, "Entering vpsdb_get_resource");

        /* If resource is a bridge, we need to populate the gateways also */
        if (type == VPS_DB_BRIDGE) {
                if (*name)
                        vps_trace(VPS_INFO, "Get bridge info for: %s", name);

                /* Get all the brige and gateway info */
                if (VPS_SUCCESS != (err = populate_bridge_info(info, name)))
                        goto out;

                vps_trace(VPS_INFO, "Gathered bridge info");
        }
        /* If resource is a host, we need to populate the gateways also */
        else if (type == VPS_DB_IO_MODULE) {
                if (name)
                        vps_trace(VPS_INFO, "Get host info for: %s", name);

                /* Get all the host, CNA and vHBA, vNIC info */
                if (VPS_SUCCESS != (err = populate_io_module_info(info, name)))
                        goto out;

                vps_trace(VPS_INFO, "Gathered host info");
        }
        /* If resource is a host, we need to populate the gateways also */
        else if (type == VPS_DB_GATEWAY)
        {
                if (name)
                        vps_trace(VPS_INFO, "Get Gateway info for: %s", name);

                /* Get all the host, CNA and vHBA, vNIC info */
                if (VPS_SUCCESS !=
                   (err = populate_gateway_information(info, name)))
                        goto out;

                vps_trace(VPS_INFO, "Gathered gateway info");
        }
        else if (type == VPS_DB_VADAPTER)
        {
                if (name)
                        vps_trace(VPS_INFO, "Get vadapter info for: %s", name);

                if (VPS_SUCCESS !=
                   (err = populate_vadapter_information(info, name)))
                        goto out;

                vps_trace(VPS_INFO, "Gathered vadapter info");
        }
        else if (type == VPS_DB_VFABRIC)
        {
                if (name)
                        vps_trace(VPS_INFO, "Get vfabric info for: %s", name);

                if (VPS_SUCCESS !=
                   (err = populate_vfabric_info(info, name)))
                        goto out;

                vps_trace(VPS_INFO, "Gathered fabric info");
        }
out:
        vps_trace(VPS_ENTRYEXIT, "Leaving vpsdb_get_resource");
        return err;
}

/*
 * vpsdb_add_resource
 *
 * This routine adds information to the database. If its a host, it may contain
 * CNAs. Each CNA may in turn contains vHBAs or vNICs. If its a bridge, the
 * gateways associated with the bridge will be added into the database
 * [IN] type : type of the object
 * [IN/OUT] rsc : vps_db resouce pointer. It already has memory allocated.
 */
vps_error
vpsdb_add_resource(uint32_t type, vpsdb_resource *rsc)
{
        vps_error err = VPS_SUCCESS;

        vps_trace(VPS_ENTRYEXIT, "Entering vpsdb_add_resource");

        /* Read the type of resource to be inserted */
        if (type != VPS_DB_IO_MODULE && type != VPS_DB_BRIDGE &&
                        type != VPS_DB_VADAPTER && type != VPS_DB_VFABRIC ) {
                vps_trace(VPS_ERROR, "Invalid type for database insertion: %x",
                                type);
                err = VPS_DBERROR_INVALID;
                goto out;
        }

        /* Add Bridge to database */
        if (type == VPS_DB_BRIDGE) {
                /* validate bridge info */
                if (VPS_SUCCESS != (err = validate_add_bridge(rsc)))
                        goto out;

                if (VPS_SUCCESS != (err = add_bridge(rsc)))
                        goto out;
        }

        /* Add Host to database */
        if (type == VPS_DB_IO_MODULE) {
                /* validate host info */
                if (VPS_SUCCESS != (err = validate_add_io_module(rsc)))
                        goto out;

                vps_trace(VPS_INFO, "Adding host to db: %s",
                                ((vpsdb_io_module_t *)(rsc->data))->name);

                if (VPS_SUCCESS != (err = add_io_module(rsc)))
                        goto out;
        }

        if (type == VPS_DB_VADAPTER) {
                vps_trace(VPS_INFO, "Adding vadapter to db");

                if (VPS_SUCCESS != (err = add_vadapter(rsc)))
                        goto out;
        }

        if (type == VPS_DB_VFABRIC) {
                vps_trace(VPS_INFO, "Adding vFABRIC to db");

                if (VPS_SUCCESS != (err = add_vfabric(rsc)))
                        goto out;
        }

        vps_trace(VPS_INFO, "Resource updated successfully in database");

out:
        vps_trace(VPS_ENTRYEXIT, "Leaving vpsdb_add_resource: %x", err);
	return err;
}

vps_error
vpsdb_edit_resource(uint32_t type, const char * where_clause)
{
        vps_error err = VPS_SUCCESS;
        char query[1024];
	void * stmt;

        vps_trace(VPS_ENTRYEXIT, "Entering vpsdb_edit_resource");

        /* Read the type of resource to be inserted */
        if (type == VPS_DB_VADAPTER) {
                vps_trace(VPS_INFO, "Editing vadapter.");
		sprintf(query,"update bxm_vadapter_attr set %s", where_clause);
        }
        if (type == VPS_DB_VFABRIC) {
                vps_trace(VPS_INFO, "Editing vFABRIC.");
		sprintf(query,"update bxm_vfabric_attr set %s", where_clause);
        }

        if (type == VPS_DB_EN_VADAPTER) {
                vps_trace(VPS_INFO, "Editing vadapter EN attr.");
		sprintf(query,"update bxm_vadapter_en_attr set %s",
                                where_clause);
        }

        stmt = vfmdb_prepare_query(query, NULL, NULL);
        if (!stmt) {
                vps_trace(VPS_ERROR," Cannot prepare sqlite3 statement");
                err = VPS_DBERROR;
                goto out;
        }

        if (VPS_SUCCESS != (err = vfmdb_execute_query(stmt, NULL, NULL))) {
                vps_trace(VPS_ERROR, "Error updating resource to datbase");
                err = VPS_DBERROR;
                goto out;
        }

        vps_trace(VPS_INFO, "Resource updated successfully in database");

out:
        vps_trace(VPS_ENTRYEXIT, "Leaving vpsdb_edit_resource: %x", err);
	return err;
}
