/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */

#include <bxmapi_common.h>
#include <db_access.h>
#include <common.h>

/*
 * This function processes the message sent by the client.
 * It then converts it into a proper structure and then gives it to the
 * db access layer to do the functionality.
 *
 * [IN]  buff  : Contains the values of the TLVs.
 * [IN]  ret_pos : contains the value of the offset
 * [OUT] op_arg : any paraeters that is to given as output.
 */




bxm_error_t
process_bxm_bd_select_inventory(uint8_t *buff, uint32_t *ret_pos,
                res_packet *op_arg)
{
        char query[1024];
        bxm_bd_attr_t attr;
        bxm_bd_attr_bitmask_t bitmask;
        vpsdb_resource vps_res;
        uint32_t count = 0;
        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering process_bxm_bd_select_inventory");

        get_api_tlv(buff, ret_pos, &attr);
        get_api_tlv(buff, ret_pos, &bitmask);
        memset(query, 0, sizeof(query));

        if (bitmask.guid) {
                add_query_parameters(query, count, "bxm_guid",
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

        op_arg->size = sizeof(bxm_bd_attr_t) * vps_res.count;
        op_arg->data = vps_res.data;
}

bxm_error_t
process_bxm_bd_query_general_attr(uint8_t* buff, uint32_t* ret_pos,
                                               res_packet* op_arg)
{
        char query[1024];
        bxm_bd_guid_t bxm_guid;
        bxm_bd_attr_bitmask_t bitmask;
        vpsdb_resource vps_res;
        uint32_t count = 0;
        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering process_bxm_bd_select_inventory");

        get_api_tlv(buff, ret_pos, &bxm_guid);
        get_api_tlv(buff, ret_pos, &bitmask);

        memset(query, 0, sizeof(query));
        memset(&vps_res, 0, sizeof(vpsdb_resource));

        if (bitmask.guid) {
                add_query_parameters(query, count, "bxm_guid",
                                        &bxm_guid, Q_UINT32);
                count++;
        }
        vpsdb_get_resource(VPS_DB_BRIDGE, &vps_res, query);

        op_arg->size = sizeof(bxm_bd_attr_t) * vps_res.count;
        op_arg->data = vps_res.data;
}


bxm_error_t
bxm_bridge_marshall_response(res_packet *buff, bxmapi_ctrl_hdr *ctrl_hdr,
                res_packet *pack, uint32_t no_of_args)
{
        uint8_t *message, *offset;
        api_tlv tlv;

        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering bxm_vadpter_marshall_response");

        ctrl_hdr->length = (sizeof(bxmapi_ctrl_hdr) + buff->size +
                        (2 * no_of_args));

        pack->data = (uint8_t *)malloc(ctrl_hdr->length);
        offset = pack->data;

        pack->size = ctrl_hdr->length;

        memcpy(offset, ctrl_hdr, sizeof(bxmapi_ctrl_hdr));
        offset += sizeof(bxmapi_ctrl_hdr);

        switch (ctrl_hdr->opcode) {
                case BXM_CREATE:
                        /* TYPE = 1 , bxm_vadapter_id_t */
                        create_api_tlv(TLV_INT, buff->size,
                                        buff->data, &offset);
                        break;

                case BXM_EDIT:
                        create_api_tlv(TLV_CHAR, buff->size,
                                        buff->data, &offset);
                        break;

                case BXM_QUERY_INVENTORY:
                        /* TYPE = Vadapter_id_t array */
                        memcpy(offset, buff->data, buff->size);
                        break;

                case BXM_QUERY:
                        create_api_tlv(TLV_BD_ATTR, buff->size,
                                        buff->data, &offset);
                        break;
        }
        free(buff->data);
        vps_trace(VPS_ENTRYEXIT, "Leaving bxm_vadpter_marshall_response");
        return err;
}
