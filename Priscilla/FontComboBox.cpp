/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "../stdafx.h"
#include "FontComboBox.h"

////------------------------------------------------
//   CFontComboBox
////------------------------------------------------

IMPLEMENT_DYNAMIC(CFontComboBox, CComboBoxFx)

CFontComboBox::CFontComboBox()
{
}

CFontComboBox::~CFontComboBox()
{
}

BEGIN_MESSAGE_MAP(CFontComboBox, CComboBoxFx)
END_MESSAGE_MAP()

void CFontComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (m_bHighContrast)
	{
		m_TextColor = RGB(255, 255, 255);
		m_BgColor = RGB(0, 0, 0);
		m_TextColorSelected = RGB(0, 0, 0);
		m_BgColorSelected = RGB(0, 255, 255);
	}

    CString title;
	if (lpDrawItemStruct->itemID == -1)
	{
        return;
	}

    GetLBText(lpDrawItemStruct->itemID, title);
    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

    CFont font;
    LOGFONT logfont;
    memset(&logfont, 0, sizeof(logfont));
    logfont.lfHeight = m_FontHeight;
    logfont.lfWidth = 0;
    logfont.lfWeight = 400;
	logfont.lfQuality = 6;
	logfont.lfCharSet = DEFAULT_CHARSET;
    _tcscpy_s(logfont.lfFaceName, 32, (LPCTSTR)title);
    font.CreateFontIndirect(&logfont);
	HGDIOBJ oldFont = pDC->SelectObject(&font);

	CBrush Brush;
	CBrush* pOldBrush;

	if (lpDrawItemStruct->itemState & ODS_SELECTED) {
		Brush.CreateSolidBrush(m_BgColorSelected);
		pOldBrush = pDC->SelectObject(&Brush);
		FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, (HBRUSH)Brush);
		SetTextColor(lpDrawItemStruct->hDC, m_TextColorSelected);
	}
	else {
		Brush.CreateSolidBrush(m_BgColor);
		pOldBrush = pDC->SelectObject(&Brush);
		FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, (HBRUSH)Brush);
		SetTextColor(lpDrawItemStruct->hDC, m_TextColor);
	}
	pDC->SelectObject(pOldBrush);
	Brush.DeleteObject();

	pDC->SetBkMode(TRANSPARENT);
	CRect rect = (CRect)(lpDrawItemStruct->rcItem);
	rect.top += m_Margin.top;
	rect.left += m_Margin.left;
	rect.bottom -= m_Margin.bottom;
	rect.right -= m_Margin.right;

	if (m_TextAlign == ES_LEFT)
	{
		pDC->DrawText(title, title.GetLength(), rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
	else if (m_TextAlign == ES_RIGHT)
	{
		pDC->DrawText(title, title.GetLength(), rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	}
	else
	{
		pDC->DrawText(title, title.GetLength(), rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	pDC->SelectObject(oldFont);
}
