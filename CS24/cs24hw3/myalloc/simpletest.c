/*! \file
 * This file contains code for performing simple tests of the memory allocator.
 * It can be edited by students to perform simple sequences of allocations and
 * deallocations to make sure the allocator works properly.
 */

#include <stdio.h>
#include <stdlib.h>

#include "myalloc.h"


/* Try to allocate a block of memory, and fill its entire contents with
 * the specified fill character.
 */
unsigned char * allocate(int size, unsigned char fill) {
    unsigned char *block = myalloc(size);
    if (block != NULL) {
        int i;

        printf("Allocated block of size %d bytes.\n", size);
        for (i = 0; i < size; i++)
            block[i] = fill;
    }
    else {
        printf("Couldn't allocate block of size %d bytes.\n", size);
    }
    return block;
}


int main(int argc, char *argv[]) {

    /* Specify the memory pool size, then initialize the allocator. */
    MEMORY_SIZE = 40000;
    init_myalloc();

    /* Perform simple allocations and deallocations. */
    /* Change the below code as you see fit, to test various scenarios. */

    unsigned char *a = allocate(146, 'A');
    printblockinfo();
    unsigned char *b = allocate(200, 'B');
    printblockinfo();
    unsigned char *c = allocate(78, 'C');
    printblockinfo();
    myfree(b);
    printblockinfo();
    myfree(a);
    printblockinfo();
    unsigned char *d = allocate(155, 'D');
    printblockinfo();
    unsigned char *e = allocate(55, 'E');
    printblockinfo();
    myfree(c);
    printblockinfo();
    myfree(d);
    printblockinfo();
    myfree(e);
    printblockinfo();

    close_myalloc();

    return 0;
}
