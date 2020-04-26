/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DiskInfo.h"
#include "DiskInfoDlg.h"
#include "AboutDlg.h"

IMPLEMENT_DYNCREATE(CAboutDlg, CDialog)

CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialogFx(CAboutDlg::IDD, pParent)
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

#ifdef SUISHO_SHIZUKU_SUPPORT
	#ifdef KUREI_KEI_SUPPORT
		m_BackgroundName = L"KureiKeiAbout";
	#else
		m_BackgroundName = L"ShizukuAbout";
	#endif
#else
	m_BackgroundName = L"Background";
#endif
}

CAboutDlg::~CAboutDlg()
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogFx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CRYSTAL_DEW_WORLD, m_CtrlCrystalDewWorld);

	DDX_Control(pDX, IDC_PROJECT_SITE_1, m_CtrlProjectSite1);
	DDX_Control(pDX, IDC_PROJECT_SITE_2, m_CtrlProjectSite2);
	DDX_Control(pDX, IDC_PROJECT_SITE_3, m_CtrlProjectSite3);
	DDX_Control(pDX, IDC_PROJECT_SITE_4, m_CtrlProjectSite4);
	DDX_Control(pDX, IDC_PROJECT_SITE_5, m_CtrlProjectSite5);

	DDX_Control(pDX, IDC_SECRET_VOICE, m_CtrlSecretVoice);

	DDX_Control(pDX, IDC_VERSION, m_CtrlVersion);
	DDX_Control(pDX, IDC_RELEASE, m_CtrlRelease);
	DDX_Control(pDX, IDC_COPYRIGHT, m_CtrlCopyright);
	DDX_Control(pDX, IDC_LICENSE, m_CtrlLicense);
	DDX_Control(pDX, IDC_EDITION, m_CtrlEdition);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialogFx::OnInitDialog();

	SetWindowText(i18n(_T("WindowTitle"), _T("ABOUT")));

	m_bShowWindow = TRUE;
	m_CtrlVersion.SetWindowTextW(PRODUCT_NAME L" " PRODUCT_VERSION);
	m_CtrlEdition.SetWindowTextW(PRODUCT_EDITION);
	m_CtrlRelease.SetWindowTextW(L"Release: " PRODUCT_RELEASE);
	m_CtrlCopyright.SetWindowTextW(PRODUCT_COPYRIGHT);
	m_CtrlLicense.SetWindowTextW(PRODUCT_LICENSE);

#ifndef SUISHO_SHIZUKU_SUPPORT
	m_CtrlProjectSite1.ShowWindow(SW_HIDE);
	m_CtrlProjectSite2.ShowWindow(SW_HIDE);
	m_CtrlProjectSite3.ShowWindow(SW_HIDE);
	m_CtrlProjectSite4.ShowWindow(SW_HIDE);
	m_CtrlProjectSite5.ShowWindow(SW_HIDE);
	m_CtrlSecretVoice.ShowWindow(SW_HIDE);
#endif
	
	UpdateDialogSize();

	CenterWindow();
	ShowWindow(SW_SHOW);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogFx)
	ON_BN_CLICKED(IDC_CRYSTAL_DEW_WORLD, &CAboutDlg::OnCrystalDewWorld)
#ifdef SUISHO_SHIZUKU_SUPPORT
	ON_BN_CLICKED(IDC_SECRET_VOICE, &CAboutDlg::OnSecretVoice)
	ON_BN_CLICKED(IDC_PROJECT_SITE_1, &CAboutDlg::OnBnClickedProjectSite1)
	ON_BN_CLICKED(IDC_PROJECT_SITE_2, &CAboutDlg::OnBnClickedProjectSite2)
	ON_BN_CLICKED(IDC_PROJECT_SITE_3, &CAboutDlg::OnBnClickedProjectSite3)
	ON_BN_CLICKED(IDC_PROJECT_SITE_4, &CAboutDlg::OnBnClickedProjectSite4)
	ON_BN_CLICKED(IDC_PROJECT_SITE_5, &CAboutDlg::OnBnClickedProjectSite5)
#endif
END_MESSAGE_MAP()

void CAboutDlg::UpdateDialogSize()
{
	ChangeZoomType(m_ZoomType);
	if (m_bHighContrast)
	{
		SetClientSize((DWORD)(SIZE_X * m_ZoomRatio), (DWORD)(SIZE_HC_Y * m_ZoomRatio), 0);
	}
	else
	{
		SetClientSize((DWORD)(SIZE_X * m_ZoomRatio), (DWORD)(SIZE_Y * m_ZoomRatio), 0);
	}

	UpdateBackground(true);

	m_CtrlCrystalDewWorld.InitControl( 24,  24, 128, 128, m_ZoomRatio, IP(L"logo"), 1, 0, CButtonCx::OwnerDrawImage);
	m_CtrlCrystalDewWorld.SetHandCursor();

#ifdef SUISHO_SHIZUKU_SUPPORT

	#ifdef KUREI_KEI_SUPPORT
	m_CtrlProjectSite1.InitControl(340, 232, 268, 100, m_ZoomRatio, NULL, 0, 0, CButtonCx::OwnerDrawTransparent | m_bHighContrast);
	m_CtrlProjectSite2.InitControl(386, 376, 48, 40, m_ZoomRatio, NULL, 0, 0, CButtonCx::OwnerDrawTransparent | m_bHighContrast);
	m_CtrlProjectSite3.InitControl(492, 376, 108, 40, m_ZoomRatio, NULL, 0, 0, CButtonCx::OwnerDrawTransparent | m_bHighContrast);
	m_CtrlProjectSite4.InitControl(386, 600, 224, 60, m_ZoomRatio, NULL, 0, 0, CButtonCx::OwnerDrawTransparent | m_bHighContrast);
	m_CtrlProjectSite5.InitControl(0, 0, 0, 0, m_ZoomRatio, NULL, 0, 0, CButtonCx::OwnerDrawTransparent | m_bHighContrast);
	m_CtrlProjectSite5.ShowWindow(SW_HIDE);
	m_CtrlSecretVoice.InitControl(132, 324, 48, 28, m_ZoomRatio, NULL, 0, 0, CButtonCx::OwnerDrawTransparent | m_bHighContrast);
	#else
	m_CtrlProjectSite1.InitControl(340, 324, 292, 20, m_ZoomRatio, NULL, 0, 0, CButtonCx::OwnerDrawTransparent | m_bHighContrast);
	m_CtrlProjectSite2.InitControl(340, 368, 292, 20, m_ZoomRatio, NULL, 0, 0, CButtonCx::OwnerDrawTransparent | m_bHighContrast);
	m_CtrlProjectSite3.InitControl(340, 392, 292, 20, m_ZoomRatio, NULL, 0, 0, CButtonCx::OwnerDrawTransparent | m_bHighContrast);
	m_CtrlProjectSite4.InitControl(340, 416, 292, 20, m_ZoomRatio, NULL, 0, 0, CButtonCx::OwnerDrawTransparent | m_bHighContrast);
	m_CtrlProjectSite5.InitControl(320, 192, 292, 92, m_ZoomRatio, NULL, 0, 0, CButtonCx::OwnerDrawTransparent | m_bHighContrast);
	m_CtrlSecretVoice.InitControl(200, 412, 40, 40, m_ZoomRatio, NULL, 0, 0, CButtonCx::OwnerDrawTransparent | m_bHighContrast);
	#endif
	
	m_CtrlProjectSite1.SetHandCursor();
	m_CtrlProjectSite2.SetHandCursor();
	m_CtrlProjectSite3.SetHandCursor();
	m_CtrlProjectSite4.SetHandCursor();
	m_CtrlProjectSite5.SetHandCursor();
	m_CtrlSecretVoice.SetHandCursor();
#endif

	m_CtrlVersion.SetFontEx(m_FontFace, 28, m_ZoomRatio, 255, RGB(0, 0, 0), FW_BOLD);
	m_CtrlEdition.SetFontEx(m_FontFace, 28, m_ZoomRatio, 255, RGB(0, 0, 0), FW_BOLD);
	m_CtrlRelease.SetFontEx(m_FontFace, 16, m_ZoomRatio, 255, RGB(0, 0, 0), FW_NORMAL);
	m_CtrlCopyright.SetFontEx(m_FontFace, 16, m_ZoomRatio, 255, RGB(0, 0, 0), FW_NORMAL);
	m_CtrlLicense.SetFontEx(m_FontFace, 16, m_ZoomRatio, 255, RGB(0, 0, 0), FW_NORMAL);

	m_CtrlVersion.InitControl(200, 12, 420, 36, m_ZoomRatio, NULL, 0, SS_CENTER, OwnerDrawTransparent | m_bHighContrast);
	m_CtrlEdition.InitControl(200, 48, 420, 36, m_ZoomRatio, NULL, 0, SS_CENTER, OwnerDrawTransparent | m_bHighContrast);
	m_CtrlRelease.InitControl(200, 88, 420, 24, m_ZoomRatio, NULL, 0, SS_CENTER, OwnerDrawTransparent | m_bHighContrast);
	m_CtrlCopyright.InitControl(200, 112, 420, 24, m_ZoomRatio, NULL, 0, SS_CENTER, OwnerDrawTransparent | m_bHighContrast);
	m_CtrlLicense.InitControl(200, 136, 420, 24, m_ZoomRatio, NULL, 0, SS_CENTER, OwnerDrawTransparent | m_bHighContrast);

	Invalidate();
}


void CAboutDlg::OnCrystalDewWorld()
{

#ifdef KUREI_KEI_SUPPORT
	OpenUrl(URL_PRONAMA);
#else
	if (GetUserDefaultLCID() == 0x0411)// Japanese
	{
		OpenUrl(URL_CRYSTAL_DEW_WORLD_JA);
	}
	else // Other Language
	{
		OpenUrl(URL_CRYSTAL_DEW_WORLD_EN);
	}
#endif
}

#ifdef SUISHO_SHIZUKU_SUPPORT
void CAboutDlg::OnSecretVoice()
{
	::PostMessage(m_ParentWnd->GetSafeHwnd(), MY_PLAY_ALERT_SOUND, 901, NULL);
}

void CAboutDlg::OnBnClickedProjectSite1()
{
	OpenUrl(URL_PROJECT_SITE_1);
}

void CAboutDlg::OnBnClickedProjectSite2()
{
	OpenUrl(URL_PROJECT_SITE_2);
}

void CAboutDlg::OnBnClickedProjectSite3()
{
	OpenUrl(URL_PROJECT_SITE_3);
}

void CAboutDlg::OnBnClickedProjectSite4()
{
	OpenUrl(URL_PROJECT_SITE_4);
}

void CAboutDlg::OnBnClickedProjectSite5()
{
	OpenUrl(URL_PROJECT_SITE_5);
}
#endif