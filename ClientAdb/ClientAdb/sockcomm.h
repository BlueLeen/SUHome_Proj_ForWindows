#pragma once

#include <WinSock2.h>
#include <io.h>

int start_server_socket(SOCKET& sockSrvfd, int nConnPort)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int nReg = 0;  
	struct sockaddr_in my_addr;   //�����������ַ�ṹ��  
	memset(&my_addr,0,sizeof(my_addr)); //���ݳ�ʼ��--����  

	wVersionRequested = MAKEWORD(1, 1);     //����.1�汾socket  
	nReg = WSAStartup(wVersionRequested,&wsaData);//��ʼ��  
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
	my_addr.sin_family=AF_INET; //����ΪIPͨ��  
	my_addr.sin_addr.s_addr=INADDR_ANY;//������IP��ַ--�������ӵ����б��ص�ַ��  
	my_addr.sin_port=htons(nConnPort); //�������˿ں�  
	/*�������������׽���--IPv4Э�飬��������ͨ�ţ�TCPЭ��*/  
	if((sockSrvfd=socket(PF_INET,SOCK_STREAM,0))<0)  
	{  
		perror("socket");  
		return 0;  
	}  
	/*���׽��ְ󶨵��������������ַ��*/  
	if (bind(sockSrvfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))<0)  
	{  
		perror("bind");  
		return 0;  
	}  
	/*������������--�������г���Ϊ5*/  
	listen(sockSrvfd,5);  
	return sockSrvfd;
}

int start_client_socket(int nConnPort, int* pnClientSockfd, char* szServerIp)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(1, 1);     //����.1�汾socket  
	int nReg = WSAStartup(wVersionRequested, &wsaData);//��ʼ��  
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
	struct sockaddr_in remote_addr; //�������������ַ�ṹ��
	memset(&remote_addr, 0, sizeof(remote_addr)); //���ݳ�ʼ��--����
	remote_addr.sin_family = AF_INET;
	//remote_addr.sin_addr.s_addr=inet_addr(szRemoteSrvAddr);
	//remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	remote_addr.sin_addr.s_addr = inet_addr(szServerIp);
	remote_addr.sin_port = htons(nConnPort);
	/*�����ͻ����׽���--IPv4Э�飬��������ͨ�ţ�TCPЭ��*/
	if ((*pnClientSockfd = socket(PF_INET, SOCK_STREAM, 0))<0)
	{
		perror("socket");
		return 0;
	}
	/*���׽��ְ󶨵��������������ַ��*/
	//while��ʾ������ӷ��������ɹ�һֱ���Խ�������
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
	struct sockaddr_in remote_addr; //�ͻ��������ַ�ṹ��  
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
	WSACleanup();//��ʼ���ر�  
}