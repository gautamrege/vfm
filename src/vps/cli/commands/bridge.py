#!/usr/bin/python
#
# Copyright (c) 2008  VirtualPlane, Inc.

"""Configure IP addresses and routes"""
import getopt
import sys
import re

path = 'src/vps/cli'
if path not in sys.path:
     sys.path.append(path)

sys.path.append("/home/rohit/pluto/src/vps/vfm/vfmapi/pyapi/build/lib.linux-x86_64-2.4")
sys.path.append("/home/rohit/pluto/src/vps/vfm/vfmapi/pyapi")

import vfm
import lib.output
import lib.process
import lib.essentials

from lib.errorhandler import *

#
# Globals
#

_VIEW_BRIDGE = "v_bxm_bridge_attr"

def _output_bridge_list_verbose(outf, name, bridge_list):
    if bridge_list:
        for (n, bridge) in bridge_list:
           outf.write('\n\tGateway Module Index :%s\n\tNumber Gateway module :%s\n\tDescription :%s\n\tvfm guid :%s\n\tFirmware Version :%s\n\tState :%s\n\tRunning Mode:%s\n\tLast Keep Alive:%s\n\t'%(bridge['gw-Module-Index'], bridge['number-gw-module'], bridge['desc'], bridge['vfm-guid'], bridge['firmware-version'], bridge['state'],bridge['running-mode'],bridge['last-keep-alive']))

# Change as described in Bug #57.                    
#           outf.write('\nGateway Module Index :%s\n\tNumber Gateway module :%s\n\tDescription :%s\n\tvfm guid :%s\n\tFirmware Version :%s\n\tState :%s\n\tRunning Mode:%s\n\tLast Keep Alive:%s\n\t'%(bridge['gw-Module-Index'], bridge['number-gw-module'],bridge['vfm-guid'],bridge['desc'], bridge['firmware-version'], bridge['state'],bridge['running-mode'],bridge['last-keep-alive']))
                    


def _output_bridge_detail_list(outf, name , bridge_list):
    print "_output_bridge_detail_list"
 

def _output_bridge_select_list(outf, name , bridge_list):
    print "_output_bridge_select_list"

_output_handlers = {
                        "BridgeSpecList"  : _output_bridge_list_verbose,
			"BridgeDetailList": _output_bridge_detail_list,
			"BridgeSelectList": _output_bridge_select_list
		   }

#
# Possible invocations:
#        show bridge <bridge-id>
# Note that the 'show' keyword is *not* included in argv
#
def show(argv):
    """
        Show Bridge-related information
        syntax: show bridge <bridge-id>

    """
    output = lib.output.CLIoutput("bridge", _output_handlers)
  
    if len(argv) > 2:
	print show.__doc__
        return output

    if len(argv) == 2:
       _show_bridge(output, argv, argv[1])
    elif len(argv) == 1:
       _show_bridge(output, argv, "ALL")
    return output


def _show_bridge(output, argv, bridge_id):
    """
       Show bridge
           syntax : show bridge
    """
    output.beginList("BridgeSpecList")
    _get_bridge_from_db(output, "bridge", bridge_id)
    output.endList("BridgeSpecList")
    output.completeOutputSuccess()


def _get_bridge_from_db(output, mode, bridge_id):
    # Get all the attributes from the database ad 
    #if bridge_id == "ALL":
    #elif bridge_id != "ALL":
    input = {}
    try:
       bridge_info = vfm.py_vfm_bd_select_inventory(input)
       #print bridge_info
    except e:
       print e

    for row in bridge_info:
          #print row, "\n"
          each_row = bridge_info[row]      
          _bridge_select(output, each_row['desc'], each_row['state'],each_row['running_mode'],each_row['firmware_version'],each_row['vfm_guid'],each_row['num_gw_modules'],each_row['gw_module_index'],each_row['last_keep_alive']) 
    return output


#
# "Show bridge" output 
# Calling : py_vfm_bd_select_inventory
# @ Name : <Name of the bridge device>
# @ Desc : <Description of the device>
# @ State: <State>
# @ Running Mode : <running_mode>
# @ Firmware Module : <firmware_version>
# @ VFM-guid : <vfm_guid>
# @ Number-gw-modules : <Number of gateway modules>
# @ Gateway-module-index : <number of gateway module>
# @ Last Keep Alive : <last keepalive from BridgeX device>
#
def _bridge_select(output, desc, state, running_mode , 
		  firmware_module , vfm_guid, no_gw_module, 
		  gw_module_index, last_keep_alive):
    """Output of the show bridge """
    output.beginAssembling("Bridge_Device")
    output.setVirtualNameValue("desc", desc)
    output.setVirtualNameValue("state",state)
    output.setVirtualNameValue("running-mode", running_mode)
    output.setVirtualNameValue("firmware-version",firmware_module)
    output.setVirtualNameValue("vfm-guid",vfm_guid)
    output.setVirtualNameValue("number-gw-module",no_gw_module)
    output.setVirtualNameValue("gw-Module-Index",gw_module_index)
    output.setVirtualNameValue("last-keep-alive",last_keep_alive)
    output.endAssembling("Bridge_Device")

#
# "Show bridge <bridge-id>"
# Calling :py_vfm_bd_query_general_attr
# Calling : py_vfm_gw_module_attr_t
# Calling : py_vfm_bd_query_port_attr
#
# @ Name : <Name of the bridge device>
# @ Desc : <Description of the device>
# @ State: <State>
# @ Running Mode : <running_mode>
# @ Firmware Module : <firmware_version>
# @ VFM-guid : <vfm_guid>
# @ Number-gw-modules : <Number of gateway modules>
# @ Gateway-module-index : <number of gateway module>
# @     Gateway-module-id : <gateway module id>
# @     Internal Protocol : <internal protocol> 
# @     External Protocol : <external protocol>
# @     Number Internal Protocol : <number of internal ports>
# @     Number External Protocol : <number of external ports>
# @     Internal Ports : <Dictionary of the internal port index>
# @     External Ports : <Dictionary of the external port index>
# @     Gateway Ids : <Dictionary of the gateway ids>
# @ Port Id:
# @	Port Id: <port id>
# @     Physical Index : <physical_index>
# @     Type : <type>
# @     Protocol : <protocol>
# @     State : <port state>
# @       mac : <mac address>
# @       wwnn : <wwnn>
# @       wwpn : <wwpn>
# @       fcid : <fcid
# @       ib_guid : <guid>
# @       lid  : <lid>
# @       qpn : <qpn>
# @       sl : <sl>
# @ supported_speed : <Supported speed of the port>
# @ actual_speed : <Actual speed of the port>
# @ rx_stat : <Incomming traffic stats on the port>
# @ tx_stat : <Incomming traffic stats on the port> 
# @ Last Keep Alive : <last keepalive from BridgeX device>
# @
def _bridge_detail(output, name , desc, running_mode,
		  firmware_module , vfm_guid, no_gw_modules,
		  gw_module_index, gw_module_id,
		  int_protocol, ext_protocol, no_int_protocol,
		  no_ext_protocol, int_ports, ext_ports, gw_ids,
		  port_id, phy_index, type, protocol, state, mac,
		  wwnn, wwpn, fcid, ib_guid, lid, qpn, sl, support_speed,
		  actual_speed, rx_stat, tx_stat, last_keep_alive):
    output.beginAssembling("Bridge_Device")
    output.setVirtualNameValue("name", name)
    output.setVirtualNameValue("desc", desc)
    output.setVirtualNameValue("state",state)
    output.setVirtualNameValue("running-mode", running_mode)
    output.setVirtualNameValue("firmware-module",firmware_module)
    output.setVirtualNameValue("vfm-guid",vfm_guid)
    output.setVirtualNameValue("number-gw-module",no_gw_module)
    output.setVirtualNameValue("gw-module-index",gw_module_index)
    output.setVirtualNameValue("gw-module-id",gw_module_id)
    output.setVirtualNameValue("int_protocol", int_protocol)
    output.setVirtualNameValue("ext_protocol",ext_protocol)
    output.setVirtualNameValue("no_int_protocols",no_int_protocol)
    output.setVirtualNameValue("no_ext_protocols",no_ext_protocol)
    output.setVirtualNameValue("int-ports",int_ports)
    output.setVirtualNameValue("ext-ports",ext_ports)
    output.setVirtualNameValue("gw-ids",gw_ids)
    output.setVirtualNameValue("port-id",port_id)
    output.setVirtualNameValue("phy-index",phy_index)
    output.setVirtualNameValue("type",type)
    output.setVirtualNameValue("protocol",protocol)
    output.setVirtualNameValue("state",state)
    output.setVirtualNameValue("mac",mac)
    output.setVirtualNameValue("wwnn",wwnn)
    output.setVirtualNameValue("wwpn",wwpn)
    output.setVirtualNameValue("fcid",fcid)
    output.setVirtualNameValue("ib_guid",ib_guid)
    output.setVirtualNameValue("lid",lid)
    output.setVirtualNameValue("qpn",qpn)
    output.setVirtualNameValue("sl",sl)
    output.setVirtualNameValue("support-speed",support_speed)
    output.setVirtualNameValue("actual-speed",actual_speed)
    output.setVirtualNameValue("rx-stat",rx_stat)
    output.setVirtualNameValue("tx-stat",tx_stat)
    output.setVirtualNameValue("last-keep-alive",last_keep_alive)
    output.endAssembling("Bridge_Device")





def _bridge_spec(output, guid, name, desc, num_gateway_module, firmware_version,
                   bxm_guid):
    """Outout the bridge spec """
    output.beginAssembling("Bridge_Device")
    output.setVirtualNameValue("GUID", guid)
    output.setVirtualNameValue("NAME",name)
    output.setVirtualNameValue("DESCRIPTION", desc)
    output.setVirtualNameValue("NUM_GW_MODULE", num_gateway_module)
    output.setVirtualNameValue("FIRMWARE_VERSION",firmware_version)
    output.setVirtualNameValue("BXM_GUID",bxm_guid)
    output.endAssembling("Bridge_Device")

#
# argv[0] == 'edit_bridge'
# argv[1] == '<object>'
# argv[2] depends on argv[1]
#
def edit(argv):
    """
      syntax: [edit] bridge <object> 
    """
    output = lib.output.CLIoutput("bridge")
    _edit_bridge(output, argv)
    return output


def _edit_bridge(output, argv):
    """
      Edit bridge related information
          syntax [edit] bridge <bridge-id>
    """

def no(argv):
    """
        syntax: [no] bridge <argument>
    """
    output = lib.output.CLIoutput("bridge")
    _no_bridge(output, argv)
    return output


def _no_bridge(output, argv):
    """
      No bridge related information
          syntax [no] bridge <bridge-id> <argument>
    """

         
