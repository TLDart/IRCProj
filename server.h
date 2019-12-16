
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
#define IP_SERVER "127.0.0.4"
#define QUIT "QUIT"
#define LIST "LIST"
#define DL "DOWNLOAD"
#define DLINV "DLINV"
#define STREAM_END "§"
#define SUCCESSFUL "SUCCESSFUL"

char *dir_path = "./files";

//variables
int clients = 0;
int welcoming_socket, client_socket;
struct sockaddr_in welcoming_socket_info, client_socket_info;
int client_socket_info_size;
int number_current_clients = 0;
int max_clients;
int server_port;
pthread_t thread_udp;
int running = 1, udp_fd;
pthread_t threads, udp_thread;


struct socket_info{
    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_info;

};

//functions
void server();
void client(int client_descriptor);
FILE *get_filepointer(char *file_name);
void list_files();
void upload_file(FILE* fp, char *path);
void* udp_client(void * arg);
void* udp_thread_handler();
void upload_file_udp(FILE* fp, char *path, struct sockaddr_in client_info);