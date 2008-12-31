/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 * 
 * Header File : Resource Discovery Definations 
 */

#ifndef __RSC_DISCOVERY_H__
#define __RSC_DISCOVERY_H__


#include <common.h>
#include <vfm_fip.h>
#include <db_access.h>


/* IO Module Type */
#define EN_IO_MODULE 0x1
#define IB_IO_MODULE 0x2

/*
 *  Add EN IO module attribute to DB.(DB Table : bxm_io_module_attr
 *
 *  [IN] *io_module : IO Module Attribute come in discovery.
 */
vps_error
add_en_io_module_to_db(fcoe_conx_vfm_adv *io_module);


#endif /*__RSC_DISCOVERY_H__ */
