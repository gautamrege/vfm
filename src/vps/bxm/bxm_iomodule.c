/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */ 

#include <rsc_discovery.h>

/*
 *  Add EN IO module attribute to DB.(DB Table : bxm_io_module_attr
 *
 *  [IN] *io_module : IO Module Attribute come in discovery.
 */
vps_error
add_en_io_module_to_db(fcoe_conx_vfm_adv *io_module)
{
          vps_error err = VPS_SUCCESS;
           
          /* VPS DB IO Module */
          vpsdb_io_module_t db_io_module;
          vpsdb_resource  db_res;

          vps_trace(VPS_ENTRYEXIT, "Entering  add_en_io_module_to_db");

          memset(&db_io_module, 0, sizeof(vpsdb_io_module_t));

          /* Copy IO Node name */
          memcpy(db_io_module.name, io_module->node_name,
                          sizeof(db_io_module.name));

          /* Copy IO Module Mac */
          memcpy(db_io_module.mac, io_module->host_mac,
                          sizeof(db_io_module.mac));

          /* Set IO type */
          db_io_module.type = EN_IO_MODULE;
         
          /* Set Number of vhba connected to IO Adapter */
          db_io_module.num_vhba = 0;

          db_res.type = VPS_DB_IO_MODULE;
          db_res.count = 1;
          db_res.data = &db_io_module;
          

          vps_trace(VPS_INFO, "MAC %X:%X:%X:%X:%X:%X",db_io_module.mac[0],
                    db_io_module.mac[1],db_io_module.mac[2],
                    db_io_module.mac[3],db_io_module.mac[4],
                    db_io_module.mac[5]);


          vps_trace(VPS_INFO, "Node Name %X:%X:%X:%X:%X:%X:%X:%X",
                    db_io_module.name[0],
                    db_io_module.name[1],db_io_module.name[2],
                    db_io_module.name[3],db_io_module.name[4],
                    db_io_module.name[5],db_io_module.name[6],
                    db_io_module.name[7]);

          /* Insert Into Database */
          if ((err = vpsdb_add_resource(VPS_DB_IO_MODULE, &db_res)) != 
                                                            VPS_SUCCESS ) {
                  vps_trace(VPS_ERROR, "DB Insert Error: %d ", err);
          }

          vps_trace(VPS_ENTRYEXIT, "Leaving add_en_io_module_to_db");

          return err;
}
