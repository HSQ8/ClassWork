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
    /* Semaphore holds a pointer to a queue to keep track of blocked threads */
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
    /* If we don't have resource, we lock then block and update queue*/
    assert(semp != NULL);
    while (semp->i == 0) {
        __sthread_lock();
        queue_append(semp->queue, sthread_current());
        sthread_block();
    }
    /* Once we reach here, it must be such that we unblocked meaning
     * We have resources
     */
    semp->i--;
}

/*
 * Increment the semaphore.
 * This operation must be atomic.
 */
void semaphore_signal(Semaphore *semp) {
    assert(semp != NULL);
    __sthread_lock();
    semp->i++;
    if (!queue_empty(semp->queue)) {
        Thread *blocked_thread = queue_take(semp->queue);
        sthread_unblock(blocked_thread);
    }
    __sthread_unlock();
}

