#include<vfmapi_common.h>
#include<vfm_vadapter.h>
#include<string.h>

vfm_vadapter_attr_t attr, attr1;
vfm_vfabric_attr_t vfabric_attr;

pack_test_data()
{

	memset(&attr, 0, sizeof(vfm_vadapter_attr_t));
        strcpy(attr.name, "vadapter");
        strcpy(attr.desc, "goood");
        attr.io_module_id = 13;
        attr.vfabric_id = 21;
        attr.protocol = 2;

	memset(&attr1, 0, sizeof(vfm_vadapter_attr_t));
        strcpy(attr1.name, "vadapter_1");
        strcpy(attr1.desc, "goood_1");
        attr1.io_module_id = 1;
        attr1.vfabric_id = 1;
        attr1.protocol = 1;

        memset(&vfabric_attr, 0, sizeof(vfm_vfabric_attr_t));
        strcpy(vfabric_attr.name, "vfabric_4");
        strcpy(vfabric_attr.desc, "goood_4");
        vfabric_attr._vfabric_id = 12;
        vfabric_attr.protocol = 7;
        vfabric_attr.primary_gateway = 127;
}

show_vadapter_data(void * op_data)
{
        int i;
        vfm_vadapter_attr_t * op_attr = (vfm_vadapter_attr_t *)op_data;
                printf("\nNAME:%s", op_attr->name);
                printf("\nDESC:%s", op_attr->desc);
                printf("\nIO_ID:%d", op_attr->io_module_id);
                printf("\nVFAB:%d", op_attr->vfabric_id);
                printf("\nPROT:%d", op_attr->protocol);
}

show_vfabric_data(void * op_data)
{
        int i;
        vfm_vfabric_attr_t * op_attr = (vfm_vfabric_attr_t *)op_data;
                printf("\nNAME:%s", op_attr->name);
                printf("\nDESC:%s", op_attr->desc);
                printf("\nGW_ID:%d", op_attr->primary_gateway);
                printf("\nVFAB:%d", op_attr->_vfabric_id);
                printf("\nPROT:%d", op_attr->protocol);
}

make_and_get_packet()
{
        int len, i , type =17, size, size1, count = 0;
        uint8_t *test_packet, *temp;
        void  **op_data;
        len = 2 * sizeof(vfm_vadapter_attr_t) + (TLV_SIZE * 3) +
              sizeof(vfm_vfabric_attr_t) + sizeof(count);
        test_packet = malloc(len);
        temp = test_packet;

        count = 3;
        size = sizeof(vfm_vadapter_attr_t);
        size1 = sizeof(vfm_vfabric_attr_t);

        pack_test_data();


        memcpy(temp, &count, sizeof(uint32_t));
        temp+= sizeof(uint32_t);

        /* Create vadapter TLV */
        for(i = 0; i < 2; i++) {
                memcpy(temp, &type, sizeof(uint32_t));
                temp+= sizeof(uint32_t);
                memcpy(temp, &size, sizeof(uint32_t));
                temp+= sizeof(uint32_t);
                memcpy(temp, &attr, sizeof(vfm_vadapter_attr_t));
                temp+= sizeof(vfm_vadapter_attr_t);
        }
        /* Create vfabric TLV */
                memcpy(temp, &type, sizeof(uint32_t));
                temp+= sizeof(uint32_t);
                memcpy(temp, &size1, sizeof(uint32_t));
                temp+= sizeof(uint32_t);
                memcpy(temp, &vfabric_attr, sizeof(vfm_vfabric_attr_t));
                temp+= sizeof(vfm_vfabric_attr_t);


        vfmimpl_unpack(test_packet, &op_data);
#ifdef TLV_TEST
        get_tlv_value(test_packet, &count, &temp);

        printf("Got back data !!\n");
        show_vadapter_data(temp);
        free(temp);
        get_tlv_value(test_packet, &count, &temp);

        printf("Got back data !!\n");
        show_vadapter_data(temp);
        free(temp);
        get_tlv_value(test_packet, &count, &temp);

        printf("Got back data !!\n");
        show_vadapter_data(temp);
        free(temp);
#endif
        show_vadapter_data(op_data[0]);
        show_vadapter_data(op_data[1]);
        show_vfabric_data(op_data[2]);

        /* Free all allocated memory */
        free(test_packet);
        for (i= 0; i<count ; i++)
                free(op_data[i]);
        free(op_data);
}

