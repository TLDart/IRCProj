#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 512	// Buffer Size

char *parse_user_message(int * protocol);
int check_valid(char *message, int* protocol);