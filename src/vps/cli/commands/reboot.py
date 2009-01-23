#!/usr/bin/python
#
# @(#)reboot.py	
#
# Copyright (c) 2008 VirtualPlane, Inc.
# 
"""Reboot the Virtual Fabric Manager. Use "reboot".
"""

import os
import sys

path = 'src/vps/cli'
if path not in sys.path:
     sys.path.append(path)

import lib.output
import lib.essentials

from lib.errorhandler import *

def edit(argv):
    """Reboot the system. 
        syntax: reboot [now]
    """

    output = lib.output.CLIoutput("reboot")

    xml_out = 0

    if len(argv) == 2 and argv[1] == "now":
	xml_out = 1

    if (len(argv) == 2 and argv[1] != "now") or len(argv) > 2:
        output.completeOutputError(InvalidArgumentCount (syntax=edit.__doc__))
	return output

    if (len(argv) == 1):
        try:
            doit = lib.essentials.confirm(
	        "Are you sure that you want to reboot the system")
        except lib.errorhandler.InteractiveCliRequired, ex:
            output.completeOutputError(ex)
            return output

        if not doit:
            print "Reboot aborted."
            output.completeOutputSuccess()
            return output

    try:
        status = os.system('/sbin/telinit -t 45 6')
	if status == 0:
	    if not xml_out: 
		print "Rebooting Virtual Fabric Manager..."
	    output.completeOutputSuccess()
	    return output
	else:
	    output.completeOutputError("System reboot failed.")
	    return output
    except:
	pass

    output.completeOutputError("Exception calling system reboot.")
    return output

