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
        printf("Error Creating a thread");
    }

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
        pthread_create(&threads,NULL,client,&welcoming_socket);

        //TODO verificar se esta tudo o que precisa ser feito feito
    }



}

void *client(void *socket_descriptor) {//TODO fazer o codigo para ler o comando e responder de maneira adequada
//void client(int socket_descriptor) {//TODO fazer o codigo para ler o comando e responder de maneira adequada
    printf("CREATED\n");
    pthread_detach(pthread_self());
    pthread_exit(NULL);

}

void *udp_thread_handler(){
    printf("Created Sucessfully\n");
}