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
