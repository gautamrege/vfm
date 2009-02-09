/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */

#include <vfmapi_common.h>
#include <vfmdb.h>
#include <vfmdb_vadapter.h>
#include <vfmdb_iomodule.h>
#include <vfmdb_gateway.h>
#include <vfmdb_vfabric.h>
#include <common.h>
#include <vfm_fip.h>

extern uint8_t g_bridge_enc_mac[MAC_ADDR_LEN];

/*
 * This function processes the message sent by the client.
 * It then converts it into a proper structure and then gives it to the
 * db access layer to do the functionality.
 *
 * [IN]  buff  : Contains the values of the TLVs.
 * [IN]  ret_pos : contains the value of the offset
 * [OUT] op_arg : any paraeters that is to given as output.
 */
vfm_error_t
process_vfm_create_vadpter(uint8_t *buff, uint32_t *ret_pos,
                res_packet *op_arg)
{

        vfm_vadapter_attr_t attr;

        vpsdb_resource vp_res;
        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering process_vfm_create_vadpter");

	memset(&attr, 0, sizeof(vfm_vadapter_attr_t));
        get_api_tlv(buff, ret_pos, (attr.name));
        get_api_tlv(buff, ret_pos, (attr.desc));
        get_api_tlv(buff, ret_pos, &(attr.protocol));

        /*
         * Then call the function from the DB to create the V_Adapter
         * This call will return the vadapter id of the newly created
         * V_Adpter. This value will be passed on to the Marshall response
         * process to send the response to the client.
         */
        vp_res.type = VFMAPI_VADAPTER;
        vp_res.count = 1;
        vp_res.data  = &attr;

        err = vpsdb_add_resource(VPS_DB_VADAPTER, &vp_res);

	if (VPS_SUCCESS != err) {

		vps_trace(VPS_ERROR, "*** ERROR creating Vadapter ***");
		op_arg->size  = sizeof(vfm_error_t);
		op_arg->data  = (uint32_t *)malloc(op_arg->size);
		memcpy(op_arg->data, &err, sizeof(vfm_error_t));
		goto out;
	}

	op_arg->data  = malloc(sizeof(vfm_vadapter_id_t));
	memcpy(op_arg->data, vp_res.data, sizeof(vfm_vadapter_id_t));
	op_arg->size  = sizeof(vfm_vadapter_id_t);

	vps_trace(VPS_INFO, "Vadapter id: %d",
			*((vfm_vadapter_id_t*)vp_res.data));

out:
	vps_trace(VPS_ENTRYEXIT, "Leaving process_vfm_create_vadpter");
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
vfm_error_t
process_vfm_edit_vadpter(uint8_t *buff, uint32_t *ret_pos, res_packet *op_arg)
{
        vfm_vadapter_id_t vadapter_id;
        vfm_vadapter_attr_bitmask_t bitmask;
        vfm_vadapter_attr_t attr;
        char query[1024];
        uint32_t count = 0;

        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering process_vfm_edit_vadpter");

        memset(query, 0 , sizeof(query));

        void * void_ptr;
        get_api_tlv(buff, ret_pos, &vadapter_id);
        get_api_tlv(buff, ret_pos, &bitmask);
        get_api_tlv(buff, ret_pos, &attr);

        if (bitmask.id) {
                vps_trace(VPS_ERROR, "***--ID CANNOT BE CHANGED-- **");
                /*
                 * Later this can be used to create the a new vadpter.
                 * If this bit is set, and the value of id is NULL
                 * Then a new VAdpter will be created.
                 */
        }
        if (bitmask.state){
                add_update_query_parameters(query, count++, "status",
                                                 &attr._state, Q_UINT32);
        }
        if (bitmask.name){
                add_update_query_parameters(query, count++, "`name`",
                                                 attr.name, Q_UINT8);
        }
        if (bitmask.desc){
                add_update_query_parameters(query, count++, "`desc`",
                                                 attr.desc, Q_UINT8);
        }
        if (bitmask.init_type) {
                add_update_query_parameters(query, count++, "init_type",
                                                 &attr.init_type, Q_UINT32);
        }
        if (bitmask.io_module_id) {
                add_update_query_parameters(query, count++, "io_module_id",
                                                 &attr.io_module_id, Q_UINT32);
        }
        if (bitmask.protocol) {
                add_update_query_parameters(query, count++, "protocol",
                                                 &attr.protocol, Q_UINT32);
        }
        if (bitmask.vfabric_id) {
                add_update_query_parameters(query, count++, "vfabric_id",
                                                 &attr.vfabric_id, Q_UINT32);
        }

	if(count > 0)	
		sprintf(query, "%s where id = %d;" , query, vadapter_id);

        /* CAll the function of the db acces layer edit the properties
         * of the vadpter.
         */
        err = vpsdb_edit_resource(VPS_DB_VADAPTER, query);
         

#ifdef VFM_TEST
        void_ptr = &attr;
        display(void_ptr, sizeof(vfm_vadapter_attr_t));
        void_ptr = &attr_db;
        display(void_ptr, sizeof(vfm_vadapter_attr_t));
#endif

        op_arg->size  = sizeof(vfm_error_t);
        op_arg->data  = malloc(op_arg->size);
        memcpy(op_arg->data, &err, sizeof(vfm_error_t));

        vps_trace(VPS_ENTRYEXIT, "Leaving process_vfm_edit_vadpter");
}

/**
 * @brief
 * This function will first strip the message TLV's into values.
 * It will then give a call to the function with the parameters
 * which will return the number of vadapter_id's and the array of
 * vadapter_ids
 *
 * @param[IN]  buff    : Contains the values of the TLVs.
 * @param[IN]  ret_pos : contains the value of the offset
 * @param[OUT] op_arg  : Any paraeters that is to given as output.
 * 
 * @return Returns 0 on success, or an error code on failure.
 */
vfm_error_t
process_vfm_vadapter_select_inventory(uint8_t *buff, uint32_t *ret_pos,
                             res_packet *op_arg)
{
        int i;
        vfm_vadapter_attr_bitmask_t *bitmask;
        vfm_vadapter_attr_t *attr =NULL;

        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering process_vfm_vadapter_select_inventory");

        /* The buff contains vadapter structure and bitmask
         * This function will first strip the message TLV's into values.
         */
        unpack_tlv(buff, ret_pos, &bitmask);
        unpack_tlv(buff, ret_pos, &attr);

        /*
         * Call the populate vadapter function which will populate
         * the vadapter information from the database.
         */
        populate_vadapter_information(bitmask, &attr, op_arg);
#ifdef VFMAPI_VADAPTER_TEST
        for (i=0; i < op_arg->count; i++) {
                op_arg->data += TLV_SIZE;
                show_vadapter_data(op_arg->data);
                op_arg->data += sizeof(vfm_vadapter_attr_t);
        }
#endif
        /*
         * Then fill the op_data size and the data.
         * size = count*sizeof structure
         * data = res.data
         */
        vps_trace(VPS_ENTRYEXIT, "Leaving process_vfm_vadapter_select_inventory");
}


/*
 * @brief This function will query the properties of the vadapter.
 * This function will first strip the message TLV's into values.
 * It will then give a call to the function with the parameters
 * which will return vadapter structure.
 * 
 * @param[IN]  *buff  : Contains the values of the TLVs.
 * @param[IN]  *ret_pos : contains the value of the offset
 * @param[OUT] *op_data : any parameters that is to given as output.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
vfm_error_t
process_vfm_vadapter_query_general_attrs(uint8_t *buff, uint32_t *ret_pos,
                res_packet *op_data)
{

        /*
         * The buff contains vadapter_id and bitmask
         * This function will first strip the message TLV's into values.
         * Collect the vadapter_id in the vadapter structure id variable.
         */

        /*
         * Call the populate vadapter function which will populate
         * the vadapter information from the database.
         * populate_vadapter_information(bitmask, &attr, &vps_res)
         */


        /*
         * The value will be returned in the op_data which will be passed
         * Then fill the op_data size and the data.
         * size = count*sizeof structure
         * data = res.data
         */
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
vfm_error_t
process_vfm_edit_en_attr(uint8_t *buff, uint32_t *ret_pos, res_packet *op_arg)
{
        vfm_vadapter_id_t vadapter_id;
        vfm_vadapter_en_attr_bitmask_t bitmask;
        vfm_vadapter_en_attr_t attr;
        char query[1024]= "update vfm_vadapter_en_attr set";
        char fmt[10];
        uint32_t fmt_count = 0, count = 0;
	void *stmt, *void_ptr;
	char * temp = query;

        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering process_vfm_edit_en_attr");


	/* The fmt will contain the format specifiers */
	memset(fmt, 0, sizeof(fmt));
        get_api_tlv(buff, ret_pos, &vadapter_id);
        get_api_tlv(buff, ret_pos, &bitmask);
        get_api_tlv(buff, ret_pos, &attr);

	/* Replcace the value if the mac by ? for late SQLITE binding*/
        if (bitmask.mac) {
		temp = temp + strlen(query);
		sprintf(temp, "%s mac = ?", temp);
		sprintf(fmt, "m");
		count++;
	}
        if (bitmask.vlan)
                add_update_query_parameters(temp, count++, "vlan",
				&attr.vlan, Q_UINT32);
        
        if (bitmask.promiscuous_mode)
                add_update_query_parameters(temp, count++, "promiscuous",
				&attr.promiscuous_mode, Q_UINT32);

        if (bitmask.silent_listener)
                add_update_query_parameters(temp, count++, "silent_listener",
				&attr.silent_listener, Q_UINT32);

	if(count > 0)	
	sprintf(query, "%s where vadapter_id = %d;" , query, vadapter_id);

	if (fmt)
		stmt = vfmdb_prepare_query_ex(query, fmt, attr.mac);
	else 
		stmt = vfmdb_prepare_query(query, fmt, NULL);
        if (!stmt) {
                vps_trace(VPS_ERROR," Cannot prepare sqlite3 statement");
                err = VPS_DBERROR;
                goto out;
        }

        if (VPS_SUCCESS != (err = vfmdb_execute_query(stmt, NULL, NULL))) {
                vps_trace(VPS_ERROR, "Could not edit EN properties ");
                err = VPS_DBERROR;
                goto out;
        }


        /* CAll the function of the db acces layer edit the properties
         * of the vadpter.
         */
        // err = vpsdb_edit_resource(VPS_DB_EN_VADAPTER, query);

#ifdef VFM_TEST
        void_ptr = &attr;
        display(void_ptr, sizeof(vfm_vadapter_attr_t));
        uint8_t host_mac[6] = {0x01, 0x02, 0xc9, 0x01, 0xc6, 0xf4};
        char query[1024]= "update vfm_vadapter_en_attr set mac = ?1";
#endif
out:
        op_arg->size  = sizeof(vfm_error_t);
        op_arg->data  = (uint32_t *)malloc(op_arg->size);
        memcpy(op_arg->data, &err, sizeof(vfm_error_t));

        vps_trace(VPS_ENTRYEXIT, "Leaving process_vfm_edit_en_attr");
}

vfm_error_t
process_vfm_vadapter_query_protocol_att(uint8_t *buff,
		uint32_t *ret_pos, res_packet *op_arg)
{
        vfm_vadapter_id_t vadapter_id;
        vfm_vadapter_en_attr_bitmask_t bitmask;
        vfm_vadapter_en_attr_t attr;
        vpsdb_resource vp_rsc;
        char query[1024]= "select * from vfm_vadapter_en_attr ";
        char fmt[10];
        uint32_t fmt_count = 0, count = 0;
	void *stmt, *void_ptr;
	char * temp = query;

        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering process_vfm_query_protocol_attr");


	/* The fmt will contain the format specifiers */
	memset(fmt, 0, sizeof(fmt));
        get_api_tlv(buff, ret_pos, &vadapter_id);
        get_api_tlv(buff, ret_pos, &bitmask);
      //  get_api_tlv(buff, ret_pos, &attr);

	/* Replcace the value if the mac by ? for late SQLITE binding/
        if (bitmask.mac) {
		temp = temp + strlen(query);
		sprintf(temp, "%s mac = ?", temp);
		sprintf(fmt, "m");
		count++;
	}
        if (bitmask.vlan)
                add_query_parameters(temp, count++, "vlan",
				&attr.vlan, Q_UINT32);

        if (bitmask.promiscuous_mode)
                add_query_parameters(temp, count++, "promiscuous",
				&attr.promiscuous_mode, Q_UINT32);

        if (bitmask.silent_listener)
                add_query_parameters(temp, count++, "silent_listener",
				&attr.silent_listener, Q_UINT32);
	if(count > 0)	
*/
	sprintf(query, "%s where vadapter_id = %d;" , query, vadapter_id);

	if (fmt)
		stmt = vfmdb_prepare_query_ex(query, fmt, attr.mac);
	else
		stmt = vfmdb_prepare_query_ex(query, fmt, NULL);
        if (!stmt) {
                vps_trace(VPS_ERROR," Cannot prepare sqlite3 statement");
                err = VPS_DBERROR;
                goto out;
        }

        if (VPS_SUCCESS != (err = vfmdb_execute_query(stmt,
					process_vadapter_en,
					&vp_rsc))) {
                vps_trace(VPS_ERROR, "Could not get EN properties ");
                err = VPS_DBERROR;
                goto out;
        }


        /* CAll the function of the db acces layer edit the properties
         * of the vadpter.
         */
        // err = vpsdb_edit_resource(VPS_DB_EN_VADAPTER, query);
         

#ifdef VFM_TEST
        void_ptr = &attr;
        display(void_ptr, sizeof(vfm_vadapter_attr_t));
#endif
out:
        op_arg->size  = sizeof(vfm_error_t);
        op_arg->data  = (uint32_t *)malloc(op_arg->size);
        memcpy(op_arg->data, &err, sizeof(vfm_error_t));

        vps_trace(VPS_ENTRYEXIT, "Leaving process_vfm_query_protocol_attr");
}

/*** TODO: Move common routines into vfmdb files ****/
vps_error __vadapter_online(vfm_vadapter_id_t vadapter_id)
{
	vps_error err = VPS_SUCCESS;
	char query[1024];
	void *stmt;
        uint8_t **offset;
	fcoe_conx_vfm_adv adv;
	vpsdb_resource rsc;
        res_packet rsc_pack;
	vfm_vadapter_attr_t *vadapter;
	vpsdb_io_module_t *io_module;
	vfm_gateway_attr_t *gateway;
	vfm_vfabric_attr_t *vfabric;

        vps_trace(VPS_ENTRYEXIT, "Entering %s", __FUNCTION__);

	/**** Find the vadapter record ****/
	sprintf(query, "select * from vfm_vadapter_attr where id = %d",
			vadapter_id);
	stmt = vfmdb_prepare_query(query, NULL, NULL);
        if (!stmt) {
                vps_trace(VPS_ERROR," Cannot prepare sqlite3 statement");
                err = VPS_DBERROR;
                goto out;
        }

	memset(&rsc, 0, sizeof(vpsdb_resource));
        if (VPS_SUCCESS != vfmdb_execute_query(stmt,
					process_vadapter,
					&rsc)) {
                vps_trace(VPS_ERROR, "Could not get vadapter");
                err = VPS_DBERROR;
                goto out;
        }

        if (rsc.count <= 0) {
                vps_trace(VPS_ERROR, " No Vadapter found ");
                err = VFM_ERROR_IOMODULE;
                goto out;
        }
	/* Populate the en / fc attributes */
	vadapter = (vfm_vadapter_attr_t*)rsc.data;
        /* TODO: Fix the following line later */
	//populate_vadapter_ex(vadapter);

	/**** Find the I/O Module record for this vadapter ****/
	sprintf(query, "select * from vfm_io_module_attr where id = %d",
			vadapter->io_module_id);
	stmt = vfmdb_prepare_query(query, NULL, NULL);
        if (!stmt) {
                vps_trace(VPS_ERROR," Cannot prepare sqlite3 statement");
                err = VPS_DBERROR;
                goto out;
        }

	memset(&rsc, 0, sizeof(vpsdb_resource));
        if (VPS_SUCCESS != vfmdb_execute_query(stmt,
					process_io_module,
					&rsc)) {
                vps_trace(VPS_ERROR, "Could not get io_module");
                err = VPS_DBERROR;
                goto out;
        }

        if (rsc.count <= 0) {
                vps_trace(VPS_ERROR, "No IO Module associated with Vadapter");
                err = VFM_ERROR_IOMODULE;
                goto out;

        }
	io_module = (vpsdb_io_module_t *)rsc.data;

	/**** Find the vfabric record ****/
	sprintf(query, "select * from vfm_vfabric_attr where id = %d",
			vadapter->vfabric_id);
	stmt = vfmdb_prepare_query(query, NULL, NULL);
        if (!stmt) {
                vps_trace(VPS_ERROR," Cannot prepare sqlite3 statement");
                err = VPS_DBERROR;
                goto out;
        }

	memset(&rsc_pack, 0, sizeof(res_packet));
        if (VPS_SUCCESS != vfmdb_execute_query(stmt,
					process_vfabric,
					&rsc_pack)) {
                vps_trace(VPS_ERROR, "Could not get vadapter");
                err = VPS_DBERROR;
                goto out;
        }
        if (rsc.count <= 0) {
                vps_trace(VPS_ERROR, "No Vfabric associated with Vadapter");
                err = VFM_ERROR_VFABRIC;
                goto out;

        }
        /* assign the offset to rsc_pack.data*/
        offset = (uint8_t **)rsc_pack.data;
	vfabric = (vfm_vfabric_attr_t *)(offset[0] + TLV_SIZE);

	/**** Find the gateway record ****/
	sprintf(query, "select * from vfm_gateway_attr where gw_id = %d",
			vfabric->primary_gateway);
	stmt = vfmdb_prepare_query(query, NULL, NULL);
        if (!stmt) {
                vps_trace(VPS_ERROR," Cannot prepare sqlite3 statement");
                err = VPS_DBERROR;
                goto out;
        }

	memset(&rsc, 0, sizeof(vpsdb_resource));
        if (VPS_SUCCESS != vfmdb_execute_query(stmt,
					process_gateway,
					&rsc)) {
                vps_trace(VPS_ERROR, "Could not get vadapter");
                err = VPS_DBERROR;
                goto out;
        }
	gateway = (vfm_gateway_attr_t *)rsc.data;

        /*
         * HACK: Ideally, we should use hte gateway MAC, but for now we use
         * the hard-coded global MAC.
         */
	/*form_req_struct(g_bridge_enc_mac, io_module->mac, INIT_VHBA, &req); */

	/* HACK: Hansraj -- please fix properly */
	memset(&adv, 0, sizeof(adv));
	memcpy(adv.host_mac, io_module->mac, 6);
	adv.max_recv = 1000;
	/* Call the function which will send the Advertisement */
        vps_trace(VPS_INFO, "Sending UNICAST ADV to host");
	create_packet(1, 2, &adv);
out:
        vps_trace(VPS_ENTRYEXIT, "Leaving %s", __FUNCTION__);
	return err;
}

/*
 * Change the running mode of the vadapter to ONLINE.
 * Note:  This operation modifies the running mode of the vadapter to ONLINE.
 * It means that VFM can activate the vadapter and start the services of the
 * vadapter i.e. it will turn the vadapters as ACTIVE and will start bridging
 * the packets between the host and the destination via the gateway.
 * After the call the VFM system will change the vadapter to ONLINE. If the
 * primary gateway is ONLINE/ACTIVE, the vfabric will also be ACTIVE.
 *
 * [in]   vadapter_id   The id of the vadapter to be activated.
 * Returns 0 on success, or an error code on failure.
 */

vfm_error_t
process_vfm_vadapter_online(uint8_t *buff, uint32_t *ret_pos,
		                res_packet *op_arg)
{
	vfm_vadapter_id_t vadapter_id;

	vps_error err = VPS_SUCCESS;
	vps_trace(VPS_ENTRYEXIT, "Entering process_vfm_vadapter_online");

	get_api_tlv(buff, ret_pos, &vadapter_id);

        if ( VPS_SUCCESS != __vadapter_online(vadapter_id)) {
                vps_trace(VPS_WARNING, "Could not activate vadapter: %d",
                                vadapter_id);
        }
        else
        {
                vps_trace(VPS_INFO, "Vadapter %d successfully activated",
                                vadapter_id);
        }

        op_arg->size  = sizeof(vfm_error_t);
        op_arg->data  = malloc(op_arg->size);
        memcpy(op_arg->data, &err, sizeof(vfm_error_t));

	vps_trace(VPS_ENTRYEXIT, "Leaving process_vfm_vadapter_online");
	return err;
}
