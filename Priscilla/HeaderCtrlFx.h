/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#pragma once

#include "CommonFx.h"

class CHeaderCtrlFx : public CHeaderCtrl
{
	DECLARE_DYNAMIC(CHeaderCtrlFx)

public:
	CHeaderCtrlFx();
	virtual ~CHeaderCtrlFx();
	void InitControl(int x, int y, double zoomRatio, CDC* bkDC, CBitmap* ctrlBitmap, COLORREF textColor, COLORREF bkColor, COLORREF lineColor, int renderMode, BOOL bHighContrast, BOOL bDarkMode);
	void SetFontEx(CString face, int size, double zoomRatio, double fontRatio, LONG fontWeight, BYTE fontRender);

protected:
	// Draw Control
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg LRESULT OnLayout(WPARAM wParam, LPARAM lParam);

	int m_X;
	int m_Y;
	COLORREF m_TextColor;
	COLORREF m_BkColor;
	COLORREF m_LineColor;
	double m_ZoomRatio;
	double m_FontRatio;
	int m_RenderMode;
	BOOL m_bHighContrast;
	BOOL m_bDarkMode;

	CFont m_Font;
	int m_FontSize;
	CDC* m_BkDC;
	CBitmap* m_CtrlBitmap;
};


