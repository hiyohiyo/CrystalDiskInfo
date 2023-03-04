/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#pragma once

#include "CommonFx.h"
#include <atlimage.h>
#include <gdiplus.h>
#pragma comment(lib, "Gdiplus.lib")
using namespace Gdiplus;

class CButtonFx : public CButton
{
	DECLARE_DYNAMIC(CButtonFx);

public:
	// Constructors
	CButtonFx();
	virtual ~CButtonFx();

	// Control
	BOOL InitControl(int x, int y, int width, int height, double zoomRatio, CDC* bkDC,
		LPCWSTR imagePath, int imageCount, DWORD textAlign, int renderMode, BOOL bHighContrast, BOOL bDarkMode, BOOL bDrawFrame);
	BOOL ReloadImage(LPCWSTR imagePath, UINT imageCount);
	void SetMargin(int top, int left, int bottom, int right, double zoomRatio);
	CSize GetSize(void);
	void SetDrawFrame(BOOL bDrawFrame);
	void SetGlassColor(COLORREF glassColor, BYTE glassAlpha);

	// Font
	void SetFontEx(CString face, int size, int sizeToolTip, double zoomRatio, double fontRatio = 1.0,
		 COLORREF textColor = RGB(0, 0, 0), LONG fontWeight = FW_NORMAL, BYTE fontRender = CLEARTYPE_NATURAL_QUALITY);

	// Mouse
	void SetHandCursor(BOOL bHandCuror = TRUE);
	void SetSelected(BOOL bSelected = TRUE);

	// ToolTip
	void SetToolTipText(LPCTSTR text);
	void SetToolTipActivate(BOOL bActivate = TRUE);
	void SetToolTipWindowText(LPCTSTR text);
	CString GetToolTipText();

protected:
	// Draw Control
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void DrawControl(CDC* drawDC, LPDRAWITEMSTRUCT lpDrawItemStruct, CBitmap& ctrlBitmap, CBitmap& bkBitmap, int no);
	virtual void DrawString(CDC* drawDC, LPDRAWITEMSTRUCT lpDrawItemStruct);

	// Image
	BOOL LoadBitmap(LPCTSTR pFileName);
	BOOL LoadBitmap(HBITMAP hBitmap);
	void SetBkReload(void);
	BOOL SetBitmap(CBitmap& bitmap);
	void LoadCtrlBk(CDC* drawDC);

	// ToolTip
	void InitToolTip();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// Message Map
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnKillfocus();
	afx_msg void OnSetfocus();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

protected:
	// Control
	int m_X;
	int m_Y;
	CSize m_CtrlSize;
	CRect m_Margin;
	int m_RenderMode;
	BOOL m_bHighContrast;
	BOOL m_bDarkMode;
	BOOL m_bDrawFrame;
	COLORREF m_FrameColor;

	// Glass
	COLORREF m_GlassColor;
	BYTE m_GlassAlpha;

	// Image
	CString m_ImagePath;
	int m_ImageCount;
	CDC* m_BkDC;
	CBitmap m_BkBitmap;
	BOOL m_bBkBitmapInit;
	BOOL m_bBkLoad;
	CBitmap m_CtrlBitmap;
	CImage m_CtrlImage;

	// Font
	DWORD m_TextAlign;
	CFont m_Font;
	CFont m_FontToolTip;
	COLORREF m_TextColor;

	// ToolTip
	CToolTipCtrl m_ToolTip;
	CString m_ToolTipText;

	// Mouse
	BOOL m_bHover;
	BOOL m_bFocas;
	BOOL m_bTrackingNow;
	BOOL m_bHandCursor;
	BOOL m_bSelected;
};
