#pragma once

#ifdef REGTOOL_EXPORTS
#define  REGTOOL_DLL __declspec(dllexport) 
#else
#define  REGTOOL_DLL __declspec(dllimport)
#endif

class REGTOOL_DLL RegTool
{
public:
	RegTool(void);
	~RegTool(void);
public:
	static	CString _RegGetString(HKEY hKeyRoot, LPCTSTR lpSubKey, LPCTSTR lpValueName);
	static	DWORD   _RegGetDword(HKEY hKeyRoot, LPCTSTR lpSubKey, LPCTSTR lpValueName);
	static	BOOL    _RegSetString(HKEY hKeyRoot, LPCTSTR lpSubKey, LPCTSTR lpValueName, LPCTSTR lpValue);
	static	BOOL    _RegSetDword(HKEY hKeyRoot, LPCTSTR lpSubKey, LPCTSTR lpValueName, DWORD dwValue);
	static BOOL WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT Value, LPCTSTR lpFileName);
	static BOOL GetPrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT& Value, LPCTSTR lpFileName, INT ValueDefault=0);
	static BOOL WritePrivateProfileString(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR Value, LPCTSTR lpFileName);
	static BOOL GetPrivateProfileString(LPCTSTR lpAppName, LPCTSTR lpKeyName, CString& Value, LPCTSTR lpFileName, LPCTSTR lpDefaultValue=NULL);
};
