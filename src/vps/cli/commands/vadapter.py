#!/usr/bin/python
#
# Copyright (c) 2008  VirtualPlane, Inc.

"""Configure Vadapter """
import getopt
import sys
import re
import string

import vfm
import lib.output
import lib.process
import lib.essentials
import lib.db.db
import vps_cli

from lib.errorhandler import *

#
# Globals
#
_LIMITED_SHOW = False
_DETAIL_SHOW = False 
_VIEW_VADAPTER = "v_vfm_vadapter_attr"
_VADPTER_ = "vfm_vadapter_attr"
_VADAPTER_EN = "vfm_vadapter_en_attr"
_VADAPTER_FC = "vfm_vadapter_fc_attr"

def _output_vadapter_list_verbose(outf, name, vadapter_list):
    """ OUTPUT FORMAT IN ROW"""

    if vadapter_list:
         FMT = "%s\n%-10s\n%-20s\n%-5s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n"
#         HEADER = ("id","name","io_module_id","vfabric_id","protocol","init_type","desc","status","mac"
#                     ,"prmiscuous_mode","silent_listener","vlan","wwnn","wwpn","fc_id","spma","fmpa") 
	
         for (n , vadapter) in vadapter_list:
                name = vadapter['NAME']
                id   = vadapter['ID']
                protocol = vadapter['PROTOCOL']
                vfabric = vadapter['VFABRIC']
                io_module = vadapter['IO_MODULE_ID']
                desc  = vadapter['DESC']
                status = vadapter['STATUS']
                init_type = vadapter['INIT_TYPE']
                running_mode = vadapter['RUNNING_MODE']
                if protocol == 1:
                        mac = vadapter['MAC']
                        promiscuous = vadapter['PROMISCUOUS']
                        silent = vadapter['SILENT']
                        vlan = vadapter['VLAN']
                elif protocol == 3:
                        wwnn = vadapter['WWNN']
                        wwpn = vadapter['WWPN']
                        spma = vadapter['SPMA']
                        fpma = vadapter['FPMA']
                        fc_id = vadapter['FC_ID']


                if _LIMITED_SHOW:
                     outf.write('General Attr:\nId: %s\n\tName: %s\n\tIO_Module: %s\n\tVfabric: %s\n\tProtocol: %s\n\tInitization_Type: %s\n\tDescription: %s\n\tStatus: %-20s\n\n' % (id, name, io_module, vfabric, protocol, init_type, desc, status))     
                
		elif _DETAIL_SHOW: 
                     if protocol == 3:
                              outf.write('General Attr:\nId: %s\nName: %s\nIO_Module: %s\nVfabric: %s\nProtocol: %s\nInitization_Type: %s\nDescription: %s\nStatus: %-20s\nRUNNING_MODE :%s\nFC_Attr:\n\tWWNN: %s\n\tWWNP: %s\n\tFC_ID: %s\n\tSPMA: %s\n\tFMPA: %s\n\n' % (id, name, io_module, vfabric, protocol, init_type, desc, status, running_mode, wwnn, wwpn,fc_id,spma,fpma)) 
               
                     elif protocol == 1:
			   outf.write('General Attr:\nId: %s\nName: %s\nIO_Module: %s\nVfabric: %s\nProtocol: %s\nInitization_Type: %s\nDescription: %s\nStatus: %-20s\nRUNNING_MODE :%s\nEN_Attr:\n\tMAC: %s\n\tVLAN: %s\n\tPromiscuous: %s\n\tSilent Listener: %s\n\n' % (id , name, io_module,vfabric, protocol, init_type, desc, status, running_mode, mac,vlan, promiscuous, silent))



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
                show vadapter [vadapter-id]
                show vadapter [vadapter-name]
                show vadapter [ help | ? ]
                 
           
    """


    output = lib.output.CLIoutput("vadapter", _output_handlers)
    # Done to mke changes if needed later with more refining.
	
    global _DETAIL_SHOW
    global _LIMITED_SHOW

    if len(argv) > 3:
	output.completeOutputError(InvalidArgumentCount(syntax = show.__doc__))
	return output
   
    if len(argv) == 2:
        _DETAIL_SHOW = True
	_LIMITED_SHOW = False
	if argv[1] == "--detail":
           _show_vadapter(output, argv)
        elif argv[1] == "?" or argv[1] == "help":
             #if vps_cli.vps_xml_mode:
             #    print "xml"
	     #	output.completeOutputError(InvalidArgumentCount(syntax = show.__doc__)) 
             #else:
             print show.__doc__
	elif "-" not in argv[1]:
	   _show_vadapter_module(output, argv, argv[1])
	else:
	   output.completeOutputError(InvalidArgumentCount(syntax = show.__doc__))
        return output

    elif len(argv) == 1:
	_DETAIL_SHOW = False
	_LIMITED_SHOW = True
        _show_vadapter(output, argv)
        return output
    else:
       output.completeOutputError(InvalidArgumentCount(syntax = show.__doc__))
       return output
    return output


def _show_vadapter_module(output, argv, vadapter_id):
    """
       Show vadapter <vadapter-id>
           syntax : show vadapter <vadapter-id>
    """
    param1 = 0
    output.beginList("VadapterSpecList")
    param1 =  _get_vadapter_values(output, "vadapter", vadapter_id)    
    output.endList("VadapterSpecList")
    if param1 != -1 :
        output.completeOutputSuccess()
    return output

def _show_vadapter(output, argv):
    """
       Show vadapter
           syntax : show vadapter
    """
    param1 = 0
    output.beginList("VadapterSpecList")
    param1 =_get_vadapter_values(output, "vadapter", "ALL")
    output.endList("VadapterSpecList")
    if param1 != -1 :
         output.completeOutputSuccess()
    return output

        
#def _get_vadapter_values(output, mode, vadapter_id = "All"):
def _get_vadapter_values(output, mode, vadapter_id):
    # Get all the attributes from the database ad 
    if vadapter_id != "ALL":
        input = {'id' : int(vadapter_id)}
    elif vadapter_id == "ALL":
        input = {}
    try:
        print input 
        vadapter_info = vfm.py_vfm_vadapter_select_inventory(input)
        #print vadapter_info
    except e:
       print e

    for (id, value) in vadapter_info.items():
       _vadapter_spec(output, id, value)

    return output



#Legacy Routine - directly queries database.
#'''def _get_vadapter_values(output, mode, vadapter_id = "All"):
# LIKE: bridge_info = vfm.py_vfm_bd_select_inventory(input)
#    if vadapter_id == "All":
#         query = "Select * from %s " % (_VIEW_VADAPTER)
#    elif vadapter_id != "All":
#         query = "Select * from %s where id = %s " % (_VIEW_VADAPTER, vadapter_id)
#    database = lib.db.db.Database()
#    cursor = database._execute(query)
#    if cursor == "Null":
#        return -1 
#    if cursor.rowcount == 0 :
#	message = "Response : No Record Found"
#	output.completeOutputError(InvalidArgumentCount(descape = message))
#	return -1 
#    for row in cursor:
#            _vadapter_spec(output, row[0], row[1], row[2], row[3],row[4],row[5],row[6],row[7],row[8],
#			row[9],row[10],row[11],row[12],row[13],row[14],row[15], row[16]) 
#    return output 
#'''     
# Id and status missing from the databse.

def _vadapter_spec(output, id, value):    

    output.beginAssembling("VadapterListAll")

    output.setVirtualNameValue("ID", id)
    output.setVirtualNameValue("NAME", value['name'])
    output.setVirtualNameValue("IO_MODULE_ID", value['io_module_id'])
    output.setVirtualNameValue("VFABRIC", value['vfabric_id'])
    output.setVirtualNameValue("PROTOCOL", value['protocol'])
    output.setVirtualNameValue("INIT_TYPE", value['init_type'])
    output.setVirtualNameValue("DESC", value['desc'])

    # When we get transient states, the state will automatically get udpated.
    # By default, it is RUNNING
    output.setVirtualNameValue("STATUS", value.get('status', 'RUNNING'))
    output.setVirtualNameValue("RUNNING_MODE", value.get('running_mode', 'OFFLINE'))


    if value['protocol'] == 1:
        output.setVirtualNameValue("MAC", value['en_attr']['mac'])
        output.setVirtualNameValue("PROMISCUOUS", value['en_attr']['promiscuous_mode'])
        output.setVirtualNameValue("SILENT", value['en_attr']['silent_listener'])
        output.setVirtualNameValue("VLAN", value['en_attr']['vlan'])
    
    elif value['protocol'] == 3:
        output.setVirtualNameValue("WWNN", value['fc_attr']['wwnn'])
        output.setVirtualNameValue("WWPN", value['fc_attr']['wwpn'])
        output.setVirtualNameValue("FC_ID",value['fc_attr']['fcid'])
        output.setVirtualNameValue("SPMA", value['fc_attr']['spma'])
        output.setVirtualNameValue("FPMA", value['fc_attr']['fpma'])

    output.endAssembling("VadapterListAll") 


#'''
#def _vadapter_spec(output, id, name, desc, init_type, protocol, status, io_module_id,vfabric,
#		    wwnn, wwpn, fc_id, spma, fmpa, mac, promiscuous, silent_listener,vlan):
#    """ Display for the show vadapter """
#    output.beginAssembling("VadapterListAll")
#    output.setVirtualNameValue("ID", id)
#    output.setVirtualNameValue("NAME", name)
#    output.setVirtualNameValue("IO_MODULE_ID", io_module_id)
#    output.setVirtualNameValue("VFABRIC", vfabric)
#    output.setVirtualNameValue("PROTOCOL", protocol)
#    output.setVirtualNameValue("INIT_TYPE", init_type)
#    output.setVirtualNameValue("DESC", desc)
#    output.setVirtualNameValue("STATUS", status)
#    output.setVirtualNameValue("MAC", mac)
#    output.setVirtualNameValue("PROMISCUOUS", promiscuous)
#    output.setVirtualNameValue("SILENT", silent_listener)
#    output.setVirtualNameValue("VLAN", vlan)
#    output.setVirtualNameValue("WWNN", wwnn)
#    output.setVirtualNameValue("WWPN", wwpn)
#    output.setVirtualNameValue("FC_ID",fc_id)
#    output.setVirtualNameValue("SPMA", spma)
#    output.setVirtualNameValue("FMPA", fmpa)
#    output.endAssembling("VadapterListAll") 
#'''

def _parse_edit_or_add_argv(output, argv, valid_args, syntax = None, call_from = 'None'):
    arg_dict = {}
    index = 0
    
    if len(argv[0].split()) > 1:
          output.completeOutputError(lib.errorhandler.InvalidArgumentCount(3, "vadapter-name", syntax,
                                                descape = "Please specify the vadapter name"))
          return output
    
    if call_from == "add" :
         if isAddName(argv[1]) != 0:
	    return output
    elif call_from == "edit" :
	 if isEditName(argv[1]) != 0:
	    return output
    else:
	output.completeOutputError(InvalidArgumentCount(descape ="'%s' Command not found . Please type [ edit | add ] vadapter [ ? | help ]  " % (argv[0])))
        return output


    if call_from == "add" :
        arg_dict['name'] = argv[1]
    elif call_from == "edit" :
	arg_dict['id'] = argv[1]
    index = 2 
    len_args = len(argv)
 
    while index < len_args:
         args = argv[index]    
         # Next argument should be from the valid_list
         if _check_valid_command_argument(valid_args, args) != 0:
             # Raise an Error
             output.completeOutputError(lib.errorhandler.InvalidArgumentCount(index+2, args, syntax,
                                       descape = ("'%s' Not a valid Keyword" % args)))
             return output
         index += 1
         if arg_dict.has_key(args):
             msg = " '%s' already specified." % (args,)
             output.completeOutputError(InvalidArgumentCount(index+1, args, syntax,
                                             descape = msg))
             return output

         if (index >= len_args):
               if _check_valid_command_argument(valid_args, argv[index-1]) != 0:
                    msg1 = "'%s' Not a valid Keyword" % args
                    output.completeOutputError(InvalidArgumentCount(index+2, args, 
                          syntax,descape = msg1))
                    return output
               msg = " '%s' doesn't have the value defined" % (args,)
               output.completeOutputError(InvalidArgumentCount(index+2, args,
                               syntax,descape = msg))
               return output

         if _check_valid_command_argument(valid_args, argv[index]) == 0:
             msg = " '%s' is not an expected value of '%s'" % (argv[index], args)
             output.completeOutputError(InvalidArgumentCount(index+3, argv[index], 
                                      syntax,descape = msg))
             return output
          
         arg_dict[args] = argv[index]
         index += 1
    #print arg_dict

    if arg_dict.has_key('status'):
         if _check_status_value(arg_dict, 'status') != 0:
              msg = " Attribute 'status' has [ONLINE | OFFLINE] as valid values"
              output.completeOutputError(InvalidArgumentCount(descape = "%s='%s' Not a valid command . \n %s \n" % 
					('status', arg_dict['status'],msg)))
              return output

    if arg_dict.has_key('init_type'):
        # Valid arguments are [ HOST | NETWORK]
        if _check_initialization_value(arg_dict, 'init_type') != 0:
              msg = "Attribute 'init_type' has [HOST | NETWORK] as valid inputs"
              output.completeOutputError(InvalidArgumentCount(descape = "%s='%s' Not a valid command . \n %s \n " % 
					('init_type', arg_dict['init_type'],msg)))
              return output 

    if arg_dict.has_key('vfabric_id'):
      if validateVfabric(output ,arg_dict, 'vfabric_id') == None:
	  msg = "Attribute \'vfabric_id\' is not valid. Numeric [0-9] length =  '%s' \n " \
		% (lib.constants._ATTR_ID_LENGHT)
          output.completeOutputError(InvalidArgumentCount(descape = msg))
	  return output

    if arg_dict.has_key('io_module_id'):
       if validateIOmoduleId(output, arg_dict , 'io_module_id') == None:
	  msg = "Valid values of Attribute \'io_module_id\' are Numeric [0-9], length =  '%s'" \
	 	 % (lib.constants._ATTR_ID_LENGHT)
	  output.completeOutputError(InvalidArgumentCount(descape = msg))
	  return output

    if arg_dict.has_key('protocol'):
         # Then it will call _add_en or add_fc respectively
          _add_vadapter_protcol(output, argv, arg_dict, call_from, syntax)
	  return output
    else:
         # Add the values to the database.
         _add_vadapter_database(output, argv, arg_dict, call_from ,syntax)
	 return output
         
#
# argv[0] == 'edit_vadapter'
# argv[1] == '<object>'
# argv[2] depends on argv[1]
#
def edit(argv):
    """
       [edit] vadapter <vadapter-id> online

       [edit] vadapter <vadapter-id> [<general-attrs>]
       protocol [EN <en-attrs>] | [FC <fc-attrs>]

       general-attrs: [name <name>]
                      [vfabric_id <vfabric_id>]
                      [io_module_id <io_module_id>]
                      [status [ONLINE | OFFLINE]]
                      [init_type [ HOST | NETWORK]]
                      [assignment_type [AUTO | MANUAL | ONCE ]]
                      [component_mask [INT]]

       en-attrs:      [mac <mac-addr>]
                      [promiscuous [Enabled | Disabled]]
                      [silent_listener [Enabled | Disabled]]
                      [vlan <vlan-value>]
       
       fc-attrs:      [wwpn <world-wide-port-name>] 
                      [wwnn <world-wide-node-name>]
     
      
    """
    output = lib.output.CLIoutput("vadapter")
    valid_list = ['assignment_type','component_mask' ,'vfabric_id', 'init_type', 'io_module_id', 'name',
                  'mac', 'promiscuous', 'protocol', 'silent_listener', 'vlan' ,'wwnn',
                  'wwpn','status']

    if (len(argv) < 2 ):
           output.completeOutputError(lib.errorhandler.InvalidArgumentCount(3, "vadapter-name", syntax=edit.__doc__,
                                                 descape = "Please specify the vadapter id"))
           return output

    if ( argv[1] == '?' or argv[1] == 'help'):
          output.completeOutputError(lib.errorhandler.InvalidArgumentCount(syntax=edit.__doc__, descape = "Help"))
          return output

    if argv[2].lower() == 'online':
           if isEditName(argv[1]) == -1:
                print "Error Not a valid Id"
                return output  
           else:
               dict = {}
               dict['id'] = int(argv[1])
               try:
                      result = vfm.py_vfm_vadapter_online(dict)
               except StandardError, e:
                      print "Error!" ,e
                      return output 
               else:
                      print result
                      return output

    _parse_edit_or_add_argv(output, argv, valid_list,syntax = edit.__doc__ , call_from = 'edit' ) 

    return output

def _check_valid_command_argument(valid_list, args):
    """
      Check from the valid_list if the args exists
    """
    if args in valid_list:
          return 0
    else:
          return -1

def _add_vadapter_protcol(output, argv, arg_dict, call_from, syntax = None):
    # This expects [ EN | FC ] as the protocol name.
    proto_value = arg_dict['protocol'].lower() 
    if proto_value in ['en', 'fc']:
          if proto_value == 'en':
              _add_vadapter_en_prop(output, argv, arg_dict, call_from)
          elif proto_value == 'fc':
              _add_vadapter_fc_prop(output, argv, arg_dict, call_from)
          else:
              print 'Unknown Error'
    else:
       output.completeOutputError(lib.errorhandler.InvalidArgumentCount(5, "[EN | FC]", syntax=edit.__doc__,
                                       descape = "Expecting [EN | FC] as protocol value only " ))
    

def _add_vadapter_database(output, argv, arg_dict, call_from, syntax = None):
    """
       [add] vadapter <vadapter-name> [<general-atts>]
       [EN <en-attrs>] | [FC <fc-attrs>]

       general-attrs: [vfabric_id <vfabric_id>]
                      [io_module_id <io_module_id>]
                      [status [ONLINE | OFFLINE]]
                      [init_type [ HOST | NETWORK]]
                      [assignment_type [AUTO | MANUAL | ONCE ]]
                      [component_mask [INT]]
    """
    valid_list = ['vfabric_id', 'init_type', 'io_module_id','name','status', 'protocol', 'assignment_type']
    #print arg_dict
    dict = {}
    if call_from == 'edit':
        arg_dict = _editing_edit_dictionary(arg_dict)
        result = vfm.py_vfm_vadapter_edit_general_attr(arg_dict)
	print "vdapter edited:", result
    elif call_from == 'add':
        arg_dict = _editing_edit_dictionary(arg_dict)
        vadapter = vfm.py_vfm_vadapter_create(arg_dict)
        print "vadpter created:", vadapter
    return output


def _add_vadapter_en_prop(output, argv, arg_dict, call_from):
    """
       [edit | add ] vadapter <vadapter-name> [<general-atts>]
       [EN <en-attrs>] | [FC <fc-attrs>]

       en-attrs:      [mac <mac-addr>]
                      [promiscuous [TRUE | FALSE]]
                      [silent_listener [TRUE | FALSE]]
                      [vlan <vlan-value>]
    """
    _NON_EN_PROP = [ 'wwnn','wwpn','fc_id','spma','fmpa']
    # Check for attributes which are not in en properties.
    for n in _NON_EN_PROP:
         if arg_dict.has_key(n):
                    output.completeOutputError(InvalidArgumentCount(syntax=_add_vadapter_en_prop.__doc__,
                                           descape = " Keyword '%s' is not an EN attribute" % (n,)))
		    return output
    
    # Check Boolean values. 
    if arg_dict.has_key('promiscuous'):
            if _check_boolean_value(arg_dict, 'promiscuous') !=0:
               msg = " Keyword 'promiscuous' expected value [Enabled| Disabled]"
	       output.completeOutputError(InvalidArgumentCount(descape = " \n '%s'='%s' Invalid promiscuous value . \n %s \n" % 
					('promiscuous',arg_dict['promiscuous'],msg)))
	       return output 

    if arg_dict.has_key('silent_listener'):
            if _check_boolean_value(arg_dict, 'silent_listener') !=0:
               msg1 = " Keyword 'silent_listener' expected value [Enabled| Disabled]"
  	       output.completeOutputError(InvalidArgumentCount(descape = "\n '%s'='%s' Invalid silent_listener value . \n %s \n " % 
					('silent_listener', arg_dict['silent_listener'], msg1)))
	       return output
	
    if arg_dict.has_key('mac'):
	if isMAC(arg_dict['mac']) != 1:
	      output.completeOutputError(InvalidArgumentCount(descape = "\n Invalid MAC address \n "))
	      return output

    if arg_dict.has_key('vlan'):
         if _isVLAN(arg_dict['vlan']) != 1:
	    output.completeOutputError(InvalidArgumentCount(descape = "\n Invalid VLAN value, Expected values : d,d,d-d or d,d or d-d Syntax:d = digit [0-9] \n"))  
	    return output
   
    #print arg_dict
    arg_dict = _editing_edit_dictionary(arg_dict)
    if call_from == 'edit':
        #
        # Change the dictionary according to the reggression.py
        # 
        result = vfm.py_vfm_vadapter_edit_general_attr(arg_dict)
        print "vdapter edited:", result
    elif call_from == 'add':
        vadapter = vfm.py_vfm_vadapter_create(arg_dict)
        print "vadpter created:", vadapter

    return output
    

def _add_vadapter_fc_prop(output, argv, arg_dict, call_from):
    """
       [edit | add] vadapter <vadapter-name> [<general-atts>]
       [EN <en-attrs>] | [FC <fc-attrs>]

       fc-attrs:   [wwpn <world-wide-port-name>] 
                   [wwnn <world-wide-node-name>]

    """
    _NON_FC_PROP = ['mac','silent_listener','promiscuous','vlan']
    # Check for attributes which are not in fc properties.
    for n in _NON_FC_PROP:
	if arg_dict.has_key(n):
                    output.completeOutputError(InvalidArgumentCount(syntax=_add_vadapter_fc_prop.__doc__,
                                           descape = " Keyword '%s' is not an FC attribute" % (n,)))
		    return output

    if arg_dict.has_key('wwpn'):
       if  _validateWorldWideNodeOrPortName(arg_dict, 'wwpn') != 0:
	       output.completeOutputError(InvalidArgumentCount(syntax=_add_vadapter_fc_prop.__doc__,
                                     descape = " '%s' = '%s' is not valid Value" % ('wwpn',arg_dict['wwpn'])))
	       return output
		
    if arg_dict.has_key('wwnn'):
       if  _validateWorldWideNodeOrPortName(arg_dict, 'wwnn') != 0:
               output.completeOutputError(InvalidArgumentCount(syntax=_add_vadapter_fc_prop.__doc__,
                                     descape = " '%s' = '%s' is not valid Value" % ('wwnn', arg_dict['wwnn'])))
	       return output 

    # FC-ID ?
    #print arg_dict
    arg_dict = _editing_edit_dictionary(arg_dict)
    if call_from == 'edit':
         #
         # Change the dictionary according to the reggression.py
         # 
        result = vfm.py_vfm_vadapter_edit_general_attr(arg_dict)
        print "vdapter edited:", result
    elif call_from == 'add':
        vadapter = vfm.py_vfm_vadapter_create(arg_dict)
        print "vadpter created:", vadapter

# Add in the function.
def add(argv):
    """
       [add ] vadapter <vadapter-name> [<general-atts>]
       protocol [EN <en-attrs>] | [FC <fc-attrs>]

       general-attrs: [vfabric_id <vfabric_id>]
                      [io_module_id <io_module_id>]
                      [status [ONLINE | OFFLINE]]
                      [init_type [ HOST | NETWORK]]
                      [assignment_type [AUTO | MANUAL | ONCE ]]
                      [component_mask [INT]]

       en-attrs:      [mac <mac-addr>]
                      [promiscuous [Enabled | Disabled]]
                      [silent_listener [Enabled | Disabled]]
                      [vlan <vlan-value>]
       
       fc-attrs:      [wwpn <world-wide-port-name>] 
                      [wwnn <world-wide-node-name>]
     
    """
    output = lib.output.CLIoutput("vadapter")
    valid_list = ['assignment_type','component_mask','init_type', 'io_module_id','status', 'vfabric_id',
                  'mac', 'promiscuous', 'protocol', 'silent_listener', 'vlan' ,'wwnn',
                  'wwpn']

    if (len(argv) < 2 ):
           output.completeOutputError(lib.errorhandler.InvalidArgumentCount(3, "vadapter-name", syntax=add.__doc__,
                                                 descape = "Please specify the vadapter name"))
           return output

    if ( argv[1] == '?' or argv[1] == 'help'):
          output.completeOutputError(lib.errorhandler.InvalidArgumentCount(syntax=add.__doc__, descape = "Help"))
          return output

    _parse_edit_or_add_argv(output, argv, valid_list,syntax = add.__doc__, call_from = 'add')

    return output

def _editing_edit_dictionary(dict):

    if 'id' in dict:
          dict['id'] = int(dict['id'])
    if 'vfabric_id' in dict:      
          dict['vfabric_id'] = int(dict['vfabric_id'])
    if 'io_module_id' in dict:
        dict['io_module_id'] = int(dict['io_module_id'])

    if 'protocol' in dict:
        if dict['protocol'].lower() == 'en':
              dict['protocol'] = int('1')
        elif dict['protocol'].lower() == 'fc':
              dict['protocol'] = int('3')
        elif dict['protocol'].lower() == 'ib':
              dict['protocol'] = int('2')
    
    if 'running_mode' in dict:
        if dict['running_mode'] == 'OFFLINE' or dict['running_mode'] == 'offline':
              dict['running_mode'] = int('0')
        elif dict['running_mode'] == 'ONLINE' or dict['running_mode'] == 'online':      
              dict['running_mode'] = int('1')

    if 'init_type' in dict:
        if dict['init_type'].lower() == 'host':
             dict['init_type'] = int('1')
        elif dict['init_type'].lower() == 'NETWORK':     
             dict['init_type'] = int('2')

    if 'assignment_type' in dict:
         if dict['assignment_type'].lower() == 'AUTO':
              dict['assignment_type'] = int('1')    
         elif dict['assignment_type'].lower() == 'ONCE':
              dict['assignment_type'] = int('2')
         elif dict['assignment_type'].lower() == 'MANUAL':
              dict['assignment_type'] = int('3')

    if 'spma' in dict:
         if dict['spma'].lower() == 'disabled':
              dict['spma'] = int('0')
         elif  dict['spma'].lower() == 'enabled':
              dict['spma'] = int('1')

    if 'fpma' in dict:
         if dict['fpma'].lower() == 'disabled':
               dict['fpma'] = int('0')
         if dict['fpma'].lower() == 'enabled':
               dict['fpma'] = int('1')

    return dict
   

def _edit_dictionary_en(dict):
    # en_attr 
    en_list = ['mac', 'vlan', 'mtu', 'promiscuous_mode', 'silent_listener']
    en_attr_dict = {}
    for n in en_list:
        print n
        if dict[n] != Null:
               en_attr_dict = {en_attr : { } }

def _edit_dictionary_fc(dict):
    # fc_attr
    fc_list = ['wwnn', 'wwpn' , 'fcid','mtu','spma', 'fpma']

              

# Check name Add
#    1. Should be alphanumeric
#    2. Should not start with an digit
#    3. Allow "_" and "-" in the name
#    4. Imp: Should be unique.
def isAddName(name):
    """ Checks the name if correct then return 1 else return -1"""	
    if lib.essentials.isAlphanumeric(name) != 0:
	print " '%s' is not valid name. \n Vadapter-name should be an alphanumeric." % (name)
        #output.completeOutputError(lib.errorhandler.InvalidArgumentCount(descape = " '%s' is not valid name. \n Vadapter-name should be an alphanumeric." % (name))) 
        return -1
   
    if lib.essentials.isStartNumeric(name) != 0:
	print "'%s' is not valid name. \n Vadapter name should not start with an digit"% (name)
	#output.completeOutputError(lib.errorhandler.InvalidArgumentCount(descape = "'%s' is not valid name. \n Vadapter name should not start with an digit"% (name)))
        return -1

    if lib.essentials.isContainSpecial(name) != 0:
	print "'%s' is not valid name. \n Vadapter name should not contain special characher" % (name)
	#output.completeOutputError(InvalidArgumentCount(descape = "'%s' is not valid name. \n Vadapter name should not contain special characher" % (name)))
        return -1

#    if lib.db.db.ifExistsInDatabase(name) == 0:
#	print NameError("'%s' is not valid name. \n Already Exists"  % (name))
#	return -1
    
    return 0
 
def isEditName(id):
    """ Checks if the name exists in the database"""
    for char in id:
        if re.compile('[0-9]+').match(char[0]) == None:
           print NameError("'%s' is not valid name. \n Id should be numeric" % (name))
           return -1
    return 0

# Evaluate String : isVLAN
def _isVLAN(v):
    """ 
      Valid value for the 1,2,3,10-20
      or it can be just 1,2,100 and 10-200
    """
    #if not v or not type(v) is (string): return 0
    v = v.replace(',','')
    v = v.replace('-','')
    for char in v:
        if re.compile('[0-9]+').match(char) == None:return 0
    return 1

# Evaluate string: isMAC
# ----------------------
def isMAC(s):
        """Checks a mac address for correctness, which means it is a 12 character string consisting of
                digits and letters between 'A' and 'F' or 'a' and 'f'"""

        s = s.replace(':', '')
        if len(s) != 12: return 0
        for char in s:
                if re.compile('[a-zA-Z0-9]+').match(char) == None: return 0
        return 1


# Here we will validat the vfabric id through the name or the id.
def validateVfabric(output ,arg_dict, key):
    """
        Check in the database for the valid key.        
        if Not found then return "None". Else return arg_dict
        
         INTERCHANGEABLE : Here we can change the id with the name.
    """
    id = arg_dict[key]
    counter = 0
    for char in id:
       counter += 1
       if re.compile('[0-9]+').match(char[0]) == None:
          output.completeOutputError(InvalidArgumentCount(descape ="'%s' = '%s' is not a valid Id. ID should be numeric " % 
				(key,id)))
          return None
       if counter > lib.constants._ATTR_ID_LENGHT:
	  output.completeOutputError(InvalidArgumentCount(descape ="'%s'='%s' is not a valid Id. \n ID should be numeric with Length = '%s' " % (key,id, lib.constants._ATTR_ID_LENGHT)))
          return None
    return arg_dict

# Here we will validat the iomodule  id through the name or the id.
def validateIOmoduleId(output ,arg_dict , key):
    """
        Check in the database for the valid key.        
        if Not found then return "None". Else return arg_dict
        INTERCHANGEABLE : Here we can change the id with the name.
    """
    id = arg_dict[key]
    counter = 0
    for char in id:
       counter += 1
       if re.compile('[0-9]+').match(char[0]) == None:
          output.completeOutputError(InvalidArgumentCount(descape ="'%s'='%s' is not a valid Id. \n ID should be numeric " % (key,id)))     
          return None
       if counter > lib.constants._ATTR_ID_LENGHT:
          output.completeOutputError(InvalidArgumentCount(descape ="'%s'='%s' is not a valid Id. \n ID should be numeric with Length = '%s' " % (key,id, lib.constants._ATTR_ID_LENGHT)))
          return None
    return arg_dict


# Checking ENABLED | DISABLED
def _check_boolean_value(arg_dict, key):
    """
      Check the values are boolean or not.
     
    """
    to_check_value = arg_dict[key].lower()
    if to_check_value in ['disabled', 'enabled']:
          return 0
    else:
          return -1

# CHECKING HOST | NETWORK
def _check_initialization_value(arg_dict, mode):
    """ 
       Check the values for given value
    """
    valid_values = ["host" , "network"]
    given_value = arg_dict[mode].lower()
    if given_value in valid_values:
           return 0
    else:
           return -1

# Checking the ONLINE | OFFLINE
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

import re

# Check the wwnn and wwpn to the given format.
def _validateWorldWideNodeOrPortName(arg_dict, mode):
    """
        Check the values for the given format
        50:00:1f:e1:50:02:f1:10
    """
    value = arg_dict[mode]
    if value not in [':']: return -1
    value = values.replace(':', '')
    if re.compile('[a-z0-9]+').match(value) == None:
        return -1
    else:
        return  0

# Validate the FC_ID in the FC properties.
# Could be any Id . Checks for the length and the digit
def validateID(id):
    """
        This checks for the valid digit format in the id.
        Also the length of the ID from the lib.constants._LENGTH_
    """

    if re.compile('[0-9]+').match(id) == None:
        output.completeOutputError(InvalidArgumentCount(descape ="'%s' is not a valid Id. ID should be numeric with Length = '%s' " 
			% (id, lib.constants._ATTR_ID_LENGHT)))
        return -1
    else:
        # Check for the lenght 
        counter = 0
        for char in id:
                counter += 1
        print counter ,  lib.constants._ATTR_ID_LENGHT
        if counter > lib.constants._ATTR_ID_LENGHT :
           output.completeOutputError(InvalidArgumentCount(descape ="'%s' exceeded the given length i.e Max Length  = '%s'" % 
			(id, lib.constants._ATTR_ID_LENGHT)))
           return -1
        else:
           return 0
    return 0


# Validate the Component Mask
def validateComponentMask(comp_mask):
    """ 
        Checks the component mask and returns the -1 if error and 
        0 if true.
    """
    return 0
	
