/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once

#include "CommonFx.h"
#include <atlimage.h>
#include <gdiplus.h>
#pragma comment(lib, "Gdiplus.lib")
using namespace Gdiplus;

class CEditFx : public CEdit
{
	DECLARE_DYNAMIC(CEditFx);

public:
	// Constructors
	CEditFx();
	virtual ~CEditFx();

	// Control
	BOOL InitControl(int x, int y, int width, int height, double zoomRatio, CDC* bkDC, LPCWSTR imagePath,
		 int imageCount, DWORD textAlign, int renderMode, BOOL bHighContrast, BOOL bDarkMode);
	void SetMargin(int top, int left, int bottom, int right, double zoomRatio);
	CSize GetSize(void);
	void SetDrawFrame(BOOL bDrawFrame);
	void SetDrawFrameEx(BOOL bDrawFrame, COLORREF frameColor = RGB(128, 128, 128));
	void SetGlassColor(COLORREF glassColor, BYTE glassAlpha);
	void Adjust();

	// Font
	void SetFontEx(CString face, int size, int sizeToolTip, double zoomRatio, double fontRatio = 1.0,
		 COLORREF textColor = RGB(0, 0, 0), LONG fontWeight = FW_NORMAL, BYTE fontRender = CLEARTYPE_NATURAL_QUALITY);

	// ToolTip
	void SetToolTipText(LPCTSTR text);
	void SetToolTipActivate(BOOL bActivate = TRUE);
	void SetToolTipWindowText(LPCTSTR text);
	CString GetToolTipText();

protected:
	// Image
	BOOL LoadBitmap(LPCTSTR fileName);
	BOOL LoadBitmap(HBITMAP hBitmap);
	void SetBkReload(void);
	BOOL SetBitmap(CBitmap& bitmap);
	void LoadCtrlBk(CDC* drawDC);
	void SetupControlImage(CBitmap& bkBitmap, CBitmap& ctrlBitmap);

	// ToolTip
	void InitToolTip();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// MessageMap
	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnEnChange();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

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
	CBrush m_BkBrush;
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
};
