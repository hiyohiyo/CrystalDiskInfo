/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include <io.h>


static const DWORD DEBUG_MODE_NONE    = 0;
static const DWORD DEBUG_MODE_LOG     = 1;
static const DWORD DEBUG_MODE_MESSAGE = 2;

static DWORD debugMode = DEBUG_MODE_NONE;

void SetDebugMode(DWORD mode)
{
	if(mode <= DEBUG_MODE_MESSAGE)
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
	static int flag = TRUE;
	static TCHAR file[MAX_PATH];
	static DWORD first = GetTickCount();
	CString output;

	output.Format(_T("%08d "), GetTickCount() - first);
	output += cstr;
	output.Append(_T("\n"));
	output.Replace(_T("\r"), _T(""));

	if(flag)
	{
		TCHAR* ptrEnd;
		::GetModuleFileName(NULL, file, MAX_PATH);
		if((ptrEnd = _tcsrchr(file, '.')) != NULL )
		{
			*ptrEnd = '\0';
			_tcscat_s(file, MAX_PATH, _T(".log"));
		}
		DeleteFile(file);
		flag = FALSE;
	}

	if(debugMode == DEBUG_MODE_NONE)
	{
		return ;
	}

	FILE *fp;
	_tfopen_s(&fp, file, _T("ac"));
	_ftprintf(fp, _T("%s"), (LPCTSTR)output);
	fflush(fp);
	fclose(fp);

	if(debugMode == DEBUG_MODE_MESSAGE)
	{
		AfxMessageBox(output);
	}
}