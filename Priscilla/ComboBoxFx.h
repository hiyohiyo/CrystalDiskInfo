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

class CComboBoxFx : public CComboBox
{
	DECLARE_DYNAMIC(CComboBoxFx);

// Constructors
public:
	CComboBoxFx();
	virtual ~CComboBoxFx();

// Control
public:
	BOOL InitControl(int x, int y, int width, int height, double zoomRatio,
		 CDC* bgDC, LPCWSTR imagePath, int imageCount, DWORD textAlign, int renderMode,
		 COLORREF bgColor, COLORREF bgColorSelected, COLORREF glassColor, BYTE glassAlpha
	);
	void SetFontHeight(int height, double zoomRatio, double fontRatio = 1.0);
	void SetItemHeightEx(int nIndex, int height, double zoomRatio, double fontRatio = 1.0);
	void SetMargin(int top, int left, int bottom, int right, double zoomRatio);
	CSize GetSize(void);
	void SetGlassColor(COLORREF glassColor, BYTE glassAlpha);
	void SetAlpha(BYTE alpha);

	// Font
	void SetFontEx(CString face, int size, int sizeToolTip, double zoomRatio, double fontRatio = 1.0,
		 BYTE textAlpha = 255, COLORREF textColor = RGB(0, 0, 0), COLORREF textColorSelected = RGB(0, 0, 0), 
		 LONG fontWeight = FW_NORMAL);

	// ToolTip
	void SetToolTipText(LPCTSTR pText);
	void SetToolTipActivate(BOOL bActivate = TRUE);
	void SetToolTipWindowText(LPCTSTR pText);
	CString GetToolTipText();

	// Mouse
	void SetHandCursor(BOOL bHandCuror = TRUE);

protected:
	// Draw Control
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DrawControl(CString title, CDC* drawDC, LPDRAWITEMSTRUCT lpDrawItemStruct, CBitmap& ctrlBitmap, CBitmap& bgBitmap, int no);
	virtual void DrawString(CString title, CDC* drawDC, LPDRAWITEMSTRUCT lpDrawItemStruct, COLORREF textColor);

	// Image
	BOOL LoadBitmap(LPCTSTR fileName);
	BOOL LoadBitmap(HBITMAP hBitmap);
	void SetBgReload(void);
	BOOL SetBitmap(CBitmap& bitmap);
	void LoadCtrlBg(CDC* drawDC);

	// ToolTip
	void InitToolTip();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// Message Map
	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
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

	// Alpha/Glass
	BYTE m_Alpha;
	COLORREF m_GlassColor;
	BYTE m_GlassAlpha;

	// Image
	CString m_ImagePath;
	int m_ImageCount;
	CDC* m_BgDC;
	CBitmap m_BgBitmap;
	BOOL m_bBgBitmapInit;
	BOOL m_bBgLoad;
	CBitmap m_CtrlBitmap;
	CImage m_CtrlImage;

	// Font
	DWORD m_TextAlign;
	CFont m_Font;
	CFont m_FontToolTip;
	COLORREF m_TextColor;
	COLORREF m_TextColorSelected;
	COLORREF m_BgColor;
	COLORREF m_BgColorSelected;
	COLORREF m_TextColorHc;
	COLORREF m_TextColorSelectedHc;
	COLORREF m_BgColorHc;
	COLORREF m_BgColorSelectedHc;
	LONG m_FontHeight;

	// ToolTip
	CToolTipCtrl m_ToolTip;
	CString m_ToolTipText;

	// Mouse
	BOOL m_bHover;
	BOOL m_bFocas;
	BOOL m_bTrackingNow;
	BOOL m_bHandCursor;
};
