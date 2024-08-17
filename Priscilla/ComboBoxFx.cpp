/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "ComboBoxFx.h"

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
//   CComboBoxFx
////------------------------------------------------

CComboBoxFx::CComboBoxFx()
{
	// Control
	m_X = 0;
	m_Y = 0;
	m_ZoomRatio = 1.0;
	m_bHighContrast = FALSE;
	m_bDarkMode = FALSE;
	m_RenderMode = SystemDraw;
	m_Margin.top = 0;
	m_Margin.left = 0;
	m_Margin.bottom = 0;
	m_Margin.right = 0;

	// Alpha/Glass
	m_Alpha = 255;
	m_GlassColor = RGB(255, 255, 255);
	m_GlassAlpha = 255;

	// Image
	m_ImageCount = 0;
	m_ImagePath = _T("");
	m_BkDC = NULL;
	m_bBkBitmapInit = FALSE;
	m_bBkLoad = FALSE;

	// Font
	m_TextAlign = SS_LEFT;
	m_TextColor = RGB(0, 0, 0);
	m_TextColorSelected = RGB(255, 255, 255);
	m_BkColor = RGB(255, 255, 255);
	m_BkColorSelected = RGB(230, 230, 230);
	m_TextColorHc = RGB(255, 255, 255);
	m_TextColorSelectedHc = RGB(0, 0, 0);
	m_BkColorHc = RGB(0, 0, 0);
	m_BkColorSelectedHc = RGB(0, 255, 255);
	m_FontHeight = 16;
	m_FontRender = CLEARTYPE_NATURAL_QUALITY;

	// Mouse
	m_bHover = FALSE;
	m_bFocas = FALSE;
	m_bTrackingNow = FALSE;
	m_bHandCursor = FALSE;
}

CComboBoxFx::~CComboBoxFx()
{
	m_BkBrush.DeleteObject();
}

IMPLEMENT_DYNAMIC(CComboBoxFx, CComboBox)

BEGIN_MESSAGE_MAP(CComboBoxFx, CComboBox)
	//{{AFX_MSG_MAP(CComboBoxFx)
	ON_WM_CTLCOLOR()
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

BOOL CComboBoxFx::InitControl(int x, int y, int width, int height, double zoomRatio, CDC* bkDC,
	 LPCWSTR imagePath, int imageCount, DWORD textAlign, int renderMode, BOOL bHighContrast, BOOL bDarkMode,
	 COLORREF bkColor, COLORREF bkColorSelected, COLORREF glassColor, BYTE glassAlpha)
{
	m_X = (int)(x * zoomRatio);
	m_Y = (int)(y * zoomRatio);
	m_ZoomRatio = zoomRatio;
	m_CtrlSize.cx = (int)(width * zoomRatio);
	m_CtrlSize.cy = (int)(height * zoomRatio);
	MoveWindow(m_X, m_Y, m_CtrlSize.cx, m_CtrlSize.cy);

	m_BkDC = bkDC;
	m_ImagePath = imagePath;
	m_ImageCount = imageCount;
	m_RenderMode = renderMode;

	m_BkColor = bkColor;
	m_BkColorSelected = bkColorSelected;
	m_GlassColor = glassColor;
	m_GlassAlpha = glassAlpha;

	// BkBrush
	m_BkBrush.DeleteObject();
	if (bDarkMode)
	{
		m_BkBrush.CreateSolidBrush(RGB(32, 32, 32));
	}
	else
	{
		m_BkBrush.CreateSolidBrush(bkColor);
	}

	if (ES_LEFT <= textAlign && textAlign <= ES_RIGHT)
	{
		m_TextAlign = textAlign;
		ModifyStyle(0, m_TextAlign);
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

	m_bHighContrast = bHighContrast;
	m_bDarkMode = bDarkMode;

	if (renderMode & SystemDraw)
	{
		return TRUE;
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

	SetBkReload();
	Invalidate();

	return TRUE;
}

void CComboBoxFx::SetFontHeight(int height, double zoomRatio, double fontRatio)
{
	m_FontHeight = (LONG)(-1 * height * zoomRatio * fontRatio);
}

void CComboBoxFx::SetItemHeightEx(int nIndex, int height, double zoomRatio, double fontRatio)
{
	if (nIndex == -1)
	{
		CRect rc;
		GetWindowRect(&rc);
		CComboBox::SetItemHeight(-1, (UINT)(height * zoomRatio - rc.Height() + GetItemHeight(-1)));
	}
	else
	{
		CComboBox::SetItemHeight(nIndex, (UINT)(height * zoomRatio * fontRatio));
	}
}

void CComboBoxFx::SetItemHeightAll(int height, double zoomRatio, double fontRatio)
{
	m_FontHeight = (LONG)(-1 * height * zoomRatio * fontRatio);

	CRect rc;
	GetWindowRect(&rc);
	CComboBox::SetItemHeight(-1, (UINT)(height * zoomRatio - rc.Height() + GetItemHeight(-1)));

	for(int i = 0; i < this->GetCount(); i++)
	{
		CComboBox::SetItemHeight(i, (UINT)(height * zoomRatio * fontRatio));
	}
}

void CComboBoxFx::SetMargin(int top, int left, int bottom, int right, double zoomRatio)
{
	m_Margin.top = (int)(top * zoomRatio);
	m_Margin.left = (int)(left * zoomRatio);
	m_Margin.bottom = (int)(bottom * zoomRatio);
	m_Margin.right = (int)(right * zoomRatio);
	m_ZoomRatio = zoomRatio;
}

CSize CComboBoxFx::GetSize(void)
{
	return m_CtrlSize;
}

void CComboBoxFx::SetGlassColor(COLORREF glassColor, BYTE glassAlpha)
{
	m_GlassColor = glassColor;
	m_GlassAlpha = glassAlpha;
}

void CComboBoxFx::SetAlpha(BYTE alpha)
{
	m_Alpha = alpha;
}

HWND CComboBoxFx::GetListHwnd()
{
#if _MSC_VER > 1310
	COMBOBOXINFO info = { 0 };
	info.cbSize = sizeof(COMBOBOXINFO);
	GetComboBoxInfo(&info);
	
	return info.hwndList;
#else
	return NULL;
#endif
}

HBRUSH CComboBoxFx::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);
	switch (nCtlColor) {
	case CTLCOLOR_EDIT:
		pDC->SetBkMode(TRANSPARENT);
		return hbr;
	case CTLCOLOR_LISTBOX:
		pDC->SetBkMode(TRANSPARENT);
		return m_BkBrush;
	default:
		return hbr;
	}
}

//------------------------------------------------
// Draw Control
//------------------------------------------------

void CComboBoxFx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (lpDrawItemStruct->itemID == -1) { return; }

	static COLORREF textColor;
	static COLORREF textColorSelected;
	static COLORREF bkColor;
	static COLORREF bkColorSelected;

	static int count = 0;
	if (m_bHighContrast)
	{
		textColor = GetTextColor(lpDrawItemStruct->hDC);
		textColorSelected = RGB(0, 0, 0);
		bkColor =  GetBkColor(lpDrawItemStruct->hDC);
		bkColorSelected = RGB(0, 255, 255);

		if (bkColor <= RGB(0x80, 0x80, 0x80)) {	textColor = RGB(255, 255, 255); }
		else { textColor = RGB(0, 0, 0); }
	}
	else if (m_bDarkMode)
	{
		textColor = RGB(255, 255, 255);
		textColorSelected = RGB(255, 255, 255);
		bkColor = RGB(32, 32, 32);
		bkColorSelected = RGB(77, 77, 77);
	}
	else
	{
		textColor = m_TextColor;
		textColorSelected = m_TextColorSelected;
		bkColor = m_BkColor;
		bkColorSelected = m_BkColorSelected;
	}

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	LoadCtrlBk(pDC);
	CString title;
	GetLBText(lpDrawItemStruct->itemID, title);

	CBrush Brush;
	CBrush* pOldBrush;
	if (lpDrawItemStruct->rcItem.left != 0 && !m_bHighContrast)
	{
		DrawControl(title, pDC, lpDrawItemStruct, m_CtrlBitmap, m_BkBitmap, ControlImageNormal);
		Brush.CreateSolidBrush(bkColorSelected);
		pOldBrush = pDC->SelectObject(&Brush);
		if (lpDrawItemStruct->itemState & ODS_SELECTED)
		{
			RECT rc = lpDrawItemStruct->rcItem;
			// rc.top = (LONG)(rc.bottom - 2 * m_ZoomRatio);
			rc.right = (LONG)(rc.left + 3 * m_ZoomRatio);
			FillRect(lpDrawItemStruct->hDC, &rc, (HBRUSH)Brush);
		}
		DrawString(title, pDC, lpDrawItemStruct, textColor);
	}
	else
	{
		if (lpDrawItemStruct->itemState & ODS_SELECTED)
		{
			Brush.CreateSolidBrush(bkColorSelected);
			pOldBrush = pDC->SelectObject(&Brush);
			FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, (HBRUSH)Brush);
			DrawString(title, pDC, lpDrawItemStruct, textColorSelected);
		}
		else
		{
			Brush.CreateSolidBrush(bkColor);
			pOldBrush = pDC->SelectObject(&Brush);
			FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, (HBRUSH)Brush);
			DrawString(title, pDC, lpDrawItemStruct, textColor);
		}
	}
	pDC->SelectObject(pOldBrush);
	Brush.DeleteObject();
}

void CComboBoxFx::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = abs(m_FontHeight);
}

void CComboBoxFx::DrawControl(CString title, CDC* drawDC, LPDRAWITEMSTRUCT lpDrawItemStruct, CBitmap& ctrlBitmap, CBitmap& bkBitmap, int no)
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
			drawDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pDrawBmpDC, 0, 0, SRCCOPY);

			delete[] DstBuffer;
			delete[] CtlBuffer;
		}
		else
		{
			pDrawBmpDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pMemDC, 0, m_CtrlSize.cy * no, SRCCOPY);
			drawDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pDrawBmpDC, 0, 0, SRCCOPY);
		}
	}
	else
	{
		pDrawBmpDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pBkDC, 0, m_CtrlSize.cy * no, SRCCOPY);
		drawDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pDrawBmpDC, 0, 0, SRCCOPY);
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

void CComboBoxFx::DrawString(CString title, CDC* drawDC, LPDRAWITEMSTRUCT lpDrawItemStruct, COLORREF textColor)
{
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
	drawDC->SetTextColor(textColor);

	if (m_TextAlign == ES_LEFT)
	{
		drawDC->DrawText(title, title.GetLength(), rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
	else if (m_TextAlign == ES_RIGHT)
	{
		drawDC->DrawText(title, title.GetLength(), rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	}
	else
	{
		drawDC->DrawText(title, title.GetLength(), rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
}

//------------------------------------------------
// Image
//------------------------------------------------

BOOL CComboBoxFx::LoadBitmap(LPCTSTR fileName)
{
	if (m_bHighContrast) { return FALSE; }
	if (fileName == NULL) { return FALSE; }

	m_CtrlImage.Destroy();
	m_CtrlImage.Load(fileName);
	if (m_CtrlImage.IsNull()) { return FALSE; }

	return LoadBitmap((HBITMAP)m_CtrlImage);
}

BOOL CComboBoxFx::LoadBitmap(HBITMAP hBitmap)
{
	if (m_bHighContrast) { return FALSE; }

	m_CtrlBitmap.Detach();
	m_CtrlBitmap.Attach(hBitmap);

	return SetBitmap(m_CtrlBitmap);
}

void CComboBoxFx::SetBkReload(void)
{
	m_bBkBitmapInit = FALSE;
	m_bBkLoad = FALSE;
}

BOOL CComboBoxFx::SetBitmap(CBitmap& bitmap)
{
	if (m_bHighContrast) { return FALSE; }

	BITMAP	bitmapinfo;
	bitmap.GetBitmap(&bitmapinfo);

	if (m_CtrlSize.cx != bitmapinfo.bmWidth
	||  m_CtrlSize.cy != bitmapinfo.bmHeight / m_ImageCount)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void CComboBoxFx::LoadCtrlBk(CDC* drawDC)
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

void CComboBoxFx::SetFontEx(CString face, int size, int sizeToolTip, double zoomRatio, double fontRatio,
     COLORREF textColor, COLORREF textColorSelected, LONG fontWeight, BYTE fontRender)
{
	LOGFONT logFont = { 0 };
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfHeight = (LONG)(-1 * size * zoomRatio * fontRatio);
	logFont.lfQuality = fontRender;
	logFont.lfWeight = fontWeight;
	m_FontRender = fontRender;

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

	if (! m_bHighContrast)
	{
		m_TextColor = textColor;
		m_TextColorSelected = textColorSelected;
	}

	if (m_ToolTip.m_hWnd != NULL)
	{
		m_ToolTip.SetFont(&m_FontToolTip);
	}
}

//------------------------------------------------
// Mouse
//------------------------------------------------

void CComboBoxFx::SetHandCursor(BOOL bHandCuror)
{
	m_bHandCursor = bHandCuror;
}

void CComboBoxFx::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTrackingNow)
	{
		TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTrackingNow = _TrackMouseEvent(&tme);
	}

	CComboBox::OnMouseMove(nFlags, point);
}

void CComboBoxFx::OnMouseHover(UINT nFlags, CPoint point)
{
#if _MSC_VER > 1310
	CComboBox::OnMouseHover(nFlags, point);
#endif
	m_bHover = TRUE;
	Invalidate();
}

void CComboBoxFx::OnMouseLeave()
{
#if _MSC_VER > 1310
	CComboBox::OnMouseLeave();
#endif
	m_bTrackingNow = FALSE;
	m_bHover = FALSE;
	Invalidate();
}

void CComboBoxFx::OnSetfocus()
{
	m_bFocas = TRUE;
	Invalidate();
}

void CComboBoxFx::OnKillfocus()
{
	m_bFocas = FALSE;
	Invalidate();
}

BOOL CComboBoxFx::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
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

//------------------------------------------------
// ToolTip
//------------------------------------------------

void CComboBoxFx::SetToolTipText(LPCTSTR text)
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

void CComboBoxFx::SetToolTipActivate(BOOL bActivate)
{
	if (m_ToolTip.GetToolCount() == 0) { return; }
	m_ToolTip.Activate(bActivate);
}

void CComboBoxFx::SetToolTipWindowText(LPCTSTR pText)
{
	SetToolTipText(pText);
	SetWindowText(pText);
}

CString CComboBoxFx::GetToolTipText()
{
	return m_ToolTipText;
}

void CComboBoxFx::InitToolTip()
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

BOOL CComboBoxFx::PreTranslateMessage(MSG* pMsg)
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);

	return CComboBox::PreTranslateMessage(pMsg);
}