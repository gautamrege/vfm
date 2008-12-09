/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */

#include <bxmapi_common.h>
#include <db_access.h>
#include <common.h>

/*
 * This function processes the message sent by the client.
 * It then converts it into a proper structure and then gives it to the
 * db access layer to do the functionality.
 *
 * [IN]  buff  : Contains the values of the TLVs.
 * [IN]  ret_pos : contains the value of the offset
 * [OUT] op_arg : any paraeters that is to given as output.
 */
bxm_error_t
process_bxm_create_vadpter(uint8_t *buff, uint32_t *ret_pos,
                res_packet *op_arg)
{

        bxm_vadapter_attr_t attr;

        vpsdb_resource vp_res;
        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering process_bxm_create_vadpter");

        get_api_tlv(buff, ret_pos, &(attr.io_module_id));
        get_api_tlv(buff, ret_pos, &(attr.protocol));
        get_api_tlv(buff, ret_pos, &(attr.name));
        get_api_tlv(buff, ret_pos, &(attr.desc));

        /*
         * TODO :
         * Then call the function from the DB to create the V_Adapter
         * This call will return the vadapter id of the newly created
         * V_Adpter. This value will be passed on to the Marshall response
         * process to send the response to the client.
         * Currently sending fixed value as 10.
         */
        vp_res.type = BXMAPI_VADAPTER;
        vp_res.count = 1;
        vp_res.data  = &attr;

        vpsdb_add_resource(VPS_DB_VADAPTER, &vp_res);

        op_arg->data  = (void *)(&attr._vadapter_id);

        vps_trace(VPS_INFO, "Vadapter id: %d", attr._vadapter_id);
        vps_trace(VPS_ENTRYEXIT, "Leaving process_bxm_create_vadpter");
        return err;

}


/*
 * This function processes the message sent by the client.
 * It then converts it into a proper structure and then gives it to the
 * db access layer to do the functionality.
 *
 * [IN]  buff  : Contains the values of the TLVs.
 * [IN]  ret_pos : contains the value of the offset
 * [OUT] op_arg : any paraeters that is to given as output.
 */
bxm_error_t
process_bxm_create_vfabric(uint8_t *buff, uint32_t *ret_pos,
                res_packet *op_arg)
{

        bxm_vfabric_attr_t attr;
        vpsdb_resource vp_res;
        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering process_bxm_create_vfabric");


        get_api_tlv(buff, ret_pos, &(attr.name));
        get_api_tlv(buff, ret_pos, &(attr.desc));
        get_api_tlv(buff, ret_pos, &(attr.protocol));

        /*
         * TODO :
         * Then call the function from the DB to create the V_Adapter
         * This call will return the vadapter id of the newly created
         * V_Adpter. This value will be passed on to the Marshall response
         * process to send the response to the client.
         * Currently sending fixed value as 10.
         */
        vp_res.type = BXMAPI_VFABRIC;
        vp_res.count = 1;
        vp_res.data  = &attr;

        vpsdb_add_resource(VPS_DB_VFABRIC, &vp_res);

        op_arg->data  = (void *)(&attr._vfabric_id);

        vps_trace(VPS_INFO, "Vadapter id: %d", attr._vfabric_id);
        vps_trace(VPS_ENTRYEXIT, "Leaving process_bxm_create_vfabric");
        return err;

}
/*
 * This function will process the request for editing the properties of
 * a VADPATER . The tlvs will be created and the function of the db access
 * layer to edit the properties will be called.
 *
 * [IN]  buff  : Contains the values of the TLVs.
 * [IN]  ret_pos : contains the value of the offset
 * [OUT] op_arg : any paraeters that is to given as output.
 */
bxm_error_t
process_bxm_edit_vadpter(uint8_t *buff, uint32_t *ret_pos, res_packet *op_arg)
{
        bxm_vadapter_id_t vadapter_id;
        bxm_vadapter_attr_bitmask_t bitmask;
        bxm_vadapter_attr_t attr, attr_db;

        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering process_bxm_edit_vadpter");


        void * void_ptr;
        get_api_tlv(buff, ret_pos, &vadapter_id);
        get_api_tlv(buff, ret_pos, &bitmask);
        get_api_tlv(buff, ret_pos, &attr);

        memset(&attr_db, 0, sizeof(bxm_vadapter_attr_t));
        /*
         * TODO :
         * Then call the function from the DB to edit the V_Adapter properties.
         * This function will call the database and update the struct and
         * return it to the databse.
         * eg :vpsdb_get_resource();
         */

        if (bitmask.id) {
                vps_trace(VPS_ERROR, "***--ID CANNOT BE CHANGED-- **");
                /*
                 * Later this can be used to create the a new vadpter.
                 * If this bit is set, and the value of id is NULL
                 * Then a new VAdpter will be created.
                 */
        }
        if (bitmask.name){
                attr_db.name = attr.name;
                vps_trace(VPS_INFO, "***-- VADAPTER NAME UPDATED --- **");
        }
        if (bitmask.desc){
                attr_db.desc = attr.desc;
                vps_trace(VPS_INFO, "***-- VADAPTER DESC UPDATED --- **");
        }
        if (bitmask.state) {
                attr_db._state = attr._state;
                vps_trace(VPS_INFO, "***-- VADAPTER STATE UPDATED --- **");
        }
        if (bitmask.init_type) {
                attr_db.init_type = attr.init_type;
                vps_trace(VPS_INFO, "***-- VADAPTER INIT_TYPE UPDATED -**");
        }
        if (bitmask.io_module_id) {
                attr_db.io_module_id = attr.io_module_id;
                vps_trace(VPS_INFO, "**-- VADAPTER IO_MODULE_ID UPDATED **");
        }
        if (bitmask.protocol) {

                attr_db.protocol  = attr.protocol;
                vps_trace(VPS_INFO, "***-- VADAPTER PROTOCOL UPDATED  **");
        }

        if (attr_db.protocol == BXM_PROTOCOL_EN) {
                memcpy(&(attr_db.en_attr), &(attr.en_attr),
                                sizeof(bxm_vadapter_en_attr_t));
                vps_trace(VPS_INFO, "** VADAPTER EN_ATTR UPDATED**");
        }
        else if (attr_db.protocol ==  BXM_PROTOCOL_FC) {
                memcpy(&(attr_db.en_attr), &(attr.en_attr),
                                sizeof(bxm_vadapter_en_attr_t));
                vps_trace(VPS_INFO, "** VADAPTER FC_ATTR UPDATED **");
        }

        if (bitmask.vfabric_id) {
                attr_db.vfabric_id= attr.vfabric_id;
                vps_trace(VPS_INFO, "**-- VADAPTER VFABRIC_ID UPDATED --**");
        }


        /* CAll the function of the db acces layer edit the properties
         * of the vadpter.
         *
         * eg: vpsds_edit_resource(type, &attr_db);
         */

#ifdef BXM_TEST
        void_ptr = &attr;
        display(void_ptr, sizeof(bxm_vadapter_attr_t));
        void_ptr = &attr_db;
        display(void_ptr, sizeof(bxm_vadapter_attr_t));
#endif

        op_arg->data  = NULL;

        vps_trace(VPS_ENTRYEXIT, "Leaving process_bxm_edit_vadpter");
}

/*
 * This function will create the message for the request .
 * The op parameter will be filled up (if any)
 * ctrl_hdr      [IN] : Control header from the request
 * buff      [IN/OUT] : This contains the op parameter that needs to
 *                      be sent to the client.
 */
bxm_error_t
bxm_vadpter_marshall_response(res_packet *buff, bxmapi_ctrl_hdr *ctrl_hdr,
                res_packet *pack, uint32_t no_of_args)
{
        uint8_t *message, *offset;
        api_tlv tlv;

        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering bxm_vadpter_marshall_response");

        ctrl_hdr->length = (sizeof(bxmapi_ctrl_hdr) + buff->size +
                        (2 * no_of_args));

        message = (uint8_t *)malloc(ctrl_hdr->length +
                        sizeof(bxmapi_ctrl_hdr));
        offset = message;

        pack->data = message;
        pack->size = ctrl_hdr->length;

        memcpy(offset, ctrl_hdr, sizeof(bxmapi_ctrl_hdr));
        offset += sizeof(bxmapi_ctrl_hdr);

        switch(ctrl_hdr->opcode) {
                case BXM_CREATE:
                        /* TYPE = 1 , bxm_vadapter_id_t */
                        create_api_tlv(TLV_INT, buff->size,
                                        buff->data, offset);
                        break;

                case BXM_EDIT:
                        create_api_tlv(TLV_CHAR, buff->size,
                                        buff->data, offset);
                        break;

                case BXM_QUERY_INVENTORY:
                        /* TYPE = Vadapter_id_t array */
                        memcpy(offset, buff->data, buff->size);
                        break;

                case BXM_QUERY:
                        create_api_tlv(TLV_VADP_ATTR, buff->size,
                                        buff->data, offset);
                        break;
        }

        vps_trace(VPS_ENTRYEXIT, "Leaving bxm_vadpter_marshall_response");
        return err;
}

/*
 * This function will first strip the message TLV's into values.
 * It will then give a call to the function with the parameters
 * which will return the number of vadapter_id's and the array of
 * vadapter_ids
 *
 * [IN]  buff    : Contains the values of the TLVs.
 * [IN]  ret_pos : contains the value of the offset
 * [OUT] op_arg  : Any paraeters that is to given as output.
 */
bxm_error_t
process_bxm_query_inventory(uint8_t *buff, uint32_t *ret_pos,
                             res_packet *temp)
{

        bxm_vadapter_attr_bitmask_t bitmask;
        bxm_vadapter_attr_t attr, attr_db;
        bxm_vadapter_id_t result[3];
        uint32_t num;

        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering process_bxm_query_inventory");

        get_api_tlv(buff, ret_pos, &bitmask);
        get_api_tlv(buff, ret_pos, &attr);

        /*
         * TODO :
         * Then call the function from the DB to query the V_Adapter's.
         * This function will return the number of vadapters and array of
         * vadapter ids.
         * eg :vpsdb_get_resource();
         */

        result[0] = 1;
        result[2] = 7;
        result[1] = 2;
        num = 3;


        temp->size = (sizeof(result) + sizeof(num) + 2*2);
        temp->data = malloc(temp->size);

        create_api_tlv(TLV_INT, sizeof(num), &num, temp->data);

        create_api_tlv(TLV_INT_ARR, sizeof(result), result,
                        (temp->data + sizeof(num) + 2));

        vps_trace(VPS_ENTRYEXIT, "Leaving process_bxm_query_inventory");
}


/*
 * This function will query the properties of the vadapter.
 */
bxm_error_t
process_bxm_query_vadapter(uint8_t *buff, uint32_t *ret_pos, void *op_data)
{

}





/*
 * This function will receive the message from the socket.
 * then process the message and do the required processing.
 * Once the processing is done, It will create the response
 * using the marshall_response() function into TLV's and
 * then send it on the socket to which it is bound.
 */
bxm_error_t
unmarshall_request(void *buff, uint32_t size, res_packet * pack)
{
        bxmapi_ctrl_hdr ctrl_hdr;
        uint32_t ret_pos = 0, num;
        uint32_t op_arg;
        res_packet op_data;

        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering unmarshall_request");

        /* Then process the control header */
        read_api_ctrl_hdr(buff, size, &ret_pos, &ctrl_hdr);
        switch (ctrl_hdr.mod_id) {
                case BXMAPI_VADAPTER:
                        switch(ctrl_hdr.opcode) {
                                case BXM_CREATE:
                                        process_bxm_create_vadpter(buff,
                                                        &ret_pos, &op_data);
                                        num = 1;
                                        bxm_vadpter_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, num);
                                        break;

                                case BXM_EDIT:
                                        process_bxm_edit_vadpter(buff,
                                                        &ret_pos, &op_data);
                                        num = 1;
                                        bxm_vadpter_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, num);
                                        break;
                                case BXM_QUERY_INVENTORY:
                                        process_bxm_query_inventory(buff,
                                                        &ret_pos, &op_data);

                                        bxm_vadpter_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, 2);
                                        break;
                                case BXM_QUERY:
                                        process_bxm_query_vadapter(buff,
                                                        &ret_pos, &op_data);

                                        bxm_vadpter_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, 1);
                                        break;
                        }
                        break;
                case BXMAPI_VFABRIC:
                        switch(ctrl_hdr.opcode) {


                                case BXM_CREATE:
                                        process_bxm_create_vfabric(buff,
                                                        &ret_pos, &op_data);
                                        num = 1;
                                        bxm_vadpter_marshall_response(&op_data,
                                                        &ctrl_hdr, pack, num);
                                        break;
                        }
                        break;
                case BXMAPI_BRIDGE_DEVICE:
                        switch(ctrl_hdr.opcode) {

                                case BXM_QUERY_INVENTORY:
                                        process_bxm_bd_select_inventory(buff,
                                                        &ret_pos, &op_data);
                                        bxm_bridge__marshall_response(&op_data,
                                                        &ctrl_hdr, pack, 2);
                                case BXM_QUERY:
                                        process_bxm_bd_query_general_attr(buff,
                                                        &ret_pos, &op_data);
                                        bxm_bridge__marshall_response(&op_data,
                                                        &ctrl_hdr, pack, 1);
                        }
                        break;

        }

        vps_trace(VPS_ENTRYEXIT, "Leaving unmarshall_request");
        return err;
}

