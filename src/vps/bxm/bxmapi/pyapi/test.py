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

result = bxm.py_bxm_vadapter_create(dict)

print "recieved data..........."

print result        

