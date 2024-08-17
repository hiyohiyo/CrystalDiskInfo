/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "Resource.h"		// main symbols

// #define THEME_DIR					_T("CdiResource\\themes\\")
// #define LANGUAGE_DIR				_T("CdiResource\\language\\")
#define DIALOG_DIR					_T("CdiResource\\dialog\\")
// #define GADGET_DIR					_T("CdiResource\\gadget\\")
#define SMART_DIR					_T("Smart\\")
#define SMART_INI					_T("Smart.ini")
#define EXCHANGE_INI				_T("Exchange.ini")
#define ALARM_HISTORY_CSV			_T("AlarmHistory.csv")
#define ALERT_MAIL_PATH				_T("CdiResource\\AlertMail.exe")
#define ALERT_MAIL_4_PATH			_T("CdiResource\\AlertMail4.exe")
#define ALERT_MAIL_48_PATH			_T("CdiResource\\AlertMail48.exe")
#define OPUS_DEC_PATH				_T("CdiResource\\opus\\opusdec.exe")
#define SHIZUKU_VOICE_PATH			_T("CdiResource\\voice\\ShizukuVoice.dll")
#define KUREI_KEI_VOICE_PATH		_T("CdiResource\\voice\\KureiKeiVoice.dll")
#define MSI_MEI_VOICE_PATH			_T("CdiResource\\voice\\MSIMeiVoice.dll")
#define AOI_VOICE_JA_PATH			_T("CdiResource\\voice\\AoiVoice-ja.dll")
#define AOI_VOICE_EN_PATH			_T("CdiResource\\voice\\AoiVoice-en.dll")

#define MENU_THEME_INDEX			3
#define MENU_LANG_INDEX				6
#define MENU_DRIVE_INDEX			4

#define GRAPH_DIALOG				_T("Graph.html")
#define GRAPH_DIALOG_IE8			_T("Graph8.html")
#define OPTION_DIALOG				_T("Option.html")

#ifdef SUISHO_AOI_SUPPORT
	#define PROJECT_COPYRIGHT   L"AoiCopyright"
#elif MSI_MEI_SUPPORT
	#define PROJECT_COPYRIGHT   L"Copyright"
#elif KUREI_KEI_SUPPORT
	#define PROJECT_COPYRIGHT   L"KureiKeiCopyright"
#else SUISHO_SHIZUKU_SUPPORT
	#define PROJECT_COPYRIGHT   L"ShizukuCopyright"
#endif

class CDiskInfoApp : public CWinApp
{
public:
	CDiskInfoApp();
	~CDiskInfoApp();

	ULONG_PTR gdiplusToken{};

	CString m_GraphDlgPath;
	CString m_OptionDlgPath;
	CString m_SmartDir;
	CString m_ExeDir;
//	CString m_GadgetDir;
	CString m_AlertMailPath;
	CString m_OpusDecPath;
#ifdef SUISHO_SHIZUKU_SUPPORT
	CString m_VoicePath;
#endif
#ifdef SUISHO_AOI_SUPPORT
	CString m_VoiceLanguage;
#endif
	CString m_Ini;
	CString m_Txt;
	CString m_SaveAsText;
	BOOL m_bCopyExit{};

	CString m_ThemeDir;
	CString m_LangDir;
	DWORD m_ThemeIndex{};
	DWORD m_LangIndex{};

// Overrides
	public:
	virtual BOOL InitInstance();

	private:
	HANDLE hMutex{};

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CDiskInfoApp theApp;