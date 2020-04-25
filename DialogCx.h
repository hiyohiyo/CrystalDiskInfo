/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once

#include "atlimage.h"

class CDialogFx : public CDialog
{
public:
	// Font Type
	static const int FT_AUTO      = 0x0000;
	static const int FT_GDI       = 0x0001;
	static const int FT_GDI_PLUS  = 0x0002;
	static const int FT_D_WRITE   = 0x0003;

	CDialogFx(UINT dlgResouce, CWnd* pParent = NULL);
	virtual ~CDialogFx();

	void SetClientRect(DWORD sizeX, DWORD sizeY, DWORD menuLine = 0);
	void ShowWindowEx(int nCmdShow);

	virtual BOOL Create(UINT nIDTemplate, CWnd* dlgWnd, UINT menuId, CWnd* pParentWnd = NULL);

	CString m_FontFace;
	CString m_CurrentLangPath;
	CString m_DefaultLangPath;
	INT m_FontType;

	enum ZOOM_TYPE
	{
		ZoomTypeAuto = 0,
		ZoomType100 = 100,
		ZoomType125 = 125,
		ZoomType150 = 150,
		ZoomType200 = 200,
		ZoomType250 = 250,
		ZoomType300 = 300,
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	HACCEL m_hAccelerator;
	BOOL m_bShowWindow;
	BOOL m_bModelessDlg;
	CWnd* m_ParentWnd;
	CWnd* m_DlgWnd;
	UINT m_MenuId;
	TCHAR m_Ini[MAX_PATH];

	double m_ZoomRatio;
	DWORD m_ZoomType;
	BOOL m_IsHighContrast;
	BOOL m_IsDrawFrame;
	INT m_Dpi;

	CString m_CxThemeDir;
	CString m_CxCurrentTheme;
	CString m_CxDefaultTheme;

	CString m_BackgroundName;
	CBitmap m_BitmapBg;
	CDC		m_BgDC;
	CImage m_ImageBg;
	CBrush m_BrushDlg;

	DWORD ChangeZoomType(DWORD zoomType);
	double GetZoomRatio();
	CString i18n(CString section, CString key, BOOL inEnglish = FALSE);
	void OpenUrl(CString url);
	BOOL ClickCheck();
	BOOL IsHighContrast();
	BOOL IsDrawFrame();

	virtual void SetupControl(int nIDDlgItem , int x, int y, int width, int height);
	virtual void UpdateDialogSize();
	virtual void UpdateBackground(bool resize = false);
	virtual CString IP(CString imageName);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSysColorChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDisplayChange(WPARAM wParam, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
//	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
};
