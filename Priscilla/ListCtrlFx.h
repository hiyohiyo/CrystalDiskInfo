/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once

#include "HeaderCtrlFx.h"
#include <atlimage.h>
#include <gdiplus.h>
#pragma comment(lib, "Gdiplus.lib")
using namespace Gdiplus;

class CListCtrlFx : public CListCtrl
{
	DECLARE_DYNAMIC(CListCtrlFx)

public:
	CListCtrlFx();
	virtual ~CListCtrlFx();

	void SetTextColor1(COLORREF color);
	void SetTextColor2(COLORREF color);
	void SetBkColor1(COLORREF color);
	void SetBkColor2(COLORREF color);
	void SetLineColor(COLORREF color);

	COLORREF GetTextColor1();
	COLORREF GetTextColor2();
	COLORREF GetBkColor1();
	COLORREF GetBkColor2();
	COLORREF GetLineColor();

	void InitControl(int x, int y, int width, int height, double zoomRatio, CDC* bgDC, BOOL bHighCotrast);
	void SetFontEx(CString face, double zoomRatio, double fontRatio = 1.0);
	CHeaderCtrlFx m_Header;

protected:
	int m_X;
	int m_Y;
	CSize m_CtrlSize;
	CRect m_Margin;
	BOOL m_bHighContrast;

	COLORREF m_TextColor1;
	COLORREF m_TextColor2;
	COLORREF m_BkColor1;
	COLORREF m_BkColor2;
	COLORREF m_LineColor;

	CFont    m_Font;
	CImageList m_Image;
	CDC* m_BgDC;

	DECLARE_MESSAGE_MAP()

	virtual void PreSubclassWindow();

	afx_msg void OnCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};


