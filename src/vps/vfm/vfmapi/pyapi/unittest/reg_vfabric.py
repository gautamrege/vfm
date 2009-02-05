import sys, os

cwd=os.getcwd()
sys.path.append("%s/build/lib.linux-x86_64-2.4" % cwd)
sys.path.append("%s/.." % cwd)

import vfm

vfabric_ip = {'name' : "Vfabric", 'desc' : "Testing vfabric", 'protocol' : 2}

def reg1():#WORKING
        vfabric = vfm.py_vfm_vfabric_create(vfabric_ip)
        print "vadpter created:", vfabric
        edit1(vfabric)                                         

def reg2():#WORKING
        try:
                vfm.py_vfm_vfabric_create({})
        except StandardError ,error:
                print error

def reg3():#NEED PROTOCOL VALIDATION
        try:
                vfm.py_vfm_vfabric_create({'protocol' : 10})
        except StandardError ,error:
                print error

def invalid_ip(): #CREATES A VADAPTER WITH PROTOCOL = ''
        try:
                vfm.py_vfm_vfabric_create({'proto' : 10})
        except StandardError ,error:
                print error

def edit1(vfabric):#WORKING
        try:
                result = vfm.py_vfm_vfabric_edit_general_attr({'id': 76, 'running_mode' : 2, 'primary_gateway' : 900, 'backup_gateway' : 9, 'auto_failover' : 1, 'auto_failback' : 0})
                print "vfabric edited:", result
        except StandardError, e:
                print e

def edit2():#ERROR IN UPDATING THE VADAPTER
        try:
                result = vfm.py_vfm_vfabric_edit_general_attr({})
                print "vfabric edited:", result
        except StandardError, e:
                print e

def edit3():#ERROR IN UPDATING THE VADAPTER
        try:
                result = vfm.py_vfm_vfabric_edit_general_attr({'id':'sadcds'})
                print "vfabric edited:", result
        except StandardError, e:
                print e

def edit4():#ERROR IN UPDATING THE VADAPTER
        try:
                result = vfm.py_vfm_vfabric_edit_general_attr({'io_module_id':3})
                print "vfabric edited:", result
        except StandardError, e:
                print e

reg1()
reg2()
"""
reg3()
invalid_ip()
edit2()
edit3()  
edit4()    
"""
