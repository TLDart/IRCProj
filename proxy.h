
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
#define IP_SERVER "12.0.0.4"
#define QUIT "QUIT"
#define LIST "LIST"
#define DL "DOWNLOAD"
#define DLINV "DLINV"
#define STREAM_END "§"

struct client_info{
    char protocolo[4];//último indice para o '\0', os outros para "TCP" ou "UDP"
    int port_destino;
    int port_origem;
    char ip_destino[INET_ADDRSTRLEN];
    char ip_origem[INET_ADDRSTRLEN];
    struct client_info *next;
};
struct passa_args{//serve para passar o nodo relativo ao cliente e o socket descriptor do cliente a thread que esta encarregue desse cliente
    int fd;
    struct client_info *pointer;
};


struct socket_info{
    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_info;

};


pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;
struct client_info *header;


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
struct client_info *create_list();
struct client_info* add_client(struct client_info *header, struct sockaddr_in *info, int port, char *protocolo);
void remove_client(struct client_info *header, struct client_info *node);
void show_stats(struct client_info *header);
