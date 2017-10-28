/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#define THEME_DIR					_T("CdiResource\\themes\\")
#define LANGUAGE_DIR				_T("CdiResource\\language\\")
#define DIALOG_DIR					_T("CdiResource\\dialog\\")
#define GADGET_DIR					_T("CdiResource\\gadget\\")
#define SMART_DIR					_T("Smart\\")
#define SMART_INI					_T("Smart.ini")
#define EXCHANGE_INI				_T("Exchange.ini")
#define ALARM_HISTORY_CSV			_T("AlarmHistory.csv")
#define ALERT_MAIL_PATH				_T("CdiResource\\AlertMail.exe")
#define ALERT_MAIL_4_PATH			_T("CdiResource\\AlertMail4.exe")
#define OPUS_DEC_PATH				_T("CdiResource\\opus\\opusdec.exe")
#define SHIZUKU_VOICE_PATH			_T("CdiResource\\voice\\ShizukuVoice.dll")
#define KUREI_KEI_VOICE_PATH		_T("CdiResource\\voice\\KureiKeiVoice.dll")

#define MENU_THEME_INDEX			3
#define MENU_LANG_INDEX				6
#define MENU_DRIVE_INDEX			4

#define GRAPH_DIALOG				_T("Graph.html")
#define GRAPH_DIALOG_IE8			_T("Graph8.html")
#define OPTION_DIALOG				_T("Option.html")

#ifdef SUISHO_SHIZUKU_SUPPORT
	#ifdef KUREI_KEI_SUPPORT
		#define DEFAULT_THEME		THEME_DIR _T("KureiKei")
		#define PROJECT_COPYRIGHT   L"KureiKeiCopyright"
	#else
		#define DEFAULT_THEME		THEME_DIR _T("Shizuku")
		#define PROJECT_COPYRIGHT   L"ShizukuCopyright"
	#endif
#else
#define DEFAULT_THEME				THEME_DIR _T("default")
#endif
#define DEFAULT_LANGUAGE			LANGUAGE_DIR _T("English.lang")

class CDiskInfoApp : public CWinApp
{
public:
	CDiskInfoApp();
	~CDiskInfoApp();

	CString m_GraphDlgPath;
	CString m_OptionDlgPath;
	CString m_SmartDir;
	CString m_ExeDir;
	CString m_GadgetDir;
	CString m_AlertMailPath;
	CString m_OpusDecPath;
#ifdef SUISHO_SHIZUKU_SUPPORT
	CString m_VoicePath;
#endif
	CString m_Ini;
	CString m_SaveAsText;
	BOOL m_FlagCopyExit;

	CString m_ThemeDir;
	CString m_LangDir;
	DWORD m_ThemeIndex;
	DWORD m_LangIndex;

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CDiskInfoApp theApp;