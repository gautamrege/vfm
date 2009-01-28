/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#ifndef INCLUDED_vfm_error
#define INCLUDED_vfm_error

#include <stdint.h>

#define VFM_SUCCESS             0
#define VFM_ERROR_SENDING       1
#define VFM_ERROR_RECEIVE       2
#define VFM_ERROR_CONNECT       3
#define VFM_ERROR_VFABRIC       4
#define VFM_ERROR_VADAPTER      5
#define VFM_ERROR_IOMODULE      6
/**
 * @file
 * @brief Error codes for VFM API.
 *
 * This file contains error codes of VFM API.
 */

/**
 * @brief An error code.
 *
 * The exact error codes could be defined later.
 */
typedef uint32_t vfm_error_t;

#endif /* INCLUDED_vfm_error */
