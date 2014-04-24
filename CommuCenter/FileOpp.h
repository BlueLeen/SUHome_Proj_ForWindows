#pragma once

#include "stdafx.h"
#include "RegTool.h"

#define APK_PATH_PARENT		_T("C:\\StrongUnion\\UserData\\Apknstaller")
#define APK_PATH			_T("C:\\StrongUnion\\UserData\\Apknstaller\\apk")
#define APK_CFG_NAME		_T("ApkCfg.ini")
#define APK_CFG_INFO		_T("INFO")

#define TINYSIZE			50

CString AttractFileName(CString csFileName)
{
	int nFind = csFileName.Find(_T("apk"));
	if (nFind > -1)
		return csFileName.Left(nFind-1);
	else
		return csFileName;
}

long long GetFileLastTime(CString csFileName, CString& csLastTime)
{
	//CString strCreateTime, strModifyTime, strAccessTime;
	//FILETIME ftCreate, ftModify, ftAccess;
	//FILETIME ftLocalCreate, ftLocalModify, ftLocalAccess;
	TCHAR strFilePath[MAX_PATH];
	CString csValue;
	FILETIME ftModify, ftLocalModify;
	_stprintf(strFilePath, _T("%s\\%s.apk"), APK_PATH, csFileName);
	HANDLE hFile = CreateFile(strFilePath, GENERIC_READ,          // open for reading
		FILE_SHARE_READ,       // share for reading
		NULL,                            // default security
		OPEN_EXISTING,          // existing file only
		FILE_FLAG_BACKUP_SEMANTICS , // normal file
		NULL);

	SYSTEMTIME stLocal;
	if (!GetFileTime(hFile, NULL, NULL, &ftModify))
	{
		return 0;
	}
	//ZeroMemory(&stLocal, sizeof(SYSTEMTIME));
	//FileTimeToLocalFileTime(&ftCreate, &ftLocalCreate);
	//FileTimeToSystemTime(&ftLocalCreate, &stLocal);
	//strCreateTime.Format("%04d-%02d-%02d %02d:%02d:%02d", stLocal.wYear, stLocal.wMonth, stLocal.wDay,					stLocal.wHour, stLocal.wMinute, stLocal.wSecond);   // 文件创建时间
	//ZeroMemory(&stLocal, sizeof(SYSTEMTIME));
	//FileTimeToLocalFileTime(&ftModify, &ftLocalModify);
	//FileTimeToSystemTime(&ftLocalModify, &stLocal);
	//strModifyTime.Format("%04d-%02d-%02d %02d:%02d:%02d", stLocal.wYear, stLocal.wMonth, stLocal.wDay,					stLocal.wHour, stLocal.wMinute, stLocal.wSecond); //  文件修改时间
	//ZeroMemory(&stLocal, sizeof(SYSTEMTIME));
	//FileTimeToLocalFileTime(&ftAccess, &ftLocalAccess);
	//FileTimeToSystemTime(&ftLocalAccess, &stLocal);
	//strAccessTime.Format("%04d-%02d-%02d %02d:%02d:%02d", stLocal.wYear, stLocal.wMonth, stLocal.wDay,					stLocal.wHour, stLocal.wMinute, stLocal.wSecond); // 文件访问时间

	ZeroMemory(&stLocal, sizeof(SYSTEMTIME));
	FileTimeToLocalFileTime(&ftModify, &ftLocalModify);
	FileTimeToSystemTime(&ftLocalModify, &stLocal);
	csLastTime.Format(_T("%04d%02d%02d%02d%02d%02d"), stLocal.wYear, stLocal.wMonth, stLocal.wDay,					stLocal.wHour, stLocal.wMinute, stLocal.wSecond); //  文件修改时间

	return _tstoi64(csLastTime.GetBuffer());
}

long long GetFileLastTime(CString csFileName)
{
	CString csLastTime;
	return GetFileLastTime(csFileName, csLastTime);
}

long long GetApkModifyTime(CString csFileName)
{
	TCHAR strFilePath[MAX_PATH];
	CString csValue;
	_stprintf(strFilePath, _T("%s\\%s"), APK_PATH_PARENT, APK_CFG_NAME);
	RegTool::GetPrivateProfileString(APK_CFG_INFO, csFileName, csValue, strFilePath);
	return _tstoi64(csValue.GetBuffer());
}

long long SetApkModifyTime(CString csFileName)
{
	CString csLastTime;
	TCHAR strFilePath[MAX_PATH];
	long long time = GetFileLastTime(csFileName, csLastTime);
	_stprintf(strFilePath, _T("%s\\%s"), APK_PATH_PARENT, APK_CFG_NAME);
	RegTool::WritePrivateProfileString(APK_CFG_INFO, csFileName, csLastTime, strFilePath);
	return time;
}

int NeedUpdateApks(CStringArray& csaApk)  
{  
	CString csFoldername;
	CFileFind tempFind;   
	BOOL bFound; //判断是否成功找到文件  
	csFoldername.Format(_T("%s\\*.*"), APK_PATH);
	bFound=tempFind.FindFile(csFoldername);   //修改" "内内容给限定查找文件类型  
	CString strTmp;   //如果找到的是文件夹 存放文件夹路径  
	while(bFound)      //遍历所有文件  
	{   
		bFound=tempFind.FindNextFile(); //第一次执行FindNextFile是选择到第一个文件，以后执行为选择到下一个文件  
		if(tempFind.IsDots())   
			continue; //如果找到的是返回上层的目录 则结束本次查找    
		else   
		{   
			long long nCurModifyTime;
			long long nLogModifyTime;
			strTmp=tempFind.GetFileName(); //保存文件名，包括后缀名  
			// 在此处添加对找到文件的处理
			strTmp=AttractFileName(strTmp);
			nCurModifyTime = GetFileLastTime(strTmp);
			nLogModifyTime = GetApkModifyTime(strTmp);
			if (nCurModifyTime>nLogModifyTime)
			{
				csaApk.Add(strTmp);
			}
		}   
	}   
	tempFind.Close();   
	return csaApk.GetCount();   
} 

int GetFileByte(unsigned char** pBuf, CString csFileName)
{
	FILE* fp = NULL;
	int nBufLen = 0;
	USES_CONVERSION;
	CString csFilePath;
	char* szFileName = T2A(csFileName.GetBuffer(0));
	csFilePath.Format(_T("%s\\%s.apk"), APK_PATH, csFileName);
	char* szFilePath = T2A(csFilePath.GetBuffer(0));
	int readLen = 0;
	int len = 0;
	fp = fopen(szFilePath, "rb");
	if (fp == NULL)
	{
		*pBuf = NULL;
		return 0;
	}
	fseek(fp, 0, SEEK_END); //定位到文件末 
	nBufLen = ftell(fp); //文件长度 
	nBufLen += 4;
	nBufLen += TINYSIZE;
	*pBuf = (unsigned char*)malloc(nBufLen);
	memcpy(*pBuf, &nBufLen, 4);
	sprintf(szFileName,"%s.apk", szFileName);
	memcpy(*pBuf+4, szFileName, TINYSIZE);
	fseek(fp, 0, SEEK_SET); //定位到文件末 
	while ((len = fread(*pBuf+ 54+readLen, 1, BUFSIZ, fp)) > 0)
	{
		readLen += len;
	}
	return nBufLen;
}
