VPS_SRC=server.c \
    common.c \
    sniffer.c \
    net_util.c \
    vfm_create_pk.c \
    sender.c \
    map_util.c \
    vfm_fip.c \
    vfm_queue.c \
    db/db_access.c \
    db/vfmdb.c \
    db/vfmdb_vadapter.c \
    db/vfmdb_iomodule.c \
    db/vfmdb_bridge_device.c \
    db/vfmdb_vfabric.c \
    vfm_iomodule.c \
    config.c

VFMAPI_SRC = vfmapi_common.c \
	     vfmapi_server.c \
             vfmapi_bridge_device.c \
	     vfmapi_vfabric.c \
             vfmapi_vadapter.c \
             vfmimpl_pack.c \
	     vfmapi_impl.c \
	     vfmimpl_unpack.c

EXT_SRC=ext/open-fcoe/crc32_le.c \
    ext/open-fcoe/crc32_le_tab.c \
    ext/open-fcoe/ether_crc32.c

EOIB=ext/eoib-swgw/bx.c \
    vfm_ib.c \
    vfm_iboe.c

DB_SRC=db/unittest/sqlite_unicode.c
 
TEST_SRC=db/unittest/db_test.c
DEFS=-DMELLANOX -DOPEN_FCOE -DVFM_NO_OBJ_CACHE
IB_DEFS= -DVFM_EOIB
CFLAGS=-g
LIBS=-lsqlite3 -lpthread
IB_LIBS= -libverbs -libumad
INC=-I. -Iinclude -Iext/open-fcoe/include -Iinclude/vfmapi -Iext/eoib-swgw

#TODO: Add .c.o rules asap
all: 
	gcc -o vfm $(CFLAGS) $(DEFS) $(INC) $(VPS_SRC) $(EXT_SRC) $(VFMAPI_SRC) $(LIBS)

eoib: 
	gcc -o vfm $(CFLAGS) $(IB_DEFS) $(DEFS) $(INC) $(EOIB) $(VPS_SRC) $(EXT_SRC) $(VFMAPI_SRC) $(LIBS) $(IB_LIBS)

dbtest: 
	gcc -o db/dbtest $(CFLAGS) $(DEFS) $(INC) $(DB_SRC) $(LIBS)

clean:
	rm -f vfm 
    
net:
	gcc -o net $(CFLAGS) $(DEFS) $(INC) net_util.c common.c -lsqlite3


