// RegTool.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "RegTool.h"

RegTool::RegTool(void)
{
}


RegTool::~RegTool(void)
{
}
CString RegTool::_RegGetString(HKEY hKeyRoot, LPCTSTR lpSubKey, LPCTSTR lpValueName)
{
	CString			strRet;
	HKEY            hKey;
	TCHAR           sz[MAX_PATH];
	DWORD           dwSize = sizeof(sz);

	strRet.Empty();
	memset(sz,0,MAX_PATH);
	if(RegOpenKey(hKeyRoot, lpSubKey, &hKey) != ERROR_SUCCESS)
		return strRet;
	if (RegQueryValueEx(hKey, lpValueName, NULL, NULL, (LPBYTE)sz, &dwSize) == ERROR_SUCCESS)
		strRet = sz;
	RegCloseKey(hKey);

	return strRet;
}
DWORD RegTool::_RegGetDword(HKEY hKeyRoot, LPCTSTR lpSubKey, LPCTSTR lpValueName)
{
	DWORD           sz;
	HKEY            hKey;
	DWORD           dwSize = sizeof(sz);

	if(RegOpenKey(hKeyRoot, lpSubKey, &hKey) != ERROR_SUCCESS)
		return NULL;
	if (RegQueryValueEx(hKey, lpValueName, NULL, NULL, (LPBYTE)&sz, &dwSize) != ERROR_SUCCESS)
		sz = NULL;
	RegCloseKey(hKey);

	return sz;
}
BOOL RegTool::_RegSetString(HKEY hKeyRoot, LPCTSTR lpSubKey, LPCTSTR lpValueName, LPCTSTR lpValue)
{
	HKEY            hKey;

	if(RegOpenKey(hKeyRoot, lpSubKey, &hKey) != ERROR_SUCCESS)
	{
		if(RegCreateKey(hKeyRoot, lpSubKey, &hKey) != ERROR_SUCCESS)
			return FALSE;
	}
	RegSetValueEx(hKey, lpValueName, 0, REG_SZ, (LPBYTE)lpValue, _tcslen(lpValue));
	RegCloseKey(hKey);

	return TRUE;
}
BOOL RegTool::_RegSetDword(HKEY hKeyRoot, LPCTSTR lpSubKey, LPCTSTR lpValueName, DWORD dwValue)
{
	BOOL			bRet=FALSE;
	HKEY            hKey;
	DWORD           dwSize = sizeof(dwValue);

	if(RegOpenKey(hKeyRoot, lpSubKey, &hKey) != ERROR_SUCCESS)
	{
		if(RegCreateKey(hKeyRoot, lpSubKey, &hKey) != ERROR_SUCCESS)
			return bRet;
	}
	if (RegSetValueEx(hKey, lpValueName, NULL, REG_DWORD, (LPBYTE)(&dwValue), dwSize) == ERROR_SUCCESS)
		bRet = TRUE;
	RegCloseKey(hKey);

	return bRet;
}
BOOL RegTool::WritePrivateProfileInt(   LPCTSTR   lpAppName,   LPCTSTR   lpKeyName,   INT   Value,   LPCTSTR   lpFileName   )   
{   
	TCHAR   ValBuf[16];   

	_stprintf(   ValBuf,TEXT(   "%i"   ),   Value);   

	return(   WritePrivateProfileString(   lpAppName,   lpKeyName,   ValBuf,   lpFileName   )   );   
}

BOOL RegTool::GetPrivateProfileInt(   LPCTSTR   lpAppName,   LPCTSTR   lpKeyName,   INT&   Value,   LPCTSTR   lpFileName, INT ValueDefault)   
{   
	Value = ::GetPrivateProfileInt(lpAppName, lpKeyName, ValueDefault, lpFileName); 
	return TRUE;
}

BOOL RegTool::WritePrivateProfileString(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR Value, LPCTSTR lpFileName)  
{
	return ::WritePrivateProfileString(lpAppName, lpKeyName, Value, lpFileName); 
}

BOOL RegTool::GetPrivateProfileString(LPCTSTR lpAppName, LPCTSTR lpKeyName, CString& Value, LPCTSTR lpFileName, LPCTSTR lpDefaultValue)  
{   
	TCHAR szValue[1024];
	::GetPrivateProfileString(lpAppName, lpKeyName, lpDefaultValue, szValue, sizeof(szValue), lpFileName); 
	Value = szValue;
	return TRUE;
}
