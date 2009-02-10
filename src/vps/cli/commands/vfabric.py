#!/usr/bin/python
#
# Copyright (c) 2008  VirtualPlane, Inc.

"""Configure Vfabric"""
import getopt
import sys
import re
import string

import vfm
import lib.output
import lib.process
import lib.essentials
import lib.db.db

from lib.errorhandler import *


_LIMITED_SHOW = False
_DETAIL_SHOW = False
_VIEW_VFABRIC = " v_vfm_vfabric_attr "

def _output_vfabric_list_verbose(outf, name, vfabric_list):
    """ OUTPUT FORMAT IN ROW"""
    if vfabric_list:
	for (n , vfabric) in vfabric_list:
		name = vfabric['NAME']
		id = vfabric['ID']
 		desc = vfabric['DESC']
		ctx_table_id = vfabric['CTX_TABLE_ID']
		primary_gw_id = vfabric['PRIMARY_GW_ID']
		backup_gw_id = vfabric['BACKUP_GW_ID']
		protocol =  vfabric['PROTOCOL']
		num_vadapter =  vfabric['NUM_VADAPTER']
		vadapter_id =  vfabric['VADAPTER_IDs']
		running_mode =  vfabric['RUNNING_MODE']
		ha_state =  vfabric['HA_STATE']
		type = vfabric['TYPE']
		auto_failover = vfabric['AUTO_FAILOVER']
		auto_failback = vfabric['AUTO_FAILBACK']
                if protocol == 1:
        		vlan = vfabric['VLAN']
	        	mac = vfabric['MAC']
                if protocol == 3:
		        fcid = vfabric['FCID']
        		wwnn = vfabric['WWNN']
        		wwpn = vfabric['WWPN']
		

                if _LIMITED_SHOW:
	 	   outf.write('General Attr:\nId: %s\n\tName: %s\n\tDesc: %s\n\tCTX_Table_ID: %s\n\tPrimary_Gateway_Id: %s\n\tProtocol: %s\n\tType: %s\n\t\n' % (id, name, desc, ctx_table_id, primary_gw_id, protocol, type))		
	
		elif _DETAIL_SHOW:
		     if protocol == 3:
			outf.write('General Attr:\nId: %s\n\tName: %s\n\tDesc: %s\n\tCTX_Table_ID: %s\n\tPrimary_Gateway_Id: %s\n\tBackUp_Gateway_Id: %s\n\tProtocol: %s\n\tType: %s\n\tAuto_FailOver: %s\n\tAuto_Failback: %s\n\tRUNNING_MODE: %s\n\tHA_STATE : %s\nFC_Attr:\n\tFCID: %s\n\tWWNN: %s\n\tWWPN: %s\n\t\n' % (id, name, desc, ctx_table_id, primary_gw_id,backup_gw_id, protocol, type,auto_failover,auto_failback, running_mode, ha_state, fcid,wwnn,wwpn))

		     elif protocol == 1:
			  outf.write('General Attr:\nId: %s\n\tName: %s\n\tDesc: %s\n\tCTX_Table_ID: %s\n\tPrimary_Gateway_Id: %s\n\tBackUp_Gateway_Id: %s\n\tProtocol: %s\n\tType: %s\n\tAuto_FailOver: %s\n\tAuto_Failback: %s\n\tRUNNING_MODE: %s\n\tHA_STATE : %s\nEN_Attr:\n\tMAC: %s\n\tVLAN: %s\n\t\n' % (id, name, desc, ctx_table_id, primary_gw_id,backup_gw_id, protocol, type,auto_failover,auto_failback, running_mode, ha_state, mac, vlan))

	 	

        

_output_handlers = {
                        "VfabricSpecList"  : _output_vfabric_list_verbose
                   }


#
# Possible invocations:
#        show vfabric <vfabric_id>
# Note that the 'show' keyword is *not* included in argv
#
def show(argv):
    """
        Show Vfabric-related information
        syntax: show vfabric 
                show vfabric --detail
                show vfabric <vfabric_id>
                show vfabric <vfabric-name>
                show vfabric [ help | ? ]
                 
    """
    output = lib.output.CLIoutput("vfabric", _output_handlers)
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
           _show_vfabric(output, argv)
        elif argv[1] == "?" or argv[1] == "help":
	   if vps_cli.vps_xml_mode:
		output.completeOutputError(InvalidArgumentCount(syntax = show.__doc__))
	   else:
                print show.__doc__
        elif "-" not in argv[1]:
           _show_vfabric_module(output, argv, argv[1])
        else:
	   output.completeOutputError(InvalidArgumentCount(syntax = show.__doc__))
        return output

    elif len(argv) == 1:
        _DETAIL_SHOW = False
        _LIMITED_SHOW = True
        _show_vfabric(output, argv)
        return output
    else:
       output.completeOutputError(InvalidArgumentCount(syntax = show.__doc__))
       return output
    return output


def _show_vfabric_module(output, argv, vfabric_id):
    """
       Show vfabric <vfabric_id>
           syntax : show vfabric <vfabric_id>
    """
    param1 = 0
    output.beginList("VfabricSpecList")
    _get_vfabric_values(output, "vfabric", vfabric_id)
    output.endList("VfabricSpecList")
    if param1 != -1:
        output.completeOutputSuccess()
    return output 

def _show_vfabric(output, argv):
    """
       Show vfabric
           syntax : show vfabric
    """
    param1 = 0
    output.beginList("VfabricSpecList")
    param1 = _get_vfabric_values(output, "vfabric", "All")
    output.endList("VfabricSpecList")
    if param1 != -1 :
        output.completeOutputSuccess()
    return output

def _get_vfabric_values(output, mode, vfabric_id):
    if vfabric_id == "All":
        input = {}
        print "vfabric_id", vfabric_id
    elif vfabric_id != "All":
        vfabric_id = int(vfabric_id)
        input = {'id' : vfabric_id}
    try:
        vfabric_info = vfm.py_vfm_vfabric_select_inventory(input)
        #print vfabric_info
    except e:
        print e

    for (id, value) in vfabric_info.items():
        _vfabric_spec(output, id, value)
           
    return output


'''
def _get_vfabric_values(output, mode, vfabric_id = "All"):
    if vfabric_id == "All":
         query = "Select * from %s " % (_VIEW_VFABRIC)
    elif vfabric_id != "All":
         query = "Select * from %s where id = %s " % (_VIEW_VFABRIC, vfabric_id)
    database = lib.db.db.Database()
    cursor = database._execute(query)
    if cursor == "Null":
        return -1 
    if cursor.rowcount == 0 :
	message = "Response : No Record Found"
        output.completeOutputError(InvalidArgumentCount(descape = message))
        return -1
    for row in cursor:
            _vfabric_spec(output, row[0], row[1], row[2], row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11],row[12],row[13],row[14],row[15])
    return output

def _vfabric_spec(output, id, name, desc, ctx_table_id, primary_gw_id, backup_gw_id, protocol,type,auto_failover,auto_failback,component_mask, vlan, mac, fcid, wwnn, wwpn): 
    output.beginAssembling("VfabricListAll")
    output.setVirtualNameValue("ID", id)
    output.setVirtualNameValue("NAME", name)
    output.setVirtualNameValue("CTX_TABLE_ID", ctx_table_id)
    output.setVirtualNameValue("PRIMARY_GW_ID", primary_gw_id)
    output.setVirtualNameValue("PROTOCOL", protocol)
    output.setVirtualNameValue("TYPE", type)
    output.setVirtualNameValue("DESC", desc)
    output.setVirtualNameValue("BACKUP_GW_ID",backup_gw_id)
    output.setVirtualNameValue("MAC", mac)
    output.setVirtualNameValue("AUTO_FAILOVER",auto_failover)
    output.setVirtualNameValue("AUTO_FAILBACK",auto_failback)
    output.setVirtualNameValue("VLAN", vlan)
    output.setVirtualNameValue("WWNN", wwnn)
    output.setVirtualNameValue("WWPN", wwpn)
    output.setVirtualNameValue("FCID",fcid)
    output.setVirtualNameValue("COMPONENT_MASK",component_mask )
    output.endAssembling("VfabricListAll")

'''
def _vfabric_spec(output, id, value):
    """ Display for the show vfabric """
    output.beginAssembling("VfabricListAll")

    output.setVirtualNameValue("ID", id)
    output.setVirtualNameValue("NAME", value['name'])
    output.setVirtualNameValue("CTX_TABLE_ID", value['ctx_table_id'])
    output.setVirtualNameValue("PRIMARY_GW_ID", value['primary_gateway'])
    output.setVirtualNameValue("PROTOCOL", value['protocol'])
    output.setVirtualNameValue("TYPE", value.get('type','HOST_INITIATED'))
    output.setVirtualNameValue("NUM_VADAPTER", value['num_vadapter_id'])
    output.setVirtualNameValue("VADAPTER_IDs", value['vadapter_id'])
    output.setVirtualNameValue("DESC", value['desc'])
    output.setVirtualNameValue("BACKUP_GW_ID",value['backup_gateway'])
    output.setVirtualNameValue("AUTO_FAILOVER",value['auto_failover'])
    output.setVirtualNameValue("AUTO_FAILBACK",value['auto_failback'])
    #Running mode:default set to OFFLINE
    output.setVirtualNameValue("RUNNING_MODE", value.get('running_mode', 'OFFLINE'))
    output.setVirtualNameValue("HA_STATE", value.get('ha_state', 'NORMAL'))

    if value['protocol'] == 1:
        output.setVirtualNameValue("MAC", value['en_attr']['mac'])
        output.setVirtualNameValue("VLAN", value['en_attr']['vlan'])

    if value['protocol'] == 3:
        output.setVirtualNameValue("WWNN", value['fc_attr']['wwnn'])
        output.setVirtualNameValue("WWPN", value['fc_attr']['wwpn'])
        output.setVirtualNameValue("FCID", value['fc_attr']['fcid'])
        
    output.endAssembling("VfabricListAll")



def edit(argv):
    """
       [edit] vfabric <vfabric-id> online 


       [edit] vfabric <vfabric-id> [<general-attrs>]
       protocol [EN <en-attrs>] | [FC <fc-attrs>]

       general-attrs: [running_mode <OFFLINE | ONLINE>]
                      [desc <"DETAILS">]
                      [ha_state < NORMAL | FAILOVER>]
                      [auto_failover < DISABLED | ENABLED>]
                      [auto_failback < DISABLED | ENABLED>]
                      [primary_gw_id <primary_gw_id>]
                      [backup_gw_id <backup_gw_id>]
                      [protocol <protocol>]
                      [component_mask <component_mask>]

       en-attrs:      [mac <mac-addr>]
                      [vlan <vlan-value>]
       
       fc-attrs:      [wwpn <world-wide-port-name>] 
                      [wwnn <world-wide-node-name>]
    """
    output = lib.output.CLIoutput("vfabric")
    valid_list = ['primary_gw_id', 'backup_gw_id', 'running_mode', 'desc', 'ha_state', 
		  'component_mask','type','mac', 'auto_failover' , 'auto_failback',
		  'protocol','vlan' ,'wwnn','wwpn',]		
    
    if (len(argv) < 2 ):
           output.completeOutputError(lib.errorhandler.InvalidArgumentCount(3, "vfabric-id",
				syntax=edit.__doc__,descape = 
				"Please specify the vfabric id"))
           return output

    if ( argv[1] == '?' or argv[1] == 'help'):
             output.completeOutputError(lib.errorhandler.InvalidArgumentCount(syntax=edit.__doc__,
                                     descape = "Help"))
             return output

    if argv[2].lower() == 'online':  
                   
            if isEditName(output, argv[1]) == -1:
                print "Error Not a valid Id"
                return output  
            else:
                dict = {}
                dict['id'] = int(argv[1])
                try:
                        result = vfm.py_vfm_vfabric_online(dict)
                except StandardError, e:
                        print "Error!" ,e
                        return output 
                else:
                        print result
                        return output

    if ( argv[1] == '?' or argv[1] == 'help'):
             output.completeOutputError(lib.errorhandler.InvalidArgumentCount(syntax=edit.__doc__, 
		descape = "Help"))
             return output
 
    _parse_edit_or_add_argv(output, argv, valid_list,syntax = edit.__doc__ , call_from = 'edit' )

    return output

def _parse_edit_or_add_argv(output, argv, valid_args, syntax = None, call_from = 'None'):
    arg_dict = {}
    index = 0

    if len(argv[0].split()) > 1:
          output.completeOutputError(lib.errorhandler.InvalidArgumentCount(3, "vfabric-name", syntax,
                                                descape = "Please specify the vfabric name"))
	  return output

    if call_from == "add" :
         if isAddName(output,argv[1]) != 0:
            return output
    elif call_from == "edit" :
         if isEditName(output, argv[1]) != 0:
            return output
    else:
        output.completeOutputError(InvalidArgumentCount(descape = "'%s' Command not found . Please type [ edit | add ] vfabric [ ? | help ]  " % (argv[0])))
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
             output.completeOutputError(InvalidArgumentCount(index+3, args, syntax,
                                       descape = ("'%s' Not a valid Keyword" % args)))
             return output
         index += 1
         if arg_dict.has_key(args):
             msg = " '%s' already specified." % (args,)
             output.completeOutputError(InvalidArgumentCount(index+2, args, syntax,
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
    
    if arg_dict.has_key('protocol'):
         # Then it will call _add_en or add_fc respectively
          _add_vfabric_protcol(output, argv, arg_dict, call_from, syntax)
          return output
    else:
         # Add the values to the database.
         _add_vfabric_database(output, argv, arg_dict, call_from, syntax)
         return output

    return output 



def _add_vfabric_protcol(output, argv, arg_dict, call_from, syntax = None):
    # This expects [ EN | FC ] as the protocol name.
    proto_value = arg_dict['protocol'].lower()
    if proto_value in ['en', 'fc']:
          if proto_value == 'en':
              _add_vfabric_en_prop(output, argv, arg_dict, call_from)
          elif proto_value == 'fc':
              _add_vfabric_fc_prop(output, argv, arg_dict, call_from)
          else:
              print 'Unknown Error'
    else:
       output.completeOutputError(lib.errorhandler.InvalidArgumentCount(5, "[EN | FC]", 
		syntax=edit.__doc__,descape = "Expecting [EN | FC] as protocol value only " ))
    return output


def _add_vfabric_database(output, argv, arg_dict,call_from, syntax = None):
    """
       [add] vfabric <vfabric-name> [<general-atts>]
       [EN <en-attrs>] | [FC <fc-attrs>]

       general-attrs: 
                      [primary_gw_id <primary_gw_id>]
                      [backup_gw_id <backup_gw_id>]
                      [protocol <protocol>]
                      [component_mask <component_mask>]

    """
    valid_list = ['primary_gw_id','backup_gw_id','component_mask', 'protocol']
    #print arg_dict
    arg_dict = _editing_edit_dictionary(arg_dict)
    if call_from == 'edit':
        print arg_dict
        result = vfm.py_vfm_vfabric_edit_general_attr(arg_dict)
        print "vfabric edited:", result
    elif call_from == 'add':
        vfabric = vfm.py_vfm_vfabric_create(arg_dict)
        print "vfabric created:", vfabric
    return output



def _add_vfabric_en_prop(output, argv, arg_dict, call_from):
    """
       [edit | add ] vfabric <vfabric-name> [<general-atts>]
       [EN <en-attrs>] | [FC <fc-attrs>]

       en-attrs:      [mac <mac-addr>]
                      [vlan <vlan-value>]
    """
    _NON_EN_PROP = [ 'wwnn','wwpn','fcid']
    # Check for attributes which are not in en properties.
    for n in _NON_EN_PROP:
         if arg_dict.has_key(n):
                    output.completeOutputError(InvalidArgumentCount(syntax=_add_vfabric_en_prop.__doc__,
                                           descape = " Keyword '%s' is not an EN attribute" % (n,)))
                    return output

    if arg_dict.has_key('mac'):
        if isMAC(arg_dict['mac']) != 1:
	      output.completeOutputError(InvalidArgumentCount(descape = "\n Invalid MAC address \n "))
              return output

    if arg_dict.has_key('vlan'):
         if _isVLAN(arg_dict['vlan']) != 1:
            output.completeOutputError(InvalidArgumentCount(descape = "\n Invalid VLAN value, Expected values : d,d,d-d or d,d or d-d Syntax:d = digit [0-9] \n"))
            return output

    print arg_dict
    arg_dict = _editing_edit_dictionary(arg_dict)
    if call_from == 'edit':
        result = vfm.py_vfm_vfabric_edit_general_attr(arg_dict)
        print "vfabric edited:", result
    elif call_from == 'add':
        vfabric = vfm.py_vfm_vfabric_create(arg_dict)
        print "vfabric created:", vfabric

    return output
	 


def _add_vfabric_fc_prop(output, argv, arg_dict, call_from):
    """
       [edit | add] vfabric <vfabric-name> [<general-atts>]
       [EN <en-attrs>] | [FC <fc-attrs>]

       fc-attrs:   [wwpn <world-wide-port-name>] 
                   [wwnn <world-wide-node-name>]

    """
    _NON_FC_PROP = ['mac','vlan']
    # Check for attributes which are not in fc properties.
    for n in _NON_FC_PROP:
        if arg_dict.has_key(n):
                    output.completeOutputError(InvalidArgumentCount(syntax=_add_vfabric_fc_prop.__doc__,
                                           descape = " Keyword '%s' is not an FC attribute \n" % (n,)))
                    return output

    if arg_dict.has_key('wwpn'):
       if  _validateWorldWideNodeOrPortName(arg_dict, 'wwpn') != 0:
               output.completeOutputError(InvalidArgumentCount(syntax=_add_vfabric_fc_prop.__doc__,
                                     descape = " '%s' = '%s' is not valid Value \n " % ('wwpn',arg_dict['wwpn'])))
               return output

    if arg_dict.has_key('wwnn'):
       if  _validateWorldWideNodeOrPortName(arg_dict, 'wwnn') != 0:
               output.completeOutputError(InvalidArgumentCount(syntax=_add_vfabric_fc_prop.__doc__,
                                     descape = " '%s' = '%s' is not valid Value \n " % ('wwnn', arg_dict['wwnn'])))
               return output

    # FC-ID ?
    print arg_dict
    arg_dict = _editing_edit_dictionary(arg_dict)
    if call_from == 'edit':
        result = vfm.py_vfm_vfabric_edit_general_attr(arg_dict)
        print "vfabric edited:", result
    elif call_from == 'add':
        vfabric = vfm.py_vfm_vfabric_create(arg_dict)
        print "vfabric created:", vfabric
    return output

# Add in the function.
def add(argv):
    """
       [add] vfabric <vfabric-name> [<general-attrs>]
       protocol [EN <en-attrs>] | [FC <fc-attrs>]

       general-attrs: [running_mode <OFFLINE | ONLINE>
                      [desc < "DETAILS">]
                      [ha_state < NORMAL | FAILOVER>]
                      [auto_failover < DISABLED | ENABLED>]
                      [auto_failback < DISABLED | ENABLED>]
                      [primary_gw_id <primary_gw_id>]
                      [backup_gw_id <backup_gw_id>]
                      [protocol <protocol>]
                      [component_mask <component_mask>]

       en-attrs:      
                      [vlan <vlan-value>]
       
       fc-attrs:      [wwpn <world-wide-port-name>] 
                      [wwnn <world-wide-node-name>]
    """ 

    output = lib.output.CLIoutput("vfabric")
    valid_list = ['primary_gw_id', 'backup_gw_id',
                  'ha_state','auto_failover', 'auto_failback', 'running_mode','desc'
                  ,'component_mask','type','mac',
                  'protocol','vlan' ,'wwnn','wwpn']


    if (len(argv) < 2 ):
           output.completeOutputError(InvalidArgumentCount(3, "vfabric-name",
                                syntax=add.__doc__,descape =
                                "Please specify the vfabric name"))
           return output


    if ( argv[1] == '?' or argv[1] == 'help'):
             output.completeOutputError(InvalidArgumentCount(syntax=add.__doc__,
                descape = "Help"))
             return output

    _parse_edit_or_add_argv(output, argv, valid_list,syntax = add.__doc__ , call_from = 'add' )

    return output

########################################################################################
#										       #
#	Checking for all the add and edit attributes			               #
#										       #
########################################################################################


def _check_valid_command_argument(valid_list, args):
    """
      Check from the valid_list if the args exists
    """
    if args in valid_list:
          return 0
    else:
          return -1


# Check name Add
#    1. Should be alphanumeric
#    2. Should not start with an digit
#    3. Allow "_" and "-" in the name
#    4. Imp: Should be unique.
def isAddName(output, name):
    """ Checks the name if correct then return 1 else return -1"""
    if lib.essentials.isAlphanumeric(name) != 0:
        output.completeOutputError(InvalidArgumentCount(descape ="'%s' is not valid name. \n Vfabric-name should be an alphanumeric." % (name)))
        return -1

    if lib.essentials.isStartNumeric(name) != 0:
        output.completeOutputError(InvalidArgumentCount(descape = "'%s' is not valid name. \n Vfabric name should not start with an digit"% (name)))
        return -1

    if lib.essentials.isContainSpecial(name) != 0:
        output.completeOutputError(InvalidArgumentCount(descape = "'%s' is not valid name. \n Vfabric name should not contain special characher" % (name)))
        return -1

#    if lib.db.db.ifExistsInDatabase(name) == 0:
#       print NameError("'%s' is not valid name. \n Already Exists"  % (name))
#       return -1

    return 0

def isEditName(output, id):
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
def validateVfabric(output, arg_dict, key):
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
         output.completeOutputError(InvalidArgumentCount(descape = "'%s' = '%s' is not a valid Id. \n ID should be numeric " % (key,id)))
         return None
       if counter > lib.constants._ATTR_ID_LENGHT:
         output.completeOutputError(InvalidArgumentCount(descape = "'%s'='%s' is not a valid Id. \n ID should be numeric with Length = '%s' " % (key,id, lib.constants._ATTR_ID_LENGHT)))
         return None
    return arg_dict

# Here we will validat the iomodule  id through the name or the id.
def validateIOmoduleId(output, arg_dict , key):
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
         output.completeOutputError(InvalidArgumentCount(descape = "'%s'='%s' is not a valid Id. \n ID should be numeric " % (key,id)))
         return None
       if counter > lib.constants._ATTR_ID_LENGHT:
         output.completeOutputError(InvalidArgumentCount(descape = "'%s'='%s' is not a valid Id. \n ID should be numeric with Length = '%s' " % (key,id, lib.constants._ATTR_ID_LENGHT)))
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
    given_value = arg_dict[mode]
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
def validateID(output, id):
    """
        This checks for the valid digit format in the id.
        Also the length of the ID from the lib.constants._LENGTH_
    """

    if re.compile('[0-9]+').match(id) == None:
        output.completeOutputError(InvalidArgumentCount(descape = "'%s' is not a valid Id. ID should be numeric with Length = '%s' " % (id, lib.constants._ATTR_ID_LENGHT)))
        return -1
    else:
        # Check for the lenght 
        counter = 0
        for char in id:
                counter += 1
        print counter ,  lib.constants._ATTR_ID_LENGHT
        if counter > lib.constants._ATTR_ID_LENGHT :
           output.completeOutputError(InvalidArgumentCount(descape ="'%s' exceeded the given length i.e Max Length  = '%s'" % (id, lib.constants._ATTR_ID_LENGHT)))
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

def _editing_edit_dictionary(dict):

    if 'id' in dict:
          dict['id'] = int(dict['id'])
                               
    if 'primary_gw_id' in dict:
           dict['primary_gw_id'] = int(dict['primary_gw_id'])
           
    if 'backup_gw_id' in dict:
           dict['backup_gw_id']  = int(dict['backup_gw_id'])

    if 'auto_failover' in dict:
             if dict['auto_failover'].lower() == 'disabled':
                    dict['auto_failover'] = int('0')
             elif  dict['auto_failover'].lower() == 'enabled':
                    dict['auto_failover'] = int('1')
# Hansraj: Setting running mode in dictionary: Start
    if 'running_mode' in dict:
              if dict['running_mode'].lower() == 'offline':
                     dict['running_mode'] = int('0')
              elif  dict['running_mode'].lower() == 'online':
                     dict['running_mode'] = int('1')
# Runnung mode : End

    if 'auto_failback' in dict:
             if dict['auto_failback'].lower() == 'disabled':
                    dict['auto_failback']  = int('0')
             elif  dict['auto_failback'].lower() == 'enabled':
                    dict['auto_failback'] = int('1')
                  
    if 'ha_state' in dict:
            if dict['ha_state'].lower() == 'normal':
                    dict['ha_state'] = int('0')
            elif dict['ha_state'].lower() == 'failover':
                    dict['ha_state'] = int('1')

    if 'protocol' in dict:
            if dict['protocol'].lower() == 'en':
                   dict['protocol'] = int('1')
            elif dict['protocol'].lower() == 'fc':
                   dict['protocol'] = int('3')
            elif dict['protocol'].lower() == 'ib':
                   dict['protocol'] = int('2')

    return dict
                    
