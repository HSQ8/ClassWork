/*============================================================================
 * Implementation of the RANDOM page replacement policy.
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
#define NULL_AGE -1


/*============================================================================
 * "Loaded Pages" Queue Data Structure
 *
 * This data structure records all pages that are currently loaded in the
 * virtual memory, so that we can choose the youngest page to evict 
 * very easily.
 * This data structure is defined in queue.h and is also reused from 
 * assignment 7.
 */
static Queue loaded;


/*============================================================================
 * Policy Implementation
 */

/* Initialize the policy.  Return nonzero for success, 0 for failure. */
int policy_init(int max_resident) {
    fprintf(stderr, "Using Aging eviction policy.\n\n");
    return 1;
}


/* Clean up the data used by the page replacement policy. */
void policy_cleanup(void) {
    while (! queue_empty(&loaded)) {
        queue_take(&loaded);
    }
}


/* This function is called when the virtual memory system maps a page into the
 * virtual address space.  Record that the page is now resident.
 */
void policy_page_mapped(page_t page) {
    queue_append(&loaded, page);
}


/* This function is called when the virtual memory system has a timer tick.
 *
 * This will set the age of each page according to whether it has 
 * been accessed or not. We always bit-shift right the page and then
 * set the most significant bit to whether the page has been accessed
 * or not. This way, we can see both when the page has been accessed 
 * and how often it was accessed.
 */
void policy_timer_tick(void) {
    /* Generate a bit mask based on the varying number of bits we 
     * use for the time.
     */
    TIMEBIT bit_mask = (TIMEBIT)(1 << ((sizeof(TIMEBIT) * 8 - 1)));
    QueueNode* head = loaded.head;
    /* Iterate through all pages. */
    while(head != NULL) {
        page_t currentpage = head->page;
        head->age >>= 1;
        if (is_page_accessed(currentpage)) {
            /* Set the bit mask only if the page has been accessed. */
            head->age |= bit_mask;
            /* Here we set the permission to NONE and clear 
             * flag accessed in order to
             * trigger another SEGV_ACCERR error. This is the only
             * way we'll know if the page was accessed again between
             * timer ticks.
             */
            set_page_permission(currentpage, PAGEPERM_NONE);
            clear_page_accessed(currentpage);
        }
        head = head->next;
    }
}

/**
 * Helper function that finds the youngest page based on age.
 * Will iterate through all ages.
 * @param  loaded the loaded queue
 * @return        page_t, which is the page number of youngest page.
 */
page_t findyoungest(Queue loaded) {
    QueueNode* head = loaded.head;
    page_t victim = head->page;
    TIMEBIT current_age = head->age;

    while(head != NULL) {
        if (head->age < current_age) {
            current_age = head->age;
            victim = head->page;
        }
        head = head->next;
    }
    return victim;
}


/* Choose a page to evict from the collection of mapped pages.  Then, record
 * that it is evicted.  This is very simple since we are implementing a aging
 * page-replacement policy, thus we take the lowest age one.
 */
page_t choose_and_evict_victim_page(void) {
    /* We get the youngest lowest age (which actually the oldest and 
     * least accessed one) with this helper function. 
     */
    page_t victim = findyoungest(loaded);
    /* Handle the case where we attempt to evict with no pages loaded
     * shouldn't happen logically.
     */
    if (victim == NULL_PAGE) {
        printf("%s\n", "Attempt evict with no pages loaded, system will abort");
        abort();
    }
    /* If we evict the page, we must also remove it from the queue. */
    queue_remove(&loaded, victim);
    return victim;
}

