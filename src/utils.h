#define BUFFER_SIZE 100

void generate_rand_alphanumeric_string(int size, char* str);
int get_user_email(uid_t userid, char* credentials,char* buffer);
int log_event(char* message, char* filepath);
