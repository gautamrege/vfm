#!/usr/bin/python
#
# Copyright (c) 2008  VirtualPlane, Inc.

"""Configure IP addresses and routes"""

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

_ERROR_MESSAGE = 'Information not found'
_IP_ADRESS_LOCATION = '/etc/sysconfig/network-scripts/ifcfg-eth0'
_IP_ADDRESS = 'IPADDR'
_BROAD_ADDRESS = 'BROADCAST'
_HW_ADDRESS  = 'HWADDR'
_NET_ADDRESS = 'NETWORK'
_NET_MASK = 'NETMASK'
_GATEWAY  = 'GATEWAY'
_ETHERNET = 'DEVICE'

#def getIPaddressSpecList():

class IPCONFIG(object):
     """ This will create the object of the class"""
     def __init__(self):
         print 'New object '

def getEthernetFromSystem():
    try :
        lines = readConfigFile()
        if lines == "":
           print 'Empty String'
    except ValueError , err:
         sys.stdout.write("Value error : %s\n" % str(err))
         sys.exit(1)
    tokens = {}
    for line in lines:
        match = line.find(_ETHERNET)
        if match == 0:
            ethernet = line.split("=")
            return ethernet[1]

def getGateWayFromSystem():
    try :
        lines = readConfigFile()
        if lines == "":
           print 'Empty String'
    except ValueError , err:
         sys.stdout.write("Value error : %s\n" % str(err))
         sys.exit(1)
    tokens = {}
    for line in lines:
        match = line.find(_GATEWAY)
        if match == 0:
            gateway = line.split("=")
            return gateway[1]

def getNetworkMaskFromSystem():
    try :
        lines = readConfigFile()
        if lines == "":
           print 'Empty String'
    except ValueError , err:
         sys.stdout.write("Value error : %s\n" % str(err))
         sys.exit(1)
    tokens = {}
    for line in lines:
        match = line.find(_NET_MASK)
        if match == 0:
            network_mask = line.split("=")
            return network_mask[1]

def getNetworkAddressFromSystem():
    try :
        lines = readConfigFile()
        if lines == "":
           print 'Empty String'
    except ValueError , err:
         sys.stdout.write("Value error : %s\n" % str(err))
         sys.exit(1)
    tokens = {}
    for line in lines:
        match = line.find(_NET_ADDRESS)
        if match == 0:
            network = line.split("=")
            return network[1]


def getHWaddrFromSystem():
    try :
        lines = readConfigFile()
        if lines == "":
           print 'Empty String'
    except ValueError , err:
         sys.stdout.write("Value error : %s\n" % str(err))
         sys.exit(1)
    tokens = {}
    for line in lines:
        match = line.find(_HW_ADDRESS)
        if match == 0:
            hardware = line.split("=")
            return hardware[1]

def getBroadcastFromSystem():
    try :
        lines = readConfigFile()
        if lines == "":
           print 'Empty String'
    except ValueError , err:
         sys.stdout.write("Value error : %s\n" % str(err))
         sys.exit(1)
    tokens = {}
    for line in lines:
        match = line.find(_BROAD_ADDRESS)
        if match == 0:
            broadcast = line.split("=")
            return broadcast[1]
    

def getIPfromSystem():
    
    try :
        lines = readConfigFile()
    	if lines == "":
           print 'Empty String' 
    except ValueError , err:
         sys.stdout.write("Value error : %s\n" % str(err))
         sys.exit(1)
    tokens = {}
    for line in lines:
        match = line.find(_IP_ADDRESS)
        if match == 0:
            ip = line.split("=") 
            return ip[1]
   
    # It will come here if not able to find the ip.
    """
        ip_error = "IP not configured"
        return ip_error
    """

def readConfigFile():
    """ This read the config file on tht system"""
    try:
         fIpcfg = open(_IP_ADRESS_LOCATION , "rb" , 0)
    except IOError , err:
         sys.stdout.write("I/O error : %s\n" % str(err))
         sys.exit(1)
    lines = fIpcfg.read().split("\n")
    fIpcfg.close()
    return lines

def errorMessages():
    """ Here we can write the error messages"""
        

 
def main():
      ip =  getIPfromSystem()
      device = getEthernetFromSystem()
      broadcast = getGateWayFromSystem()
      hardware = getHWaddrFromSystem()
      networkMask = getNetworkMaskFromSystem()
      networkAddr = getNetworkAddressFromSystem()
      gateway = getGateWayFromSystem()
      if ip == None:
         raise NameError("INTERNAL ERROR: %s " % \
                  (_ERROR_MESSAGE))

if __name__ == '__main__':
    v = main()
    sys.exit(v)



