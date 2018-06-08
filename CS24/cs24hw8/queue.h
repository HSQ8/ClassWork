#ifndef QUEUE_H
#define QUEUE_H
#include "virtualmem.h"

/* timebit must be unsigned int
 * The way you change the number of bits you use in your time is to
 * simply change the type used here in the typedef. For instance,
 * a 16 bit time would use a unsigned 16 bit int, where as 32 bit 
 * would use uint_32_t etc. All types must be unsigned.
 */

typedef uint16_t TIMEBIT;


/*! A single node of a queue of pages. */  
typedef struct _queuenode {
    /* The page itself*/
    page_t page;
    /* The age of the page, used in the aging evict method. */
    TIMEBIT age;
    struct _queuenode *prev;
    struct _queuenode *next;
} QueueNode;


/*!
 * A queue of pages.  This type is used to keep track of pages in our 
 * evict functions.
 */
typedef struct _queue {
    QueueNode *head;  /*!< The first page in the queue. */
    QueueNode *tail;  /*!< The last page in the queue. */
} Queue;


int queue_empty(Queue *queuep);
void queue_append(Queue *queuep, page_t page);
page_t queue_take(Queue *queuep);
int queue_remove(Queue *queuep, page_t page);


#endif /* QUEUE_H */

