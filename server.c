#include "server.h"


int main(int argc, char* argv[]){

    if(argc != 3){
        printf("Wrong Command Syntax\n"
               " Use : \"./server <Port> <Max_Clients>\"\n");
        exit(-1);
    }
    if((atoi(argv[1]) < 0 || atoi(argv[1]) > 65356)){
        printf("Invalid Port, port must be an integer between 1 and 65536\n");
        exit(-1);
    }
    if((atoi(argv[2]) < 1)){
        printf("Invalid number, number of clients must be a number greater than 0.\n");
        exit(-1);
    }

    //stores the server port and the maximum number of simultaneous users
    server_port = atoi(argv[1]);
    max_clients = atoi(argv[2]);

    //sets the memory to zero
    bzero((void *) &welcoming_socket_info, sizeof(welcoming_socket_info));

    //loads the info of the server to the struct for the socket
    welcoming_socket_info.sin_family = AF_INET;
    welcoming_socket_info.sin_addr.s_addr = inet_addr(IP_SERVER);
    welcoming_socket_info.sin_port = htons(server_port);

    client_socket_info_size = sizeof(struct sockaddr_in);

    if((welcoming_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("Erro ao criar o welcoming socket.\n");
    }

    if(bind(welcoming_socket,(struct sockaddr *) &welcoming_socket_info, sizeof(struct sockaddr_in)) == -1){
        printf("Erro ao dar bind do welcoming socket.\n");
    }

    if(listen(welcoming_socket, 5) == - 1){
        printf("Error in the listening function");
    }


    while(1){//TODO colocar uma condicao de paragem para que possamos fechar o servidor

        while(waitpid(-1,NULL,WNOHANG)>0);

        client_socket = accept(welcoming_socket, (struct sockaddr *) &client_socket_info, (socklen_t *) &client_socket_info_size);
        //create the handler thread for the received client
        if(fork() == 0){
            client(client_socket);
            exit(0);
        }

        //TODO verificar se esta tudo o que precisa ser feito feito
    }

}

void client(int socket_descriptor){//TODO fazer o codigo para ler o comando e responder de maneira adequada
    printf("CHEGUEI\n");
    //varables for the command received
    char buffer[BUFFER_SIZE];
    int nread;//used to store the number of bytes read
    FILE *fp;//file pointer used to open the required file
    char *token;
    char del[2] = " ";
    int protocol_mode;//if it is equal to 0 (TCP), if it is equal to 1 (UDP)
    int encription_mode;//if it is equal to 0 (NOR), if it is equal to 1 (ENC)




    while(running) {//Reads the commands sent by the client
        if ((nread = read(socket_descriptor, buffer, BUFFER_SIZE - 1)) < 0) {
            printf("Erro ao ler o comando do cliente.\n");
        } else {
            printf("%s\n", buffer);
        }

        buffer[nread] = '\0';
        //if the code has '\n' it is removed
        if (nread > 0) {
            if (buffer[nread - 1] == '\n') buffer[nread - 1] = '\0';

            //verificar qual e o tipo de protocolo que e para ser usado e se e para encriptar ou nao a informacao enviada
            token = strtok(buffer, del);
            if (strcmp(token, LIST) == 0) {
                //printf("Listing.\n");
                write(client_socket,LIST, BUFFER_SIZE - 1);

                //list the files in the download directory
                list_files();
                //sends the EOF message
                write(client_socket, STREAM_END, BUFFER_SIZE - 1);
                //TODO codigo para dar list dos ficheiros armazenados no server
            } else if (strcmp(token, DL) == 0) {

                //gets the protocol and encription types selected by the client
                token = strtok(NULL, del);
                if (strcmp(token, "TCP") == 0) {
                    protocol_mode = 0;
                } else if (strcmp(token, "UDP") == 0) {
                    protocol_mode = 1;
                } else {
                    printf("Erro no protocolo enviado\n");
                }
                token = strtok(NULL, del);

                if (strcmp(token, "NOR") == 0) {
                    encription_mode = 0;
                } else if (strcmp(token, "ENC") == 0) {
                    encription_mode = 1;
                } else {
                    printf("Erro no tipo de encriptacao escolhido.\n");
                }
                token = strtok(NULL, del);
                //check if the file name received exists in the download directory

                if((fp = get_filepointer(token)) != NULL){
                    printf("Upload starting.\n");
                    write(client_socket, DL, BUFFER_SIZE - 1);
                    strcpy(buffer, token);
                    upload_file(fp, buffer);//sends the file

                    //sends the EOF message
                    write(client_socket, STREAM_END, BUFFER_SIZE - 1);
                }
                else{
                    //printf("%s\n",token);
                    write(client_socket, DLINV, BUFFER_SIZE - 1);
                }
            } else if (strcmp(token, QUIT) == 0) {
                printf("Client Quitting.\n");
                //sends the quitting message
                write(client_socket, QUIT, BUFFER_SIZE - 1);
                running = 0;
            } else {
                printf("Erro no comando enviado pelo cliente.\n");
            }
        }
    }
}


//on success returns the file pointer to the file, otherwise returns null
FILE *get_filepointer(char *file_name){
    //Variables for the directory
    struct dirent *dp;
    DIR *dir;
    FILE *fpointer = NULL;
    dir = opendir(dir_path);
    char filename[100];
    int i;
    strcpy(filename, dir_path);
    filename[strlen(filename)] = '/';
    for(i = 0; i < strlen(file_name) ; i++){
        filename[strlen(dir_path) + i + 1] = file_name[i];
    }
    filename[strlen(dir_path) + i + 1] = '\0';

    while((dp = readdir(dir)) != NULL){
        printf("%s %s %d\n",dp -> d_name, file_name,strcmp(dp -> d_name, file_name));
        if(strcmp(dp -> d_name, file_name) == 0){
            printf("%s\n",filename);
            fpointer = fopen(filename,"rb");
            if(fpointer == NULL) printf("FODEU\n");
            return fpointer;
        }
    }
    return NULL;
}

void list_files(){
    FILE *fp;
    DIR *dir;
    struct dirent *dp;
    int i = 0;
    dir = opendir(dir_path);
    while((dp = readdir(dir))){
        if(strcmp(dp->d_name,".") != 0 && strcmp(dp->d_name,"..") != 0){
            //printf("%s\n", dp->d_name);
            write(client_socket, dp->d_name, BUFFER_SIZE - 1);
        }
    }
}

void upload_file(FILE* fp, char *path){
    char buffer[BUFFER_SIZE];
    struct stat properties;
    int total = 0;
    fstat(fileno(fp), &properties);
    int nread = 0;
    long file_size = properties . st_size;
    long size_to_write = 0;

    printf("<<<<<<<<<<<>>>>> %lld\n", properties . st_size);
    memset(buffer, '\0', BUFFER_SIZE - 1);
    sprintf(buffer, "%lld", properties . st_size);
    printf("-------->%s\n", buffer);
    write(client_socket, buffer, sizeof(size_t));

    usleep(100);
    while(total < file_size){
        if((file_size - total) / (BUFFER_SIZE - 1) == 0 ){
            size_to_write = (file_size - total) % (BUFFER_SIZE - 1);
        }
        else{
            size_to_write = BUFFER_SIZE - 1;
        }

        //memset(buffer, '\0', BUFFER_SIZE - 1);
        nread = fread(buffer, sizeof(char), size_to_write, fp);
        buffer[nread] = '\0';
        printf("%s", buffer);
        total += nread;
        write(client_socket, buffer, size_to_write);
        printf("Enviado : %d\n", nread);
    }

    fclose(fp);



}