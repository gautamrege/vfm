#include <vfmapi_common.h>

/*
 * Create a new vfabric.
 * Parameters:
 * [in]   name        The user-defined name of the virtual fabric.
 * [in]   desc        The user-defined description of the virtual fabric.
 *                    Can be NULL.
 * [in]   protocol    The protocol of the virtual fabric.
 * [out]  vfabric_id  The id of newly allocated vfabric is stored at the
 *                    location to which this argument points. On error,
 *                    VFM_INVALID_ID will be stored at this location.
 * Returns 0 on success, or an error code on failure.
 */

vfm_error_t
vfm_vfabric_create(char *   name,
                   char *   desc,
                   vfm_protocol_t protocol,
                   vfm_vfabric_id_t * vfabric_id)
{
        api_tlv tlv;
        uint32_t mesg_len, res_len, sock_fd;
        uint32_t no_of_args = 3;
        uint8_t *message, *offset;
        vfmapi_ctrl_hdr ctrl_hdr;
        res_packet pack, op_pack;
        vfm_error_t err = VFM_SUCCESS;

        mesg_len = (sizeof(vfmapi_ctrl_hdr) + sizeof(vfm_protocol_t) +
                        2 * NAME_SIZE + 2 * no_of_args);

        memset(&ctrl_hdr , 0, sizeof(vfmapi_ctrl_hdr));

        err = create_ctrl_hdr(VFMAPI_VFABRIC, VFM_CREATE,
                        mesg_len, &ctrl_hdr);

        message = (uint8_t *)malloc(mesg_len);
        memset(message , 0, mesg_len);
        offset = message;

        memcpy(offset, &ctrl_hdr, sizeof(vfmapi_ctrl_hdr));
        offset += sizeof(vfmapi_ctrl_hdr);

        /* TYPE = 2, name  */
        create_api_tlv(TLV_CHAR, NAME_SIZE, name, &offset);

        /* TYPE = 2 , desc */
        create_api_tlv(TLV_CHAR, NAME_SIZE, desc, &offset);

        /* TYPE = 1 , vfm_protocol_t */
        create_api_tlv(TLV_INT, sizeof(vfm_protocol_t),
                        &(protocol), &offset);


        /* TODO: Call the function to send the packet to server */
        display(message, mesg_len);

        err = create_connection(&sock_fd);
        if (err)
                return err;
        err = process_request(sock_fd, message, mesg_len, &pack);
        if (err)
                return err;
        err = unmarshall_response(pack.data, pack.size, &op_pack);

        *vfabric_id = *(vfm_vfabric_id_t *)op_pack.data;
        free(op_pack.data);
        free(pack.data);

        return err;
}

/*
 * Edit properties of a vfabric. 
 * Parameters:
 * [in]   vfabric_id   The vfabric Id of the vfabric to change the attributes
 *                     of. 
 * [in]   bitmask      Specifies a bitmask value of the specific properties of
 *                     the object to be updated. This bitmask flag should be
 *                     bit-AND'ed against order of attributes in the struct
 *                     vfm_vfabric_attr_t. 
 * [in]   attr         Pointer to the structure containing the attrs of the
 *                     vfabric that should be changed.
 *
 * Returns 0 on success, or an error code on failure.
 */
vfm_error_t
vfm_vfabric_edit_general_attr(vfm_vfabric_id_t vfabric_id,
                vfm_vfabric_attr_bitmask_t * bitmask,
                vfm_vfabric_attr_t * attr)
{
        api_tlv tlv;
        uint32_t mesg_len, res_len;
        uint32_t no_of_args = 3;
        uint8_t *message, *offset;
        vfmapi_ctrl_hdr ctrl_hdr;
        int sock_fd;
        res_packet pack, op_pack;

        vfm_error_t err = VFM_SUCCESS;

        mesg_len = (sizeof(vfmapi_ctrl_hdr) +sizeof(vfm_vfabric_id_t) +
                        sizeof(vfm_vfabric_attr_bitmask_t) +
                        sizeof(vfm_vfabric_attr_t) + 2 * no_of_args);

        memset(&ctrl_hdr , 0, sizeof(vfmapi_ctrl_hdr));

        err = create_ctrl_hdr(VFMAPI_VFABRIC, VFM_EDIT, mesg_len, &ctrl_hdr);
        if (err)
                return err;

        message = (uint8_t *)malloc(mesg_len);
        memset(message , 0, mesg_len);
        offset = message;

        memcpy(offset, &ctrl_hdr, sizeof(vfmapi_ctrl_hdr));
        offset += sizeof(vfmapi_ctrl_hdr);

        /* TYPE = 1 , vfabric_id */
        create_api_tlv(TLV_INT, sizeof(vfm_vfabric_id_t), &vfabric_id,
                        &offset);

        /* TYPE =TLV_BIT_MASK , vfm_protocol_t */

        create_api_tlv(TLV_VFABRIC_ATTR_BITMASK,
                        sizeof(vfm_vfabric_attr_bitmask_t), bitmask, &offset);

        /* TYPE = TLV_VADP_ATTR, vfm_vfabric_attr_t */
        create_api_tlv(TLV_VFABRIC_ATTR, sizeof(vfm_vfabric_attr_t),
                        attr, &offset);


        display(message, mesg_len);
        err = create_connection(&sock_fd);
        if (err)
                return err;

        err = process_request(sock_fd, message, mesg_len, &pack);
        if (err)
                return err;

        err = unmarshall_response(pack.data, pack.size, &op_pack);

        err = *(vfm_error_t *)(op_pack.data);
        free(op_pack.data);
        return err;


}

/*
 * Change the running mode of the vfabric to ONLINE.
 * Note:
 * This operation modifies the running mode of the vfabric to ONLINE.
 * It means that VFM can activate the vfabric and start the services of the
 * vfabric i.e. it will turn the vadapters as ACTIVE and will start bridging
 * the packets between the host and the destination via the gateway.
 * After the call the VFM system will change the vfabric to ONLINE.
 * If the primary gateway is ONLINE/ACTIVE, the vfabric will also be ACTIVE.
 * Parameters:
 * [in]   vfabric_id  : The id of the vfabric to be activated.
 * Returns:
 * Returns 0 on success, or an error code on failure.
 */

vfm_error_t
vfm_vfabric_online(vfm_vfabric_id_t vfabric_id)
{
        api_tlv tlv;
        uint32_t mesg_len, res_len;
        uint32_t no_of_args = 1;
        uint8_t *message, *offset;
        vfmapi_ctrl_hdr ctrl_hdr;
        int sock_fd;
        res_packet pack, op_pack;

        vfm_error_t err = VFM_SUCCESS;

        mesg_len = (sizeof(vfmapi_ctrl_hdr) + sizeof(vfm_vfabric_id_t) +
                        2 * no_of_args);

        memset(&ctrl_hdr , 0, sizeof(vfmapi_ctrl_hdr));

        err = create_ctrl_hdr(VFMAPI_VFABRIC, VFM_ONLINE, mesg_len, &ctrl_hdr);
        if (err)
                return err;

        message = (uint8_t *)malloc(mesg_len);
        memset(message , 0, mesg_len);
        offset = message;

        memcpy(offset, &ctrl_hdr, sizeof(vfmapi_ctrl_hdr));
        offset += sizeof(vfmapi_ctrl_hdr);


        /* TYPE = 1 , vfabric_id */
        create_api_tlv(TLV_INT, sizeof(vfm_vfabric_id_t), &vfabric_id,
                        &offset);

        display(message, mesg_len);
        err = create_connection(&sock_fd);
        if (err)
                return err;

        err = process_request(sock_fd, message, mesg_len, &pack);
        if (err)
                return err;

        err = unmarshall_response(pack.data, pack.size, &op_pack);

        return err;
}

