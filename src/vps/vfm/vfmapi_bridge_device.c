/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */


/* TODO : Comments */

#include <vfmapi_common.h>
#include <vfmdb.h>
#include <common.h>
#include <vfmdb_bridge_device.h>

vfm_error_t
pack_bridge_data(vpsdb_resource *vps_rsc, res_packet *op_data)
{
        vps_error err = VPS_SUCCESS;
        int i = 0, count = 0;
        vfm_bd_attr_t *bridge;
        uint32_t gw_module_size = 0;
        uint8_t *offset = op_data->data;

        memset(op_data, 0, sizeof(res_packet));

        op_data->data = (void *) malloc(sizeof(uint32_t));

        memcpy(op_data->data, &vps_rsc->count, sizeof(uint32_t));

        op_data->size += sizeof(uint32_t);

        for(i = 0; i < vps_rsc->count; i++) {

                bridge = ((vfm_bd_attr_t*)(vps_rsc->data)) + i;
                gw_module_size = (sizeof(vfm_gw_module_index_t) *
                                         bridge->_num_gw_module);
                
                op_data->size += (sizeof(vfm_bd_attr_t) + gw_module_size);

                op_data->data = realloc(op_data->data, op_data->size);

                offset = (op_data->data + op_data->size) - 
                           (sizeof(vfm_bd_attr_t) + gw_module_size);

                memcpy(offset, bridge, sizeof(vfm_bd_attr_t));
                offset += sizeof(vfm_bd_attr_t);

                memcpy(offset, bridge->_gw_module_index, gw_module_size);
                offset += gw_module_size;
        }
out:
        return err;
}


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
        char fmt[MAX_ARGS];
        void **args;


        void *stmt;

        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering process_vfm_bd_select_inventory");
        
        memset(&attr, 0, sizeof(vfm_bd_attr_t));
        memset(&bitmask, 0, sizeof(vfm_bd_attr_bitmask_t));
        memset(fmt, 0, sizeof(fmt));
        memset(&vps_res, 0, sizeof(vpsdb_resource));

        get_api_tlv(buff, ret_pos, &attr);
        get_api_tlv(buff, ret_pos, &bitmask);

        /* Maximum unicode arguments are 2 */
        args = (void **) malloc(MAX_ARGS * sizeof(void *));

        
        err = populate_bridge_information(attr, bitmask, &vps_res);

        if (err != VPS_SUCCESS) {
                vps_trace(VPS_ERROR, "Could not get EN properties ");
                err = VPS_DBERROR;
                goto out;
        }
        /* pack the bridge data*/
        pack_bridge_data(&vps_res, op_arg);
        free(vps_res.data);
        free(args);
out:
        vps_trace(VPS_ENTRYEXIT, "Leaving process_vfm_create_vadpter");
        return err;
}

vfm_error_t
process_vfm_bd_query_general_attr(uint8_t* buff, uint32_t* ret_pos,
                res_packet* op_arg)
{
        char query[1024] = "select * from vfm_bridge_device ";
        vfm_bd_attr_t attr;
        vfm_bd_attr_bitmask_t bitmask;
        vpsdb_resource vps_res;
        uint32_t count = 0;
        char fmt[MAX_ARGS];
        void **args;
        void *stmt;
        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering process_vfm_bd_select_inventory");

        memset(&attr, 0, sizeof(vfm_bd_attr_t));
        memset(&bitmask, 0, sizeof(vfm_bd_attr_bitmask_t));

        get_api_tlv(buff, ret_pos, &attr._bd_guid);
        get_api_tlv(buff, ret_pos, &bitmask);

        memset(&vps_res, 0, sizeof(vpsdb_resource));

        memset(fmt, '\0', sizeof(fmt));

        err = populate_bridge_information(attr, bitmask, &vps_res);

        if (err != VPS_SUCCESS) {
                vps_trace(VPS_ERROR, "Could not get EN properties ");
                err = VPS_DBERROR;
                goto out;
        }

        op_arg->size = sizeof(vfm_bd_attr_t) * vps_res.count;
        op_arg->data = vps_res.data;
        free(args);
out:
        vps_trace(VPS_ENTRYEXIT, "Leaving process__vfm_bd_query_general_attr");
        return err;
}

