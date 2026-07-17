#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/select.h>
#define PORT 25060
char buff[128];
int main(){
    int sockfd, r;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t len = sizeof(cli_addr);
    char response[128];
    fd_set readfds;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    printf("\nUDP TIME SERVER\n");
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
                sendto(sockfd, msg, strlen(msg), 0,(struct sockaddr*)&cli_addr, len);
            }
        }
        if(FD_ISSET(sockfd, &readfds)){
            r = recvfrom(sockfd, buff, sizeof(buff), 0,(struct sockaddr*)&cli_addr, &len);
            buff[r] = '\0';
            printf("CLIENT: %s", buff);
            if(strncmp(buff, "exit", 4) == 0){
                printf("Client exited.\n");
                continue;
            }
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            strftime(response, sizeof(response),"%Y-%m-%d %H:%M:%S", t);
            printf("SERVER TIME: %s\n", response);
            sendto(sockfd, response, strlen(response), 0,
                   (struct sockaddr*)&cli_addr, len);
        }
    }
    close(sockfd);
    return 0;
}
