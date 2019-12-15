/* Client
 * Usage:
 * Client <Proxy Address> <Server Address> <Port>

 */
#include <time.h>
#include "client.h"



int main(int argc, char* argv[]) {
    //Declaring variables
    int socket_tcp_descriptor,socket_udp_descriptor, port, running = 1;
    struct hostent *server_ptr, *proxy_ptr;
    struct sockaddr_in proxy_tcp,proxy_udp, host;
    char buffer[BUFFER_SIZE];
    char* message;
    int nread;
    /*Initial Behaviors*/
    //Parsing commands
    if(argc != 5){
        printf("Wrong Command Syntax\n"
               " Use : \"./client <ProxyIP> <HostAdress> <Port> <Protocol>\"\n");
        exit(-1);
    }
    printf(" %s %s %s \n",argv[0],argv[1], argv[2]);
    if ((proxy_ptr = gethostbyname(argv[2])) == 0){
        printf("Could not get Server Address, Exiting\n");
        exit(-1);
    }
    if ((server_ptr = gethostbyname(argv[1])) == 0){
        printf("Couldn't get Proxy Server Hostname, Exiting\n");
        exit(-1);
    }

    if((atoi(argv[3]) < 0 || atoi(argv[3]) > 65356)){
        printf("Invalid Port, port must be an integer between 1 and 65536\n");
        exit(-1);
    }
    if(strcmp(argv[4], "tcp") == 0){
        protocol = PROTOCOL_TCP;
    }
    else if(strcmp(argv[4], "udp") == 0){
        protocol = PROTOCOL_UDP;
    }
    else{
        printf("Invalid Protocol, Needs to be either 'tcp' or 'udp'");
        exit(-1);
    }

    if(protocol == PROTOCOL_TCP) {
        /*Establish a connection to the proxy_tcp via TCP*/
        bzero((void *) &proxy_tcp, sizeof(struct sockaddr_in));
        proxy_tcp.sin_family = AF_INET; //Defines IPV4
        proxy_tcp.sin_addr.s_addr = ((struct in_addr *) (proxy_ptr->h_addr))->s_addr; //
        proxy_tcp.sin_port = htons((short) atoi(argv[3])); //Port

        if ((socket_tcp_descriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1) { // Create the socket and bind it to a file descriptor
            printf("Error Creating Socket\n");
            exit(-1);
        }
        /*After setting the type of connection*/
        if (connect(socket_tcp_descriptor, (struct sockaddr *) &proxy_tcp, sizeof(proxy_tcp)) < 0) {// Connect to the proxy_tcp
            printf("Error connecting to the Proxy\n");
            exit(-1);
        }

        /*First message / Specify Protocol*/
        /*The first message sends a string containg the ip of the server to connect to and the protocol used by the server in a later connection*/

        sprintf(buffer, "%s",argv[2]); //Firstly the we send the IP of the server, then we send the port, then we send, protocol;
        message = malloc(strlen(buffer));
        strcpy(message,buffer);
        write(socket_tcp_descriptor, message, 1 + strlen(message));
        free(message);
        /*After the initial connection start communicating with the server*/

        while (running) {
            message = parse_user_message();
            write(socket_tcp_descriptor, message, 1 + strlen(message));
            nread = read(socket_tcp_descriptor, buffer, BUFFER_SIZE - 1);
            buffer[nread] = '\0';
            printf("Message received : %s %d\n", buffer, strcmp(buffer, LIST) == 0);
            if (strcmp(buffer, LIST) == 0) {
                receive_listing(socket_tcp_descriptor);
            }

            if (strcmp(buffer, DL) == 0) {
                receive_file(socket_tcp_descriptor, message, PROTOCOL_TCP);
            }
            if (strcmp(buffer, DLINV) == 0) {
                printf("You have selected an invalid file to download\n");
            }

            if (strcmp(buffer, QUIT) == 0) {
                running = 0;
            }
            free(message);
        }
        close(socket_tcp_descriptor);
        printf("CLIENT GOING AWAY\n");
    }
    else{
        bzero((void *) &proxy_udp, sizeof(struct sockaddr_in));
        proxy_udp.sin_family = AF_INET; //Defines IPV4
        proxy_udp.sin_addr.s_addr = ((struct in_addr *) (proxy_ptr->h_addr))->s_addr; //
        proxy_udp.sin_port = htons((short) atoi(argv[3])); //Port

        socket_udp_descriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        if(bind(socket_udp_descriptor, (struct sockaddr *) &proxy_udp, sizeof(proxy_udp)) == -1){
            printf("Falha ao dar bind do socket UDP.\n");
            exit(-1);
        }


    }
    }


void receive_listing(int fd){
    char buffer[BUFFER_SIZE];
    int nread;
    printf("Listing items in the Donwload Directory\n");
    do{
        nread = read(fd, buffer, BUFFER_SIZE - 1);
        buffer[nread] ='\0';
        //printf("\t\t%s\n", buffer);
        if(strcmp(buffer, STREAM_END) != 0){
            printf("\t\t-> %s\n", buffer);
        }
    }while(strcmp(buffer, STREAM_END) != 0);
}


void receive_file(int fd, char* msg,int protocol) {
    int nread;
    char buffer[BUFFER_SIZE];
    struct timespec begin;
    char delimiter[2] = " ";
    char *token = strtok(msg, delimiter);
    char filename[100];
    FILE *fp;
    int i;
    long file_size = 0;
    long total_read = 0;
    long size_to_read;

    clock_gettime(CLOCK_REALTIME, &begin);
    for (i = 0; i < 3; i++) {
        token = strtok(NULL, delimiter);
    }

    memset(buffer, '\0', BUFFER_SIZE);
    if ((nread = read(fd, buffer, BUFFER_SIZE - 1)) <= 0) {
        printf("Erro ao ler o tamanho do ficheiro");
    } else {
        file_size = atol(buffer);
        printf("tamanho do ficheiro : %ld\n", file_size);
    }


    strcpy(filename, path);
    filename[strlen(filename)] = '/';
    for (i = 0; i < strlen(token); i++) {
        filename[strlen(path) + i + 1] = token[i];
    }
    filename[strlen(path) + i + 1] = '\0';

    fp = fopen(filename, "wb");

    while (total_read < file_size) {
        //memset(buffer, '\0', BUFFER_SIZE - 1);
        if ((file_size - total_read) / (BUFFER_SIZE - 1) == 0) {
            size_to_read = (file_size - total_read) % (BUFFER_SIZE - 1);
        } else {
            size_to_read = BUFFER_SIZE - 1;
        }

        nread = read(fd, buffer, size_to_read);
        buffer[nread] = '\0';
        printf("---->lido : %d\n", nread);
        printf("---> %s\n", buffer);
        fwrite(buffer, sizeof(char), size_to_read, fp); // Write to file

        total_read += nread;
    }
    //READS EOF
    read(fd, buffer, BUFFER_SIZE - 1);
    //printf("--> %s", buffer);
    print_info(begin, filename, total_read, protocol);


    fclose(fp);

}

void print_info(struct timespec begin, char* name, long bytes,int protocol) {
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    int time_to_transfer = now.tv_sec - begin.tv_sec;

    char *protocol_char = (protocol == PROTOCOL_TCP) ? "TCP" : "UDP";

    printf("Name of the file transfered: %s\n"
           "Total bytes received : %ld \n"
           "Transfer Protocol: %s\n"
           "Total Download Time: %d seconds\n", name, bytes, protocol_char, time_to_transfer);

}