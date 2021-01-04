#include <stdio.h>
#include <Winsock2.h>
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
    SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);
    if (sockClient == -1)
    {
        printf("fail to create server socket\n");
        return -1;
    }

    SOCKADDR_IN addrSrv;
    addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(6000);
    if (connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) < 0)
    {
        perror("connet error!\n");
        exit(1);
    }
    printf("connect success\n");
    char ip[50];
    scanf("%s", ip);
    send(sockClient, ip, 50, 0);
    while (1)
    {
        char recvBuf[2048];
        memset(recvBuf, 0, 2048);
        recv(sockClient, recvBuf, 2048, 0);
        if (strcmp(recvBuf, "#") == 0)
        {
            break;
        }
        printf("%s\n", recvBuf);
    }
    closesocket(sockClient);
    WSACleanup();
    return 0;
}