/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "../stdafx.h"
#include "ScrollBarFx.h"
#include "OsInfoFx.h"

CScrollBarFx::CScrollBarFx()
{
	m_X = 0;
	m_Y = 0;
	m_BkDC = NULL;
	m_RenderMode = SystemDraw;
	m_bHighContrast = FALSE;
	m_bDarkMode = FALSE;
}

CScrollBarFx::~CScrollBarFx()
{
	m_BkBrush.DeleteObject();
}

IMPLEMENT_DYNAMIC(CScrollBarFx, CScrollBar)

BEGIN_MESSAGE_MAP(CScrollBarFx, CScrollBar)
	//{{AFX_MSG_MAP(CScrollBarFx)
	ON_WM_HSCROLL_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CScrollBarFx::InitControl(int x, int y, int width, int height, double zoomRatio, CDC* bkDC, int renderMode, BOOL bHighContrast, BOOL bDarkMode, int min, int max, int pos)
{
	m_X = (int)(x * zoomRatio);
	m_Y = (int)(y * zoomRatio);
	m_CtrlSize.cx = (int)(width * zoomRatio);
	m_CtrlSize.cy = (int)(height * zoomRatio);
	MoveWindow(m_X, m_Y, m_CtrlSize.cx, m_CtrlSize.cy);

	m_BkDC = bkDC;
	m_RenderMode = renderMode;
	m_bHighContrast = bHighContrast;
	m_bDarkMode = bDarkMode;

	// BkBrush
	m_BkBrush.DeleteObject();
	if (bDarkMode)
	{
		m_BkBrush.CreateSolidBrush(RGB(32, 32, 32));
	}
	else
	{
		m_BkBrush.CreateSolidBrush(RGB(255, 255, 255));
	}

	SetScrollRange(min, max, TRUE);
	SetScrollPos(pos);
	
	Invalidate();

	return TRUE;
}

void CScrollBarFx::HScroll(UINT nSBCode, UINT nPos)
{
	int position = GetScrollPos();
	switch (nSBCode)
	{
	case SB_LINELEFT:
		position -= 1;
		break;
	case SB_LINERIGHT:
		position += 1;
		break;
	case SB_PAGELEFT:
		position -= 5;
		break;
	case SB_PAGERIGHT:
		position += 5;
		break;
	case SB_LEFT:
		break;
	case SB_RIGHT:
		break;
	case SB_THUMBTRACK:
		position = nPos;
		break;
	}
	SetScrollPos(position);
}