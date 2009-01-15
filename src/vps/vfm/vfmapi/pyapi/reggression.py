import sys, os

cwd=os.getcwd()
sys.path.append("%s/build/lib.linux-x86_64-2.4" % cwd)
sys.path.append("%s/.." % cwd)

#print sys.path
import vfm

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
        vfabric = vfm.py_vfm_vfabric_create(vfabric_ip)
        print "Vfabric created:", vfabric

        vadapter = vfm.py_vfm_vadapter_create(vadapter_ip)
        print "vadpter created:", vadapter

        result = vfm.py_vfm_vfabric_edit_general_attr({'id': vfabric['id'],
                                                'primary_gateway':1})
        print "vfabric edited:", result

        result = vfm.py_vfm_vadapter_edit_general_attr({'id':vadapter['id'],'io_module_id':1, 'vfabric_id':vfabric['id']})
        print "vdapter edited:", result

        result = vfm.py_vfm_vfabric_online({'id':vfabric['id']})
        print "vfabric is online:", result

def reg2():
        print """\nRegression Test 2:
1. Display bridge inventory.
"""
        input = {}
        bridge_info = vfm.py_vfm_bd_select_inventory(input)
        print bridge_info

reg1()
reg2()
