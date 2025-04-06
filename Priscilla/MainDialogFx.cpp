/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "MainDialogFx.h"
#include <ctime>
using namespace std;

CMainDialogFx::CMainDialogFx(UINT dlgResouce, CWnd* pParent)
		:CDialogFx(dlgResouce, pParent)
{
	// Common
	m_bStartup = FALSE;
	m_bWindowMinimizeOnce = TRUE;
	m_bResident = FALSE;
	m_bResidentMinimize = FALSE;

	// Theme
	m_ThemeKeyName = _T("Theme");

	TCHAR* ptrEnd;
	TCHAR ini[MAX_PATH];
	TCHAR tmp[MAX_PATH];
	CString directry;


	GetModuleFileName(NULL, ini, MAX_PATH);
	if ((ptrEnd = _tcsrchr(ini, '.')) != NULL)
	{
		*ptrEnd = '\0';
#if _MSC_VER > 1310
		_tcscat_s(ini, MAX_PATH, _T(".ini"));
#else
		_tcscat(ini, _T(".ini"));
#endif
		m_Ini = ini;
	}

#if _MSC_VER > 1310
	CString tmpPath;
	tmpPath = m_Ini;
	tmpPath.Replace(_T(".ini"), _T(".tmp"));

	if (! CanWriteFile(tmpPath))
	{
		TCHAR drive[_MAX_DRIVE];
		TCHAR ext[_MAX_EXT];
		TCHAR appData[MAX_PATH];
		TCHAR dir[_MAX_DIR];
		TCHAR fileName[_MAX_FNAME];
		GetModuleFileName(NULL, ini, MAX_PATH);
		_tsplitpath(ini, drive, dir, fileName, ext);
		SHGetSpecialFolderPath(NULL, appData, CSIDL_APPDATA, 0);
		directry.Format(_T("%s\\%s"), appData, PRODUCT_FILENAME);
		CreateDirectory(directry, NULL);
		m_Ini.Format(_T("%s\\%s\\%s.ini"), appData, PRODUCT_FILENAME, fileName);
	}
#endif

	GetModuleFileName(NULL, tmp, MAX_PATH);
	if ((ptrEnd = _tcsrchr(tmp, '\\')) != NULL) { *ptrEnd = '\0'; }
	m_ThemeDir.Format(_T("%s\\%s"), tmp, THEME_DIR);
	m_LangDir.Format(_T("%s\\%s"), tmp, LANGUAGE_DIR);
	m_VoiceDir.Format(_T("%s\\%s"), tmp, VOICE_DIR);
}

CMainDialogFx::~CMainDialogFx()
{
}

BEGIN_MESSAGE_MAP(CMainDialogFx, CDialogFx)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


LRESULT CMainDialogFx::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
#if _MSC_VER > 1310
	LRESULT lr = 0;
	if (UAHWndProc(m_hWnd, message, wParam, lParam, &lr)) {
		return lr;
	}
#endif

	return CDialogFx::WindowProc(message, wParam, lParam);
}

int CALLBACK HasFontProc(ENUMLOGFONTEX* lpelfe, NEWTEXTMETRICEX* lpntme, int FontType, LPARAM lParam)
{
	*(BOOL*)lParam = TRUE;
	return 0;
}

CString CMainDialogFx::GetDefaultFont()
{
#if _MSC_VER <= 1310
	int stockFont = DEFAULT_GUI_FONT;
	if (IsNT3())
	{
		return _T("MS Shell Dlg");
	}
	HFONT hFont = (HFONT)GetStockObject(stockFont);
	LOGFONT lf = { 0 };

	if (GetObject(hFont, sizeof(LOGFONT), &lf))
	{
		return lf.lfFaceName;
	}
#endif

	CClientDC dc(this);
	LOGFONT logfont;
	BOOL hasFont = FALSE;
	ZeroMemory(&logfont, sizeof(LOGFONT));
	lstrcpy(logfont.lfFaceName, DEFAULT_FONT_FACE_1);
	logfont.lfCharSet = DEFAULT_CHARSET;
	::EnumFontFamiliesEx(dc.m_hDC, &logfont, (FONTENUMPROC)HasFontProc, (LPARAM)(&hasFont), 0);

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
		title.Format(_T(" %s - %s"), PRODUCT_SHORT_NAME, (LPCTSTR)message);
	}
	else
	{
		title.Format(_T(" %s %s %s"), PRODUCT_NAME, PRODUCT_VERSION, PRODUCT_EDITION);
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
#if _MSC_VER > 1310
		if (IsFileExist(m_ThemeDir + m_RecommendTheme + _T("\\") + m_BackgroundName + _T("-300.png")))
		{
			defaultTheme = m_RecommendTheme;
		}
#else
		if (IsFileExist(m_ThemeDir + m_RecommendTheme + _T("\\") + m_BackgroundName + _T("-100.png")))
		{
			defaultTheme = m_RecommendTheme;
		}
#endif

		GetPrivateProfileStringFx(_T("Setting"), m_ThemeKeyName, defaultTheme, str, 256, m_Ini);
		m_CurrentTheme = str;
	}

// Set Language
	GetPrivateProfileStringFx(_T("Setting"), _T("Language"), _T(""), str, 256, m_Ini);

	langPath.Format(_T("%s\\%s.lang"), (LPCTSTR)m_LangDir, str);
#ifdef UNICODE
	m_DefaultLangPath.Format(_T("%s\\%s.lang"), (LPCTSTR)m_LangDir, DEFAULT_LANGUAGE);
#else
	m_DefaultLangPath.Format(_T("%s\\%s9x.lang"), (LPCTSTR)m_LangDir, DEFAULT_LANGUAGE);
#endif
	if(_tcscmp(str, _T("")) != 0 && IsFileExist((LPCTSTR)langPath))
	{
		m_CurrentLang = str;
		m_CurrentLang.Replace(_T("9x"), _T(""));
#ifdef UNICODE
		m_CurrentLangPath.Format(_T("%s\\%s.lang"), (LPCTSTR)m_LangDir, (LPCTSTR)m_CurrentLang);
#else
		m_CurrentLangPath.Format(_T("%s\\%s9x.lang"), (LPCTSTR)m_LangDir, (LPCTSTR)m_CurrentLang);
#endif
	}
	else
	{
		CString currentLocalID;
		currentLocalID.Format(_T("0x%04X"), GetUserDefaultLCID());
		PrimaryLangID = PRIMARYLANGID(GetUserDefaultLCID());


		langPath.Format(_T("%s\\*.lang"), (LPCTSTR)m_LangDir);

		hFind = ::FindFirstFile(langPath, &findData);
		if(hFind != INVALID_HANDLE_VALUE)
		{
			do{
				if(findData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
				{

					CString cstr;
					cstr = findData.cFileName;

#ifdef UNICODE
					if (cstr.Find(_T("9x.lang")) >= 0)
					{
						continue;
					}
#else
					if (cstr.Find(_T("9x.lang")) == -1)
					{
						continue;
					}
#endif
					i++;

					cstr.Format(_T("%s\\%s"), (LPCTSTR)m_LangDir, findData.cFileName);
					GetPrivateProfileStringFx(_T("Language"), _T("LOCALE_ID"), _T(""), str, 256, cstr);
					if((ptrEnd = _tcsrchr(findData.cFileName, '.')) != NULL){*ptrEnd = '\0';}

					if(_tcsstr(str, currentLocalID) != NULL)
					{
						m_CurrentLang = findData.cFileName;
						m_CurrentLang.Replace(_T("9x"), _T(""));
#ifdef UNICODE
						m_CurrentLangPath.Format(_T("%s\\%s.lang"), (LPCTSTR)m_LangDir, findData.cFileName);
#else
						m_CurrentLangPath.Format(_T("%s\\%s9x.lang"), (LPCTSTR)m_LangDir, findData.cFileName);
#endif
					}
					if(PrimaryLangID == PRIMARYLANGID(_tcstol(str, NULL, 16)))
					{
						PrimaryLang = findData.cFileName;
						PrimaryLang.Replace(_T("9x"), _T(""));
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
#ifdef UNICODE
				m_CurrentLangPath.Format(_T("%s\\%s.lang"), (LPCTSTR)m_LangDir, (LPCTSTR)m_CurrentLang);
#else
				m_CurrentLangPath.Format(_T("%s\\%s9x.lang"), (LPCTSTR)m_LangDir, (LPCTSTR)m_CurrentLang);
#endif
			}
			else
			{
				m_CurrentLang = PrimaryLang;
#ifdef UNICODE
				m_CurrentLangPath.Format(_T("%s\\%s.lang"), (LPCTSTR)m_LangDir, (LPCTSTR)PrimaryLang);
#else
				m_CurrentLangPath.Format(_T("%s\\%s9x.lang"), (LPCTSTR)m_LangDir, (LPCTSTR)PrimaryLang);
#endif
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
	WIN32_FIND_DATA findData;
	 
	HANDLE hFind;
	CString themePath;
	CString themeCssPath;
	CString langPath;
	int i = 0;
	TCHAR *ptrEnd = NULL;
	TCHAR str[256];

	srand((unsigned int)std::time(NULL));

	menu.Attach(GetMenu()->GetSafeHmenu());
	subMenu.Attach(menu.GetSubMenu(MENU_THEME_INDEX)->GetSafeHmenu());

	themePath.Format(_T("%s\\*.*"), (LPCTSTR)m_ThemeDir);

	// Add Random as first choice.
	subMenu.AppendMenu(MF_STRING, (UINT_PTR)WM_THEME_ID + i, m_RandomThemeLabel + m_RandomThemeName);
	i++;
	m_MenuArrayTheme.Add(m_RandomThemeLabel);

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

	if(m_CurrentTheme.Compare(m_RandomThemeLabel) == 0)
	{
		m_CurrentTheme = GetRandomTheme();
		m_RandomThemeName = _T(" (") + m_CurrentTheme + _T(")");
		// Keep currentItemID the same as the first item if "Random".
		currentItemID = WM_THEME_ID;

		SUBMENU_MODIFY_MENU(WM_THEME_ID, MF_STRING, WM_THEME_ID, m_RandomThemeLabel + m_RandomThemeName);
	}
	else if(! FlagHitTheme)
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
	langPath.Format(_T("%s\\*.lang"), (LPCTSTR)m_LangDir);
	i = 0;
	hFind = ::FindFirstFile(langPath, &findData);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		do{
			if(findData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
			{
				// Add Language
				CString cstr;
				cstr = findData.cFileName;
#ifdef UNICODE
				if (cstr.Find(_T("9x.lang")) >= 0)
				{
					continue;
				}
#else
				if (cstr.Find(_T("9x.lang")) == -1)
				{
					continue;
				}
#endif			
				newItemID = WM_LANGUAGE_ID + i;
				i++;

				cstr.Format(_T("%s\\%s"), (LPCTSTR)m_LangDir, findData.cFileName);
				GetPrivateProfileStringFx(_T("Language"), _T("LANGUAGE"), _T(""), str, 256, cstr);
				if((ptrEnd = _tcsrchr(findData.cFileName, L'.')) != NULL)
				{
					*ptrEnd = '\0';
				}

				cstr.Format(_T("%s, [%s]"), str, findData.cFileName);
				if(L'A' <= findData.cFileName[0] && findData.cFileName[0] <= L'N')
				{
					subMenuAN.AppendMenu(MF_STRING, (UINT_PTR)newItemID, cstr);
				}
				else
				{
					subMenuOZ.AppendMenu(MF_STRING, (UINT_PTR)newItemID, cstr);
				}
				m_MenuArrayLang.Add(findData.cFileName);

				cstr = findData.cFileName;
#ifndef UNICODE
				cstr.Replace(_T("9x"), _T(""));
#endif
				if(m_CurrentLang.Compare(cstr) == 0)
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
		AfxMessageBox(_T("FATAL ERROR: Missing Language File!!"));
	}
}

BOOL CMainDialogFx::CheckThemeEdition(CString name)
{
	if (name.Find(_T(".")) != 0) { return TRUE; }

	return FALSE;
}

BOOL CMainDialogFx::OnInitDialog()
{
	return CDialogFx::OnInitDialog();
}

void CMainDialogFx::ChangeTheme(CString themeName)
{
	WritePrivateProfileStringFx(_T("Setting"), m_ThemeKeyName, themeName, m_Ini);
}

BOOL CMainDialogFx::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// Select Theme
	if(WM_THEME_ID <= wParam && wParam < WM_THEME_ID + (WPARAM)m_MenuArrayTheme.GetSize())
	{
		CMenu menu;
		CMenu subMenu;
		menu.Attach(GetMenu()->GetSafeHmenu());
		subMenu.Attach(menu.GetSubMenu(MENU_THEME_INDEX)->GetSafeHmenu());

		m_CurrentTheme = m_MenuArrayTheme.GetAt(wParam - WM_THEME_ID);
		if (m_CurrentTheme.Compare(m_RandomThemeLabel) == 0)
		{
			m_CurrentTheme = GetRandomTheme();
			m_RandomThemeLabel = _T("Random");
			m_RandomThemeName = _T(" (") + m_CurrentTheme + _T(")");

			// ChangeTheme save the theme configuration to profile; so if we are on
			// Random, then save Random to profile.
			ChangeTheme(m_RandomThemeLabel);
		}
		else
		{
			ChangeTheme(m_MenuArrayTheme.GetAt(wParam - WM_THEME_ID));
			m_RandomThemeName = _T("");
		}

		SUBMENU_MODIFY_MENU(WM_THEME_ID, MF_STRING, WM_THEME_ID, m_RandomThemeLabel + m_RandomThemeName);
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

void CMainDialogFx::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = m_MinSizeX;
	lpMMI->ptMinTrackSize.y = m_MinSizeY;

	lpMMI->ptMaxTrackSize.x = m_MaxSizeX;
	lpMMI->ptMaxTrackSize.y = m_MaxSizeY;

	CDialogFx::OnGetMinMaxInfo(lpMMI);
}

void CMainDialogFx::SaveWindowPosition()
{
	WINDOWPLACEMENT place = { sizeof(WINDOWPLACEMENT) };
	GetWindowPlacement(&place);

	CString x, y;
	x.Format(_T("%d"), place.rcNormalPosition.left);
	y.Format(_T("%d"), place.rcNormalPosition.top);
	WritePrivateProfileStringFx(_T("Setting"), _T("X"), x, m_Ini);
	WritePrivateProfileStringFx(_T("Setting"), _T("Y"), y, m_Ini);
}

void CMainDialogFx::RestoreWindowPosition()
{
	const int x = GetPrivateProfileInt(_T("Setting"), _T("X"), INT_MIN, m_Ini);
	const int y = GetPrivateProfileInt(_T("Setting"), _T("Y"), INT_MIN, m_Ini);

	RECT rw, rc;
	GetWindowRect(&rw);

	rc.left = x;
	rc.top = y;
	rc.right = x + rw.right - rw.left;
	rc.bottom = y + rw.bottom - rw.top;

#if _MSC_VER > 1310
	HMONITOR hMonitor = MonitorFromRect(&rc, MONITOR_DEFAULTTONULL);
	if (hMonitor == NULL)
	{
		CenterWindow();
	}
	else
	{
		// Get Taskbar Size
		APPBARDATA	taskbarInfo = { 0 };
		taskbarInfo.cbSize = sizeof(APPBARDATA);
		taskbarInfo.hWnd = m_hWnd;
		SHAppBarMessage(ABM_GETTASKBARPOS, &taskbarInfo);
		CRect taskbarRect = taskbarInfo.rc;

		if (taskbarInfo.rc.top <= 0 && taskbarInfo.rc.left <= 0) // Top Side or Left Side
		{
			if (taskbarRect.Height() > taskbarRect.Width()) // Left Side
			{
				if (x < taskbarRect.Width()) // Overlap
				{
					SetWindowPos(NULL, taskbarRect.Width(), y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
				}
				else
				{
					SetWindowPos(NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
				}
			}
			else // Top Side
			{
				if (y < taskbarRect.Height()) // Overlap
				{
					SetWindowPos(NULL, x, taskbarRect.Height(), 0, 0, SWP_NOSIZE | SWP_NOZORDER);
				}
				else
				{
					SetWindowPos(NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
				}
			}
		}
		else
		{
			SetWindowPos(NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}
	}
#else
	CenterWindow();
#endif
}

DWORD CMainDialogFx::GetZoomType()
{
	return GetPrivateProfileInt(_T("Setting"), _T("ZoomType"), ZoomTypeAuto, m_Ini);
}

void CMainDialogFx::SetZoomType(DWORD zoomType)
{
	CString cstr;
	cstr.Format(_T("%d"), m_ZoomType);
	WritePrivateProfileStringFx(_T("Setting"), _T("ZoomType"), cstr, m_Ini);
}

void CMainDialogFx::UpdateThemeInfo()
{
	CString theme = m_ThemeDir + m_CurrentTheme + _T("\\theme.ini");

	m_LabelText = GetControlColor(_T("LabelText"), 0, theme);
	m_MeterText = GetControlColor(_T("MeterText"), 0, theme);
	m_ComboText = GetControlColor(_T("ComboText"), 0, theme);
	m_ComboTextSelected = GetControlColor(_T("ComboTextSelected"), 0, theme);
	m_ComboBk   = GetControlColor(_T("ComboBk"), 255, theme);
	m_ComboBkSelected = GetControlColor(_T("ComboBkSelected"), 192, theme);
	m_ButtonText= GetControlColor(_T("ButtonText"), 0, theme);
	m_EditText  = GetControlColor(_T("EditText"), 0, theme);
	m_EditBk    = GetControlColor(_T("EditBk"), 255, theme);
	m_ListText1 = GetControlColor(_T("ListText1"), 0, theme);
	m_ListText2 = GetControlColor(_T("ListText2"), 0, theme);
	m_ListTextSelected = GetControlColor(_T("ListTextSelected"), 0, theme);
	m_ListBk1 = GetControlColor(_T("ListBk1"), 255, theme);
	m_ListBk2 = GetControlColor(_T("ListBk2"), 255, theme);
	m_ListBkSelected = GetControlColor(_T("ListBkSelected"), 0, theme);
	m_ListLine1 = GetControlColor(_T("ListLine1"), 0, theme);
	m_ListLine2 = GetControlColor(_T("ListLine2"), 0, theme);
	m_Glass = GetControlColor(_T("Glass"), 255, theme);
	m_Frame = GetControlColor(_T("Frame"), 128, theme);
	m_Background = GetBackgroundColor(_T("Background"), theme);

	m_ComboAlpha = GetControlAlpha(_T("ComboAlpha"), 255, theme);
	m_EditAlpha = GetControlAlpha(_T("EditAlpha"), 255, theme);
	m_GlassAlpha = GetControlAlpha(_T("GlassAlpha"), 128, theme);

	m_CharacterPosition = GetCharacterPosition(theme);

	m_ParentTheme1 = GetParentTheme(1, theme);
	m_ParentTheme2 = GetParentTheme(2, theme);
}

COLORREF CMainDialogFx::GetControlColor(CString name, BYTE defaultColor, CString theme)
{
	COLORREF reverseColor;

	reverseColor = GetPrivateProfileInt(_T("Color"), name, RGB(defaultColor, defaultColor, defaultColor), theme);
	
	COLORREF color = RGB(GetBValue(reverseColor), GetGValue(reverseColor), GetRValue(reverseColor));

	return color;
}

COLORREF CMainDialogFx::GetBackgroundColor(CString name, CString theme)
{
	COLORREF reverseColor;

	reverseColor = GetPrivateProfileInt(_T("Color"), name, 0xFFFFFFFF, theme);

	if (reverseColor == 0xFFFFFFFF)
	{
		return 0xFFFFFFFF; // 0xFFFFFFF = Disabled
	}
	else
	{
		COLORREF color = RGB(GetBValue(reverseColor), GetGValue(reverseColor), GetRValue(reverseColor));
		return color;
	}	
}

BYTE CMainDialogFx::GetControlAlpha(CString name, BYTE defaultAlpha, CString theme)
{
	BYTE alpha = (BYTE)GetPrivateProfileInt(_T("Alpha"), name, defaultAlpha, theme);
	
	return alpha;
}

BYTE CMainDialogFx::GetCharacterPosition(CString theme)
{
	BYTE position = (BYTE)GetPrivateProfileInt(_T("Character"), _T("Position"), 0, theme);

	return position;
}

CString CMainDialogFx::GetParentTheme(int i, CString theme)
{
	CString cstr;
	cstr.Format(_T("ParentTheme%d"), i);
	TCHAR str[256];
	GetPrivateProfileStringFx(_T("Info"), cstr, _T(""), str, 256, theme);
	cstr = str;

	return cstr;
}

CString CMainDialogFx::GetRandomTheme() {
	// We need to add/subtract one to exclude first item ("Random").
	UINT i = 1 + rand() % ((UINT)m_MenuArrayTheme.GetSize() - 1);
	return m_MenuArrayTheme.GetAt(i);
}

void CMainDialogFx::SaveImage()
{
#if _MSC_VER > 1310
	BOOL bDwmEnabled = FALSE;

	HMODULE hModule = LoadLibraryEx(_T("dwmapi.dll"), NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
	typedef HRESULT(WINAPI* FuncDwmGetWindowAttribute) (HWND hwnd, DWORD dwAttribute, PVOID pvAttribute, DWORD cbAttribute);
	typedef HRESULT(WINAPI* FuncDwmIsCompositionEnabled)(BOOL* pfEnabled);
	FuncDwmGetWindowAttribute pDwmGetWindowAttribute = NULL;
	FuncDwmIsCompositionEnabled pDwmIsCompositionEnabled = NULL;
	if (hModule)
	{
		pDwmGetWindowAttribute = (FuncDwmGetWindowAttribute)GetProcAddress(hModule, "DwmGetWindowAttribute");
		pDwmIsCompositionEnabled = (FuncDwmIsCompositionEnabled)GetProcAddress(hModule, "DwmIsCompositionEnabled");
	}

	if (pDwmGetWindowAttribute && pDwmIsCompositionEnabled)
	{
		pDwmIsCompositionEnabled(&bDwmEnabled);
	}

	CRect rc1;
	CRect rc2;
	CRect rc3;

	CImage* image1 = new CImage();
	CImage* image2 = new CImage();
	CImage* image3 = new CImage();

	if (bDwmEnabled)
	{
		GetWindowRect(&rc1);
		if (image1->Create(rc1.Width(), rc1.Height(), 32))
		{
			HDC hImage1DC = image1->GetDC();
			if (IsWin81orLater())
			{
				::PrintWindow(m_hWnd, hImage1DC, 2); // PW_RENDERFULLCONTENT, Windows 8.1 or later

				GetClientRect(&rc3);
				if (image3->Create(rc3.Width(), rc3.Height(), 32))
				{
					HDC hImage3DC = image3->GetDC();
					::PrintWindow(m_hWnd, hImage3DC, 1); // PW_CLIENTONLY
					int targetY = 0;
					int offsetX = (rc1.Width() - rc3.Width()) / 2;
					int offsetY = (rc1.Height() - rc3.Height()) / 2;

					// Compare Screenshot
					for (int y = offsetY; y < rc1.Height() - rc3.Height(); y++)
					{
						for (int x = 0; x < rc3.Width(); x++)
						{
							if (image1->GetPixel(offsetX + x, y) == image3->GetPixel(x, 0))
							{
								if (x == rc3.Width() / 2)
								{
									for (int yy = 0; yy < rc3.Height(); yy++)
									{
										if (image1->GetPixel(offsetX, y + yy) == image3->GetPixel(0, yy))
										{
											if (yy == rc3.Height() / 2)
											{
												targetY = y;
												goto loopEnd;
											}
										}
										else
										{
											break;
										}
									}
								}
							}
							else
							{
								break;
							}
						}
					}

				loopEnd:
					if (targetY > 0)
					{
						::BitBlt(hImage1DC, (rc1.Width() - rc3.Width()) / 2, targetY, rc3.Width(), rc3.Height(), hImage3DC, 0, 0, SRCCOPY);
					}
					image3->ReleaseDC();
				}
			}
			else
			{
				::PrintWindow(m_hWnd, hImage1DC, 0);
			}

			pDwmGetWindowAttribute(m_hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rc2, sizeof(rc2));
			// cstr.Format(_T("Width=%d/%d, Height=%d/%d"), rc1.Width(), rc2.Width(), rc1.Height(), rc2.Height());
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
		if (image1->Create(rc1.Width(), rc1.Height(), 32))
		{
			HDC hImage1DC = image1->GetDC();
			::PrintWindow(m_hWnd, hImage1DC, 0);
			SaveImageDlg(image1);
			image1->ReleaseDC();
		}
	}
	SAFE_DELETE(image1);
	SAFE_DELETE(image2);
	SAFE_DELETE(image3);
#endif
}

void CMainDialogFx::SaveImageDlg(CImage* image)
{
	CString path;
	SYSTEMTIME st;
	GetLocalTime(&st);
	path.Format(_T("%s_%04d%02d%02d%02d%02d%02d"), PRODUCT_FILENAME, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	CString filter = _T("PNG (*.png)|*.png|JPEG (*.jpg)|*.jpg|BMP (*.bmp)|*.bmp||");
	CFileDialog save(FALSE, _T(""), path, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER, filter);

	if (save.DoModal() == IDOK)
	{
		HRESULT result = image->Save(save.GetPathName());
		if (SUCCEEDED(result))
		{
			//	AfxMessageBox(_T("SUCCEEDED");
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// Task Tray
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef OPTION_TASK_TRAY

UINT CMainDialogFx::wmTaskbarCreated = ::RegisterWindowMessage(_T("TaskbarCreated"));

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