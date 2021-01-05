#include <windows.h>
#include <stdio.h>
#include <Winsock2.h>

#pragma comment(lib, "WS2_32.lib")
struct data6
{
    unsigned int d4 : 6;
    unsigned int d3 : 6;
    unsigned int d2 : 6;
    unsigned int d1 : 6;
};
// 协议中加密部分使用的是base64方法
char con628(char c6);
void base64(char *dbuf, char *buf128, int len);
void sendemail(char *email, char *body);
int open_socket(struct sockaddr *addr);

int main()
{
    char email[] = "3072230851@qq.com";
    char body[] = "From: \"lucy\"<2643811975@qq.com>\r\n"
                  "To: \"dasiy\"<3072230851@qq.com>\r\n"
                  "Subject: Hello\r\n\r\n"
                  "Hello World, Hello Email!";
    sendemail(email, body);
    return 0;
}

char con628(char c6)
{
    char rtn = '\0';
    if (c6 < 26)
        rtn = c6 + 65;
    else if (c6 < 52)
        rtn = c6 + 71;
    else if (c6 < 62)
        rtn = c6 - 4;
    else if (c6 == 62)
        rtn = 43;
    else
        rtn = 47;
    return rtn;
}

// base64的实现
void base64(char *dbuf, char *buf128, int len)
{
    struct data6 *ddd = NULL;
    int i = 0;
    char buf[256] = {0};
    char *tmp = NULL;
    char cc = '\0';
    memset(buf, 0, 256);
    strcpy(buf, buf128);
    for (i = 1; i <= len / 3; i++)
    {
        tmp = buf + (i - 1) * 3;
        cc = tmp[2];
        tmp[2] = tmp[0];
        tmp[0] = cc;
        ddd = (struct data6 *)tmp;
        dbuf[(i - 1) * 4 + 0] = con628((unsigned int)ddd->d1);
        dbuf[(i - 1) * 4 + 1] = con628((unsigned int)ddd->d2);
        dbuf[(i - 1) * 4 + 2] = con628((unsigned int)ddd->d3);
        dbuf[(i - 1) * 4 + 3] = con628((unsigned int)ddd->d4);
    }
    if (len % 3 == 1)
    {
        tmp = buf + (i - 1) * 3;
        cc = tmp[2];
        tmp[2] = tmp[0];
        tmp[0] = cc;
        ddd = (struct data6 *)tmp;
        dbuf[(i - 1) * 4 + 0] = con628((unsigned int)ddd->d1);
        dbuf[(i - 1) * 4 + 1] = con628((unsigned int)ddd->d2);
        dbuf[(i - 1) * 4 + 2] = '=';
        dbuf[(i - 1) * 4 + 3] = '=';
    }
    if (len % 3 == 2)
    {
        tmp = buf + (i - 1) * 3;
        cc = tmp[2];
        tmp[2] = tmp[0];
        tmp[0] = cc;
        ddd = (struct data6 *)tmp;
        dbuf[(i - 1) * 4 + 0] = con628((unsigned int)ddd->d1);
        dbuf[(i - 1) * 4 + 1] = con628((unsigned int)ddd->d2);
        dbuf[(i - 1) * 4 + 2] = con628((unsigned int)ddd->d3);
        dbuf[(i - 1) * 4 + 3] = '=';
    }
    return;
}
// 发送邮件
void sendemail(char *email, char *body)
{
    int sockfd = 0;
    struct sockaddr_in their_addr = {0};
    char buf[1500] = {0};
    char rbuf[1500] = {0};
    char login[128] = {0};
    char pass[128] = {0};
    WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 2), &WSAData);
    memset(&their_addr, 0, sizeof(their_addr));
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(25);
    their_addr.sin_addr.s_addr = inet_addr("120.241.25.80"); //qq smtp 服务器
    // 连接邮件服务器，如果连接后没有响应，则2 秒后重新连接
    sockfd = open_socket((struct sockaddr *)&their_addr);
    memset(rbuf, 0, 1500);
    while (recv(sockfd, rbuf, 1500, 0) == 0)
    {
        printf("reconnect...\n");
        sockfd = open_socket((struct sockaddr *)&their_addr);
        memset(rbuf, 0, 1500);
    }

    printf("connect: %s\n", rbuf);

    // EHLO
    memset(buf, 0, 1500);
    sprintf(buf, "EHLO abcdefg-PC\r\n");
    send(sockfd, buf, strlen(buf), 0);
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    printf("EHLO: %s\n", rbuf);

    // AUTH LOGIN
    memset(buf, 0, 1500);
    sprintf(buf, "AUTH LOGIN\r\n");
    send(sockfd, buf, strlen(buf), 0);
    // printf("%s\n", buf);
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    printf("AUTH: %s\n", rbuf);

    // USER
    memset(buf, 0, 1500);
    sprintf(buf, "2643811975"); //你的qq号
    memset(login, 0, 128);
    base64(login, buf, strlen(buf));
    sprintf(buf, "%s\r\n", login);
    send(sockfd, buf, strlen(buf), 0);
    // printf("%s\n", buf);
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    printf("user: %s\n", rbuf);

    // PASSWORD
    sprintf(buf, "bpgblkyafitiebga"); //使用授权码
    memset(pass, 0, 128);
    base64(pass, buf, strlen(buf));
    sprintf(buf, "%s\r\n", pass);
    send(sockfd, buf, strlen(buf), 0);
    // printf("%s\n", buf);

    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    printf("password: %s\n", rbuf);

    // MAIL FROM
    memset(buf, 0, 1500);
    sprintf(buf, "MAIL FROM: <2643811975@qq.com>\r\n");
    send(sockfd, buf, strlen(buf), 0);
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    printf("mail from: %s\n", rbuf);

    // RCPT TO 第一个收件人
    sprintf(buf, "RCPT TO:<%s>\r\n", email);
    send(sockfd, buf, strlen(buf), 0);
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    printf("mail to: %s\n", rbuf);

    // DATA 准备开始发送邮件内容
    sprintf(buf, "DATA\r\n");
    send(sockfd, buf, strlen(buf), 0);
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    printf("data begin: %s\n", rbuf);

    // 发送邮件内容，\r\n.\r\n内容结束标记
    sprintf(buf, "%s\r\n.\r\n", body);
    send(sockfd, buf, strlen(buf), 0);
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    printf("data end: %s\n", rbuf);

    // QUIT
    sprintf(buf, "QUIT\r\n");
    send(sockfd, buf, strlen(buf), 0);
    memset(rbuf, 0, 1500);
    recv(sockfd, rbuf, 1500, 0);
    printf("quit: %s\n", rbuf);

    // VC2005 需要使用
    closesocket(sockfd);
    WSACleanup();
    return 0;
}
// 打开TCP Socket连接
int open_socket(struct sockaddr *addr)
{
    int sockfd = 0;
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        fprintf(stderr, "Open sockfd(TCP) error!\n");
        exit(-1);
    }
    if (connect(sockfd, addr, sizeof(struct sockaddr)) < 0)
    {
        fprintf(stderr, "Connect sockfd(TCP) error!\n");
        exit(-1);
    }
    return sockfd;
}