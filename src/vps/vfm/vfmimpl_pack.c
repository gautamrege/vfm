/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */

#include <vfmapi_common.h>
#include <vfmdb.h>
#include <common.h>

/*
 * This functions will pack the tlvs from the ip_pack and then copy
 * that tlvs into op_pack (Countinues memory.
 * (Assuming op_pack has pre-allocated memory in vfm_marshall_response and 
 *  offset is indiacting the current position in op_pack->data)
 *  - First get the no of TLVS from ip_pack->count and copy it in offset.
 *  - Then memcopy the TLV from (ip_pack->data) into op_pack->data.
 * *ip_pack [IN] :ip_pack->count - Contains the no of TLVS in ip_pack->data.
 *                ip_pack->size  - Sizeof the total TLVS (to allocate Memory).
 *                ip_pack->data  - Pointer to array of pointers to the TLVS.
 *
 * **offset [IN/OUT] :Contains the current offset of the op_pack->data
 *
 */

vfm_error_t 
vfm_pack_data(res_packet *ip_pack, uint8_t **offset)
{
        int i = 0;
        uint32_t size = 0;
        uint8_t **temp = ip_pack->data;
        vfm_error_t err = VPS_SUCCESS; 
        /* copy the count of the TLVs in offset*/
        memcpy(*offset, &ip_pack->count, sizeof(ip_pack->count));
        *offset += sizeof(ip_pack->count);

        for(i = 0; i < ip_pack->count; i++) {
                /* get the size of the TLV (including TYPE and LENGTH)*/
                size = ((api_tlv *)temp[i])->length;
                /* Copy the TLV into the offset */
                memcpy(*offset, temp[i], size);
                *offset += size;
        }
        return err;
}
