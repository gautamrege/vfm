#!/usr/bin/python
#
# help.py	
#
# Copyright (c) 2008  VirtualPlane, Inc.
#

"""Use "help", "help commands", or "help <modifier>"."""

import string
import sys

path = 'src/vps/cli'
if path not in sys.path:
     sys.path.append(path)

import command_ref
import lib.output

from lib.errorhandler import *

#
# Support show help, add help, no help, clear help
#
def show(argv):
    """Print all help messages for commands that take the "show" modifier
        syntax: show help
    """
    output = lib.output.CLIoutput("help")
    output.setVirtualNameValue("Help", _SHOW_HELP_TEXT)
    output.completeOutputSuccess()
    return output


def add(argv):
    """Print all help messages for commands that take the "add" modifier
        syntax: add help
    """
    output = lib.output.CLIoutput("help")
    output.setVirtualNameValue("Help", _ADD_HELP_TEXT)
    output.completeOutputSuccess()
    return output


def no(argv):
    """Print all help messages for commands that take the "no" modifier
        syntax: no help
    """
    output = lib.output.CLIoutput("help")
    output.setVirtualNameValue("Help", _NO_HELP_TEXT)
    output.completeOutputSuccess()
    return output


def clear(argv):
    """Print all help messages for commands that take the "clear" modifier
        syntax: clear help
    """
    output = lib.output.CLIoutput("help")
    output.setVirtualNameValue("Help", _CLEAR_HELP_TEXT)
    output.completeOutputSuccess()
    return output


#
# Support help -- using the edit mode since edit have no argument
#
def edit(argv):
    """Print help messages
        syntax: help
                help commands
                help <command-category>
                help <modifier>
                help [<modifier>] <command-name>
        modifier: add | clear | edit | no | show
    """
    output = lib.output.CLIoutput("help")
    if len(argv) == 1:
        # No argument - print out the general help
        output.setVirtualNameValue("Help", _GENERAL_HELP)
        output.completeOutputSuccess()
        return output

    # Now the arguments can be a category or a modifier or a command
    # Try treat it as a category first
    if len(argv) == 2:
        category = argv[1]
        try:
            output.setVirtualNameValue("Help", _display_help_category(category))
            output.completeOutputSuccess()
            return output
        except KeyError:
            # Not a category. Fine.
            pass

    # Maybe it's a modifier or a plugin... Get the plugin.
    try:
        try:
            modifier, cmd, plugin_name = \
                      lib.command_ref.cmdToPlugin(argv[1:])
        except lib.command_ref.UnknownPluginException, ex:
            modifier = ex.getModifier()
            # No plugin because the user wants help on a modifier ("help no")?
            # If the exception doesn't have any command, that means the user
            # only entered a modifier.
            if not ex.getCommand():
                output.setVirtualNameValue('Help', _display_modifier_help(modifier))
                output.completeOutputSuccess()
                return output
            raise ex

        if not sys.modules.has_key(plugin_name):
            try:
                __import__(plugin_name)
            except ImportError:
                raise NameError("INTERNAL ERROR: no such plugin: " + \
                                plugin_name)

        module = sys.modules[plugin_name]
        doc_str = None
        if hasattr(module, modifier):
            function = getattr(module, modifier)
            doc_str = getattr(function, '__doc__', None)

        if doc_str is None and modifier == lib.command_ref.EDIT_MODIFIER:
            doc_str = module.__doc__
        if doc_str is not None:
            output.setVirtualNameValue('Help', doc_str)
            output.completeOutputSuccess()
            return output
    except (lib.command_ref.UnknownPluginException,
            NameError, AttributeError), ex:
        pass

    # Try to match it in the general section help dict.
    orig_arg = string.join(argv[1:]).lower()
    try:
        help_text = _select_one_help_from_dict(orig_arg, _ALL_COMMANDS_DICT)
        header = _make_help_header('Commands')
        output.setVirtualNameValue('Help', header + help_text)
        output.completeOutputSuccess()
        return output
    except KeyError, ex:
        _logger.debug("=== not in the section of all commands")

    # Error. Nothing matches. Try to show the help for the modifier
    help_text = _display_modifier_help(modifier)
    output.completeOutputError('No help available for "%s".\n' % orig_arg +
                            help_text)
    return output


def _display_modifier_help(modifier):
    '''Do the help on a modifier.'''
    if modifier == lib.command_ref.SHOW_MODIFIER:
        return _SHOW_HELP_TEXT
    elif modifier == lib.command_ref.NO_MODIFIER:
        return _NO_HELP_TEXT
    elif modifier == lib.command_ref.CLEAR_MODIFIER:
        return _CLEAR_HELP_TEXT
    elif modifier == lib.command_ref.MOD_ADD:
        return _ADD_HELP_TEXT
    elif modifier == lib.command_ref.EDIT_MODIFIER:
        return _EDIT_HELP_TEXT
    else:
        return _GENERAL_HELP

def _select_help_from_dict(commands, dict):
    '''We want to display the help topics in "commands". The "dict" contains
    the help information.
    Return a formatted paragraph (w/o any header) of the help dictionary.
    '''
    output = ''
    for cmd in commands:
        try:
            output += _select_one_help_from_dict(cmd, dict)
        except KeyError:
            output += "%-32s%s\n" % (cmd, "[missing help info]")
    return output

def _select_one_help_from_dict(command, dict):
    '''Same as _select_help_from_dict, only that this method accept one command
    instead of a list of commands.
    '''
    desc = dict[command]
    line = "%-32s%s\n" % (command, desc)
    return line

def _display_help_category(category_name):
    '''Do the help on a help category.
    Return a formatted paragraph (with header).
    May raise KeyError for invalid category.
    '''
    try:
        commands = _HELP_CATEGORY[category_name]
    except KeyError, ex:
        raise ex

    commands.sort()
    help_text = _select_help_from_dict(commands, _ALL_COMMANDS_DICT)
    header = _make_help_header(category_name)
    return header + help_text

def _make_help_header(title):
    title_cap = title[0].upper() + title[1:]
    header = title_cap + '\n' + _SEPARATOR + '\n'
    return header



_SEPARATOR = \
"----------------------------------------------------------------------------"

#
#
_EDIT_HELP_TEXT = \
'''
Commands
------------------------------------------------------------------------------
edit ip                           Edit IP configuration information
edit vadapter                     Edit vNIC and vHBA configuration
edit vfabric                      Edit virtual fabric configuration
'''

_ADD_HELP_TEXT = \
'''
Commands
------------------------------------------------------------------------------
add ip                           Add IP configuration information
add vadapter                     Add vNIC and vHBA configuration
add vfabric                      Add virtual fabric configuration
'''

_SHOW_HELP_TEXT = \
'''
Commands
------------------------------------------------------------------------------
show help                       Show command descriptions
show hostname                   Show the host name
show ip                         Show IP configuration
show vadapter                   Show vNIC and vHBA configuration 
show vfabric 			Show virtual fabric configuration 
show gateway                    Show gateway information
show iomodule                   Show converge network adapter configuration 
show bridge                     Show multifabric bridge device configuration
'''

_NO_HELP_TEXT = \
'''
Commands
------------------------------------------------------------------------------
no ip                           Clear IP configuration
no vadapter                     Clear vNIC and vHBA configuration
no vfabric 			Clear virtual fabric configuration
'''

_CLEAR_HELP_TEXT = \
'''
Commands
------------------------------------------------------------------------------
clear ip                        Clear IP configuration
clear vadapter                  Clear vNIC and vHBA configuration
clear vfabric 			Clear virtual fabricconfiguration
'''

_ALL_COMMANDS_DICT = \
{
    'admin'             : 'Manage Virtual Fabric Manager administration accounts',
    'add'               : 'Add a new object',
    'edit'              : 'Edit the configuration of an existing object',
    'exit'              : 'Exit from the CLI session',
    'end'               : 'End the edit config terminal session',
    'halt'              : 'Halt the system',
    'help'              : 'Show this help screen',
    'hostname'          : 'Configure hostname',
    'ip'                : 'Configure ip',
    'save'              : 'Save configuration or diagnostic information ' \
                          'for support',
    'show'              : 'Show commands. Use "help show" for more info',
    'software'          : 'Install new software or switch to previous software',
    'gateway'           : 'Show gateway configuration', 
    'bridge'            : 'Show multifabric bridge device configuration',
    'iomodule'          : 'Show Converge Network Adapter Configuration',
    'vadapter'          : 'Show vNIC and vHBA configuration',
    'clear'             : 'Clear configuration',
    'no'                : 'Clear configuration',
}

_GENERAL_HELP = """

Type 'help' or '?' to get a list of commands in that category.

Prefixes
------------------------------------------------------------------------------
add                             Add a new object
edit    [optional]              Edit the configuration of an existing object
clear                           Remove an object or attribute
no                              Same as 'clear' command
show                            Show commands. Use 'help show' for more info

Commands:
------------------------------------------------------------------------------
commands                        Lists all commands
gateway                         Gateway configuration
bridge                          Multifabric bridge device configuration
iomodule                        Converge Network Adapter Configuration
vadapter                        vNIC and vHBA configuration
"""

#
# Build the categorized help dictionary
#
_HELP_CATEGORY = {}
_HELP_CATEGORY["CLI"] = \
    ["add", "clear", "echo", "exit", "no", "quit", "show"]
_HELP_CATEGORY["commands"] = _ALL_COMMANDS_DICT.keys()
_HELP_CATEGORY["gateway"] = \
    ["show"]
_HELP_CATEGORY["iomodule"] = \
    ["show"]
_HELP_CATEGORY["vadapter"] = \
    ["show" ,"edit" , "no", "clear", "add" ]
_HELP_CATEGORY["bridge"] = \
    ["show"]
_HELP_CATEGORY["vfabric"] = \
    ["show", "edit", "no", "clear", "add"]
