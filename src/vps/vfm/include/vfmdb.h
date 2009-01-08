/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#ifndef __VFM_DB_H_
#define __VFM_DB_H_

#include <common.h>
#include <vfmapi_common.h>
#include <stdlib.h>


#define VPS_DB_ALL             0x1
#define VPS_DB_BRIDGE          0x2
#define VPS_DB_GATEWAY         0x3
#define VPS_DB_IO_MODULE       0x4
#define VPS_DB_CNA             0x5
#define VPS_DB_VHBA            0x6
#define VPS_DB_VNIC            0x7
#define VPS_DB_VADAPTER        0x8
#define VPS_DB_VFABRIC         0x9
#define VPS_DB_EN_VADAPTER     0x10

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

/* TODO: To be removed on iomodule is defined */
typedef struct _vpsdb_io_module_t {
	uint32_t id;
        char name[8];
        uint32_t type;
        uint8_t mac[6];         /* Mac address of the CNA */
        uint8_t guid[8];
        uint32_t num_vhba;
        uint32_t num_vnic;
        uint32_t slot;
        uint32_t port;
        uint32_t supported_speed;
}vpsdb_io_module_t;

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

/**
 * vfmdb_prepare_query
 *  This function is used to prepare a query with positional parameteres. This
 * is necessary when we have either unicode strings OR non NULL terminated 
 * strings.
 *
 * Usage Example:
 * char *query = "insert into vfm_bridge_device values(?1, ?2, 'v1.1', ?3);";
 * stmt = vfmdb_prepare_query(query, "ggs", guid1, guid2, "Bridge1 Desc");
 *
 * @param[IN] query The query with positional parameters. 
 *
 * @param[IN] fmt The string format specifier. In case there are no positional
 * parameters, simply pass fmt as NULL
 *  String format specifiers supported are:
 * 'g' - 8 bytes (guid)
 * 'm' - 6 bytes (mac)
 * 's' - strlen decides length
 *
 * @param[IN] args This an array of pointers that point to the arguments. The 
 * value of the arguments depends on the fmt string.
 *
 * @return Opaque pointer that should be passed to vfmdb_execute_query. NULL is
 * returned on failure.
 */ 
void* vfmdb_prepare_query(const char *query, const char *fmt, void **args);

/**
 * vfmdb_prepare_query_ex
 *  This function is used to prepare a query with positional parameteres. This
 * is necessary when we have either unicode strings OR non NULL terminated 
 * strings.
 *
 * Usage Example:
 * char *query = "insert into vfm_bridge_device values(?1, ?2, 'v1.1', ?3);";
 * stmt = vfmdb_prepare_query_ex(query, "ggs", guid1, guid2, "Bridge1 Desc");
 *
 * @param[IN] query The query with positional parameters. 
 *
 * @param[IN] fmt The string format specifier. In case there are no positional
 * parameters, simply pass fmt as NULL
 *  String format specifiers supported are:
 * 'g' - 8 bytes (guid)
 * 'm' - 6 bytes (mac)
 * 's' - strlen decides length
 *
 * @return Opaque pointer that should be passed to vfmdb_execute_query. NULL is
 * returned on failure.
 */ 
void* vfmdb_prepare_query_ex(const char *query, const char *fmt, ...);

/**
 * vfmdb_execute_query
 *  This routine executes the query prepared from vfmdb_prepare_query. The 
 * result can be processed with the callback and optional argument.
 *
 * @param[IN] stmt The opaque pointer returned from vfmdb_prepare_query. This
 * should not be NULL.
 *
 * @param[IN] sql_cb The callback routine for processing each row of the result
 * set. The callback MUST return 0 on success. This parameter may be NULL in 
 * case no result is expected.
 *
 * @param[IN] arg The argument (if non NULL) will be passed as the first 
 * argument to the callback.
 *
 * @return 0 on success and database error code on failure.
 */
vps_error vfmdb_execute_query(void *stmt,
                int (*sql_cb)(void*, int, uint8_t**, char**),
                void *arg);

#endif /* __VFM_DB_H_ */
