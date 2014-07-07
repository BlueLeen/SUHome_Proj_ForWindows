// ClientAdb.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ClientAdb.h"
#include "sockcomm.h"
#include "RegTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define ADB_TCP_SERVER_PORT 7100
#define SOCKET_SELECT_OPTION "Please select options: 1.send packets 2.receive packets 3.exit!\n"
#define SOCKET_SEND_COMMENTS "Edit the packet--The packet format is \"code content\" Stop send packet write 0\n"
#define SOCKET_RECV_COMMENTS "Start receive the packete(If want to stop the receive,please send packet'code equal 0 or 90)--The packet is:\n"
// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

void GetModulePath(LPTSTR lpszPath)
{
	TCHAR szPath[MAX_PATH];
	::GetModuleFileName(AfxGetInstanceHandle(), szPath, MAX_PATH);
	size_t nLen = lstrlen(szPath);
	for (size_t i = nLen - 1; i >= 0; i--)
	{
		if (szPath[i] == _T('\\'))
		{
			szPath[i] = 0;
			break;
		}
	}
	_tcscpy(lpszPath, szPath);
}

int extract_pack(void* buf)
{
	char szLog[200] = { 0 };
	unsigned int len = ntohl(*(int*)buf);
	unsigned int code = ntohl(*(int*)((unsigned char*)buf+4));
	char szContent[200] = { 0 };
	//if(code == 104)
	//	return len;
	if(len > 8)
	{
		memcpy(szContent, (unsigned char*)buf+8, len-8);
		//parse_code(code, szContent, global_client_sockfd);
		sprintf(szLog, "The receive package is:%d %d %s", len,
			code, szContent);
		printf("%s\n", szLog);
	}
	else
	{
		sprintf(szLog, "The receive package is:%d %d,the content is:no!", len,
			code);
		printf("%s\n", szLog);
	}
	return len;
}


DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	int sockFd = (int)lpParameter;
	char buf[BUFSIZ] = { 0 };
	while(true)
	{
		receive_socket_packs(sockFd, buf, BUFSIZ);
		extract_pack(buf);
	}
	return 0;
}


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// 初始化 MFC 并在失败时显示错误
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: MFC 初始化失败\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: 在此处为应用程序的行为编写代码。
		TCHAR szPath[MAX_PATH];
		CString csIpAddr;
		GetModulePath(szPath);
		_stprintf(szPath, _T("%s\\%s"), szPath, _T("setting.ini"));
		RegTool::GetPrivateProfileString(_T("info"), _T("svrip"), csIpAddr, szPath);
		CStringA csaIpAddr(csIpAddr);
		int sockFd;
		start_client_socket(ADB_TCP_SERVER_PORT, &sockFd, csaIpAddr.GetBuffer(0));

		char buf[BUFSIZ] = { 0 };
		char szContent[100] = { 0 };
		receive_socket_packs(sockFd, buf, BUFSIZ);
		int packSize = ntohl(*(int*)buf);
		int code = ntohl(*(int*)(buf + 4));
		memcpy(szContent, buf+8, packSize-8);
		printf("Receive package size:%d, code:%d, content:%s\n", packSize, code, szContent);
		while(TRUE)
		{
			int nOption = 0;
			printf(SOCKET_SELECT_OPTION);
			scanf("%d", &nOption);
			if (nOption == 1)
			{
				HANDLE hThread = CreateThread(NULL, 0, ThreadProc, (LPVOID)sockFd, CREATE_SUSPENDED, NULL);
				ResumeThread(hThread);
				CloseHandle(hThread);
				while(TRUE)
				{
					printf(SOCKET_SEND_COMMENTS);
					int len;
					int code;
					char content[200] = { 0 };
					scanf("%d %s", &code, content);
					*(int*)(buf+4) = htonl(code);
					memcpy(buf+8, content, strlen(content)+1);
					len = 8+strlen(content)+1;
					*(int*)buf = htonl(len);
					send_socket_packs(sockFd, buf, len);//发送欢迎信息
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
					len = receive_socket_packs(sockFd, buf, BUFSIZ);//发送欢迎信息
					code = ntohl(*(int*)(buf+4));
					memcpy(content, buf+8, len-8);
					if (code == 0 || code ==90)
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
		close_client_socket(sockFd);
	}
	printf("The communication is\n");
	return nRetCode;
}
