#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "multimap.h"


/*============================================================================
 * TYPES
 *
 *   These types are defined in the implementation file so that they can
 *   be kept hidden to code outside this source file.  This is not for any
 *   security reason, but rather just so we can enforce that our testing
 *   programs are generic and don't have any access to implementation details.
 *============================================================================*/

 /* Represents a value that is associated with a given key in the multimap. */
typedef struct multimap_value {
    int value;
    struct multimap_value *next;
} multimap_value;


/* Represents a key and its associated values in the multimap, as well as
 * pointers to the left and right child nodes in the multimap. */
typedef struct multimap_node {
    /* The key-value that this multimap node represents. */
    int key;
    uint32_t num_values;
    uint32_t num_spaces;

    /* A linked list of the values associated with this key in the multimap. */
    multimap_value *values;

    /* The tail of the linked list of values, so that we can retain the order
     * they were added to the list.
     */
    multimap_value *values_tail;

    /* The left child of the multimap node.  This will reference nodes that
     * hold keys that are strictly less than this node's key.
     */
    uint64_t left_child;

    /* The right child of the multimap node.  This will reference nodes that
     * hold keys that are strictly greater than this node's key.
     */
    uint64_t right_child;
    multimap * self_ptr;
    uint64_t num_levels;
    uint64_t num_occupied_levels;
} multimap_node;


/* The entry-point of the multimap data structure. */
struct multimap {
    multimap_node *root;
    /* With this policy we double the number of levels every time
     * we don't have enough space
     */
};


/*============================================================================
 * HELPER FUNCTION DECLARATIONS
 *
 *   Declarations of helper functions that are local to this module.  Again,
 *   these are not visible outside of this module.
 *============================================================================*/

multimap_node * alloc_mm_node();

multimap_node * find_mm_node(multimap_node *root, int key,
                             int create_if_not_found);

void free_multimap_values(multimap_value *values);
void free_multimap_node(multimap_node *node);


/*============================================================================
 * FUNCTION IMPLEMENTATIONS
 *============================================================================*/

/* Allocates a multimap node, and zeros out its contents so that we know what
 * the initial value of everything will be.
 */
multimap_node * alloc_mm_node() {
    multimap_node *node = malloc(sizeof(multimap_node));
    bzero(node, sizeof(multimap_node));


    return node;
}


/* This helper function searches for the multimap node that contains the
 * specified key.  If such a node doesn't exist, the function can initialize
 * a new node and add this into the structure, or it will simply return NULL.
 * The one exception is the root - if the root is NULL then the function will
 * return a new root node.
 */
multimap_node * find_mm_node(multimap_node *root, int key,
                             int create_if_not_found) {
    multimap_node *node;

    /* If the entire multimap is empty, the root will be NULL. */
    if (root == NULL) {
        if (create_if_not_found) {
            root = alloc_mm_node();
            root->key = key;
        }
        return root;
    }

    /* Now we know the multimap has at least a root node, so start there. */
    node = root;
    while (1) {
        if (node->key == key)
            break;

        if (node->key > key) {   /* Follow left child */
            if (node->left_child == NULL && create_if_not_found) {
                /* No left child, but caller wants us to create a new node. */
                multimap_node *new = alloc_mm_node();
                new->key = key;
                /* Initializes values that keep track of our block
                 * memory
                 */
                new->num_values = 0;
                new->num_spaces = 0;

                node->left_child = new;
            }
            node = node->left_child;
        }
        else {                   /* Follow right child */
            if (node->right_child == NULL && create_if_not_found) {
                /* No right child, but caller wants us to create a new node. */
                multimap_node *new = alloc_mm_node();
                new->key = key;
                
                /* Initializes values that keep track of our block
                 * memory
                 */
                new->num_values = 0;
                new->num_spaces = 0;

                node->right_child = new;
            }
            node = node->right_child;
        }

        if (node == NULL)
            break;
    }

    return node;
}

void resize_multiheap_levels() {

}


/* This helper function frees all values in a multimap node's value-list. */
void free_multimap_values(multimap_value *values) {
    /* Only root block needs to be freed, since it is now one continuous
     * block
     */
    free(values);
}


/* This helper function frees a multimap node, including its children and
 * value-list.
 */
void free_multimap_node(multimap_node *node) {
    if (node == NULL)
        return;

    /* Free the children first. */
    free_multimap_node(node->left_child);
    free_multimap_node(node->right_child);

    /* Free the list of values. */
    free_multimap_values(node->values);

#ifdef DEBUG_ZERO
    /* Clear out what we are about to free, to expose issues quickly. */
    bzero(node, sizeof(multimap_node));
#endif
    free(node);
}


/* Initialize a multimap data structure. */
multimap * init_multimap() {
    multimap *mm = malloc(sizeof(multimap));
    mm->root = NULL;
    return mm;
}


/* Release all dynamically allocated memory associated with the multimap
 * data structure.
 */
void clear_multimap(multimap *mm) {
    assert(mm != NULL);
    free_multimap_node(mm->root);
    mm->root = NULL;
}


/* Adds the specified (key, value) pair to the multimap. */
void mm_add_value(multimap *mm, int key, int value) {
    multimap_node *node;
    multimap_value *new_value;

    assert(mm != NULL);

    /* Look up the node with the specified key.  Create if not found. */
    node = find_mm_node(mm->root, key, /* create */ 1);
    if (mm->root == NULL)
        mm->root = node;

    assert(node != NULL);
    assert(node->key == key);

    /* Add the new value to the multimap node. */
    if (node->num_values == 0) {
        // allocate 1 node here, this is the base case
        new_value = malloc(sizeof(multimap_value));
        new_value->value = value;
        new_value->next = NULL;

        if (node->values_tail != NULL) {
            node->values_tail->next = new_value;
        }
        else {
            node->values = new_value;
        }
        node->values_tail = new_value;
        node->num_values++;
        node->num_spaces++;
    } else if(node->num_values < node->num_spaces 
        && node->values_tail != NULL) {
        /* We have space left*/
        /* Increment the last pointer*/
        new_value = (node->values_tail)++;
        new_value->value = value;
        new_value->next = NULL;
        node->num_values++;

    } else if (node->num_values == node->num_spaces) {
        /* This is the case where we need to resize the storage space*/
        /* Attempts to double the block by reallocating*/
        multimap_value* newzone = (multimap_value *) realloc(node->values, 
            node->num_spaces * 2 * sizeof(multimap_value));
        if(newzone != NULL) {
            /* Allocation successful, the data is assume to be copied over*/
            /* Then we need to adjust pointers*/
            int i;
            node->values = newzone;
            for (i = 0; i < node->num_values; ++i) {
                (node->values + i)->next = newzone + 1 + i;
            }
            node->values_tail = newzone + node->num_values;
            node->values_tail->next = NULL;

            /* Done adjusting values, must now insert new node*/
            new_value = (node->values_tail)++;
            new_value->value = value;
            new_value->next = NULL;
            node->num_values++;
            node->num_spaces *= 2;
        } else {
            exit(1);
        }
    }
    
}


/* Returns nonzero if the multimap contains the specified key-value, zero
 * otherwise.
 */
int mm_contains_key(multimap *mm, int key) {
    return find_mm_node(mm->root, key, /* create */ 0) != NULL;
}


/* Returns nonzero if the multimap contains the specified (key, value) pair,
 * zero otherwise.
 */
int mm_contains_pair(multimap *mm, int key, int value) {
    multimap_node *node;
    multimap_value *curr;

    node = find_mm_node(mm->root, key, /* create */ 0);
    if (node == NULL)
        return 0;

    curr = node->values;
    while (curr != NULL) {
        if (curr->value == value)
            return 1;

        curr = curr->next;
    }

    return 0;
}


/* This helper function is used by mm_traverse() to traverse every pair within
 * the multimap.
 */
void mm_traverse_helper(multimap_node *node, void (*f)(int key, int value)) {
    multimap_value *curr;

    if (node == NULL)
        return;

    mm_traverse_helper(node->left_child, f);

    curr = node->values;
    while (curr != NULL) {
        f(node->key, curr->value);
        curr = curr->next;
    }

    mm_traverse_helper(node->right_child, f);
}


/* Performs an in-order traversal of the multimap, passing each (key, value)
 * pair to the specified function.
 */
void mm_traverse(multimap *mm, void (*f)(int key, int value)) {
    mm_traverse_helper(mm->root, f);
}

