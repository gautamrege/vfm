BASEDIR=..
SRC= bxmapi.c \
     bxmapi_vadapter.c \
     bxmapi_vfabric.c \
     bxmapi_gateway.c \
     bxmapi_bridge_device.c \
     bxmapi_connect.c \
     $(BASEDIR)/bxmapi_common.c
CFLAGS= -g -fPIC
BXMAPI=bxmapi
LIBS= -lpthread
BXMAPI_INC_PATH= $(BASEDIR)/include/bxmapi
INC= -I$(BASEDIR)/include -I$(BXMAPI_INC_PATH)
DEFS= -DVADAPTER_EDIT -DEDIT_PROTOCOL
#TODO: Add .c.o rules asap
all: bxmapi
	
client: 
	gcc -o client $(CFLAGS) $(DEFS) $(INC) $(LIBS) $(SRC)
bxmapi: 
	gcc -shared -o lib$(BXMAPI).so $(CFLAGS) $(DEFS) $(INC) $(LIBS) $(SRC)

test: bxmapi
	gcc -o cli unittest/bxmapi_test.c -L. -lbxmapi $(CFLAGS) $(DEFS) $(INC)

try:
	gcc -o test unittest/bxmapi_test.c $(CFLAGS) $(DEFS) $(INC) $(SRC)
clean:
	rm -f lib$(BXMAPI).so cli client
