/*
 * Define a bounded buffer containing records that describe the
 * results in a producer thread.
 *
 *--------------------------------------------------------------------
 * Adapted from code for CS24 by Jason Hickey.
 * Copyright (C) 2003-2010, Caltech.  All rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <assert.h>

#include "sthread.h"
#include "bounded_buffer.h"
#include "semaphore.h"
/*
 * The bounded buffer data.
 */
struct _bounded_buffer {
    /* The maximum number of elements in the buffer */
    int length;

    /* The index of the first element in the buffer */
    int first;

    /* The number of elements currently stored in the buffer */
    int count;

    /* Semaphore that keeps track of empty spaces, this prevents
     * bounded buffer overflows by restricting writes whenever there
     * are no more empty spaces
     */
    Semaphore *spaces;
    
    /* Semaphore that tracks number of spaces in buffer. This helps
     * prevent underflows / invalid reads since if we have no valid data 
     * to give out, then the semaphore blocks the thread that attempts to
     * take resources.
     */
    Semaphore *resources;
    
    /* Semaphore that keep track of the buffer itself so that 
     * there are no concurrent accessions. Blocks other threads from
     * modifying. 
     */
    Semaphore *bound_buffer_lock;

    /* The values in the buffer */
    BufferElem *buffer;
};


#define EMPTY -1


/*
 * Allocate a new bounded buffer.
 */
BoundedBuffer *new_bounded_buffer(int length) {
    BoundedBuffer *bufp;
    BufferElem *buffer;
    int i;

    /* Allocate the buffer */
    buffer = (BufferElem *) malloc(length * sizeof(BufferElem));
    bufp = (BoundedBuffer *) malloc(sizeof(BoundedBuffer));
    if (buffer == 0 || bufp == 0) {
        fprintf(stderr, "new_bounded_buffer: out of memory\n");
        exit(1);
    }
    /* Initialize */

    memset(bufp, 0, sizeof(BoundedBuffer));

    for (i = 0; i != length; i++) {
        buffer[i].id = EMPTY;
        buffer[i].arg = EMPTY;
        buffer[i].val = EMPTY;
    }

    bufp->length = length;
    bufp->buffer = buffer;

    /* allocate some space for semaphores */

    /* We allocate this semaphore with the number of spaces. */
    bufp->spaces = new_semaphore(length);
    /* resources start with 0 since the our bounded buffer doesn't
     * any data value yet.
     */ 
    bufp->resources = new_semaphore(0);

    /* We use this semaphore as a mutex to prevent concurrent access
     * to the buffer
     */
    bufp->bound_buffer_lock = new_semaphore(1);
    return bufp;
}

/*
 * Add an integer to the buffer.  Yield control to another
 * thread if the buffer is full.
 */
void bounded_buffer_add(BoundedBuffer *bufp, const BufferElem *elem) {
    /* Wait until the buffer has space
     * The reason why we can wait on the resource without having a lock
     * on the buffer itself is because the signal of this resource from
     * the other function is locked within a buffer lock.Thus when we signal
     * from another thread, we increase the semaphore count and unblock this
     * thread, which would then decrease the semaphore and continue to execute
     */
    semaphore_wait(bufp->spaces);

    /* Once the buffer has space, we will try to obtain a lock
     * on the buffer itself so we can modify it.
     */
    semaphore_wait(bufp->bound_buffer_lock);

    /* Now the buffer has space.  Copy the element data over. */
    int idx = (bufp->first + bufp->count) % bufp->length;
    bufp->buffer[idx].id  = elem->id;
    bufp->buffer[idx].arg = elem->arg;
    bufp->buffer[idx].val = elem->val;

    bufp->count = bufp->count + 1;

    /*Must signal the other semaphore to increase its counter*/
    semaphore_signal(bufp->resources);
    semaphore_signal(bufp->bound_buffer_lock);
}

/*
 * Get an integer from the buffer.  Yield control to another
 * thread if the buffer is empty.
 */
void bounded_buffer_take(BoundedBuffer *bufp, BufferElem *elem) {
    /* Wait until the buffer has a value to retrieve 
     * The reason why we can wait on the resource without having a lock
     * on the buffer itself is because the signal of this resource from
     * the other function is locked within a buffer lock. Thus when we signal
     * from another thread, we increase the semaphore count and unblock this
     * thread, which would then decrease the semaphore and continue to execute
     */
    semaphore_wait(bufp->resources);
    
    /* Once the buffer has space, we will try to obtain a lock
     * on the buffer itself so we can modify it.
     */
    semaphore_wait(bufp->bound_buffer_lock);

    /* Copy the element from the buffer, and clear the record */
    elem->id  = bufp->buffer[bufp->first].id;
    elem->arg = bufp->buffer[bufp->first].arg;
    elem->val = bufp->buffer[bufp->first].val;

    bufp->buffer[bufp->first].id  = -1;
    bufp->buffer[bufp->first].arg = -1;
    bufp->buffer[bufp->first].val = -1;

    bufp->count = bufp->count - 1;
    bufp->first = (bufp->first + 1) % bufp->length;
    
    /* Signal the other semaphore that we have more spaces now that we
     * took a resource.
     */
    semaphore_signal(bufp->spaces);
    semaphore_signal(bufp->bound_buffer_lock);
}

