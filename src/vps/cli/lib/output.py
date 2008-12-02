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

path = 'src/vps/cli'
if path not in sys.path:
      sys.path.append(path)

import lib.constants
import lib.escape
import lib.XMLtoOutput
import lib.XMLtoText

_CLI_Paren       = "CLI"
_CLI_OUTPUT      = "CLIOutput"
_RESPONSE_STATUS = "ResponseStatus"
_DESCRIPTION     = "Description"

class XMLoutput:
    class XMLindentation:
        def __init__(self, indentation = 0):
            self.__indent = indentation
            self.__lines  = []

        def __output(self, line):
            self.__lines.append(line)


        def getLines(self):
            return self.__lines
 
        def printXML(self, name, value):
            if value is not None:
               value = lib.escape.escape(str(value))
            else:
               value = ""
            self.__output("%*s<%s>%s</%s>" % \
               (self.__indent, '', name, lib.escape.xml_encode(value), name))

        def printOpenParen(self, name):
            self.__output("%*s<%s>" % (self.__indent, '', name))
            self.changeIndent(3, True)
        
        def printCloseParen(self, name):
            self.changeIndent(3, False)
            self.__output("%*s</%s>" % (self.__indent, '', name))        

        def extend(self, lines):
            self.__lines.extend(lines)

        def changeIndent(self, indent_by, toIncrease = True):
             if toIncrease:
                  self.__indent = self.__indent + indent_by
             else :
                  self.__indent = self.__indent - indent_by



    def __init__(self, unformat_xml_lines=None):
        """This to set the unformated xml lines in XML output"""
        self.__errorlist = []
        self.__executeReportSuccess = True
        self.__ifConfirm = False
      
        if unformat_xml_lines is None:
            self.__completed = False
            self.__output = []
            self.__CLIParen            = self.setIndentation(2)
            self.__CLIoutputParen      = self.setIndentation(4)
            self.__responseStatusParen = self.setIndentation(4)
            self.__descriptionParen    = self.setIndentation(6)
            self.callPrintOpenParen()
        else: 
            line = self.splitting(line)
            #typeChecking(unformat_xml_lines, line)
            self.__completed = True
            if type(unformat_xml_lines) == types.ListType:
               self.__output = [ self.splitting(line) for line in \
                              unformat_xml_lines ]
            else:
               self.__output = [ self.splitting(unformat_xml_lines) ]



    def setIndentation(self, indent = 0):
        return  XMLoutput.XMLindentation(indent)

          
    def splitting(self, line):
         if line.endswith('\n'):
             return line[:-1]
         else:
             return line                          

    def callPrintOpenParen(self):
        self.__CLIParen.printOpenParen(_CLI_Paren)
        self.__CLIoutputParen.printOpenParen(_CLI_OUTPUT)
        self.__responseStatusParen.printOpenParen(_RESPONSE_STATUS)
        self.__descriptionParen.printOpenParen(_DESCRIPTION)
    
    def executeReportSuccess (self):
        return self.__executeReportSuccess

    def getOutput(self, xml_output = 1):
        if not self.__completed:
                   raise RuntimeError("XML not formated correctly")
        if (xml_output):
           return self.__output
        else:
           return lib.XMLtoText.XMLtoText(self.__output).getOutput()

    def completeOutputError(self, except_instance, argument_index = None):
        self.attachError(except_instance, argument_index)
        self.completeOutput()

    def completeOutput(self):
       
        # If the completed variable is true then return.
        if self.__completed:
              return 
       
        self.__output.append("<Response>")
        #self.__output.append("<CLI>")
        if self.executeReportSuccess():
              # Close the CLIOutput parenthesis
               self.__CLIoutputParen.printCloseParen(_CLI_OUTPUT)
               self.__output.extend(self.__CLIoutputParen.getLines())
        else:
           self.__output.append(" " * 4 + 
                             self.__CLIoutputParen.printCloseParen(_CLI_OUTPUT))            
        # Set the ResponseStatus
        if self.executeReportSuccess():
              self.__responseStatusParen.printXML("Status", "Success")
        else:
              self.__responseStatusParen.printXML("Status", "Failure")


        # Now we have to fill the desciption list.
        self.__descriptionParen.printCloseParen(_DESCRIPTION)
        self.__responseStatusParen.extend(
                           self.__descriptionParen.getLines())

        # Close reponseStatus
        self.__responseStatusParen.printCloseParen(_RESPONSE_STATUS)
        self.__output.extend(self.__responseStatusParen.getLines())

        # Close the Reponse
        self.__output.append("</Response>")
        self.__completed = True

    def hasError(self):
        return (len(self.__errorlist) != 0)
  
    def getErrorTexts(self):
        return self.__errorlist

    def attachInformation(self, msg, attachment_type, code_number):
        if self.__completed:
                 RuntimeError("XML not formated correctly")
        self.__descriptionParen.printOpenParen(_DESCRIPTION)
        self.__descriptionParen.printXML("Type", attachment_type)
        self.__descriptionParen.printXML("Code", code_number)
        self.__descriptionParen.printXML("Message", msg)
        self.__descriptionParen.printCloseParen(_DESCRIPTION)
        self.__ifConfirm = True

       
    def attachError(self, msg, attachment_index = None, code = -1):
        """ 
           This will attach the error to the xml
           Since this has to be descriptive so seperated from attachInformaiton.
        """    
        if self.__completed:
               print 'Here'
               RuntimeError("XML not formated correctly")
        self.__descriptionParen.printOpenParen(_DESCRIPTION)
        self.__descriptionParen.printXML("Type", "Error")
        if code < 0 :
               code = lib.constants.GEN_UNSPECIFIED
        self.__descriptionParen.printXML("Code", code)
        self.__descriptionParen.printXML("Message", msg)
        if attachment_index:
             self.__descriptionParen.printXML("Information", attachment_index)
        self.__descriptionParen.printCloseParen(_DESCRIPTION)

    def completeOutputSuccess(self):
        self.completeOutput()


class CLIoutput(object):

      _SUCCESS  = "Success"
      _FAILURE  = "Failure"
      _INPROCESS= "Inprocess"

      def __init__(self, cmd, output_format = {}):
        
          self.__valueStack = [ ("Output", {}) ]
          self.__description = []
          self.__status = CLIoutput._INPROCESS
          self.__textOutputFormat = output_format
          self.beginAssembling("CLIoutput")
          self.__clioutput = self.__getFirstValue()
          self.__isOutputGenerated = True
          self.__indentation = 0
          self.__fileObjects = []
          self.__pipeObject = []

      def completeOutputError(self, errorhandler):
          self.attachError(errorhandler)


      def attachError(self, errorhandler):
          if self.__status == CLIoutput._INPROCESS:
               print exception
               self.__status = CLIoutput._FAILURE
          elif self.__status != CLIoutput._FAILURE:
               msg = "Status is '%s'" % (self.__status,)
               raise RuntimeError(msg)
          desc = {}
          desc[ "Type" ] = "Error"
          desc[ "Message" ] = str(errorhandler)
          try:
              code = exception.getCode()
          except AttributeError:
              code = None
          if code is None:
              code = codes.GEN_UNSPECIFIED
          desc[ "Code" ] = code
          try:
              syntax = errorhandler.getSyntax()
              if syntax is not None:
                  desc[ "Syntax" ] = syntax
          except AttributeError:
              pass
          self.__description.append(("Description", desc))
    
           
      def pipeHandler(self, outputf, po4, xml_output):
          while True:
                each_line = po4.fromchild.readline()
                if not each_line:
                    break
                line = lib.escape.escape(each_line)
                if xml_output:
                    line = self.__fileHandlerXMLencoding(line)
                outputf.write(line)


      def __XMLopenTag(self, outputf, tag):
          """ Writes the open tag"""
          prefix = ' ' * self.__indentation
          outputf.write("%s<%s>\n" % (prefix, tag))
          self.__indentation += 4

      def __XMLclosetag(self, outputf, tag):
          """Writes the closing tag"""
          self.__indentation -=4
          prefix = ' ' * self.__indentation
          outputf.write("%s</%s>\n" % (prefix, tag))

      def __XMLvalue(self, outputf, name, value):
          if isinstance(value, types.FileType):
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
                   self.__XMLvalue(outputf, listname, listval)
             self.__XMLclosetag(outputf, name)
          elif isinstance(value, types.DictType):
             self.__XMLopenTag(outputf, name)
             for x in value:
                 self.__XMLvalue(outputf, x, value[x])
             self.__XMLclosetag(outputf, name)
          else:
             prefix = ' ' * self.__indentation
             outputf.write("%s<%s>%s</%s>\n" % (prefix, name, 
                 lib.escape.xml_encode(lib.escape.escape(str(value))), name))


      def __defaultTextHandler(self, outputf, name, value):
          if isinstance(value, types.DictType):
             attrs = value.keys()
             attrs.sort()
             for x in attrs:
                 self.__TextValue(outputf, x , value[x])
          elif isinstance(value, types.ListType):
             for(listname, listval) in value:
                 self.__TextValue(outputf,listname,listval)
          elif isinstance(value, types.FileType):
             self.__FileHandler(outputf, value, xml_output = False)
          elif isinstance(value, popen2.Popen4):
             self.__PipeHandler(outputf, value, xml_output = False)
          else :
             outputf.write("%s: %s\n" % (name, lib.escape.escape(str(value))))

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

      def outputText(self, outputf):
          if not self.__isOutputGenerated:
             return
          self.__isOutputGenerated = False 
          virt_outputf = lib.XMLtoText._ExternalizedOutput(outputf)
          try:
              if self.__status in [ CLIoutput._SUCCESS, CLIoutput._INPROCESS ]:
                   self.__TextValue(virt_outputf, "CLIoutput", self.__clioutput)
                   self.outputdescription(virt_outputf)
              elif self.__status == CLIoutput._FAILURE:
                   self.outputdescription(virt_outputf)
              else:
                   virt_outputf.write("Status: %s\n" % (self.__status,))
          except self.__status == CLIoutput._FAILURE:
                   self.outputdescription(virt_outputf)
          except _OutputException:
              pass
          except:
              exc = sys.exc_info()
          self.__garbageCollection


      def outputdescription(self, outputf):
          for ( x, desc) in self.__description:
              try:
                  outputf.write("%s \n" % (desc["Syntax"],))
                  break
              except:
                  pass
          for ( x, desc) in self.__description:
              print 'Inside description'
              print x, desc
              outputf.write("%s: %s\n" % (desc["Type"], desc["Message"]))
 

      def executeReportSuccess (self):
          return self.__status != CLIoutput.FAILURE

      def beginList(self, name):
          virtual_list = []
          self.setVirtualNameValue(name, virtual_list)
          self.__valueStack.append((name, virtual_list))

      def endList(self, name):
          virtual_container = self.__fetchFirstStack(name)
          if not isinstance(virtual_container, types.ListType):
              msg = "endList(%s): First element is not a list" % (name,)
              raise RuntimeError(msg)


      def beginAssembling(self, name):
          virtual_assemble = { }
          self.setVirtualNameValue(name, virtual_assemble)
          self.__valueStack.append((name, virtual_assemble))

      def endAssembling(self, name):
          virtual_container = self.__fetchFirstStack(name)
          if not isinstance(virtual_container, types.DictType):
               msg = "endAssembling(%s): First element is not a Assembling" % \
                                 (name,)
               raise RuntimeError(msg)

      def setVirtualNameValue(self, name, value):
          try:
              first = self.__getFirstValue()
              if isinstance(first, types.DictType):
                 if first.has_key(name):
                    msg = "Attempt to change Value changed '%s'"\
                                          % (name,)
                    raise RuntimeError(msg)
                 first[ name ] = value
              elif isinstance(first, types.ListType):
                   first.append((name, value))
              else:
                   raise RuntimeError("Unknown object")
          except IndexError:
               msg = "No assembling found for %s" % (name,)
               raise RuntimeError(msg)
          if isinstance(value, types.FileType):
               self.__fileObjects.append(value)
          elif isinstance(value, popen2.Popen4):
               self.__pipeObjects.append(value)


      def __fileHandler(self, outputf, inputf, xml_output):
          """
                Output the content of the file identifier.
          """
          for token in inputf:
               line = lib.escape.escape(token)
               if xml_output:
                  line = self.__fileHandlerXMLencoding(line)
               outputf.write(line)

      def __fileHandlerXMLencoding(self, line):
          xml_line = lib.escape.xml_encode(line)
          return xml_line

      def __getFirstValue(self):
          return self.__valueStack[-1][1]

      def __fetchFirstStack(self, value):
          try:
               first = self.__valueStack.pop()
               if first[0] != value:
                  msg = "Incompatible: first=%s vallue=%s" % (top[0], value)
                  raise RuntimeError(msg)
               return first[1]
          except IndexError:
               msg = "Output Container Not Found"
               raise RuntimeError(msg)


      def completeOutput(self):
          if self.__status == CLIoutput.PENDING:
              self.completeOutputSuccess()

      def completeOutputSuccess(self):
           
          if self.__status == CLIoutput._INPROCESS:
             self.endAssembling("CLIoutput")
             self.__status = CLIoutput._SUCCESS
          elif self.__status != CLIoutput._SUCCESS:
              msg = "Status is '%s'" % (self.__status,)
              raise RuntimeError(msg)


      def outputCLItoXML(self, outputf):
          """COnverts the CLI to XML"""
          if not self.__isOutputGenerated:
               return
          self.__isOutputGenerated = False

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



class _XMLparseException(Exception):
      pass


class _OutputException(IOError):
      pass


