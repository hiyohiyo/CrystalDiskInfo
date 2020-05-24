/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "../stdafx.h"
#include "../Resource.h"
#include "DialogFx.h"
#include "UtilityFx.h"
#include "OsInfoFx.h"

#include <Shlwapi.h>
#include <strsafe.h>

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
	m_bBkImage = FALSE;
	m_MenuId = 0;
	m_ParentWnd = NULL;
	m_DlgWnd = NULL;
	m_hAccelerator = 0;
	m_bDrag = FALSE;
	m_FontScale = 100;
	m_FontRatio = 1.0;

	m_MaxSizeX = 65535;
	m_MinSizeX = 0;
	m_MaxSizeY = 65535;
	m_MinSizeY = 0;

	// Zoom
	m_Dpi = 96;
	m_ZoomRatio = 1.0;
	m_ZoomType = ZoomTypeAuto;
}

CDialogFx::~CDialogFx()
{
}

BEGIN_MESSAGE_MAP(CDialogFx, CDialog)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
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

BOOL CDialogFx::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bHighContrast = IsHighContrast();
	CDC *pDC = GetDC();
	m_Dpi = GetDeviceCaps(pDC->m_hDC, LOGPIXELSY);
	ReleaseDC(pDC);

	HMODULE hModule = GetModuleHandle(L"Shcore.dll");
	if (hModule != NULL)
	{
		typedef HRESULT(WINAPI* FuncGetDpiForMonitor) (HMONITOR hmonitor, UINT dpiType, UINT* dpiX, UINT* dpiY);
		FuncGetDpiForMonitor pGetDpiForMonitor = (FuncGetDpiForMonitor)GetProcAddress(hModule, "GetDpiForMonitor");
		if (pGetDpiForMonitor != NULL)
		{
			UINT dpiX, dpiY;
			pGetDpiForMonitor(MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), 0, &dpiX, &dpiY);
			m_Dpi = dpiY;
		}
	}

	m_hAccelerator = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR));

	m_bInitializing = FALSE;

	return TRUE;
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
	m_bDarkMode = SetDarkMode(m_hWnd);
}

void CDialogFx::SetClientSize(int sizeX, int sizeY, DWORD menuLine)
{
	CRect rc;
	CRect clientRc;
	CRect currentRc;
	rc.left = 0;
	rc.top = 0;
	rc.right = sizeX;
	rc.bottom = sizeY;
	int X = 0, Y = 0;

	GetWindowRect(&currentRc);
	GetClientRect(&clientRc);
	X = currentRc.left;
	Y = currentRc.top;

	if (clientRc.Height() == sizeY && clientRc.Width() == sizeX)
	{
		return;
	}

	rc.right = sizeX;
	rc.bottom = sizeY;
	SetWindowPos(&CWnd::wndTop, X, Y, rc.right, rc.bottom, SWP_NOMOVE);
	GetClientRect(&clientRc);

	rc.right += sizeX - clientRc.Width();
	rc.bottom += sizeY - clientRc.Height();

	SetWindowPos(&CWnd::wndTop, X, Y, rc.right, rc.bottom, SWP_NOMOVE);
	GetClientRect(&clientRc);
}

void CDialogFx::UpdateBackground(BOOL resize, BOOL bDarkMode)
{
	HRESULT hr;
	BOOL    br = FALSE;
	CImage srcBitmap;
	double ratio = m_ZoomRatio;
	m_bBkImage = FALSE;

	if (resize) { m_ZoomRatio = 3.0; }

	hr = srcBitmap.Load(IP(m_BackgroundName));

	if (resize) { m_ZoomRatio = ratio; }

	if (SUCCEEDED(hr))
	{
		m_bBkImage = TRUE;
		CBitmap	baseBitmap;
		CDC		baseDC;
		CDC* pWndDC = GetDC();

		int w = (int)(m_ZoomRatio / 3.0 * srcBitmap.GetWidth());
		int h = (int)(m_ZoomRatio / 3.0 * srcBitmap.GetHeight());

		baseBitmap.CreateCompatibleBitmap(pWndDC, srcBitmap.GetWidth(), srcBitmap.GetHeight());
		baseDC.CreateCompatibleDC(pWndDC);

		m_BkBitmap.DeleteObject();
		m_BkDC.DeleteDC();
		m_BkBitmap.CreateCompatibleBitmap(pWndDC, w, h);
		m_BkDC.CreateCompatibleDC(pWndDC);

		ReleaseDC(pWndDC);

		baseDC.SelectObject(&baseBitmap);
		m_BkDC.SelectObject(&m_BkBitmap);

		srcBitmap.BitBlt(baseDC.GetSafeHdc(), 0, 0, SRCCOPY);
		srcBitmap.Destroy();

		Bitmap* pBitmap = Bitmap::FromHBITMAP((HBITMAP)baseBitmap.GetSafeHandle(), NULL);
		Graphics	g(m_BkDC.GetSafeHdc());
		g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
		g.DrawImage(pBitmap, 0, 0, w, h);

		delete	pBitmap;
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
		if (bDarkMode)
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
// Zoom
//------------------------------------------------

DWORD CDialogFx::ChangeZoomType(DWORD zoomType)
{
	DWORD current = (DWORD)(m_Dpi / 96.0 * 100);

	if(zoomType == ZoomTypeAuto)
	{
		if (current >= 300)
		{
			zoomType = ZoomType300;
		}
		else if (current >= 250)
		{
			zoomType = ZoomType250;
		}
		else if(current >= 200)
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
	HIGHCONTRAST hc;
	hc.cbSize = sizeof(HIGHCONTRAST);
	SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST), &hc, 0);

	return hc.dwFlags & HCF_HIGHCONTRASTON;
}

//------------------------------------------------
// Utility
//------------------------------------------------

CString CDialogFx::IP(CString imageName) /// ImagePath
{
	CString imagePath;
	imagePath.Format(L"%s%s\\%s-%03d.png", m_ThemeDir.GetString(), m_CurrentTheme.GetString(), imageName.GetString(), (DWORD)(m_ZoomRatio * 100));
	if (IsFileExist(imagePath))
	{
		return imagePath;
	}
	imagePath.Format(L"%s%s\\%s-%03d.png", m_ThemeDir.GetString(), m_ParentTheme1.GetString(), imageName.GetString(), (DWORD)(m_ZoomRatio * 100));
	if (IsFileExist(imagePath))
	{
		return imagePath;
	}
	imagePath.Format(L"%s%s\\%s-%03d.png", m_ThemeDir.GetString(), m_ParentTheme2.GetString(), imageName.GetString(), (DWORD)(m_ZoomRatio * 100));
	if (IsFileExist(imagePath))
	{
		return imagePath;
	}
	imagePath.Format(L"%s%s\\%s-%03d.png", m_ThemeDir.GetString(), m_DefaultTheme.GetString(), imageName.GetString(), (DWORD)(m_ZoomRatio * 100));
	if (IsFileExist(imagePath))
	{
		return imagePath;
	}

	return L"";
}

CString CDialogFx::i18n(CString section, CString key, BOOL inEnglish)
{
	TCHAR str[256];
	CString cstr;

	if(inEnglish)
	{
		GetPrivateProfileString(section, key, L"", str, 256, m_DefaultLangPath);
		cstr = str;
	}
	else
	{
		GetPrivateProfileString(section, key, L"", str, 256, m_CurrentLangPath);
		cstr = str;
		if(cstr.IsEmpty())
		{
			GetPrivateProfileString(section, key, L"", str, 256, m_DefaultLangPath);
			cstr = str;
		}
	}

	return cstr;
}

void CDialogFx::OpenUrl(CString url)
{
	INT_PTR result = 0;
	result = (INT_PTR)(ShellExecute(NULL, L"open", url, NULL, NULL, SW_SHOWNORMAL));
	if(result <= 32)
	{
		CString args;
		args.Format(L"url.dll,FileProtocolHandler %s", url.GetString());
		ShellExecuteW(NULL, L"open", L"rundll32.exe", args, NULL, SW_SHOWNORMAL);
	}
}

void CDialogFx::SetLayeredWindow(HWND hWnd, BYTE alpha)
{
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

HBRUSH CDialogFx::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	switch (nCtlColor) {
	case CTLCOLOR_DLG:
		if (m_bHighContrast && !m_bBkImage)
		{
			return hbr;
		}
		else
		{
			return (HBRUSH)m_BrushDlg;
		}
		break;
	default:
		break;
	}

	return hbr;
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

	if (GetWin10Version() >= 1709) // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
	{
		ChangeZoomType(m_ZoomType);
		m_bDpiChanging = TRUE;
		SetTimer(TimerUpdateDialogSizeDpiChanged, TIMER_UPDATE_DIALOG, NULL);
	}
	else if(m_ZoomType == ZoomTypeAuto)
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

	SetTimer(TimerUpdateDialogSizeDisplayChange, TIMER_UPDATE_DIALOG, NULL);

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

	if (!lstrcmp(LPCTSTR(lParam), L"ImmersiveColorSet")) {
		//アプリモードが切り替わった。
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