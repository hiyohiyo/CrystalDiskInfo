/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once

#include "CommonFx.h"

class CHeaderCtrlFx : public CHeaderCtrl
{
	DECLARE_DYNAMIC(CHeaderCtrlFx)

public:
	CHeaderCtrlFx();
	virtual ~CHeaderCtrlFx();
	void InitControl(int x, int y, double zoomRatio, CDC* bgDC, CBitmap* ctrlBitmap, COLORREF textColor, COLORREF lineColor, int renderMode);

protected:
	int m_X;
	int m_Y;
	COLORREF m_TextColor;
	COLORREF m_LineColor;
	double m_ZoomRatio;
	int m_RenderMode;
	BOOL m_bHighContrast;

	CDC* m_BgDC;
	CBitmap* m_CtrlBitmap;

	// Draw Control
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};


