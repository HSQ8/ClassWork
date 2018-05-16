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
struct multimap;
struct multimap_node;
typedef struct multimap_value {
    int value;
    struct multimap_value *next;
} multimap_value;

/* Represents a key and its associated values in the multimap, as well as
 * pointers to the left and right child nodes in the multimap. */
typedef struct multimap_node {
    /* The key-value that this multimap node represents. */
    int key;
    uint64_t num_values;
    uint64_t num_spaces;

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
    struct multimap_node *root;
    int64_t size;
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
void free_multimap_node(multimap_node *node, multimap_node *root);
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
    // in this strategy, we keep indicies that we then translate into
    // pointers everytime we need to return a pointer
    // 
    // also, in this case, we compute the level based on the index position
    // and the can extend the array as we please
    // we always maintain a node* to the root as a basis for computing 
    // any pointers we need
    // will have to mark null indicies with NULL_INDEX 
    // 

    /* If the entire multimap is empty, the root will be NULL. */
    if (mm->root == NULL) {
        if (create_if_not_found) {
            mm->root = alloc_mm_node();
            mm->root->key = key;
            mm->size = 1;
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
                /* computes index of parent in array*/
                uint32_t index = node - mm->root;
                if ( index * 2 + 1 > mm->size - 1) {
                    resize_multiheap_levels(mm);
                    node = mm->root + index;
                }
                multimap_node *new = mm->root + (index * 2 + 1);
                new->key = key;
                /* Initializes values that keep track of our block
                 * memory
                 */
                new->num_values = 0;
                new->num_spaces = 0;

                new->left_child = NULL_INDEX;
                new->right_child = NULL_INDEX;

                node->left_child = (index * 2 + 1);
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
                // we only need to extend when we follow right child

                /* No right child, but caller wants us to create a new node. */
                int32_t index = node - mm->root;
                if ( index * 2 + 2 > mm->size - 1) {
                    resize_multiheap_levels(mm);
                    node = mm->root + index;
                }
                multimap_node *new = mm->root + (index * 2 + 2);
                new->key = key;
                
                /* Initializes values that keep track of our block
                 * memory
                 */
                new->num_values = 0;
                new->num_spaces = 0;

                new->left_child = NULL_INDEX;
                new->right_child = NULL_INDEX;

                node->right_child = (index * 2 + 2);
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

/**
 * double the memory pool every time we run out of space
 * @param root [description]
 */
void resize_multiheap_levels(multimap* mm) {
    multimap_node* newzone = (multimap_node *) realloc(mm->root, 
            mm->size * 4 * sizeof(multimap_node));
    if (newzone != NULL) {
        mm->root = newzone;
        mm->size *= 4;
    } else {
        exit(1);
    }
}


/* This helper function frees all values in a multimap node's value-list. */
void free_multimap_values(multimap_value *values) {
    /* Only root block needs to be freed, since it is now one continuous
     * block
     */
        while (values != NULL) {
        multimap_value *next = values->next;
#ifdef DEBUG_ZERO
        /* Clear out what we are about to free, to expose issues quickly. */
        bzero(values, sizeof(multimap_value));
#endif
        free(values);
        values = next;
    }
}


/* This helper function frees a multimap node, including its children and
 * value-list.
 */
void free_multimap_node(multimap_node *node, multimap_node* root) {
    if (node == NULL) {
        return;
    }

    /* Free the children first. */
    if (node->left_child != NULL_INDEX) {
        free_multimap_node(root + node->left_child, root);
    }

    if (node->left_child != NULL_INDEX) {
        free_multimap_node(root + node->right_child, root);
    }

    /* Free the list of values. */
    free_multimap_values(node->values);
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
    free_multimap_node(mm->root, mm->root);
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

    new_value = malloc(sizeof(multimap_value));
    new_value->value = value;
    new_value->next = NULL;

    if (node->values_tail != NULL)
        node->values_tail->next = new_value;
    else
        node->values = new_value;

    node->values_tail = new_value;
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
void mm_traverse_helper(multimap_node *root, multimap_node *node, 
    void (*f)(int key, int value)) {
    multimap_value *curr;

    if (node == NULL)
        return;

    if (node->left_child != NULL_INDEX) {
        mm_traverse_helper(root, root + node->left_child, f);
    }

    curr = node->values;
    while (curr != NULL) {
        f(node->key, curr->value);
        curr = curr->next;
    }
    if (node->right_child != NULL_INDEX) {
        mm_traverse_helper(root, root + node->right_child, f);
    }
}


/* Performs an in-order traversal of the multimap, passing each (key, value)
 * pair to the specified function.
 */
void mm_traverse(multimap *mm, void (*f)(int key, int value)) {
    /* We pass in two of the same parameter due to the recursive definition*/
    mm_traverse_helper(mm->root, mm->root, f);
}

