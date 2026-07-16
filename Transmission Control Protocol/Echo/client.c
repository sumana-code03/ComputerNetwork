#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
struct sockaddr_in serv_addr;    
int skfd, r, w;
unsigned short serv_port = 25050;
char serv_ip[] = "127.0.0.1";
char rbuff[128];
char sbuff[128] = "good morning"; 
int main()
{
	bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serv_port);
    inet_aton(serv_ip, (&serv_addr.sin_addr));
	printf("\nTCP ECHO CLIENT.\n");
    if((skfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("\nCLIENT ERROR: Cannot create socket.\n");
        exit(1);
    }
	if((connect(skfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0){
		printf("\nCLIENT ERROR: Cannot connect to the server.\n");
		close(skfd);
		exit(1);
	}
	printf("\nCLIENT: Connected to the server.\n");
	if((w = write(skfd, sbuff, 128)) < 0)       	{
		printf("\nCLIENT ERROR: Cannot send message to the echo server.\n");
		close(skfd);
		exit(1);
	}
	printf("\nCLIENT: Message sent to echo server.\n");
	if((r = read(skfd, rbuff, 128)) < 0)
		printf("\nCLIENT ERROR: Cannot receive mesage from server.\n");
	else{
		rbuff[r] = '\0';
		printf("\nCLIENT: Message from echo server: %s\n", rbuff);
	}
	close(skfd);
	exit(1);
}
