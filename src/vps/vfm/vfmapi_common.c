/*
 * Copyright
 */
#include <vfmapi_common.h>
#include <common.h>

/*
 * Process Control Header
 * [IN]  *buff           : Buffer read from socket.
 * [IN]  size            : Number of bytes read from socket.
 * [OUT] *ret_pos        : Number of bytes processed.
 * [OUT] vfmapi_ctrl_hdr : Control header structure.
 *
 * Returns : error code
 *                   Incomplete packet
 * TODO : logging of the messages. For now doing with vps_logs.
 */
vfm_error_t
read_api_ctrl_hdr(uint8_t *buff,
                uint32_t size,
                uint32_t *ret_pos,
                vfmapi_ctrl_hdr *ctrl_hdr)
{
        vfm_error_t err = VPS_SUCCESS;
        /*vps_trace(VPS_ENTRYEXIT, "Entering read_api_ctrl_hdr");*/

        /*Copy control header*/
        memcpy(ctrl_hdr, buff + *ret_pos, sizeof(vfmapi_ctrl_hdr));

         /* If number of bytes read is less than length_of_packet */
        *ret_pos = *ret_pos + sizeof(vfmapi_ctrl_hdr);

/*        vps_trace(VPS_ENTRYEXIT, "Leaving read_api_ctrl_hdr");*/
        return err;
}

/*
 * get_api_tlv : This function fills the output parametr for the
 * unmarshalling the request.
 * This does not create the whole TLV, as an improvement over the previous,
 * it directly fills the output parameter without using extra memory
 * allocation.
 */
vfm_error_t
get_api_tlv(uint8_t *buff, uint32_t *ret_pos, void * op_arg)
{
        uint8_t *ptr = buff + *ret_pos;
        api_tlv tlv;
        vfm_error_t err = VPS_SUCCESS;

       /*vps_trace(VPS_ENTRYEXIT, "Entering get_api_tlv");*/

        /* Copy Type */
        memcpy(&(tlv.type), ptr, sizeof(tlv.type));
        ptr += sizeof(tlv.type);

        tlv.type = ntohl(tlv.type);
        /* Copy the length into the tlv */
        memcpy(&(tlv.length), ptr, sizeof(tlv.length));
        ptr += sizeof(tlv.length);

        tlv.length = ntohl(tlv.length);
        memcpy(op_arg, ptr, tlv.length);
        //printf("\nTYPE : %d \t LEN: %d, VALUE: %x ", tlv.type,
          //              tlv.length, *((uint8_t *)op_arg));
        *ret_pos += (TLV_SIZE + tlv.length);

        /*vps_trace(VPS_ENTRYEXIT, "Leaving get_api_tlv");*/
        return err;
}

/*
 * This function creates the control header from the information given.
 */
vfm_error_t
create_ctrl_hdr(uint8_t mod, uint8_t opcode, uint32_t len,
                vfmapi_ctrl_hdr *ctrl_hdr)
{

        vfm_error_t err = VPS_SUCCESS;
/*        vps_trace(VPS_ENTRYEXIT, "Entering create_ctrl_hdr");*/

        ctrl_hdr->mod_id = mod;
        ctrl_hdr->opcode = opcode;
        ctrl_hdr->length = len;

/*        vps_trace(VPS_ENTRYEXIT, "Leaving create_ctrl_hdr"); */
        return err;
}

/*
 * Create TLVs.
 * NOTE : Pass the address of the offset to create_tlv to increment the
 *        offset.
 */
vfm_error_t
create_api_tlv(uint8_t type, uint32_t len, void *value, uint8_t **offset)
{
        api_tlv tlv;
        vfm_error_t err = VPS_SUCCESS;
        uint32_t tlv_size = sizeof(tlv.type) + sizeof(tlv.length);

/*        vps_trace(VPS_ENTRYEXIT, "Entering create_api_tlv");*/

        tlv.type = htonl(type);
        tlv.length = htonl(len);
        memcpy(*offset, &tlv, TLV_SIZE);
        memcpy(*offset + TLV_SIZE, value, len);
        *offset +=(len + TLV_SIZE);

/*        vps_trace(VPS_ENTRYEXIT, "Leaving create_api_tlv");*/
        return err;

}

/* #ifdef VFM_TEST */
/* This Function just displays the buff */
display(uint8_t *buff, uint32_t size)
{
#ifdef API_TEST
        uint32_t i, j = 0;

	if (size) {
        printf("\n*** NEW MESAGE ***\n");
        for (i = 0; i< size; i++) {
		if(i > j + 15) {
			printf("\n");
			j = i;
		}
                printf(" 0x%0.2x", buff[i]);
	
	}
	printf("\n");
	}
	else
#endif
		return 0;
}
/*#endif */

/*
 * This function is to generate the query for accessing the data.
 * 'WHERE' clause is not included in this query.
 */

void
add_query_parameters(char* buff, int count, const char* db_field,
                                  void* attr_field, uint8_t data_type)
{
        if (count > 0) {
                if (data_type == Q_UINT8)
                        sprintf(buff, " %s and %s = \"%s\"", buff, db_field,
                                                     (uint8_t *)attr_field);
                if (data_type == Q_UINT32)
                        sprintf(buff, " %s and %s = %d", buff, db_field,
                                                    *(uint32_t *)attr_field);
                if (data_type == Q_UINT64)
                        sprintf(buff, " %s and %s = %ld", buff, db_field,
                                                    *(uint64_t *)attr_field);
                if (data_type == Q_NAMED_PARAM)
                        sprintf(buff, " %s and %s = %s", buff, db_field,
                                        (uint8_t *)attr_field);
        }
        else {
                if (data_type == Q_UINT8)
                        sprintf(buff, " where %s = \"%s\"", db_field,
                                                    (uint8_t *)attr_field);
                if (data_type == Q_UINT32)
                        sprintf(buff, " where %s = %d", db_field,
                                                    *(uint32_t *)attr_field);
                if (data_type == Q_UINT64)
                        sprintf(buff, " where %s = %ld", db_field,
                                                    *(uint64_t *)attr_field);
                if (data_type == Q_NAMED_PARAM)
                        sprintf(buff, " where %s = %s", db_field,
                                        (uint8_t *)attr_field);
        }
}

/* 
 * This function is to generate the query for accessing the data.
 * 'WHERE' clause is not included in this query.
 */

void
add_update_query_parameters(char* buff, int count, const char* db_field,
                                  void* attr_field, uint8_t data_type)
{
	buff = buff + strlen(buff);
        if (count > 0) {

                if (data_type == Q_UINT8)
                        sprintf(buff, " %s , %s = '%s'", buff, db_field,
                                        (uint8_t *)attr_field);
                if (data_type == Q_UINT32)
                        sprintf(buff, " %s , %s = %d", buff, db_field,
                                        *(uint32_t *)attr_field);
                if (data_type == Q_UINT64)
                        sprintf(buff, " %s , %s = %ld", buff, db_field,
                                        *(uint64_t *)attr_field);
                if (data_type == Q_NAMED_PARAM)
                        sprintf(buff, " %s , %s = %s", buff, db_field,
                                        (uint8_t *)attr_field);
        }
        else {
                if (data_type == Q_UINT8)
                        sprintf(buff, " %s = '%s'", db_field,
                                        (uint8_t *)attr_field);
                if (data_type == Q_UINT32)
                        sprintf(buff, " %s = %d", db_field,
                                        *(uint32_t *)attr_field);
                if (data_type == Q_UINT64)
                        sprintf(buff, " %s = %ld", db_field,
                                        *(uint64_t *)attr_field);
                if (data_type == Q_NAMED_PARAM)
                        sprintf(buff, " %s = %s", db_field,
                                        (uint8_t *)attr_field);
        }
}
/*#endif */
