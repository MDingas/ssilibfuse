#ifndef VALIDATION_H
#define VALIDATION_H

#define HASH_CODE_SIZE 10
#define TIMEOUT 5

int validate(char* hash_code, char* email);
void new_validation_window(char* email);
void new_alert_window(char* message);

enum {
    VALID_CODE,
    INVALID_CODE,
    TIMEOUT_EXCEEDED,
};

#endif
