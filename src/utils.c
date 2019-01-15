#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "utils.h"

void generate_rand_alphanumeric_string(int size, char* buffer) {
        char* alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

        // Generate seed
        srand(time(NULL));
        for (int i = 0; i < size; i++) {
                buffer[i] = alphabet[(rand() % ((strlen(alphabet)) - 1)) + 0];
        }
        buffer[size] = '\0';
}

char* get_user_email(char* credentials,char* buffer){
        char* username = getenv("USER");

        snprintf(buffer, BUFFER_SIZE,"gawk -f get_email.awk username=%s %s", username, credentials);

        FILE* fd = popen(buffer, "r");
        if(fd == NULL) return NULL;

        int n = fscanf(fd, "%s",buffer);
        pclose(fd);

        //Email not found
        if(n <= 0) return NULL;
}

