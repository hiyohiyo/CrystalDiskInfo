/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "ListCtrlEx.h"

IMPLEMENT_DYNAMIC(CListCtrlEx, CListCtrl)

BOOL CListCtrlEx::IsHighContrast()
{
	HIGHCONTRAST hc;
	hc.cbSize = sizeof(HIGHCONTRAST);
	SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST), &hc, 0);
	return hc.dwFlags & HCF_HIGHCONTRASTON;
}

CListCtrlEx::CListCtrlEx()
{
	m_TextColor1 = RGB(0, 0, 0);
	m_TextColor2 = RGB(0, 0, 0);
	m_BkColor1   = RGB(255, 255, 255);
	m_BkColor2   = RGB(248, 248, 248);
	m_LineColor  = RGB(224, 224, 224);

//	m_Image.Create(1, 50, ILC_COLOR, 0, 0);
}

CListCtrlEx::~CListCtrlEx()
{
	m_Font.DeleteObject();
//	m_Image.DeleteImageList();
}

BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
  ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CListCtrlEx::OnNMCustomdraw)
END_MESSAGE_MAP()

void CListCtrlEx::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(IsHighContrast())
	{
		return;
	}

	LPNMLVCUSTOMDRAW lpLVCustomDraw = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

	switch(lpLVCustomDraw->nmcd.dwDrawStage)
	{
	case CDDS_ITEMPREPAINT:
	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		if(lpLVCustomDraw->nmcd.dwItemSpec % 2 == 0)
		{
			lpLVCustomDraw->clrText = m_TextColor1;
			lpLVCustomDraw->clrTextBk = m_BkColor1;
		}
		else
		{
			lpLVCustomDraw->clrText = m_TextColor2;
			lpLVCustomDraw->clrTextBk = m_BkColor2;
		}
		break;
	case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM:
		{
			RECT rc;
			CBrush brush(m_LineColor);

			CHeaderCtrl* header = this->GetHeaderCtrl();
			if(header != NULL)
			{
				int count = header->GetItemCount();
				for(int i = 0; i < count; i++)
				{
					ListView_GetSubItemRect(m_hWnd, lpLVCustomDraw->nmcd.dwItemSpec, i, LVIR_LABEL, &rc);
					rc.left = rc.right - 1;
					FillRect(lpLVCustomDraw->nmcd.hdc, &rc, (HBRUSH) brush.GetSafeHandle());
				}
			}
		}
		break;
    default:
		break;
	}

	*pResult = 0;
	*pResult |= CDRF_NOTIFYPOSTPAINT;
	*pResult |= CDRF_NOTIFYITEMDRAW;
	*pResult |= CDRF_NOTIFYSUBITEMDRAW;
}

void CListCtrlEx::SetTextColor1(COLORREF color){m_TextColor1 = color;}
void CListCtrlEx::SetTextColor2(COLORREF color){m_TextColor2 = color;}
void CListCtrlEx::SetBkColor1(COLORREF color)  {m_BkColor1   = color;}
void CListCtrlEx::SetBkColor2(COLORREF color)  {m_BkColor2   = color;}
void CListCtrlEx::SetLineColor(COLORREF color) {m_LineColor  = color;}

COLORREF CListCtrlEx::GetTextColor1(){return m_TextColor1;}
COLORREF CListCtrlEx::GetTextColor2(){return m_TextColor2;}
COLORREF CListCtrlEx::GetBkColor1()  {return m_BkColor1;}
COLORREF CListCtrlEx::GetBkColor2()  {return m_BkColor2;}
COLORREF CListCtrlEx::GetLineColor() {return m_LineColor;}

void CListCtrlEx::SetFontEx(CString face, double zoomRatio)
{
	LOGFONT logFont = {0};

//	GetFont()->GetLogFont(&logFont);
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfHeight = (LONG)(-12 * zoomRatio);
	logFont.lfQuality = 6;
	if(face.GetLength() < 32)
	{
		wsprintf(logFont.lfFaceName, _T("%s"), face.GetString());
	}
	else
	{
		wsprintf(logFont.lfFaceName, _T(""));
	}

	m_Font.DeleteObject();
	m_Font.CreateFontIndirect(&logFont);
//	GetHeaderCtrl()->SetFont(&m_Font);
	SetFont(&m_Font);
//	SetImageList(&m_Image, LVSIL_STATE);
}