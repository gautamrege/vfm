/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */
#ifndef __VPS_COMMON_H__
#define __VPS_COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdint.h>
#include <pthread.h>
#include <sqlite3.h>
#include <string.h>

/*
 * For now define vps_error as an int. Later, it can be seamlessly put as a
 * struct if needed. Hence we are defining error parsing routines.
 */
typedef uint32_t vps_error;

/** ERROR CODES **/
#define VPS_SUCCESS             0

#define VPS_ERROR_DB_INIT       1
#define VPS_ERROR_IGNORE        2
#define VPS_ERROR_SOCK_OPEN     3
#define VPS_ERROR_SOCK_BIND     4
#define VPS_ERROR_READ          5
#define VPS_ERROR_TUNNEL        6
#define VPS_ERROR_INCOMPLETE_PK 7    
#define VPS_ERROR_GET_MAC       8
#define VPS_ERROR_GET_IFINDEX   9
#define VPS_ERROR_GET_MTU       10
#define VPS_ERROR_SEND_PK       11

#define VPS_DBERROR                     100
#define VPS_DBERROR_INVALID             101
#define VPS_DBERROR_INVALID_RESOURCE    102
/*
 * Dummy routine to get error value. It is recommended that we use this so as
 * to maintain uniformity later
 */
#define GET_ERROR(err) err
#define SET_ERROR(err, value) err = value;

/* TRACE LOGS */
#define VPS_NO_LOGS   0
#define VPS_ERROR     1
#define VPS_WARNING   2
#define VPS_INFO      3
#define VPS_ENTRYEXIT 4

/* Trace function */
void vps_trace(int level, const char* format, ...);

#endif /* __VPS_COMMON_H__ */
