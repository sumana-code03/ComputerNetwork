#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
struct sockaddr_in serv_addr, cli_addr;
int listenfd, connfd;
int r, w;
socklen_t cli_addr_len;
unsigned short serv_port = 25020;
char serv_ip[] = "127.0.0.1";
char sbuff[128];
char rbuff[128];
int main()
{
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serv_port);
    inet_aton(serv_ip, &serv_addr.sin_addr);
    printf("\nTCP CHAT SERVER\n");
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("SERVER ERROR: Socket creation failed\n");
        exit(1);
    }
    if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        printf("SERVER ERROR: Bind failed\n");
        close(listenfd);
        exit(1);
    }
    if(listen(listenfd, 5) < 0){
        printf("SERVER ERROR: Listen failed\n");
        close(listenfd);
        exit(1);
    }
    cli_addr_len = sizeof(cli_addr);
    printf("Waiting for client...\n");
    connfd = accept(listenfd,(struct sockaddr*)&cli_addr,&cli_addr_len);
    if(connfd < 0){
        printf("SERVER ERROR: Accept failed\n");
        close(listenfd);
        exit(1);
    }
    printf("Client Connected: %s\n", inet_ntoa(cli_addr.sin_addr));
    while(1){
        bzero(rbuff, sizeof(rbuff));
        r = read(connfd, rbuff, sizeof(rbuff));
        if(r <= 0){
            printf("Client Disconnected\n");
            break;
        }
        rbuff[r] = '\0';
        printf("\nClient : %s\n", rbuff);
        printf("Server : ");
        fgets(sbuff, sizeof(sbuff), stdin);
        w = write(connfd, sbuff, strlen(sbuff));
        if(w < 0){
            printf("SERVER ERROR: Write failed\n");
            break;
        }
        if(strncmp(sbuff, "bye", 3) == 0)
            break;
    }
    close(connfd);
    close(listenfd);
    return 0;
}
