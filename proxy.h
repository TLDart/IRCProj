
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>

pthread_t udp_thread, threads;
int welcoming_socket, client_socket;
struct sockaddr_in welcoming_socket_info, client_socket_info;
int client_socket_info_size, server_port;


#define BUFFER_SIZE  512
#define IP_PROXY "127.0.0.2"
#define QUIT "QUIT"
#define LIST "LIST"
#define DL "DOWNLOAD"
#define DLINV "DLINV"
#define STREAM_END "§"


void *udp_thread_handler();
//void client(int socket_descriptor);
void *client(void *socket_descriptor);

