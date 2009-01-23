#!/usr/bin/python2 -tt
#
# Copyright (c) 2008  VirtualPlane Systems, Inc.
#
#
import getopt
import sys
import os
import re
import sqlite

path = 'src/vps/cli'
if path not in sys.path:
     sys.path.append(path) 
    
DB_LOC = ''
DB_NAME = '/home/hansraj/pluto_dev/src/vps/vfm/db/vfm.db'
#DB_NAME = '/usr/local/vps/bxm/bxmv3.db'
_CURSOR = ""
_CURRENT_TABLE = ""

class Database(object):
      """Have all the functions needed to store, 
      retrieve, execute, commit, create""" 

      def _init_(self):
          '''Initialize the object'''
          # Connect to the database
          conn = self._connect()
          
          # Create the cursor to the database
          cursor = self._createCursor(conn)

          # Set the cursor
          self._setCursor(cursor)
          
          # Set the current table on which to work.
          # Close the connection    

          print 'All set for the database'

      def _connect(self):
          ''' Connect to the database'''
          #sys.path.append(DB_LOC)
          db_location = DB_NAME
          connection = sqlite.connect(db_location)
          return connection
         
      def _createCursor(self, connection):
          '''This creates the cursor to the latest database'''
          cursor = connection.cursor()
          return cursor            
      
      def _getCursor(self):
          '''This will return the latest cursor in the database'''
          return _CURSOR
  
      def _setCursor(self, cursor):
          '''This will set the Cursor to the public variable'''
          _CURSOR = cursor
           
      def _createTable(self, tableName):
          '''This will create the table with the given name'''
          cursor = _getCursor()
          cmd = 'Create Table' + tableName + '(sno integer primary key, \
                 mode varchar(10) , sub_mode varchar(10),\
                 status varchar(10), descape varchar(20))'    
          print cmd
          _execute(cmd, cursor)

      def _currentDBLocation(self):
          '''returns the current database location''' 
          return DB_LOC

      def _currentCursor(self, cursor):
          '''This will return the current cursor location'''
          return cursor.lastrowid
 
      def _execute(self, cmd):
          '''Will execute the latest command'''
          # Have a try except and return 0 if success otherwise -1.
          try:
              conn = self._connect()
          except:
              print 'Can\'t Connect to the Database. Please copy "bxm.db" at \
                            /src/vps/vfm/db'
              return 'Null' 
          cursor = self._createCursor(conn)
          cursor.execute(cmd)
          conn.commit()
          return cursor 
          
      def _fetchAll(self,cursor):
          '''Fetch all the rows from the database'''
          data = cursor.fetchall()
          return data
 
      def _isConnected(self):
          '''Will return 0 if connected otherwise -1'''
          return -1
    
      def _fetchRow(self, cmd, cursor):
          '''Fetch the rows in the databse'''

      def _commit(self, connection):
          '''Commit the database'''
          connection.commit()

      def _disConnect(self):
          '''DisConnect from the database'''

    
def _main():
    """This function is invoked when hostname.py functions as a
    stand-alone program
    """

    # After this we will call the functions.
    #
    # 1. Database location
    # 2. Connect
    # 3. Check is connect
    # 4. Make Table
    # 5. Input new row execute
    # 6. fetch all
    # 7. fetch row
    # 8.  

    try:
        opts, args = getopt.getopt(sys.argv[1:], "c:d:i:she")
   
    except getopt.GetoptError:
        # Print help information :
        _usage()
        sys.exit(2)
    
    database = Database()   
    cmd = "edit_database"
    print opts
    for o, a in opts:
        print o , a
        if o == "-e":
           sys.exit(2)
        elif o == "-i":
           cmd = "insert_data"
        elif o == "-s":
           cmd = "show_data"
        elif o == "-d":
           cmd = "delete_data"
        elif o == "-h":
           _usage()
           sys.exit(0)
        else :
           _usage()
           sys.exit(1)

    print cmd
    args.insert(0, cmd);
    if cmd == "show_data":
       show(database, args)
    elif cmd == "insert_data":
       insert(args)
    elif cmd == "delete_data":
       delete(args)
    else:
       print args
    sys.exit(0)        
    

def insert(args):
    """ Insert the arguments in the database
        DataBase Design is 
        1. Mode 
        2. Sub-Mode
        3. Status
        4. Descapeription
                So I will get 4 args in the similar manner for insert.
        Here Descapeription is optional .If not found then put null there.
    """
     
def show(database, args): 
    """ Show the arguments from the database
       Show will depend on the argument given. This is just to test the
       database access.
       Main Fetch Function we need are :
       1. Fetch submode with the current mode given
       2. Fetch the status 
       3. On show we should display all the details.
    """
    #print database._currentDBLocation()   
    print 'SHOW'
    #print database._connect()
    cmd = 'Select * from bxm_vadaptor_attr'
    print database._execute(cmd)

def delete(args):
    """ Delete the arguments from the database
        Delete the submode when Mode and submode are given
        So args contain 1. Mode 2. Sub-mode
        Then search in the table and delete it from there.
   
    """

def _usage():
    """ Print the usage of all the inputs"""
    Usage = "Usage: db [-i] | db <insert>" + "db [-s] | db <show>" 
    Usage1= "db [-d] | db <delete>" + "db [-h] | db <help>" 
    Usage2= "db [-c] | db <create>" + "db [-e] | db <exit>"

    print >>sys.stderr, Usage + Usage1 + Usage2 
          
 
if __name__ == '__main__':
        _main()

