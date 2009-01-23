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
           outf.write('\nId :%s\n\tName :%s\n\tDescription :%s\n\tNumber-gateway-module :%s\n\tFirmware-Version :%s\n\tbxm-guid :%s\n\t'   % (bridge['GUID'], bridge['NAME'],
                             bridge['DESCRIPTION'], bridge['NUM_GW_MODULE'],bridge['FIRMWARE_VERSION'], bridge['BXM_GUID']))
                    

_output_handlers = {
                        "BridgeSpecList"  : _output_bridge_list_verbose
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
	output.completeOutputError(lib.errorhandler.InvalidArgumentCount(syntax = show__doc__))	
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
    if bridge_id == "ALL":
	query = "Select * from %s " % (_VIEW_BRIDGE)
    elif bridge_id != "ALL":
       query = "Select * from %s where guid = %s " % (_VIEW_BRIDGE, bridge_id)
    #print query
    database = lib.db.db.Database()
    cursor = database._execute(query)
    for row in cursor:
         _bridge_spec(output, row[0], row[1], row[2], row[3],  
                      row[4], row[5])
    return


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

         
