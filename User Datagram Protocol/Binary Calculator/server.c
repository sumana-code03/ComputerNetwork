#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#define PORT 25028
char buff[128];
void decToBinary(int n, char *res){
    char temp[64]; int i = 0;
    if(n == 0){ strcpy(res, "0"); return; }
    while(n > 0){ temp[i++] = (n % 2) + '0'; n /= 2; }
    int j = 0; while(i > 0) res[j++] = temp[--i];
    res[j] = '\0';
}
int binToDecimal(char *bin){
    int result = 0;
    for(int i = 0; bin[i] != '\0'; i++){
        if(bin[i] != '0' && bin[i] != '1') return -1;
        result = result * 2 + (bin[i] - '0');
    }
    return result;
}
int main(){
    int sockfd, r;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len = sizeof(cli_addr);
    char response[128];
    fd_set readfds;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    printf("\nUDP CALCULATOR SERVER\n");
    while(1){
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        FD_SET(0, &readfds);
        select(sockfd + 1, &readfds, NULL, NULL, NULL);
        if(FD_ISSET(0, &readfds)){
            fgets(buff, sizeof(buff), stdin);
            if(strncmp(buff, "exit", 4) == 0){
                printf("SERVER: Sending shutdown signal...\n");
                char msg[] = "SERVER_EXIT";
                sendto(sockfd, msg, strlen(msg), 0,
                       (struct sockaddr*)&cli_addr, cli_len);
                continue;
            }
        }
        if(FD_ISSET(sockfd, &readfds)){
            r = recvfrom(sockfd, buff, sizeof(buff), 0,(struct sockaddr*)&cli_addr, &cli_len);
            buff[r] = '\0';
            printf("CLIENT: %s", buff);
            if(strncmp(buff, "exit", 4) == 0){
                printf("Client exited.\n");
                continue;
            }
            if(buff[0] == '1'){
                int num;
                if(sscanf(buff, "1 %d", &num) != 1)
                    strcpy(response, "Invalid format");
                else
                    decToBinary(num, response);
            }
            else if(buff[0] == '2'){
                char bin[100]; int val;
                if(sscanf(buff, "2 %s", bin) != 1)
                    strcpy(response, "Invalid format");
                else{
                    val = binToDecimal(bin);
                    if(val == -1)
                        strcpy(response, "Invalid binary number");
                    else
                        sprintf(response, "%d", val);
                }
            }
            else if(buff[0] == '3'){
                char expr[100], command[200];
                FILE *fp;
                if(sscanf(buff, "3 %[^\n]", expr) != 1)
                    strcpy(response, "Invalid format");
                else{
                    sprintf(command, "echo \"%s\" | bc", expr);
                    fp = popen(command, "r");
                    if(fp == NULL)
                        strcpy(response, "Error executing bc");
                    else{
                        if(fgets(response, sizeof(response), fp) == NULL)
                            strcpy(response, "Invalid expression");
                        else
                            response[strcspn(response, "\n")] = '\0';
                        pclose(fp);
                    }
                }
            }
            else
                strcpy(response, "Invalid choice");
            printf("SERVER RESULT: %s\n", response);
            sendto(sockfd, response, strlen(response), 0,
                   (struct sockaddr*)&cli_addr, cli_len);
        }
    }
    close(sockfd);
    return 0;
}
