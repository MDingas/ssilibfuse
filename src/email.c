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

int send_confirmation_code(char* code, char* destination_email) {
        char* subject = "Codigo de confirmacao para acesso a ficheiro";
        char* template = "Codigo de acesso gerado: %s.\nSe nao realizou qualquer pedido, ignore este email.";
        char message[strlen(template) + strlen(code)];

        sprintf(message,template,code);

        return send_email(message,subject,destination_email);
}


int main() {
        printf("Main start\n");
        char code[15];
        generate_rand_alphanumeric_string(14,code);
        send_confirmation_code(code, "pauloedgar2@gmail.com");
        printf("Main end\n");
}
