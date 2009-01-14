#ifndef INCLUDED_vfm_bridge_devide
#define INCLUDED_vfm_bridge_devide

/*
 * Populate the brige information all get the gateway module information
 * for each bridge device.
 */

vps_error
populate_bridge_information(vfm_bd_attr_t attr, vfm_bd_attr_bitmask_t
                bitmask, vpsdb_resource *rsc);

#endif /* INCLUDED_vfm_bridge_device */
