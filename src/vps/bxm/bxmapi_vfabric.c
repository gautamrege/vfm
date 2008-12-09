#include<bxmapi_common.h>

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
	 * Then call the function from the DB to create the V_Fabric
	 * This call will return the vfabric id of the newly created
	 * V_Fabric. This value will be passed on to the Marshall response
	 * process to send the response to the client.
	 */
	vp_res.type = BXMAPI_VFABRIC;
	vp_res.count = 1;
	vp_res.data  = &attr;

	vpsdb_add_resource(VPS_DB_VFABRIC, &vp_res);

	op_arg->data  = (void *)(&attr._vfabric_id);

	vps_trace(VPS_INFO, "V_fabric id: %d", attr._vfabric_id);
	vps_trace(VPS_ENTRYEXIT, "Leaving process_bxm_create_vfabric");
	return err;

}
/*
 * Change the running mode of the vfabric to ONLINE.
 * Note:  This operation modifies the running mode of the vfabric to ONLINE.
 * It means that BXM can activate the vfabric and start the services of the
 * vfabric i.e. it will turn the vadapters as ACTIVE and will start bridging
 * the packets between the host and the destination via the gateway.
 * After the call the BXM system will change the vfabric to ONLINE. If the 
 * primary gateway is ONLINE/ACTIVE, the vfabric will also be ACTIVE.
 * Parameters:
 * [in]   vfabric_id   The id of the vfabric to be activated.
 * Returns 0 on success, or an error code on failure.
 */

bxm_error_t
process_bxm_vfabric_online(uint8_t *buff, uint32_t *ret_pos,
		                res_packet *op_arg)
{

	bxm_vfabric_id_t vfabric_id;
	bxm_vfabric_attr_t vfabric_attr;
	bxm_vadapter_attr_t vadapter_attr;
	vpsdb_io_module io_module;
	req_action req;
	vpsdb_resource vp_res;

	vps_error err = VPS_SUCCESS;
	vps_trace(VPS_ENTRYEXIT, "Entering process_bxm_create_vfabric");


	get_api_tlv(buff, ret_pos, &vfabric_id);

	/*
	 * TODO :
	 * Then call the function from the DB to get the values from the DB. 
	 */

	
	vp_res.type = BXMAPI_VFABRIC;
	vp_res.count = 1;
	vp_res.data  = &vfabric_attr;

	err = vpsdb_get_resource(VPS_DB_VFABRIC, &vp_res);
	if(err)
		goto out;

	for (i = 0; i < vfabric_attr._num_vadapter; i++) {

		vp_res.type = BXMAPI_VADAPTER;
		vp_res.count = 1;
		vadapter_attr._vadapter_id = vfabric_attr._vadapter_id[i];
		vp_res.data  = &vadapter_attr;

		err = vpsdb_get_resource(VPS_DB_VADAPTER, &vp_res);
		if(err)
			goto out;


		vp_res.type = VPS_DB_IO_MODULE;
		vp_res.count = 1;
		io_module.id = vadapter_attr.io_module_id;
		vp_res.data  = &io_module;

		err = vpsdb_get_resource(VPS_DB_IO_MODULE, &vp_res);
		if(err)
			goto out;
		/* Call the function which will send the Advertisement */
		form_req_struct(vfabric_attr.primary_gateway, io_module.mac,
				INIT_VHBA, &req);
		create_packet(&req);

	}

out:	vps_trace(VPS_ENTRYEXIT, "Leaving process_bxm_create_vfabric");
	return err;
}
