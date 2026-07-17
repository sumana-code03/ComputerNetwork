#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#define PORT 25070
char buff[256];
int main(){
    int sockfd, r;
    struct sockaddr_in serv_addr;
    socklen_t len = sizeof(serv_addr);
    fd_set readfds;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_aton("127.0.0.1", &serv_addr.sin_addr);
    printf("\nUDP CHAT CLIENT\n");
    while(1){
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        FD_SET(0, &readfds);
        select(sockfd + 1, &readfds, NULL, NULL, NULL);
        if(FD_ISSET(0, &readfds)){
            fgets(buff, sizeof(buff), stdin);
            sendto(sockfd, buff, strlen(buff), 0,(struct sockaddr*)&serv_addr, len);
            if(strncmp(buff, "exit", 4) == 0){
                printf("Client exiting.\n");
                break;
            }
        }
        if(FD_ISSET(sockfd, &readfds)){
            r = recvfrom(sockfd, buff, sizeof(buff), 0,(struct sockaddr*)&serv_addr, &len);
            buff[r] = '\0';
            if(strncmp(buff, "SERVER_EXIT", 11) == 0){
                printf("\nServer requested shutdown. Exiting...\n");
                break;
            }
            printf("\nSERVER: %s", buff);
        }
    }
    close(sockfd);
    return 0;
}
