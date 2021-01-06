#include <windows.h>
#include <stdio.h>
#include <Winsock2.h>
#include <string.h>

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

int main()
{
    //sendemail(email, body);

    struct sockaddr_in smtpServer = {0};
    char sendBuffer[1500] = {0};
    char rcvBuffer[1500] = {0};
    WSADATA WSAData;
    if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
    {
        fprintf(stderr, "Init error!\n");
        exit(-1);
    }
    
    memset(&smtpServer, 0, sizeof(smtpServer));
    smtpServer.sin_family = AF_INET;
    smtpServer.sin_port = htons(25);
    HOSTENT* serverInfo = gethostbyname("smtp.qq.com");
    smtpServer.sin_addr.s_addr = *((DWORD*)serverInfo->h_addr_list[0]); //qq smtp 服务器

    int mailSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (mailSocket < 0)
    {
        fprintf(stderr, "Open socket(TCP) error!\n");
        exit(-1);
    }
    if (connect(mailSocket, (struct sockaddr *)&smtpServer, sizeof(struct sockaddr)) < 0)
    {
        fprintf(stderr, "Connect socket(TCP) error!\n");
        exit(-1);
    }
    memset(rcvBuffer, 0, 1500);
    if (recv(mailSocket, rcvBuffer, 1500, 0) == 0)
    {
        fprintf(stderr, "fail to receive!\n");
        exit(-1);
    }

    printf("connect: %s\n", rcvBuffer);

    // EHLO
    char localHost[20] = {0};
    gethostname(localHost, 20);
    memset(sendBuffer, 0, 1500);
    strcat(localHost, "-PC\r\n");
    char ehlo[1500] = {0};
    strcat(ehlo, "EHLO ");
    sprintf(sendBuffer, strcat(ehlo,localHost));
    // printf("%s\n", sendBuffer);

    send(mailSocket, sendBuffer, strlen(sendBuffer), 0);
    memset(rcvBuffer, 0, 1500);
    recv(mailSocket, rcvBuffer, 1500, 0);
    printf("EHLO: %s\n", rcvBuffer);

    // AUTH LOGIN
    memset(sendBuffer, 0, 1500);
    sprintf(sendBuffer, "AUTH LOGIN\r\n");
    send(mailSocket, sendBuffer, strlen(sendBuffer), 0);
    // printf("%s\n", buf);
    memset(rcvBuffer, 0, 1500);
    recv(mailSocket, rcvBuffer, 1500, 0);
    printf("AUTH: %s\n", rcvBuffer);

    // USER
    char emailFrom[128];
    printf("input your qq: ");
    scanf("%s", emailFrom);
    char qq[128] = {0};
    strcpy(qq, emailFrom);
    memset(sendBuffer, 0, 1500);
    sprintf(sendBuffer, emailFrom); //你的qq号
    memset(emailFrom, 0, sizeof(emailFrom));
    base64(emailFrom, sendBuffer, strlen(sendBuffer));
    sprintf(sendBuffer, "%s\r\n", emailFrom);
    send(mailSocket, sendBuffer, strlen(sendBuffer), 0);
    // printf("%s\n", buf);
    memset(rcvBuffer, 0, sizeof(rcvBuffer));
    recv(mailSocket, rcvBuffer, 1500, 0);
    printf("user: %s\n", rcvBuffer);

    // PASSWORD
    char password[128];
    printf("input your auth code: ");
    scanf("%s", password);

    // "bpgblkyafitiebga"
    sprintf(sendBuffer, password); //使用授权码
    memset(password, 0, sizeof(password));
    base64(password, sendBuffer, strlen(sendBuffer));
    sprintf(sendBuffer, "%s\r\n", password);
    send(mailSocket, sendBuffer, strlen(sendBuffer), 0);
    // printf("%s\n", buf);
    memset(rcvBuffer, 0, 1500);
    recv(mailSocket, rcvBuffer, 1500, 0);
    printf("password: %s\n", rcvBuffer);

    // MAIL FROM
    memset(sendBuffer, 0, 1500);
    sprintf(sendBuffer, "MAIL FROM: <%s@qq.com>\r\n", qq);
    send(mailSocket, sendBuffer, strlen(sendBuffer), 0);
    memset(rcvBuffer, 0, 1500);
    recv(mailSocket, rcvBuffer, 1500, 0);
    printf("mail from: %s\n", rcvBuffer);


    // RCPT TO
    char emailTo[50];
    printf("input receive email address, split with space, ends with #: ");
    scanf("%s", emailTo);

    while (strcmp("#", emailTo) != 0)
    {
        sprintf(sendBuffer, "RCPT TO:<%s>\r\n", emailTo);
        send(mailSocket, sendBuffer, strlen(sendBuffer), 0);
        memset(rcvBuffer, 0, 1500);
        recv(mailSocket, rcvBuffer, 1500, 0);
        printf("mail to: %s\n", rcvBuffer);
        scanf("%s", emailTo);
    }
    

    // DATA 准备开始发送邮件内容
    sprintf(sendBuffer, "DATA\r\n");
    send(mailSocket, sendBuffer, strlen(sendBuffer), 0);
    memset(rcvBuffer, 0, 1500);
    recv(mailSocket, rcvBuffer, 1500, 0);
    printf("data begin: %s\n", rcvBuffer);

    char subject[50];
    printf("input your email subject: ");
    getchar();
    //scanf("%s", subject);
    gets(subject);

    char content[500];
    printf("input your email content: ");
    //scanf("%s", content);
    gets(content);

    char body[] = "From: \"lucy\"<2643811975@qq.com>\r\n"
                  "To: \"dasiy\"<3072230851@qq.com>\r\n"
                  "Subject: Hello\r\n\r\n"
                  "Hello World, Hello Email!";
    
    char from[200] = {0};
    strcat(from, "From: \"");
    strcat(from, qq);
    strcat(from, "\"<");
    strcat(from, qq);
    strcat(from, "@qq.com>\r\n");

    char emailBody[10000] = {0};
    //strcat(emailBody, from);
    strcat(emailBody, "Subject: ");
    strcat(emailBody, subject);
    strcat(emailBody, "\r\n\r\n");
    strcat(emailBody, content);

    // 发送邮件内容，\r\n.\r\n内容结束标记
    sprintf(sendBuffer, "%s\r\n.\r\n", emailBody);
    send(mailSocket, sendBuffer, strlen(sendBuffer), 0);
    memset(rcvBuffer, 0, 1500);
    recv(mailSocket, rcvBuffer, 1500, 0);
    printf("data end: %s\n", rcvBuffer);

    // QUIT
    sprintf(sendBuffer, "QUIT\r\n");
    send(mailSocket, sendBuffer, strlen(sendBuffer), 0);
    memset(rcvBuffer, 0, 1500);
    recv(mailSocket, rcvBuffer, 1500, 0);
    printf("quit: %s\n", rcvBuffer);

    // VC2005 需要使用
    closesocket(mailSocket);
    WSACleanup();
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
    struct sockaddr_in smtpServer = {0};
    char sendBuffer[1500] = {0};
    char rcvBuffer[1500] = {0};
    char account[128] = {0};
    char pass[128] = {0};
    WSADATA WSAData;
    if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
    {
        fprintf(stderr, "Init error!\n");
        exit(-1);
    }
    
    memset(&smtpServer, 0, sizeof(smtpServer));
    smtpServer.sin_family = AF_INET;
    smtpServer.sin_port = htons(25);
    //smtpServer.sin_addr.s_addr = inet_addr("120.241.25.80"); //qq smtp 服务器
    HOSTENT* serverInfo = gethostbyname("smtp.qq.com");
    smtpServer.sin_addr.s_addr = *((DWORD*)serverInfo->h_addr_list[0]); //qq smtp 服务器

    int mailSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (mailSocket < 0)
    {
        fprintf(stderr, "Open socket(TCP) error!\n");
        exit(-1);
    }
    if (connect(mailSocket, (struct sockaddr *)&smtpServer, sizeof(struct sockaddr)) < 0)
    {
        fprintf(stderr, "Connect socket(TCP) error!\n");
        exit(-1);
    }
    memset(rcvBuffer, 0, 1500);
    if (recv(mailSocket, rcvBuffer, 1500, 0) == 0)
    {
        fprintf(stderr, "fail to receive!\n");
        exit(-1);
    }

    printf("connect: %s\n", rcvBuffer);

    // EHLO
    char localHost[20] = {0};
    gethostname(localHost, 20);
    memset(sendBuffer, 0, 1500);
    strcat(localHost, "-PC\r\n");
    char ehlo[1500] = {0};
    strcat(ehlo, "EHLO ");
    sprintf(sendBuffer, strcat(ehlo,localHost));
    printf("%s\n", sendBuffer);
    send(mailSocket, sendBuffer, strlen(sendBuffer), 0);
    memset(rcvBuffer, 0, 1500);
    recv(mailSocket, rcvBuffer, 1500, 0);
    printf("EHLO: %s\n", rcvBuffer);

    // AUTH LOGIN
    memset(sendBuffer, 0, 1500);
    sprintf(sendBuffer, "AUTH LOGIN\r\n");
    send(mailSocket, sendBuffer, strlen(sendBuffer), 0);
    // printf("%s\n", buf);
    memset(rcvBuffer, 0, 1500);
    recv(mailSocket, rcvBuffer, 1500, 0);
    printf("AUTH: %s\n", rcvBuffer);

    // USER
    memset(sendBuffer, 0, 1500);
    sprintf(sendBuffer, "2643811975"); //你的qq号
    memset(account, 0, 128);
    base64(account, sendBuffer, strlen(sendBuffer));
    sprintf(sendBuffer, "%s\r\n", account);
    send(mailSocket, sendBuffer, strlen(sendBuffer), 0);
    // printf("%s\n", buf);
    memset(rcvBuffer, 0, 1500);
    recv(mailSocket, rcvBuffer, 1500, 0);
    printf("user: %s\n", rcvBuffer);

    // PASSWORD
    sprintf(sendBuffer, "bpgblkyafitiebga"); //使用授权码
    memset(pass, 0, 128);
    base64(pass, sendBuffer, strlen(sendBuffer));
    sprintf(sendBuffer, "%s\r\n", pass);
    send(mailSocket, sendBuffer, strlen(sendBuffer), 0);
    // printf("%s\n", buf);

    memset(rcvBuffer, 0, 1500);
    recv(mailSocket, rcvBuffer, 1500, 0);
    printf("password: %s\n", rcvBuffer);

    // MAIL FROM
    memset(sendBuffer, 0, 1500);
    sprintf(sendBuffer, "MAIL FROM: <2643811975@qq.com>\r\n");
    send(mailSocket, sendBuffer, strlen(sendBuffer), 0);
    memset(rcvBuffer, 0, 1500);
    recv(mailSocket, rcvBuffer, 1500, 0);
    printf("mail from: %s\n", rcvBuffer);

    // RCPT TO 第一个收件人
    sprintf(sendBuffer, "RCPT TO:<%s>\r\n", email);
    send(mailSocket, sendBuffer, strlen(sendBuffer), 0);
    memset(rcvBuffer, 0, 1500);
    recv(mailSocket, rcvBuffer, 1500, 0);
    printf("mail to: %s\n", rcvBuffer);

    // DATA 准备开始发送邮件内容
    sprintf(sendBuffer, "DATA\r\n");
    send(mailSocket, sendBuffer, strlen(sendBuffer), 0);
    memset(rcvBuffer, 0, 1500);
    recv(mailSocket, rcvBuffer, 1500, 0);
    printf("data begin: %s\n", rcvBuffer);

    // 发送邮件内容，\r\n.\r\n内容结束标记
    sprintf(sendBuffer, "%s\r\n.\r\n", body);
    send(mailSocket, sendBuffer, strlen(sendBuffer), 0);
    memset(rcvBuffer, 0, 1500);
    recv(mailSocket, rcvBuffer, 1500, 0);
    printf("data end: %s\n", rcvBuffer);

    // QUIT
    sprintf(sendBuffer, "QUIT\r\n");
    send(mailSocket, sendBuffer, strlen(sendBuffer), 0);
    memset(rcvBuffer, 0, 1500);
    recv(mailSocket, rcvBuffer, 1500, 0);
    printf("quit: %s\n", rcvBuffer);

    // VC2005 需要使用
    closesocket(mailSocket);
    WSACleanup();
    return;
}