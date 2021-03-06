/*============================================================================
 * Implementation of the CLRU page replacement policy.
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
#include "virtualmem.h"


/*============================================================================
 * "Loaded Pages" Queue Data Structure
 *
 * This data structure records all pages that are currently loaded in the
 * virtual memory, so that we can choose the first page (after processing) 
 * to evict very easily.
 * This data structure is defined in queue.h
 *
 * This is also reused from assignment 7.
 */
static Queue loaded;


/*============================================================================
 * Policy Implementation
 */

/* Initialize the policy.  Return nonzero for success, 0 for failure. */
int policy_init(int max_resident) {
    fprintf(stderr, "Using CLRU eviction policy.\n\n");
    /* There's nothing to initialize really. */
    /* Return nonzero if initialization succeeded. */
    return 1;
}


/* Clean up the data used by the page replacement policy. */
void policy_cleanup(void) {
    /* Clear out our queue. */
    while (! queue_empty(&loaded)) {
        queue_take(&loaded);
    }
}


/* This function is called when the virtual memory system maps a page into the
 * virtual address space.  Record that the page is now resident.
 */
void policy_page_mapped(page_t page) {
    /* Add the page to the queue. */
    queue_append(&loaded, page);
}


/* This function is called when the virtual memory system has a timer tick. */

/**
 * Every time the clock ticks, we must move pages to the back of the 
 * queue according to whether their access bit was set or not,
 * then we update their access bit so we may detect their accession again 
 * in the next interval.
 */
void policy_timer_tick(void) {
    QueueNode* head = loaded.head;
    page_t temppage = NULL_PAGE;
    /* iterate through all pages. */
    while(head != NULL) {
        temppage = head->page;
        head = head->next;
        /* If page is accessed, we move it to the back of the queue. */
        if (is_page_accessed(temppage)) {
            if (queue_remove(&loaded, temppage) == 0) {
                printf("%s\n", "failed to remove from empty queue");
                abort();
            }
            /* Here we set the permission to NONE and clear 
             * flag accessed in order to
             * trigger another SEGV_ACCERR error. This is the only
             * way we'll know if the page was accessed again between
             * timer ticks.
             */
            clear_page_accessed(temppage);
            /* Add to back of queue. */
            queue_append(&loaded, temppage);
            set_page_permission(temppage, PAGEPERM_NONE);
        }
    }
}


/* Choose a page to evict from the collection of mapped pages.  Then, record
 * that it is evicted.  This is very simple since we are implementing a CLRU
 * page-replacement policy.
 */
page_t choose_and_evict_victim_page(void) {
    /* Evict the first thing in the queue. */
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

