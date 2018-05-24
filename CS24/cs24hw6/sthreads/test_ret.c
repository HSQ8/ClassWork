/*!
 * A simple test program for exercising the threads API.
 * and different terminating times
 * 
 * ----------------------------------------------------------------
 * This program tests for argument passing into threading functions.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "sthread.h"

/*
 * In this version of the code,
 * thread scheduling is explicit.
 */

/*! This thread-function prints the passed in argument
 * for argument number of times over and over again, since the arguments
 * differ, each thread runs the for loop for different number of cycles
 * and thus threads passed with the larger value will terminate later
 */
static void loop(void *arg) {
    int i;
    int iarg = (intptr_t) arg;
    printf("%s%d%s%d%s\n", " Thread id#", iarg, " should print ", iarg, 
     " times.");
    /* Prints the argument iarg times and yields each time. */
    for (i = 0; i < iarg; ++i) {
        sthread_yield();
        printf("%s%d\n", " Thread id#", iarg);
    }
}

/*
 * The main function starts the two loops in separate threads
 * the start the thread scheduler.
 */
int main(int argc, char **argv) {

    sthread_create(loop, (void *) 2);
    sthread_create(loop, (void *) 4);
    sthread_create(loop, (void *) 8);
    sthread_create(loop, (void *) 16);
    sthread_create(loop, (void *) 32);
    sthread_create(loop, (void *) 64);
    sthread_start();
    return 0;
}


