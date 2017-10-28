/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once

class CListCtrlEx : public CListCtrl
{
	DECLARE_DYNAMIC(CListCtrlEx)

public:
	CListCtrlEx();
	virtual ~CListCtrlEx();

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

	void SetFontEx(CString face, double zoomRatio);
	BOOL IsHighContrast();

protected:
	COLORREF m_TextColor1;
	COLORREF m_TextColor2;
	COLORREF m_BkColor1;
	COLORREF m_BkColor2;
	COLORREF m_LineColor;

	CFont    m_Font;
	CImageList m_Image;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};


