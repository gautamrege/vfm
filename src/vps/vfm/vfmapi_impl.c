/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */

#include <vfmapi_common.h>
#include <common.h>





/*
 * This function will create the message for the request .
 * The op parameter will be filled up (if any)
 * ctrl_hdr      [IN] : Control header from the request
 * buff      [IN/OUT] : This contains the op parameter that needs to
 *                      be sent to the client.
 */
vfm_error_t
vfm_marshall_response(res_packet *buff, vfmapi_ctrl_hdr *ctrl_hdr,
                res_packet *pack, uint32_t no_of_args)
{
        uint8_t *offset;
        api_tlv tlv;

        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering vfm_marshall_response");

        ctrl_hdr->length = (sizeof(vfmapi_ctrl_hdr) + buff->size +
                        sizeof(uint32_t) + (TLV_SIZE * no_of_args));

        pack->data = (uint8_t *)malloc(ctrl_hdr->length);
        offset = pack->data;
        pack->size = ctrl_hdr->length;

        memcpy(offset, ctrl_hdr, sizeof(vfmapi_ctrl_hdr));
        offset += sizeof(vfmapi_ctrl_hdr);

        switch(ctrl_hdr->opcode) {
                case VFM_CREATE:
                        /* TYPE = 1 , vfm_vadapter_id_t */
                        create_api_tlv(TLV_INT, buff->size,
                                        buff->data, &offset);
                        break;

                case VFM_EDIT:
                case VFM_ONLINE:
                        create_api_tlv(TLV_INT, buff->size,
                                        buff->data, &offset);
                        break;

                case VFM_EDIT_PROTOCOL_ATTR:
                        create_api_tlv(TLV_INT, buff->size,
                                        buff->data, &offset);
                        break;
                case VFM_QUERY_INVENTORY:
                case VFM_QUERY:
                        create_api_tlv(TLV_INT, sizeof(uint32_t),
                                        &buff->size, &offset);
                        create_api_tlv(TLV_BD_ATTR, buff->size,
                                        buff->data, &offset);
                        break;
                case VFM_ERROR:
                        create_api_tlv(TLV_INT, sizeof(vps_error),
                                        &buff->size, &offset);

        }

        free(buff->data);
        vps_trace(VPS_ENTRYEXIT, "Leaving vfm_marshall_response");
        return err;
}

/*
 * This function will receive the message from the socket.
 * then process the message and do the required processing.
 * Once the processing is done, It will create the response
 * using the marshall_response() function into TLV's and
 * then send it on the socket to which it is bound.
 */
vfm_error_t
unmarshall_request(void *buff, uint32_t size, res_packet * pack)
{
        vfmapi_ctrl_hdr ctrl_hdr;
        uint32_t ret_pos = 0, num;
        uint32_t op_arg;
        res_packet op_data;

        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering unmarshall_request");

        /* Then process the control header */
        read_api_ctrl_hdr(buff, size, &ret_pos, &ctrl_hdr);
        switch (ctrl_hdr.mod_id) {
                case VFMAPI_VADAPTER:
                        switch(ctrl_hdr.opcode) {
                                case VFM_CREATE:
                                        process_vfm_create_vadpter(buff,
                                                        &ret_pos, &op_data);
                                        num = 1;
                                        vfm_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, num);
                                        break;
                                case VFM_EDIT:
                                        err = process_vfm_edit_vadpter(buff,
                                                        &ret_pos, &op_data);
                                        vfm_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, 1);
                                        break;
                                case VFM_EDIT_PROTOCOL_ATTR:
                                        err = process_vfm_edit_en_attr(buff,
                                                        &ret_pos, &op_data);
                                        vfm_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, 1);
                                        break;
                                case VFM_QUERY_INVENTORY:
                                        process_vfm_query_inventory(buff,
                                                        &ret_pos, &op_data);

                                        vfm_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, 2);
                                        break;
				case VFM_ONLINE:
					process_vfm_vadapter_online(buff,
							&ret_pos, &op_data);
                                        vfm_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, 1);
					break;
                                case VFM_QUERY:
                                        process_vfm_query_vadapter(buff,
                                                        &ret_pos, &op_data);

                                        vfm_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, 1);
                                        break;
                        }
                        break;
                case VFMAPI_VFABRIC:
                        switch(ctrl_hdr.opcode) {


                                case VFM_CREATE:
                                        process_vfm_create_vfabric(buff,
                                                        &ret_pos, &op_data);
                                        num = 1;
                                        vfm_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, num);
                                        break;
                                case VFM_EDIT:
                                        err = process_vfm_edit_vfabric(buff,
                                                        &ret_pos, &op_data);
                                        vfm_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, 1);
					break;
				case VFM_ONLINE:
					err = process_vfm_vfabric_online(buff,
							&ret_pos, &op_data);
                                        /* 
                                         * If the function returns an error
                                         * code then change the control header
                                         * opcode to VFM_ERROR
                                         */
                                        if (err != VPS_SUCCESS)
                                                ctrl_hdr.opcode = VFM_ERROR; 
                                        vfm_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, 1);
					break;
                        }
                        break;
                case VFMAPI_BRIDGE_DEVICE:
                        switch(ctrl_hdr.opcode) {

                                case VFM_QUERY_INVENTORY:
                                        process_vfm_bd_select_inventory(buff,
                                                        &ret_pos, &op_data);
                                        vfm_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, 2);
                                        break;
                                case VFM_QUERY:
                                        process_vfm_bd_query_general_attr(buff,
                                                        &ret_pos, &op_data);
                                        vfm_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, 2);
                                        break;
                        }
                        break;

        }

	/** TODO: Fix the memory corruption */
	//free(op_data.data);
        vps_trace(VPS_ENTRYEXIT, "Leaving unmarshall_request");
        return err;
}
