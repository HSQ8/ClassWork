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

    /* The left child of the multimap node.  This will reference nodes that
     * hold keys that are strictly less than this node's key.
     * Note in my updated optimal implementation I use an int instead
     * to store not a pointer to the child, but an index to the location
     * of the child in the array.
     */
    

    int64_t left_child;

    /* The right child of the multimap node.  This will reference nodes that
     * hold keys that are strictly greater than this node's key.
     * Note in my updated optimal implementation I use an int instead
     * to store not a pointer to the child, but an index to the location
     * of the child in the array.
     */
    int64_t right_child;
} multimap_node;


/* The entry-point of the multimap data structure. */
struct multimap {
    /*
     * This is the pointer to the pool of memory where the nodes will
     * be stored in an array like fashion.
     */
    multimap_node *root;
    
    /* This value keeps track of the size of the currently allocated
     * array
     */
    int64_t size;
    
    /* This value keeps track of how many nodes there are currently
     * in the array implementation of the binary tree. It is also the 
     * index of any new nodes we wish to add. 
     */
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
void resize_multimap_pool(multimap* mm);
/* This is a helper function that initializes new nodes */
multimap_node* add_node(multimap *mm, multimap_node *node, int key);


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
 *
 * Note, in the optimized version of this function, we instead traverse
 * allocate space for any new nodes with a helper function that
 * extends the block of memory dedicated to the array. 
 */
multimap_node * find_mm_node(multimap *mm, int key,
 int create_if_not_found) {
    multimap_node *node;

    /* If the entire multimap is empty, the root will be NULL. */
    if (mm->root == NULL) {
        if (create_if_not_found) {
            mm->root = alloc_mm_node();
            mm->root->key = key;
            /* Initialize child nodes and other book keeping 
             * parameters
             */
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
            node = add_node(mm, node, key);
            /* Adjust current node. */
            node->left_child = mm->num_nodes;
            /* Increment node count. */
            mm->num_nodes++;
        }
        if (node->left_child == NULL_INDEX) {
            node = NULL;
        } else {
            /* Otherwise this op will point to a valid location. */
            node = mm->root + node->left_child;
        }
    }
        else {                   /* Follow right child */
    if (node->right_child == NULL_INDEX && create_if_not_found) {
        /* No right child, but caller wants us to create a new node. */
        node = add_node(mm, node, key);
        /* Adjust current node. */
        node->right_child = mm->num_nodes;
        /* Increment node count. */
        mm->num_nodes++;
    }
    if (node->right_child == NULL_INDEX) {
        node = NULL;
    } else {
        /* Otherwise this op will point to a valid location. */
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
 * Helper function that adds a new node to the slab and initialize
 * everything to the proper values. This function also adjusts the size
 * of the pool / slab in the memory suppose we need more space.
 * @param  mm   the pointer to the multimap
 * @param  node pointer to the current node we're considering
 * @param  key  the key value we are adding
 * @return      pointer to the new location of the node if we realloc
 */
multimap_node* add_node(multimap *mm, multimap_node *node, int key) {
    /**
     * If we don't have enough space in our current array,
     * invoke the resize function and extend the array.
     * Also readjust the node pointer to the current node
     * since realloc may copy contents to a new memory region
     */
    if ( mm->num_nodes >= mm->size) {
        int64_t index = node - mm->root;
        resize_multimap_pool(mm);
        node = mm->root + index;
    }
    multimap_node *new = mm->root + mm->num_nodes;
    /* Initialize values. */
    new->key = key;
    new->num_values = 0;
    new->num_spaces = 0;
    new->left_child = NULL_INDEX;
    new->right_child = NULL_INDEX;
    return node;

}

/**
 * This function takes a multimap struct and attempts to extend the 
 * size of the allocated array inside that is used to store the binary tree
 * representation of the nodes
 * @param mm pointer to the multimap struct
 */
void resize_multimap_pool(multimap* mm) {
    /* Attempts to realloc by either extending or copying to a new region */
    multimap_node* newzone = (multimap_node *) realloc(mm->root, 
        mm->size * 4 * sizeof(multimap_node));
    /* If extend successful, assume memory copied, adjust pointers and 
     * associated book keeping values.
     */
    if (newzone != NULL) {
        mm->root = newzone;
        mm->size *= 4;
    } else {
        /* Error handling for out of memory scenario. */
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
    /* Since now our tree is in a continuous pool, we simply free
     * values one by one.
     */
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
/**
 * In the updated mm_add_value function, we take a different approach to 
 * allocating space for the values associated with each value.
 * where as before each node pointed to a linked list of values, now each
 * node points to a continuous block of values array. Though the individual
 * value structs are now in an array one by one, we still maintain the
 * pointers and the traversal of the linked list functionalities inside the
 * array so that we may still traverse this implementation with
 * preexisting algorithms without the need for extensive modification.
 * @param mm    pointer to the multimap
 * @param key   key where the following value will be added
 * @param value value to be added at key
 */
void mm_add_value(multimap *mm, int key, int value) {
    multimap_node *node;
    multimap_value *new_value;

    assert(mm != NULL);

    /* Look up the node with the specified key.  Create if not found. */
    node = find_mm_node(mm, key, /* create */ 1);
    if (mm->root == NULL) {
        mm->root = node;
    }

    assert(node != NULL);
    assert(node->key == key);

    /* Add the new value to the multimap node. */
    if (node->num_values == 0) {
        /* allocate 1 node here, this is the base case
         * Here we also take care of some initialize work
         */
        new_value = malloc(sizeof(multimap_value));
        new_value->value = value;
        new_value->next = NULL;

        node->values = new_value;
        node->num_values++;
        node->num_spaces++;
    } else if(node->num_values < node->num_spaces) {
        /* This is the case where there are spaces left in our array,
         * so we simply create new values in the continuous chunk of free
         * space we already have.
         */

        /* Get one past the last pointer to obtain new value location. */
        new_value = (node->values + node->num_values);
        new_value->value = value;
        new_value->next = NULL;
        /* Update current tail*/
        multimap_value* tempend = node->values + node->num_values - 1;
        tempend->next = new_value;
        node->num_values++;

    } else if (node->num_values == node->num_spaces) {
        /* This is the case where we need to resize the storage space.
         * Attempts to double the block by reallocating. 
         */
        multimap_value* newzone = (multimap_value *) realloc(node->values, 
            node->num_spaces * 2 * sizeof(multimap_value));
        /* Realloc successful. */
        if(newzone != NULL) {
            /* Allocation successful, the data is assume to be copied over
             * Then we need to adjust pointers.
             */
            int i;
            node->values = newzone;
            for (i = 0; i < node->num_values; ++i) {
                (node->values + i)->next = node->values + 1 + i;
            }
            /* Done adjusting values, must now insert new node. */
            new_value = node->values + node->num_values; 
            new_value->value = value;
            new_value->next = NULL;
            
            node->num_values++;
            /* Adjust book keeping values. */
            node->num_spaces *= 2;
        } else {
            /* Simple error handling for out of memory cases. */
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

