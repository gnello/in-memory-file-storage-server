
#ifndef GNL_HUFFMAN_TREE_H
#define GNL_HUFFMAN_TREE_H

/**
 * Holds the huffman tree information.
 */
struct gnl_huffman_tree_t {

    // the bytes dictionary
    char **dictionary;

    // the root node of the tree
    struct gnl_huffman_tree_node_t *root;
};

/**
 * The huffman tree cipher to use for decoding.
 */
struct gnl_huffman_tree_cipher;

/**
 * Build a huffman tree based on the given bytes.
 *
 * @param bytes The array of bytes.
 * @param count The number of bytes.
 *
 * @return      Returns an huffman tree instance on success,
 *              NULL otherwise.
 */
struct gnl_huffman_tree_t *gnl_huffman_tree_init(const void *bytes, size_t count);

/**
 * Destroy the given huffman tree.
 *
 * @param tree  The huffman tree to destroy.
 */
void gnl_huffman_tree_destroy(struct gnl_huffman_tree_t *tree);

/**
 * Encode the given bytes into dest using the Huffman data
 * compression algorithm.
 *
 * @param bytes The bytes to encode.
 * @param count The number of bytes to encode.
 * @param dest  The destination where to put the encoded bytes.
 *
 * @return      Return a gnl_huffman_tree_cipher struct to use for
 *              decoding on success, NULL otherwise.
 */
struct gnl_huffman_tree_cipher *gnl_huffman_encode(const void *bytes, size_t count, char **dest);

/**
 * Decode the given code into dest using the given cipher.
 *
 * @param code      The code to decode.
 * @param cipher    The cipher to use for decoding. It is returned from the
 *                  encoding call.
 * @param dest      The destination where to put the decoded bytes.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
int gnl_huffman_decode(const char *code, struct gnl_huffman_tree_cipher *cipher, char **dest);

#endif //GNL_HUFFMAN_TREE_H