/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#pragma once

#include <winnt.h>

//------------------------------------------------
//   System Info
//------------------------------------------------

void GetCpuInfo(CString& cpuInfo, CString& cpuName, int* clock, int* cores, int* threads);
void GetGpuInfo(CString& gpuInfo);
void GetBaseBoardInfo(CString& baseBoardInfo);
void GetComputerSystemInfo(CString& computerSystemInfo);
void GetScreenInfo(CString& screenInfo, int* width, int* height, int* color, CString& smoothing);
void GetMemoryInfo(CString& screenInfo, int* size = NULL);

#if defined(_M_IX86) || defined(_M_X64)
void GetCpuid(unsigned int param, unsigned int* _eax, unsigned int* _ebx, unsigned int* _ecx, unsigned int* _edx);
void GetHypervisorVendorString(char* vendorString);
#endif

#if _MSC_VER <= 1310
/// https://www.betaarchive.com/wiki/index.php/Microsoft_KB_Archive/124207
BOOL IsCoProcessorPresent();
BOOL IsFMTOWNS();
#endif