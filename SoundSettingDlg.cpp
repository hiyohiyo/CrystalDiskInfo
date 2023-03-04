/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DiskInfo.h"
#include "DiskInfoDlg.h"
#include "SoundSettingDlg.h"

IMPLEMENT_DYNCREATE(CSoundSettingDlg, CDialog)

static CDiskInfoDlg* p;

CSoundSettingDlg::CSoundSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogFx(CDiskInfoDlg::IDD, pParent)
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

CSoundSettingDlg::~CSoundSettingDlg()
{
}

void CSoundSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogFx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_FILE_PATH, m_FilePath);

	DDX_Control(pDX, IDC_FILE_PATH, m_CtrlFilePath);
	DDX_Control(pDX, IDC_BUTTON_SELECT_FILE, m_CtrlSelectFile);
	DDX_Control(pDX, IDC_BUTTON_PLAY, m_CtrlPlay);
	DDX_Control(pDX, IDC_SLIDER_VOLUME, m_CtrlSlider);
	DDX_Control(pDX, IDC_STATIC_VOLUME, m_CtrlVolume);
	DDX_Control(pDX, IDC_VALUE_VOLUME, m_CtrlValueVolume);
	DDX_Control(pDX, IDC_BUTTON_DEFAULT, m_CtrlDefault);
	DDX_Control(pDX, IDC_BUTTON_OK, m_CtrlOk);
}

BOOL CSoundSettingDlg::OnInitDialog()
{
	CDialogFx::OnInitDialog();

	SetWindowTitle(i18n(_T("WindowTitle"), _T("SOUND_SETTINGS")));

	m_bShowWindow = TRUE;

	TCHAR str[256];
	GetPrivateProfileStringFx(_T("Setting"), _T("AlertSoundPath"), _T(""), str, 256, m_Ini);
	m_FilePath = str;
	UpdateData(FALSE);

	m_InitialVolume = GetPrivateProfileInt(_T("Setting"), _T("AlertSoundVolume"), 80, m_Ini);
	if (m_InitialVolume < 0 || m_InitialVolume > 100)
	{
		m_InitialVolume = 80;
	}
	m_CurrentVolume = m_InitialVolume;
	CString cstr;
	cstr.Format(L"%d", m_CurrentVolume);
	m_CtrlValueVolume.SetWindowTextW(cstr);

	m_CtrlDefault.SetWindowTextW(i18n(_T("HealthStatus"), _T("DEFAULT")));
	m_CtrlOk.SetWindowTextW(i18n(_T("Dialog"), _T("OK")));

	UpdateDialogSize();

	CenterWindow();
	ShowWindow(SW_SHOW);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CSoundSettingDlg, CDialogFx)
	ON_WM_CTLCOLOR()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_SELECT_FILE, &CSoundSettingDlg::OnSelectFile)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CSoundSettingDlg::OnPlay)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT, &CSoundSettingDlg::OnDefault)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CSoundSettingDlg::OnOk)
END_MESSAGE_MAP()

void CSoundSettingDlg::UpdateDialogSize()
{
	CDialogFx::UpdateDialogSize();

	ChangeZoomType(m_ZoomType);
	SetClientSize(SIZE_X, SIZE_Y, m_ZoomRatio);
	UpdateBackground(FALSE, m_bDarkMode);

	m_CtrlFilePath.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);
	m_CtrlSelectFile.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);
	m_CtrlPlay.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);
	m_CtrlValueVolume.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);
	m_CtrlDefault.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);
	m_CtrlOk.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, RGB(0, 0, 0), FW_NORMAL, m_FontRender);

	m_CtrlFilePath.InitControl(8, 8, 416, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, TRUE);
	m_CtrlSelectFile.InitControl(428, 8, 24, 24, m_ZoomRatio, &m_BkDC, IP(L"selectSound"), 2, BS_CENTER, OwnerDrawImage, FALSE, m_bDarkMode, FALSE);
	m_CtrlSelectFile.SetHandCursor();
	m_CtrlPlay.InitControl(456, 8, 24, 24, m_ZoomRatio, &m_BkDC, IP(L"playSound"), 2, BS_CENTER, OwnerDrawImage, FALSE, m_bDarkMode, FALSE);
	m_CtrlPlay.SetHandCursor();
	m_CtrlSlider.InitControl(32, 40, 396, 24, m_ZoomRatio, &m_BkDC, SystemDraw, m_bHighContrast, m_bDarkMode, 0, 100, m_InitialVolume);
	SetVolumeImage();
	m_CtrlValueVolume.InitControl(428, 40, 52, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, TRUE);
	m_CtrlDefault.InitControl(40, 72, 160, 24, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, SystemDraw, m_bHighContrast, m_bDarkMode, FALSE);
	m_CtrlOk.InitControl(280, 72, 160, 24, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, SystemDraw, m_bHighContrast, m_bDarkMode, FALSE);

	m_CtrlFilePath.SetDrawFrame(TRUE);
	m_CtrlValueVolume.SetDrawFrame(TRUE);

	m_CtrlOk.SetHandCursor();
	m_CtrlDefault.SetHandCursor();

	SetDarkModeControl(m_CtrlOk.GetSafeHwnd(), m_bDarkMode);
	SetDarkModeControl(m_CtrlDefault.GetSafeHwnd(), m_bDarkMode);

	Invalidate();
}

void CSoundSettingDlg::OnSelectFile()
{
	CFileDialog dlg(TRUE, _T(""), _T(""), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER,
		i18n(_T("Customize"), _T("SUPPORTED_SOUND_FORMAT"))
		+ _T("|*.wav;*.mp3;*.opus;|WAVE (*.wav)|*.wav|MP3 (*.mp3)|*.mp3|Opus (*.opus)|*.opus|")
		+ i18n(_T("Customize"), _T("ALL_FILES"))
		+ _T(" (*.*)|*.*||"));

	if(dlg.DoModal() == IDOK)
	{
		m_FilePath = dlg.GetPathName();
		WritePrivateProfileStringFx(_T("Setting"), _T("AlertSoundPath"), m_FilePath, m_Ini);
		m_CtrlFilePath.SetToolTipText(m_FilePath);
		UpdateData(FALSE);
	}
}

void CSoundSettingDlg::OnPlay()
{
	const int pos = m_CtrlSlider.GetPos();
	if (pos != m_CurrentVolume)
	{
		m_CurrentVolume = pos;
		CString volume;
		volume.Format(_T("%d"), m_CurrentVolume);
		WritePrivateProfileStringFx(_T("Setting"), _T("AlertSoundVolume"), volume, m_Ini);
	}

	::PostMessage(m_ParentWnd->GetSafeHwnd(), MY_PLAY_ALERT_SOUND, NULL, NULL);
}

void CSoundSettingDlg::OnDefault()
{
	m_CtrlSlider.SetPos(80);
	m_CtrlValueVolume.SetWindowTextW(L"80");

	m_FilePath = _T("");
	WritePrivateProfileStringFx(_T("Setting"), _T("AlertSoundPath"), m_FilePath, m_Ini);
	m_CtrlFilePath.SetToolTipText(m_FilePath);
	UpdateData(FALSE);
}

void CSoundSettingDlg::OnOk()
{
	CString volume;
	volume.Format(_T("%d"), m_CtrlSlider.GetPos());
	WritePrivateProfileStringFx(_T("Setting"), _T("AlertSoundVolume"), volume, m_Ini);

	CDialogFx::OnCancel();
}

void CSoundSettingDlg::OnCancel()
{
	// Revert
	if (m_CurrentVolume != m_InitialVolume)
	{
		CString volume;
		volume.Format(_T("%d"), m_InitialVolume);
		WritePrivateProfileStringFx(_T("Setting"), _T("AlertSoundVolume"), volume, m_Ini);
	}

	CDialogFx::OnCancel();
}

HBRUSH CSoundSettingDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogFx::OnCtlColor(pDC, pWnd, nCtlColor);

	switch (nCtlColor)
	{
	case CTLCOLOR_STATIC:
		if (pWnd->m_hWnd == m_CtrlSlider.m_hWnd && !m_CtrlSlider.m_bHighContrast)
		{
			pDC->SetBkMode(TRANSPARENT);
			return m_CtrlSlider.m_BkBrush;
		}
		return hbr;
		break;
	default:
		return hbr;
	}
}

void CSoundSettingDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CDialogFx::OnHScroll(nSBCode, nPos, pScrollBar);

	CString cstr;
	cstr.Format(L"%d", m_CtrlSlider.GetPos());
	m_CtrlValueVolume.SetWindowTextW(cstr);
	SetVolumeImage();
}

void CSoundSettingDlg::SetVolumeImage()
{
	int volume = m_CtrlSlider.GetPos();
	CString imageName = L"";
	if (volume > 66)
	{
		imageName = L"volumeL";
	}
	else if (volume > 33)
	{
		imageName = L"volumeM";
	}
	else if (volume > 0)
	{
		imageName = L"volumeS";
	}
	else
	{
		imageName = L"volumeZ";
	}
	m_CtrlVolume.InitControl(8, 40, 24, 24, m_ZoomRatio, &m_BkDC, IP(imageName), 1, BS_CENTER, OwnerDrawImage, FALSE, FALSE, FALSE);
}
