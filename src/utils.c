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

int get_user_email(char* credentials,char* buffer){
    char* username = getenv("USER");
    char command[BUFFER_SIZE];

    sprintf(command, "awk -f get_email.awk username=%s %s", username, credentials);

    FILE* fp = popen(command, "r");

    if(fp == NULL ) {
        perror("fopen");
        return -1;
    }

    char* b = fgets(buffer, BUFFER_SIZE ,fp);
    fclose(fp);

    //Email not found
    if(b == NULL)
        return -2;

    return 0;
}

int log_event(char* message, char* filepath) {

    FILE* fp = fopen(filepath, "a+");

    if (fp == NULL) {
        perror("fopen");
        return -1;
    } else {

        fprintf(fp, "%s", message);

        fclose(fp);
        return 0;
    }
}
