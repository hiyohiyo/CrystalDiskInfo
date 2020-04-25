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
	
}

CHeaderCtrlFx::~CHeaderCtrlFx()
{
}

BEGIN_MESSAGE_MAP(CHeaderCtrlFx, CHeaderCtrl)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CHeaderCtrlFx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* drawDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	CBrush br;
	br.CreateSolidBrush(RGB(0, 0, 0));

	drawDC->SetBkMode(TRANSPARENT);
	drawDC->SetTextColor(RGB(255,255,255));

	drawDC->FillRect(&(lpDrawItemStruct->rcItem), &br);

	br.DeleteObject();
	br.CreateSolidBrush(RGB(255, 255, 255));

	CRect rect = lpDrawItemStruct->rcItem;
	rect.left = rect.right - 1;
	drawDC->FillRect(&rect, &br);

	HDITEM hi;
	TCHAR str[256];
	hi.mask = HDI_TEXT;
	hi.pszText = str;
	hi.cchTextMax = 256;
	GetItem(lpDrawItemStruct->itemID, &hi);

	rect = (CRect)(lpDrawItemStruct->rcItem);
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
		HDC hDC = ::GetDC(this->m_hWnd);
		HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
		rectRightItem.left = rectRightItem.right;
		rectRightItem.right = rectClient.right;
		::FillRect(hDC, &rectRightItem, hBrush);
		DeleteObject(hBrush);
		::ReleaseDC(this->m_hWnd, hDC);
	}
}