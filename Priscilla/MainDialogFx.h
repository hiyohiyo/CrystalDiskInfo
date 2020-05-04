/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once
#include "DialogFx.h"
#include "UtilityFx.h"
#include "OsInfoFx.h"

class CMainDialogFx : public CDialogFx
{
public:
	CMainDialogFx(UINT dlgResouce, CWnd* pParent = NULL);
	virtual ~CMainDialogFx();

	// Zoom
	DWORD GetZoomType();
	void SetZoomType(DWORD zoomType);

	// Getter
	int GetFontScale();
	double GetFontRatio();
	CString GetFontFace();
	CString GetCurrentLangPath();
	CString GetDefaultLangPath();
	CString GetThemeDir();
	CString GetCurrentTheme();
	CString GetDefaultTheme();
	CString GetParentTheme1();
	CString GetParentTheme2();
	CString GetIniPath();

protected:
	void InitMenu();
	void InitThemeLang();
	void ChangeTheme(CString ThemeName);
	void SetWindowTitle(CString message);
	void UpdateThemeInfo();
	COLORREF GetControlColor(CString name, BYTE defaultColor, CString theme);
	BYTE GetControlAlpha(CString name, BYTE defaultAlpha, CString theme);
	BYTE GetCharacterPosition(CString theme);
	CString GetParentTheme(int i, CString theme);

	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL CheckThemeEdition(CString name);
	virtual CString GetDefaultFont();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);

	// Common
	BOOL m_bStartup;
	BOOL m_bWindowMinimizeOnce;
	BOOL m_bResident;
	BOOL m_bResidentMinimize;

	// Theme
	CString m_ThemeKeyName;
	CString m_RecommendTheme;
	CStringArray m_MenuArrayTheme;

	// Language
	CStringArray m_MenuArrayLang;

#ifdef OPTION_TASK_TRAY
	// Task Tray
	static UINT wmTaskbarCreated;
	BOOL AddTaskTray(UINT id, UINT callback, HICON icon, CString tip);
	BOOL RemoveTaskTray(UINT id);
	BOOL ModifyTaskTray(UINT id, HICON icon, CString tip);
	BOOL ModifyTaskTrayIcon(UINT id, HICON icon);
	BOOL ModifyTaskTrayTip(UINT id, CString tip);
	BOOL ShowBalloon(UINT id, DWORD infoFlag, CString infoTitle, CString info);
#endif

};