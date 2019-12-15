#include "proxy.h"
int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf("Wrong Command Syntax\n"
               " Use : \"./proxy <Port> \n");
        exit(-1);
    }
    if ((atoi(argv[1]) < 0 || atoi(argv[1]) > 65356)) {
        printf("Invalid Port, port must be an integer between 1 and 65536\n");
        exit(-1);
    }
    server_port = atoi(argv[1]);

    if(pthread_create(&udp_thread, NULL, udp_thread_handler, NULL) < 0){
        printf("Error Creating a thread UDP");
    }

    if(pthread_create(&tcp_thread, NULL, tcp_thread_handler, NULL) < 0){
        printf("Error Creating a thread UDP");
    }
    while(running){
        //read_user_input();

    }


}






void *udp_thread_handler(){
    printf("Created Sucessfully\n");
}


void *tcp_thread_handler(){
    printf("Created Sucessfully\n");
    //Handle TCP

    bzero((void *) &welcoming_socket_info, sizeof(welcoming_socket_info));

    //loads the info of the server to the struct for the socket
    welcoming_socket_info.sin_family = AF_INET;
    welcoming_socket_info.sin_addr.s_addr = inet_addr(IP_PROXY);
    welcoming_socket_info.sin_port = htons(server_port);

    client_socket_info_size = sizeof(struct sockaddr_in);

    if((welcoming_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("Erro ao criar o welcoming socket.\n");
        exit(-1);
    }
    if(bind(welcoming_socket,(struct sockaddr *) &welcoming_socket_info, sizeof(struct sockaddr_in)) == -1){
        printf("Erro ao dar bind do welcoming socket.\n");
        exit(-1);
    }

    if(listen(welcoming_socket, 5) == - 1){
        printf("Error in the listening function");
        exit(-1);
    }

    while(1){//TODO colocar uma condicao de paragem para que possamos fechar o servidor

        while(waitpid(-1,NULL,WNOHANG)>0);

        client_socket = accept(welcoming_socket, (struct sockaddr *) &client_socket_info, (socklen_t *) &client_socket_info_size);
        //create the handler thread for the received client
        pthread_create(&threads,NULL,client,&client_socket);

        //TODO verificar se esta tudo o que precisa ser feito feito
    }
}

void *client(void *arg) {
//void client(int socket_descriptor) {
    int client_socket_fd = *((int *) arg);
    struct sockaddr_in server_tcp;
    int socket_tcp_descriptor_server, nread;
    struct hostent *server_ptr;
    char buffer[BUFFER_SIZE];
    int client_running = 1;
    printf("CONNECTED\n");
    printf("Ready to get server info\n");

    //Receive Initial Message and connect to server
    if ((nread = read(client_socket_fd, buffer, BUFFER_SIZE - 1)) < 0) {
        printf("Erro ao ler o comando do cliente.\n");
    } else {
        printf("%s\n", buffer);
    }
    buffer[nread] = '\0';
    printf("SERVER IP : %s\n", buffer);
    write(client_socket_fd, "SUCCESSFUL", 1 + strlen(buffer));

    if ((server_ptr = gethostbyname(buffer)) == 0){
        printf("Could not get Server Address, Exiting\n");
        exit(-1);
    }
    bzero((void *) &server_tcp, sizeof(struct sockaddr_in));
    server_tcp.sin_family = AF_INET; //Defines IPV4
    server_tcp.sin_addr.s_addr = ((struct in_addr *) (server_ptr->h_addr))->s_addr; //
    server_tcp.sin_port = htons((short)server_port); //Port

    if ((socket_tcp_descriptor_server = socket(AF_INET, SOCK_STREAM, 0)) == -1) { // Create the socket and bind it to a file descriptor
        printf("Error Creating Socket\n");
        exit(-1);
    }
    /*After setting the type of connection*/
    if (connect(socket_tcp_descriptor_server, (struct sockaddr *) &server_tcp, sizeof(server_tcp)) < 0) {// Connect to the proxy_tcp
        printf("Error connecting to the Proxy\n");
        exit(-1);
    }

    //Cheking if the server is alive
    printf("READING\n");
    nread = read(client_socket_fd, buffer, BUFFER_SIZE - 1);
    printf("%d", nread);
    buffer[nread] = '\0';
    printf("%s\n", buffer);
    //write(socket_tcp_descriptor_server, buffer, 1 + strlen(buffer));

    printf("READS BACK");
    nread = read(socket_tcp_descriptor_server, buffer, BUFFER_SIZE - 1);
    buffer[nread] = '\0';
    printf("WRITE BACK");
    write(client_socket_fd, buffer, 1 + strlen(buffer));


    if(strcmp(buffer,QUIT) == 0){//If the server is full
        printf("Proxy exits because server is full\n");
        pthread_detach(pthread_self());
        pthread_exit(NULL);
    }

    while(client_running){
            if ((nread = read(client_socket_fd, buffer, BUFFER_SIZE - 1)) < 0) {
                printf("Erro ao ler o comando do cliente.\n");
            } else {
                printf("%s\n", buffer);
            }
        buffer[nread] = '\0';
        if (nread > 0) {
            if (buffer[nread - 1] == '\n') buffer[nread - 1] = '\0';
            write(socket_tcp_descriptor_server, buffer, 1 + strlen(buffer));
            nread = read(socket_tcp_descriptor_server, buffer, BUFFER_SIZE - 1);
            buffer[nread] = '\0';
            if (strcmp(buffer, LIST) == 0) {
                //printf("Listing.\n");
                write(client_socket_fd,LIST, BUFFER_SIZE - 1);
                receive_listing(client_socket_fd,socket_tcp_descriptor_server);
                write(client_socket_fd, STREAM_END, BUFFER_SIZE - 1);

            }
            if (strcmp(buffer, DL) == 0) {
                write(client_socket_fd,DL, BUFFER_SIZE - 1);
                receive_file(client_socket_fd,socket_tcp_descriptor_server);
            }
            if (strcmp(buffer, DLINV) == 0) {
                write(client_socket_fd, DLINV, BUFFER_SIZE - 1);
            }
            if (strcmp(buffer, QUIT) == 0) {
                write(client_socket_fd,QUIT, BUFFER_SIZE - 1);
                client_running = 0;
            }
        }
    }
    close(socket_tcp_descriptor_server);
    close(client_socket_fd);
    pthread_detach(pthread_self());
    pthread_exit(NULL);

}

void receive_file(int client_fd, int server_fd) {
    int nread;
    char buffer[BUFFER_SIZE];
    long file_size = 0;
    long total_read = 0;
    long size_to_read;

    memset(buffer, '\0', BUFFER_SIZE);
    if ((nread = read(server_fd, buffer, BUFFER_SIZE - 1)) <= 0) {
        write(client_fd,buffer,BUFFER_SIZE -1);
        printf("Erro ao ler o tamanho do ficheiro");
    } else {
        write(client_fd,buffer,BUFFER_SIZE -1);
        file_size = atol(buffer);
        printf("tamanho do ficheiro : %ld\n", file_size);
    }

    while (total_read < file_size) {
        //memset(buffer, '\0', BUFFER_SIZE - 1);
        if ((file_size - total_read) / (BUFFER_SIZE - 1) == 0) {
            size_to_read = (file_size - total_read) % (BUFFER_SIZE - 1);
        } else {
            size_to_read = BUFFER_SIZE - 1;
        }

        nread = read(server_fd, buffer, size_to_read);
        buffer[nread] = '\0';
        printf("---->lido : %d\n", nread);
        printf("---> %s\n", buffer);
        write(client_fd, buffer, size_to_read); // Write to client

        total_read += nread;
    }
    //Receive and write eof
    read(server_fd, buffer, BUFFER_SIZE - 1);
    //printf("-AASDASDASDASDASDASD ASD ASD ASD AS %s\n", buffer);
    write(client_fd,buffer,BUFFER_SIZE - 1);
    printf("--> %s\n", buffer);

}

void receive_listing(int client_fd, int server_fd){
    char buffer[BUFFER_SIZE];
    int nread;
    do{
        nread = read(server_fd, buffer, BUFFER_SIZE - 1);
        buffer[nread] ='\0';
        //printf("\t\t%s\n", buffer);
        if(strcmp(buffer, STREAM_END) != 0){
            printf("%s\n", buffer);
            write(client_fd,buffer, BUFFER_SIZE - 1);
        }
    }while(strcmp(buffer, STREAM_END) != 0);
}