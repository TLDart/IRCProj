#include "parser.h"
/*
int main(){
    printf("%d -> %s\n", check_valid("LIST"),"LIST");
    printf("%d -> %s\n", check_valid("DOWNLOAD TCP ENC test"),"DOWNLOAD TCP ENC test");
    printf("%d -> %s\n", check_valid("DOWNLOAD TCP ENC"),"DOWNLOAD TCP ENC");
    printf("%d -> %s\n", check_valid("QUIT"),"QUIT");
    printf("%d -> %s\n", check_valid("DOWNLOAD ENC TCP test"),"DOWNLOAD ENC TCP test");
    printf("%d -> %s\n", check_valid("QUIT MERDA"),"QUIT MERDA");
    printf("%d -> %s\n", check_valid("MERDA"),"MERDA");
}
*/

char *parse_user_message(){
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
            if(check_valid(nowaste)){
                return nowaste;
            }
            free(nowaste);
        }
        printf("You command was invalid\n");
    }
}


int check_valid(char *message){
    /*Checks the validity of a message according to the predefined requirements
     * Return
     *      1 if the message is valid
     *      0 if the message is no valid
     * */
    char delimiter[2] = " ";
    char *token;
    char *copy = malloc(strlen(message));
    strcpy(copy, message);

    token = strtok(copy, delimiter);
    if(strcmp(token, "LIST") == 0){
        if((strtok(NULL, delimiter)) != NULL) return 0;// if there is another element abort
        else return 1;
    }
    else if(strcmp(token, "DOWNLOAD") == 0){
        token = strtok(NULL, delimiter);
        if(token == NULL) return 0;
        else{
            if(strcmp(token, "UDP") == 0 || strcmp(token, "TCP") == 0){
                token = strtok(NULL, delimiter);
                if(token == NULL) return 0;//If there is another element abort
                else{
                    if(strcmp(token, "ENC") == 0 || strcmp(token, "NOR") == 0){
                        token = strtok(NULL, delimiter);
                        if(token == NULL) return 0;
                        else{
                            token = strtok(NULL, delimiter);
                            if(token != NULL) return 0;
                            else return 1;
                        }
                    }
                    return 0;
                }
            }
            return 0;
        }
    }
    else if(strcmp(token, "QUIT") == 0){
        if((strtok(NULL, delimiter)) != NULL){ //If there is another element abort
            return 0;
        }
        return 1;
    }
    else{
        return 0;
    }
}