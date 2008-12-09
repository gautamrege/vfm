#include <bxmapi_common.h>

/*
 * Create a new vfabric.
 * Parameters:
 * [in]   name        The user-defined name of the virtual fabric.
 * [in]   desc        The user-defined description of the virtual fabric.
 *                    Can be NULL.
 * [in]   protocol    The protocol of the virtual fabric.
 * [out]  vfabric_id  The id of newly allocated vfabric is stored at the
 *                    location to which this argument points. On error,
 *                    BXM_INVALID_ID will be stored at this location.
 * Returns 0 on success, or an error code on failure.
 */

bxm_error_t
bxm_vfabric_create(char *   name,
                   char *   desc,
                   bxm_protocol_t protocol,
                   bxm_vfabric_id_t * vfabric_id)
{
        api_tlv tlv;
        uint32_t mesg_len, res_len, sock_fd;
        uint32_t no_of_args = 3;
        uint8_t *message, *offset;
        bxmapi_ctrl_hdr ctrl_hdr;
        res_packet pack, op_pack;
        bxm_error_t err = BXM_SUCCESS;

        mesg_len = (sizeof(bxmapi_ctrl_hdr) + sizeof(bxm_protocol_t) +
                        strlen(name) + strlen(desc) + 2 * no_of_args);

        memset(&ctrl_hdr , 0, sizeof(bxmapi_ctrl_hdr));

        err = create_ctrl_hdr(BXMAPI_VFABRIC, BXM_CREATE,
                        mesg_len, &ctrl_hdr);

        message = (uint8_t *)malloc(mesg_len);
        offset = message;

        memcpy(offset, &ctrl_hdr, sizeof(bxmapi_ctrl_hdr));
        offset += sizeof(bxmapi_ctrl_hdr);

        /* TYPE = 2, name  */
        create_api_tlv(TLV_CHAR, strlen(name),
                        name, offset);

        /* TYPE = 2 , desc */
        create_api_tlv(TLV_CHAR, strlen(desc),
                        desc, offset);

        /* TYPE = 1 , bxm_protocol_t */
        create_api_tlv(TLV_INT, sizeof(bxm_protocol_t),
                        &(protocol), offset);


        /* TODO: Call the function to send the packet to server */
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

/*
 *
 *
 * Edit properties of a vfabric.
 * Parameters:
 * [in] vfabric_id  The vfabric Id of the vfabric to change the attributes of.
 * [in] bitmask   Specifies a bitmask value of the specific properties of the
 * object to be updated. This bitmask flag should be bit-AND'ed against order
 * of attributes in the struct bxm_vfabric_attr_t.
 * [in]   attr   Pointer to the structure containing the attrs of the vfabric
 * that should be changed.
 * Returns:
 * Returns 0 on success, or an error code on failure.
 */
bxm_error_t
bxm_vfabric_edit_general_attr(bxm_vfabric_id_t vfabric_id,
                bxm_vfabric_attr_bitmask_t * bitmask,
                bxm_vfabric_attr_t * attr)
{
        api_tlv tlv;
        uint32_t mesg_len, res_len;
        uint32_t no_of_args = 3;
        uint8_t *message, *offset;
        bxmapi_ctrl_hdr ctrl_hdr;
        int sock_fd;
        res_packet pack, op_pack;

        bxm_error_t err = BXM_SUCCESS;

        mesg_len = (sizeof(bxmapi_ctrl_hdr) +sizeof(bxm_vfabric_id_t) +
                        sizeof(bxm_vfabric_attr_bitmask_t) +
                        sizeof(bxm_vfabric_attr_t) + 2 * no_of_args);

        memset(&ctrl_hdr , 0, sizeof(bxmapi_ctrl_hdr));

        err = create_ctrl_hdr(BXMAPI_VFABRIC, BXM_EDIT, mesg_len, &ctrl_hdr);
        if (err)
                return err;

        message = (uint8_t *)malloc(mesg_len);
        offset = message;

        memcpy(offset, &ctrl_hdr, sizeof(bxmapi_ctrl_hdr));
        offset += sizeof(bxmapi_ctrl_hdr);

        /* TYPE = 1 , vfabric_id */
        create_api_tlv(TLV_INT, sizeof(bxm_vfabric_id_t), &vfabric_id,
                        offset);

        /* TYPE =TLV_BIT_MASK , bxm_protocol_t */

        create_api_tlv(TLV_VFABRIC_ATTR_BITMASK,
                        sizeof(bxm_vfabric_attr_bitmask_t), bitmask, offset);

        /* TYPE = TLV_VADP_ATTR, bxm_vfabric_attr_t */
        create_api_tlv(TLV_VFABRIC_ATTR, sizeof(bxm_vfabric_attr_t),
                        attr, offset);


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

/*
 * Change the running mode of the vfabric to ONLINE.
 * Note:
 * This operation modifies the running mode of the vfabric to ONLINE.
 * It means that BXM can activate the vfabric and start the services of the
 * vfabric i.e. it will turn the vadapters as ACTIVE and will start bridging
 * the packets between the host and the destination via the gateway.
 * After the call the BXM system will change the vfabric to ONLINE.
 * If the primary gateway is ONLINE/ACTIVE, the vfabric will also be ACTIVE.
 * Parameters:
 * [in]   vfabric_id  : The id of the vfabric to be activated.
 * Returns:
 * Returns 0 on success, or an error code on failure.
 */

bxm_error_t
bxm_vfabric_online(bxm_vfabric_id_t vfabric_id)
{
        api_tlv tlv;
        uint32_t mesg_len, res_len;
        uint32_t no_of_args = 1;
        uint8_t *message, *offset;
        bxmapi_ctrl_hdr ctrl_hdr;
        int sock_fd;
        res_packet pack, op_pack;

        bxm_error_t err = BXM_SUCCESS;

        mesg_len = (sizeof(bxmapi_ctrl_hdr) + sizeof(bxm_vfabric_id_t) +
                        2 * no_of_args);

        memset(&ctrl_hdr , 0, sizeof(bxmapi_ctrl_hdr));

        err = create_ctrl_hdr(BXMAPI_VFABRIC, BXM_ONLINE, mesg_len, &ctrl_hdr);
        if (err)
                return err;

        message = (uint8_t *)malloc(mesg_len);
        offset = message;

        memcpy(offset, &ctrl_hdr, sizeof(bxmapi_ctrl_hdr));
        offset += sizeof(bxmapi_ctrl_hdr);


        /* TYPE = 1 , vfabric_id */
        create_api_tlv(TLV_INT, sizeof(bxm_vfabric_id_t), &vfabric_id,
                        offset);

        err = create_connection(&sock_fd);
        if (err)
                return err;

        err = process_request(sock_fd, message, mesg_len, &pack);
        if (err)
                return err;

        err = unmarshall_response(pack.data, pack.size, &op_pack);

        return err;
}

