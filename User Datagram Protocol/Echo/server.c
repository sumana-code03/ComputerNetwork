#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define PORT 25020
int main(
{
    int sockfd, r;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t len = sizeof(cli_addr);
    char buff[128];
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    printf("\nUDP SERVER \n");
    r = recvfrom(sockfd, buff, sizeof(buff), 0,(struct sockaddr*)&cli_addr, &len);
    buff[r] = '\0';
    printf("\nSERVER: Received '%s' from %s\n",buff, inet_ntoa(cli_addr.sin_addr));
    printf("\nSERVER: Exiting...\n");
    close(sockfd);
    return 0;
}
