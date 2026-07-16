#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
struct sockaddr_in serv_addr, cli_addr;
int listenfd, connfd, r, w, val, cli_addr_len;
unsigned short serv_port = 25050;
char serv_ip[] = "127.0.0.1";
char buff[128];
int main()
{
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(serv_port); 
    inet_aton(serv_ip, (&serv_addr.sin_addr)); 
    printf("\nTCP ECHO SERVER.\n");
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
	for( ; ;)
	{
		printf("\nSERVER: Listenning for clients...Press Cntrl + c to stop echo server:\n");
		if((connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &cli_addr_len)) < 0){
			printf("\nSERVER ERROR: Cannot accept client connections.\n");
			close(listenfd);
			exit(1);
		}
		printf("\nSERVER: Connection fron client %s accepted.\n", inet_ntoa(cli_addr.sin_addr));
		if((r = read(connfd, buff, 128)) < 0)
	        printf("\nSERVER ERROR: Cannot receive message from client.\n");
	    else{
            buff[r] = '\0';
			if((w = write(connfd, buff, 128)) < 0)
		        printf("\nSERVER ERROR: Cannot send message to the client.\n");
			else
				printf("\nSERVER: Echoed back %s to %s.\n", buff, inet_ntoa(cli_addr.sin_addr));
		}
	} 
} 
