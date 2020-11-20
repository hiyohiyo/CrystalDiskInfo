/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once

#include "CommonFx.h"
#include <atlimage.h>
#include <gdiplus.h>
#pragma comment(lib, "Gdiplus.lib")
using namespace Gdiplus;

class CScrollBarFx : public CScrollBar
{
	DECLARE_DYNAMIC(CScrollBarFx)

public:
	CScrollBarFx();
	virtual ~CScrollBarFx();
	BOOL InitControl(int x, int y, int width, int height, double zoomRatio, CDC* bkDC, int renderMode, BOOL bHighContrast, BOOL bDarkMode, int min, int max, int pos);

	BOOL m_bHighContrast;
	CBrush m_BkBrush;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void HScroll(UINT nSBCode, UINT nPos);

	int m_X;
	int m_Y;
	CSize m_CtrlSize;
	CRect m_Margin;
	int m_RenderMode;
	BOOL m_bDarkMode;
	CDC* m_BkDC;
};

