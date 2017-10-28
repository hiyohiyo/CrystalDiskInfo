/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DiskInfo.h"
#include "DiskInfoDlg.h"
#include "GraphDlg.h"

#include "GetFileVersion.h"
#include "GetOsInfo.h"
#include "IsCurrentUserLocalAdministrator.h"

#include <afxole.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDiskInfoApp

BEGIN_MESSAGE_MAP(CDiskInfoApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;

// CDiskInfoApp construction

CDiskInfoApp::CDiskInfoApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CDiskInfoApp::~CDiskInfoApp()
{
	GdiplusShutdown(gdiplusToken);
}

// The one and only CDiskInfoApp object

CDiskInfoApp theApp;

//-----------------------------------------------------------------------------
// Prototypes
//-----------------------------------------------------------------------------
static BOOL IsFileExistEx(const TCHAR* path, const TCHAR* fileName);
static BOOL RunAsRestart();
// CDiskInfoApp initialization

BOOL CDiskInfoApp::InitInstance()
{
	BOOL flagEarthlight = FALSE;
	BOOL flagStartupExit = FALSE;
	m_FlagCopyExit = FALSE;

	int defaultDisk = -1;
	HANDLE hMutex = NULL;

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	CWinApp::InitInstance();

	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// IE Version Check.
	/*
	if(GetFileVersion(_T("Shdocvw.dll")) < 471)
	{
		AfxMessageBox(_T("CrystalDiskInfo is required IE 6.0 or later."));
	}
	*/

	// CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	// for WMI error
	SetErrorMode(SEM_FAILCRITICALERRORS);

	// Init m_Ini
	TCHAR *ptrEnd;
	TCHAR ini[MAX_PATH];
	::GetModuleFileName(NULL, ini, MAX_PATH);
	if((ptrEnd = _tcsrchr(ini, '\\')) != NULL)
	{
		*ptrEnd = '\0';
		_tcscat_s(ini, MAX_PATH, _T("\\DiskInfo.ini"));
	}
	m_Ini = ini;

	CString cstr;
	DWORD debugMode = GetPrivateProfileInt(_T("Setting"), _T("DebugMode"), 0, m_Ini);
	SetDebugMode(debugMode);
	cstr.Format(_T("%d"), debugMode);
	WritePrivateProfileString(_T("Setting"), _T("DebugMode"), cstr, m_Ini);

	int argc = 0;
	int index = 0;
	LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	if(argc > 1)
	{
		CString cstr;
		cstr = argv[1];
	
		if(cstr.CompareNoCase(_T("/Earthlight")) == 0)
		{
			flagEarthlight = TRUE;
			if(argc > 2)
			{
				defaultDisk = _tstoi(argv[2]);
			}
		}
		if(cstr.CompareNoCase(_T("/Startup")) == 0)
		{
			int time = 0;
			time = GetPrivateProfileInt(_T("Setting"), _T("StartupWaitTime"), 30, m_Ini);
			if(time >= 0)
			{
				Sleep(time * 1000);
			}
			TCHAR str[MAX_PATH];
			::GetModuleFileName(NULL, str, MAX_PATH);
			ShellExecute(NULL, NULL, str, NULL, NULL, SW_SHOWNORMAL);
			return FALSE;
		}
		if(cstr.CompareNoCase(_T("/Exit")) == 0)
		{
			flagStartupExit = TRUE;
		}		
		if(cstr.CompareNoCase(_T("/Copy")) == 0)
		{
			m_SaveAsText = m_Ini;
			m_SaveAsText.Replace(_T("ini"), _T("txt"));
		}
		if(cstr.CompareNoCase(_T("/CopyExit")) == 0)
		{
			m_SaveAsText = m_Ini;
			m_SaveAsText.Replace(_T("ini"), _T("txt"));
			m_FlagCopyExit = TRUE;
		}

	}

	// DEBUG
	// flagEarthlight = TRUE;

	if(! flagEarthlight)
	{
		DebugPrint(_T("CreateMutex"));
		hMutex = ::CreateMutex(NULL, FALSE, PROJECT_NAME);
		if(GetLastError() == ERROR_ALREADY_EXISTS)
		{
			DebugPrint(_T("ERROR_ALREADY_EXISTS"));
			return FALSE;
		}
	}

	CString DefaultTheme;
	CString DefaultLanguage;
	TCHAR tmp[MAX_PATH];

	GetModuleFileName(NULL, tmp, MAX_PATH);
	if((ptrEnd = _tcsrchr(tmp, '\\')) != NULL)
	{
		*ptrEnd = '\0';
	}

	m_ExeDir.Format(_T("%s\\"), tmp);
	m_ThemeDir.Format(_T("%s\\%s"), tmp, THEME_DIR);
	m_LangDir.Format(_T("%s\\%s"), tmp, LANGUAGE_DIR);
	m_SmartDir.Format(_T("%s\\%s"), tmp, SMART_DIR);
	m_GadgetDir.Format(_T("%s\\%s"), tmp, GADGET_DIR);

	if(IsDotNet4())
	{
		m_AlertMailPath.Format(_T("%s\\%s"), tmp, ALERT_MAIL_4_PATH);
	}
	else if(IsDotNet2())
	{
		m_AlertMailPath.Format(_T("%s\\%s"), tmp, ALERT_MAIL_PATH);
	}
	else
	{
		m_AlertMailPath = _T("");
	}
	m_OpusDecPath.Format(_T("%s\\%s"), tmp, OPUS_DEC_PATH);

#ifdef SUISHO_SHIZUKU_SUPPORT
	m_VoicePath.Format(_T("%s\\%s"), tmp, SHIZUKU_VOICE_PATH);
	#ifdef KUREI_KEI_SUPPORT
		m_VoicePath.Format(_T("%s\\%s"), tmp, KUREI_KEI_VOICE_PATH);
	#endif
#endif
	
	m_ThemeIndex = MENU_THEME_INDEX;
	m_LangIndex = MENU_LANG_INDEX;

	DefaultTheme.Format(_T("%s\\%s"), tmp, DEFAULT_THEME);
	DefaultLanguage.Format(_T("%s\\%s"), tmp, DEFAULT_LANGUAGE);

	OSVERSIONINFOEX osvi;
	BOOL bosVersionInfoEx;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if(!(bosVersionInfoEx = GetVersionEx((OSVERSIONINFO *)&osvi)))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx((OSVERSIONINFO *)&osvi);
	}

	if((BOOL)GetPrivateProfileInt(_T("Workaround"), _T("IE8MODE"), 0, m_Ini))
	{
		m_GraphDlgPath.Format(_T("%s\\") DIALOG_DIR GRAPH_DIALOG_IE8, tmp);

		if(! IsFileExistEx(m_GraphDlgPath, GRAPH_DIALOG_IE8))		{	return FALSE;	}
	}
	else
	{
		m_GraphDlgPath.Format(_T("%s\\") DIALOG_DIR GRAPH_DIALOG, tmp);
		if(! IsFileExistEx(m_GraphDlgPath, GRAPH_DIALOG))			{	return FALSE;	}
	}

	m_OptionDlgPath.Format(_T("%s\\") DIALOG_DIR OPTION_DIALOG, tmp);

//	if(! IsFileExistEx(DefaultTheme, DEFAULT_THEME))				{	return FALSE;	}
	if(! IsFileExistEx(DefaultLanguage, DEFAULT_LANGUAGE))			{	return FALSE;	}

// for Windows NT family
#ifdef _UNICODE


	if(! IsCurrentUserLocalAdministrator())
	{
		if(osvi.dwMajorVersion < 6)
		{
			AfxMessageBox(_T("CrystalDiskInfo is required Administrator Privileges."));
		}
		RunAsRestart();
		return FALSE;
	}
#endif

	BOOL flagAfxOleInit = FALSE;

	if(flagEarthlight)
	{
		CGraphDlg dlg(NULL, defaultDisk);
		m_pMainWnd = &dlg;
		INT_PTR nResponse = dlg.DoModal();
	}
	else
	{
		// No Server Busy Dialog!!

		DebugPrint(_T("AfxOleInit()"));
		if(AfxOleInit())
		{
			flagAfxOleInit = TRUE;
			DebugPrint(_T("AfxOleGetMessageFilter()->SetMessagePendingDelay"));
			AfxOleGetMessageFilter()->SetMessagePendingDelay(60 * 1000);
			DebugPrint(_T("AfxOleGetMessageFilter()->EnableNotRespondingDialog(FALSE)"));
			AfxOleGetMessageFilter()->EnableNotRespondingDialog(FALSE);
			DebugPrint(_T("AfxOleGetMessageFilter()->EnableBusyDialog(FALSE)"));
			AfxOleGetMessageFilter()->EnableBusyDialog(FALSE);
		}
		else
		{
			DebugPrint(_T("CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);"));
			CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		}

		CDiskInfoDlg dlg(NULL, flagStartupExit);
		m_pMainWnd = &dlg;

		BOOL flagReExec = FALSE;

		DebugPrint(_T("dlg.DoModal()"));
		if(dlg.DoModal() == RE_EXEC)
		{
			flagReExec = TRUE;
		}

		::ReleaseMutex(hMutex);
		::CloseHandle(hMutex);

		if(flagReExec)
		{
			TCHAR str[MAX_PATH];
			::GetModuleFileName(NULL, str, MAX_PATH);
			ShellExecute(NULL, NULL, str, NULL, NULL, SW_SHOWNORMAL);
		}
	}

	if(! flagAfxOleInit)
	{
		DebugPrint(_T("CoUninitialize();"));
		CoUninitialize();
	}

	return FALSE;
}

BOOL IsFileExistEx(const TCHAR* path, const TCHAR* fileName)
{
	if(! IsFileExist(path))
	{
		CString cstr;
		cstr.Format(_T("Not Found \"%s\"."), fileName); 
		AfxMessageBox(cstr);
		return FALSE;
	}
	return TRUE;
}

BOOL RunAsRestart()
{
	int count;
#ifdef _UNICODE
	TCHAR** cmd = ::CommandLineToArgvW(::GetCommandLine(), &count);
#else
	TCHAR** cmd = ::__argv;
	count = ::__argc;
#endif

	if(count < 2 || _tcscmp(cmd[1], _T("runas")) != 0)
	{
		TCHAR path[MAX_PATH];
		::GetModuleFileName(NULL, path, MAX_PATH);
		if(::ShellExecute(NULL, _T("runas"), path, _T("runas"), NULL, SW_SHOWNORMAL)
			> (HINSTANCE)32)
		{
			return TRUE;
		}
	}
	return FALSE;
}