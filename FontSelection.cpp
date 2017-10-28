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

static CDiskInfoDlg *p;

CFontSelection::CFontSelection(CWnd* pParent)
	: CDialogCx(CFontSelection::IDD, pParent)
{
	p = (CDiskInfoDlg*)pParent;
	_tcscpy_s(m_Ini, MAX_PATH, ((CDiskInfoApp*)AfxGetApp())->m_Ini);

	m_CurrentLangPath = ((CMainDialog*)pParent)->m_CurrentLangPath;
	m_DefaultLangPath = ((CMainDialog*)pParent)->m_DefaultLangPath;
	m_ZoomType = ((CMainDialog*)pParent)->GetZoomType();
	m_FontFace = ((CMainDialog*)pParent)->m_FontFace;
	m_FontType = ((CMainDialog*) pParent)->m_FontType;

	m_CxThemeDir = ((CDiskInfoApp*)AfxGetApp())->m_ThemeDir;
	m_CxCurrentTheme = ((CMainDialog*)pParent)->m_CurrentTheme;
	m_CxDefaultTheme = ((CMainDialog*)pParent)->m_DefaultTheme;
}

CFontSelection::~CFontSelection()
{
}

void CFontSelection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, ID_OK, m_CtrlOk);
	DDX_Control(pDX, IDC_FONT_COMBO, m_FontComboBox);
	DDX_Control(pDX, IDC_FONT_TYPE_COMBO, m_FontTypeComboBox);
}


BEGIN_MESSAGE_MAP(CFontSelection, CDialogCx)
	ON_BN_CLICKED(ID_OK, &CFontSelection::OnBnClickedOk)
END_MESSAGE_MAP()


BOOL CFontSelection::OnInitDialog()
{
	CDialogCx::OnInitDialog();

	SetWindowText(i18n(_T("WindowTitle"), _T("FONT_SETTING")));

    CClientDC dc(this);
    LOGFONT logfont; 
    ZeroMemory(&logfont, sizeof(LOGFONT)); 
    logfont.lfCharSet = ANSI_CHARSET;

    ::EnumFontFamiliesExW(dc.m_hDC, &logfont, (FONTENUMPROC)EnumFontFamExProc, (LPARAM)&m_FontComboBox, 0);

	int no = m_FontComboBox.FindStringExact(0, m_FontFace);
	if(no >= 0)
	{
		m_FontComboBox.SetCurSel(no);
	}
	else
	{
		no = m_FontComboBox.FindStringExact(0, _T("ƒƒCƒŠƒI"));
		if(no >= 0)
		{
			m_FontComboBox.SetCurSel(no);
		}
		else
		{
			no = m_FontComboBox.FindStringExact(0, _T("Tahoma"));
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

	m_FontTypeComboBox.AddString(i18n(_T("HealthStatus"), _T("Default")));
	m_FontTypeComboBox.AddString(L"GDI");
	m_FontTypeComboBox.AddString(L"GDI+");
//	m_FontTypeComboBox.AddString(L"DirectWrite");
	if (0 <= m_FontType && m_FontType <= 2)
	{
		m_FontTypeComboBox.SetCurSel(m_FontType);
	}
	else
	{
		m_FontTypeComboBox.SetCurSel(0);
	}

	UpdateDialogSize();

	return TRUE;
}

void CFontSelection::UpdateDialogSize()
{
	ChangeZoomType(m_ZoomType);
	SetClientRect((DWORD)(SIZE_X * m_ZoomRatio), (DWORD)(SIZE_Y * m_ZoomRatio), 0);

	UpdateBackground();

	m_FontComboBox.SetFontHeight(16, m_ZoomRatio);
	m_FontComboBox.SetFontEx(m_FontFace, 16, m_ZoomRatio);
	m_FontComboBox.SetItemHeight(-1, (UINT)(24 * m_ZoomRatio));
	for (int i = 0; i < m_FontComboBox.GetCount(); i++)
	{
		m_FontComboBox.SetItemHeight(i, (UINT)(20 * m_ZoomRatio));
	}
	m_FontComboBox.MoveWindow((DWORD)(8 * m_ZoomRatio), (DWORD)(8 * m_ZoomRatio), (DWORD)(304 * m_ZoomRatio), (DWORD)(200 * m_ZoomRatio));

	m_FontTypeComboBox.SetItemHeight(-1, (UINT) (24 * m_ZoomRatio));
	for (int i = 0; i < m_FontTypeComboBox.GetCount(); i++)
	{
		m_FontTypeComboBox.SetItemHeight(i, (UINT) (20 * m_ZoomRatio));
	}
	m_FontTypeComboBox.SetFontEx(m_FontFace, 14, m_ZoomRatio);
	m_FontTypeComboBox.MoveWindow((DWORD) (8 * m_ZoomRatio), (DWORD) (44 * m_ZoomRatio), (DWORD) (304 * m_ZoomRatio), (DWORD) (200 * m_ZoomRatio));

	m_CtrlOk.SetFontEx(m_FontFace, 12, m_ZoomRatio);
	m_CtrlOk.InitControl(80, 84, 160, 28, m_ZoomRatio, NULL, 0, SS_CENTER, CButtonCx::SystemDraw | m_IsHighContrast);

	Invalidate();
}

CString CFontSelection::GetFontFace()
{
	return m_FontFace;
}

INT CFontSelection::GetFontType()
{
	return m_FontType;
}

int CALLBACK EnumFontFamExProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, int FontType, LPARAM lParam)
{
	CFontComboBox* pFontComboBox = (CFontComboBox*)lParam;
//	CComboBox* pFontComboBox = (CComboBox*)lParam;
	if(pFontComboBox->FindStringExact(0, (TCHAR*)lpelfe->elfLogFont.lfFaceName) == CB_ERR
	&& _tcschr((TCHAR*)lpelfe->elfLogFont.lfFaceName, _T('@')) == NULL
	)
	{
		pFontComboBox->AddString((TCHAR*)lpelfe->elfLogFont.lfFaceName);
	}
    return TRUE;
}

void CFontSelection::OnBnClickedOk()
{
	m_FontComboBox.GetLBText(m_FontComboBox.GetCurSel(), m_FontFace);
	m_FontType = m_FontTypeComboBox.GetCurSel();

	CDialog::OnOK();
}
