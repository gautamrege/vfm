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

_VIEW_IOMODULE = "bxm_io_module_attr"

def _output_iomodule_list_verbose(outf, name, iomodule_list):
    """ Output Format"""
    if iomodule_list:

	for (n , iomodule) in iomodule_list:
	      id = iomodule['ID'] 
              name = iomodule['NAME']
              type = iomodule['TYPE']
  	      mac = iomodule['MAC']
	      guid = iomodule['GUID']
  	      num_vhba = iomodule['NUM_VHBA']
	      num_vnic = iomodule['NUM_VNIC']
  	      slot = iomodule['SLOT']
	      port = iomodule['PORT']
	      support = iomodule['SUPPORT_SPEED']
  
              outf.write('\nId:%s\n\tName:%s\n\tType:%s\n\tMAC:%s\n\tGUID:%s\n\tNum_vhba:%s\n\tNum_vnic:%s\n\tslot:%s\n\tport:%s\n\tsupport_speed:%s\n\t' % (id, name, type, mac, guid, num_vhba, num_vnic, slot, port, support))


_output_handlers = {
                        "IomoduleSpecList"  : _output_iomodule_list_verbose
                   }


#
# Possible invocations:
#        show iomodule <iomodule-id>
# Note that the 'show' keyword is *not* included in argv
#
def show(argv):
    """
        Show io-module-related information
        syntax: show iomodule <iomodule-id>
		show iomodule

    """
    output = lib.output.CLIoutput("iomodule", _output_handlers)
    
    if len(argv) > 2:
	print show.__doc__
	return output

    if len(argv) == 2:
       _show_imodule_module(output, argv, argv[1])
       return output

    if len(argv) == 1:
       _show_iomodule(output, argv)
       return output
    
    else:
        print show.__doc__
	return output
	
    return output

def _show_imodule_module(output, argv, io_module):
    output.beginList("IomoduleSpecList")
    _get_iomodule_values(output, "iomodule", io_module)
    output.endList("IomoduleSpecList")
    output.completeOutputSuccess()


def _show_iomodule(output, argv):
    """
       Show iomodule
           syntax : show iomodule
    """
    output.beginList("IomoduleSpecList")
    _get_iomodule_values(output, "iomodule")
    output.endList("IomoduleSpecList")
    output.completeOutputSuccess()
		
def _get_iomodule_values(output, mode, io_module_id = "All"):
    if io_module_id == "All":
	query = "Select * from %s " % (_VIEW_IOMODULE)
    elif io_module_id != "All":
        query = "Select * from %s where id = %s " % (_VIEW_IOMODULE, io_module_id)
    #print query
    database = lib.db.db.Database()
    cursor = database._execute(query)
    for row in cursor:
           iomodule_spec(output, row[0], row[1], row[2], row[3],row[4],row[5],row[6],row[7],row[8],
                        row[9])
    return output


def iomodule_spec(output, id , name , type, mac, guid, num_vhba, num_vnic, slot, port,support_speed):
    output.beginAssembling("IomoduleListAll")
    output.setVirtualNameValue("ID", id)
    output.setVirtualNameValue("NAME", name)
    output.setVirtualNameValue("TYPE", type)
    output.setVirtualNameValue("MAC", mac)
    output.setVirtualNameValue("GUID", guid)
    output.setVirtualNameValue("NUM_VHBA", num_vhba)
    output.setVirtualNameValue("NUM_VNIC", num_vnic)
    output.setVirtualNameValue("SLOT", slot)
    output.setVirtualNameValue("PORT", port)
    output.setVirtualNameValue("SUPPORT_SPEED", support_speed)
    output.endAssembling("IomoduleListAll")  
    
