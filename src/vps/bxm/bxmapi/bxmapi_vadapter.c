/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#include <bxmapi_common.h>
#include <bxm_vadapter.h>


/*
 * This function Marshals the message for creating the
 * Virtual adapter object
 *
 * io_module_id [IN]
 * protocol     [IN]
 * name         [IN]
 * desc         [IN]
 * vadapter_id  [OUT]
 */
bxm_error_t
bxm_vadapter_create(char *name, char *desc,
                bxm_protocol_t protocol,
                bxm_vadapter_id_t *vadapter_id)
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

        err = create_ctrl_hdr(BXMAPI_VADAPTER, BXM_CREATE,
                        mesg_len, &ctrl_hdr);
        message = (uint8_t *)malloc(mesg_len);
        offset = message;

        memcpy(offset, &ctrl_hdr, sizeof(bxmapi_ctrl_hdr));
        offset += sizeof(bxmapi_ctrl_hdr);

        /* TYPE = 2, name  */
        create_api_tlv(TLV_CHAR, strlen(name), name, offset);

        /* TYPE = 2 , desc */
        create_api_tlv(TLV_CHAR, strlen(desc), desc, offset);

        /* TYPE = 1 , bxm_protocol_t */
        create_api_tlv(TLV_INT, sizeof(bxm_protocol_t), &protocol, offset);

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
 *  Edit properties of a vadapter.
 *  Parameters:
 *  [in] vadapter_id : The id of the virtual adapter
 *                     to change the attributes of.
 *  [in] bitmask     : Specifies an bitmask value. The bitmask indicates the
 *                     specific properties of the object to be updated.
 *  [in] attr        : Pointer to the structure containing the attrs of
 *                     the virtual adapter that should be updated.
 */

bxm_error_t
bxm_vadapter_edit_property (bxm_vadapter_id_t vadapter_id,
                bxm_vadapter_attr_bitmask_t *   bitmask,
                bxm_vadapter_attr_t *   attr)
{
        api_tlv tlv;
        uint32_t mesg_len, res_len;
        uint32_t no_of_args = 3;
        uint8_t *message, *offset;
        bxmapi_ctrl_hdr ctrl_hdr;
        int sock_fd;
        res_packet pack, op_pack;

        bxm_error_t err = BXM_SUCCESS;

        mesg_len = (sizeof(bxmapi_ctrl_hdr) +sizeof(bxm_vadapter_id_t) +
/*TODO:*/                        sizeof(bxm_vadapter_attr_bitmask_t) +
                        sizeof(bxm_vadapter_attr_t) + 2 * no_of_args);

        memset(&ctrl_hdr , 0, sizeof(bxmapi_ctrl_hdr));

        err = create_ctrl_hdr(BXMAPI_VADAPTER, BXM_EDIT, mesg_len, &ctrl_hdr);
        if (err)
                return err;

        message = (uint8_t *)malloc(mesg_len);
        offset = message;

        memcpy(offset, &ctrl_hdr, sizeof(bxmapi_ctrl_hdr));
        offset += sizeof(bxmapi_ctrl_hdr);

        /* TYPE = 1 , vadapter_id */
        create_ctrl_hdr(TLV_INT, sizeof(bxm_vadapter_id_t), &vadapter_id,
                        offset);

        /* TYPE =TLV_BIT_MASK , bxm_protocol_t */

        create_ctrl_hdr(TLV_BIT_MASK, sizeof(bxm_vadapter_attr_bitmask_t),
                        bitmask, offset);

        /* TYPE = TLV_VADP_ATTR, bxm_vadapter_attr_t */
        create_ctrl_hdr(TLV_VADP_ATTR, sizeof(bxm_vadapter_attr_t),
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
* Get the list of existing vadapters in the local inventory.
* Parameters:
* [in] attr   : Pointer to the structure containing the attrs of the vadapters
*               that should match with the properties in the inventory.
* [in] bitmask: Specifies an bit mask value. The bitmask indicates the specific
*               properties of the object to be matched. This bitmask flag
*               should be bit-AND'ed against order of attributes in the struct
*               bxm_vadapter_attr_t.
*
* [out] num_result : Number of resource objects allocated in result
* [out] result     : Pointer to an array of vadapter attribute struct. It is
*                    responsibility of the caller to allocate and free memory
*                    of the strucutres. The results found are stored at the
*                    location to which this argument points. NULL will mark the
*                    end of the list, if there are less than num_results
*                    objects found in the inventory.
* Returns:
* Returns 0 on success, or an error code on failure.
*/
bxm_error_t
bxm_vadapter_select_inventory(bxm_vadapter_attr_t * attr,
                bxm_vadapter_attr_bitmask_t *bitmask,
                uint32_t num_result,
                bxm_vadapter_id_t *result[])
{

        api_tlv tlv;
        uint32_t mesg_len, res_len;
        uint32_t no_of_args = 2;
        uint8_t *message, *offset;
        bxmapi_ctrl_hdr ctrl_hdr;
        int sock_fd;
        res_packet pack, op_pack;

        bxm_error_t err = BXM_SUCCESS;

        mesg_len = (sizeof(bxmapi_ctrl_hdr) +
                        sizeof(bxm_vadapter_attr_bitmask_t) +
                        sizeof(bxm_vadapter_attr_t) + 2 * no_of_args);

        memset(&ctrl_hdr , 0, sizeof(bxmapi_ctrl_hdr));

        err = create_ctrl_hdr(BXMAPI_VADAPTER, BXM_QUERY, mesg_len, &ctrl_hdr);
        if (err)
                return err;

        message = (uint8_t *)malloc(mesg_len);
        offset = message;

        memcpy(offset, &ctrl_hdr, sizeof(bxmapi_ctrl_hdr));
        offset += sizeof(bxmapi_ctrl_hdr);


        /* TYPE =TLV_BIT_MASK , bxm_protocol_t */
        create_api_tlv(TLV_BIT_MASK, sizeof(bxm_vadapter_attr_bitmask_t),
                              bitmask, offset);

        /* TYPE = TLV_VADP_ATTR, bxm_vadapter_attr_t */
        create_api_tlv(TLV_VADP_ATTR, sizeof(bxm_vadapter_attr_t),
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

bxm_error_t
bxm_vadapter_destroy(bxm_vadapter_id_t vadapter_id)
{
        api_tlv tlv;
        uint32_t mesg_len, res_len;
        uint32_t no_of_args = 1;
        uint8_t *message, *offset;
        bxmapi_ctrl_hdr ctrl_hdr;
        int sock_fd;
        res_packet pack, op_pack;

        bxm_error_t err = BXM_SUCCESS;

        mesg_len = (sizeof(bxmapi_ctrl_hdr) + sizeof(bxm_vadapter_id_t) +
                        2 * no_of_args);

        memset(&ctrl_hdr , 0, sizeof(bxmapi_ctrl_hdr));

        err = create_ctrl_hdr(BXMAPI_VADAPTER, BXM_DESTROY,
                        mesg_len, &ctrl_hdr);
        if (err)
                return err;

        message = (uint8_t *)malloc(mesg_len);
        offset = message;

        memcpy(offset, &ctrl_hdr, sizeof(bxmapi_ctrl_hdr));
        offset += sizeof(bxmapi_ctrl_hdr);


        /* TYPE = TLV_INT, bxm_protocol_t */
        create_api_tlv(TLV_INT, sizeof(bxm_vadapter_id_t),
                        &vadapter_id, offset);

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
 * Edit protocol specific properties of a vadapter.
 * Parameters:
 * [in] vadapter_id : The id of the virtual adapter to change the protocol
 *                      attributes of.
 * [in] bitmask     : Specifies an bitmask value. The bitmask indicates the
 *                    specific protocol properties of the object to be updated.
 *                    This bitmask flag should be bit-AND'ed against order of a
 *                    ttributes in the struct bxm_vadapter_en_attr_t or
 *                    bxm_vadapter_fc_attr_t based on the protocol type.
 * [in]   attr      : Pointer to the structure containing the attrs of the
 *                    virtual adapter that should be updated.
 *
 * Returns 0 on success, or an error code on failure.
 */
bxm_error_t
bxm_vadapter_edit_protocol_attr(bxm_vadapter_id_t vadapter_id,
                void * bitmask,
                void * attr)
{

}


/*
 * Change the running mode of the vfabric to ONLINE.
 * This operation modifies the running mode of the vadapter to ONLINE.
 * This operation is the trigger to BXM to start the services of the vadapter.
 * After the call the BXM system can instantiate the vadapter (based on whether
 * it's a HOST/NETWORK Initiated Vadapter). After the vadapter is instantiated
 * on the physical I/O module, its status is changed to ACTIVE.
 * Note: After the call the BXM system will change the vdapter to ONLINE.
 *       If the vfabric is ONLINE/ACTIVE, the vadapter will also be ACTIVE.
 * Parameters:
 * [in]   vadapter_id   The id of the vadapter to be activated.
 *
 * Returns 0 on success, or an error code on failure.
 * TODO: Complete the function :
 */
bxm_error_t
bxm_vadapter_online(bxm_vadapter_id_t vadapter_id)
{

        api_tlv tlv;
        uint32_t mesg_len, res_len;
        uint32_t no_of_args = 2;
        uint8_t *message, *offset;
        bxmapi_ctrl_hdr ctrl_hdr;
        int sock_fd;
        res_packet pack, op_pack;

        bxm_error_t err = BXM_SUCCESS;

        mesg_len = (sizeof(bxmapi_ctrl_hdr) + sizeof(bxm_vadapter_id_t) +
                        2 * no_of_args);

        memset(&ctrl_hdr , 0, sizeof(bxmapi_ctrl_hdr));

        err = create_ctrl_hdr(BXMAPI_VADAPTER, BXM_QUERY, mesg_len, &ctrl_hdr);
        if (err)
                return err;

        message = (uint8_t *)malloc(mesg_len);
        offset = message;

        memcpy(offset, &ctrl_hdr, sizeof(bxmapi_ctrl_hdr));
        offset += sizeof(bxmapi_ctrl_hdr);


        /* TYPE =TLV_BIT_MASK , bxm_protocol_t */
        create_api_tlv(TLV_INT, sizeof(bxm_vadapter_id_t),
                              &vadapter_id, offset);
        


}

