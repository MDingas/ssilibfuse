#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int send_email(char* message, char* subject, char* destination) {

        char* template = "echo \"%s\" | mail -s \"%s\" %s";

        char command[strlen(template) + strlen(message) + strlen(subject) + strlen(destination)];

        sprintf(command, template, message, subject, destination);

        return system(command);
}

void generate_rand_alphanumeric_string(int size, char* str) {
        char* alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

        // Generate seed
        srand(time(NULL));

        for (int i = 0; i < size; i++) {
                str[i] = alphabet[(rand() % ((strlen(alphabet)) - 1)) + 0];
        }
}


int main() {
        printf("Main start\n");
        char code[8];
        generate_rand_alphanumeric_string(7,code);
        printf("generated code %s\n", code);
        printf("Main end\n");
}
