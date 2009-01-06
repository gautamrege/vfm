import sys

sys.path.append('/home/hansraj/pluto/src/vps/bxm/bxmapi/pyapi/build/lib.linux-x86_64-2.4')
sys.path.append('/home/hansraj/pluto/src/vps/bxm/bxmapi')

import bxm

vadapter_ip = {'name' : "Vadapter", 'desc' : "Testing vadapter", 'protocol' : 1}
vfabric_ip = {'name' : "Vfabric", 'desc' : "Testing vfabric"}

def reg1():
        print """\nRegression Test 1:
 1. Create a vfabric
 2. Create a vadapter
 3. Edit vfabric : primary_gw = 1
 4. Edit vadapter: vfabric_id = <step 1> & io_module = 1
 5. Vfabric online
"""
        vfabric = bxm.py_bxm_vfabric_create(vfabric_ip)
        print "Vfabric created:", vfabric

        vadapter = bxm.py_bxm_vadapter_create(vadapter_ip)
        print "vadpter created:", vadapter

        result = bxm.py_bxm_vfabric_edit_general_attr({'id': vfabric['id'],
                                                'primary_gateway':1})
        print "vfabric edited:", result

        result = bxm.py_bxm_vadapter_edit_general_attr({'id':vadapter['id'],'io_module_id':1, 'vfabric_id':vfabric['id']})
        print "vdapter edited:", result

        result = bxm.py_bxm_vfabric_online({'id':vfabric['id']})
        print "vfabric is online:", result

reg1()
