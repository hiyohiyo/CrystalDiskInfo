/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once

#include "CommonFx.h"
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
	void SetTextSelected(COLORREF color);
	void SetBkColor1(COLORREF color);
	void SetBkColor2(COLORREF color);
	void SetBkSelected(COLORREF color);
	void SetLineColor1(COLORREF color);
	void SetLineColor2(COLORREF color);
	void SetGlassColor(COLORREF glassColor, BYTE glassAlpha);

	COLORREF GetTextColor1();
	COLORREF GetTextColor2();
	COLORREF GetTextSelected();
	COLORREF GetBkColor1();
	COLORREF GetBkColor2();
	COLORREF GetBkSelected();
	COLORREF GetLineColor1();
	COLORREF GetLineColor2();

	BOOL InitControl(int x, int y, int width, int height, int maxWidth, int maxHeight, double zoomRatio, CDC* bgDC, int renderMode);
	void SetFontEx(CString face, double zoomRatio, double fontRatio = 1.0);
	void EnableHeaderOwnerDraw(BOOL bOwnerDraw);

protected:
	int m_X;
	int m_Y;
	CSize m_CtrlSize;
	CRect m_Margin;
	int m_RenderMode;
	BOOL m_bHighContrast;
	CHeaderCtrlFx m_Header;

	COLORREF m_TextColor1;
	COLORREF m_TextColor2;
	COLORREF m_TextSelected;
	COLORREF m_BkColor1;
	COLORREF m_BkColor2;
	COLORREF m_BkSelected;
	COLORREF m_LineColor1;
	COLORREF m_LineColor2;

	CFont    m_Font;
	CImageList m_Image;
	CDC* m_BgDC;

	// Glass
	COLORREF m_GlassColor;
	BYTE m_GlassAlpha;

	// Image
	CBitmap m_BgBitmap;
	CBitmap m_CtrlBitmap;
	CImage m_CtrlImage;

	void SetupControlImage(CBitmap& bgBitmap, CBitmap& ctrlBitmap);

	virtual void PreSubclassWindow();
	afx_msg void OnCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
};


