/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "UtilityFx.h"

#include <io.h>
#pragma comment(lib,"version.lib")

////------------------------------------------------
//   Debug
////------------------------------------------------

static const DWORD DEBUG_MODE_NONE = 0;
static const DWORD DEBUG_MODE_LOG = 1;
static const DWORD DEBUG_MODE_MESSAGE = 2;

static DWORD debugMode = DEBUG_MODE_NONE;

void SetDebugMode(DWORD mode)
{
	if (mode <= DEBUG_MODE_MESSAGE)
	{
		debugMode = mode;
	}
	else
	{
		debugMode = DEBUG_MODE_NONE;
	}
}

void DebugPrint(CString cstr)
{
	static BOOL flag = TRUE;
	static TCHAR file[MAX_PATH] = _T("");
	static DWORD first = (DWORD)GetTickCountFx();
	CString output;

	output.Format(_T("%08d "), (DWORD)GetTickCountFx() - first);
	output += cstr;
	output.Append(_T("\n"));
	output.Replace(_T("\r"), _T(""));

	if (flag)
	{
		TCHAR* ptrEnd;
		::GetModuleFileName(NULL, file, MAX_PATH);
		if ((ptrEnd = _tcsrchr(file, '.')) != NULL)
		{
			*ptrEnd = '\0';
#if _MSC_VER <= 1310
			_tcscat(file, _T(".log"));
#else
			_tcscat_s(file, MAX_PATH, _T(".log"));
#endif		
		}
		DeleteFile(file);
		flag = FALSE;
	}

	if (debugMode == DEBUG_MODE_NONE)
	{
		return;
	}

	FILE* fp = NULL;
#if _MSC_VER <= 1310
	fp = _tfopen(file, _T("ac"));
#else
	_tfopen_s(&fp, file, _T("ac"));
#endif
	
	if (fp != NULL)
	{
		_ftprintf(fp, _T("%s"), (LPCTSTR)output);
		fflush(fp);
		fclose(fp);
	}

	if (debugMode == DEBUG_MODE_MESSAGE)
	{
		AfxMessageBox(output);
	}
}

////------------------------------------------------
//   File Information
////------------------------------------------------

int GetFileVersion(const TCHAR* file, TCHAR* version)
{
	ULONG reserved = 0;
	VS_FIXEDFILEINFO vffi = { 0 };
	TCHAR* buf = NULL;
	int  Locale = 0;
	TCHAR str[256] = { 0 };

	UINT size = GetFileVersionInfoSize((TCHAR*)file, &reserved);
	TCHAR* vbuf = new TCHAR[size];
	if (GetFileVersionInfo((TCHAR*)file, 0, size, vbuf))
	{
		VerQueryValue(vbuf, _T("\\"), (void**)&buf, &size);
		CopyMemory(&vffi, buf, sizeof(VS_FIXEDFILEINFO));

		VerQueryValue(vbuf, _T("\\VarFileInfo\\Translation"), (void**)&buf, &size);
		CopyMemory(&Locale, buf, sizeof(int));
		wsprintf(str, _T("\\StringFileInfo\\%04X%04X\\%s"),
			LOWORD(Locale), HIWORD(Locale), _T("FileVersion"));
		VerQueryValue(vbuf, str, (void**)&buf, &size);

#if _MSC_VER <= 1310
		_tcscpy(str, buf);
#else
		_tcscpy_s(str, 256, buf);
#endif
		
		if (version != NULL)
		{
#if _MSC_VER <= 1310
			_tcscpy(version,buf);
#else
			_tcscpy_s(version, 256, buf);
#endif
		}
	}
	delete[] vbuf;

	if (_tcscmp(str, _T("")) != 0)
	{
		return int(_tstof(str) * 100);
	}
	else
	{
		return 0;
	}
}

void GetFileVersionEx(const TCHAR* file, CString& version)
{
	ULONG reserved = 0;
	VS_FIXEDFILEINFO vffi = { 0 };
	TCHAR* buf = NULL;
	int  Locale = 0;
	TCHAR str[256] = { 0 };

	UINT size = GetFileVersionInfoSize((TCHAR*)file, &reserved);
	TCHAR* vbuf = new TCHAR[size];
	if (GetFileVersionInfo((TCHAR*)file, 0, size, vbuf))
	{
		VerQueryValue(vbuf, _T("\\"), (void**)&buf, &size);
		CopyMemory(&vffi, buf, sizeof(VS_FIXEDFILEINFO));

		if (vffi.dwSignature == 0xfeef04bd)
		{
			version.Format(_T("%d.%d.%d.%d"),
				HIWORD(vffi.dwFileVersionMS),
				LOWORD(vffi.dwFileVersionMS),
				HIWORD(vffi.dwFileVersionLS),
				LOWORD(vffi.dwFileVersionLS));
		}
	}
	delete[] vbuf;
}

BOOL IsFileExist(const TCHAR* path)
{
	FILE* fp = NULL;

#if _MSC_VER <= 1310
	fp = _tfopen(path, _T("rb"));
	if (fp == NULL)
#else
	errno_t err = 0;
	err = _tfopen_s(&fp, path, _T("rb"));
	if (err != 0 || fp == NULL)
#endif
	{
		return FALSE;
	}
	fclose(fp);
	return TRUE;
}

BOOL CanWriteFile(const TCHAR* path)
{
	HANDLE hFile = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_DELETE_ON_CLOSE, NULL);

	if (hFile == INVALID_HANDLE_VALUE) { return FALSE; }
	const char* testData = "1";
	DWORD bytesWritten;
	BOOL writeResult = WriteFile(hFile, testData, (DWORD)strlen(testData), &bytesWritten, NULL);
	CloseHandle(hFile);

	return writeResult;
}


////------------------------------------------------
//   Utility
////------------------------------------------------

typedef ULONGLONG(WINAPI* FuncGetTickCount64)();

ULONGLONG GetTickCountFx()
{
	HMODULE hModule = GetModuleHandle(_T("kernel32.dll"));
	FuncGetTickCount64 pGetTickCount64 = NULL;
	if (hModule)
	{
		pGetTickCount64 = (FuncGetTickCount64)GetProcAddress(hModule, "GetTickCount64");
	}

	if (pGetTickCount64)
	{
		return (ULONGLONG)pGetTickCount64();
	}
	else
	{
#if _MSC_VER <= 1310
		return (ULONGLONG)GetTickCount();
#else
		return _Pragma("warning(suppress: 28159)") (ULONGLONG)GetTickCount();
#endif
	}
}

ULONG64 B8toB64(BYTE b0, BYTE b1, BYTE b2, BYTE b3, BYTE b4, BYTE b5, BYTE b6, BYTE b7)
{
	ULONG64 data =
		  ((ULONG64)b7 << 56)
		+ ((ULONG64)b6 << 48)
		+ ((ULONG64)b5 << 40)
		+ ((ULONG64)b4 << 32)
		+ ((ULONG64)b3 << 24)
		+ ((ULONG64)b2 << 16)
		+ ((ULONG64)b1 <<  8)
		+ ((ULONG64)b0 <<  0);

	return data;
}

DWORD B8toB32(BYTE b0, BYTE b1, BYTE b2, BYTE b3)
{
	DWORD data =
		  ((DWORD)b3 << 24)
		+ ((DWORD)b2 << 16)
		+ ((DWORD)b1 <<  8)
		+ ((DWORD)b0 <<  0);

	return data;
}

void SplitCString(const CString& str, const CString& delimiter, CStringArray& arr)
{
	int startPos = 0;
	while (startPos >= 0)
	{
		int endPos = str.Find(delimiter, startPos);
		if (endPos == -1)
		{
			arr.Add(str.Mid(startPos));
			break;
		}
		arr.Add(str.Mid(startPos, endPos - startPos));
		startPos = endPos + lstrlen(delimiter);
	}
}

////------------------------------------------------
//   .ini support function
////------------------------------------------------

DWORD GetPrivateProfileStringFx(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpDefault, LPTSTR  lpReturnedString, DWORD nSize, LPCTSTR lpFileName)
{
	DWORD result = 0;
	CString key = lpKeyName;
	key.Replace(_T("="), _T("%#3D"));
	key.Replace(_T("\""), _T("%#22"));
	result = GetPrivateProfileString(lpAppName, key, lpDefault, lpReturnedString, nSize, lpFileName);

	CString value = lpReturnedString;
	value.Replace(_T("%#3D"), _T("="));
	value.Replace(_T("%#22"), _T("\""));

#if _MSC_VER <= 1310
	_tcscpy(lpReturnedString, (LPCTSTR)value);
#else
	_tcscpy_s(lpReturnedString, nSize, (LPCTSTR)value);
#endif

	return result;
}

UINT GetPrivateProfileIntFx(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nDefault, LPCTSTR lpFileName)
{
	CString key = lpKeyName;
	key.Replace(_T("="), _T("%#3D"));
	key.Replace(_T("\""), _T("%#22"));

	return GetPrivateProfileInt(lpAppName, key, nDefault, lpFileName);
}

BOOL WritePrivateProfileStringFx(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpString, LPCTSTR lpFileName)
{
	CString key = lpKeyName;
	key.Replace(_T("="), _T("%#3D"));
	key.Replace(_T("\""), _T("%#22"));

	CString value = lpString;
	value.Replace(_T("="), _T("%#3D"));
	value.Replace(_T("\""), _T("%#22"));

	value = _T("\"" + value + _T("\""));

	return WritePrivateProfileString(lpAppName, key, value, lpFileName);
}

////------------------------------------------------
//   Check CodeSign
////------------------------------------------------
#if _MSC_VER > 1310
#include <Softpub.h>
#pragma comment(lib, "Wintrust.lib")
#pragma comment(lib, "Crypt32.lib")

BOOL CheckCodeSign(LPCWSTR certName, LPCWSTR filePath)
{
#ifdef _DEBUG
	return TRUE;
#endif

	// check sign

	WINTRUST_FILE_INFO FileData = { sizeof(WINTRUST_FILE_INFO) };
	FileData.pcwszFilePath = filePath;

	GUID WVTPolicyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;
	WINTRUST_DATA WinTrustData = { sizeof(WinTrustData) };
	WinTrustData.dwUIChoice = WTD_UI_NONE;
	WinTrustData.fdwRevocationChecks = WTD_REVOKE_NONE;
	WinTrustData.dwUnionChoice = WTD_CHOICE_FILE;
	WinTrustData.dwStateAction = WTD_STATEACTION_VERIFY;
	WinTrustData.pFile = &FileData;

	const LONG ret = WinVerifyTrust(NULL, &WVTPolicyGUID, &WinTrustData);

	bool cert_chk = false;

	if (ret == ERROR_SUCCESS) {
		// retreive the signer certificate and display its information
		CRYPT_PROVIDER_DATA const* psProvData = NULL;
		CRYPT_PROVIDER_SGNR* psProvSigner = NULL;
		CRYPT_PROVIDER_CERT* psProvCert = NULL;

		psProvData = WTHelperProvDataFromStateData(WinTrustData.hWVTStateData);
		if (psProvData) {
			psProvSigner = WTHelperGetProvSignerFromChain((PCRYPT_PROVIDER_DATA)psProvData, 0, FALSE, 0);
			if (psProvSigner) {
				psProvCert = WTHelperGetProvCertFromChain(psProvSigner, 0);
				if (psProvCert) {
					wchar_t szCertName[200] = {0};
					DWORD dwStrType = CERT_X500_NAME_STR;
					CertGetNameStringW(psProvCert->pCert, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, &dwStrType, szCertName, 200);
					cert_chk = !(szCertName[0] == '\0' || wcscmp(szCertName, certName) != 0);
				}
			}
		}
	}
	WinTrustData.dwStateAction = WTD_STATEACTION_CLOSE;
	(void)WinVerifyTrust(NULL, &WVTPolicyGUID, &WinTrustData);
	if (!cert_chk)  return FALSE;

	return TRUE;
}
#endif

////------------------------------------------------
//   Play Sound
////------------------------------------------------

#include <mmsystem.h>
#include "digitalv.h"

#pragma comment(lib, "winmm.lib")

/*
void ShowMciError(DWORD error)
{
	TCHAR errorBuf[MAXERRORLENGTH];

	if (mciGetErrorString(error, (LPTSTR)errorBuf, MAXERRORLENGTH))
	{
		AfxMessageBox(errorBuf);
	}
	else
	{
		AfxMessageBox(_T("Unknown Error");
	}
}
*/

BOOL AlertSound(const CString& alertSoundPath, int volume)
{
#if _MSC_VER <= 1310
	PlaySound(alertSoundPath, NULL, SND_SYNC);
#else

	static MCI_OPEN_PARMS mop = { 0 };
	static MCI_PLAY_PARMS mpp = { 0 };
	static MCI_GENERIC_PARMS mgp = { 0 };
	MCIERROR error = 0;

	if (mop.wDeviceID != 0)
	{
		// Stop and close
		error = mciSendCommandW(mop.wDeviceID, MCI_STOP, 0, 0);
		error = mciSendCommandW(mop.wDeviceID, MCI_CLOSE, 0, 0);
		mop.wDeviceID = 0;
		ZeroMemory(&mop, sizeof(MCI_OPEN_PARMS));
		if (error)
		{
			// ShowMciError(error);
			return FALSE;
		}
	}

	if (alertSoundPath.Compare(_T("")) == 0) {
		// Close
		error = mciSendCommandW(mop.wDeviceID, MCI_CLOSE, 0, 0);
		if (error)
		{
			// ShowMciError(error);
			return FALSE;
		}
		return TRUE;
	}

	// Open
	mop.lpstrElementName = (LPCTSTR)alertSoundPath;
	mop.lpstrDeviceType = _T("MPEGVideo");
	error = mciSendCommandW(NULL, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD_PTR)(&mop));
	
	if (error)
	{
		// ShowMciError(error);
		return FALSE;
	}

	// Set volume
	if (volume < 0 || volume > 100) { volume = 80; }

	MCI_DGV_SETAUDIO_PARMS parms = { 0 };
	parms.dwItem = MCI_DGV_SETAUDIO_VOLUME;
	parms.dwValue = volume * 10; // 0-1000
	error = mciSendCommand(mop.wDeviceID, MCI_SETAUDIO, MCI_DGV_SETAUDIO_ITEM | MCI_DGV_SETAUDIO_VALUE, (DWORD_PTR)&parms);
	if (error)
	{
		// ShowMciError(error);
		return FALSE;
	}
	// Seek
	error = mciSendCommand(mop.wDeviceID, MCI_SEEK, MCI_SEEK_TO_START, reinterpret_cast<DWORD_PTR>(&mgp));
	if (error)
	{
		// ShowMciError(error);
		return FALSE;
	}

	// Play
	error = mciSendCommandW(mop.wDeviceID, MCI_PLAY, 0, reinterpret_cast<DWORD_PTR>(&mpp));
	if (error)
	{
		// ShowMciError(error);
		return FALSE;
	}
#endif

	return TRUE;
}


////------------------------------------------------
//   Hash
////------------------------------------------------

#if _MSC_VER > 1310
#include <iostream>
#include <iomanip>
#include <sstream>
#include <windows.h>
#include <wincrypt.h>
CStringA MD5(const CStringA& str)
{
	HCRYPTPROV hProv = 0;
	HCRYPTHASH hHash = 0;
	BYTE hash[16];
	DWORD hashLen = 16;
	CStringA hashStr;

	if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
		return "";
	}

	if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) {
		CryptReleaseContext(hProv, 0);
		return "";
	}

	std::string utf8Str(str);

	if (!CryptHashData(hHash, reinterpret_cast<const BYTE*>(utf8Str.c_str()), (DWORD)utf8Str.size(), 0)) {
		CryptDestroyHash(hHash);
		CryptReleaseContext(hProv, 0);
		return "";
	}

	if (!CryptGetHashParam(hHash, HP_HASHVAL, hash, &hashLen, 0)) {
		CryptDestroyHash(hHash);
		CryptReleaseContext(hProv, 0);
		return "";
	}

	for (DWORD i = 0; i < hashLen; ++i) {
		CString temp;
		temp.Format(_T("%02x"), hash[i]);
		hashStr += temp;
	}

	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);

	return hashStr;
}
#else
#include "md5.h"
CStringA MD5(const CStringA& str)
{
	char* input = (char*)(LPCSTR)str;
	uint8_t result[16] = { 0 };
	md5String(input, result);

	CStringA hashStr;
	for (int i = 0; i < 16; ++i)
	{
		CStringA byteStr;
		byteStr.Format("%02x", result[i]);
		hashStr += byteStr;
	}

	return hashStr;
}
#endif

////------------------------------------------------
//   Character Converter
////------------------------------------------------

CStringA URLEncode(const CStringA& str)
{
	CStringA encoded;
	for (int i = 0; i < str.GetLength(); i++)
	{
		if (isalnum((BYTE)str[i]) || str[i] == '-' || str[i] == '_' || str[i] == '.' || str[i] == '~')
		{
			encoded += str[i];
		}
		else
		{
			encoded.AppendFormat("%%%02X", (BYTE)str[i]);
		}
	}
	return encoded;
}

#if _MSC_VER <= 1310

// https://github.com/roytam1/RetroZilla/commit/e44dd98dbf2effeff3305430835e889d76ec73b4#diff-065242b491de972d8519dbbae0c4c8dfb6a1a3c3eb8b9936d56a4c1ad63ec7abR126-R282
/*** UTF16<-->UTF8 functions minicking MultiByteToWideChar/WideCharToMultiByte ***/
int utf8GetMaskIndex(unsigned char n) {
	if ((unsigned char)(n + 2) < 0xc2) return 1; // 00~10111111, fe, ff
	if (n < 0xe0)                      return 2; // 110xxxxx
	if (n < 0xf0)                      return 3; // 1110xxxx
	if (n < 0xf8)                      return 4; // 11110xxx
	if (n < 0xfc)                      return 5; // 111110xx
	return 6; // 1111110x
}

int wc2Utf8Len(wchar_t** n, int* len) {
	wchar_t* ch = *n, ch2;
	int qch;
	if ((0xD800 <= *ch && *ch <= 0xDBFF) && *len) {
		ch2 = *(ch + 1);
		if (0xDC00 <= ch2 && ch2 <= 0xDFFF) {
			qch = 0x10000 + (((*ch - 0xD800) & 0x3ff) << 10) + ((ch2 - 0xDC00) & 0x3ff);
			(*n)++;
			(*len)--;
		}
	}
	else
		qch = (int)*ch;

	if (qch <= 0x7f)           return 1;
	else if (qch <= 0x7ff)     return 2;
	else if (qch <= 0xffff)    return 3;
	else if (qch <= 0x1fffff)  return 4;
	else if (qch <= 0x3ffffff) return 5;
	else                       return 6;
}

int Utf8ToWideChar(unsigned int unused1, unsigned long unused2, char* sb, int ss, wchar_t* wb, int ws) {
	static const unsigned char utf8mask[] = { 0, 0xff, 0x1f, 0x0f, 0x07, 0x03, 0x01 };
	char* p = (char*)(sb);
	char* e = (char*)(sb + ss);
	wchar_t* w = wb;
	int cnt = 0, t, qch;

	if (ss < 1) {
		ss = lstrlenA(sb);
		e = (char*)(sb + ss);
	}

	if (wb && ws) {
		for (; p < e; ++w) {
			t = utf8GetMaskIndex(*p);
			qch = (*p++ & utf8mask[t]);
			while (p < e && --t)
				qch <<= 6, qch |= (*p++) & 0x3f;
			if (qch < 0x10000) {
				if (cnt <= ws)
					*w = (wchar_t)qch;
				cnt++;
			}
			else {
				if (cnt + 2 <= ws) {
					*w++ = (wchar_t)(0xD800 + (((qch - 0x10000) >> 10) & 0x3ff)),
						*w = (wchar_t)(0xDC00 + (((qch - 0x10000)) & 0x3ff));
				}
				cnt += 2;
			}
		}
		if (cnt < ws) {
			*(wb + cnt) = 0;
			return cnt;
		}
		else {
			*(wb + ws) = 0;
			return ws;
		}
	}
	else {
		for (t; p < e;) {
			t = utf8GetMaskIndex(*p);
			qch = (*p++ & utf8mask[t]);
			while (p < e && --t)
				qch <<= 6, qch |= (*p++) & 0x3f;
			if (qch < 0x10000)
				cnt++;
			else
				cnt += 2;
		}
		return cnt + 1;
	}
}

int WideCharToUtf8(unsigned int unused1, unsigned long unused2, wchar_t* wb, int ws, char* sb, int ss) {
	wchar_t* p = (wchar_t*)(wb);
	wchar_t* e = (wchar_t*)(wb + ws);
	wchar_t* oldp;
	char* s = sb;
	int cnt = 0, qch, t;

	if (ws < 1) {
		ws = lstrlenW(wb);
		e = (wchar_t*)(wb + ws);
	}

	if (sb && ss) {
		for (t; p < e; ++p) {
			oldp = p;
			t = wc2Utf8Len(&p, &ws);

			if (p != oldp) { /* unicode surrogates encountered */
				qch = 0x10000 + (((*oldp - 0xD800) & 0x3ff) << 10) + ((*p - 0xDC00) & 0x3ff);
			}
			else
				qch = *p;

			if (qch <= 0x7f)
				*s++ = (char)(qch),
				cnt++;
			else if (qch <= 0x7ff)
				*s++ = 0xc0 | (char)(qch >> 6),
				*s++ = 0x80 | (char)(qch & 0x3f),
				cnt += 2;
			else if (qch <= 0xffff)
				*s++ = 0xe0 | (char)(qch >> 12),
				*s++ = 0x80 | (char)((qch >> 6) & 0x3f),
				*s++ = 0x80 | (char)(qch & 0x3f),
				cnt += 3;
			else if (qch <= 0x1fffff)
				*s++ = 0xf0 | (char)(qch >> 18),
				*s++ = 0x80 | (char)((qch >> 12) & 0x3f),
				*s++ = 0x80 | (char)((qch >> 6) & 0x3f),
				*s++ = 0x80 | (char)(qch & 0x3f),
				cnt += 4;
			else if (qch <= 0x3ffffff)
				*s++ = 0xf8 | (char)(qch >> 24),
				*s++ = 0x80 | (char)((qch >> 18) & 0x3f),
				*s++ = 0x80 | (char)((qch >> 12) & 0x3f),
				*s++ = 0x80 | (char)((qch >> 6) & 0x3f),
				*s++ = 0x80 | (char)(qch & 0x3f),
				cnt += 5;
			else
				*s++ = 0xfc | (char)(qch >> 30),
				*s++ = 0x80 | (char)((qch >> 24) & 0x3f),
				*s++ = 0x80 | (char)((qch >> 18) & 0x3f),
				*s++ = 0x80 | (char)((qch >> 12) & 0x3f),
				*s++ = 0x80 | (char)((qch >> 6) & 0x3f),
				*s++ = 0x80 | (char)(qch & 0x3f),
				cnt += 6;
		}
		if (cnt < ss) {
			*(sb + cnt) = 0;
			return cnt;
		}
		else {
			*(sb + ss) = 0;
			return ss;
		}
	}
	else {
		for (t; p < e; ++p) {
			t = wc2Utf8Len(&p, &ws);
			cnt += t;
		}
		return cnt + 1;
	}
}
/*** Ends ***/

#ifdef UNICODE
CStringA UTF16toUTF8(const CStringW& utf16str)
{
	if (IsNT3() || IsNT4())
	{
		CStringA utf8str;
		WCHAR* utf16string = new WCHAR[(utf16str.GetLength() + 1) * 2];
		wsprintf(utf16string, L"%s", utf16str);
		int utf16Length = utf16str.GetLength();
		int utf8Length = WideCharToUtf8(CP_UTF8, 0, utf16string, -1, NULL, 0);
		if (utf8Length <= 0) { return ""; }
		WideCharToUtf8(CP_UTF8, 0, utf16string, -1, utf8str.GetBuffer(utf8Length), utf8Length);
		utf8str.ReleaseBuffer();
		delete utf16string;
		return utf8str;
	}
	else
	{
		CStringA utf8str;
		int utf8Length = WideCharToMultiByte(CP_UTF8, 0, utf16str, -1, NULL, 0, NULL, NULL);
		if (utf8Length <= 0) { return ""; }
		WideCharToMultiByte(CP_UTF8, 0, utf16str, -1, utf8str.GetBuffer(utf8Length), utf8Length, NULL, NULL);
		utf8str.ReleaseBuffer();
		return utf8str;
	}
}

CStringW UTF8toUTF16(const CStringA& utf8str)
{
	CStringW utf16str;
	CHAR* utf8string = new CHAR[(utf8str.GetLength() + 1) * 2];
	sprintf(utf8string, "%s", utf8str);
	int utf8Length = utf8str.GetLength();
	int utf16Length = Utf8ToWideChar(1200, 0, utf8string, -1, NULL, 0);
	if (utf16Length <= 0) { return L""; }
	Utf8ToWideChar(1200, 0, utf8string, -1, utf16str.GetBuffer(utf16Length), utf16Length);
	utf16str.ReleaseBuffer();
	delete utf8string;
	return utf16str;
}

#else
CStringA ANSItoUTF8(const CStringA& ansiStr)
{
	int utf16Length = MultiByteToWideChar(CP_ACP, 0, ansiStr, -1, NULL, 0);
	if (utf16Length <= 0) { return ""; }
	CStringW utf16str;
	MultiByteToWideChar(CP_ACP, 0, ansiStr, -1, utf16str.GetBuffer(utf16Length), utf16Length);
	utf16str.ReleaseBuffer();

	CStringA utf8str;
	int utf8Length = WideCharToUtf8(CP_UTF8, 0, utf16str.GetBuffer(utf16Length), -1, NULL, 0);
	if (utf8Length <= 0) { return ""; }
	WideCharToUtf8(CP_UTF8, 0, utf16str.GetBuffer(utf16Length), -1, utf8str.GetBuffer(utf8Length), utf8Length);
	utf8str.ReleaseBuffer();
	return utf8str;
}
#endif
#else

CStringA UTF16toUTF8(const CStringW& utf16str)
{
	CStringA utf8str(CW2A(utf16str, CP_UTF8));
	return utf8str;
}

CStringW UTF8toUTF16(const CStringA& utf8str)
{
	CStringW utf16str;
	utf16str = CA2W(utf8str);
	return utf16str;
}
#endif

#ifdef UNICODE
CStringA UE(const CStringW& utf16str)
{
	return URLEncode(UTF16toUTF8(utf16str));
}
#else
CStringA UE(const CStringA& ansiStr)
{
	return URLEncode(ANSItoUTF8(ansiStr));
}
#endif
////------------------------------------------------
//   Clipboard
////------------------------------------------------

void SetClipboardText(CString clip)
{
	if (OpenClipboard(NULL))
	{
		HGLOBAL clipbuffer;
		TCHAR* buffer = NULL;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, sizeof(TCHAR) * (clip.GetLength() + 1));
		if (clipbuffer != NULL)
		{
			buffer = (TCHAR*)GlobalLock(clipbuffer);
			if (buffer != NULL)
			{
#if _MSC_VER <= 1310
				_tcscpy(buffer, LPCTSTR(clip));
#else
				_tcscpy_s(buffer, clip.GetLength() + 1, LPCTSTR(clip));
#endif
			}
			GlobalUnlock(clipbuffer);
#ifdef UNICODE
			SetClipboardData(CF_UNICODETEXT, clipbuffer);
#else
			SetClipboardData(CF_TEXT, clipbuffer);
#endif
		}
		CloseClipboard();
	}
}
