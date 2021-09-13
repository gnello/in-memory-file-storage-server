#include <stdlib.h>
#include <string.h>
#include "../include/gnl_huffman_tree.h"
#include "./gnl_min_heap_t.c"
#include <gnl_macro_beg.h>

struct gnl_huffman_tree_node_t {
    struct gnl_huffman_tree_node_t *left;
    struct gnl_huffman_tree_node_t *right;
    int freq;
    char byte;
};

struct gnl_huffman_tree_t {

    // the bytes dictionary
    char **dictionary;

    // the min heap of the tree
    struct gnl_min_heap_t *min_heap;

    // the root node of the tree
    struct gnl_huffman_tree_node_t *node;
};

struct gnl_huffman_tree_cipher {

    // the root node of the tree
    struct gnl_huffman_tree_node_t *node;

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

static void destroy_node(struct gnl_huffman_tree_node_t *node) {
    if (node->left != NULL) {
        destroy_node(node->left);
    }

    if (node->right != NULL) {
        destroy_node(node->right);
    }

    free(node);
}

static int is_leaf(struct gnl_huffman_tree_node_t *node) {
    return node->left == NULL && node->right == NULL;
}

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

static int create_min_heap(struct gnl_huffman_tree_t *tree, int *freq) {
    int res;

    // for each byte
    for (size_t i=0; i<255; i++) {

        if (freq[i] > 0) {
            struct gnl_huffman_tree_node_t *node = create_node(freq[i], i);
            GNL_NULL_CHECK(node, errno, -1)

            res = gnl_min_heap_insert(tree->min_heap, node, node->freq);
            GNL_MINUS1_CHECK(res, errno, -1)
        }
    }

    return 0;
}

static int tree_build(struct gnl_huffman_tree_t *tree) {
    struct gnl_huffman_tree_node_t *left;
    struct gnl_huffman_tree_node_t *right;
    struct gnl_huffman_tree_node_t *node;

    int res;

    while (tree->min_heap->size > 1) {
        left = gnl_min_heap_extract_min(tree->min_heap);
        GNL_NULL_CHECK(left, errno, -1)

        right = gnl_min_heap_extract_min(tree->min_heap);
        GNL_NULL_CHECK(right, errno, -1)

        node = create_node(left->freq + right->freq, 0);
        GNL_NULL_CHECK(node, errno, -1)

        node->left = left;
        node->right = right;

        res = gnl_min_heap_insert(tree->min_heap, node, node->freq);
        GNL_MINUS1_CHECK(res, errno, -1)
    }

    tree->node = gnl_min_heap_extract_min(tree->min_heap);
    GNL_NULL_CHECK(tree->node, errno, -1)

    return 0;
}

static int create_dictionary_recursive(struct gnl_huffman_tree_node_t *node, char **dictionary, char *code_path, int index) {
    int res;

    if (node->left != NULL) {
        code_path[index] = '0';
        res = create_dictionary_recursive(node->left, dictionary, code_path, index + 1);
        GNL_MINUS1_CHECK(res, errno, -1)
    }

    if (node->right != NULL) {
        code_path[index] = '1';
        res = create_dictionary_recursive(node->right, dictionary, code_path, index + 1);
        GNL_MINUS1_CHECK(res, errno, -1)
    }

    if (is_leaf(node)) {
        dictionary[(int)node->byte] = calloc(index+1, (sizeof(char)));
        GNL_NULL_CHECK(dictionary[(int)node->byte], ENOMEM, -1)

        for (size_t i=0; i<index; i++) {
            dictionary[(int)node->byte][i] = code_path[i];
        }

        dictionary[(int)node->byte][index] = '\0';
    }

    return 0;
}

static int tree_depth(struct gnl_huffman_tree_node_t *node) {
    if (node == NULL) {
        return 0;
    }

    int ldepth = tree_depth(node->left);
    int rdepth = tree_depth(node->right);

    if (ldepth > rdepth) {
        return ldepth + 1;
    } else {
        return rdepth + 1;
    }
}

static int create_dictionary(struct gnl_huffman_tree_t *tree) {
    tree->dictionary = (char **)calloc(256, sizeof(char *));
    GNL_NULL_CHECK(tree->dictionary, ENOMEM, -1)

    int depth = tree_depth(tree->node);

    char *code_path = malloc(depth * sizeof(char));
    GNL_NULL_CHECK(code_path, ENOMEM, -1)

    create_dictionary_recursive(tree->node, tree->dictionary, code_path, 0);

    free(code_path);

    return 0;
}

static void destroy_tree_metadata(struct gnl_huffman_tree_t *tree) {
    for (size_t i=0; i<256; i++) {
        free(tree->dictionary[i]);
    }

    free(tree->dictionary);

    gnl_min_heap_destroy(tree->min_heap, NULL);

    free(tree);
}

/**
 * {@inheritDoc}
 */
struct gnl_huffman_tree_t *gnl_huffman_tree_init(const void *bytes, size_t count) {
    // allocate memory
    struct gnl_huffman_tree_t *tree = (struct gnl_huffman_tree_t *)malloc(sizeof(struct gnl_huffman_tree_t));
    GNL_NULL_CHECK(tree, ENOMEM, NULL)

    // initialize values
    tree->min_heap = gnl_min_heap_init();
    GNL_NULL_CHECK(tree->min_heap, errno, NULL)

    // calculate the frequencies
    int *freq = calculate_frequencies(bytes, count);

    // create the mean heap
    int res = create_min_heap(tree, freq);
    GNL_MINUS1_CHECK(res, errno, NULL)

    // free memory
    free(freq);

    // build the tree
    res = tree_build(tree);
    GNL_MINUS1_CHECK(res, errno, NULL)

    // assign codes
    res = create_dictionary(tree);
    GNL_MINUS1_CHECK(res, errno, NULL)

    return tree;
}

/**
 * {@inheritDoc}
 */
void gnl_huffman_tree_destroy(struct gnl_huffman_tree_t *tree) {
    destroy_node(tree->node);

    destroy_tree_metadata(tree);
}

struct gnl_huffman_tree_cipher *gnl_huffman_encode(const void *bytes, size_t count, char **dest) {
    // allocate memory
    struct gnl_huffman_tree_cipher *cipher = (struct gnl_huffman_tree_cipher *)malloc(sizeof(struct gnl_huffman_tree_cipher));
    GNL_NULL_CHECK(cipher, errno, NULL)

    struct gnl_huffman_tree_t *tree = gnl_huffman_tree_init(bytes, count);
    GNL_NULL_CHECK(tree, errno, NULL)

    *dest = NULL;

    int dest_index = 0;

    // for each byte to encode
    for (size_t i=0; i<count; i++) {
        // get the single byte
        char byte = *((char *)bytes + i);

        // get the size of the byte code
        int code_size = strlen(tree->dictionary[(int)byte]);

        // re-allocate memory
        char *temp = realloc(*dest, (dest_index + code_size) * sizeof(char));
        GNL_NULL_CHECK(temp, ENOMEM, NULL)

        *dest = temp;

        // put the code of the byte into dest
        for (size_t j=0; j<code_size; j++) {
            (*dest)[dest_index++] = tree->dictionary[(int)byte][j];
        }
    }

    // build the cipher
    cipher->node = tree->node;
    cipher->count = dest_index;

    cipher->code = calloc(cipher->count, sizeof(char));
    GNL_NULL_CHECK(cipher->code, errno, NULL)

    for (size_t i=0; i<cipher->count; i++) {
        cipher->code[i] = (*dest)[i];
    }

    destroy_tree_metadata(tree);

    return cipher;
}

int gnl_huffman_decode(const char *code, struct gnl_huffman_tree_cipher *cipher, char **dest) {
    *dest = NULL;

    struct gnl_huffman_tree_node_t *node = cipher->node;

    int dest_index = 0;

    for (size_t i=0; i<cipher->count; i++) {
        if (code[i] == '0') {
            node = node->left;
        } else {
            node = node->right;
        }

        if (is_leaf(node)) {
            // re-allocate memory
            char *temp = realloc(*dest, (dest_index + 1) * sizeof(char));
            GNL_NULL_CHECK(temp, ENOMEM, -1)

            *dest = temp;

            (*dest)[dest_index++] = node->byte;

            // reset the node
            node = cipher->node;

            printf("%c", node->byte);
        }
    }

    int res = 0;
    if (cipher->node != node) {
        errno = EINVAL;
        free(*dest);

        res = -1;
    }

    free(cipher->code);
    destroy_node(cipher->node);
    free(cipher);

    return res;
}

#include <gnl_macro_end.h>