#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "WS2_32.Lib")

int msgContinue = 1;
char* remoteAddr;

DWORD WINAPI sendMsg(LPVOID sockConn)
{
    while(msgContinue){
        char sendBuf[50];
        memset(sendBuf, 0, 50);
        scanf("%s", sendBuf);
        // sprintf(sendBuf, "Welcome %s to here!", inet_ntoa(addrClient.sin_addr));
        send((SOCKET)sockConn, sendBuf, strlen(sendBuf) + 1, 0); // 向客户端发送信息
        if (strcmp(sendBuf, "#") == 0)
		{
			msgContinue = 0;
			break;
		}
    }
    return 0;
}

DWORD WINAPI receiveMsg(LPVOID sockConn)
{
    while(msgContinue){
        char recvBuf[50];
        memset(recvBuf, 0, 50);
        recv((SOCKET)sockConn, recvBuf, 50, 0); // 接收客户端发来的信息
        if(strcmp(recvBuf, "#") == 0)
        {
            msgContinue = 0;
            printf("exit\n");
            break;
        }
        printf("%s: %s\n", remoteAddr, recvBuf);
    }
    return 0;
}

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
        printf("fail to create server socket\n");
        return -1;
    }
    
    SOCKADDR_IN addrSrv;
    addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(6000);

    // 建立套接字与通信地址的联系
    if (bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) == -1)
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

    SOCKET sockConn = accept(sockSrv, (SOCKADDR*)&addrClient, &len); // 接受请求
    if (sockConn == -1)
    {
        printf("fail to accept connection\n");
        return -1;
    }

    remoteAddr = inet_ntoa(addrClient.sin_addr);
    printf("connnect to %s\n", remoteAddr);

    HANDLE sendHandle = CreateThread(NULL, 0, sendMsg, (LPVOID)sockConn, 0, NULL);
    if (sendHandle == NULL) 
    {
        printf("fail to create send thread\n");
        return -1;
    }
    
    HANDLE receiveHandle = CreateThread(NULL, 0, receiveMsg, (LPVOID)sockConn, 0, NULL);
    if (receiveHandle == NULL) 
    {
        printf("fail to create receive thread\n");
        return -1;
    }
    
    // while (1)
    // {
    //     char recvBuf[50];
    //     recv(sockConn, recvBuf, 50, 0); // 接收客户端发来的信息
    //     printf("%s\n", recvBuf);
    //     char sendBuf[50];
    //     scanf("%s", sendBuf);
    //     // sprintf(sendBuf, "Welcome %s to here!", inet_ntoa(addrClient.sin_addr));
    //     send(sockConn, sendBuf, strlen(sendBuf) + 1, 0); // 向客户端发送信息
    // }
    // closesocket(sockConn);
    while (msgContinue)
	{

	}
    closesocket(sockConn);
	WSACleanup();
	CloseHandle(sendHandle);
	CloseHandle(receiveHandle);
    return 0;
}