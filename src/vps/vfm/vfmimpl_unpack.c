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
process_vfabric_data(uint8_t *buff, uint32_t *ret_pos, res_packet *op_data)
{
        int i, type, size, length;
        uint8_t *temp = buff + *ret_pos;
        vfm_vfabric_attr_t * vfabric;
        vfm_error_t err = VPS_SUCCESS;

        /* vps_trace(VPS_ENTRYEXIT, "Entering %s", __FUNCTION__); */
        /*
         * First read the count from the packet. i.e. the number of TLVs in
         * the packet.
         */
        memcpy(&op_data->count, temp,  sizeof(op_data->count));
        temp += sizeof(op_data->count);
        /* 
         * Allocate the array of structures depending upon the count
         * Do error handling for allocation.
         */

        op_data->data = malloc(op_data->count * sizeof(vfm_vfabric_attr_t));
        if (op_data->data == NULL)
                goto out;
        vfabric = (vfm_vfabric_attr_t *)op_data->data;

        /*
         * Then start processing all the TLVs.
         */
        for (i=0; i < op_data->count; i++) {

                memcpy(&type, temp, sizeof(type));
                temp += sizeof(type);
                memcpy(&length, temp, sizeof(length));
                temp += sizeof(length);

                memcpy(vfabric, temp, sizeof(vfm_vfabric_attr_t));
                temp += sizeof(vfm_vfabric_attr_t); 

                /* Allocate memory for vadapter ids. and set the pointer
                 * Point the vadapter_id to the offset where the vadapter id's
                 * stored ahead of the start pointer containing the vfabric
                 */
                size = (vfabric->_num_vadapter *sizeof(vfm_vadapter_id_t));
                vfabric->_vadapter_id = malloc(size);
                memcpy(vfabric->_vadapter_id, temp, size);
                temp += size;
                vfabric ++; 
         }
out: 
         /*        vps_trace(VPS_ENTRYEXIT, "Leaving %s", __FUNCTION__); */
         return err;
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
unpack_tlv(uint8_t * buff, uint32_t *ret_pos, void **op_data)
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
vfmimpl_unpack(uint8_t *buff, uint32_t *ret_pos, void **op_data)
{
        int count, offset = 0, i;
        uint8_t *temp = buff + *ret_pos;
        vfm_error_t err = VPS_SUCCESS;

        /*        vps_trace(VPS_ENTRYEXIT, "Entering %s", __FUNCTION__); */
        /*
         * First read the count from the packet. i.e. the number of TLVs in
         * the packet.
         */
        memcpy(&count, temp,  sizeof(count));
        offset += sizeof(count);
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
               err = unpack_tlv(temp, &offset, &op_data[i]);
               if(err)
                      goto out;
         }
out: 
         /*        vps_trace(VPS_ENTRYEXIT, "Leaving %s", __FUNCTION__); */
         *ret_pos = count;
         return err;

}

