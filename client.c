/* Client
 * Usage:
 * Client <Proxy Address> <Server Address> <Port>

 */
#include <time.h>
#include "client.h"

int main(int argc, char* argv[]) {
    //Declaring variables
    int fd, port, running = 1;
    struct hostent *server_ptr, *proxy_ptr;
    struct sockaddr_in proxy, host;
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
    if ((proxy_ptr = gethostbyname(argv[1])) == 0){
        printf("Could not get Server Address, Exiting\n");
        exit(-1);
    }
    if ((server_ptr = gethostbyname(argv[2])) == 0){
        printf("Couldn't get Proxy Server Hostname, Exiting\n");
        exit(-1);
    }

    if((atoi(argv[3]) < 0 || atoi(argv[3]) > 65356)){
        printf("Invalid Port, port must be an integer between 1 and 65536\n");
        exit(-1);
    }
    if(strcmp(argv[4], "tcp\n") == 0){
        printf("Invalid Protocol, Needs to be either 'tcp' or 'udp'\n");
        exit(-1);
    }

    /*Establish a connection to the proxy via TCP*/
    bzero((void *) &proxy, sizeof(struct sockaddr_in));
    proxy.sin_family = AF_INET; //Defines IPV4
    proxy.sin_addr.s_addr = ((struct in_addr *)(proxy_ptr->h_addr))->s_addr; //
    proxy.sin_port = htons((short) atoi(argv[3])); //Port

    if((fd = socket(AF_INET,SOCK_STREAM,0)) == -1) { // Create the socket and bind it to a file descriptor
        printf("Error Creating Socket\n");
        exit(-1);
    }
    /*After setting the type of connection*/
    if(connect(fd,(struct sockaddr *)&proxy,sizeof (proxy)) < 0){// Connect to the proxy
        printf("Error connecting to the Proxy\n");
        exit(-1);
    }

    /*First message / Specify Protocol*/
    /*The first message sends a string containg the ip of the server to connect to and the protocol used by the server in a later connection*/

    /*sprintf(buffer, "%s,%s,%s",argv[2],argv[3],argv[4]); //Firstly the we send the IP of the server, then we send the port, then we send, protocol;
    message = malloc(strlen(buffer));
    strcpy(message,buffer);
    write(fd, message, 1 + strlen(message));
    free(message);*/
    /*After the initial connection start communicating with the server*/

    while(running){
        message = parse_user_message();
        write(fd,message, 1 + strlen(message));
        nread = read(fd, buffer, BUFFER_SIZE - 1);
        buffer[nread] ='\0';
        printf("Message received : %s %d\n", buffer,strcmp(buffer, LIST) == 0 );
        if(strcmp(buffer, LIST) == 0){
            receive_listing(fd);
        }

        if(strcmp(buffer, DL) == 0){
            receive_file(fd,message);
        }
        if(strcmp(buffer,DLINV) == 0){
            printf("You have selected an invalid file to download\n");
        }

        if(strcmp(message,QUIT) == 0){
            running = 0;
        }
        free(message);
    }
    close(fd);
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


void receive_file(int fd, char* msg) {
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
    if((nread = read(fd, buffer, BUFFER_SIZE - 1)) <= 0){
        printf("Erro ao ler o tamanho do ficheiro");
    }
    else{
        file_size = atol(buffer);
        printf("tamanho do ficheiro : %ld\n", file_size);
    }


    strcpy(filename, path);
    filename[strlen(filename)] = '/';
    for (i = 0; i < strlen(token); i++) {
        filename[strlen(path) + i + 1] = token[i];
    }


    fp = fopen(filename, "wb");

    while(total_read < file_size){
        //memset(buffer, '\0', BUFFER_SIZE - 1);
        if((file_size - total_read) / (BUFFER_SIZE - 1) == 0 ){
            size_to_read = (file_size - total_read) % (BUFFER_SIZE - 1);
        }
        else{
            size_to_read = BUFFER_SIZE - 1;
        }

        nread = read(fd, buffer, size_to_read);
        buffer[nread] = '\0';
        printf("---->lido : %d\n", nread);
        printf("---> %s\n", buffer);
        fwrite(buffer, sizeof(char), size_to_read, fp); // Write to file

        total_read += nread;
    }
    read(fd, buffer, BUFFER_SIZE - 1);
    //printf("--> %s", buffer);

    fclose(fp);



}
