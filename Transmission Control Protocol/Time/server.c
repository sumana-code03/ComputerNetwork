#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
struct sockaddr_in serv_addr, cli_addr;
int listenfd, connfd, r, w, cli_addr_len;
unsigned short serv_port = 25020;
char serv_ip[] = "127.0.0.1";
char buff[128];
int main()
{
    time_t current_time;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serv_port);
    inet_aton(serv_ip, (&serv_addr.sin_addr));
    printf("\nTCP TIME SERVER.\n");
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("\nSERVER ERROR: Cannot create socket.\n");
        exit(1);
    }
    if((bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0){
        printf("\nSERVER ERROR: Cannot bind.\n");
        close(listenfd);
        exit(1);
    }
    if((listen(listenfd, 5)) < 0){
        printf("\nSERVER ERROR: Cannot listen.\n");
        close(listenfd);
        exit(1);
    }
    cli_addr_len = sizeof(cli_addr);
    for( ; ; ){
        printf("\nSERVER: Waiting for client...\n");
        if((connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &cli_addr_len)) < 0){
            printf("\nSERVER ERROR: Cannot accept client.\n");
            continue;
        }
        printf("\nConnected to %s\n", inet_ntoa(cli_addr.sin_addr));
        while(1){
            r = read(connfd, buff, 128);
            if(r < 0){
                printf("\nSERVER ERROR: Read failed.\n");
                break;
            }
            if(r == 0){
                printf("\nClient disconnected.\n");
                break;
            }
            buff[r] = '\0';
            printf("CLIENT: %s\n", buff);
            if(strncmp(buff, "exit", 4) == 0){
                printf("Client exited.\n");
                break;
            }
            time(&current_time);
            strcpy(buff, ctime(&current_time));
            if((w = write(connfd, buff, strlen(buff))) < 0){
                printf("\nSERVER ERROR: Write failed.\n");
                break;
            }
            else
                printf("SERVER: Sent time to client.%s\n");
        }
        close(connfd);
    }
}
