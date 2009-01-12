/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */


/* TODO : Comments */

#include <vfmapi_common.h>
#include <vfmdb.h>
#include <common.h>
#include <vfmdb_bridge_device.h>

/*
 * [IN]  *buff  : Contains the values of the TLVs.
 * [IN]  *ret_pos : contains the value of the offset
 * [OUT] *op_arg : any paraeters that is to given as output.
 */

vfm_error_t
process_vfm_bd_select_inventory(uint8_t *buff, uint32_t *ret_pos,
                res_packet *op_arg)
{
        char query[1024] = "select * from vfm_bridge_device ";
        vfm_bd_attr_t attr;
        vfm_bd_attr_bitmask_t bitmask;
        vpsdb_resource vps_res;
        uint32_t count = 0, i = 0;
        char fmt[10];
        void **args;
        void *stmt;

        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering process_vfm_bd_select_inventory");

        get_api_tlv(buff, ret_pos, &attr);
        get_api_tlv(buff, ret_pos, &bitmask);
        memset(fmt, 0, sizeof(fmt));

        /* Maximum unicode arguments are 2 */
        args = (void **) malloc(2 * sizeof(void *));
        if (bitmask.guid) {
                add_query_parameters(query, count, "guid", "?1", Q_NAMED_PARAM);
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
        sprintf(query, "%s;", query);

        if (fmt[0])
                stmt = vfmdb_prepare_query(query, fmt, args);
        else
                stmt = vfmdb_prepare_query(query, fmt, NULL);

        if (!stmt) {
                vps_trace(VPS_ERROR," Cannot prepare sqlite3 statement");
                err = VPS_DBERROR;
                goto out;
        }
        if (VPS_SUCCESS != (err = vfmdb_execute_query(stmt,
                                        process_bridge,
                                        &vps_res))) {
                vps_trace(VPS_ERROR, "Could not get EN properties ");
                err = VPS_DBERROR;
                goto out;
        }

        //vpsdb_get_resource(VPS_DB_BRIDGE, &vps_res, query);

        op_arg->size = sizeof(vfm_bd_attr_t) * vps_res.count;
        op_arg->data = vps_res.data;
out:
        vps_trace(VPS_ENTRYEXIT, "Leaving process_vfm_create_vadpter");
        return err;
}

vfm_error_t
process_vfm_bd_query_general_attr(uint8_t* buff, uint32_t* ret_pos,
                res_packet* op_arg)
{
        char query[1024] = "select * from vfm_bridge_device ";
        vfm_bd_guid_t vfm_guid;
        vfm_bd_attr_bitmask_t bitmask;
        vpsdb_resource vps_res;
        uint32_t count = 0;
        char fmt[10];
        void **args;
        void *stmt;
        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering process_vfm_bd_select_inventory");

        get_api_tlv(buff, ret_pos, &vfm_guid);
        get_api_tlv(buff, ret_pos, &bitmask);

        memset(&vps_res, 0, sizeof(vpsdb_resource));

        memset(fmt, '\0', sizeof(fmt));

        if (bitmask.guid) {
                sprintf(query, "%s where guid = ?1;", query);
                sprintf(fmt, "g");
                args = (void **)malloc(sizeof(void *));
                args[0] = &vfm_guid;
        }

        stmt = vfmdb_prepare_query(query, fmt, args);

        if (!stmt) {
                vps_trace(VPS_ERROR," Cannot prepare sqlite3 statement");
                err = VPS_DBERROR;
                goto out;
        }
        if (VPS_SUCCESS != (err = vfmdb_execute_query(stmt,
                                        process_bridge,
                                        &vps_res))) {
                vps_trace(VPS_ERROR, "Could not get EN properties ");
                err = VPS_DBERROR;
                goto out;
        }

        //vpsdb_get_resource(VPS_DB_BRIDGE, &vps_res, query);

        op_arg->size = sizeof(vfm_bd_attr_t) * vps_res.count;
        op_arg->data = vps_res.data;
out:
        vps_trace(VPS_ENTRYEXIT, "Leaving process__vfm_bd_query_general_attr");
        return err;
}

