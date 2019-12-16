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
    int protocol_selected = 1;// Starts in TCP
    /*Initial Behaviors*/
    //Parsing commands
    if(argc != 5){
        printf("Wrong Command Syntax\n"
               " Use : \"./client <ProxyIP> <HostAdress> <Port> <Protocol>\"\n");
        exit(-1);
    }
    printf(" %s %s %s \n",argv[0],argv[1], argv[2]);
    //if ((proxy_ptr = gethostbyname(argv[1])) == 0){
    if ((proxy_ptr = gethostbyname(argv[1])) == 0){
        printf("Could not get Server Address, Exiting\n");
        exit(-1);
    }
    //if ((server_ptr = gethostbyname(argv[1])) == 0){
    /*if ((server_ptr = gethostbyname("127.0.0.3")) == 0){
        printf("Couldn't get Proxy Server Hostname, Exiting\n");
        exit(-1);
    }
    else{
        printf("CONNECTED");
    }*/

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
        /*Establish a connection to the proxy_tcp via TCP*/
        bzero((void *) &proxy_tcp, sizeof(struct sockaddr_in));
        proxy_tcp.sin_family = AF_INET; //Defines IPV4
        proxy_tcp.sin_addr.s_addr = ((struct in_addr *) (proxy_ptr->h_addr))->s_addr; //
        proxy_tcp.sin_port = htons((short) atoi(argv[3])); //Port

    /*Establish a connection to the proxy_tcp via UDP*/
        bzero((void *) &client_udp_socket, sizeof(struct sockaddr_in));
        client_udp_socket.sin_family = AF_INET;
        client_udp_socket.sin_port = htons((short)atoi(argv[3]));
        client_udp_socket.sin_addr.s_addr = inet_addr("127.0.0.22");

    if((socket_udp_descriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        printf("Error creating the socket UDP\n");
}
        if(bind(socket_udp_descriptor, (struct sockaddr *) &client_udp_socket, sizeof(client_udp_socket)) == -1){
            perror("Falha ao dar bind do socket UDP.\n");
            exit(-1);
        }

        if ((socket_tcp_descriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1) { // Create the socket and bind it to a file descriptor
            printf("Error Creating Socket\n");
            exit(-1);
        }
        /*After setting the type of connection*/
        if (connect(socket_tcp_descriptor, (struct sockaddr *) &proxy_tcp, sizeof(proxy_tcp)) < 0) {// Connect to the proxy_tcp
            if ((server_ptr = gethostbyname(argv[2])) == 0){
                printf("Couldn't get Proxy Server Hostname, Exiting\n");
                exit(-1);
            }
            else {
                printf("CONNECTED");
            }
            printf("Connection to Proxy Failed. Trying Direct Connection\n");
            close(socket_tcp_descriptor);
            bzero((void *) &proxy_tcp, sizeof(struct sockaddr_in));
            proxy_tcp.sin_family = AF_INET; //Defines IPV4
            proxy_tcp.sin_addr.s_addr = ((struct in_addr *) (server_ptr->h_addr))->s_addr; //
            proxy_tcp.sin_port = htons((short) atoi(argv[3])); //Port

            if ((socket_tcp_descriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1) { // Create the socket and bind it to a file descriptor
                printf("Error Creating Socket\n");
                exit(-1);
            }
            /*After setting the type of connection*/
            if (connect(socket_tcp_descriptor, (struct sockaddr *) &proxy_tcp, sizeof(proxy_tcp)) < 0) {// Connect to the proxy_tcp
                printf("Can't connect to the server\n");
                exit(-1);
            }
            printf("Connecting Directly to the server\n");
        }
        else{
            /*First message / Specify Protocol*/
            /*The first message sends a string containg the ip of the server to connect to and the protocol used by the server in a later connection*/
            sprintf(buffer, "%s",argv[2]); //Firstly the we send the IP of the server, then we send the port, then we send, protocol;
            message = malloc(strlen(buffer));
            strcpy(message,buffer);
            write(socket_tcp_descriptor, message, 1 + strlen(message));
            free(message);
            /*After the initial connection start communicating with the server*/
            //Getting response from the proxy
            nread = read(socket_tcp_descriptor, buffer, BUFFER_SIZE - 1);
            write(socket_tcp_descriptor, ALIVE, 1 + strlen(message));
            printf("SENDS ALIVE\n");
        }

        //Verify if the server is alive

        nread = read(socket_tcp_descriptor, buffer, BUFFER_SIZE - 1);
        buffer[nread] = '\0';

        if(strcmp(buffer, QUIT) == 0){
            printf("Server is is Full, try again later\n");
            exit(-1);
        }
        while (running) {
            message = parse_user_message(&protocol_selected);

            if (protocol_selected == PROTOCOL_TCP) {
                write(socket_tcp_descriptor, message, 1 + strlen(message));
                nread = read(socket_tcp_descriptor, buffer, BUFFER_SIZE - 1);
                buffer[nread] = '\0';
                printf("Message received : %s %d\n", buffer, strcmp(buffer, LIST) == 0);
                if (strcmp(buffer, LIST) == 0) {
                    receive_listing(socket_tcp_descriptor);
                }

                if (strcmp(buffer, DL) == 0) {
                    receive_file_tcp(socket_tcp_descriptor, message, PROTOCOL_TCP);
                }
                if (strcmp(buffer, DLINV) == 0) {
                    printf("You have selected an invalid file to download\n");
                }

                if (strcmp(buffer, QUIT) == 0) {
                    running = 0;
                }
                free(message);
            }
            else{//In case of an udp connection
                int i;
                char msg_to_send[BUFFER_SIZE] = "127.0.0.4";

                //inet_ntop(AF_INET, &(proxy_tcp.sin_addr),msg_to_send,INET_ADDRSTRLEN);
                int size = (int) strlen(msg_to_send);
                msg_to_send[size] = ',';
                for (i = 0; i < strlen(message); i++) {
                    msg_to_send[size + i + 1] = message[i];
                }
                msg_to_send[size + 1+ strlen(message)] = '\0';
                printf("server %s %ld\n", msg_to_send,strlen(msg_to_send));
                sendto(socket_udp_descriptor,msg_to_send, BUFFER_SIZE - 1,0,(struct sockaddr *) &proxy_tcp, sizeof(proxy_tcp));
                if((nread = recvfrom(socket_udp_descriptor, buffer, BUFFER_SIZE - 1, 0, NULL, NULL)) <0){
                    perror("ERRO");
                }
                buffer[nread] = '\0';
                if (strcmp(buffer, DL) == 0) {
                    printf("RECEIVING FILE\n");
                    receive_file_udp(socket_udp_descriptor, message, PROTOCOL_UDP);
                }
                if (strcmp(buffer, DLINV) == 0) {
                    printf("You have selected an invalid file to download\n");
                }

                printf(" MESSGE RECEIVED :%s\n", buffer);
                printf(" MESSGE RECEIVED :%d\n", nread);

                    //receive_file_udp(socket_udp_descriptor, message, PROTOCOL_UDP);
                protocol_selected = 1;
            }
        }
        close(socket_tcp_descriptor);
        printf("CLIENT GOING AWAY\n");

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


void receive_file_tcp(int fd, char* msg,int protocol) {
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
    printf("--asdasdasdasdasdasd> %s|asdsd\n", buffer);
    print_info(begin, token, total_read, protocol);


    fclose(fp);

}

void receive_file_udp(int fd, char* msg,int protocol) {
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
    if ((nread = recvfrom(fd, buffer, BUFFER_SIZE - 1,0,NULL,NULL)) <= 0) {
        printf("Erro ao ler o tamanho do ficheiro\n");
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

        nread = recvfrom(fd, buffer, size_to_read,0,NULL,NULL);
        buffer[nread] = '\0';
        printf("---->lido : %d\n", nread);
        printf("---> %s\n", buffer);
        fwrite(buffer, sizeof(char), size_to_read, fp); // Write to file

        total_read += nread;
    }
    //READS EOF
    recvfrom(fd, buffer, size_to_read,0,NULL,NULL);
    printf("--asdasdasdasdasdasd> %s|asdsd\n", buffer);
    print_info(begin, token, total_read, protocol);
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