/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#ifndef INCLUDED_bxm_vadapter
#define INCLUDED_bxm_vadapter

#include "bxm_common.h"
#include "bxm_error.h"
#include "bxm_gateway.h"
#include "bxm_state.h"

/**
 * @file
 * @brief Sample BXM virtual I/O adapter API.
 *
 * This file contains attributes and function prototypes for managaing a
 * Virtual Adapter resource.
 */
/**
 * @brief Initiation type of vadapter.
 */
typedef enum VAdapterInitiationTypeEnum
{
    /**
     * @brief Creation of virtual adapters is initiated by host.
     *
     * @note This flag indicates that BXM should initiate the instantiation
     * of the vadapter on the host.
     */
    HOST_INITIATED 	= 0x1,

    /**
     * @brief Creation of virtual adapters is initiated by BXM.
     *
     * @note This flag indicates that BXM should allow the instantiation
     * of the vadapter by the host.
     */
    NETWORK_INITIATED 	= 0x2
}
bxm_init_type_t;

/**
 * @brief Protocol attribute assignment of vadapter.
 */
typedef enum VAdapterAssignmentTypeEnum
{
    /**
     * @brief Protocol properties are auto-generated dynamically by the
     * system for the virtual adapter.
     * @mode This mode implies that the system controls the specific protocol
     * properties. The system can change this property without notifying
     * the user or any user input. E.g, new MAC address is assigned
     * automatically if the system detects MAC collision
     */
    ASSIGNED_AUTO 	= 0x1,

    /**
     * @brief Protocol properties are auto-generated once by the system and
     * remains persistent in the configuration unless explictly modified.
     * @mode This mode implies that the system generates the specific protocol
     * properties once. The system cannot change this property once assigned.
     * The user has to manually assign the MAC address or change the
     * assigned mode to auto-generated.
     */
    ASSIGNED_ONCE	= 0x2,

    /**
     * @brief Protocol properties are manually assigned by the user. The
     * value remains persistent in the configuration unless explictly
     * modified by the user.
     * @mode This mode implies that the user specifies the specific protocol
     * properties once. The system cannot change this property.
     */
    ASSIGNED_MANUAL	= 0x3
}
bxm_assignment_type_t;

/**
 * @brief A structure for the Ethernet properties specific to an vadapter
 */
typedef struct
{
    /**
     * @brief MAC address assigned to a vadapter
     */
    en_mac_addr_t mac;


    /**
     * @brief VLAN assigned to a vadapter.
     * Note: Can a vadapter have more than 1 VLAN
     */
    vlan_id_t vlan;

    /**
     * @brief Max transmission unit of the vNIC
     * @note By default the MTU is set to TODO 
     */
    uint32_t mtu;

    /**
     * @brief Flag to indicate if the interface is in Promiscuous mode
     * See Section 2.6.5 of PRM
     */
    uint8_t promiscuous_mode;

    /**
     * @brief Flag to indicate if the interface is in silent listener
     * See Section 2.6.6 of PRM
     */
    uint8_t silent_listener;

    /**
     * @brief Bitmask to identify the bitmask of properties that can be
     * edited by the API.
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the vadapter API.
     */
    bxm_component_mask_t _component_mask;
}
bxm_vadapter_en_attr_t;


/**
 * @brief A bitmask structure for the Ethernet properties
 */
typedef struct
{
    uint8_t mac:1;
    uint8_t vlan:1;
    uint8_t mtu:1;
    uint8_t promiscuous_mode:1;
    uint8_t silent_listener:1;
    uint8_t component_mask:1;
}
bxm_vadapter_en_attr_bitmask_t;


/**
 * @brief A structure for the Fibre Channel properties specific to a vHBA.
 */
typedef struct
{
    /**
     * @brief World Wide Node Name of the vadapter
     */
    uint64_t wwnn;

    /**
     * @brief World Wide Port Name of the vadapter
     */
    uint64_t wwpn;

    /**
     * @brief 24 bit FC ID of the vadapter
     */
    fc_id_t fcid;

    /**
     * @brief Max transmission unit of the vHBA
     * @note By default the MTU is set to TODO 
     */
    uint32_t mtu;

    /**
     * @brief Flag to indicate if the vadapter supports Server Provided MAC
     * Address (SPMA)
     */
    uint8_t spma;

    /**
     * @brief Flag to indicate if the vadapter supports Fabric Provided MAC
     * Address (FPMA)
     */
    uint8_t fpma;

    /**
     * @brief Bitmask to identify the bitmask of properties that can be
     * edited by the API.
     */
    bxm_component_mask_t _component_mask;
}
bxm_vadapter_fc_attr_t;


/**
 * @brief A bitmask structure for the Fibre Channel properties
 */
typedef struct
{
    uint8_t wwnn:1;
    uint8_t wwpn:1;
    uint8_t fcid:1;
    uint8_t mtu:1;
    uint8_t spma:1;
    uint8_t fpma:1;
    uint8_t component_mask:1;
}
bxm_vadapter_fc_attr_bitmask_t;


/**
 * @brief A structure for the properties of a virtual I/O adapter
 */
typedef struct
{
    /**
     * @brief A unique virtual IO adapter identifier.
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the vadapter API. This identifier is created by the system when the
     * vdapter is created.
     */
    bxm_vadapter_id_t _vadapter_id;

    /**
     * @brief A user defined name of the virtual IO adapter.
     *
     * @note The name of the vdapter should be unique within a BXM context.
     * @note This is a mandotory field. A user cannot create the vadapter
     * without specifying a unique name for the vadapter.
     */
    char name[64];

    /**
     * @brief A user defined description of the virtual IO adapter.
     *
     * @note This is a optional field. 
     */
    char desc[64];

    /**
     * @brief User defined protocol of the virtual adapter.
     *
     * @note This is a mandotory field. A user cannot create the vadapter
     * without specifying a protocol for the vadapter.
     */
    bxm_protocol_t protocol;

    /**
     * @brief State of virtual adapter.
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the vadapter API. This property can only be modified by the system when
     * the vadapter change its state.
     */
    bxm_state_t _state;

    /**
     * @brief The running mode of the virtual adapter
     *
     * @note By default the running mode of the vadapter is OFFLINE when it is
     * created.
     */
    bxm_running_mode_t running_mode;

    /**
     * @brief The ID of the physical interface (on the host), on which the
     * virtual interface will be created.
     *
     * @note This field is mandatory to be specified by the user for
     * NETWORK_INITIATED vadapter.
     */
    bxm_io_module_t io_module_id;

    /**
     * @brief The attribute to specify if the vadapter is Host Initiated or
     * Network Initiated.
     * 
     * @note By default the initialization type is NETWORK_INITIATED.
     */
    bxm_init_type_t init_type;

    /**
     * @brief The attribute to specify how to assign the protocol properties
     * like MAC address, WWPN and WWNN.
     * 
     * @note By default the assignment type is ASSIGNED_AUTO.
     */
    bxm_assignment_type_t assignment_type;

    /**
     * @brief The protocol specific properties of the virtual adapter
     */
    union
    {
	bxm_vadapter_en_attr_t en_attr;
	bxm_vadapter_fc_attr_t fc_attr;
    };

    /**
     * @brief The id of the vfabric that the vadapter is a member of.
     *
     * @note It is mandatory to join a vfabric before a vadapter can be
     * activated.
     */
    bxm_vfabric_id_t vfabric_id;

    /**
     * @brief The id of the gateway of the vfabric that the vadapter is 
     * a member of.
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the vadapter API. This property can only be modified by the system when
     * the vfabric change its gateway e.g. after fail-over.
     */
    bxm_gw_id_t _gw_id;

    /**
     * @brief Bitmask to identify the bitmask of properties that can be
     * edited by the API.
     *
     * @note This is a read-only property i.e. a user cannot edit it using
     * the vadapter API.
     */
    bxm_component_mask_t _component_mask;
}
bxm_vadapter_attr_t;


/**
 * @brief A bitmask structure for the properties of a Virtual Adapter
 */
typedef struct
{
    uint8_t id:1;
    uint8_t name:1;
    uint8_t desc:1;
    uint8_t protocol:1;
    uint8_t state:1;
    uint8_t running_mode:1;
    uint8_t io_module_id:1;
    uint8_t init_type:1;
    uint8_t assignment_type:1;
    uint8_t protocol_attr:1;
    uint8_t vfabric_id:1;
    uint8_t gw_id:1;
    uint8_t component_mask:1;
}
bxm_vadapter_attr_bitmask_t;


/**
 * @brief Information class of vadapter properties. Information class
 * represents pre-defined bitmasks to access specific properties of the
 * object.
 */
enum VAdapterInfoClassEnum
{
    /**
     * @brief The bitmask to select no properties of the vadapter.
     * @note This bitmask is used while querying the inventory of vadapter
     */
    VADAPTER_INFO_NONE		= 0x00,

    /**
     * @brief The bitmask selects of the following properties of the
     * vadapter:
     * 		- Id, name, desc, protocol, desc, vfabric
     */
    VADAPTER_INFO_SUMMARY	= 0x01,

    /**
     * @brief The bitmask selects of all the properties of the vadapter. Thisi
     * is only valid for query functions. 
     */
    VADAPTER_INFO_ALL_READ       = 0x02,

    /**
     * @brief The bitmask selects of all the editable properties of the
     * vadapter. This is usefull for edit functions to modify all editable
     * properties. 
     */
    VADAPTER_INFO_ALL_WRITE      = 0x03,

    /**
     * @brief This bitmask selects of the protocol of the vadapter.
     */
    VADAPTER_INFO_PROTOCOL	= 0x04,

    /**
     * @brief This bitmask selects of the I/O module Id of the vadapter.
     */
    VADAPTER_INFO_IO_MODULE	= 0x05,

    /**
     * @brief This bitmask selects of the state and the running mode of the
     * vadapter.
     */
    VADAPTER_INFO_STATE		= 0x06,

    /**
     * @brief This bitmask selects of the running mode of the vadapter.
     */
    VADAPTER_INFO_RUNNING_MODE	= 0x07,

    /**
     * @brief Bitmask to query or edit the vfabric id of the vadapter.
     *
     * @note When a vadapter tries to join a vfabric, it is required that the
     * following properties of vadapter and the vfabric do not conflict.
     *		- Protocol: Both vadapter and vfabric should support the
     *		same protocol type.
     *		- VLAN: For EN vadapter, it is requied that the VLAN of the
     *		vadapter and the vlan are the same.
     *		- If any of the above properties are not specified in
     *		either vadapter or vfabric, the vadapter cannot join the
     *		vfabric.
     *
     * @note If an ACTIVE vadapter unjoins a vfabric, the vfabric field
     * will be cleared. The vadapter will stop operating and the its state 
     * will change to UN_INITIALIZED.  
     */
    VADAPTER_INFO_VIRTUAL_FABRIC	= 0x08,

    /**
     * @brief Bitmask to query the gateway used by the vadapter.
     */
    VADAPTER_INFO_GATEWAY	= 0x09,
}
bxm_vadapter_info_class_t;






/**
 * @brief Create a new virtual I/O adapter.
 *
 * This function creates a vadapter object and returns an identifier. It
 * adds the local configuration of a virtual I/O adapter on the BXM. The actual
 * instantiation of the vadapter on the host will not happen until the vadapter
 * properties e.g it is added to a vfabric and it is activated.
 *
 * @param[in] protocol The vadapter protocol is stored at this location.
 * @param[in] name The user-defined name of the vadapter
 * @param[in] desc The user-defined description of the vadapter
 * @param[out] vadapter_id The id of the newly allocated vadapter is stored
 *     stored at the location to which this argument points.  On error,
 *     BXM_INVALID_ID will be stored at this location.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_vadapter_create(char *name, char *desc,
				bxm_protocol_t protocol,
				bxm_vadapter_id_t *vadapter_id);


/**
 * @brief Create a new virtual I/O adapter by cloning from an existing
 * vadapter. Refer to the note below for the semantics of cloning a
 * vadapter.
 *
 * @note Cloning of a vadapter results in creating a new vadapter object that
 * inherits the clonable properties of the donor vadapter. Here is the list
 * of vadapter properties that can and cannot be copied from the donor to
 * the clone: 
 * 	- Can: desc, protocol, io_module_id, init_type, assignment_type,
 * 	vfabric_id, gw_id.
 * 		- EN properties: All except MAC address. See below for
 * 		details.
 * 		- FC properties: All except WWNN, WWPN and FCID. See below
 * 		for details.
 * 	- Cannot: Id, name, state, running_mode.
 * 		- EN properties: MAC address. MAC address for a vadapter is
 * 		generated based on the assignment_type. If the assignment
 * 		type is ASSIGNED_MANUAL, the MAC address is left blank for
 * 		the user to specify. Otherwise the system auto-generated a
 * 		unique MAC for the clone.
 * 		- FC properties: FCID, WWNN and WWNN. WWNs for a vadapter
 * 		is generated based on the assignment_type. If the assignment  
 * 		type is ASSIGNED_MANUAL, the WWNs are left blank for the user
 * 		to specify. Otherwise the system auto-generates unique WWNs
 * 		for the clone.
 * @note By default the new clone will be in OFFLINE mode.
 * 
 * @param[in] name The user-defined name of the vadapter
 * @param[in] desc The user-defined description of the vadapter
 * @param[out] src_vadapter_id The id of the vadapter from which the new
 *     vadapter has to be clones is stored at this location.
 * @param[out] new_vadapter_id The id of the newly allocated vadapter is
 *     stored at the location to which this argument points.  On error,
 *     BXM_INVALID_ID will be stored at this location.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_vadapter_clone(char *name, char *desc,
				bxm_vadapter_id_t src_vadapter_id,
				bxm_vadapter_id_t *new_vadapter_id);


/**
 * @brief Edit properties of a vadapter.
 *
 * @param[in] vadapter_id The id of the virtual adapter to change the
 * 	attributes of.
 * @param[in] bitmask Specifies an bitmask value. The bitmask indicates
 * 	the specific properties of the object to be updated.  This bitmask
 * 	flag should be bit-AND'ed against order of attributes in the struct 
 * 	 bxm_vadapter_attr_t.
 * @param[in] attr Pointer to the structure containing the attrs of the
 * 	virtual adapter that should be updated.
 *
 * @note There are explicit APIs available to set certain properties of the
 * resource e.g. vadapter_online, vadapter_offline etc. Internally these
 * these APIs may call bxm_vadapter_edit_general_attrs with appropriate
 * bitmask.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_vadapter_edit_general_attr(bxm_vadapter_id_t vadapter_id,
					bxm_vadapter_attr_bitmask_t *bitmask,
					bxm_vadapter_attr_t *attr);


/**
 * @brief Edit protocol specific properties of a vadapter.
 *
 * @param[in] vadapter_id The id of the virtual adapter to change the
 * 	protocol attributes of.
 * @param[in] bitmask Specifies an bitmask value. The bitmask indicates
 * 	the specific protocol properties of the object to be updated.  This
 * 	bitmask flag should be bit-AND'ed against order of attributes in the
 * 	struct bxm_vadapter_en_attr_t or bxm_vadapter_fc_attr_t based on
 * 	the protocol type.
 * @param[in] attr Pointer to the structure containing the attrs of the
 * 	virtual adapter that should be updated.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_vadapter_edit_protocol_attr(bxm_vadapter_id_t vadapter_id,
			    void *bitmask /* typecast according to protocol*/,
			    void *attr /* typecase according to protocol */);


/**
 * @brief Change the running mode of the vfabric to ONLINE.
 *
 * This operation modifies the running mode of the vadapter to ONLINE. This
 * operation is the trigger to BXM to start the services of the vadapter. After
 * the call the BXM system can instantiate the vadapter (based on whether it's
 * a HOST/NETWORK Initiated Vadapter). After the vadapter is instantiated on
 * the physical I/O module, its status is changed to ACTIVE.
 *
 * @note After the call the BXM system will change the vdapter to ONLINE.
 * If the vfabric is ONLINE/ACTIVE, the vadapter will also be ACTIVE.
 *
 * @param[in] vadapter_id The id of the vadapter to be activated.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_vadapter_online(bxm_vadapter_id_t vadapter_id);


/**
 * @brief Change the running mode of the vfabric to OFFLINE
 *
 * This function modifies the running mode of the vadapter to OFFLINE. This
 * operation is the trigger to BXM to diable all services of the vadapter.
 * After the call the BXM system will deactivate the virtual adapter. The
 * vadapter object and its properties remains in the BXM's configuration in
 * OFFLINE/DISABLED state.
 *
 * NOTE: For vNIC BXM will send appropriate commands e.g. Context table
 * updates etc, to delete the vadapter
 *
 * @param[in] vadapter_id The id of the vadapter to be disabled.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_vadapter_offline(bxm_vadapter_id_t vadapter_id);


/**
 * @brief Get the list of existing vadapters in the local inventory
 *
 * @param[in] attr Pointer to the structure containing the attrs of the
 * 	vadapters that should match with the properties in the inventory.
 * @param[in] bitmask Specifies the bitmask value. This bitmask flag should be
 * 	bit-AND'ed against the given attr to select the attributes in the
 * 	bxm_gateway_attr_t struct that should match the attributes of the
 * 	resource in the inventory. To select all resources unconditionally
 * 	the bitmast should be 0.
 * @param[in] num_result Number of resource objects allocated in result. If
 *	value of num_result is 0, the attribute changes its property and
 *	becomes an output parameter. The function will store the number of
 *	objects in the inventory at the location to which this argument points
 * @param[out] result Pointer to an array of vadapter attribute struct.
 * 	It is the responsibility of the caller to allocate and free memory of
 * 	the strucutres. The results found are stored at the location to which
 * 	this argument points. NULL will mark the end of the list, if there are
 * 	less than num_results objects found in the inventory.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_vadapter_select_inventory(bxm_vadapter_attr_t *attr,
			    bxm_vadapter_attr_bitmask_t *bitmask,
			    uint32_t num_result,
			    bxm_vadapter_id_t *result[]);


/**
 * @brief Query the properties of a virtual I/O adapter in the local
 * 	configuration
 *
 * @param[out] vadapter_id A valid vadapter id of the vadapter
 * @param[in] bitmask Specifies an bit mask value. The bitmask indicates the
 * 	specific properties of the object to be retrieved. This bitmask flag
 * 	should be bit-AND'ed against order of attributes in the struct
 * 	bxm_vadapter_attr_t.
 *
 * @param[out] result Pointer to the struct of vadapter attribute.
 * 	The caller would be responsibe to allocate and free memory of
 * 	the structure. The result is stored at the location to which
 * 	this argument points. If no match is found or on error, NULL will be
 * 	stored in this location.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_vadapter_query_general_attrs(bxm_vadapter_id_t vadapter_id,
			    bxm_vadapter_attr_bitmask_t *bitmask,
			    bxm_vadapter_attr_t *result);


/**
 * @brief Query protocol properties of a vadapter.
 *
 * @param[in] vadapter_id The id of the virtual adapter to query the
 * 	attributes of.
 * @param[in] bitmask Specifies an bitmask value. The bitmask
 * 	indicates the specific properties of the object to be queried.
 * @param[out] result Pointer to the struct of vadapter protocol attribute.
 * 	The caller would be responsibe to allocate and free memory of
 * 	the structure. The result is stored at the location to which
 * 	this argument points. If no match is found or on error, NULL will be
 * 	stored in this location.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_vadapter_query_protocol_attr(bxm_vadapter_id_t vadapter_id,
			    void *bitmask /* typecast according to protocol*/,
			    void *attr /* typecase according to protocol */);


/**
 * @brief Destroy a vadapter.
 *
 * This destroys the specified vadapter from the BXM. If the vadapter is ACTIVE
 * it has to be disabled before it is destroyed. Commands will be sent to
 * vfabrics and host to release all resources associated to the vadapter. All
 * entries corresponding to the vadapter are removed from the configuration.  
 * 
 * @note when a virtual IO adapter has been destroyed its id can be re-used
 * for another virtual IO adapter.
 *
 * @param[in] vadapter_id The vadapter to destroy.
 *
 * @return Returns 0 on success, or an error code on failure. Regardless of
 *     any error, the vadapter will no longer exist after
 *     bxm_vadapter_destroy()  returns, and the vadapter ID will be
 *     invalid for use with any other functions.
 */
bxm_error_t bxm_vadapter_destroy(bxm_vadapter_id_t vadapter_id);

#endif /* INCLUDED_bxm_vadapter */
