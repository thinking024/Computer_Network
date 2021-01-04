#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "WS2_32.Lib")

int main()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(1, 1);

    if (WSAStartup(wVersionRequested, &wsaData) != 0)
    {
        return -1;
    }
    if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
    {
        WSACleanup();
        return -1;
    }
    SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);
    if (sockSrv == -1)
    {
        printf("fail to create clienter socket\n");
        return -1;
    }

    SOCKADDR_IN addrSrv;
    addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(6000);

    // 建立套接字与通信地址的联系
    if (bind(sockSrv, (SOCKADDR *)&addrSrv, sizeof(SOCKADDR)) == -1)
    {
        printf("fail to bind\n");
        return -1;
    }
    // 监听客户端连接请求
    if (listen(sockSrv, 5) == -1)
    {
        printf("fail to listen\n");
        return -1;
    }

    SOCKADDR_IN addrClient;
    int len = sizeof(SOCKADDR);

    while (1)
    {
        SOCKET sockConn = accept(sockSrv, (SOCKADDR *)&addrClient, &len); // 接受请求
        if (sockConn == -1)
        {
            printf("fail to accept connection\n");
            return -1;
        }
        char* remoteAddr = inet_ntoa(addrClient.sin_addr);
        printf("connnect to %s\n", remoteAddr);

        char recvBuf[50];
        recv(sockConn, recvBuf, 50, 0); // 接收客户端发来的信息
        printf("receive: %s\n", recvBuf);

        // 向目标网址发请求
        int num;
        SOCKET s;
        WSADATA wsa;
        struct sockaddr_in client;
        char sndBuf[1024], rcvBuf[2048];
        
        memset(&client, 0, sizeof(client));
        memset(sndBuf, 0, 1024);
        memset(rcvBuf, 0, 2048);

        WSAStartup(MAKEWORD(2, 1), &wsa);
        if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("socket error!");
            exit(1);
        }

        u_long ip = inet_addr(recvBuf);
        //转换不成功时按域名解析
        if (ip == INADDR_NONE)
        {
            HOSTENT *pHostent = gethostbyname(recvBuf);
            if (pHostent)
            {
                ip = (*(struct in_addr *)pHostent->h_addr).s_addr;
            }
            else
            {
                printf("error");
                WSACleanup();
                return -1;
            }
        }

        client.sin_family = AF_INET;
        client.sin_port = htons(80);
        client.sin_addr.S_un.S_addr = ip;
        if ((connect(s, (struct sockaddr *)&client, sizeof(client))) < 0)
        {
            perror("connet error!");
            exit(1);
        }

        strcat(sndBuf, "GET / HTTP/1.0\r\n");
        strcat(sndBuf, "\r\n");

        if ((num = send(s, sndBuf, 1024, 0)) < 0)
        {
            perror("send error!");
            exit(1);
        }
        puts("send success!\n");
        do
        {
            if ((num = recv(s, rcvBuf, 2048, 0)) < 0)
            {
                perror("recv error!");
                exit(1);
            }
            else if (num > 0)
            {
                send(sockConn, rcvBuf, strlen(rcvBuf) + 1, 0); // 向客户端发送信息
                memset(rcvBuf, 0, 2048);
            }
        } while (num > 0);

        puts("\nread success!\n");
        send(sockConn, "#", 2, 0); // 结束通信
        closesocket(s);
        WSACleanup();
        closesocket(sockConn);
    }
    WSACleanup();
    return 0;
}