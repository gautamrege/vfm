/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 *
 * Header File: Data Structures for Task Queue.
 */
#ifndef __VFM_TASK_QUEUE_H_
#define __VFM_TASK_QUEUE_H_

#include <common.h>
#include <vfm_fip.h>

/* Task types */
typedef enum __vfm_task_type {
        FIP_TASK           =  0,  /* To proceesing FIP packet*/
        GW_ADV_TASK        =  1,  /* To sending GW advertisement */
        GW_KEEP_ALIVE_TASK =  2,  /* To sending GW keeep alive */
        CREATE_VADAPTER    =  3,  /* To create vAdapter on host */
        DISTROY_VADAPTER   =  4  /* To distroy vAdapter */
}vfm_task_type;        

/* Task struct */
typedef struct __vfm_task {
        uint8_t vfm_type;           /* Task for local VFM or remote VFM*/
        vfm_task_type task_type;    /* Task type for processing */
        uint8_t *data;              /* Task data */
        int size;                   /* Task data size */
        struct __vfm_task *next;    /* Next task pointer */
}vfm_task;


/*
 * Add packet data to queue.
 *
 * [IN] *data      : Data Data.
 * [IN] size       : Task data size.
 * [IN] task_type  : Task type.
 * [IN] vfm_type   : VFM type - LVFM or RVFM
 *
 * Return : err
 *
 */

vps_error
add_task_to_queue(uint8_t *data, int size, vfm_task_type task_type,
                                                        uint8_t vfm_type);

/*
 * Get task from queue for processing.
 *
 * Return : vfm_task*
 *           THE CALLSER SHOULD FREEE THE TASK data !
 */
vfm_task *
get_task_from_queue();

#endif /* __VFM_TASK_QUEUE_H_ */
