import sys, os

cwd=os.getcwd()
sys.path.append("%s/build/lib.linux-x86_64-2.4" % cwd)
sys.path.append("%s/.." % cwd)
import vfm

bridge = [ 'bd_guid' , '01:30:48:6C:B3:DE:03:00', 'desc' , 'bridge is new', 'state' , '5',
               'running_mode' , 1, 'firmware_version' , 'v1.11',
               'vfm_guid', '01:30:48:77:67:EE:00:00' ]

def inventory():
        execute({})

def reg_guid():
        input = {'bd_guid' : '01:30:48:6D:B3:DE:03:00'}
        p = dict([(bridge[0], bridge[1])])
        execute(input)

def reg_desc():
        p = dict([(bridge[2], bridge[3])])
        execute(p)

def reg_state():
        p = dict([(bridge[4], bridge[5])])
        execute(p)

def reg_running():
        p = dict([(bridge[6], bridge[7])])
        execute(p)

def reg_firmware():
        p = dict([(bridge[8], bridge[9])])
        execute(p)

def reg_vfm_guid():
        p = dict([(bridge[10], bridge[11])])
        execute(p)

# NEGATIVE TESTS

def ne_reg_guid():
        p = dict([(bridge[0], 'A1:3A:48:6C:B3:DE:03:00')])
        execute(p)
        p = dict([('bd_guid', 'AA:BB:CC:cartridge')])
        execute(p)

def ne_reg_desc():
        p = dict([(bridge[2], 'asasasas')])
        execute(p)
        p = dict([(bridge[2], 'asasasashksfkkdflaslashdfhashkjhhsdfhdhasfkjlhladhsfjhadhsdkjlhlhhaskfkjhadshkdashkhaklh')])
        execute(p)
        p = dict([('desc', 'gw111 bridge')])
        execute(p)

def ne_reg_state():
        p = dict([(bridge[4], bridge[7])])
        execute(p)

def ne_reg_running():
        p = dict([(bridge[6], bridge[9])])
        execute(p)

def ne_reg_firmware():
        p = dict([(bridge[8], 'no_idea')])
        execute(p)
        p = dict([(bridge[8], 'V.20101')])
        execute(p)
        p = dict([('desc', 'a2llslsjfjflsksjjkjdjaasV.20101')])
        execute(p)

def ne_reg_vfm_guid():
        p = dict([(bridge[10], 'A1:3A:48:6C:B3:DE:03:00')])
        execute(p)
        p = dict([('vfm_guid', 'AA:BB:CC:cartridge')])
        execute(p)

def execute(p) :
        print ' 2. Display bridge inventory.'
        print p
        try:
            bridge_info = vfm.py_vfm_bd_select_inventory(p)
        except StandardError, e:
                print "ERROR ! " ,e 
        else :
                print bridge_info

print ' ---*** POSITIVE TESTS ***---'
inventory()
inventory()
inventory()
inventory()

reg_guid()
"""
reg_desc()
reg_firmware()
reg_vfm_guid()

#/* Not working for now. Need to look in to */
#reg_state()
#reg_running()
#execute({ 'desc' : 'bridge is new'})

print '--- *** NEGATIVE TESTS ***---'
ne_reg_guid()
ne_reg_desc()
ne_reg_firmware()
ne_reg_vfm_guid()

#/* Not working for now. Need to look in to */
#ne_reg_state() 
#ne_reg_running()
#execute({ 'desc' : 'bridge is new'})'''
"""
