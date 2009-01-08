#include <vfmapi_common.h>
#include <vfm_fip.h>
#include <vfmdb.h>
#include <vfmdb_vfabric.h>
#include <vfmdb_vadapter.h>

extern uint8_t g_bridge_enc_mac[MAC_ADDR_LEN];
uint8_t host_mac[MAC_ADDR_LEN] = {0x00, 0x02, 0xc9, 0x01, 0xc6, 0xf4};

/*
 * This function creates the request to send a GW advertisement
 */
vps_error
form_req_struct(uint8_t *src, uint8_t *dest, uint8_t type, req_action *req)
{
	vps_error err = VPS_SUCCESS;
	vps_trace(VPS_ENTRYEXIT, "Entering form_req_struct");

	req->control_hdr.opcode = 1;
	req->control_hdr.subcode = 2;
	req->control_hdr.desc_list_length = 13;
	req->control_hdr.flags |= SET_FP;
	req->control_hdr.flags |= SET_SP;
	req->control_hdr.flags |= SET_F;
	/* TODO: Currently the max_recv = 1000, Later to be read from DB*/
	req->max_recv = 1000;


	if (type == KEEP_ALIVE)
		req->control_hdr.flags |= SET_S;

	if (type == INIT_VHBA) {
		req->control_hdr.flags |= SET_S;
		req->control_hdr.flags |= SET_A;
	}

	memcpy(req->host_mac, dest, MAC_ADDR_LEN);
	memcpy(req->gw_mac, src, MAC_ADDR_LEN);

	vps_trace(VPS_ENTRYEXIT, "Leaving form_req_struct");
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
vfm_error_t
process_vfm_create_vfabric(uint8_t *buff, uint32_t *ret_pos,
		res_packet *op_arg)
{

	vfm_vfabric_attr_t attr;
	vpsdb_resource vp_res;
	vps_error err = VPS_SUCCESS;
	vps_trace(VPS_ENTRYEXIT, "Entering process_vfm_create_vfabric");


	get_api_tlv(buff, ret_pos, &(attr.name));
	get_api_tlv(buff, ret_pos, &(attr.desc));
	get_api_tlv(buff, ret_pos, &(attr.protocol));

	/*
	 * Then call the function from the DB to create the V_Fabric
	 * This call will return the vfabric id of the newly created
	 * V_Fabric. This value will be passed on to the Marshall response
	 * process to send the response to the client.
	 */
        vp_res.count = 1;
	vp_res.data  = &attr;

	vpsdb_add_resource(VPS_DB_VFABRIC, &vp_res);

        op_arg->size  = sizeof(vfm_vfabric_id_t);
        op_arg->data  = malloc(op_arg->size);
        memcpy(op_arg->data, vp_res.data, sizeof(vfm_vfabric_id_t));

        vps_trace(VPS_INFO, "Vfabric id: %d",
			*((vfm_vfabric_id_t*)vp_res.data));

	vps_trace(VPS_ENTRYEXIT, "Leaving process_vfm_create_vfabric");
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
process_vfm_edit_vfabric(uint8_t *buff, uint32_t *ret_pos, res_packet *op_arg)
{
        vfm_vfabric_id_t vfabric_id;
        vfm_vfabric_attr_bitmask_t bitmask;
        vfm_vfabric_attr_t attr, attr_db;
        char query[1024];
        void * void_ptr;
        uint32_t count = 0;

        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering process_vfm_edit_vadpter");

        get_api_tlv(buff, ret_pos, &vfabric_id);
        get_api_tlv(buff, ret_pos, &bitmask);
        get_api_tlv(buff, ret_pos, &attr);

        memset(query, 0, sizeof(query));

        /* Create the query to edit the db according to the bitmask set */
        if (bitmask.vfm_vfabric_id) 
                vps_trace(VPS_ERROR, "***--ID CANNOT BE CHANGED-- **");
                /*
                 * Later this can be used to create the a new vadpter.
                 * If this bit is set, and the value of id is NULL
                 * Then a new VAdpter will be created.
                 */
        
        if (bitmask.name)
                add_update_query_parameters(query, count++, "`name`",
                                                 attr.name, Q_UINT8);
        
        if (bitmask.desc)
                add_update_query_parameters(query, count++, "`desc`",
                                                      attr.desc, Q_UINT8);
        
        if (bitmask.primary_gateway) 
                add_update_query_parameters(query, count++, "primary_gw_id",
                                &(attr.primary_gateway), Q_UINT32);
        
        if (bitmask.backup_gateway) 
                add_update_query_parameters(query, count++, "backup_gw_id",
                                &attr.backup_gateway, Q_UINT32);
        
        if (bitmask.protocol_attr) 
                add_update_query_parameters(query, count++, "protocol",
                                                 &attr.protocol, Q_UINT32);
        
        if (bitmask.ctx_table_id) 
                add_update_query_parameters(query, count++, "ctx_table_id",
                                                 &attr._ctx_table_id, Q_UINT32);
        

	if(count > 0)	
		sprintf(query, "%s where id = %d;" , query, vfabric_id);

        /* Call the function of the db acces layer edit the properties
         * of the vfabric.
         */
        err = vpsdb_edit_resource(VPS_DB_VFABRIC, query);
         

#ifdef VFM_TEST
        void_ptr = &attr;
        display(void_ptr, sizeof(vfm_vfabric_attr_t));
        void_ptr = &attr_db;
        display(void_ptr, sizeof(vfm_vfabric_attr_t));
#endif
        op_arg->size  = sizeof(vfm_error_t);
        op_arg->data  = malloc(op_arg->size);
        memcpy(op_arg->data, &err, sizeof(vfm_error_t));

        vps_trace(VPS_ENTRYEXIT, "Leaving process_vfm_edit_vfabric");
}


/*
 * Change the running mode of the vfabric to ONLINE.
 * Note:  This operation modifies the running mode of the vfabric to ONLINE.
 * It means that VFM can activate the vfabric and start the services of the
 * vfabric i.e. it will turn the vadapters as ACTIVE and will start bridging
 * the packets between the host and the destination via the gateway.
 * After the call the VFM system will change the vfabric to ONLINE. If the 
 * primary gateway is ONLINE/ACTIVE, the vfabric will also be ACTIVE.
 *
 * [in]   vfabric_id   The id of the vfabric to be activated.
 * Returns 0 on success, or an error code on failure.
 */

vfm_error_t
process_vfm_vfabric_online(uint8_t *buff, uint32_t *ret_pos,
		                res_packet *op_arg)
{
	vfm_vfabric_id_t vfabric_id;
	vfm_vfabric_attr_t *vfabric_attr;
	vfm_vadapter_attr_t *vadapter;
	vpsdb_io_module_t *io_module;
	vfm_gateway_attr_t *gateway_attr;
	req_action req;
	char query[1024];
	void *stmt;
	vpsdb_resource res;
	int i, count;

	vps_error err = VPS_SUCCESS;
	vps_trace(VPS_ENTRYEXIT, "Entering process_vfm_create_vfabric");


	get_api_tlv(buff, ret_pos, &vfabric_id);

	/* TODO: Update the vfabric cached object with status = online */

	/*
	 * The vfabric structure will contains the vadapter id's. and the num
	 * of vadpaters.
	 * Using this info we can find the vapater io_module and gets its MAC.
	 * and using the primary gateway id from vfabric, get the physical
	 * index and find its MAC. This information is given to the form_req 
	 * function and then a request can be formed using the create_packet.
	 *
	 * NOTE: create_packet function is from the vfm_create_packet file, 
	 * and is changed from the previous one to take only one argument.
	 */
	sprintf(query, "select * from vfm_vadapter_attr where vfabric_id = %d",
			vfabric_id);
	stmt = vfmdb_prepare_query(query, NULL, NULL);
        if (!stmt) {
                vps_trace(VPS_ERROR,"Cannot prepare sqlite3 statement");
                err = VPS_DBERROR;
                goto out;
	}

	/* Get all the vadapter objects */
	memset(&res, 0, sizeof(vpsdb_resource));
        if (VPS_SUCCESS != (err = vfmdb_execute_query(stmt,
					process_vadapter,
					&res))) {
                vps_trace(VPS_ERROR, "Could not get the vfabric object ");
                err = VPS_DBERROR;
                goto out;
        }

	/* For each vadapter, call vfm_vadapter_online API */
	for (i = 0; i < res.count; i++) {
		vadapter = ((vfm_vadapter_attr_t *)res.data) + i;
		if ( VPS_SUCCESS != __vadapter_online(vadapter->_vadapter_id)) {
			vps_trace(VPS_WARNING, "Could not activate adapter: %d",
					vadapter->_vadapter_id);
		}
                else
                {
			vps_trace(VPS_INFO, "Vadapter %d successfully activated",
					vadapter->_vadapter_id);
                }
	}

out:    op_arg->size  = sizeof(vfm_error_t);
        op_arg->data  = malloc(op_arg->size);
        memcpy(op_arg->data, &err, sizeof(vfm_error_t));

	vps_trace(VPS_ENTRYEXIT, "Leaving process_vfm_create_vfabric");
	return err;
}


process_edit_protocol_attr()
{
/*
        TODO : use this for edit protocol attr
        if (attr.protocol == VFM_PROTOCOL_EN) {
		strcpy(query, " update vfm_vfabric_en_attr set ");

		sprintf(query, "%s update vfm_vfabric_en_attr set
				where id = %d;" , query, vfabric_id);
		
                memcpy(&(attr_db.en_attr), &(attr.en_attr),
				
                                sizeof(vfm_vfabric_en_attr_t));
                vps_trace(VPS_INFO, "** VADAPTER EN_ATTR UPDATED**");
        
        else if (attr_db.protocol ==  VFM_PROTOCOL_FC) {
                memcpy(&(attr_db.en_attr), &(attr.en_attr),
                                sizeof(vfm_vfabric_en_attr_t));
                vps_trace(VPS_INFO, "** VADAPTER FC_ATTR UPDATED **");
        
*/
}
