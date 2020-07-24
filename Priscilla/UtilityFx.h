/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once

////------------------------------------------------
//   Debug
////------------------------------------------------

void SetDebugMode(DWORD mode);
void DebugPrint(CString cstr);

////------------------------------------------------
//   File Information
////------------------------------------------------

int GetFileVersion(const TCHAR* fileName, TCHAR* version = NULL);
BOOL IsFileExist(const TCHAR* fileName);

////------------------------------------------------
//   Utility
////------------------------------------------------

ULONGLONG GetTickCountFx();

ULONG64 B8toB64(BYTE b0, BYTE b1 = 0, BYTE b2 = 0, BYTE b3 = 0, BYTE b4 = 0, BYTE b5 = 0, BYTE b6 = 0, BYTE b7 = 0);
DWORD B8toB32(BYTE b0, BYTE b1 = 0, BYTE b2 = 0, BYTE b3 = 0);
