#pragma once
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Shcore.lib") // 任意: DPI

class CImageToast : public CWnd
{
public:
    CImageToast();
    virtual ~CImageToast();

    // pngPath: 表示するPNG
    // urlToOpen: 左クリック時に開くURL（nullptr か空なら何もしない）
    BOOL Show(LPCWSTR pngPath, UINT showMillis = 30000,
              BOOL enableFade = TRUE, BYTE maxAlpha = 255,
              int margin = 16, int fadeInMs = 200, int fadeOutMs = 250,
              LPCWSTR urlToOpen = nullptr);

    // 後からURLだけ差し替える場合（表示前/後どちらでもOK）
    void SetLink(LPCWSTR urlToOpen);

    // 直ちに閉じる（フェード設定があればフェードアウト）
    void CloseNow();

protected:
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    DECLARE_MESSAGE_MAP()

    BOOL EnsureWindowCreated();
    BOOL LoadPngToDIB(LPCWSTR path);
    void UpdateLayered();
    void BeginClose(BOOL force = FALSE);
    void StartFadeTimer(BOOL fadeIn);

    void OpenUrlIfAny(); // ★ 追加

private:
    // 設定
    CString m_pngPath;
    UINT  m_showMillis{ 30000 };
    BOOL  m_enableFade{ TRUE };
    BYTE  m_maxAlpha{ 255 };
    int   m_margin{ 16 };
    int   m_fadeInMs{ 200 };
    int   m_fadeOutMs{ 250 };
    CString m_url;          // ★ 左クリックで開くURL

    // 状態
    HBITMAP m_hDib{ nullptr };
    SIZE    m_bmpSize{ 0,0 };
    BYTE    m_curAlpha{ 0 };
    BOOL    m_closing{ FALSE };
    BOOL    m_opened{ FALSE }; // ★ 多重起動防止
};
