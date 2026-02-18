/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#pragma once
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Shcore.lib")

class CImageToast : public CWnd
{
public:
    CImageToast();
    virtual ~CImageToast();

    BOOL Show(LPCWSTR pngPath, UINT showMillis = 30000,
              BOOL enableFade = TRUE, BYTE maxAlpha = 255,
              int margin = 16, int fadeInMs = 200, int fadeOutMs = 250,
              LPCWSTR urlToOpen = nullptr);

    void SetLink(LPCWSTR urlToOpen);

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

    void OpenUrlIfAny();

private:
    CString m_pngPath;
    UINT  m_showMillis{ 30000 };
    BOOL  m_enableFade{ TRUE };
    BYTE  m_maxAlpha{ 255 };
    int   m_margin{ 16 };
    int   m_fadeInMs{ 200 };
    int   m_fadeOutMs{ 250 };
    CString m_url;

    // èÛë‘
    HBITMAP m_hDib{ nullptr };
    SIZE    m_bmpSize{ 0,0 };
    BYTE    m_curAlpha{ 0 };
    BOOL    m_closing{ FALSE };
    BOOL    m_opened{ FALSE };
};
