#!/usr/bin/python2 -tt
#
# Copyright (c) 2008  VirtualPlane Systems, Inc.
#

import sys

try:
    import constants
except ImportError:
    pass

import escape

class UnknownInterfaceCheck(Exception):
    def __init__(self, name, reason):
        Exception.__init__(self)
        self.name = name
        self.reason = reason
    def __str__(self):
        return 'invalid interface "' + self.name + '": ' + self.reason


class UnknownIpCheck(Exception):
    def __init__(self, ip, reason):
        Exception.__init__(self)
        self.ip = ip
        self.reason = reason
    def __str__(self):
        return 'invalid IP address "' + self.ip + '": ' + self.reason

class UnknownNetmaskCheck(Exception):
    def __init__(self, netmask, reason):
        Exception.__init__(self)
        self.netmask = netmask
        self.reason = reason
    def __str__(self):
        return "invalid netmask '%s': %s" % (self.netmask, self.reason)

class InvalidArgumentCount(Exception):
    def __init__(self, pos=None, param=None, syntax=None, descape=None):
        Exception.__init__(self)
        self.pos = pos
        self.param = ''
        self.syntax = syntax
        self.descape = descape
        #self.__str__()

    def __str__(self):
        if self.pos is None:
            if self.descape:
                output = self.descape
            else:
                output = "Incorrect number of arguments"
        else:
            output = 'Bad parameter %s at position %s.' % \
                (self.param, self.pos)
            if self.descape:
                output += ' ' + self.descape
        
        #print self.getSyntax()
        return output

    def getSyntax(self):
        return self.syntax

    def getCode(self):
        return constants.GEN_SYNTAX_ERROR

class OutputError(Exception):
      def __init__(self, msg=None, syntax=None):
          Exception.__init__(self)
          self.msg = msg
          self.syntax = syntax
	  #self.__str__()

      def __str__(self):
          if self.msg is None:
                if self.syntax is None:
                     output = "Error Occured . Please type \"help\" for more information."
                else:
                     print self.syntax
          else:
                output = self.msg
	  print "\t",output
          return output

      def getMsg(self):
	  return self.msg

      def getSyntax(self):
	  return self.syntax

class InvalidNameException(Exception):
    def __init__(self, value, reason=None, object='object'):
        Exception.__init__(self)
        self.value = vps.escape.special_character(value)
        self.reason = reason
        self.object = object
    def __str__(self):
        rslt = "Cannot find %s '%s'" % (self.object, self.value)
        if self.reason is not None:
            return rslt + ": " + self.reason
        return rslt
    def getCode(self):
        return constants.GEN_UNKNOWN_OBJECT


class InactivityCheck(Exception):
    def __init__(self, reason):
        Exception.__init__(self)
        self.reason = reason
    def __str__(self):
        return 'not an active node: ' + self.reason
    def getCode(self):
        return constants.GEN_NOT_ACTIVE

class InteractiveCliRequired(Exception):
    def __init__(self, reason=None):
        Exception.__init__(self)
        self.reason = reason
    def __str__(self):
        msg = "The CLI must be used in interactive mode for this command."
        if self.reason is not None:
            msg += " %s" % (self.reason,)
        return msg

class StringUnspecial_characterException(Exception):
    def __init__(self, special_characterd_string, reason, last_token_end=None, pos=None):
        Exception.__init__(self, special_characterd_string, reason, pos, last_token_end)
        self.special_characterd_string = quoted_string
        self.pos = pos
        self.lastTokenEnd = last_token_end
        self.reason = reason
    def __str__(self):
        if self.pos is not None and self.lastTokenEnd is not None:
            diff = self.pos - self.lastTokenEnd
            remaining_text = self.special_characterd_string[self.lastTokenEnd:]
            return """Couldn't interpret %r, error at index %d: %s""" % (
                remaining_text, diff, self.reason)
        else:
            return """Couldn't interpret %r: %s""" % (
                self.special_characterd_string, self.reason)


class ModifierDoesntExistException(Exception):
    def __init__(self, modifier, command, module_docstring=None):
        Exception.__init__(self)
        self.modifier = modifier
        self.command = command
        self.moduleDocstring = module_docstring
    def __str__(self):
        msg = "'%s' command can't be invoked by the action %s" % (self.command,
                                                           self.modifier)
        if not self.moduleDocstring:
            return msg
        else:
            return msg + "\n" + self.moduleDocstring

