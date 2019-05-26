/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DiskInfo.h"
#include "DiskInfoDlg.h"
#include "TemperatureDlg.h"

static CDiskInfoDlg* p;

IMPLEMENT_DYNAMIC(CTemperatureDlg, CDialog)

CTemperatureDlg::CTemperatureDlg(CWnd* pParent /*=NULL*/)
	: CDialogCx(CTemperatureDlg::IDD, pParent)
{
	p = (CDiskInfoDlg*)pParent;
	_tcscpy_s(m_Ini, MAX_PATH, ((CDiskInfoApp*)AfxGetApp())->m_Ini);

	m_CurrentLangPath = ((CMainDialog*)pParent)->m_CurrentLangPath;
	m_DefaultLangPath = ((CMainDialog*)pParent)->m_DefaultLangPath;
	m_ZoomType = ((CMainDialog*)pParent)->GetZoomType();
	m_FontFace = ((CMainDialog*)pParent)->m_FontFace;
	m_CxThemeDir = ((CDiskInfoApp*)AfxGetApp())->m_ThemeDir;
	m_CxCurrentTheme = ((CMainDialog*)pParent)->m_CurrentTheme;
	m_CxDefaultTheme = ((CMainDialog*)pParent)->m_DefaultTheme;
}

CTemperatureDlg::~CTemperatureDlg()
{
}

void CTemperatureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogCx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_VALUE_TEMPERATURE, m_ValueTemperature);
	DDX_Text(pDX, IDC_VALUE_TEMPERATURE_F, m_ValueTemperatureF);
//	DDX_Text(pDX, IDC_LABEL_TEMPERATURE, m_LabelTemperature);

	DDX_Control(pDX, IDC_SCROLLBAR_TEMPERATURE, m_CtrlScrollbarTemperature);
	DDX_Control(pDX, IDC_VALUE_TEMPERATURE, m_CtrlValueTemperature);
	DDX_Control(pDX, IDC_VALUE_TEMPERATURE_F, m_CtrlValueTemperatureF);
//	DDX_Control(pDX, IDC_LABEL_TEMPERATURE, m_CtrlLabelTemperature);
	DDX_Control(pDX, IDC_SELECT_DISK, m_CtrlSelectDisk);
	DDX_Control(pDX, IDC_APPLY, m_CtrlApply);
	DDX_Control(pDX, IDC_DEFAULT, m_CtrlDefault);
}


BEGIN_MESSAGE_MAP(CTemperatureDlg, CDialogCx)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_APPLY, &CTemperatureDlg::OnBnClickedApply)
	ON_BN_CLICKED(IDC_DEFAULT, &CTemperatureDlg::OnBnClickedDefault)
	ON_CBN_SELCHANGE(IDC_SELECT_DISK, &CTemperatureDlg::OnCbnSelchangeSelectDisk)
END_MESSAGE_MAP()

BOOL CTemperatureDlg::OnInitDialog()
{
	CDialogCx::OnInitDialog();

	SetWindowText(i18n(_T("Alarm"), _T("ALARM_TEMPERATURE")));

	m_CtrlScrollbarTemperature.SetScrollRange(20, 80);

	m_FlagShowWindow = TRUE;

	InitLang();
	InitSelectDisk();

	UpdateDialogSize();

	CenterWindow();
	ShowWindow(SW_SHOW);

	if (p->m_Ata.vars.GetCount() == 0)
	{
		m_CtrlSelectDisk.EnableWindow(FALSE);
		m_CtrlScrollbarTemperature.EnableWindow(FALSE);
//		m_CtrlLabelTemperature.EnableWindow(FALSE);
		m_CtrlValueTemperature.EnableWindow(FALSE);
		m_CtrlValueTemperatureF.EnableWindow(FALSE);
		m_CtrlApply.EnableWindow(FALSE);
		m_CtrlDefault.EnableWindow(FALSE);
	}

	return TRUE;
}

void CTemperatureDlg::UpdateDialogSize()
{
	ChangeZoomType(m_ZoomType);
	SetClientRect((DWORD)(SIZE_X * m_ZoomRatio), (DWORD)(SIZE_Y * m_ZoomRatio), 0);

	UpdateBackground();

	m_CtrlScrollbarTemperature.MoveWindow((DWORD)(8 * m_ZoomRatio), (DWORD)(44 * m_ZoomRatio), (DWORD)(280 * m_ZoomRatio), (DWORD)(20 * m_ZoomRatio));
//	m_CtrlLabelTemperature.SetFontEx(m_FontFace, 12, m_ZoomRatio);
	m_CtrlValueTemperature.SetFontEx(m_FontFace, 12, m_ZoomRatio);
   	m_CtrlValueTemperatureF.SetFontEx(m_FontFace, 12, m_ZoomRatio);
//	m_CtrlLabelTemperature.InitControl(8, 44, 384, 24, m_ZoomRatio, NULL, 0, SS_LEFT, CStaticCx::OwnerDrawGlass | m_IsHighContrast);
	m_CtrlValueTemperature.InitControl(292, 44, 48, 20, m_ZoomRatio, NULL, 0, SS_CENTER, CStaticCx::OwnerDrawGlass | m_IsHighContrast);
	m_CtrlValueTemperatureF.InitControl(344, 44, 48, 20, m_ZoomRatio, NULL, 0, SS_CENTER, CStaticCx::OwnerDrawGlass | m_IsHighContrast);

	m_CtrlApply.SetFontEx(m_FontFace, 12, m_ZoomRatio);
	m_CtrlDefault.SetFontEx(m_FontFace, 12, m_ZoomRatio);

	m_CtrlApply.InitControl(220, 76, 160, 28, m_ZoomRatio, NULL, 0, SS_CENTER, CButtonCx::SystemDraw | m_IsHighContrast);
	m_CtrlDefault.InitControl(20, 76, 160, 28, m_ZoomRatio, NULL, 0, SS_CENTER, CButtonCx::SystemDraw | m_IsHighContrast);

	m_CtrlSelectDisk.SetFontEx(m_FontFace, 14, m_ZoomRatio);
	m_CtrlSelectDisk.MoveWindow((DWORD)(8 * m_ZoomRatio), (DWORD)(8 * m_ZoomRatio), (DWORD)(384 * m_ZoomRatio), (DWORD)(40 * m_ZoomRatio));

	m_IsDrawFrame = IsDrawFrame();

	m_CtrlValueTemperature.SetDrawFrame(m_IsDrawFrame);
	m_CtrlValueTemperatureF.SetDrawFrame(m_IsDrawFrame);

	Invalidate();
}

void CTemperatureDlg::InitLang()
{
//	m_CtrlLabelTemperature.SetWindowTextW(i18n(_T("Alarm"), _T("ALARM_TEMPERATURE")));

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
			driveLetter.Format(_T("(%s)"), p->m_Ata.vars[i].DriveMap);
		}
		cstr.Format(_T("(%d) %s %s %s"), i + 1, p->m_Ata.vars.GetAt(i).Model, temp, driveLetter);

		m_CtrlSelectDisk.AddString(cstr);
	}
	m_CtrlSelectDisk.SetCurSel(0);

	UpdateData(TRUE);
	UpdateSelectDisk(0);

	UpdateData(TRUE);
	m_DiskIndex = 0;
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

	CDialogCx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CTemperatureDlg::UpdateSelectDisk(DWORD index)
{
	if (p->m_Ata.vars[index].IsSsd)
	{
		m_CtrlScrollbarTemperature.SetScrollPos(GetPrivateProfileInt(_T("AlarmTemperature"), p->m_Ata.vars[index].ModelSerial, 60, m_Ini));
	}
	else
	{
		m_CtrlScrollbarTemperature.SetScrollPos(GetPrivateProfileInt(_T("AlarmTemperature"), p->m_Ata.vars[index].ModelSerial, 50, m_Ini));
	}
	m_CtrlScrollbarTemperature.EnableWindow(TRUE);

	m_ValueTemperature.Format(L"%d °C", m_CtrlScrollbarTemperature.GetScrollPos());
	m_ValueTemperatureF.Format(L"%d °F", m_CtrlScrollbarTemperature.GetScrollPos() * 9 / 5 + 32);
	UpdateData(FALSE);
}

void CTemperatureDlg::OnBnClickedApply()
{
	UpdateData(TRUE);

	WritePrivateProfileString(_T("AlarmTemperature"), p->m_Ata.vars[m_DiskIndex].ModelSerial, m_ValueTemperature, m_Ini);
	p->m_Ata.vars[m_DiskIndex].AlarmTemperature = _tstoi(m_ValueTemperature);
	p->SendMessage(WM_COMMAND, ID_REFRESH);
}

void CTemperatureDlg::OnBnClickedDefault()
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
