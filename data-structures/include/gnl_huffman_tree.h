
#ifndef GNL_HUFFMAN_TREE_H
#define GNL_HUFFMAN_TREE_H

struct gnl_huffman_tree_t;

struct gnl_huffman_tree_cipher;

/**
 * {@inheritDoc}
 */
struct gnl_huffman_tree_t *gnl_huffman_tree_init(const void *bytes, size_t count);

void gnl_huffman_tree_destroy(struct gnl_huffman_tree_t *tree);

struct gnl_huffman_tree_cipher *gnl_huffman_encode(const void *bytes, size_t count, char **dest);

int gnl_huffman_decode(const char *code, struct gnl_huffman_tree_cipher *cipher, char **dest);

#endif //GNL_HUFFMAN_TREE_H