/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "resource.h"
#include "DialogCx.h"
#include "GetOsInfo.h"
#include <Shlwapi.h>
#include "GetFileVersion.h"

#include <strsafe.h>

using namespace Gdiplus;
#pragma	comment(lib,"Gdiplus.lib")

// defined by Windows 8.1/Windows 2012 R2
#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

typedef HRESULT(WINAPI *FuncGetDpiForMonitor) (HMONITOR hmonitor, UINT dpiType, UINT *dpiX, UINT *dpiY);

CDialogCx::CDialogCx(UINT dlgResouce, CWnd* pParent)
				:CDialogEx(dlgResouce, pParent)
{
	DebugPrint(L"CDialogCx::CDialogCx");

	m_FlagShowWindow = FALSE;
	m_FlagModelessDlg = FALSE;
	m_ParentWnd = NULL;
	m_DlgWnd = NULL;
	m_MenuId = 0;
	m_FontType = FT_GDI;

	m_BackgroundName = L"background";

	m_ZoomRatio = 1.0;
	m_ZoomType = ZOOM_TYPE_AUTO;
}

CDialogCx::~CDialogCx()
{
}

void CDialogCx::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CDialogCx::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_IsHighContrast = IsHighContrast();
	m_IsDrawFrame = FALSE;
	CDC *pDC = GetDC();
	m_Dpi = GetDeviceCaps(pDC->m_hDC, LOGPIXELSY);
	ReleaseDC(pDC);

	HMODULE hModule = GetModuleHandle(_T("Shcore.dll"));
	if (hModule != NULL)
	{
		FuncGetDpiForMonitor pGetDpiForMonitor = (FuncGetDpiForMonitor)GetProcAddress(hModule, "GetDpiForMonitor");
		if (pGetDpiForMonitor != NULL)
		{
			UINT dpiX, dpiY;
			pGetDpiForMonitor(MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), 0, &dpiX, &dpiY);
			m_Dpi = dpiY;
		}
	}

	m_hAccelerator = ::LoadAccelerators(AfxGetInstanceHandle(),
		                                MAKEINTRESOURCE(IDR_ACCELERATOR));
	return TRUE;
}

BEGIN_MESSAGE_MAP(CDialogCx, CDialog)
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_DPICHANGED, &CDialogCx::OnDpiChanged)
	ON_MESSAGE(WM_SYSCOLORCHANGE, &CDialogCx::OnSysColorChange)
	ON_MESSAGE(WM_DISPLAYCHANGE, &CDialogCx::OnDisplayChange)
END_MESSAGE_MAP()

BOOL CDialogCx::PreTranslateMessage(MSG* pMsg) 
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

double CDialogCx::GetZoomRatio()
{
	return m_ZoomRatio;
}

DWORD CDialogCx::ChangeZoomType(DWORD zoomType)
{
	DWORD current = (DWORD)(m_Dpi / 96.0 * 100);

	if(zoomType == ZOOM_TYPE_AUTO)
	{
		if(current >= 300)
		{
			zoomType = ZOOM_TYPE_300;
		}
		else if (current >= 250)
		{
			zoomType = ZOOM_TYPE_250;
		}
		else if(current >= 200)
		{
			zoomType = ZOOM_TYPE_200;
		}
		else if(current >= 150)
		{
			zoomType = ZOOM_TYPE_150;
		}
		else if(current >= 125)
		{
			zoomType = ZOOM_TYPE_125;
		}
		else
		{
			zoomType = ZOOM_TYPE_100;
		}
	}

	m_ZoomRatio = zoomType / 100.0;

	return zoomType;
}

void CDialogCx::SetClientRect(DWORD sizeX, DWORD sizeY, DWORD menuLine)
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
	X = currentRc.left - (clientRc.Width() - sizeX) / 2;
	Y = currentRc.top;

	if(clientRc.Height() == sizeY && clientRc.Width() == sizeX)
	{
		return;
	}

	WINDOWINFO wi = {0};
	wi.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(&wi);

// 0x94CE004C

	AdjustWindowRect(&rc, wi.dwStyle, TRUE);
	SetWindowPos(&CWnd::wndTop, X, Y,
		rc.Width(), rc.Height() + GetSystemMetrics(SM_CYMENU) * menuLine, SWP_NOMOVE);

	GetClientRect(&clientRc);
	if(clientRc.Height() != sizeY)
	{
		SetWindowPos(&CWnd::wndTop, X, Y, rc.Width(),
			rc.Height() + GetSystemMetrics(SM_CYMENU) * menuLine + sizeY - clientRc.Height(), SWP_NOMOVE);
	}
}

BOOL CDialogCx::Create(UINT nIDTemplate, CWnd* pDlgWnd, UINT menuId, CWnd* pParentWnd)
{
	m_FlagModelessDlg = TRUE;
	m_ParentWnd = pParentWnd;
	m_DlgWnd = pDlgWnd;
	m_MenuId = menuId;

	if(m_MenuId != 0 && m_ParentWnd != NULL)
	{
		CMenu *menu = m_ParentWnd->GetMenu();
		menu->EnableMenuItem(m_MenuId, MF_GRAYED);
		m_ParentWnd->SetMenu(menu);
		m_ParentWnd->DrawMenuBar();
	}

	return CDialog::Create(nIDTemplate, pParentWnd);
}

void CDialogCx::OnCancel() 
{
	if(m_FlagModelessDlg)
	{
		if(m_MenuId != 0 && m_ParentWnd != NULL)
		{
			CMenu *menu = m_ParentWnd->GetMenu();
			menu->EnableMenuItem(m_MenuId, MF_ENABLED);
			m_ParentWnd->SetMenu(menu);
			m_ParentWnd->DrawMenuBar();
		}
		CWnd::DestroyWindow();
	}
	else
	{
		CDialog::OnCancel();
	}
}

void CDialogCx::PostNcDestroy()
{
	if(m_FlagModelessDlg)
	{
		m_DlgWnd = NULL;
		delete this;
	}
}

void CDialogCx::OnOK()
{
}

void CDialogCx::ShowWindowEx(int nCmdShow)
{
	m_FlagShowWindow = TRUE;
	ShowWindow(nCmdShow);
	SetForegroundWindow();
}

CString CDialogCx::i18n(CString section, CString key, BOOL inEnglish)
{
	TCHAR str[256];
	CString cstr;

	if(inEnglish)
	{
		GetPrivateProfileString(section, key, _T(""), str, 256, m_DefaultLangPath);
		cstr = str;
	}
	else
	{
		GetPrivateProfileString(section, key, _T(""), str, 256, m_CurrentLangPath);
		cstr = str;
		if(cstr.IsEmpty())
		{
			GetPrivateProfileString(section, key, _T(""), str, 256, m_DefaultLangPath);
			cstr = str;
		}
	}

	return cstr;
}

// 2012/5/6
// http://msdn.microsoft.com/ja-jp/magazine/cc163834(en-us).aspx
void CDialogCx::OpenUrl(CString url)
{
	INT_PTR result = 0;
	result = (INT_PTR)(ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_SHOWNORMAL));
	if(result <= 32)
	{
		result = (INT_PTR)(ShellExecuteW(NULL, _T("open"), _T("iexplore.exe"), url, NULL, SW_SHOWNORMAL));
		if(result <= 32)
		{
			CString args;
			args.Format(_T("url.dll,FileProtocolHandler %s"), url.GetString());
			ShellExecuteW(NULL, _T("open"), _T("rundll32.exe"), args, NULL, SW_SHOWNORMAL);
		}
	}
}

void CDialogCx::SetupControl(int nIDDlgItem , int x, int y, int width, int height)
{
	GetDlgItem(nIDDlgItem)->MoveWindow(
		(int)(x * m_ZoomRatio),
		(int)(y * m_ZoomRatio),
		(int)(width * m_ZoomRatio),
		(int)(height * m_ZoomRatio)
		);
}

BOOL CDialogCx::IsHighContrast()
{
	HIGHCONTRAST hc;
	hc.cbSize = sizeof(HIGHCONTRAST);
	SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST), &hc, 0);
	return hc.dwFlags & HCF_HIGHCONTRASTON;
}

BOOL CDialogCx::IsDrawFrame()
{
	BOOL isDrawFrame = FALSE;
	CDC *pDC = GetDC();
	if (m_IsHighContrast || GetDeviceCaps(pDC->m_hDC, BITSPIXEL) * GetDeviceCaps(pDC->m_hDC, PLANES) < 24)
	{
		isDrawFrame = TRUE;
	}
	ReleaseDC(pDC);
	
	return isDrawFrame;
}

afx_msg LRESULT CDialogCx::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	static DWORD preTime = 0;
	DWORD currentTime = GetTickCount();
	if (currentTime - preTime < 1000)
	{
		return 0;
	}
	else
	{
		preTime = currentTime;
	}


	m_Dpi = (INT)HIWORD(wParam);
	if(m_ZoomType == ZOOM_TYPE_AUTO)
	{
		DWORD oldZoomRatio = (DWORD)(m_ZoomRatio * 100);
		if (ChangeZoomType(m_ZoomType) != oldZoomRatio)
		{
			UpdateDialogSize();
		}
	}

	return 0;
}

afx_msg LRESULT CDialogCx::OnSysColorChange(WPARAM wParam, LPARAM lParam)
{
	m_IsHighContrast = IsHighContrast();
	UpdateDialogSize();
	return 0;
}

afx_msg LRESULT CDialogCx::OnDisplayChange(WPARAM wParam, LPARAM lParam)
{
	UpdateDialogSize();
	return 0;
}

void CDialogCx::UpdateDialogSize()
{

}

void CDialogCx::UpdateBackground(bool resize)
{
	HRESULT hr;
	BOOL    br = FALSE;
	CImage srcBitmap;
	double ratio = m_ZoomRatio;

	if (resize) m_ZoomRatio = 3.0;

	hr = srcBitmap.Load(IP(m_BackgroundName));

	if (resize) m_ZoomRatio = ratio;

	if(SUCCEEDED(hr))
	{
		CBitmap	baseBitmap;
		CDC		baseDC;
		CDC*	pWndDC = GetDC();

		int w = (int)(m_ZoomRatio / 3.0 * srcBitmap.GetWidth());
		int h = (int)(m_ZoomRatio / 3.0 * srcBitmap.GetHeight());

		baseBitmap.CreateCompatibleBitmap(pWndDC, srcBitmap.GetWidth(), srcBitmap.GetHeight());
		baseDC.CreateCompatibleDC(pWndDC);
		
		m_BitmapBg.DeleteObject();
		m_BgDC.DeleteDC();
		m_BitmapBg.CreateCompatibleBitmap(pWndDC, w, h);
		m_BgDC.CreateCompatibleDC(pWndDC);

		ReleaseDC(pWndDC);

		baseDC.SelectObject(&baseBitmap);
		m_BgDC.SelectObject(&m_BitmapBg);

		srcBitmap.BitBlt(baseDC.GetSafeHdc(), 0, 0, SRCCOPY);
		srcBitmap.Destroy();

		Bitmap*	pBitmap = Bitmap::FromHBITMAP((HBITMAP)baseBitmap.GetSafeHandle(), NULL);
		Graphics	g(m_BgDC.GetSafeHdc());
		g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
		g.DrawImage(pBitmap, 0, 0, w, h);

		delete	pBitmap;
		baseBitmap.DeleteObject();
		baseDC.DeleteDC();

		m_BrushDlg.DeleteObject();
		m_BrushDlg.CreatePatternBrush(&m_BitmapBg);

		return;
	}
	
	m_BrushDlg.DeleteObject();
	m_BrushDlg.CreateHatchBrush(HS_BDIAGONAL, RGB(0xF8, 0xF8, 0xF8));
}

CString CDialogCx::IP(CString imageName)
{
	CString imagePath;
	imagePath.Format(L"%s%s\\%s-%3d.png", m_CxThemeDir.GetString(), m_CxCurrentTheme.GetString(), imageName.GetString(), (DWORD)(m_ZoomRatio * 100));
	if(IsFileExist(imagePath))
	{
		return imagePath;
	}
	imagePath.Format(L"%s%s\\%s-%3d.png", m_CxThemeDir.GetString(), m_CxDefaultTheme.GetString(), imageName.GetString(), (DWORD)(m_ZoomRatio * 100));
	if(IsFileExist(imagePath))
	{
		return imagePath;
	}
	return L"";	
}

HBRUSH CDialogCx::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	CBrush nullb;

	switch(nCtlColor){
	case CTLCOLOR_STATIC:
		if (!m_IsHighContrast)
		{
			nullb.CreateStockObject(NULL_BRUSH);
			pDC->SetBkMode(TRANSPARENT);
			return nullb;
		}
		break;
	case CTLCOLOR_BTN:	//	■透過ボタンに必要な処理。
		if (!m_IsHighContrast)
		{
			nullb.CreateStockObject(NULL_BRUSH);
			pDC->SetBkMode(TRANSPARENT);
			return nullb;
		}
		break;
	case CTLCOLOR_DLG:
		if (!m_IsHighContrast)
		{	
			pDC->SetBkColor(RGB(255, 255, 255));
			return (HBRUSH) m_BrushDlg;
		}
		break;
	default:
		break;
	}

	// TODO:  既定値を使用したくない場合は別のブラシを返します。
	return hbr;
}

/*
typedef BOOL(WINAPI *FuncEnableNonClientDpiScaling) (HWND hwnd);

BOOL CDialogCx::OnNcCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (!CDialogEx::OnNcCreate(lpCreateStruct))
		return FALSE;

	FuncEnableNonClientDpiScaling pEnableNonClientDpiScaling = (FuncEnableNonClientDpiScaling)GetProcAddress(GetModuleHandle(_T("User32.dll")), "EnableNonClientDpiScaling");

	if (pEnableNonClientDpiScaling != NULL)
	{
		pEnableNonClientDpiScaling(m_hWnd);
	}
	return TRUE;
}
*/