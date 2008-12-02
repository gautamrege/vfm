#!/usr/bin/python2 -tt
#
# Copyright (c) 2008  VirtualPlane, Inc.
#

import errno
import getpass
import os
import string
import sys

from errorhandler import *

import commands.password

_password = 'vpsmlx'

def setNewPassword(new_password):
    """Set new password"""
    global _password
    _password = new_password

def getOldPassword():
    """This is to get old password"""
    return _password

def enterExecutiveMode(password):
    """
       Here we get the password and will check it with the existing password
       returns 0 if valid otherwise -1
    """
    # This is a function; a wrapper-less wrapper.
    getCurrentUsername = getpass.getuser
     
    old_password =  commands.password.getPassword(getCurrentUsername)
    if password == old_password:
       return 0
    else :
       return -1

# This is a function; a wrapper-less wrapper.
getCurrentUsername = getpass.getuser

vps_refuse_interactive = False

def isInteractive():
    if vps_refuse_interactive:
        return False
    try:
        return os.isatty(sys.stdin.fileno())
    except ValueError:  
        return False


def setNoninteractive():
    global vps_refuse_interactive
    vps_refuse_interactive = True


def dispatch_action(output, argv, actions, help):
    """
      output : Object from Output class 
      argv   : This is the requested command or plugin like "ip"
      action : This is the action to be performed
      help   : Incase of any error then print the help
    """

    if len(argv) < 2:
           output.completeOutputError(InvalidArgumentCount (syntax = help))
           return output


    let_go = check_condition(actions, argv)
    
    if len(let_go) == 1: 
       arguments = argv[2:]
       action = let_go[0]
       if len(action) == 2 or len(args) == action[2]:
             return action[1](output, args)
       else:
             output.completeOutputError(InvalidArgumentCount (syntax = 
                             action[1].__doc__))
             return output
    output.completeOutputError(InvalidArgumentCount (syntax = help))
    return output


def check_condition(actions, argv):
    ''' It will return True if successfull'''

    for each_action in actions:
        if each_action[0].startswith(argv[1].lower()):
           return each_action 

# Check if a parameter is a number object: isNumberType
def isNumberType(parameter):
        """Return 1 if the input parameter is an integer, long, or float."""

        return (type(parameter) in [IntType, LongType, FloatType])


# Check if a parameter is a string object: isStringType
def isStringType(parameter):
        """Return 1 if the input parameter is a string."""

        return (type(parameter) in StringTypes)


# Evaluate string: isNumber
def isNumber(s):
        """Returns true if string s is a number."""

        try: long(s)
        except: return 0
        return 1


# Evaluate string: isDigitString
def isDigitString(s):
        """Returns true if string s is comprised entirely of digits."""

        for char in s:
                if char not in string.digits: return 0
        return 1



def confirmation_required(asking, reply_list = [ "no", "yes"], \
                             if_interrupt = 0, case_sensitive = 0):
    ''' 
       This asks for the question when commands like reboot, 
       halt or change 
       of password etc are called for confirmation.
    ''' 
     
    prompt = "%s [%s]? " % (asking, "/".join(reply_list))
    if case_sensitive:
        reply = reply_list
    else:
        reply = [ ans.lower() for ans in reply_list ]
    while 1:
        try :
             input = raw_input(prompt)
             for values in range(0, len(reply)):
                  if case_sensitive:
                      reply = input
                  else:
                      reply = input.lower()
                  if reply == reply[values]:
                      return values
        except EOFError:
           print
        except KeyboardInterrupt:
           print
           if if_interrupt >= 0:
                  return if_interrupt
                 
