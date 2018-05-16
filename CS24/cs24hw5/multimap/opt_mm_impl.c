#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "multimap.h"
#define NULL_INDEX -1


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
    int64_t num_values;
    int64_t num_spaces;

    /* A linked list of the values associated with this key in the multimap. */
    multimap_value *values;

    /* The tail of the linked list of values, so that we can retain the order
     * they were added to the list.
     */
    multimap_value *values_tail;

    /* The left child of the multimap node.  This will reference nodes that
     * hold keys that are strictly less than this node's key.
     */
    int64_t left_child;

    /* The right child of the multimap node.  This will reference nodes that
     * hold keys that are strictly greater than this node's key.
     */
    int64_t right_child;
} multimap_node;


/* The entry-point of the multimap data structure. */
struct multimap {
    multimap_node *root;
    
    int64_t size;
    
    /* This is also the index of the next available node*/
    int64_t num_nodes;
};


/*============================================================================
 * HELPER FUNCTION DECLARATIONS
 *
 *   Declarations of helper functions that are local to this module.  Again,
 *   these are not visible outside of this module.
 *============================================================================*/

multimap_node * alloc_mm_node();

multimap_node * find_mm_node(multimap *mm, int key,
                             int create_if_not_found);

void free_multimap_values(multimap_value *values);
void free_multimap_node(multimap_node *node, multimap* mm);
void resize_multiheap_levels(multimap* mm);


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
multimap_node * find_mm_node(multimap *mm, int key,
                             int create_if_not_found) {
    multimap_node *node;

    /* If the entire multimap is empty, the root will be NULL. */
    if (mm->root == NULL) {
        if (create_if_not_found) {
            mm->root = alloc_mm_node();
            mm->root->key = key;
            /* My implementation*/
            mm->size = 1;
            mm->num_nodes = 1;
            mm->root->left_child = NULL_INDEX;
            mm->root->right_child = NULL_INDEX;
        }
        return mm->root;
    }

    /* Now we know the multimap has at least a root node, so start there. */
    node = mm->root;
    while (1) {
        if (node->key == key) {
            break;
        }

        if (node->key > key) {   /* Follow left child */
            if (node->left_child == NULL_INDEX && create_if_not_found) {
                /* No left child, but caller wants us to create a new node. */
                
                int64_t index = node - mm->root;
                if ( mm->num_nodes >= mm->size) {
                    resize_multiheap_levels(mm);
                    node = mm->root + index;
                }
                multimap_node *new = mm->root + mm->num_nodes;

                new->key = key;
                new->num_values = 0;
                new->num_spaces = 0;
                new->left_child = NULL_INDEX;
                new->right_child = NULL_INDEX;

                node->left_child = mm->num_nodes;
                mm->num_nodes++;
            }
            if (node->left_child == NULL_INDEX) {
                node = NULL;
            } else {
                /* Otherwise this op will point to a valid location*/
                node = mm->root + node->left_child;
            }
        }
        else {                   /* Follow right child */
            if (node->right_child == NULL_INDEX && create_if_not_found) {
                /* No right child, but caller wants us to create a new node. */
                
                int64_t index = node - mm->root;
                if ( mm->num_nodes >= mm->size) {
                    resize_multiheap_levels(mm);
                    node = mm->root + index;
                }
                multimap_node *new = mm->root + mm->num_nodes;
                
                new->key = key;
                new->num_values = 0;
                new->num_spaces = 0;
                new->left_child = NULL_INDEX;
                new->right_child = NULL_INDEX;

                node->right_child = mm->num_nodes;
                mm->num_nodes++;
            }
            if (node->right_child == NULL_INDEX) {
                node = NULL;
            } else {
                /* Otherwise this op will point to a valid location*/
                node = mm->root + node->right_child;
            }
        }
        if (node == NULL) {
            break;
        }
    }
    return node;
}

void resize_multiheap_levels(multimap* mm) {
    multimap_node* newzone = (multimap_node *) realloc(mm->root, 
            mm->size * 4 * sizeof(multimap_node));
    if (newzone != NULL) {
        mm->root = newzone;
        mm->size *= 4;
    } else {
        printf("size requested %lu\n", mm->size * 4 *
         sizeof(multimap_node));
        printf("%s\n", "failed to realloc array");
        exit(1);
    }
}


/* This helper function frees all values in a multimap node's value-list. */
void free_multimap_values(multimap_value *values) {
    free(values);
}


/* This helper function frees a multimap node, including its children and
 * value-list.
 */
void free_multimap_node(multimap_node *node, multimap* mm) {
    int i;
    for (i = 0; i < mm->num_nodes; ++i) {
        free_multimap_values(mm->root[i].values);
    }
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
    free_multimap_node(mm->root, mm);
    free(mm->root);
    mm->root = NULL;
}


/* Adds the specified (key, value) pair to the multimap. */
void mm_add_value(multimap *mm, int key, int value) {
    multimap_node *node;
    multimap_value *new_value;

    assert(mm != NULL);

    /* Look up the node with the specified key.  Create if not found. */
    node = find_mm_node(mm, key, /* create */ 1);
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

        node->values = new_value;
        node->values_tail = new_value;
        node->num_values++;
        node->num_spaces++;
    } else if(node->num_values < node->num_spaces 
        && node->values_tail != NULL) {
        /* We have space left*/
        /* Get one past the last pointer to obtain new value location*/
        new_value = (node->values_tail + 1);
        new_value->value = value;
        new_value->next = NULL;
        /* Update current tail*/
        node->values_tail->next = new_value;
        node->num_values++;
        node->values_tail = new_value;

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
                (node->values + i)->next = node->values + 1 + i;
            }
            /* Done adjusting values, must now insert new node*/
            new_value = node->values + node->num_values; 
            new_value->value = value;
            new_value->next = NULL;
            
            node->values_tail = new_value;
            node->num_values++;
            node->num_spaces *= 2;
        } else {
            printf("%s\n", "failed to realloc linked list");
            exit(1);
        }
    }
}


/* Returns nonzero if the multimap contains the specified key-value, zero
 * otherwise.
 */
int mm_contains_key(multimap *mm, int key) {
    return find_mm_node(mm, key, /* create */ 0) != NULL;
}


/* Returns nonzero if the multimap contains the specified (key, value) pair,
 * zero otherwise.
 */
int mm_contains_pair(multimap *mm, int key, int value) {
    multimap_node *node;
    multimap_value *curr;

    node = find_mm_node(mm, key, /* create */ 0);
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
void mm_traverse_helper(multimap_node *node, void (*f)(int key, int value),
    multimap_node *root) {
    multimap_value *curr;

    if (node->left_child != NULL_INDEX) {
        mm_traverse_helper(root + node->left_child, f, root);
    }
    curr = node->values;
    while (curr != NULL) {
        f(node->key, curr->value);
        curr = curr->next;
    }

    if (node->right_child != NULL_INDEX) {
        mm_traverse_helper(root + node->right_child, f, root);
    }
}


/* Performs an in-order traversal of the multimap, passing each (key, value)
 * pair to the specified function.
 */
void mm_traverse(multimap *mm, void (*f)(int key, int value)) {
    mm_traverse_helper(mm->root, f, mm->root);
}

