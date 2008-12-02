#!/usr/bin/python
#
# Copyright (c) 2008  VirtualPlane, Inc.

"""Configure IP addresses and routes"""
import getopt
import sys
import re
import string

path = 'src/vps/cli'
if path not in sys.path:
     sys.path.append(path)

import lib.output
import lib.process
import lib.essentials
import lib.db.db

from lib.errorhandler import *

#
# Globals
#


_LIMITED_SHOW = False
_DETAIL_SHOW = False 
_VIEW_VADAPTER = "v_bxm_vadapter_attr"
_VADPTER_ = "bxm_vadapter_attr"
_VADAPTER_EN = "bxm_vadapter_en_attr"
_VADAPTER_FC = "bxm_vadapter_fc_attr"

def _output_vadapter_list_verbose(outf, name, vadapter_list):
    """ OUTPUT FORMAT IN ROW"""
    if vadapter_list:
         FMT = "%s\n%-10s\n%-20s\n%-5s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n"
         HEADER = ("id","name","io_module","vfabric","protocol","init_type","desc","status","mac"
                     ,"prmiscuous","silent","vlan","wwnn","wwpn","fc_id","spma","fmpa") 
	
         for (n , vadapter) in vadapter_list:
                name = vadapter['NAME']
                id   = vadapter['ID']
                protocol = vadapter['PROTOCOL']
                vfabric = vadapter['VFABRIC']
                io_module = vadapter['IO_MODULE_ID']
                desc  = vadapter['DESC']
                status = vadapter['STATUS']
                init_type = vadapter['INIT_TYPE']
                mac = vadapter['MAC']
                promiscuous = vadapter['PROMISCUOUS']
                silent = vadapter['SILENT']
                vlan = vadapter['VLAN']
                wwnn = vadapter['WWNN']
                wwpn = vadapter['WWPN']
                spma = vadapter['SPMA']
                fmpa = vadapter['FMPA']
                fc_id = vadapter['FC_ID']
		
                if _LIMITED_SHOW:
                     outf.write('General Attr:\nId : %s\n\tName: %s\n\tIO_Module : %s\n\tVfabric : %s\n\tProtocol : %s\n\tInitization_Type : %s\n\tDescription : %s\n\tStatus: %-20s\n\n' % (id, name, io_module, vfabric, protocol, init_type, desc, status))     
                
		elif _DETAIL_SHOW: 
                     if protocol.lower() == 'fc':
                              outf.write('General Attr:\nId : %s\nName: %s\nIO_Module : %s\nVfabric : %s\nProtocol : %s\nInitization_Type : %s\nDescription : %s\nStatus: %-20s\nFC_Attr :\n\tWWNN : %s\n\tWWNP : %s\n\tFC_ID : %s\n\tSPMA : %s\n\tFMPA : %s\n\n' % (id, name, io_module, vfabric, protocol, init_type, desc, status,wwnn, wwpn,fc_id,spma,fmpa)) 
               
                     elif protocol.lower() == 'en':
			   outf.write('General Attr:\nId : %s\nName: %s\nIO_Module : %s\nVfabric : %s\nProtocol : %s\nInitization_Type : %s\nDescription : %s\nStatus: %-20s\nEN_Attr:\n\tMAC : %s\n\tVLAN: %s\n\tPromiscuous: %s\n\tSilent Listener: %s\n\n' % (id , name, io_module,vfabric, protocol, init_type, desc, status,mac,vlan, promiscuous, silent))



_output_handlers = {
                        "VadapterSpecList"  : _output_vadapter_list_verbose 
                   }

#
# Possible invocations:
#        show vadapter <vadapter-id>
# Note that the 'show' keyword is *not* included in argv
#
def show(argv):
    """
        Show Vadapter-related information
        syntax:	show vadapter 
		show vadapter --detail
                show vadapter <vadapter-id>
           
    """
    output = lib.output.CLIoutput("vadapter", _output_handlers)
    # Done to mke changes if needed later with more refining.
	
    global _DETAIL_SHOW
    global _LIMITED_SHOW

    if len(argv) > 3:
	print show.__doc__
	return output

    if len(argv) == 2:
        _DETAIL_SHOW = True
	_LIMITED_SHOW = False
	if argv[1] == "--detail":
           _show_vadapter(output, argv)
        elif argv[1] == "?" or argv[1] == "help":
	   print show.__doc__
	elif "-" not in argv[1]:
	   _show_vadapter_module(output, argv, argv[1])
	else:
	   print show.__doc__
        return output

    elif len(argv) == 1:
	_DETAIL_SHOW = False
	_LIMITED_SHOW = True
        _show_vadapter(output, argv)
        return output
    else:
       print show.__doc__
       return output
    return output


def _show_vadapter_module(output, argv, vadapter_id):
    """
       Show vadapter <vadapter-id>
           syntax : show vadapter <vadapter-id>
    """
    output.beginList("VadapterSpecList")
    _get_vadapter_values(output, "vadapter", vadapter_id)    
    output.endList("VadapterSpecList")
    output.completeOutputSuccess()

def _show_vadapter(output, argv):
    """
       Show vadapter
           syntax : show vadapter
    """
    output.beginList("VadapterSpecList")
    _get_vadapter_values(output, "vadapter")    
    output.endList("VadapterSpecList")
    output.completeOutputSuccess()
    return output

def _get_vadapter_values(output, mode, vadapter_id = "All"):
    """ Call database to fecth values"""
    if vadapter_id == "All":
         query = "Select * from %s " % (_VIEW_VADAPTER)
    elif vadapter_id != "All":
         query = "Select * from %s where id = %s " % (_VIEW_VADAPTER, vadapter_id)
    #print query
    database = lib.db.db.Database()
    cursor = database._execute(query)
    if cursor == "Null":
        return output
    #if not cursor.next():
    #    print 'Error'
    if cursor.rowcount == 0 :
        print "\tResponse : No Record Found"
    for row in cursor:
            _vadapter_spec(output, row[0], row[1], row[2], row[3],row[4],row[5],row[6],row[7],row[8],
			row[9],row[10],row[11],row[12],row[13],row[14],row[15], row[16]) 
    return output 
     
# Id and status missing from the databse.

def _vadapter_spec(output, id, name, desc, init_type, protocol, status, io_module_id,vfabric,
		    wwnn, wwpn, fc_id, spma, fmpa, mac, promiscuous, silent_listener,vlan):
    """ Display for the show vadapter """
    output.beginAssembling("VadapterListAll")
    output.setVirtualNameValue("ID", id)
    output.setVirtualNameValue("NAME", name)
    output.setVirtualNameValue("IO_MODULE_ID", io_module_id)
    output.setVirtualNameValue("VFABRIC", vfabric)
    output.setVirtualNameValue("PROTOCOL", protocol)
    output.setVirtualNameValue("INIT_TYPE", init_type)
    output.setVirtualNameValue("DESC", desc)
    output.setVirtualNameValue("STATUS", status)
    output.setVirtualNameValue("MAC", mac)
    output.setVirtualNameValue("PROMISCUOUS", promiscuous)
    output.setVirtualNameValue("SILENT", silent_listener)
    output.setVirtualNameValue("VLAN", vlan)
    output.setVirtualNameValue("WWNN", wwnn)
    output.setVirtualNameValue("WWPN", wwpn)
    output.setVirtualNameValue("FC_ID",fc_id)
    output.setVirtualNameValue("SPMA", spma)
    output.setVirtualNameValue("FMPA", fmpa)
    output.endAssembling("VadapterListAll") 

def _parse_edit_or_add_argv(output, argv, valid_args, syntax = None):
    arg_dict = {}
    index = 0
    
    #print syntax

    if len(argv[0].split()) > 1:
          lib.errorhandler.InvalidArgumentCount(3, "vadapter-name", syntax,
                                                descape = "Please specify the vadapter name")
          return
        
    # argv 1 should be an vadapter name.
    arg_dict['name'] = argv[1]
    index = 2 
    len_args = len(argv)
  
    #print arg_dict
    while index < len_args:
         args = argv[index]    
         # Next argument should be from the valid_list
         if _check_valid_command_argument(valid_args, args) != 0:
             # Raise an Error
             lib.errorhandler.InvalidArgumentCount(index+3, args, syntax,
                                       descape = ("'%s' Not a valid Keyword" % args))
             return
         index += 1
         if arg_dict.has_key(args):
             msg = " '%s' already specified." % (args,)
             lib.errorhandler.InvalidArgumentCount(index+2, args, syntax,
                                             descape = msg)
             return

         if (index >= len_args):
               if _check_valid_command_argument(valid_args, argv[index-1]) != 0:
                    msg1 = "'%s' Not a valid Keyword" % args
                    lib.errorhandler.InvalidArgumentCount(index+2, args, 
                          syntax,descape = msg1)
                    return
               msg = " '%s' doesn't have the value defined" % (args,)
               lib.errorhandler.InvalidArgumentCount(index+2, args,
                               syntax,descape = msg)
               return

         if _check_valid_command_argument(valid_args, argv[index]) == 0:
             msg = " '%s' is not an expected value of '%s'" % (argv[index], args)
             lib.errorhandler.InvalidArgumentCount(index+3, argv[index], 
                                      syntax,descape = msg)
             return
          
         arg_dict[args] = argv[index]
         index += 1
    #print arg_dict

    if arg_dict.has_key('running_mode'):
         if _check_status_value(arg_dict, 'running_mode') != 0:
              msg = " Attribute 'running_mode' has [ONLINE | OFFLINE] as valid values"
              lib.errorhandler.InvalidArgumentCount(syntax,
                                           descape = msg)
              return

    if arg_dict.has_key('init_type'):
        # Valid arguments are [ HOST | NETWORK]
        if _check_initialization_value(arg_dict, 'init_type') != 0:
              msg = "Attribute 'init_type' has [HOST | NETWORK] as valid inputs"
              lib.errorhandler.InvalidArgumentCount(syntax, descape = msg)

    if arg_dict.has_key('protocol'):
         # Then it will call _add_en or add_fc respectively
          _add_vadapter_protcol(output, argv, arg_dict, syntax)
    else:
         # Add the values to the database.
         _add_vadapter_database(output, argv, arg_dict, syntax)
         
#
# argv[0] == 'edit_vadapter'
# argv[1] == '<object>'
# argv[2] depends on argv[1]
#
def edit(argv):
    """
       [edit] vadapter <vadapter-name> properties [<general-attrs>]
       protocol [EN <en-attrs>] | [FC <fc-attrs>]

       general-attrs: [vfabric <vfabric-id>]
                      [io_module_id <io-module-id>]
                      [status [ONLINE | OFFLINE]]
                      [init_type [ HOST | NETWORK]]

       en-attrs:      [mac <mac-addr>]
                      [promiscuous [TRUE | FALSE]]
                      [silent_listener [TRUE | FALSE]]
                      [vlan <vlan-value>]
       
       fc-attrs:      [wwpn <world-wide-port-name>] 
                      [wwnn <world-wide-node-name>]
     
      
    """
    output = lib.output.CLIoutput("vadapter")
    valid_list = ['vfabric', 'init_type', 'io_module_id', 'properties'
                  'mac', 'promiscuous', 'protocol', 'silent_listner', 'vlan' ,'wwnn',
                  'wwpn','status']


    if (len(argv) < 2 ):
           lib.errorhandler.InvalidArgumentCount(3, "vadapter-name", syntax=edit.__doc__,
                                                 descape = "Please specify the vadapter name")
           return 

    if ( argv[1] == '?' or argv[1] == 'help'):
          lib.errorhandler.InvalidArgumentCount(syntax=edit.__doc__, descape = "Help")
          return
  
    _parse_edit_or_add_argv(output, argv, valid_list,syntax = edit.__doc__) 

def _check_valid_command_argument(valid_list, args):
    """
      Check from the valid_list if the args exists
    """
    if args in valid_list:
          return 0
    else:
          return -1

def _add_vadapter_protcol(output, argv, arg_dict, syntax = None):
    # This expects [ EN | FC ] as the protocol name.
    proto_value = arg_dict['proto'].lower() 
    if proto_value in ['en', 'fc']:
          if proto_value == 'en':
              _add_vadapter_en_prop(output, argv, arg_dict)
          elif proto_value == 'fc':
              _add_vadapter_fc_prop(output, argv, arg_dict)
          else:
              print 'Unknown Error'
    else:
       lib.errorhandler.InvalidArgumentCount(5, " [EN | FC]", syntax=edit.__doc__,
                                       descape = "Expecting [EN | FC] only " )
    

def _add_vadapter_database(output, argv, arg_dict, syntax = None):
    """
       [add] vadapter <vadapter-name> properties [<general-atts>]
       [EN <en-attrs>] | [FC <fc-attrs>]

       general-attrs: [vfabric <vfabric-id>]
                      [io_module <io-module-id>]
                      [running_mode [ONLINE | OFFLINE]]
                      [init_type [ HOST | NETWORK]]
    """
    valid_list = ['vfabric', 'init_type', 'io_module_id','name','status', 'protocol']
    print arg_dict
    for n in valid_list:
        if not arg_dict.has_key(n):
                arg_dict[n] = 100
    #query = "INSERT INTO bxm_vfabric_attr VALUES(89,12347,76365,'V Fabric3','Desc of VFabric3',2,3,1,0,1,2)"          
    return output
    #database = lib.db.db.Database()
    #cursor = database._execute(query)


def _add_vadapter_en_prop(output, argv, arg_dict):
    """
       [edit | add ] vadapter <vadapter-name> properties [<general-atts>]
       [EN <en-attrs>] | [FC <fc-attrs>]

       en-attrs:      [mac <mac-addr>]
                      [promiscuous_mode [TRUE | FALSE]]
                      [silent_listener [TRUE | FALSE]]
                      [vlan <vlan-value>]
    """
    _NON_EN_PROP = [ 'wwnn','wwpn','fc_id','spma','fmpa']
    # Check for attributes which are not in en properties.
    for n in _NON_EN_PROP:
         if arg_dict.has_key(n):
                    lib.errorhandler.InvalidArgumentCount(syntax=_add_vadapter_en_prop.__doc__,
                                           descape = " Keyword '%s' is not an EN attribute" % (n,))
    
    # Check Boolean values. 
    if arg_dict.has_key('promiscuous'):
            if _check_boolean_value(arg_dict, 'promiscuous') !=0:
               msg = " Keyword 'promiscuous' expects values [TRUE | FALSE]"
               lib.errorhandler.InvalidArgumentCount(syntax=_add_vadapter_en_prop.__doc__,
                                           descape = msg)
    if arg_dict.has_key('silent_listener'):
            if _check_boolean_value(arg_dict, 'silent_listener') !=0:
               msg1 = " Keyword 'silent_listener' expects values [TRUE | FALSE]"
               lib.errorhandler.InvalidArgumentCount(syntax=_add_vadapter_en_prop.__doc__,
                                           descape = msg1)
	
    if arg_dict.has_key('mac'):
	if isMAC(arg_dict['mac']) != 1:
	      print NameError("Not a valid MAC address given")	

    if arg_dict.has_key('vlan'):
         if _isVLAN(arg_dict['vlan']) != 1:
	    print NameError("Incorrect VLAN value, Valid values : d,d,d-d or d,d or d-d Syntax:d = digit [0-9]")
    

# Evaluate Stirng : isVLAN
def _isVLAN(v):
    """ 
      Valid value for the 1,2,3,10-20
      or it can be just 1,2,100 and 10-200
    """
    #if not v or not type(v) is (string): return 0
    v = v.replace(',','')
    v = v.replace('-','')
    for char in v:
	if char not in [0-9]:return 0
    return 1

# Evaluate string: isMAC
# ----------------------
def isMAC(s, ibmac=0):
        """Checks a mac address for correctness, which means it is a 12 character string consisting of
                digits and letters between 'A' and 'F' or 'a' and 'f'"""

        if not s or not type(s) is (string): return 0
        s = s.replace(':', '')
        s = s.replace('-', '')
        if not ibmac:
                if len(s) != 12: return 0
        elif len(s) not in [12, 32]: return 0
        for char in s:
                if char not in string.hexdigits: return 0
        return 1


def _add_vadapter_fc_prop(output, argv, arg_dict):
    """
       [edit | add] vadapter <vadapter-name> properties [<general-atts>]
       [EN <en-attrs>] | [FC <fc-attrs>]

       fc-attrs:   [wwpn <world-wide-port-name>] 
                   [wwnn <world-wide-node-name>]

    """
    #print "IN : _add_vadapter_fc_prop"
    # call parse function for getting the values in the dictionary.
    # Number of arguments.
  
    
    #print "OUT: _add_vadapter_fc_prop"


def _check_boolean_value(arg_dict, key):
    """
      Check the values are boolean or not.
     
    """
    to_check_value = arg_dict[key].lower()
    if to_check_value in ['true', 'false']:
          return 0
    else:
          return -1

def _check_initialization_value(arg_dict, mode):
    """ 
       Check the values for given value
    """
    valid_values = ['host' , 'network']
    given_value = arg_dict[mode].lower()
    if given_value in valid_values:
           return 0
    else:
           return -1 

def _check_status_value(arg_dict, mode):
    """ 
       Check the values for given value
    """
    valid_values = ['online', 'offline']
    given_value = arg_dict[mode].lower()
    if given_value in valid_values:
           return 0
    else:
           return -1

def add(argv):
    """
       [add ] vadapter <vadapter-name> properties [<general-atts>]
       protocol [EN <en-attrs>] | [FC <fc-attrs>]

       general-attrs: [vfabric <vfabric-id>]
                      [io_module <io-module-id>]
                      [running_mode [ONLINE | OFFLINE]]
                      [init_type [ HOST | NETWORK]]

       en-attrs:      [mac <mac-addr>]
                      [promiscuous_mode [TRUE | FALSE]]
                      [silent_listener [TRUE | FALSE]]
                      [vlan <vlan-value>]
       
       fc-attrs:      [wwpn <world-wide-port-name>] 
                      [wwnn <world-wide-node-name>]
     
    """
    output = lib.output.CLIoutput("vadapter")
    valid_list = ['gw_id', 'init_type', 'io-module-id',
                  'mac', 'promiscuous', 'proto', 'silent_listner', 'vlan' ,'wwnn',
                  'wwpn','running_mode', 'vfabric']


    if (len(argv) < 2 ):
           lib.errorhandler.InvalidArgumentCount(3, "vadapter-name", syntax=add.__doc__,
                                                 descape = "Please specify the vadapter name")
           return

    if ( argv[1] == '?' or argv[1] == 'help'):
          lib.errorhandler.InvalidArgumentCount(syntax=add.__doc__, descape = "Help")
          return

    _parse_edit_or_add_argv(output, argv, valid_list,syntax = add.__doc__)
