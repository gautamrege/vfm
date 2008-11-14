SRC=server.c \
    common.c \
    sniffer.c \
    db_access.c \
    net_util.c \
    vfm_create_pk.c \
    sender.c \
    crc32_le.c \
    crc32_le_tab.c \
    ether_crc32.c \
	map_util.c \
    vfm_fip.c \
    queue.c
TEST_SRC=db_test.c
DEFS=-DMELLANOX -DB_TEST
CFLAGS=-g
LIBS=-lsqlite3 -lpthread
INC=-I.

#TODO: Add .c.o rules asap
all: 
	gcc -o vfm $(CFLAGS) $(DEFS) $(INC) $(SRC) $(LIBS)

clean:
	rm -f vfm 
    
net:
	gcc -o net $(CFLAGS) $(DEFS) $(INC) net_util.c common.c -lsqlite3


