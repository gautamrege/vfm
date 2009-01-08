/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#ifndef INCLUDED_vfm_gateway
#define INCLUDED_vfm_gateway

#include "vfm_common.h"
#include "vfm_error.h"
#include "vfm_port.h"
#include "vfm_state.h"
#include "vfm_vfabric.h"


/**
 * @file
 * @brief API to manage a Gateway resource.
 *
 * This file contains attributes and function prototypes for managing a
 * Gateway resource.
 */

/**
 * @brief A structure for the properties of a BridgeX Gateway
 */
typedef struct
{
    /**
     * @brief A unique id for the Gateway.
     * @note This is a read-only property i.e. a user cannot edit it using
     * the gateway API. This identifier is created by the system when the
     * gateway object is created.
     */
    vfm_gw_id_t _gw_id;

    /**
     * @brief A physical index of the Gateway based on the configuration of
     * the BridgeX device.
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the gateway API. The index is queried from the Bridge device
     * configuration when the gateway object is created.
     */
    char _physical_index[8];

    /**
     * @brief A user defined description of the Gateway.
     */
    char desc[8];

    /**
     * @brief The ID of the Gateway module.
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the gateway API. The module id is queried from the Bridge device
     * configuration when the gateway object is created.
     */
    vfm_gw_module_id_t _gw_module_id;

    /**
     * @brief The state of the gateway
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the gateway API. This property can only be modified by the system when
     * the gateway change its state.
     */
    vfm_state_t _state;

    /**
     * @brief The running mode of the gateway
     * @note By default the running mode of the gateway is OFFLINE when it is
     * created.
     */
    vfm_running_mode_t running_mode;

    /**
     * @brief Protocol of the internal port of the gateway
     * @note This is a read-only property i.e. a user cannot edit it using
     * the gateway API. This property is derived from the internal port id of
     * the gateway
     */
    vfm_protocol_t _int_protocol;

    /**
     * @brief Protocol of the external port of the gateway
     * @note This is a read-only property i.e. a user cannot edit it using
     * the gateway API. This property is derived from the external port id of
     * the gateway
     */
    vfm_protocol_t _ext_protocol;

    /**
     * @brief Internal port of the gateway
     * @note This is a read-only property i.e. a user cannot edit it using
     * the gateway API. This property is set by the system when the gateway is
     * created. 
     */
    vfm_port_index_t _int_port;

    /**
     * @brief External port in the gateway
     * @note This is a read-only property i.e. a user cannot edit it using
     * the gateway API. This property is set by the system when the gateway is
     * created. 
     */
    vfm_port_index_t _ext_port;

    /**
     * @brief Number of virtual fabrics the gateway is a member of.
     * @note This is a read-only property i.e. a user cannot edit it using
     * the gateway API. This property is set by the system when the gateway is
     * created. 
     */
    uint32_t _num_vfabrics;

    /**
     * @brief List of virtual Fabric that the Gateway is a member of.
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the gateway API. This property can only be modified by the system when
     * a gateway is assigned to a vfabric.
     *
     * @note A gateway cannot be configured as the primary gateway of more
     * than one EN vfabric with same VLAN.
     * @note A gateway cannot be configured as the primary gateway of only
     * one FC vfabric at a time. (TBD: This restriction may not be necessary).
     */
    vfm_vfabric_id_t *_vfabric_list;

    /**
     * @brief Number of vadapters in the fabric.
     * @note This is a read-only property i.e. a user cannot edit it using
     * the gateway API. This property is set by the system when the gateway is
     * created. 
     */
    uint32_t _num_vadapters;

    /**
     * @brief List of virtual adapter that are associatd with the Gateway
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the gateway API. This property can only be modified by the system when
     * a gateway is assigned to a vfabric.
     *
     * @note The list of vadapters is derived from the vfabrics that the
     * gateway is a member of
     */
    vfm_vadapter_id_t *_vadapter_list;

    /**
     * @brief Flag to indicate that Ingress learning mode is enabled
     * See Section 2.6.2 of PRM
     */
    uint8_t ingress_learning;

    /**
     * @brief Flag to indicate that Ingress secure mode is enabled
     * See Section 2.6.3 of PRM
     */
    uint8_t ingress_secure;

    /**
     * @brief Flag to indicate that Egress secure mode is enabled
     * See Section 2.6.4 of PRM
     */
    uint8_t egress_secure;

    /**
     * @brief Flag to indicate if Flood unknown is allowed
     * See Section 2.6.5 of PRM
     */
    uint8_t flood;

    /**
     * @brief Flag to indicate that the Gateway will validate the checksum of
     * ingress packets.
     * See section 1.2.3.2 in the PRM
     */
    uint8_t checksum_offload;

    /**
     * @brief Time since the last keepalive from the gateway (applies to
     * Remote VFM only).
     */
    time_t last_keep_alive;

    /**
     * @brief The GUID of the VFM of which the Gateway is associated with.
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the gateway API. This property can only be modified by the system when
     * a gateway is associated to a VFM.
     */
    vfm_guid_t _vfm_guid;

    /**
     * @brief Bitmask to identify the bitmask of properties that can be
     * edited by the API. This is a constant value defined by the system
     * when the object is created.
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the gateway API.
     */
    vfm_component_mask_t  _component_mask;
}
vfm_gateway_attr_t;


/**
 * @brief A bitmask structure for the properties of a BridgeX Gateway.  A
 * bit set to 1 means that the corresponding data object has been
 * selected.
 */
typedef struct
{
    uint8_t gw_id:1;
    uint8_t physical_index:1;
    uint8_t desc:1;
    uint8_t gw_module_id:1; 
    uint8_t state:1;
    uint8_t running_mode:1;
    uint8_t int_protocol:1;
    uint8_t ext_protocol:1;
    uint8_t int_port:1;
    uint8_t ext_port:1;
    uint8_t vfabric_list:1;
    uint8_t vadapter_list:1;
    uint8_t ingress_learning:1;
    uint8_t ingress_secure:1;
    uint8_t egress_secure:1;
    uint8_t flood:1;
    uint8_t checksum_offload:1;
    uint8_t last_keep_alive:1;
    uint8_t vfm_guid:1;
    uint8_t component_mask;
}
vfm_gateway_bitmask_t;


/**
 * @brief Special, reserved bitmask values used as Information class of
 * gateway properties.
 */
typedef enum GatewayInfoClassEnum
{
    /**
     * @brief The bitmask to select no properties of the gateway.
     * @note This bitmask is used while querying the inventory of gateway
     */
    GW_INFO_NONE		= 0x00,

    /**
     * @brief The bitmask selects of the following properties of the
     * gateway:
     * 		- Id, physical_index, internal/external protocol
     */
    GW_INFO_SUMMARY	= 0x01,

    /**
     * @brief The bitmask selects of all the properties of the gateway. This
     * is only valid for query functions. 
     */
    GW_INFO_ALL_READ       = 0x02,

    /**
     * @brief The bitmask selects of all the editable properties of the
     * gateway. This is usefull for edit functions to modify all editable
     * properties. 
     */
    GW_INFO_ALL_WRITE      = 0x03,

    /**
     * @brief This bitmask selects of the state and the running mode of the
     * gateway.
     */
    GW_INFO_STATE          = 0x04,

    /**
     * @brief This bitmask selects of the running mode of the gateway.
     */
    GW_INFO_RUNNING_MODE   = 0x05,

    /**
     * @brief This bitmask selects the list of vfabric that the gateway is
     * a member of.
     */
    GW_INFO_VIRTUAL_FABRIC = 0x06,

    /**
     * @brief This bitmask selects the list of vadapters asociated with 
     * the gateway (vfabric).
     */
    GW_INFO_VIRTUAL_ADAPTER = 0x07,
}
vfm_gateway_info_class_t;


/**
 * @brief Edit properties of a gateway.
 *
 * @param[in] gw_id The gateway ID of the gateway to change the attributes of.
 * @param[in] bitmask Specifies an bitmask value. The bitmask indicates the
 * 	specific properties of the object to be updated. This bistmask
 * 	flag should be bit-AND'ed against order of attributes in the
 * 	struct vfm_gateway_attr_t.
 * @param[in] attr Pointer to the structure containing the attrs of the
 * 	gateway that should be changed.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
vfm_error_t vfm_gateway_edit_general_attr(vfm_gw_id_t gw_id,
			    vfm_gateway_bitmask_t *bitmask,
			    vfm_gateway_attr_t *attr);


/**
 * @brief Edit properties of a gateway port
 *
 * @param[in] gw_id The gateway ID of the gateway to change the port attr of.
 * @param[in] port_type Identifies whether the port is an INTERNAL or EXTERNAL
 * 		port.
 * @param[in] bitmask Specifies an bitmask value. The bitmask indicates the
 * 	specific properties of the port to be updated. This bistmask
 * 	flag should be bit-AND'ed against order of attributes in the
 * 	struct vfm_port_attr_t.
 * @param[in] attr Pointer to the structure containing the attrs of the
 * 	gateway port that should be changed.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
vfm_error_t vfm_gateway_edit_port_attr(vfm_gw_id_t gw_id,
				vfm_port_type_t port_type,
				vfm_port_attr_bitmask_t *bitmask,
				vfm_port_attr_t *attr);

/**
 * @brief Change the running mode of the vfabric to ONLINE.
 *
 * @note This operation modifies the running mode of the gateway to ONLINE. It
 * means that VFM can activate the gateway and start the services of the
 * gateway i.e. it will turn the vfabrics and the corresponding vadapters as
 * ACTIVE and will start bridging the packets between the internal and external
 * ports.
 *
 * @note After the call VFM will activate the gateway and change the status to
 * ONLINE/ACTIVE. Once the gateway is ACTIVE, the corresponding vfabrics and
 * vadapters can be ACTIVE
 *
 * @param[in] gw_id The id of the gateway to be activated.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
vfm_error_t vfm_gateway_online(vfm_gw_id_t gw_id);


/**
 * @brief Change the running mode of the vfabric to OFFLINE.
 *
 * @note This operation modifies the running mode of the gateway to OFFLINE. It
 * means that VFM can disable the gateway and stop the services of the
 * gateway. It will also turn the vfabrics and the corresponding vadapters as
 * DISABLED.
 *
 * @note After the call VFM will deactivate the gateway and change the status
 * OFFLINE/DISABLED. Once the gateway is DISABLED, the corresponding vfabrics
 * and vadapters are also DISABLED.
 * 
 * @param[in] gw_id The id of the gateway to be disabled.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
vfm_error_t vfm_gateway_offline(vfm_gw_id_t gw_id);


/**
 * @brief Get the list of existing gateways in the local inventory
 *
 * @param[in] attr Pointer to the structure containing the attrs of the
 * 	gateway that should match with the properties in the inventory.
 * @param[in] bitmask Specifies the bitmask value. This bitmask flag should be
 * 	bit-AND'ed against the given attr to select the attributes in the
 * 	vfm_gateway_attr_t struct that should match the attributes of the
 * 	resource in the inventory. To select all resources unconditionally
 * 	the bitmast should be 0.
 * @param[in] num_result Number of resource objects allocated in result. If
 *	value of num_result is 0, the attribute changes its property and
 *	becomes an output parameter. The function will store the number of
 *	objects in the inventory at the location to which this argument points
 * @param[out] result Pointer to an array of gateway attribute struct.
 * 	It is the responsibility of the caller to allocate and free memory of
 * 	the strucutres. The results found are stored at the location to which
 * 	this argument points. NULL will mark the end of the list, If there are
 * 	less than num_results objects found in the inventory.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
vfm_error_t vfm_gateway_select_inventory(vfm_gateway_attr_t attr,
			    vfm_gateway_bitmask_t bitmask,
			    uint32_t *num_result,
			    vfm_gw_id_t *result[]);


/**
 * @brief Query the properties of a gateway in the local configuration
 *
 * @param[in] gw_id The id of the gateway.
 * @param[in] bitmask Specifies an bit mask value. The bitmask
 * 	indicates the specific properties of the object to be queried.
 *
 * @param[out] result Pointer to the struct of gateway attribute.
 * 	The caller would be responsibe to allocate and free memory of
 * 	the structure. The result is stored at the location to which
 * 	this argument points. If no match is found or on error, NULL will be
 * 	stored in this location.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
vfm_error_t vfm_gateway_query_general_attr(vfm_gw_id_t gw_id,
			    vfm_gateway_bitmask_t bitmask,
			    vfm_gateway_attr_t *result);


/**
 * @brief Query the properties of a gateway port
 *
 * @param[in] gw_id The id of the gateway.
 * @param[in] port_type Identifies whether the port is an INTERNAL or EXTERNAL
 * 		port.
 * @param[in] bitmask Specifies an bitmask value. The bitmask indicates the
 * 	specific properties of the port to be queried. This bitmask
 * 	flag should be bit-AND'ed against order of attributes in the
 * 	struct vfm_port_attr_t.
 * @param[out] result The pointer to the struct of port properties.
 * 	The caller would be responsibe to allocate and free memory of
 * 	the structure. The result is stored at the location to which
 * 	this argument points. On error, NULL will be stored in this location.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
vfm_error_t vfm_gateway_query_port_attr(vfm_gw_id_t gw_id,
				vfm_port_type_t port_type,
				vfm_port_attr_bitmask_t *bitmask,
				vfm_port_attr_t *result);
#endif /* INCLUDED_vfm_gateway */
