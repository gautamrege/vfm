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

def output_gateway_list_verbose(outf, name, gateway_list):
    if gateway_list:
       FMT1 = "%-5s\t%-5s\t%-12s\t%-10s\n"
       FMT2 = "%s\t%s\t%-2s\t%-5s\t%s\n"
       FMT3 = "%-10s\t%-5s\t%-5s\n"

       HEADER1 = ("GW_ID", "GW_MODULE", "BXM_GUID", "NAME")
       HEADER2 = ("DESC", "INT_PORT", "EXT_PORT", "INGRESS_LEARNING", "INGRESS_SECURE")
       HEADER3 = ("EGRESS_SECURE", "FLOOD", "CHECKSUM_OFFLOAD")

       outf.write(FMT1 % HEADER1)
       outf.write(FMT2 % HEADER2)
       outf.write(FMT3 % HEADER3)

       outf.write("%s\n" % (70 * '-'))
       outf.write("%s\n" % (70 * '-'))
       outf.write("%s\n" % (70 * '-'))
       for (n, gateway) in gateway_list:
              outf.write()             

_output_handlers = {
                        "GatewaySpecList"  : _output_gateway_list_verbose
                   }


#
# Possible invocations:
#        show gateway <gateway-id>
# Note that the 'show' keyword is *not* included in argv
#
def show(argv):
    """
        Show Bridge-related information
        syntax: show gateway <gateway-id>

    """
    output = lib.output.CLIoutput("gateway")
    #_show_gateway(output, argv)
    return output


def _show_gateway(output, argv):
    """
       Show gateway
           syntax : show gateway
    """
    output.beginList("GatewaySpecList")
    _get_gateway_from_db(output,"gateway")
    output.endList("GatewaySpecList")
    output.completeOutputSuccess()


def _get_gateway_from_db(output, mode):
    """
       Query to get data from gateway.
    """
    print 'Send query'



def _gateway_spec(output, gw_id, gw_module, bxm_guid, name, 
                    desc, int_port, ext_port,ingress_learning,
                    ingress_secure, egress_secure, flood, checksum_offload):
    """Output the gateway spec"""
    output.beginAssembling("GatewayList")
    output.setVirtualNameValue("GW_ID", gw_id)
    output.setVirtualNameValue("GW_MODULE", gw_module)
    output.setVirtualNameValue("BXM_GUID", bxm_guid)
    output.setVirtualNameValue("NAME", name)
    output.setVirtualNameValue("DESC", desc)
    output.setVirtualNameValue("INT_PORT",int_port)
    output.setVirtualNameValue("EXT_PORT", ext_port)
    output.setVirtualNameValue("INGRESS_LEARNING", ingress_learning)
    output.setVirtualNameValue("INGRESS_SECURE", ingress_secure)
    output.setVirtualNameValue("EGRESS_SECURE",egress_secure)
    output.setVirtualNameValue("FLOOD", flood)
    output.setVirtualNameValue("CHECKSUM_OFFLOAD", checksum_offload)
    output.endAssembling("GatewayList")

#
# argv[0] == 'edit_gateway'
# argv[1] == '<object>'
# argv[2] depends on argv[1]
#
def edit(argv):
    """
      syntax: [edit] gateway <object> 
    """
    output = lib.output.CLIoutput("gateway")
    lib.essentials.dispatch_action(output, argv,
                             (("gateway", _edit_gateway)),
                             edit.__doc__)
    return output


def _edit_gateway(output, argv):
    """
      Edit gateway related information
          syntax [edit] gateway <gateway-id>
    """

