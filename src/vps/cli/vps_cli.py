#!/usr/bin/python
#
# Copyright (c) 2008 VirtualPlane
#

import cmd
import getopt
import os
import sys
import termios
import time
import getpass

path = '/cli/src/vps/cli'
if path not in sys.path:
      sys.path.append(path)

import commands
import command_ref
import lib.output
import lib.escape
import lib.errorhandler
import lib.process
import lib.essentials
import lib.constants
import lib.interpreter
import lib.display
 
vps_initial_terminal_settings = None
vps_enable_exec_mode = False
vps_enable_config_mode = False
vps_current_prompt = lib.constants._VFM_STAN 

_SUB_MODE_GW = command_ref.MOD_GATEWAY
_SUB_MODE_BRIDGE = command_ref.MOD_BRIDGE
_SUB_MODE_VADAPTER = command_ref.MOD_VADAPTER


def _step_back_prompt(token):
    '''This will bring the prompt in the command mode shell
        for example :
        VFM(config-gw001)#end
        VFM(config-gateway)#end
        VFM#
    '''
    if token == _SUB_MODE_GW:
       return 'gateway'
    elif token == _SUB_MODE_BRIDGE:
       return 'bridge'
    elif token == _SUB_MODE_VADAPTER:
       return 'vadapter'
    else :
       return lib.constants._VFM_EXEC


def iterate_back_cmd():
    '''This will iterate back the command prompt'''
    current_prompt_temp = get_current_prompt()
    new_prompt = ""

    if 'config' not in current_prompt_temp :
        if 'setup' not in current_prompt_temp :
            if get_current_prompt() == lib.constants._VFM_STAN:
               return -1
        else:
            set_current_prompt(lib.constants._VFM_EXEC)
            new_prompt = lib.constants._VFM_EXEC
    else :
        tokens = current_prompt_temp.split('-')
        token_len = len(tokens)

        if token_len == 1:
            raise NameError("Incorrect Command Mode")

        if token_len == 2 :
           tokens_temp = tokens[1].split(')')
           if tokens_temp[0] in command_ref.NON_SUB_CMD_MOD:
              set_current_prompt(lib.constants._VFM_EXEC)
           else :
              temporary_prompt = _step_back_prompt(tokens_temp[0])
              new_prompt = tokens[0]+'-'+temporary_prompt+')#'
              set_current_prompt(new_prompt)
        elif token_len > 2 :
           for i in range(len(tokens) - 1):
               new_prompt = new_prompt+tokens[i]
               if i < len(tokens) - 2:
                   new_prompt = new_prompt+'-'
           new_prompt = new_prompt + ')#'
           set_current_prompt(new_prompt)
    return "%s " % (new_prompt,)


def restore_terminal_settings():
   global vps_initial_terminal_settings
   if vps_initial_terminal_settings != None:
       try:
           termios.tcsetattr(sys.stdin.fileno(), termios.TCSANOW,
                              vps_initial_terminal_settings)
       except termios.error:
           pass

def check_mode_change(cmd):
   '''This checks the commands through which we can change the modes'''
   current_prompt = get_current_prompt()
   current_command = cmd

   current_command_tokens = current_command.split()
   current_command_tokens_len = len(current_command_tokens)

   if current_prompt == lib.constants._VFM_EXEC:
      if current_command_tokens[0] in command_ref.SUB_CMD_MOD:
          return -1

   elif 'config' in current_prompt :
      if '-' in current_prompt :
          current_prompt_token = current_prompt.split('-')
          current_prompt_token_mode = current_prompt_token[1].split(')')
          if current_prompt_token_mode[0] in command_ref.SUB_CMD_MOD \
             and current_command_tokens[0] in command_ref.SUB_CMD_MOD :
                 return -1

          elif current_prompt_token_mode[0] in command_ref.NON_SUB_CMD_MOD \
               and current_command_tokens[0] in command_ref.SUB_CMD_MOD :

               #Selct the correct mode to look out sub mode
               if current_prompt_token_mode[0] == 'gateway':
                  if current_command_tokens[0] not in _SUB_MODE_GW :
                     return -1

               elif current_prompt_token_mode[0] == 'bridge':
                    if current_command_tokens[0] not in _SUB_MODE_BRIDGE :
                       return -1

               elif current_prompt_token_mode[0] == 'vadapter':
                    if current_command_tokens[0] not in _SUB_MODE_VADAPTER:
                       return -1

      else :
          if current_command_tokens[0] in command_ref.SUB_CMD_MOD :
             return -1

   return 0


vps_pid = os.getpid()
vps_oneshot = False
vps_xml_mode = False 
vps_oneshot = False
vps_tokenized = False 
vps_trace = False
vps_execute = True

def welcome_prompt_note():
    '''Prompt welcome note'''
    return lib.constants.VPS_PRODUCT + ' Version ' + lib.constants.SOFTWARE_VERSION

def create_cli_prompt():
    """Creates the prompt used by vps interactive CLI sessions"""
    set_current_prompt(lib.constants._VFM_STAN)
    return "%s " % (lib.constants._VFM_STAN,)

def create_enable_EXEC_prompt():
    """ Creates the prompt From user EXEC mode,enter the enable EXEC command."""
    return "%s " % (lib.constants._VFM_EXEC,)


def check_correct_sub_cmd_mode(prompt, prompt_extension):
    '''This checks for the sub_command_prompt compatibility'''
    new_prompt_extension = ""
    len_prompt_tokens = 0
    prompt_tokens = prompt.split('-')
    len_prompt_tokens = len(prompt_tokens)

    if len(prompt_extension) > 1:
       for prompt_temp in prompt_extension:
           new_prompt_extension = new_prompt_extension+ '-' + prompt_temp
    else :
       new_prompt_extension  = new_prompt_extension + '-' + prompt_extension[0]
    if len_prompt_tokens == 1:
       new_prompt = prompt + '-' + prompt_extension[0] + ')#'
       return new_prompt
    if len_prompt_tokens < 3 :
       if prompt_tokens[1] in command_ref.COMMAND_MODIFIER_NON_CO_EXISTS and \
          prompt_extension[0] in command_ref.COMMAND_MODIFIER_NON_CO_EXISTS :
          #
          #
          # Replace the existing prompt with new only 
          # if they are in command mode.
          # Else if they are in sub command mode then return saying
          # Cant change from one sub mode to another.
          #
          #
          new_prompt = prompt_tokens[0]+new_prompt_extension+')#'
          return new_prompt
       else :
          new_prompt = prompt+new_prompt_extension+')#'
          return new_prompt
    elif len_prompt_tokens > 2 :
       if prompt_tokens[1] in command_ref.COMMAND_MODIFIER_NON_CO_EXISTS :

          if len(prompt_extension) == 1:
             if prompt_extension[0] in command_ref.COMMAND_MODIFIER_NON_CO_EXISTS :
                raise NameError("Incorrect Sub Command Mode: %s" % \
                               (prompt_extension[0],))
             else:
                new_prompt = prompt+new_prompt_extension+')#'
                return new_prompt
          else:
             for prompt_ext_list in prompt_extension :
                if prompt_ext_list in command_ref.COMMAND_MODIFIER_NON_CO_EXISTS :
                   raise NameError("Incorrect Sub Command Mode: %s" % \
                                  (prompt_extension[0],))
             new_prompt = prompt+new_prompt_extension+')#'
             return new_prompt
       else :
            print 'Is an Error for config-mode'



def create_enable_config_prompt(prompt_ext = ""):
    """ Creates the prompt From user EXEC mode,enter the enable EXEC command."""
    temp_current_prompt = ""
    prompt_tokens = ""
    
    if '>' not in get_current_prompt():
        if '#' not in get_current_prompt():
            raise NameError("Incorrect Prompt")

    prompt_tokens_count = prompt_ext.split(' ')
    prompt_len = len(prompt_tokens_count)

    temp_current_prompt = get_current_prompt().split('#')
    new_prompt = temp_current_prompt[0]

    if prompt_ext == "":
       return "%s " % (get_current_prompt(),)
    if 'config' in temp_current_prompt[0]:
       #
       #
       # I am trying to change the mode of the command
       # for example 
       # VFM(config-gateway)#  
       # VFM(config-gateway)# configure v | vadapter
       # VFM(config-vadapter)# 
       #
       #
       position = temp_current_prompt[0].split(')')
       new_prompt = check_correct_sub_cmd_mode(position[0],prompt_tokens_count)
    else :
       new_prompt  = new_prompt + '(config'
       for prompt in prompt_tokens_count:
           prompt_tokens = prompt_tokens + '-' + prompt
       if 'terminal' not in prompt_tokens :
           new_prompt  = new_prompt + prompt_tokens + ')#'
       else :
           new_prompt   = new_prompt + ')#'
    set_current_prompt(new_prompt)
    return "%s " % (new_prompt,)

def create_enable_interface_prompt():
    """ Creates the prompt From user EXEC mode,enter the enable EXEC command."""
    return "%s# " % ('Router(config-if)',)

def set_current_prompt(prompt = lib.constants._CLI_PROMPT_):
    '''Here we will set the current prompt after it read '''
    global vps_current_prompt
    vps_current_prompt = prompt

def get_current_prompt():
    '''This will return the current prompt for taking the command'''
    return vps_current_prompt

def set_latest_command(command = ""):
    '''This stores the latest command given by the user'''
    global vps_latest_command
    vps_latest_command = command

def get_latest_command():
    '''This will return the latest command know if "" then no command'''
    global vps_latest_command
    return vps_latest_command

def get_current_time():
    """Prints time, eg: 2008-09-08 15:10:08"""
    time_fmt = "%Y-%m-%d %H:%M:%S"
    return time.strftime(time_fmt, time.localtime())

def get_session_identifier():
    """prints a line for every log line"""
    return "%s [%d] [%s:%s]" % (get_current_time(),vps_pid ,
                                vps_username, vps_remote_addr)

def handle_traceback():
    pass

def cmd_tokenize(cmd_line, cmd_line_complete=True):
    """
       Here we will tokenize the commands into tokens.
    """
    tokens = lib.escape.tokenize(cmd_line, cmd_line_complete)
    if vps_trace:
        print "command line args: ", repr(in_line), "-->", tokens

    return tokens


def cli_runner(token = None, first_time = False ,command = None):
    '''
       This will run the CLI by prompting Interpreter
    '''
    interp_kernel = lib.interpreter.Interpreter()

    if token == None:
        interp_kernel = lib.interpreter.Interpreter(create_cli_prompt())
        if not first_time:
           print welcome_prompt_note()
           while 1:
                try:
                    interp_kernel.cmdloop()
                    break
                except KeyboardInterrupt:
                    print

    if token == 'disable':
       interp_kernel = lib.interpreter.Interpreter(create_cli_prompt())

    if token == 'enable':
       ''' enable mode'''
       global vps_enable_exec_mode
       vps_enable_exec_mode = True
       set_current_prompt(lib.constants._VFM_EXEC)
       #print get_current_prompt()
       interp_kernel = lib.interpreter.Interpreter(
                           create_enable_EXEC_prompt())

    if token != None and token != 'enable' and token != 'disable':
       '''Config Mode'''
       global vps_enable_config_mode
       vps_enable_config_mode = True
       tokens_after_split = token.split(' ')
       for token_check in tokens_after_split:
          if token_check not in command_ref.COMMAND_MODIFIER_NON_CO_EXISTS:
              raise NameError("Unknown Command Module : '%s'" % (token_check))

       length = len(tokens_after_split)
       #This is the sub-level i.e 3
       if length > 3:
              raise NameError("Limit to sub interfaces is restricted to 3")
       interp_kernel = lib.interpreter.Interpreter(create_enable_config_prompt(token))


def main(argv):
    '''
    Usage:
        cli [-dxXnH] [<cmd>]

    Options:
            -db          Enable debugger (pdb). Default off.
            -X          Enable XML output. Default human readable text.
            -e          Disable execution. Default on.
            -l          Disable old history logging. Default on.
            -c "<cmd>"  Run the <cmd> and exit. The <cmd> should be special_characterd.
    '''
    # After parsing the arguments we will call the cli_runner.

    global vps_oneshot
    global vps_enable_debugger
    global vps_execute
    global vps_username
    global vps_xml_mode

    try:
        opts, args = getopt.getopt(argv[1:], 'c:dnHxX')
    except Exception, e:
        print >>sys.stderr, "error parsing options: %s" % (str(e))
        return 3

    log_history = True
    command = []
    for o in opts:
        if o[0] == '-c':
            lib.essentials.setNoninteractive()
            vps_oneshot = True
            command = o[1]
        if o[0] == '-db':
            vps_enable_debugger = True
        if o[0] == '-e':
            vps_execute = False
        if o[0] == '-l':
            log_history = False
        if o[0] == '-X':
            vps_xml_mode = True

    try:
        # Line-buffering
        vps_traceback_file = open("/tmp/traceback", 'a',
                                buffering=1)
        if log_history:
            vps_history_file = vps_traceback_file
    except (IOError, OSError):
        pass


    vps_username = lib.essentials.getCurrentUsername()

    # Determine remote address, if possible:
    if 'REMOTE_ADDR' in os.environ:
        # This is set by the Apache server when the WebUI calls us
        vps_remote_addr = os.environ['REMOTE_ADDR'] + ":web"
    elif 'SSH_CLIENT' in os.environ:
        # This is set by sshd when we are in an SSH session
        vps_remote_addr = os.environ['SSH_CLIENT'].split()[0] + ":ssh"
    else:
        # Just set the ttyname
        if lib.essentials.isInteractive():
            tty = os.ttyname(sys.stdin.fileno())
            prefix = '/dev/'
            if tty.startswith(prefix):
                tty = tty[len(prefix):]
            vps_remote_addr = tty

    if command:
        if args:
            print >>sys.stderr, "error: trailing arguments %s" % str(args)
            return 3
        else:
            cli_runner('None', True, command)
    elif args:
        vps_oneshot = True
        vps_tokenized = True
        cli_runner('None', True, args)
    else:
        cli_runner()

    return 0

if __name__ == '__main__':
    try:
        exit_code = main(sys.argv)
        sys.exit(exit_code)
    finally:
        restore_terminal_settings()

