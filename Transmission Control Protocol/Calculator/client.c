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
unsigned short serv_port = 25070;
char serv_ip[] = "127.0.0.1";
char buff[128];
int main()
{
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serv_port);
    inet_aton(serv_ip, (&serv_addr.sin_addr));
    printf("\nTCP CALCULATOR CLIENT\n");
    if((skfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\nCLIENT ERROR: Cannot create socket.\n");
        exit(1);
    }
    if((connect(skfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0)
    {
        printf("\nCLIENT ERROR: Cannot connect.\n");
        close(skfd);
        exit(1);
    }
    printf("\nConnected to server.\n");
    while(1)
    {
        printf("\nEnter expression (e.g., 10 + 5) or 'exit': ");
        fgets(buff, 128, stdin);
        w = write(skfd, buff, strlen(buff));
        if(w < 0)
        {
            printf("\nCLIENT ERROR: Write failed.\n");
            break;
        }
        if(strncmp(buff, "exit", 4) == 0)
        {
            printf("Client exiting.\n");
            break;
        }
        r = read(skfd, buff, 128);
        if(r < 0)
        {
            printf("\nCLIENT ERROR: Read failed.\n");
            break;
        }
        if(r == 0)
        {
            printf("\nServer disconnected.\n");
            break;
        }
        buff[r] = '\0';
        printf("RESULT: %s\n", buff);
    }
    close(skfd);
    return 0;
}
