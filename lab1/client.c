#include <stdio.h>
#include <Winsock2.h>
#include <Windows.h>

#pragma comment(lib, "Ws2_32.lib")
int msgContinue = 1;
const char* remoteAddr = "10.1.12.131";

DWORD WINAPI sendMsg(LPVOID sockConn)
{
	while (msgContinue) {
		char sendBuf[50];
		memset(sendBuf, 0, 50);
		scanf("%s", sendBuf);
		// sprintf(sendBuf, "Welcome %s to here!", inet_ntoa(addrClient.sin_addr));
		send((SOCKET)sockConn, sendBuf, strlen(sendBuf) + 1, 0);
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
	while (msgContinue) {
		char recvBuf[50];
		memset(recvBuf, 0, 50);
		recv((SOCKET)sockConn, recvBuf, 50, 0);
		if (strcmp(recvBuf, "#") == 0)
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
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);
	if (sockClient == -1)
	{
		printf("fail to create socket\n");
		return -1;
	}

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr(remoteAddr);  // ip地址
	addrSrv.sin_family = AF_INET; // 地址族
	addrSrv.sin_port = htons(6000); // 端口号
	if (connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		printf("fail to connect\n");
		return -1;
	} // 连接

	HANDLE sendHandle = CreateThread(NULL, 0, sendMsg, (LPVOID)sockClient, 0, NULL);
	if (sendHandle == NULL)
	{
		printf("fail to create send thread\n");
		return -1;
	}

	HANDLE receiveHandle = CreateThread(NULL, 0, receiveMsg, (LPVOID)sockClient, 0, NULL);
	if (receiveHandle == NULL)
	{
		printf("fail to create receive thread\n");
		return -1;
	}
	while (msgContinue)
	{

	}
	closesocket(sockClient);
	WSACleanup();
	CloseHandle(sendHandle);
	CloseHandle(receiveHandle);
	return 0;
}