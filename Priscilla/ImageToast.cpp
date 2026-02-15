#include "stdafx.h" // または stdafx.h
#include "ImageToast.h"
#include <shellscalingapi.h>

#ifndef CMK_MIN
#define CMK_MIN(a,b) (( (a) < (b) ) ? (a) : (b))
#endif
#ifndef CMK_MAX
#define CMK_MAX(a,b) (( (a) > (b) ) ? (a) : (b))
#endif

using namespace Gdiplus;

static const UINT IDT_CLOSE = 1;
static const UINT IDT_FADE  = 2;

BEGIN_MESSAGE_MAP(CImageToast, CWnd)
    ON_WM_CREATE()
    ON_WM_TIMER()
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_ERASEBKGND()
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

CImageToast::CImageToast() {}
CImageToast::~CImageToast() {
    if (m_hDib) { ::DeleteObject(m_hDib); m_hDib = nullptr; }
}

int CImageToast::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    return 0;
}

BOOL CImageToast::OnEraseBkgnd(CDC* /*pDC*/) { return TRUE; }

void CImageToast::OnLButtonDown(UINT, CPoint)
{
    OpenUrlIfAny();   // ★ 左クリックでURLを開く
    BeginClose(FALSE);
}

void CImageToast::OnRButtonDown(UINT, CPoint)
{
    BeginClose(FALSE); // 右クリックは閉じるだけ
}

void CImageToast::OnKeyDown(UINT, UINT, UINT)
{
    BeginClose(FALSE);
}

BOOL CImageToast::EnsureWindowCreated()
{
    if (m_hWnd && ::IsWindow(m_hWnd)) return TRUE;

    CString cls = AfxRegisterWndClass(0, ::LoadCursor(nullptr, IDC_HAND), 0, 0);
    DWORD ex = WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED;
    DWORD st = WS_POPUP;

    if (!CreateEx(ex, cls, L"", st, CRect(0,0,0,0), nullptr, 0))
        return FALSE;

    ShowWindow(SW_SHOWNOACTIVATE);
    return TRUE;
}

static BOOL CreateARGBDIB(HDC hdc, int w, int h, HBITMAP& outBmp, void** outBits)
{
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h; // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP hbmp = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!hbmp || !bits) return FALSE;
    outBmp = hbmp; *outBits = bits; return TRUE;
}

BOOL CImageToast::LoadPngToDIB(LPCWSTR path)
{
    // ここでは GDI+ が既に初期化されている前提
    Gdiplus::Bitmap bmp(path);
    if (bmp.GetLastStatus() != Gdiplus::Ok) return FALSE;

    const int w = (int)bmp.GetWidth();
    const int h = (int)bmp.GetHeight();
    if (w <= 0 || h <= 0) return FALSE;

    HDC hdcScreen = ::GetDC(nullptr);

    HBITMAP hbmp = nullptr;
    void* bits = nullptr;
    if (!CreateARGBDIB(hdcScreen, w, h, hbmp, &bits)) {
        ::ReleaseDC(nullptr, hdcScreen);
        return FALSE;
    }

    // hbmp を選択したメモリDCを作り、そこから Graphics を生成する
    HDC hdcMem = ::CreateCompatibleDC(hdcScreen);
    HGDIOBJ oldBmp = ::SelectObject(hdcMem, hbmp);

    {
        Gdiplus::Graphics g(hdcMem);
        g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy); // アルファをそのままコピー
        // 透明で初期化（任意：PNGより小さい描画などの保険）
        Gdiplus::SolidBrush clearBrush(Gdiplus::Color(0, 0, 0, 0));
        g.FillRectangle(&clearBrush, 0, 0, w, h);
        // PNGを転送
        g.DrawImage(&bmp, 0, 0, w, h);
    }

    ::SelectObject(hdcMem, oldBmp);
    ::DeleteDC(hdcMem);
    ::ReleaseDC(nullptr, hdcScreen);

    if (m_hDib) ::DeleteObject(m_hDib);
    m_hDib = hbmp;
    m_bmpSize.cx = w; m_bmpSize.cy = h;
    return TRUE;
}

void CImageToast::UpdateLayered()
{
    if (!m_hWnd || !::IsWindow(m_hWnd) || !m_hDib) return;

    HDC hdcScreen = ::GetDC(nullptr);
    HDC hdcMem = ::CreateCompatibleDC(hdcScreen);
    HBITMAP hOld = (HBITMAP)::SelectObject(hdcMem, m_hDib);

    POINT ptSrc{ 0,0 };
    SIZE  sz{ m_bmpSize.cx, m_bmpSize.cy };
    BLENDFUNCTION bf{};
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = m_curAlpha;
    bf.AlphaFormat = AC_SRC_ALPHA;

    POINT cursor{}; ::GetCursorPos(&cursor);
    HMONITOR mon = ::MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{ sizeof(mi) }; ::GetMonitorInfo(mon, &mi);
    RECT wa = mi.rcWork;

    POINT ptDst{};
    ptDst.x = wa.right  - sz.cx - m_margin;
    ptDst.y = wa.bottom - sz.cy - m_margin;

    ::UpdateLayeredWindow(m_hWnd, hdcScreen, &ptDst, &sz, hdcMem, &ptSrc, 0, &bf, ULW_ALPHA);

    ::SelectObject(hdcMem, hOld);
    ::DeleteDC(hdcMem);
    ::ReleaseDC(nullptr, hdcScreen);
}

void CImageToast::StartFadeTimer(BOOL fadeIn)
{
    if (!m_enableFade) return;
    m_closing = !fadeIn;
    SetTimer(IDT_FADE, 16, nullptr);
}

void CImageToast::BeginClose(BOOL force)
{
    if (!m_hWnd) return;

    if (force || !m_enableFade) {
        KillTimer(IDT_FADE);
        KillTimer(IDT_CLOSE);
        DestroyWindow();
        return;
    }
    if (!m_closing) {
        m_closing = TRUE;
        SetTimer(IDT_FADE, 16, nullptr);
    }
}

void CImageToast::OnTimer(UINT_PTR id)
{
    if (id == IDT_CLOSE) {
        KillTimer(IDT_CLOSE);
        BeginClose(FALSE);
        return;
    }
    if (id == IDT_FADE) {
        int stepIn = CMK_MAX(1, (int)m_maxAlpha * 16 / CMK_MAX(1, m_fadeInMs));
        int stepOut = CMK_MAX(1, (int)m_maxAlpha * 16 / CMK_MAX(1, m_fadeOutMs));

        if (!m_closing) {
            if (m_curAlpha < m_maxAlpha) {
                m_curAlpha = (BYTE)CMK_MIN((int)m_maxAlpha, (int)m_curAlpha + stepIn);
                UpdateLayered();
            }
            else {
                KillTimer(IDT_FADE);
            }
        }
        else {
            if (m_curAlpha > 0) {
                m_curAlpha = (BYTE)CMK_MAX(0, (int)m_curAlpha - stepOut);
                UpdateLayered();
            }
            else {
                KillTimer(IDT_FADE);
                DestroyWindow();
            }
        }
        return;
    }
    CWnd::OnTimer(id);
}

BOOL CImageToast::Show(LPCWSTR pngPath, UINT showMillis, BOOL enableFade, BYTE maxAlpha,
                       int margin, int fadeInMs, int fadeOutMs, LPCWSTR urlToOpen)
{
    m_pngPath    = pngPath;
    m_showMillis = showMillis;
    m_enableFade = enableFade;
    m_maxAlpha   = maxAlpha;
    m_margin     = margin;
    m_fadeInMs   = fadeInMs;
    m_fadeOutMs  = fadeOutMs;
    m_curAlpha   = enableFade ? 0 : maxAlpha;
    m_closing    = FALSE;
    m_opened     = FALSE;
    m_url        = (urlToOpen ? urlToOpen : L""); // ★

    if (!EnsureWindowCreated()) return FALSE;
    if (!LoadPngToDIB(m_pngPath)) {
        BeginClose(TRUE);
        return FALSE;
    }

    UpdateLayered();
    ShowWindow(SW_SHOWNOACTIVATE);

    SetTimer(IDT_CLOSE, m_showMillis, nullptr);
    if (m_enableFade) StartFadeTimer(TRUE);

    return TRUE;
}

void CImageToast::SetLink(LPCWSTR urlToOpen)
{
    m_url = (urlToOpen ? urlToOpen : L"");
}

void CImageToast::CloseNow()
{
    BeginClose(FALSE);
}

// ★ URL起動本体（多重起動防止）
void CImageToast::OpenUrlIfAny()
{
    if (m_opened) return;
    if (m_url.IsEmpty()) return;

    m_opened = TRUE;

    // 既定ブラウザで開く（http/https/file/mailto 等に対応）
    // 戻り値 >32 で成功。失敗時でも UI は閉じます。
    ::ShellExecuteW(nullptr, L"open", m_url, nullptr, nullptr, SW_SHOWNORMAL);
}
