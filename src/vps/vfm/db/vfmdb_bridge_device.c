
/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#include <vfmapi_common.h>


/*
 * This routine is called ONCE for each record from the result set - Bridge
 */
int
process_bridge(void *data, int num_cols, uint8_t **values, char **cols)
{
        vpsdb_resource *rsc = (vpsdb_resource*)data;
        vfm_bd_attr_t *bridge;
        uint32_t i;
       
        vps_trace(VPS_ENTRYEXIT, "Entering process_bridge. Count: %d",
                        rsc->count);

        /* Read the existing resrouce count for re-allocation */
        if (NULL == (rsc->data = realloc(rsc->data,
                     sizeof(vfm_bd_attr_t) * (rsc->count + 1)))) {
                /*
                 * The block could not be realloc'ed. This can cause serious
                 * problems .Hence we return with a db_error
                 */
                vps_trace(VPS_ERROR, "Could not realloc memory: %d",
                                rsc->count + 1);
                return 1; /* This will propogate with SQL_ABORT */
        }

        /* Go the the bridge array offset */
        bridge = rsc->data + (sizeof(vfm_bd_attr_t) * rsc->count);
        memset(bridge, 0 , sizeof(vfm_bd_attr_t));
        /* Fill the bridge structure */
        for (i = 0; i < num_cols; i++) {
                if (strcmp(cols[i], "guid") == 0)
                        memcpy(&bridge->_bd_guid , values[i], sizeof(vfm_bd_guid_t));
                else if (strcmp(cols[i], "vfm_guid") == 0)
                        memcpy(&bridge->_vfm_guid, values[i], sizeof(vfm_guid_t));
                else if (strcmp(cols[i], "desc") == 0) {
                        memcpy(bridge->desc, values[i], 64);
                }
                else if (strcmp(cols[i], "firmware_version") == 0) {
                        memcpy(bridge->_firmware_version, values[i], 64);
                }
        }
#ifdef VFM_NO_OBJ_CACHE
        /*
         * For now, populate all transietn information with dummy data
         */
        bridge->_last_keep_alive = time(NULL);
        bridge->_state = 5;
        bridge->running_mode = 1;
#endif

        /* After the data is correctly populated, increment the bridge count */
        rsc->count++;
        vps_trace(VPS_INFO, "Bridge successfully read");
        vps_trace(VPS_ENTRYEXIT, "Leaving process_bridge");
        return 0; /* Callback must return 0 on success */
}

/* Process the gatewaye module information from the database */

int
process_gateway_module(void *data, int num_cols, uint8_t **values, char **cols)
{
        vpsdb_resource *rsc = (vpsdb_resource*)data;
        uint8_t *gw_module;
        uint32_t temp;
        uint8_t i;
        vps_trace(VPS_ENTRYEXIT, "Entering process_gateway. Count: %d",
                rsc->count);

        if (NULL == (rsc->data = realloc(rsc->data,
                     sizeof(vfm_gw_module_index_t) * (rsc->count + 1)))) {
                /*
                 * The block could not be realloc'ed. This can cause serious
                 * problems. Hence we return with a db_error
                 */
                vps_trace(VPS_ERROR, "Could not realloc memory: %d",
                                rsc->count + 1);
                return 1; /* This will propogate with SQL_ABORT */
        }

        /* Go the the bridge array offset */
        gw_module = rsc->data + (sizeof(vfm_gw_module_index_t) * rsc->count);

        /* Fill the bridge structure */
        for (i = 0; i < num_cols; i++) {
                if (strcmp(cols[i], "gw_module_id") == 0) {
                        memset(gw_module, 0, sizeof(vfm_gw_module_index_t));
                        memcpy(gw_module, values[i], 
                                sizeof(vfm_gw_module_index_t));
                }
        }
        /* After the data is correctly populated, increment the bridge count */
        rsc->count++;
        vps_trace(VPS_INFO, "Gateway successfully read");
        vps_trace(VPS_ENTRYEXIT, "Leaving processs_gateway.");
        return 0;
}


/* 
 * This function will prepare query for getting the gw module information 
 * from the database based on the bridge guid
 */

vps_error
populate_gateway_module_information(vfm_bd_guid_t bd_guid, vpsdb_resource *rsc)
{
        vfm_gw_module_attr_t *gateway;
        vps_error err = VPS_SUCCESS;
        char fmt[MAX_ARGS];
        void **args;
        void *stmt;
        char query[1024] = "select * from vfm_gw_module_attr ";
        uint8_t i = 0;
        int count = 0;

        memset(fmt, 0, sizeof(fmt));

        args = (void **) malloc(MAX_ARGS * sizeof(void *));

        /* If name exists, we need information only for that bridge */
        if (bd_guid != 0) {
                add_query_parameters(query, count, "vfm_bridge_guid",
                                "?1", Q_NAMED_PARAM);
                sprintf(fmt, "g");
                args[i] = &bd_guid;
                count++;
        }

        /* Update the resource information with the type */
        rsc->type = VPS_DB_GATEWAY;
        query[strlen(query)] = ';' ; 
        
        stmt = vfmdb_prepare_query(query, fmt, args);

        if (!stmt) {
                vps_trace(VPS_ERROR," Cannot prepare sqlite3 statement");
                err = VPS_DBERROR;
                goto out;
        }
        /* Get the bridges */
        if (VPS_SUCCESS != (err = vfmdb_execute_query(stmt,
                            process_gateway_module,
                            /* gateway call back function */
                            rsc))) {
                vps_trace(VPS_ERROR, "Could not get gateway information");
                goto out;
        }
        free(args);
out:
        return err;
}

/* 
 * This function will populate detail bridge information from the database.
 * and process the bridge guid to get the gateway module information.       
 */

vps_error
populate_bridge_information(vfm_bd_attr_t attr, 
                                vfm_bd_attr_bitmask_t bitmask,
                                vpsdb_resource *vps_rsc)
{
        
        char query[1024] = "select * from vfm_bridge_device ";
        vfm_bd_guid_t bd_guid;
        vfm_bd_attr_t *bridge;
        char fmt[MAX_ARGS];
        void **args;
        void *stmt;
        vpsdb_resource gw_rsc;
        int count = 0 , i = 0;
        memset(fmt, 0, sizeof(fmt));
        vps_error err = VPS_SUCCESS;

        args = (void **) malloc(MAX_ARGS * sizeof(void *));
        memset(&gw_rsc, 0, sizeof(vpsdb_resource));

        if (bitmask.guid) {
                add_query_parameters(query, count, "guid",
                                "?1", Q_NAMED_PARAM);
                sprintf(fmt, "g");
                args[i++] = &attr._bd_guid;
                count++;
        }
        if (bitmask.desc) {
                add_query_parameters(query, count, "`desc`",
                                attr.desc, Q_UINT8);
                count++;
        }
        if (bitmask.firmware_version) {
                add_query_parameters(query, count, "firmware_version",
                                attr._firmware_version, Q_UINT8);
                count++;
        }
        if(bitmask.vfm_guid) {
                add_query_parameters(query, count, "vfm_guid",
                                "?2", Q_NAMED_PARAM);
                sprintf(fmt, "%sg", fmt);
                args[i++] = &attr._vfm_guid;
                count++;
        }
        query[strlen(query)] = ';' ; 

        stmt = vfmdb_prepare_query(query, fmt, args);

        if (!stmt) {
                vps_trace(VPS_ERROR," Cannot prepare sqlite3 statement");
                err = VPS_DBERROR;
                goto out;
        }
        if (VPS_SUCCESS != (err = vfmdb_execute_query(stmt,
                                        process_bridge,
                                        vps_rsc))) {
                vps_trace(VPS_ERROR, "Could not get EN properties ");
                err = VPS_DBERROR;
                goto out;
        }

        /* For each bridge, get the gateway */
        for (i = 0; i < vps_rsc->count; i++) {
                bridge = ((vfm_bd_attr_t*)(vps_rsc->data)) + i;

                /* reset the gateway resource */
                memset(&gw_rsc, 0, sizeof(gw_rsc));
                memcpy(&bd_guid, &(bridge->_bd_guid), sizeof(vfm_bd_guid_t));

                /*
                 * For each gateway, the external port status is updated
                 * in the func.
                 */
                populate_gateway_module_information(bd_guid, &gw_rsc);
                /*
                 * Move data from the resource into the bridge. Pointer
                 * assignment. Now, the ownership of the gateway allocation
                 * is with the bridge structure.
                 */
                bridge->_num_gw_module = gw_rsc.count;
                bridge->_gw_module_index = gw_rsc.data;
        }
        free(args);
out:
        return err;
}
