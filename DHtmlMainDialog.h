/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#pragma once

#define MAIN_CSS_FILE_NAME	_T("Main.css")

class CDHtmlMainDialog : public CDHtmlDialogEx
{
public:
	CDHtmlMainDialog(UINT dlgResouce, UINT dlgHtml, 
		CString ThemeDir, DWORD ThemeIndex, CString LangDir, DWORD LangIndex,
		CWnd* pParent = NULL);
	virtual ~CDHtmlMainDialog();

	CString m_CurrentLocalID;

	CString m_ThemeDir;
	CString m_LangDir;
	DWORD m_ThemeIndex;
	DWORD m_LangIndex;

	CStringArray m_MenuArrayTheme;
	CStringArray m_MenuArrayLang;
	CString m_CurrentTheme;
	CString m_CurrentLang;

	void InitMenu();
	void InitThemeLang();
	void ChangeTheme(CString ThemeName);
	void SetWindowTitle(CString message, CString mode = _T(""));

	BOOL m_bStartup;
	BOOL m_bInitializing;
	BOOL m_bWindoowMinimizeOnce;
	BOOL m_bResidentMinimize;

	// Task Tray
	BOOL m_bResident;
	static UINT wmTaskbarCreated;
	BOOL AddTaskTray(UINT id, UINT callback, HICON icon, CString tip);
	BOOL RemoveTaskTray(UINT id);
	BOOL ModifyTaskTray(UINT id, HICON icon, CString tip);
	BOOL ModifyTaskTrayIcon(UINT id, HICON icon);
	BOOL ModifyTaskTrayTip(UINT id, CString tip);
	BOOL ShowBalloon(UINT id, DWORD infoFlag, CString infoTitle, CString info);

	DWORD GetZoomType();
	void SetZoomType(DWORD zoomType);

	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	DECLARE_DHTML_EVENT_MAP()

};