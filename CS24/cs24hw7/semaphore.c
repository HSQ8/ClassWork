/*
 * General implementation of semaphores.
 *
 *--------------------------------------------------------------------
 * Adapted from code for CS24 by Jason Hickey.
 * Copyright (C) 2003-2010, Caltech.  All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include "queue.h"

#include "sthread.h"
#include "semaphore.h"
#include "glue.h"

/*
 * The semaphore data structure contains a semaphore variable
 */
struct _semaphore {
    int i;
    /* Semaphore holds a pointer to a queue to keep track of blocked threads 
     * We add this structure to the semaphore so that we can fairly
     * unblock threads in the same order they are blocked. By using a 
     * queue, we can make sure that all blocked threads will
     * eventually get unblocked fairly.
     */
    Queue *queue;
};

/************************************************************************
 * Top-level semaphore implementation.
 */

/*
 * Allocate a new semaphore.  The initial value of the semaphore is
 * specified by the argument.
 */
Semaphore *new_semaphore(int init) {
    Semaphore *semp = NULL;
    /* Malloc some space for semaphore*/
    semp = (Semaphore *) malloc(sizeof(Semaphore));
    if (semp == NULL) {
        printf("%s\n", "Memory allocation for semaphore failed");
        exit(1);
    }
    semp->i = init;
    /* malloc some space for a queue*/
    semp->queue = (Queue *) malloc(sizeof(Queue));
    if (semp->queue == NULL) {
        printf("%s\n", "Memory allocation for queue failed");
        exit(1);
    }

    return semp;
}

/*
 * Decrement the semaphore.
 * This operation must be atomic, and it blocks iff the semaphore is zero.
 */
void semaphore_wait(Semaphore *semp) {
    assert(semp != NULL);
    /* If we don't have resource, we lock then block and update queue*/
    /* Here we lock the thread since this function isn't reentrant, we 
     * modifying some very important program states here.
     */
    while (semp->i == 0) {
        /* Here we first lock the thread so that we cannot get 
         * interrupted by the timer interrupt.
         */
        __sthread_lock();
        /* We add the thread to the semaphore's queue to keep track of it*/
        queue_append(semp->queue, sthread_current());
        /* block the thread so we don't waste cpu power 
         * We block after append because block switches, so if block
         * happened before append, then we wouldn't properly update the 
         * queue in the semaphore
         */
        sthread_block();
        __sthread_lock();
    }
    /* Once we reach here, it must be such that we unblocked meaning
     * We have resources, then we decrement the semaphore
     */
    semp->i--;
    __sthread_unlock();
}

/*
 * Increment the semaphore.
 * This operation must be atomic.
 */
void semaphore_signal(Semaphore *semp) {
    assert(semp != NULL);
    /* Since we are potentially unblocking threads,
     * and since we are modifying the semaphore,
     * we don't want any states to potentially become corrupt since
     * this function isn't reentrant (we are modifying some very important
     * program states here), and thus we lock before we do any
     * modifications.
     */
    __sthread_lock();
    semp->i++;
    if (!queue_empty(semp->queue)) {
        /* If we have any threads available to unblock, we unlock it because
         * we now have resources
         */
        Thread *blocked_thread = queue_take(semp->queue);
        sthread_unblock(blocked_thread);
    }
    __sthread_unlock();
}

