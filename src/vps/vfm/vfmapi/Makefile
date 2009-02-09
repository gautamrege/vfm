BASEDIR=..
SRC= vfmapi.c \
     vfmapi_vadapter.c \
     vfmapi_vfabric.c \
     vfmapi_gateway.c \
     vfmapi_bridge_device.c \
     vfmapi_connect.c \
     $(BASEDIR)/vfmapi_common.c \
     $(BASEDIR)/vfmimpl_unpack.c
CFLAGS= -g -fPIC
VFMAPI=vfmapi
LIBS= -lpthread
VFMAPI_INC_PATH= $(BASEDIR)/include/vfmapi
INC= -I$(BASEDIR)/include -I$(VFMAPI_INC_PATH)
DEFS=
#TODO: Add .c.o rules asap
all: vfmapi install
install:
	install -T libvfmapi.so /usr/lib64/libvfmapi.so
client: 
	gcc -o client $(CFLAGS) $(DEFS) $(INC) $(LIBS) $(SRC)
vfmapi: 
	gcc -shared -o lib$(VFMAPI).so $(CFLAGS) $(DEFS) $(INC) $(LIBS) $(SRC)
test: vfmapi
	gcc -o cli unittest/vfmapi_test.c unittest/vfm_unpack_test.c -L. -lvfmapi $(CFLAGS) $(DEFS) $(INC)
clean:
	rm -f /usr/lib64/lib$(VFMAPI).so lib$(VFMAPI).so cli client

