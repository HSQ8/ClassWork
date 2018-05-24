/*
 * The simple thread scheduler.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "sthread.h"
#include "queue.h"


/*! The default stack size of threads, 1MiB of stack space. */
#define DEFAULT_STACKSIZE       (1 << 20)


/************************************************************************
 * Interface to the assembly.
 */


/*! This function must be called to start thread processing. */
void __sthread_start(void);


/*!
 * The is the entry point into the scheduler, to be called
 * whenever a thread action is required.
 */
void __sthread_switch(void);

/*!
 * Initialize the context for a new thread.
 *
 * The stackp pointer should point to the *end* of the area allocated for the
 * thread's stack.  (Don't forget that x86 stacks grow downward in memory.)
 *
 * The function f is initially called with the argument given.  When f
 * returns, __sthread_finish() is automatically called by the threading
 * library to ensure that the thread is cleaned up and deallocated properly.
 */
ThreadContext *__sthread_initialize_context(void *stackp, ThreadFunction f,
                                            void *arg);

/************************************************************************
 * Internal helper functions.
 */

/*
 * The initial thread context is initialized such that this function
 * will be called automatically when a thread's function returns.
 */
void __sthread_finish(void);

/*
 * This function deallocates the memory associated with a thread
 * when it terminates.
 */
void __sthread_delete(Thread *threadp);


/************************************************************************
 * Global variables and scheduler-level queue operations.
 */

/*!
 * The thread that is currently running.
 *
 * Invariant: during normal operation, there is exactly one thread in
 * the ThreadRunning state, and this variable points to that thread.
 *
 * Note that at start-up of the threading library, this will be NULL.
 */
static Thread *current;

/*!
 * The queue of ready threads.  Invariant:  All threads in the ready queue
 * are in the state ThreadReady.
 */
static Queue ready_queue;

/*!
 * The queue of blocked threads.  Invariant:  All threads in the blocked
 * queue are in the state ThreadBlocked.
 */
static Queue blocked_queue;


/*!
 * Add a thread to the appropriate scheduling queue, based on its state.
 */
static void enqueue_thread(Thread *threadp) {
    assert(threadp != NULL);

    switch(threadp->state) {
    case ThreadReady:
        queue_append(&ready_queue, threadp);
        break;
    case ThreadBlocked:
        queue_append(&blocked_queue, threadp);
        break;
    default:
        fprintf(stderr, "Thread state has been corrupted: %d\n",
                threadp->state);
        exit(1);
    }
}

/************************************************************************
 * Scheduler.
 */

/*
 * The scheduler is called with the context of the current thread,
 * or NULL when the scheduler is first started.
 *
 * The general operation of this function is:
 *   1.  Save the context argument into the current thread.
 *   2.  Either queue up or deallocate the current thread,
 *       based on its state.
 *   3.  Select a new "ready" thread to run, and set the "current"
 *       variable to that thread.
 *        - If no "ready" thread is available, examine the system
 *          state to handle this situation properly.
 *   4.  Return the context of the thread to run, so that a context-
 *       switch will be performed to start running the next thread.
 *
 * This function is global because it needs to be called from the assembly.
 */
ThreadContext *__sthread_scheduler(ThreadContext *context) {
    /* Checks for initial case when context passed in is NULL */
    if (current != NULL) {
        current->context = context;
        if (current->state == ThreadFinished) {
            /* If thread is finished, free up memory of the thread. */
            __sthread_delete(current);
        } else if (current->state == ThreadRunning) {
            /* If there is only one thread running, then we return early
             * and let that thread keep running. This saves the time
             * we would have spent enqueue and dequeue this one thread.
             */
            if (queue_empty(&ready_queue)) {
                return current->context;
            }
            /* If thread is running, then we put in in ready queue. */
            current->state = ThreadReady;
            enqueue_thread(current);
        } else if(current->state == ThreadBlocked) {
            /* If thread is blocked, then we put in in blocked queue. */
            enqueue_thread(current);
        }
    }
    /* Code for getting thread from queue to set up the next thread
     * for execution.
     */
    if (!queue_empty(&ready_queue)) {
        /* If there are available threads to run, run them */
        current = queue_take(&ready_queue);
        current->state = ThreadRunning;
    } else if (queue_empty(&ready_queue)) {
        /* This case handles if we don't have any threads to run and
         * all threads are blocked, or we just don't have any threads
         * at all and all threads are finished.
         */
        if (queue_empty(&blocked_queue)) {
            printf("%s\n", "All threads finished");
            exit(0);
        } else {
            printf("%s\n", "Deadlock Detected");
            exit(1);
        }
    }

    /* Return the next thread to resume executing. */
    return current->context;
}


/************************************************************************
 * Thread operations.
 */

/*
 * Start the scheduler.
 */
void sthread_start(void)
{
    __sthread_start();
}

/*
 * Create a new thread.
 *
 * This function allocates a new context, and a new Thread
 * structure, and it adds the thread to the Ready queue.
 */
Thread * sthread_create(void (*f)(void *arg), void *arg) {
    /**
     * Allocates memory for new_thread, try and catch any failures of 
     * malloc if we run out of memory.
     */
    Thread *new_thread = (Thread *) malloc(sizeof(Thread));
    if (new_thread == NULL) {
        printf("%s\n", "malloc failed");
        exit(1);
    }

    /**
     * Allocates memory for stack, try and catch any failures of 
     * malloc if we run out of memory.
     */
    new_thread->memory = malloc(DEFAULT_STACKSIZE);
    if (new_thread->memory == NULL) {
        printf("%s\n", "malloc failed");
        exit(1);
    }
    /* Update thread state flag */
    new_thread->state = ThreadReady;
    /* Put thread in appropriate queue */
    enqueue_thread(new_thread);
    /* Modify stack pointer to end of memory pool because stack 
     * grows down.
     */
    void* stackp = (void *) ((unsigned char *) new_thread->memory + 
        DEFAULT_STACKSIZE);
    /* Use the initialize function to populate the context for the first
     * load of the context.
     */
    new_thread->context = __sthread_initialize_context(stackp, f, arg);
    return new_thread;
}


/*
 * This function is called automatically when a thread's function returns,
 * so that the thread can be marked as "finished" and can then be reclaimed.
 * The scheduler will call __sthread_delete() on the thread before scheduling
 * a new thread for execution.
 *
 * This function is global because it needs to be referenced from assembly.
 */
void __sthread_finish(void) {
    printf("Thread %p has finished executing.\n", (void *) current);
    current->state = ThreadFinished;
    __sthread_switch();
}


/*!
 * This function is used by the scheduler to release the memory used by the
 * specified thread.  The function deletes the memory used for the thread's
 * context, as well as the memory for the Thread struct.
 */
void __sthread_delete(Thread *threadp) {
    /* Free up stack memory */
    free(threadp->memory);
    /* Free the thread struct itself */
    free(threadp);
    threadp->memory = NULL;
    threadp->context = NULL;
    threadp = NULL;
}


/*!
 * Yield, so that another thread can run.  This is easy: just
 * call the scheduler, and it will pick a new thread to run.
 */
void sthread_yield() {
    __sthread_switch();
}


/*!
 * Block the current thread.  Set the state of the current thread
 * to Blocked, and call the scheduler.
 */
void sthread_block() {
    current->state = ThreadBlocked;
    __sthread_switch();
}


/*!
 * Unblock a thread that is blocked.  The thread is placed on
 * the ready queue.
 */
void sthread_unblock(Thread *threadp) {

    /* Make sure the thread was blocked */
    assert(threadp->state == ThreadBlocked);

    /* Remove from the blocked queue */
    queue_remove(&blocked_queue, threadp);

    /* Re-queue it */
    threadp->state = ThreadReady;
    enqueue_thread(threadp);
}

