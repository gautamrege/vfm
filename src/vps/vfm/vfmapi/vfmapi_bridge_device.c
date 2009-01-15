/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */

#include <vfmapi_common.h>

vfm_error_t
unpack_bridge_data(res_packet *op_pack,uint32_t *num_result,
                                vfm_bd_attr_t *result[])
{
        int i = op_pack->size;
        int j = 0, size = 0;
        vfm_error_t err = VFM_SUCCESS;
        uint8_t *offset;
        offset = (uint8_t *)op_pack->data;

        memcpy(num_result, offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        *result = (vfm_bd_attr_t *) malloc(sizeof(vfm_bd_attr_t) 
                                                * (*num_result));
        for(i = 0; i < *num_result; i++) {
                
                memcpy(*result + i, offset, sizeof(vfm_bd_attr_t));

                offset += sizeof(vfm_bd_attr_t);
                size = (sizeof(vfm_gw_module_index_t) *
                                (*result + i)->_num_gw_module);

                (*result + i)->_gw_module_index = 
                        (vfm_gw_module_index_t *)malloc(size);

                memcpy((*result + i)->_gw_module_index, offset, size);
                offset += size; 
        }
out:
        return err;
}
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
        int i = 0;
        int num = 0;
        uint8_t *message, *offset, *res_mesg;
        vfmapi_ctrl_hdr ctrl_hdr;
        int sock_fd;
        res_packet pack, op_pack;

        vfm_error_t err = VFM_SUCCESS;

        *result = NULL;

        mesg_len = (sizeof(vfmapi_ctrl_hdr) +  sizeof(vfm_bd_attr_bitmask_t) +
                        sizeof(vfm_bd_attr_t) + TLV_SIZE * no_of_args);

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
        
        err = unpack_bridge_data(&op_pack, &num, result);
        *num_result = num;
        free(op_pack.data);
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
        /* put bridge_guid into the attr struct */
        /* Check if the bitmask for the guid is set */
        /* call the vfm_bd_select_inventory API */
        /* populate the FIRST element from the result array into the
           output result */
        vfm_error_t err = VFM_SUCCESS;
        vfm_bd_attr_t attr;
        int num = 0;
        memset(&attr, 0, sizeof(vfm_bd_attr_t));

        if(bitmask.guid)
                attr._bd_guid = bridge_guid;
        else
                err = -1;
        err = vfm_bd_select_inventory(&attr, bitmask, &num, &result);
        return err;
}
