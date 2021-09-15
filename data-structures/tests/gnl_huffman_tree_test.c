#include <stdio.h>
#include <string.h>
#include <gnl_colorshell.h>
#include <gnl_file_to_pointer.h>
#include <gnl_assert.h>
#include "../src/gnl_huffman_tree.c"

int can_calculate_frequencies() {
    const char *str = "One Late Night is a short immersive horror-game experience, starring an unnamed graphic designer "
                      "employee, working late one night at the";

    int *freq = calculate_frequencies(str, strlen(str) + 1);

    int expected[255] = {0};
    expected['\0'] = 1;
    expected[' '] = 20;
    expected[','] = 2;
    expected['-'] = 1;
    expected['L'] = 1;
    expected['N'] = 1;
    expected['O'] = 1;
    expected['a'] = 9;
    expected['c'] = 2;
    expected['d'] = 2;
    expected['e'] = 18;
    expected['g'] = 7;
    expected['h'] = 6;
    expected['i'] = 10;
    expected['k'] = 1;
    expected['l'] = 2;
    expected['m'] = 5;
    expected['n'] = 10;
    expected['o'] = 6;
    expected['p'] = 3;
    expected['r'] = 11;
    expected['s'] = 5;
    expected['t'] = 8;
    expected['u'] = 1;
    expected['v'] = 1;
    expected['w'] = 1;
    expected['x'] = 1;
    expected['y'] = 1;

    for (size_t i=0; i<255; i++) {
        if (expected[i] != freq[i]) {
            return -1;
        }
    }

    free(freq);

    return 0;
}

int can_get_tree() {
    const char *str = "One Late Night is a short immersive horror-game experience, starring an unnamed graphic designer "
                      "employee, working late one night at the";

    struct gnl_huffman_tree_t *tree = gnl_huffman_tree_init(str, strlen(str) + 1);

    if (tree == NULL) {
        return -1;
    }

    if (tree_depth(tree->root) != 8) {
        return -1;
    }

    char expected[255][40] = {0};
    strcpy(expected['\0'], "0000110");
    strcpy(expected[' '], "110");
    strcpy(expected[','], "010100");
    strcpy(expected['-'], "0000111");
    strcpy(expected['L'], "0101101");
    strcpy(expected['N'], "0101010");
    strcpy(expected['O'], "0100110");
    strcpy(expected['a'], "0111");
    strcpy(expected['c'], "010000");
    strcpy(expected['d'], "010010");
    strcpy(expected['e'], "100");
    strcpy(expected['g'], "0011");
    strcpy(expected['h'], "0010");
    strcpy(expected['i'], "1011");
    strcpy(expected['k'], "000010");
    strcpy(expected['l'], "010001");
    strcpy(expected['m'], "11100");
    strcpy(expected['n'], "1010");
    strcpy(expected['o'], "0001");
    strcpy(expected['p'], "00000");
    strcpy(expected['r'], "1111");
    strcpy(expected['s'], "11101");
    strcpy(expected['t'], "0110");
    strcpy(expected['u'], "0101011");
    strcpy(expected['v'], "0101110");
    strcpy(expected['w'], "0101111");
    strcpy(expected['x'], "0101100");
    strcpy(expected['y'], "0100111");

    for (size_t i=0; i<256; i++) {
        if (strcmp(expected[i], "") != 0 && strcmp(expected[i], tree->dictionary[i]) != 0) {
            return -1;
        }
    }

    gnl_huffman_tree_destroy(tree);

    return 0;
}

int can_decode_encoded() {
    const char *str = "One Late Night is a short immersive horror-game experience, starring an unnamed graphic designer "
                      "employee, working late one night at the";

    struct gnl_huffman_tree_artifact *artifact = gnl_huffman_tree_encode(str, strlen(str) + 1);

    if (artifact == NULL) {
        return -1;
    }

    if (artifact->code == NULL) {
        return -1;
    }

    if (artifact->size >= strlen(str)) {
        return -1;
    }

    if (artifact->bit_count == 0) {
        return -1;
    }

    void *decoded_string;
    size_t count;
    int res = gnl_huffman_tree_decode(artifact, &decoded_string, &count);

    if (res == -1) {
        return -1;
    }

    if (strcmp(str, decoded_string) != 0) {
        return -1;
    }

    if (count != strlen(str) + 1) {
        return -1;
    }

    free(decoded_string);

    return 0;
}

int can_decode_file() {
    long size;
    char *content = NULL;

    int res = gnl_file_to_pointer("./testfile.txt", &content, &size);
    if (res == -1) {
        return -1;
    }

    struct gnl_huffman_tree_artifact *artifact = gnl_huffman_tree_encode(content, size);

    if (artifact == NULL) {
        return -1;
    }

    if (artifact->code == NULL) {
        return -1;
    }

    if (artifact->size > size) {
        return -1;
    }

    void *decoded;
    size_t count;
    res = gnl_huffman_tree_decode(artifact, &decoded, &count);

    if (res == -1) {
        return -1;
    }

    if (memcmp(decoded, content, count) != 0) {
        return -1;
    }

    if (count != size) {
        return -1;
    }

    free(decoded);
    free(content);

    return 0;
}

int can_decode_img() {
    long size;
    char *content = NULL;

    int res = gnl_file_to_pointer("./states-of-a-programmer.png", &content, &size);
    if (res == -1) {
        return -1;
    }

    struct gnl_huffman_tree_artifact *artifact = gnl_huffman_tree_encode(content, size);

    if (artifact == NULL) {
        return -1;
    }

    if (artifact->code == NULL) {
        return -1;
    }

    if (artifact->size > size) {
        return -1;
    }

    void *decoded;
    size_t count;
    res = gnl_huffman_tree_decode(artifact, &decoded, &count);

    if (res == -1) {
        return -1;
    }

    if (memcmp(decoded, content, count) != 0) {
        return -1;
    }

    if (count != size) {
        return -1;
    }

    free(decoded);
    free(content);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_huffman_tree test:\n\n");

    gnl_assert(can_calculate_frequencies, "can calculate the frequencies of a string.");
    gnl_assert(can_get_tree, "can build an huffman tree.");
    gnl_assert(can_decode_encoded, "can decode an encoded string.");
    gnl_assert(can_decode_file, "can decode an encoded file.");

    // the following test is heavy for valgrind
    //gnl_assert(can_decode_img, "can decode an encoded an image.");

    // the gnl_huffman_tree_destroy method is implicitly tested

    printf("\n");
}