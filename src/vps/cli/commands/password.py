#!/usr/bin/python
#
# Copyright (c) 2008  VirtualPlane, Inc.

"""Configure IP addresses and routes"""
import getopt
import sys
import re
import getpass

path = 'src/vps/cli'
if path not in sys.path:
     sys.path.append(path)

import lib.output
import lib.essentials
import lib.db.db

from lib.errorhandler import *

_password = 'vpsmlx'
_TABLE_NAME = 'User_Account'

# This is a function; a wrapper-less wrapper.
getCurrentUsername = getpass.getuser

def getPassword(username):
    """ This function gets the password from the database to match it"""
    query = "Select password from %s where user = '%s'" % (_TABLE_NAME, 'admin')
    #print query
    # Call execute function form db 
    database = lib.db.db.Database()
    cursor = database._execute(query)
    for row in cursor:
        password = row[0]
    return password     

def edit(argv):
    """
      syntax: [edit] password enable -p new_password 
              [edit] password login -p new_password 
    """
    output = lib.output.CLIoutput("password")
	
    if len(argv) != 4:
          print edit.__doc__
	  return output
    if argv[1] in ["enable", "login"] and argv[2] == "-p":
         if argv[1] == "enable":
	    _edit_password_enable(output,argv)
            return output
         else:
            _edit_password_login(output, argv)
            return output
    else:
	print edit.__doc__
	return output


def _edit_password_enable(output,argv):
    """
      syntax: [edit] password -u user -p new_password 
    """
    # Call database to change the password. 
    user = 'admin' 
    password = argv[3]
    query = 'Update %s set password = "%s" where user = "%s" ' % (_TABLE_NAME, password, user)
    #print query  
    database = lib.db.db.Database()
    cursor = database._execute(query)
    #print cursor.fetchall()
    return output


def _edit_password_login(output, argv):
    print 'Change Password in the login'        

#cmd = [_PASSWORD, 'addr']
#        (exitval, output_result, err, fullstatus) = lib.process.execute_command(cmd)

