/*!
 * A simple test program for exercising the threads API.
 * 
 * ----------------------------------------------------------------
 * This program tests for argument passing into threading functions.
 * and thread terminating at different times
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "sthread.h"

/* 
 * the two loops prints alternating 5 and 7, since the printed value 
 * is the same each time we print, we show that indeed, the arguments 
 * are being saved and restored properly.
 * 
 * In this version of the code,
 * thread scheduling is explicit.
 */

/* This thread-function prints the passed in argument
 * for argument number of times over and over again.
 */
static void loop1(void *arg) {
    int i;
    int iarg = (intptr_t) arg;
    printf("%d%s%d%s\n", iarg, "should print ", iarg, " times.");
    /* Prints the argument iarg times and yields each time. */
    for (i = 0; i < iarg; ++i) {
        sthread_yield();
        printf("%s%d\n", " loop 1 argument passed in is: ", iarg);
    }
}

/* 
 * The main function starts the two loops in separate threads,
 * then start the thread scheduler.
 */
int main(int argc, char **argv) {
    /* here we start two threads with different arguments that we're 
     *printing out 
     */
    sthread_create(loop1, (void *) 5);
    sthread_create(loop1, (void *) 7);

    sthread_start();

    return 0;
}


