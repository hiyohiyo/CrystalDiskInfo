/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DiskInfo.h"
#include "DiskInfoDlg.h"
#include "HealthDlg.h"

static CDiskInfoDlg* p;

IMPLEMENT_DYNAMIC(CHealthDlg, CDialog)

CHealthDlg::CHealthDlg(CWnd* pParent /*=NULL*/)
	: CDialogFx(CHealthDlg::IDD, pParent)
{
	p = (CDiskInfoDlg*)pParent;

	m_ZoomType = p->GetZoomType();
	m_FontFace = p->GetFontFace();
	m_FontScale = p->GetFontScale();
	m_FontRatio = p->GetFontRatio();
	m_FontRender = p->GetFontRender();
	m_CurrentLangPath = p->GetCurrentLangPath();
	m_DefaultLangPath = p->GetDefaultLangPath();
	m_ThemeDir = p->GetThemeDir();
	m_CurrentTheme = p->GetCurrentTheme();
	m_DefaultTheme = p->GetDefaultTheme();
	m_Ini = p->GetIniPath();
}

CHealthDlg::~CHealthDlg()
{
}

void CHealthDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogFx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_VALUE_05, m_Value05);
	DDX_Text(pDX, IDC_VALUE_C5, m_ValueC5);
	DDX_Text(pDX, IDC_VALUE_C6, m_ValueC6);
	DDX_Text(pDX, IDC_VALUE_FF, m_ValueFF);
	DDX_Text(pDX, IDC_VALUE_05X, m_Value05X);
	DDX_Text(pDX, IDC_VALUE_C5X, m_ValueC5X);
	DDX_Text(pDX, IDC_VALUE_C6X, m_ValueC6X);
	DDX_Text(pDX, IDC_VALUE_FFX, m_ValueFFX);
	DDX_Text(pDX, IDC_LABEL_05, m_Label05);
	DDX_Text(pDX, IDC_LABEL_C5, m_LabelC5);
	DDX_Text(pDX, IDC_LABEL_C6, m_LabelC6);
	DDX_Text(pDX, IDC_LABEL_FF, m_LabelFF);

	DDX_Control(pDX, IDC_SCROLLBAR_05, m_CtrlScrollbar05);
	DDX_Control(pDX, IDC_SCROLLBAR_C5, m_CtrlScrollbarC5);
	DDX_Control(pDX, IDC_SCROLLBAR_C6, m_CtrlScrollbarC6);
	DDX_Control(pDX, IDC_SCROLLBAR_FF, m_CtrlScrollbarFF);
	DDX_Control(pDX, IDC_VALUE_05, m_CtrlValue05);
	DDX_Control(pDX, IDC_VALUE_C5, m_CtrlValueC5);
	DDX_Control(pDX, IDC_VALUE_C6, m_CtrlValueC6);
	DDX_Control(pDX, IDC_VALUE_FF, m_CtrlValueFF);
	DDX_Control(pDX, IDC_VALUE_05X, m_CtrlValue05X);
	DDX_Control(pDX, IDC_VALUE_C5X, m_CtrlValueC5X);
	DDX_Control(pDX, IDC_VALUE_C6X, m_CtrlValueC6X);
	DDX_Control(pDX, IDC_VALUE_FFX, m_CtrlValueFFX);
	DDX_Control(pDX, IDC_LABEL_05, m_CtrlLabel05);
	DDX_Control(pDX, IDC_LABEL_C5, m_CtrlLabelC5);
	DDX_Control(pDX, IDC_LABEL_C6, m_CtrlLabelC6);
	DDX_Control(pDX, IDC_LABEL_FF, m_CtrlLabelFF);
	DDX_Control(pDX, IDC_SELECT_DISK, m_CtrlSelectDisk);
	DDX_Control(pDX, IDC_APPLY, m_CtrlApply);
	DDX_Control(pDX, IDC_DEFAULT, m_CtrlDefault);
}


BEGIN_MESSAGE_MAP(CHealthDlg, CDialogFx)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_APPLY, &CHealthDlg::OnApply)
	ON_BN_CLICKED(IDC_DEFAULT, &CHealthDlg::OnDefault)
	ON_CBN_SELCHANGE(IDC_SELECT_DISK, &CHealthDlg::OnCbnSelchangeSelectDisk)
END_MESSAGE_MAP()

BOOL CHealthDlg::OnInitDialog()
{
	CDialogFx::OnInitDialog();

	SetWindowTitle(i18n(_T("WindowTitle"), _T("HEALTH_STATUS_SETTING"))
		+ _T(" - ") + i18n(_T("HealthStatus"), _T("THRESHOLD_OF_CAUTION"))
		+ _T(" (") + i18n(_T("Dialog"), _T("LIST_RAW_VALUES")) + _T(")"));

	m_CtrlScrollbar05.SetScrollRange(0x00, 0xFF);
	m_CtrlScrollbarC5.SetScrollRange(0x00, 0xFF);
	m_CtrlScrollbarC6.SetScrollRange(0x00, 0xFF);
	m_CtrlScrollbarFF.SetScrollRange(0x00, 0x63);

	m_bShowWindow = TRUE;

	InitLang();
	InitSelectDisk();

	UpdateDialogSize();

	CenterWindow();
	ShowWindow(SW_SHOW);

	if (p->m_Ata.vars.GetCount() == 0)
	{
		m_CtrlSelectDisk.EnableWindow(FALSE);

		m_CtrlScrollbar05.EnableWindow(FALSE);
		m_CtrlScrollbarC5.EnableWindow(FALSE);
		m_CtrlScrollbarC6.EnableWindow(FALSE);
		m_CtrlScrollbarFF.EnableWindow(FALSE);

		m_CtrlLabel05.EnableWindow(FALSE);
		m_CtrlLabelC5.EnableWindow(FALSE);
		m_CtrlLabelC6.EnableWindow(FALSE);
		m_CtrlLabelFF.EnableWindow(FALSE);

		m_CtrlValue05.EnableWindow(FALSE);
		m_CtrlValueC5.EnableWindow(FALSE);
		m_CtrlValueC6.EnableWindow(FALSE);
		m_CtrlValueFF.EnableWindow(FALSE);

		m_CtrlValue05X.EnableWindow(FALSE);
		m_CtrlValueC5X.EnableWindow(FALSE);
		m_CtrlValueC6X.EnableWindow(FALSE);
		m_CtrlValueFFX.EnableWindow(FALSE);

		m_CtrlApply.EnableWindow(FALSE);
		m_CtrlDefault.EnableWindow(FALSE);
	}

	return TRUE;
}

void CHealthDlg::UpdateDialogSize()
{
	CDialogFx::UpdateDialogSize();

	ChangeZoomType(m_ZoomType);
	SetClientSize(SIZE_X, SIZE_Y, m_ZoomRatio);
	UpdateBackground(FALSE, m_bDarkMode);

	m_CtrlScrollbar05.MoveWindow((int)(16 * m_ZoomRatio), (int)( 72 * m_ZoomRatio), (int)(280 * m_ZoomRatio), (int)(20 * m_ZoomRatio));
	m_CtrlScrollbarC5.MoveWindow((int)(16 * m_ZoomRatio), (int)(128 * m_ZoomRatio), (int)(280 * m_ZoomRatio), (int)(20 * m_ZoomRatio));
	m_CtrlScrollbarC6.MoveWindow((int)(16 * m_ZoomRatio), (int)(184 * m_ZoomRatio), (int)(280 * m_ZoomRatio), (int)(20 * m_ZoomRatio));
	m_CtrlScrollbarFF.MoveWindow((int)(16 * m_ZoomRatio), (int)(240 * m_ZoomRatio), (int)(280 * m_ZoomRatio), (int)(20 * m_ZoomRatio));

	m_CtrlLabel05.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);
	m_CtrlLabelC5.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);
	m_CtrlLabelC6.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);
	m_CtrlLabelFF.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);

	m_CtrlValue05.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);
	m_CtrlValueC5.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);
	m_CtrlValueC6.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);
	m_CtrlValueFF.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);

	m_CtrlValue05X.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);
	m_CtrlValueC5X.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);
	m_CtrlValueC6X.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);
	m_CtrlValueFFX.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);

	m_CtrlLabel05.InitControl(8,  44, 384, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, CStaticFx::Border::UNDERLINE);
	m_CtrlLabelC5.InitControl(8, 100, 384, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, CStaticFx::Border::UNDERLINE);
	m_CtrlLabelC6.InitControl(8, 156, 384, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, CStaticFx::Border::UNDERLINE);
	m_CtrlLabelFF.InitControl(8, 212, 384, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, CStaticFx::Border::UNDERLINE);

	m_CtrlValue05.InitControl(300,  72, 40, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, TRUE);
	m_CtrlValueC5.InitControl(300, 128, 40, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, TRUE);
	m_CtrlValueC6.InitControl(300, 184, 40, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, TRUE);
	m_CtrlValueFF.InitControl(300, 240, 40, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, TRUE);

	m_CtrlValue05X.InitControl(344,  72, 40, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, TRUE);
	m_CtrlValueC5X.InitControl(344, 128, 40, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, TRUE);
	m_CtrlValueC6X.InitControl(344, 184, 40, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, TRUE);
	m_CtrlValueFFX.InitControl(344, 240, 40, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, TRUE);

	m_CtrlApply.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);
	m_CtrlDefault.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);

	m_CtrlApply.InitControl(220, 272, 160, 24, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, SystemDraw, m_bHighContrast, m_bDarkMode, TRUE);
	m_CtrlDefault.InitControl(20, 272, 160, 24, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, SystemDraw, m_bHighContrast, m_bDarkMode, FALSE);

	m_CtrlSelectDisk.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), RGB(0, 0, 0), FW_NORMAL, m_FontRender);
	m_CtrlSelectDisk.InitControl(8, 8, 384, 40, m_ZoomRatio, &m_BkDC, NULL, 0, ES_LEFT, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, RGB(255, 255, 255), RGB(160, 220, 255), RGB(255, 255, 255), 0);
	m_CtrlSelectDisk.SetMargin(0, 4, 0, 0, m_ZoomRatio);

	m_CtrlValue05.SetDrawFrame(TRUE);
	m_CtrlValueC5.SetDrawFrame(TRUE);
	m_CtrlValueC6.SetDrawFrame(TRUE);
	m_CtrlValueFF.SetDrawFrame(TRUE);
	m_CtrlValue05X.SetDrawFrame(TRUE);
	m_CtrlValueC5X.SetDrawFrame(TRUE);
	m_CtrlValueC6X.SetDrawFrame(TRUE);
	m_CtrlValueFFX.SetDrawFrame(TRUE);

	m_CtrlApply.SetHandCursor();
	m_CtrlDefault.SetHandCursor();

	SetDarkModeControl(m_CtrlApply.GetSafeHwnd(), m_bDarkMode);
	SetDarkModeControl(m_CtrlDefault.GetSafeHwnd(), m_bDarkMode);

	SetDarkModeControl(m_CtrlScrollbar05.GetSafeHwnd(), m_bDarkMode);
	SetDarkModeControl(m_CtrlScrollbarC5.GetSafeHwnd(), m_bDarkMode);
	SetDarkModeControl(m_CtrlScrollbarC6.GetSafeHwnd(), m_bDarkMode);
	SetDarkModeControl(m_CtrlScrollbarFF.GetSafeHwnd(), m_bDarkMode);

	// SetLayeredWindow(m_CtrlSelectDisk.GetListHwnd(), m_ComboAlpha);
	for (int i = -1; i < m_CtrlSelectDisk.GetCount(); i++)
	{
		m_CtrlSelectDisk.SetItemHeightEx(i, 24, m_ZoomRatio, m_FontRatio);
	}

	Invalidate();
}

void CHealthDlg::InitLang()
{
	m_CtrlLabel05.SetWindowTextW(_T("[05] ") + i18n(_T("Smart"), _T("05")));
	m_CtrlLabelC5.SetWindowTextW(_T("[C5] ") + i18n(_T("Smart"), _T("C5")));
	m_CtrlLabelC6.SetWindowTextW(_T("[C6] ") + i18n(_T("Smart"), _T("C6")));
	m_CtrlLabelFF.SetWindowTextW(_T("[FF] ") + i18n(_T("SmartSsd"), _T("FF")));

	m_CtrlApply.SetWindowTextW(i18n(_T("HealthStatus"), _T("APPLY")));
	m_CtrlDefault.SetWindowTextW(i18n(_T("HealthStatus"), _T("DEFAULT")));

	UpdateData(TRUE);
}

void CHealthDlg::InitSelectDisk()
{
	CString select;
	CString cstr;

	if(p->m_Ata.vars.GetCount() <= 0)
	{
		return ;
	}

	for(int i = 0; i < p->m_Ata.vars.GetCount(); i++)
	{
		CString temp;
		if(p->m_Ata.vars[i].IsSsd)
		{
			temp = _T("[SSD]");	
		}

		CString driveLetter;
		if (p->m_Ata.vars[i].DriveMap.IsEmpty())
		{
			if (p->m_Ata.vars[i].PhysicalDriveId >= 0)
			{
				driveLetter.Format(_T("(Disk %d)"), p->m_Ata.vars[i].PhysicalDriveId);
			}
		}
		else
		{
			driveLetter.Format(_T("(%s)"), p->m_Ata.vars[i].DriveMap.GetString());
		}
		cstr.Format(_T("(%02d) %s %s %s"), i + 1, p->m_Ata.vars.GetAt(i).Model.GetString(), temp.GetString(), driveLetter.GetString());

		m_CtrlSelectDisk.AddString(cstr);

		if (i == p->GetSelectedDrive())
		{
			m_CtrlSelectDisk.SetCurSel(i);
			m_DiskIndex = i;
		}
	}

	UpdateSelectDisk(m_DiskIndex);
}


void CHealthDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int position = pScrollBar->GetScrollPos();
	switch(nSBCode)
	{

	case SB_LINELEFT:
		position -= 1;
		break;
	case SB_LINERIGHT:
		position += 1;
		break;
	case SB_PAGELEFT:
		position -= 8;
		break;
	case SB_PAGERIGHT:
		position += 8;
		break;
	case SB_LEFT:
		break;
	case SB_RIGHT:
		break;
	case SB_THUMBTRACK:
		position = nPos;
		break;
	}
	pScrollBar->SetScrollPos(position);

	CString cstr;
	if(*pScrollBar == m_CtrlScrollbar05)
	{
		m_Value05X.Format(L"%02Xh", m_CtrlScrollbar05.GetScrollPos());
		m_Value05.Format(L"%d", m_CtrlScrollbar05.GetScrollPos());
	}
	else if(*pScrollBar == m_CtrlScrollbarC5)
	{
		m_ValueC5X.Format(L"%02Xh", m_CtrlScrollbarC5.GetScrollPos());
		m_ValueC5.Format(L"%d", m_CtrlScrollbarC5.GetScrollPos());
	}
	else if(*pScrollBar == m_CtrlScrollbarC6)
	{
		m_ValueC6X.Format(L"%02Xh", m_CtrlScrollbarC6.GetScrollPos());
		m_ValueC6.Format(L"%d", m_CtrlScrollbarC6.GetScrollPos());
	}
	else if(*pScrollBar == m_CtrlScrollbarFF)
	{
		m_ValueFFX.Format(L"%02Xh", m_CtrlScrollbarFF.GetScrollPos());
		m_ValueFF.Format(L"%d", m_CtrlScrollbarFF.GetScrollPos());
	}

	UpdateData(FALSE);

	CDialogFx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CHealthDlg::UpdateSelectDisk(DWORD index)
{
	if(p->m_Ata.vars[index].IsSsd)
	{
		m_CtrlScrollbar05.SetScrollPos(0);
		m_CtrlScrollbarC5.SetScrollPos(0);
		m_CtrlScrollbarC6.SetScrollPos(0);
		if(p->m_Ata.vars[index].Life < 0)
		{
			m_CtrlScrollbarFF.SetScrollPos(0);
			m_CtrlScrollbarFF.EnableWindow(FALSE);
			m_ValueFF = L"";
			m_ValueFFX = L"";
		}
		else
		{
			m_CtrlScrollbarFF.SetScrollPos(GetPrivateProfileIntFx(_T("ThreasholdOfCautionFF"), p->m_Ata.vars[index].ModelSerial, 10, m_Ini));
			m_CtrlScrollbarFF.EnableWindow(TRUE);
			m_ValueFF.Format(_T("%d"), m_CtrlScrollbarFF.GetScrollPos());
			m_ValueFFX.Format(_T("%02Xh"), m_CtrlScrollbarFF.GetScrollPos());
		}
		m_CtrlScrollbar05.EnableWindow(FALSE);
		m_CtrlScrollbarC5.EnableWindow(FALSE);
		m_CtrlScrollbarC6.EnableWindow(FALSE);
		m_Value05 = L"";
		m_Value05X = L"";
		m_ValueC5 = L"";
		m_ValueC5X = L"";
		m_ValueC6 = L"";
		m_ValueC6X = L"";
	}
	else
	{
		m_CtrlScrollbar05.SetScrollPos(GetPrivateProfileIntFx(_T("ThreasholdOfCaution05"), p->m_Ata.vars[index].ModelSerial, 1, m_Ini));
		m_CtrlScrollbarC5.SetScrollPos(GetPrivateProfileIntFx(_T("ThreasholdOfCautionC5"), p->m_Ata.vars[index].ModelSerial, 1, m_Ini));
		m_CtrlScrollbarC6.SetScrollPos(GetPrivateProfileIntFx(_T("ThreasholdOfCautionC6"), p->m_Ata.vars[index].ModelSerial, 1, m_Ini));
		m_CtrlScrollbarFF.SetScrollPos(0);

		m_CtrlScrollbar05.EnableWindow(TRUE);
		m_CtrlScrollbarC5.EnableWindow(TRUE);
		m_CtrlScrollbarC6.EnableWindow(TRUE);
		m_CtrlScrollbarFF.EnableWindow(FALSE);
		m_Value05.Format(_T("%d"), m_CtrlScrollbar05.GetScrollPos());
		m_ValueC5.Format(_T("%d"), m_CtrlScrollbarC5.GetScrollPos());
		m_ValueC6.Format(_T("%d"), m_CtrlScrollbarC6.GetScrollPos());
		m_Value05X.Format(_T("%02Xh"), m_CtrlScrollbar05.GetScrollPos());
		m_ValueC5X.Format(_T("%02Xh"), m_CtrlScrollbarC5.GetScrollPos());
		m_ValueC6X.Format(_T("%02Xh"), m_CtrlScrollbarC6.GetScrollPos());
		m_ValueFF = L"";
		m_ValueFFX = L"";
	}
	UpdateData(FALSE);
}

void CHealthDlg::OnApply()
{
	UpdateData(TRUE);

	if(! p->m_Ata.vars[m_DiskIndex].IsSsd)
	{
		WritePrivateProfileStringFx(_T("ThreasholdOfCaution05"), p->m_Ata.vars[m_DiskIndex].ModelSerial, m_Value05, m_Ini);
		WritePrivateProfileStringFx(_T("ThreasholdOfCautionC5"), p->m_Ata.vars[m_DiskIndex].ModelSerial, m_ValueC5, m_Ini);
		WritePrivateProfileStringFx(_T("ThreasholdOfCautionC6"), p->m_Ata.vars[m_DiskIndex].ModelSerial, m_ValueC6, m_Ini);

		p->m_Ata.vars[m_DiskIndex].Threshold05 = (WORD)_tstoi(m_Value05);
		p->m_Ata.vars[m_DiskIndex].ThresholdC5 = (WORD)_tstoi(m_ValueC5);
		p->m_Ata.vars[m_DiskIndex].ThresholdC6 = (WORD)_tstoi(m_ValueC6);
	}
	else if(p->m_Ata.vars[m_DiskIndex].Life >= 0)
	{
		WritePrivateProfileStringFx(_T("ThreasholdOfCautionFF"), p->m_Ata.vars[m_DiskIndex].ModelSerial, m_ValueFF, m_Ini);
		p->m_Ata.vars[m_DiskIndex].ThresholdFF = (WORD)_tstoi(m_ValueFF);
	}
	p->SendMessage(WM_COMMAND, ID_REFRESH);
}

void CHealthDlg::OnDefault()
{
	if(! p->m_Ata.vars[m_DiskIndex].IsSsd)
	{
		m_CtrlScrollbar05.SetScrollPos(1);
		m_CtrlScrollbarC5.SetScrollPos(1);
		m_CtrlScrollbarC6.SetScrollPos(1);
		m_Value05.Format(_T("%d"), m_CtrlScrollbar05.GetScrollPos());
		m_ValueC5.Format(_T("%d"), m_CtrlScrollbarC5.GetScrollPos());
		m_ValueC6.Format(_T("%d"), m_CtrlScrollbarC6.GetScrollPos());
		m_Value05X.Format(_T("%02Xh"), m_CtrlScrollbar05.GetScrollPos());
		m_ValueC5X.Format(_T("%02Xh"), m_CtrlScrollbarC5.GetScrollPos());
		m_ValueC6X.Format(_T("%02Xh"), m_CtrlScrollbarC6.GetScrollPos());

	}
	else if(p->m_Ata.vars[m_DiskIndex].Life >= 0)
	{
		m_CtrlScrollbarFF.SetScrollPos(10);
		m_ValueFF.Format(_T("%d"), m_CtrlScrollbarFF.GetScrollPos());
		m_ValueFFX.Format(_T("%02Xh"), m_CtrlScrollbarFF.GetScrollPos());
	}
	UpdateData(FALSE);
}

void CHealthDlg::OnCbnSelchangeSelectDisk()
{
	if(m_DiskIndex != m_CtrlSelectDisk.GetCurSel())
	{
		m_DiskIndex = m_CtrlSelectDisk.GetCurSel();
		UpdateSelectDisk(m_DiskIndex);
	}
}
