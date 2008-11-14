/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 *
 * Header File: Data Structures for Map.
 */
#ifndef __VFM_MAP_UTIL_H_
#define __VFM_MAP_UTIL_H_

#include <common.h>

#define REQ_ENTRY_MAP_LEN 1024

typedef struct _req_entry_map {
  uint16_t oxid;
  uint8_t  mac[MAC_ADDR_LEN];
  uint16_t ctrl_flags;
  uint8_t  vhba_mac[MAC_ADDR_LEN];
  /* Flag to check for FLOGI , flag =1 or FDISC flag = 0 */
  uint32_t flag;
}req_entry_map;

/*
 * Add Entry to map
 *
 * [IN] oxid           : Originator Exchange ID.
 * [IN] mac                : Mac aadress of requester.
 *
 * Return :vfm_gen_oxid VFM generated OXID.or if 0 then no empty index.
 */
uint16_t
add_entry_to_map(req_entry_map *entry);

/*
 * Get Entry from map
 *
 * [IN] vfm_gen_oxid : VFM generated  OXID.
 * [OUT] oxid                : Original oxid of requester.
 * [OUT] mac                 : Requester mac address.
 *
 * Return : err
 *                  No entry for given oxid.
 */
req_entry_map*
get_entry_from_map(uint16_t vfm_gen_oxid);

/*
 * Remove  Entry from map
 *
 * [IN] vfm_gen_oxid : VFM generated  OXID.
 *
 */
void
remove_entry_from_map(uint16_t vfm_gen_oxid);


#endif  /* End of __VFM_MAP_UTIL_H_ */
