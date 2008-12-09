/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#ifndef INCLUDED_bxm_buf
#define INCLUDED_bxm_buf

#include "bxm_error.h"

/**
 * @file
 * @brief Sample BXM buffer management API.
 */

/**
 * @brief A buffer.
 *
 * This will contain control PDUs as needed by the BXM infrastructure.
 *
 * Alternatively, bxm_buf_t could be an opaque data type, and the API could
 * provide bxm_get_buf_data() and bxm_get_buf_length() functions to access the
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
bxm_buf_t;

/**
 * @brief Allocate a new buffer.
 *
 * @param[in] length The length of the buffer to allocate.
 * @param[out] buf On success, the new bxm_buf_t will be stored at the location
 *     pointed to by this parameter.
 *
 * @return Returns 0 on success, or an error code on failure.
 */
bxm_error_t bxm_buf_alloc(size_t length, bxm_buf_t *buf);

/**
 * @brief Free a buffer.
 *
 * @param[in] buf The buffer to free.
 */
void bxm_buf_free(bxm_buf_t *buf);

#endif /* INCLUDED_bxm_buf */
