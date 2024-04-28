/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DiskInfo.h"
#include "DiskInfoDlg.h"
#include "AboutDlg.h"

IMPLEMENT_DYNCREATE(CAboutDlg, CDialog)

CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialogFx(CAboutDlg::IDD, pParent)
{
	CMainDialogFx* p = (CMainDialogFx*)pParent;

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

#ifdef SUISHO_AOI_SUPPORT
	m_BackgroundName = L"AoiAbout";
#elif MSI_MEI_SUPPORT
	m_BackgroundName = L"About";
#elif KUREI_KEI_SUPPORT
	m_BackgroundName = L"KureiKeiAbout";
#elif SUISHO_SHIZUKU_SUPPORT
	m_BackgroundName = L"ShizukuAbout";
#else
	m_BackgroundName = L"";
#endif
}

CAboutDlg::~CAboutDlg()
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogFx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGO, m_CtrlLogo);

	DDX_Control(pDX, IDC_PROJECT_SITE_1, m_CtrlProjectSite1);
	DDX_Control(pDX, IDC_PROJECT_SITE_2, m_CtrlProjectSite2);
	DDX_Control(pDX, IDC_PROJECT_SITE_3, m_CtrlProjectSite3);
	DDX_Control(pDX, IDC_PROJECT_SITE_4, m_CtrlProjectSite4);
	DDX_Control(pDX, IDC_PROJECT_SITE_5, m_CtrlProjectSite5);

	DDX_Control(pDX, IDC_SECRET_VOICE, m_CtrlSecretVoice);
	DDX_Control(pDX, IDC_VERSION, m_CtrlVersion);
	DDX_Control(pDX, IDC_LICENSE, m_CtrlLicense);	
	DDX_Control(pDX, IDC_RELEASE, m_CtrlRelease);
	DDX_Control(pDX, IDC_COPYRIGHT1, m_CtrlCopyright1);
	DDX_Control(pDX, IDC_COPYRIGHT2, m_CtrlCopyright2);
	DDX_Control(pDX, IDC_EDITION, m_CtrlEdition);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialogFx::OnInitDialog();

	SetWindowTitle(i18n(L"WindowTitle", L"ABOUT"));

	m_bShowWindow = TRUE;
	m_CtrlVersion.SetWindowTextW(PRODUCT_NAME L" " PRODUCT_VERSION);
	m_CtrlEdition.SetWindowTextW(PRODUCT_EDITION);
	m_CtrlRelease.SetWindowTextW(L"Release: " PRODUCT_RELEASE);
	m_CtrlCopyright1.SetWindowTextW(PRODUCT_COPYRIGHT_1);
	m_CtrlCopyright2.SetWindowTextW(PRODUCT_COPYRIGHT_2);
	m_CtrlLicense.SetWindowTextW(PRODUCT_LICENSE);

#ifndef SUISHO_SHIZUKU_SUPPORT
	m_CtrlProjectSite1.ShowWindow(SW_HIDE);
	m_CtrlProjectSite2.ShowWindow(SW_HIDE);
	m_CtrlProjectSite3.ShowWindow(SW_HIDE);
	m_CtrlProjectSite4.ShowWindow(SW_HIDE);
	m_CtrlProjectSite5.ShowWindow(SW_HIDE);
#endif

	UpdateDialogSize();

	CenterWindow();
	ShowWindow(SW_SHOW);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogFx)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_LOGO, &CAboutDlg::OnLogo)
	ON_BN_CLICKED(IDC_LICENSE, &CAboutDlg::OnLicense)
	ON_BN_CLICKED(IDC_VERSION, &CAboutDlg::OnVersion)
#ifdef SUISHO_SHIZUKU_SUPPORT
	ON_BN_CLICKED(IDC_SECRET_VOICE, &CAboutDlg::OnSecretVoice)
	ON_BN_CLICKED(IDC_PROJECT_SITE_1, &CAboutDlg::OnProjectSite1)
	ON_BN_CLICKED(IDC_PROJECT_SITE_2, &CAboutDlg::OnProjectSite2)
	ON_BN_CLICKED(IDC_PROJECT_SITE_3, &CAboutDlg::OnProjectSite3)
	ON_BN_CLICKED(IDC_PROJECT_SITE_4, &CAboutDlg::OnProjectSite4)
	ON_BN_CLICKED(IDC_PROJECT_SITE_5, &CAboutDlg::OnProjectSite5)
#endif
END_MESSAGE_MAP()


void CAboutDlg::UpdateDialogSize()
{
	CDialogFx::UpdateDialogSize();

	ChangeZoomType(m_ZoomType);
	SetClientSize(SIZE_X, SIZE_Y, m_ZoomRatio);
	UpdateBackground(TRUE, m_bDarkMode);

#ifdef SUISHO_SHIZUKU_SUPPORT
	m_CtrlProjectSite1.SetHandCursor();
	m_CtrlProjectSite2.SetHandCursor();
	m_CtrlProjectSite3.SetHandCursor();
	m_CtrlProjectSite4.SetHandCursor();
	m_CtrlProjectSite5.SetHandCursor();
	m_CtrlSecretVoice.SetHandCursor();
#endif

#ifdef SUISHO_AOI_SUPPORT
	m_CtrlProjectSite1.InitControl(184, 516, 148, 16, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlProjectSite2.InitControl(244, 548, 108, 16, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlProjectSite3.InitControl(232, 564, 180, 16, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlProjectSite4.InitControl(244, 584, 120, 16, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlProjectSite5.InitControl(0, 0, 0, 0, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlLogo.InitControl(32, 496, 128, 144, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlSecretVoice.InitControl(364, 264, 44, 20, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);

#elif MSI_MEI_SUPPORT
	m_CtrlProjectSite1.InitControl(24, 460, 348, 128, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlProjectSite2.InitControl(168, 604, 36, 24, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlProjectSite3.InitControl(332, 604, 104, 24, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlProjectSite4.InitControl(20, 20, 120, 40, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlProjectSite5.InitControl(464, 604, 168, 24, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlLogo.InitControl(80, 64, 128, 128, m_ZoomRatio, &m_BkDC, IP(L"Logo"), 1, BS_CENTER, OwnerDrawImage, FALSE, FALSE, FALSE);
	m_CtrlSecretVoice.InitControl(392, 240, 48, 40, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);

#elif KUREI_KEI_SUPPORT
	m_CtrlProjectSite1.InitControl(340, 232, 268, 100, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlProjectSite2.InitControl(386, 376, 48, 40, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlProjectSite3.InitControl(492, 376, 108, 40, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlProjectSite4.InitControl(386, 600, 224, 60, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlProjectSite5.InitControl(0, 0, 0, 0, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlProjectSite5.ShowWindow(SW_HIDE);
	m_CtrlLogo.InitControl(12, 20, 128, 128, m_ZoomRatio, &m_BkDC, IP(L"Logo"), 1, BS_CENTER, OwnerDrawImage, FALSE, FALSE, FALSE);
	m_CtrlSecretVoice.InitControl(132, 324, 48, 28, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);

#elif SUISHO_SHIZUKU_SUPPORT
	m_CtrlProjectSite1.InitControl(64, 368, 140, 16, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlProjectSite2.InitControl(64, 416, 148, 16, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlProjectSite3.InitControl(64, 432, 184, 16, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlProjectSite4.InitControl(40, 460, 208, 16, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlProjectSite5.InitControl(92, 504, 432, 124, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlLogo.InitControl(80, 12, 128, 128, m_ZoomRatio, &m_BkDC, IP(L"Logo"), 1, BS_CENTER, OwnerDrawImage, FALSE, FALSE, FALSE);
	m_CtrlSecretVoice.InitControl(392, 280, 44, 24, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);

#else
	m_CtrlLogo.InitControl(12, 12, 128, 128, m_ZoomRatio, &m_BkDC, IP(L"Logo"), 1, BS_CENTER, OwnerDrawImage, FALSE, FALSE, FALSE);
#endif

	m_CtrlLogo.SetHandCursor();

#ifdef MSI_MEI_SUPPORT
	COLORREF fontColor = RGB(255, 255, 255);
#else
	COLORREF fontColor = RGB(0, 0, 0);
#endif

	m_CtrlVersion.SetFontEx(m_FontFace, 22, 22, m_ZoomRatio, m_FontRatio, fontColor, FW_BOLD, m_FontRender);
	m_CtrlEdition.SetFontEx(m_FontFace, 18, 18, m_ZoomRatio, m_FontRatio, fontColor, FW_BOLD, m_FontRender);
	m_CtrlRelease.SetFontEx(m_FontFace, 16, 16, m_ZoomRatio, m_FontRatio, fontColor, FW_NORMAL, m_FontRender);
	m_CtrlCopyright1.SetFontEx(m_FontFace, 16, 16, m_ZoomRatio, m_FontRatio, fontColor, FW_NORMAL, m_FontRender);
	m_CtrlCopyright2.SetFontEx(m_FontFace, 16, 16, m_ZoomRatio, m_FontRatio, fontColor, FW_NORMAL, m_FontRender);
	m_CtrlLicense.SetFontEx(m_FontFace, 16, 16, m_ZoomRatio, m_FontRatio, fontColor, FW_NORMAL, m_FontRender);

	m_CtrlVersion.SetHandCursor();
	m_CtrlLicense.SetHandCursor();

#ifdef SUISHO_AOI_SUPPORT
	m_CtrlVersion.InitControl(0, 52, 288, 28, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlEdition.InitControl(0, 80, 288, 28, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlRelease.InitControl(0, 116, 288, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlCopyright1.InitControl(0, 136, 288, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlCopyright2.InitControl(0, 156, 288, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlLicense.InitControl(0, 176, 288, 20, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);

#elif MSI_MEI_SUPPORT
	m_CtrlVersion.InitControl(0, 204, 288, 28, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlEdition.InitControl(0, 232, 288, 28, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlRelease.InitControl(0, 268, 288, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlCopyright1.InitControl(0, 288, 288, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlCopyright2.InitControl(0, 308, 288, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlLicense.InitControl(0, 0, 0, 0, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);

#elif KUREI_KEI_SUPPORT
	m_CtrlVersion.InitControl(152, 12, 476, 28, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlEdition.InitControl(152, 40, 476, 28, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlRelease.InitControl(152, 72, 476, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlCopyright1.InitControl(152, 96, 476, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlCopyright2.InitControl(152, 116, 476, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlLicense.InitControl(152, 136, 476, 20, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);

#elif SUISHO_SHIZUKU_SUPPORT
	m_CtrlVersion.InitControl(0, 152, 288, 28, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlEdition.InitControl(0, 180, 288, 28, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlRelease.InitControl(0, 216, 288, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlCopyright1.InitControl(0, 236, 288, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlCopyright2.InitControl(0, 256, 288, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlLicense.InitControl(0, 276, 288, 20, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);

#else
	m_CtrlVersion.InitControl(140, 12, 340, 28, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, FALSE);
	m_CtrlEdition.InitControl(140, 40, 340, 28, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, FALSE);
	m_CtrlRelease.InitControl(140, 76, 340, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, FALSE);
	m_CtrlCopyright1.InitControl(140, 100, 340, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, FALSE);
	m_CtrlLicense.InitControl(140, 120, 340, 20, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, m_bHighContrast, m_bDarkMode, FALSE);
	m_CtrlCopyright2.ShowWindow(SW_HIDE);
#endif

	Invalidate();
}

BOOL CAboutDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);

	return pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &m_BkDC, 0, 0, SRCCOPY);
}

void CAboutDlg::OnLogo()
{
	if (GetUserDefaultLCID() == 0x0411)// Japanese
	{
		OpenUrl(URL_MAIN_JA);
	}
	else // Other Language
	{
		OpenUrl(URL_MAIN_EN);
	}
}

void CAboutDlg::OnVersion()
{
	if (GetUserDefaultLCID() == 0x0411)// Japanese
	{
		OpenUrl(URL_VERSION_JA);
	}
	else // Other Language
	{
		OpenUrl(URL_VERSION_EN);
	}

}
void CAboutDlg::OnLicense()
{
	if (GetUserDefaultLCID() == 0x0411)// Japanese
	{
		OpenUrl(URL_LICENSE_JA);
	}
	else // Other Language
	{
		OpenUrl(URL_LICENSE_EN);
	}
}

#ifdef SUISHO_SHIZUKU_SUPPORT
void CAboutDlg::OnSecretVoice()
{
	::PostMessage(m_ParentWnd->GetSafeHwnd(), MY_PLAY_ALERT_SOUND, 901, NULL);
}

void CAboutDlg::OnProjectSite1()
{
	OpenUrl(URL_PROJECT_SITE_1);
}

void CAboutDlg::OnProjectSite2()
{
	OpenUrl(URL_PROJECT_SITE_2);
}

void CAboutDlg::OnProjectSite3()
{
	OpenUrl(URL_PROJECT_SITE_3);
}

void CAboutDlg::OnProjectSite4()
{
	OpenUrl(URL_PROJECT_SITE_4);
}

void CAboutDlg::OnProjectSite5()
{
	OpenUrl(URL_PROJECT_SITE_5);
}
#endif