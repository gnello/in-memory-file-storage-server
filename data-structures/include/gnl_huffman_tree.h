
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
 * The huffman tree artifact to use for decoding.
 */
struct gnl_huffman_tree_artifact;

/**
 * Build a huffman tree based on the given bytes.
 *
 * @param bytes The array of bytes.
 * @param count The number of bytes.
 *
 * @return      Returns an huffman tree instance on success,
 *              NULL otherwise.
 */
extern struct gnl_huffman_tree_t *gnl_huffman_tree_init(const void *bytes, size_t count);

/**
 * Destroy the given huffman tree.
 *
 * @param tree  The huffman tree to destroy.
 */
extern void gnl_huffman_tree_destroy(struct gnl_huffman_tree_t *tree);

/**
 * Destroy the given artifact.
 *
 * @param artifact  The artifact to destroy.
 */
extern void gnl_huffman_tree_destroy_artifact(struct gnl_huffman_tree_artifact *artifact);

/**
 * Encode the given bytes using the Huffman data compression algorithm.
 *
 * @param bytes The bytes to encode.
 * @param count The number of bytes to encode.
 *
 * @return      Return a gnl_huffman_tree_artifact struct to use for
 *              decoding on success, NULL otherwise.
 */
extern struct gnl_huffman_tree_artifact *gnl_huffman_tree_encode(const void *bytes, size_t count);

/**
 * Decode the given code into dest using the given artifact.
 *
 * @param artifact  The artifact to use for decoding. It is returned from the
 *                  encoding call.
 * @param bytes     The destination where to put the decoded bytes.
 * @param count     The destination where to put the number of bytes decoded.
 *
 * @return          Returns 0 on success, -1 otherwise.
 */
extern int gnl_huffman_tree_decode(struct gnl_huffman_tree_artifact *artifact, void **bytes, size_t *count);

/**
 * Get the size of the given artifact.
 *
 * @param artifact  The artifact of which get the size.
 *
 * @return          The size of the given artifact on success,
 *                  -1 otherwise.
 */
extern int gnl_huffman_tree_size(struct gnl_huffman_tree_artifact *artifact);

#endif //GNL_HUFFMAN_TREE_H