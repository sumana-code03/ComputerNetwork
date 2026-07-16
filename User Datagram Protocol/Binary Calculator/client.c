#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define PORT 25028
char buff[128];
int main(){
    int sockfd, r;
    struct sockaddr_in serv_addr;
    socklen_t len = sizeof(serv_addr);
    int choice;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_aton("127.0.0.1", &serv_addr.sin_addr);
    printf("\nUDP CALCULATOR CLIENT\n");
    while(1){
        printf("\n===== MENU =====\n1. Decimal to Binary\n2. Binary to Decimal\n3. Solve Expression\n4. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();
        if(choice == 4){
            strcpy(buff, "exit");
            sendto(sockfd, buff, strlen(buff), 0,(struct sockaddr*)&serv_addr, len);
            printf("Client exiting.\n");
            break;
        }
        if(choice == 1){
            int num;
            printf("Enter decimal number: ");
            scanf("%d", &num);
            getchar();
            sprintf(buff, "1 %d", num);
        }
        else if(choice == 2){
            printf("Enter binary number: ");
            scanf("%s", buff);
            getchar();
            char temp[128];
            snprintf(temp, sizeof(temp), "2 %s", buff);
            strcpy(buff, temp);
        }
        else if(choice == 3){
            printf("Enter expression: ");
            fgets(buff, sizeof(buff), stdin);
            char temp[128];
            snprintf(temp, sizeof(temp), "3 %s", buff);
            strcpy(buff, temp);
        }
        else{
            printf("Invalid choice. Try again.\n");
            continue;
        }
        sendto(sockfd, buff, strlen(buff), 0,
               (struct sockaddr*)&serv_addr, len);
        r = recvfrom(sockfd, buff, sizeof(buff), 0,
                     (struct sockaddr*)&serv_addr, &len);
        buff[r] = '\0';
        if(strncmp(buff, "SERVER_EXIT", 11) == 0){
            printf("Server requested shutdown. Exiting...\n");
            break;
        }
        printf("RESULT: %s\n", buff);
    }
    close(sockfd);
    return 0;
}
