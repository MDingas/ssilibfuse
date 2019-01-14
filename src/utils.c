#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "utils.h"

void generate_rand_alphanumeric_string(int size, char* str) {
        char* alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

        // Generate seed
        srand(time(NULL));
        for (int i = 0; i < size; i++) {
                str[i] = alphabet[(rand() % ((strlen(alphabet)) - 1)) + 0];
        }
        str[size] = '\0';
}
