/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/
#include "stdafx.h"
#include "resource.h"
#include "GetFileVersion.h"
#include "GetOsInfo.h"

UINT CDHtmlMainDialog::wmTaskbarCreated = ::RegisterWindowMessage(_T("TaskbarCreated"));

CDHtmlMainDialog::CDHtmlMainDialog(UINT dlgResouce, UINT dlgHtml, 
		CString ThemeDir, DWORD ThemeIndex, CString LangDir, DWORD LangIndex,
		CWnd* pParent)
		:CDHtmlDialogEx(dlgResouce, dlgHtml, pParent)
{
	m_ThemeDir = ThemeDir;
	m_ThemeIndex = ThemeIndex;
	m_LangDir = LangDir;
	m_LangIndex = LangIndex;

	m_FlagInitializing = TRUE;
	m_FlagWindoowMinimizeOnce = TRUE;
	m_FlagResidentMinimize = FALSE;
}

CDHtmlMainDialog::~CDHtmlMainDialog()
{
}

BEGIN_MESSAGE_MAP(CDHtmlMainDialog, CDHtmlDialogEx)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CDHtmlMainDialog)
END_DHTML_EVENT_MAP()

void CDHtmlMainDialog::SetWindowTitle(CString message, CString mode)
{
	CString title;

	if(! message.IsEmpty())
	{
		title.Format(_T("%s - %s"), PRODUCT_SHORT_NAME, message);
	}
	else if(! mode.IsEmpty())
	{
		title.Format(_T("%s %s %s"), PRODUCT_NAME, PRODUCT_VERSION, mode);
	}
	else
	{
		title.Format(_T("%s %s %s"), PRODUCT_NAME, PRODUCT_VERSION, PRODUCT_EDITION);
	}
	SetWindowText(title);
}

void CDHtmlMainDialog::InitThemeLang()
{
	TCHAR str[256];
	TCHAR *ptrEnd;

	WIN32_FIND_DATA findData;
	HANDLE hFind;
	CString langPath;
	int i = 0;
	WORD PrimaryLangID;
	CString PrimaryLang;

// Set Theme
	if(m_CurrentTheme.IsEmpty())
	{
#ifdef SUISHO_SHIZUKU_SUPPORT
	#ifdef KUREI_KEI_SUPPORT
		GetPrivateProfileString(_T("Setting"), _T("ThemeKureiKei"), _T("default"), str, 256, m_Ini);
	#else
		GetPrivateProfileString(_T("Setting"), _T("ThemeShizuku"), _T("default"), str, 256, m_Ini);
	#endif
#else
		GetPrivateProfileString(_T("Setting"), _T("Theme"), _T("default"), str, 256, m_Ini);
#endif
		m_CurrentTheme = str;
	}

// Set Language
	GetPrivateProfileString(_T("Setting"), _T("Language"), _T(""), str, 256, m_Ini);

	langPath.Format(_T("%s\\%s.lang"), m_LangDir, str);
	m_DefaultLangPath.Format(_T("%s\\%s.lang"), m_LangDir, _T("English"));

	if(_tcscmp(str, _T("")) != 0 && IsFileExist((const TCHAR*)langPath))
	{
		m_CurrentLang = str;
		m_CurrentLangPath.Format(_T("%s\\%s.lang"), m_LangDir, str);
	}
	else
	{
		m_CurrentLocalID.Format(_T("0x%04X"), GetUserDefaultLCID());
		PrimaryLangID = PRIMARYLANGID(GetUserDefaultLCID());

		langPath.Format(_T("%s\\*.lang"), m_LangDir);

		hFind = ::FindFirstFile(langPath, &findData);
		if(hFind != INVALID_HANDLE_VALUE)
		{
			do{
				if(findData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
				{
					i++;
					CString cstr;
					cstr.Format(_T("%s\\%s"), m_LangDir, findData.cFileName);
					GetPrivateProfileString(_T("Language"), _T("LOCALE_ID"), _T(""), str, 256, cstr);
					if((ptrEnd = _tcsrchr(findData.cFileName, '.')) != NULL){*ptrEnd = '\0';}

					if(_tcsstr(str, m_CurrentLocalID) != NULL)
					{
						m_CurrentLang = findData.cFileName;
						m_CurrentLangPath.Format(_T("%s\\%s.lang"), m_LangDir, findData.cFileName);
					}
					if(PrimaryLangID == PRIMARYLANGID(_tcstol(str, NULL, 16)))
					{
						PrimaryLang = findData.cFileName;
					}
				}
			}while(::FindNextFile(hFind, &findData) && i <= 0xFF);
		}
		FindClose(hFind);

		if(m_CurrentLang.IsEmpty())
		{
			if(PrimaryLang.IsEmpty())
			{
				m_CurrentLang = _T("English");
				m_CurrentLangPath.Format(_T("%s\\%s.lang"), m_LangDir, m_CurrentLang);
			}
			else
			{
				m_CurrentLang = PrimaryLang;
				m_CurrentLangPath.Format(_T("%s\\%s.lang"), m_LangDir, PrimaryLang);
			}	
		}
	}
}

void CDHtmlMainDialog::InitMenu()
{
	CMenu menu;
	CMenu subMenu;
	BOOL FlagHitTheme = FALSE;
	BOOL FlagHitLang = FALSE;
	UINT newItemID = 0;
	UINT currentItemID = 0;
	UINT defaultStyleItemID = 0;
	UINT defaultLanguageItemID = 0;
	WIN32_FIND_DATA findData;
	WIN32_FIND_DATA findCssData;
	HANDLE hFind;
	HANDLE hCssFind;
	CString themePath;
	CString themeCssPath;
	CString langPath;
	int i = 0;
	TCHAR *ptrEnd;
	TCHAR str[256];
	
	menu.Attach(GetMenu()->GetSafeHmenu());
	subMenu.Attach(menu.GetSubMenu(m_ThemeIndex)->GetSafeHmenu());
//	subMenu.RemoveMenu(0, MF_BYPOSITION);

	themePath.Format(_T("%s\\*.*"), m_ThemeDir);

	hFind = ::FindFirstFile(themePath, &findData);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		while(::FindNextFile(hFind, &findData) && i <= 0xFF)
		{
			if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
//				themeCssPath.Format(_T("%s\\%s\\%s"), m_ThemeDir,
//									findData.cFileName, MAIN_CSS_FILE_NAME);
				hCssFind = ::FindFirstFile(themeCssPath, &findCssData);
				if(hCssFind != INVALID_HANDLE_VALUE)
				{
					// Add Theme
					newItemID = WM_THEME_ID + i;
					i++;
					subMenu.AppendMenu(MF_STRING, (UINT_PTR)newItemID, findData.cFileName);
					m_MenuArrayTheme.Add(findData.cFileName);
					if(m_CurrentTheme.Compare(findData.cFileName) == 0)
					{
						currentItemID = newItemID;
						FlagHitTheme = TRUE;
					}
					if(_tcsstr(findData.cFileName, _T("default")) != NULL)
					{
						defaultStyleItemID = newItemID;
					}
				}
			}
		}
	}
	FindClose(hFind);

	if(! FlagHitTheme)
	{
		currentItemID = defaultStyleItemID;
		m_CurrentTheme = _T("default");
	}

	subMenu.CheckMenuRadioItem(WM_THEME_ID, WM_THEME_ID + (UINT)m_MenuArrayTheme.GetSize(),
								currentItemID, MF_BYCOMMAND);

	subMenu.Detach();

#ifdef _UNICODE
	subMenu.Attach(menu.GetSubMenu(m_LangIndex)->GetSafeHmenu());

	CMenu subMenuAN;
	CMenu subMenuOZ;

	subMenuAN.Attach(subMenu.GetSubMenu(0)->GetSafeHmenu()); // 1st is "A~N"
	subMenuAN.RemoveMenu(0, MF_BYPOSITION);
	subMenuOZ.Attach(subMenu.GetSubMenu(1)->GetSafeHmenu()); // 2nd is "O~Z"
	subMenuOZ.RemoveMenu(0, MF_BYPOSITION);

	langPath.Format(_T("%s\\*.lang"), m_LangDir);
	i = 0;
	hFind = ::FindFirstFile(langPath, &findData);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		do{
			if(findData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
			{
				newItemID = WM_LANGUAGE_ID + i;
				i++;

				// Add Language
				CString cstr;
				cstr.Format(_T("%s\\%s"), m_LangDir, findData.cFileName);
				GetPrivateProfileString(_T("Language"), _T("LANGUAGE"), _T(""), str, 256, cstr);
				if((ptrEnd = _tcsrchr(findData.cFileName, '.')) != NULL)
				{
					*ptrEnd = '\0';
				}

				cstr.Format(_T("%s, [%s]"), str, findData.cFileName);
				if('A' <= findData.cFileName[0] && findData.cFileName[0] <= 'N')
				{
					subMenuAN.AppendMenu(MF_STRING, (UINT_PTR)newItemID, cstr);
				}
				else
				{
					subMenuOZ.AppendMenu(MF_STRING, (UINT_PTR)newItemID, cstr);
				}
				m_MenuArrayLang.Add(findData.cFileName);

				if(m_CurrentLang.Compare(findData.cFileName) == 0)
				{
					currentItemID = newItemID;
					FlagHitLang = TRUE;
				}
			}
		}while(::FindNextFile(hFind, &findData) && i <= 0xFF);
	}
	FindClose(hFind);

	subMenuAN.CheckMenuRadioItem(WM_LANGUAGE_ID, WM_LANGUAGE_ID + (UINT)m_MenuArrayLang.GetSize(),
								currentItemID, MF_BYCOMMAND);
	subMenuOZ.CheckMenuRadioItem(WM_LANGUAGE_ID, WM_LANGUAGE_ID + (UINT)m_MenuArrayLang.GetSize(),
								currentItemID, MF_BYCOMMAND);

	subMenuOZ.Detach();
	subMenuAN.Detach();
	subMenu.Detach();
	menu.Detach();
#else
	subMenu.Attach(menu.GetSubMenu(m_LangIndex)->GetSafeHmenu());
	subMenu.RemoveMenu(0, MF_BYPOSITION);//A~N
	subMenu.RemoveMenu(0, MF_BYPOSITION);//O~Z
	langPath.Format(_T("%s\\*.lang"), m_LangDir);
	i = 0;
	hFind = ::FindFirstFile(langPath, &findData);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		do{
			if(findData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
			{
				newItemID = WM_LANGUAGE_ID + i;
				i++;

				// Add Language
				CString cstr;
				cstr.Format(_T("%s\\%s"), m_LangDir, findData.cFileName);
				GetPrivateProfileString(_T("Language"), _T("LANGUAGE"), _T(""), str, 256, cstr);
				if((ptrEnd = _tcsrchr(findData.cFileName, '.')) != NULL){
					*ptrEnd = '\0';
				}

				cstr.Format(_T("%s, [%s]"), str, findData.cFileName);
				subMenu.AppendMenu(MF_STRING, (UINT_PTR)newItemID, cstr);
				m_MenuArrayLang.Add(findData.cFileName);

				if(m_CurrentLang.Compare(findData.cFileName) == 0)
				{
					currentItemID = newItemID;
					FlagHitLang = TRUE;
				}
			}
		}while(::FindNextFile(hFind, &findData) && i <= 0xFF);

	}
	FindClose(hFind);

	subMenu.CheckMenuRadioItem(WM_LANGUAGE_ID, WM_LANGUAGE_ID + (UINT)m_MenuArrayLang.GetSize(),
								currentItemID, MF_BYCOMMAND);

	subMenu.Detach();
	menu.Detach();
#endif
	if(! FlagHitLang)
	{
		AfxMessageBox(_T("Fatal Error. Missing Language Files!!"));
	}
}

BOOL CDHtmlMainDialog::OnInitDialog()
{
	return CDHtmlDialogEx::OnInitDialog();
}

void CDHtmlMainDialog::ChangeTheme(CString ThemeName)
{
	CComPtr<IHTMLDocument2> pHtmlDoc;
	CComPtr<IHTMLStyleSheet> pHtmlStyleSheet;
	CComPtr<IHTMLStyleSheetsCollection> pStyleSheetsCollection;

	HRESULT hr;
	CComBSTR bstr;
	CString cstr;
	LONG length;

	hr = GetDHtmlDocument(&pHtmlDoc);
	if(FAILED(hr)) return ;

	hr = pHtmlDoc->get_styleSheets(&pStyleSheetsCollection);
	if(FAILED(hr)) return ;

	hr = pStyleSheetsCollection->get_length(&length);
	if(FAILED(hr)) return ;

	VARIANT index;
	VariantInit(&index);
// by ordinal
	index.vt = VT_I4;
	index.intVal = 0;
// by name
//	index.vt = VT_BSTR;
//	index.bstrVal = L"StyleSheet";
	VARIANT dispatch;
	VariantInit(&dispatch);
	dispatch.vt = VT_DISPATCH;

	hr = pStyleSheetsCollection->item(&index, &dispatch);
	if(FAILED(hr)) return ;

	dispatch.pdispVal->QueryInterface(IID_IHTMLStyleSheet, (void **) &pHtmlStyleSheet);
	if(FAILED(hr)) return ;

	cstr.Format(_T("%s\\%s\\%s"), m_ThemeDir, ThemeName, MAIN_CSS_FILE_NAME);
	bstr = cstr;
	hr = pHtmlStyleSheet->put_href(bstr);
	if(FAILED(hr)) return ;

	VariantClear(&index);
	VariantClear(&dispatch);

#ifdef SUISHO_SHIZUKU_SUPPORT
	#ifdef KUREI_KEI_SUPPORT
		WritePrivateProfileString(_T("Setting"), _T("ThemeKureiKei"), ThemeName, m_Ini);
	#else
		WritePrivateProfileString(_T("Setting"), _T("ThemeShizuku"), ThemeName, m_Ini);
	#endif
#else
	WritePrivateProfileString(_T("Setting"), _T("Theme"), ThemeName, m_Ini);
#endif


	
}

BOOL CDHtmlMainDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// Select Theme
	if(WM_THEME_ID <= wParam && wParam < WM_THEME_ID + (UINT)m_MenuArrayTheme.GetSize())
	{
		CMenu menu;
		CMenu subMenu;
		menu.Attach(GetMenu()->GetSafeHmenu());
		subMenu.Attach(menu.GetSubMenu(m_ThemeIndex)->GetSafeHmenu());

		m_CurrentTheme = m_MenuArrayTheme.GetAt(wParam - WM_THEME_ID);
		ChangeTheme(m_MenuArrayTheme.GetAt(wParam - WM_THEME_ID));
		subMenu.CheckMenuRadioItem(WM_THEME_ID, WM_THEME_ID + (UINT)m_MenuArrayTheme.GetSize(),
									(UINT)wParam, MF_BYCOMMAND);
		subMenu.Detach();
		menu.Detach();
	}

	return CDHtmlDialogEx::OnCommand(wParam, lParam);
}

void CDHtmlMainDialog::OnWindowPosChanging(WINDOWPOS * lpwndpos)
{
	if(! m_FlagShowWindow)
	{
		lpwndpos->flags &= ~SWP_SHOWWINDOW;
	}
	
	if(m_FlagWindoowMinimizeOnce && ! m_FlagInitializing)
	{
		m_FlagWindoowMinimizeOnce = FALSE;
		if(m_FlagResident && m_FlagResidentMinimize)
		{
			ShowWindow(SW_MINIMIZE);
		}
	}

    CDialog::OnWindowPosChanging(lpwndpos);
}

DWORD CDHtmlMainDialog::GetZoomType()
{
	return GetPrivateProfileInt(_T("Setting"), _T("ZoomType"), ZOOM_TYPE_AUTO, m_Ini);
}

void CDHtmlMainDialog::SetZoomType(DWORD zoomType)
{
	CString cstr;
	cstr.Format(_T("%d"), m_ZoomType);
	WritePrivateProfileString(_T("Setting"), _T("ZoomType"), cstr, m_Ini);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// Task Tray
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Add TaskTray
BOOL CDHtmlMainDialog::AddTaskTray(UINT id, UINT callback, HICON icon, CString tip)
{
	if(m_FlagResident)
	{
		NOTIFYICONDATA nidata;
		nidata.cbSize = sizeof(NOTIFYICONDATA);
		nidata.hWnd = m_hWnd;
		nidata.uID = id;
		nidata.uFlags = NIF_TIP | NIF_ICON | NIF_MESSAGE;
		nidata.hIcon = icon;
		nidata.uVersion = NOTIFYICON_VERSION;
		nidata.uCallbackMessage = callback;
		_tcscpy_s(nidata.szTip, 128, tip.Left(127));

		::Shell_NotifyIcon(NIM_SETVERSION, &nidata);
		int waitCount = 10;
		if(m_FlagStartup)
		{
			waitCount = 20;
		}
		for(int i = 0; i < waitCount; i++)
		{
			if(::Shell_NotifyIcon(NIM_ADD, &nidata))
			{
				return TRUE;
			}
			Sleep(100 * i);
		}
	}
	return FALSE;
}

// Update TaskTray Icon
BOOL CDHtmlMainDialog::ModifyTaskTrayIcon(UINT id, HICON icon)
{
	if(m_FlagResident)
	{
		NOTIFYICONDATA nidata;
		nidata.cbSize = sizeof(NOTIFYICONDATA);
		nidata.hWnd = m_hWnd;
		nidata.uID = id;
		nidata.uFlags = NIF_ICON;
		nidata.hIcon = icon;
		for(int i = 0; i < 3; i++)
		{
			if(::Shell_NotifyIcon(NIM_MODIFY, &nidata))
			{
				return TRUE;
			}
			Sleep(100 * i);
		}
	}
	return FALSE;
}

// Update TaskTray Tips
BOOL CDHtmlMainDialog::ModifyTaskTrayTip(UINT id, CString tip)
{
	if(m_FlagResident)
	{
		NOTIFYICONDATA nidata;
		nidata.cbSize = sizeof(NOTIFYICONDATA);
		nidata.hWnd = m_hWnd;
		nidata.uID = id;
		nidata.uFlags = NIF_TIP;
	
		_tcscpy_s(nidata.szTip, 128, tip.Left(127));

		for(int i = 0; i < 3; i++)
		{
			if(::Shell_NotifyIcon(NIM_MODIFY, &nidata))
			{
				return TRUE;
			}
			Sleep(100 * i);
		}
	}
	return FALSE;
}

// Update TaskTray
BOOL CDHtmlMainDialog::ModifyTaskTray(UINT id, HICON icon, CString tip)
{
	if(m_FlagResident)
	{
		NOTIFYICONDATA nidata;
		nidata.cbSize = sizeof(NOTIFYICONDATA);
		nidata.hWnd = m_hWnd;
		nidata.uID = id;
		nidata.uFlags = NIF_TIP|NIF_ICON;
		nidata.hIcon = icon;
		_tcscpy_s(nidata.szTip, 128, tip.Left(127));

		for(int i = 0; i < 3; i++)
		{
			if(::Shell_NotifyIcon(NIM_MODIFY, &nidata))
			{
				return TRUE;
			}
			Sleep(100 * i);
		}
	}
	return FALSE;
}

// Show Balloon
BOOL CDHtmlMainDialog::ShowBalloon(UINT id, DWORD infoFlag, CString infoTitle, CString info)
{
	if(m_FlagResident)
	{
		NOTIFYICONDATA nidata;
		nidata.cbSize = sizeof(NOTIFYICONDATA);
		nidata.hWnd = m_hWnd;
		nidata.uID = id;
		nidata.uFlags = NIF_INFO;
		nidata.dwInfoFlags = infoFlag;

		_tcscpy_s(nidata.szInfo, 256, info.Left(255));
		_tcscpy_s(nidata.szInfoTitle, 64, infoTitle.Left(63));

		for(int i = 0; i < 3; i++)
		{
			if(::Shell_NotifyIcon(NIM_MODIFY, &nidata))
			{
				return TRUE;
			}
			Sleep(100 * i);
		}
	}
	return FALSE;
}

// Remove TaskTray
BOOL CDHtmlMainDialog::RemoveTaskTray(UINT id)
{
	if(m_FlagResident)
	{
		NOTIFYICONDATA nidata;
		nidata.cbSize = sizeof(NOTIFYICONDATA);
		nidata.hWnd = m_hWnd;
		nidata.uID = id;

		for(int i = 0; i < 3; i++)
		{
			if(::Shell_NotifyIcon(NIM_DELETE, &nidata))
			{
				return TRUE;
			}
			Sleep(100 * i);
		}
	}
	return FALSE;
}
