BASEDIR=..
SRC= vfmapi.c \
     vfmapi_vadapter.c \
     vfmapi_vfabric.c \
     vfmapi_gateway.c \
     vfmapi_bridge_device.c \
     vfmapi_connect.c \
     $(BASEDIR)/vfmapi_common.c
CFLAGS= -g -fPIC
VFMAPI=vfmapi
LIBS= -lpthread
VFMAPI_INC_PATH= $(BASEDIR)/include/vfmapi
INC= -I$(BASEDIR)/include -I$(VFMAPI_INC_PATH)
DEFS=
#TODO: Add .c.o rules asap
all: vfmapi
	
client: 
	gcc -o client $(CFLAGS) $(DEFS) $(INC) $(LIBS) $(SRC)
vfmapi: 
	gcc -shared -o lib$(VFMAPI).so $(CFLAGS) $(DEFS) $(INC) $(LIBS) $(SRC)

test: vfmapi
	gcc -o cli unittest/vfmapi_test.c -L. -lvfmapi $(CFLAGS) $(DEFS) $(INC)

try:
	gcc -o test unittest/vfmapi_test.c $(CFLAGS) $(DEFS) $(INC) $(SRC)
clean:
	rm -f lib$(VFMAPI).so cli client
