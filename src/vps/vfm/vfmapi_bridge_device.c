/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */


/* TODO : Comments */

#include <vfmapi_common.h>
#include <vfmdb.h>
#include <common.h>

/*
 * [IN]  *buff  : Contains the values of the TLVs.
 * [IN]  *ret_pos : contains the value of the offset
 * [OUT] *op_arg : any paraeters that is to given as output.
 */




vfm_error_t
process_vfm_bd_select_inventory(uint8_t *buff, uint32_t *ret_pos,
                res_packet *op_arg)
{
        char query[1024];
        vfm_bd_attr_t attr;
        vfm_bd_attr_bitmask_t bitmask;
        vpsdb_resource vps_res;
        uint32_t count = 0;
        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering process_vfm_bd_select_inventory");

        get_api_tlv(buff, ret_pos, &attr);
        get_api_tlv(buff, ret_pos, &bitmask);
        memset(query, 0, sizeof(query));

        if (bitmask.guid) {
                add_query_parameters(query, count, "vfm_guid",
                                                 &attr._bd_guid, Q_UINT32);
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
        vpsdb_get_resource(VPS_DB_BRIDGE, &vps_res, query);

        op_arg->size = sizeof(vfm_bd_attr_t) * vps_res.count;
        op_arg->data = vps_res.data;
}

vfm_error_t
process_vfm_bd_query_general_attr(uint8_t* buff, uint32_t* ret_pos,
                                               res_packet* op_arg)
{
        char query[1024];
        vfm_bd_guid_t vfm_guid;
        vfm_bd_attr_bitmask_t bitmask;
        vpsdb_resource vps_res;
        uint32_t count = 0;
        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering process_vfm_bd_select_inventory");

        get_api_tlv(buff, ret_pos, &vfm_guid);
        get_api_tlv(buff, ret_pos, &bitmask);

        memset(query, 0, sizeof(query));
        memset(&vps_res, 0, sizeof(vpsdb_resource));

        if (bitmask.guid) {
                add_query_parameters(query, count, "vfm_guid",
                                        &vfm_guid, Q_UINT32);
                count++;
        }
        vpsdb_get_resource(VPS_DB_BRIDGE, &vps_res, query);

        op_arg->size = sizeof(vfm_bd_attr_t) * vps_res.count;
        op_arg->data = vps_res.data;
}


vfm_error_t
vfm_bridge_marshall_response(res_packet *buff, vfmapi_ctrl_hdr *ctrl_hdr,
                res_packet *pack, uint32_t no_of_args)
{
        uint8_t *message, *offset;
        api_tlv tlv;

        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering vfm_vadpter_marshall_response");

        ctrl_hdr->length = (sizeof(vfmapi_ctrl_hdr) + buff->size +
                        (2 * no_of_args));

        pack->data = (uint8_t *)malloc(ctrl_hdr->length);
        offset = pack->data;

        pack->size = ctrl_hdr->length;

        memcpy(offset, ctrl_hdr, sizeof(vfmapi_ctrl_hdr));
        offset += sizeof(vfmapi_ctrl_hdr);

        switch (ctrl_hdr->opcode) {
                case VFM_CREATE:
                        /* TYPE = 1 , vfm_vadapter_id_t */
                        create_api_tlv(TLV_INT, buff->size,
                                        buff->data, &offset);
                        break;

                case VFM_EDIT:
                        create_api_tlv(TLV_CHAR, buff->size,
                                        buff->data, &offset);
                        break;

                case VFM_QUERY_INVENTORY:
                        /* TYPE = Vadapter_id_t array */
                        memcpy(offset, buff->data, buff->size);
                        break;

                case VFM_QUERY:
                        create_api_tlv(TLV_BD_ATTR, buff->size,
                                        buff->data, &offset);
                        break;
        }
        free(buff->data);
        vps_trace(VPS_ENTRYEXIT, "Leaving vfm_vadpter_marshall_response");
        return err;
}
