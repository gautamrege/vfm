/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#ifndef INCLUDED_vfm_buf
#define INCLUDED_vfm_buf

#include "vfm_error.h"

/**
 * @file
 * @brief Sample VFM buffer management API.
 */

/**
 * @brief A buffer.
 *
 * This will contain control PDUs as needed by the VFM infrastructure.
 *
 * Alternatively, vfm_buf_t could be an opaque data type, and the API could
 * provide vfm_get_buf_data() and vfm_get_buf_length() functions to access the
 * data and length attributes.
 */
typedef struct
{
    /**
     * @brief Pointer to a buffer.
     */
    void	*data;
    /**
     * @brief Length of the buffer.
     */
    uint64_t	length;
}
vfm_buf_t;

/**
 * @brief Allocate a new buffer.
 *
 * @param[in] length The length of the buffer to allocate.
 * @param[out] buf On success, the new vfm_buf_t will be stored at the location
 *     pointed to by this parameter.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
vfm_error_t vfm_buf_alloc(size_t length, vfm_buf_t *buf);

/**
 * @brief Free a buffer.
 *
 * @param[in] buf The buffer to free.
 */
void vfm_buf_free(vfm_buf_t *buf);

#endif /* INCLUDED_vfm_buf */
