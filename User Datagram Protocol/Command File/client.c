#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define PORT 25036
#define BUF_SIZE 256
int main(){
    int sockfd, r;
    struct sockaddr_in serv_addr;
    socklen_t len = sizeof(serv_addr);
    char buff[BUF_SIZE];
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_aton("127.0.0.1", &serv_addr.sin_addr);
    printf("\nUDP COMMAND CLIENT\n");
    while(1){
        printf("\nEnter command (or exit): ");
        fgets(buff, sizeof(buff), stdin);
        sendto(sockfd, buff, strlen(buff), 0,
               (struct sockaddr*)&serv_addr, len);
        if(strncmp(buff, "exit", 4) == 0){
            printf("Client exiting.\n");
            break;
        }
        r = recvfrom(sockfd, buff, sizeof(buff), 0,
                     (struct sockaddr*)&serv_addr, &len);
        buff[r] = '\0';

        if(strncmp(buff, "FILE", 4) == 0){
            long filesize;
            sscanf(buff, "FILE %ld", &filesize);
            FILE *fp = fopen("received.txt", "w");
            printf("Receiving %ld bytes...\n", filesize);
            long received = 0;
            while(received < filesize){
                r = recvfrom(sockfd, buff, sizeof(buff), 0,
                             (struct sockaddr*)&serv_addr, &len);
                fwrite(buff, 1, r, fp);
                received += r;
            }
            fclose(fp);
            printf("Saved to received.txt\n");
        }
        else{
            printf("OUTPUT:\n%s\n", buff);
            FILE *fpp=fopen("received.txt","w");
            fprintf(fpp,"%s",buff);
            fclose(fpp);
        }
        if(strncmp(buff, "SERVER_EXIT", 11) == 0){
            printf("Server requested shutdown. Exiting...\n");
            break;
        }
    }
    close(sockfd);
    return 0;
}
