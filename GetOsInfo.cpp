/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "GetOsInfo.h"
#include "GetFileVersion.h"
//#include "wbemcli.h"     // WMI interface declarations

typedef BOOL (WINAPI *_GetProductInfo)(DWORD, DWORD, DWORD, DWORD, PDWORD);
typedef BOOL (WINAPI *_GetNativeSystemInfo)(LPSYSTEM_INFO);
typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE hProcess,PBOOL Wow64Process);

_GetProductInfo pGetProductInfo = NULL;
_GetNativeSystemInfo pGetNativeSystemInfo = NULL;

BOOL IsWow64()
{
	BOOL bIsWow64 = FALSE;
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(_T("kernel32")), "IsWow64Process");
	if(fnIsWow64Process != NULL)
	{
		if(! fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
		{
			bIsWow64 = FALSE;
		}
	}
	return bIsWow64;
}

BOOL IsX64()
{
	SYSTEM_INFO si = {0};
	
	pGetNativeSystemInfo = (_GetNativeSystemInfo)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetNativeSystemInfo");
	if(pGetNativeSystemInfo != NULL)
	{
		pGetNativeSystemInfo(&si);
		if(si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL IsArm32()
{
	SYSTEM_INFO si = { 0 };

	pGetNativeSystemInfo = (_GetNativeSystemInfo)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetNativeSystemInfo");
	if (pGetNativeSystemInfo != NULL)
	{
		pGetNativeSystemInfo(&si);
		if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL IsArm64()
{
	SYSTEM_INFO si = { 0 };

	pGetNativeSystemInfo = (_GetNativeSystemInfo)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetNativeSystemInfo");
	if (pGetNativeSystemInfo != NULL)
	{
		pGetNativeSystemInfo(&si);
		if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM64)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL IsIa64()
{
	SYSTEM_INFO si = {0};
	
	pGetNativeSystemInfo = (_GetNativeSystemInfo)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetNativeSystemInfo");
	if(pGetNativeSystemInfo != NULL)
	{
		pGetNativeSystemInfo(&si);
		if(si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL IsSidebar()
{
	OSVERSIONINFOEX osvi;
	BOOL bosVersionInfoEx;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if(!(bosVersionInfoEx = GetVersionEx((OSVERSIONINFO *)&osvi)))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx((OSVERSIONINFO *)&osvi);
	}

	if(osvi.dwMajorVersion >= 6 && osvi.dwMinorVersion < 2 && osvi.wProductType == VER_NT_WORKSTATION)
	{
		return TRUE;
	}
	
	return FALSE;
}

void GetOsName(CString& OsFullName)
{
	CString osName, osType, osCsd, osVersion, osBuild, osFullName, osArchitecture;
	CString cstr;
	TCHAR path[MAX_PATH];
	OSVERSIONINFOEX osvi;
	BOOL bosVersionInfoEx;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if(!(bosVersionInfoEx = GetVersionEx((OSVERSIONINFO *)&osvi)))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx((OSVERSIONINFO *)&osvi);
	}

	switch(osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_WINDOWS:
		switch(osvi.dwMinorVersion)
		{
		case 0: // Windows 95
			if(LOWORD(osvi.dwBuildNumber) >= 1214)
			{
				osName = _T("Windows 95 OSR2.5");
			}
			else if(LOWORD(osvi.dwBuildNumber) >= 1212)
			{
				osName = _T("Windows 95 OSR2.1");
			}
			else if(LOWORD(osvi.dwBuildNumber) == 1111)
			{
				osName = _T("Windows 95 OSR2");
			}
			else
			{
				osName = _T("Windows 95");	
			}
			break;
		case 10: // Windows 98
			if(LOWORD(osvi.dwBuildNumber) >= 2222)
			{
				osName = _T("Windows 98 SE");
			}
			else if(LOWORD(osvi.dwBuildNumber) >= 2000)
			{
				osName = _T("Windows 98 SP1");
			}
			else
			{
				osName = _T("Windows 98");	
			}
			break;
		case 90:
			osName = _T("Windows Me");
			break;
		default:
			osName = "Windows 9x";
			break;
		}
		osVersion.Format(_T("%d.%d"), osvi.dwMajorVersion, osvi.dwMinorVersion);
		osBuild.Format(_T("%d"), LOWORD(osvi.dwBuildNumber));

		OsFullName.Format(_T("%s [%s Build %s]"), osName.GetString(), osVersion, osBuild);
		break;

	case VER_PLATFORM_WIN32_NT:
		if(osvi.dwMajorVersion == 3)
		{
			osName.Format(_T("Windows NT3.%d"), osvi.dwMinorVersion);
		}
		else if(osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
		{
			osName = _T("Windows NT4");
		}
		else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
		{
			osName = _T("Windows 2000");
		}
		else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
		{
			osName = _T("Windows XP");
		}
		else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
		{
			if(GetSystemMetrics(SM_SERVERR2))
			{
				osName = _T("Windows Server 2003 R2");
			}
			else if(osvi.wSuiteMask == VER_SUITE_STORAGE_SERVER)
			{
				osName = _T("Windows Storage Server 2003");
			}
			else if(osvi.wProductType == VER_NT_WORKSTATION && IsX64())
			{
				osName = _T("Windows XP");
			}
			else
			{
				osName = _T("Windows Server 2003");
			}
		}
		else if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0)
		{
			if(osvi.wProductType != VER_NT_WORKSTATION)
			{
				osName = _T("Windows Server 2008");
			}
			else
			{
				osName = _T("Windows Vista");
			}
		}
		else if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1)
		{
			if(osvi.wProductType != VER_NT_WORKSTATION)
			{
				osName = _T("Windows Server 2008 R2");
			}
			else
			{
				osName = _T("Windows 7");
			}
		}
		else if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2)
		{
			if(osvi.wProductType != VER_NT_WORKSTATION)
			{
				osName = _T("Windows Server 2012");
			}
			else
			{
				osName = _T("Windows 8");
			}
		}
		else if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3)
		{
			if(osvi.wProductType != VER_NT_WORKSTATION)
			{
				osName = _T("Windows Server 2012 R2");
			}
			else
			{
				osName = _T("Windows 8.1");
			}
		}
		else if ((osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 4)
			|| (osvi.dwMajorVersion == 10 && osvi.dwMinorVersion == 0))
		{
			if (osvi.wProductType != VER_NT_WORKSTATION)
			{
				osName = _T("Windows Server 2016");
			}
			else
			{
				osName = _T("Windows 10");
			}
		}
		else
		{
			osName.Format(_T("Windows NT %d.%d"), osvi.dwMajorVersion, osvi.dwMinorVersion);
		}

		if(osvi.dwMajorVersion >= 6)
		{
			pGetProductInfo = (_GetProductInfo)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetProductInfo");

			if(pGetProductInfo)
			{
				DWORD productType = 0;
				pGetProductInfo(osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &productType);

				switch(productType)
				{
				case PRODUCT_UNLICENSED:
					osType = (_T("Unlicensed"));
					break;
				case PRODUCT_BUSINESS:
					osType = (_T("Business"));
					break;
				case PRODUCT_BUSINESS_N:
					osType = (_T("Business N"));
					break;
				case PRODUCT_CLUSTER_SERVER:
					osType = (_T("Cluster Server"));
					break;
				case PRODUCT_DATACENTER_SERVER:
					osType = (_T("Datacenter (Full installation)"));
					break;
				case PRODUCT_DATACENTER_SERVER_CORE:
					osType = (_T("Datacenter (Server Core installation)"));
					break;
				case PRODUCT_ENTERPRISE:
					osType = (_T("Enterprise"));
					break;
				case PRODUCT_ENTERPRISE_N:
					osType = (_T("Enterprise N"));
					break;
				case PRODUCT_ENTERPRISE_SERVER:
					osType = (_T("Enterprise (Full installation)"));
					break;
				case PRODUCT_ENTERPRISE_SERVER_CORE:
					osType = (_T("Enterprise (Server Core installation)"));
					break;
				case PRODUCT_ENTERPRISE_SERVER_IA64:
					osType = (_T("Datacenter Enterprise for Itanium-based Systems"));
					break;
				case PRODUCT_HOME_BASIC:
					osType = (_T("Home Basic"));
					break;
				case PRODUCT_HOME_BASIC_N:
					osType = (_T("Home Basic N"));
					break;
				case PRODUCT_HOME_PREMIUM:
					osType = (_T("Home Premium"));
					break;
				case PRODUCT_HOME_PREMIUM_N:
					osType = (_T("Home Premium N"));
					break;
				case PRODUCT_HOME_SERVER:
					osType = (_T("Home Server"));
					break;
				case PRODUCT_SERVER_FOR_SMALLBUSINESS:
					osType = (_T("Server for Small Business"));
					break;
				case PRODUCT_SMALLBUSINESS_SERVER:
					osType = (_T("Small Business Server"));
					break;
				case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
					osType = (_T("Small Business Server Premium"));
					break;
				case PRODUCT_STANDARD_SERVER:
					osType = (_T("Server Standard (full installation)"));
					break;
				case PRODUCT_STANDARD_SERVER_CORE:
					osType = (_T("Server Standard (core installation)"));
					break;
				case PRODUCT_STARTER:
					osType = (_T("Starter"));
					break;
				case PRODUCT_STORAGE_ENTERPRISE_SERVER:
					osType = (_T("Storage Server Enterprise"));
					break;
				case PRODUCT_STORAGE_EXPRESS_SERVER:
					osType = (_T("Storage Server Express"));
					break;
				case PRODUCT_STORAGE_STANDARD_SERVER:
					osType = (_T("Storage Server Standard"));
					break;
				case PRODUCT_STORAGE_WORKGROUP_SERVER:
					osType = (_T("Storage Server Workgroup"));
					break;
				case PRODUCT_ULTIMATE:
					osType = (_T("Ultimate"));
					break;
				case PRODUCT_ULTIMATE_N:
					osType = (_T("Ultimate N"));
					break;
				case PRODUCT_WEB_SERVER:
					osType = (_T("Web Server"));
					break;
				case PRODUCT_PROFESSIONAL:
					if(osvi.dwMinorVersion >= 2)
					{
						osType = (_T("Pro"));
					}
					else
					{
						osType = (_T("Professional"));
					}
					break;
				case PRODUCT_PROFESSIONAL_N:
					if(osvi.dwMinorVersion >= 2)
					{
						osType = (_T("Pro N"));
					}
					else
					{
						osType = (_T("Professional N"));
					}
					break;

				case RRODUCT_ESSENTIALBUSINESS_SERVER_MGMT:
				case PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL:
				case PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC:
				case PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC:
					osType = (_T("Essentials"));
					break;
				}
			}
		}
		else if(bosVersionInfoEx)
		{
			if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
			{
				if(osvi.wProductType == VER_NT_WORKSTATION)
				{
					osType = _T("Professional");
				}
				else if(osvi.wProductType == VER_NT_SERVER)
				{
					if(osvi.wSuiteMask & VER_SUITE_DATACENTER)
					{
						osType = _T("DataCenter Server");
					}
					else if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
					{
						osType = _T("Advanced Server");
					}
					else
					{
						osType = _T("Server");
					}
				}
			}
			else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion >= 1)
			{
				if(osvi.wSuiteMask & VER_SUITE_PERSONAL)
				{
					osType = _T("Home Edition");
				}
				else if(osvi.wSuiteMask & VER_SUITE_DATACENTER)
				{
					osType = _T("Datacenter Edition");
				}
				else if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
				{
					osType = _T("Enterprise Edition");
				}
				else if(osvi.wSuiteMask & VER_SUITE_BLADE)
				{
					osType = _T("Web Edition");
				}
				else if(osvi.wProductType == VER_NT_WORKSTATION)
				{
					osType = _T("Professional");
				}


				// Meida Center & Tablet
				if(GetSystemMetrics(SM_MEDIACENTER))
				{
					GetWindowsDirectory(path, MAX_PATH);
					_tcscat_s(path, MAX_PATH, _T("\\ehome\\ehshell.exe"));
					TCHAR str[256];
					if(GetFileVersion(path, str))
					{					
						cstr = str;
						if(cstr.Find(_T("5.1")) == 0)
						{
							osType = _T("Media Center ");
							cstr.Replace(_T("5.1."), _T(""));
							double num = _tstof(cstr);
							if(num <= 2600.1200)
							{
								osType += _T("2002");
							}
							else if(num <= 2600.2500)
							{
								osType += _T("2004");
							}
							else
							{
								osType += _T("2005");
							}
						}
					}
				}
				else if(GetSystemMetrics(SM_TABLETPC))
				{
					osType = _T("Tablet PC");
				}
			}
		}
		else // NT4 SP5
		{
			HKEY hKey;
			TCHAR productType[80];
			DWORD bufLen;
			
			if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\ProductOptions"),
				0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
			{
				if(RegQueryValueEx(hKey, _T("ProductType"), NULL, NULL, (LPBYTE)productType, &bufLen) == ERROR_SUCCESS)
				{
					if(lstrcmpi(_T("WINNT"), productType) == 0)
					{
						osType = "Workstation";
					}
					else if(lstrcmpi(_T("LANMANNT"), productType) == 0)
					{
						osType = "Server";
					}
					else if(lstrcmpi(_T("SERVERNT"), productType) == 0)
					{
						osType = "Advanced Server";
					}
				}
				RegCloseKey(hKey);
			}
		}

		osCsd = osvi.szCSDVersion;
		osCsd.Replace(_T("Service Pack "), _T("SP"));

		osVersion.Format(_T("%d.%d"), osvi.dwMajorVersion, osvi.dwMinorVersion);
		osBuild.Format(_T("%d"), osvi.dwBuildNumber);

		if(IsX64())
		{
			osArchitecture = _T("x64");
		}
		else if (IsArm32())
		{
			osArchitecture = _T("ARM32");
		}
		else if (IsArm64())
		{
			osArchitecture = _T("ARM64");
		}
		else if(IsIa64())
		{
			osArchitecture = _T("IA64");
		}
		else
		{
			osArchitecture = _T("x86");
		}

		if(! osCsd.IsEmpty())
		{
			osFullName.Format(_T("%s %s %s [%s Build %s] (%s)"), osName, osType, osCsd, osVersion, osBuild, osArchitecture);
		}
		else
		{
			osFullName.Format(_T("%s %s [%s Build %s] (%s)"), osName, osType, osVersion, osBuild, osArchitecture);
		}
		OsFullName = osFullName;
		break;
	}
}

BOOL IsClassicSystem()
{
	OSVERSIONINFOEX osvi;
	BOOL bosVersionInfoEx;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if(!(bosVersionInfoEx = GetVersionEx((OSVERSIONINFO *)&osvi)))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx((OSVERSIONINFO *)&osvi);
	}

	if(osvi.dwPlatformId == VER_PLATFORM_WIN32_NT && osvi.dwMajorVersion == 4) // for NT4
	{
		return TRUE;
	}

	if(GetIeVersion() < 700)
	{
		return TRUE;
	}

	return FALSE;
}

DWORD GetIeVersion()
{
	static INT ieVersion = -1;

	if(ieVersion != -1)
	{
		return ieVersion;
	}

	DWORD type = REG_SZ;
	ULONG size = 256;
	HKEY  hKey = NULL;
	BYTE  buf[256];
	CString cstr;

	switch(GetFileVersion(_T("Shdocvw.dll")))
	{
	case 470: ieVersion = 300;	break;
	case 471: ieVersion = 400;	break;
	case 472: ieVersion = 401;	break;
	case 500: ieVersion = 500;	break;
	case 550: ieVersion = 550;	break;
	case 600:
	default:
		ieVersion = 600;
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Internet Explorer"), 
			0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			if(RegQueryValueEx(hKey, _T("Version"), NULL, &type, buf, &size) == ERROR_SUCCESS)
			{
				cstr = (TCHAR*)buf;
				ieVersion = _tstoi(cstr) * 100;
				if(ieVersion == 900 && RegQueryValueEx(hKey, _T("svcVersion"), NULL, &type, buf, &size) == ERROR_SUCCESS)
				{
					cstr = (TCHAR*)buf;
					if(_tstoi(cstr) * 100 > 900)
					{
						ieVersion = _tstoi(cstr) * 100;
					}
				}
			}
		}
		RegCloseKey(hKey);
		break;
	}

	return ieVersion;
}

BOOL IsIe556()
{
	switch(GetIeVersion())
	{
	case 550:
	case 600:
		return TRUE;
		break;
	default:
		return FALSE;
		break;
	}
}

BOOL IsDotNet2()
{
	static INT dotNet2 = -1;

	if(dotNet2 == -1)
	{
		dotNet2 = FALSE;
		DWORD type = REG_DWORD;
		ULONG size = sizeof(DWORD);
		HKEY  hKey = NULL;
		DWORD buf = 0;

		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\NET Framework Setup\\NDP\\v2.0.50727"), 0, KEY_READ, &hKey) == ERROR_SUCCESS
		|| RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Wow6432Node\\Microsoft\\NET Framework Setup\\NDP\\v2.0.50727"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			if(RegQueryValueEx(hKey, _T("Install"), NULL, &type, (LPBYTE)&buf, &size) == ERROR_SUCCESS)
			{
				if(buf == 1)
				{
					dotNet2 = TRUE;
				}
			}
			RegCloseKey(hKey);			
		}
	}

	return (BOOL)dotNet2;
}

BOOL IsDotNet4()
{
	static INT dotNet4 = -1;

	if(dotNet4 == -1)
	{
		dotNet4 = FALSE;
		DWORD type = REG_DWORD;
		ULONG size = sizeof(DWORD);
		HKEY  hKey = NULL;
		DWORD buf = 0;

		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\NET Framework Setup\\NDP\\v4\\Client"), 0, KEY_READ, &hKey) == ERROR_SUCCESS
		|| RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\NET Framework Setup\\NDP\\v4\\Full"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			if(RegQueryValueEx(hKey, _T("Install"), NULL, &type, (LPBYTE)&buf, &size) == ERROR_SUCCESS)
			{
				if(buf == 1)
				{
					dotNet4 = TRUE;
				}
			}
			RegCloseKey(hKey);			
		}
	}

	return (BOOL)dotNet4;
}