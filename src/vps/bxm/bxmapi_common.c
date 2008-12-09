/*
 * Copyright
 */
#include <bxmapi_common.h>
#include <common.h>

/*
 * Process Control Header
 * [IN]  *buff           : Buffer read from socket.
 * [IN]  size            : Number of bytes read from socket.
 * [OUT] *ret_pos        : Number of bytes processed.
 * [OUT] bxmapi_ctrl_hdr : Control header structure.
 *
 * Returns : error code
 *                   Incomplete packet
 * TODO : logging of the messages. For now doing with vps_logs.
 */
bxm_error_t
read_api_ctrl_hdr(uint8_t *buff,
                uint32_t size,
                uint32_t *ret_pos,
                bxmapi_ctrl_hdr *ctrl_hdr)
{
        bxm_error_t err = VPS_SUCCESS;
        /*vps_trace(VPS_ENTRYEXIT, "Entering read_api_ctrl_hdr");*/

        /*Copy control header*/
        memcpy(ctrl_hdr, buff + *ret_pos, sizeof(bxmapi_ctrl_hdr));

         /* If number of bytes read is less than length_of_packet */
        *ret_pos = *ret_pos + sizeof(bxmapi_ctrl_hdr);

/*        vps_trace(VPS_ENTRYEXIT, "Leaving read_api_ctrl_hdr");*/
}

/*
 * get_api_tlv : This function fills the output parametr for the
 * unmarshalling the request.
 * This does not create the whole TLV, as an improvement over the previous,
 * it directly fills the output parameter without using extra memory
 * allocation.
 */
bxm_error_t
get_api_tlv (uint8_t *buff, uint32_t *ret_pos, void * op_arg)
{
        uint8_t *ptr = buff + *ret_pos;
        api_tlv tlv;
        bxm_error_t err = VPS_SUCCESS;

       /*vps_trace(VPS_ENTRYEXIT, "Entering get_api_tlv");*/

        /* Copy Type */
        memcpy(&(tlv.type), ptr, sizeof(tlv.type));
        ptr += sizeof(tlv.type);

        /* Copy the length into the tlv */
        memcpy(&(tlv.length), ptr, sizeof(tlv.length));
        ptr += sizeof(tlv.length);

        memcpy(op_arg, ptr, tlv.length);
        printf("\nTYPE : %d \t LEN: %d, VALUE: %x ", tlv.type,
                        tlv.length, *((uint8_t *)op_arg));
        *ret_pos += (sizeof(tlv.type) + sizeof(tlv.length) + tlv.length);

        /*vps_trace(VPS_ENTRYEXIT, "Leaving get_api_tlv");*/
        return err;
}

/*
 * This function creates the control header from the information given.
 */
bxm_error_t
create_ctrl_hdr(uint8_t mod, uint8_t opcode, uint32_t len,
                bxmapi_ctrl_hdr *ctrl_hdr)
{

        bxm_error_t err = VPS_SUCCESS;
/*        vps_trace(VPS_ENTRYEXIT, "Entering create_ctrl_hdr");*/

        ctrl_hdr->mod_id = mod;
        ctrl_hdr->opcode = opcode;
        ctrl_hdr->length = len;

/*        vps_trace(VPS_ENTRYEXIT, "Leaving create_ctrl_hdr"); */
        return err;
}

/*
 * TODO Create TLVs.. NOT USING IT PRESENTLY..
 * Not needed for creating the tlv's.
 */
bxm_error_t
create_api_tlv(uint8_t type, uint32_t len, void *value, uint8_t *offset)
{
        api_tlv tlv;
        bxm_error_t err = VPS_SUCCESS;

/*        vps_trace(VPS_ENTRYEXIT, "Entering create_api_tlv");*/

        tlv.type = type;
        tlv.length = len;
        memcpy(offset, &tlv, 2);
        memcpy(offset + 2, value, tlv.length);
        offset +=(tlv.length + 2);

/*        vps_trace(VPS_ENTRYEXIT, "Leaving create_api_tlv");*/
        return err;

}

/* #ifdef BXM_TEST */
/* This Function just displays the buff */
display(uint8_t *buff, uint32_t size) {

        uint32_t i;

        printf("\n*** NEW MESAGE ***\n");
        for (i = 0; i< size; i++)
                printf("0x%x,", buff[i]);
        printf("\n");
}
/*#endif */
