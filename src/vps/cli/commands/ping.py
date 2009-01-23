#!/usr/bin/python
#
# Copyright (c) 2008  VirtualPlane, Inc.
#
"""Ping a host. Use "ping".
"""

import popen2
import socket

import lib.output

from lib.errorhandler import *

def _output_ping_list_verbose(outf, name, ping_list):
    print ping_list['PingOutput']


_output_handlers = {
                        "PingSpecList"  : _output_ping_list_verbose
                   }

def edit(argv):
    """Ping a host (or IP address)

        Syntax: ping <host> [count <count>]
    """

    output = lib.output.CLIoutput("ping")

    if len(argv) == 4 and not "count".startswith(argv[2].lower()):
	print 'Not here'
        output.completeOutputError(InvalidArgumentCount (syntax=edit.__doc__,
					    desc="Expecting 'count'", pos=2,
					    param=argv[2]))
        return output

    if len(argv) not in [2, 4]:
	print 'not even here'
        output.completeOutputError(InvalidArgumentCount (syntax=edit.__doc__))
        return output

    name = argv[1]
    count = "3"

    if len(argv) == 4:
	count = argv[3]

    try:
        cnt = int(count)
	if (cnt <= 0):
	    output.completeOutputError("count should be a number greater than 0")
	    return output
	if (cnt > 1024):
	    output.completeOutputError("count should not exceed 1024")
	    return output
    except ValueError:
        output.completeOutputError("count should be a number greater than 0")
        return output

    try:
        ipaddr = socket.gethostbyname(name)
    except socket.error, e:
        output.completeOutputError("cannot resolve '%s': %s" % (name, e))
        return output

    try:
        pipe = popen2.Popen4("/bin/ping -nb -c %d -w %d %s" % \
                                                (cnt, cnt+10, name),
                                                0)
	#output.beginList("PingSpecList")
        output.beginAssembling("Ping")
        output.setVirtualNameValue("PingOutput", pipe)
        output.endAssembling("Ping")
	#output.endList("PingSpecList")
        output.completeOutputSuccess()
    except OSError, e:
        output.completeOutputError("Internal error: ping failed: %s" % (e,))
       
    return output

