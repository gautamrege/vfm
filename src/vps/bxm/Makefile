VPS_SRC=server.c \
    common.c \
    sniffer.c \
    net_util.c \
    vfm_create_pk.c \
    sender.c \
    map_util.c \
    vfm_fip.c \
    bxm_queue.c \
    vfm_vadapter.c \
    db/db_access.c \
    db/vfmdb.c \
    db/vfmdb_vadapter.c \
    db/vfmdb_iomodule.c \
    bxm_iomodule.c \
    config.c

BXMAPI_SRC = bxmapi_common.c \
	     bxmapi_server.c \
             bxmapi_bridge_device.c \
	     bxmapi_vfabric.c \
	     bxmapi_impl.c

EXT_SRC=ext/open-fcoe/crc32_le.c \
    ext/open-fcoe/crc32_le_tab.c \
    ext/open-fcoe/ether_crc32.c

EOIB=ext/eoib-swgw/bx.c \
    bxm_ib.c \
    bxm_iboe.c

DB_SRC=db/unittest/sqlite_unicode.c
 
TEST_SRC=db/unittest/db_test.c
DEFS=-DMELLANOX -DOPEN_FCOE
IB_DEFS= -DVFM_EOIB
CFLAGS=-g
LIBS=-lsqlite3 -lpthread
IB_LIBS= -libverbs -libumad
INC=-I. -Iinclude -Iext/open-fcoe/include -Iinclude/bxmapi -Iext/eoib-swgw

#TODO: Add .c.o rules asap
all: 
	gcc -o bxm $(CFLAGS) $(DEFS) $(INC) $(VPS_SRC) $(EXT_SRC) $(BXMAPI_SRC) $(LIBS)

eoib: 
	gcc -o bxm $(CFLAGS) $(IB_DEFS) $(DEFS) $(INC) $(EOIB) $(VPS_SRC) $(EXT_SRC) $(BXMAPI_SRC) $(LIBS) $(IB_LIBS)

dbtest: 
	gcc -o db/dbtest $(CFLAGS) $(DEFS) $(INC) $(DB_SRC) $(LIBS)

clean:
	rm -f bxm 
    
net:
	gcc -o net $(CFLAGS) $(DEFS) $(INC) net_util.c common.c -lsqlite3


