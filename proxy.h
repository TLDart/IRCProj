
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

pthread_t udp_thread, tcp_thread, threads;
int welcoming_socket, client_socket, running = 1;
struct sockaddr_in welcoming_socket_info, client_socket_info;
int client_socket_info_size, server_port;


#define BUFFER_SIZE  512
#define IP_PROXY "127.0.0.3"
#define QUIT "QUIT"
#define LIST "LIST"
#define DL "DOWNLOAD"
#define DLINV "DLINV"
#define STREAM_END "§"


void *udp_thread_handler();
//void client(int socket_descriptor);
void *client(void *socket_descriptor);
void *tcp_thread_handler();
void receive_listing(int client_fd, int server_fd);
void receive_file(int client_fd, int server_fd);
