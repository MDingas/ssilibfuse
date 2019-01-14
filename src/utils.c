#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
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

char* get_user_email(char* credentials){
    char buffer[BUFFER_SIZE];
    char* uid = getenv("USER");
    
    snprintf(buffer, BUFFER_SIZE,"gawk -f get_email.awk uid=%s %s", uid, credentials);

    FILE* fd = popen(buffer, "r");
    if(fd == NULL) return NULL;

    fscanf(fd, "%s",buffer);
    pclose(fd);
    
    return strdup(buffer);
}

