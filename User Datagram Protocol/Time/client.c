#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define PORT 25060
char buff[128];
int main(){
    int sockfd, r;
    struct sockaddr_in serv_addr;
    socklen_t len = sizeof(serv_addr);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_aton("127.0.0.1", &serv_addr.sin_addr);
    printf("\nUDP TIME CLIENT\n");
    while(1){
        printf("\nEnter 'time' to get time or 'exit': ");
        fgets(buff, sizeof(buff), stdin);
        sendto(sockfd, buff, strlen(buff), 0,(struct sockaddr*)&serv_addr, len);
        if(strncmp(buff, "exit", 4) == 0){
            printf("Client exiting.\n");
            break;
        }
        r = recvfrom(sockfd, buff, sizeof(buff), 0,(struct sockaddr*)&serv_addr, &len);
        buff[r] = '\0';
        if(strncmp(buff, "SERVER_EXIT", 11) == 0){
            printf("Server requested shutdown. Exiting...\n");
            break;
        }
        printf("CURRENT TIME: %s\n", buff);
    }
    close(sockfd);
    return 0;
}
