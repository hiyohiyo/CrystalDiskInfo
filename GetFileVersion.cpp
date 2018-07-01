/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "GetFileVersion.h"

#pragma comment(lib,"version.lib")

int GetFileVersion(const TCHAR* file, TCHAR* version)
{
	ULONG reserved = 0;	
	VS_FIXEDFILEINFO vffi;
	TCHAR *buf = NULL;
	int  Locale = 0;
	TCHAR str[256];
	str[0] = '\0';

	UINT size = GetFileVersionInfoSize((TCHAR*)file, &reserved);
	TCHAR *vbuf = new TCHAR[size];
	if(GetFileVersionInfo((TCHAR*)file, 0, size, vbuf))
	{
		VerQueryValue(vbuf, _T("\\"), (void**)&buf, &size);
		CopyMemory(&vffi, buf, sizeof(VS_FIXEDFILEINFO));

		VerQueryValue(vbuf, _T("\\VarFileInfo\\Translation"), (void**)&buf, &size);
		CopyMemory(&Locale, buf, sizeof(int));
		wsprintf(str, _T("\\StringFileInfo\\%04X%04X\\%s"), 
					LOWORD(Locale), HIWORD(Locale), _T("FileVersion"));
		VerQueryValue(vbuf, str, (void**)&buf, &size);

		_tcscpy_s(str, 256, buf);
		if(version != NULL)
		{
			_tcscpy_s(version, 256, buf);
		}
	}
	delete [] vbuf;

	if(_tcscmp(str, _T("")) != 0)
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
	FILE* fp;
	errno_t err;

	err = _tfopen_s(&fp, path, _T("rb"));
	if(err != 0 || fp == NULL)
	{
		return FALSE;
	}
	fclose(fp);
	return TRUE;
}
