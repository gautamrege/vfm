import sys

sys.path.append('/home/hansraj/bxm_ib_interim/src/vps/bxm/bxmapi/pyapi/build/lib.linux-i686-2.5')
sys.path.append('/home/hansraj/bxm_ib_interim/src/vps/bxm/bxmapi')
import bxm

dict = {}
result = {}
dict["name"] = "vadap10"
dict["desc"] = "test"
dict["protocol"]  = 1


print dict

print "sending data..........."
vfabric_id = 1
io_module_id = 1

result = bxm.py_bxm_vadapter_create(dict)
result = bxm.py_bxm_vadapter_edit({'io_module_id' : io_module_id,
                                   'vfabric_id' : vfabric_id})
result = bxm.py_bxm_vfabric_online({'vfabric_id' : vfabric_id})


print "recieved data..........."

print result        

