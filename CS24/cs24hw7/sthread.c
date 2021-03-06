/*
* The simple thread scheduler.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "sthread.h"
#include "queue.h"
#include "timer.h"
#include "glue.h"


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
        fprintf(stderr, "Thread state has been corrupted: %d\n", threadp->state);
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

    if (context != NULL) {
    /* Add the current thread to the ready queue */
        assert(current != NULL);

    /* The thread could be Running, Blocked or Finished.
     * If Running, we must switch it back to Ready.
     */
        if (current->state == ThreadRunning)
            current->state = ThreadReady;

    /* If Running or Blocked, we just store the thread context and
     * re-queue the thread.  If Finished, then we need to deallocate
     * the thread's memory, because it is done.
     */
        if (current->state != ThreadFinished) {
            current->context = context;
            enqueue_thread(current);
        }
        else {
            __sthread_delete(current);
        }
    }

/* Choose a new thread from the ready queue.  If there are no more
 * Ready threads, the system is either deadlocked, or we are finished
 * running and the program can be terminated.
 */
    current = queue_take(&ready_queue);
    if (current == NULL) {
        if (queue_empty(&blocked_queue)) {
            fprintf(stderr, "All threads completed, exiting.\n");
            exit(0);
        }
        else {
            fprintf(stderr, "The system is deadlocked!\n");
            exit(1);
        }
    }

    current->state = ThreadRunning;
/* Return the next thread to resume executing. */
    return current->context;
}


/************************************************************************
* Thread operations.
*/

/*
* Start the scheduler.
*/
void sthread_start(int timer) {
    if (timer) {
        start_timer();
    }
    /* Thread start invokes the scheduler, thus we 
     * obtain a lock so that the timer does not interrupt the scheduler
     * because we all scheduler inside start
     */
    __sthread_lock();
    __sthread_start();
    /* Theoretically, we should have an unlock thread immediately following
     * the start after we locked, but since we also unlock inside _restore
     * there is no need for a redundant lock. 
     * The reason why we unlock inside restore is because the timer interrupt
     * calls switch, and since the timer should not unlock inside itself,
     * we should unlock from within restore after switch.
     */
}

/*
* Create a new thread.
*
* This function allocates a new context, and a new Thread
* structure, and it adds the thread to the Ready queue.
*/
Thread * sthread_create(void (*f)(void *arg), void *arg) {
    /* We must lock in create because we are initializing 
     * threads, which invokes malloc, which is not reentrant,
     * which implies that sthread create is also not reentrant
     * which means we should lock it.
     */
    __sthread_lock();
    Thread *threadp;
    void *memory;

/* Create a stack for use by the thread */
    memory = (void *) malloc(DEFAULT_STACKSIZE);
    if (memory == NULL) {
        fprintf(stderr, "Can't allocate a stack for the new thread\n");
        exit(1);
    }

/* Create a thread struct */
    threadp = (Thread *) malloc(sizeof(Thread));
    if (threadp == NULL) {
        fprintf(stderr, "Can't allocate a thread context\n");
        free(memory);
        exit(1);
    }

/* Initialize the thread */
    threadp->state = ThreadReady;
    threadp->memory = memory;
    threadp->context = __sthread_initialize_context(
        (char *) memory + DEFAULT_STACKSIZE, f, arg);
    enqueue_thread(threadp);
    /* We must unlock explicitly since we can't rely on switch to unlock. */
    __sthread_unlock();
    return threadp;
}


/*
* Return the pointer to the currently running thread.
*/
Thread * sthread_current(void) {
    return current;
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
    /*  sthread finish invokes a switch as we finish executing the
     *  current thread, thus we must lock to prevent the timer
     *  from interrupting the switch to another currently active thread.
     */
    __sthread_lock();
    current->state = ThreadFinished;
    __sthread_switch();
    /* Theoretically, we should have an unlock thread immediately following
     * the switch after we locked, but since we also unlock inside _restore
     * there is no need for a redundant lock. 
     * The reason why we unlock inside restore is because the timer interrupt
     * calls switch, and since the timer should not unlock inside itself,
     * we should unlock from within restore after switch.
     */
}


/*!
* This function is used by the scheduler to release the memory used by the
* specified thread.  The function deletes the memory used for the thread's
* context, as well as the memory for the Thread struct.
*/
void __sthread_delete(Thread *threadp) {
    assert(threadp != NULL);
    free(threadp->memory);
    free(threadp);
}


/*!
* Yield, so that another thread can run.  This is easy: just
* call the scheduler, and it will pick a new thread to run.
*/
void sthread_yield() {
    /* yield invokes a switch, thus we must have a lock to prevent
     * the timer from interrupting the switch process.
     */
    __sthread_lock();
    __sthread_switch();
    /* Theoretically, we should have an unlock thread immediately following
     * the switch after we locked, but since we also unlock inside _restore
     * there is no need for a redundant lock. 
     * The reason why we unlock inside restore is because the timer interrupt
     * calls switch, and since the timer should not unlock inside itself,
     * we should unlock from within restore after switch.
     */
}


/*!
* Block the current thread.  Set the state of the current thread
* to Blocked, and call the scheduler.
*/
void sthread_block() {
    /* sthread_block blocks the current thread and switches to a new 
     * thread, this process must not be interrupted by the timer. Thus
     * we obtain a lock. Since block calls switch which calls scheduler,
     * we should protect the scheduler from being interrupted.
     */
    __sthread_lock();
    current->state = ThreadBlocked;
    __sthread_switch();
    /* Theoretically, we should have an unlock thread immediately following
     * the switch after we locked, but since we also unlock inside _restore
     * there is no need for a redundant lock. 
     * The reason why we unlock inside restore is because the timer interrupt
     * calls switch, and since the timer should not unlock inside itself,
     * we should unlock from within restore after switch.
     */
}


/*!
* Unblock a thread that is blocked.  The thread is placed on
* the ready queue.
*/
void sthread_unblock(Thread *threadp) {
    /* We should call lock here since we are modifying shared memory
     * which are the queues that govern the threading process.
     */
    __sthread_lock();
/* Make sure the thread was blocked */
    assert(threadp->state == ThreadBlocked);

/* Remove from the blocked queue */
    queue_remove(&blocked_queue, threadp);

/* Re-queue it */
    threadp->state = ThreadReady;
    enqueue_thread(threadp);
    /* We must explicitly unlock here since we did not call switch*/
    __sthread_unlock();
}

