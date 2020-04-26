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

void CHeaderCtrlFx::InitControl(int x, int y, double zoomRatio, CDC* bgDC, CBitmap* ctrlBitmap, int renderMode)
{
	m_X = (int)(x * zoomRatio);
	m_Y = (int)(y * zoomRatio);
	m_BgDC = bgDC;
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

	CBrush br;
	br.CreateSolidBrush(RGB(0, 0, 0));

	drawDC->SetBkMode(TRANSPARENT);
	drawDC->SetTextColor(RGB(255,255,255));
//	drawDC->SetTextColor(RGB(0, 0, 0));

	drawDC->FillRect(&(lpDrawItemStruct->rcItem), &br);

	CRect clientRect;
	GetClientRect(&clientRect);

	/*
	if (m_CtrlBitmap != NULL)
	{
		CDC BgDC;
		BgDC.CreateCompatibleDC(m_BgDC);
		BgDC.SelectObject(m_CtrlBitmap);

		CRect rc = lpDrawItemStruct->rcItem;
		drawDC->BitBlt(rc.left, rc.top, rc.right, rc.bottom, &BgDC, rc.left, rc.top, SRCCOPY);
	}
	*/

	br.DeleteObject();
	br.CreateSolidBrush(RGB(255, 255, 255));

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
	rect.left += 8;
	drawDC->DrawText(hi.pszText, lstrlen(str), rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

void CHeaderCtrlFx::OnPaint()
{
	if (m_bHighContrast || m_RenderMode & SystemDraw)
	{
		return CHeaderCtrl::OnPaint();
	}

	/*
//	if (m_CtrlBitmap != NULL)
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
	*/
}