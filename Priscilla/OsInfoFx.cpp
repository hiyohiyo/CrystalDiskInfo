/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#include "../stdafx.h"
#include "OsInfoFx.h"
#include "UtilityFx.h"

typedef BOOL (WINAPI* FuncGetProductInfo)(DWORD, DWORD, DWORD, DWORD, PDWORD);
typedef BOOL (WINAPI* FuncGetNativeSystemInfo)(LPSYSTEM_INFO);
typedef BOOL (WINAPI* FuncIsWow64Process)(HANDLE hProcess,PBOOL Wow64Process);
typedef LONG (WINAPI* FuncRtlGetVersion)(POSVERSIONINFOEXW osvi);

#if _MSC_VER > 1310
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

BOOL IsWindowsBuildOrGreater(DWORD build)
{
	OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0 };
	osvi.dwBuildNumber = build;
	return (VerifyVersionInfoW(&osvi, VER_BUILDNUMBER, VerSetConditionMask(0, VER_BUILDNUMBER, VER_GREATER_EQUAL)) == TRUE);
}
#endif

BOOL GetVersionFx(POSVERSIONINFOEXW osvi)
{
	static FuncRtlGetVersion pRtlGetVersion = NULL;
	if (pRtlGetVersion == NULL)
	{
		HMODULE hModule = GetModuleHandle(_T("ntdll.dll"));
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

#if _MSC_VER > 1310
BOOL IsX64()
{
	static BOOL b = -1;
	if (b == -1)
	{
		b = FALSE;
		HMODULE hModule = GetModuleHandle(_T("kernel32.dll"));
		if (hModule)
		{
			FuncGetNativeSystemInfo pGetNativeSystemInfo = (FuncGetNativeSystemInfo)GetProcAddress(hModule, "GetNativeSystemInfo");
			if (pGetNativeSystemInfo != NULL)
			{
				SYSTEM_INFO si = { 0 };
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
		HMODULE hModule = GetModuleHandle(_T("kernel32.dll"));
		if (hModule)
		{
			FuncGetNativeSystemInfo pGetNativeSystemInfo = (FuncGetNativeSystemInfo)GetProcAddress(hModule, "GetNativeSystemInfo");
			if (pGetNativeSystemInfo != NULL)
			{
				SYSTEM_INFO si = { 0 };
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
		HMODULE hModule = GetModuleHandle(_T("kernel32.dll"));
		if (hModule)
		{
			FuncGetNativeSystemInfo pGetNativeSystemInfo = (FuncGetNativeSystemInfo)GetProcAddress(hModule, "GetNativeSystemInfo");
			if (pGetNativeSystemInfo != NULL)
			{
				SYSTEM_INFO si = { 0 };
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
		HMODULE hModule = GetModuleHandle(_T("kernel32.dll"));
		if (hModule)
		{
			FuncGetNativeSystemInfo pGetNativeSystemInfo = (FuncGetNativeSystemInfo)GetProcAddress(hModule, "GetNativeSystemInfo");
			if (pGetNativeSystemInfo != NULL)
			{
				SYSTEM_INFO si = { 0 };
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
		HMODULE hModule = GetModuleHandle(_T("kernel32.dll"));
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

		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\NET Framework Setup\\NDP\\v2.0.50727"), 0, KEY_READ, &hKey) == ERROR_SUCCESS
		||  RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Wow6432Node\\Microsoft\\NET Framework Setup\\NDP\\v2.0.50727"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			if (RegQueryValueEx(hKey, _T("Install"), NULL, &type, (LPBYTE)&buf, &size) == ERROR_SUCCESS)
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

		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\NET Framework Setup\\NDP\\v4\\Client"), 0, KEY_READ, &hKey) == ERROR_SUCCESS
		||  RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\NET Framework Setup\\NDP\\v4\\Full"), 0, KEY_READ, &hKey) == ERROR_SUCCESS
		||  RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\WOW6432Node\\Microsoft\\NET Framework Setup\\NDP\\v4\\Client"), 0, KEY_READ, &hKey) == ERROR_SUCCESS
		||  RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\WOW6432Node\\Microsoft\\NET Framework Setup\\NDP\\v4\\Full"), 0, KEY_READ, &hKey) == ERROR_SUCCESS
			)
		{
			if (RegQueryValueEx(hKey, _T("Install"), NULL, &type, (LPBYTE)&buf, &size) == ERROR_SUCCESS)
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

		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\NET Framework Setup\\NDP\\v4\\Client"), 0, KEY_READ, &hKey) == ERROR_SUCCESS
		||  RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\NET Framework Setup\\NDP\\v4\\Full"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			if (RegQueryValueEx(hKey, _T("Install"), NULL, &type, (LPBYTE)&buf, &size) == ERROR_SUCCESS)
			{
				if (buf == 1)
				{
					if (RegQueryValueEx(hKey, _T("Release"), NULL, &type, (LPBYTE)&buf, &size) == ERROR_SUCCESS)
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

	if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\ThemeManager\\"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(hKey, _T("ThemeActive"), NULL, &type, buf, &size) == ERROR_SUCCESS)
		{
			cstr = (TCHAR*)buf;
			if (cstr.Find(_T("1")) == 0)
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
		b = IsWindowsBuildOrGreater(17763) ? TRUE : FALSE;
	}
	return b;
}

BOOL HasSidebar()
{
	static BOOL b = -1;
	if (b == -1)
	{
		b = FALSE;
		OSVERSIONINFOEXW osvi;
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

	switch (GetFileVersion(_T("Shdocvw.dll")))
	{
	case 470: ieVersion = 300;	break;
	case 471: ieVersion = 400;	break;
	case 472: ieVersion = 401;	break;
	case 500: ieVersion = 500;	break;
	case 550: ieVersion = 550;	break;
	case 600:
	default:
		ieVersion = 600;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Internet Explorer"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			if (RegQueryValueEx(hKey, _T("Version"), NULL, &type, buf, &size) == ERROR_SUCCESS)
			{
				cstr = (TCHAR*)buf;
				ieVersion = _tstoi(cstr) * 100;
				if (ieVersion == 900 && RegQueryValueEx(hKey, _T("svcVersion"), NULL, &type, buf, &size) == ERROR_SUCCESS)
				{
					cstr = (TCHAR*)buf;
					if (_tstoi(cstr) * 100 > 900)
					{
						ieVersion = _tstoi(cstr) * 100;
					}
				}
			}
			RegCloseKey(hKey);
		}		
		break;
	}

	return ieVersion;
}
#endif

BOOL IsWin9x()
{


	OSVERSIONINFO osvi = { 0 };
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx((OSVERSIONINFO*)&osvi);

	return osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS;
}

BOOL IsPC98()
{
	static BOOL b = -1;

	DWORD type = REG_SZ;
	ULONG size = 256;
	HKEY  hKey = NULL;
	BYTE  buf[256] = {};
	CString cstr;

	if (b == -1)
	{
		b = FALSE;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DESCRIPTION\\System"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			if (RegQueryValueEx(hKey, _T("Identifier"), NULL, &type, buf, &size) == ERROR_SUCCESS)
			{
				cstr = (TCHAR*)buf;
				if (cstr.Find(_T("NEC PC-98")) == 0)
				{
					b = TRUE;
				}
			}
			RegCloseKey(hKey);
		}
	}

	return b;
}

void GetOsName(CString& osFullName)
{
	CString osName, osType, osCsd, osVersion, osBuild, osArchitecture, osDisplayVersion;
	CString osNameWmi;
	CString cstr;

#if _MSC_VER == 1310
	// For Win9x
	OSVERSIONINFO osv = { 0 };
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx((OSVERSIONINFO*)&osv);

	if (osv.dwPlatformId == VER_PLATFORM_WIN32s)
	{
		osFullName = _T("Windows 3.x + Win32s");
		return;
	}
	else if (osv.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		switch (osv.dwMinorVersion)
		{
		case 0: // Windows 95
			if (LOWORD(osv.dwBuildNumber) >= 1214)
			{
				osName = _T("Windows 95 OSR2.5");
			}
			else if (LOWORD(osv.dwBuildNumber) >= 1212)
			{
				osName = _T("Windows 95 OSR2.1");
			}
			else if (LOWORD(osv.dwBuildNumber) >= 1111)
			{
				osName = _T("Windows 95 OSR2");
			}
			else
			{
				osName = _T("Windows 95");
			}
			break;
		case 10: // Windows 98
			if (LOWORD(osv.dwBuildNumber) >= 2222)
			{
				osName = _T("Windows 98 SE");
			}
			else if (LOWORD(osv.dwBuildNumber) >= 2000)
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
			osName = _T("Windows 9x");
			break;
		}
		osVersion.Format(_T("%d.%d"), osv.dwMajorVersion, osv.dwMinorVersion);
		osBuild.Format(_T("%d"), LOWORD(osv.dwBuildNumber));

		osFullName.Format(_T("%s [%s Build %s]"), (LPCTSTR)osName.GetString(), (LPCTSTR)osVersion.GetString(), (LPCTSTR)osBuild.GetString());
		return;
	}
#endif

#if _MSC_VER > 1310
#ifdef UNICODE
	OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0 };
	GetVersionFx((OSVERSIONINFOEXW*)&osvi);
#else
	OSVERSIONINFOEX osvi = { sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0 };
	GetVersionEx((OSVERSIONINFO*)&osvi);
#endif

#else
	OSVERSIONINFOEX osvi = { sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0 };
	GetVersionEx((OSVERSIONINFO*)&osvi);
#endif


	GetOsNameWmi(osNameWmi);
	
	if (osNameWmi.IsEmpty())
	{
		switch (osvi.dwPlatformId)
		{
		case VER_PLATFORM_WIN32_NT:
			if (osvi.dwMajorVersion == 3)
			{
				osName.Format(_T("Windows NT 3.%d"), osvi.dwMinorVersion);
			}
			else if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
			{
				osName = _T("Windows NT 4.0");
			}
			else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
			{
				osName = _T("Windows 2000");
			}
			else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
			{
				osName = _T("Windows XP");
			}
			else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
			{
				if (GetSystemMetrics(SM_SERVERR2))
				{
					osName = _T("Windows Server 2003 R2");
				}
				else if (osvi.wSuiteMask == 0x00002000 /*VER_SUITE_STORAGE_SERVER*/)
				{
					osName = _T("Windows Storage Server 2003");
				}
				else if (osvi.wProductType == VER_NT_WORKSTATION)
				{
					osName = _T("Windows XP");
				}
				else
				{
					osName = _T("Windows Server 2003");
				}
			}
			else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0)
			{
				if (osvi.wProductType != VER_NT_WORKSTATION)
				{
					osName = _T("Windows Server 2008");
				}
				else
				{
					osName = _T("Windows Vista");
				}
			}
			else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1)
			{
				if (osvi.wProductType != VER_NT_WORKSTATION)
				{
					osName = _T("Windows Server 2008 R2");
				}
				else
				{
					osName = _T("Windows 7");
				}
			}
			else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2)
			{
				if (osvi.wProductType != VER_NT_WORKSTATION)
				{
					osName = _T("Windows Server 2012");
				}
				else
				{
					osName = _T("Windows 8");
				}
			}
			else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3)
			{
				if (osvi.wProductType != VER_NT_WORKSTATION)
				{
					osName = _T("Windows Server 2012 R2");
				}
				else
				{
					osName = _T("Windows 8.1");
				}
			}
			else if ((osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 4)
				|| (osvi.dwMajorVersion >= 10 && osvi.dwMinorVersion >= 0))
			{
				if (osvi.wProductType != VER_NT_WORKSTATION)
				{
					if (osvi.dwBuildNumber >= 26040)
					{
						osName = _T("Windows Server 2025");
					}
					else if (osvi.dwBuildNumber >= 20344)
					{
						osName = _T("Windows Server 2022");
					}
					else if (osvi.dwBuildNumber >= 17763)
					{
						osName = _T("Windows Server 2019");
					}
					else
					{
						osName = _T("Windows Server 2016");
					}
				}
				else
				{
					if (osvi.dwBuildNumber >= 21996)
					{
						osName = _T("Windows 11");
					}
					else
					{
						osName = _T("Windows 10");
					}
				}
			}
			else
			{
				osName.Format(_T("Windows NT %d.%d"), osvi.dwMajorVersion, osvi.dwMinorVersion);
			}

			if (osvi.dwMajorVersion >= 6)
			{
				HMODULE hModule = GetModuleHandle(_T("kernel32.dll"));
				FuncGetProductInfo pGetProductInfo = NULL;
				if (hModule)
				{
					pGetProductInfo = (FuncGetProductInfo)GetProcAddress(hModule, "GetProductInfo");
				}

				if (pGetProductInfo)
				{
					DWORD productType = 0;
					pGetProductInfo(osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &productType);

					switch (productType)
					{
					case PRODUCT_UNLICENSED:
						osType = _T("Unlicensed");
						break;
					case PRODUCT_BUSINESS:
						osType = _T("Business");
						break;
					case PRODUCT_BUSINESS_N:
						osType = _T("Business N");
						break;
					case PRODUCT_CLUSTER_SERVER:
						osType = _T("Cluster Server");
						break;
					case PRODUCT_DATACENTER_SERVER:
						osType = _T("Datacenter (Full installation)");
						break;
					case PRODUCT_DATACENTER_SERVER_CORE:
						osType = _T("Datacenter (Server Core installation)");
						break;
					case PRODUCT_ENTERPRISE:
						osType = _T("Enterprise");
						break;
					case PRODUCT_ENTERPRISE_N:
						osType = _T("Enterprise N");
						break;
					case PRODUCT_ENTERPRISE_SERVER:
						osType = _T("Enterprise (Full installation)");
						break;
					case PRODUCT_ENTERPRISE_SERVER_CORE:
						osType = _T("Enterprise (Server Core installation)");
						break;
					case PRODUCT_ENTERPRISE_SERVER_IA64:
						osType = _T("Datacenter Enterprise for Itanium-based Systems");
						break;
					case PRODUCT_HOME_BASIC:
						osType = _T("Home Basic");
						break;
					case PRODUCT_HOME_BASIC_N:
						osType = _T("Home Basic N");
						break;
					case PRODUCT_HOME_PREMIUM:
						osType = _T("Home Premium");
						break;
					case PRODUCT_HOME_PREMIUM_N:
						osType = _T("Home Premium N");
						break;
					case PRODUCT_HOME_SERVER:
						osType = _T("Home Server");
						break;
					case PRODUCT_SERVER_FOR_SMALLBUSINESS:
						osType = _T("Server for Small Business");
						break;
					case PRODUCT_SMALLBUSINESS_SERVER:
						osType = _T("Small Business Server");
						break;
					case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
						osType = _T("Small Business Server Premium");
						break;
					case PRODUCT_STANDARD_SERVER:
						osType = _T("Server Standard (full installation)");
						break;
					case PRODUCT_STANDARD_SERVER_CORE:
						osType = _T("Server Standard (core installation)");
						break;
					case PRODUCT_STARTER:
						osType = _T("Starter");
						break;
					case PRODUCT_STORAGE_ENTERPRISE_SERVER:
						osType = _T("Storage Server Enterprise");
						break;
					case PRODUCT_STORAGE_EXPRESS_SERVER:
						osType = _T("Storage Server Express");
						break;
					case PRODUCT_STORAGE_STANDARD_SERVER:
						osType = _T("Storage Server Standard");
						break;
					case PRODUCT_STORAGE_WORKGROUP_SERVER:
						osType = _T("Storage Server Workgroup");
						break;
					case PRODUCT_ULTIMATE:
						osType = _T("Ultimate");
						break;
					case PRODUCT_ULTIMATE_N:
						osType = _T("Ultimate N");
						break;
					case PRODUCT_WEB_SERVER:
						osType = _T("Web Server");
						break;
					case PRODUCT_PROFESSIONAL:
						if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion <= 2)
						{
							osType = _T("Professional");
						}
						else
						{
							osType = _T("Pro");
						}
						break;
					case PRODUCT_PROFESSIONAL_N:
						if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion <= 2)
						{
							osType = _T("Professional N");
						}
						else
						{
							osType = _T("Pro N");
						}
						break;
					case PRODUCT_PRO_WORKSTATION:
						osType = _T("Pro for Workstation");
						break;
					case PRODUCT_PRO_WORKSTATION_N:
						osType = _T("Pro for Workstation N");
						break;
					case PRODUCT_PRO_FOR_EDUCATION:
						osType = _T("Pro for Education");
						break;
					case PRODUCT_PRO_FOR_EDUCATION_N:
						osType = _T("Pro for Education N");
						break;
					case PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL:
					case PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC:
					case PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC:
						osType = _T("Essentials Server");
						break;
					}
				}
			}
			else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
			{
				if (osvi.wProductType == VER_NT_WORKSTATION)
				{
					osType = _T("Professional");
				}
				else if (osvi.wProductType == VER_NT_SERVER)
				{
					if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
					{
						osType = _T("DataCenter Server");
					}
					else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
					{
						osType = _T("Advanced Server");
					}
					else
					{
						osType = _T("Server");
					}
				}
			}
			else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion >= 1)
			{
				if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
				{
					osType = _T("Home Edition");
				}
				else if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
				{
					osType = _T("Datacenter Edition");
				}
				else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
				{
					osType = _T("Enterprise Edition");
				}
				else if (osvi.wSuiteMask & VER_SUITE_BLADE)
				{
					osType = _T("Web Edition");
				}
				else if (osvi.wProductType == VER_NT_WORKSTATION)
				{
					osType = _T("Professional");
				}

				// Meida Center & Tablet
				if (GetSystemMetrics(SM_MEDIACENTER))
				{
					TCHAR path[MAX_PATH] = {};
					TCHAR str[256] = {};
					UINT length = GetWindowsDirectory(path, MAX_PATH);

#if _MSC_VER == 1310
					_tcscat(path, _T("\\ehome\\ehshell.exe"));
#else
					_tcscat_s(path, MAX_PATH, _T("\\ehome\\ehshell.exe"));
#endif				

					if (length != 0 && GetFileVersion(path, str))
					{
						cstr = str;
						if (cstr.Find(_T("5.1")) == 0)
						{
							osType = _T("Media Center ");
							cstr.Replace(_T("5.1."), _T(""));
							double num = _tstof(cstr);
							if (num <= 2600.1200)
							{
								osType += _T("2002");
							}
							else if (num <= 2600.2500)
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
				else if (GetSystemMetrics(SM_TABLETPC))
				{
					osType = _T("Tablet PC");
				}
			}
		}
	}

	osCsd = osvi.szCSDVersion;
	osCsd.Replace(_T("Service Pack "), _T("SP"));

	if (osvi.dwMajorVersion >= 10)
	{
		DWORD value = 0;
		DWORD type = REG_SZ;
		TCHAR str[256];
		ULONG size = 256 * sizeof(TCHAR);
		HKEY  hKey = NULL;

		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			if (RegQueryValueEx(hKey, _T("DisplayVersion"), NULL, &type, (LPBYTE)str, &size) == ERROR_SUCCESS)
			{
				osDisplayVersion = str;
			}

			if (osDisplayVersion.IsEmpty() && RegQueryValueEx(hKey, _T("ReleaseId"), NULL, &type, (LPBYTE)str, &size) == ERROR_SUCCESS)
			{
				osDisplayVersion = str;
			}
		}
	}

	osVersion.Format(_T("%d.%d"), osvi.dwMajorVersion, osvi.dwMinorVersion);
	osBuild.Format(_T("%d"), osvi.dwBuildNumber);

#if _MSC_VER > 1310
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
#else
	osArchitecture = _T("x86");
#endif

	if (!osNameWmi.IsEmpty())
	{
		if (!osDisplayVersion.IsEmpty())
		{
			osFullName.Format(_T("%s %s [%s Build %s] (%s)"), (LPCTSTR)osNameWmi, (LPCTSTR)osDisplayVersion, (LPCTSTR)osVersion, (LPCTSTR)osBuild, (LPCTSTR)osArchitecture);

		}
		else if (!osCsd.IsEmpty())
		{
			osFullName.Format(_T("%s %s [%s Build %s] (%s)"), (LPCTSTR)osNameWmi, (LPCTSTR)osCsd, (LPCTSTR)osVersion, (LPCTSTR)osBuild, (LPCTSTR)osArchitecture);
		}
		else
		{
			osFullName.Format(_T("%s [%s Build %s] (%s)"), (LPCTSTR)osNameWmi, (LPCTSTR)osVersion, (LPCTSTR)osBuild, (LPCTSTR)osArchitecture);
		}
	}
	else
	{
		if (!osDisplayVersion.IsEmpty())
		{
			osFullName.Format(_T("%s %s %s [%s Build %s] (%s)"), (LPCTSTR)osName, (LPCTSTR)osType, (LPCTSTR)osDisplayVersion, (LPCTSTR)osVersion, (LPCTSTR)osBuild, (LPCTSTR)osArchitecture);

		}
		else if (!osCsd.IsEmpty())
		{
			osFullName.Format(_T("%s %s %s [%s Build %s] (%s)"), (LPCTSTR)osName, (LPCTSTR)osType, (LPCTSTR)osCsd, (LPCTSTR)osVersion, (LPCTSTR)osBuild, (LPCTSTR)osArchitecture);
		}
		else if (!osType.IsEmpty())
		{
			osFullName.Format(_T("%s %s [%s Build %s] (%s)"), (LPCTSTR)osName, (LPCTSTR)osType, (LPCTSTR)osVersion, (LPCTSTR)osBuild, (LPCTSTR)osArchitecture);
		}
		else
		{
			osFullName.Format(_T("%s [%s Build %s] (%s)"), (LPCTSTR)osName, (LPCTSTR)osVersion, (LPCTSTR)osBuild, (LPCTSTR)osArchitecture);
		}	
	}
}

//------------------------------------------------
// Get OS Information by WMI
//------------------------------------------------

//warning : enum3, enum class
#if _MSC_VER > 1310
#pragma warning(disable : 26812)
#endif

#include <comdef.h>
#include <comutil.h>
#include <wbemcli.h>
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "wbemuuid.lib")

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#ifndef safeCloseHandle
#define safeCloseHandle(h) { if( h != NULL ) { ::CloseHandle(h); h = NULL; } }
#endif
#ifndef safeVirtualFree
#define safeVirtualFree(h,b,c) { if( h != NULL ) { ::VirtualFree(h, b, c); h = NULL; } }
#endif

void GetOsNameWmi(CString& osName)
{
	CString query = _T("Select * from Win32_OperatingSystem");
	
	IWbemLocator*			pIWbemLocator = NULL;
	IWbemServices*			pIWbemServices = NULL;
	IEnumWbemClassObject*	pEnumCOMDevs = NULL;
	IWbemClassObject*		pCOMDev = NULL;
	ULONG					uReturned = 0;
	BOOL					flag = FALSE;

	try
	{
		if (SUCCEEDED(CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER,
			IID_IWbemLocator, (LPVOID*)&pIWbemLocator)))
		{
			long securityFlag = 0;
#if _MSC_VER > 1310
			if (IsWindowsVersionOrGreaterFx(6, 0)) { securityFlag = WBEM_FLAG_CONNECT_USE_MAX_WAIT; }
#endif
			if (SUCCEEDED(pIWbemLocator->ConnectServer(_bstr_t(_T("root\\cimv2")),
				NULL, NULL, 0L, securityFlag, NULL, NULL, &pIWbemServices)))
			{
				if (SUCCEEDED(CoSetProxyBlanket(pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
					NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE)))
				{
					if (SUCCEEDED(pIWbemServices->ExecQuery(_bstr_t(_T("WQL")),
						_bstr_t(query), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumCOMDevs)))
					{
						while (pEnumCOMDevs && SUCCEEDED(pEnumCOMDevs->Next(10000, 1, &pCOMDev, &uReturned)) && uReturned == 1)
						{
							CString name;
							VARIANT pVal;
							VariantInit(&pVal);

							if (pCOMDev->Get(L"Caption", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
							{
								name = pVal.bstrVal;
								VariantClear(&pVal);
#ifdef UNICODE
								name.Replace(_T("™"), _T(""));
								name.Replace(_T("®"), _T(""));
#endif
								name.Replace(_T("Microsoft "), _T(""));
								name.Trim();
							}
							VariantInit(&pVal);

							osName = name;
						}
					}
				}
			}
		}
	}
	catch (...)
	{
		
	}

	SAFE_RELEASE(pCOMDev);
	SAFE_RELEASE(pEnumCOMDevs);
	SAFE_RELEASE(pIWbemServices);
	SAFE_RELEASE(pIWbemLocator);
}