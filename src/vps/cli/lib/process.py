#!/usr/bin/python2 -tt
#
# Copyright (c) 2008  VirtualPlane Systems, Inc.
#
#
""" File where all the process related functions are controlled"""

import errno
import fcntl
import os
import popen2
import resource
import select
import signal
import sys
import time


class TimeCheck(Exception):
    def __init(self, timeout, identifier=None):
        Exception.__init__(self, timeout, identifier)
    def __str__(self):
        timeout = self[0]
        identifier = self[1]
        if identifier:
            return "Task %s timed out (max %d seconds)" % (identifier, timeout)
        else:
            return "Task timed out (max %d seconds)" % (timeout,)


def execute_command(cmd_str, timeout=None):
    """ 
       Here all the command strings are passed and then they are run on the 
       command line with error , output returned to the calling function
    """

    # Check the times if not None then run the counter.
    if timeout is not None:
         start_timer = True
         # this is the time when the command execution should stop.
         stop_timer = time.time() + timeout
         classifier = "<execute_command %r>" % (cmd_str,)
    else:
         start_timer = False

    # Build in function of popen
    pop3 = popen2.Popen3(cmd_str, capturestderr = 1)

    error_file_dir = pop3.childerr.fileno()
    output_file_dir = pop3.fromchild.fileno()

    polling_object = select.poll()
    polling_object.register(error_file_dir, select.POLLIN)
    polling_object.register(output_file_dir, select.POLLIN)

    timeout_temp = False
    no_of_file_dir = 2
    output_lines = []
    error_lines  = []
    
    try :
         while no_of_file_dir != 0:
               try:
                   if start_timer:
                       remaining_timer = stop_timer - time.time()
                       if remaining_timer < 0 or remaining_timer > timeout:
                             raise TimeCheck(timeout, classifier)
                       residual = polling_object.poll(remaining_timer)
                   else:
                       residual = polling_object.poll()
               except select.error, ex:
                   if ex[0] == errno.EINTR:
                      continue
                   else:
                      break

               for (fd, event) in residual:
                    if event & (select.POLLERR | select.POLLHUP):
                         polling_object.unregister(fd)
                         no_of_file_dir = no_of_file_dir - 1
                         if fd == error_file_dir:
                              error_lines.extend(pop3.childerr.readlines())
                         else:
                              output_lines.extend(pop3.fromchild.readlines())
                              continue

                    # Not a POLLIN event, continue
                    if not event & select.POLLIN:
                       continue

                    # POLLING : Pick which one to read from
                    if fd == error_file_dir:
                          error_lines.append(pop3.childerr.readline())
                    else:
                          output_lines.append(pop3.fromchild.readline())
          
         #Gather exit status
         if start_timer:
             remaining_timer = stop_timer - time.time()
             while 0 < remaining_timer < timeout:
                status = pop3.poll()
                if status >= 0:
                   break
                if remaining_timer >= 1:
                   sleep_time = 1
                else:
                   sleep_time = remaining_timer
                time.sleep(sleep_time)
                remaining_timer = stop_timer - time.time()
             else:
                raise TimeCheck(timeout, classified)
         else:
             status = pop3.wait()

    except TimeCheck:
           os.kill(po3.pid, signal.SIGKILL)
           status = pop3.wait()
           timeout_temp = True

    # Cleanup
    pop3.fromchild.close()
    pop3.childerr.close()
    pop3.tochild.close()

    if os.WIFEXITED(status):
        exitval = os.WEXITSTATUS(status)
    else:
        if timed_out:
            exitval = -2
        else:
            exitval = -1
    return (exitval, output_lines, error_lines, status)



