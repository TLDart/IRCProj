#include "proxy.h"
int main(int argc, char* argv[]) {
    char *message;
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

    if(pthread_create(&tcp_thread, NULL, tcp_thread_handler, NULL) < 0){
        printf("Error Creating a thread UDP");
    }
    while(running){
        read_user_input();
    }

}
void read_user_input(){
    /*Get user input and makes sure it is valid to send to the server*/
    char temp[BUFFER_SIZE], *nowaste;
    /*Get the user message*/
    while(1){
        printf("Type your command: ");
        fgets(temp, BUFFER_SIZE, stdin);
        if(strcmp(temp, "\n") != 0){
            //printf("here");
            nowaste = malloc(strlen(temp)* sizeof(char));
            strcpy(nowaste, temp);
            nowaste[strcspn(nowaste, "\n")] = 0;
            if(check_valid(nowaste) < 0){
                printf("Your command was invalid\n");
            }
            free(nowaste);
        }
    }
}

int check_valid(char* message){
    /**
     * Checks if the message is valid
     *
     *  Returns 1 if the message is either SHOW OR SAVE
     *  Returns n >= 0 if message is LOSSES and the value is valid
     *  Returns -1 if the mesage is invalid
     *
     */
    char delimiter[2]  = " ";
    char *token = strtok(message, delimiter);
    if(strcmp(token, "SHOW") == 0 || strcmp(token, "SAVE") == 0){
        return 1;
    }
    else{
        if(strcmp(token, "LOSSES") == 0){
            token = strtok(NULL, delimiter);
            if(token != NULL){
                //printf("->>>>>>>%s\n", token);
                if(atoi(token) != 0 || strcmp(token,"0") == 0){
                    losses = atoi(token);
                    //printf("%d\n", losses);
                    return losses;
                }
                return -1;
            }
            return -1;
        }
    }
    return -1;
    }


void *udp_thread_handler(){
    printf("Created Sucessfully\n");
    char buffer[BUFFER_SIZE];
    struct sockaddr_in other;
    socklen_t s_other = sizeof(other);
    struct socket_info socketinfo;
    int nread;
    //loads the info of the server to the struct for the socket

    client_socket_info_size_udp = sizeof(struct sockaddr_in);

    if((udp_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        printf("Erro ao criar o welcoming socket.\n");
        exit(-1);
    }
    if(bind(udp_fd,(struct sockaddr *) &welcoming_socket_info, sizeof(struct sockaddr_in)) == -1){
        printf("Erro ao dar bind do welcoming socket.\n");
        exit(-1);
    }

    while(1){

        //create the handler thread for the received client
        if((nread = recvfrom(udp_fd, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *) &other, &s_other)) == -1){
            printf("Erro a ler a mensagem.\n");
        }
        buffer[nread] = '\0';
        printf("MESSAGE IN HANDLER: %s\n", buffer);
        strcpy(socketinfo.buffer, buffer);
        socketinfo.client_info = other;
        printf("MESSAGE IN STRUCT: %s\n", socketinfo.buffer);
        pthread_create(&threads,NULL,udp_client,&socketinfo);
    }
}
void* udp_client(void* arg){
    //sends to the server the ip and port of the client so he can send back the info
    printf("GOT HERE\n");
    struct socket_info  socket= *((struct socket_info*) arg);
    char delimiter[2] = ",";
    char ip_to_string [BUFFER_SIZE];
    int i = 0;
    char msg[BUFFER_SIZE];
    char ip [INET_ADDRSTRLEN];
    char command [BUFFER_SIZE];
    int flag = 0;
    socklen_t server_tcp_size = sizeof(server_tcp);

    //Parses the IP
    for(i = 0; i < strlen(socket.buffer); i++){
        if(socket.buffer[i] == ','){
                flag = 1;
                i++;//passa a ',' a frente
        }
        if(flag == 0){
            ip[i] = socket.buffer[i];//escreve o ip no buffer
            }
        else if(flag == 1){
            printf("%c\n", socket.buffer[i]);
            command[i - 1 - strlen(ip)] = socket.buffer[i];//escreve o
            }
    }
    printf("IP up here: %s\n", ip);
    //printf("command up here: %s\n", command);

    if(strcmp(ip,"127.0.0.4") == 0){
        inet_ntop(AF_INET,&(socket.client_info.sin_addr),ip_to_string,INET_ADDRSTRLEN);

        sprintf(msg,"%s,%d,%s",ip_to_string,socket.client_info.sin_port,command);
        printf("%s\n",msg);
        sendto(udp_fd,msg, BUFFER_SIZE - 1,0,(struct sockaddr *) &server_tcp, sizeof(server_tcp));
    }else{
        printf("IP DOWN here: %s\n", ip);
        printf("COMMAND DOWN here: %s\n", command);
        printf("OH RIP\n");
        printf("---> %s\n", socket.buffer);

        char* string = malloc(strlen(socket.buffer));
        char* token, *ip_to_send, *port, *message;
        char del[2] = ",";
        int i = 0;
        struct hostent *client_ptr;
        struct sockaddr_in client_info;
        strcpy(string,socket.buffer);

        token = strtok_r(string,del,&string);
        //sets the client IP
        ip_to_send = token;
        token = strtok_r(NULL, del, &string);
        port = token;
        token = strtok_r(NULL, del, &string);
        message = token;

        printf("IP: %s\n",ip_to_send);
        printf("PORT: %s\n",port);
        printf("COMMAND: %s\n",message);

        if((client_ptr = gethostbyname("127.0.0.22")) < 0){
            perror("DEU merda");
        }


        bzero((void *) &client_info, sizeof(struct sockaddr_in));
        client_info.sin_port = htons((short) atoi(token));
        client_info.sin_addr.s_addr = ((struct in_addr *) (client_ptr->h_addr))->s_addr;
        client_info.sin_family = AF_INET;




        if(sendto(udp_fd,"THIS IS A MESSAGE", BUFFER_SIZE -  1, 0, (struct sockaddr *) &client_info, sizeof(client_info)) < 0){
            perror("PRoxy sending message");
        }
    }

    //char* token = strtok_r(socket.buffer,delimiter, &socket.buffer);
    //token = strtok_r(NULL,delimiter, &buffer);
    //printf(" PORT : %d\n", socket.client_info.sin_port);
    //printf(" IP : %s\n",ip_to_string);
    //inet_ntop(AF_INET, &(server_tcp.sin_addr),msg_to_send,INET_ADDRSTRLEN);

    printf("THREAD EXITS\n");
    pthread_detach(pthread_self());
    pthread_exit(NULL);
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

    if(pthread_create(&udp_thread, NULL, udp_thread_handler, NULL) < 0){
        printf("Error Creating a thread UDP");
    }

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
        printf("Error connecting to the Server\n");
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