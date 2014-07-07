#pragma once

#include <WinSock2.h>
#include <io.h>

int start_server_socket(SOCKET& sockSrvfd, int nConnPort)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int nReg = 0;  
	struct sockaddr_in my_addr;   //服务器网络地址结构体  
	memset(&my_addr,0,sizeof(my_addr)); //数据初始化--清零  

	wVersionRequested = MAKEWORD(1, 1);     //请求.1版本socket  
	nReg = WSAStartup(wVersionRequested,&wsaData);//初始化  
	if (0 != nReg)  
	{  
		printf("err code :%d",WSAGetLastError());  
		return 0;  
	}  
	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)  
	{  
		printf("VERSION DIFF");  
		WSACleanup();  
		return 0;  
	} 
	my_addr.sin_family=AF_INET; //设置为IP通信  
	my_addr.sin_addr.s_addr=INADDR_ANY;//服务器IP地址--允许连接到所有本地地址上  
	my_addr.sin_port=htons(nConnPort); //服务器端口号  
	/*创建服务器端套接字--IPv4协议，面向连接通信，TCP协议*/  
	if((sockSrvfd=socket(PF_INET,SOCK_STREAM,0))<0)  
	{  
		perror("socket");  
		return 0;  
	}  
	/*将套接字绑定到服务器的网络地址上*/  
	if (bind(sockSrvfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))<0)  
	{  
		perror("bind");  
		return 0;  
	}  
	/*监听连接请求--监听队列长度为5*/  
	listen(sockSrvfd,5);  
	return sockSrvfd;
}

int start_client_socket(int nConnPort, int* pnClientSockfd, char* szServerIp)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(1, 1);     //请求.1版本socket  
	int nReg = WSAStartup(wVersionRequested, &wsaData);//初始化  
	if (0 != nReg)
	{
		printf("err code :%d", WSAGetLastError());
		return 0;
	}
	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
	{
		printf("VERSION DIFF");
		WSACleanup();
		return 0;
	}
	struct sockaddr_in remote_addr; //服务器端网络地址结构体
	memset(&remote_addr, 0, sizeof(remote_addr)); //数据初始化--清零
	remote_addr.sin_family = AF_INET;
	//remote_addr.sin_addr.s_addr=inet_addr(szRemoteSrvAddr);
	//remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	remote_addr.sin_addr.s_addr = inet_addr(szServerIp);
	remote_addr.sin_port = htons(nConnPort);
	/*创建客户端套接字--IPv4协议，面向连接通信，TCP协议*/
	if ((*pnClientSockfd = socket(PF_INET, SOCK_STREAM, 0))<0)
	{
		perror("socket");
		return 0;
	}
	/*将套接字绑定到服务器的网络地址上*/
	//while表示如果连接服务器不成功一直尝试进行连接
	while (connect(*pnClientSockfd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr))<0)
	{
		Sleep(2);
		perror("connect");
		//return 0;
	}
	return *pnClientSockfd;
}

int accept_client_socket(SOCKET& sockCltfd, const SOCKET& sockSrvfd)
{
	struct sockaddr_in remote_addr; //客户端网络地址结构体  
	int sin_size;
	sin_size=sizeof(struct sockaddr_in);  
	if((sockCltfd=accept(sockSrvfd,(struct sockaddr *)&remote_addr,&sin_size))<0)  
	{  
		perror("accept");  
		return 0;  
	}  
	printf("accept client %s\n",inet_ntoa(remote_addr.sin_addr));
	return sockCltfd;
}

int send_socket_packs(const SOCKET& sockCltfd, char* szBuf, int nSize)
{
	int len = send(sockCltfd, szBuf, nSize,0);
	return len;
}

int receive_socket_packs(const SOCKET& sockCltfd, char* szBuf, int nSize)
{
	int len=recv(sockCltfd, szBuf, nSize, 0);
	szBuf[len] = '\0';
	return len;
}

void close_client_socket(const SOCKET& sockCltfd)
{

	closesocket(sockCltfd);
}

void close_server_socket(const SOCKET& sockSrvfd)
{
	closesocket(sockSrvfd);
	WSACleanup();//初始化关闭  
}