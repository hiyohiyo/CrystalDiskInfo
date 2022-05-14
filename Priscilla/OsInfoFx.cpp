/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "../stdafx.h"
#include "OsInfoFx.h"
#include "UtilityFx.h"

typedef BOOL (WINAPI* FuncGetProductInfo)(DWORD, DWORD, DWORD, DWORD, PDWORD);
typedef BOOL (WINAPI* FuncGetNativeSystemInfo)(LPSYSTEM_INFO);
typedef BOOL (WINAPI* FuncIsWow64Process)(HANDLE hProcess,PBOOL Wow64Process);
typedef LONG (WINAPI* FuncRtlGetVersion)(POSVERSIONINFOEXW osvi);

BOOL IsWindowsVersionOrGreaterFx(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor)
{
	OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0 };
	DWORDLONG        const dwlConditionMask = VerSetConditionMask(
		VerSetConditionMask(
			VerSetConditionMask(
				0, VER_MAJORVERSION, VER_GREATER_EQUAL),
			VER_MINORVERSION, VER_GREATER_EQUAL),
		VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

	osvi.dwMajorVersion = wMajorVersion;
	osvi.dwMinorVersion = wMinorVersion;
	osvi.wServicePackMajor = wServicePackMajor;

	return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwlConditionMask) != FALSE;
}

BOOL IsWindowBuildOrGreater(DWORD build)
{
	OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0 };
	osvi.dwBuildNumber = build;
	return (VerifyVersionInfoW(&osvi, VER_BUILDNUMBER, VerSetConditionMask(0, VER_BUILDNUMBER, VER_GREATER_EQUAL)) == TRUE);
}

BOOL GetVersionFx(POSVERSIONINFOEXW osvi)
{
	static FuncRtlGetVersion pRtlGetVersion = NULL;
	if (pRtlGetVersion == NULL)
	{
		HMODULE hModule = GetModuleHandle(L"ntdll.dll");
		if (hModule)
		{
			pRtlGetVersion = (FuncRtlGetVersion)GetProcAddress(hModule, "RtlGetVersion");
		}
	}

	if (pRtlGetVersion)
	{
		if (pRtlGetVersion(osvi) >= 0) // NT_SUCCESS(pRtlGetVersion(osvi) 
		{
			return TRUE;
		}
	}	
	return FALSE;
}

BOOL IsX64()
{
	static BOOL b = -1;
	if (b == -1)
	{
		b = FALSE;
		HMODULE hModule = GetModuleHandle(L"kernel32.dll");
		if (hModule)
		{
			FuncGetNativeSystemInfo pGetNativeSystemInfo = (FuncGetNativeSystemInfo)GetProcAddress(hModule, "GetNativeSystemInfo");
			if (pGetNativeSystemInfo != NULL)
			{
				SYSTEM_INFO si{};
				pGetNativeSystemInfo(&si);
				if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
				{
					b = TRUE;
				}
			}
		}
	}
	return b;
}

BOOL IsIa64()
{
	static BOOL b = -1;
	if (b == -1)
	{
		b = FALSE;
		HMODULE hModule = GetModuleHandle(L"kernel32.dll");
		if (hModule)
		{
			FuncGetNativeSystemInfo pGetNativeSystemInfo = (FuncGetNativeSystemInfo)GetProcAddress(hModule, "GetNativeSystemInfo");
			if (pGetNativeSystemInfo != NULL)
			{
				SYSTEM_INFO si{};
				pGetNativeSystemInfo(&si);
				if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
				{
					b = TRUE;
				}
			}
		}
	}
	return b;
}

BOOL IsArm32()
{
	static BOOL b = -1;
	if (b == -1)
	{
		b = FALSE;
		HMODULE hModule = GetModuleHandle(L"kernel32.dll");
		if (hModule)
		{
			FuncGetNativeSystemInfo pGetNativeSystemInfo = (FuncGetNativeSystemInfo)GetProcAddress(hModule, "GetNativeSystemInfo");
			if (pGetNativeSystemInfo != NULL)
			{
				SYSTEM_INFO si{};
				pGetNativeSystemInfo(&si);
				if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM)
				{
					b = TRUE;
				}
			}
		}
	}
	return b;
}

BOOL IsArm64()
{
	static BOOL b = -1;
	if (b == -1)
	{
		b = FALSE;
		HMODULE hModule = GetModuleHandle(L"kernel32.dll");
		if (hModule)
		{
			FuncGetNativeSystemInfo pGetNativeSystemInfo = (FuncGetNativeSystemInfo)GetProcAddress(hModule, "GetNativeSystemInfo");
			if (pGetNativeSystemInfo != NULL)
			{
				SYSTEM_INFO si{};
				pGetNativeSystemInfo(&si);
				if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM64)
				{
					b = TRUE;
				}
			}
		}
	}
	return b;
}

BOOL IsWow64()
{
	static BOOL b = -1;
	if (b == -1)
	{
		b = FALSE;
		HMODULE hModule = GetModuleHandle(L"kernel32.dll");
		if (hModule)
		{
			FuncIsWow64Process pIsWow64Process = (FuncIsWow64Process)GetProcAddress(hModule, "IsWow64Process");
			if (pIsWow64Process != NULL)
			{
				pIsWow64Process(GetCurrentProcess(), &b);
			}
		}
	}
	return b;
}

BOOL IsIe556()
{
	switch (GetIeVersion())
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
	static BOOL b = -1;

	if (b == -1)
	{
		b = FALSE;
		DWORD type = REG_DWORD;
		ULONG size = sizeof(DWORD);
		HKEY  hKey = NULL;
		DWORD buf = 0;

		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\NET Framework Setup\\NDP\\v2.0.50727", 0, KEY_READ, &hKey) == ERROR_SUCCESS
		||  RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wow6432Node\\Microsoft\\NET Framework Setup\\NDP\\v2.0.50727", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			if (RegQueryValueEx(hKey, L"Install", NULL, &type, (LPBYTE)&buf, &size) == ERROR_SUCCESS)
			{
				if (buf == 1)
				{
					b = TRUE;
				}
			}
			RegCloseKey(hKey);
		}
	}

	return (BOOL)b;
}

BOOL IsDotNet4()
{
	static BOOL b = -1;

	if (b == -1)
	{
		b = FALSE;
		DWORD type = REG_DWORD;
		ULONG size = sizeof(DWORD);
		HKEY  hKey = NULL;
		DWORD buf = 0;

		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\NET Framework Setup\\NDP\\v4\\Client", 0, KEY_READ, &hKey) == ERROR_SUCCESS
		||  RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\NET Framework Setup\\NDP\\v4\\Full", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			if (RegQueryValueEx(hKey, L"Install", NULL, &type, (LPBYTE)&buf, &size) == ERROR_SUCCESS)
			{
				if (buf == 1)
				{
					b = TRUE;
				}
			}
			RegCloseKey(hKey);
		}
	}

	return b;
}

BOOL IsDotNet48()
{
	static BOOL b = -1;

	if (b == -1)
	{
		b = FALSE;
		DWORD type = REG_DWORD;
		ULONG size = sizeof(DWORD);
		HKEY  hKey = NULL;
		DWORD buf = 0;

		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\NET Framework Setup\\NDP\\v4\\Client", 0, KEY_READ, &hKey) == ERROR_SUCCESS
		||  RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\NET Framework Setup\\NDP\\v4\\Full", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			if (RegQueryValueEx(hKey, L"Install", NULL, &type, (LPBYTE)&buf, &size) == ERROR_SUCCESS)
			{
				if (buf == 1)
				{
					if (RegQueryValueEx(hKey, L"Release", NULL, &type, (LPBYTE)&buf, &size) == ERROR_SUCCESS)
					{
						if (buf >= 528040)
						{
							b = TRUE;
						}
					}
				}
			}
			RegCloseKey(hKey);
		}
	}

	return b;
}

BOOL IsNT5()
{
	static BOOL b = -1;
	if (b == -1)
	{
		b = !IsWindowsVersionOrGreaterFx(6, 0) && IsWindowsVersionOrGreaterFx(5, 0) ? TRUE : FALSE;
	}
	return b;
}

BOOL IsNT6orLater()
{
	static BOOL b = -1;
	if (b == -1)
	{
		b = IsWindowsVersionOrGreaterFx(6, 0) ? TRUE : FALSE;
	}
	return b;
}

BOOL IsWin2k()
{
	static BOOL b = -1;
	if (b == -1)
	{
		b = IsWindowsVersionOrGreaterFx(5, 0) && !IsWindowsVersionOrGreaterFx(5, 1) ? TRUE : FALSE;
	}
	return b;
}

BOOL IsWinXpOrLater()
{
	static BOOL b = -1;
	if (b == -1)
	{
		b = IsWindowsVersionOrGreaterFx(5, 1) ? TRUE : FALSE;
	}
	return b;
}

BOOL IsWinXpLuna()
{
	static BOOL xp = -1;
	BOOL b = FALSE;
	if (xp == -1)
	{
		xp = IsWindowsVersionOrGreaterFx(5, 1) && !IsWindowsVersionOrGreaterFx(6, 0) ? TRUE : FALSE;
	}
	if (xp == FALSE)
	{ 
		return FALSE;
	}

	// Luna Check
	DWORD type = REG_DWORD;
	ULONG size = 256;
	HKEY  hKey = NULL;
	BYTE  buf[256] = {};
	CString cstr;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\ThemeManager\\", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(hKey, L"ThemeActive", NULL, &type, buf, &size) == ERROR_SUCCESS)
		{
			cstr = (TCHAR*)buf;
			if (cstr.Find(L"1") == 0)
			{
				b = TRUE;
			}
		}
		RegCloseKey(hKey);		
	}
	return b;
}

BOOL IsWin8orLater()
{
	static BOOL b = -1;
	if (b == -1)
	{
		b = IsWindowsVersionOrGreaterFx(6, 2) ? TRUE : FALSE;
	}
	return b;
}

BOOL IsWin81orLater()
{
	static BOOL b = -1;
	if (b == -1)
	{
		b = IsWindowsVersionOrGreaterFx(6, 3) ? TRUE : FALSE;
	}
	return b;
}

BOOL IsDarkModeSupport()
{
	static BOOL b = -1;
	if (b == -1)
	{
		b = IsWindowBuildOrGreater(17763) ? TRUE : FALSE;
	}
	return b;
}

BOOL HasSidebar()
{
	static BOOL b = -1;
	if (b == -1)
	{
		b = FALSE;
		OSVERSIONINFOEX osvi;
		GetVersionFx(&osvi);

		if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion < 2 && osvi.wProductType == VER_NT_WORKSTATION)
		{
			b = TRUE;
		}
	}
	return b;
}

DWORD GetIeVersion()
{
	static INT ieVersion = -1;

	if (ieVersion != -1)
	{
		return ieVersion;
	}

	DWORD type = REG_SZ;
	ULONG size = 256;
	HKEY  hKey = NULL;
	BYTE  buf[256] = {};
	CString cstr;

	switch (GetFileVersion(L"Shdocvw.dll"))
	{
	case 470: ieVersion = 300;	break;
	case 471: ieVersion = 400;	break;
	case 472: ieVersion = 401;	break;
	case 500: ieVersion = 500;	break;
	case 550: ieVersion = 550;	break;
	case 600:
	default:
		ieVersion = 600;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Internet Explorer", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			if (RegQueryValueEx(hKey, L"Version", NULL, &type, buf, &size) == ERROR_SUCCESS)
			{
				cstr = (TCHAR*)buf;
				ieVersion = _tstoi(cstr) * 100;
				if (ieVersion == 900 && RegQueryValueEx(hKey, L"svcVersion", NULL, &type, buf, &size) == ERROR_SUCCESS)
				{
					cstr = (TCHAR*)buf;
					if (_tstoi(cstr) * 100 > 900)
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

void GetOsName(CString& OsFullName)
{
	CString osName, osType, osCsd, osVersion, osBuild, osFullName, osArchitecture;
	CString cstr;

	OSVERSIONINFOEX osvi = { sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0 };
	GetVersionFx((OSVERSIONINFOEX*)&osvi);

	switch(osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_WINDOWS:
		switch(osvi.dwMinorVersion)
		{
		case 0: // Windows 95
			if(LOWORD(osvi.dwBuildNumber) >= 1214)
			{
				osName = L"Windows 95 OSR2.5";
			}
			else if(LOWORD(osvi.dwBuildNumber) >= 1212)
			{
				osName = L"Windows 95 OSR2.1";
			}
			else if(LOWORD(osvi.dwBuildNumber) == 1111)
			{
				osName = L"Windows 95 OSR2";
			}
			else
			{
				osName = L"Windows 95";	
			}
			break;
		case 10: // Windows 98
			if(LOWORD(osvi.dwBuildNumber) >= 2222)
			{
				osName = L"Windows 98 SE";
			}
			else if(LOWORD(osvi.dwBuildNumber) >= 2000)
			{
				osName = L"Windows 98 SP1";
			}
			else
			{
				osName = L"Windows 98";	
			}
			break;
		case 90:
			osName = L"Windows Me";
			break;
		default:
			osName = L"Windows 9x";
			break;
		}
		osVersion.Format(L"%d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion);
		osBuild.Format(L"%d", LOWORD(osvi.dwBuildNumber));

		OsFullName.Format(L"%s [%s Build %s]", (LPCTSTR)osName.GetString(), (LPCTSTR)osVersion.GetString(), (LPCTSTR)osBuild.GetString());
		break;

	case VER_PLATFORM_WIN32_NT:
		if(osvi.dwMajorVersion == 3)
		{
			osName.Format(L"Windows NT3.%d", osvi.dwMinorVersion);
		}
		else if(osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
		{
			osName = L"Windows NT4";
		}
		else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
		{
			osName = L"Windows 2000";
		}
		else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
		{
			osName = L"Windows XP";
		}
		else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
		{
			if(GetSystemMetrics(SM_SERVERR2))
			{
				osName = L"Windows Server 2003 R2";
			}
			else if(osvi.wSuiteMask == VER_SUITE_STORAGE_SERVER)
			{
				osName = L"Windows Storage Server 2003";
			}
			else if(osvi.wProductType == VER_NT_WORKSTATION && IsX64())
			{
				osName = L"Windows XP";
			}
			else
			{
				osName = L"Windows Server 2003";
			}
		}
		else if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0)
		{
			if(osvi.wProductType != VER_NT_WORKSTATION)
			{
				osName = L"Windows Server 2008";
			}
			else
			{
				osName = L"Windows Vista";
			}
		}
		else if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1)
		{
			if(osvi.wProductType != VER_NT_WORKSTATION)
			{
				osName = L"Windows Server 2008 R2";
			}
			else
			{
				osName = L"Windows 7";
			}
		}
		else if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2)
		{
			if(osvi.wProductType != VER_NT_WORKSTATION)
			{
				osName = L"Windows Server 2012";
			}
			else
			{
				osName = L"Windows 8";
			}
		}
		else if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3)
		{
			if(osvi.wProductType != VER_NT_WORKSTATION)
			{
				osName = L"Windows Server 2012 R2";
			}
			else
			{
				osName = L"Windows 8.1";
			}
		}
		else if ((osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 4)
			|| (osvi.dwMajorVersion >= 10 && osvi.dwMinorVersion >= 0))
		{
			if (osvi.wProductType != VER_NT_WORKSTATION)
			{
				if (osvi.dwBuildNumber >= 20344)
				{
					osName = L"Windows Server 2022";
				}
				else if(osvi.dwBuildNumber >= 17763)
				{
					osName = L"Windows Server 2019";
				}
				else
				{
					osName = L"Windows Server 2016";
				}
			}
			else
			{
				if (osvi.dwBuildNumber >= 21996)
				{
					osName = L"Windows 11";
				}
				else
				{
					osName = L"Windows 10";
				}
			}
		}
		else
		{
			osName.Format(L"Windows NT %d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion);
		}

		if(osvi.dwMajorVersion >= 6)
		{
			HMODULE hModule = GetModuleHandle(L"kernel32.dll");
			FuncGetProductInfo pGetProductInfo = NULL;
			if (hModule)
			{
				pGetProductInfo = (FuncGetProductInfo)GetProcAddress(hModule, "GetProductInfo");
			}

			if(pGetProductInfo)
			{
				DWORD productType = 0;
				pGetProductInfo(osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &productType);

				switch(productType)
				{
				case PRODUCT_UNLICENSED:
					osType = L"Unlicensed";
					break;
				case PRODUCT_BUSINESS:
					osType = L"Business";
					break;
				case PRODUCT_BUSINESS_N:
					osType = L"Business N";
					break;
				case PRODUCT_CLUSTER_SERVER:
					osType = L"Cluster Server";
					break;
				case PRODUCT_DATACENTER_SERVER:
					osType = L"Datacenter (Full installation)";
					break;
				case PRODUCT_DATACENTER_SERVER_CORE:
					osType = L"Datacenter (Server Core installation)";
					break;
				case PRODUCT_ENTERPRISE:
					osType = L"Enterprise";
					break;
				case PRODUCT_ENTERPRISE_N:
					osType = L"Enterprise N";
					break;
				case PRODUCT_ENTERPRISE_SERVER:
					osType = L"Enterprise (Full installation)";
					break;
				case PRODUCT_ENTERPRISE_SERVER_CORE:
					osType = L"Enterprise (Server Core installation)";
					break;
				case PRODUCT_ENTERPRISE_SERVER_IA64:
					osType = L"Datacenter Enterprise for Itanium-based Systems";
					break;
				case PRODUCT_HOME_BASIC:
					osType = L"Home Basic";
					break;
				case PRODUCT_HOME_BASIC_N:
					osType = L"Home Basic N";
					break;
				case PRODUCT_HOME_PREMIUM:
					osType = L"Home Premium";
					break;
				case PRODUCT_HOME_PREMIUM_N:
					osType = L"Home Premium N";
					break;
				case PRODUCT_HOME_SERVER:
					osType = L"Home Server";
					break;
				case PRODUCT_SERVER_FOR_SMALLBUSINESS:
					osType = L"Server for Small Business";
					break;
				case PRODUCT_SMALLBUSINESS_SERVER:
					osType = L"Small Business Server";
					break;
				case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
					osType = L"Small Business Server Premium";
					break;
				case PRODUCT_STANDARD_SERVER:
					osType = L"Server Standard (full installation)";
					break;
				case PRODUCT_STANDARD_SERVER_CORE:
					osType = L"Server Standard (core installation)";
					break;
				case PRODUCT_STARTER:
					osType = L"Starter";
					break;
				case PRODUCT_STORAGE_ENTERPRISE_SERVER:
					osType = L"Storage Server Enterprise";
					break;
				case PRODUCT_STORAGE_EXPRESS_SERVER:
					osType = L"Storage Server Express";
					break;
				case PRODUCT_STORAGE_STANDARD_SERVER:
					osType = L"Storage Server Standard";
					break;
				case PRODUCT_STORAGE_WORKGROUP_SERVER:
					osType = L"Storage Server Workgroup";
					break;
				case PRODUCT_ULTIMATE:
					osType = L"Ultimate";
					break;
				case PRODUCT_ULTIMATE_N:
					osType = L"Ultimate N";
					break;
				case PRODUCT_WEB_SERVER:
					osType = L"Web Server";
					break;
				case PRODUCT_PROFESSIONAL:
					if(osvi.dwMinorVersion >= 2)
					{
						osType = L"Pro";
					}
					else
					{
						osType = L"Professional";
					}
					break;
				case PRODUCT_PROFESSIONAL_N:
					if(osvi.dwMinorVersion >= 2)
					{
						osType = L"Pro N";
					}
					else
					{
						osType = L"Professional N";
					}
					break;
				case PRODUCT_PRO_WORKSTATION:
					osType = L"Pro for Workstation";
					break;
				case PRODUCT_PRO_WORKSTATION_N:
					osType = L"Pro for Workstation N";
					break;
				case PRODUCT_PRO_FOR_EDUCATION:
					osType = L"Pro for Education";
					break;
				case PRODUCT_PRO_FOR_EDUCATION_N:
					osType = L"Pro for Education N";
					break;
				case PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL:
				case PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC:
				case PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC:
					osType = L"Essentials Server";
					break;
				}
			}
		}
		else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
		{
			if(osvi.wProductType == VER_NT_WORKSTATION)
			{
				osType = L"Professional";
			}
			else if(osvi.wProductType == VER_NT_SERVER)
			{
				if(osvi.wSuiteMask & VER_SUITE_DATACENTER)
				{
					osType = L"DataCenter Server";
				}
				else if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
				{
					osType = L"Advanced Server";
				}
				else
				{
					osType = L"Server";
				}
			}
		}
		else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion >= 1)
		{
			if(osvi.wSuiteMask & VER_SUITE_PERSONAL)
			{
				osType = L"Home Edition";
			}
			else if(osvi.wSuiteMask & VER_SUITE_DATACENTER)
			{
				osType = L"Datacenter Edition";
			}
			else if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
			{
				osType = L"Enterprise Edition";
			}
			else if(osvi.wSuiteMask & VER_SUITE_BLADE)
			{
				osType = L"Web Edition";
			}
			else if(osvi.wProductType == VER_NT_WORKSTATION)
			{
				osType = L"Professional";
			}

			// Meida Center & Tablet
			if(GetSystemMetrics(SM_MEDIACENTER))
			{
				TCHAR path[MAX_PATH] = {};
				TCHAR str[256] = {};
				UINT length = GetWindowsDirectoryW(path, MAX_PATH);
				_tcscat_s(path, MAX_PATH, L"\\ehome\\ehshell.exe");
				if(length != 0 && GetFileVersion(path, str))
				{					
					cstr = str;
					if(cstr.Find(L"5.1") == 0)
					{
						osType = L"Media Center ";
						cstr.Replace(L"5.1.", L"");
						double num = _tstof(cstr);
						if(num <= 2600.1200)
						{
							osType += L"2002";
						}
						else if(num <= 2600.2500)
						{
							osType += L"2004";
						}
						else
						{
							osType += L"2005";
						}
					}
				}
			}
			else if(GetSystemMetrics(SM_TABLETPC))
			{
				osType = L"Tablet PC";
			}
		}

		osCsd = osvi.szCSDVersion;
		osCsd.Replace(L"Service Pack ", L"SP");

		osVersion.Format(L"%d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion);
		osBuild.Format(L"%d", osvi.dwBuildNumber);

		if(IsX64())
		{
			osArchitecture = L"x64";
		}
		else if (IsArm32())
		{
			osArchitecture = L"ARM32";
		}
		else if (IsArm64())
		{
			osArchitecture = L"ARM64";
		}
		else if(IsIa64())
		{
			osArchitecture = L"IA64";
		}
		else
		{
			osArchitecture = L"x86";
		}

		if(! osCsd.IsEmpty())
		{
			osFullName.Format(L"%s %s %s [%s Build %s] (%s)", (LPCTSTR)osName, (LPCTSTR)osType, (LPCTSTR)osCsd, (LPCTSTR)osVersion, (LPCTSTR)osBuild, (LPCTSTR)osArchitecture);
		}
		else
		{
			osFullName.Format(L"%s %s [%s Build %s] (%s)", (LPCTSTR)osName, (LPCTSTR)osType, (LPCTSTR)osVersion, (LPCTSTR)osBuild, (LPCTSTR)osArchitecture);
		}
		OsFullName = osFullName;
	}
}
