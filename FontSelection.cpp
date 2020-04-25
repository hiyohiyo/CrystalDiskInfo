/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DiskInfo.h"
#include "DiskInfoDlg.h"
#include "FontSelection.h"

int CALLBACK EnumFontFamExProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, int FontType, LPARAM lParam);

IMPLEMENT_DYNAMIC(CFontSelection, CDialog)

CFontSelection::CFontSelection(CWnd* pParent)
	: CDialogFx(CFontSelection::IDD, pParent)
{
	CMainDialog* p = (CMainDialog*)pParent;

	m_ZoomType = p->GetZoomType();
	m_FontScale = p->GetFontScale();
	m_FontRatio = p->GetFontRatio();
	m_FontFace = p->GetFontFace();
	m_CurrentLangPath = p->GetCurrentLangPath();
	m_DefaultLangPath = p->GetDefaultLangPath();
	m_ThemeDir = p->GetThemeDir();
	m_CurrentTheme = p->GetCurrentTheme();
	m_DefaultTheme = p->GetDefaultTheme();
	m_Ini = p->GetIniPath();

	m_BackgroundName = L"";
}

CFontSelection::~CFontSelection()
{
}

void CFontSelection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, ID_OK, m_CtrlOk);
	DDX_Control(pDX, IDC_FONT_FACE, m_LabelFontFace);
	DDX_Control(pDX, IDC_FONT_SCALE, m_LabelFontScale);
	DDX_Control(pDX, IDC_FONT_COMBO, m_FontComboBox);
	DDX_Control(pDX, IDC_FONT_SCALE_COMBO, m_FontScaleComboBox);
	DDX_Control(pDX, IDC_SET_DEFAULT, m_ButtonSetDefault);
}

BEGIN_MESSAGE_MAP(CFontSelection, CDialogFx)
	ON_BN_CLICKED(ID_OK, &CFontSelection::OnBnClickedOk)
	ON_BN_CLICKED(IDC_SET_DEFAULT, &CFontSelection::OnSetDefault)
END_MESSAGE_MAP()

BOOL CFontSelection::OnInitDialog()
{
	CDialogFx::OnInitDialog();

	SetWindowText(i18n(L"WindowTitle", L"FONT_SETTING"));

    CClientDC dc(this);
    LOGFONT logfont; 
    ZeroMemory(&logfont, sizeof(LOGFONT)); 
    logfont.lfCharSet = DEFAULT_CHARSET;

    ::EnumFontFamiliesExW(dc.m_hDC, &logfont, (FONTENUMPROC)EnumFontFamExProc, (LPARAM)&m_FontComboBox, 0);

	int no = m_FontComboBox.FindStringExact(0, m_FontFace);
	if(no >= 0)
	{
		m_FontComboBox.SetCurSel(no);
	}
	else
	{
		no = m_FontComboBox.FindStringExact(0, L"Segoe UI");
		if(no >= 0)
		{
			m_FontComboBox.SetCurSel(no);
		}
		else
		{
			no = m_FontComboBox.FindStringExact(0, L"Tahoma");
			if(no >= 0)
			{
				m_FontComboBox.SetCurSel(no);
			}
			else
			{
				m_FontComboBox.SetCurSel(0);
			}
		}
	}

	CString cstr;

	for (int i = 50; i <= 150; i += 10)
	{
		cstr.Format(L"%d", i);
		m_FontScaleComboBox.AddString(cstr);
		if (m_FontScale == i) { m_FontScaleComboBox.SetCurSel(m_FontScaleComboBox.GetCount() - 1);  }
	}

	m_LabelFontFace.SetWindowTextW(i18n(L"Dialog", L"FONT_FACE"));
	m_LabelFontScale.SetWindowTextW(i18n(L"Dialog", L"FONT_SCALE"));

	m_ButtonSetDefault.SetWindowTextW(i18n(L"Dialog", L"DEFAULT"));

	UpdateDialogSize();

	return TRUE;
}

void CFontSelection::UpdateDialogSize()
{
	BYTE textAlpha = 255;
	COLORREF textColor = RGB(0, 0, 0);
	COLORREF textSelectedColor = RGB(0, 0, 0);

	ChangeZoomType(m_ZoomType);
	SetClientSize((int)(SIZE_X * m_ZoomRatio), (int)(SIZE_Y * m_ZoomRatio), 0);

	UpdateBackground();

	m_LabelFontFace.InitControl(8, 8, 472, 24, m_ZoomRatio, &m_BgDC, NULL, 0, SS_LEFT, OwnerDrawTransparent | m_bHighContrast);
	m_LabelFontScale.InitControl(8, 96, 472, 24, m_ZoomRatio, &m_BgDC, NULL, 0, SS_LEFT, OwnerDrawTransparent | m_bHighContrast);
	m_FontComboBox.InitControl(20, 36, 480, 360, m_ZoomRatio, &m_BgDC, NULL, 0, ES_LEFT, OwnerDrawGlass | m_bHighContrast, RGB(255, 255, 255), RGB(160, 220, 255), RGB(255, 255, 255), 0);
	m_FontScaleComboBox.InitControl(20, 124, 480, 360, m_ZoomRatio, &m_BgDC, NULL, 0, ES_LEFT, OwnerDrawGlass | m_bHighContrast, RGB(255, 255, 255), RGB(160, 220, 255), RGB(255, 255, 255), 0);

	m_FontComboBox.SetMargin(0, 4, 0, 0, m_ZoomRatio);
	m_FontScaleComboBox.SetMargin(0, 4, 0, 0, m_ZoomRatio);
	m_ButtonSetDefault.SetGlassColor(RGB(0, 0, 0), 128);
	m_CtrlOk.SetGlassColor(RGB(255, 255, 255), 128);

	m_ButtonSetDefault.InitControl(40, 180, 160, 32, m_ZoomRatio, &m_BgDC, NULL, 0, BS_CENTER, SystemDraw | m_bHighContrast);
	m_CtrlOk.InitControl(320, 180, 160, 32, m_ZoomRatio, &m_BgDC, NULL, 0, BS_CENTER, SystemDraw | m_bHighContrast);

	m_LabelFontFace.SetFontEx(m_FontFace, 20, 20, m_ZoomRatio, m_FontRatio);
	m_LabelFontScale.SetFontEx(m_FontFace, 20, 20, m_ZoomRatio, m_FontRatio);

	m_FontComboBox.SetFontHeight(28, m_ZoomRatio, m_FontRatio);
	m_FontComboBox.SetFontEx(m_FontFace, 28, 28, m_ZoomRatio, m_FontRatio, textAlpha, textColor, textSelectedColor, FW_NORMAL);
	m_FontComboBox.SetItemHeightEx(-1, 44, m_ZoomRatio, m_FontRatio);
	for (int i = 0; i < m_FontComboBox.GetCount(); i++)
	{
		m_FontComboBox.SetItemHeightEx(i, 44, m_ZoomRatio, m_FontRatio);
	}

	m_FontScaleComboBox.SetFontEx(m_FontFace, 20, 20, m_ZoomRatio, m_FontRatio, textAlpha, textColor, textSelectedColor, FW_NORMAL);
	m_FontScaleComboBox.SetFontEx(m_FontFace, 20, 20, m_ZoomRatio, m_FontRatio, textAlpha, textColor, textSelectedColor, FW_NORMAL);
	m_FontScaleComboBox.SetItemHeightEx(-1, 28, m_ZoomRatio, m_FontRatio);
	for (int i = 0; i < m_FontScaleComboBox.GetCount(); i++)
	{
		m_FontScaleComboBox.SetItemHeightEx(i, 28, m_ZoomRatio, m_FontRatio);
	}

	m_ButtonSetDefault.SetFontEx(m_FontFace, 20, 20, m_ZoomRatio, m_FontRatio);
	m_CtrlOk.SetFontEx(m_FontFace, 20, 20, m_ZoomRatio, m_FontRatio);
	
	m_ButtonSetDefault.SetHandCursor();
	m_CtrlOk.SetHandCursor();

	Invalidate();
}

CString CFontSelection::GetFontFace()
{
	return m_FontFace;
}

int CFontSelection::GetFontScale()
{
	return m_FontScale;
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

void CFontSelection::OnBnClickedOk()
{
	CString cstr;

	m_FontComboBox.GetLBText(m_FontComboBox.GetCurSel(), m_FontFace);
	m_FontScaleComboBox.GetLBText(m_FontScaleComboBox.GetCurSel(), cstr);
	m_FontScale = _wtoi(cstr);

	CDialog::OnOK();
}


void CFontSelection::OnSetDefault()
{
	m_FontComboBox.ResetContent();

	CClientDC dc(this);
	LOGFONT logfont;
	ZeroMemory(&logfont, sizeof(LOGFONT));
	logfont.lfCharSet = DEFAULT_CHARSET;

	::EnumFontFamiliesExW(dc.m_hDC, &logfont, (FONTENUMPROC)EnumFontFamExProc, (LPARAM)& m_FontComboBox, 0);

	int no = m_FontComboBox.FindStringExact(0, L"Segoe UI");
	if (no >= 0)
	{
		m_FontComboBox.SetCurSel(no);
	}
	else
	{
		no = m_FontComboBox.FindStringExact(0, L"Tahoma");
		if (no >= 0)
		{
			m_FontComboBox.SetCurSel(no);
		}
		else
		{
			m_FontComboBox.SetCurSel(0);
		}
	}

	m_FontScaleComboBox.SetCurSel(5);
}
