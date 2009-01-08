#include<vfmapi_common.h>
#include<vfm_vadapter.h>
#include<vfm_bridge_device.h>
#include<string.h>

uint8_t host_mac[6] = {0x00, 0x02, 0xc9, 0x01, 0xc6, 0xf4};

/* Globals for tesing */
int num;
vfm_vadapter_attr_bitmask_t bitmask;
vfm_vadapter_attr_t attr;
vfm_vadapter_en_attr_bitmask_t en_bitmask;
vfm_vadapter_en_attr_t en_attr;
vfm_vfabric_attr_bitmask_t vfabric_bitmask;
vfm_vfabric_attr_t vfabric_attr;
char name[64] ={0};

void test_vadapter_create()
{
        vfm_vadapter_create("vadapter_new", "new_one", 1, &num);
        printf("NEW VADPTER ID: %d", num);
}

test_vadapter_edit()
{
        memset(&bitmask, 0, sizeof(vfm_vadapter_attr_bitmask_t));
        bitmask.vfabric_id = 1;
        bitmask.name = 1;
        bitmask.desc = 1;
        bitmask.io_module_id = 1;

	memset(&attr, 0, sizeof(vfm_vadapter_attr_t));
        strcpy(attr.name, "vadapter_11");
        strcpy(attr.desc, "goood_11");
        attr.io_module_id = 13;
        attr.vfabric_id = 21;
        attr.protocol = 2;
        vfm_vadapter_edit_general_attr(11, &bitmask, &attr);
}

test_vfabric_create()
{
	vfm_vfabric_create("vfabric_new", "new_fabric", 1, &num);
        printf("NEW VFABRIC ID: %d", num);
}

test_vfabric_edit()
{
        memset(&vfabric_bitmask, 0, sizeof(vfm_vfabric_attr_bitmask_t));
        vfabric_bitmask.vfm_vfabric_id = 1;
        vfabric_bitmask.primary_gateway = 1;
        vfabric_bitmask.name = 1;
        vfabric_bitmask.desc = 1;

        memset(&vfabric_attr, 0, sizeof(vfm_vfabric_attr_t));
        strcpy(vfabric_attr.name, "vfabric_4");
        strcpy(vfabric_attr.desc, "goood_4");
        vfabric_attr._vfabric_id = 12;
        vfabric_attr.protocol = 7;
        vfabric_attr.primary_gateway = 127;
        vfm_vfabric_edit_general_attr(2, &vfabric_bitmask, &vfabric_attr);
}

void test_vadapter_edit_protocol()
{
        memset(&en_bitmask, 0, sizeof(vfm_vadapter_en_attr_bitmask_t));
	en_bitmask.mac = 1;
	en_bitmask.vlan = 1;

	memset(&en_attr, 0, sizeof(vfm_vadapter_en_attr_t));
	memcpy(en_attr.mac, host_mac, 6);
	en_attr.vlan = 99;
	vfm_vadapter_edit_protocol_attr(2, &en_bitmask, &en_attr);
//	vfm_vadapter_query_protocol_attr(2, &en_bitmask, &en_attr);
}

test_vfabric_online()
{
        vfm_vfabric_online(1);
}

int main()
{
        //test_vadapter_create();
        //test_vadapter_edit();
        //test_vadapter_edit_protocol();
        //test_vfabric_create();
        test_vfabric_edit();
        //test_vfabric_online();
}
