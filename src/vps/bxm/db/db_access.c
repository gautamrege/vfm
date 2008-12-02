/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#include <db_access.h>

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

        vps_trace(VPS_ENTRYEXIT, "Entering vpsdb_update");

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
add_gateway(const char* bridge_id, vpsdb_gateway *gw)
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
        sprintf(insert_gateway_query,
                        "%s, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
                        insert_gateway_query, gw->gw_id,
                        gw->ext_protocol, gw->sp, gw->se, gw->flood,
                        gw->egress_secure, gw->ingress_secure,
                        gw->l2_lookup, 0, gw->int_port);
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

        if (VPS_SUCCESS
                != (err = add_external_ports(gw->gw_id, gw->ext_ports))) {
                vps_trace(VPS_ERROR, "Error in adding gateway to database");
                goto out;
        }

out:
        vps_trace(VPS_ENTRYEXIT, "Leaving add_gateway: %x", err);
        return err;
}

vps_error
add_bridge(vpsdb_resource *info)
{
        vps_error err = VPS_SUCCESS;
        vpsdb_bridge *bridge = (vpsdb_bridge*)info->data;
        vpsdb_bridge *ptr = NULL;
        vpsdb_gateway *gw = NULL;
        uint32_t i, j;
        uint8_t tmp[32];
        char insert_bridge_query[1024] = "insert into bridges(mac, "
                "node_name, "
                "db_id, last_bc_mac, "
                "max_recv, vendor, "
                "model_number, "
                "fw_version) values (";

        vps_trace(VPS_ENTRYEXIT, "Entering add_bridge");

        /* Iterate each bridge info */
        for (i = 0; i < info->count; i++) {
                /* Set the correct bridge data-structure offset */
                ptr = bridge + i;

                /*** Formulate the query to insert the bridge into the DB ***/
                /* Mac address */
                memset(tmp, 0, sizeof(tmp));
                memcpy(tmp, ptr->mac, 6);
                sprintf(insert_bridge_query, "%s\"%s\"",
                                insert_bridge_query, tmp);

                /* node name */
                memset(tmp, 0, sizeof(tmp));
                memcpy(tmp, ptr->node_name, 8);
                sprintf(insert_bridge_query, "%s, \"%s\"",
                                insert_bridge_query, tmp);

                /* Database Id */
                sprintf(insert_bridge_query, "%s, %d",
                                insert_bridge_query, ptr->db_id);

                /* last associated Bridge Controller */
                memset(tmp, 0, sizeof(tmp));
                memcpy(tmp, ptr->last_bc_mac, 6);
                sprintf(insert_bridge_query, "%s, \"%s\"",
                                insert_bridge_query, tmp);

                /* Max recv */
                sprintf(insert_bridge_query, "%s, %d", insert_bridge_query,
                                ptr->max_recv);
                /* TODO: Hard-code for now the model, vendor and fw_version */
                sprintf(insert_bridge_query, "%s, \"%s\", \"%s\", \"%s\"",
                                insert_bridge_query, "Mellanox",
                                "ConnectX", "MT_version");
                strcat(insert_bridge_query, ");");

                vps_trace(VPS_INFO, "insert bridge query: %s",
                                insert_bridge_query);

                /** Execute the query to insert bridge into the DB ***/
                if (VPS_SUCCESS != (err = vpsdb_update(insert_bridge_query))) {
                        vps_trace(VPS_ERROR,
                                  "Error in adding bridge to the database");
                        goto out;
                }

                /* Insert gateway information into the database */
                for (j = 0; j < ptr->num_gateways;j++) {
                        /* set the gateway offset */
                        gw = ptr->gateways + j;
                        if (VPS_SUCCESS != (err = add_gateway(ptr->mac, gw))) {
                                vps_trace(VPS_ERROR,
                                  "Error in adding bridge to the database");
                                goto out;
                        }
                }
        }

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
process_gateway(void *data, int num_cols, char **values, char **cols)
{
        vpsdb_resource *rsc = (vpsdb_resource*)data;
        vpsdb_gateway *gateway;
        uint8_t i;
        vps_trace(VPS_ENTRYEXIT, "Entering process_gateway. Count: %d",
                rsc->count);

        if (NULL == (rsc->data = realloc(rsc->data,
                     sizeof(vpsdb_gateway) * (rsc->count + 1)))) {
                /*
                 * The block could not be realloc'ed. This can cause serious
                 * problems. Hence we return with a db_error
                 */
                vps_trace(VPS_ERROR, "Could not realloc memory: %d",
                                rsc->count + 1);
                return 1; /* This will propogate with SQL_ABORT */
        }

        /* Go the the bridge array offset */
        gateway = rsc->data + (sizeof(vpsdb_gateway) * rsc->count);

        /* Fill the bridge structure */
        for (i = 0; i < num_cols; i++) {
                if (strcmp(cols[i], "gw_id") == 0)
                        gateway->gw_id = atoi(values[i]);
                else if (strcmp(cols[i], "flag_ep") == 0)
                        gateway->ext_protocol = atoi(values[i]);
                else if (strcmp(cols[i], "flag_sp") == 0)
                        gateway->sp = atoi(values[i]);
                else if (strcmp(cols[i], "flag_se") == 0)
                        gateway->se = atoi(values[i]);
                else if (strcmp(cols[i], "flag_f") == 0)
                        gateway->flood = atoi(values[i]);
                else if (strcmp(cols[i], "flag_es") == 0)
                        gateway->egress_secure = atoi(values[i]);
                else if (strcmp(cols[i], "flag_is") == 0)
                        gateway->ingress_secure = atoi(values[i]);
                else if (strcmp(cols[i], "flag_l") == 0)
                        gateway->l2_lookup = atoi(values[i]);
                else if (strcmp(cols[i], "ips") == 0)
                        gateway->int_port = atoi(values[i]);
                else if (strcmp(cols[i], "conn_speed") == 0)
                        gateway->connection_speed = atoi(values[i]);
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
        vpsdb_bridge *bridge;
        uint32_t i;

        vps_trace(VPS_ENTRYEXIT, "Entering process_bridge. Count: %d",
                        rsc->count);

        /* Read the existing resrouce count for re-allocation */
        if (NULL == (rsc->data = realloc(rsc->data,
                     sizeof(vpsdb_bridge) * (rsc->count + 1)))) {
                /*
                 * The block could not be realloc'ed. This can cause serious
                 * problems .Hence we return with a db_error
                 */
                vps_trace(VPS_ERROR, "Could not realloc memory: %d",
                                rsc->count + 1);
                return 1; /* This will propogate with SQL_ABORT */
        }

        /* Go the the bridge array offset */
        bridge = rsc->data + (sizeof(vpsdb_bridge) * rsc->count);

        /* Fill the bridge structure */
        for (i = 0; i < num_cols; i++) {
                if (strcmp(cols[i], "mac") == 0)
                        memcpy(bridge->mac, values[i], 6);
                else if (strcmp(cols[i], "node_name") == 0)
                        memcpy(bridge->node_name, values[i], 8);
                else if (strcmp(cols[i], "db_id") == 0)
                        bridge->db_id = atoi(values[i]);
                else if (strcmp(cols[i], "last_bc_mac") == 0)
                        memcpy(bridge->last_bc_mac, values[i], 6);
                else if (strcmp(cols[i], "max_recv") == 0)
                        bridge->max_recv = atoi(values[i]);
                else if (strcmp(cols[i], "vendor") == 0)
                        memcpy(bridge->vendor, values[i], 64);
                else if (strcmp(cols[i], "model_number") == 0)
                        memcpy(bridge->model, values[i], 64);
                else if (strcmp(cols[i], "fw_version") == 0)
                        memcpy(bridge->fw_version, values[i], 64);
        }

        /* After the data is correctly populated, increment the bridge count */
        rsc->count++;
        vps_trace(VPS_INFO, "Bridge successfully read");
        vps_trace(VPS_ENTRYEXIT, "Leaving process_bridge");
        return 0; /* Callback must return 0 on success */
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
populate_gw_external_ports(vpsdb_gateway *gw)
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
        "select id, status from external_ports where gw_id = %d;", gw->gw_id);

        vps_trace(VPS_INFO, "populate_gw_external_ports query: %s", query);

        /* We already know resource information. */
        rsc.count = 15;
        rsc.data = gw->ext_ports;

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
populate_gateway_information(vpsdb_resource *rsc, const char* name)
{
        vpsdb_gateway *bridge;
        vps_error err = VPS_SUCCESS;
        char query[1024] = "select * from gateways ";
        uint8_t i;

        /* If name exists, we need information only for that bridge */
        if (name)
                sprintf(query, "%s where bridge_id = \"%s\";", query, name);
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
                populate_gw_external_ports((vpsdb_gateway*)rsc->data + i);
        }
out:
        return err;
}

vps_error
populate_bridge_info(vpsdb_resource *rsc, const char* name)
{
        vpsdb_bridge *bridge;
        vps_error err = VPS_SUCCESS;
        char query[1024] = "select * from bridges ";
        char tmp_str[1024];
        uint8_t i;
        vpsdb_resource gw_rsc;

        /* If name exists, we need information only for that bridge */
        if (name)
                sprintf(query, "%s where mac = \"%s\";", query, name);
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
                bridge = (vpsdb_bridge*)rsc->data + i;

                /* reset the gateway resource */
                memset(&gw_rsc, 0, sizeof(gw_rsc));

                /* 'name' parameter should be a NULL terminated string */
                memset(tmp_str, 0, sizeof(tmp_str));
                memcpy(tmp_str, bridge->mac, 6);
                /*
                 * For each gateway, the external port status is updated
                 * in the func.
                 */
                populate_gateway_information(&gw_rsc, tmp_str);
                /*
                 * Move data from the resource into the bridge. Pointer
                 * assignment. Now, the ownership of the gateway allocation
                 * is with the bridge structure.
                 */
                bridge->num_gateways = gw_rsc.count;
                bridge->gateways = gw_rsc.data;
        }
out:
        return err;
}

vps_error
populate_host_info(vpsdb_resource *info, const char* name)
{
        vps_error err = VPS_SUCCESS;
        return err;
}

vps_error
add_host(vpsdb_resource *info)
{
        vps_error err = VPS_SUCCESS;
        return err;
}

vps_error
validate_add_host(vpsdb_resource *host)
{
        vps_error err = VPS_SUCCESS;
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
                if (name)
                        vps_trace(VPS_INFO, "Get bridge info for: %s", name);

                /* Get all the brige and gateway info */
                if (VPS_SUCCESS != (err = populate_bridge_info(info, name)))
                        goto out;

                vps_trace(VPS_INFO, "Gathered bridge info");
        }
        /* If resource is a host, we need to populate the gateways also */
        else if (type == VPS_DB_HOST) {
                if (name)
                        vps_trace(VPS_INFO, "Get host info for: %s", name);

                /* Get all the host, CNA and vHBA, vNIC info */
                if (VPS_SUCCESS != (err = populate_host_info(info, name)))
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
 */
vps_error
vpsdb_add_resource(uint32_t type, vpsdb_resource *rsc)
{
        vps_error err = VPS_SUCCESS;

        vps_trace(VPS_ENTRYEXIT, "Entering vpsdb_add_resource");

        /* Read the type of resource to be inserted */
        if (type != VPS_DB_HOST && type != VPS_DB_BRIDGE) {
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

                vps_trace(VPS_INFO, "Adding bridge to db: %s",
                                ((vpsdb_bridge*)(rsc->data))->node_name);

                if (VPS_SUCCESS != (err = add_bridge(rsc)))
                        goto out;
        }

        /* Add Host to database */
        if (type == VPS_DB_HOST) {
                /* validate host info */
                if (VPS_SUCCESS != (err = validate_add_host(rsc)))
                        goto out;

                vps_trace(VPS_INFO, "Adding host to db: %s",
                                ((vpsdb_host*)(rsc->data))->name);

                if (VPS_SUCCESS != (err = add_host(rsc)))
                        goto out;
        }

        vps_trace(VPS_INFO, "Resource updated successfully in database");

out:
        vps_trace(VPS_ENTRYEXIT, "Leaving vpsdb_add_resource: %x", err);
}
