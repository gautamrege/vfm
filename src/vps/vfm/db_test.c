/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#ifdef DB_TEST

#include <common.h>
#include <db_access.h>

void test_update_bridge()
{
    vps_error err;

    vpsdb_resource info;
    vpsdb_bridge bridge;
    vpsdb_gateway *gw;
    vpsdb_resource rsc;
    uint16_t ext_1[15] = {0,1,2,3,0,1,2,3,0,1,2,3,0,1,2};
    uint16_t ext_2[15] = {2,3,0,1,2,3,0,1,2,3,0,1,2,3,1};

    /**** Populate a bridge *****/
    memset(&bridge, 0, sizeof(bridge));
    memcpy(bridge.mac, "1A0B0E", 6);
    memcpy(bridge.node_name, "Gateway2", 8);
    bridge.db_id = 11;
    memcpy(bridge.last_bc_mac, "010203", 6);
    bridge.max_recv = 512;
    bridge.num_gateways = 2;

    bridge.gateways = (vpsdb_gateway*)malloc(2 * sizeof(vpsdb_gateway));
    gw = bridge.gateways;

    /* First gateway */
    gw->gw_id = 7;
    gw->ext_protocol = 0;
    gw->sp = 1;
    gw->se = 0;
    gw->flood = 1;
    gw->egress_secure = 1;
    gw->ingress_secure = 1;
    gw->l2_lookup = 0;
    gw->int_port = 1;
    /* External port status for first gateway */
    memcpy(gw->ext_ports, ext_1, sizeof(ext_1));

    /* Second gateway */
    gw++;

    gw->gw_id = 8;
    gw->ext_protocol = 1;
    gw->sp = 0;
    gw->se = 1;
    gw->flood = 0;
    gw->egress_secure = 1;
    gw->ingress_secure = 0;
    gw->l2_lookup = 1;
    gw->int_port = 1;
    /* External port status for first gateway */
    memcpy(gw->ext_ports, ext_2, sizeof(ext_1)); 

    /* Make a vpsdb_resource */
    rsc.type = VPS_DB_BRIDGE;
    rsc.count = 1;
    rsc.data = &bridge;
    err = vpsdb_add_resource(VPS_DB_BRIDGE, &rsc);
    vps_trace(VPS_ERROR, "vpsdb_add_resource: %d", err);
}

void get_gateway_info(vpsdb_gateway* gw)
{
    int i;
    printf("      + Gw Id     : %d\n", gw->gw_id);
    printf("         + Ext Proto : %d\n", gw->ext_protocol);
    printf("         + SP        : %d\n", gw->sp);
    printf("         + SE        : %d\n", gw->se);
    printf("         + Flood     : %d\n", gw->flood);
    printf("         + Egress S  : %d\n", gw->egress_secure);
    printf("         + Ingress S : %d\n", gw->ingress_secure);
    printf("         + L2 lookup : %d\n", gw->l2_lookup);
    printf("         + Int port  : %d\n", gw->int_port);
    printf("         + Conn Speed: %d\n", gw->connection_speed);

    /* External ports status */
    printf("         + Ext Ports : ");
    for(i = 0; i < 15; i++)
        printf("%d:%x ", i+1, gw->ext_ports[i]);
    printf("\n");
}

void get_all_gw()
{
    vpsdb_resource rsc;
    vpsdb_gateway *gateway;
    uint8_t tmp_str[128];
    int i, j;

    memset(&rsc, 0, sizeof(rsc));
    if (VPS_SUCCESS != vpsdb_get_resource(VPS_DB_GATEWAY, &rsc, NULL))
    {
        vps_trace(VPS_ERROR, "Error in vpsdb_get_resource");
        goto out;
    }

    for (j = 0; j < rsc.count; j++)
        get_gateway_info((vpsdb_gateway*)rsc.data + j);
out:
    return;
}

void get_bridge_info()
{
    vpsdb_resource rsc;
    vpsdb_bridge *bridge;
    uint8_t tmp_str[128];
    int i, j;

    memset(&rsc, 0, sizeof(rsc));
    if (VPS_SUCCESS != vpsdb_get_resource(VPS_DB_BRIDGE, &rsc, NULL))
    {
        vps_trace(VPS_ERROR, "Error in vpsdb_get_resource");
        goto out;
    }

    /* Print bridge information */
    printf("\n== BRIDGES (%d) ==\n", rsc.count);
    for(i = 0; i < rsc.count; i++)
    {
        bridge = (vpsdb_bridge*)rsc.data + i;

        memset(tmp_str, 0, sizeof(tmp_str));
        memcpy(tmp_str, bridge->mac, sizeof(bridge->mac));
        printf("+ Mac Addr : %s\n", tmp_str);

        memset(tmp_str, 0, sizeof(tmp_str));
        memcpy(tmp_str, bridge->node_name, sizeof(bridge->node_name));
        printf("  + Node_name: %s\n", tmp_str);

        printf("  + DB Id    : %d\n", bridge->db_id);

        memset(tmp_str, 0, sizeof(tmp_str));
        memcpy(tmp_str, bridge->last_bc_mac, sizeof(bridge->last_bc_mac));
        printf("  + Last BC  : %s\n", tmp_str);

        printf("  + Max Recv : %d\n", bridge->max_recv);

        memset(tmp_str, 0, sizeof(tmp_str));
        memcpy(tmp_str, bridge->vendor, sizeof(bridge->vendor));
        printf("  + Vendor   : %s\n", tmp_str);

        memset(tmp_str, 0, sizeof(tmp_str));
        memcpy(tmp_str, bridge->model, sizeof(bridge->model));
        printf("  + Model    : %s\n", tmp_str);

        memset(tmp_str, 0, sizeof(tmp_str));
        memcpy(tmp_str, bridge->fw_version, sizeof(bridge->fw_version));
        printf("  + Fw Ver   : %s\n", tmp_str);

        printf("  + === GATEWAYS (%d) ===:\n", bridge->num_gateways);
        for (j = 0; j < bridge->num_gateways; j++)
            get_gateway_info((vpsdb_gateway*)bridge->gateways + j);
    }
out:
    return;
}
#endif /* DB_TEST */

