/*! \file
 * Declarations for a simple memory allocator.  The allocator manages a small
 * pool of memory, provides memory chunks on request, and reintegrates freed
 * memory back into the pool.
 *
 * Adapted from Andre DeHon's CS24 2004, 2006 material.
 * Copyright (C) California Institute of Technology, 2004-2009.
 * All rights reserved.
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

void printblockinfo();

Header* getNextBlock(Header* ptr);

Header* getNextFree(Header* ptr);

Header* getPrevFree(Header* ptr);

void insertIntoExplicitList(Header* current);

void coalesce(Header* current, Header* right);

