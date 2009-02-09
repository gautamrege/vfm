import sys, os

cwd=os.getcwd()
sys.path.append("%s/build/lib.linux-x86_64-2.4" % cwd)
sys.path.append("%s/.." % cwd)

#print sys.path
import vfm

vadapter_ip = {'name' : "Vadapter", 'desc' : "Testing vadapter", 'protocol' : 1}
vfabric_ip = {'name' : "Vfabric", 'desc' : "Testing vfabric"}

def reg1():#WORKING
        vadapter = vfm.py_vfm_vadapter_create(vadapter_ip)
        print "vadpter created:", vadapter
        edit1(vadapter)
        result = vfm.py_vfm_vadapter_edit_general_attr({'id':vadapter['id'],'io_module_id':1, 'vfabric_id': 34})
        print "vdapter edited:", result

def reg2():#WORKING
        try:
                vfm.py_vfm_vadapter_create({})
        except StandardError ,error:
                print error

def reg3():#NEED PROTOCOL VALIDATION
        try:
                vfm.py_vfm_vadapter_create({'protocol' : 10})
        except StandardError ,error:
                print error

def invalid_ip(): #CREATES A VADAPTER WITH PROTOCOL = ''
        try:
                vfm.py_vfm_vadapter_create({'proto' : 10})
        except StandardError ,error:
                print error

def edit1(vadapter):#WORKING
        try:
                result = vfm.py_vfm_vadapter_edit_general_attr({'id':vadapter['id'],'io_module_id':3, 'vfabric_id':30})
                print "vdapter edited:", result
        except StandardError, e:
                print e

def edit2():#ERROR IN UPDATING THE VADAPTER
        try:
                result = vfm.py_vfm_vadapter_edit_general_attr({})
                print "vdapter edited:", result
        except StandardError, e:
                print e

def edit3():#ERROR IN UPDATING THE VADAPTER
        try:
                result = vfm.py_vfm_vadapter_edit_general_attr({'id':'sadcds'})
                print "vdapter edited:", result
        except StandardError, e:
                print e

def edit4():#ERROR IN UPDATING THE VADAPTER
        try:
                result = vfm.py_vfm_vadapter_edit_general_attr({'io_module_id':3})
                print "vdapter edited:", result
        except StandardError, e:
                print e

def inventory():
        try:
                result = vfm.py_vfm_vadapter_select_inventory({})
                print result
        except StandardError, e:
                print e


inventory()                
'''
reg1()
reg2()
reg3()
invalid_ip()
edit2()
edit3()  
edit4()'''
