/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "../stdafx.h"
#include "EditFx.h"

////------------------------------------------------
//   CEditFx
////------------------------------------------------

CEditFx::CEditFx()
{
	// Control
	m_X = 0;
	m_Y = 0;
	m_bHighContrast = FALSE;
	m_bDarkMode = FALSE;
	m_bDrawFrame = FALSE;
	m_RenderMode = OwnerDrawImage;
	m_Margin.top = 0;
	m_Margin.left = 0;
	m_Margin.bottom = 0;
	m_Margin.right = 0;

	// Glass
	m_GlassColor = RGB(255, 255, 255);
	m_GlassAlpha = 128;

	// Image
	m_ImageCount = 0;
	m_BkDC = NULL;
	m_bBkBitmapInit = FALSE;
	m_bBkLoad = FALSE;

	// Font
	m_TextAlign = SS_LEFT;
	m_TextColor = RGB(0, 0, 0);
}

CEditFx::~CEditFx()
{
}

IMPLEMENT_DYNAMIC(CEditFx, CEdit)

BEGIN_MESSAGE_MAP(CEditFx, CEdit)
	//{{AFX_MSG_MAP(CEditFx)
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(EN_CHANGE, OnEnChange)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------
// Control
//------------------------------------------------

BOOL CEditFx::InitControl(int x, int y, int width, int height, double zoomRatio, CDC* bkDC, 
	LPCWSTR imagePath, int imageCount, DWORD textAlign, int renderMode, BOOL bHighContrast, BOOL bDarkMode)
{
	m_X = (int)(x * zoomRatio);
	m_Y = (int)(y * zoomRatio);
	m_CtrlSize.cx = (int)(width * zoomRatio);
	m_CtrlSize.cy = (int)(height * zoomRatio);
	MoveWindow(m_X, m_Y, m_CtrlSize.cx, m_CtrlSize.cy);

	m_BkDC = bkDC;
	m_ImagePath = imagePath;
	m_ImageCount = imageCount;
	m_RenderMode = renderMode;

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

	if (m_bHighContrast)
	{
		return TRUE;
	}
	else
	{
		SetBkReload();
		LoadCtrlBk(m_BkDC);
	}

	if (renderMode & OwnerDrawGlass)
	{
		m_ImageCount = 1;
		m_CtrlImage.Destroy();
		m_CtrlImage.Create(m_CtrlSize.cx, m_CtrlSize.cy * m_ImageCount, 32);

		RECT rect;
		rect.top = 0;
		rect.left = 0;
		rect.right = m_CtrlSize.cx;
		rect.bottom = m_CtrlSize.cy;

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
	else if (renderMode & OwnerDrawImage)
	{
		if (!LoadBitmap(imagePath))
		{
		}
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
	else
	{
		return TRUE;
	}

	SetupControlImage(m_BkBitmap, m_CtrlBitmap);

	m_BkBrush.DeleteObject();
	m_BkBrush.CreatePatternBrush(&m_CtrlBitmap);

	Invalidate();

	return TRUE;
}

void CEditFx::SetMargin(int top, int left, int bottom, int right, double zoomRatio)
{
	m_Margin.top = (int)(top * zoomRatio);
	m_Margin.left = (int)(left * zoomRatio);
	m_Margin.bottom = (int)(bottom * zoomRatio);
	m_Margin.right = (int)(right * zoomRatio);

	SetMargins(m_Margin.left, m_Margin.right);
}

CSize CEditFx::GetSize(void)
{
	return m_CtrlSize;
}

void CEditFx::SetDrawFrame(BOOL bDrawFrame)
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

void CEditFx::SetDrawFrameEx(BOOL bDrawFrame, COLORREF frameColor)
{
	m_bDrawFrame = bDrawFrame;
	m_FrameColor = frameColor;
}

void CEditFx::SetGlassColor(COLORREF glassColor, BYTE glassAlpha)
{
	m_GlassColor = glassColor;
	m_GlassAlpha = glassAlpha;
}

void CEditFx::Adjust()
{
	CRect rectGet, rectSet;
	GetRect(rectGet);

	CClientDC dc(this);
	HGDIOBJ oldFont = dc.SelectObject(&m_Font);
	CSize size = dc.GetTextExtent(L"ÁAaPpQqYy8!");
	dc.SelectObject(oldFont);

	rectSet.top = (m_CtrlSize.cy - size.cy) / 2;
	rectSet.bottom = rectSet.top + size.cy;
	rectSet.left = rectGet.left;
	rectSet.right = rectGet.right;

	SetRect(rectSet);
}

HBRUSH CEditFx::CtlColor(CDC* pDC, UINT nCtlColor)
{
	if (m_bHighContrast)
	{
		pDC->SetBkMode(TRANSPARENT);
		return NULL;
	}
	else
	{
		pDC->SetTextColor(m_TextColor);
		pDC->SetBkMode(TRANSPARENT);
		return m_BkBrush;
	}
}

//------------------------------------------------
// Image
//------------------------------------------------

BOOL CEditFx::LoadBitmap(LPCTSTR fileName)
{
	if (m_bHighContrast) { return FALSE; }
	if (fileName == NULL) { return FALSE; }

	m_CtrlImage.Destroy();
	m_CtrlImage.Load(fileName);
	if (m_CtrlImage.IsNull()) { return FALSE; }

	return LoadBitmap((HBITMAP)m_CtrlImage);
}

BOOL CEditFx::LoadBitmap(HBITMAP hBitmap)
{
	if (m_bHighContrast) { return FALSE; }

	m_CtrlBitmap.Detach();
	m_CtrlBitmap.Attach(hBitmap);

	return SetBitmap(m_CtrlBitmap);
}

void CEditFx::SetBkReload(void)
{
	m_bBkBitmapInit = FALSE;
	m_bBkLoad = FALSE;
}

BOOL CEditFx::SetBitmap(CBitmap& bitmap)
{
	if (m_bHighContrast) { return FALSE; }

	BITMAP	bitmapInfo;
	bitmap.GetBitmap(&bitmapInfo);

	if (m_CtrlSize.cx != bitmapInfo.bmWidth
	||  m_CtrlSize.cy != bitmapInfo.bmHeight / m_ImageCount)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void CEditFx::LoadCtrlBk(CDC* drawDC)
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

void CEditFx::SetupControlImage(CBitmap& bkBitmap, CBitmap& ctrlBitmap)
{
	if (m_BkDC->GetDeviceCaps(BITSPIXEL) * m_BkDC->GetDeviceCaps(PLANES) >= 24)
	{
		BITMAP CtlBmpInfo, DstBmpInfo;
		bkBitmap.GetBitmap(&DstBmpInfo);
		DWORD DstLineBytes = DstBmpInfo.bmWidthBytes;
		DWORD DstMemSize = DstLineBytes * DstBmpInfo.bmHeight;
		ctrlBitmap.GetBitmap(&CtlBmpInfo);
		DWORD CtlLineBytes = CtlBmpInfo.bmWidthBytes;
		DWORD CtlMemSize = CtlLineBytes * CtlBmpInfo.bmHeight;

		if(DstBmpInfo.bmWidthBytes != CtlBmpInfo.bmWidthBytes
		|| DstBmpInfo.bmHeight != CtlBmpInfo.bmHeight) { return ; }

		BYTE* DstBuffer = new BYTE[DstMemSize];
		bkBitmap.GetBitmapBits(DstMemSize, DstBuffer);
		BYTE* CtlBuffer = new BYTE[CtlMemSize];
		ctrlBitmap.GetBitmapBits(CtlMemSize, CtlBuffer);

		int baseY = 0;
		for (LONG py = 0; py < DstBmpInfo.bmHeight; py++)
		{
			int dn = py * DstLineBytes;
			int cn = (baseY + py) * CtlLineBytes;
			for (LONG px = 0; px < DstBmpInfo.bmWidth; px++)
			{
				BYTE a = CtlBuffer[cn + 3];
				BYTE na = 255 - a;
				CtlBuffer[dn + 0] = (BYTE)((CtlBuffer[cn + 0] * a + DstBuffer[dn + 0] * na) / 255);
				CtlBuffer[dn + 1] = (BYTE)((CtlBuffer[cn + 1] * a + DstBuffer[dn + 1] * na) / 255);
				CtlBuffer[dn + 2] = (BYTE)((CtlBuffer[cn + 2] * a + DstBuffer[dn + 2] * na) / 255);
				dn += (DstBmpInfo.bmBitsPixel / 8);
				cn += (CtlBmpInfo.bmBitsPixel / 8);
			}
		}

		if (m_bDrawFrame)
		{
			BYTE r, g, b;
			r = GetRValue(m_FrameColor);
			g = GetGValue(m_FrameColor);
			b = GetBValue(m_FrameColor);
			
			for (LONG py = 0; py < DstBmpInfo.bmHeight; py += DstBmpInfo.bmHeight - 1)
			{
				int dn = py * DstLineBytes;
				int cn = (baseY + py) * CtlLineBytes;
				for (LONG px = 0; px < DstBmpInfo.bmWidth; px++)
				{
					CtlBuffer[dn + 0] = b;
					CtlBuffer[dn + 1] = g;
					CtlBuffer[dn + 2] = r;
					CtlBuffer[cn + 3] = 0;
					dn += (DstBmpInfo.bmBitsPixel / 8);
					cn += (CtlBmpInfo.bmBitsPixel / 8);
				}
			}

			for (LONG py = 0; py < DstBmpInfo.bmHeight; py++)
			{
				int dn = py * DstLineBytes;
				int cn = (baseY + py) * CtlLineBytes;
				for (LONG px = 0; px < DstBmpInfo.bmWidth; px += DstBmpInfo.bmWidth - 1)
				{
					CtlBuffer[dn + 0] = b;
					CtlBuffer[dn + 1] = g;
					CtlBuffer[dn + 2] = r;
					CtlBuffer[cn + 3] = 0;
					dn += (DstBmpInfo.bmBitsPixel / 8) * (DstBmpInfo.bmWidth - 1);
					cn += (CtlBmpInfo.bmBitsPixel / 8) * (DstBmpInfo.bmWidth - 1);
				}
			}
		}

		ctrlBitmap.SetBitmapBits(CtlMemSize, CtlBuffer);

		delete[] DstBuffer;
		delete[] CtlBuffer;
	}
}

//------------------------------------------------
// Font
//------------------------------------------------

void CEditFx::SetFontEx(CString face, int size, int sizeToolTip, double zoomRatio, double fontRatio,
     COLORREF textColor, LONG fontWeight, BYTE fontRender)
{
	LOGFONT logFont = { 0 };
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfHeight = (LONG)(-1 * size * zoomRatio * fontRatio);
	logFont.lfQuality = fontRender;
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
// Message Map
//------------------------------------------------

void CEditFx::OnEnChange()
{
	Invalidate();
}

void CEditFx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_RETURN)
	{
		return ;
	}
	else
	{
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

//------------------------------------------------
// ToolTip
//------------------------------------------------

void CEditFx::SetToolTipText(LPCTSTR text)
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

void CEditFx::SetToolTipActivate(BOOL bActivate)
{
	if (m_ToolTip.GetToolCount() == 0) { return; }
	m_ToolTip.Activate(bActivate);
}

void CEditFx::SetToolTipWindowText(LPCTSTR text)
{
	SetToolTipText(text);
	SetWindowText(text);
}

CString CEditFx::GetToolTipText()
{
	return m_ToolTipText;
}

void CEditFx::InitToolTip()
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

BOOL CEditFx::PreTranslateMessage(MSG* pMsg)
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);

	return CEdit::PreTranslateMessage(pMsg);
}