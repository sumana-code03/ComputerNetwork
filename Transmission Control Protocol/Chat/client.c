#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
struct sockaddr_in serv_addr;
int skfd;
int r, w;
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
    printf("\nTCP CHAT CLIENT\n");
    if((skfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("CLIENT ERROR: Socket creation failed\n");
        exit(1);
    }
    if(connect(skfd,(struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        printf("CLIENT ERROR: Connection failed\n");
        close(skfd);
        exit(1);
    }
    printf("Connected to Server\n");
    while(1){
        printf("\nClient : ");
        fgets(sbuff, sizeof(sbuff), stdin);
        w = write(skfd, sbuff, strlen(sbuff));
        if(w < 0){
            printf("CLIENT ERROR: Write failed\n");
            break;
        }
        if(strncmp(sbuff, "bye", 3) == 0)
            break;
        bzero(rbuff, sizeof(rbuff));
        r = read(skfd, rbuff, sizeof(rbuff));
        if(r <= 0){
            printf("Server Disconnected\n");
            break;
        }
        rbuff[r] = '\0';
        printf("Server : %s", rbuff);
    }
    close(skfd);
    return 0;
}
