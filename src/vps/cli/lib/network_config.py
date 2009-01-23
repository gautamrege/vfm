#!/usr/bin/python
#
# Copyright (c) 2008  VirtualPlane, Inc.

"""Configure hostname """

import getopt
import sys
import string

path = 'src/vps/cli'
if path not in sys.path:
     sys.path.append(path)

import lib.output
import lib.process
import lib.essentials
import lib.errorhandler

_KILLALL_CMD = "/usr/bin/killall"
SERVICE_EXE = "/sbin/service"
_IP = "/sbin/ip"
_ROUTE = '/sbin/route'
_IFCONFIG = '/sbin/ifconfig'

class NetworkConfig(object):
    
    def __init__(self):
        '''Initilization'''

    def setHostname(self, hostname):
        '''Set Hostname ''' 

    def save(self):
        '''Save the config file'''

    def notify_hostname_changed(self, network_config):
        # update login prompts
        lib.process.execute_command((_KILLALL_CMD, "-q", "mingetty"))
        # update syslog
        lib.process.execute_command((SERVICE_EXE, "syslog", "reload"))
        # Have to update SSL certificates as well

    def getIPList(self):
        ''' This is ip address list given to certain interace'''
        cmd = [_IP, 'addr']
        (exitval, output, err, fullstatus) = lib.process.execute_command(cmd)
        return output        
  
    def getRequiredIp(self, ip_list, interface):
        ''' This is the required IP'''
        for n in ip_list:
            tokens = n.split()
            if "inet" in tokens:
               if interface == tokens[len(tokens) -1 ]:
                  req_ip =  tokens[1].split('/')
                  return req_ip[0]
 
    def eraseIp(self, old_ip):
        ''' Erase old ip'''
        # erase old ip
        return 0
       
    def getIProuteList(self):
        '''This will return the list of te routes'''
        cmd = [_ROUTE, '-n']
        (exitval, output, err, fullstatus) = lib.process.execute_command(cmd)
        return output

    def getDefaultRoute(self, route_list):
        '''This will select the defult Ip out of the list'''
        for n in route_list:
            token_ip_route = n.split()
            if token_ip_route[0] == '0.0.0.0':
               return n
   
    def eraseIProute(self, route):
        '''Erase the IP from the route list'''
        print 'eraseIProute'
        cmd = [_IP, 'route', 'del', 'default' , 'via', route]
        if interface:
            cmd += ['dev', interface]
        print cmd
        (exitval, out, err, fullstatus) = lib.process.execute_command(cmd)
        return exitval

    def add_default_route(self, gateway, interface):
        """Set the default route, replacing the current default 
        route if one exists.Returns 0 on success."""
        cmd = [_IP, 'route', 'replace', 'default', 'via', gateway]
        print cmd
        if interface:
          cmd += ['dev', interface]
        (exitval, out, err, fullstatus) = lib.process.execute_command(cmd)
        return exitval
  
    def add_default_ip(self, Ipaddress, netmask, Interface):
        """ Set the Ip address of the machine"""
        cmd = [_IFCONFIG, Interface, Ipaddress, 'netmask', netmask]
        print cmd, Interface
        if Interface:
            (exitval, out, err, fullstatus) = lib.process.execute_command(cmd)
            print out
        return exitval


    def addDefaultIProute(gateway, interface):
        ''' Add default route'''

   
