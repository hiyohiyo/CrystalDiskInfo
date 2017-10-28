/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DiskInfo.h"
#include "DiskInfoDlg.h"
#include "HealthDlg.h"

static CDiskInfoDlg *p;

IMPLEMENT_DYNAMIC(CHealthDlg, CDialog)

CHealthDlg::CHealthDlg(CWnd* pParent /*=NULL*/)
	: CDialogCx(CHealthDlg::IDD, pParent)
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

CHealthDlg::~CHealthDlg()
{
}

void CHealthDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogCx::DoDataExchange(pDX);

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


BEGIN_MESSAGE_MAP(CHealthDlg, CDialogCx)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_APPLY, &CHealthDlg::OnBnClickedApply)
	ON_BN_CLICKED(IDC_DEFAULT, &CHealthDlg::OnBnClickedDefault)
	ON_CBN_SELCHANGE(IDC_SELECT_DISK, &CHealthDlg::OnCbnSelchangeSelectDisk)
END_MESSAGE_MAP()

BOOL CHealthDlg::OnInitDialog()
{
	CDialogCx::OnInitDialog();

	SetWindowText(i18n(_T("WindowTitle"), _T("HEALTH_STATUS_SETTING"))
		+ _T(" - ") + i18n(_T("HealthStatus"), _T("THRESHOLD_OF_CAUTION"))
		+ _T(" (") + i18n(_T("Dialog"), _T("LIST_RAW_VALUES")) + _T(")"));

	m_CtrlScrollbar05.SetScrollRange(0x00, 0xFF);
	m_CtrlScrollbarC5.SetScrollRange(0x00, 0xFF);
	m_CtrlScrollbarC6.SetScrollRange(0x00, 0xFF);
	m_CtrlScrollbarFF.SetScrollRange(0x00, 0x63);

	m_FlagShowWindow = TRUE;

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
	ChangeZoomType(m_ZoomType);
	SetClientRect((DWORD)(SIZE_X * m_ZoomRatio), (DWORD)(SIZE_Y * m_ZoomRatio), 0);

	UpdateBackground();

	m_CtrlScrollbar05.MoveWindow((DWORD)(16 * m_ZoomRatio), (DWORD)( 72 * m_ZoomRatio), (DWORD)(280 * m_ZoomRatio), (DWORD)(20 * m_ZoomRatio));
	m_CtrlScrollbarC5.MoveWindow((DWORD)(16 * m_ZoomRatio), (DWORD)(128 * m_ZoomRatio), (DWORD)(280 * m_ZoomRatio), (DWORD)(20 * m_ZoomRatio));
	m_CtrlScrollbarC6.MoveWindow((DWORD)(16 * m_ZoomRatio), (DWORD)(184 * m_ZoomRatio), (DWORD)(280 * m_ZoomRatio), (DWORD)(20 * m_ZoomRatio));
	m_CtrlScrollbarFF.MoveWindow((DWORD)(16 * m_ZoomRatio), (DWORD)(240 * m_ZoomRatio), (DWORD)(280 * m_ZoomRatio), (DWORD)(20 * m_ZoomRatio));

	m_CtrlLabel05.SetFontEx(m_FontFace, 12, m_ZoomRatio);
	m_CtrlLabelC5.SetFontEx(m_FontFace, 12, m_ZoomRatio);
	m_CtrlLabelC6.SetFontEx(m_FontFace, 12, m_ZoomRatio);
	m_CtrlLabelFF.SetFontEx(m_FontFace, 12, m_ZoomRatio);

	m_CtrlValue05.SetFontEx(m_FontFace, 12, m_ZoomRatio);
	m_CtrlValueC5.SetFontEx(m_FontFace, 12, m_ZoomRatio);
	m_CtrlValueC6.SetFontEx(m_FontFace, 12, m_ZoomRatio);
	m_CtrlValueFF.SetFontEx(m_FontFace, 12, m_ZoomRatio);

	m_CtrlValue05X.SetFontEx(m_FontFace, 12, m_ZoomRatio);
	m_CtrlValueC5X.SetFontEx(m_FontFace, 12, m_ZoomRatio);
	m_CtrlValueC6X.SetFontEx(m_FontFace, 12, m_ZoomRatio);
	m_CtrlValueFFX.SetFontEx(m_FontFace, 12, m_ZoomRatio);

	m_CtrlLabel05.InitControl(8,  44, 384, 24, m_ZoomRatio, NULL, 0, SS_LEFT, CStaticCx::OwnerDrawGlass | m_IsHighContrast);
	m_CtrlLabelC5.InitControl(8, 100, 384, 24, m_ZoomRatio, NULL, 0, SS_LEFT, CStaticCx::OwnerDrawGlass | m_IsHighContrast);
	m_CtrlLabelC6.InitControl(8, 156, 384, 24, m_ZoomRatio, NULL, 0, SS_LEFT, CStaticCx::OwnerDrawGlass | m_IsHighContrast);
	m_CtrlLabelFF.InitControl(8, 212, 384, 24, m_ZoomRatio, NULL, 0, SS_LEFT, CStaticCx::OwnerDrawGlass | m_IsHighContrast);

	m_CtrlValue05.InitControl(300,  72, 40, 20, m_ZoomRatio, NULL, 0, SS_CENTER, CStaticCx::OwnerDrawGlass | m_IsHighContrast);
	m_CtrlValueC5.InitControl(300, 128, 40, 20, m_ZoomRatio, NULL, 0, SS_CENTER, CStaticCx::OwnerDrawGlass | m_IsHighContrast);
	m_CtrlValueC6.InitControl(300, 184, 40, 20, m_ZoomRatio, NULL, 0, SS_CENTER, CStaticCx::OwnerDrawGlass | m_IsHighContrast);
	m_CtrlValueFF.InitControl(300, 240, 40, 20, m_ZoomRatio, NULL, 0, SS_CENTER, CStaticCx::OwnerDrawGlass | m_IsHighContrast);

	m_CtrlValue05X.InitControl(344,  72, 40, 20, m_ZoomRatio, NULL, 0, SS_CENTER, CStaticCx::OwnerDrawGlass | m_IsHighContrast);
	m_CtrlValueC5X.InitControl(344, 128, 40, 20, m_ZoomRatio, NULL, 0, SS_CENTER, CStaticCx::OwnerDrawGlass | m_IsHighContrast);
	m_CtrlValueC6X.InitControl(344, 184, 40, 20, m_ZoomRatio, NULL, 0, SS_CENTER, CStaticCx::OwnerDrawGlass | m_IsHighContrast);
	m_CtrlValueFFX.InitControl(344, 240, 40, 20, m_ZoomRatio, NULL, 0, SS_CENTER, CStaticCx::OwnerDrawGlass | m_IsHighContrast);

	m_CtrlApply.SetFontEx(m_FontFace, 12, m_ZoomRatio);
	m_CtrlDefault.SetFontEx(m_FontFace, 12, m_ZoomRatio);

	m_CtrlApply.InitControl(220, 268, 160, 28, m_ZoomRatio, NULL, 0, SS_CENTER, CButtonCx::SystemDraw | m_IsHighContrast);
	m_CtrlDefault.InitControl(20, 268, 160, 28, m_ZoomRatio, NULL, 0, SS_CENTER, CButtonCx::SystemDraw | m_IsHighContrast);

	m_CtrlSelectDisk.SetFontEx(m_FontFace, 14, m_ZoomRatio);
	m_CtrlSelectDisk.MoveWindow((DWORD)(8 * m_ZoomRatio), (DWORD)(8 * m_ZoomRatio), (DWORD)(384 * m_ZoomRatio), (DWORD)(40 * m_ZoomRatio));

	m_IsDrawFrame = IsDrawFrame();

	m_CtrlValue05.SetDrawFrame(m_IsDrawFrame);
	m_CtrlValueC5.SetDrawFrame(m_IsDrawFrame);
	m_CtrlValueC6.SetDrawFrame(m_IsDrawFrame);
	m_CtrlValueFF.SetDrawFrame(m_IsDrawFrame);
	m_CtrlValue05X.SetDrawFrame(m_IsDrawFrame);
	m_CtrlValueC5X.SetDrawFrame(m_IsDrawFrame);
	m_CtrlValueC6X.SetDrawFrame(m_IsDrawFrame);
	m_CtrlValueFFX.SetDrawFrame(m_IsDrawFrame);

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

	CDialogCx::OnHScroll(nSBCode, nPos, pScrollBar);
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
			m_CtrlScrollbarFF.SetScrollPos(GetPrivateProfileInt(_T("ThreasholdOfCautionFF"), p->m_Ata.vars[index].ModelSerial, 10, m_Ini));
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
		m_CtrlScrollbar05.SetScrollPos(GetPrivateProfileInt(_T("ThreasholdOfCaution05"), p->m_Ata.vars[index].ModelSerial, 1, m_Ini));
		m_CtrlScrollbarC5.SetScrollPos(GetPrivateProfileInt(_T("ThreasholdOfCautionC5"), p->m_Ata.vars[index].ModelSerial, 1, m_Ini));
		m_CtrlScrollbarC6.SetScrollPos(GetPrivateProfileInt(_T("ThreasholdOfCautionC6"), p->m_Ata.vars[index].ModelSerial, 1, m_Ini));
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

void CHealthDlg::OnBnClickedApply()
{
	UpdateData(TRUE);

	if(! p->m_Ata.vars[m_DiskIndex].IsSsd)
	{
		WritePrivateProfileString(_T("ThreasholdOfCaution05"), p->m_Ata.vars[m_DiskIndex].ModelSerial, m_Value05, m_Ini);
		WritePrivateProfileString(_T("ThreasholdOfCautionC5"), p->m_Ata.vars[m_DiskIndex].ModelSerial, m_ValueC5, m_Ini);
		WritePrivateProfileString(_T("ThreasholdOfCautionC6"), p->m_Ata.vars[m_DiskIndex].ModelSerial, m_ValueC6, m_Ini);

		p->m_Ata.vars[m_DiskIndex].Threshold05 = _tstoi(m_Value05);
		p->m_Ata.vars[m_DiskIndex].ThresholdC5 = _tstoi(m_ValueC5);
		p->m_Ata.vars[m_DiskIndex].ThresholdC6 = _tstoi(m_ValueC6);
	}
	else if(p->m_Ata.vars[m_DiskIndex].Life >= 0)
	{
		WritePrivateProfileString(_T("ThreasholdOfCautionFF"), p->m_Ata.vars[m_DiskIndex].ModelSerial, m_ValueFF, m_Ini);
		p->m_Ata.vars[m_DiskIndex].ThresholdFF = _tstoi(m_ValueFF);
	}
	p->SendMessage(WM_COMMAND, ID_REFRESH);
}

void CHealthDlg::OnBnClickedDefault()
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
