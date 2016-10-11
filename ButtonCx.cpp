//---------------------------------------------------------------------------
// CButtonCx Copyright (C) 2013 Crystal Dew World All rights reserved.
//---------------------------------------------------------------------------
// CAlphaButton をベースにカスタマイズ
//---------------------------------------------------------------------------
// CAlphaButton Copyright(C)2007 ソフト屋 巣 All rights reserved.
//---------------------------------------------------------------------------
// 特徴:
// ・ホバー状態をサポートしています。
// ・ボタンの５状態を画像で表示することが出来ます。
// ・アルファ付の画像をサポートしています。GDI+を使用。
// ・ツールチップをサポートしています。
//---------------------------------------------------------------------------
// ライセンス(修正BSDライセンスを元にしています)
//---------------------------------------------------------------------------
// CAlphaButton Copyright(C)2007 ソフト屋 巣 All rights reserved.
// 1.ソース・コード形式であれバイナリ形式であれ、変更の有無にかかわらず、以下の条件を満たす限りにおいて、再配布および使用を許可します
// 1-1.ソース・コード形式で再配布する場合、上記著作権表示、本条件書および第2項の責任限定規定を必ず含めてください。
// 1-2.バイナリ(DLLやライブラリ等)形式で再配布する場合、上記著作権表示、本条件書および下記責任限定規定を、配布物とともに提供される文書および／または他の資料に必ず含めてください。
// 1-3.アプリケーションに組み込んで配布する場合は特に著作権表示は必要ありませんが、下記責任限定規定を、配布物とともに提供される文書および／または他の資料に含めてください。
// 2.本ソフトウェアは無保証です。自己責任で使用してください。
// 3.著作権者の名前を、広告や宣伝に勝手に使用しないでください。
//---------------------------------------------------------------------------
// 更新履歴
//---------------------------------------------------------------------------
// Ver0.802 2007/12/06 アルファの無い画像のバグを修正。
// Ver0.801 2007/12/05 リソースのロードに対応しました。
// 	pngもリソース化できます。
// Ver0.800 2007/12/05 GDI+を使わないコードに変更してみました。
// 	遅いようなら、AlphaButton.hのUSE_GDIPを変更すればGDI+に戻せます。
// Ver0.701 2007/11/26 GDI+を使ったバージョンで公開。
//---------------------------------------------------------------------------

#include "stdafx.h"
#include "ButtonCx.h"
#include <atlimage.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

// 計測スイッチ
#define	CHECK_DRAWTIME	(0)	// ボタンの描画時間を計測する場合は1にする。


//------------------------------------------------
// コンストラクタ・デストラクタ
//------------------------------------------------

CButtonCx::CButtonCx()
	: m_bHover(FALSE)
	, m_bFocas(FALSE)
	, m_bTrackingNow(FALSE)
	, m_bSelected(FALSE)
	, m_bBgBitmapInit(FALSE)
	, m_bBgLoad(FALSE)
	, m_GpFont(NULL)
	, m_GpBrush(NULL)
	, m_GpStringformat(NULL)
	, m_TextAlign(BS_LEFT)
	, m_ImagePath(L"")
	, m_ImageCount(1)
	, m_bHandCursor(FALSE)
	, m_RenderMode(0)
	, m_bHighContrast(FALSE)
	, m_Alpha(255)
	, m_FontType(FT_GDI)
{
	m_Margin.top = 0;
	m_Margin.left = 0;
	m_Margin.bottom = 0;
	m_Margin.right = 0;
}

CButtonCx::~CButtonCx()
{
	m_Font.DeleteObject();
	SAFE_DELETE(m_GpFont);
	SAFE_DELETE(m_GpBrush);
	SAFE_DELETE(m_GpStringformat);
}

IMPLEMENT_DYNAMIC(CButtonCx, CButton)

BEGIN_MESSAGE_MAP(CButtonCx, CButton)
	//{{AFX_MSG_MAP(CButtonCx)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

// CButtonCx メッセージ ハンドラ
//------------------------------------------------
// ツールチップ関連
//------------------------------------------------

// ツールチップを初期化する。
void CButtonCx::InitToolTip()
{
	// ツールチップは未初期化？
	if(m_ToolTip.m_hWnd == NULL)
	{
		// ツールチップを初期化する。
		m_ToolTip.Create(this, TTS_ALWAYSTIP|TTS_BALLOON|TTS_NOANIMATE|TTS_NOFADE);
		// ツールチップを無効にする。
		m_ToolTip.Activate(FALSE);
		// ツールチップにフォントを設定します。
		m_ToolTip.SetFont(&m_Font);
		// ツールチップを複数行対応にする。
		m_ToolTip.SendMessageW(TTM_SETMAXTIPWIDTH, 0, 1024);
		// ツールチップを表示するまでの時間を設定する。
		// m_ToolTip.SetDelayTime(1000);
	}
}

// コントロールに渡る前のメッセージを処理。
BOOL CButtonCx::PreTranslateMessage(MSG* pMsg) 
{
	// ツールチップを初期化する。
	InitToolTip();
	
	// ツール ヒント コントロールにメッセージを渡して処理。
	m_ToolTip.RelayEvent(pMsg);

	return CButton::PreTranslateMessage(pMsg);
}

// ツールチップのメッセージを設定する。
void CButtonCx::SetToolTipText(LPCTSTR pText)
{
	// ツールチップを初期化する。
	InitToolTip();

	// テキストが有効？
	if (pText == NULL){ return; }

	CString toolTipText = pText;
	// テキストの内容が異なる？
	if (! toolTipText.IsEmpty() && m_ToolTipText.Find(toolTipText) == 0){ return; }
	m_ToolTipText = toolTipText;

	// ツール ヒント コントロールに登録されているツールの数は無効？
	if(m_ToolTip.GetToolCount() == 0)
	{
		// クライアント領域のサイズを取得します
		CRect rect; 
		GetClientRect(rect);
		// ツール ヒント コントロールにツールを登録します
		m_ToolTip.AddTool(this, m_ToolTipText, rect, 1);
	}
	else
	{
		// ツールにツール ヒント テキストを設定します。
		m_ToolTip.UpdateTipText(m_ToolTipText, this, 1);
	}
	// ツールチップを有効にします。
	SetToolTipActivate(TRUE);
}

// ツールチップの有効無効を切り替えます。
void CButtonCx::SetToolTipActivate(BOOL bActivate)
{
	// ツール ヒント コントロールに登録されているツールの数は無効？
	if(m_ToolTip.GetToolCount() == 0) return;

	// ツールチップの有効無効を切り替えます。
	m_ToolTip.Activate(bActivate);
}

// コントロールとツールチップのメッセージを設定する。
void CButtonCx::SetToolTipWindowText(LPCTSTR pText)
{
	SetToolTipText(pText);
	SetWindowText(pText);
}

//------------------------------------------------
// コントロール画像の関連
//------------------------------------------------

// コントロールのオーナドロー
void CButtonCx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// 描画のＣＤＣ

	CDC *drawDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	
	// 背景を取り込む。
	LoadCtrlBg(drawDC);
	
	if(IsWindowEnabled())
	{
		// ボタンが押されている？
		if(lpDrawItemStruct->itemState & ODS_SELECTED)
		{
			// 押されたボタンの描画
			if(m_ImageCount >= 2)
			{

				DrawControl(drawDC, lpDrawItemStruct, m_CtrlBitmap, m_BgBitmap, 1);//2);
			}
			else
			{
				DrawControl(drawDC, lpDrawItemStruct, m_CtrlBitmap, m_BgBitmap, 0);//1);
			}
		}
		else
		{
			// ホバー状態？
			if(m_bHover)
			{
				// ホバー状態の描画。
				if(m_ImageCount >= 2)
				{
					DrawControl(drawDC, lpDrawItemStruct, m_CtrlBitmap, m_BgBitmap, 1);//1);
				}
				else
				{
					DrawControl(drawDC, lpDrawItemStruct, m_CtrlBitmap, m_BgBitmap, 0);//1);
				}
			}
			// 選択中？
			else if(m_bSelected)
			{
				if(m_ImageCount >= 4)
				{
					DrawControl(drawDC, lpDrawItemStruct, m_CtrlBitmap, m_BgBitmap, 3);//1);
				}
				else
				{
					DrawControl(drawDC, lpDrawItemStruct, m_CtrlBitmap, m_BgBitmap, 1);//1);
				}
			}

			else
			{
				// フォーカスしている？
				if(m_bFocas)
				{
					// フォーカス状態の描画。
					DrawControl(drawDC, lpDrawItemStruct, m_CtrlBitmap, m_BgBitmap, 0);//4);
				}
				else
				{
					// 通常状態の描画。
					DrawControl(drawDC, lpDrawItemStruct, m_CtrlBitmap, m_BgBitmap, 0);
				}
			}
		}
	}
	else
	{
		// コントロールが無効
		DrawControl(drawDC, lpDrawItemStruct, m_CtrlBitmap, m_BgBitmap, 0);//3);
	}
}

void CButtonCx::DrawString(CDC *drawDC, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// テキストを得る。
	CString title;
	GetWindowText(title);

	// テキストが空の場合および GDI+ Font/Brush が設定されていない場合何もしない。
	if (title.IsEmpty() || m_GpFont == NULL || m_GpBrush == NULL)
	{
		return;
	}

	// 透過モードにする。
	drawDC->SetBkMode(TRANSPARENT);

	// テキストの描画位置
	CRect rect = (CRect) (lpDrawItemStruct->rcItem); // クライアント四角形の取得
	// マージン設定
	rect.top += m_Margin.top;
	rect.left += m_Margin.left;
	rect.bottom -= m_Margin.bottom;
	rect.right -= m_Margin.right;

	CArray<CString, CString> arr;
	arr.RemoveAll();

	CString resToken;
	int curPos = 0;
	resToken = title.Tokenize(L"\r\n", curPos);
	while (resToken != L"")
	{
		arr.Add(resToken);
		resToken = title.Tokenize(L"\r\n", curPos);
	}

	if (m_FontType == FT_GDI_PLUS || m_FontType == FT_AUTO)	// GDI+
	{
		Gdiplus::Graphics g(drawDC->m_hDC);

		const Gdiplus::PointF pointF(0.0, 0.0);
		Gdiplus::RectF extentF;

		for (int i = 0; i < arr.GetCount(); i++)
		{
			CRect r;
			r.top = (LONG) (((double) rect.Height()) / arr.GetCount() * i);
			r.bottom = (LONG) (((double) rect.Height()) / arr.GetCount() * (i + 1));
			r.left = rect.left;
			r.right = rect.right;

			g.MeasureString(arr.GetAt(i), arr.GetAt(i).GetLength() + 1, m_GpFont, pointF, &extentF); // "+ 1" for workdaround 

			REAL y;
			FontFamily ff;
			m_GpFont->GetFamily(&ff);
			REAL ascent = (REAL) ff.GetCellAscent(FontStyleRegular);
			REAL lineSpacing = (REAL) ff.GetLineSpacing(FontStyleRegular);

			y = r.CenterPoint().y - (extentF.Height * ascent / lineSpacing) / 2;

			Gdiplus::PointF pt(rect.CenterPoint().x - (extentF.Width / 2), y);
			Gdiplus::RectF rectF(pt.X, pt.Y, (REAL) extentF.Width, (REAL) extentF.Height);

			g.SetTextRenderingHint(TextRenderingHintAntiAlias);
			g.DrawString(arr.GetAt(i), -1, m_GpFont, rectF, m_GpStringformat, m_GpBrush);
		}
	}
	else // GDI
	{
		for (int i = 0; i < arr.GetCount(); i++)
		{
			CRect r;
			r.top = rect.top + (LONG) (((double) rect.Height()) / arr.GetCount() * i);
			r.bottom = rect.top + (LONG) (((double) rect.Height()) / arr.GetCount() * (i + 1));
			r.left = rect.left;
			r.right = rect.right;

			CRect rectI;
			CSize extent;
			HGDIOBJ oldFont = drawDC->SelectObject(m_Font);
			GetTextExtentPoint32(drawDC->m_hDC, arr.GetAt(i), arr.GetAt(i).GetLength() + 1, &extent);
			rectI.top = r.top + (r.Height() - extent.cy) / 2;
			rectI.bottom = rectI.top + extent.cy;
			rectI.left = r.left;
			rectI.right = r.right;
			DrawText(drawDC->m_hDC, arr.GetAt(i), arr.GetAt(i).GetLength(), r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			drawDC->SelectObject(oldFont);
		}
	}
	// いつか DirectWrite 描画に対応したいものである。。。
}

// コントロールの描画
void CButtonCx::DrawControl(CDC *drawDC, LPDRAWITEMSTRUCT lpDrawItemStruct, CBitmap &CtrlBitmap, CBitmap &BgBitmap, int no)
{
#if CHECK_DRAWTIME
	// 時間計測
	DWORD StartMsTime;				// スタート時間
	DWORD EndMsTime;				// 終了時間
	StartMsTime = timeGetTime();	// スタート時間計測
#endif

	// 合成用のＣＤＣにビットマップを割り当てる。
	CDC *pMemDC = new CDC;
	CBitmap* pOldMemBitmap;
	pMemDC->CreateCompatibleDC(drawDC);
	pOldMemBitmap = pMemDC->SelectObject(&CtrlBitmap);
	CDC *pBgDC = new CDC;
	CBitmap* pOldBgBitmap;
	pBgDC->CreateCompatibleDC(drawDC);
	pOldBgBitmap = pBgDC->SelectObject(&BgBitmap);

	if (drawDC->GetDeviceCaps(BITSPIXEL) * drawDC->GetDeviceCaps(PLANES) < 24)
	{
		BLENDFUNCTION blendfunc = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

		// 取り込んでいた背景を描画
		drawDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pBgDC, 0, m_CtrlSize.cy * no, SRCCOPY);
		if (!m_CtrlImage.IsNull())
		{
			if (m_CtrlImage.GetBPP() == 32)
			{
				// コントロールをAlpha合成
				drawDC->AlphaBlend(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pMemDC, 0, m_CtrlSize.cy * no, m_CtrlSize.cx, m_CtrlSize.cy, blendfunc);
			}
			else
			{
				// コントロールをそのまま描画
				drawDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pMemDC, 0, m_CtrlSize.cy * no, SRCCOPY);
			}
		}	
		// 描画用のビットマップに文字列を描画。
		DrawString(drawDC, lpDrawItemStruct);
	}
	else
	{
		// 描画用のビットマップを用意する。
		CBitmap DrawBmp;
		DrawBmp.CreateCompatibleBitmap(drawDC, m_CtrlSize.cx, m_CtrlSize.cy);
		// 描画用のＣＤＣを用意する。
		CDC *pDrawBmpDC = new CDC;
		CBitmap* pOldDrawBitmap;
		pDrawBmpDC->CreateCompatibleDC(drawDC);
		pOldDrawBitmap = pDrawBmpDC->SelectObject(&DrawBmp);

		// イメージはロードされている？
		if (!m_CtrlImage.IsNull())
		{
			// 画像にアルファチャンネルがある？
			if (m_CtrlImage.GetBPP() == 32 || m_Alpha < 255)
			{
				// ビットマップを得るために必要なバッファのサイズを得る。
				BITMAP	CtlBmpInfo, DstBmpInfo;
				BgBitmap.GetBitmap(&DstBmpInfo);
				DWORD DstLineBytes = DstBmpInfo.bmWidthBytes;
				DWORD DstMemSize = DstLineBytes * DstBmpInfo.bmHeight;
				CtrlBitmap.GetBitmap(&CtlBmpInfo);
				DWORD CtlLineBytes = 0;
				if (m_CtrlImage.GetBPP() == 24)
				{
					CtlLineBytes = CtlBmpInfo.bmWidth * 3;
					if (CtlBmpInfo.bmWidth % 2)
					{
						CtlLineBytes += 1;
					}
				}
				else
				{
					CtlLineBytes = CtlBmpInfo.bmWidthBytes;
				}
				DWORD CtlMemSize = CtlLineBytes * CtlBmpInfo.bmHeight;

				// ビットマップを得るバッファを用意してバッファに情報を得る。
				BYTE *DstBuffer = new BYTE[DstMemSize];
				BgBitmap.GetBitmapBits(DstMemSize, DstBuffer);
				BYTE *CtlBuffer = new BYTE[CtlMemSize];
				CtrlBitmap.GetBitmapBits(CtlMemSize, CtlBuffer);

				if (m_CtrlImage.GetBPP() == 32 && m_Alpha < 255)
				{
					// 背景とコントロールをアルファ合成する。
					int baseY = m_CtrlSize.cy * no;
					for (LONG py = 0; py < DstBmpInfo.bmHeight; py++)
					{
						int dn = py * DstLineBytes;
						int cn = (baseY + py) * CtlLineBytes;
						for (LONG px = 0; px < DstBmpInfo.bmWidth; px++)
						{
							// 画像のアルファ値と指定したアルファ値を乗じたものを使用。
							int a = CtlBuffer[cn + 3] * m_Alpha / 255;
							int na = 255 - a;
							// 背景と合成する。
							DstBuffer[dn + 0] = (CtlBuffer[cn + 0] * a + DstBuffer[dn + 0] * na) / 255;
							DstBuffer[dn + 1] = (CtlBuffer[cn + 1] * a + DstBuffer[dn + 1] * na) / 255;
							DstBuffer[dn + 2] = (CtlBuffer[cn + 2] * a + DstBuffer[dn + 2] * na) / 255;
							// 次のデータ。
							dn += (DstBmpInfo.bmBitsPixel / 8);
							cn += (CtlBmpInfo.bmBitsPixel / 8);
						}
					}
				}
				else if (m_Alpha < 255)
				{
					// 背景とコントロールをアルファ合成する。
					int baseY = m_CtrlSize.cy * no;
					for (LONG py = 0; py < DstBmpInfo.bmHeight; py++)
					{
						int dn = py * DstLineBytes;
						int cn = (baseY + py) * CtlLineBytes;
						for (LONG px = 0; px < DstBmpInfo.bmWidth; px++)
						{
							// 指定したのアルファ値を使用。
							int a = m_Alpha;
							int na = 255 - a;
							// 背景と合成する。
							DstBuffer[dn + 0] = (BYTE)((CtlBuffer[cn + 0] * a + DstBuffer[dn + 0] * na) / 255);
							DstBuffer[dn + 1] = (BYTE)((CtlBuffer[cn + 1] * a + DstBuffer[dn + 1] * na) / 255);
							DstBuffer[dn + 2] = (BYTE)((CtlBuffer[cn + 2] * a + DstBuffer[dn + 2] * na) / 255);
							// 次のデータ。
							dn += (DstBmpInfo.bmBitsPixel / 8);
							cn += (CtlBmpInfo.bmBitsPixel / 8);
						}
					}
				}
				else
				{
					// 背景とコントロールをアルファ合成する。
					int baseY = m_CtrlSize.cy * no;
					for (LONG py = 0; py < DstBmpInfo.bmHeight; py++)
					{
						int dn = py * DstLineBytes;
						int cn = (baseY + py) * CtlLineBytes;
						for (LONG px = 0; px < DstBmpInfo.bmWidth; px++)
						{
							// 画像のアルファ値を得る。
							int a = CtlBuffer[cn + 3];
							int na = 255 - a;
							// 背景と合成する。
							DstBuffer[dn + 0] = (CtlBuffer[cn + 0] * a + DstBuffer[dn + 0] * na) / 255;
							DstBuffer[dn + 1] = (CtlBuffer[cn + 1] * a + DstBuffer[dn + 1] * na) / 255;
							DstBuffer[dn + 2] = (CtlBuffer[cn + 2] * a + DstBuffer[dn + 2] * na) / 255;
							// 次のデータ。
							dn += (DstBmpInfo.bmBitsPixel / 8);
							cn += (CtlBmpInfo.bmBitsPixel / 8);
						}
					}
				}

				// 描画用のビットマップに書き込み。
				DrawBmp.SetBitmapBits(DstMemSize, DstBuffer);

				// 描画用のビットマップに文字列を描画。
				DrawString(pDrawBmpDC, lpDrawItemStruct);

				// 画面に描画する。
				drawDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pDrawBmpDC, 0, 0, SRCCOPY);

				// バッファの解放。
				delete [] DstBuffer;
				delete [] CtlBuffer;
			}
			else
			{
				// コントロールをそのまま描画する。
				pDrawBmpDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pMemDC, 0, m_CtrlSize.cy * no, SRCCOPY);
				// 文字列を描画。
				DrawString(pDrawBmpDC, lpDrawItemStruct);
				// 画面に描画する。
				drawDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pDrawBmpDC, 0, 0, SRCCOPY);
			}
		}
		else
		{
			// 取り込んでいた背景を描画する。
			pDrawBmpDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pBgDC, 0, m_CtrlSize.cy * no, SRCCOPY);
			// 文字列を描画する。
			DrawString(pDrawBmpDC, lpDrawItemStruct);
			// 画面に描画する。
			drawDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, pDrawBmpDC, 0, 0, SRCCOPY);
		}
		// デバイスコンテキストのclean up
		pDrawBmpDC->SelectObject(&pOldDrawBitmap);
		pDrawBmpDC->DeleteDC();
		delete pDrawBmpDC;
	}

	// デバイスコンテキストのclean up
	pMemDC->SelectObject(&pOldMemBitmap);
	pBgDC->SelectObject(&pOldBgBitmap);
	pMemDC->DeleteDC();
	pBgDC->DeleteDC();
	delete pMemDC;
	delete pBgDC;

	#if CHECK_DRAWTIME
	// 計測結果
	EndMsTime = timeGetTime();		// 終了時間計測
	OutputDebugString( TEXT("----- 計測結果 ----\n") );
	CString msg;
	msg.Format( TEXT("%ums\n"), EndMsTime-StartMsTime );
	OutputDebugString( msg );
	OutputDebugString( TEXT("-------------------\n") );
#endif
}

// リソースからビットマップを割り当てる。
BOOL CButtonCx::LoadBitmap(UINT nIDResource, LPCTSTR pResourceType)
{
	if (m_bHighContrast) { return FALSE; }

	// リソース名を得る。
	LPCTSTR pResourceName = MAKEINTRESOURCE(nIDResource);
	
	// リソースハンドルを得る。
	HRSRC hResource = FindResource(NULL, pResourceName, pResourceType);
	if(hResource == NULL){return FALSE;}

	// リソースのサイズを得る。
	DWORD dwResourceSize = SizeofResource(NULL, hResource);
	if(dwResourceSize == 0){return FALSE;}

	HGLOBAL hGlobal = LoadResource(NULL, hResource);
	if (hGlobal == NULL) { return FALSE; }

	// リソースデータのポインタを得る。
	const void* pResourceData = LockResource(hGlobal);
	if(pResourceData == NULL){return FALSE;}

	// リソースのバッファを作成する。
	HGLOBAL hResourceBuffer = GlobalAlloc(GMEM_MOVEABLE, dwResourceSize);
	if(hResourceBuffer == NULL){return FALSE;}

	// メモリブロックをロックしてポインタを得る。
	void* pResourceBuffer = GlobalLock(hResourceBuffer);
	if(pResourceBuffer == NULL){GlobalFree(hResourceBuffer);return FALSE;}

	// リソースの内容をバッファにコピーする。
	CopyMemory(pResourceBuffer, pResourceData, dwResourceSize);
	IStream* pIStream = NULL;
	// グローバルメモリのストリームを作成する。
	if(CreateStreamOnHGlobal(hResourceBuffer, FALSE, &pIStream) == S_OK)
	{
		// リソースをロードする。
		m_CtrlImage.Load(pIStream);
		// リソースストリームを解放する。
		pIStream->Release();
		GlobalUnlock(pResourceBuffer);
		GlobalFree(hResourceBuffer);
	}
	else
	{
		// リソースを解放する。
		GlobalUnlock(pResourceBuffer);
		GlobalFree(hResourceBuffer);
		return FALSE;
	}
	
	// 失敗？
	if(m_CtrlImage.IsNull()){return FALSE;}
	
	// HBITMAPからビットマップを割り当てる。
	return LoadBitmap((HBITMAP)m_CtrlImage);
}

// ファイルからビットマップ割り当てる。
BOOL CButtonCx::LoadBitmap(LPCTSTR pFname)
{
	if (m_bHighContrast) { return FALSE; }
	// ファイル名が無効？
	if(pFname == NULL){return FALSE;}

	m_CtrlImage.Destroy();

	// ファイルをロードする。
	m_CtrlImage.Load(pFname);
	// 失敗？
	if(m_CtrlImage.IsNull()){return FALSE;}
	
	// HBITMAPからビットマップを割り当てる。
	return LoadBitmap((HBITMAP)m_CtrlImage);
}

// HBITMAPからビットマップを割り当てる。
BOOL CButtonCx::LoadBitmap(HBITMAP hbm)
{
	if (m_bHighContrast) { return FALSE; }
	m_CtrlBitmap.Detach();

	// ビットマップを設定する。
	m_CtrlBitmap.Attach(hbm);  // HBITMAP を CBitmap に結びつける。

	// ビットマップ情報を設定する。
	return SetBitmap(m_CtrlBitmap);
}

// ビットマップデータのセットアップ
BOOL CButtonCx::SetBitmap(CBitmap &cbitmap)
{
	if (m_bHighContrast) {return FALSE;}
	// コントロールのサイズを得る。
	BITMAP	bitmapinfo;
	cbitmap.GetBitmap(&bitmapinfo);
	if(m_CtrlSize.cx != bitmapinfo.bmWidth
	|| m_CtrlSize.cy != bitmapinfo.bmHeight / m_ImageCount)
	{
		// オーナードローを止める
		ModifyStyle(BS_OWNERDRAW, 0);
		return FALSE;
	}
	else
	{
		// オーナー描画に変更する。
		ModifyStyle(0, BS_OWNERDRAW);
		return TRUE;
	}
}

// コントロールサイズを得る。
CSize CButtonCx::GetSize(void)
{
	return m_CtrlSize;
}

// 背景を取り込む。
void CButtonCx::LoadCtrlBg(CDC* drawDC)
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

	// 画像が有効？
	if (&m_CtrlBitmap != NULL)
	{
			// 背景ビットマップは未初期化？
		if(! m_bBgBitmapInit)
		{
			// 背景ビットマップを初期化。
			m_BgBitmap.DeleteObject();
			m_BgBitmap.CreateCompatibleBitmap(drawDC, m_CtrlSize.cx, m_CtrlSize.cy);
			// 初期化済み
			m_bBgBitmapInit = TRUE;
		}
		
		// 背景はロード未済み？
		if(! m_bBgLoad)
		{
			CBitmap* pOldBitmap;
			// 背景をロードする。
			CDC* pMemDC = new CDC;
			pMemDC->CreateCompatibleDC(drawDC);
			pOldBitmap = pMemDC->SelectObject(&m_BgBitmap);
			pMemDC->BitBlt(0, 0, m_CtrlSize.cx, m_CtrlSize.cy, drawDC, 0, 0, SRCCOPY);
			// MemDCを破棄する。
			pMemDC->SelectObject(pOldBitmap);
			pMemDC->DeleteDC();
			delete pMemDC;
			// ロード済み
			m_bBgLoad = TRUE;
		}
	}
}

// 背景のリロードを設定(内部に取り込んでいるため)
void CButtonCx::SetBgReload(void)
{
	// 再ロードを指示。
	m_bBgBitmapInit = FALSE;
	m_bBgLoad = FALSE;
}

//------------------------------------------------
// マウスの移動関連
//------------------------------------------------

// マウスの移動
void CButtonCx::OnMouseMove(UINT nFlags, CPoint point) 
{
	// トラッキングしていない？
	if(! m_bTrackingNow)
	{
		// トラッキングをチェック。
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTrackingNow = _TrackMouseEvent(&tme);
	}
	CButton::OnMouseMove(nFlags, point);
}

// マウスが上にあるイベント。
void CButtonCx::OnMouseHover(UINT nFlags, CPoint point)
{
	CButton::OnMouseHover(nFlags, point);

	// ホバー有効
	m_bHover = TRUE;
	// 再描画
	Invalidate();
}

// マウスが上から離れた場合のイベント。
void CButtonCx::OnMouseLeave()
{
	CButton::OnMouseLeave();

	// トラッキングが外れた。
	m_bTrackingNow = FALSE;
	// ホバー無効
	m_bHover = FALSE;
	// 再描画
	Invalidate();
}

// フォーカスが有効
void CButtonCx::OnSetfocus()
{
	// フォーカス有効
	m_bFocas = TRUE;
	// 再描画
	Invalidate();
}

// フォーカスが無効
void CButtonCx::OnKillfocus()
{
	// フォーカス無効
	m_bFocas = FALSE;
	// 再描画
	Invalidate();
}

BOOL CButtonCx::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if(m_bHandCursor)
	{
		// ハンドカーソルに変更
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
	}
	else
	{
		// 通常カーソルに戻す
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	}
    return TRUE;
}

void CButtonCx::SetHandCursor(BOOL bHandCuror)
{
	m_bHandCursor = bHandCuror;
}

void CButtonCx::SetDrawFrame(BOOL bDrawFrame)
{
	if (bDrawFrame)
	{
		// スタティックエッジを有効
		ModifyStyleEx(0, WS_EX_STATICEDGE, SWP_DRAWFRAME);
	}
	else
	{
		// スタティックエッジを無効
		ModifyStyleEx(WS_EX_STATICEDGE, 0, SWP_DRAWFRAME);
	}
}

//------------------------------------------------
// フォント関連
//------------------------------------------------

void CButtonCx::SetFontEx(CString face, int size, double zoomRatio, BYTE textAlpha, COLORREF textColor, LONG fontWeight, INT fontType)
{
	LOGFONT logFont = {0};
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfHeight = (LONG)(-1 * size * zoomRatio);
	logFont.lfQuality = 6;
	logFont.lfWeight = fontWeight;
	if(face.GetLength() < 32)
	{
		wsprintf(logFont.lfFaceName, _T("%s"), face.GetString());
	}
	else
	{
		wsprintf(logFont.lfFaceName, _T(""));
	}

	m_Font.DeleteObject();
	m_Font.CreateFontIndirect(&logFont);
	SetFont(&m_Font);

	// フォント描画方法を設定します。
	if (FT_AUTO <= fontType && fontType <= FT_GDI_PLUS)
	{
		m_FontType = fontType;
	}
	else
	{
		m_FontType = FT_AUTO;
	}

	// ツールチップにフォントを設定します。
	if(m_ToolTip.m_hWnd != NULL)
	{
		m_ToolTip.SetFont(&m_Font);
	}

	CDC *pDC = GetDC();
	SAFE_DELETE(m_GpFont);
	SAFE_DELETE(m_GpBrush);
	SAFE_DELETE(m_GpStringformat);
	m_GpFont = new Gdiplus::Font(pDC->m_hDC, m_Font);
	m_GpBrush= new Gdiplus::SolidBrush(
		Gdiplus::Color(textAlpha, GetRValue(textColor), GetGValue(textColor), GetBValue(textColor)));
	m_GpStringformat = new Gdiplus::StringFormat;
	m_GpStringformat->SetAlignment(StringAlignmentCenter);
//	m_GpStringformat->SetLineAlignment(StringAlignmentCenter);
	m_GpStringformat->SetFormatFlags(StringFormatFlagsNoClip);
	ReleaseDC(pDC);
}


//------------------------------------------------
// 初期化関連
//------------------------------------------------

// 画像ファイルで初期化
BOOL CButtonCx::InitControl(int x, int y, int width, int height, double zoomRatio, LPCWSTR imagePath, UINT imageCount, UINT textAlign, UINT renderMode)
{
	m_ImagePath = imagePath;
	m_ImageCount = imageCount;
	MoveWindow((int)(x * zoomRatio), (int)(y * zoomRatio), (int)(width * zoomRatio), (int)(height * zoomRatio));
	m_CtrlSize.cx = (int)(width * zoomRatio);
	m_CtrlSize.cy = (int)(height * zoomRatio);

	m_RenderMode = renderMode;

	// ツール ヒント コントロールにツールを登録します
	if(m_ToolTip.m_hWnd != NULL)
	{
		// ツールを削除
		while(m_ToolTip.GetToolCount() != 0)
		{
			m_ToolTip.DelTool(this, 1);
		}
		// クライアント領域のサイズを取得します
		CRect rect; 
		GetClientRect(rect);
		m_ToolTip.AddTool(this, m_ToolTipText, rect, 1);
	}

	if(BS_LEFT <= textAlign && textAlign <= BS_CENTER)
	{
		m_TextAlign = textAlign;
	}

	if (renderMode & HighContrast)
	{
		m_bHighContrast = TRUE;
		// オーナー描画を止め、テキストアライメント再設定する。
		ModifyStyle(BS_OWNERDRAW, m_TextAlign);
		return TRUE;
	}
	else
	{
		m_bHighContrast = FALSE;
	}

	if(renderMode & SystemDraw)
	{
		// オーナー描画を止め、テキストアライメント再設定する。
		ModifyStyle(BS_OWNERDRAW, m_TextAlign);
		return TRUE;
	}
	SetBgReload();

	// オーナー描画に変更する。
	ModifyStyle(0, BS_OWNERDRAW);

	if(renderMode & OwnerDrawGlass)
	{
		m_CtrlImage.Destroy();
		m_CtrlImage.Create(m_CtrlSize.cx, m_CtrlSize.cy, 32);
		RECT rect;
		rect.left = rect.top = 0;
		rect.right = m_CtrlSize.cx;
		rect.bottom = m_CtrlSize.cy;
		CDC* pDC = CDC::FromHandle(m_CtrlImage.GetDC());
		pDC->SetDCPenColor(RGB(128, 128, 128));
		pDC->SelectObject(GetStockObject(DC_PEN));
		pDC->Rectangle(&rect);
		m_CtrlImage.ReleaseDC();

		m_CtrlBitmap.Detach();
		m_CtrlBitmap.Attach((HBITMAP)m_CtrlImage);
	
		DWORD length = m_CtrlSize.cx * m_CtrlSize.cy * 4;
		BYTE *bitmapBits = new BYTE[length];
		m_CtrlBitmap.GetBitmapBits(length, bitmapBits);
		
		for(int y = 0; y < m_CtrlSize.cy; y++)
		{
			for(int x = 0; x < m_CtrlSize.cx; x++)
			{
			//	bitmapBits[(y * m_CtrlSize.cx + x) * 4 + 0] = 255;
			//	bitmapBits[(y * m_CtrlSize.cx + x) * 4 + 1] = 255;
			//	bitmapBits[(y * m_CtrlSize.cx + x) * 4 + 2] = 255;
				bitmapBits[(y * m_CtrlSize.cx + x) * 4 + 3] = (BYTE)128;
			}
		}
		m_CtrlBitmap.SetBitmapBits(length, bitmapBits);
		delete [] bitmapBits;
	}
	else if(renderMode & OwnerDrawImage)
	{
		if (!LoadBitmap(imagePath))
		{
			// オーナー描画を止め、テキストアライメント再設定する。
			ModifyStyle(BS_OWNERDRAW, m_TextAlign);
		}
	}

	// 再描画
	Invalidate();
	return TRUE;
}

BOOL CButtonCx::ReloadImage(LPCWSTR imagePath, UINT imageCount)
{
	// 同じファイルはリロードしない
	if(imagePath != NULL && m_ImagePath.Compare(imagePath) == 0)
	{
		return FALSE;
	}
	m_ImagePath = imagePath;
	m_ImageCount = imageCount;
	
	LoadBitmap(imagePath);
	
	// 再描画
	Invalidate();
	return TRUE;
}

void CButtonCx::SetMargin(int top, int left, int bottom, int right, double zoomRatio)
{
	m_Margin.top = (int)(top * zoomRatio);
	m_Margin.left = (int)(left * zoomRatio);
	m_Margin.bottom = (int)(bottom * zoomRatio);
	m_Margin.right = (int)(right * zoomRatio);
}

void CButtonCx::SetAlpha(BYTE alpha)
{
	m_Alpha = alpha;
}

void CButtonCx::SetSelected(BOOL bSelected)
{
	m_bSelected = bSelected;
	Invalidate();
}

