#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
struct sockaddr_in serv_addr, cli_addr;
int listenfd, connfd, r, w, cli_addr_len;
unsigned short serv_port = 25050;
char serv_ip[] = "127.0.0.1";
char buff[128];
void handle_exit(int sig)
{
    printf("\nServer exiting...\n");
    close(connfd); close(listenfd);
    exit(0);
}
void decToBinary(int n, char *res)
{
    char temp[64]; int i = 0;
    if(n == 0)
    {
        strcpy(res, "0");
        return;
    }
    while(n > 0)
    {
        temp[i++] = (n % 2) + '0';
        n /= 2;
    }
    int j = 0;
    while(i > 0)
        res[j++] = temp[--i];
    res[j] = '\0';
}
int binToDec(char *bin)
{
    int res = 0;
    while (*bin)
    {
        if(*bin == '0' || *bin == '1')
            res = (res << 1) + (*bin - '0');
        bin++;
    }
    return res;
}
double parseExpression(char **str);
double parseNumber(char **str)
{
    double num = 0;
    while(isdigit(**str))
    {
        num = num * 10 + (**str - '0');
        (*str)++;
    }
    return num;
}
double parseFactor(char **str)
{
    double result;
    if(**str == '(')
    {
        (*str)++;
        result = parseExpression(str);
        if(**str == ')')
            (*str)++;
    }
    else
        result = parseNumber(str);
    return result;
}
double parseTerm(char **str, int error)
{
    double result = parseFactor(str);
    while(**str == '' || **str == '/')
    {
        char op = *(str)++;
        double val = parseFactor(str);
        if(op == '')
            result *= val;
        else
        {
            if(val != 0)
                result /= val;
            else
                *error = 1;
        }
    }
    return result;
}
double parseExpression(char **str)
{
    int error = 0;
    double result = parseTerm(str, &error);
    while(**str == '+' || **str == '-')
    {
        char op = *(*str)++;
        double val = parseTerm(str, &error);
        if(op == '+')
            result += val;
        else
            result -= val;
    }
    return result;
}
int main()
{
    char response[128];
    signal(SIGINT, handle_exit);
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serv_port);
    inet_aton(serv_ip, (&serv_addr.sin_addr));
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(listenfd, 5);
    cli_addr_len = sizeof(cli_addr);
    printf("Server running on port %d...\n", serv_port);
    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &cli_addr_len);
        while(1)
        {
            r = read(connfd, buff, sizeof(buff));
            if(r <= 0)
                break;
            buff[r] = '\0';
            printf("CLIENT SENT: %s\n", buff);
            if(strncmp(buff, "exit", 4) == 0)
                break;
            if(buff[0] == '1')
            {
                int num; sscanf(buff, "1 %d", &num);
                decToBinary(num, response);
            }
            else if(buff[0] == '2')
            {
                char expr[100], *ptr = expr;
                sscanf(buff, "2 %[^\n]", expr);
                sprintf(response, "%.2f", parseExpression(&ptr));
            }
            else if(buff[0] == '3')
            {
                char bin[64]; sscanf(buff, "3 %s", bin);
                sprintf(response, "%d", binToDec(bin));
            }
            else
                strcpy(response, "Invalid Choice");
            printf("SERVER RESULT: %s\n", response);
            write(connfd, response, strlen(response));
        }
        close(connfd);
    }
    return 0;
}
