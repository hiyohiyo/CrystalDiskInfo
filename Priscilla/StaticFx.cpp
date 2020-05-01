/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "../stdafx.h"
#include "StaticFx.h"

////------------------------------------------------
//   CStaticFx
////------------------------------------------------

CStaticFx::CStaticFx()
{
	// Control
	m_X = 0;
	m_Y = 0;
	m_RenderMode = SystemDraw;
	m_bHighContrast = FALSE;

	// Glass
	m_GlassColor = RGB(255, 255, 255);
	m_GlassAlpha = 255;

	// Meter
	m_bMeter = FALSE;
	m_MeterRatio = 0.0;

	// Image
	m_ImageCount = 0;
	m_BgDC = NULL;
	m_bBgBitmapInit = FALSE;
	m_bBgLoad = FALSE;

	// Font
	m_TextAlign = SS_LEFT;
	m_TextColor = RGB(0, 0, 0);

	// Mouse
	m_bHover = FALSE;
	m_bFocas = FALSE;
	m_bTrackingNow = FALSE;
	m_bHandCursor = FALSE;
}

CStaticFx::~CStaticFx()
{
}

IMPLEMENT_DYNAMIC(CStaticFx, CStatic)

BEGIN_MESSAGE_MAP(CStaticFx, CStatic)
	//{{AFX_MSG_MAP(CStaticFx)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------
// Control
//------------------------------------------------

BOOL CStaticFx::InitControl(int x, int y, int width, int height, double zoomRatio,
	 CDC* bgDC, LPCWSTR imagePath, int imageCount, DWORD textAlign, int renderMode)
{
	m_X = (int)(x * zoomRatio);
	m_Y = (int)(y * zoomRatio);
	m_CtrlSize.cx = (int)(width * zoomRatio);
	m_CtrlSize.cy = (int)(height * zoomRatio);
	MoveWindow(m_X, m_Y, m_CtrlSize.cx, m_CtrlSize.cy);

	m_BgDC = bgDC;
	m_ImagePath = imagePath;
	m_ImageCount = imageCount;
	m_RenderMode = renderMode;

	if (SS_LEFT <= textAlign && textAlign <= SS_RIGHT)
	{
		m_TextAlign = textAlign;
	}

	if (m_ToolTip.m_hWnd != NULL)
	{
		if (m_ToolTip.GetToolCount() != 0)
		{
			m_ToolTip.DelTool(this, 1);
		}
		CRect rect;
		GetClientRect(rect);
		m_ToolTip.AddTool(this, m_ToolTipText, rect, 1);
	}

	if (renderMode & HighContrast)
	{
		m_bHighContrast = TRUE;
		ModifyStyle(SS_OWNERDRAW, m_TextAlign | SS_CENTERIMAGE);

		return TRUE;
	}
	else if (renderMode & SystemDraw)
	{
		ModifyStyle(BS_OWNERDRAW, m_TextAlign | SS_CENTERIMAGE);

		return TRUE;
	}
	else
	{
		m_bHighContrast = FALSE;
		SetBgReload();
		ModifyStyle(m_TextAlign | SS_CENTERIMAGE, SS_OWNERDRAW);
	}

	if (renderMode & OwnerDrawImage)
	{
		if (!LoadBitmap(imagePath))
		{
			ModifyStyle(SS_OWNERDRAW, m_TextAlign);
		}
	}
	else if (renderMode & OwnerDrawGlass)
	{
		m_ImageCount = 1;
		m_CtrlImage.Destroy();
		m_CtrlImage.Create(m_CtrlSize.cx, m_CtrlSize.cy * m_ImageCount, 32);

		RECT rect;
		rect.left = 0;
		rect.right = m_CtrlSize.cx;

		CDC* pDC = CDC::FromHandle(m_CtrlImage.GetDC());
		rect.top = 0;
		rect.bottom = m_CtrlSize.cy;
		pDC->SetDCPenColor(RGB(128, 128, 128));
		pDC->SelectObject(GetStockObject(DC_PEN));
		pDC->Rectangle(&rect);

		m_CtrlImage.ReleaseDC();

		m_CtrlBitmap.Detach();
		m_CtrlBitmap.Attach((HBITMAP)m_CtrlImage);

		DWORD length = m_CtrlSize.cx * m_CtrlSize.cy * m_ImageCount * 4;
		BYTE* bitmapBits = new BYTE[length];
		m_CtrlBitmap.GetBitmapBits(length, bitmapBits);

		BYTE r = (BYTE)GetRValue(m_GlassColor);
		BYTE g = (BYTE)GetGValue(m_GlassColor);
		BYTE b = (BYTE)GetBValue(m_GlassColor);
		BYTE a = m_GlassAlpha;

		for (int y = 0; y < (int)(m_CtrlSize.cy * m_ImageCount); y++)
		{
			for (int x = 0; x < m_CtrlSize.cx; x++)
			{
				bitmapBits[(y * m_CtrlSize.cx + x) * 4 + 0] = b;
				bitmapBits[(y * m_CtrlSize.cx + x) * 4 + 1] = g;
				bitmapBits[(y * m_CtrlSize.cx + x) * 4 + 2] = r;
				bitmapBits[(y * m_CtrlSize.cx + x) * 4 + 3] = a;
			}
		}

		m_CtrlBitmap.SetBitmapBits(length, bitmapBits);
		delete[] bitmapBits;
	}
	else if (renderMode & OwnerDrawTransparent)
	{
		m_ImageCount = 1;
		m_CtrlImage.Destroy();
		m_CtrlImage.Create(m_CtrlSize.cx, m_CtrlSize.cy * m_ImageCount, 32);
		m_CtrlBitmap.Detach();
		m_CtrlBitmap.Attach((HBITMAP)m_CtrlImage);

		DWORD length = m_CtrlSize.cx * m_CtrlSize.cy * m_ImageCount * 4;
		BYTE* bitmapBits = new BYTE[length];
		m_CtrlBitmap.GetBitmapBits(length, bitmapBits);

		for (int y = 0; y < (int)(m_CtrlSize.cy * m_ImageCount); y++)
		{
			for (int x = 0; x < m_CtrlSize.cx; x++)
			{
				//	bitmapBits[(y * m_CtrlSize.cx + x) * 4 + 0] = 255;
				//	bitmapBits[(y * m_CtrlSize.cx + x) * 4 + 1] = 255;
				//	bitmapBits[(y * m_CtrlSize.cx + x) * 4 + 2] = 255;
				bitmapBits[(y * m_CtrlSize.cx + x) * 4 + 3] = (BYTE)0;
			}
		}

		m_CtrlBitmap.SetBitmapBits(length, bitmapBits);
		delete[] bitmapBits;
	}

	Invalidate();

	return TRUE;
}

void CStaticFx::SetMargin(int top, int left, int bottom, int right, double zoomRatio)
{
	m_Margin.top = (int)(top * zoomRatio);
	m_Margin.left = (int)(left * zoomRatio);
	m_Margin.bottom = (int)(bottom * zoomRatio);
	m_Margin.right = (int)(right * zoomRatio);
}

CSize CStaticFx::GetSize(void)
{
	return m_CtrlSize;
}

void CStaticFx::SetDrawFrame(BOOL bDrawFrame)
{
	if (bDrawFrame)
	{
		ModifyStyleEx(0, WS_EX_STATICEDGE, SWP_DRAWFRAME);
	}
	else
	{
		ModifyStyleEx(WS_EX_STATICEDGE, 0, SWP_DRAWFRAME);
	}
}

void CStaticFx::SetGlassColor(COLORREF glassColor, BYTE glassAlpha)
{
	m_GlassColor = glassColor;
	m_GlassAlpha = glassAlpha;
}

void CStaticFx::SetMeter(BOOL bMeter, double meterRatio)
{
	m_bMeter = bMeter;
	if (meterRatio > 0)
	{
		m_MeterRatio = meterRatio;
	}
	else
	{
		m_MeterRatio = 0.0;
	}

	Invalidate();
}

//------------------------------------------------
// Draw Control
//------------------------------------------------

void CStaticFx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* drawDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	LoadCtrlBg(drawDC);

	DrawControl(drawDC, lpDrawItemStruct, m_CtrlBitmap, m_BgBitmap, ControlImageNormal);
}

void CStaticFx::DrawControl(CDC* drawDC, LPDRAWITEMSTRUCT lpDrawItemStruct, CBitmap& CtrlBitmap, CBitmap& BgBitmap, int no)
{
	CDC* pMemDC = new CDC;
	CBitmap* pOldMemBitmap;
	pMemDC->CreateCompatibleDC(drawDC);
	pOldMemBitmap = pMemDC->SelectObject(&CtrlBitmap);
	CDC* pBgDC = new CDC;
	CBitmap* pOldBgBitmap;
	pBgDC->CreateCompatibleDC(drawDC);
	pOldBgBitmap = pBgDC->SelectObject(&BgBitmap);

	if (drawDC->GetDeviceCaps(BITSPIXEL) * drawDC->GetDeviceCaps(PLANES) < 24) 
	{
		drawDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pBgDC, 0, m_CtrlSize.cy, SRCCOPY);
		if (!m_CtrlImage.IsNull())
		{
			if (m_bMeter)
			{
				int meter = (int)(m_CtrlSize.cx * (m_MeterRatio));
				drawDC->BitBlt(meter, 0, m_CtrlSize.cx - meter, m_CtrlSize.cy, pMemDC, meter, m_CtrlSize.cy * 0, SRCCOPY);
				drawDC->BitBlt(0, 0, meter, m_CtrlSize.cy, pMemDC, 0, m_CtrlSize.cy * 1, SRCCOPY);
			}
			else
			{
				drawDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pMemDC, 0, m_CtrlSize.cy * no, SRCCOPY);
			}
		}
		DrawString(drawDC, lpDrawItemStruct);
	}
	else // Full Color (24/32bit)
	{
		CBitmap DrawBmp;
		DrawBmp.CreateCompatibleBitmap(drawDC, m_CtrlSize.cx, m_CtrlSize.cy);
		CDC* pDrawBmpDC = new CDC;
		CBitmap* pOldDrawBitmap;
		pDrawBmpDC->CreateCompatibleDC(drawDC);
		pOldDrawBitmap = pDrawBmpDC->SelectObject(&DrawBmp);

		if (!m_CtrlImage.IsNull())
		{
			if (m_CtrlImage.GetBPP() == 32)
			{
				BITMAP CtlBmpInfo, DstBmpInfo;
				BgBitmap.GetBitmap(&DstBmpInfo);
				DWORD DstLineBytes = DstBmpInfo.bmWidthBytes;
				DWORD DstMemSize = DstLineBytes * DstBmpInfo.bmHeight;
				CtrlBitmap.GetBitmap(&CtlBmpInfo);
				DWORD CtlLineBytes = CtlBmpInfo.bmWidthBytes;;
				DWORD CtlMemSize = CtlLineBytes * CtlBmpInfo.bmHeight;

				BYTE* DstBuffer = new BYTE[DstMemSize];
				BgBitmap.GetBitmapBits(DstMemSize, DstBuffer);
				BYTE* CtlBuffer = new BYTE[CtlMemSize];
				CtrlBitmap.GetBitmapBits(CtlMemSize, CtlBuffer);

				if (m_bMeter)
				{
					int meter = (int)(m_CtrlSize.cx * m_MeterRatio);
					int baseY;
					baseY = m_CtrlSize.cy;
					for (LONG py = 0; py < DstBmpInfo.bmHeight; py++)
					{
						int dn = py * DstLineBytes;
						int cn = (baseY + py) * CtlLineBytes;
						for (LONG px = 0; px < meter; px++)
						{
							int a = CtlBuffer[cn + 3];
							int na = 255 - a;
							DstBuffer[dn + 0] = (BYTE)((CtlBuffer[cn + 0] * a + DstBuffer[dn + 0] * na) / 255);
							DstBuffer[dn + 1] = (BYTE)((CtlBuffer[cn + 1] * a + DstBuffer[dn + 1] * na) / 255);
							DstBuffer[dn + 2] = (BYTE)((CtlBuffer[cn + 2] * a + DstBuffer[dn + 2] * na) / 255);
							dn += (DstBmpInfo.bmBitsPixel / 8);
							cn += (CtlBmpInfo.bmBitsPixel / 8);
						}
						cn -= baseY * CtlLineBytes;
						for (LONG px = meter; px < DstBmpInfo.bmWidth; px++)
						{
							int a = CtlBuffer[cn + 3];
							int na = 255 - a;
							DstBuffer[dn + 0] = (BYTE)((CtlBuffer[cn + 0] * a + DstBuffer[dn + 0] * na) / 255);
							DstBuffer[dn + 1] = (BYTE)((CtlBuffer[cn + 1] * a + DstBuffer[dn + 1] * na) / 255);
							DstBuffer[dn + 2] = (BYTE)((CtlBuffer[cn + 2] * a + DstBuffer[dn + 2] * na) / 255);
							dn += (DstBmpInfo.bmBitsPixel / 8);
							cn += (CtlBmpInfo.bmBitsPixel / 8);
						}
					}
				}
				else
				{
					int baseY = m_CtrlSize.cy * no;
					for (LONG py = 0; py < DstBmpInfo.bmHeight; py++)
					{
						int dn = py * DstLineBytes;
						int cn = (baseY + py) * CtlLineBytes;
						for (LONG px = 0; px < DstBmpInfo.bmWidth; px++)
						{
							BYTE a = CtlBuffer[cn + 3];
							BYTE na = 255 - a;
							DstBuffer[dn + 0] = (BYTE)((CtlBuffer[cn + 0] * a + DstBuffer[dn + 0] * na) / 255);
							DstBuffer[dn + 1] = (BYTE)((CtlBuffer[cn + 1] * a + DstBuffer[dn + 1] * na) / 255);
							DstBuffer[dn + 2] = (BYTE)((CtlBuffer[cn + 2] * a + DstBuffer[dn + 2] * na) / 255);
							dn += (DstBmpInfo.bmBitsPixel / 8);
							cn += (CtlBmpInfo.bmBitsPixel / 8);
						}
					}
				}

				DrawBmp.SetBitmapBits(DstMemSize, DstBuffer);
				DrawString(pDrawBmpDC, lpDrawItemStruct);
				drawDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pDrawBmpDC, 0, 0, SRCCOPY);

				delete[] DstBuffer;
				delete[] CtlBuffer;
			}
			else
			{
				if (m_bMeter)
				{
					int meter = (int)(m_CtrlSize.cx * (m_MeterRatio));
					pDrawBmpDC->BitBlt(meter, 0, m_CtrlSize.cx - meter, m_CtrlSize.cy, pMemDC, meter, m_CtrlSize.cy * 0, SRCCOPY);
					pDrawBmpDC->BitBlt(0, 0, meter, m_CtrlSize.cy, pMemDC, 0, m_CtrlSize.cy * 1, SRCCOPY);
					DrawString(pDrawBmpDC, lpDrawItemStruct);
					drawDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pDrawBmpDC, 0, 0, SRCCOPY);
				}
				else
				{
					pDrawBmpDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pMemDC, 0, m_CtrlSize.cy* no, SRCCOPY);
					DrawString(pDrawBmpDC, lpDrawItemStruct);
					drawDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pDrawBmpDC, 0, 0, SRCCOPY);
				}
			}
		}
		else
		{
			if (m_bMeter)
			{
				pDrawBmpDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pBgDC, 0, m_CtrlSize.cy * no, SRCCOPY);
				DrawString(pDrawBmpDC, lpDrawItemStruct);
				drawDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pDrawBmpDC, 0, 0, SRCCOPY);
			}
			else
			{
				pDrawBmpDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pBgDC, 0, m_CtrlSize.cy* no, SRCCOPY);
				DrawString(pDrawBmpDC, lpDrawItemStruct);
				drawDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pDrawBmpDC, 0, 0, SRCCOPY);
			}
		}

		// Clean up
		pDrawBmpDC->SelectObject(&pOldDrawBitmap);
		pDrawBmpDC->DeleteDC();
		delete pDrawBmpDC;
	}

	// Clean up
	pMemDC->SelectObject(&pOldMemBitmap);
	pBgDC->SelectObject(&pOldBgBitmap);
	pMemDC->DeleteDC();
	pBgDC->DeleteDC();
	delete pMemDC;
	delete pBgDC;
}

void CStaticFx::DrawString(CDC* drawDC, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CString title;
	GetWindowText(title);

	if (title.IsEmpty())
	{
		return;
	}

	drawDC->SetBkMode(TRANSPARENT);
	CRect rect = (CRect)(lpDrawItemStruct->rcItem);
	rect.top += m_Margin.top;
	rect.left += m_Margin.left;
	rect.bottom -= m_Margin.bottom;
	rect.right -= m_Margin.right;

	CRect rectI;
	CSize extent;
	HGDIOBJ oldFont = drawDC->SelectObject(m_Font);
	drawDC->SetTextColor(m_TextColor);
	extent = drawDC->GetTextExtent(title);

	if (m_bMeter && rect.Width() < extent.cx)
	{
		title.Replace(L",", L".");
		int score = _wtoi(title);
		title.Format(L"%d", score);
		extent = drawDC->GetTextExtent(title);
	}

	if (m_TextAlign == SS_LEFT)
	{
		drawDC->DrawText(title, title.GetLength(), rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
	else if (m_TextAlign == SS_RIGHT)
	{
		drawDC->DrawText(title, title.GetLength(), rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	}
	else
	{
		drawDC->DrawText(title, title.GetLength(), rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	drawDC->SelectObject(oldFont);
}

//------------------------------------------------
// Image
//------------------------------------------------

BOOL CStaticFx::LoadBitmap(LPCTSTR fileName)
{
	if (m_bHighContrast) { return FALSE; }
	if (fileName == NULL) { return FALSE; }

	m_CtrlImage.Destroy();
	m_CtrlImage.Load(fileName);
	if (m_CtrlImage.IsNull()) { return FALSE; }

	return LoadBitmap((HBITMAP)m_CtrlImage);
}

BOOL CStaticFx::LoadBitmap(HBITMAP hBitmap)
{
	if (m_bHighContrast) { return FALSE; }

	m_CtrlBitmap.Detach();
	m_CtrlBitmap.Attach(hBitmap);

	return SetBitmap(m_CtrlBitmap);
}

void CStaticFx::SetBgReload(void)
{
	m_bBgBitmapInit = FALSE;
	m_bBgLoad = FALSE;
}

BOOL CStaticFx::SetBitmap(CBitmap& bitmap)
{
	if (m_bHighContrast) { return FALSE; }

	BITMAP	bitmapInfo;
	bitmap.GetBitmap(&bitmapInfo);
	if (m_CtrlSize.cx != bitmapInfo.bmWidth
	||  m_CtrlSize.cy != bitmapInfo.bmHeight / m_ImageCount)
	{
		ModifyStyle(SS_OWNERDRAW, 0);
		return FALSE;
	}
	else
	{
		ModifyStyle(0, SS_OWNERDRAW);
		return TRUE;
	}
}

void CStaticFx::LoadCtrlBg(CDC* drawDC)
{
	if (m_bHighContrast) { SetBgReload(); return; }

	if (m_BgBitmap.m_hObject != NULL)
	{
		BITMAP bitmapInfo;
		m_BgBitmap.GetBitmap(&bitmapInfo);
		if (bitmapInfo.bmBitsPixel != drawDC->GetDeviceCaps(BITSPIXEL))
		{
			SetBgReload();
		}
	}

	if (&m_CtrlBitmap != NULL)
	{
		if (!m_bBgBitmapInit)
		{
			m_BgBitmap.DeleteObject();
			m_BgBitmap.CreateCompatibleBitmap(drawDC, m_CtrlSize.cx, m_CtrlSize.cy);
			m_bBgBitmapInit = TRUE;
		}

		if (!m_bBgLoad)
		{
			CBitmap* pOldBitmap;
			CDC* pMemDC = new CDC;
			pMemDC->CreateCompatibleDC(drawDC);
			pOldBitmap = pMemDC->SelectObject(&m_BgBitmap);
			pMemDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, m_BgDC, m_X, m_Y, SRCCOPY);
			pMemDC->SelectObject(pOldBitmap);
			pMemDC->DeleteDC();
			delete pMemDC;
			m_bBgLoad = TRUE;
		}
	}
}

//------------------------------------------------
// Font
//------------------------------------------------

void CStaticFx::SetFontEx(CString face, int size, int sizeToolTip, double zoomRatio, double fontRatio,
     COLORREF textColor, LONG fontWeight)
{
	LOGFONT logFont = { 0 };
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfHeight = (LONG)(-1 * size * zoomRatio * fontRatio);
	logFont.lfQuality = 6;
	logFont.lfWeight = fontWeight;
	if (face.GetLength() < 32)
	{
		wsprintf(logFont.lfFaceName, L"%s", face.GetString());
	}
	else
	{
		wsprintf(logFont.lfFaceName, L"");
	}

	m_Font.DeleteObject();
	m_Font.CreateFontIndirect(&logFont);
	SetFont(&m_Font);

	logFont.lfHeight = (LONG)(-1 * sizeToolTip * zoomRatio);
	m_FontToolTip.DeleteObject();
	m_FontToolTip.CreateFontIndirect(&logFont);

	m_TextColor = textColor;

	if (m_ToolTip.m_hWnd != NULL)
	{
		m_ToolTip.SetFont(&m_FontToolTip);
	}
}

//------------------------------------------------
// Mouse
//------------------------------------------------

void CStaticFx::SetHandCursor(BOOL bHandCuror)
{
	m_bHandCursor = bHandCuror;
}

void CStaticFx::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTrackingNow)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTrackingNow = _TrackMouseEvent(&tme);
	}
	CStatic::OnMouseMove(nFlags, point);
}

void CStaticFx::OnMouseHover(UINT nFlags, CPoint point)
{
	CStatic::OnMouseHover(nFlags, point);

	m_bHover = TRUE;
	Invalidate();
}

void CStaticFx::OnMouseLeave()
{
	CStatic::OnMouseLeave();

	m_bTrackingNow = FALSE;
	m_bHover = FALSE;
	Invalidate();
}

void CStaticFx::OnSetfocus()
{
	m_bFocas = TRUE;
	Invalidate();
}

void CStaticFx::OnKillfocus()
{
	m_bFocas = FALSE;
	Invalidate();
}

BOOL CStaticFx::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_bHandCursor)
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
	}
	else
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	}

	return TRUE;
}

//------------------------------------------------
// ToolTip
//------------------------------------------------

void CStaticFx::SetToolTipText(LPCTSTR text)
{
	if (text == NULL) { return; }

	InitToolTip();
	m_ToolTipText = text;
	if (m_ToolTip.GetToolCount() == 0)
	{
		CRect rect;
		GetClientRect(rect);
		m_ToolTip.AddTool(this, m_ToolTipText, rect, 1);
	}
	else
	{
		m_ToolTip.UpdateTipText(m_ToolTipText, this, 1);
	}

	SetToolTipActivate(TRUE);
}

void CStaticFx::SetToolTipActivate(BOOL bActivate)
{
	if (m_ToolTip.GetToolCount() == 0) { return; }
	m_ToolTip.Activate(bActivate);
}

void CStaticFx::SetToolTipWindowText(LPCTSTR pText)
{
	SetToolTipText(pText);
	SetWindowText(pText);
}

CString CStaticFx::GetToolTipText()
{
	return m_ToolTipText;
}

void CStaticFx::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
	{
		m_ToolTip.Create(this, TTS_ALWAYSTIP | TTS_BALLOON | TTS_NOANIMATE | TTS_NOFADE);
		m_ToolTip.Activate(FALSE);
		m_ToolTip.SetFont(&m_FontToolTip);
		m_ToolTip.SendMessageW(TTM_SETMAXTIPWIDTH, 0, 1024);
		m_ToolTip.SetDelayTime(TTDT_AUTOPOP, 8000);
		m_ToolTip.SetDelayTime(TTDT_INITIAL, 500);
		m_ToolTip.SetDelayTime(TTDT_RESHOW, 100);
	}
}

BOOL CStaticFx::PreTranslateMessage(MSG* pMsg)
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);

	return CStatic::PreTranslateMessage(pMsg);
}

BOOL CStaticFx::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}