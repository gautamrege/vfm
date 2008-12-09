/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#ifndef INCLUDED_bxm_table
#define INCLUDED_bxm_table

#include "bxm_error.h"

/**
 * @file
 * @brief Sample BXM context table management API.
 */

/**
 * @brief A context table identifier.
 *
 * Context table identifier numbers are chosen by the BXM. They can also
 * be derived from the vHUB/vNPIV identifier.
 *
 * They must be unique (per BridgeX Manager).
 */
typedef uint32_t bxm_ctx_table_id_t;

/**
 * @brief Type of context table entries
 */
typedef enum
{
    /**
     * @brief vNIC/vHBA address
     */
    BXM_ENTRY_VADAPTER_ADDRESS	= 0,

    /**
     * @brief Gateway address
     */
    BXM_ENTRY_GATEWAY_ADDRESS   = 1,

    /**
     * @brief Unicast Miss address
     */
    BXM_UNICAST_MISS_ADDRESS    = 2,

    /**
     * @brief Multicast entry address
     */
    BXM_MULTICAST_ENTRY_ADDRESS    = 3,

    /**
     * @brief Unicast Miss address (Default multicast group) 
     */
    BXM_MULTICAST_MISS_ADDRESS    = 4
}
bxm_table_entry_type_t

/**
 * @brief A structure for the Context table entry format of a FCoE/EoIB
 */
typedef struct
{
    /**
     * @brief Identifies context table entry type.
     */
    bxm_table_entry_type_t type;

    /**
     * @brief Lid of vNIC/vHBA. Valid only for Entry type 0.
     * NOTE: For IB we use LID instead of IB_GUID to route messages. Since
     * host LID can change, BXM would need to keep track of the host LID
     * from keep-alive packets and update the context table if something
     * changes.
     */
    uint16_t lid;

    union
    {
	/**
	 * @brief MAC address of the vNIC. Valid for EoIB.
	 */
	bxm_en_mac_addr_t mac;

	/**
	 * @brief FCID of the vHBA. Valid for FCoIB.
	 */
	bxm_fc_id_t fcid;
    }
 
    /**
     * @brief Indicates if VLAN is valid or not. Valid for entry type 0 and
     * for EoIB only.
     */
    bool vlan_valid;

    /**
     * @brief Applies only for EoIB and entry type 0
     */
    vlan_id_t vlan;
    
    /**
     * @brief QP Number of vNIC/vHBA. Valid only for entry type 0
     */
    uint32_t qpn;

    /**
     * @brief Service Level value of vNIC/vHBA. Valid only for entry type 0
     */
    uint8_t  sl;
}
bxm_ib_ctx_table_entry_t;


/**
 * @brief A structure for the Context table entry format of a FCoE
 * 
 */
typedef struct
{
    /**
     * @brief Identifies context table entry type.
     */
    bxm_table_entry_type_t type;

    /** 
     * @brief MAC address of the vHBA
     */
    bxm_en_mac_addr_t ib_guid;

    /**
     * @brief FCID of the vHBA.  
     */
    bxm_fc_id_t fcid;
}
bxm_en_ctx_table_entry_t;


/**
 * @brief A structure for the Context table of a vHUB
 */
typedef struct
{
    bxm_ctx_table_id_t table_id;
    uint32_t num_entry;
    union
    {
	bxm_ib_ctx_table_entry_t entry[];
	bxm_en_ctx_table_entry_t entry[];
    };
}
bxm_ctx_table_attr_t;


/**
 * @brief Create a new table.
 *
 * @param[in] key_length The length of the keys for this table.
 * @param[in] value_length The length of the values for this table.
 * @param[out] table_id The ID for the new table. This function will return an
 *     error if a table already exists with this ID.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_ctx_table_create(size_t key_length,
				size_t value_length,
				bxm_ctx_table_id_t table_id);


/**
 * @brief Destroy a table.
 *
 * @param[in] table_id The Id of the table to destroy.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_ctx_table_destroy(bxm_ctx_table_id_t table_id);

/**
 * @brief Insert an entry into a table.
 *
 * This function will fail if an entry already exists with the specified key.
 * On success, the key and value will be copied into the table.
 *
 * @param[in] table_id The if of the table to modify.
 * @param[in] key The key to insert into the table.
 * @param[in] value The value associated with this key.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_ctx_table_insert(bxm_ctx_table_id_t table_id,
				void const *key,
				void const *value);


/**
 * @brief Lookup an entry from the table.
 *
 * @param[in] table_id The id of the table to search.
 * @param[in] key The key to look up.
 * @param[out] value On success, the value of the entry will be stored at the
 *     location pointed to by this parameter.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_ctx_table_lookup(bxm_ctx_table_id_t table_id,
				void const *key,
				void *value);

/**
 * @brief Remove an entry from the table.
 *
 * @param[in] table_id The id of the table to modify.
 * @param[in] key The key to remove.
 * @param[out] value If this parameter is non-NULL, the value of the removed
 *     entry will be stored at the location pointed to by this parameter on a
 *     successful return.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_ctx_table_remove(bxm_ctx_table_id_t table_id,
				void const *key,
				void *value);

#endif /* INCLUDED_bxm_table */
