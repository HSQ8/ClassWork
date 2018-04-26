/*! \file
 * Implementation of a simple memory allocator.  The allocator manages a small
 * pool of memory, provides memory chunks on request, and reintegrates freed
 * memory back into the pool.
 *
 * Adapted from Andre DeHon's CS24 2004, 2006 material.
 * Copyright (C) California Institute of Technology, 2004-2010.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>

#include "myalloc.h"


/**
 * documentation for block
 * each block will contain a header whose size is made up of a combination of
 * 32 bit signed int values and pointers
 *
 * the first value will be the size of the block as well as its availability,
 * if it were negative, then the block is not available, if it is positive,
 * then it is available.
 *
 * the second value will be the distance to the previous block
 * the third value will be the distance to the next free block
 *
 * each block looks like this
 * +-------+-------+-------+-------+------+
 * | int_1 | int_2 | int_3 | int_4 |  P   |
 * +-------+-------+-------+-------+------+
 *
 * int_1, + for available, - for unavailable, describe size of block
 * int_2, distance to header of previous block
 * int_3 distance to next free block
 * int_4 distance to the previous free block
 * P, payload
 *
 *
 *  * all block sizes will be tracked in bytes
 */

/*!
 * These variables are used to specify the size and address of the memory pool
 * that the simple allocator works against.  The memory pool is allocated within
 * init_myalloc(), and then myalloc() and free() work against this pool of
 * memory that mem points to.
 */
int MEMORY_SIZE;
unsigned char *mem;
static int HEADER_SIZE;
static int block_counter = 0;


/* TODO:  The unacceptable allocator uses an external "free-pointer" to track
 *        where free memory starts.  If your allocator doesn't use this
 *        variable, get rid of it.
 *
 *        You can declare data types, constants, and statically declared
 *        variables for managing your memory pool in this section too.
 */
static unsigned char *freeptr;


/*!
 * This function initializes both the allocator state, and the memory pool.  It
 * must be called before myalloc() or myfree() will work at all.
 *
 * Note that we allocate the entire memory pool using malloc().  This is so we
 * can create different memory-pool sizes for testing.  Obviously, in a real
 * allocator, this memory pool would either be a fixed memory region, or the
 * allocator would request a memory region from the operating system (see the
 * C standard function sbrk(), for example).
 */
void init_myalloc() {

    /*
     * Allocate the entire memory pool, from which our simple allocator will
     * serve allocation requests.
     */
    mem = (unsigned char *) malloc(MEMORY_SIZE);
    Header* head;
    HEADER_SIZE = sizeof(Header);
    Header start;
    if (mem == 0) {
        fprintf(stderr,
            "init_myalloc: could not get %d bytes from the system\n",
            MEMORY_SIZE);
        abort();
    }
    /*Initializes the first 4 bytes to mark the end of the block, the
    previous block is at addressed zero, so it points to itself
    and the next free block is also itself*/

    head = (Header *)mem;
    *head = start;
    head->size = MEMORY_SIZE - HEADER_SIZE;
    head->next_free = NULL;
    head->prev_free = NULL;
    head->prev_block = NULL;
    head->inUse = 0;
    block_counter++;
    freeptr = mem;
}

void printblockinfo(){
    if(1){
        printf("%s\n","BLOCKS=============================");
        int accumulativeHeader_memory = 0;
        int allocated_bytes = 0;
        int accounted_memory = 0;
        printf("number of blocks%d\n", block_counter);
        printf("total number of bytes%d\n", MEMORY_SIZE);
        Header* start = (Header*)mem;
        while(start != NULL){
            printf("size of header %ld\n", sizeof(Header));
            printf("size of block %d\n", (int)start->size);

            if(start->inUse == 1){
                allocated_bytes += (int)start->size;
            }
            printf("inUse: %d\n", (int)start->inUse);
            accumulativeHeader_memory += sizeof(Header);

            accounted_memory += ((int)start->size + sizeof(Header));

            start = getNextBlock(start);

        }
        printf("accumulated memory overhead %d\n",accumulativeHeader_memory + allocated_bytes );
        printf("allocated memory %d\n",allocated_bytes );
        printf("accounted memory %d\n",accounted_memory );
        printf("%s\n","END_BLOCKS=========================");
    }
}
/*!
 * Attempt to allocate a chunk of memory of "size" bytes.  Return 0 if
 * allocation fails.
 */
unsigned char *myalloc(int _size) {

    Header* header, *current;
    Header newHeader;
    unsigned char* returnPtr = (unsigned char*) 0;
    int smallestGoodSize;

    header = (Header*) mem;
    //finds first block not currently in use
    while(header->inUse == 1){
          header = getNextBlock(header);
          // we reach the end of the memory pool and haven't found a
          // free block, meaning we run out of memory.
          if(header == NULL){
              return returnPtr;
          }
    }
    if(header->inUse == 1){
        //cannot find any free blocks
        fprintf(stderr, "myalloc: (1)cannot service request of size %d\n",
         _size);
        return returnPtr;

    }
    // state assumption is that we found one one free block, regardless
    // of size
    current = header;
    smallestGoodSize = header->size;

    // find the smallest block that can still contain payload
    // out of the current free blocks
    //printf("%s\n", "NEWALLOC RUN==========");
    while(header != NULL){
        //printf("%d\n", header->size);
        if((header->size < smallestGoodSize || smallestGoodSize < _size) &&
        header->size >= _size){
            smallestGoodSize = header->size;
            current = header;
        }
        //header = header->next_free;
        header = getNextFree(header);
    }

    header = current; // now both pointers point to optimal block
    // if in our optimization we did not find a suitable block
    // we return 0
    if(smallestGoodSize >= _size){
        header->inUse = 1;
        returnPtr = (unsigned char*)(current+1);
    } else {
        //fprintf(stderr, "myalloc: (2)cannot service request of size %d\n",
        // _size);
    }
    // conditional split block if current allocated block is larger
    // than we need
    if(current->size > _size + HEADER_SIZE){
        current = (Header*)((unsigned char *)current + HEADER_SIZE + _size);
        *current = newHeader;
        current->size = header->size - HEADER_SIZE - _size;
        current->inUse = 0;
        current->prev_block = header;
        current->next_free = getNextFree(current);
        current->prev_free = getPrevFree(current);
        if(getNextBlock(current) != NULL){
            getNextBlock(current)->prev_block = current;
        }
        header->next_free = NULL;
        header->size = _size;
        block_counter++;
    }

    return returnPtr;
}

Header* getNextFree(Header* ptr){
    Header* temp = getNextBlock(ptr);
    while(temp != NULL){
        if(temp->inUse == 0){
            return temp;
        }
        temp = getNextBlock(temp);
    }
    return NULL;
}

Header* getPrevFree(Header* ptr){
    Header* temp = ptr->prev_block;
    while(temp != NULL){
        if(temp->inUse == 0){
            return temp;
        }
        temp = temp->prev_block;
    }
    return NULL;
}

Header* getNextBlock(Header* ptr){
    Header* next = NULL;
    if((unsigned char*)ptr+ptr->size + 2 * HEADER_SIZE < mem + MEMORY_SIZE){
        next = (Header*)((unsigned char*)ptr+ptr->size+HEADER_SIZE);
    }
    return next;
}

/*!
 * Free a previously allocated pointer.  oldptr should be an address returned by
 * myalloc().
 */
void myfree(unsigned char *oldptr) {
    // first, add current free block into explicit free list, then
    // we have to coalesce both sides, if possible by first coalescing
    // right side then moving the pointer to the previous left block and
    // attempt to coalesce the right block(which was the current center block)
    Header* current, *left, *right;
    current = (Header*)(oldptr - HEADER_SIZE);
    current->inUse = 0;
    insertIntoExplicitList(current);
    right = getNextBlock(current);
    left = current->prev_block;

    if(right != NULL){
        if(right->inUse == 0){
            coalesce(current, right);
        }
    }
    if(left != NULL){
        if(left->inUse == 0){
            coalesce(left, current);
        }
    }

}

void coalesce(Header* current, Header* right){
    current->next_free = right->next_free;
    if(right->next_free != NULL){
        right->next_free->prev_free = current;
    }
    if(getNextBlock(right) != NULL){
        getNextBlock(right)->prev_block = current;
    }
    current->size += right->size + HEADER_SIZE;
    block_counter--;
}

void insertIntoExplicitList(Header* current){
    Header *left, *right;
    left = getPrevFree(current);
    right = getNextFree(current);

    if(right != NULL){
        right->prev_free = current;
        current->next_free = right;
    }

    if(left != NULL){
        left->next_free = current;
        current->prev_free = left;
    }
}

/*!
 * Clean up the allocator state.
 * All this really has to do is free the user memory pool. This function mostly
 * ensures that the test program doesn't leak memory, so it's easy to check
 * if the allocator does.
 */
void close_myalloc() {
    free(mem);
}
