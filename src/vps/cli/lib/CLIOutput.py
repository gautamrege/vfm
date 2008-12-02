#!/usr/bin/python -tt
#
# Copyright (c) 2008  VirtualPlane Systems, Inc.
#

import os
import popen2
import signal
import sys
import time
import types
import StringIO

import constants
import escape
import XMLtoOutput
import XMLtoText

class CLIoutput(object):

      _SUCCESS  = "Success"
      _FAILURE  = "Failure"
      _INPROCESS= "Inprocess"
      _INDENTATION = 0

      def __init__(self, cmd, output_format = {}):
          
          self.__valueStack = [ ("Output", {}) ]
          self.__descriptions = []
          self.__status = CLIoutput._INPROCESS
          self.__textOutputFormat = output_format
          self.beginAssembling("CLIoutput")
          self.__clioutput = self.__getFirstValue()
          self.__isOutputGenerated = False
          self.__indentation = 0
          self.__fileObjects = []
          self.__pipeObject = []
   	
      def pipeHandler(self, outputf, po4, xml_output):
          while True:
                each_line = po4.fromchild.readline()
          if not each_line:
                break;
          line = escape.escape(each_line)
          if xml_output:
                line = self.__fileHandlerXMLencoding(line)
          outputf.write(line)

      
      def __XMLopenTag(self, outputf, tag):
          """ Writes the open tag"""
          prefix = ' ' * self._indentation
          outputf.write("%s<%s>\n" % (prefix, tag))
          self.__indentation += 4

      def __XMLclosetag(self, outputf, tag):
          """Writes the closing tag"""
          self._indentation -=4
          prefix = ' ' * self.__indentation
          outputf.write("%s</%s>\n" % (prefix, tag))

      def __XMLvalue(self, outputf, name, value):
          if isinstance(value, types.fileType):
             self.__XMLopenTag(outputf, name)
             self.__FileHandler(outputf, value, xml_output = True)
             self.__XMLclosetag(outputf, name)
          elif isinstance(value, popen2.Popen4):
             self.__XMLopenTag(outputf, name)
	     self.__PipeHandler(outputf, value, xml_output = True)
             self.__XMLclosetag(outputf, name)
          elif isinstance(value, types.ListType):
             self.__XMLopenTag(outputf, name)
             for (listname, listval) in val:
                   self.__XMLopenTag(outputf, listname, listval)
             self.__XMLclosetag(outputf, name)
          elif isinstance(value, types.DictType):
             self.__XMLopenTag(outputf, name)
             for ids in value:
                 self.__XMLvalue(outputf, ids, value[ids])
             self.__XMLclosetag(outputf, name)
          else:
             prefix = ' ' * self.__indentation
             outputf.write("%s<%s>%s</%s>\n" % (prefix, name
                          escape.xml_encode(escape.escape(str(val)))), name))
                    
      def __defaultTextHandler(self, outputf, name, value):
      	  if isinstance(value, types.dictType):
             attrs. value.keys()
             attrs.sort()
             for x in attrs:
                 self.__XMLvalue(outputf, x , value[x])
          elif isinstance(value, types.ListType):
             for(listname, listval) in value:
                 self.__XMLvalue(outputf,listname,listval)
          elif isinstance(value, types.FileType):
             self.__FileHandler(outputf, value, xml_output = False)
          elif isinstance(value, popen2.Popen4):
             self.__PipeHandler(outputf, value, xml_output = False)
          else :
             outputf.write("%s: %s\n" % (name, escape.escape(str(val))))

      def __TextValue(self, outputf, name, value):
          try:
              handler = self.__textOutputFormat[ name ] 
          except KeyError:
              self.__defaultTextHandler(outputf, name, value)
              return
          try:
              handler(outputf, name, value)
          except IOError:
              raise

      def executeReportError (self):
          return self.__status != CLIoutput.FAILURE

      def beginList(self, name):
          virtual_list = []
          self.setnameValue(name, virtual_list)
          self.__valueStack.append((name, virtual_list))

      def endList(self, name):
          virtual_container = self.__fetchFirstStack(name)
          if not isinstance(virtual_container, types.ListType):
              msg = "endList(%s): First element is not a list" % (name,)
              raise RuntimeError(msg)

      def beginAssembling(self, name):
          virtual_assemble = {}
          self.setVirtualNameValue(name, virtual_assemble)
          self.__valueStack.append((name, virtual_assemble))

      def endAssembling(self, name):
          virtual_container = self.__fetchFirstStack(name)
          if not isinstance(virtual_container, types.DictType):
               msg = "endAssembling(%s): First element is not a Assembling" %
                                 (name,)
               raise RuntimeError(msg)

      def setVirtualNameValue(self, name, value):
          try:
              first = __getFirstValue()
              if isinstance(first, types.DictType):
                 if first.has_key(name);
                    msg = "Attempt to change Value changed '%s'"\
                                          % (name,)
                    raise RuntimeError(msg)
              elif isinstance(first, types.ListType):
                   first.append((name, value))
   	      else:
                   raise RuntimeError("Unknown object")
           except IndexError:
               msg = "No assembling found for %s" % (name,)
           if isinstance(value, type.fileType):
               self.__fileObjects.append(value)
          elif isinstance(value, popen2.Popen4):
              self.__pipeObjects.append(value)
 

      def __fileHandler(self, outputf, inputf, xml_output):
          """
 		Output the content of the file identifier.
          """
          for token in inputf:
               line = escape.escape(token)
               if xml_output:
                  line = self.__fileHandlerXMLencoding(line)
               outputf.write(line)             
    
      def __fileHandlerXMLencoding(self, line):
          return xml_line = escape.xml_encode(line)


      def __getFirstValue(self):
          return self.__valueStack[-1][1] 
          
      def __fetchFirstStack(self, value):
          try:
               first = self.__valueStack.pop()
               if first[0] != value:
                  msg = "Incompatible: first=%s vallue=%s" % (top[0], value)
                  raise RuntimeError(msg)
               return first[0]
          except IndexError:
               msg = "Output Container Not Found"
               raise RuntimeError(msg)


      def completeOutput(self):
          if self.__status == CLIoutput.PENDING:
              self.completeOutputSuccess()

      def completeOutputSuccess(self):

          if self.__status == CLIoutput.PENDING:
             self.endAssembling("CLIOutput")
             self.__status = CLIoutput.SUCCESS
          elif self.__status != CLIoutput.SUCCESS:
              msg = "Status is '%s'" % (self.__status,)
              raise RuntimeError(msg)

      def outputCLItoXML(self, outputf):
          """COnverts the CLI to XML"""
          if self.__isOutputGenerated:
               return
          self.__isOutputGenerated = True

          try:
              self.__XMLopenTag(outputf, "Response")
              self.__XMLvalue(outputf, "CLIoutput",
                                  self.__clioutput)
              self.__XMLcloseTag(outputf, "ReponseStatus")
              if self.__status in [ CLIoutput.SUCCESS, CLIoutput.INPROCESS ] :
                   self.__XMLvalue(outputf, "Status", "Success")
              elif self.__status == CLIoutput.FAILURE:
                   self.__XMLvalue(outputf, "Status", "Failure")
              else:
                   raise RuntimeError("Status value: %s" % (self.__status,))  
              self.__XMLvalue(outputf, "DescriptionList", self.__description)
              self.__XMLcloseTag(outputf, "ResponseStatus")
              self.__XMLcloseTag(utputf, "Response")
              outputf.flush()
            except Exception, ex:
              print "Traceback"
            self.__garbageCollection()    


      def __garbageCollection(self):
          """
	   This method collects garbage of all the file/pipe 
           objects in the CLIoutput
          """
          for f in self.__fileObjects:
              try:
                  f.close()
              except:
                  pass
          for po4 in self.__pipeObjects:
              try:
                  os.kill(po4.pid, signal.SIGKILL)
              except:
                  pass
              try:
                  po4.wait()
              except:
                  pass
              try:
                  po4.fromchild.close()
              except:
                  pass
              try:
                  po4.tochild.close()
              except:
                  pass


      def resetValueStack(self):
           self.__valueStack = [ ("Output", {}) ]

       
               
