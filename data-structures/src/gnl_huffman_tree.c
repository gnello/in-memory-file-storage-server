#include <stdlib.h>
#include <string.h>
#include "../include/gnl_huffman_tree.h"
#include "./gnl_min_heap_t.c"
#include <gnl_macro_beg.h>

/**
 * The node structure of the huffman tree.
 */
struct gnl_huffman_tree_node_t {

    // the left node
    struct gnl_huffman_tree_node_t *left;

    // the right node
    struct gnl_huffman_tree_node_t *right;

    // the frequency of the byte
    int freq;

    // the byte
    char byte;
};

/**
 * {@inheritDoc}
 */
struct gnl_huffman_tree_cipher {

    // the root node of the tree
    struct gnl_huffman_tree_node_t *root;

    // the number of bytes in the encoded series
    size_t count;

    // the encoded series
    char *code;
};

/**
 * Create a new huffman tree node.
 *
 * @param freq  The frequency to assign to the new node.
 * @param byte  The byte of which the frequency refers to.
 *
 * @return      The new node created on success, NULL otherwise.
 */
static struct gnl_huffman_tree_node_t *create_node(int freq, char byte) {
    // allocate memory
    struct gnl_huffman_tree_node_t *node = (struct gnl_huffman_tree_node_t *)malloc(sizeof(struct gnl_huffman_tree_node_t));
    GNL_NULL_CHECK(node, ENOMEM, NULL)

    // initialize the new node
    node->freq = freq;
    node->byte = byte;
    node->left = NULL;
    node->right = NULL;

    return node;
}

/**
 * Recursively destroy the given node and his sub-trees.
 *
 * @param node  The node to destroy.
 */
static void destroy_node(struct gnl_huffman_tree_node_t *node) {
    if (node->left != NULL) {
        destroy_node(node->left);
    }

    if (node->right != NULL) {
        destroy_node(node->right);
    }

    free(node);
}

/**
 * Check whether the given node is a leaf or not.
 *
 * @param node  The node to check.
 *
 * @return      Returns 1 if the given node is a leaf,
 *              0 otherwise.
 */
static int is_leaf(struct gnl_huffman_tree_node_t *node) {
    return node->left == NULL && node->right == NULL;
}

/**
 * Calculate the frequencies of each byte in the given array bytes.
 *
 * @param bytes The array of bytes.
 * @param count The number of bytes.
 *
 * @return      Returns a pointer to the frequencies array created
 *              on success, -1 otherwise.
 */
static int *calculate_frequencies(const void *bytes, size_t count) {
    // 1 byte = 2^8 bits
    int *freq = calloc(256, sizeof(int));
    GNL_NULL_CHECK(freq, ENOMEM, NULL)

    // calculate the frequencies
    for (size_t i=0; i<count; i++) {
        freq[(int)*((char *)bytes + i)]++;
    }

    return freq;
}

/**
 * Create a min heap based on the given frequencies.
 *
 * @param freq  The frequencies array to use.
 *
 * @return      Returns the created min_heap on success,
 *              NULL otherwise.
 */
static struct gnl_min_heap_t *create_min_heap(int *freq) {
    // instantiate a new heap
    struct gnl_min_heap_t *min_heap = gnl_min_heap_init();
    GNL_NULL_CHECK(min_heap, errno, NULL)

    int res;

    // for each byte
    for (size_t i=0; i<255; i++) {

        if (freq[i] > 0) {
            struct gnl_huffman_tree_node_t *node = create_node(freq[i], i);
            GNL_NULL_CHECK(node, errno, NULL)

            res = gnl_min_heap_insert(min_heap, node, node->freq);
            GNL_MINUS1_CHECK(res, errno, NULL)
        }
    }

    return min_heap;
}

/**
 * Create a huffman tree from the given min heap.
 *
 * @param min_heap  The min heap to use to create the huffman tree.
 *
 * @return          Returns the huffman tree created on success,
 *                  NULL otherwise.
 */
static struct gnl_huffman_tree_t *create_tree(struct gnl_min_heap_t *min_heap) {
    struct gnl_huffman_tree_t *tree = (struct gnl_huffman_tree_t *)malloc(sizeof(struct gnl_huffman_tree_t));
    GNL_NULL_CHECK(tree, ENOMEM, NULL)

    struct gnl_huffman_tree_node_t *left;
    struct gnl_huffman_tree_node_t *right;
    struct gnl_huffman_tree_node_t *node;

    int res;

    while (min_heap->size > 1) {
        left = gnl_min_heap_extract_min(min_heap);
        GNL_NULL_CHECK(left, errno, NULL)

        right = gnl_min_heap_extract_min(min_heap);
        GNL_NULL_CHECK(right, errno, NULL)

        node = create_node(left->freq + right->freq, 0);
        GNL_NULL_CHECK(node, errno, NULL)

        node->left = left;
        node->right = right;

        res = gnl_min_heap_insert(min_heap, node, node->freq);
        GNL_MINUS1_CHECK(res, errno, NULL)
    }

    tree->root = gnl_min_heap_extract_min(min_heap);
    GNL_NULL_CHECK(tree->root, errno, NULL)

    return tree;
}

/**
 * Recursively get the highest depth of the sub-tree of
 * the given node.
 *
 * @param node  The node to use to start.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
static int tree_depth(struct gnl_huffman_tree_node_t *node) {
    if (node == NULL) {
        return 0;
    }

    // get the left and right depths
    int ldepth = tree_depth(node->left);
    int rdepth = tree_depth(node->right);

    // pick the highest depth
    if (ldepth > rdepth) {
        return ldepth + 1;
    } else {
        return rdepth + 1;
    }
}

/**
 * Recursively create a dictionary for the given tree node.
 *
 * @param node          The node to use to start.
 * @param dictionary    The pointer where to store the dictionary.
 * @param code          The code of the current node. It is filled during
 *                      the recursive calls.
 * @param index         The current index of the code char array. It is
 *                      assigned during the recursive calls.
 *
 * @return              Returns 0 on success, -1 otherwise.
 */
static int create_dictionary_recursive(struct gnl_huffman_tree_node_t *node, char **dictionary, char *code, int index) {
    int res;

    // check the left node
    if (node->left != NULL) {
        code[index] = '0';

        // repeat on the sub-tree
        res = create_dictionary_recursive(node->left, dictionary, code, index + 1);
        GNL_MINUS1_CHECK(res, errno, -1)
    }

    // check the right node
    if (node->right != NULL) {
        code[index] = '1';

        // repeat on the sub-tree
        res = create_dictionary_recursive(node->right, dictionary, code, index + 1);
        GNL_MINUS1_CHECK(res, errno, -1)
    }

    // if the node is a leaf
    if (is_leaf(node)) {
        // allocate memory for the dictionary
        dictionary[(int)node->byte] = calloc(index+1, (sizeof(char)));
        GNL_NULL_CHECK(dictionary[(int)node->byte], ENOMEM, -1)

        // assign the code to the byte into the dictionary
        for (size_t i=0; i<index; i++) {
            dictionary[(int)node->byte][i] = code[i];
        }

        // close the code
        dictionary[(int)node->byte][index] = '\0';
    }

    return 0;
}

/**
 * Create a dictionary for the given tree.
 *
 * @param tree  The huffman tree to use.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
static int create_dictionary(struct gnl_huffman_tree_t *tree) {
    // allocate memory
    tree->dictionary = (char **)calloc(256, sizeof(char *));
    GNL_NULL_CHECK(tree->dictionary, ENOMEM, -1)

    // get the tree depth
    int depth = tree_depth(tree->root);

    // allocate memory for the code
    char *code = malloc(depth * sizeof(char));
    GNL_NULL_CHECK(code, ENOMEM, -1)

    create_dictionary_recursive(tree->root, tree->dictionary, code, 0);

    // free memory
    free(code);

    return 0;
}

/**
 * Destroy the given huffman tree but preserve his nodes.
 *
 * @param tree  The tree to destroy.
 */
static void destroy_tree_safe(struct gnl_huffman_tree_t *tree) {
    for (size_t i=0; i<256; i++) {
        free(tree->dictionary[i]);
    }

    free(tree->dictionary);
    free(tree);
}

/**
 * {@inheritDoc}
 */
struct gnl_huffman_tree_t *gnl_huffman_tree_init(const void *bytes, size_t count) {

    // calculate the frequencies
    int *freq = calculate_frequencies(bytes, count);

    // create the mean heap
    struct gnl_min_heap_t *min_heap = create_min_heap(freq);
    GNL_NULL_CHECK(min_heap, errno, NULL)

    // free memory
    free(freq);

    // build the tree
    struct gnl_huffman_tree_t *tree = create_tree(min_heap);
    GNL_NULL_CHECK(tree, errno, NULL)

    // free memory
    gnl_min_heap_destroy(min_heap, NULL);

    // assign codes
    int res = create_dictionary(tree);
    GNL_MINUS1_CHECK(res, errno, NULL)

    return tree;
}

/**
 * {@inheritDoc}
 */
void gnl_huffman_tree_destroy(struct gnl_huffman_tree_t *tree) {
    destroy_tree_safe(tree);
    destroy_node(tree->root);
}

/**
 * {@inheritDoc}
 */
struct gnl_huffman_tree_cipher *gnl_huffman_encode(const void *bytes, size_t count, char **dest) {
    // allocate memory
    struct gnl_huffman_tree_cipher *cipher = (struct gnl_huffman_tree_cipher *)malloc(sizeof(struct gnl_huffman_tree_cipher));
    GNL_NULL_CHECK(cipher, errno, NULL)

    struct gnl_huffman_tree_t *tree = gnl_huffman_tree_init(bytes, count);
    GNL_NULL_CHECK(tree, errno, NULL)

    // initialize the destination
    *dest = NULL;

    // the current offset of the destination
    int offset = 0;

    // for each byte to encode
    for (size_t i=0; i<count; i++) {
        // get the single byte
        char byte = *((char *)bytes + i);

        // get the size of the byte code
        int code_size = strlen(tree->dictionary[(int)byte]);

        // re-allocate memory
        char *temp = realloc(*dest, (offset + code_size) * sizeof(char));
        GNL_NULL_CHECK(temp, ENOMEM, NULL)

        *dest = temp;

        // put the code of the byte into dest
        for (size_t j=0; j<code_size; j++) {
            (*dest)[offset++] = tree->dictionary[(int)byte][j];
        }
    }

    // build the cipher
    cipher->root = tree->root;
    cipher->count = offset;

    // allocate memory
    cipher->code = calloc(cipher->count, sizeof(char));
    GNL_NULL_CHECK(cipher->code, errno, NULL)

    // copy the code
    for (size_t i=0; i<cipher->count; i++) {
        cipher->code[i] = (*dest)[i];
    }

    // destroy the tree but preserve his nodes
    // for the cipher
    destroy_tree_safe(tree);

    return cipher;
}

/**
 * {@inheritDoc}
 */
int gnl_huffman_decode(const char *code, struct gnl_huffman_tree_cipher *cipher, char **dest) {
    // initialize the destination
    *dest = NULL;

    // start from the root of the cipher huffman tree
    struct gnl_huffman_tree_node_t *node = cipher->root;

    // the current offset of the destination
    int offset = 0;

    // for each byte of the code
    for (size_t i=0; i<cipher->count; i++) {

        // re-assign the current node based on
        // the code
        if (code[i] == '0') {
            node = node->left;
        } else {
            node = node->right;
        }

        // if the node is a leaf, then it is time to decode
        if (is_leaf(node)) {
            // re-allocate memory for the destination
            char *temp = realloc(*dest, (offset + 1) * sizeof(char));
            GNL_NULL_CHECK(temp, ENOMEM, -1)

            // re-assign the pointer
            *dest = temp;

            // actual decode: assign the byte to the
            // current destination offset
            (*dest)[offset++] = node->byte;

            // reset the node
            node = cipher->root;
        }
    }

    // validate the decode
    int res = 0;

    // if cipher->root != node, then the
    // given cipher or code is invalid
    if (cipher->root != node) {
        errno = EINVAL;
        free(*dest);

        res = -1;
    }

    // free memory
    free(cipher->code);
    destroy_node(cipher->root);
    free(cipher);

    return res;
}

#include <gnl_macro_end.h>