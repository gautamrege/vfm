#!/usr/bin/python2
#
# Copyright (c) 2008  VirtualPlane Systems, Inc.
#

import string
import types
import command_ref
import sys

path = 'src/vps/cli'
if path not in sys.path:
      sys.path.append(path)
    
import lib.db.db
import lib.constants

MODIFIER = ''

"""
_COMMAND_EDIT_REF = \
{

    'hostname'          : 'commands.hostname',
    'software'          : 'commands.software'
}
"""

# define my own dictionary

_COMMAND_REF = \
{
    'admin'		: 'commands.admin',
    'exit'		: 'exit',
    'quit'              : 'quit',
    'end'               : 'end',
    'enable'            : 'enable',
    'disable'           : 'disable',
    'setup'             : 'setup',
     'configure'        : 
      {
        ''              : 'configure terminal',
        'terminal'      : 
        {
          ''            : 'configure terminal',
          'edit'        : 'configure terminal', 
          'add'         : 'configure terminal',
          'show'        : 'configure terminal',
          'help'        : 'configure terminal',
          'clear'       : 'configure terminal',
          'no'          : 'configure terminal',
          MODIFIER      : 'configure terminal'
        },       
      },
    'interface'         : 'commands.interface',
    'arp'               : 'commands.arp',
    'password'          : 'commands.password',
    'clock'             : 'commands.clock',
    'help'		: 'commands.help',
    '?'                 : 'commands.help',
    'hostname'		: 'commands.hostname',
    'ip'		: 
      {
	''              : 'commands.ip',
 	'address'       : 'commands.ip',
	'route'		: 'commands.ip'
      },
    'ping'		: 'commands.ping',
    'reboot'		: 'commands.reboot',
    'halt'              : 'commands.halt',
    '_reload'		: 'commands.reload',
    'vadapter'          : 
     {
       ''               : 'commands.vadapter',
       'vhba001'        : 'commands.vadapter',
       'proto'          : 'commands.vadapter',
       'io-module-id'   : 'commands.vadapter',
       'gw_id'          : 'commands.vadapter',
       'init_type'      : 'commands.vadapter', 
       'running_mode'   : 'commands.vadapter',
       'promiscous'     : 'commands.vadapter',
       'silent_list'    : 'commands.vadapter',
       'mac'            : 'commands.vadapter',
       'vlan'           : 'commands.vadapter',
       'wwnn'           : 'commands.vadapter',
       'wwpn'           : 'commands.vadapter',
       'vfabric'        : 'commands.vadapter'
     },
    'gw_id'             : 'commands.bxm',
    'bridge'            : 'commands.bridge',
    'iomodule'          : 'commands.iomodule',
    'gateway'           : 'commands.gateway',
    'vfabric'           : 'commands.vfabric',
}

#
# Fro adding new sub command mode .Add specific commands in the
# COMMAND_REF inside the required parameters aswell outside to make 
# prompt change from anywhere
# 
#
#  Also you have to add the the sub command mode in 
#  COMMAND_MODIFIER_NON_CO_EXISTS
#  MOD_<Object> 
#
#  Similarly if you add a command mode then add new command in
#  COMMAND_MODIFIER_NON_CO_EXISTS
#  NON_SUB_CMD_MOD
#
EDIT_MODIFIER = "edit"
SHOW_MODIFIER = "show"
NO_MODIFIER = "no"
CLEAR_MODIFIER = "clear"
MOD_ADD = "add"
COMMAND_MODIFIER = [EDIT_MODIFIER, SHOW_MODIFIER, NO_MODIFIER, CLEAR_MODIFIER, MOD_ADD]

NON_SUB_CMD_MOD = ["gateway" , 'vfabric', "vadapter", "terminal" , "bridge" , "iomodule"]

MOD_GATEWAY = ""
MOD_VADAPTER = ""
MOD_BRIDGE = ""
MOD_IOMODULE = ""

def get_vadapter_sub_mode():
    global MOD_VADAPTER
    query = "Select id from %s"% (lib.constants._VIEW_VADAPTER)
    database = lib.db.db.Database()
    cursor = database._execute(query) 
    counter = 0
    sub_mode = ""
    for ids in cursor:
          if counter > 0:
              sub_mode  = sub_mode + "," + "\"" +str(ids[0]) + "\""
          else:
	      sub_mode = "\"" + str(ids[0]) + "\""
	      counter += 1
    MOD_VADAPTER = sub_mode

def get_bridge_sub_mode():
    global MOD_BRIDGE
    query = "Select guid from %s"% (lib.constants._VIEW_BRIDGE)
    database = lib.db.db.Database()
    cursor = database._execute(query)
    counter = 0
    sub_mode = ""
    for ids in cursor:
          if counter > 0:
              sub_mode  = sub_mode + "," + "\"" +str(ids[0]) + "\""
          else:
              sub_mode = "\"" + str(ids[0]) + "\""
              counter += 1
    MOD_BRIDGE = sub_mode

def get_iomodule_sub_mode():
    global MOD_IOMODULE
    query = "Select id from %s"% (lib.constants._VIEW_IOMODULE)
    database = lib.db.db.Database()
    cursor = database._execute(query)
    counter = 0
    sub_mode = ""
    for ids in cursor:
          if counter > 0:
              sub_mode  = sub_mode + "," + "\"" +str(ids[0]) + "\""
          else:
              sub_mode = "\"" + str(ids[0]) + "\""
              counter += 1
    MOD_IOMODULE = sub_mode

def get_gateway_sub_mode():
    global MOD_GATEWAY
    query = "Select gw_id from %s"% (lib.constants._VIEW_GATEWAY)
    database = lib.db.db.Database()
    cursor = database._execute(query)
    counter = 0
    sub_mode = ""
    for ids in cursor:
          if counter > 0:
              sub_mode  = sub_mode + "," + "\"" +str(ids[0]) + "\""
          else:
              sub_mode = "\"" + str(ids[0]) + "\""
              counter += 1
    MOD_GATEWAY = sub_mode

#get_bridge_sub_mode()
#get_iomodule_sub_mode()
#get_gateway_sub_mode()
#get_vadapter_sub_mode()
 
SUB_CMD_MOD = ""

SUB_CMD_MOD = [ MOD_GATEWAY ,MOD_VADAPTER, MOD_BRIDGE] 
COMMAND_MODIFIER_NON_CO_EXISTS =SUB_CMD_MOD + NON_SUB_CMD_MOD

ALL_MODIFIERS = [ EDIT_MODIFIER, SHOW_MODIFIER, NO_MODIFIER, CLEAR_MODIFIER, MOD_ADD ]

class UnknownPluginException(Exception):
    def __init__(self, modifier, cmd):
        Exception.__init__(self)
        self.__modifier = modifier
        self.__cmd = cmd
    def __str__(self):
        if self.__cmd:
            return "Unknown command: " + self.__cmd
        else:
            return "The modifier %r must be followed by a command" % \
                   (self.__modifier,)
    def getModifier(self):
        '''The modifier in the command (EDIT_MODIFIER by default)'''
        return self.__modifier
    def getCommand(self):
        """The command that doesn't match any command"""
        return self.__cmd

def cmdToMode(command, cmd_tokens):
    # Here we will return the mode components.
    tokens = cmd_tokens
    target = "configure" + " "+ cmd_tokens[0]
    modifier = __getModifier(cmd_tokens)
    return (modifier, string.join(command), target, tokens)

   
def cmdToModifier(cmd_tokens, current_mode):
    if len(cmd_tokens) == 1 and cmd_tokens[0] == "show" \
                  and "config" in current_mode :

            tokens = []
            token = current_mode.split("-")
            mode = token[1].split(")")
            if mode[0] in NON_SUB_CMD_MOD and mode[0] != 'terminal':
                    command_name = "commands."+mode[0]
    elif len(cmd_tokens) == 1 and cmd_tokens[0] == "edit" \
		and "config" in current_mode :
            tokens = []
            token = current_mode.split("-")
            mode = token[1].split(")")
	    if mode[0] in NON_SUB_CMD_MOD and mode[0] != 'terminal':
		command_name = "commands."+mode[0]
    return (cmd_tokens[0], mode[0], command_name)
     
    
def cmdToPlugin(cmd_tokens, current_mode):
    """
    # Try to strip the modifier
    if len(cmd_tokens) == 1 and cmd_tokens[0] == "show" \
                  and "config" in current_mode :
            tokens = []
            token = current_mode.split("-")
            mode = token[1].split(")")
            if mode[0] in COMMAND_MODIFIER_NON_CO_EXISTS and mode[0] != 'terminal':
		    temp_args = lib.interpreter.vps_current_module
  	            if mode[0] in SUB_CMD_MOD:
			  if mode[0] == MOD_VADAPTER:
				mode[0] = "vadpater"
                    		command_name = "commands.vadapter"
			  elif mode[0] == MOD_BRIDGE:
				mode[0] = "bridge"
				command_name = "commands.bridge"
			  elif mode[0] == MOD_GATEWAY:
				mode[0] = "gateway"
				command_name = "commands.gateway"
			  else:
				command_name = "commands."+mode[0]
                    else:
			  command_name = "commands."+mode[0]
                    return (cmd_tokens[0], mode[0], command_name)
    """
    modifier = __getModifier(cmd_tokens)
  
    # Lookup the map
    the_map = _COMMAND_REF
    consumed_tokens = []
    scanned_tokens = []

    while 1:
        if not cmd_tokens:
            #
            # Try to find if there is a command associated with an "" key,
            # signifying to use the commands for the last token
            #
            try:
                target = the_map[""]
            except KeyError:
                    raise UnknownPluginException(modifier, \
                          string.join(consumed_tokens))

            return (modifier, string.join(consumed_tokens), target)
	
        next_token = cmd_tokens[0]
        scanned_tokens.append(next_token)
	try:
            target = the_map[next_token.lower()]
            # consume it
            consumed_tokens.append(next_token)
            cmd_tokens.pop(0)
        except KeyError:
            try:
                target = the_map[""]
            except KeyError:
                raise UnknownPluginException(modifier, \
                        string.join(scanned_tokens))

        # If we get a string, that's the command name.
        if type(target) == types.StringType:
            return (modifier, string.join(consumed_tokens), target)

        # Otherwise we get a dict. So keep going.
        the_map = target
    return None


def cmdCompletion(completed_cmd_tokens, incomplete_token):
    output = []

    if not completed_cmd_tokens:
        # For the first word, also consider the mod strings.
        output.extend(__prefixMatch(ALL_MODIFIERS, incomplete_token))
    else:
        # Try to strip the modifier
        __getModifier(completed_cmd_tokens)

    if (not completed_cmd_tokens) and (not incomplete_token):
        return output

    # The normal commands
    the_map = _COMMAND_REF
    while completed_cmd_tokens:
        next_token = completed_cmd_tokens.pop(0)
        try:
            target = the_map[next_token.lower()]
        except KeyError:
            # No such key in the map.
            return output

        # We get a string, that's the command name and we've exhausted the map.
        if type(target) == types.StringType:
            return output

        # Otherwise we get a dict. So keep going.
        the_map = target

    # At this point, we've used up all the completed_cmd_tokens. So try to do a
    # case-insensitive prefix match of incomplete_token in the map.
    output.extend(__prefixMatch(the_map.keys(), incomplete_token))
    return output


def __getModifier(cmd_tokens):
    '''Determine the modifier.
    Returns modifier.
    '''
    head = cmd_tokens[0].lower()
    if head in ALL_MODIFIERS:
        cmd_tokens.pop(0)
        return head
    # default 'edit'.
    return EDIT_MODIFIER


def __prefixMatch(candidate_list, prefix):
    output = []
    l_prefix = prefix.lower()
    for candidate in candidate_list:
        # No hidden commands
        if candidate.startswith('_'):
            continue
        # Must match prefix
        if not candidate.startswith(l_prefix):
            continue

        # Match! Nice. Now do case adjustment.
        if l_prefix:
            cased_string = string.replace(candidate, l_prefix, prefix,
                                            maxsplit=1)
        else:
            cased_string = candidate
        output.append(cased_string + ' ')
    return output


def requiresActiveNode(modifier, cmd):
    'Returns True if the modifier variant of cmd requires an active node.'
    if modifier == SHOW_MODIFIER:
        return cmd.lower() in _SHOW_ONLY_ON_ACTIVE
    if modifier in (EDIT_MODIFIER, NO_MODIFIER, CLEAR_MODIFIER, MOD_ADD):
        return cmd.lower() in _CHANGE_ONLY_ON_ACTIVE
    return False


def requiresPrimaryNode(modifier, cmd):
    'Returns True if the modifier variant of cmd requires a primary node.'
