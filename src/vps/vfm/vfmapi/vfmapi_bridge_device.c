/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */

#include <vfmapi_common.h>
#include <vfm_bridge_device.h>


/*
 * Get the list of existing bridge device in the local inventory.
 * Parameters:
 * [in] attr        : Pinter to the structure containing the attrs of the
 *                    bridge device that should match with the properties in
 *                    the inventory.
 * [in] bitmask     : Specifies the bitmask value. This bitmask flag should be
 *                    bit-AND'ed against order of attributes in the struct
 *                    vfm_bd_attr_t
 * [out] num_result : Number of resource objects allocated in result
 *                    objects found in the inventory.
 * [out] result     : Pointer to an array of bridge device attribute struct.
 *                    It is the responsibility of the caller to allocate and
 *                    free memory of the strucutres. The results found are
 *                    stored at the location to which this argument points.
 *                    NULL will mark the end of the list, If there are less
 *                    than num_results objects found in the inventory.
 * Returns:
 * Returns 0 on success, or an error code on failure.
 */
vfm_error_t
vfm_bd_select_inventory (vfm_bd_attr_t * attr,
                vfm_bd_attr_bitmask_t bitmask,
                uint32_t *num_result,
                vfm_bd_attr_t *result[])
{

        api_tlv tlv;
        uint32_t mesg_len, res_len;
        uint32_t no_of_args = 2;
        uint8_t *message, *offset, *res_mesg;
        vfmapi_ctrl_hdr ctrl_hdr;
        int sock_fd;
        res_packet pack, op_pack;

        vfm_error_t err = VFM_SUCCESS;

        mesg_len = (sizeof(vfmapi_ctrl_hdr) +  sizeof(vfm_bd_attr_bitmask_t) +
                        sizeof(vfm_bd_attr_t) + 2 * no_of_args);

        memset(&ctrl_hdr , 0 , sizeof(vfmapi_ctrl_hdr));

        err = create_ctrl_hdr(VFMAPI_BRIDGE_DEVICE, VFM_QUERY_INVENTORY,
                                         mesg_len, &ctrl_hdr);
        if (err != VFM_SUCCESS)
               goto out;

        message = (uint8_t *)malloc(mesg_len);
        offset = message;

        memcpy(offset, &ctrl_hdr, sizeof(vfmapi_ctrl_hdr));
        offset += sizeof(vfmapi_ctrl_hdr);


        /* TYPE = TLV_BD_ATTR, vfm_bd_attr_t */
        create_api_tlv(TLV_BD_ATTR, sizeof(vfm_bd_attr_t), attr, &offset);


        /* TYPE =TLV_BIT_MASK , vfm_bd_attr_bitmask_t */
        create_api_tlv(TLV_BIT_MASK, sizeof(vfm_bd_attr_bitmask_t),
                        &bitmask, &offset);


        display(message, mesg_len);
        err = create_connection(&sock_fd);
        if (err != VFM_SUCCESS)
                goto out;

        err = process_request(sock_fd, message, mesg_len, &pack);
        if (err != VFM_SUCCESS) {
                printf("\n Error in process request : %d ", err);
                goto out;
        }

        err = unmarshall_response(pack.data, pack.size, &op_pack);

out:
        return err;

}


/*
 * brief Query the properties of a specific BridgeX device
 * parameters :
 * [IN]  bridge_guid  : The GUID of the Bridge device
 * [IN]  bitmask      : Specifies the bitmask value. This bitmask flag should
 *                      be bit-AND'ed against order of attributes in the
 *                      struct vfm_bd_attr_t.
 * [OUT] *result      : Pointer to the struct of bridge device attribute.
 *                      The caller would be responsibe to allocate and free
 *                      memory of the strucuture. The result is stored at the
 *                      location to which this argument points. If no match
 *                      is found or on error, NULL will be stored in this
 *                      location.
 * Returns :
 *                      Returns 0 on success, or an error code on failure.
 */


vfm_error_t
vfm_bd_query_general_attr(vfm_bd_guid_t bridge_guid,
                          vfm_bd_attr_bitmask_t bitmask,
                          vfm_bd_attr_t *result)
{
        api_tlv tlv;
        uint32_t mesg_len, res_len;
        uint32_t no_of_args = 1;
        uint8_t *message, *offset, *res_mesg;
        vfmapi_ctrl_hdr ctrl_hdr;
        int sock_fd;
        res_packet pack, op_pack;

        vfm_error_t err = VFM_SUCCESS;

        mesg_len = (sizeof(vfmapi_ctrl_hdr) + sizeof(vfm_bd_guid_t) +
                        sizeof(vfm_bd_attr_bitmask_t) + 2 * no_of_args);

        memset(&ctrl_hdr , 0 , sizeof(vfmapi_ctrl_hdr));

        err = create_ctrl_hdr(VFMAPI_BRIDGE_DEVICE, VFM_QUERY, mesg_len,
                                                                &ctrl_hdr);
        if (err)
                return err;

        message = (uint8_t *)malloc(mesg_len);
        offset = message;

        memcpy(offset, &ctrl_hdr, sizeof(vfmapi_ctrl_hdr));
        offset += sizeof(vfmapi_ctrl_hdr);


        /* TYPE = TLV_GUID, vfm_vadapter_attr_t */
        create_api_tlv(TLV_GUID, sizeof(vfm_bd_guid_t), &bridge_guid,
                                                                  &offset);
        /* TYPE =TLV_BIT_MASK , vfm_bd_attr_bitmask_t */
        create_api_tlv(TLV_BIT_MASK, sizeof(vfm_bd_attr_bitmask_t),
                        &bitmask, &offset);


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
