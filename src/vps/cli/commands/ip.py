#!/usr/bin/python
#
# Copyright (c) 2008  VirtualPlane, Inc.

"""Configure IP addresses and routes"""
import getopt
import sys
import re

path = 'src/vps/cli'
if path not in sys.path:
     sys.path.append(path)

import lib.output
import lib.process
import lib.essentials
import lib.ip_config
import lib.network_config

from lib.errorhandler import *

#
# Globals
#
_IP = '/sbin/ip'
_ROUTE = '/sbin/route'
_HA_RESOURCES_FILE = "/etc/ha.d/haresources"
_IFCONFIG = '/sbin/ifconfig'
ip_regex = None

#
# argv[0] is <ip-address> (optional)
# argv[1] is 'usage' (optional)
#

def _output_ipaddr_list(outf, name, ipaddr_list):
    if ipaddr_list:
        FMT = "%-15s\t%-15s\t%-12s\t%s\t%s\n"
        HEADERS = ("IP Address", "Netmask", "Interface", "Active", "Scope")
        outf.write(FMT % HEADERS)
        outf.write("%s\n" % (70 * '-'))
        for (n, ip) in ipaddr_list:
            outf.write(FMT % (ip['IPaddress'], ip['IPnetmask'],
                                ip['InterfaceName'], ip['Scope'],
                                ip['Active']))

def _output_iproute_list(outf, name, iproute_list):
    FMT = "%-15s\t%-15s\t%-15s\t%-15s\n"
    HEADERS = ("IP Address", "Netmask", "Interface", "Gateway")
    outf.write(FMT % HEADERS)
    outf.write("%s\n" % (70 * '-'))
    for (n, iproute) in iproute_list:
        outf.write(FMT % (iproute['Destination'], iproute['IPnetmask'],
                            iproute['InterfaceName'], iproute['Gateway']))


_output_handlers = {
                        "IPaddressSpecList" : _output_ipaddr_list,
                        "IProuteList"       : _output_iproute_list
                   }

#
# Possible invocations:
#        show ip address
#        show ip route
# Note that the 'show' keyword is *not* included in argv
#
def show(argv):
    """
        Show IP-related information
        syntax: show ip address|route

    """
  
    output = lib.output.CLIoutput("ip", _output_handlers)
    if len(argv) > 2:
        lib.errorhandler.InvalidArgumentCount(syntax=show.__doc__)
        return output

    if len(argv) == 1 :
	if 'address' in argv[0] or 'route' in argv[0]:
	     keyword = argv[0].split()
	     if keyword[1].startswith('address'):
		_show_ip_addresses(output, [])
	     elif keyword[1].startswith('route'):
		_show_ip_routes(output, [])
	else:
             output.completeOutputError(InvalidArgumentCount(syntax=show.__doc__)) 
	     return output
             #print show.__doc__
    else:
	 output.completeOutputError(InvalidArgumentCount(syntax=show.__doc__))
         #print show.__doc__
         return output 

    return output

def _show_ip_addresses(output, argv):
    """Show IP addresses
        syntax: show ip address [<ip-address> [usage]]
    """
    if len(argv) > 2:
        output.completeOutputError(InvalidArgumentCount(
            syntax=_show_ip_addresses.__doc__))
        return output

    addr = None
    if len(argv) >= 1:
        addr = argv[0]
        try:
            checkIp(addr)
        except UnknownIpCheck, ex:
            output.completeOutputError(InvalidArgumentCount(2, argv[0],
                _show_ip_addresses.__doc__, str(ex)))
            return output
        if len(argv) == 2:
            if (argv[1] == '' or not "usage".startswith(argv[1].lower())):
                output.completeOutputError(InvalidArgumentCount(
                    3, argv[1], _show_ip_addresses.__doc__))
                return output
    cmd = [_IP, 'addr']
    (exitval, output_result, err, fullstatus) = lib.process.execute_command(cmd)
    output.beginList("IPaddressSpecList")
    for ln in output_result[1:]:      
        fields = ln.split()
        if fields[-1].find('eth') == 0:
           ethernet = fields[-1]
           netmask = getSubNetMask(ethernet)
           addr = fields[1].split("/")
           ipAddress = addr[0]
           scope = fields[5]
           active = isActive(ethernet) 
           _ip_address_spec(output, ipAddress, netmask, ethernet, scope, active )
    output.endList("IPaddressSpecList")
       
    if len(argv) == 2:
        _ipaddr_usage(output, address_list, addr)
        return

    if ipAddress is None:
       output.completeOutputError("cannot find IP address ")
    else:
        output.completeOutputSuccess()

def isActive(interface):
    cmd = [_IFCONFIG, interface]
    (exitval, output, err, fullstatus) = lib.process.execute_command(cmd)
    to_return = 'DOWN'
    for ln in output[1:]:
        length = len(ln)
        if length > 1 :
            fields = ln.split()
            if fields[0].find('UP') == 0:
               return fields[0]
        else :
             return to_return
 
def getSubNetMask(interface):
    cmd = [_IFCONFIG, interface]
    (exitval, output, err, fullstatus) = lib.process.execute_command(cmd)
    for ln in output[1:]:  
        fields = ln.split()
        if fields[0].find('inet') == 0:
           if fields[3].find('Mask') == 0:
              token = fields[3].split(":")
              for mask in token :
               return token[1]

def _ip_address_spec(output, ipaddress, netmask, interface, scope, active ):
    """Output an IPaddressSpec in XML form"""
    output.beginAssembling("IPaddressSpec")
    output.setVirtualNameValue("IPaddress", ipaddress)
    output.setVirtualNameValue("IPnetmask", netmask)
    output.setVirtualNameValue("InterfaceName", interface)
    output.setVirtualNameValue("Active", scope)
    output.setVirtualNameValue("Scope", active)
    output.endAssembling("IPaddressSpec")

def _show_ip_routes(output, argv):
    """Show network routes
        syntax: show ip route
    """
    cmd = [_ROUTE, '-n']
    (exitval, output_result, err, fullstatus) = lib.process.execute_command(cmd)
    
    output.beginList("IProuteList")
    for ln in output_result[2:]:        
        fields = ln.split()
        if fields[-1] == "lo":
            continue
        output.beginAssembling("IProute")
        output.setVirtualNameValue("Destination", fields[0])
        output.setVirtualNameValue("IPnetmask", fields[2])
        output.setVirtualNameValue("Gateway", fields[1])
        output.setVirtualNameValue("InterfaceName", fields[-1])
        output.endAssembling("IProute")
    output.endList("IProuteList")


def checkNetmask(name):
    tokens = name.split('.')
    length = len(tokens)
    if length not in [1, 4]:
        raise UnknownNetmaskCheck(name, "invalid netmask form")

    if length == 1:
        try:
            value = int(tokens[0])
            if value not in range(0, 33):
                raise UnknownNetmaskCheck(name, "netmask must be 0 - 32")
        except ValueError:
            raise UnknownNetmaskCheck(name, "netmask must be an integer")
    else:
        zero_token = 0
        for token in tokens:
            try:
                value = int(token)
                if not zero_token:
                    if value in [0, 128, 192, 224, 240, 248, 252, 254, 255]:
                        if value != 255:
                            zero_token = 1
                    else:
                        raise UnknownNetmaskCheck(
                            name, "invalid netmask component %s" % token)
                else:
                    if value != 0:
                        raise UnknownNetmaskCheck(
                            name, "invalid netmask component %s" % token)
            except ValueError:
                raise UnknownNetmaskCheck(
                    name, "invalid netmask component %s" % token)

#
# loopback_ok and multicast_ok have the following values:
# - 0: not allowed
# - 1: allowed
# - 2: required
#
def checkIp(ip, loopback_ok = 0, multicast_ok = 0):
    """Check if a string is a valid IP address in dotted quad notation.
    Also check if the address is in certain excluded address groups (multicast,
    loopback, etc.)
    """
    
    global ip_regex
    if not ip_regex:
        ip_regex = re.compile('^(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3})$')
    match_obj = ip_regex.match(ip)
    if not match_obj:
        raise UnknownIpCheck(ip, 'must be in dotted decimal notation')

    for n in range(1,5):
        quad = int(match_obj.group(n))
        if quad > 255:
            raise UnknownIpCheck(ip, 'each quad must be in the ' + 'range 0-255')

        if n == 1:
            if quad >= 240:
                raise UnknownIpCheck(ip, 'class E addresses not allowed')
            if 224 <= quad < 240 and multicast_ok == 0:
                raise UnknownIpCheck(ip, 'multicast addresses not allowed')
            if (quad < 224 or quad >= 240) and multicast_ok == 2:
                raise UnknownIpCheck(ip, 'not a multicast address')
            if quad == 127 and loopback_ok == 0:
                raise UnknownIpCheck(ip, 'loopback addresses not allowed')
            if quad != 127 and loopback_ok == 2:
                raise UnknownIpCheck(ip, 'not a loopback address')


def checkInterface(name):
    "Checks if 'name' is a valid network interface"
    f = open("/proc/net/dev")
    lines = f.readlines()
    f.close()
    for l in lines[2:]:
        words = l.split()
        if words[0].split(':')[0] == name: return
    raise UnknownInterfaceCheck(name, "Interface Not Found")

#
# Support classes
#
class _AttachedAddress:
    """Each object maintains information on an IP address attached
    to this system. It is expected that all such IP addresses are
    also present in the configuration, but this is not required.
    """

    #
    # Data members:
    #   ipaddress:      obvious
    #   prefix:         if there is a configured entry for this
    #                   IP address, then the prefix is derived from the
    #                   netmask; otherwise, it is the value reported
    #                   by 'ip addr show'
    #   interface:      obvious
    #   spec:           this is the IPaddressSpec if the address is
    #                   configured or None if the address is not
    #                   configured
    #   active:         true if the address is presently assigned to some
    #                   interface on this system
    #
    def __init__(self, ipaddress, prefix, interface, spec, active):
        self.__ipaddress = ipaddress
        self.__prefix = prefix
        self.__interface = interface
        self.__spec = spec
        self.__active = active

    def getIPaddress(self):
        return self.__ipaddress

    def getIPnetmask(self):
        return lib.network_config.prefix2dotted(self.__prefix)

    def getConfiguredIPnetmask(self):
        return self.__spec.getIPnetmask()

    def getNetworkAddress(self):
        return lib.network_config.network_address(self.__ipaddress, self.__prefix)

    def getPrefix(self):
        return self.__prefix

    def getInterface(self):
        return self.__interface

    def isConfigured(self):
        return self.__spec != None

    def isActive(self):
        return self.__active

    def getSpec(self):
        return self.__spec

    def addr_to_u32(self, addr):
        l = addr.split('.')
        return (long(l[0]) << 24) + (int(l[1]) << 16) + (int(l[2]) << 8) + \
               int(l[3])

    def __cmp__(self, other):
        return cmp((self.__interface, self.addr_to_u32(self.__ipaddress)),
                   (other.__interface, self.addr_to_u32(other.__ipaddress)))

#
# argv[0] == 'edit_ip'
# argv[1] == 'address|route'
# argv[2] depends on argv[1]
#
def edit(argv):
    """
        syntax: [edit] ip [address <ip-address> <address-attr> |route default <route-attr>]
	
	address-attr    : [interface <interface>]
			  [netmask   <netmask>  ]
	
	route-attr      : [interface <interface>]
			  [gateway   <gateway-address>  ]
    """
    output_cli = lib.output.CLIoutput("ip")

    if 'address' in argv[0] or 'route' in argv[0]:
         keyword = argv[0].split()
         if keyword[1].startswith('address'):
                _edit_ip_address(output_cli, argv)
         elif keyword[1].startswith('route'):
                _edit_ip_route(output_cli, argv)
    else:
          output_cli.completeOutputError(InvalidArgumentCount(syntax = edit.__doc__))
    return output_cli

#
# argv[2] == 'interface'
# argv[3] == interface-name 
# argv[4] == ip-address
# argv[5] == 'netmask'
# argv[6] == netmask 
#

def _edit_ip_address(output, argv):
    """Edit IP-address-related information
        syntax: [edit] ip address <ip-address> interface <interface>
                netmask <netmask>
    """
    argc = len(argv)
    interface = None
 
    if argc < 6:
       output.completeOutputError(InvalidArgumentCount(syntax=_edit_ip_address.__doc__))
       return output


    if argv[2] == '' or not "interface".startswith(argv[2].lower()):
         if argv[4] == '' or not "netmask".startwith(argv[4].lower()):
           msg = 'Expected keywords "interface" and "netmask" '
           output.completeOutputError(InvalidArgumentCount(syntax=_edit_ip_address.__doc__))
           return output

    interface = argv[3]
    try:
        checkInterface(interface)
    except UnknownInterfaceCheck, ex:
        error_msg = ex
        output.completeOutputError(InvalidArgumentCount(descape = "Error: error_msg"))
        return output
 
    ip_addr   = argv[1]
    try:
        checkIp(ip_addr)
    except UnknownIpCheck, ex:
        error_msg = ex
        output.completeOutputError(InvalidArgumentCount(descape = "Error: error_msg"))
        return output
         
    
    netmask_addr   = argv[5]
    try:
        checkNetmask(netmask_addr)
    except UnknownNetmaskCheck, ex:
        error_msg = ex
        output.completeOutputError(InvalidArgumentCount(descape = "Error: error_msg"))
        return output
    
    #
    # These lines are for getting the default route.
    #
    net_config = lib.network_config.NetworkConfig()
    #
    # 1. Get the ip address existing on the interface
    # 2. Remove the ip address
    # 3. Add the ip address
    # 4. ifconfig 'interface' 'ip_addr' netmask 'netmask_addr' up
    #

    ip_list = net_config.getIPList()
    old_ip = net_config.getRequiredIp(ip_list, interface)
    
    if old_ip == ip_addr:
       raise NameError("Ip Already Exist . Please give another ip")
 
    if net_config.eraseIp(old_ip) == -1:
       raise NameError("Existing ip can't be deleted.Please try again")

    #
    # Now add the new default ip. 
    #
    retval = net_config.add_default_ip(ip_addr, netmask_addr, interface)
    if retval == 0:
        route_list.addDefaultIP(ip_addr, netmask_addr, interface)
        signal_ip_changed()
        net_config.save()
        output.completeOutputSuccess()
    else:
        output.completeOutputError("failed to edit the IP")
    return output

#
# argv[0] == 'default'
# argv[1] == 'gateway'
# argv[2] == ip-address
# argv[3] == 'interface'
# argv[4] == interface-name
#
def _edit_ip_route(output , argv):
    """Edit IP-route-related information
        syntax: [edit] ip route default gateway <gateway-address>
                          interface <interface>
    """
    argc = len(argv)
    interface = None

    if argc < 6:
       output.completeOutputError(InvalidArgumentCount(syntax = _edit_ip_route.__doc__))
       return output

    if argv[1] == '' or not "default".startswith(argv[1].lower()) \
           or argv[2] == '' or not "gateway".startswith(argv[2].lower()):
       msg = 'Expected initial keywords "default" and "gateway".'
       output.completeOutputError(InvalidArgumentCount(5, "ip-route",syntax=_edit_ip_route.__doc__, desc=msg))
       return output

    gateway = argv[3]
    if argc == 5:
        if argv[4] == '' or not "interface".startswith(argv[4].lower()):
            msg = 'Expected keyword "interface".'
	    output.completeOutputError(InvalidArgumentCount(7, "ip-route",
                _edit_ip_route.__doc__, msg))
            return output

        interface = argv[5]
        try:
            checkInterface(interface)
        except UnknownInterfaceCheck, ex:
            output.completeOutputError(InvalidArgumentCount(8, "ip-route",
		_edit_ip_route.__doc__, "Not a Valid Interface"))
            return output

    try:
        checkIp(gateway)
    except UnknownIpCheck, ex:
	output.completeOutputError(InvalidArgumentCount(6, "ip-route",
		_edit_ip_route.__doc__, "Not a valid Ip Address"))
        return output

    #
    # These lines are for getting the default route.
    net_config = lib.network_config.NetworkConfig()
    route_list = net_config.getIProuteList()
    route = net_config.getDefaultRoute(route_list)
   
    #route = _getDefaultRoute(route) 
    if route is None:
        output.completeOutputError(
            'no default route exists; use "add ip route"')
        return output
 
    #net_config.eraseIProute(route)

    #
    # Now add the new default route.  If a default route already exists
    # it will be replaced.
    #
    """
    retval = net_config.add_default_route(gateway, interface)
    if retval == 0:
        route_list.addDefaultIProute(gateway, interface)
        signal_route_changed()
        net_config.save()
        output.completeOutputSuccess()
    else:
        output.completeOutputError("failed to change default route")
    """
    return output


def _signal_route_changed():
    """This method is invoked when an IP route is changed on the system.

         the proper action is to invoke 'smcctl network reconfigure'
         This will in turn update all services that are interested
         in such events. Currently, the only one interested is click,
         and we invoke it directly.
    """
    # If click is not running at the moment the notification will fail with
    # IOError.  We trap and ignore this exception here to shield callers
    try:
        #We have to set up a way to signal the required services.
        print '_signal_route_changed'
    except:
        pass

def _getDefaultRoute():
    
    """ It will return the default output to the edit function
        Show network routes
        syntax: show ip route
    """
    cmd = [_ROUTE, '-n']
    (exitval, output, err, fullstatus) = lib.process.execute_command(cmd)

    for ln in output[2:]:        # skip first and second lines
        fields = ln.split()
        if fields[0] == "default":
            print fields
            return fields[2]
