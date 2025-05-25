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
typedef BOOL(WINAPI* FuncGetLogicalProcessorInformation)(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD);

#endif

void GetProcessorInfo(int* cores, int* threads)
{
	*cores = 0;
	*threads = 0;

#if _MSC_VER > 1310
	HMODULE hModule = GetModuleHandle(_T("kernel32.dll"));
	FuncGetLogicalProcessorInformationEx pGetLogicalProcessorInformationEx = NULL;
	FuncGetLogicalProcessorInformation pGetLogicalProcessorInformation = NULL;

	if (hModule)
	{
		pGetLogicalProcessorInformationEx = (FuncGetLogicalProcessorInformationEx)GetProcAddress(hModule, "GetLogicalProcessorInformationEx");
		pGetLogicalProcessorInformation = (FuncGetLogicalProcessorInformation)GetProcAddress(hModule, "GetLogicalProcessorInformation");

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
	// for Windows XP SP3/Vista
	else if(pGetLogicalProcessorInformation != NULL)
	{
		DWORD length = 0;
		pGetLogicalProcessorInformation(NULL, &length);

		SYSTEM_LOGICAL_PROCESSOR_INFORMATION* buffer = new SYSTEM_LOGICAL_PROCESSOR_INFORMATION[length / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION)];
		pGetLogicalProcessorInformation(&buffer[0], &length);

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
	else // - Windows XP SP2
	{
		SYSTEM_INFO si = { 0 };
		GetSystemInfo(&si);

		*cores = si.dwNumberOfProcessors;
		*threads = si.dwNumberOfProcessors;
	}
#else
	SYSTEM_INFO si = {0};
	GetSystemInfo(&si);

	*cores = si.dwNumberOfProcessors;
	*threads = si.dwNumberOfProcessors;
#endif
}

#if defined(_M_IX86) || defined(_M_X64)

void getProcessorBrandString(char* brandString);
void getCpuName(char* cpuName);
unsigned int getCacheInfoIntel(int test);
CStringA getCpuModelName(CStringA vendor, unsigned int family, unsigned int model, unsigned int stepping, unsigned int type, unsigned int L2Cashe, unsigned int fpu);

#if _MSC_VER > 1310
#include <intrin.h> // for __cpuid

void GetCpuid(unsigned int param, unsigned int* _eax, unsigned int* _ebx, unsigned int* _ecx, unsigned int* _edx)
{
	int cpuInfo[4] = { 0 };
	__cpuid(cpuInfo, param);

	*_eax = cpuInfo[0];
	*_ebx = cpuInfo[1];
	*_ecx = cpuInfo[2];
	*_edx = cpuInfo[3];
}
#else
BOOL IsCpuidSupported()
{
	bool supported = false;
	__asm {
		pushfd
		pop     eax
		mov     ebx, eax
		xor eax, 0x200000
		push    eax
		popfd
		pushfd
		pop     eax
		xor eax, ebx
		test    eax, 0x200000
		jz      not_supported
		mov     supported, 1
		not_supported:
		push    ebx
		popfd
	}

	return supported;
}

void GetCpuid(unsigned int param, unsigned int* _eax, unsigned int* _ebx, unsigned int* _ecx, unsigned int* _edx)
{
	static BOOL bIsCpuid = IsCpuidSupported();
	if (!bIsCpuid) { return; }

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
#endif

void getProcessorBrandString(char* brandString)
{
	unsigned int eax = 0;
	unsigned int ebx = 0;
	unsigned int ecx = 0;
	unsigned int edx = 0;

	__try
	{
		// Check if CPUID supports brand string
		GetCpuid(0x80000000, &eax, &ebx, &ecx, &edx);
		if (eax < 0x80000004) {
			strcpy(brandString, "");
			return;
		}

		// Get brand string
		for (int i = 0x80000002; i <= 0x80000004; ++i) {
			GetCpuid(i, &eax, &ebx, &ecx, &edx);
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

void GetHypervisorVendorString(char* vendorString)
{
	unsigned int eax = 0;
	unsigned int ebx = 0;
	unsigned int ecx = 0;
	unsigned int edx = 0;

	__try
	{
		GetCpuid(0x40000000, &eax, &ebx, &ecx, &edx);
		memcpy(vendorString, &ebx, 4);
		memcpy(vendorString + 4, &ecx, 4);
		memcpy(vendorString + 8, &edx, 4);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		strcpy(vendorString, "");
	}
}

#if _MSC_VER <= 1310

BOOL Is486orAbove();
BOOL IsCyrixCPU();
BOOL IsAmd486();
BOOL readCcr(BYTE addr, BYTE* value);
CStringA getCyrixModelName();

inline unsigned char inpb(unsigned short port)
{
	unsigned char val;
	__asm {
		mov dx, port
		in  al, dx
		mov val, al
	}
	return val;
}

inline void outpb(unsigned short port, unsigned char data)
{
	__asm {
		mov dx, port
		mov al, data
		out dx, al
	}
}

// https://github.com/captainys/TOWNSEMU/issues/147#issuecomment-2764633838
BOOL IsFMTOWNS()
{
	static BOOL b = -1;
	if (b == -1)
	{
		b = FALSE;

		if (IsPC98())
		{
			return FALSE;
		}

		if (! IsWin95First())
		{
			return FALSE;
		}

		if (GetUserDefaultLCID() != 0x0411) // Japanese
		{
			return FALSE;
		}

		__try
		{
			BYTE in30 = (BYTE)inpb(0x30);
			BYTE in31 = (BYTE)inpb(0x31);
			if (in30 && in30 != 0xFF && in31 && in31 != 0xFF)
			{
				b = TRUE;
				return TRUE;
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return FALSE;
		}
	}

	return b;
}

BOOL Is486orAbove()
{
	BOOL result = FALSE;

	__asm {
		pushfd
		pop eax
		mov ecx, eax
		xor eax, 0x40000
		push eax
		popfd

		pushfd
		pop eax
		cmp eax, ecx
		jz is_not_80486

		mov result, 1
		jmp end

		is_not_80486 :
			mov result, 0
			end :
	}

	return result;
}

BOOL IsCyrixCPU()
{
	BOOL result = FALSE;

	__asm {
		xor ax, ax
		sahf
		mov ax, 5
		mov bx, 2
		div bl
		lahf
		cmp ah, 2
		jne not_cyrix

		mov result, 1
		jmp end

		not_cyrix:
			mov result, 0
			end:
	}

	return result;
}

BOOL IsAmd486()
{
	__try
	{
		outpb(0xC0, 0x55);
		BYTE val = (BYTE)inpb(0xC0);
		if (val == 0x55)
		{
			return TRUE;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return FALSE;
	}

	return FALSE;
}

BOOL readCcr(BYTE addr, BYTE* value)
{
	__try
	{
		outpb(0x22, addr);
		*value = inpb(0x23);
		return TRUE;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return FALSE;
	}
}

// http://www.bitsavers.org/components/cyrix/appnotes/Cyrix_CPU_Detection_Guide_1997.pdf
CStringA getCyrixModelName()
{
	BYTE dir0 = 0;

	if(! readCcr(0xFE, &dir0))
	{
		return "Cyrix Unknown CPU";
	}
		 if (0x00 <= dir0 && dir0 <= 0x1F) { return "Cyrix Cx486"; }
	else if (0x20 <= dir0 && dir0 <= 0x2F) { return "Cyrix 5x86"; }
	else if (0x30 <= dir0 && dir0 <= 0x3F) { return "Cyrix 6x86"; }
	else if (0x40 <= dir0 && dir0 <= 0x4F) { return "Cyrix MediaGX"; }
	else if (0x50 <= dir0 && dir0 <= 0x5F) { return "Cyrix 6x86MX/MII"; }
	else
	{
		 return "Cyrix Unknown CPU";
	}
}

#endif

void getCpuName(char* cpuName)
{
	unsigned int eax = 0;
	unsigned int ebx = 0;
	unsigned int ecx = 0;
	unsigned int edx = 0;
	unsigned int maxCpuId = 0;

#if _MSC_VER <= 1310
	if(! IsCpuidSupported())
	{
		CStringA modelName = "";

		if (! Is486orAbove())
		{
			modelName = "386 Generation Processor";
		}
		else if (IsFMTOWNS())
		{
			
		}
		else if (IsCyrixCPU())
		{
			modelName = "Cyrix Unknown CPU";
			if (IsWin9x() && !IsPC98() && !IsFMTOWNS()) // PC-98 and FM TOWNS are not supported. 
			{
				modelName = getCyrixModelName();
			}
		}
		else if (IsWin9x())
		{
			if (IsAmd486())
			{
				modelName = "AMD Am486/Am5x86";
			}
		}

		if (modelName.IsEmpty())
		{
			SYSTEM_INFO si = { 0 };
			GetSystemInfo(&si);

			switch (si.dwProcessorType)
			{
			case PROCESSOR_INTEL_386:
				modelName = "386 Generation Processor";
				break;
			case PROCESSOR_INTEL_486:
				modelName = "486 Generation Processor";
				break;
			case PROCESSOR_INTEL_PENTIUM:
				modelName = "586 Generation Processor";
				break;
			case PROCESSOR_ARCHITECTURE_UNKNOWN:
			default:
				modelName = "Unknown Processor";
				break;
			}
		}

		sprintf(cpuName, "%s", modelName.GetString());
		return;
	}
#endif

	char vendorString[13] = {0};
	GetCpuid(0x00, &eax, &ebx, &ecx, &edx);
	memcpy(vendorString, &ebx, 4);
	memcpy(vendorString + 4, &edx, 4);
	memcpy(vendorString + 8, &ecx, 4);

	GetCpuid(0x80000000, &eax, &ebx, &ecx, &edx);
	maxCpuId = eax;

	if (maxCpuId >= 0x80000004)
	{
		getProcessorBrandString(cpuName);
	}
	else 
	{
		CStringA vendor = vendorString;
		GetCpuid(0x1, &eax, &ebx, &ecx, &edx);

		unsigned int version = eax;
		unsigned int family = (version >> 8) & 0xF;
		unsigned int model = (version >> 4) & 0xF;
		unsigned int stepping = version & 0xF;
		unsigned int type = (version >> 12) & 0x3;

		unsigned int fpu = 1; // Under Construction

		unsigned int cacheL2 = 0;
		unsigned int temp = 0;
		if (vendor.Find("GenuineIntel") == 0)
		{
			GetCpuid(0x2, &eax, &ebx, &ecx, &edx);
			temp = getCacheInfoIntel((eax & 0xFF000000) >> 24); if (temp > 0) { cacheL2 = temp; }
			temp = getCacheInfoIntel((eax & 0x00FF0000) >> 16); if (temp > 0) { cacheL2 = temp; }
			temp = getCacheInfoIntel((eax & 0x0000FF00) >> 8);  if (temp > 0) { cacheL2 = temp; }
			temp = getCacheInfoIntel((eax & 0x000000FF) >> 0);  if (temp > 0) { cacheL2 = temp; }
			temp = getCacheInfoIntel((ebx & 0xFF000000) >> 24); if (temp > 0) { cacheL2 = temp; }
			temp = getCacheInfoIntel((ebx & 0x00FF0000) >> 16); if (temp > 0) { cacheL2 = temp; }
			temp = getCacheInfoIntel((ebx & 0x0000FF00) >> 8);  if (temp > 0) { cacheL2 = temp; }
			temp = getCacheInfoIntel((ebx & 0x000000FF) >> 0);  if (temp > 0) { cacheL2 = temp; }
			temp = getCacheInfoIntel((ecx & 0xFF000000) >> 24); if (temp > 0) { cacheL2 = temp; }
			temp = getCacheInfoIntel((ecx & 0x00FF0000) >> 16); if (temp > 0) { cacheL2 = temp; }
			temp = getCacheInfoIntel((ecx & 0x0000FF00) >> 8);  if (temp > 0) { cacheL2 = temp; }
			temp = getCacheInfoIntel((ecx & 0x000000FF) >> 0);  if (temp > 0) { cacheL2 = temp; }
			temp = getCacheInfoIntel((edx & 0xFF000000) >> 24); if (temp > 0) { cacheL2 = temp; }
			temp = getCacheInfoIntel((edx & 0x00FF0000) >> 16); if (temp > 0) { cacheL2 = temp; }
			temp = getCacheInfoIntel((edx & 0x0000FF00) >> 8);  if (temp > 0) { cacheL2 = temp; }
			temp = getCacheInfoIntel((edx & 0x000000FF) >> 0);  if (temp > 0) { cacheL2 = temp; }
		}

		if(maxCpuId >= 0x80000006)
		{
			GetCpuid(0x80000006, &eax, &ebx, &ecx, &edx);
			cacheL2 = ((ebx >> 12) & 0xF) * 256;
		}

		CStringA modelName = getCpuModelName(vendor, family, model, stepping, type, cacheL2, fpu);

		sprintf(cpuName, "%s", modelName.GetString());
	}
}

unsigned int getCacheInfoIntel(int test)
{
	unsigned int CacheL2 = 0;
	switch (test)
	{
	case 0x39:CacheL2 = 128;break;
	case 0x3A:CacheL2 = 192;break;
	case 0x3B:CacheL2 = 128;break;
	case 0x3C:CacheL2 = 256;break;
	case 0x3D:CacheL2 = 384;break;
	case 0x3E:CacheL2 = 512;break;
	case 0x41:CacheL2 = 128;break;
	case 0x42:CacheL2 = 256;break;
	case 0x43:CacheL2 = 512;break;
	case 0x44:CacheL2 = 1024;break;
	case 0x45:CacheL2 = 2048;break;
	case 0x48:CacheL2 = 3072;break;
	case 0x49:CacheL2 = 4096;break;
	case 0x4E:CacheL2 = 6144;break;
	case 0x78:CacheL2 = 1024;break;
	case 0x79:CacheL2 = 128;break;
	case 0x7A:CacheL2 = 256;break;
	case 0x7B:CacheL2 = 512;break;
	case 0x7C:CacheL2 = 1024;break;
	case 0x7D:CacheL2 = 2048;break;
	case 0x7E:CacheL2 = 256;break;
	case 0x7F:CacheL2 = 512;break;
	case 0x81:CacheL2 = 128;break;
	case 0x82:CacheL2 = 256;break;
	case 0x83:CacheL2 = 512;break;
	case 0x84:CacheL2 = 1024;break;
	case 0x85:CacheL2 = 2048;break;
	case 0x86:CacheL2 = 512;break;
	case 0x87:CacheL2 = 1024;break;
	default:;
	}

	return CacheL2;
}

CStringA getCpuModelName(CStringA vendor, unsigned int family, unsigned int model, unsigned int stepping, unsigned int type, unsigned int cacheL2, unsigned int fpu)
{
	CStringA modelName;
	int F = family;
	int M = model;
	int S = stepping;
	int CacheL2 = cacheL2;
	char* n = NULL;
	char* v = NULL;

	if (vendor.Find("GenuineIntel") == 0)
	{
		v = "Intel";
		switch (family)
		{
		case 0x06:
			switch (model)
			{
				/////////////
				// Cascades
				/////////////
			case 0xA:
				n = "Pentium III Xeon";
				break;
				///////////
				// Banias
				///////////
			case 9:
				if (CacheL2 == 1024) {
					n = "Pentium M";
				} else {
					n = "Celeron M";
				}
				break;
				///////////////
				// Coppermine
				///////////////
			case 8:
				if (CacheL2 >= 256) {
					n = "Pentium III";
				} else if (CacheL2 <= 128) {
					n = "Celeron";
				}
				break;
			///////////
			// Katmai
			///////////
			case 7:
				if (CacheL2 == 1024) {
					n = "Pentium III Xeon";
				} else {
					n = "Pentium III";
				}
				break;
			//////////////////////
			// Dixon & Mendocino
			//////////////////////
			case 6:
				if ((S == 0xA || S == 0xD) && CacheL2 == 256) {
					n = "Mobile Pentium II";
				} else if ((S == 0xA || S == 0xD) && CacheL2 == 128) {
					n = "Mobile Celeron";
				} else {
					n = "Celeron";
				}
				break;
			//////////////
			// Deschutes
			//////////////
			case 5:
				if (CacheL2 > 512) {
					n = "Pentium II Xeon";
				} else if (CacheL2 == 512 && type == 0x1) {
					n = "Pentium II OverDrive";
				} else if (CacheL2 == 512) {
					n = "Pentium II";
				} else if (CacheL2 == 0) {
					n = "Celeron";
				}
				break;
			case 4:
				n = "OverDrive";
				break;
			case 3:
				if (type == 0x1) {
					n = "Pentium II OverDrive";
				} else {
					n = "Pentium II";
				}
				break;
			///////
			// P6
			///////
			case 2:
				n = "Pentium Pro";
				break;
			case 1:
				n = "Pentium Pro";
				break;
			case 0:
				n = "Pentium Pro";
				break;
			}
			break;
		////////////
		// Pentium 
		////////////
		case 5:
			switch (model) {
			case 8:
				n = "Pentium MMX";
				break;
			case 7:
				n = "Pentium";
				break;
			case 6:
				n = "Pentium OverDrive";
				break;
			case 5:
				n = "Pentium OverDrive";
				break;
			case 4:
				if (type == 1) {
					n = "Pentium MMX OverDrive";
				} else {
					n = "Pentium MMX";
				}
				break;
			case 3:
				n = "Pentium OverDrive";
				break;
			case 2:
				if (type == 1) {
					n = "Pentium OverDrive";
				} else {
					n = "Pentium";
				}
				break;
			case 1:
			case 0:
				if (type == 1) {
					n = "Pentium OverDrive";
				} else {
					n = "Pentium";
				}
				break;
			default:
				n = "Pentium";
				break;
			}
			break;
		////////
		// 486
		////////
		case 4:
			switch (model) {
			case 9:
				n = "486DX4 WB";
				break;
			case 8:
				n = "486DX4";
				break;
			case 7:
				n = "486DX2 WB";
				break;
			case 5:
				n = "486SX2";
				break;
			case 4:
				n = "486SL";
				break;
			case 3:
				n = "486DX2";
				break;
			case 2:
				n = "486SX";
				break;
			case 1:
			case 0:
				n = "486DX";
				break;
			default:
				n = "486";
				break;
			}
			break;
		case 3:
			n = "386";
			break;
		default:
			n = "Unknown CPU";
			break;
		}
	}
	else if (vendor.Find("AuthenticAMD") == 0)
	{
		v = "AMD";
		switch (family)
		{
		case 5:
			switch (model) {
			case 0xD:
			case 0xC:
				if (CacheL2 == 256) {
					n = "K6-III+";
				} else {
					n = "K6-2+";
				}
				break;
			case 0xA:
				n = "Geode LX";
				break;
			case 9:
				n = "K6-III";
				break;
			case 8:
				n = "K6-2";
				break;
			case 7:
				n = "K6";
				break;
			case 3:
			case 2:
			case 1:
			case 0:
				n = "K5";
				break;
			default:
				n = "Unknown CPU";
				break;
			}
			break;
		case 4:
			switch (model) {
			case 0xF:
			case 0xE:
				n = "Am5x86";
				break;
			case 9:
			case 8:
				n = "Am486DX4/Am5x86";
				break;
			case 7:
			case 3:
				if (fpu)
				{
					n = "Am486DX2";
				}
				else
				{
					n = "Am486SX2";
				}
				break;
			default:
				n = "Unknown CPU";
				break;
			}
			break;
		default:
			n = "Unknown CPU";
			break;
		}
	}
	else if (vendor.Find("CentaurHauls") == 0 || vendor.Find("VIA VIA VIA") == 0)
	{
		v = "";
		switch (family)
		{
		case 6:
			switch (model)
			{
			case 0xF:
				n = "VIA Nano";
				break;
			case 0xD:
			case 0xC:
			case 0xB:
			case 0xA:
				n = "VIA C7";
				break;
			case 9:
			case 8:
			case 7:
				n = "VIA C3";
				break;
			case 6:
				n = "VIA Cyrix III";
				break;
			case 5:
				if (stepping == 0)
				{
					n = "VIA 6x86MX";
				}
				else
				{
					n = "VIA Cyrix III";
				}				
				break;
			case 4:
				n = "VIA Cyrix III";
				break;
			case 3:
				n = "VIA 6x86MX";
				break;
			case 2:
				n = "VIA Cyrix III";
				break;
			case 0:
				n = "VIA 6x86MX";
				break;
			default:
				n = "VIA Unknown CPU";
				break;
			}
			break;
		case 5:
			switch (model)
			{
			case 9:
				n = "IDT WinChip 3";
				break;
			case 8:
				n = "IDT WinChip 2";
				break;
			case 4:
				n = "IDT WinChip C6";
				break;
			default:
				n = "VIA Unknown CPU";
				break;
			}
			break;
		default:
			n = "VIA Unknown CPU";
			break;
		}
	}
	else if (vendor.Find("CyrixInstead") == 0)
	{
		v = "Cyrix";
		switch (family)
		{
		case 6:
			switch (model)
			{
			case 0:
				n = "MII";
				break;
			default:
				n = "Unknown CPU";
				break;
			}
			break;
		case 5:
			switch (model)
			{
			case 9:
				n = "Geode";
				break;
			case 4:
				n = "MediaGX";
				break;
			case 2:
				n = "6x86";
				break;
			default:
				n = "Unknown CPU";
				break;
			}
			break;
		case 4:
			switch (model)
			{
			case 4:
				n = "MediaGX";
				break;
			default:
				n = "Unknown CPU";
				break;
			}
			break;
		default:
			n = "Unknown CPU";
			break;
		}
	}
	else if (vendor.Find("SiS SiS SiS") == 0)
	{
		v = "SiS";
		n = "55x";
	}
	else if (vendor.Find("Geode by NSC") == 0)
	{
		v = "NSC";
		n = "Geode";
	}
	else if (vendor.Find("NexGenDriven") == 0)
	{
		v = "NexGen";
		n = "Nx586";
	}
	else if (vendor.Find("RiseRiseRise") == 0)
	{
		v = "Rise";
		n = "mP6";
	}
	else if (vendor.Find("UMC UMC UMC") == 0)
	{
		v = "UMC";
		n = "Green CPU";
	}
	else
	{
		v = NULL;
		n = NULL;
	}

	if (v != NULL && n != NULL)
	{
		modelName.Format("%s %s", v, n);
		return modelName;
	}
	else
	{
		return "";
	}	
}
#endif

void GetCpuInfo(CString& cpuInfo, CString& cpuName, int* clock, int* cores, int* threads)
{
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

#if defined(_M_IX86) || defined(_M_X64)
							char brandString[49] = { 0 };
							getCpuName(brandString);
							//getProcessorBrandString(brandString);
							name = brandString;
							name.TrimLeft();
							name.TrimRight();
							cpuName = name;
#endif
							VARIANT pVal;
							VariantInit(&pVal);
							if (name.IsEmpty() && pCOMDev->Get(L"Name", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
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

	if (cpuInfo.IsEmpty())
	{
		TCHAR str[256] = {};
		DWORD value = 0;
		DWORD type = REG_SZ;
		ULONG size = 256 * sizeof(TCHAR);
		HKEY  hKey = NULL;

#if defined(_M_IX86) || defined(_M_X64)
		char brandString[49] = { 0 };
		getCpuName(brandString);
		//getProcessorBrandString(brandString);
		cpuInfo = brandString;
		cpuInfo.TrimLeft();
		cpuInfo.TrimRight();
#endif

		if (cpuInfo.IsEmpty() && !IsWin9x())
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

#ifdef _M_IX86

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

	cpuName.Replace(_T("(R)"), _T(""));
	cpuName.Replace(_T("(r)"), _T(""));
	cpuName.Replace(_T("(TM)"), _T(""));
	cpuName.Replace(_T("(tm)"), _T(""));
	cpuInfo.Replace(_T("(R)"), _T(""));
	cpuInfo.Replace(_T("(TM)"), _T(""));
	cpuInfo.Replace(_T("(t)"), _T(""));
	cpuInfo.Replace(_T("(tm)"), _T(""));
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
				cstr.Format(L"%s [%dMB]", adapterDesc.Description, (int)(adapterDesc.DedicatedVideoMemory / (1024 * 1024)));

				if (cstr.Find(L"Microsoft Basic Render Driver") == 0)
				{
					pAdapter->Release();
					continue;
				}

				if (gpuInfo.IsEmpty())
				{
					gpuInfo = cstr;
				}
				else if (gpuInfo.Find(cstr) >= 0)
				{
					// Duplication
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

#ifndef UNICODE
	if (IsWin9x())
	{
		TCHAR str[256] = { 0 };
		GetPrivateProfileStringFx(_T("boot.description"), _T("display.drv"), _T(""), str, 256, _T("system.ini"));
		gpuInfo = str;
	}
#endif

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
		if (gpuInfo.IsEmpty())
		{
			TCHAR str[256] = {};
			DWORD value = 0;
			DWORD type = REG_SZ;
			ULONG size = 256 * sizeof(TCHAR);
			HKEY  hKey = NULL;
			HKEY  hKey2 = NULL;
			type = REG_SZ;
			size = 256 * sizeof(TCHAR);
			CString videoKey, key;
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\VIDEO"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
			{
				for (int i = 0; i < 10; i++)
				{
					videoKey.Format(_T("\\Device\\Video%d"), i);
					key = _T("");
					if (RegQueryValueEx(hKey, videoKey, NULL, &type, (LPBYTE)str, &size) == ERROR_SUCCESS)
					{
						key = str;
					}

					if (!key.IsEmpty())
					{
						key.MakeLower().Replace(_T("\\registry\\machine\\"), _T(""));
						if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, key, 0, KEY_READ, &hKey2) == ERROR_SUCCESS)
						{
							type = REG_SZ;
							size = 256 * sizeof(TCHAR);
							if (RegQueryValueEx(hKey2, _T("DriverDesc"), NULL, &type, (LPBYTE)str, &size) == ERROR_SUCCESS)
							{
								CString cstr = str;
								cstr.TrimLeft();
								cstr.TrimRight();

								if (gpuInfo.IsEmpty())
								{
									gpuInfo = cstr;
								}
								else
								{
									gpuInfo += _T(" | ") + cstr;
								}
								RegCloseKey(hKey2);
								break;
							}
							
							type = REG_BINARY;
							if (RegQueryValueEx(hKey2, _T("HardwareInformation.AdapterString"), NULL, &type, (LPBYTE)str, &size) == ERROR_SUCCESS)
							{
							#ifndef UNICODE
								char buf[256] = {};
								WideCharToMultiByte(CP_ACP, 0, (WCHAR*)str, size, buf, 256, 0, 0);
								CString cstr = buf;
							#else
								CString cstr = str;
							#endif
								cstr.TrimLeft();
								cstr.TrimRight();

								if (gpuInfo.IsEmpty())
								{
									gpuInfo = cstr;
								}
								else
								{
									gpuInfo += _T(" | ") + cstr;
								}
								RegCloseKey(hKey2);
								break;
							}
							else
							{
								_tcscpy(str, key);
								PathRemoveFileSpec(str);
								CString cstr = PathFindFileName(str);
								cstr += _T(" compatible device");
								if (gpuInfo.IsEmpty())
								{
									gpuInfo = cstr;
								}
								else
								{
									gpuInfo += _T(" | ") + cstr;
								}
								RegCloseKey(hKey2);
								break;
							}
						}
					}
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

#if _MSC_VER <= 1310
	if (IsPC98())
	{
		computerSystemInfo = _T("[PC-98] ") + computerSystemInfo;
	}
	else if (IsFMTOWNS())
	{
		computerSystemInfo = _T("[FM TOWNS] ") + computerSystemInfo;
	}
#endif
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
		memoryInfo.Format(_T("%.1fGB"), (memory.ullTotalPhys / 1024.0 / 1024 / 1024));

	}
	else
	{
		memoryInfo.Format(_T("%dMB"), (int)(memory.ullTotalPhys / 1024.0 / 1024 + 1));
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
			memoryInfo.Format(_T("%.1fGB"), (memory.ullTotalPhys / 1024.0 / 1024 / 1024));

		}
		else
		{
			memoryInfo.Format(_T("%dMB"), (int)(memory.ullTotalPhys / 1024.0 / 1024 + 1));
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
			memoryInfo.Format(_T("%.1fGB"), (memory.dwTotalPhys / 1024.0 / 1024 / 1024));

		}
		else
		{
			memoryInfo.Format(_T("%dMB"), (int)(memory.dwTotalPhys / 1024.0 / 1024 + 1));
}
		if (size != NULL) { *size = (int)(memory.dwTotalPhys / 1024.0 / 1024 + 1); }
	}
#endif
}

#if _MSC_VER <= 1310
/// https://www.betaarchive.com/wiki/index.php/Microsoft_KB_Archive/124207
BOOL IsCoProcessorPresent()
{
#define MY_ERROR_WRONG_OS 0x20000000
	HKEY hKey;
	SYSTEM_INFO SystemInfo;

	// return FALSE if we are not running under Windows NT
	// this should be expanded to cover alternative Win32 platforms

	if (!(GetVersion() & 0x7FFFFFFF))
	{
		SetLastError(MY_ERROR_WRONG_OS);
		return(FALSE);
	}

	// we return TRUE if we're not running on x86
	// other CPUs have built in floating-point, with no registry entry

	GetSystemInfo(&SystemInfo);

	if ((SystemInfo.dwProcessorType != PROCESSOR_INTEL_386) &&
		(SystemInfo.dwProcessorType != PROCESSOR_INTEL_486) &&
		(SystemInfo.dwProcessorType != PROCESSOR_INTEL_PENTIUM))
	{
		return(TRUE);
	}

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DESCRIPTION\\System\\FloatingPointProcessor"),
		0, KEY_EXECUTE,	&hKey) != ERROR_SUCCESS)
	{
		// GetLastError() will indicate ERROR_RESOURCE_DATA_NOT_FOUND
		// if we can't find the key.  This indicates no coprocessor present
		return(FALSE);
	}

	RegCloseKey(hKey);
	return(TRUE);
}
#endif

