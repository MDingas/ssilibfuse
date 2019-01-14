#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "email.h"

int send_email(char* message, char* subject, char* destination) {
        char* template = "echo \"%s\" | mail -s \"%s\" %s";
        char command[strlen(template) + strlen(message) + strlen(subject) + strlen(destination)];
        sprintf(command, template, message, subject, destination);

        return system(command);
}

int send_confirmation_code(char* code, char* destination_email) {
        char* subject = "Codigo de confirmacao para acesso a ficheiro";
        char* template = "Codigo de acesso gerado: %s.\nSe nao realizou qualquer pedido, ignore este email.";
        char message[strlen(template) + strlen(code)];

        sprintf(message,template,code);

        return send_email(message,subject,destination_email);
}
