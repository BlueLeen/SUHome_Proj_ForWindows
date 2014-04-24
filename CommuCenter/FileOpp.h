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
	//strCreateTime.Format("%04d-%02d-%02d %02d:%02d:%02d", stLocal.wYear, stLocal.wMonth, stLocal.wDay,					stLocal.wHour, stLocal.wMinute, stLocal.wSecond);   // �ļ�����ʱ��
	//ZeroMemory(&stLocal, sizeof(SYSTEMTIME));
	//FileTimeToLocalFileTime(&ftModify, &ftLocalModify);
	//FileTimeToSystemTime(&ftLocalModify, &stLocal);
	//strModifyTime.Format("%04d-%02d-%02d %02d:%02d:%02d", stLocal.wYear, stLocal.wMonth, stLocal.wDay,					stLocal.wHour, stLocal.wMinute, stLocal.wSecond); //  �ļ��޸�ʱ��
	//ZeroMemory(&stLocal, sizeof(SYSTEMTIME));
	//FileTimeToLocalFileTime(&ftAccess, &ftLocalAccess);
	//FileTimeToSystemTime(&ftLocalAccess, &stLocal);
	//strAccessTime.Format("%04d-%02d-%02d %02d:%02d:%02d", stLocal.wYear, stLocal.wMonth, stLocal.wDay,					stLocal.wHour, stLocal.wMinute, stLocal.wSecond); // �ļ�����ʱ��

	ZeroMemory(&stLocal, sizeof(SYSTEMTIME));
	FileTimeToLocalFileTime(&ftModify, &ftLocalModify);
	FileTimeToSystemTime(&ftLocalModify, &stLocal);
	csLastTime.Format(_T("%04d%02d%02d%02d%02d%02d"), stLocal.wYear, stLocal.wMonth, stLocal.wDay,					stLocal.wHour, stLocal.wMinute, stLocal.wSecond); //  �ļ��޸�ʱ��

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
	BOOL bFound; //�ж��Ƿ�ɹ��ҵ��ļ�  
	csFoldername.Format(_T("%s\\*.*"), APK_PATH);
	bFound=tempFind.FindFile(csFoldername);   //�޸�" "�����ݸ��޶������ļ�����  
	CString strTmp;   //����ҵ������ļ��� ����ļ���·��  
	while(bFound)      //���������ļ�  
	{   
		bFound=tempFind.FindNextFile(); //��һ��ִ��FindNextFile��ѡ�񵽵�һ���ļ����Ժ�ִ��Ϊѡ����һ���ļ�  
		if(tempFind.IsDots())   
			continue; //����ҵ����Ƿ����ϲ��Ŀ¼ ��������β���    
		else   
		{   
			long long nCurModifyTime;
			long long nLogModifyTime;
			strTmp=tempFind.GetFileName(); //�����ļ�����������׺��  
			// �ڴ˴���Ӷ��ҵ��ļ��Ĵ���
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
	fseek(fp, 0, SEEK_END); //��λ���ļ�ĩ 
	nBufLen = ftell(fp); //�ļ����� 
	nBufLen += 4;
	nBufLen += TINYSIZE;
	*pBuf = (unsigned char*)malloc(nBufLen);
	memcpy(*pBuf, &nBufLen, 4);
	sprintf(szFileName,"%s.apk", szFileName);
	memcpy(*pBuf+4, szFileName, TINYSIZE);
	fseek(fp, 0, SEEK_SET); //��λ���ļ�ĩ 
	while ((len = fread(*pBuf+ 54+readLen, 1, BUFSIZ, fp)) > 0)
	{
		readLen += len;
	}
	return nBufLen;
}
