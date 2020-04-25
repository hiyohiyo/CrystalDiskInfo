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
END_MESSAGE_MAP()

void CHeaderCtrlFx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* drawDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	CBrush br;
	br.CreateSolidBrush(RGB(255, 0, 0));

	drawDC->FillRect(&(lpDrawItemStruct->rcItem), &br);
}