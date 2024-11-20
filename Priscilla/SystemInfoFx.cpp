/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "OsInfoFx.h"
#include "SystemInfoFx.h"
#include "UtilityFx.h"


//------------------------------------------------
// Get System Information by WMI
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

#if _MSC_VER > 1310
DWORD CountSetBits(ULONG_PTR bitMask) {
	DWORD LSHIFT = sizeof(ULONG_PTR) * 8 - 1;
	DWORD bitSetCount = 0;
	ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;
	for (DWORD i = 0; i <= LSHIFT; ++i) {
		bitSetCount += ((bitMask & bitTest) ? 1 : 0);
		bitTest >>= 1;
	}
	return bitSetCount;
}

typedef ULONGLONG(WINAPI* FuncGetLogicalProcessorInformationEx)(LOGICAL_PROCESSOR_RELATIONSHIP, PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, PDWORD);
#endif

void GetProcessorInfo(int* cores, int* threads)
{
	*cores = 0;
	*threads = 0;

#if _MSC_VER > 1310
	HMODULE hModule = GetModuleHandle(_T("kernel32.dll"));
	FuncGetLogicalProcessorInformationEx pGetLogicalProcessorInformationEx = NULL;

	if (hModule)
	{
		pGetLogicalProcessorInformationEx = (FuncGetLogicalProcessorInformationEx)GetProcAddress(hModule, "GetLogicalProcessorInformationEx");
	}

	// for Windows 7 or later
	if (pGetLogicalProcessorInformationEx != NULL)
	{
		DWORD length = 0;
		pGetLogicalProcessorInformationEx(RelationAll, nullptr, &length);

		BYTE* buffer = new BYTE[length];
		if (pGetLogicalProcessorInformationEx(RelationAll, reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer), &length)) {
			DWORD offset = 0;
			while (offset < length) {
				PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX info = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer + offset);

				if (info->Relationship == RelationProcessorCore) {
					*cores += 1;
					for (int group = 0; group < info->Processor.GroupCount; ++group) {
						*threads += CountSetBits(info->Processor.GroupMask[group].Mask);
					}
				}

				offset += info->Size;
			}
		}
		delete[] buffer;
	}
	// for Windows XP/Vista
	else
	{
		DWORD length = 0;
		GetLogicalProcessorInformation(NULL, &length);

		SYSTEM_LOGICAL_PROCESSOR_INFORMATION* buffer = new SYSTEM_LOGICAL_PROCESSOR_INFORMATION[length / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION)];
		GetLogicalProcessorInformation(&buffer[0], &length);

		for (DWORD i = 0; i != length / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); ++i) {
			if (buffer[i].Relationship == RelationProcessorCore) {
				*cores += 1;
				ULONG_PTR mask = buffer[i].ProcessorMask;
				while (mask) {
					if (mask & 1) {
						*threads += 1;
					}
					mask >>= 1;
				}
			}
		}
		delete[] buffer;
	}
#else
	SYSTEM_INFO si = {0};
	GetSystemInfo(&si);

	*cores = si.dwNumberOfProcessors;
	*threads = si.dwNumberOfProcessors;
#endif
}

#if defined(_M_IX86) || defined(_M_X64)

#if _MSC_VER > 1310
#include <intrin.h> // for __cpuid

void getProcessorBrandString(char* brandString) {

	__try
	{
		int cpuInfo[4] = { 0 };
		__cpuid(cpuInfo, 0x80000000);
		if (cpuInfo[0] >= 0x80000004) {
			for (int i = 0x80000002; i <= 0x80000004; ++i) {
				__cpuid(cpuInfo, i);
				memcpy(brandString + (i - 0x80000002) * 16, cpuInfo, sizeof(cpuInfo));
			}
		}
		else {
			strcpy(brandString, "");
		}	
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		strcpy(brandString, "");
	}
}
#else
void getCpuid(unsigned int param, unsigned int* _eax, unsigned int* _ebx, unsigned int* _ecx, unsigned int* _edx)
{
	unsigned int a, b, c, d; 
	__asm {
		MOV EAX, param
		CPUID
		MOV a, EAX
		MOV b, EBX
		MOV c, ECX
		MOV d, EDX
	}
	*_eax = a;
	*_ebx = b;
	*_ecx = c;
	*_edx = d;
}

void getProcessorBrandString(char* brandString)
{
	unsigned int eax = 0;
	unsigned int ebx = 0;
	unsigned int ecx = 0;
	unsigned int edx = 0;

	__try
	{
		// Check if CPUID supports brand string
		getCpuid(0x80000000, &eax, &ebx, &ecx, &edx);
		if (eax < 0x80000004) {
			strcpy(brandString, "");
			return;
		}

		// Get brand string
		for (int i = 0x80000002; i <= 0x80000004; ++i) {
			getCpuid(i, &eax, &ebx, &ecx, &edx);
			memcpy(brandString + (i - 0x80000002) * 16, &eax, 4);
			memcpy(brandString + (i - 0x80000002) * 16 + 4, &ebx, 4);
			memcpy(brandString + (i - 0x80000002) * 16 + 8, &ecx, 4);
			memcpy(brandString + (i - 0x80000002) * 16 + 12, &edx, 4);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		strcpy(brandString, "");
	}
}

#endif
#endif

void GetCpuInfo(CString& cpuInfo, CString& cpuName, int* clock, int* cores, int* threads)
{

#if _MSC_VER <= 1310
	if (!IsWin9x())
	{
#endif

	CString query = _T("Select * from Win32_Processor");

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
#if _MSC_VER > 1310
				if (SUCCEEDED(CoSetProxyBlanket(pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
					NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE)))
#endif
				{
					if (SUCCEEDED(pIWbemServices->ExecQuery(_bstr_t(_T("WQL")),
						_bstr_t(query), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumCOMDevs)))
					{
						while (pEnumCOMDevs && SUCCEEDED(pEnumCOMDevs->Next(10000, 1, &pCOMDev, &uReturned)) && uReturned == 1)
						{
							CString name;
							UINT32 speed = 0;
							// UINT32 cores = 0;
							// UINT32 threads = 0;

							VARIANT pVal;
							VariantInit(&pVal);
							if (pCOMDev->Get(L"Name", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
							{
								name = pVal.bstrVal;
								name.TrimLeft();
								name.TrimRight();
								cpuName = name;
								VariantClear(&pVal);
							}
							if (pCOMDev->Get(L"MaxClockSpeed", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
							{
								speed = pVal.intVal;
								if(clock != NULL) { *clock = (int)speed; }
								VariantClear(&pVal);
							}

							/*
							if (pCOMDev->Get(L"NumberOfCores", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
							{
								*cores = pVal.intVal;
								VariantClear(&pVal);
							}
							if (pCOMDev->Get(L"NumberOfLogicalProcessors", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
							{
								*threads = pVal.intVal;
								VariantClear(&pVal);
							}
							*/

							GetProcessorInfo(cores, threads);
							
							if (speed > 0 && *cores > 0 && *threads > 0)
							{
								if (*cores > 1 && *threads > 1)
								{
									cpuInfo.Format(_T("%s %dMHz (%dcores/%dthreads)"), (LPCTSTR)name, speed, *cores, *threads);
								}
								else if (*cores == 1 && *threads == 1)
								{
									cpuInfo.Format(_T("%s %dMHz (%dcore/%dthread)"), (LPCTSTR)name, speed, *cores, *threads);
								}
								else if (*cores == 1)
								{
									cpuInfo.Format(_T("%s %dMHz (%dcore/%dthreads)"), (LPCTSTR)name, speed, *cores, *threads);
								}
							}
							else if (*cores > 0 && *threads > 0)
							{
								if (*cores > 1 && *threads > 1)
								{
									cpuInfo.Format(_T("%s (%dcores/%dthreads)"), (LPCTSTR)name, *cores, *threads);
								}
								else if (*cores == 1 && *threads == 1)
								{
									cpuInfo.Format(_T("%s (%dcore/%dthread)"), (LPCTSTR)name, *cores, *threads);
								}
								else if (*cores == 1)
								{
									cpuInfo.Format(_T("%s (%dcore/%dthreads)"), (LPCTSTR)name, *cores, *threads);
								}
							}
							else
							{
								cpuInfo = name;
							}

							break;
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

#if _MSC_VER <= 1310
	}
#endif

	if (cpuInfo.IsEmpty())
	{
		TCHAR str[256] = {};
		DWORD value = 0;
		DWORD type = REG_SZ;
		ULONG size = 256 * sizeof(TCHAR);
		HKEY  hKey = NULL;

		if (! IsWin9x())
		{
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
			{
				if (RegQueryValueEx(hKey, _T("ProcessorNameString"), NULL, &type, (LPBYTE)str, &size) == ERROR_SUCCESS)
				{
					cpuInfo = str;
					cpuInfo.TrimLeft();
					cpuInfo.TrimRight();
				}
			}
		}

#if defined(_M_IX86) || defined(_M_X64)
		if (cpuInfo.IsEmpty())
		{
			char brandString[48] = { 0 };
			getProcessorBrandString(brandString);
			cpuInfo = brandString;
			cpuInfo.TrimLeft();
			cpuInfo.TrimRight();
		}
#endif

#ifdef _M_IX86
		if (cpuInfo.IsEmpty())
		{
			SYSTEM_INFO si = { 0 };
			GetSystemInfo(&si);

			switch (si.dwProcessorType)
			{
			case PROCESSOR_INTEL_386:
				cpuInfo = _T("Intel 386 Processor");
				break;
			case PROCESSOR_INTEL_486:
				cpuInfo = _T("Intel 486 Processor");
				break;
			case PROCESSOR_INTEL_PENTIUM:
				cpuInfo = _T("Intel Pentium Processor");
				break;
			case PROCESSOR_ARCHITECTURE_UNKNOWN:
			default:
				cpuInfo = _T("Unknown Processor");
				break;
			}			
		}
#endif
		cpuName = cpuInfo;

		if (!IsWin9x())
		{
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
			{
				type = REG_DWORD;
				size = sizeof(DWORD);
				if (RegQueryValueEx(hKey, _T("~MHz"), NULL, &type, (LPBYTE)&value, &size) == ERROR_SUCCESS)
				{
					cpuInfo.Format(_T("%s %dMHz"), (LPCTSTR)cpuInfo, value);
					if (clock != NULL) { *clock = (int)value; }
				}
				RegCloseKey(hKey);
			}
		}

		GetProcessorInfo(cores, threads);
		if (*cores > 0 && *threads > 0)
		{
			CString cstr;
			if (*cores > 1 && *threads > 1)
			{
				cstr.Format(_T(" (%dcores/%dthreads)"), *cores, *threads);
			}
			else if(*cores == 1 && *threads == 1)
			{
				cstr.Format(_T(" (%dcore/%dthread)"), *cores, *threads);
			}
			else if (*cores == 1)
			{
				cstr.Format(_T(" (%dcore/%dthreads)"), *cores, *threads);
			}
			
			cpuInfo += cstr;
		}		
	}

	cpuInfo.Replace(_T("(R)"), _T(""));
	cpuInfo.Replace(_T("(TM)"), _T(""));
}

void GetGpuInfo(CString& gpuInfo)
{
#if _MSC_VER > 1310

	#pragma comment(lib, "dxgi.lib")

	HMODULE hModule = LoadLibraryEx(_T("dxgi.dll"), NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
	typedef HRESULT(WINAPI* FuncCreateDXGIFactory)(REFIID, void**);
	FuncCreateDXGIFactory pCreateDXGIFactory = NULL;

	if (hModule != NULL)
	{
		pCreateDXGIFactory = (FuncCreateDXGIFactory)GetProcAddress(hModule, "CreateDXGIFactory");
	}

	if (pCreateDXGIFactory != NULL)
	{
		IDXGIFactory* pFactory = nullptr;
		if (SUCCEEDED(pCreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory)))
		{
			IDXGIAdapter* pAdapter = nullptr;
			DXGI_ADAPTER_DESC adapterDesc;

			for (UINT i = 0; pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
			{
				pAdapter->GetDesc(&adapterDesc);

				CString cstr;
				cstr.Format(L"%s [%d MB]", adapterDesc.Description, (int)(adapterDesc.DedicatedVideoMemory / (1024 * 1024)));

				if (cstr.Find(L"Microsoft Basic Render Driver") == 0)
				{
					pAdapter->Release();
					continue;
				}

				if (gpuInfo.IsEmpty())
				{
					gpuInfo = cstr;
				}
				else
				{
					gpuInfo += _T(" | ") + cstr;
				}
				pAdapter->Release();
			}
			pFactory->Release();
		}
	}

	if (! gpuInfo.IsEmpty())
	{
		gpuInfo.Replace(_T("(R)"), _T(""));
		gpuInfo.Replace(_T("(TM)"), _T(""));
		return;
	}
#endif

	CString query = _T("Select * from Win32_VideoController");

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
#if _MSC_VER > 1310
				if (SUCCEEDED(CoSetProxyBlanket(pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
					NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE)))
#endif
				{
					if (SUCCEEDED(pIWbemServices->ExecQuery(_bstr_t(_T("WQL")),
						_bstr_t(query), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumCOMDevs)))
					{
						while (pEnumCOMDevs && SUCCEEDED(pEnumCOMDevs->Next(10000, 1, &pCOMDev, &uReturned)) && uReturned == 1)
						{
							CString name;

							VARIANT pVal;
							VariantInit(&pVal);
							if (pCOMDev->Get(L"Name", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
							{
								name = pVal.bstrVal;
								name.TrimLeft();
								name.TrimRight();
								VariantClear(&pVal);
							}

							// exclusion
							// if (name.Find(_T("Virtual")) >= 0 || name.Find(_T("Remote")) >= 0 || name.Find(_T("ASPEED")) == 0)
							if (gpuInfo.IsEmpty())
							{
								gpuInfo = name;
							}
							else
							{
								gpuInfo += _T(" | ") + name;
							}
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

	if (gpuInfo.IsEmpty())
	{
		TCHAR str[256] = {};
		DWORD value = 0;
		DWORD type = REG_SZ;
		ULONG size = 256 * sizeof(TCHAR);
		HKEY  hKey = NULL;

		// GPU
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\0000"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			type = REG_SZ;
			size = 256 * sizeof(TCHAR);
			if (RegQueryValueEx(hKey, _T("DriverDesc"), NULL, &type, (LPBYTE)str, &size) == ERROR_SUCCESS)
			{
				gpuInfo = str;
				gpuInfo.TrimLeft();
				gpuInfo.TrimRight();
			}
			RegCloseKey(hKey);
		}
		if (gpuInfo.IsEmpty())
		{
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\0001"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
			{
				type = REG_SZ;
				size = 256 * sizeof(TCHAR);
				if (RegQueryValueEx(hKey, _T("DriverDesc"), NULL, &type, (LPBYTE)str, &size) == ERROR_SUCCESS)
				{
					gpuInfo = str;
					gpuInfo.TrimLeft();
					gpuInfo.TrimRight();
				}
				RegCloseKey(hKey);
			}
		}
	}

	gpuInfo.Replace(_T("(R)"), _T(""));
	gpuInfo.Replace(_T("(TM)"), _T(""));
}

void GetBaseBoardInfo(CString& baseBoardInfo)
{
	CString query = _T("Select * from Win32_BaseBoard");

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
#if _MSC_VER > 1310
				if (SUCCEEDED(CoSetProxyBlanket(pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
					NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE)))
#endif
				{
					if (SUCCEEDED(pIWbemServices->ExecQuery(_bstr_t(_T("WQL")),
						_bstr_t(query), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumCOMDevs)))
					{
						while (pEnumCOMDevs && SUCCEEDED(pEnumCOMDevs->Next(10000, 1, &pCOMDev, &uReturned)) && uReturned == 1)
						{
							CString manufacturer;
							CString product;

							VARIANT pVal;
							VariantInit(&pVal);
							if (pCOMDev->Get(L"Manufacturer", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
							{
								manufacturer = pVal.bstrVal;
								manufacturer.TrimLeft();
								manufacturer.TrimRight();
								VariantClear(&pVal);
							}
							if (pCOMDev->Get(L"Product", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
							{
								product = pVal.bstrVal;
								product.TrimLeft();
								product.TrimRight();
								VariantClear(&pVal);
							}
							baseBoardInfo = manufacturer + _T(" ") + product;

							baseBoardInfo.Replace(_T("To Be Filled By O.E.M."), _T(""));
							baseBoardInfo.Replace(_T("To be filled by O.E.M."), _T(""));
							baseBoardInfo.Replace(_T("Not Available"), _T(""));
							baseBoardInfo.TrimLeft();
							baseBoardInfo.TrimRight();
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

	if (baseBoardInfo.IsEmpty())
	{
		TCHAR str[256] = {};
		DWORD value = 0;
		DWORD type = REG_SZ;
		ULONG size = 256 * sizeof(TCHAR);
		HKEY  hKey = NULL;

		// BaseBoard/System
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DESCRIPTION\\System\\BIOS"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			size = 256 * sizeof(TCHAR);
			if (RegQueryValueEx(hKey, _T("BaseBoardManufacturer"), NULL, &type, (LPBYTE)str, &size) == ERROR_SUCCESS)
			{
				baseBoardInfo = str;

			}
			size = 256 * sizeof(TCHAR);
			if (RegQueryValueEx(hKey, _T("BaseBoardProduct"), NULL, &type, (LPBYTE)str, &size) == ERROR_SUCCESS)
			{
				baseBoardInfo += _T(" ");
				baseBoardInfo += str;
			}

			baseBoardInfo.Replace(_T("To Be Filled By O.E.M."), _T(""));
			baseBoardInfo.Replace(_T("To be filled by O.E.M."), _T(""));
			baseBoardInfo.Replace(_T("Not Available"), _T(""));
			baseBoardInfo.TrimLeft();
			baseBoardInfo.TrimRight();

			RegCloseKey(hKey);
		}
	}
}

void GetComputerSystemInfo(CString& computerSystemInfo)
{
	CString query = _T("Select * from Win32_ComputerSystem");

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
#if _MSC_VER > 1310
				if (SUCCEEDED(CoSetProxyBlanket(pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
					NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE)))
#endif
				{
					if (SUCCEEDED(pIWbemServices->ExecQuery(_bstr_t(_T("WQL")),
						_bstr_t(query), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumCOMDevs)))
					{
						while (pEnumCOMDevs && SUCCEEDED(pEnumCOMDevs->Next(10000, 1, &pCOMDev, &uReturned)) && uReturned == 1)
						{
							CString manufacturer;
							CString model;

							VARIANT pVal;
							VariantInit(&pVal);
							if (pCOMDev->Get(L"Manufacturer", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
							{
								manufacturer = pVal.bstrVal;
								manufacturer.TrimLeft();
								manufacturer.TrimRight();
								VariantClear(&pVal);
							}
							if (pCOMDev->Get(L"Model", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
							{
								model = pVal.bstrVal;
								model.TrimLeft();
								model.TrimRight();
								VariantClear(&pVal);
							}
							computerSystemInfo = manufacturer + _T(" ") + model;

							computerSystemInfo.Replace(_T("To Be Filled By O.E.M."), _T(""));
							computerSystemInfo.Replace(_T("To be filled by O.E.M."), _T(""));
							computerSystemInfo.Replace(_T("System manufacturer"), _T(""));
							computerSystemInfo.Replace(_T("System Product Name"), _T(""));
							computerSystemInfo.Replace(_T("Not Available"), _T(""));
							computerSystemInfo.TrimLeft();
							computerSystemInfo.TrimRight();
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

	if (computerSystemInfo.IsEmpty())
	{
		TCHAR str[256] = {};
		DWORD value = 0;
		DWORD type = REG_SZ;
		ULONG size = 256 * sizeof(TCHAR);
		HKEY  hKey = NULL;

		// BaseBoard/System
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DESCRIPTION\\System\\BIOS"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			size = 256 * sizeof(TCHAR);
			if (RegQueryValueEx(hKey, _T("SystemManufacturer"), NULL, &type, (LPBYTE)str, &size) == ERROR_SUCCESS)
			{
				computerSystemInfo = str;
			}
			size = 256 * sizeof(TCHAR);
			if (RegQueryValueEx(hKey, _T("SystemProductName"), NULL, &type, (LPBYTE)str, &size) == ERROR_SUCCESS)
			{
				computerSystemInfo += _T(" ");
				computerSystemInfo += str;
			}

			computerSystemInfo.Replace(_T("To Be Filled By O.E.M."), _T(""));
			computerSystemInfo.Replace(_T("To be filled by O.E.M."), _T(""));
			computerSystemInfo.Replace(_T("Not Available"), _T(""));
			computerSystemInfo.Replace(_T("System Product Name"), _T(""));
			computerSystemInfo.Replace(_T("System manufacturer"), _T(""));
			computerSystemInfo.TrimLeft();
			computerSystemInfo.TrimRight();

			RegCloseKey(hKey);
		}
	}
}

void GetScreenInfo(CString& screenInfo, int* width, int* height, int* color, CString& smoothing)
{
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	if(width != NULL){*width = screenWidth;}
	if(height != NULL){*height = screenHeight;}

	HDC hDC = ::GetDC(NULL);
	int bitsPerPixel = ::GetDeviceCaps(hDC, BITSPIXEL);
	int planes = ::GetDeviceCaps(hDC, PLANES);
	int colorDepth = bitsPerPixel * planes;
	if(color != NULL){*color = colorDepth;}
	::ReleaseDC(NULL, hDC);

	DWORD fontSmoothingType = 0;
	SystemParametersInfoW(SPI_GETFONTSMOOTHINGTYPE, 0, &fontSmoothingType, 0);
	screenInfo.Format(_T("%dx%d %dbit"), screenWidth, screenHeight, colorDepth);

	if (fontSmoothingType == FE_FONTSMOOTHINGSTANDARD)
	{
		screenInfo += _T(" (Smoothing)");
		smoothing = _T("Smoothing");
	}
	else if (fontSmoothingType == FE_FONTSMOOTHINGCLEARTYPE)
	{
		screenInfo += _T(" (ClearType)");
		smoothing = _T("ClearType");
	}
}

void GetMemoryInfo(CString& memoryInfo, int* size)
{
#if _MSC_VER > 1310
	MEMORYSTATUSEX memory = {};
	memory.dwLength = sizeof(memory);
	GlobalMemoryStatusEx(&memory);

	if ((int)(memory.ullTotalPhys / 1024 / 1024 / 1024 + 1) > 1)
	{
		memoryInfo.Format(_T("%.1f GB"), (memory.ullTotalPhys / 1024.0 / 1024 / 1024));

	}
	else
	{
		memoryInfo.Format(_T("%d MB"), (int)(memory.ullTotalPhys / 1024.0 / 1024 + 1));
	}
	if(size != NULL){*size = (int)(memory.ullTotalPhys / 1024.0 / 1024 + 1);}
#else

	// for Windows 2000
	typedef BOOL(WINAPI* FuncGlobalMemoryStatusEx)(LPMEMORYSTATUSEX);
	HMODULE hModule = GetModuleHandle(_T("kernel32.dll"));
	FuncGlobalMemoryStatusEx pGlobalMemoryStatusEx = NULL;
	if (hModule)
	{
		pGlobalMemoryStatusEx = (FuncGlobalMemoryStatusEx)GetProcAddress(hModule, "GlobalMemoryStatusEx");
	}

	if (pGlobalMemoryStatusEx != NULL) // for Windows 2000
	{
		MEMORYSTATUSEX memory;
		memory.dwLength = sizeof(memory);
		pGlobalMemoryStatusEx(&memory);

		if ((int)(memory.ullTotalPhys / 1024 / 1024 / 1024 + 1) > 1)
		{
			memoryInfo.Format(_T("%.1f GB"), (memory.ullTotalPhys / 1024.0 / 1024 / 1024));

		}
		else
		{
			memoryInfo.Format(_T("%d MB"), (int)(memory.ullTotalPhys / 1024.0 / 1024 + 1));
		}
		if (size != NULL) { *size = (int)(memory.ullTotalPhys / 1024.0 / 1024 + 1); }
	}
	else
	{
		MEMORYSTATUS memory;
		memory.dwLength = sizeof(memory);
		GlobalMemoryStatus(&memory);

		if ((int)(memory.dwTotalPhys / 1024 / 1024 / 1024 + 1) > 1)
		{
			memoryInfo.Format(_T("%.1f GB"), (memory.dwTotalPhys / 1024.0 / 1024 / 1024));

		}
		else
		{
			memoryInfo.Format(_T("%d MB"), (int)(memory.dwTotalPhys / 1024.0 / 1024 + 1));
}
		if (size != NULL) { *size = (int)(memory.dwTotalPhys / 1024.0 / 1024 + 1); }
	}
#endif
}