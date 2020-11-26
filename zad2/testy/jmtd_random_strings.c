#include "../encstrset.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    srand(2137);

    char *strs[100];
    for (int i = 0; i < 100; i++) {
        strs[i] = malloc(53 * sizeof(char));

        for (int j = 0; j < 50; j++) {
            char r = (rand() % 300) - 150;
            strs[i][j] = r;
        }
    }

    unsigned long set1 = encstrset_new();

    for (int i = 0; i < 10000; i++) {
        int v = rand() % 100, k = rand() % 100;
        encstrset_insert(set1, strs[v], strs[k]);
    }

    for (int i = 0; i < 100; i++) {
        free(strs[i]);
    }

    return 0;
}
