
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


#define BUFFER_SIZE  512
#define IP_PROXY "127.0.0.3"
#define QUIT "QUIT"
#define LIST "LIST"
#define DL "DOWNLOAD"
#define DLINV "DLINV"
#define STREAM_END "§"


struct socket_info{
    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_info;

};


pthread_t udp_thread, tcp_thread, threads;
int welcoming_socket, client_socket, running = 1;
struct sockaddr_in welcoming_socket_info, client_socket_info;
struct sockaddr_in welcoming_socket_udp;
int client_socket_info_size,client_socket_info_size_udp, server_port, losses, udp_fd;
struct sockaddr_in server_tcp;




void *udp_thread_handler();
//void client(int socket_descriptor);
void *client(void *socket_descriptor);
void *tcp_thread_handler();
void receive_listing(int client_fd, int server_fd);
void receive_file(int client_fd, int server_fd);
void read_user_input();
int check_valid(char* message);
void* udp_client();
