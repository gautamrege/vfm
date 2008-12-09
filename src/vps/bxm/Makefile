
VPS_SRC=server.c \
    common.c \
    sniffer.c \
    net_util.c \
    vfm_create_pk.c \
    sender.c \
    map_util.c \
    vfm_fip.c \
    bxm_queue.c \
    bxm_ib.c \
    bxm_iboe.c\
    db/db_access.c \
    config.c

BXMAPI_SRC = bxmapi_common.c \
	     bxmapi_server.c \
             bxmapi_bridge_device.c\
	     bxmapi_impl.c

EXT_SRC=ext/open-fcoe/crc32_le.c \
    ext/open-fcoe/crc32_le_tab.c \
    ext/open-fcoe/ether_crc32.c \
    ext/eoib-swgw/bx.c
 
TEST_SRC=db/unittest/db_test.c
DEFS=-DMELLANOX
CFLAGS=-g
LIBS=-lsqlite3 -lpthread -libverbs -libumad
INC=-I. -Iinclude -Iext/open-fcoe/include -Iinclude/bxmapi -Iext/eoib-swgw

#TODO: Add .c.o rules asap
all: 
	gcc -o bxm $(CFLAGS) $(DEFS) $(INC) $(VPS_SRC) $(EXT_SRC) $(BXMAPI_SRC) $(LIBS)

clean:
	rm -f bxm 
    
net:
	gcc -o net $(CFLAGS) $(DEFS) $(INC) net_util.c common.c -lsqlite3


