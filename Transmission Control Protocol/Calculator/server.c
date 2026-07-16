#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
struct sockaddr_in serv_addr, cli_addr;
int listenfd, connfd, r, w, cli_addr_len;
unsigned short serv_port = 25070;
char serv_ip[] = "127.0.0.1";
char buff[128];
void handle_exit(int sig)
{
    printf("\nServer exiting...\n");
    close(connfd);
    close(listenfd);
    exit(0);
}
int main()
{
    float num1, num2, result;
    char op;
    char response[128];
    signal(SIGINT, handle_exit);
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serv_port);
    inet_aton(serv_ip, (&serv_addr.sin_addr));
    printf("\nTCP CALCULATOR SERVER\n");
    printf("Press Ctrl+C to exit server\n\n");
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\nSERVER ERROR: Cannot create socket.\n");
        exit(1);
    }
    if((bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0)
    {
        printf("\nSERVER ERROR: Cannot bind.\n");
        close(listenfd);
        exit(1);
    }
    if((listen(listenfd, 5)) < 0)
    {
        printf("\nSERVER ERROR: Cannot listen.\n");
        close(listenfd);
        exit(1);
    }
    cli_addr_len = sizeof(cli_addr);
    while(1)
    {
        printf("\nSERVER: Waiting for client...\n");
        if((connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &cli_addr_len)) < 0)
        {
            printf("\nSERVER ERROR: Cannot accept client.\n");
            continue;
        }
        printf("\nConnected to %s\n", inet_ntoa(cli_addr.sin_addr));
        while(1)
        {
            r = read(connfd, buff, 128);
            if(r < 0)
            {
                printf("\nSERVER ERROR: Read failed.\n");
                break;
            }
            if(r == 0)
            {
                printf("\nClient disconnected.\n");
                break;
            }
            buff[r] = '\0';
            printf("CLIENT: %s", buff);
            if(strncmp(buff, "exit", 4) == 0)
            {
                printf("Client exited.\n");
                break;
            }
            if(sscanf(buff, "%f %c %f", &num1, &op, &num2) != 3)
            {
                strcpy(response, "Invalid format");
            }
            else
            {
                switch(op)
                {
                    case '+': result = num1 + num2; break;
                    case '-': result = num1 - num2; break;
                    case '*': result = num1 * num2; break;
                    case '/':
                        if(num2 == 0)
                        {
                            strcpy(response, "Error: Division by zero");
                            goto send;
                        }
                        result = num1 / num2;
                        break;
                    case '%':
                        if(num2 == 0)
                        {
                            strcpy(response, "Error: Modulo by zero");
                            goto send;
                        }
                        result = ((int)num1 % (int)num2);
                        break;
                    default:
                        strcpy(response, "Invalid operator");
                        goto send;
                }
                sprintf(response, "%.2f", result);
            }
            send:
            printf("SERVER RESULT: %s\n", response);
            w = write(connfd, response, strlen(response));
            if(w < 0)
            {
                printf("\nSERVER ERROR: Write failed.\n");
                break;
            }
        }
        close(connfd);
    }
    close(listenfd);
    return 0;
}
