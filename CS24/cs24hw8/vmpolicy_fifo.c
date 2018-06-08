/*============================================================================
 * Implementation of the FIFO page replacement policy.
 *
 * We don't mind if paging policies use malloc() and free(), just because it
 * keeps things simpler.  In real life, the pager would use the kernel memory
 * allocator to manage data structures, and it would endeavor to allocate and
 * release as little memory as possible to avoid making the kernel slow and
 * prone to memory fragmentation issues.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"
#include "vmpolicy.h"


/*============================================================================
 * "Loaded Pages" Queue Data Structure
 *
 * This data structure records all pages that are currently loaded in the
 * virtual memory, so that we can choose the first page to evict very easily.
 * This data structure is defined in queue.h
 *
 * Reused from assignment 7.
 */
static Queue loaded;


/*============================================================================
 * Policy Implementation
 */

/* Initialize the policy.  Return nonzero for success, 0 for failure. */
int policy_init(int max_resident) {
    fprintf(stderr, "Using FIFO eviction policy.\n\n");
    /* There's nothing to initialize really. */
    /* Return nonzero if initialization succeeded. */
    return 1;
}


/* Clean up the data used by the page replacement policy. */
void policy_cleanup(void) {
    /* Since queue take handles freeing pointers, we just need to
     * keep taking from queue until it is empty. 
     */
    while (! queue_empty(&loaded)) {
        queue_take(&loaded);
    }
}


/* This function is called when the virtual memory system maps a page into the
 * virtual address space.  Record that the page is now resident.
 */
void policy_page_mapped(page_t page) {
    /* Add the page to our queue. */
    queue_append(&loaded, page);
}


/* This function is called when the virtual memory system has a timer tick. */
void policy_timer_tick(void) {
    /* Do nothing! */
}


/* Choose a page to evict from the collection of mapped pages.  Then, record
 * that it is evicted.  This is very simple since we are implementing a FIFO
 * page-replacement policy.
 */
page_t choose_and_evict_victim_page(void) {
    /* Take the front of the queue. */
    page_t victim = queue_take(&loaded);
    /* Handle the case where we attempt to evict with no pages loaded
     * shouldn't happen logically.
     */
    if (victim == NULL_PAGE) {
        printf("%s\n", "Attempt evict with no pages loaded, system will abort");
        abort();
    }
    return victim;
}

