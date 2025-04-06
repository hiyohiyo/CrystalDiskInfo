/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "Resource.h"
#include "DialogFx.h"
#include "UtilityFx.h"
#include "OsInfoFx.h"

//#include <Shlwapi.h>
#if _MSC_VER > 1310
#include <strsafe.h>
#endif

using namespace Gdiplus;
#pragma	comment(lib,"Gdiplus.lib")

// defined by Windows 8.1/Windows 2012 R2
#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

////------------------------------------------------
//   CDialogFx
////------------------------------------------------

CDialogFx::CDialogFx(UINT dlgResouce, CWnd* pParent)
	      :CDialog(dlgResouce, pParent)
{
	// Dialog
	m_bInitializing = TRUE;
	m_bDpiChanging = FALSE;
	m_bShowWindow = FALSE;
	m_bModelessDlg = FALSE;
	m_bHighContrast = FALSE;
	m_bDarkMode = FALSE;
	m_bDisableDarkMode = FALSE;
	m_bBkImage = FALSE;
	m_MenuId = 0;
	m_ParentWnd = NULL;
	m_DlgWnd = NULL;
	m_hAccelerator = NULL;
	m_bDrag = FALSE;
	m_FontScale = 100;
	m_FontRatio = 1.0;
	m_FontRender = CLEARTYPE_NATURAL_QUALITY;
	m_hPal = NULL;

	m_SizeX = 0;
	m_MaxSizeX = 65535;
	m_MinSizeX = 0;
	m_SizeY = 0;
	m_MaxSizeY = 65535;
	m_MinSizeY = 0;

	// Zoom
	m_Dpi = 96;
	m_ZoomRatio = 1.0;
	m_ZoomType = ZoomTypeAuto;

	// Color for SubClass
	m_LabelText = 0x00000000;
	m_MeterText = 0x00000000;
	m_ComboText = 0x00000000;
	m_ComboTextSelected = 0x00808080;
	m_ComboBk = 0x00FFFFFF;
	m_ComboBkSelected = 0x00808080;
	m_ButtonText = 0x00000000;
	m_EditText = 0x00000000;
	m_EditBk = 0x00FFFFFF;
	m_ListText1 = 0x00000000;
	m_ListText2 = 0x00000000;
	m_ListTextSelected = 0x00000000;
	m_ListBk1 = 0x00FFFFFF;
	m_ListBk2 = 0x00FFFFFF;
	m_ListBkSelected = 0x00808080;
	m_ListLine1 = 0x00000000;
	m_ListLine2 = 0x00000000;
	m_Glass = 0x00808080;
	m_Frame = 0x00808080;
	m_Background = 0xFFFFFFFF; // Disabled

	m_ComboAlpha = 0;
	m_EditAlpha = 0;
	m_GlassAlpha = 0;

	m_CharacterPosition = 0;

	// Theme for SubClass
	m_OffsetX = 0;

	// Voice for SubClass
	m_VoiceVolume = 0;
}

CDialogFx::~CDialogFx()
{
	if(m_hPal) DeleteObject(m_hPal);
}

BEGIN_MESSAGE_MAP(CDialogFx, CDialog)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_DPICHANGED, &CDialogFx::OnDpiChanged)
	ON_MESSAGE(WM_DISPLAYCHANGE, &CDialogFx::OnDisplayChange)
	ON_MESSAGE(WM_SYSCOLORCHANGE, &CDialogFx::OnSysColorChange)
	ON_MESSAGE(WM_SETTINGCHANGE, &CDialogFx::OnSettingChange)
	ON_MESSAGE(WM_ENTERSIZEMOVE, &CDialogFx::OnEnterSizeMove)
	ON_MESSAGE(WM_EXITSIZEMOVE, &CDialogFx::OnExitSizeMove)
END_MESSAGE_MAP()

//------------------------------------------------
// Dialog
//------------------------------------------------

BOOL CDialogFx::Create(UINT nIDTemplate, CWnd* pDlgWnd, UINT menuId, CWnd* pParentWnd)
{
	m_bModelessDlg = TRUE;
	m_ParentWnd = pParentWnd;
	m_DlgWnd = pDlgWnd;
	m_MenuId = menuId;

	if (m_MenuId != 0 && m_ParentWnd != NULL)
	{
		CMenu* menu = m_ParentWnd->GetMenu();
		menu->EnableMenuItem(m_MenuId, MF_GRAYED);
		m_ParentWnd->SetMenu(menu);
		m_ParentWnd->DrawMenuBar();
	}

	return CDialog::Create(nIDTemplate, pParentWnd);
}

int CDialogFx::GetDpi()
{
	INT dpi = 96;
	CDC* pDC = GetDC();
	dpi = GetDeviceCaps(pDC->m_hDC, LOGPIXELSY);
	ReleaseDC(pDC);

	HMODULE hModule = GetModuleHandle(_T("Shcore.dll"));
	if (hModule)
	{
		typedef HRESULT(WINAPI* FuncGetDpiForMonitor) (HMONITOR hmonitor, UINT dpiType, UINT* dpiX, UINT* dpiY);
		typedef HMONITOR(WINAPI* FuncMonitorFromWindow) (HWND hwnd, DWORD dwFlags);
		
		FuncGetDpiForMonitor pGetDpiForMonitor = (FuncGetDpiForMonitor)GetProcAddress(hModule, "GetDpiForMonitor");
		FuncMonitorFromWindow pMonitorFromWindow = (FuncMonitorFromWindow)GetProcAddress(hModule, "MonitorFromWindow");

		if (pGetDpiForMonitor && pMonitorFromWindow)
		{
			UINT dpiX, dpiY;
			pGetDpiForMonitor(pMonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), 0, &dpiX, &dpiY);
			dpi = dpiY;
		}
	}

	return dpi;
}

BOOL CDialogFx::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bHighContrast = IsHighContrast();
	m_Dpi = GetDpi();
	m_hAccelerator = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR));

	// m_bInitializing = FALSE;

	return TRUE;
}

void CDialogFx::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (nType == SIZE_RESTORED)
	{
		UpdateBackground(TRUE, FALSE);
		Invalidate();
	}
}

BOOL CDialogFx::PreTranslateMessage(MSG* pMsg) 
{
	if(m_hAccelerator != NULL)
	{
		if(::TranslateAccelerator(m_hWnd, m_hAccelerator, pMsg) != 0)
		{
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CDialogFx::PostNcDestroy()
{
	if (m_bModelessDlg)
	{
		m_DlgWnd = NULL;
		delete this;
	}
	else
	{
		CDialog::PostNcDestroy();
	}
}

void CDialogFx::UpdateDialogSize()
{
#if _MSC_VER > 1310
	if (! m_bDisableDarkMode)
	{
		m_bDarkMode = SetDarkMode(m_hWnd);
	}
	else
	{
		UnsetDarkMode(m_hWnd);
		m_bDarkMode = FALSE;
	}
#endif
}

void CDialogFx::SetClientSize(int sizeX, int sizeY, double zoomRatio)
{
	RECT rw, rc;
	GetWindowRect(&rw);
	GetClientRect(&rc);

	if (rc.right != 0)
	{
		int ncaWidth = (rw.right - rw.left) - (rc.right - rc.left);
		int ncaHeight = (rw.bottom - rw.top) - (rc.bottom - rc.top);

		SetWindowPos(NULL, 0, 0, (int)(sizeX * zoomRatio) + ncaWidth, (int)(sizeY * zoomRatio) + ncaHeight, SWP_NOMOVE | SWP_NOZORDER);

		GetWindowRect(&rw);
		GetClientRect(&rc);

		int ncaHeightMenu = (rw.bottom - rw.top) - (rc.bottom - rc.top);

		if (ncaHeight != ncaHeightMenu)
		{
			SetWindowPos(NULL, 0, 0, (int)(sizeX * zoomRatio) + ncaWidth, (int)(sizeY * zoomRatio) + ncaHeightMenu, SWP_NOMOVE | SWP_NOZORDER);
		}
	}
}

void CDialogFx::UpdateBackground(BOOL resize, BOOL bDarkMode)
{
	HRESULT hr;
	CImage srcBitmap;
	double ratio = m_ZoomRatio;
	m_bBkImage = FALSE;

#if _MSC_VER > 1310
	if (resize) { m_ZoomRatio = 3.0; }
	hr = srcBitmap.Load(IP(m_BackgroundName));
	if (resize) { m_ZoomRatio = ratio; }
#else
	if (resize) { m_ZoomRatio = 1.0; }
	hr = srcBitmap.Load(IP(m_BackgroundName));
	if (resize) { m_ZoomRatio = ratio; }
#endif

	HDC hScreenDC = ::GetDC(NULL); // get desktop DC
	if(!m_hPal && GetDeviceCaps(hScreenDC, RASTERCAPS) & RC_PALETTE)
	{
		m_hPal = CreateHalftonePalette(hScreenDC);
	}
	DeleteDC(hScreenDC); // delete it after use

	if (SUCCEEDED(hr))
	{
		m_bBkImage = TRUE;
		CBitmap	baseBitmap;
		CDC		baseDC;
		CDC* pWndDC = GetDC();

#if _MSC_VER > 1310
		int w = (int)(m_ZoomRatio / 3.0 * srcBitmap.GetWidth());
		int h = (int)(m_ZoomRatio / 3.0 * srcBitmap.GetHeight());
#else
		int w = (int)(m_ZoomRatio * srcBitmap.GetWidth());
		int h = (int)(m_ZoomRatio * srcBitmap.GetHeight());
#endif
		int orgw = srcBitmap.GetWidth();
		int orgh = srcBitmap.GetHeight();
		if(m_hPal && pWndDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
		{
			SelectPalette( pWndDC->GetSafeHdc(), m_hPal, TRUE );
			pWndDC->RealizePalette();
			pWndDC->SetStretchBltMode(HALFTONE);
		}
		baseBitmap.CreateCompatibleBitmap(pWndDC, orgw, orgh);
		baseDC.CreateCompatibleDC(pWndDC);
		if(m_hPal && baseDC.GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
		{
			SelectPalette( baseDC.GetSafeHdc(), m_hPal, FALSE );
			baseDC.RealizePalette();
			baseDC.SetStretchBltMode(HALFTONE);
		}

		m_BkBitmap.DeleteObject();
		m_BkDC.DeleteDC();
		m_BkBitmap.CreateCompatibleBitmap(pWndDC, w, h);
		m_BkDC.CreateCompatibleDC(pWndDC);
		if(m_hPal && baseDC.GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
		{
			SelectPalette( m_BkDC.GetSafeHdc(), m_hPal, FALSE );
			m_BkDC.RealizePalette();
			m_BkDC.SetStretchBltMode(HALFTONE);
		}

		ReleaseDC(pWndDC);

		baseDC.SelectObject(&baseBitmap);
		m_BkDC.SelectObject(&m_BkBitmap);

		srcBitmap.BitBlt(baseDC.GetSafeHdc(), 0, 0, SRCCOPY);
		srcBitmap.Destroy();

		if(m_hPal || (LOBYTE(LOWORD(GetVersion()))) == 3)
		{
			m_BkDC.StretchBlt(0, 0, w, h, &baseDC, 0, 0,  orgw, orgh, SRCCOPY);
		}
		else
		{
			Bitmap* pBitmap = Bitmap::FromHBITMAP((HBITMAP)baseBitmap.GetSafeHandle(), NULL);
			Graphics	g(m_BkDC.GetSafeHdc());
			g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
			g.DrawImage(pBitmap, 0, 0, w, h);

			delete	pBitmap;
		}

		baseBitmap.DeleteObject();
		baseDC.DeleteDC();

		m_BrushDlg.DeleteObject();
		m_BrushDlg.CreatePatternBrush(&m_BkBitmap);

		return;
	}
	else if (m_bHighContrast)
	{
		m_BrushDlg.DeleteObject();
		m_BrushDlg.CreateSolidBrush(RGB(0, 0, 0));
	}
	else
	{
		CBitmap	baseBitmap;
		CDC		baseDC;
		CDC* pWndDC = GetDC();

		CRect rect;
		GetClientRect(&rect);
		int w = rect.Width();
		int h = rect.Height();

		m_BkBitmap.DeleteObject();
		m_BkBitmap.CreateCompatibleBitmap(pWndDC, w, h);
		m_BkDC.DeleteDC();
		m_BkDC.CreateCompatibleDC(pWndDC);
		m_BkDC.SelectObject(&m_BkBitmap);

		m_BrushDlg.DeleteObject();
		COLORREF bkColor;

		if (m_Background != 0xFFFFFFFF)
		{
			bkColor = m_Background;
		}
		else if (bDarkMode)
		{
			bkColor = RGB(32, 32, 32);
		}
		else
		{
			bkColor = RGB(255, 255, 255);
		}
		m_BrushDlg.CreateSolidBrush(bkColor);

		m_BkDC.FillRect(&rect, &m_BrushDlg);

		ReleaseDC(pWndDC);
	}
}

void CDialogFx::SetWindowTitle(CString title)
{
	SetWindowText(_T(" ") + title + _T(" "));
}

void CDialogFx::OnOK()
{
}

void CDialogFx::OnCancel()
{
	if (m_bModelessDlg)
	{
		if (m_MenuId != 0 && m_ParentWnd != NULL)
		{
			CMenu* menu = m_ParentWnd->GetMenu();
			menu->EnableMenuItem(m_MenuId, MF_ENABLED);
			m_ParentWnd->SetMenu(menu);
			m_ParentWnd->DrawMenuBar();
		}
		CDialog::DestroyWindow();
	}
	else
	{
		CDialog::OnCancel();
	}
}

//------------------------------------------------
// Font
//------------------------------------------------

int CDialogFx::GetFontScale()
{
	return m_FontScale;
}

BYTE CDialogFx::GetFontRender()
{
	return m_FontRender;
}

double CDialogFx::GetFontRatio()
{
	return m_FontRatio;
}

CString CDialogFx::GetFontFace()
{
	return m_FontFace;
}

//------------------------------------------------
// Zoom
//------------------------------------------------

DWORD CDialogFx::ChangeZoomType(DWORD zoomType)
{
	DWORD current = (DWORD)ceil(m_Dpi / 96.0 * 100);
	int width = GetSystemMetrics(SM_CXSCREEN);

	if(zoomType == ZoomTypeAuto)
	{
#if _MSC_VER > 1310
		if (current >= 300)
		{
			zoomType = ZoomType300;
		}
		else if (current >= 250)
		{
			zoomType = ZoomType250;
		}
		else
#endif
		if(current >= 200)
		{
			zoomType = ZoomType200;
		}
		else if(current >= 150)
		{
			zoomType = ZoomType150;
		}
		else if(current >= 125)
		{
			zoomType = ZoomType125;
		}
#ifdef CRYSTALMARK_RETRO
#ifdef SUISHO_SHIZUKU_SUPPORT
		else if (width < 900) { zoomType = ZoomType050; }
		else if (width < 1200){ zoomType = ZoomType075; }
#else
//		else if (width < 732) { zoomType = ZoomType050; }
		else if (width < 732) { zoomType = ZoomType064; }
		else if (width < 976) { zoomType = ZoomType075; }
#endif
#endif
		else
		{
			zoomType = ZoomType100;
		}
	}

	m_ZoomRatio = zoomType / 100.0;

	return zoomType;
}

//------------------------------------------------
// Theme
//------------------------------------------------

BOOL CDialogFx::IsHighContrast()
{
	HIGHCONTRAST hc = { sizeof(HIGHCONTRAST) };
	SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST), &hc, 0);

	return hc.dwFlags & HCF_HIGHCONTRASTON;
}

BOOL CDialogFx::IsDisableDarkMode()
{
	return m_bDisableDarkMode;
}

//------------------------------------------------
// Utility
//------------------------------------------------

CString CDialogFx::IP(CString imageName) /// ImagePath
{
	CString imagePath;
	imagePath.Format(_T("%s%s\\%s-%03d.png"), (LPCTSTR)m_ThemeDir, (LPCTSTR)m_CurrentTheme, (LPCTSTR)imageName, (DWORD)(m_ZoomRatio * 100));
	if (IsFileExist(imagePath))
	{
		return imagePath;
	}
	imagePath.Format(_T("%s%s\\%s-%03d.png"), (LPCTSTR)m_ThemeDir, (LPCTSTR)m_ParentTheme1, (LPCTSTR)imageName, (DWORD)(m_ZoomRatio * 100));
	if (IsFileExist(imagePath))
	{
		return imagePath;
	}
	imagePath.Format(_T("%s%s\\%s-%03d.png"), (LPCTSTR)m_ThemeDir, (LPCTSTR)m_ParentTheme2, (LPCTSTR)imageName, (DWORD)(m_ZoomRatio * 100));
	if (IsFileExist(imagePath))
	{
		return imagePath;
	}
	imagePath.Format(_T("%s%s\\%s-%03d.png"), (LPCTSTR)m_ThemeDir, (LPCTSTR)m_DefaultTheme, (LPCTSTR)imageName, (DWORD)(m_ZoomRatio * 100));
	if (IsFileExist(imagePath))
	{
		return imagePath;
	}

	return _T("");
}

CString CDialogFx::i18n(CString section, CString key, BOOL inEnglish)
{
	TCHAR str[256];
	CString cstr;

	if(inEnglish)
	{
		GetPrivateProfileStringFx(section, key, _T(""), str, 256, m_DefaultLangPath);
		cstr = str;
	}
	else
	{
		GetPrivateProfileStringFx(section, key, _T(""), str, 256, m_CurrentLangPath);
		cstr = str;
		if(cstr.IsEmpty())
		{
			GetPrivateProfileStringFx(section, key, _T(""), str, 256, m_DefaultLangPath);
			cstr = str;
		}
	}

	return cstr;
}

void CDialogFx::OpenUrl(CString url)
{
	INT_PTR result = 0;
	result = (INT_PTR)(ShellExecute(NULL, _T("open"), (LPCTSTR)url, NULL, NULL, SW_SHOWNORMAL));
	if(result <= 32)
	{
		CString args;
		args.Format(_T("url.dll,FileProtocolHandler %s"), (LPCTSTR)url);
		ShellExecute(NULL, _T("open"), _T("rundll32.exe"), args, NULL, SW_SHOWNORMAL);
	}
}

void CDialogFx::SetLayeredWindow(HWND hWnd, BYTE alpha)
{
#if _MSC_VER > 1310
	if (IsWin2k()) { return; }

	::SetWindowLong(hWnd, GWL_EXSTYLE, ::GetWindowLong(hWnd, GWL_EXSTYLE) ^ WS_EX_LAYERED);
	::SetWindowLong(hWnd, GWL_EXSTYLE, ::GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	if (m_bHighContrast)
	{
		::SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
	}
	else
	{
		::SetLayeredWindowAttributes(hWnd, 0, alpha, LWA_ALPHA);
	}
#endif
}

//------------------------------------------------
// MessageMap
//------------------------------------------------

void CDialogFx::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TimerUpdateDialogSizeDpiChanged:
		if (m_bDrag)
		{
			KillTimer(TimerUpdateDialogSizeDpiChanged);
			SetTimer(TimerUpdateDialogSizeDpiChanged, TIMER_UPDATE_DIALOG, NULL);
		}
		else
		{
			m_bDpiChanging = FALSE;
			KillTimer(TimerUpdateDialogSizeDpiChanged);
			UpdateDialogSize();
		}
		break;
	case TimerUpdateDialogSizeDisplayChange:
		KillTimer(TimerUpdateDialogSizeDisplayChange);
		UpdateDialogSize();
		break;
	case TimerUpdateDialogSizeSysColorChange:
		KillTimer(TimerUpdateDialogSizeSysColorChange);
		UpdateDialogSize();
		break;
	case TimerUpdateDialogSizeSettingChange:
		KillTimer(TimerUpdateDialogSizeSettingChange);
		UpdateDialogSize();
		break;
	}
}

BOOL CDialogFx::OnEraseBkgnd(CDC* pDC)
{
	if (m_bHighContrast)
	{
		return CDialog::OnEraseBkgnd(pDC);
	}

	if(m_hPal && pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
	{
		SelectPalette( pDC->GetSafeHdc(), m_hPal, TRUE );
		pDC->RealizePalette();
		pDC->SetStretchBltMode(HALFTONE);
	}

	CRect rect;
	GetClientRect(&rect);
		
	return pDC->StretchBlt(0, 0, rect.Width(), rect.Height(), &m_BkDC, 0, 0, rect.Width(), rect.Height(), SRCCOPY);
}

afx_msg LRESULT CDialogFx::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	if (m_bInitializing) { return 0; }

	static ULONGLONG preTime = 0;
	ULONGLONG currentTime = GetTickCountFx();
	if (currentTime - preTime < 1000)
	{
		return 0;
	}
	else
	{
		preTime = currentTime;
	}

	m_Dpi = (INT)HIWORD(wParam);

#if _MSC_VER > 1310
	if (IsWindowsBuildOrGreater(16299)) // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
	{
		ChangeZoomType(m_ZoomType);
		m_bDpiChanging = TRUE;
		SetTimer(TimerUpdateDialogSizeDpiChanged, TIMER_UPDATE_DIALOG, NULL);
	}
	else 
#endif	
	if(m_ZoomType == ZoomTypeAuto)
	{
		DWORD oldZoomRatio = (DWORD)(m_ZoomRatio * 100);
		if (ChangeZoomType(m_ZoomType) != oldZoomRatio)
		{
			m_bDpiChanging = TRUE;
			SetTimer(TimerUpdateDialogSizeDpiChanged, TIMER_UPDATE_DIALOG, NULL);
		}
	}

	return 0;
}

afx_msg LRESULT CDialogFx::OnDisplayChange(WPARAM wParam, LPARAM lParam)
{
	if (m_bInitializing) { return 0; }

	CDC* cdc = GetDC();
	if (cdc)
	{
		int color = cdc->GetDeviceCaps(BITSPIXEL) * cdc->GetDeviceCaps(PLANES);
		if (color != wParam)
		{
			SetTimer(TimerUpdateDialogSizeDisplayChange, TIMER_UPDATE_DIALOG, NULL);
		}
		ReleaseDC(cdc);
	}

	return 0;
}

afx_msg LRESULT CDialogFx::OnSysColorChange(WPARAM wParam, LPARAM lParam)
{
	if (m_bInitializing) { return 0; }

	m_bHighContrast = IsHighContrast();

	SetTimer(TimerUpdateDialogSizeSysColorChange, TIMER_UPDATE_DIALOG, NULL);

	return 0;
}

afx_msg LRESULT CDialogFx::OnSettingChange(WPARAM wParam, LPARAM lParam)
{
	if (m_bInitializing) { return 0; }

	if (!lstrcmp(LPCTSTR(lParam), _T("ImmersiveColorSet")))
	{
		SetTimer(TimerUpdateDialogSizeSettingChange, TIMER_UPDATE_DIALOG, NULL);
	}

	return 0;
}

afx_msg LRESULT CDialogFx::OnEnterSizeMove(WPARAM wParam, LPARAM lParam)
{
	m_bDrag = TRUE;

	return TRUE;
}

afx_msg LRESULT CDialogFx::OnExitSizeMove(WPARAM wParam, LPARAM lParam)
{
	m_bDrag = FALSE;

	return TRUE;
}