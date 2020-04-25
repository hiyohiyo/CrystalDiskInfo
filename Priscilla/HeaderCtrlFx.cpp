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
}

CHeaderCtrlFx::~CHeaderCtrlFx()
{
}

BEGIN_MESSAGE_MAP(CHeaderCtrlFx, CHeaderCtrl)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CHeaderCtrlFx::InitControl(int x, int y, double zoomRatio, CDC* bgDC)
{
	m_X = (int)(x * zoomRatio);
	m_Y = (int)(y * zoomRatio);
	m_BgDC = bgDC;
}

void CHeaderCtrlFx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* drawDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	CBrush br;
	br.CreateSolidBrush(RGB(0, 0, 0));

	drawDC->SetBkMode(TRANSPARENT);
	drawDC->SetTextColor(RGB(255,255,255));
	drawDC->SetTextColor(RGB(0, 0, 0));

	drawDC->FillRect(&(lpDrawItemStruct->rcItem), &br);

	CRect clientRect;
	GetClientRect(&clientRect);

	CRect rc = lpDrawItemStruct->rcItem;
	drawDC->BitBlt(rc.left, rc.top, rc.right, rc.bottom, m_BgDC, m_X + rc.left, m_Y + rc.top, SRCCOPY);




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
	CHeaderCtrl::OnPaint();

	RECT rectRightItem;
	int iItemCount = Header_GetItemCount(this->m_hWnd);
	Header_GetItemRect(this->m_hWnd, iItemCount - 1, &rectRightItem);
	RECT rectClient;
	GetClientRect(&rectClient);
	if (rectRightItem.right < rectClient.right)
	{
		CDC* drawDC = GetDC();
		drawDC->BitBlt(rectRightItem.right, rectClient.top, rectClient.right, rectClient.bottom, m_BgDC, m_X + rectRightItem.right, m_Y + rectClient.top, SRCCOPY);

		/*
		HDC hDC = ::GetDC(this->m_hWnd);
		HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
		rectRightItem.left = rectRightItem.right;
		rectRightItem.right = rectClient.right;
		::FillRect(hDC, &rectRightItem, hBrush);
		DeleteObject(hBrush);
		::ReleaseDC(this->m_hWnd, hDC);
		*/
	}
}