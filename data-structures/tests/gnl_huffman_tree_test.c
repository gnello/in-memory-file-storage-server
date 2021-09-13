#include <stdio.h>
#include <string.h>
#include <gnl_colorshell.h>
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

int can_decode_encoded() {
    const char *str = "One Late Night is a short immersive horror-game experience, starring an unnamed graphic designer "
                      "employee, working late one night at the";

    char *dest;
    struct gnl_huffman_tree_cipher *cipher = gnl_huffman_encode(str, strlen(str) + 1, &dest);

    if (cipher == NULL) {
        return -1;
    }

    if (dest == NULL) {
        return -1;
    }

    char *decoded_string;
    int res = gnl_huffman_decode(dest, cipher, &decoded_string);

    if (res == -1) {
        return -1;
    }

    if (strcmp(str, decoded_string) != 0) {
        return -1;
    }

    free(dest);
    free(decoded_string);

    return 0;
}

int main() {
    gnl_printf_yellow("> gnl_huffman_tree test:\n\n");

    gnl_assert(can_calculate_frequencies, "can calculate the frequencies of a string.");
    gnl_assert(can_decode_encoded, "can decode an encoded string.");

    // the gnl_huffman_tree_destroy method is implicitly tested in every assertion

    printf("\n");
}