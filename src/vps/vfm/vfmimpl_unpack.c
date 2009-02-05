#include <vfmapi_common.h>

/* @brief
 * This function will unpack the data from the Vfabric TLV
 * The TLV will be in the form of
 *
 * |~~~~~|~~~~~~~|~~~~~~~~~~~~~~~~~~|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
 * |TYPE |LENGTH |VFABRIC STRUCTURE | VADAPTER ID'S ASSOCIATED WITH VFABRIC|
 * |_____|_______|__________________|______________________________________|
 *                       |          ^   
 *                       |__________| 
 *                                 
 *
 * This TLV will be processed and the srtucture for the vfabric will be
 * and the vadapter id's. It will call the get_api_tlv() routine to fill up
 * the values.
 * @param[IN]
 */
vfm_error_t
process_vfabric_data(void * op_data)
{
        
        vfm_vfabric_attr_t * op_attr = (vfm_vfabric_attr_t *)op_data;
        /*
         * Point the vadapter_id to the offset where the vadapter id's are
         * stored ahead of the start pointer containing the vfabric struct.
         */
        (op_attr->_vadapter_id) = op_data + sizeof(vfm_vfabric_attr_t);
}

/*
 * @brief
 * This function strips the Type and Length from the TLV and allocates 
 * memory to the output parameter into which it stores the value.
 * The caller function is responsible for freeing of the memory.
 * 
 * @param[in]  *buff : Contains the data with the TLVs.
 * @param[in]  *ret_pos : Contains the offset to buff.
 * @param[out] **op_data : Contains the address of the output parameter.
 *
 * @returns Returns 0 for success or Non zero error code for failure.
 */
vfm_error_t
get_tlv_value(uint8_t * buff, uint32_t *ret_pos, void **op_data)
{
        int type, length;
        uint8_t *offset = buff + *ret_pos;
        vfm_error_t err = VFM_SUCCESS;

        memcpy(&type, offset,  sizeof(type));
        offset += sizeof(type);
        memcpy(&length, offset,  sizeof(length));
        offset += sizeof(length);

        *op_data = malloc(length);
        if (*op_data == NULL) {
                err =  -1;
                goto out;
        }

        memcpy(*op_data, offset, length);
        if (type == TLV_VFABRIC_ATTR) {
                process_vfabric_data(*op_data);
        }
        *ret_pos += TLV_SIZE + length;
out:
        return err;
}


/* 
 * @brief
 * The packet can contain different type of data types into it.
 * The data will be processed depeding upon the type of TLV.
 * The memory allocation will be done and result array of pointers will
 * be populated.
 *  
 * @param[in] *buff : Pointer to the buffer read from the socket. It points
 *                    ahead of the control header.
 * @param[out] **op_data : This will contain the array of pointers to the 
 *                          data.
 * @returns Returns 0 for success or Non zero error code for failure.
 */
vfm_error_t
vfmimpl_unpack(uint8_t *buff, void ***op_data)
{
        int count, type, length, ret_pos = 0, i;
        void *temp;
        vfm_error_t err = VPS_SUCCESS;

        /*        vps_trace(VPS_ENTRYEXIT, "Entering %s", __FUNCTION__); */
        /*
         * First read the count from the packet. i.e. the number of TLVs in
         * the packet.
         */
        memcpy(&count, buff,  sizeof(count));
        buff += sizeof(count);
        /* 
         * Allocate the result i.e. arrary of pointers depending upon the count
         * this array will store pointers to the data from the TLV.
         * Do error handling for allocation.
         */

        *op_data = malloc(count * sizeof(void *));
        if (*op_data == NULL)
                goto out;

        /*
         * Then start processing all the TLVs.
         * 1. Allocate memory for each data pointer depending upon the length
         * field on the TLV and copy the data from the buff into it.
         * And assign it in the result array of pointers.
         */
        for (i=0; i<count; i++) {
               err = get_tlv_value(buff, &ret_pos, &op_data[0][i]);
               if(err)
                      goto out;
         }
out: 
         /*        vps_trace(VPS_ENTRYEXIT, "Leaving %s", __FUNCTION__); */
         return err;

}

