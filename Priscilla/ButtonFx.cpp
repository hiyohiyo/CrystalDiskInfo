/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "ButtonFx.h"

#if _MSC_VER <= 1310
#define ON_WM_MOUSEHOVER() \
	{ 0x2A1 /*WM_MOUSEHOVER*/, 0, 0, 0, AfxSig_vwp, \
		(AFX_PMSG)(AFX_PMSGW) \
		(static_cast< void (AFX_MSG_CALL CWnd::*)(UINT, CPoint) > (OnMouseHover)) },

#define ON_WM_MOUSELEAVE() \
	{ 0x2A3 /*WM_MOUSELEAVE*/, 0, 0, 0, AfxSig_vv, \
		(AFX_PMSG)(AFX_PMSGW) \
		(static_cast< void (AFX_MSG_CALL CWnd::*)(void) > (OnMouseLeave)) },
#endif

////------------------------------------------------
//   CButtonFx
////------------------------------------------------

CButtonFx::CButtonFx()
{
	// Control
	m_X = 0;
	m_Y = 0;
	m_RenderMode = SystemDraw;
	m_bHighContrast = FALSE;
	m_bDarkMode = FALSE;
	m_bDrawFrame = FALSE;
	m_FrameColor = RGB(128, 128, 128);

	// Glass
	m_GlassColor = RGB(255, 255, 255);
	m_GlassAlpha = 255;

	// Image
	m_ImageCount = 0;
	m_BkDC = NULL;
	m_bBkBitmapInit = FALSE;
	m_bBkLoad = FALSE;

	// Font
	m_TextAlign = BS_LEFT;
	m_TextColor = RGB(0, 0, 0);

	// Mouse
	m_bHover = FALSE;
	m_bFocas = FALSE;
	m_bTrackingNow = FALSE;
	m_bHandCursor = FALSE;
	m_bSelected = FALSE;

	// Margin
	m_Margin.top = 0;
	m_Margin.left = 0;
	m_Margin.bottom = 0;
	m_Margin.right = 0;
}

CButtonFx::~CButtonFx()
{
}

IMPLEMENT_DYNAMIC(CButtonFx, CButton)

BEGIN_MESSAGE_MAP(CButtonFx, CButton)
	//{{AFX_MSG_MAP(CButtonFx)
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

BOOL CButtonFx::InitControl(int x, int y, int width, int height, double zoomRatio, CDC* bkDC,
	LPCTSTR imagePath, int imageCount, DWORD textAlign, int renderMode, BOOL bHighContrast, BOOL bDarkMode, BOOL bDrawFrame)
{
	m_X = (int)(x * zoomRatio);
	m_Y = (int)(y * zoomRatio);
	m_CtrlSize.cx = (int)(width * zoomRatio + 0.5);
	m_CtrlSize.cy = (int)(height * zoomRatio + 0.5);
	MoveWindow(m_X, m_Y, m_CtrlSize.cx, m_CtrlSize.cy);

	m_BkDC = bkDC;
	m_ImagePath = imagePath;
	m_ImageCount = imageCount;
	m_RenderMode = renderMode;

	if (BS_LEFT <= textAlign && textAlign <= BS_CENTER)
	{
		m_TextAlign = textAlign;
	}

	if(m_ToolTip.m_hWnd != NULL)
	{
		if (m_ToolTip.GetToolCount() != 0)
		{
			m_ToolTip.DelTool(this, 1);
		}
		CRect rect;
		GetClientRect(rect);
		m_ToolTip.AddTool(this, m_ToolTipText, rect, 1);
	}

	m_bHighContrast = bHighContrast;
	m_bDarkMode = bDarkMode;
	m_bDrawFrame = bDrawFrame;

	if (m_bHighContrast)
	{
		ModifyStyle(BS_OWNERDRAW, m_TextAlign);

		return TRUE;
	}
	else if(renderMode & SystemDraw)
	{
		ModifyStyle(BS_OWNERDRAW, m_TextAlign);

		return TRUE;
	}
	else
	{
		SetBkReload();
		ModifyStyle(0, BS_OWNERDRAW);
	}

	if (renderMode & OwnerDrawImage)
	{
		if (!LoadBitmap(imagePath))
		{
			ModifyStyle(BS_OWNERDRAW, m_TextAlign);
		}
	}
	else
	{
		m_ImageCount = 1;
		m_CtrlImage.Destroy();
		m_CtrlImage.Create(m_CtrlSize.cx, m_CtrlSize.cy * m_ImageCount, 32);
		m_CtrlBitmap.Detach();
		m_CtrlBitmap.Attach((HBITMAP)m_CtrlImage);
		DWORD length = m_CtrlSize.cx * m_CtrlSize.cy * m_ImageCount * 4;
		BYTE* bitmapBits = new BYTE[length];
		m_CtrlBitmap.GetBitmapBits(length, bitmapBits);

		BYTE r, g, b, a;
		if (renderMode & OwnerDrawGlass)
		{
			r = (BYTE)GetRValue(m_GlassColor);
			g = (BYTE)GetGValue(m_GlassColor);
			b = (BYTE)GetBValue(m_GlassColor);
			a = m_GlassAlpha;
		}
		else // OwnerDrawTransparent
		{
			r = 0;
			g = 0;
			b = 0;
			a = 0;
		}

		for (int y = 0; y < (int)(m_CtrlSize.cy * m_ImageCount); y++)
		{
			for (int x = 0; x < m_CtrlSize.cx; x++)
			{
				DWORD p = (y * m_CtrlSize.cx + x) * 4;
#if _MSC_VER > 1310
#pragma warning( disable : 6386 )
#endif
				bitmapBits[p + 0] = b;
				bitmapBits[p + 1] = g;
				bitmapBits[p + 2] = r;
				bitmapBits[p + 3] = a;
#if _MSC_VER > 1310
#pragma warning( default : 6386 )
#endif
			}
		}

		m_CtrlBitmap.SetBitmapBits(length, bitmapBits);
		delete[] bitmapBits;
	}

	Invalidate();

	return TRUE;
}

BOOL CButtonFx::ReloadImage(LPCTSTR imagePath, UINT imageCount)
{
	if (imagePath != NULL && m_ImagePath.Compare(imagePath) == 0)
	{
		return FALSE;
	}

	m_ImagePath = imagePath;
	m_ImageCount = imageCount;

	LoadBitmap(imagePath);

	Invalidate();
	return TRUE;
}

void CButtonFx::SetMargin(int top, int left, int bottom, int right, double zoomRatio)
{
	m_Margin.top = (int)(top * zoomRatio);
	m_Margin.left = (int)(left * zoomRatio);
	m_Margin.bottom = (int)(bottom * zoomRatio);
	m_Margin.right = (int)(right * zoomRatio);
}

CSize CButtonFx::GetSize(void)
{
	return m_CtrlSize;
}

void CButtonFx::SetDrawFrame(BOOL bDrawFrame)
{
	if (bDrawFrame && m_bHighContrast)
	{
		ModifyStyleEx(0, WS_EX_STATICEDGE, SWP_DRAWFRAME);
	}
	else
	{
		ModifyStyleEx(WS_EX_STATICEDGE, 0, SWP_DRAWFRAME);
	}
	m_bDrawFrame = bDrawFrame;
}

void CButtonFx::SetGlassColor(COLORREF glassColor, BYTE glassAlpha)
{
	m_GlassColor = glassColor;
	m_GlassAlpha = glassAlpha;
}

//------------------------------------------------
// Draw Control
//------------------------------------------------

void CButtonFx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (m_bHighContrast) { return CButton::DrawItem(lpDrawItemStruct); }

	CDC* drawDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	LoadCtrlBk(drawDC);

	if (IsWindowEnabled())
	{
		if (m_bSelected && m_ImageCount > ControlImageSelected)
		{
			DrawControl(drawDC, lpDrawItemStruct, m_CtrlBitmap, m_BkBitmap, ControlImageSelected);
		}
		else if ((lpDrawItemStruct->itemState & ODS_SELECTED || m_bHover) && m_ImageCount > ControlImageHover)
		{
			DrawControl(drawDC, lpDrawItemStruct, m_CtrlBitmap, m_BkBitmap, ControlImageHover);
		}
		else if ((lpDrawItemStruct->itemState & ODS_FOCUS || m_bFocas) && m_ImageCount > ControlImageFocus)
		{
			DrawControl(drawDC, lpDrawItemStruct, m_CtrlBitmap, m_BkBitmap, ControlImageFocus);
		}
		else
		{
			DrawControl(drawDC, lpDrawItemStruct, m_CtrlBitmap, m_BkBitmap, ControlImageNormal);
		}
	}
	else
	{
		if (m_ImageCount > ControlImageDisabled)
		{
			DrawControl(drawDC, lpDrawItemStruct, m_CtrlBitmap, m_BkBitmap, ControlImageDisabled);
		}
		else
		{
			DrawControl(drawDC, lpDrawItemStruct, m_CtrlBitmap, m_BkBitmap, ControlImageNormal);
		}
	}
}

void CButtonFx::DrawControl(CDC* drawDC, LPDRAWITEMSTRUCT lpDrawItemStruct, CBitmap& ctrlBitmap, CBitmap& bkBitmap, int no)
{
	CDC* pMemDC = new CDC;
	CBitmap* pOldMemBitmap;
	pMemDC->CreateCompatibleDC(drawDC);
	pOldMemBitmap = pMemDC->SelectObject(&ctrlBitmap);
	CDC* pBkDC = new CDC;
	CBitmap* pOldBkBitmap;
	pBkDC->CreateCompatibleDC(drawDC);
	pOldBkBitmap = pBkDC->SelectObject(&bkBitmap);

	CBitmap DrawBmp;
	DrawBmp.CreateCompatibleBitmap(drawDC, m_CtrlSize.cx, m_CtrlSize.cy);
	CDC* pDrawBmpDC = new CDC;
	CBitmap* pOldDrawBitmap;
	pDrawBmpDC->CreateCompatibleDC(drawDC);
	pOldDrawBitmap = pDrawBmpDC->SelectObject(&DrawBmp);

	int color = drawDC->GetDeviceCaps(BITSPIXEL) * drawDC->GetDeviceCaps(PLANES);

	if (!m_CtrlImage.IsNull())
	{
		if (m_CtrlImage.GetBPP() == 32)
		{
			CBitmap* bk32Bitmap;
			CImage bk32Image;
			if (color == 32)
			{
				bk32Bitmap = &bkBitmap;
			}
			else
			{
				bk32Image.Create(m_CtrlSize.cx, m_CtrlSize.cy, 32);
				::BitBlt(bk32Image.GetDC(), 0, 0, m_CtrlSize.cx, m_CtrlSize.cy, *pBkDC, 0, 0, SRCCOPY);
				bk32Bitmap = CBitmap::FromHandle((HBITMAP)bk32Image);
			}

			BITMAP CtlBmpInfo, DstBmpInfo;
			bk32Bitmap->GetBitmap(&DstBmpInfo);
			DWORD DstLineBytes = DstBmpInfo.bmWidthBytes;
			DWORD DstMemSize = DstLineBytes * DstBmpInfo.bmHeight;
			ctrlBitmap.GetBitmap(&CtlBmpInfo);
			DWORD CtlLineBytes = CtlBmpInfo.bmWidthBytes;
			DWORD CtlMemSize = CtlLineBytes * CtlBmpInfo.bmHeight;

			if ((DstBmpInfo.bmWidthBytes != CtlBmpInfo.bmWidthBytes)
			||  (DstBmpInfo.bmHeight != CtlBmpInfo.bmHeight / m_ImageCount))
			{
				// Error Check //
			}
			else
			{
				BYTE* DstBuffer = new BYTE[DstMemSize];
				bk32Bitmap->GetBitmapBits(DstMemSize, DstBuffer);
				BYTE* CtlBuffer = new BYTE[CtlMemSize];
				ctrlBitmap.GetBitmapBits(CtlMemSize, CtlBuffer);

				int baseY = m_CtrlSize.cy * no;
				for (LONG py = 0; py < DstBmpInfo.bmHeight; py++)
				{
					int dn = py * DstLineBytes;
					int cn = (baseY + py) * CtlLineBytes;
					for (LONG px = 0; px < DstBmpInfo.bmWidth; px++)
					{
#if _MSC_VER > 1310
#pragma warning( disable : 6385 )
#pragma warning( disable : 6386 )
#endif
						BYTE a = CtlBuffer[cn + 3];
						BYTE na = 255 - a;
						DstBuffer[dn + 0] = (BYTE)((CtlBuffer[cn + 0] * a + DstBuffer[dn + 0] * na) / 255);
						DstBuffer[dn + 1] = (BYTE)((CtlBuffer[cn + 1] * a + DstBuffer[dn + 1] * na) / 255);
						DstBuffer[dn + 2] = (BYTE)((CtlBuffer[cn + 2] * a + DstBuffer[dn + 2] * na) / 255);
						dn += (DstBmpInfo.bmBitsPixel / 8);
						cn += (CtlBmpInfo.bmBitsPixel / 8);
#if _MSC_VER > 1310
#pragma warning( default : 6386 )
#pragma warning( default : 6385 )
#endif
					}
				}

				if (color == 32)
				{
					DrawBmp.SetBitmapBits(DstMemSize, DstBuffer);
				}
				else
				{
					bk32Bitmap->SetBitmapBits(DstMemSize, DstBuffer);
					::BitBlt(pDrawBmpDC->GetSafeHdc(), 0, 0, m_CtrlSize.cx, m_CtrlSize.cy, bk32Image.GetDC(), 0, 0, SRCCOPY);
					bk32Image.ReleaseDC();
				}
				DrawString(pDrawBmpDC, lpDrawItemStruct);
				drawDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pDrawBmpDC, 0, 0, SRCCOPY);

				delete[] DstBuffer;
				delete[] CtlBuffer;
			}
		}
		else
		{
			pDrawBmpDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pMemDC, 0, m_CtrlSize.cy * no, SRCCOPY);
			DrawString(pDrawBmpDC, lpDrawItemStruct);
			drawDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pDrawBmpDC, 0, 0, SRCCOPY);
		}
	}
	else
	{
		pDrawBmpDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pBkDC, 0, m_CtrlSize.cy * no, SRCCOPY);
		DrawString(pDrawBmpDC, lpDrawItemStruct);
		drawDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pDrawBmpDC, 0, 0, SRCCOPY);
	}

	if (m_bDrawFrame)
	{
		HGDIOBJ oldPen;
		POINT point;

		COLORREF color1;
		COLORREF color2;

		if (m_bHover)
		{
			color1 = RGB(0x20, 0x98, 0xF4);
			color2 = RGB(0x20, 0x8B, 0xDE);
		}
		else
		{
		// Windows 11 color
			color1 = RGB(0x00, 0x78, 0xD4);
			color2 = RGB(0x00, 0x6B, 0xBE);
		}

		CPen pen1; pen1.CreatePen(PS_SOLID, 1, color1);
		CPen pen2; pen2.CreatePen(PS_SOLID, 1, color2);

		oldPen = SelectObject(drawDC->m_hDC, pen1);
		MoveToEx(drawDC->m_hDC, 0, m_CtrlSize.cy - 1, &point);
		LineTo(drawDC->m_hDC, m_CtrlSize.cx - 1, m_CtrlSize.cy - 1);
		LineTo(drawDC->m_hDC, m_CtrlSize.cx - 1, 0);
		LineTo(drawDC->m_hDC, m_CtrlSize.cx - 1, m_CtrlSize.cy - 1);
		SelectObject(drawDC->m_hDC, pen2);
		MoveToEx(drawDC->m_hDC, 0, m_CtrlSize.cy - 2, &point);
		LineTo(drawDC->m_hDC, 0, 0);
		LineTo(drawDC->m_hDC, m_CtrlSize.cx - 1, 0);
		SelectObject(drawDC->m_hDC, oldPen);

		pen1.DeleteObject();
		pen2.DeleteObject();
	}

	pDrawBmpDC->SelectObject(&pOldDrawBitmap);
	pDrawBmpDC->DeleteDC();
	delete pDrawBmpDC;
	pMemDC->SelectObject(&pOldMemBitmap);
	pMemDC->DeleteDC();
	delete pMemDC;
	pBkDC->SelectObject(&pOldBkBitmap);
	pBkDC->DeleteDC();
	delete pBkDC;
}

void CButtonFx::DrawString(CDC* drawDC, LPDRAWITEMSTRUCT lpDrawItemStruct)
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

	CArray<CString, CString> arr;
	arr.RemoveAll();

	CString resToken;
	int curPos = 0;
	resToken = title.Tokenize(_T("\r\n"), curPos);
	while (resToken != _T(""))
	{
		arr.Add(resToken);
		resToken = title.Tokenize(_T("\r\n"), curPos);
	}

	for (int i = 0; i < arr.GetCount(); i++)
	{
		CRect r;
		r.top = rect.top + (LONG)(((double)rect.Height()) / arr.GetCount() * i);
		r.bottom = rect.top + (LONG)(((double)rect.Height()) / arr.GetCount() * (i + 1.0));
		r.left = rect.left;
		r.right = rect.right;

		CRect rectI;
		CSize extent;
		HGDIOBJ oldFont = drawDC->SelectObject(m_Font);
		if ((m_RenderMode & OwnerDrawTransparent) && m_bDarkMode)
		{
			SetTextColor(drawDC->m_hDC, RGB(255, 255, 255));
		}
		else
		{
			SetTextColor(drawDC->m_hDC, m_TextColor);
		}
		GetTextExtentPoint32(drawDC->m_hDC, arr.GetAt(i), arr.GetAt(i).GetLength() + 1, &extent);

		if (m_TextAlign == BS_LEFT)
		{
			drawDC->DrawText(arr.GetAt(i), arr.GetAt(i).GetLength(), r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		}
		else if (m_TextAlign == BS_RIGHT)
		{
			drawDC->DrawText(arr.GetAt(i), arr.GetAt(i).GetLength(), r, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		}
		else
		{
			drawDC->DrawText(arr.GetAt(i), arr.GetAt(i).GetLength(), r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}

		drawDC->SelectObject(oldFont);
	}
}

//------------------------------------------------
// Image
//------------------------------------------------

BOOL CButtonFx::LoadBitmap(LPCTSTR fileName)
{
	if (m_bHighContrast) { return FALSE; }
	if (fileName == NULL) { return FALSE; }

	m_CtrlImage.Destroy();
	m_CtrlImage.Load(fileName);
	if (m_CtrlImage.IsNull()) { return FALSE; }

	return LoadBitmap((HBITMAP)m_CtrlImage);
}

BOOL CButtonFx::LoadBitmap(HBITMAP hBitmap)
{
	if (m_bHighContrast) { return FALSE; }

	m_CtrlBitmap.Detach();
	m_CtrlBitmap.Attach(hBitmap);

	return SetBitmap(m_CtrlBitmap);
}

void CButtonFx::SetBkReload(void)
{
	m_bBkBitmapInit = FALSE;
	m_bBkLoad = FALSE;
}

BOOL CButtonFx::SetBitmap(CBitmap& bitmap)
{
	if (m_bHighContrast) { return FALSE; }

	BITMAP	bitmapInfo;
	bitmap.GetBitmap(&bitmapInfo);

	if (m_CtrlSize.cx != bitmapInfo.bmWidth
	||  m_CtrlSize.cy != bitmapInfo.bmHeight / m_ImageCount)
	{
		ModifyStyle(BS_OWNERDRAW, 0);
		return FALSE;
	}
	else
	{
		ModifyStyle(0, BS_OWNERDRAW);
		return TRUE;
	}
}

void CButtonFx::LoadCtrlBk(CDC* drawDC)
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

//------------------------------------------------
// Font
//------------------------------------------------

void CButtonFx::SetFontEx(CString face, int size, int sizeToolTip, double zoomRatio, double fontRatio, COLORREF textColor, LONG fontWeight, BYTE fontRender)
{
	LOGFONT logFont = { 0 };
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfHeight = (LONG)(-1 * size * zoomRatio * fontRatio);
	logFont.lfQuality = fontRender;
	logFont.lfWeight = fontWeight;

	if (face.GetLength() < 32)
	{
		wsprintf(logFont.lfFaceName, _T("%s"), (LPCTSTR)face);
	}
	else
	{
		wsprintf(logFont.lfFaceName, _T(""));
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

void CButtonFx::SetHandCursor(BOOL bHandCuror)
{
	m_bHandCursor = bHandCuror;
}

void CButtonFx::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTrackingNow)
	{
		TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTrackingNow = _TrackMouseEvent(&tme);
	}

	CButton::OnMouseMove(nFlags, point);
}

void CButtonFx::OnMouseHover(UINT nFlags, CPoint point)
{
#if _MSC_VER > 1310
	CButton::OnMouseHover(nFlags, point);
#endif

	m_bHover = TRUE;
	Invalidate();
}

void CButtonFx::OnMouseLeave()
{
#if _MSC_VER > 1310
	CButton::OnMouseLeave();
#endif

	m_bTrackingNow = FALSE;
	m_bHover = FALSE;
	Invalidate();
}

void CButtonFx::OnSetfocus()
{
	m_bFocas = TRUE;
	Invalidate();
}

void CButtonFx::OnKillfocus()
{
	m_bFocas = FALSE;
	Invalidate();
}

BOOL CButtonFx::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	HCURSOR hCursor = NULL;
	if (m_bHandCursor)
	{
		hCursor = AfxGetApp()->LoadStandardCursor(IDC_HAND);
		if (hCursor)
		{
			::SetCursor(hCursor);
		}		
	}
	else
	{
		hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		if (hCursor)
		{
			::SetCursor(hCursor);
		}
	}

	return TRUE;
}

void CButtonFx::SetSelected(BOOL bSelected)
{
	m_bSelected = bSelected;
	Invalidate();
}

//------------------------------------------------
// ToolTip
//------------------------------------------------

void CButtonFx::SetToolTipText(LPCTSTR text)
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

void CButtonFx::SetToolTipActivate(BOOL bActivate)
{
	if (m_ToolTip.GetToolCount() == 0) { return; }
	m_ToolTip.Activate(bActivate);
}

void CButtonFx::SetToolTipWindowText(LPCTSTR text)
{
	SetToolTipText(text);
	SetWindowText(text);
}

CString CButtonFx::GetToolTipText()
{
	return m_ToolTipText;
}

void CButtonFx::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
	{
		m_ToolTip.Create(this, TTS_ALWAYSTIP | TTS_BALLOON | TTS_NOANIMATE | TTS_NOFADE);
		m_ToolTip.Activate(FALSE);
		m_ToolTip.SetFont(&m_FontToolTip);
		m_ToolTip.SendMessage(TTM_SETMAXTIPWIDTH, 0, 1024);
		m_ToolTip.SetDelayTime(TTDT_AUTOPOP, 8000);
		m_ToolTip.SetDelayTime(TTDT_INITIAL, 500);
		m_ToolTip.SetDelayTime(TTDT_RESHOW, 100);
	}
}

BOOL CButtonFx::PreTranslateMessage(MSG* pMsg)
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);

	return CButton::PreTranslateMessage(pMsg);
}

BOOL CButtonFx::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}