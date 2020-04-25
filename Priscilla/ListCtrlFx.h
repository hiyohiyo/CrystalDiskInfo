/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once

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

	void InitControl(int x, int y, int width, int height, double zoomRatio, BOOL bHighCotrast);
	void SetFontEx(CString face, double zoomRatio, double fontRatio = 1.0);

protected:
	BOOL m_bHighContrast;

	COLORREF m_TextColor1;
	COLORREF m_TextColor2;
	COLORREF m_BkColor1;
	COLORREF m_BkColor2;
	COLORREF m_LineColor;

	CFont    m_Font;
	CImageList m_Image;

	DECLARE_MESSAGE_MAP()

	virtual void PreSubclassWindow();

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};


