#!/usr/bin/python
#
# Copyright (c) 2008  VirtualPlane, Inc.

"""Show or clear ARP cache contents"""

import sys

path = 'src/vps/cli'
if path not in sys.path:
     sys.path.append(path)

import lib.output
import lib.process
import lib.essentials

from lib.errorhandler import *

_ARP = "/sbin/arp"

def _output_arp_list(outf, name, arp_list):
    FMT = "%-15s   %-17s   %s\n"
    if len(arp_list) != 0:
        outf.write(FMT % ("IP Address", "Ethernet Address", "Interface"))
    for (n, e) in arp_list:
        ipaddr = e.get('IPaddress', ["0.0.0.0"])
        ethaddr = e.get('HWaddress', ["00:00:00:00:00:00"])
        interface_name = e.get('InterfaceName', "--")
        outf.write(FMT % (ipaddr, ethaddr, interface_name))


_output_handlers = {
                        "ARPentryList" : _output_arp_list
                    }

#
# Possible invocations:
#    show arp
# Note that the 'show' keyword is *not* included in argv
#
def show(argv):
    """Show the contents of the ARP cache
        syntax: show arp
    """
    output = lib.output.CLIoutput("arp", _output_handlers)
    if len(argv) != 1:
        output.completeOutputError(InvalidArgumentCount (syntax = show.__doc__))
        return output

    arp_entries = open("/proc/net/arp", 'r').readlines()
    output.beginList("ARPentryList")
    for line in arp_entries[1:]:           # skip header
        fields = line.split()
        output.beginAssembling("ARPentry")
        output.setVirtualNameValue("IPaddress", fields[0])
        if int(fields[2], 16) == 0:
            output.setVirtualNameValue("HWaddress", "(unresolved)")
        else:
            output.setVirtualNameValue("HWaddress", fields[3])
        output.setVirtualNameValue("InterfaceName", fields[5])
        output.endAssembling("ARPentry")

    output.endList("ARPentryList")
    output.completeOutputSuccess()
    print output
    return output


#
# argv[0] == 'clear_arp'
# argv[1] == IP address (optional)
#
def clear(argv):
    """Invalidate an ARP entry or all ARP entries
        syntax: clear arp [<ip-address>]
    """
    return no(argv)


#
# argv[0] == 'no_arp' or 'clear_arp' if called through clear
# argv[1] == IP address (optional)
#
def no(argv):
    """Invalidate an ARP entry or all ARP entries
        syntax: no arp [<ip-address>]
    """
    if argv[0] == 'clear_arp':
        msg = clear.__doc__
    else:
        msg = no.__doc__

    output = lib.output.CLIoutput("arp")
    if len(argv) > 2:
        output.completeOutputError(InvalidArgumentCount (syntax = msg))
        return output

    if len(argv) == 1:
        arp_entries = open("/process/net/arp", 'r').readlines()
        for line in arp_entries[1:]:           # skip header
            cmd = (_ARP, "-d", line.split()[0])
            lib.process.execute_command(cmd)
        output.completeOutputSuccess()
        return output

    ipaddr = argv[1]
    try:
        lib.essentials.checkIp(ipaddr)
    except InvalidIpException, ex:
        output.completeOutputError(ex)
        return output

    cmd = (_ARP, "-d", ipaddr)
    (exitval, output, err, fullstatus) = lib.process.execute_command(cmd)
    if exitval != 0:
        output.completeOutputError("IP address %s not in ARP cache" % (ipaddr,))
    else:
        output.completeOutputSuccess()
    return output
