/*
 *  Copyright (c) 2008  VirtualPlane Systems, Inc.
 */

#include<bxm_queue.h>


pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond =  PTHREAD_COND_INITIALIZER;

bxm_task *first_task = NULL; /*Locate the first task in queue*/
bxm_task *last_task  = NULL; /*Locate the last task in queue*/

int num_task_in_queue = 0;


/*
 * Add task data to queue.
 *
 * [IN] *data     : Task Data.
 * [IN] size      : Task data  size.
 * [IN] task_type : Task type.
 * [IN] bxm_type   : VFM type - LBC or RBC
 *
 * Return : err
 *
 */
vps_error
add_task_to_queue(uint8_t *data, int size, bxm_task_type task_type,
                                                     uint8_t bxm_type)
{
        vps_error err = VPS_SUCCESS;

        bxm_task *new_task;

        vps_trace(VPS_ENTRYEXIT, "Entering add_task_to_queue");

        /*Allocate memory for queue element*/
        new_task = (bxm_task *)malloc(sizeof(bxm_task));

        /*
         * Set VFM Type- If task receive forn LBC then it is 0
         * If task receive forn RBC then it is 1
         * Add task type.
         */
        new_task->bxm_type  = bxm_type;
        new_task->task_type = task_type;

        /*Set size of task deta*/
        new_task->size = size;

        /*Allocate memory for task data and copy task data*/
        new_task->data = (uint8_t *)malloc(size);
        memcpy(new_task->data, data, size);

        /*----- CRITICAL REGION START--- */

        pthread_mutex_lock(&queue_mutex);

        /*
         * IF no task in queue then add task and set it to first task
         * else add task to end of queue.
         */
        if (first_task == NULL) {
                first_task = new_task;
        }
        else {
                last_task->next = new_task;
        }

        /*Update last task pointer to new added task*/
        last_task  = new_task;

        /*Set next task pointer to NULL for added new task*/
        new_task->next = NULL;

        /*Increment number of task in queue for processing*/
        num_task_in_queue++;

        pthread_cond_broadcast(&queue_cond);
        pthread_mutex_unlock(&queue_mutex);
        /*----- CRITICAL REGION END--- */

        vps_trace(VPS_ENTRYEXIT, "Leaving add_task_to_queue");

        return err;
}

/*
 * Get task from queue for processing.
 *
 * Return : task.. THE CALLSER SHOULD FREEE THE PACKET !
 */
bxm_task *
get_task_from_queue()
{
        vps_error err = VPS_SUCCESS;

        bxm_task *task = NULL;

        vps_trace(VPS_ENTRYEXIT, "Entering get_task_from_queue");

        /*----- CRITICAL REGION START--- */
        pthread_mutex_lock(&queue_mutex);

        while (num_task_in_queue == 0)
          pthread_cond_wait(&queue_cond, &queue_mutex);

        task = first_task;

        if (first_task && first_task->next != NULL) {
           first_task = first_task->next;
        }
        else {
                last_task = NULL;
                first_task = NULL;
        }

        /*Decrement number of tasks in queue*/
        num_task_in_queue--;

        pthread_mutex_unlock(&queue_mutex);
        /*----- CRITICAL REGION END--- */

        vps_trace(VPS_ENTRYEXIT, "Leaving get_task_from_queue");

        return task;
}
