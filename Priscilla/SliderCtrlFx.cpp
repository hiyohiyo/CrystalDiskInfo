/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
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
	m_bBkBitmapInit = FALSE;
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
	SendMessage(TBM_SETTHUMBLENGTH, m_CtrlSize.cy, 0);

	m_BkDC = bkDC;
	m_RenderMode = renderMode;
	m_bHighContrast = bHighContrast;
	m_bDarkMode = bDarkMode;

	SetBkReload();
	LoadCtrlBk(bkDC);
	m_BkBrush.DeleteObject();
	if (bDarkMode)
	{
	//	m_BkBrush.CreateSolidBrush(RGB(32, 32, 32));
		m_BkBrush.CreatePatternBrush(&m_BkBitmap);
	}
	else
	{
	//	m_BkBrush.CreateSolidBrush(RGB(255, 255, 255));
		m_BkBrush.CreatePatternBrush(&m_BkBitmap);
	}

	// Range, Pos
	SetRange(min, max, TRUE);
	SetPos(pos);
	
	Invalidate();

	return TRUE;
}

void CSliderCtrlFx::SetBkReload(void)
{
	m_bBkBitmapInit = FALSE;
	m_bBkLoad = FALSE;
}

void CSliderCtrlFx::LoadCtrlBk(CDC* drawDC)
{
	if (m_bHighContrast) { SetBkReload(); return; }

	if (m_BkBitmap.m_hObject != NULL)
	{
		BITMAP bitmapInfo;
		m_BkBitmap.GetBitmap(&bitmapInfo);
		if (bitmapInfo.bmBitsPixel != drawDC->GetDeviceCaps(BITSPIXEL))
		{
			SetBkReload();
		}
	}

	if (&m_CtrlBitmap != NULL)
	{
		if (!m_bBkBitmapInit)
		{
			m_BkBitmap.DeleteObject();
			m_BkBitmap.CreateCompatibleBitmap(drawDC, m_CtrlSize.cx, m_CtrlSize.cy);
			m_bBkBitmapInit = TRUE;
		}

		if (!m_bBkLoad)
		{
			CBitmap* pOldBitmap;
			CDC* pMemDC = new CDC;
			pMemDC->CreateCompatibleDC(drawDC);
			pOldBitmap = pMemDC->SelectObject(&m_BkBitmap);
			pMemDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, m_BkDC, m_X, m_Y, SRCCOPY);
			pMemDC->SelectObject(pOldBitmap);
			pMemDC->DeleteDC();
			delete pMemDC;
			m_bBkLoad = TRUE;
		}
	}
}