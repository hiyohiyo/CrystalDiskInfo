/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "../stdafx.h"
#include "ListCtrlFx.h"
#include "GetOsInfo.h"

IMPLEMENT_DYNAMIC(CListCtrlFx, CListCtrl)

CListCtrlFx::CListCtrlFx()
{
	m_X = 0;
	m_Y = 0;
	m_bNT6orLater = IsNT6orLater();
	m_BkDC = NULL;
	m_bHighContrast = FALSE;
	m_RenderMode = SystemDraw;

	// Color
	m_TextColor1   = RGB(0, 0, 0);
	m_TextColor2   = RGB(0, 0, 0);
	m_TextSelected = RGB(0, 0, 0);
	m_BkColor1     = RGB(255, 255, 255);
	m_BkColor2     = RGB(248, 248, 248);
	m_BkSelected   = RGB(248, 248, 255);
	m_LineColor1   = RGB(224, 224, 224);
	m_LineColor2   = RGB(240, 240, 240);

	// Glass
	m_GlassColor = RGB(255, 255, 255);
	m_GlassAlpha = 128;
}

CListCtrlFx::~CListCtrlFx()
{
}

BEGIN_MESSAGE_MAP(CListCtrlFx, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CListCtrlFx::OnCustomdraw)
END_MESSAGE_MAP()

BOOL CListCtrlFx::InitControl(int x, int y, int width, int height, int maxWidth, int maxHeight, double zoomRatio, CDC* bkDC, int renderMode)
{
	m_X = (int)(x * zoomRatio);
	m_Y = (int)(y * zoomRatio);
	m_CtrlSize.cx = (int)(width * zoomRatio);
	m_CtrlSize.cy = (int)(height * zoomRatio);
	MoveWindow(m_X, m_Y, m_CtrlSize.cx, m_CtrlSize.cy);
	maxWidth = (int)(maxWidth * zoomRatio);
	maxHeight = (int)(maxHeight * zoomRatio);

	m_BkDC = bkDC;
	m_RenderMode = renderMode;

	if (renderMode & HighContrast)
	{
		m_bHighContrast = TRUE;
		SetBkImage(L"");
	}
	else if (renderMode & OwnerDrawGlass)
	{
		m_bHighContrast = FALSE;
		m_BkBitmap.DeleteObject();
		m_BkBitmap.CreateCompatibleBitmap(m_BkDC, maxWidth, maxHeight);
		CDC BkDC;
		BkDC.CreateCompatibleDC(m_BkDC);
		BkDC.SelectObject(m_BkBitmap);
		BkDC.BitBlt(0, 0, maxWidth, maxHeight, m_BkDC, m_X + 2, m_Y + 2, SRCCOPY);

		m_CtrlImage.Destroy();
		m_CtrlImage.Create(maxWidth, maxHeight, 32);

		RECT rect;
		rect.top = 0;
		rect.left = 0;
		rect.right = maxWidth;
		rect.bottom = maxHeight;

		m_CtrlBitmap.Detach();
		m_CtrlBitmap.Attach((HBITMAP)m_CtrlImage);

		DWORD length = maxWidth * maxHeight * 4;
		BYTE* bitmapBits = new BYTE[length];
		m_CtrlBitmap.GetBitmapBits(length, bitmapBits);

		BYTE r = (BYTE)GetRValue(m_GlassColor);
		BYTE g = (BYTE)GetGValue(m_GlassColor);
		BYTE b = (BYTE)GetBValue(m_GlassColor);
		BYTE a = m_GlassAlpha;

		for (int y = 0; y < maxHeight; y++)
		{
			for (int x = 0; x < maxWidth; x++)
			{
				bitmapBits[(y * maxWidth + x) * 4 + 0] = b;
				bitmapBits[(y * maxWidth + x) * 4 + 1] = g;
				bitmapBits[(y * maxWidth + x) * 4 + 2] = r;
				bitmapBits[(y * maxWidth + x) * 4 + 3] = a;
			}
		}

		m_CtrlBitmap.SetBitmapBits(length, bitmapBits);
		delete[] bitmapBits;

		SetupControlImage(m_BkBitmap, m_CtrlBitmap);
		if(m_bNT6orLater)
		{
			SetBkImage((HBITMAP)m_CtrlBitmap);
			m_Header.InitControl(x, y, zoomRatio, bkDC, &m_CtrlBitmap, m_TextColor1, m_BkColor1, m_LineColor1, m_RenderMode);
		}
		else
		{
			SetBkColor(m_BkColor1);
			m_Header.InitControl(x, y, zoomRatio, bkDC, NULL, m_TextColor1, m_BkColor1, m_LineColor1, m_RenderMode);
		}
	}
	else
	{
		m_bHighContrast = FALSE;
		if(m_bNT6orLater)
		{
			SetBkImage(L"");
			SetBkColor(m_BkColor1);
			m_Header.InitControl(x, y, zoomRatio, bkDC, NULL, m_TextColor1, m_BkColor1, m_LineColor1, m_RenderMode);
		}
		else
		{
			SetBkColor(m_BkColor1);
			m_Header.InitControl(x, y, zoomRatio, bkDC, NULL, m_TextColor1, m_BkColor1, m_LineColor1, m_RenderMode);
		}
	}

	return TRUE;
}

void CListCtrlFx::SetupControlImage(CBitmap& bkBitmap, CBitmap& ctrlBitmap)
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

		if (DstBmpInfo.bmWidthBytes != CtlBmpInfo.bmWidthBytes
			|| DstBmpInfo.bmHeight != CtlBmpInfo.bmHeight) {
			return;
		}

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

		ctrlBitmap.SetBitmapBits(CtlMemSize, CtlBuffer);

		delete[] DstBuffer;
		delete[] CtlBuffer;
	}
}

void CListCtrlFx::OnCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW lpLVCustomDraw = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

	switch(lpLVCustomDraw->nmcd.dwDrawStage)
	{
	case CDDS_ITEMPREPAINT:
	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		if(! m_bHighContrast)
		{
			if(lpLVCustomDraw->nmcd.dwItemSpec % 2 == 0)
			{
				lpLVCustomDraw->clrText = m_TextColor1;
				lpLVCustomDraw->clrTextBk = m_BkColor1;
			}
			else
			{
				lpLVCustomDraw->clrText = m_TextColor2;
				lpLVCustomDraw->clrTextBk = m_BkColor2;
			}
		}
		break;
	case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM:
		{
			RECT rc;
			CBrush br1(m_LineColor1);
			CBrush br2(m_LineColor2);

			CHeaderCtrl* header = this->GetHeaderCtrl();
			if(header != NULL)
			{
				int count = header->GetItemCount();
				for(int i = 0; i < count; i++)
				{
					ListView_GetSubItemRect(m_hWnd, lpLVCustomDraw->nmcd.dwItemSpec, i, LVIR_LABEL, &rc);
					LONG left = rc.left;
					rc.left = rc.right - 1;
					FillRect(lpLVCustomDraw->nmcd.hdc, &rc, (HBRUSH)br1.GetSafeHandle());
					rc.left = left;
					rc.top = rc.bottom - 1;				
					FillRect(lpLVCustomDraw->nmcd.hdc, &rc, (HBRUSH)br2.GetSafeHandle());
				}
			}
		}
		break;
    default:
		break;
	}

	*pResult = 0;
	*pResult |= CDRF_NOTIFYPOSTPAINT;
	*pResult |= CDRF_NOTIFYITEMDRAW;
	*pResult |= CDRF_NOTIFYSUBITEMDRAW;
}

void CListCtrlFx::SetTextColor1(COLORREF color){m_TextColor1 = color;}
void CListCtrlFx::SetTextColor2(COLORREF color){m_TextColor2 = color;}
void CListCtrlFx::SetTextSelected(COLORREF color){m_TextSelected = color;}
void CListCtrlFx::SetBkColor1(COLORREF color){m_BkColor1 = color;}
void CListCtrlFx::SetBkColor2(COLORREF color){m_BkColor2 = color;}
void CListCtrlFx::SetBkSelected(COLORREF color){m_BkSelected = color;}
void CListCtrlFx::SetLineColor1(COLORREF color){m_LineColor1 = color;}
void CListCtrlFx::SetLineColor2(COLORREF color){m_LineColor2 = color;}

void CListCtrlFx::SetGlassColor(COLORREF glassColor, BYTE glassAlpha)
{
	m_GlassColor = glassColor;
	m_GlassAlpha = glassAlpha;
}

COLORREF CListCtrlFx::GetTextColor1(){return m_TextColor1;}
COLORREF CListCtrlFx::GetTextColor2(){return m_TextColor2;}
COLORREF CListCtrlFx::GetTextSelected(){return m_TextSelected;}
COLORREF CListCtrlFx::GetBkColor1(){return m_BkColor1;}
COLORREF CListCtrlFx::GetBkColor2(){return m_BkColor2;}
COLORREF CListCtrlFx::GetBkSelected(){return m_BkSelected;}
COLORREF CListCtrlFx::GetLineColor1(){return m_LineColor1;}
COLORREF CListCtrlFx::GetLineColor2(){return m_LineColor2;}

void CListCtrlFx::SetFontEx(CString face, int size, double zoomRatio, double fontRatio)
{
	LOGFONT logFont = {0};
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfHeight = (LONG)(-1 * size * zoomRatio * fontRatio);
	logFont.lfQuality = 6;
	if(face.GetLength() < 32)
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

	m_Header.SetFontEx(face, size, zoomRatio, fontRatio);
}

void CListCtrlFx::PreSubclassWindow()
{
	CListCtrl::PreSubclassWindow();

	CHeaderCtrlFx* pHeader = (CHeaderCtrlFx*)GetHeaderCtrl();
	m_Header.SubclassWindow(pHeader->GetSafeHwnd());
}

void CListCtrlFx::EnableHeaderOwnerDraw(BOOL bOwnerDraw)
{
	if (m_RenderMode & HighContrast)
	{
		HDITEM hi = { 0 };
		hi.mask = HDI_FORMAT;
		for (int i = 0; i < m_Header.GetItemCount(); i++)
		{
			m_Header.GetItem(i, &hi);
			hi.fmt &= ~HDF_OWNERDRAW;
			m_Header.SetItem(i, &hi);
		}
		return;
	}
	else if (m_RenderMode & OwnerDrawGlass)
	{
		HDITEM hi = {0};
		hi.mask = HDI_FORMAT;
		if (bOwnerDraw)
		{
			for (int i = 0; i < m_Header.GetItemCount(); i++)
			{
				m_Header.GetItem(i, &hi);
				hi.fmt |= HDF_OWNERDRAW;
				m_Header.SetItem(i, &hi);
			}
		}
		else
		{
			for (int i = 0; i < m_Header.GetItemCount(); i++)
			{
				m_Header.GetItem(i, &hi);
				hi.fmt &= ~HDF_OWNERDRAW;
				m_Header.SetItem(i, &hi);
			}
		}
	}
}
