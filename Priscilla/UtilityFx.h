/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
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
void GetFileVersionEx(const TCHAR* file, CString& version);
BOOL IsFileExist(const TCHAR* fileName);
BOOL CanWriteFile(const TCHAR* fileName);

////------------------------------------------------
//   Utility
////------------------------------------------------

ULONGLONG GetTickCountFx();

ULONG64 B8toB64(BYTE b0, BYTE b1 = 0, BYTE b2 = 0, BYTE b3 = 0, BYTE b4 = 0, BYTE b5 = 0, BYTE b6 = 0, BYTE b7 = 0);
DWORD B8toB32(BYTE b0, BYTE b1 = 0, BYTE b2 = 0, BYTE b3 = 0);
void SplitCString(const CString& str, const CString& delimiter, CStringArray& arr);


// ---------------------------------------------------------
// 20260123: Safe for unaligned/page-boundary (Using memcpy for atomic-like MOV) >>>

#ifndef NODISCARD
  #if (defined(__cplusplus) && __cplusplus >= 201703L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
    #define NODISCARD [[nodiscard]]
  #elif defined(_MSC_VER) && (_MSC_VER >= 1700)
    #define NODISCARD _Check_return_
  #else
    #define NODISCARD
  #endif
#endif

// SAL annotation support
#ifndef _In_reads_
  #ifdef _MSC_VER
    #include <sal.h>
  #endif
  //without SAL
  #ifndef _In_reads_
    #define _In_reads_(s)
  #endif
#endif

/* 8byte(le) to ULONG64 (Safe for unaligned/page-boundary) */
NODISCARD ULONG64 B8toB64le_ptr(_In_reads_(8) const BYTE* v) noexcept;
/* 4byte(le) to DWORD (Safe for unaligned/page-boundary) */
NODISCARD DWORD B8toB32le_ptr(_In_reads_(4) const BYTE* v) noexcept;
/* 4byte(le) to INT (Controlled sign extension) */
NODISCARD INT B8toINTle(_In_reads_(4) const BYTE* v) noexcept;
/* 2byte(le) to USHORT (Safe for unaligned/page-boundary) */
NODISCARD USHORT B8toB16le_ptr(_In_reads_(2) const BYTE* v) noexcept;
/* 2byte(le) to signed SHORT (Controlled sign extension) */
NODISCARD SHORT B8toSHORTle_ptr(_In_reads_(2) const BYTE* v) noexcept;
/* 6byte(le) to ULONG64 (Safe for page-boundary) */
NODISCARD ULONG64 B8toB64le(const BYTE(&v)[6]) noexcept;
/* 6byte(le) to DWORD (Safe for page-boundary) */
NODISCARD DWORD B8toB32le(const BYTE(&v)[6]) noexcept;
/* 6byte(le) to INT (Controlled sign extension) */
NODISCARD INT B8toINTle(const BYTE(&v)[6]) noexcept;
/* 6byte(le) to USHORT (Safe for page-boundary) */
NODISCARD USHORT B8toB16le(const BYTE(&v)[6]) noexcept;


// 20260123: Safe for unaligned/page-boundary <<<
// ---------------------------------------------------------




////------------------------------------------------
//   .ini support function
////------------------------------------------------

DWORD GetPrivateProfileStringFx(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpDefault, LPTSTR  lpReturnedString,DWORD nSize,LPCTSTR lpFileName);
UINT GetPrivateProfileIntFx(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nDefault, LPCTSTR lpFileName);
BOOL WritePrivateProfileStringFx(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpString, LPCTSTR lpFileName);

////------------------------------------------------
//   Check CodeSign
////------------------------------------------------

#if _MSC_VER > 1310
BOOL CheckCodeSign(LPCWSTR certName, LPCWSTR filePath);
#endif

////------------------------------------------------
//   Play Sound
////------------------------------------------------

BOOL AlertSound(const CString& alertSoundPath, int volume);

////------------------------------------------------
//   Hash
////------------------------------------------------

CStringA MD5(const CStringA& str);

////------------------------------------------------
//   Character Converter
////------------------------------------------------

CStringW UTF8toUTF16(const CStringA& utf8str);
CStringA UTF16toUTF8(const CStringW& utf16str);
CStringA URLEncode(const CStringA& str);
CStringA UE(const CStringW& utf16str);
CStringA UE(const CStringA& ansiStr);

////------------------------------------------------
//   Clipboard
////------------------------------------------------

void SetClipboardText(CString clip);