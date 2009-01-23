#!/usr/bin/python
#
# Copyright (c) 2008 VirtualPlane
#
#
#
import os
import cmd
import sys
import termios


path = 'src/vps/cli'
if path not in sys.path:
      sys.path.append(path)


import lib.output
import lib.constants
import vps_cli

class Display:
      ''' This class extracts the data out of the xml given to it'''
  
      def displayVPS(xml_output):
          if isinstance(xml_output, lib.output.XMLoutput): 
             data_output = xml_output.getOutput(vps_cli.get_vps_xml_mode())
             if len(data_output) < Display.__estimatedTotalSize():
                Display.__differentDisplayVPS(data_output, 'Small')
             else:
                Display.__differentDisplayVPS(data_output, 'Full')
          elif isinstance(xml_output, lib.output.CLIoutput):
               data_output = lib.XMLtoText.XMLtoText(xml_output)
           
	
      def __estimatedTotalSize():
          ''' This gets the estimated size of terminal in bin'''
          files_open = os.popen('/bin/stty size')
          output = files_open.read()
          files_open.close()
          try:
              temp_tokens = output.split()[0]
              number_of_rows = int(temp_tokens)
              return number_of_rows
          except:
              return lib.constants.TERMINAL_COUNTS

      def __differentDisplayVPS(data, Size):
          ''' Here Size determines whether we need small size
              display or full page display
          '''
          if Size == 'Small':
             for data_line in data:
                 print data_line
          elif Size == 'Full':
             os.environ['SHELL'] = '/bin/true'
             files_dir_open = os.popen('/bin/more', 'w')
             try:
                 for data_line in data:
                      print files_dir_open, data_line   
                 files_dir_open.close()
             except IOError:
                  pass
      displayVPS = staticmethod(displayVPS)
      __estimatedTotalSize = staticmethod(__estimatedTotalSize)
      __differentDisplayVPS = staticmethod(__differentDisplayVPS)

