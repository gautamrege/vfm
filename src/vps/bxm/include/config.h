/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */


#ifndef __VPS_CONFIG_H__
#define __VPS_CONFIG_H__

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* BXM listen Protocol type */
#define BXM_EN_PROTOCOL 1       /* EN protoclol */
#define BXM_IB_PROTOCOL 2       /* IB protoclol */
#define BXM_FC_PROTOCOL 3       /* FC protoclol */

#define LOG_LEVEL "log_level"

#define MGMT_MODE_LOCAL "mgmt_mode_local"
#define MGMT_MODE_REMOTE "mgmt_mode_remote"


#define INTERNAL_PROTOCOL "internal_protocol"

#define INTERNAL_PROTOCOL_IB "IB"
#define INTERNAL_PROTOCOL_EN "EN"
#define INTERNAL_PROTOCOL_FC "FC"

#define GW_INTERNAL_MAC "gw_internal_mac"
#define GW_MAC "gw_mac"

#define VFM_ETH_INTF "vfm_eth_intf"

#define WWPN "wwpn"
#define WWNN "wwnn"

#define FCOE_T11 "fcoe_t11"


/* Print Configutation and Validate.*/
void
print_and_validate_config();

/* Process line and  extract values*/
void
process_config_line(char *type, char *value);


/* Print Address(Mac ,WWPN, WWNN)*/
void
print_address(const char *name, uint8_t *addr, uint8_t length);

/*
 * Parse Config File.
 */
int
read_config();

/*
 * String trim
 */
void
str_trim(char *str);

/*
 * Parse input buffer to address according to address length.
 * Mac or WWPN or WWNN address.
 *
 * [IN] input_type : Name of address holder.
 * [IN] *buff      : Input buufer read from the file (Only one line).\
 * [IN] length     : Length of address.(Mac = 6, WWPN and WWNN length=8).
 * [OUT]*out_buff  : Output address buffer.
 */
void
parse_address(const char *input_type, uint8_t *buff, int length, 
                uint8_t *out_buff);




#endif /* __VPS_CONFIG_H__*/
