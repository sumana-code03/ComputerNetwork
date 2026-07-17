#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define PORT 25020
int main(){
    int sockfd, w;
    struct sockaddr_in serv_addr;
    socklen_t len = sizeof(serv_addr);
    char sbuff[128] = "===good morning===";
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_aton("127.0.0.1", &serv_addr.sin_addr);
    printf("\nUDP CLIENT\n");
    w = sendto(sockfd, sbuff, strlen(sbuff), 0,(struct sockaddr*)&serv_addr, len);
    printf("\nCLIENT: Message sent: %s\n", sbuff);
    printf("CLIENT: Exiting...\n");
    close(sockfd);
    return 0;
}
