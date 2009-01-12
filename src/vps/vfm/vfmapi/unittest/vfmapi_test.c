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
char * 
parse_unicode_to_string(uint64_t val, int length)
{
        uint8_t guid[8];
        uint8_t *buff;
        int i = 0; 
        int j = 0;

        buff = malloc(24);
        memset(buff, 0, 24);
        memset(guid, 0, sizeof(guid));
        memcpy(guid, &val, sizeof(guid));

        for(i =0 ; i < length; i++)
        {
                buff[j]   = ((guid[i] & 0xF0) >> 4);
                if (buff[j] <=0x09)
                        buff[j] += 48;
                else if (buff[j] >= 0x0A && buff[j] <= 0x0F)
                        buff[j] += 55;

                j++;
                buff[j] = (guid[i] & 0x0F );
                if (buff[j] <= 0x09)
                        buff[j] += 48;
                else if (buff[j] >= 0x0A && buff[j] <= 0x0F)
                        buff[j] += 55;
                j++;
                if ( i < (length-1))
                        buff[j] = ':';
                j++;
        }
//        printf("\nLength %d", strlen(buff));
        return buff;
}


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
test_bridge_inventory()
{
        vfm_bd_attr_t attr;
        vfm_bd_attr_bitmask_t bitmask;
        uint32_t num = 0;
        vfm_bd_attr_t *result;
        memset(&bitmask, 0, sizeof(vfm_bd_attr_bitmask_t));
        vfm_bd_select_inventory(&attr, bitmask, &num, &result);
        memset(&attr, 0 , sizeof(vfm_bd_attr_t));
        
        printf("\n guid : %s ", parse_unicode_to_string(result->_bd_guid, 8));

        memcpy(attr.desc, result->desc, sizeof(attr.desc));
        printf("\n desc : %s", attr.desc);
        memcpy(attr._firmware_version, result->_firmware_version, 
                        sizeof(attr._firmware_version));
        printf("\n firmware_version : %s", attr._firmware_version);
        
        printf("\n guid : %s ", parse_unicode_to_string(result->_vfm_guid, 8));

}

test_bridge_device()
{
        uint8_t guid[8] = {0x00, 0x30, 0x48, 0x68, 0xB3, 0xDE, 0x00, 0x00};
        vfm_bd_attr_t attr;
        vfm_bd_attr_bitmask_t bitmask;
        uint32_t num = 0;
        vfm_bd_attr_t *result;
        memset(&bitmask, 0, sizeof(vfm_bd_attr_bitmask_t));
        bitmask.guid = 1;
        memcpy(&attr._bd_guid, guid, sizeof(guid));
        vfm_bd_query_general_attr(attr._bd_guid, bitmask, result);
}

int main()
{
        //test_vadapter_create();
        //test_vadapter_edit();
        //test_vadapter_edit_protocol();
        //test_vfabric_create();
        //test_vfabric_edit();
        //test_vfabric_online();
        test_bridge_inventory();
        //test_bridge_device();
}
