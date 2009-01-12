/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */

#include <vfmapi_common.h>

/*
 * This function receives packet from the socket and then processes the
 * message to get the required values from the buffer TLV's
 * Depending upon the type of packet, the respective functions are called
 * to process the buffer from the socket.
 *
 * [IN] : *buff : pointer to the buffer received from the socket.
 * [IN] : size  : size of the buffer.
 */

vfm_error_t
unmarshall_response(void *buff, uint32_t size, res_packet *pack)
{

        vfmapi_ctrl_hdr ctrl_hdr;
        uint32_t ret_pos = 0;
        vfm_error_t err = VFM_SUCCESS;

        err = read_api_ctrl_hdr(buff, size, &ret_pos, &ctrl_hdr);
        if (err)
                goto out;
        /*
         * First check with the control header opcode and then if the no of
         * changes depending on the ctrl->mod_id then add a switch case
         * inside this switch for the specific case.
         */
        switch (ctrl_hdr.opcode) {
                case VFM_CREATE:
                        pack->data = malloc(sizeof(uint32_t));
                        pack->size = sizeof(uint32_t);
                        err = get_api_tlv(buff, &ret_pos, pack->data);
                        if (err)
                                goto out;
                        break;
                case VFM_EDIT:
                case VFM_ONLINE:
                case VFM_EDIT_PROTOCOL_ATTR:
                        pack->data = malloc(sizeof(uint32_t));
                        pack->size = sizeof(uint32_t);
                        err = get_api_tlv(buff, &ret_pos, pack->data);
                        if (err)
                                goto out;
                        break;
                case VFM_QUERY_INVENTORY:
                case VFM_QUERY:
                        err = get_api_tlv(buff, &ret_pos, &pack->size);
                        pack->data = malloc(sizeof(pack->size));
                        memset(pack->data, 0, pack->size);
                        err = get_api_tlv(buff, &ret_pos, pack->data);
                        if(err)
                             goto out;
                        break;
                case VFM_DESTROY:
                        err = get_api_tlv(buff, &ret_pos, pack->data);
                        if (err)
                             goto out;
                        break;
        }
        display(pack->data, pack->size);
out:
        return err;

}
