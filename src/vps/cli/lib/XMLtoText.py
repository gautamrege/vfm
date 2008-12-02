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
import lib.output

#
# XMLtoText will convert the XML object to plaintext output.
#
#

class XMLtoText:
       def __init__(self, outputStream):
           self.input_stream  = StringIO.StringIO()
           self.output_stream = StringIO.StringIO()
           self.Description  = [{}]
           self.Status       = 'Failure'
           self.CLIOutput    = {}
           self.indentation  = 0 
           try:
               self.__convertStream(outputStream)
               self.__parser()
           except:
               if (self.Status == 'Failure'):
                       self.Description = [ { 'Type':['Error'],
                                            'Code':[0],
                                             'Message':['Response : ' + \
                                                   'Command Not Found \n Please type help for more information'], \
                                                    'Information':[1]} ]

       #
       # Convert the Stream into StringIo object from string, XMLoutput
       #
       def __convertStream(self, outputStream):
           if isinstance(outputStream, lib.output.XMLoutput):
                lines = outputStream.getOutput()
                for line in lines:
                       print >> self.input_stream, line
           elif isinstance (outputStream, str):
                self.input_stream = StringIO.StringIO(outputStream)
           elif isinstance(outputStream, lib.output.CLIoutput):
                lines = outputStream.getOutput()
                for line in lines:
                    print >> self.input_stream, line
           else:
                self.input_stream = outputStream

       #
       # parser parse the input XML object to the Output
       #
       def __parser(self):
           temp = self.input_stream.getvalue()
           self.input_stream.seek(0)
           Output = lib.XMLtoOutput.make_dict(self.input_stream, 'Output', try_unescape =True)
           ReponseStatus = Output['ReponseStatus'][0]
           self.Status   = ReponseStatus['Status'][0]
           Descriptionlist   = ReponseStatus['Description'][0]
           try:
               self.Description = Descriptionlist['Description']
           except:
               pass
          
           if (self.Status == 'Success'):
                   self.CLIOutput = Output['CLIOutput'][0]

       #
       # Return the Output as strings
       #
       #
       def getOutput(self):
           self.__OutputPlainText()
           self.output_stream.seek(0)
           return self.output_stream.getvalue().splitlines()

       #
       #  Return the XMl output in text.
       #
       def __OutputPlainText(self):
           if len(self.Description[0]) > 0:
                 self.__printDescription()
           if (self.Status == 'Success'):
                 self.__printOutput()

       #
       #  Display any message from Output to plain text.
       #
       #
       def __printDescription(self):
           for description in self.Description:
                type = description['Type'][0]
                msg  = description['Message'][0]
                print >> self.output_stream, type + ":" + msg


       #
       # Process compound XML elements
       #
       #
       def __executeDictionary(self, orig_tag, orig_value):
           self.indent = indent + 2
           print >> self.output_stream, "%s*s%s:" % (self.indent, '', orig_tag)
           self.indent = indent+2
           for tag in orig_tag.iterkeys():
               items = orig_values[tag]
               for item in items:
                        self.__executeTag(tag, item)
           self.indent = indent - 2
           print >> self.output_stream, ''
           self.indent = indent - 2

       #
       # Execute XML element
       #
       #
       def __executeItem(self, tag, item):
           print >> self.output_stream, \
                      "%*s%s: %s" % (self.indent, '', tag, item)

       #
       # execute XML element
       #
       def __executeTag(self, tag, item):
           if item == '':
              return
           if isinstance(item, dict):
              self.__executeDictionary(tag, item)
           else:
              self.__executeitem(tag, item)

       #
       #
       # 
       #
       #
       def __printOutput(self):
           #
           # Check for the ResponseOutput. 
           # 
           if self.CLIOutput == '':
              return
              for tag in self.CLIOutput.iterkeys():
                  items = self.CLIOutput[tag]
                  for item in items:
                      if globals().has_key(tag):
                         function = globals()[tag]
                         if callable(function):
                            function(self.output_stream, item)
                            continue
                      self.__executeTag(tag, item) 


class _ExternalizedOutput(object):
    def __init__(self, outf):
        self.__outputFile = outf

    def write(self, s):
        try:
            self.__outputFile.write(lib.escape.escape(s))
        except IOError:
            raise _OutputException()

    def flush(self):
        try:
            self.__outputFile.flush()
        except IOError:
            raise _OutputException()


class _OutputException(IOError):
    """We raise this errorhandler if we are encounter an IOError
    when writing to a file
    """
    pass


