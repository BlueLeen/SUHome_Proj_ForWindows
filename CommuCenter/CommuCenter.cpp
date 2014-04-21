// CommuCenter.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "CommuCenter.h"
#include "sockcomm.h"
#include "FileOpp.h"
#include  <afxpriv.h>

#define MAX_LOADSTRING 100
#define SOCKET_START_TOKEN  "Start-Leen:Welcome to my server\n"
#define APK_CFG_DIR		    _T("apkdir")

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);


DWORD WINAPI ThreadProc(LPVOID lpParameter);
DWORD WINAPI ThreadSend(LPVOID lpParameter);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	MSG msg;
	//HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_COMMUCENTER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	//hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_COMMUCENTER));

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		//if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释:
//
//    仅当希望
//    此代码与添加到 Windows 95 中的“RegisterClassEx”
//    函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//    这样应用程序就可以获得关联的
//    “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_COMMUCENTER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_CLIPCHILDREN | WS_POPUP,
      0, 0, 10, 10, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   HANDLE hThread = CreateThread(NULL, 0, ThreadProc, NULL, CREATE_SUSPENDED, NULL);
   SetThreadPriority(hThread, REALTIME_PRIORITY_CLASS);
   ResumeThread(hThread);
   CloseHandle(hThread);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		//PostQuitMessage(0);
		break;
	case WM_CLOSE:
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	SOCKET sockSrv;
	start_server_socket(sockSrv, 8000);
	//wait for the client to connect
	while(TRUE)
	{
		SOCKET sockClt;
		char buf[BUFSIZ];  //数据传送的缓冲区  
		int len = 0;
		accept_client_socket(sockClt, sockSrv);
		try
		{
			if (sockClt != INVALID_SOCKET)
			{
				printf("%s\n", buf);
				long long ulTime = _atoi64(buf);
				printf("%lld\n", ulTime);
				receive_socket_packs(sockClt, buf,BUFSIZ);
				if (buf[0]==_T('S') && buf[10]==_T('s'))
				{
					send_socket_packs(sockClt, SOCKET_START_TOKEN, sizeof(SOCKET_START_TOKEN));//发送欢迎信息  
					HANDLE hThread = CreateThread(NULL, 0, ThreadSend, (LPVOID)sockClt, CREATE_SUSPENDED, NULL);
					ResumeThread(hThread);
					WaitForSingleObject(hThread, INFINITE);
					CloseHandle(hThread);
					//while((len=receive_socket_packs(sockClt, buf,BUFSIZ))>0)
					//{   
					//	printf("%s\n",buf); 
					//	if(send_socket_packs(sockClt,buf,len)<0)  
					//	{  
					//		perror("write");  
					//		return 1;  
					//	}  
					//}
				}
			}
			close_client_socket(sockClt);
		}
		catch (...)
		{
			close_client_socket(sockClt);
		}
	}
	close_server_socket(sockSrv);
	return 0;
}

DWORD WINAPI ThreadSend(LPVOID lpParameter)
{
	TCHAR szFile[MAX_PACKAGE_NAME];
	CString csValue;
	long long dirTime;
	long long dirOldTime;
	char buf[BUFSIZ];  //数据传送的缓冲区 
	int len = 0;
	SOCKET sockClient = (SOCKET)lpParameter;
	_stprintf(szFile, _T("%s\\%s"), APK_PATH_PARENT, APK_CFG_NAME);
	RegTool::GetPrivateProfileString(APK_CFG_INFO, APK_CFG_DIR, csValue, szFile);
	dirTime = _tstoi64(csValue.GetBuffer());
	receive_socket_packs(sockClient, buf, BUFSIZ);
	dirOldTime = _atoi64(buf);
	if (dirTime == dirOldTime)
		printf("%s\n", buf);
	else
	{
		strcpy(buf, "-up");
		send_socket_packs(sockClient, buf, strlen(buf));
		CStringArray apkArray;
		NeedUpdateApks(apkArray);
		for (int i = 0; i < apkArray.GetSize(); i++)
		{
			CString csFileName;
			CFile file;
			//int fileLength = 0;
			int readLen = BUFSIZ;
			int sendFileLength = 0;
			csFileName.Format(_T("%s\\%s.apk"), APK_PATH, apkArray[i]);
			file.Open(csFileName, CFile::modeRead | CFile::typeBinary);
			//fileLength = file.GetLength();
			USES_CONVERSION;
			char* szFile = T2A(apkArray[i]);
			sprintf(buf, "s %s", szFile);
			send_socket_packs(sockClient, buf, strlen(buf));
			while (readLen)
			{
				file.Seek(sendFileLength, CFile::begin);
				int readLen = file.Read(buf, BUFSIZ);
				if (send_socket_packs(sockClient, buf, readLen)<0)
				{
					perror("write");
					return 1;
				}
				sendFileLength += readLen;
			}
			sprintf(buf, "e %s", szFile);
			send_socket_packs(sockClient, buf, strlen(buf));
		}
	}
	//while((len=receive_socket_packs(sockClient, buf,BUFSIZ))>0)
	//{   
	//	if(send_socket_packs(sockClient,buf,len)<0)  
	//	{  
	//		perror("write");  
	//		return 1;  
	//	}  
	//}
	return 0;
}

//DWORD WINAPI ThreadProc(LPVOID lpParameter)
//{
//	SOCKET sockSrv;
//	start_server_socket(sockSrv, 8000);
//	while(TRUE)
//	{
//		SOCKET sockClt;
//		char buf[BUFSIZ];  //数据传送的缓冲区  
//		int len = 0;
//		accept_client_socket(sockClt, sockSrv);
//		try
//		{
//			if (sockClt != INVALID_SOCKET)
//			{
//				printf("%s\n", buf);
//				long long ulTime = _atoi64(buf);
//				printf("%lld\n", ulTime);
//				receive_socket_packs(sockSrv, buf,BUFSIZ);
//				send_socket_packs(sockClt, "Welcome to my server\n", 21);//发送欢迎信息  
//				while((len=receive_socket_packs(sockClt, buf,BUFSIZ))>0)
//				{   
//					printf("%s\n",buf); 
//					if(send_socket_packs(sockClt,buf,len)<0)  
//					{  
//						perror("write");  
//						return 1;  
//					}  
//				}
//			}
//			close_client_socket(sockClt);
//		}
//		catch (...)
//		{
//			close_client_socket(sockClt);
//		}
//	}
//	close_server_socket(sockSrv);
//	return 0;
//}
