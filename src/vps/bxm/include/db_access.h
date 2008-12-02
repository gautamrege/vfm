/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#ifndef __DB_ACCESS_H__
#define __DB_ACCESS_H__

#include <common.h>

#define VPS_DB_ALL             0x1
#define VPS_DB_BRIDGE          0x2
#define VPS_DB_GATEWAY         0x3
#define VPS_DB_HOST            0x4
#define VPS_DB_CNA             0x5
#define VPS_DB_VHBA            0x6
#define VPS_DB_VNIC            0x7

/*
 * This file is used as the database access layer for sqlite.
 * These are the binding for each resource that will get / set the data
 * from the database.
 *
 * NOTE: This will also be used by the CLI to gain access to information:
 */

/*
 * ******************* Device Tree Heirarchy ***********************
 * Hosts
 * +---Host1
 * |   + Host properties
 * |   +--- CNA1
 * |   |        + CNA properties
 * |   |        +---- vHBA1
 * |   |        |         + vHBA properties
 * |   |        +---- vHBA2
 * |   |                  + vHBA properties
 * |   +--- CNA2
 * |          + CNA properties
 * |                +---- vHBA3
 * |                |         + vHBA properties
 * |                +---- vHBA4
 * |                          + vHBA properties
 * +---Host2
 *         ...
 *
 * Bridges
 * +--- Bridge1
 * |        + Bridge properties
 * |        +--- Gateway1
 * |        |        + Gateway properties
 * |        +--- Gateway2
 * |        |        + Gateway properties
 * +--- Bridge2
 * |        +--- Gateway3
 * |        |        + Gateway properties
 *          ...
 */

typedef struct _vpsdb_gateway {
        uint16_t gw_id;         /* the gateway id */

        uint8_t ext_protocol;   /* External Protocol */
        uint8_t sp;             /* ignore */
        uint8_t se;             /* ignore */
        uint8_t flood;          /* flood */
        uint8_t egress_secure;  /* egress secure */
        uint8_t ingress_secure; /* ingress secure */
        uint8_t l2_lookup;      /* L2 lookup */

        uint16_t int_port;      /* internal port status */
        uint16_t ext_ports[15]; /* external ports status: 15 ports */

        uint32_t connection_speed;
}vpsdb_gateway;

typedef struct _vpsdb_bridge {
        uint8_t mac[6];         /* mac address of bridge */
        char node_name[8];
        uint16_t db_id;         /* database id */
        uint8_t last_bc_mac[6]; /* last asscociated bridge controller */
        uint32_t max_recv;      /* Max receive size of buffer */

        char vendor[64];        /* Vendor */
        char model[64];         /* bridge model. */
        char fw_version[64];    /* firmware version */

        uint32_t num_gateways;  /* number of bridges */
        vpsdb_gateway *gateways;/* array to bridge data structures */
}vpsdb_bridge;

typedef struct _vpsdb_vHBA {
        uint8_t priority;
        uint8_t fcf_mac[6];     /* FCF-Mac address of the CNA */
        uint32_t fc_map;
        uint8_t switch_name[8];
        uint8_t fabric_name[8];
        uint32_t fka_adv_period;
}vpsdb_vHBA;

typedef struct _vpsdb_cna {
        uint8_t mac[6];         /* Mac address of the CNA */
        uint32_t max_recv;      /* Max receive data payload */
}vpsdb_cna;

typedef struct _vpsdb_host {
        char name[8];

        uint32_t num_cna;
        vpsdb_cna *cna;
}vpsdb_host;

/*
 * vpsdb_resource
 *
 * This is a data structure which can be wrapped by an opaque pointer.
 * type: The type of resource: bridge, host, cna, gateway, vHBA, vNIC etc.
 * count: Number of items the data will point to.
 * data: The pointer to the array of objects for the data-type specified.
 */
typedef struct _vpsdb_resource {
        uint32_t type;
        uint32_t count;
        void *data;
}vpsdb_resource;

/**
 * vpsdb_get_resource
 * This routine gets resources from the database for various or all types of
 * resources stored. This routine should be used by the CLI to get information
 * from the database for display.
 *
 * type (IN) - Type of resource.
 *             VPS_DB_BRIDGE   - All bridges
 *             VPS_DB_GATEWAY  - All gateways
 *             VPS_DB_HOST         - All hosts
 *             VPS_DB_CNA          - All CNAs
 *             VPS_DB_VHBA         - All vHBAs
 *             VPS_DB_VNIC         - All vNICs  (TODO: later)
 *
 * info (OUT) - The output data structure which contains display information.
 * name (IN) - Optionally the name can be populated if a particular resource
 *             needs to be queried.
 *
 * Returns:
 * vps_error: VPS_SUCCESS on success.
 *            VPS_UNKNOWN_RESOURCE will be returned if the name is not present
 *
 */
vps_error vpsdb_get_resource(uint32_t type, vpsdb_resource *info,
                const char *name);

/**
 * vpsdb_add_resource
 * This function adds a resource to the database. This is an internal API and
 * is used by the VFM framework for updating information to the database from
 * reading the control packets.
 *
 * type (IN) - Type of the resource
 *             VPS_DB_BRIDGE - All bridges (they contain the gateways)
 *             VPS_DB_HOST - All hosts. They contain the CNAs and the CNAs
 *             in turn may contain vHBA, vNICs.
 */
vps_error vpsdb_add_resource(uint32_t type, vpsdb_resource *rsc);

#endif /* __DB_ACCESS_H__ */
