/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#ifndef INCLUDED_bxm_state
#define INCLUDED_bxm_state

/**
 * @file
 * @brief States of BXM resources
 */


/**
 * @brief List of the states of resources on BXM
 */
typedef enum
{
    /**
     * @brief Resource is in an unknown state
     */
    BXM_STATE_UNKNOWN			= 0,

    /**
     * @brief Resource in is UN-INITIALIZED state
     */
    BXM_STATE_UNINIT 			= 1,

    /**
     * @brief Resource in is INITIALIZING state
     */
    BXM_STATE_INITIALIZING		= 2,

    /**
     * @brief Resource in is INITIALIZED state
     */
    BXM_STATE_INITIALIZED		= 3,

    /**
     * @brief Resource in is ACTIVATING state
     */
    BXM_STATE_ACTIVATING		= 4,

    /**
     * @brief Resource in is ACTIVE state
     */
    BXM_STATE_ACTIVE			= 5,

    /**
     * @brief Resource in is DISABLING state
     */
    BXM_STATE_DISABLING			= 6,

    /**
     * @brief Resource in is DISABLED state
     */
    BXM_STATE_DISABLE			= 7,

    /**
     * @brief Resource in is UNAVAILABLE state. This is an error state.
     */
    BXM_STATE_UNAVAILABLE		= 8,

    /*
     * @brief Sentinal to mark the end of the list
     */
    BXM_STATE_END			= 9
}
bxm_state_t;


/**
 * @brief List of the running mode of resources on BXM
 */
typedef enum
{
    /**
     * @brief Resource in is OFFLINE mode
     */
    BXM_RUN_MODE_OFFLINE 		= 0,

    /**
     * @brief Resource in is ONLINE mode
     */
    BXM_RUN_MODE_ONLINE			= 1,
}
bxm_running_mode_t;


/**
 * @brief List of the states of a port
 */
typedef enum
{
    /**
     * @brief Port is down.
     */
    BXM_PORT_STATE_DOWN			= 0,

    /**
     * @brief Port is in initializing state (Only applies to IB port)
     */
    BXM_PORT_STATE_INITIALIZING		= 1,

    /**
     * @brief Port is working.
     */
    BXM_PORT_STATE_UP			= 2,
}
bxm_port_state_t;


/**
 * @brief List of the HA state of resources
 */
typedef enum
{
    /**
     * @brief The primary resource is in use. 
     */
    BXM_HA_PRIMARY_ONLINE	= 1,

    /**
     * @brief The resource is failing over to the backup.
     */
    BXM_HA_FAILING_OVER		= 2,

    /**
     * @brief The backup resource is in use.
     */
    BXM_HA_BACKUP_ONLINE	= 3,

    /**
     * @brief The resource is failing back to the primary.
     */
    BXM_HA_FAILING_BACK		= 4
}
bxm_ha_state_t;

#endif /* INCLUDED_bxm_state */
