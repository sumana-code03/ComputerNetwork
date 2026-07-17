#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#define PORT 25036
#define BUF_SIZE 256
int main(){
    int sockfd, r;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len = sizeof(cli_addr);
    char buff[BUF_SIZE];
    char response[2048];
    fd_set readfds;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    printf("\nUDP COMMAND SERVER\n");
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
                sendto(sockfd, msg, strlen(msg), 0,(struct sockaddr*)&cli_addr, cli_len);
                continue;
            }
        }
        if(FD_ISSET(sockfd, &readfds)){
            r = recvfrom(sockfd, buff, sizeof(buff), 0,
                         (struct sockaddr*)&cli_addr, &cli_len);
            buff[r] = '\0';
            printf("CLIENT CMD: %s", buff);
            if(strncmp(buff, "exit", 4) == 0){
                printf("Client exited.\n");
                continue;
            }
            if(strncmp(buff, "cd", 2) == 0){
                char path[128];
                if(sscanf(buff, "cd %s", path) != 1)
                    strcpy(response, "Invalid format");
                else{
                    if(chdir(path) == 0)
                        strcpy(response, "Directory changed");
                    else
                        strcpy(response, "Failed to change directory");
                }
                sendto(sockfd, response, strlen(response), 0,
                       (struct sockaddr*)&cli_addr, cli_len);
                continue;
            }

            FILE *fp = popen(buff, "r");
            if(fp == NULL){
                strcpy(response, "Error executing command");
                sendto(sockfd, response, strlen(response), 0,
                       (struct sockaddr*)&cli_addr, cli_len);
                continue;
            }
            char temp[256];
            response[0] = '\0';
            while(fgets(temp, sizeof(temp), fp) != NULL){
                if(strlen(response) + strlen(temp) < sizeof(response)-1)
                    strcat(response, temp);
                else
                    break;
            }
            if(feof(fp)){
                printf("SERVER RESULT:\n%s\n", response);
                sendto(sockfd, response, strlen(response), 0,
                       (struct sockaddr*)&cli_addr, cli_len);
            }
            else{
                FILE *file = fopen("output.txt", "w");
                fputs(response, file);
                while(fgets(temp, sizeof(temp), fp) != NULL)
                    fputs(temp, file);
                fclose(file);
                FILE *f = fopen("output.txt", "r");
                fseek(f, 0, SEEK_END);
                long size = ftell(f);
                rewind(f);
                sprintf(response, "FILE %ld", size);
                sendto(sockfd, response, strlen(response), 0,
                       (struct sockaddr*)&cli_addr, cli_len);
                int n;
                while((n = fread(temp, 1, sizeof(temp), f)) > 0){
                    sendto(sockfd, temp, n, 0,(struct sockaddr*)&cli_addr, cli_len);
                    usleep(1000);
                }
                fclose(f);
                printf("SERVER RESULT: Sent as file (%ld bytes)\n", size);
            }
            pclose(fp);
        }
    }
    close(sockfd);
    return 0;
}
