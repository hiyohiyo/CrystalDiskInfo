/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#include "../stdafx.h"
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
	static TCHAR file[MAX_PATH] = L"";
	static DWORD first = (DWORD)GetTickCountFx();
	CString output;

	output.Format(L"%08d ", (DWORD)GetTickCountFx() - first);
	output += cstr;
	output.Append(L"\n");
	output.Replace(L"\r", L"");

	if (flag)
	{
		TCHAR* ptrEnd;
		::GetModuleFileName(NULL, file, MAX_PATH);
		if ((ptrEnd = _tcsrchr(file, '.')) != NULL)
		{
			*ptrEnd = '\0';
			_tcscat_s(file, MAX_PATH, L".log");
		}
		DeleteFile(file);
		flag = FALSE;
	}

	if (debugMode == DEBUG_MODE_NONE)
	{
		return;
	}

	FILE* fp{};
	_tfopen_s(&fp, file, L"ac");
	if (fp != NULL)
	{
		_ftprintf(fp, L"%s", (LPCTSTR)output);
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
	VS_FIXEDFILEINFO vffi{};
	TCHAR* buf = NULL;
	int  Locale = 0;
	TCHAR str[256]{};

	UINT size = GetFileVersionInfoSize((TCHAR*)file, &reserved);
	TCHAR* vbuf = new TCHAR[size];
	if (GetFileVersionInfo((TCHAR*)file, 0, size, vbuf))
	{
		VerQueryValue(vbuf, L"\\", (void**)&buf, &size);
		CopyMemory(&vffi, buf, sizeof(VS_FIXEDFILEINFO));

		VerQueryValue(vbuf, L"\\VarFileInfo\\Translation", (void**)&buf, &size);
		CopyMemory(&Locale, buf, sizeof(int));
		wsprintf(str, L"\\StringFileInfo\\%04X%04X\\%s",
			LOWORD(Locale), HIWORD(Locale), L"FileVersion");
		VerQueryValue(vbuf, str, (void**)&buf, &size);

		_tcscpy_s(str, 256, buf);
		if (version != NULL)
		{
			_tcscpy_s(version, 256, buf);
		}
	}
	delete[] vbuf;

	if (_tcscmp(str, L"") != 0)
	{
		return int(_tstof(str) * 100);
	}
	else
	{
		return 0;
	}
}

BOOL IsFileExist(const TCHAR* path)
{
	FILE* fp{};
	errno_t err{};

	err = _tfopen_s(&fp, path, L"rb");
	if (err != 0 || fp == NULL)
	{
		return FALSE;
	}
	fclose(fp);
	return TRUE;
}

////------------------------------------------------
//   Utility
////------------------------------------------------

typedef ULONGLONG(WINAPI* FuncGetTickCount64)();

ULONGLONG GetTickCountFx()
{
	HMODULE hModule = GetModuleHandle(L"kernel32.dll");
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
		return _Pragma("warning(suppress: 28159)") (ULONGLONG)GetTickCount();
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

////------------------------------------------------
//   .ini support function
////------------------------------------------------

DWORD GetPrivateProfileStringFx(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpDefault, LPTSTR  lpReturnedString, DWORD nSize, LPCTSTR lpFileName)
{
	DWORD result = 0;
	CString key = lpKeyName;
	key.Replace(L"=", L"%#3D");
	key.Replace(L"\"", L"%#22");
	result = GetPrivateProfileString(lpAppName, key, lpDefault, lpReturnedString, nSize, lpFileName);

	CString value = lpReturnedString;
	value.Replace(L"%#3D", L"=");
	value.Replace(L"%#22", L"\"");
	_tcscpy_s(lpReturnedString, nSize, value.GetString());

	return result;
}

UINT GetPrivateProfileIntFx(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nDefault, LPCTSTR lpFileName)
{
	CString key = lpKeyName;
	key.Replace(L"=", L"%#3D");
	key.Replace(L"\"", L"%#22");

	return GetPrivateProfileInt(lpAppName, key, nDefault, lpFileName);
}

BOOL WritePrivateProfileStringFx(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpString, LPCTSTR lpFileName)
{
	CString key = lpKeyName;
	key.Replace(L"=", L"%#3D");
	key.Replace(L"\"", L"%#22");

	CString value = lpString;
	value.Replace(L"=", L"%#3D");
	value.Replace(L"\"", L"%#22");

	value = L"\"" + value + L"\"";

	return WritePrivateProfileString(lpAppName, key, value, lpFileName);
}

////------------------------------------------------
//   Check CodeSign
////------------------------------------------------

#include <Softpub.h>
#pragma comment(lib, "Wintrust.lib")
#pragma comment(lib, "Crypt32.lib")

BOOL CheckCodeSign(LPCTSTR certName, LPCTSTR filePath)
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
					wchar_t szCertName[200] = {};
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
