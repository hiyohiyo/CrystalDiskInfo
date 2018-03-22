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

#include "MainDialog.h"

UINT CMainDialog::wmTaskbarCreated = ::RegisterWindowMessage(_T("TaskbarCreated"));

CMainDialog::CMainDialog(UINT dlgResouce, 
		CString ThemeDir, DWORD ThemeIndex, CString LangDir, DWORD LangIndex,
		CWnd* pParent)
		:CDialogCx(dlgResouce, pParent)
{

	DebugPrint(L"CMainDialog::CMainDialog");
	m_ThemeDir = ThemeDir;
	m_ThemeIndex = ThemeIndex;
	m_LangDir = LangDir;
	m_LangIndex = LangIndex;

#ifdef SUISHO_SHIZUKU_SUPPORT
	#ifdef KUREI_KEI_SUPPORT
		m_DefaultTheme = L"KureiKei";
		m_RecommendTheme = L"KureiKeiRecoding";
	#else
		m_DefaultTheme = L"Shizuku";
		m_RecommendTheme = L"ShizukuHotaru";
	#endif
#else
	m_DefaultTheme = L"default";
#endif

	m_FlagInitializing = TRUE;
	m_FlagWindoowMinimizeOnce = TRUE;
	m_FlagResidentMinimize = FALSE;
}

CMainDialog::~CMainDialog()
{
}

BEGIN_MESSAGE_MAP(CMainDialog, CDialogCx)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

void CMainDialog::SetWindowTitle(CString message, CString mode)
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

void CMainDialog::InitThemeLang()
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
		CString defaultTheme = m_DefaultTheme;

#ifdef SUISHO_SHIZUKU_SUPPORT
	#ifdef KUREI_KEI_SUPPORT
		if (IsFileExist(m_ThemeDir + m_RecommendTheme + L"\\KureiKeiBackground-300.png"))
		{
			defaultTheme = m_RecommendTheme;
		}
	#else
		if (IsFileExist(m_ThemeDir + m_RecommendTheme + L"\\ShizukuBackground-300.png"))
		{
			defaultTheme = m_RecommendTheme;
		}
	#endif
#endif

#ifdef SUISHO_SHIZUKU_SUPPORT
	#ifdef KUREI_KEI_SUPPORT
			GetPrivateProfileString(_T("Setting"), _T("ThemeKureiKei"), defaultTheme, str, 256, m_Ini);
	#else
			GetPrivateProfileString(_T("Setting"), _T("ThemeShizuku"), defaultTheme, str, 256, m_Ini);
	#endif
#else
		GetPrivateProfileString(_T("Setting"), _T("Theme"), defaultTheme, str, 256, m_Ini);
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

void CMainDialog::InitMenu()
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
	 
	HANDLE hFind;
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
				CString name = findData.cFileName;
#ifdef SUISHO_SHIZUKU_SUPPORT
	#ifdef KUREI_KEI_SUPPORT
				if(name.Find(L"KureiKei") == 0)
	#else
				if(name.Find(L"Shizuku") == 0)
	#endif
#else
				if(name.Find(L"Shizuku") != 0 && name.Find(L"KureiKei") != 0  && name.Find(L".") != 0)
#endif
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

					if(_tcsstr(findData.cFileName, m_DefaultTheme) != NULL)
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
		m_CurrentTheme = m_DefaultTheme;
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

BOOL CMainDialog::OnInitDialog()
{
	return CDialogCx::OnInitDialog();
}

void CMainDialog::ChangeTheme(CString ThemeName)
{
	UpdateDialogSize();
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

BOOL CMainDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
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

	return CDialogCx::OnCommand(wParam, lParam);
}

void CMainDialog::OnWindowPosChanging(WINDOWPOS * lpwndpos)
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

DWORD CMainDialog::GetZoomType()
{
	return GetPrivateProfileInt(_T("Setting"), _T("ZoomType"), ZOOM_TYPE_AUTO, m_Ini);
}

void CMainDialog::SetZoomType(DWORD zoomType)
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
BOOL CMainDialog::AddTaskTray(UINT id, UINT callback, HICON icon, CString tip)
{
	if(m_FlagResident)
	{
		NOTIFYICONDATA nidata = {0};
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
BOOL CMainDialog::ModifyTaskTrayIcon(UINT id, HICON icon)
{
	if(m_FlagResident)
	{
		NOTIFYICONDATA nidata = { 0 };
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
BOOL CMainDialog::ModifyTaskTrayTip(UINT id, CString tip)
{
	if(m_FlagResident)
	{
		NOTIFYICONDATA nidata = { 0 };
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
BOOL CMainDialog::ModifyTaskTray(UINT id, HICON icon, CString tip)
{
	if(m_FlagResident)
	{
		NOTIFYICONDATA nidata = { 0 };
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
BOOL CMainDialog::ShowBalloon(UINT id, DWORD infoFlag, CString infoTitle, CString info)
{
	if(m_FlagResident)
	{
		NOTIFYICONDATA nidata = { 0 };
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
BOOL CMainDialog::RemoveTaskTray(UINT id)
{
	if(m_FlagResident)
	{
		NOTIFYICONDATA nidata = { 0 };
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
