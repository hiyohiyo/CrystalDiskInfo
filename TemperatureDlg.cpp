/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DiskInfo.h"
#include "DiskInfoDlg.h"
#include "TemperatureDlg.h"

static CDiskInfoDlg* p;

IMPLEMENT_DYNAMIC(CTemperatureDlg, CDialog)

CTemperatureDlg::CTemperatureDlg(CWnd* pParent /*=NULL*/)
	: CDialogFx(CTemperatureDlg::IDD, pParent)
{
	p = (CDiskInfoDlg*)pParent;

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
}

CTemperatureDlg::~CTemperatureDlg()
{
}

void CTemperatureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogFx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_VALUE_TEMPERATURE, m_ValueTemperature);
	DDX_Text(pDX, IDC_VALUE_TEMPERATURE_F, m_ValueTemperatureF);
	DDX_Control(pDX, IDC_SCROLLBAR_TEMPERATURE, m_CtrlScrollbarTemperature);
	DDX_Control(pDX, IDC_VALUE_TEMPERATURE, m_CtrlValueTemperature);
	DDX_Control(pDX, IDC_VALUE_TEMPERATURE_F, m_CtrlValueTemperatureF);
	DDX_Control(pDX, IDC_SELECT_DISK, m_CtrlSelectDisk);
	DDX_Control(pDX, IDC_APPLY, m_CtrlApply);
	DDX_Control(pDX, IDC_DEFAULT, m_CtrlDefault);
}

BEGIN_MESSAGE_MAP(CTemperatureDlg, CDialogFx)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_APPLY, &CTemperatureDlg::OnApply)
	ON_BN_CLICKED(IDC_DEFAULT, &CTemperatureDlg::OnDefault)
	ON_CBN_SELCHANGE(IDC_SELECT_DISK, &CTemperatureDlg::OnCbnSelchangeSelectDisk)
END_MESSAGE_MAP()

BOOL CTemperatureDlg::OnInitDialog()
{
	CDialogFx::OnInitDialog();

	SetWindowTitle(i18n(_T("Alarm"), _T("ALARM_TEMPERATURE")));

	m_CtrlScrollbarTemperature.SetScrollRange(20, 80);

	m_bShowWindow = TRUE;

	InitLang();
	InitSelectDisk();

	UpdateDialogSize();

	CenterWindow();
	ShowWindow(SW_SHOW);

	if (p->m_Ata.vars.GetCount() == 0)
	{
		m_CtrlSelectDisk.EnableWindow(FALSE);
		m_CtrlScrollbarTemperature.EnableWindow(FALSE);
		m_CtrlValueTemperature.EnableWindow(FALSE);
		m_CtrlValueTemperatureF.EnableWindow(FALSE);
		m_CtrlApply.EnableWindow(FALSE);
		m_CtrlDefault.EnableWindow(FALSE);
	}

	return TRUE;
}

void CTemperatureDlg::UpdateDialogSize()
{
	CDialogFx::UpdateDialogSize();

	ChangeZoomType(m_ZoomType);
	SetClientSize(SIZE_X, SIZE_Y, m_ZoomRatio);
	UpdateBackground(FALSE, m_bDarkMode);

	m_CtrlScrollbarTemperature.MoveWindow((int)(8 * m_ZoomRatio), (int)(44 * m_ZoomRatio), (int)(280 * m_ZoomRatio), (int)(20 * m_ZoomRatio));
	m_CtrlValueTemperature.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);
   	m_CtrlValueTemperatureF.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);
	m_CtrlValueTemperature.InitControl(292, 44, 48, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, TRUE);
	m_CtrlValueTemperatureF.InitControl(344, 44, 48, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, TRUE);

	m_CtrlApply.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);
	m_CtrlDefault.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);

	m_CtrlApply.InitControl(220, 80, 160, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, SystemDraw, m_bHighContrast, m_bDarkMode, FALSE);
	m_CtrlDefault.InitControl(20, 80, 160, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, SystemDraw, m_bHighContrast, m_bDarkMode, FALSE);

	m_CtrlSelectDisk.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), RGB(0, 0, 0), FW_NORMAL, m_FontRender);
	m_CtrlSelectDisk.InitControl(8, 8, 384, 40, m_ZoomRatio, &m_BkDC, NULL, 0, ES_LEFT, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, RGB(255, 255, 255), RGB(160, 220, 255), RGB(255, 255, 255), 0);
	m_CtrlSelectDisk.SetMargin(0, 4, 0, 0, m_ZoomRatio);

	m_CtrlValueTemperature.SetDrawFrame(TRUE);
	m_CtrlValueTemperatureF.SetDrawFrame(TRUE);

	m_CtrlApply.SetHandCursor();
	m_CtrlDefault.SetHandCursor();

	SetDarkModeControl(m_CtrlApply.GetSafeHwnd(), m_bDarkMode);
	SetDarkModeControl(m_CtrlDefault.GetSafeHwnd(), m_bDarkMode);
	SetDarkModeControl(m_CtrlScrollbarTemperature.GetSafeHwnd(), m_bDarkMode);

	m_CtrlSelectDisk.SetItemHeightAll(24, m_ZoomRatio, m_FontRatio);

	Invalidate();
}

void CTemperatureDlg::InitLang()
{
	m_CtrlApply.SetWindowTextW(i18n(_T("HealthStatus"), _T("APPLY")));
	m_CtrlDefault.SetWindowTextW(i18n(_T("HealthStatus"), _T("DEFAULT")));
}

void CTemperatureDlg::InitSelectDisk()
{
	CString select;
	CString cstr;

	if (p->m_Ata.vars.GetCount() <= 0)
	{
		return;
	}

	for (int i = 0; i < p->m_Ata.vars.GetCount(); i++)
	{
		CString temp;
		if (p->m_Ata.vars[i].IsSsd)
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


void CTemperatureDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int position = pScrollBar->GetScrollPos();
	switch (nSBCode)
	{

	case SB_LINELEFT:
		position -= 1;
		break;
	case SB_LINERIGHT:
		position += 1;
		break;
	case SB_PAGELEFT:
		position -= 5;
		break;
	case SB_PAGERIGHT:
		position += 5;
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
	if (*pScrollBar == m_CtrlScrollbarTemperature)
	{
		m_ValueTemperature.Format(L"%d °C", m_CtrlScrollbarTemperature.GetScrollPos());
		m_ValueTemperatureF.Format(L"%d °F", m_CtrlScrollbarTemperature.GetScrollPos() * 9 / 5 + 32);
	}

	UpdateData(FALSE);

	CDialogFx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CTemperatureDlg::UpdateSelectDisk(DWORD index)
{
	if (p->m_Ata.vars[index].IsSsd)
	{
		m_CtrlScrollbarTemperature.SetScrollPos(GetPrivateProfileIntFx(_T("AlarmTemperature"), p->m_Ata.vars[index].ModelSerial, 60, m_Ini));
	}
	else
	{
		m_CtrlScrollbarTemperature.SetScrollPos(GetPrivateProfileIntFx(_T("AlarmTemperature"), p->m_Ata.vars[index].ModelSerial, 50, m_Ini));
	}
	m_CtrlScrollbarTemperature.EnableWindow(TRUE);

	m_ValueTemperature.Format(L"%d °C", m_CtrlScrollbarTemperature.GetScrollPos());
	m_ValueTemperatureF.Format(L"%d °F", m_CtrlScrollbarTemperature.GetScrollPos() * 9 / 5 + 32);
	UpdateData(FALSE);
}

void CTemperatureDlg::OnApply()
{
	UpdateData(TRUE);

	WritePrivateProfileStringFx(_T("AlarmTemperature"), p->m_Ata.vars[m_DiskIndex].ModelSerial, m_ValueTemperature, m_Ini);
	p->m_Ata.vars[m_DiskIndex].AlarmTemperature = _tstoi(m_ValueTemperature);
	p->SendMessage(WM_COMMAND, ID_REFRESH);
}

void CTemperatureDlg::OnDefault()
{
	if (p->m_Ata.vars[m_DiskIndex].IsSsd)
	{
		m_CtrlScrollbarTemperature.SetScrollPos(60);
	}
	else
	{
		m_CtrlScrollbarTemperature.SetScrollPos(50);
	}
	
	m_ValueTemperature.Format(L"%d °C", m_CtrlScrollbarTemperature.GetScrollPos());
	m_ValueTemperatureF.Format(L"%d °F", m_CtrlScrollbarTemperature.GetScrollPos() * 9 / 5 + 32);

	UpdateData(FALSE);
}

void CTemperatureDlg::OnCbnSelchangeSelectDisk()
{
	if (m_DiskIndex != m_CtrlSelectDisk.GetCurSel())
	{
		m_DiskIndex = m_CtrlSelectDisk.GetCurSel();
		UpdateSelectDisk(m_DiskIndex);
	}
}
