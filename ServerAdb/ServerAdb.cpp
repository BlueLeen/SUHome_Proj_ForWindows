// ServerAdb.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "ServerAdb.h"
#include "sockcomm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ADB_TCP_SERVER_PORT 7100
#define SOCKET_STATR_TOKEN "OK"
#define SOCKET_SELECT_OPTION "Please select options: 1.send packets 2.receive packets 3.exit!\n"
#define SOCKET_SEND_COMMENTS "Edit the packet--The packet format is \"code content\" Stop send packet write 0\n"
#define SOCKET_RECV_COMMENTS "Start receive the packete(If want to stop the receive,please send packet'code equal 0 or 90)--The packet is:\n"

// Ψһ��Ӧ�ó������

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// ��ʼ�� MFC ����ʧ��ʱ��ʾ����
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: ���Ĵ�������Է���������Ҫ
		_tprintf(_T("����: MFC ��ʼ��ʧ��\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: �ڴ˴�ΪӦ�ó������Ϊ��д���롣
		SOCKET sockSrv;
		SOCKET sockClt;
		start_server_socket(sockSrv, ADB_TCP_SERVER_PORT);
		//wait for the client to connect
		while(TRUE)
		{
			char buf[BUFSIZ] = { 0 };  //���ݴ��͵Ļ�����  
			int len = 0;
			accept_client_socket(sockClt, sockSrv);
			*(int*)(buf+4) = htonl(100);
			memcpy(buf+8, SOCKET_STATR_TOKEN, strlen(SOCKET_STATR_TOKEN)+1);
			*(int*)buf = htonl(8+strlen(SOCKET_STATR_TOKEN)+1);
			send_socket_packs(sockClt, buf, 8+strlen(SOCKET_STATR_TOKEN)+1);//���ͻ�ӭ��Ϣ 
			BOOL bExit = FALSE;
			while(!bExit)
			{
				int nOption = 0;
				printf(SOCKET_SELECT_OPTION);
				scanf("%d", &nOption);
				if (nOption == 1)
				{
					printf(SOCKET_SEND_COMMENTS);
					while (TRUE)
					{
						int len;
						int code;
						char content[200] = { 0 };
						scanf("%d %s", &code, content);
						*(int*)(buf+4) = htonl(code);
						memcpy(buf+8, content, strlen(content)+1);
						len = 8+strlen(content)+1;
						*(int*)buf = htonl(len);
						send_socket_packs(sockClt, buf, len);//���ͻ�ӭ��Ϣ
						printf("Send packets:%d %d %s\n", len, code, content);
						if (code == 0)
						{
							break;
						}
					}
				}
				else if (nOption == 2)
				{
					while(TRUE)
					{
						printf(SOCKET_RECV_COMMENTS);
						int len;
						int code;
						char content[200] = { 0 };
						len = receive_socket_packs(sockClt, buf, BUFSIZ);//���ͻ�ӭ��Ϣ
						code = ntohl(*(int*)(buf+4));
						memcpy(content, buf+8, len-8);
						if (code == 0 || code == 90)
						{
							break;
						}

						printf("\n>>>>Recv packets:%d %d %s\n\n", len, code, content);
					}
				}
				else
				{
					break;
				}
			}
			//try
			//{

			//}
			//catch (...)
			//{
			//	close_client_socket(sockClt);
			//}
		}
		close_client_socket(sockClt);
		close_server_socket(sockSrv);
	}

	return nRetCode;
}
