/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "../stdafx.h"
#include "MainDialogFx.h"

CMainDialogFx::CMainDialogFx(UINT dlgResouce, CWnd* pParent)
		:CDialogFx(dlgResouce, pParent)
{
	// Common
	m_bStartup = FALSE;
	m_bWindowMinimizeOnce = TRUE;
	m_bResident = FALSE;
	m_bResidentMinimize = FALSE;

	// Theme
	m_ThemeKeyName = L"Theme";

	TCHAR* ptrEnd;
	TCHAR ini[MAX_PATH];
	TCHAR tmp[MAX_PATH];

#ifdef UWP
	TCHAR appData[MAX_PATH];
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fileName[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];

	GetModuleFileName(NULL, ini, MAX_PATH);
	_wsplitpath(ini, drive, dir, fileName, ext);
	SHGetSpecialFolderPath(NULL, appData, CSIDL_APPDATA, 0);
	m_Ini.Format(L"%s\\%s.ini", appData, fileName);
#else
	GetModuleFileName(NULL, ini, MAX_PATH);
	if ((ptrEnd = _tcsrchr(ini, '.')) != NULL)
	{
		*ptrEnd = '\0';
		_tcscat_s(ini, MAX_PATH, L".ini");
		m_Ini = ini;
	}
#endif

	GetModuleFileName(NULL, tmp, MAX_PATH);
	if ((ptrEnd = _tcsrchr(tmp, '\\')) != NULL) { *ptrEnd = '\0'; }
	m_ThemeDir.Format(L"%s\\%s", tmp, THEME_DIR);
	m_LangDir.Format(L"%s\\%s", tmp, LANGUAGE_DIR);
}

CMainDialogFx::~CMainDialogFx()
{
}

BEGIN_MESSAGE_MAP(CMainDialogFx, CDialogFx)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

int CALLBACK HasFontProc(ENUMLOGFONTEX* lpelfe, NEWTEXTMETRICEX* lpntme, int FontType, LPARAM lParam)
{
	if (_tcscmp(lpelfe->elfLogFont.lfFaceName, DEFAULT_FONT_FACE_1) == 0)
	{
		lParam = TRUE;
	}
	return TRUE;
}

CString CMainDialogFx::GetDefaultFont()
{
	CClientDC dc(this);
	LOGFONT logfont;
	BOOL hasFont = FALSE;
	ZeroMemory(&logfont, sizeof(LOGFONT));
	logfont.lfCharSet = DEFAULT_CHARSET;
	::EnumFontFamiliesExW(dc.m_hDC, &logfont, (FONTENUMPROC)HasFontProc, (INT_PTR)(&hasFont), 0);

	if (hasFont)
	{
		return DEFAULT_FONT_FACE_1;
	}
	else
	{
		return DEFAULT_FONT_FACE_2;
	}
}

CString CMainDialogFx::GetCurrentLangPath()
{
	return m_CurrentLangPath;
}

CString CMainDialogFx::GetDefaultLangPath()
{
	return m_DefaultLangPath;
}

CString CMainDialogFx::GetThemeDir()
{
	return m_ThemeDir;
}

CString CMainDialogFx::GetCurrentTheme()
{
	return m_CurrentTheme;
}

CString CMainDialogFx::GetDefaultTheme()
{
	return m_DefaultTheme;
}

CString CMainDialogFx::GetParentTheme1()
{
	return m_ParentTheme1;
}

CString CMainDialogFx::GetParentTheme2()
{
	return m_ParentTheme2;
}

CString CMainDialogFx::GetIniPath()
{
	return m_Ini;
}

void CMainDialogFx::SetWindowTitle(CString message)
{
	CString title;

	if(! message.IsEmpty())
	{
		title.Format(L"%s - %s", PRODUCT_SHORT_NAME, message.GetString());
	}
	else
	{
		title.Format(L"%s %s %s", PRODUCT_NAME, PRODUCT_VERSION, PRODUCT_EDITION);
	}

	SetWindowText(title);
}

void CMainDialogFx::InitThemeLang()
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

		if (IsFileExist(m_ThemeDir + m_RecommendTheme + L"\\" + m_BackgroundName + L"-300.png"))
		{
			defaultTheme = m_RecommendTheme;
		}

		GetPrivateProfileString(L"Setting", m_ThemeKeyName, defaultTheme, str, 256, m_Ini);
		m_CurrentTheme = str;
	}

// Set Language
	GetPrivateProfileString(L"Setting", L"Language", L"", str, 256, m_Ini);

	langPath.Format(L"%s\\%s.lang", (LPTSTR)m_LangDir.GetString(), str);
	m_DefaultLangPath.Format(L"%s\\%s.lang", (LPTSTR)m_LangDir.GetString(), DEFAULT_LANGUAGE);

	if(_tcscmp(str, L"") != 0 && IsFileExist((const TCHAR*)langPath))
	{
		m_CurrentLang = str;
		m_CurrentLangPath.Format(L"%s\\%s.lang", (LPTSTR)m_LangDir.GetString(), str);
	}
	else
	{
		CString currentLocalID;
		currentLocalID.Format(L"0x%04X", GetUserDefaultLCID());
		PrimaryLangID = PRIMARYLANGID(GetUserDefaultLCID());

		langPath.Format(L"%s\\*.lang", (LPTSTR)m_LangDir.GetString());

		hFind = ::FindFirstFile(langPath, &findData);
		if(hFind != INVALID_HANDLE_VALUE)
		{
			do{
				if(findData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
				{
					i++;
					CString cstr;
					cstr.Format(L"%s\\%s", (LPTSTR)m_LangDir.GetString(), findData.cFileName);
					GetPrivateProfileString(L"Language", L"LOCALE_ID", L"", str, 256, cstr);
					if((ptrEnd = _tcsrchr(findData.cFileName, '.')) != NULL){*ptrEnd = '\0';}

					if(_tcsstr(str, currentLocalID) != NULL)
					{
						m_CurrentLang = findData.cFileName;
						m_CurrentLangPath.Format(L"%s\\%s.lang", (LPTSTR)m_LangDir.GetString(), findData.cFileName);
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
				m_CurrentLang = DEFAULT_LANGUAGE;
				m_CurrentLangPath.Format(L"%s\\%s.lang", (LPTSTR)m_LangDir.GetString(), (LPTSTR)m_CurrentLang.GetString());
			}
			else
			{
				m_CurrentLang = PrimaryLang;
				m_CurrentLangPath.Format(L"%s\\%s.lang", (LPTSTR)m_LangDir.GetString(), (LPTSTR)PrimaryLang.GetString());
			}	
		}
	}

	UpdateThemeInfo();
}

void CMainDialogFx::InitMenu()
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
	TCHAR *ptrEnd = NULL;
	TCHAR str[256];
	
	menu.Attach(GetMenu()->GetSafeHmenu());
	subMenu.Attach(menu.GetSubMenu(MENU_THEME_INDEX)->GetSafeHmenu());

	themePath.Format(L"%s\\*.*", (LPTSTR)m_ThemeDir.GetString());

	hFind = ::FindFirstFile(themePath, &findData);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		while(::FindNextFile(hFind, &findData) && i <= 0xFF)
		{
			if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				CString name = findData.cFileName;
				if(CheckThemeEdition(name))
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

	CMenu subMenuAN;
	CMenu subMenuOZ;
	subMenu.CheckMenuRadioItem(WM_THEME_ID, WM_THEME_ID + (UINT)m_MenuArrayTheme.GetSize(),
								currentItemID, MF_BYCOMMAND);
	subMenu.Detach();
	subMenu.Attach(menu.GetSubMenu(MENU_LANG_INDEX)->GetSafeHmenu());
	subMenuAN.Attach(subMenu.GetSubMenu(0)->GetSafeHmenu()); // 1st is "A~N"
	subMenuAN.RemoveMenu(0, MF_BYPOSITION);
	subMenuOZ.Attach(subMenu.GetSubMenu(1)->GetSafeHmenu()); // 2nd is "O~Z"
	subMenuOZ.RemoveMenu(0, MF_BYPOSITION);
	langPath.Format(L"%s\\*.lang", (LPTSTR)m_LangDir.GetString());
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
				cstr.Format(L"%s\\%s", (LPTSTR)m_LangDir.GetString(), findData.cFileName);
				GetPrivateProfileString(L"Language", L"LANGUAGE", L"", str, 256, cstr);
				if((ptrEnd = _tcsrchr(findData.cFileName, L'.')) != NULL)
				{
					*ptrEnd = '\0';
				}

				cstr.Format(L"%s, [%s]", str, findData.cFileName);
				if(L'A' <= findData.cFileName[0] && findData.cFileName[0] <= L'N')
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

	if(! FlagHitLang)
	{
		AfxMessageBox(L"FATAL ERROR: Missing Language Files!!");
	}
}

BOOL CMainDialogFx::CheckThemeEdition(CString name)
{
	if (name.Find(L".") != 0) { return TRUE; }

	return FALSE;
}

BOOL CMainDialogFx::OnInitDialog()
{
	return CDialogFx::OnInitDialog();
}

void CMainDialogFx::ChangeTheme(CString themeName)
{
	WritePrivateProfileString(L"Setting", m_ThemeKeyName, themeName, m_Ini);
}

BOOL CMainDialogFx::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// Select Theme
	if(WM_THEME_ID <= wParam && wParam < WM_THEME_ID + (UINT)m_MenuArrayTheme.GetSize())
	{
		CMenu menu;
		CMenu subMenu;
		menu.Attach(GetMenu()->GetSafeHmenu());
		subMenu.Attach(menu.GetSubMenu(MENU_THEME_INDEX)->GetSafeHmenu());

		m_CurrentTheme = m_MenuArrayTheme.GetAt(wParam - WM_THEME_ID);
		ChangeTheme(m_MenuArrayTheme.GetAt(wParam - WM_THEME_ID));
		subMenu.CheckMenuRadioItem(WM_THEME_ID, WM_THEME_ID + (UINT)m_MenuArrayTheme.GetSize(),
									(UINT)wParam, MF_BYCOMMAND);
		subMenu.Detach();
		menu.Detach();

		UpdateThemeInfo();
		UpdateDialogSize();
	}

	return CDialogFx::OnCommand(wParam, lParam);
}

void CMainDialogFx::OnWindowPosChanging(WINDOWPOS * lpwndpos)
{
	if(! m_bShowWindow)
	{
		lpwndpos->flags &= ~SWP_SHOWWINDOW;
	}
	
	if(m_bWindowMinimizeOnce && ! m_bInitializing)
	{
		m_bWindowMinimizeOnce = FALSE;
		if(m_bResident && m_bResidentMinimize)
		{
			ShowWindow(SW_MINIMIZE);
		}
	}

    CDialogFx::OnWindowPosChanging(lpwndpos);
}

DWORD CMainDialogFx::GetZoomType()
{
	return GetPrivateProfileInt(L"Setting", L"ZoomType", ZoomTypeAuto, m_Ini);
}

void CMainDialogFx::SetZoomType(DWORD zoomType)
{
	CString cstr;
	cstr.Format(L"%d", m_ZoomType);
	WritePrivateProfileString(L"Setting", L"ZoomType", cstr, m_Ini);
}

void CMainDialogFx::UpdateThemeInfo()
{
	CString theme = m_ThemeDir + m_CurrentTheme + L"\\theme.ini";

	m_LabelText = GetControlColor(L"LabelText", 0, theme);
	m_MeterText = GetControlColor(L"MeterText", 0, theme);
	m_ComboText = GetControlColor(L"ComboText", 0, theme);
	m_ComboTextSelected = GetControlColor(L"ComboTextSelected", 0, theme);
	m_ComboBk   = GetControlColor(L"ComboBk", 255, theme);
	m_ComboBkSelected = GetControlColor(L"ComboBkSelected", 192, theme);
	m_ButtonText= GetControlColor(L"ButtonText", 0, theme);
	m_EditText  = GetControlColor(L"EditText", 0, theme);
	m_EditBk    = GetControlColor(L"EditBk", 255, theme);
	m_ListText1 = GetControlColor(L"ListText1", 0, theme);
	m_ListText2 = GetControlColor(L"ListText2", 0, theme);
	m_ListTextSelected = GetControlColor(L"ListTextSelected", 0, theme);
	m_ListBk1 = GetControlColor(L"ListBk1", 255, theme);
	m_ListBk2 = GetControlColor(L"ListBk2", 255, theme);
	m_ListBkSelected = GetControlColor(L"ListBkSelected", 0, theme);
	m_ListLine1 = GetControlColor(L"ListLine1", 0, theme);
	m_ListLine2 = GetControlColor(L"ListLine2", 0, theme);
	m_Glass = GetControlColor(L"Glass", 255, theme);
	m_Frame = GetControlColor(L"Frame", 128, theme);

	m_ComboAlpha = GetControlAlpha(L"ComboAlpha", 255, theme);
	m_EditAlpha = GetControlAlpha(L"EditAlpha", 255, theme);
	m_GlassAlpha = GetControlAlpha(L"GlassAlpha", 128, theme);

	m_CharacterPosition = GetCharacterPosition(theme);

	m_ParentTheme1 = GetParentTheme(1, theme);
	m_ParentTheme2 = GetParentTheme(2, theme);
}

COLORREF CMainDialogFx::GetControlColor(CString name, BYTE defaultColor, CString theme)
{
	COLORREF reverseColor;

	reverseColor = GetPrivateProfileInt(L"Color", name, RGB(defaultColor, defaultColor, defaultColor), theme);
	
	COLORREF color = RGB(GetBValue(reverseColor), GetGValue(reverseColor), GetRValue(reverseColor));

	return color;
}

BYTE CMainDialogFx::GetControlAlpha(CString name, BYTE defaultAlpha, CString theme)
{
	BYTE alpha = GetPrivateProfileInt(L"Alpha", name, defaultAlpha, theme);
	
	return alpha;
}

BYTE CMainDialogFx::GetCharacterPosition(CString theme)
{
	BYTE position = GetPrivateProfileInt(L"Character", L"Position", 0, theme);

	return position;
}

CString CMainDialogFx::GetParentTheme(int i, CString theme)
{
	CString cstr;
	cstr.Format(L"ParentTheme%d", i);
	TCHAR str[256];
	GetPrivateProfileString(L"Info", cstr, L"", str, 256, theme);
	cstr = str;

	return cstr;
}

void CMainDialogFx::SaveImage()
{
	BOOL bDwmEnabled = FALSE;

	static HMODULE hModule = LoadLibraryEx(L"dwmapi.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
	typedef HRESULT(WINAPI* FuncDwmGetWindowAttribute) (HWND hwnd, DWORD dwAttribute, PVOID pvAttribute, DWORD cbAttribute);
	typedef HRESULT(WINAPI* FuncDwmIsCompositionEnabled)(BOOL* pfEnabled);
	static FuncDwmGetWindowAttribute pDwmGetWindowAttribute = (FuncDwmGetWindowAttribute)GetProcAddress(hModule, "DwmGetWindowAttribute");
	static FuncDwmIsCompositionEnabled pDwmIsCompositionEnabled = (FuncDwmIsCompositionEnabled)GetProcAddress(hModule, "DwmIsCompositionEnabled");
	if (hModule && pDwmGetWindowAttribute && pDwmIsCompositionEnabled)
	{
		pDwmIsCompositionEnabled(&bDwmEnabled);
	}

	CRect rc1;
	CRect rc2;

	CImage* image1 = new CImage();
	CImage* image2 = new CImage();

	if (bDwmEnabled)
	{
		GetWindowRect(&rc1);
		if (image1->Create(rc1.Width(), rc1.Height(), 32))
		{
			HDC hImage1DC = image1->GetDC();
			if (IsWin81orLater())
			{
				::PrintWindow(m_hWnd, hImage1DC, 2); // PW_RENDERFULLCONTENT, Windows 8.1 or later
			}
			else
			{
				::PrintWindow(m_hWnd, hImage1DC, 0);
			}

			pDwmGetWindowAttribute(m_hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rc2, sizeof(rc2));
			// cstr.Format(L"Width=%d/%d, Height=%d/%d", rc1.Width(), rc2.Width(), rc1.Height(), rc2.Height());
			// AfxMessageBox(cstr);
			if (rc1.Width() > rc2.Width())
			{
				if (image2->Create(rc2.Width(), rc2.Height(), 32))
				{
					HDC hImage2DC = image2->GetDC();
					::BitBlt(hImage2DC, 0, 0, rc2.Width(), rc2.Height(), hImage1DC, (rc1.Width() - rc2.Width()) / 2, 0, SRCCOPY);
					image2->ReleaseDC();
					SaveImageDlg(image2);
				}
				else
				{
					SaveImageDlg(image1);
				}
			}
			else
			{
				SaveImageDlg(image1);
			}
			image1->ReleaseDC();
		}
	}
	else
	{
		GetWindowRect(&rc1);
		// CString cstr;
		// cstr.Format(L"Width=%d, Height=%d", rc1.Width(), rc1.Height());
		// AfxMessageBox(cstr);
		if (image1->Create(rc1.Width(), rc1.Height(), 32))
		{
			HDC hImage1DC = image1->GetDC();
			if (IsWin81orLater())
			{
				::PrintWindow(m_hWnd, hImage1DC, 2); // PW_RENDERFULLCONTENT, Windows 8.1 or later
			}
			else
			{
				::PrintWindow(m_hWnd, hImage1DC, 0);
			}
			SaveImageDlg(image1);
			image1->ReleaseDC();
		}
	}

	SAFE_DELETE(image1);
	SAFE_DELETE(image2);
}

void CMainDialogFx::SaveImageDlg(CImage* image)
{
	CString path;
	SYSTEMTIME st;
	GetLocalTime(&st);
	path.Format(L"%s_%04d%02d%02d%02d%02d%02d", PRODUCT_NAME, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	CString filter = L"PNG (*.png)|*.png|JPEG (*.jpg)|*.jpg|BMP (*.bmp)|*.bmp||";
	CFileDialog save(FALSE, L"", path, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER, filter);

	if (save.DoModal() == IDOK)
	{
		HRESULT result = image->Save(save.GetPathName());
		if (SUCCEEDED(result))
		{
			//	AfxMessageBox(L"SUCCEEDED");
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// Task Tray
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef OPTION_TASK_TRAY

UINT CMainDialogFx::wmTaskbarCreated = ::RegisterWindowMessage(L"TaskbarCreated");

// Add TaskTray
BOOL CMainDialogFx::AddTaskTray(UINT id, UINT callback, HICON icon, CString tip)
{
	if(m_bResident)
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
		if(m_bStartup)
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
BOOL CMainDialogFx::ModifyTaskTrayIcon(UINT id, HICON icon)
{
	if(m_bResident)
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
BOOL CMainDialogFx::ModifyTaskTrayTip(UINT id, CString tip)
{
	if(m_bResident)
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
BOOL CMainDialogFx::ModifyTaskTray(UINT id, HICON icon, CString tip)
{
	if(m_bResident)
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
BOOL CMainDialogFx::ShowBalloon(UINT id, DWORD infoFlag, CString infoTitle, CString info)
{
	if(m_bResident)
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
BOOL CMainDialogFx::RemoveTaskTray(UINT id)
{
	if(m_bResident)
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

#endif