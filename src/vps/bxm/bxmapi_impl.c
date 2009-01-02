/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */

#include <bxmapi_common.h>
#include <common.h>

/*
 * This function will create the message for the request .
 * The op parameter will be filled up (if any)
 * ctrl_hdr      [IN] : Control header from the request
 * buff      [IN/OUT] : This contains the op parameter that needs to
 *                      be sent to the client.
 */
bxm_error_t
bxm_marshall_response(res_packet *buff, bxmapi_ctrl_hdr *ctrl_hdr,
                res_packet *pack, uint32_t no_of_args)
{
        uint8_t *offset;
        api_tlv tlv;

        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering bxm_marshall_response");

        ctrl_hdr->length = (sizeof(bxmapi_ctrl_hdr) + buff->size +
                        (2 * no_of_args));

        pack->data = (uint8_t *)malloc(ctrl_hdr->length);
        offset = pack->data;
        pack->size = ctrl_hdr->length;

        memcpy(offset, ctrl_hdr, sizeof(bxmapi_ctrl_hdr));
        offset += sizeof(bxmapi_ctrl_hdr);

        switch(ctrl_hdr->opcode) {
                case BXM_CREATE:
                        /* TYPE = 1 , bxm_vadapter_id_t */
                        create_api_tlv(TLV_INT, buff->size,
                                        buff->data, &offset);
                        break;

                case BXM_EDIT:
                case BXM_ONLINE:
                        create_api_tlv(TLV_INT, buff->size,
                                        buff->data, &offset);
                        break;

                case BXM_EDIT_PROTOCOL_ATTR:
                        create_api_tlv(TLV_INT, buff->size,
                                        buff->data, &offset);
                        break;
                case BXM_QUERY_INVENTORY:
                        /* TYPE = Vadapter_id_t array */
                        memcpy(offset, buff->data, buff->size);
                        break;

                case BXM_QUERY:
                        create_api_tlv(TLV_VADP_ATTR, buff->size,
                                        buff->data, &offset);
                        break;
        }

//	free(buff->data);
        vps_trace(VPS_ENTRYEXIT, "Leaving bxm_marshall_response");
        return err;
}

/*
 * This function will receive the message from the socket.
 * then process the message and do the required processing.
 * Once the processing is done, It will create the response
 * using the marshall_response() function into TLV's and
 * then send it on the socket to which it is bound.
 */
bxm_error_t
unmarshall_request(void *buff, uint32_t size, res_packet * pack)
{
        bxmapi_ctrl_hdr ctrl_hdr;
        uint32_t ret_pos = 0, num;
        uint32_t op_arg;
        res_packet op_data;

        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering unmarshall_request");

        /* Then process the control header */
        read_api_ctrl_hdr(buff, size, &ret_pos, &ctrl_hdr);
        switch (ctrl_hdr.mod_id) {
                case BXMAPI_VADAPTER:
                        switch(ctrl_hdr.opcode) {
                                case BXM_CREATE:
                                        process_bxm_create_vadpter(buff,
                                                        &ret_pos, &op_data);
                                        num = 1;
                                        bxm_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, num);

                                        break;

                                case BXM_EDIT:
                                        err = process_bxm_edit_vadpter(buff,
                                                        &ret_pos, &op_data);
                                        bxm_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, 1);
                                        break;
                                case BXM_EDIT_PROTOCOL_ATTR:
                                        err = process_bxm_edit_en_attr(buff,
                                                        &ret_pos, &op_data);
                                        bxm_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, 1);
                                        break;
                                case BXM_QUERY_INVENTORY:
                                        process_bxm_query_inventory(buff,
                                                        &ret_pos, &op_data);

                                        bxm_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, 2);
                                        break;
                                case BXM_QUERY:
                                        process_bxm_query_vadapter(buff,
                                                        &ret_pos, &op_data);

                                        bxm_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, 1);
                                        break;
                        }
                        break;
                case BXMAPI_VFABRIC:
                        switch(ctrl_hdr.opcode) {


                                case BXM_CREATE:
                                        process_bxm_create_vfabric(buff,
                                                        &ret_pos, &op_data);
                                        num = 1;
                                        bxm_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, num);
                                        break;
                                case BXM_EDIT:
                                        err = process_bxm_edit_vfabric(buff,
                                                        &ret_pos, &op_data);
                                        bxm_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, 1);
					break;
				case BXM_ONLINE:
					process_bxm_vfabric_online(buff,
							&ret_pos, &op_data);
                                        bxm_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, 1);
					break;
                        }
                        break;
                case BXMAPI_BRIDGE_DEVICE:
                        switch(ctrl_hdr.opcode) {

                                case BXM_QUERY_INVENTORY:
                                        process_bxm_bd_select_inventory(buff,
                                                        &ret_pos, &op_data);
                                        bxm_bridge_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, 2);
                                case BXM_QUERY:
                                        process_bxm_bd_query_general_attr(buff,
                                                        &ret_pos, &op_data);
                                        bxm_bridge_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, 1);
                        }
                        break;

        }

	/** TODO: Fix the memory corruption */
	//free(op_data.data);
        vps_trace(VPS_ENTRYEXIT, "Leaving unmarshall_request");
        return err;
}
