#include "../stdafx.h"
#include "SliderCtrlFx.h"
#include "OsInfoFx.h"

IMPLEMENT_DYNAMIC(CSliderCtrlFx, CSliderCtrl)

CSliderCtrlFx::CSliderCtrlFx()
{
	m_X = 0;
	m_Y = 0;
	m_BkDC = NULL;
	m_RenderMode = SystemDraw;
	m_bHighContrast = FALSE;
	m_bDarkMode = FALSE;
}

CSliderCtrlFx::~CSliderCtrlFx()
{
	m_BkBrush.DeleteObject();
}

BOOL CSliderCtrlFx::InitControl(int x, int y, int width, int height, double zoomRatio, CDC* bkDC, int renderMode, BOOL bHighContrast, BOOL bDarkMode, int min, int max, int pos)
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

	// Range, Pos
	SetRange(min, max, TRUE);
	SetPos(pos);
	
	Invalidate();

	return TRUE;
}
