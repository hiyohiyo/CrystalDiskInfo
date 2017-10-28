/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DiskInfo.h"
#include "DiskInfoDlg.h"
#include "SoundSettingDlg.h"

IMPLEMENT_DYNCREATE(CSoundSettingDlg, CDialog)

static CDiskInfoDlg *p;

CSoundSettingDlg::CSoundSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogCx(CDiskInfoDlg::IDD, pParent)
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

CSoundSettingDlg::~CSoundSettingDlg()
{
}

void CSoundSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogCx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_FILE_PATH, m_FilePath);

	DDX_Control(pDX, IDC_FILE_PATH, m_CtrlFilePath);
	DDX_Control(pDX, IDC_BUTTON_SELECT_FILE, m_CtrlSelectFile);
	DDX_Control(pDX, IDC_BUTTON_PLAY, m_CtrlPlay);
	DDX_Control(pDX, IDC_BUTTON_DEFAULT, m_CtrlDefault);
	DDX_Control(pDX, IDC_BUTTON_OK, m_CtrlOk);
}

BOOL CSoundSettingDlg::OnInitDialog()
{
	CDialogCx::OnInitDialog();

	SetWindowText(i18n(_T("WindowTitle"), _T("SOUND_SETTINGS")));

	m_FlagShowWindow = TRUE;

	TCHAR str[256];
	GetPrivateProfileString(_T("Setting"), _T("AlertSoundPath"), _T(""), str, 256, m_Ini);
	m_FilePath = str;
	UpdateData(FALSE);

	m_CtrlDefault.SetWindowTextW(i18n(_T("HealthStatus"), _T("DEFAULT")));

	UpdateDialogSize();

	CenterWindow();
	ShowWindow(SW_SHOW);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CSoundSettingDlg, CDialogCx)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_FILE, &CSoundSettingDlg::OnBnClickedButtonSelectFile)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CSoundSettingDlg::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT, &CSoundSettingDlg::OnBnClickedButtonDefault)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CSoundSettingDlg::OnBnClickedButtonOk)
END_MESSAGE_MAP()

void CSoundSettingDlg::UpdateDialogSize()
{
	ChangeZoomType(m_ZoomType);
	SetClientRect((DWORD)(SIZE_X * m_ZoomRatio), (DWORD)(SIZE_Y * m_ZoomRatio), 0);

	UpdateBackground();

	m_CtrlFilePath.SetFontEx(m_FontFace, 12, m_ZoomRatio);
	m_CtrlSelectFile.SetFontEx(m_FontFace, 12, m_ZoomRatio);
	m_CtrlPlay.SetFontEx(m_FontFace, 12, m_ZoomRatio);
	m_CtrlDefault.SetFontEx(m_FontFace, 12, m_ZoomRatio);
	m_CtrlOk.SetFontEx(m_FontFace, 12, m_ZoomRatio);

	m_CtrlFilePath.InitControl(8, 8, 416, 24, m_ZoomRatio, NULL, 0, SS_LEFT, CStaticCx::OwnerDrawGlass | m_IsHighContrast);
	m_CtrlSelectFile.InitControl(428, 8, 24, 24, m_ZoomRatio, IP(L"selectSound"), 2, BS_CENTER, CButtonCx::OwnerDrawImage);
	m_CtrlSelectFile.SetHandCursor();
	m_CtrlPlay.InitControl(456, 8, 24, 24, m_ZoomRatio, IP(L"playSound"), 2, BS_CENTER, CButtonCx::OwnerDrawImage);
	m_CtrlPlay.SetHandCursor();
	m_CtrlDefault.InitControl(40, 40, 160, 28, m_ZoomRatio, NULL, 0, BS_CENTER, CButtonCx::SystemDraw | m_IsHighContrast);
	m_CtrlOk.InitControl(280, 40, 160, 28, m_ZoomRatio, NULL, 0, BS_CENTER, CButtonCx::SystemDraw | m_IsHighContrast);

	m_IsDrawFrame = IsDrawFrame();
	m_CtrlFilePath.SetDrawFrame(m_IsDrawFrame);

	Invalidate();
}

void CSoundSettingDlg::OnBnClickedButtonSelectFile()
{
	CFileDialog dlg(TRUE, _T(""), _T(""), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER,
		i18n(_T("Customize"), _T("SUPPORTED_SOUND_FORMAT"))
		+ _T("|*.wav;*.mp3;*.opus;|WAVE (*.wav)|*.wav|MP3 (*.mp3)|*.mp3|Opus (*.opus)|*.opus|")
		+ i18n(_T("Customize"), _T("ALL_FILES"))
		+ _T(" (*.*)|*.*||"));

	if(dlg.DoModal() == IDOK)
	{
		m_FilePath = dlg.GetPathName();
		WritePrivateProfileString(_T("Setting"), _T("AlertSoundPath"), _T("\"") + m_FilePath + _T("\""), m_Ini);
		m_CtrlFilePath.SetToolTipText(m_FilePath);
		UpdateData(FALSE);
	}
}

void CSoundSettingDlg::OnBnClickedButtonPlay()
{
	::PostMessage(m_ParentWnd->GetSafeHwnd(), MY_PLAY_ALERT_SOUND, NULL, NULL);
}


void CSoundSettingDlg::OnBnClickedButtonDefault()
{
	m_FilePath = _T("");
	WritePrivateProfileString(_T("Setting"), _T("AlertSoundPath"), m_FilePath, m_Ini);
	m_CtrlFilePath.SetToolTipText(m_FilePath);
	UpdateData(FALSE);
}

void CSoundSettingDlg::OnBnClickedButtonOk()
{
	CDialogCx::OnCancel();
}
