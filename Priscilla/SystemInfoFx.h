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

void GetCpuInfo(CString& cpuInfo, int* cores, int* threads);
void GetGpuInfo(CString& gpuInfo);
void GetBaseBoardInfo(CString& baseBoardInfo);
void GetComputerSystemInfo(CString& computerSystemInfo);
void GetScreenInfo(CString& screenInfo);
void GetMemoryInfo(CString& screenInfo);