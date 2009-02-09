
/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#include <vfmapi_common.h>

vps_error
create_tlv_packet(uint8_t type, uint32_t len, void *value, uint8_t *offset)
{
        api_tlv tlv;
        vps_error err = VPS_SUCCESS;
        uint32_t tlv_size = sizeof(tlv.type) + sizeof(tlv.length);

        tlv.type = htonl(type);
        tlv.length = htonl(len);
        memcpy(offset, &tlv, TLV_SIZE);
        memcpy(offset + TLV_SIZE, value, len);
        return err;
}

int
process_vadapter_id(void *data, int num_cols, uint8_t **values, char **cols)
{
        res_packet *rsc = (res_packet*)data;
        uint32_t *vadapter_id;
        uint32_t i;

        vps_trace(VPS_ENTRYEXIT, "Entering process_vadapter_id");
        
        vadapter_id = (rsc->data + sizeof(vfm_vfabric_attr_t) + TLV_SIZE +
                                (rsc->count * sizeof(vfm_vadapter_id_t)));

        /* Fill the vfabric structure */
        for (i = 0; i < num_cols; i++) {
                if (strcmp(cols[i], "id") == 0)
                        *vadapter_id = atoi(values[i]);
        }
        rsc->count++;
        vps_trace(VPS_ENTRYEXIT, "Leaving process_vadapter_id");
        return 0; /* Callback must return 0 on success */
}

/*
 * This function will get the vadapter id from the databases which are attached
 * to vfabric id.
 * ** THE Memory is preallocated so dont allocate the momory again. **
 */
vfm_error_t
populate_vadapter_id(vfm_vfabric_id_t vfabric_id, res_packet *rsc)
{
        char query[512];
        void *stmt;
        vfm_error_t err = VPS_SUCCESS;

        sprintf(query, "select id from vfm_vadapter_attr where vfabric_id =%d", 
                                                                vfabric_id);
        stmt = vfmdb_prepare_query(query, NULL, NULL);

        if (!stmt) {
                vps_trace(VPS_ERROR," Cannot prepare sqlite3 statement");
                err = VPS_DBERROR;
                goto out;
        }
        /*call process_vfabric and get the data of vfabric from the database*/
        if (VPS_SUCCESS != (err = vfmdb_execute_query(stmt,
                                        process_vadapter_id,
                                        rsc))) {
                vps_trace(VPS_ERROR, "Could not get EN properties ");
                err = VPS_DBERROR;
                goto out;
        }
out:
        return err;
}

int
process_vfabric_en_attr(void *data, int num_cols, uint8_t **values, char **cols)
{
        vfm_vfabric_attr_t *vfabric = (vfm_vfabric_attr_t *)data;
        uint32_t i;

        vps_trace(VPS_ENTRYEXIT, "Entering process_vfabric_en_attr");
        
        /* Fill the vfabric structure */
        for (i = 0; i < num_cols; i++) {
                if (strcmp(cols[i], "vlan") == 0)
                        vfabric->en_attr.vlan = atoi(values[i]);
                if (strcmp(cols[i], "mac") == 0)
                        memcpy(vfabric->en_attr._mac, values[i],
                                        sizeof(en_mac_addr_t));
        }
        vps_trace(VPS_ENTRYEXIT, "Leaving process_vfabric_en_attr");
        return 0; /* Callback must return 0 on success */
}

int
process_vfabric_fc_attr(void *data, int num_cols, uint8_t **values, char **cols)
{
        vfm_vfabric_attr_t *vfabric = (vfm_vfabric_attr_t *)data;
        uint32_t i;

        vps_trace(VPS_ENTRYEXIT, "Entering process_vfabric_fc_attr");
        
        /* Fill the vfabric structure */
        for (i = 0; i < num_cols; i++) {
                if (strcmp(cols[i], "wwnn") == 0)
                        memcpy(&(vfabric->fc_attr._wwnn), values[i], 8);
                else if (strcmp(cols[i], "wwpn") == 0)
                        memcpy(&(vfabric->fc_attr._wwpn), values[i], 8);
                else if (strcmp(cols[i], "fcid") == 0)
                        memcpy(vfabric->fc_attr._fcid, values[i], 3);
        }
        
        vps_trace(VPS_ENTRYEXIT, "Leaving process_vfabric_fc_attr");
        return 0; /* Callback must return 0 on success */
}


/* This function will populate the information of either en or fc attr
 * depending on the vfabric id.
 */
vps_error
populate_vfabric_protocol_information(vfm_vfabric_attr_t *vfabric)
{
        char query[512];
        void *stmt;
        memset(query, 0, sizeof(query));
        vps_error err = VPS_SUCCESS;
        if (vfabric->protocol == VFM_PROTOCOL_EN) {
                sprintf(query, "select * from vfm_vfabric_en_attr where vfabric_id = %d ;", vfabric->_vfabric_id);

                stmt = vfmdb_prepare_query(query, NULL, NULL);

                if (!stmt) {
                        vps_trace(VPS_ERROR," Cannot prepare sqlite3 statement");
                        err = VPS_DBERROR;
                        goto out;
                }
                /*call process_vfabric and get the data of vfabric from the database*/
                if (VPS_SUCCESS != (err = vfmdb_execute_query(stmt,
                                                process_vfabric_en_attr,
                                                vfabric))) {
                        vps_trace(VPS_ERROR, "Could not get vfabric protocols ");
                        err = VPS_DBERROR;
                        goto out;
                }
        }
        else if (vfabric->protocol == VFM_PROTOCOL_FC) {
                sprintf(query, "select * from vfm_vfabric_fc_attr where vfabric_id = %d ;", vfabric->_vfabric_id);

                stmt = vfmdb_prepare_query(query, NULL, NULL);

                if (!stmt) {
                        vps_trace(VPS_ERROR," Cannot prepare sqlite3 statement");
                        err = VPS_DBERROR;
                        goto out;
                }
                /*call process_vfabric and get the data of vfabric from the database*/
                if (VPS_SUCCESS != (err = vfmdb_execute_query(stmt,
                                                process_vfabric_fc_attr,
                                                vfabric))) {
                        vps_trace(VPS_ERROR, "Could not get vfabric protocols ");
                        err = VPS_DBERROR;
                        goto out;
                }
        }
out:
        return err;
}
/* 
 * Process_vfabric is called for each vfabric present in the database.
 * So in the Query we will get the count of the vadapters attached to the
 * vfabric.
 *  - the data or rsc->data will be a pointer to an array of pointers.
 *  - ALWAYS ** - Reallocate memory for pointers not the data - **
 *  -  
 *  - allocate the memory (sizeof(vfm_vfabric_attr_t) + 
 *                      sizeof(no_of_vadapters * sizeof(uint32_t)))
 *  - then copy the vfabric structure data
 *  - then fire the query to get the vadapter id based on the vfabric id.
 *  - assign that data to vfabric structure.
 *  - then fire the query depending in the protocol of vadapter(EN/FC)
 *    and get the relevent data from the database.
 *
 */

        int
process_vfabric(void *data, int num_cols, uint8_t **values, char **cols)
{
        res_packet *rsc = (res_packet*)data;
        res_packet vfm_rsc;
        vfm_vfabric_attr_t vfabric;
        uint8_t **offset;
        uint32_t i, size = 0;
        vps_error err = VPS_SUCCESS;
        vps_trace(VPS_ENTRYEXIT, "Entering process_vfabric");
        memset(&vfabric, 0, sizeof(vfm_vfabric_attr_t));

        /* Fill the vfabric structure */
        for (i = 0; i < num_cols; i++) {
                if (strcmp(cols[i], "primary_gw_id") == 0)
                        vfabric.primary_gateway = atoi(values[i]);
                else if (strcmp(cols[i], "id") == 0)
                        vfabric._vfabric_id = atoi(values[i]);
                else if (strcmp(cols[i], "backup_gw_id") == 0)
                        vfabric.backup_gateway = atoi(values[i]);
                else if (strcmp(cols[i], "name") == 0)
                        strcpy(vfabric.name,values[i]);
                else if (strcmp(cols[i], "desc") == 0)
                        strcpy(vfabric.desc,values[i]);
                else if (strcmp(cols[i], "ctx_table_id") == 0)
                        vfabric._ctx_table_id = atoi(values[i]);
                else if (strcmp(cols[i], "protocol") == 0)
                        vfabric.protocol = atoi(values[i]);
                else if (strcmp(cols[i], "auto_failover") == 0)
                        vfabric.auto_failover = atoi(values[i]);
                else if (strcmp(cols[i], "auto_failback") == 0)
                        vfabric.auto_failback = atoi(values[i]);
                else if (strcmp(cols[i], "vadapter_id") == 0)
                        vfabric._num_vadapter = atoi(values[i]);
        }


        /* Read the existing resrouce count for re-allocation */
        if (NULL == (rsc->data = realloc(rsc->data,(
                     sizeof(void *) * (rsc->count + 1))))) {
                /*
                 * The block could not be realloc'ed. This can cause serious
                 * problems .Hence we return with a db_error
                 */
                vps_trace(VPS_ERROR, "Could not realloc memory: %d",
                                rsc->count + 1);
                return 1; /* This will propogate with SQL_ABORT */
        }
        /* Go the the vfabric array offset */
        offset = (uint8_t **)rsc->data;
        /* Calculate the size of the data(vfabric structure + vadapter count)*/
        size = sizeof(vfm_vfabric_attr_t) + TLV_SIZE +
                (vfabric._num_vadapter * sizeof(vfm_vadapter_id_t));
        /* allocate the memory*/        
        vfm_rsc.data = malloc(size);
        /* Create the TLV of the allocated data*/
        *((uint32_t*)vfm_rsc.data) =  TLV_VFABRIC_ATTR;
        *((uint32_t*)(vfm_rsc.data + sizeof(uint32_t))) = (size - TLV_SIZE);
        memcpy(vfm_rsc.data + TLV_SIZE, &vfabric, sizeof(vfm_vfabric_attr_t));
        
        /* Call populate_vadapter_id to get the vadapter id */
        populate_vadapter_id(vfabric._vfabric_id, &vfm_rsc);
        
        if (vfabric.protocol == VFM_PROTOCOL_EN ||
                        vfabric.protocol == VFM_PROTOCOL_FC) {
               err = populate_vfabric_protocol_information((vfm_rsc.data +
                                                                   TLV_SIZE));
               if (err != VPS_SUCCESS) {
                       vps_trace(VPS_ERROR, 
                                  "Error in processing vfabric protocols");
                       return 1;
               }
        }
        else {
                vps_trace(VPS_ERROR, "Protocol is not set for vfabric id : %d",
                                vfabric._vfabric_id);
                return 1; /* This will propogate with SQL_ABORT */
        }

        offset[rsc->count] = vfm_rsc.data;
        /* After the data is correctly populated, increment the bridge count */
        rsc->count++;
        /* the resource size = (vfabric structure + vadapter count + TLV type 
           + length of TLV*/
        rsc->size += size; 
        vps_trace(VPS_INFO, "Vfabric successfully read");

        vps_trace(VPS_ENTRYEXIT, "Leaving process_vfabric");
        return 0; /* Callback must return 0 on success */
}

test_pack_data(res_packet *rsc)
{
       vfm_vfabric_attr_t attr, attr1;
       res_packet ip_res1, ip_res2;
       uint8_t **temp_data;
       uint8_t *offset;

       int size = sizeof(vfm_vfabric_attr_t) + (2 * sizeof(uint32_t));
       
       ip_res1.data = malloc(size);
       ip_res2.data = malloc(size);

       /* allocate the memory to rsc->data for 2 TLV*/
       rsc->data =malloc(2 * sizeof(void *));

       memset(&attr, 0, sizeof(vfm_vfabric_attr_t));
       memset(&attr1, 0, sizeof(vfm_vfabric_attr_t));

       attr._vfabric_id = 1;
       attr1._vfabric_id = 2;

       strcpy(attr.name, "Vfabric 1");
       strcpy(attr1.name, "Vfabric 2");

       strcpy(attr.desc, "Vfabric test");

       attr.protocol = 1;
       attr._num_vadapter = 2;
       attr1._num_vadapter = 1;

       attr._vadapter_id = malloc(2 * sizeof(uint32_t));
       attr1._vadapter_id = malloc(1 * sizeof(uint32_t));

       attr._vadapter_id[0] = 1;
       attr._vadapter_id[1] = 2;
       attr1._vadapter_id[0] = 1;

       attr.primary_gateway = 1000;
       offset = ip_res1.data;
       memcpy(offset, &attr, sizeof(vfm_vfabric_attr_t));
       offset += sizeof(vfm_vfabric_attr_t);
       memcpy(offset, attr._vadapter_id, 2 *sizeof(uint32_t));

       offset = ip_res2.data;
       memcpy(offset, &attr1, sizeof(vfm_vfabric_attr_t));
       offset += sizeof(vfm_vfabric_attr_t);
       memcpy(offset, attr1._vadapter_id, 2 *sizeof(uint32_t));

       temp_data = (uint8_t **)rsc->data;

       temp_data[0] = malloc(size);
       temp_data[1] = malloc(size);

       
       create_tlv_packet(TLV_VFABRIC_ATTR, size, ip_res1.data, temp_data[0]);
       create_tlv_packet(TLV_VFABRIC_ATTR, size, ip_res2.data, temp_data[1]);
       
       /*Fill up the resource data*/
       rsc->count = 2;
       rsc->size = 2 * (size + TLV_SIZE);

}


vps_error
populate_vfabric_information(vfm_vfabric_attr_t *attr,
                vfm_vfabric_attr_bitmask_t *bitmask, res_packet *rsc)
{

#ifdef VFM_PACK_TEST
        test_pack_data(rsc);
#else
        /*
         * prepare the query based on the bitmask of the vfabric structure
         * and the bitmasks of either EN of FC structure.
         */
        char query[1024] = "Select * from v_vfm_vfabric_attr_with_vadapter_count";
        void *stmt;
        int count = 0 , i = 0, params = 1;
        res_packet vf_rsc;
        vps_error err = VPS_SUCCESS;

        memset(&vf_rsc, 0, sizeof(res_packet));

        if (bitmask->vfm_vfabric_id) {
                add_query_parameters(query, count++, "`id`",
                                &(attr->_vfabric_id), Q_UINT32);
        }
        if (bitmask->name) {
                add_query_parameters(query, count++, "`name`",
                                attr->name, Q_UINT8);
        }
        if (bitmask->desc) {
                add_query_parameters(query, count++, "`desc`",
                                attr->desc, Q_UINT8);
        }
        if (bitmask->primary_gateway) {
                add_query_parameters(query, count++, "primary_gw_id",
                                &attr->primary_gateway, Q_UINT32);
        }
        if (bitmask->primary_gateway) {
                add_query_parameters(query, count++, "backup_gw_id",
                                &attr->backup_gateway, Q_UINT32);
        }
        if (bitmask->auto_failback) {
                add_query_parameters(query, count++, "auto_failback",
                                &attr->auto_failback, Q_UINT32);
        }
        if (bitmask->ctx_table_id) {
                add_query_parameters(query, count++, "ctx_table_id",
                                &attr->_ctx_table_id, Q_UINT32);
        }
        query[strlen(query)] = ';' ; 

        stmt = vfmdb_prepare_query(query, NULL, NULL);

        if (!stmt) {
                vps_trace(VPS_ERROR," Cannot prepare sqlite3 statement");
                err = VPS_DBERROR;
                goto out;
        }
        /*call process_vfabric and get the data of vfabric from the database*/
        if (VPS_SUCCESS != (err = vfmdb_execute_query(stmt,
                                        process_vfabric,
                                        &vf_rsc))) {
                vps_trace(VPS_ERROR, "Could not get EN properties ");
                err = VPS_DBERROR;
                goto out;
        }

        rsc->count = vf_rsc.count;
        rsc->size = vf_rsc.size;
        rsc->data = vf_rsc.data;
        /*
         * Then depending on the protocol of the vfabric get the EN/FC 
         * attributes from the database i.e either from vfm_vadapter_en_attr
         * table or vfm_vadapter_fc_attr table
         */
out:
        return err;
#endif         
}


