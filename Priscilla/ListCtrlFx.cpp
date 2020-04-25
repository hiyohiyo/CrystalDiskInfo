/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "../stdafx.h"
#include "ListCtrlFx.h"

IMPLEMENT_DYNAMIC(CListCtrlFx, CListCtrl)

CListCtrlFx::CListCtrlFx()
{
	m_bHighContrast = FALSE;

	m_TextColor1 = RGB(0, 0, 0);
	m_TextColor2 = RGB(0, 0, 0);
	m_BkColor1   = RGB(255, 255, 255);
	m_BkColor2   = RGB(248, 248, 248);
	m_LineColor  = RGB(224, 224, 224);
}

CListCtrlFx::~CListCtrlFx()
{
	m_Font.DeleteObject();
}

BEGIN_MESSAGE_MAP(CListCtrlFx, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CListCtrlFx::OnCustomdraw)
END_MESSAGE_MAP()

void CListCtrlFx::InitControl(int x, int y, int width, int height, double zoomRatio, CDC* bgDC, BOOL bHighCotrast)
{
	// MoveWindow((int)(x * zoomRatio), (int)(y * zoomRatio), (int)(width * zoomRatio), (int)(height * zoomRatio));

	m_X = (int)(x * zoomRatio);
	m_Y = (int)(y * zoomRatio);
	m_CtrlSize.cx = (int)(width * zoomRatio);
	m_CtrlSize.cy = (int)(height * zoomRatio);
	MoveWindow(m_X, m_Y, m_CtrlSize.cx, m_CtrlSize.cy);

	m_BgDC = bgDC;
	m_bHighContrast = bHighCotrast;

	CBitmap bitmap;
	CDC cdc;

	bitmap.CreateCompatibleBitmap(m_BgDC, m_CtrlSize.cx, m_CtrlSize.cy);
	cdc.CreateCompatibleDC(m_BgDC);
	cdc.SelectObject(bitmap);
	cdc.BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, m_BgDC, m_X, m_Y, SRCCOPY);


	/*
	CImage image;

	image.Destroy();
	image.Create(m_CtrlSize.cx, m_CtrlSize.cy, 32);
	::BitBlt(image.GetDC(), 0, 0, m_CtrlSize.cx, m_CtrlSize.cy, *m_BgDC, m_X, m_Y,SRCCOPY);
	*/

	SetBkImage((HBITMAP)bitmap);

}

void CListCtrlFx::OnCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(m_bHighContrast)
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

void CListCtrlFx::SetTextColor1(COLORREF color){m_TextColor1 = color;}
void CListCtrlFx::SetTextColor2(COLORREF color){m_TextColor2 = color;}
void CListCtrlFx::SetBkColor1(COLORREF color)  {m_BkColor1   = color;}
void CListCtrlFx::SetBkColor2(COLORREF color)  {m_BkColor2   = color;}
void CListCtrlFx::SetLineColor(COLORREF color) {m_LineColor  = color;}

COLORREF CListCtrlFx::GetTextColor1(){return m_TextColor1;}
COLORREF CListCtrlFx::GetTextColor2(){return m_TextColor2;}
COLORREF CListCtrlFx::GetBkColor1()  {return m_BkColor1;}
COLORREF CListCtrlFx::GetBkColor2()  {return m_BkColor2;}
COLORREF CListCtrlFx::GetLineColor() {return m_LineColor;}

void CListCtrlFx::SetFontEx(CString face, double zoomRatio, double fontRatio)
{
	LOGFONT logFont = {0};
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfHeight = (LONG)(-12 * zoomRatio * fontRatio);
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
	SetFont(&m_Font);
}

void CListCtrlFx::PreSubclassWindow()
{
	CListCtrl::PreSubclassWindow();

	CHeaderCtrlFx* pHeader = (CHeaderCtrlFx*)GetHeaderCtrl();
	m_Header.SubclassWindow(pHeader->GetSafeHwnd());
}