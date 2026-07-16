#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
struct sockaddr_in serv_addr;
int skfd, r;
unsigned short serv_port = 25050;
char serv_ip[] = "127.0.0.1";
char buff[128];
int main()
{
    int choice;
    skfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serv_port);
    inet_aton(serv_ip, (&serv_addr.sin_addr));
    if(connect(skfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Connection Failed\n");
        return 1;
    }
    while(1)
    {
        printf("\n===MENU===\n1.DecToBin\n2.BODMAS\n3.BinToDec\n4.Exit\nChoice: ");
        scanf("%d", &choice);
        getchar();
        if(choice == 4)
        {
            write(skfd, "exit", 4); break;
        }
        if(choice == 1)
        {
            int n;
            printf("Decimal: ");
            scanf("%d", &n);
            sprintf(buff, "1 %d", n);
        }
        else if(choice == 2)
        {
            char expr[100];
            printf("Expression: ");
            fgets(expr, 100, stdin);
            sprintf(buff, "2 %s", expr);
        }
        else if(choice == 3)
        {
            char bin[64];
            printf("Binary: ");
            scanf("%s", bin);
            sprintf(buff, "3 %s", bin);
        }
        else
            continue;
        write(skfd, buff, strlen(buff));
        r = read(skfd, buff, sizeof(buff));
        buff[r] = '\0';
        printf("RESULT: %s\n", buff);
    }
    close(skfd);
    return 0;
}
