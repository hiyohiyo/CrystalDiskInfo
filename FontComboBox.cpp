/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DiskInfo.h"
#include "ComboBoxCx.h"
#include "FontComboBox.h"


// CFontComboBox

IMPLEMENT_DYNAMIC(CFontComboBox, CComboBox)

CFontComboBox::CFontComboBox()
{
	m_FontHeight = (LONG)(-1 * 16 * 1.00);
}

CFontComboBox::~CFontComboBox()
{
}


BEGIN_MESSAGE_MAP(CFontComboBox, CComboBoxCx)
END_MESSAGE_MAP()

void CFontComboBox::SetFontHeight(int height, double zoomRatio)
{
	m_FontHeight = (LONG)(-1 * height * zoomRatio);
}

void CFontComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	lpMeasureItemStruct->itemHeight = abs(m_FontHeight);
}

void CFontComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CString cstr;
    if (lpDrawItemStruct->itemID == -1)
        return;
    GetLBText(lpDrawItemStruct->itemID, cstr);
    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

    CFont font;
    LOGFONT logfont;
    memset(&logfont, 0, sizeof(logfont));
    logfont.lfHeight = m_FontHeight;
    logfont.lfWidth = 0;
    logfont.lfWeight = 400;
	logfont.lfCharSet = DEFAULT_CHARSET;
    pDC->SelectObject(&font);
    _tcscpy_s(logfont.lfFaceName, 32, (LPCTSTR)cstr);
    font.CreateFontIndirect(&logfont);
    pDC->SelectObject(&font);

    pDC->DrawText(cstr, &lpDrawItemStruct->rcItem, DT_SINGLELINE | DT_VCENTER);
}


int CFontComboBox::CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct)
{
	/*
	LPCTSTR lpszText1 = (LPCTSTR) lpCompareItemStruct->itemData1;
	ASSERT(lpszText1 != NULL);
	LPCTSTR lpszText2 = (LPCTSTR) lpCompareItemStruct->itemData2;
	ASSERT(lpszText2 != NULL);
	
	return _tcscmp(lpszText2, lpszText1);
	*/
	return -1;
}