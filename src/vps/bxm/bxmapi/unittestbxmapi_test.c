#include<bxmapi_common.h>
#include<bxm_vadapter.h>
#include<bxm_bridge_device.h>
#include<string.h>

int main()
{
/*
        bxm_vadapter_attr_bitmask_t bitmask;
        bxm_vadapter_attr_t attr;
        int num;
        bxm_vadapter_id_t *result[2];
        memset(&bitmask, 7, sizeof(bxm_vadapter_attr_bitmask_t));
        //memset(&attr, 3, sizeof(bxm_vadapter_bitmask_t));
        attr._vadapter_id = 4;
        memcpy(&attr.name, "mac1", 4);
        memcpy(&attr.desc, "yac2", 4);
        attr._state = 1;
        attr.running_mode = 0;
        attr.init_type = 2;
        attr.io_module_id = 14343;
        attr.protocol = 1;
        //attr.en_attr.mac[6] = {0x1,0x2,0x3,0x1,0x2,0x3};
        
        attr.vfabric_id = 12;

      //  bxm_vadapter_select_inventory(&attr, &bitmask, num, result);
        bxm_vadapter_create("sas", "desc", 2, &num);
        printf("recieved data.........");
        // bxm_vadapter_edit_property(1, &bitmask, &attr);
        //        bxm_vadapter_create("sas", "desc", 1, 2, &num);
        //        bxm_vadapter_edit_property(1, &bitmask, &attr);
       
      */  
        bxm_bd_attr_t attr;
        bxm_bd_attr_bitmask_t bitmask;
        int num; 
        bxm_bd_attr_t *result[10];
        memset(&bitmask, 0, sizeof(bxm_bd_attr_bitmask_t));
        memset(&attr, 0, sizeof(bxm_bd_attr_t));
       
        
                bxm_bd_select_inventory(&attr, bitmask, &num, result);

                sleep(2);
                bxm_bd_select_inventory(&attr, bitmask, &num, result);
                sleep(2);
                bxm_bd_select_inventory(&attr, bitmask, &num, result);
                sleep(2);
                bxm_bd_select_inventory(&attr, bitmask, &num, result);
                sleep(2);
                bxm_bd_select_inventory(&attr, bitmask, &num, result);
                sleep(2);
                bxm_bd_select_inventory(&attr, bitmask, &num, result);
                bxm_bd_select_inventory(&attr, bitmask, &num, result);
                bxm_bd_select_inventory(&attr, bitmask, &num, result);
                bxm_bd_select_inventory(&attr, bitmask, &num, result);
        

}


