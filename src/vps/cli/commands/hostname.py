#!/usr/bin/python
#
# Copyright (c) 2008  VirtualPlane, Inc.
#
"""
Get/set hostname. Use "hostname" or "show hostname".
"""

import getopt
import sys
import os
import re
import socket


path = 'src/vps/cli'
if path not in sys.path:
     sys.path.append(path)

import lib.output
import lib.essentials
import lib.process
import lib.network_config

# Command to set the hostname
_SET_HOSTNAME_CMD            = "/bin/hostname"
_KILLALL_CMD                 = "/usr/bin/killall"

_XMLTAG_Hostname             = "Hostname"
_SET_PATH                    = "/usr/local"

def _set_hostname(hostname):
    """Sets the hostname,
    """
    
    cmd = (_SET_HOSTNAME_CMD, hostname)
    exitval, out, err, fullstatus = lib.process.execute_command(cmd)
   
    if exitval != 0:
        return 0

    # System hostname updated, update configuration file.
    net_config = lib.network_config.NetworkConfig()
    net_config.setHostname(hostname)
    net_config.save()
    net_config.notify_hostname_changed(net_config)
    return 1

def show(argv):
    """Get the hostname
        syntax: show hostname
   """ 
    output = lib.output.CLIoutput("hostname")
    if len(argv) > 1:
        output.completeOutputError(InvalidArgumentCount (syntax=show.__doc__))
        return output
    # Get hostname from the system.
    hostname = getSystemHostname()
    #print hostname
    if hostname is None:
        output.completeOutputError("unable to determine current hostname")
        return output
    output.beginAssembling("hostname")
    output.setVirtualNameValue("hostname",hostname)
    output.endAssembling("hostname")
    output.completeOutputSuccess()
    return output


def getSystemHostname():
    """Returns the hostname. Returns None on error."""
    try:
        return socket.gethostname()
    except socket.error:
        return None

def edit(argv):
    """Set the hostname
        syntax: [edit] hostname <new-hostname>
    """
    output = lib.output.CLIoutput("hostname")
  
    if len(argv) != 2:
        output.completeOutputError(InvalidArgumentCount (syntax=edit.__doc__))
        return output

    new_name = argv[1]
    try:
        checkHostname(new_name)
    except InvalidHostnameException, ex:
        output.completeOutputError(ex)
        return output

    #if '.' in new_name:
    #    output.completeOutputError("cannot specify a domain")
    #    return output
   
    if _set_hostname(new_name) != 1:
        output.completeOutputError("could not set the hostname")
        return output
   
    output.completeOutputSuccess()
    return output

def _init():
    """Hostname initialization.
       Reads the hostname from the configuration, and sets the hostname.
    """
    try:
        hostname = lib.essentials.getHostname()
        cmd = (_SET_HOSTNAME_CMD, hostname)
        lib.process.execute_command(cmd)
    except:
	pass


def _main():
    """This function is invoked when hostname.py functions as a
    stand-alone program
    """
    try:
        opts, args = getopt.getopt(sys.argv[1:], "c:d:shi")

    except getopt.GetoptError:
	# print help information and exit:
        _usage()
        sys.exit(2)
    cmd = "edit_hostname"
    for o, a in opts:
        if o == "-c":
            sys.exit(_check_hostname(a))
        elif o == "-i":
            cmd = "init_hostname"
        elif o == "-s":
            cmd = "show_hostname"
        elif o == "-h":
            _usage()
            sys.exit(0)
	else:
            _usage()
            sys.exit(1)
    args.insert(0, cmd);
    if cmd == "edit_hostname":
	edit(args)
    elif cmd == "show_hostname":
	show(args)
    elif cmd == "init_hostname":
	_init()
    else:
	print args
    sys.exit(0)

def _check_hostname(hostname):
    "Returns 0 if the argument is a valid hostname."
    try:
        checkHostname(hostname)
    except InvalidHostnameException:
        return 1
    return 0

def checkHostname(name):
    if len(name) > 255:
        raise InvalidHostnameException(name, "name exceeds 255 characters long")

    simple_names = name.split('.')
    for simple_name in simple_names:
        checkSimpleHostname(simple_name)

simple_hostname_regex = None
def checkSimpleHostname(name):

    if len(name) > 63:
        raise InvalidHostnameException(name, "name exceeds 63 characters long")
    
    global simple_hostname_regex
    if simple_hostname_regex == None:
        # This makes sure the name is not empty string
        simple_hostname_regex = re.compile('^[a-zA-Z0-9\-_%]+$')

    match = simple_hostname_regex.match(name)
    if match is None:
        raise InvalidHostnameException(name,
                "only letter, digits, '-', '_' and '%' are allowed")
    if name[0] == '-' or name[-1] == '-':
        raise InvalidHostnameException(name,
                'name must start and end with a letter or a digit')

def _usage():
    print >>sys.stderr, "Usage: hostname [-s] | hostname <new hostname>"


if __name__ == '__main__':
	_main()
