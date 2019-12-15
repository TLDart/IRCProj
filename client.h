#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <dirent.h>
#include "parser.h"

#define QUIT "QUIT"
#define LIST "LIST"
#define DL "DOWNLOAD"
#define DLINV "DLINV"
#define STREAM_END "§"
#define PROTOCOL_TCP 1
#define PROTOCOL_UDP 0

int protocol;

#define path "./ClientDL/"

void receive_listing(int fd);
void receive_file(int fd, char* msg, int protocol);
void print_info(struct timespec begin, char* name, long bytes,int protocol);