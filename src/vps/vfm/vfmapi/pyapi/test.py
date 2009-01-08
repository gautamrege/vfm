import sys

sys.path.append('/home/hansraj/pluto/src/vps/vfm/vfmapi/pyapi/build/lib.linux-x86_64-2.4')
sys.path.append('/home/hansraj/pluto/src/vps/vfm/vfmapi')

import vfm

dict = {}
result = {}
dict["name"] = "vadap13"
dict["desc"] = "test"
dict["protocol"]  = 1

vadapter_id =3
vfabric_id = 1
io_module_id = 2


def vadapter_create():
        result = vfm.py_vfm_vadapter_create(dict)

def vadapter_edit():
        result = vfm.py_vfm_vadapter_edit_general_attr(
                                        {'io_module_id' : io_module_id,
                                        'vfabric_id' : vfabric_id,
                                        'id' : vadapter_id})
def vfabric_create():
        result = vfm.py_vfm_vfabric_create({'protocol' : 1,
                                                'name' : "V Fabric10",
                                                'desc' : "test",
                                                'type' : 1})
def vfabric_edit():
        result = vfm.py_vfm_vfabric_edit_general_attr({
        'primary_gateway' : 123456,                
        'backup_gateway' : 1234,
        'type' :1,
        'auto_failover' : 0,
        'protocol' : 2,
        'id' : 3})

def vfabric_online():
        result = vfm.py_vfm_vfabric_online({'id' : vfabric_id})

option = raw_input("Enter the option : ")

print "\nSending data......."

if option == '0':
        vadapter_create()
elif option == '1':
        vadapter_edit()
elif option == '2':
        vfabric_create()
elif option == '3':
        vfabric_edit()
elif option == '4':
        vfabric_online()

print "\nrecieved data..........."
print result        
