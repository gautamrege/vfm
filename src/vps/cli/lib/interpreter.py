#!/usr/bin/python
#
# Copyright (c) 2008 VirtualPlane
#
#
#

import os
import cmd
import sys
import getpass
import getopt
import termios
import time

path = 'src/vps/cli'
if path not in sys.path:
      sys.path.append(path)

import lib.db.db
import lib.output
import lib.constants
import lib.errorhandler
import lib.display
import vps_cli
import command_ref

vps_trace = False
vps_execute = True
vps_current_module = ''


class Interpreter(cmd.Cmd):
      """
         The Cmd class provides a simple framework for writing 
         line-oriented command interpreters.The it provides the 
         content to the output class and which uses display class
         to display the content
      """

      def __init__(self, prompt = ">"):
          cmd.Cmd.__init__(self)
          self.prompt = prompt
          self.command_ends = ("EOF",)
    
      def __set_prompt(self,prompt="# "):
          cmd.Cmd.__init__(self)
          self.prompt = prompt
          self.ignored_commands = ("EOF",)

      def emptyline(self):
          ''' Called when no command is entered '''
          return 0

      def postcmd(self, stop, line):
          '''
             Hook method executed just after a command dispatch is finished
             line is the command line which was executed,           
             stop is a variable which indicates whether execution 
                   will be terminated after the call to postcmd();
          '''
          new_prompt = self.prompt.split()
          if(vps_cli.get_current_prompt() != new_prompt[0]):
              if vps_cli.get_current_prompt() == None:
                    vps_cli.set_current_prompt(new_prompt[0])
          if not stop:
              self.prompt = vps_cli.create_enable_config_prompt()
          return stop


       #
       # Methods called when the cmd module wants to auto-completion
       #
      def completenames(self, text, line, begidx, endidx):
          '''
            This is called to automatically complete the commands on the cli
          '''
          if not line:
             # The user didn't enter anything. Show the basics.
             output = command_ref.ALL_MODIFIERS + ['help', 'quit']
             output.sort()
             return output
          else:
             return self.completedefault(text, line, begidx, endidx)

      def completedefault(self, text, line, begidx, endidx):
          '''
            This shows the list in case , the auto complete is
            not able to complete the full command.
          '''
          completed_portion = line[:begidx]
          completed_tokens = vps_cli.cmd_tokenize(completed_portion,
                             cmd_line_complete=False)
          output = command_ref.cmdCompletion(completed_tokens, text)
          if not output:
             output = [text]
             output.sort()
          return output

      #
      # The methods are invoked when the modules are called 
      #
      def do_EOF(self, ln):
          '''The user entered Ctrl-D, for example.'''
          return -1

      def do_help(self, ln = None):
          '''
          This method is called when help is required.This invokes the command on 
          default.
          '''
          if not ln:
             ln = 'help'
          else:
             ln = 'help ' + ln
          return self.default(ln)
          
      def __invokeConfig(self, modifier, cmd, command_name, args):
          '''Calls the correct prompt function'''

          if vps_cli.get_current_prompt() == lib.constants._VFM_STAN \
              or vps_cli.get_current_prompt() == None:
              print NameError("'%s' command can't be invoked in standard mode. " % \
                          (args))
	      return

          #
          # FIXME: Should check if the modifier is "add" then 
          #       args should contain an unique <object>
          #        

          if modifier not in command_ref.COMMAND_MODIFIER:
             raise NameError("'%s' command can't be invoked by the action  %s" % \
                          (cmd, modifier))
          #else :
          #   print 'Call the appropriate module to handle the modifier'

          # This is done due to changes made in the design.
          # FIXME: Make some changes according to the final design.
          #

          if vps_cli.check_mode_change(cmd) != 0:
             raise NameError("Incorrect command to change mode \
                                          Usage : <mode> <object>")

	  #print modifier , cmd , command_name , args
          cmd = command_name
          args_len = len(args)
	 	
          if cmd == "":
             return -1

          if cmd != "":
             tokens = cmd.split()
 
             #if args_len == 0 and tokens[1] != "":
             #      vps_cli.cli_runner(tokens[1])

             if args_len > 0:
                if tokens[0] != 'configure':
                      print 'This is modifier ' ,args
                else :
                     if args_len < 3 :
                        command_extension = tokens[1]
			global vps_current_module
			vps_current_module = args[1]
                        vps_cli.cli_runner(command_extension)
                     elif args_len == 3:
                          if args[2] == 'configure':
                             print lib.errorhandler.InvalidArgumentCount ()
                          else:
                             print 'This is modifier ' ,args
                     elif args_len == 4 :
                          if args[2] == 'configure' and args[3] != "":
                                  command_extension = tokens[1] + ' ' +  args[1] + ' ' + args[3]
                                  vps_cli.cli_runner(command_extension)
                     elif args_len > 4:
                          print args
          return 0

      #def self.__invokeSet():
      #	  """ Set value """ 
		
      def __invokeExit(self, modifier, cmd, command_name, args):
          '''Return -1 to stop the CLI, 0 otherwise.'''
          if vps_cli.iterate_back_cmd() == -1:
             return -1

      def __invokeEcho(self, modifier, cmd, command_name, args):
          '''Return -1 to stop the CLI, 0 otherwise.'''
          if modifier != command_ref.EDIT_MODIFIER:
              raise NameError("'%s' command can't be invoked by the action  %s" % \
                              (cmd, modifier))
          print " ".join(args)
          return 0

      def __invokeDisable(self):
          '''This will bring the prompt back to standard mode'''
          vps_cli.set_current_prompt(lib.constants._VFM_STAN)
          new_prompt = lib.constants._VFM_STAN
          vps_cli.cli_runner('disable')

      def __invokeQuit(self, modifier, cmd, command_name, args):
          '''Return -1 to stop the CLI, 0 otherwise.'''
          if modifier != command_ref.EDIT_MODIFIER:
              raise NameError("'%s' command can't be invoked by the action %s" % \
                             (cmd, modifier))
          return -1

      def __invokeLoginExec(self):
          '''Here we check for the password before the user enters exec prompt'''
          incorrect_attempts = 0
          while(True):
             if incorrect_attempts > 2:
                break
             try :
               p = getpass.getpass('Enter password for privilege execution mode:')
               if lib.essentials.enterExecutiveMode(p) == 0:
                   return vps_cli.cli_runner('enable')
               else :
                  incorrect_attempts = incorrect_attempts + 1
                  print 'Permission denied, please try again.'
             except:
                  #print "\n"
                  break          

      def __setVariableValue(self, variable, value):

          if variable == 'trace':
              vps_cli.vps_trace = value
          elif variable == 'execute':
              vps_cli.vps_execute = value
          else:
              print >> sys.stderr, "bad variable:", variable

      def __showVariableValue(self, variable_name, variable_value):
          if variable_value:
              print "%s: ON" % (variable_name)
          else:
              print "%s: OFF" % (variable_name)

      def __displayVariableValues(self):
 
          self.__showVariableValue("vps_trace", vps_cli.vps_trace)
          self.__showVariableValue("vps_execute", vps_cli.vps_execute)

      def __invokeSet(self, modifier, cmd, command_name, args):
          '''
             This is a method which returns -1 to stop. 
             FIXME: This will be called when the terminal is 
                    on XX(config-terminal)# and the admin
                    wants to set any parameters of command
                    plugin.
          '''
          if modifier == command_ref.SHOW_MODIFIER:
              if len(args) == 0:
                  self.__displayVariableValues()
              else:
                 raise lib.errorhandler.InvalidArgumentCount ()
          elif modifier == command_ref.EDIT_MODIFIER:
              if len(args) == 1:
                  self.__setVariableValue(args[0], True)
              else:
                 raise lib.errorhandler.InvalidArgumentCount ()
          elif modifier == command_ref.NO_MODIFIER or \
                    modifier == command_ref.CLEAR_MODIFIER:
              if len(args) == 1:
                 self.__setVariableValue(args[0], False)
              else:
                 raise lib.errorhandler.InvalidArgumentCount ()
          return 0


      def __invokeExecuteCommand(self, modifier, cmd, command_name, args):
          '''
             This invokes the appropriate command to execute them
          '''
          #Spawn the command plugin if not yet spawned
          if not sys.modules.has_key(command_name):
             try :
                 __import__(command_name)
             except ImportError :
                raise NameError("No such command present: %s" %(command_name,))
        
          # In Else check if the command plugin is loaded or exist in the module.
          
          module = sys.modules[command_name]
          if not hasattr(module, modifier):
             #
             # If the command module is not found then
             # display the appropriate error.
             #
             try:
                 execute_string = "docstring = %s.__doc__"% (command_name,)
                 exec execute_string
             except :
                 docstring = None
	         raise NameError("No such command present: %s" %(modifier + ' ' + cmd,))
          func = getattr(module, modifier)
          orig_args = list(args)

          argv0 = "%s_%s" % (modifier, cmd)
          args.insert(0, argv0)
          if vps_cli.vps_execute:
             global INTERNAL_ERROR_MESSAGE
             try:
                 if func:
                     output = func(args) 
             except KeyboardInterrupt:
                 raise RuntimeError('execution was interrupted by the user')
             except lib.errorhandler.InteractiveCliRequired:
                 raise
             except Exception, ex:
                  command = ' '.join([modifier, cmd] + map(lib.escape.special_character, orig_args))
                  raise RuntimeError(lib.constants.INTERNAL_ERROR_MESSAGE + '\n\n' + command)


          if isinstance(output, lib.output.XMLoutput) or \
                     isinstance(output, lib.output.CLIoutput):
             return output
                       
      def _invokeModeShow(self,xml_output, current_mode , command, tokens):
          """ Here we will show the sub mode when we find the curent mode is executive"""
          # Call database and display the modes.
          token = current_mode.split("-")
          mode = token[1].split(")")
          database = lib.db.db.Database()
          if mode[0] in command_ref.NON_SUB_CMD_MOD:
		 command_name = "commands."+mode[0]
		 xml_output = self.__invokeExecuteCommand(command, mode[0], command_name,tokens)
		 return xml_output
          elif mode[0] not in command_ref.NON_SUB_CMD_MOD:
                 query = 'Select submode from mode_status where mode = ' + '"'+mode[0]+'"'
          alldata = database._execute(query)
   
      def default(self, command):
          """
            All the processing goes in here.
            This is the central point of the intrepreter.py 
          """
          xml_output = lib.output.XMLoutput()
	  
          try:
             # Tokenization of the command
             if not vps_cli.vps_tokenized:
                  tokens = vps_cli.cmd_tokenize(command, cmd_line_complete=True)
             else:
                tokens = []
                for arg in command:
                      tokens.append(lib.escape.unescape(arg))
             
             # configure terminal is handled here. 
             currentmode =  vps_cli.get_current_prompt()
	     for token in tokens:
		if token == 'terminal':
			modifier, cmd, command_name, tokens = \
                             command_ref.cmdToMode(command, tokens)
			command_name = "configure terminal"
                 	return self.__invokeConfig(modifier, cmd, command_name, tokens)
			
             # vadapter <vadapter-name> type commands handled here.
             if len(tokens) == 2 and tokens[0] in command_ref.NON_SUB_CMD_MOD:
                 modifier, cmd, command_name, tokens = \
                             command_ref.cmdToMode(command, tokens)
                 return self.__invokeConfig(modifier, cmd, command_name, tokens)


	     # vadapter [ So when length is 1 then is an error]
             if len(tokens) == 1 and tokens[0] in command_ref.NON_SUB_CMD_MOD:
		raise NameError( " \n '%s' Not a valid command. \n Please type : 'help %s' \n " % (tokens[0], tokens[0]))
		return

 	     #
             # So when len is 1 and the modifier is "show" [now] .Later add [edit] 
	     # in here.
	 
   	     if len(tokens) == 1 and command == "show" \
                           and "config" in currentmode :

                  modifier, cmd, command_name = command_ref.cmdToModifier(tokens, currentmode)
                  token = currentmode.split("-")
                  mode = token[1].split(")")
                  if mode[0] in command_ref.NON_SUB_CMD_MOD and command == "show":
                          tokens = []
			  tokens.insert(0,vps_current_module)
	     elif tokens[0] == "edit" and "config" in currentmode :
		  token = currentmode.split("-")
		  mode = token[1].split(")")
		  command_name = "commands."+mode[0] 
		  cmd = mode[0]
		  modifier = tokens[0]
		  index  = tokens.index('edit')
		  tokens.pop(index)
		  tokens.insert(0,vps_current_module)		
	     else:	 
             	  # Get the command from the command_ref. All other command handled here.
             	  modifier, cmd, command_name = \
                               command_ref.cmdToPlugin(tokens, currentmode)
	
             if command_name == 'exit':
                 return self.__invokeExit(modifier, cmd, command_name, tokens)
             elif command_name == 'set':
                 return self.__invokeSet(modifier, cmd, command_name, tokens)
             elif command_name == 'enable':
                 return self.__invokeLoginExec()
             elif command_name == 'end':
                 return self.__invokeDisable()

             # Invoke the actual command
	     #print "modifier", modifier
	     #print "cmd" , cmd
	     #print "command_name", command_name
     	     #print "tokens", tokens

             xml_output = self.__invokeExecuteCommand(modifier, cmd, command_name,tokens)
          except (lib.errorhandler.StringUnspecial_characterException,
                  command_ref.UnknownPluginException,
                  lib.errorhandler.ModifierDoesntExistException, NameError), ex:
                  xml_output.completeOutputError(ex)
          except (lib.errorhandler.InvalidArgumentCount ,
                  lib.errorhandler.InteractiveCliRequired,RuntimeError), ex:
                  xml_output.completeOutputError(ex)

          try:
               # Display the output through the display class
               if isinstance(xml_output, lib.output.CLIoutput):
                   if vps_cli.get_vps_xml_mode():
                         xml_output.outputCLItoXML(sys.stdout)
                   else:
                         os.environ['SHELL'] = '/bin/true'
                         os.environ['LESSSECURE'] = "1"
                         os.environ['LESS'] = "-X -d -F -PHit 'Q' to quit --> $" 
                         try:
                            pipe_line = os.popen("/usr/bin/less", "w", 0)
                            xml_output.outputText(pipe_line)
                            #lib.display.Display.displayVPS(xml_output)
                         finally:
                            pipe_line.close()
                           
               else:
                    lib.display.Display.displayVPS(xml_output)
               
          except Exception, ex:
               # On catching error while displying
               xml_output = lib.output.XMLoutput()
               xml_output.completeOutputError(ex)
               lib.display.Display.displayVPS(xml_output)

              # FIXME: One shot command details later   
              #
              #  Have to comeup with an an execution plan.
              #  
                
