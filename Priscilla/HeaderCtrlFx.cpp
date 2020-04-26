/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "../stdafx.h"
#include "HeaderCtrlFx.h"

IMPLEMENT_DYNAMIC(CHeaderCtrlFx, CHeaderCtrl)

CHeaderCtrlFx::CHeaderCtrlFx()
{
	m_X = 0;
	m_Y = 0;

	m_TextColor = RGB(0, 0, 0);
	m_LineColor = RGB(224, 224, 224);

	m_BgDC = NULL;
	m_CtrlBitmap = NULL;
	m_bHighContrast = FALSE;
	m_RenderMode = SystemDraw;
}

CHeaderCtrlFx::~CHeaderCtrlFx()
{
}

BEGIN_MESSAGE_MAP(CHeaderCtrlFx, CHeaderCtrl)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CHeaderCtrlFx::InitControl(int x, int y, double zoomRatio, CDC* bgDC, CBitmap* ctrlBitmap, COLORREF textColor, COLORREF lineColor, int renderMode)
{
	m_X = (int)(x * zoomRatio);
	m_Y = (int)(y * zoomRatio);
	m_ZoomRatio = zoomRatio;
	m_BgDC = bgDC;
	m_TextColor = textColor;
	m_LineColor = lineColor;
	m_CtrlBitmap = ctrlBitmap;
	m_RenderMode = renderMode;
	m_bHighContrast = renderMode & HighContrast;
}

void CHeaderCtrlFx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (m_bHighContrast || m_RenderMode & SystemDraw)
	{
		return CHeaderCtrl::DrawItem(lpDrawItemStruct);
	}

	CDC* drawDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	drawDC->SetBkMode(TRANSPARENT);
	drawDC->SetTextColor(m_TextColor);

	CRect clientRect;
	GetClientRect(&clientRect);

	if (m_CtrlBitmap != NULL)
	{
		CDC BgDC;
		BgDC.CreateCompatibleDC(m_BgDC);
		BgDC.SelectObject(m_CtrlBitmap);

		CRect rc = lpDrawItemStruct->rcItem;
		drawDC->BitBlt(rc.left, rc.top, rc.right, rc.bottom, &BgDC, rc.left, rc.top, SRCCOPY);
	}

	CBrush br;
	br.CreateSolidBrush(m_LineColor);

	CRect rect = lpDrawItemStruct->rcItem;
	rect.left = rect.right - 1;
	drawDC->FillRect(&rect, &br);

	rect = lpDrawItemStruct->rcItem;
	rect.top = rect.bottom - 1;
	drawDC->FillRect(&rect, &br);

	HDITEM hi;
	TCHAR str[256];
	hi.mask = HDI_TEXT;
	hi.pszText = str;
	hi.cchTextMax = 256;
	GetItem(lpDrawItemStruct->itemID, &hi);

	rect = (CRect)(lpDrawItemStruct->rcItem);
	rect.left += (int)(4 * m_ZoomRatio);
	drawDC->DrawText(hi.pszText, lstrlen(hi.pszText), rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

void CHeaderCtrlFx::OnPaint()
{
	if (m_bHighContrast || m_RenderMode & SystemDraw)
	{
		return CHeaderCtrl::OnPaint();
	}

	CHeaderCtrl::OnPaint();

	if (m_CtrlBitmap != NULL)
	{
		RECT rectRightItem;
		int iItemCount = Header_GetItemCount(this->m_hWnd);
		Header_GetItemRect(this->m_hWnd, iItemCount - 1, &rectRightItem);
		RECT rectClient;
		GetClientRect(&rectClient);
		if (rectRightItem.right < rectClient.right)
		{
			CDC* drawDC = GetDC();
			CDC BgDC;
			BgDC.CreateCompatibleDC(m_BgDC);
			BgDC.SelectObject(m_CtrlBitmap);
			drawDC->BitBlt(rectRightItem.right, rectClient.top, rectClient.right, rectClient.bottom, &BgDC, rectRightItem.right, rectClient.top, SRCCOPY);
		}
	}
}