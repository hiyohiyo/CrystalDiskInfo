/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "../stdafx.h"
#include "FontComboBoxFx.h"

////------------------------------------------------
//   CFontComboBox
////------------------------------------------------

IMPLEMENT_DYNAMIC(CFontComboBox, CComboBoxFx)

CFontComboBox::CFontComboBox()
{
	CComboBoxFx();
}

CFontComboBox::~CFontComboBox()
{
	m_BkBrush.DeleteObject();
}

BEGIN_MESSAGE_MAP(CFontComboBox, CComboBoxFx)
END_MESSAGE_MAP()

void CFontComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (lpDrawItemStruct->itemID == -1) { return; }

	static COLORREF textColor;
	static COLORREF textColorSelected;
	static COLORREF bkColor;
	static COLORREF bkColorSelected;

	if (m_bHighContrast)
	{
		textColor = GetTextColor(lpDrawItemStruct->hDC);
		textColorSelected = RGB(0, 0, 0);
		bkColor = GetBkColor(lpDrawItemStruct->hDC);
		bkColorSelected = RGB(0, 255, 255);

		if (bkColor == RGB(0, 0, 0)) { textColor = RGB(255, 255, 255); }
		else if (bkColor == RGB(255, 255, 255)) { textColor = RGB(0, 0, 0); }
	}
	else if (m_bDarkMode)
	{
		textColor = RGB(255, 255, 255);
		textColorSelected = RGB(255, 255, 255);
		bkColor = RGB(32, 32, 32);
		bkColorSelected = RGB(77, 77, 77);
	}
	else
	{
		textColor = m_TextColor;
		textColorSelected = m_TextColorSelected;
		bkColor = m_BkColor;
		bkColorSelected = m_BkColorSelected;
	}

    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	LoadCtrlBk(pDC);
	CString title;
	GetLBText(lpDrawItemStruct->itemID, title);

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
	if (lpDrawItemStruct->rcItem.left != 0 && !m_bHighContrast)
	{
		DrawControl(title, pDC, lpDrawItemStruct, m_CtrlBitmap, m_BkBitmap, ControlImageNormal);
		Brush.CreateSolidBrush(bkColorSelected);
		pOldBrush = pDC->SelectObject(&Brush);
		if (lpDrawItemStruct->itemState & ODS_SELECTED)
		{
			RECT rc = lpDrawItemStruct->rcItem;
			// rc.top = (LONG)(rc.bottom - 2 * m_ZoomRatio);
			rc.right = (LONG)(rc.left + 3 * m_ZoomRatio);
			FillRect(lpDrawItemStruct->hDC, &rc, (HBRUSH)Brush);
		}
		DrawString(title, pDC, lpDrawItemStruct, textColor);
	}
	else
	{
		if (lpDrawItemStruct->itemState & ODS_SELECTED)
		{
			Brush.CreateSolidBrush(bkColorSelected);
			pOldBrush = pDC->SelectObject(&Brush);
			FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, (HBRUSH)Brush);
			DrawString(title, pDC, lpDrawItemStruct, textColorSelected);
		}
		else
		{
			Brush.CreateSolidBrush(bkColor);
			pOldBrush = pDC->SelectObject(&Brush);
			FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, (HBRUSH)Brush);
			DrawString(title, pDC, lpDrawItemStruct, textColor);
		}
	}
	pDC->SelectObject(pOldBrush);
	Brush.DeleteObject();
	pDC->SelectObject(oldFont);
}
