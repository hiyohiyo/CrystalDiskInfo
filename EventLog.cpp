/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "EventLog.h"

#define REGISTRY_KEY _T("SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\CrystalDiskInfo")

BOOL InstallEventSource()
{
    HKEY hk = NULL;
    DWORD level;
    DWORD length = MAX_PATH;
    TCHAR fileName[MAX_PATH];

    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGISTRY_KEY, 0, KEY_ALL_ACCESS, &hk) != ERROR_SUCCESS)
	{
		if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, REGISTRY_KEY, 0, NULL, REG_OPTION_NON_VOLATILE,
						KEY_ALL_ACCESS, NULL, &hk, NULL) != ERROR_SUCCESS)
		{
			return FALSE;
        }
    }

    if(RegQueryValueEx(hk, _T("EventMessageFile"), NULL, NULL, 
						(LPBYTE)fileName, &length) != ERROR_SUCCESS)
	{
		::GetModuleFileName(NULL, fileName, MAX_PATH);
        if(RegSetValueEx(hk, _T("EventMessageFile"), 0,REG_EXPAND_SZ, 
			(LPBYTE)fileName, (DWORD)(_tcslen(fileName) + 1) * 2) != ERROR_SUCCESS)
		{
			RegCloseKey(hk);
			return FALSE;
        }
    }

    if(RegQueryValueEx(hk, _T("TypesSupported"), NULL, NULL, (LPBYTE)&level,
                        (LPDWORD)&length) != ERROR_SUCCESS)
	{
        level = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
		length = sizeof(DWORD);
        if(RegSetValueEx(hk, _T("TypesSupported"), 0, REG_DWORD, (LPBYTE)&level, length) != ERROR_SUCCESS)
		{
			RegCloseKey(hk);
			return FALSE;
        }
    }

    if(RegCloseKey(hk) == ERROR_SUCCESS)
	{
		return TRUE;
    }
	else
	{
		return FALSE;
	}
}

BOOL WriteEventLog(DWORD eventId, WORD eventType, PTCHAR source, CString message)
{
	HANDLE hEventLog = NULL;
	DWORD flag = 0;
	BOOL result = FALSE;

	hEventLog = RegisterEventSource(NULL, source);
	if(hEventLog == NULL)
	{
		return FALSE;
	}

	LPCTSTR str[1];
	str[0] = message.GetString();

	if(600 <= eventId && eventId < 700)
	{
		flag = 0x80000000; // Warning
	}
	else
	{
		flag = 0x40000000; // Informational
	}

	result = ReportEventW(hEventLog, eventType, 0, eventId + flag, NULL,
						1, 0, (LPCWSTR *)str, NULL);

	if(! result)
	{
		return FALSE;
	}

	return DeregisterEventSource(hEventLog);
}

BOOL UninstallEventSource()
{
	if(SHDeleteKey(HKEY_LOCAL_MACHINE, REGISTRY_KEY) == ERROR_SUCCESS)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
