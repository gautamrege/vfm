#!/usr/bin/python
#
# @(#)halt.py	
#
# Copyright (c) 2008 VirtualPlane, Inc.
# 
"""Halt the Virtual Fabric Manager. Use "halt".
"""

import sys
import os

path = '/usr/local/cli/src/vps/cli'
if path not in sys.path:
     sys.path.append(path)

import lib.output
import lib.essentials

from lib.errorhandler import *

def edit(argv):
    """Halt the system. 
        syntax: halt [now]
    """

    output = lib.output.CLIoutput("halt")

    xml_out = 0

    if len(argv) == 2 and argv[1] == "now":
	xml_out = 1

    if (len(argv) == 2 and argv[1] != "now") or len(argv) > 2:
        output.completeOutputError(InvalidArgumentCount ())
	return output

    if (len(argv) == 1):
        try:
            doit = lib.essentials.confirm(
                "Are you sure that you want to halt the system")
        except lib.errorhandler.InteractiveCliRequired, ex:
            output.completeOutputError(ex)
            return output
            
        if not doit:
	    print "\nHalt aborted."
            output.completeOutputSuccess()
	    return output

    try:
	#
	# Instruct init to allows 45secs for processes to terminate (before
        # SIGKILLing them) and switch to runlevel 0.
	#
        status = os.system('/sbin/telinit -t 45 0')
	if status == 0:
	    if not xml_out: 
		print "Halting Virtual Fabric Manager..."
	    output.completeOutputSuccess()
	    return output
	else:
	    output.completeOutputError("System halt failed.")
	    return output
    except:
	pass

    output.completeOutputError("Exception calling system halt.")
    return output

