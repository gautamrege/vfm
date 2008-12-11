/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#ifndef INCLUDED_bxm_functions
#define INCLUDED_bxm_functions

#include "bxm_common.h"
#include "bxm_error.h"
#include "bxm_port.h"
#include "bxm_state.h"
#include "bxm_gateway.h"

/**
 * @file
 * @brief API to manage bridge device resource.
 *
 * This file contains attributes and function prototypes of managing a BridgeX
 * device resource.  
 */

/**
 * @brief A structure for the properties of a Gateway module
 */
typedef struct
{
    /**
     * @brief Gateway module Id.
     * @note This is a read-only property i.e. a user cannot edit it using
     * the bridge_device API. This identifier is created by the system when the
     * bridge device object is created.
     */
    bxm_gw_module_id_t _gw_module_id;

    /**
     * @brief A physical index of the module based on the configuration of
     * the BridgeX device.
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the bridge_device or the gateway API. The index is queried from the
     * Bridge device configuration when the bridge device object is created. 
     */
    bxm_gw_module_index_t _gw_module_index;

    /**
     * @brief Protocol of the internal port of the gateway module
     * @note All internal ports of a module should have the same protocol.
     */
    bxm_protocol_t int_protocol;

    /**
     * @brief Protocol of the external port of the gateway
     * @note All external ports of a module should have the same protocol.
     */
    bxm_protocol_t ext_protocol;

    /**
     * @brief Number of Internal Ports in a Gateway module.
     * @note This is a read-only property i.e. a user cannot edit it using
     * the bridge device API. This property is queried by the system from the
     * physical configuration of the bridge device when the bridge device
     * object is created.
     */
    uint8_t _num_int_ports;

    /**
     * @brief Number of External Ports in a Gateway module.
     * @note This is a read-only property i.e. a user cannot edit it using
     * the bridge device API. This property is queried by the system from the
     * physical configuration of the bridge device when the bridge device
     * object is created.
     */
    uint8_t _num_ext_ports;

    /**
     * @brief List of internal ports in a Gateway module.
     * @note This is a read-only property i.e. a user cannot edit it using
     * the bridge device API. This property is queried by the system from the
     * physical configuration of the bridge device when the bridge device
     * object is created.
     */
    bxm_port_index_t *_int_port;

    /**
     * @brief List of external ports in a Gateway module.
     * @note This is a read-only property i.e. a user cannot edit it using
     * the bridge device API. This property is queried by the system from the
     * physical configuration of the bridge device when the bridge device
     * object is created.
     */
    bxm_port_index_t *_ext_port;

    /**
     * @brief Number of Gateways per Gateway module.
     * @note This is a read-only property i.e. a user cannot edit it using
     * the bridge device API. This property is queried by the system from the
     * physical configuration of the bridge device when the bridge device
     * object is created.
     * @note The number of gateways should be equal to the number of
     * gateways.
     */ 
    uint8_t num_gw;

    /**
     * @brief IDs of Gateways in the Gateway module.
     * @note This is a read-only property i.e. a user cannot edit it using
     * the bridge device API. This property is created by the system when
     * the gateway objects for the bridge device is created.
     */
    bxm_gw_id_t *_gw_id;

    /**
     * @brief Bitmask to identify the bitmask of properties that can be
     * edited by the API. This is a constant value defined by the system
     * when the object is created.
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the bridge device API.
     */
    bxm_component_mask_t _component_mask;
}
bxm_gw_module_attr_t;


/**
 * @brief A bitmask structure for the properties of a Gateway module.
 * A bit set to 1 means that the corresponding data object has been
 * selected.
 */
typedef struct
{
    uint8_t gw_module_id:1;
    uint8_t gw_module_index:1;
    uint8_t int_protocol:1;
    uint8_t ext_protocol:1;
    uint8_t num_int_ports;
    uint8_t num_ext_ports;
    uint8_t int_ports;
    uint8_t ext_ports;
    uint8_t num_gw;
    uint8_t gw;
    uint8_t component_mask:1;
}
bxm_gw_module_attr_bitmask_t;


/**
 * @brief A structure for the properties specific to a BridgeX Device.
 */
typedef struct
{
    /**
     * @brief A BridgeX device identifier.
     * @note This is a read-only property i.e. a user cannot edit it using
     * the gateway API. This identifier is queried by the system from the
     * physical configuration of the bridge device when the bridge device
     * object is created.
     */
    bxm_bd_guid_t _bd_guid;

    /**
     * @brief A user defined description of the BridgeX device.
     */
    char desc[64];

    /**
     * @brief The state of the Bridge device
     * 
     * @note This is a read-only property i.e. a user cannot edit it using
     * the bridge_device API. This property can only be modified by the system
     * when the bridge device change its state. 
     */
    bxm_state_t _state;

    /**
     * @brief The running mode of the Bridge device
     * @note By default the running mode of the device is OFFLINE when it is
     * created.
     */
    bxm_running_mode_t running_mode;

    /**
     * @brief Number of Gateway Modules
     * @note This is a read-only property i.e. a user cannot edit it using
     * the bridge device API. This property is queried by the system from
     * the physical configuration of the bridge device when the bridge device
     * object is created.
     */
    uint8_t _num_gw_module;

    /**
     * @brief Gateway modules in a BridgeX device
     * @note This is a read-only property i.e. a user cannot edit it using
     * the bridge device API. This property is queried by the system from
     * the physical configuration of the bridge device when the bridge device
     * object is created.
     */
    bxm_gw_module_index_t *_gw_module_index;

    /**
     * @brief Firmware version installed on the Bridge device.
     * @note This is a read-only property i.e. a user cannot edit it using
     * the bridge device API. This property is queried by the system from
     * the physical configuration of the bridge device when the bridge device
     * object is created.
     */
    char _firmware_version[64];

    /**
     * @brief Time since last keepalive from the BridgeX device.
     * @note This is a read-only property i.e. a user cannot edit it using
     * the bridge device API.
     * @note The keep-alives are valid only if BXM is running in remote
     * mode. In local mode the health of the bridge device is measured by
     * sending a command to the bridge device and check the result.
     */
    time_t _last_keep_alive;

    /**
     * @brief The GUID of the BXM of which the device is associated with.
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the gateway API. This property can only be modified by the system when
     * a bridge device is associated to a BXM.
     */
    bxm_guid_t _bxm_guid;

    /**
     * @brief Bitmask to identify the bitmask of properties that can be
     * edited by the API. This is a constant value defined by the system
     * when the object is created.
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the bridge device API.
     */
    bxm_component_mask_t _component_mask;
}
bxm_bd_attr_t;


/**
 * @brief A bitmask structure for the properties of a BridgeX Device.
 * A bit set to 1 means that the corresponding data object has been
 * selected.
 */
typedef struct
{
    uint8_t guid:1;
    uint8_t desc:1;
    uint8_t state:1;
    uint8_t running_mode:1;
    uint8_t num_gw_module;
    uint8_t gw_module_index:1;
    uint8_t firmware_version:1;
    uint8_t last_keep_alive:1;
    uint8_t bxm_guid:1;
    uint8_t component_mask:1;
}
bxm_bd_attr_bitmask_t;


/**
 * @brief Special, reserved bitmask values used as Information class of
 * gateway properties.
 */
enum BridgeDeviceInfoClassEnum
{
    /**
     * @brief The bitmask to select no properties of the bridge device.
     * @note This bitmask is used while querying the inventory of bridge
     * device.
     */
    BRIDGE_INFO_NONE		= 0x00,

    /**
     * @brief The bitmask selects of the following properties of the
     * bridge device:
     * 		- Guid, num_module, internal/external port ids and protocol
     */
    BRIDGE_INFO_SUMMARY	= 0x01,

    /**
     * @brief The bitmask selects of all the properties of the bridge device.
     * This bitmask is only valid for query functions. 
     */
    BRIDGE_INFO_ALL_READ       = 0x03,

    /**
     * @brief The bitmask selects of all the editable properties of the
     * bridge device. This is usefull for edit functions to modify all editable
     * properties. 
     */
    BRIDGE_INFO_ALL_WRITE      = 0x04,

    /**
     * @brief This bitmask selects of the state and the running mode of the
     * bridge device.
     */
    BRIDGE_INFO_STATE		= 0x04,

    /**
     * @brief This bitmask selects of the running mode of the bridge
     * device.
     */
    BRIDGE_INFO_RUNNING_MODE	= 0x05,

    /**
     * @brief This bitmask selects the gateway module and gateway information
     * of the bridge device. 
     */
    BRIDGE_INFO_GATEWAY	= 0x06,

}
bxm_bd_info_class_t;


/**
 * @brief Initiate the discovery of the Bridge devices.
 *
 * The BridgeX Manager will start advertizing itself in the network and
 * listen to solicitations from the gateways. The identities of the 
 * discovered devices and their Gateways will be added to the BXM's
 * inventory after discovery.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_bd_discover();


/**
 * @brief Edit properties of a bridge device.
 *
 * @param[in] bd_guid The bridge device ID of the bridge device to change the
 * 	attributes of.
 * @param[in] bitmask Specifies an bitmask value. The bitmask indicates the
 * 	specific properties of the object to be updated. This bistmask
 * 	flag should be bit-AND'ed against order of attributes in the
 * 	struct bxm_bridge device_attr_t.
 * @param[in] attr Pointer to the structure containing the attrs of the
 * 	bridge device that should be changed.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_bd_edit_general_attr(bxm_bd_guid_t bd_guid,
			    bxm_bd_attr_bitmask_t *bitmask,
			    bxm_bd_attr_t *attr);


/**
 * @brief Edit properties of a gateway module
 *
 * @param[in] bd_guid The ID of the bridge device to change the port attr of.
 * @param[in] module_index Identifies the gateway module on the bridge device.
 * @param[in] bitmask Specifies an bitmask value. The bitmask indicates the
 * 	specific properties of the port to be updated. This bistmask
 * 	flag should be bit-AND'ed against order of attributes in the
 * 	struct bxm_port_attr_t.
 * @param[in] attr Pointer to the structure containing the attrs of the
 * 	bridge device port that should be changed.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_bd_edit_gw_module_attr(bxm_bd_guid_t bd_guid,
				bxm_gw_module_index_t module_index,
				bxm_gw_module_attr_bitmask_t *bitmask,
				bxm_gw_module_attr_t *attr);


/**
 * @brief Edit properties of a bridge device port
 *
 * @param[in] bd_guid The ID of the bridge device to change the port attr of.
 * @param[in] port_index Identifies the port on the bridge device.
 * @param[in] bitmask Specifies an bitmask value. The bitmask indicates the
 * 	specific properties of the port to be updated. This bistmask
 * 	flag should be bit-AND'ed against order of attributes in the
 * 	struct bxm_port_attr_t.
 * @param[in] attr Pointer to the structure containing the attrs of the
 * 	bridge device port that should be changed.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_bd_edit_port_attr(bxm_bd_guid_t bd_guid,
				bxm_port_index_t port_index,
				bxm_port_attr_bitmask_t *bitmask,
				bxm_port_attr_t *attr);


/**
 * @brief Change the running mode of the bridge device to ONLINE.
 *
 * @note This operation modifies the running mode of the bridge device to
 * ONLINE. It means that BXM can activate the bridge device and start the
 * services of the bridge device i.e. it will turn the gateways, vfabrics and
 * the corresponding vadapters as ACTIVE.
 *
 * @param[in] bd_guid The guid of the bridge device to be activated.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_bd_online(bxm_bd_guid_t bd_guid);


/**
 * @brief Change the running mode of the bridge device to OFFLINE.
 *
 * @note This operation modifies the running mode of the bridge device to
 * OFFLINE. It means that BXM can disable the bridge device and stop the
 * services of the bridge device. It will also turn the gateways, vfabrics and
 * the corresponding vadapters as DISABLED. 
 *
 * @param[in] gw_id The id of the bridge device to be disabled.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_bd_offline(bxm_gw_id_t gw_id);


/**
 * @brief Query the inventory of BridgeX device
 *
 * @param[in] attr Pointer to the structure containing the attrs of the
 * 	bridge device that should match with the properties in the
 * 	inventory.
 * @param[in] bitmask Specifies the bitmask value. This bitmask flag should be
 * 	bit-AND'ed against the given attr to select the attributes in the
 * 	bxm_bd_attr_t struct that should match the attributes of the
 * 	resource in the inventory. To select all resources unconditionally
 * 	the bitmast should be 0.
 * @param[in] num_result Number of resource objects allocated in result. If
 *	value of num_result is 0, the attribute changes its property and
 *	becomes an output parameter. The function will store the number of
 *	objects in the inventory at the location to which this argument points
 * @param[out] result Pointer to an array of bridge device attribute struct.
 * 	It is the responsibility of the caller to allocate and free memory of
 * 	the strucutres. The results found are stored at the location to which
 * 	this argument points. NULL will mark the end of the list, If there are
 * 	less than num_results objects found in the inventory.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_bd_select_inventory(bxm_bd_attr_t *attr,
				    bxm_bd_attr_bitmask_t bitmask,
				    uint32_t *num_result,
				    bxm_bd_attr_t *result[]);


/**
 * @brief Query the properties of a specific BridgeX device
 *
 * @param[in] bd_guid The GUID of the Bridge device
 * @param[in] bitmask Specifies the bitmask value. This bitmask flag should be
 * 	bit-AND'ed against order of attributes in the struct bxm_bd_attr_t.
 *
 * @param[out] result Pointer to the struct of bridge device attribute.
 * 	The caller would be responsibe to allocate and free memory of
 * 	the strucuture. The result is stored at the location to which
 * 	this argument points. If no match is found or on error, NULL will be
 * 	stored in this location.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_bd_query_general_attr(bxm_bd_guid_t bd_guid,
				    bxm_bd_attr_bitmask_t bitmask,
				    bxm_bd_attr_t *result);


/**
 * @brief Query the properties of a Gateway module of a BridgeX device
 *
 * @param[in] bd_guid The GUID of the Bridge device
 * @param[in] num_gw_module Number of gateway modules in the query list.
 * @param[in] gw_module_id The list of IDs of the Gateway module in the Bridge
 * 	device, whose properties are queried.
 * @param[out] result The pointer to the struct of gateway module properties.
 * 	The caller would be responsibe to allocate and free memory of
 * 	the structure. The result is stored at the location to which
 * 	this argument points. NULL will mark the end of the list, If there are
 * 	less than num_gw_module objects found in the inventory.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_bd_query_gw_module_attr(bxm_bd_guid_t bd_guid,
				    uint8_t num_gw_module,
				    bxm_gw_module_id_t gw_module_id[],
				    bxm_gw_module_attr_t *result[]);


/**
 * @brief Query the properties of a port in a BridgeX device
 *
 * @param[in] bd_guid The GUID of the Bridge device
 * @param[in] num_ports Number of port id requested in the port_id[]
 * @param[in] port_id The Array of IDs of the ports in the Bridge device
 *
 * @param[out] result The pointer to the struct of port properties.
 * 	The caller would be responsibe to allocate and free memory of
 * 	the structure. The result is stored at the location to which
 * 	this argument points. NULL will mark the end of the list, If there are
 * 	less than num_gw_module objects found in the inventory.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_bd_query_port_attr(bxm_bd_guid_t bd_guid,
				    uint8_t num_ports,
				    bxm_port_id_t port_id[],
				    bxm_port_attr_t *result[]);

/* TODO: Add edit functions to edit some properties e.g protocol */

#endif /* INCLUDED_bxm_resource_functions */
