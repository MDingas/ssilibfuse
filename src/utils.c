#include <string.h>
#include <time.h>
#include <errno.h>
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

int get_user_email(uid_t userid, char* credentials,char* buffer){

    char command[300];

    char *token;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    FILE* fp = fopen(credentials, "r");

    if (fp == NULL)
        return -1;


    while ((read = getline(&line, &len, fp)) != -1) {
        if (atoi(strtok(line, ":")) == userid) {
            strcpy(buffer, strtok(NULL, ":"));
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);
    return -2;
}

int log_event(char* message, char* filepath) {

    FILE* fp = fopen(filepath, "a+");

    if (fp == NULL) {
        perror("fopen");
        return -1;
    } else {

        fprintf(fp, "%s\n", message);

        fclose(fp);
        return 0;
    }
}
