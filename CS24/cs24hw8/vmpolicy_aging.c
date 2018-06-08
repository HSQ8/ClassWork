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
 * virtual memory, so that we can choose a random page to evict very easily.
 * This data structure is defined in queue.h
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


/* This function is called when the virtual memory system has a timer tick. */
void policy_timer_tick(void) {
    TIMEBIT bit_mask = (TIMEBIT)(1 << (sizeof(TIMEBIT) * 8 - 1));
    QueueNode* head = loaded.head;
    while(head != NULL) {
        page_t currentpage = head->page;
        head->age >>= 1;
        if (is_page_accessed(currentpage)) {
            head->age |= bit_mask;
            set_page_permission(currentpage, PAGEPERM_NONE);
            clear_page_accessed(currentpage);
        }
        head = head->next;
    }
}


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
 * that it is evicted.  This is very simple since we are implementing a random
 * page-replacement policy.
 */
page_t choose_and_evict_victim_page(void) {
    page_t victim = findyoungest(loaded);
    /* Handle the case where we attempt to evict with no pages loaded
     * shouldn't happen logically.
     */
    if (victim == NULL_PAGE) {
        printf("%s\n", "Attempt evict with no pages loaded, system will abort");
        abort();
    }
    queue_remove(&loaded, victim);
    return victim;
}

