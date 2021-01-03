//http get 
#include<stdio.h>
#include<winsock2.h>
#include<windows.h>

#pragma comment(lib, "ws2_32.lib")
 
int main()
{
	int num;
	
	SOCKET s;
	WSADATA wsa;
	struct sockaddr_in serv;
	
	char sndBuf[1024], rcvBuf[2048];
 
	WSAStartup(MAKEWORD(2, 1), &wsa);
 
 
	if ((s = socket(AF_INET, SOCK_STREAM, 0))<0)
	{
		perror("socket error!");
		exit(1);
	}
 
	memset(&serv, 0, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_port = htons(80);
	serv.sin_addr.S_un.S_addr = inet_addr("120.226.2.175");
 
	if ((connect(s, (struct sockaddr *)&serv, sizeof(serv)))<0)
	{
		perror("connet error!");
		exit(1);
	}
 
	memset(sndBuf, 0, 1024);
	memset(rcvBuf, 0, 2048);
 
	strcat(sndBuf, "GET / HTTP/1.0\r\n");
	
	strcat(sndBuf, "\r\n");
	
	puts(sndBuf);
  
	if ((num = send(s,sndBuf,1024, 0))<0)
   {
		perror("send error!");
		exit(1);
	}
	
	puts("send success!\n");
 
	do{
 
		if ((num = recv(s, rcvBuf, 2048, 0))<0)
		{
			perror("recv error!");
			exit(1);
		}
		else if (num>0)
		{
			printf("%s", rcvBuf);
			memset(rcvBuf, 0, 2048);
		}
	} while (num>0);
	
	puts("\nread success!\n");
 
	closesocket(s);
	WSACleanup();
	
	return 0;
}