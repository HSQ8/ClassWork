/*! \file
 * Declarations for a simple memory allocator.  The allocator manages a small
 * pool of memory, provides memory chunks on request, and reintegrates freed
 * memory back into the pool.
 *
 * Adapted from Andre DeHon's CS24 2004, 2006 material.
 * Copyright (C) California Institute of Technology, 2004-2009.
 * All rights reserved.
 */


/**
 * this is the header definition for my allocation algorithm
 * we keep a size, a pointer to next and previous free to allow for
 * constant time deallocation
 *
 * we keep a prev_block pointer to simplify some math
 * we keep a char to specify whether the block is in use.
 *
 * the total size of this header with padding is 40 bytes.
 */
typedef struct Header{
    int32_t size;
    struct Header* prev_block;
    struct Header* next_free;
    struct Header* prev_free;
    char inUse;
}Header;

/*! Specifies the size of the memory pool the allocator has to work with. */
extern int MEMORY_SIZE;


/* Initializes allocator state, and memory pool state too. */
void init_myalloc();


/* Attempt to allocate a chunk of memory of "size" bytes. */
unsigned char * myalloc(int _size);


/* Free a previously allocated pointer. */
void myfree(unsigned char *oldptr);


/* Clean up the allocator and memory pool state. */
void close_myalloc();

/* This is a debugging function, */
void printblockinfo();

/* Finds the next block based on a computation with the current input
pointer and the size of the block that the header points to
this function is constant time*/
Header* getNextBlock(Header* ptr);

/*Finds the next free block for use when splitting blocks during allocation
to maintain explicit free list. This function is linear in time complexity*/
Header* getNextFree(Header* ptr);

/*computes the previous free block by a linear time search down one direction*/
Header* getPrevFree(Header* ptr);

/* This function takes the current freed header and adjusts the explicit free
list to maintain doubly linked list. This function is constant time as
each header stores two pointers to its neighboring free blocks*/
void insertIntoExplicitList(Header* current);

/*this function combines adjacent free blocks. It is constant time due to
our headers having a bidirectional linked list of explicit free blocks.*/
void coalesce(Header* current, Header* right);

