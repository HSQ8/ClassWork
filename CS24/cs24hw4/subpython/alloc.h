/*! \file
 * Declarations for a simple memory allocator.  The allocator manages a small
 * pool of memory, provides memory chunks on request, and reintegrates freed
 * memory back into the pool.
 *
 * Adapted from Andre DeHon's CS24 2004, 2006 material.
 * Copyright (C) California Institute of Technology, 2004-2009.
 * All rights reserved.
 */

#ifndef IMPALLOC_H
#define IMPALLOC_H

#include "types.h"
#include <stdlib.h>
#include <string.h>


/* Initializes allocator state, and memory pool state too. */
void init_alloc(int memory_size);

/* Returns true if an address is within the pool; false otherwise. */
bool is_pool_address(void *addr);

/* Attempt to allocate a value from the implicit allocator. */
Value * alloc(ValueType type, int data_size);

/* Dereference a Reference into its corresponding Value. */
Value * deref(Reference ref);

/* Print all allocated objects and free regions in the pool. */
void memdump(void);

/* Runs the garbage collector to reclaim unused space. */
int collect_garbage(void);

/* Clean up the allocator and memory pool state. */
void close_alloc(void);
/* Iterate through global variables and find reachable objects*/
void mark_references_not_free(const char *name, Reference ref);

/* Compacts the memory and collects garbage*/
void compact_memory(void);

#endif /* IMPALLOC_H */
