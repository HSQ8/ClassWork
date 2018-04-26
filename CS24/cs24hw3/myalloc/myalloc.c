/*! \file
 * Implementation of a simple memory allocator.  The allocator manages 
 * a small pool of memory, provides memory chunks on request, 
 * and reintegrates freed memory back into the pool.
 *
 * Adapted from Andre DeHon's CS24 2004, 2006 material.
 * Copyright (C) California Institute of Technology, 2004-2010.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>

#include "myalloc.h"

/**
 * My allocator uses a struct as a header and it is both defined
 * and documented in the myalloc.h file.
 */


/*!
 * These variables are used to specify the size and address of the 
 * memory pool that the simple allocator works against. 
 * The memory pool is allocated within init_myalloc(),
 * and then myalloc() and free() work against this pool of
 * memory that mem points to.
 */
int MEMORY_SIZE;
/* variable constants used to signal whether a block is free or not*/
int FREE = 0;
int USED = 1;
unsigned char *mem;
/* The Header size will be initialized in the initialize function.  */
static int HEADER_SIZE;
/* We have a block counter for sanity check reasons.  */
static int block_counter = 0;


/*!
 * This function initializes both the allocator state, and the memory pool. 
 * It* must be called before myalloc() or myfree() 
 * will work at all.
 *
 * Note that we allocate the entire memory pool using malloc().  
 * This is so we can create different memory-pool sizes 
 * for testing.  Obviously, in a real allocator, this
 * memory pool would either be a fixed memory region, or the
 * allocator would request a memory region from the operating system
 * (see the C standard function sbrk(), for example).
 *
 * This function is a constant time allocation as it initializes
 * one block of size MEMORY_SIZE with a header.
 */
void init_myalloc() {

    /*
     * Allocate the entire memory pool, from which our simple 
     * allocator will serve allocation requests.
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
/**
 * Here, we initialize the entire memory pool as one big block with 
 * a header.
 * This is done in constant time and allows use to subsequently 
 * create new blocks with an anchor header. This header 
 * can be freed, but it should always exist.
 */

    head = (Header *)mem;
    *head = start;
    head->size = MEMORY_SIZE - HEADER_SIZE;
    head->next_free = NULL;
    head->prev_free = NULL;
    head->prev_block = NULL;
    head->inUse = 0;
    block_counter++;
}

/**
 * [printblockinfo description] This function prints a representation
 * of the current state of the number of blocks, the size of each block
 * and the accumulation of different blocks in memory utilization in 
 * order to perform certain sanity checks and aid with debug.
 */
void printblockinfo() {
    printf("%s\n", "BLOCKS=============================");
    int accumulativeHeader_memory = 0;
    int allocated_bytes = 0;
    int accounted_memory = 0;
    printf("number of blocks%d\n", block_counter);
    printf("total number of bytes%d\n", MEMORY_SIZE);
    Header* start = (Header*)mem;
    while (start != NULL) {
        printf("size of header %ld\n", sizeof(Header));
        printf("size of block %d\n", (int)start->size);
        if (start->inUse == USED) {
            allocated_bytes += (int)start->size;
        }
        printf("inUse: %d\n", (int)start->inUse);
        accumulativeHeader_memory += sizeof(Header);
        accounted_memory += ((int)start->size + sizeof(Header));
        start = getNextBlock(start);
    }
    printf("accumulated memory overhead %d\n", 
        accumulativeHeader_memory + allocated_bytes );
    printf("allocated memory %d\n", allocated_bytes );
    printf("accounted memory %d\n", accounted_memory );
    printf("%s\n", "END_BLOCKS=========================");
}
/*!
 * Attempt to allocate a chunk of memory of "size" bytes.  Return 0 if
 * allocation fails.
 *
 * This function uses a best fit strategy to find the smallest block that 
 * will * fullfill the allocation request. This block splits whenever
 * the space allocated for a block has enough blank space left over for
 * another header
 * plus 1 byte or more. When splitting new blocks, this function adjusts
 * pointers in the header to maintain an explicit free list made up of
 * a doubly linked list of free blocks.
 * @param  _size [the number of bytes we want to allocate]
 * @return       [a pointer to the allocated memory pool if success, 
 * otherwise, returns 0, aka null pointer]
 *
 * Time complexity: This function uses a best fit strategy by
 * traversing through the free list and finds the smallest block that can
 * still satisfy the allocation request.
  */
unsigned char *myalloc(int _size) {
    /* define some variables. */
    Header* header, *current;
    Header newHeader;
    unsigned char* returnPtr = (unsigned char*) 0;
    int smallestGoodSize;
    header = (Header*) mem;
    /* finds first block not currently in use */
    while (header->inUse == USED) {
          header = getNextBlock(header);
          /**
           * we reach the end of the memory pool and haven't found a
           * free block, meaning we run out of memory. Then we return
           * a null (0) pointer
           */         
          if (header == NULL) {
              return returnPtr;
          }
    }

    /**
     * state of the program here is the assumption that we found one one
     * free block, regardless of size, thus we set the size of the 
     * preliminary block size for allocation consideration to be the first
     * comparison value, we will then iterate through the free list to find
     * the most optimal block size, updating this comparison value. */
    current = header;
    smallestGoodSize = header->size;

    /* find the smallest block that can still contain payload
     * out of the current free blocks */
    while (header != NULL) {
        /* 
         * this statement says that we will update the header-block 
         * in consideration if there is a new block that satisfies the 
         * allocation with less memory, or if the current block is not 
         * enough to allocate the size of the memory we want.
         * */
        if ((header->size < smallestGoodSize || smallestGoodSize < _size) &&
        header->size >= _size) {
            smallestGoodSize = header->size;
            current = header;
        }
        header = getNextFree(header);
    }

    header = current; 
    /* now both pointers point to optimal block for allocation
     * if in our optimization we did not find a suitable block
     * that is too small, then we return zero. */
    if (smallestGoodSize >= _size) {
        header->inUse = 1;
        returnPtr = (unsigned char*)(current+1);
    } else {
        /* *  
         * immediately return NULL pointer
         * */
        return returnPtr;
    }
    /* conditional split block if current allocated block is larger
     * here, we split if the remaining space in our block can contain
     * another header with a nonzero data payload size.
     * we accomplish this by computing the end of the payload and adding a
     * new header, then adjusting the pointers to keep track of explicit 
     * free list 
     */
    if (current->size > _size + HEADER_SIZE) {
        current = (Header *)((unsigned char *)current + HEADER_SIZE + _size);
        *current = newHeader;
        current->size = header->size - HEADER_SIZE - _size;
        current->inUse = 0;
        current->prev_block = header;
        current->next_free = getNextFree(current);
        current->prev_free = getPrevFree(current);
        if (getNextBlock(current) != NULL) {
            getNextBlock(current)->prev_block = current;
        }
        header->next_free = NULL;
        header->size = _size;
        block_counter++;
    }

    return returnPtr;
}

/**
 * This is a helper function that finds the next free block
 * by traversing down the all the blocks
 * @param  ptr [takes in the pointer to the current block]
 * @return     [return the pointer to the right adjacent free
 * block, if unavailable, return NULL]
 */
Header* getNextFree(Header* ptr) {
    Header* temp = getNextBlock(ptr);
    while (temp != NULL) {
        if (temp->inUse == FREE) {
            return temp;
        }
        temp = getNextBlock(temp);
    }
    return NULL;
}

/**
 * This is a helper function that finds the previous free block
 * by traversing up the all the blocks
 * @param  ptr [takes in the pointer to the current block]
 * @return     [return the pointer to the left adjacent free
 * block, if unavailable, return NULL]
 */
Header* getPrevFree(Header* ptr) {
    Header* temp = ptr->prev_block;
    while (temp != NULL) {
        if (temp->inUse == FREE) {
            return temp;
        }
        temp = temp->prev_block;
    }
    return NULL;
}

/**
 * This function computes the next block based on the location 
 * of the current header and the size of the current block's payload.
 * @param  ptr [a pointer to the current header]
 * @return     [a pointer to the next block adjacent to the right
 * if not available, return NULLptr]
 */
Header* getNextBlock(Header* ptr) {
    Header* next = NULL;
    if ((unsigned char*)ptr + ptr->size + 
        2 * HEADER_SIZE < mem + MEMORY_SIZE) {
        next = (Header*)((unsigned char*)ptr + ptr->size + HEADER_SIZE);
    }
    return next;
}

/*!
 * Free a previously allocated pointer.  oldptr should be an address returned
 * by
 * myalloc().
 *
 * This function takes advantage of the doubly linked free block list
 * to first reincorporate the newly freed block into the explicit free list
 * then it adjusts the explicit free list to first coalesce the 
 * block to the right, and then coalesce the block to the left.
 * This should be linear time, as we need to compute the
 * block's place back into the free list
 * but coalescing, which is a subroutine 
 * called from this function, should be constant time.
  */
void myfree(unsigned char *oldptr) {
    /* first, add current free block into explicit free list, then
     * we have to coalesce both sides, if possible by first coalescing
     * right side then moving the pointer to the previous left block and
     * attempt to coalesce the right block(which was the current center
     block)
     */
    if (!(oldptr >= mem && oldptr < (mem + MEMORY_SIZE))) {
        fprintf(stderr, 
            "attempting to free outside of memory pool");
        abort();
    }
    Header* current, *left, *right;
    current = (Header*)(oldptr - HEADER_SIZE);
    
    /* This checks whether the current block we're attempting to free is 
     * already freed, if it is , then we're doing something wrong
     * and we abort */
    if (current->inUse == FREE) {
        fprintf(stderr, 
            "block already freed");
        abort();
    }
    current->inUse = 0;
    /* we modify explicit free list */
    insertIntoExplicitList(current);
    right = getNextBlock(current);
    left = current->prev_block;

    /* if we can coalesce right, we do it */
    if (right != NULL) {
        if (right->inUse == FREE) {
            coalesce(current, right);
        }
    }
    /* if we can coalesce left, we do it */
    if (left != NULL) {
        if (left->inUse == FREE) {
            coalesce(left, current);
        }
    }

}

/**
 * coalesces a center block with the right side block
 * @param current the center block
 * @param right   the right side block
 *
 * This function will adjust the pointers of the center and right block
 * to merge the two. It is constant time since two pointers have already 
 * been computed for the two blocks when we freed it.
 */
void coalesce(Header* current, Header* right) {
    current->next_free = right->next_free;
    if (right->next_free != NULL) {
        right->next_free->prev_free = current;
    }
    if (getNextBlock(right) != NULL) {
        getNextBlock(right)->prev_block = current;
    }
    current->size += right->size + HEADER_SIZE;
    block_counter--;
}

/**
 * this adjusts the exlplicit free list as we free a block
 * @param current pointer to the current block.
 */
void insertIntoExplicitList(Header* current) {
    Header *left, *right;
    /*here we recompute it as a failsafe, but it should be find if we 
     *directly access the stored member */
    left = getPrevFree(current);
    right = getNextFree(current);

    /* left = current->prev_free;
     * right = current->next_free;
     */


    if (right != NULL) {
        right->prev_free = current;
        current->next_free = right;
    }

    if (left != NULL) {
        left->next_free = current;
        current->prev_free = left;
    }
}

/*!
 * Clean up the allocator state.
 * All this really has to do is free the user memory pool. This 
 * function mostly ensures that the test program doesn't leak 
 * memory, so it's easy to check if the allocator does.
 */
void close_myalloc() {
    free(mem);
}
