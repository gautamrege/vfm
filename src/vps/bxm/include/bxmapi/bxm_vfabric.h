/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#ifndef INCLUDED_bxm_vfabric
#define INCLUDED_bxm_vfabric

#include "bxm_common.h"
#include "bxm_error.h"
#include "bxm_gateway.h"
#include "bxm_port.h"
#include "bxm_state.h"
#include "bxm_vadapter.h"


/**
 * @file
 * @brief API to manage virtual fabric resource.
 *
 * This file contains attribues and function prototypes of managing a Virtual
 * Fabric resource.
 */

/**
 * @brief A context table id. This is a unique identifier for a context table.
 * This id is unique within a BXM context and auto-generated by the
 * system.
 */
typedef uint32_t bxm_ctx_table_id_t;

/**
 * @brief A structure for the Ethernet properties specific to a vFabric. 
 */
typedef struct
{
    /**
     * @brief MAC address of the assigned gateway's external port.
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the vfabric API. This property is derived from the EN properties of the
     * associated gateway's external port.  
     * 
     * @note In the case of a fail-over (if HA is configured), this value is
     * updated to the MAC address of the backup gateway.  
     */
    en_mac_addr_t _mac;

    /**
     * @brief VLAN Id configured for the EN vfabric.
     * 
     * @note This is a mandotory field. A user cannot activate the EN vfabric
     * withou specifying a VLAN for the vfabric.
     * @note A user can configure multiple vfabric wih same VLAN.
     */
    vlan_id_t vlan;

    /**
     * @brief Bitmask to identify the bitmask of properties that can be
     * edited by the API. This is a constant value defined by the system
     * when the object is created.
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the vfabric API.
     */
    bxm_component_mask_t _component_mask;
}
bxm_vfabric_en_attr_t;

/**
 * @brief A structure for the Fibre Channel properties of a vfabric. 
 */
typedef struct
{
    /**
     * @brief World Wide Node Name of the assigned gateway's external port
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the vfabric API. This property is derived from the FC properties of the
     * associated gateway's external port.  
     * 
     * @note In the case of a fail-over (if HA is configured), this value is
     * updated to the WWNN of the backup gateway.
     */
    uint64_t _wwnn;

    /**
     * @brief World Wide Port Name of the assigned gateway's external port
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the vfabric API. This property is derived from the FC properties of the
     * associated gateway's external port.  
     * 
     * @note In the case of a fail-over (if HA is configured), this value is
     * updated to the WWPN of the backup gateway.  
     */
    uint64_t _wwpn;

    /**
     * @brief 24 bit FC ID of the assigned Gateway's external port
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the vfabric API. This property is derived from the FC properties of the
     * associated gateway's external port.  
     * 
     * @note In the case of a fail-over (if HA is configured), this value is
     * updated to the FCID of the backup gateway.  
     */
    fc_id_t _fcid;

    /**
     * @brief Bitmask to identify the bitmask of properties that can be
     * edited by the API. This is a constant value defined by the system
     * when the object is created.
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the vadapter API.
     */
    bxm_component_mask_t _component_mask;
}
bxm_vfabric_fc_attr_t;

/**
 * @brief A structure for the properties of a virtual fabric.
 */
typedef struct
{
    /**
     * @brief Unique id for virtual fabric
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the vfabric API. This identifier is created by the system when the
     * vfabric is created.
     */
    bxm_vfabric_id_t _vfabric_id;

    /**
     * @brief User defined name for virtual fabric.
     *
     * @note The name of the vfabric should be unique within a BXM context.
     * @note This is a mandotory field. A user cannot create the vfabric
     * without specifying a unique name for the vfabric.
     */
    char name[64];

    /**
     * @brief User defined description for virtual fabric.
     * 
     * @note This is a optional field.
     */
    char desc[64];

    /**
     * @brief User defined protocol of the virtual fabric.
     *
     * @note This is a mandotory field. A user cannot create the vfabric
     * without specifying a protocol for the vfabric.
     */
    bxm_protocol_t protocol;

    /**
     * @brief State of virtual fabric.
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the vfabric API. This property can only be modified by the system when
     * the vfabric change its state.
     */
    bxm_state_t _state;

    /**
     * @brief Running mode of the virtual fabric.
     *
     * @note By default the running mode of the vfabric is OFFLINE when it is
     * created.
     */
    bxm_running_mode_t  running_mode;

    /**
     * @brief The context table of the vfabric
     * @note This is a read-only property i.e. a user cannot edit it using
     * the vfabric API. This property can only be modified by the system when
     * the vfabric change its state.
     */
    bxm_ctx_table_id_t _ctx_table_id;

    /**
     * @brief Protocol specific properties of the vfabric
     */
    union
    {
	bxm_vfabric_en_attr_t en_attr;
	bxm_vfabric_fc_attr_t fc_attr;
    };

    /**
     * @brief The number of vadapters associated with the vfabric. 
     * 
     * @note This is a read-only property i.e. a user cannot edit it using
     * the vfabric API. This property can only be modified by the system when
     * a vadapter joins or leave a vfabric.
     */
    uint32_t _num_vadapter;

    /**
     * @brief The list of vadapters associated with the vfabric. 
     * 
     * @note This is a read-only property i.e. a user cannot edit it using
     * the vfabric API. This property can only be modified by the system when
     * a vadapter joins or leave a vfabric.
     */
    bxm_vadapter_id_t *_vadapter_id;

    /**
     * @brief The Id of the primary gateway associated with the vfabric.
     * 
     * @note The protocol of the gateway should match with the protocol of
     * the vfabric
     * @note A gateway cannot be configured as the primary gateway of more
     * than one EN vfabric with same VLAN. 
     */
    bxm_gw_id_t primary_gateway;

    /**
     * @brief The id of the backup gateway associated with the vfabric
     *
     * @note The backup gateway should have compatible protocol attributes
     * with the primary gateway.
     *
     * @note When a gateway is a member of HA group, it cannot change its
     * properties that are in-compatible with its peer gateway. The user
     * has to first break the cluster to change the gateway property.
     */
    bxm_gw_id_t backup_gateway;

    /**
     * @brief The HA state of the current vfabric
     *
     * @note This field identifies whether the primary or the backup gateway
     * is in use. 
     * 
     * @note This property is modified when the fail-over or fail-back of
     * gateway occurs.
     * @note If auto_failback or auto_failover is enabled, this state is
     * read-only for the API user. Otherwise a user can edit this field to
     * force a fail-over or fail-back.
     */ 
    bxm_ha_state_t ha_state;

    /**
     * @brief Flag to indicate if failover is automatic or manual.
     *
     * @note The backup gateway should be ONLINE for automatic fail-over to
     * succeed
     * @note By default this property is DIABLED. 
     */
    uint8_t auto_failover;

    /**
     * @brief Flag to indicate if failback is automatic or manual.
     *
     * @note The primary gateway should be ONLINE for automatic fail-back to
     * succeed
     * @note By default this property is DIABLED. 
     */
    uint8_t auto_failback;

    /**
     * @brief Bitmask to identify the bitmask of properties that can be
     * edited by the API. This is a constant value defined by the system
     * when the object is created.
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the vadapter API.
     */
    bxm_component_mask_t _component_mask;
}
bxm_vfabric_attr_t;


/**
 * @brief  A bitmask structure for the vFabric properties of the vfabric.
 */
typedef struct
{
    uint8_t bxm_vfabric_id:1;
    uint8_t name:1;
    uint8_t desc:1;
    uint8_t state:1;
    uint8_t running_mode:1;
    uint8_t ctx_table_id:1;
    uint8_t protocol_attr:1;
    uint8_t vadapter_id:1;
    uint8_t primary_gateway:1;
    uint8_t backup_gateway:1;
    uint8_t ha_state:1;
    uint8_t auto_failback:1;
    uint8_t component_mask:1;
}
bxm_vfabric_attr_bitmask_t;


/**
 * @brief Information class of vfabric properties. Information class
 * represents pre-defined bitmasks to access specific properties of the
 * object.
 */
typedef enum VFabricInfoClassEnum
{
    /**
     * @brief The bitmask to select no properties of the vfabric.
     * @note This bitmask is used while querying the inventory of vfabric
     */
    VFABRIC_INFO_NONE		= 0x00,

    /**
     * @brief The bitmask selects of the following properties of the
     * vfabric:
     * 		- Id, name, desc, protocol
     */
    VFABRIC_INFO_SUMMARY	= 0x01,

    /**
     * @brief The bitmask selects of all the properties of the vfabric. This
     * is only valid for query functions. 
     */
    VFABRIC_INFO_ALL_READ       = 0x02,

    /**
     * @brief The bitmask selects of all the editable properties of the
     * vfabric. This is usefull for edit functions to modify all editable
     * properties. 
     */
    VFABRIC_INFO_ALL_WRITE      = 0x03,

    /**
     * @brief This bitmask selects of the protocol of the vfabric.
     */
    VFABRIC_INFO_PROTOCOL	= 0x04,

    /**
     * @brief This bitmask selects of the state and the running mode of the
     * vfabric.
     */
    VFABRIC_INFO_STATE		= 0x05,

    /**
     * @brief This bitmask selects of the running mode of the vfabric.
     */
    VFABRIC_INFO_RUNNING_MODE	= 0x06,

    /**
     * @brief Bitmask to query or edit the primary gateway of the vfabric.
     */
    VFABRIC_INFO_PRIMARY_GATEWAY = 0x07,

    /**
     * @brief Bitmask to query or edit the backup gateway of the vfabric.
     */
    VFABRIC_INFO_HIGH_AVAILABILTY = 0x08,
}
bxm_vfabric_info_class_t;


/**
 * @brief Create a new vfabric.      	
 * 
 * @param[in] name The user-defined name of the virtual fabric.
 * @param[in] desc The user-defined description of the virtual fabric. Can
 * 	be NULL.
 * @param[in] protocol The protocol of the virtual fabric.
 * @param[out] vfabric_id The id of newly allocated vfabric is stored at the
 * 	location to which this argument points. On error, BXM_INVALID_ID will
 * 	be stored at this location.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_vfabric_create(char *name, char *desc, 
				bxm_protocol_t protocol,
				bxm_vfabric_id_t *vfabric);


/**
 * @brief Create a new vfabric by cloning from an existing vfabric object.
 * Refer to the note below for the semantics of cloning a vfabric.
 *
 * @note Cloning of a vfabric results in creating a new vfabric object that
 * inherits the clonable properties of the donor vfabric. Here is the
 * list of vfabric properties that can and cannot be copied from the donor to
 * the clone: 
 * 	- Can: desc, vadapters, VLAN (only for EN Vfabric), fail-over,
 * 	fail-back properties.
 * 	- Cannot: Id, name, Primary/Backup Gateway, state, running_mode,
 * 	ha_state, context-table.
 * @note By default the new clone will be in OFFLINE mode.
 * @note Cloning a vfabric, would in-turn clone the vadapters associated
 * with the vfabric, according to the guidelines of cloning a vadapter.
 * 
 * @param[in] name The user-defined name of the vfabric
 * @param[in] desc The user-defined description of the vfabric
 * @param[out] donor_vfabric_id The id of the donor vfabric from which the new
 * 	vfabric has to be cloned is stored at this location.
 * @param[out] clone_vfabric_id The id of the newly clones vfabric is
 * 	stored at the location to which this argument points. On error,
 * 	BXM_INVALID_ID will be stored at this location.
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_vfabric_clone(char *name, char *desc,
				bxm_vfabric_id_t donor_vfabric_id,
				bxm_vfabric_id_t *clone_vfabric_id);


/**
 * @brief Edit properties of a vfabric.
 *
 * @param[in] vfabric_id The vfabric Id of the vfabric to change the
 * 	attributes of.
 * @param[in] bitmask Specifies a bitmask value of the specific properties
 * 	of the object to be updated. This bitmask flag should be bit-AND'ed
 * 	against order of attributes in the struct bxm_vfabric_attr_t.  
 * @param[in] attr Pointer to the structure containing the attrs of
 * 	the vfabric that should be changed.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_vfabric_edit_general_attr(bxm_vfabric_id_t vfabric_id,
					bxm_vfabric_attr_bitmask_t *bitmask,
					bxm_vfabric_attr_t *attr);


/**
 * @brief Edit protocol specific properties of a vfabric.
 *
 * @param[in] vfabric_id The vfabric Id of the vfabric to change the
 * 	protocol attributes of.
 * @param[in] bitmask Specifies an bitmask value. The bitmask indicates
 *      the specific protocol properties of the object to be updated.  This
 *      bitmask flag should be bit-AND'ed against order of attributes in the
 *      struct bxm_vfabric_en_attr_t or bxm_vfabric_fc_attr_t based on the
 *      protocol type.
 * @param[in] attr Pointer to the structure containing the protocol attrs of
 * 	the vfabric that should be changed.
 *
 * @note The only vfabric protocol property that can be modified is VLAN
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_vfabric_edit_protocol_attr(bxm_vfabric_id_t vfabric_id,
			    void *bitmask /* typecast according to protocol*/,
			    void *attr /* typecase according to protocol */);


/**
 * @brief Change the running mode of the vfabric to ONLINE.
 * 
 * @note This operation modifies the running mode of the vfabric to ONLINE. It
 * means that BXM can activate the vfabric and start the services of the
 * vfabric i.e. it will turn the vadapters as ACTIVE and will start bridging
 * the packets between the host and the destination via the gateway. 
 *
 * @note After the call the BXM system will change the vfabric to ONLINE.
 * If the primary gateway is ONLINE/ACTIVE, the vfabric will also be
 * ACTIVE.
 *
 * @param[in] vfabric_id The id of the vfabric to be activated.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_vfabric_online(bxm_vfabric_id_t vfabric_id);


/**
 * @brief Change the running mode of the vfabric to OFFLINE.
 *
 * This function modifies the running mode of the vfabric to OFFLINE. It
 * means that BXM can de-activate an active vdabric and stop the services of
 * the vfabric i.e. it will turn the active vadapters as DISABLE and will stop
 * bridging the packets between the host and the destination via the gateway.  
 * 
 * After the call the BXM system will change the vfabric to OFFLINE/DISABLE and
 * deactivate the virtual adapters.
 * 
 * @param[in] vfabric_id The id of the vfabric to be activated.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_vfabric_offline(bxm_vfabric_id_t vfabric_id);

/**
 * @brief Get the list of existing vfabric in the local inventory
 *
 * @param[in] attr Pointer to the structure containing the attrs of the
 * 	vfabric that should match with the properties in the inventory.
 * @param[in] bitmask Specifies the bitmask value. This bitmask flag should be
 * 	bit-AND'ed against the given attr to select the attributes in the
 * 	bxm_gateway_attr_t struct that should match the attributes of the
 * 	resource in the inventory. To select all resources unconditionally
 * 	the bitmast should be 0.
 * @param[in] num_result Number of resource objects allocated in result. If
 *	value of num_result is 0, the attribute changes its property and
 *	becomes an output parameter. The function will store the number of
 *	objects in the inventory at the location to which this argument points
 * @param[out] result Pointer to an array of vfabric attribute struct.
 * 	It is the responsibility of the caller to allocate and free memory of
 * 	the strucutres. The results found are stored at the location to which
 * 	this argument points. NULL will mark the end of the list, if there are
 * 	less than num_results objects found in the inventory.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_vfabric_select_inventory(bxm_vfabric_attr_t *attr,
			    bxm_vfabric_attr_bitmask_t bitmask,
			    uint32_t *num_result,
			    bxm_vfabric_id_t *result[]);

/**
 * @brief Query the properties of a vfabric in the local configuration.
 *
 * @param[in] vfabric_id A valid id of the vfabric
 * @param[in] bitmask Specifies an bit mask value. The bitmask indicates the
 * 	specific properties of the object to be queried.
 * @param[out] result Pointer to the struct of vfabric attribute. The caller
 * 	would be responsibe to allocate and free memory of the structure.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_vfabric_query_general_attr(bxm_vfabric_id_t vfabric_id,
					bxm_vfabric_attr_bitmask_t bitmask,
					bxm_vfabric_attr_t *result);


/**
 * @brief Query protocol specific properties of a vfabric.
 *
 * @param[in] vfabric_id The vfabric Id of the vfabric to query the
 * 	protocol attributes of.
 * @param[in] bitmask Specifies a bitmask value of the specific properties
 * 	of the object to be queried. This bitmask flag should be bit-AND'ed
 * 	against order of attributes in the struct bxm_vfabric_attr_t. 
 * @param[out] result Pointer to the struct of vfabric protocol attribute.
 *      The caller would be responsibe to allocate and free  memory of the
 *      structure. The result is stored at the location to which this argument
 *      points. If no match is found or on error, NULL will be stored in this
 *      location. 
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_vfabric_query_protocol_attr(bxm_vfabric_id_t vfabric_id,
			    void *bitmask /* typecast according to protocol*/,
			    void *result /* typecase according to protocol */);


/**
 * @brief Destroy a vfabric.
 * 
 * This destroys the entire vfabric. All underlying associations to virtual
 * I/O adapters and Gateways are removed. When a vfabric is destroyed the
 * configuration of Bridge device and the hosts are updated.
 *
 * @note After a vfabric is destroyed the vadapters that are members of the
 * vfabric are DISABLED. They vadapters will need to rejoin a new vfabric
 * to be activated. 
 *
 * @param[in] vfabric_id The id of the vfabric to be destroyed.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_vfabric_destroy(bxm_vfabric_id_t vfabric);

#endif /* INCLUDED_bxm_vfabric */
