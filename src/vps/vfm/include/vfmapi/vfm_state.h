/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#ifndef INCLUDED_vfm_state
#define INCLUDED_vfm_state

/**
 * @file
 * @brief States of VFM resources
 */


/**
 * @brief List of the states of resources on VFM
 */
typedef enum
{
    /**
     * @brief Resource is in an unknown state
     */
    VFM_STATE_UNKNOWN			= 0,

    /**
     * @brief Resource in is UN-INITIALIZED state
     */
    VFM_STATE_UNINIT 			= 1,

    /**
     * @brief Resource in is INITIALIZING state
     */
    VFM_STATE_INITIALIZING		= 2,

    /**
     * @brief Resource in is INITIALIZED state
     */
    VFM_STATE_INITIALIZED		= 3,

    /**
     * @brief Resource in is ACTIVATING state
     */
    VFM_STATE_ACTIVATING		= 4,

    /**
     * @brief Resource in is ACTIVE state
     */
    VFM_STATE_ACTIVE			= 5,

    /**
     * @brief Resource in is DISABLING state
     */
    VFM_STATE_DISABLING			= 6,

    /**
     * @brief Resource in is DISABLED state
     */
    VFM_STATE_DISABLE			= 7,

    /**
     * @brief Resource in is UNAVAILABLE state. This is an error state.
     */
    VFM_STATE_UNAVAILABLE		= 8,

    /*
     * @brief Sentinal to mark the end of the list
     */
    VFM_STATE_END			= 9
}
vfm_state_t;


/**
 * @brief List of the running mode of resources on VFM
 */
typedef enum
{
    /**
     * @brief Resource in is OFFLINE mode
     */
    VFM_RUN_MODE_OFFLINE 		= 0,

    /**
     * @brief Resource in is ONLINE mode
     */
    VFM_RUN_MODE_ONLINE			= 1,
}
vfm_running_mode_t;


/**
 * @brief List of the states of a port
 */
typedef enum
{
    /**
     * @brief Port is down.
     */
    VFM_PORT_STATE_DOWN			= 0,

    /**
     * @brief Port is in initializing state (Only applies to IB port)
     */
    VFM_PORT_STATE_INITIALIZING		= 1,

    /**
     * @brief Port is working.
     */
    VFM_PORT_STATE_UP			= 2,
}
vfm_port_state_t;


/**
 * @brief List of the HA state of resources
 */
typedef enum
{
    /**
     * @brief The primary resource is in use. 
     */
    VFM_HA_PRIMARY_ONLINE	= 1,

    /**
     * @brief The resource is failing over to the backup.
     */
    VFM_HA_FAILING_OVER		= 2,

    /**
     * @brief The backup resource is in use.
     */
    VFM_HA_BACKUP_ONLINE	= 3,

    /**
     * @brief The resource is failing back to the primary.
     */
    VFM_HA_FAILING_BACK		= 4
}
vfm_ha_state_t;

#endif /* INCLUDED_vfm_state */
