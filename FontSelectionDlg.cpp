/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DiskInfo.h"
#include "DiskInfoDlg.h"
#include "FontSelectionDlg.h"

int CALLBACK EnumFontFamExProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, int FontType, LPARAM lParam);

IMPLEMENT_DYNAMIC(CFontSelectionDlg, CDialog)

CFontSelectionDlg::CFontSelectionDlg(CWnd* pParent)
	: CDialogFx(CFontSelectionDlg::IDD, pParent)
{
	CMainDialogFx* p = (CMainDialogFx*)pParent;

	m_ZoomType = p->GetZoomType();
	m_FontScale = p->GetFontScale();
	m_FontRatio = 1.0; // p->GetFontRatio();
	m_FontFace = p->GetFontFace();
	m_FontRender = p->GetFontRender();
	m_CurrentLangPath = p->GetCurrentLangPath();
	m_DefaultLangPath = p->GetDefaultLangPath();
	m_ThemeDir = p->GetThemeDir();
	m_CurrentTheme = p->GetCurrentTheme();
	m_DefaultTheme = p->GetDefaultTheme();
	m_Ini = p->GetIniPath();

	m_BackgroundName = L"";
}

CFontSelectionDlg::~CFontSelectionDlg()
{
}

void CFontSelectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OK, m_CtrlOk);
	DDX_Control(pDX, IDC_FONT_FACE,  m_LabelFontFace);
	DDX_Control(pDX, IDC_FONT_SCALE, m_LabelFontScale);
	DDX_Control(pDX, IDC_FONT_RENDER, m_LabelFontRender);
	DDX_Control(pDX, IDC_FONT_FACE_COMBO, m_CtrlFontFace);
	DDX_Control(pDX, IDC_FONT_SCALE_COMBO, m_CtrlFontScale);
	DDX_Control(pDX, IDC_FONT_RENDER_COMBO, m_CtrlFontRender);
	DDX_Control(pDX, IDC_SET_DEFAULT, m_CtrlDefault);
}

BEGIN_MESSAGE_MAP(CFontSelectionDlg, CDialogFx)
	ON_BN_CLICKED(IDC_OK, &CFontSelectionDlg::OnOk)
	ON_BN_CLICKED(IDC_SET_DEFAULT, &CFontSelectionDlg::OnSetDefault)
END_MESSAGE_MAP()

BOOL CFontSelectionDlg::OnInitDialog()
{
	CDialogFx::OnInitDialog();

	SetWindowTitle(i18n(L"WindowTitle", L"FONT_SETTING"));

	SetDefaultFont(m_FontFace);

	CString cstr;
	for (int i = 50; i <= 150; i += 10)
	{
		cstr.Format(L"%d", i);
		m_CtrlFontScale.AddString(cstr);
		if (m_FontScale == i) { m_CtrlFontScale.SetCurSel(m_CtrlFontScale.GetCount() - 1);  }
	}

	m_CtrlFontRender.AddString(i18n(L"Dialog", L"ENABLED"));
	m_CtrlFontRender.AddString(i18n(L"Dialog", L"DISABLED"));

	if (m_FontRender == CLEARTYPE_NATURAL_QUALITY)
	{
		m_CtrlFontRender.SetCurSel(0);
	}
	else
	{
		m_CtrlFontRender.SetCurSel(1);
	}

	m_LabelFontFace.SetWindowTextW(i18n(L"Dialog", L"FONT_FACE"));
	m_LabelFontScale.SetWindowTextW(i18n(L"Dialog", L"FONT_SCALE"));
	m_LabelFontRender.SetWindowTextW(L"ClearType");
	m_CtrlDefault.SetWindowTextW(i18n(L"Dialog", L"DEFAULT"));
	m_CtrlOk.SetWindowTextW(i18n(L"Dialog", L"OK"));

	UpdateDialogSize();

	return TRUE;
}

void CFontSelectionDlg::UpdateDialogSize()
{
	CDialogFx::UpdateDialogSize();

	COLORREF textColor = RGB(0, 0, 0);
	COLORREF textSelectedColor = RGB(0, 0, 0);

	ChangeZoomType(m_ZoomType);
	SetClientSize(SIZE_X, SIZE_Y, m_ZoomRatio);

	UpdateBackground(FALSE, m_bDarkMode);

	m_LabelFontFace.SetFontEx(m_FontFace, 16, 16, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);
	m_LabelFontScale.SetFontEx(m_FontFace, 16, 16, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);
	m_LabelFontRender.SetFontEx(m_FontFace, 16, 16, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);

	m_LabelFontFace.InitControl(8, 8, 432, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, FALSE);
	m_LabelFontScale.InitControl(8, 80, 208, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, FALSE);
	m_LabelFontRender.InitControl(240, 80, 208, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, FALSE);
	m_CtrlFontFace.InitControl(20, 32, 440, 360, m_ZoomRatio, &m_BkDC, NULL, 0, ES_LEFT, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, RGB(255, 255, 255), RGB(160, 220, 255), RGB(255, 255, 255), 0);
	m_CtrlFontScale.InitControl(20, 104, 208, 360, m_ZoomRatio, &m_BkDC, NULL, 0, ES_LEFT, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, RGB(255, 255, 255), RGB(160, 220, 255), RGB(255, 255, 255), 0);
	m_CtrlFontRender.InitControl(252, 104, 208, 360, m_ZoomRatio, &m_BkDC, NULL, 0, ES_LEFT, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, RGB(255, 255, 255), RGB(160, 220, 255), RGB(255, 255, 255), 0);

	m_CtrlDefault.InitControl(40, 156, 168, 32, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, SystemDraw, m_bHighContrast, m_bDarkMode, FALSE);
	m_CtrlOk.InitControl(272, 156, 168, 32, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, SystemDraw, m_bHighContrast, m_bDarkMode, FALSE);

	m_CtrlFontFace.SetMargin(0, 4, 0, 0, m_ZoomRatio);
	m_CtrlFontScale.SetMargin(0, 4, 0, 0, m_ZoomRatio);
	m_CtrlFontRender.SetMargin(0, 4, 0, 0, m_ZoomRatio);

	m_CtrlFontFace.SetFontHeight(20, m_ZoomRatio, m_FontRatio);
	m_CtrlFontFace.SetFontEx(m_FontFace, 20, 20, m_ZoomRatio, m_FontRatio, textColor, textSelectedColor, FW_NORMAL, m_FontRender);
	m_CtrlFontFace.SetItemHeightEx(-1, 36, m_ZoomRatio, m_FontRatio);
	for (int i = 0; i < m_CtrlFontFace.GetCount(); i++)
	{
		m_CtrlFontFace.SetItemHeightEx(i, 32, m_ZoomRatio, m_FontRatio);
	}

	m_CtrlFontScale.SetFontHeight(20, m_ZoomRatio, m_FontRatio);
	m_CtrlFontScale.SetFontEx(m_FontFace, 20, 20, m_ZoomRatio, m_FontRatio, textColor, textSelectedColor, FW_NORMAL, m_FontRender);
	m_CtrlFontScale.SetItemHeightEx(-1, 36, m_ZoomRatio, m_FontRatio);
	for (int i = 0; i < m_CtrlFontScale.GetCount(); i++)
	{
		m_CtrlFontScale.SetItemHeightEx(i, 32, m_ZoomRatio, m_FontRatio);
	}

	m_CtrlFontRender.SetFontHeight(20, m_ZoomRatio, m_FontRatio);
	m_CtrlFontRender.SetFontEx(m_FontFace, 20, 20, m_ZoomRatio, m_FontRatio, textColor, textSelectedColor, FW_NORMAL, m_FontRender);
	m_CtrlFontRender.SetItemHeightEx(-1, 36, m_ZoomRatio, m_FontRatio);
	for (int i = 0; i < m_CtrlFontRender.GetCount(); i++)
	{
		m_CtrlFontRender.SetItemHeightEx(i, 32, m_ZoomRatio, m_FontRatio);
	}

	m_CtrlDefault.SetFontEx(m_FontFace, 16, 16, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);
	m_CtrlOk.SetFontEx(m_FontFace, 16, 16, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);
	
	m_CtrlDefault.SetHandCursor();
	m_CtrlOk.SetHandCursor();

	SetDarkModeControl(m_CtrlDefault.GetSafeHwnd(), m_bDarkMode);
	SetDarkModeControl(m_CtrlOk.GetSafeHwnd(), m_bDarkMode);

	Invalidate();
}

void CFontSelectionDlg::OnOk()
{
	CString cstr;

	m_CtrlFontFace.GetLBText(m_CtrlFontFace.GetCurSel(), m_FontFace);
	m_CtrlFontScale.GetLBText(m_CtrlFontScale.GetCurSel(), cstr);
	m_FontScale = _wtoi(cstr);
	if (m_CtrlFontRender.GetCurSel() == 0)
	{
		m_FontRender = CLEARTYPE_NATURAL_QUALITY;
	}
	else
	{
		m_FontRender = ANTIALIASED_QUALITY;
	}

	CDialog::OnOK();
}

int CALLBACK EnumFontFamExProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, int FontType, LPARAM lParam)
{
	CFontComboBox* pFontComboBox = (CFontComboBox*)lParam;
	if(pFontComboBox->FindStringExact(0, (TCHAR*)lpelfe->elfLogFont.lfFaceName) == CB_ERR
	&& _tcschr((TCHAR*)lpelfe->elfLogFont.lfFaceName, L'@') == NULL
	&& lpelfe->elfLogFont.lfCharSet != SYMBOL_CHARSET
	)
	{
		pFontComboBox->AddString((TCHAR*)lpelfe->elfLogFont.lfFaceName);
	}
    return TRUE;
}

void CFontSelectionDlg::OnSetDefault()
{
	SetDefaultFont(L"");
	m_CtrlFontScale.SetCurSel(5);
	m_CtrlFontRender.SetCurSel(0);
}

void CFontSelectionDlg::SetDefaultFont(CString fontFace)
{
	m_CtrlFontFace.ResetContent();

	CClientDC dc(this);
	LOGFONT logfont;
	ZeroMemory(&logfont, sizeof(LOGFONT));
	logfont.lfCharSet = DEFAULT_CHARSET;

	::EnumFontFamiliesExW(dc.m_hDC, &logfont, (FONTENUMPROC)EnumFontFamExProc, (LPARAM)&m_CtrlFontFace, 0);

	int no = m_CtrlFontFace.FindStringExact(0, fontFace);
	if (no >= 0)
	{
		m_CtrlFontFace.SetCurSel(no);
	}
	else
	{
		no = m_CtrlFontFace.FindStringExact(0, DEFAULT_FONT_FACE_1);
		if (no >= 0)
		{
			m_CtrlFontFace.SetCurSel(no);
		}
		else
		{
			no = m_CtrlFontFace.FindStringExact(0, DEFAULT_FONT_FACE_2);
			if (no >= 0)
			{
				m_CtrlFontFace.SetCurSel(no);
			}
			else
			{
				m_CtrlFontFace.SetCurSel(0);
			}
		}
	}

	for (int i = 0; i < m_CtrlFontFace.GetCount(); i++)
	{
		m_CtrlFontFace.SetItemHeightEx(i, 32, m_ZoomRatio, m_FontRatio);
	}
}