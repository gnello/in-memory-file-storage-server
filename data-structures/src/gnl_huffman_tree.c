#include <stdlib.h>
#include <string.h>
#include <gnl_macro_beg.h>

struct gnl_huffman_tree_node_t {
    struct gnl_huffman_tree_node_t *left;
    struct gnl_huffman_tree_node_t *right;
    int freq;
    char byte;
};

struct gnl_huffman_tree_t {

    // the bytes code
    char **dictionary;

    // the number of nodes of the tree
    int nodes_count;

    // the tail index
    int tail;

    // the array of nodes of the tree
    struct gnl_huffman_tree_node_t **nodes;
};

/**
 * Insert the given node into the given huffman tree.
 *
 * @param tree  The huffman tree where to insert the node.
 * @param node  The node to insert.
 *
 * @return      Returns 0 on success, -1 otherwise.
 */
static int tree_insert(struct gnl_huffman_tree_t *tree, struct gnl_huffman_tree_node_t *node) {
    // validate parameters
    GNL_NULL_CHECK(tree, EINVAL, -1)
    GNL_NULL_CHECK(node, EINVAL, -1)

    // the parent node index
    int j;

    // the current node index, this is the candidate
    // index where to insert the given node
    int i = tree->tail;

    // re-allocate memory
    struct gnl_huffman_tree_node_t **temp = realloc(tree->nodes, (tree->nodes_count + 1) * sizeof(struct gnl_huffman_tree_node_t *));
    GNL_NULL_CHECK(temp, errno, -1)

    tree->nodes = temp;

    // starting from the tail, climb the tree
    // (i/2 is the parent of the i node)
    while ((j = (i-1) / 2)) {
        // if the frequency of the parent node is less or equal
        // than the frequency of the node to insert, stop
        if (tree->nodes[j]->freq <= node->freq) {
            break;
        }

        // downgrade the parent node
        tree->nodes[i] = tree->nodes[j];

        // update the candidate index
        i = j;
    }

    // at this point, we have to cases:
    // - case 1: the tree was empty, so we insert the node
    // at the root (in this case i = 1)
    // - case 2; the tree was not empty, so we insert the node
    // before the smallest and after the grater (in this case
    // tree->nodes[i] >= tree->nodes[i-1] && tree->nodes[i] < tree->nodes[i+1])
    tree->nodes[i] = node;

    // increase the tail index
    tree->tail++;

    return 0;
}

/**
 * Remove the min node from the given huffman tree.
 *
 * @param tree  The huffman tree from where remove the min node.
 *
 * @return      Returns the node removed on success, -1 otherwise.
 */
static struct gnl_huffman_tree_node_t *tree_remove(struct gnl_huffman_tree_t *tree) {
    // validate parameters
    GNL_NULL_CHECK(tree, EINVAL, NULL)
    GNL_NULL_CHECK(tree, EINVAL, NULL)

    // the current node index, if i=0, than
    // this is the min node
    int i = 0;

    // the left index
    int l;

    // get the min node
    struct gnl_huffman_tree_node_t *node = tree->nodes[i];

    // if the tail count is <1 do not
    // remove anything
    if (tree->tail < 1) {
        return NULL;
    }

    // decrease the tail count
    tree->tail--;

    // while the left node index is less than the tail count
    while ((l = (i*2) + 1) < tree->tail) {

        // if the right node index frequency is less than
        // the left node frequency
        if (l+1 < tree->tail && tree->nodes[l+1]->freq < tree->nodes[l]->freq) {
            // increase the left index
            l++;
        }

        // downgrade the i node
        tree->nodes[i] = tree->nodes[l];

        // update the min node index
        i = l;
    }

    // assign the node
    tree->nodes[i] = tree->nodes[tree->tail];

    return node;
}

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

void tmp(struct gnl_huffman_tree_node_t *node) {
    printf("%c %d\n", node->byte, node->freq);

    if (node->left != NULL) {
        printf("left: ");
        tmp(node->left);
    }
    if (node->right != NULL) {
        printf("right: ");
        tmp(node->right);
    }
}

static int create_min_heap(struct gnl_huffman_tree_t *tree, int *freq) {
    int res;

    // for each byte
    for (size_t i=0; i<255; i++) {

        if (freq[i] > 0) {
            struct gnl_huffman_tree_node_t *node = create_node(freq[i], i);
            GNL_NULL_CHECK(node, errno, -1)

            res = tree_insert(tree, node);
            GNL_MINUS1_CHECK(res, errno, -1)

            tree->nodes_count++;
        }
    }

    return 0;
}

static int tree_build(struct gnl_huffman_tree_t *tree) {
    struct gnl_huffman_tree_node_t *left;
    struct gnl_huffman_tree_node_t *right;
    struct gnl_huffman_tree_node_t *node;

    int res;

    while (tree->tail > 1) {
        left = tree_remove(tree);
        GNL_NULL_CHECK(left, errno, -1)

        right = tree_remove(tree);
        GNL_NULL_CHECK(right, errno, -1)

        node = create_node(left->freq + right->freq, 0);
        GNL_NULL_CHECK(node, errno, -1)

        node->left = left;
        node->right = right;

        res = tree_insert(tree, node);
        GNL_MINUS1_CHECK(res, errno, -1)
    }
    tmp(tree->nodes[0]);
    printf("-------------\n");
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
        dictionary[(int)node->byte] = malloc(index * (sizeof(char)));
        GNL_NULL_CHECK(dictionary[(int)node->byte], ENOMEM, -1)

printf("%c:", node->byte);
        for (size_t i=0; i<index; i++) {
            dictionary[(int)node->byte][i] = code_path[i];

            printf("%c", code_path[i]);
        } printf("\n");
    }

    return 0;
}

static int create_dictionary(struct gnl_huffman_tree_t *tree) {
    tree->dictionary = (char **)calloc(256, sizeof(char *));
    GNL_NULL_CHECK(tree->dictionary, ENOMEM, -1)

    char *code_path = malloc(tree->nodes_count * sizeof(char));
    GNL_NULL_CHECK(code_path, ENOMEM, -1)

    create_dictionary_recursive(tree->nodes[0], tree->dictionary, code_path, 0);

    free(code_path);

    return 0;
}

/**
 * {@inheritDoc}
 */
struct gnl_huffman_tree_t *gnl_huffman_tree_init(const void *bytes, size_t count) {
    // allocate memory
    struct gnl_huffman_tree_t *tree = (struct gnl_huffman_tree_t *)malloc(sizeof(struct gnl_huffman_tree_t));
    GNL_NULL_CHECK(tree, ENOMEM, NULL)

    // initialize values
    tree->nodes_count = 0;
    tree->tail = 0;
    tree->nodes = NULL;

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
    destroy_node(tree->nodes[0]);

    for (size_t i=0; i<256; i++) {
        free(tree->dictionary[i]);
    }

    free(tree->dictionary);

    free(tree->nodes);

    free(tree);
}

int gnl_huffman_encode(const struct gnl_huffman_tree_t *tree, char **dest, const void *bytes, size_t count) {
    *dest = NULL;

    size_t dest_index = 0;

    for (size_t i=0; i<count; i++) {
        // get the single byte
        char byte = *((char *)bytes + i);

        // get the size of the byte code
        int code_size = strlen(tree->dictionary[(int)byte]);

        // re-allocate memory
        char *temp = realloc(*dest, (dest_index + code_size) * sizeof(char));
        GNL_NULL_CHECK(temp, ENOMEM, -1)

        *dest = temp;

        for (size_t j=0; j<code_size; j++) {
            (*dest)[dest_index++] = tree->dictionary[(int)byte][j];
        }
    }

    return 0;
}
//
//void gnl_huffman_decode(const char *s, node t, char **dest)
//{
//
//}

#include <gnl_macro_end.h>