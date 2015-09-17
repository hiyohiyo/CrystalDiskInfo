#pragma once
#include <atlimage.h>

#include <gdiplus.h>
#pragma comment(lib, "Gdiplus.lib")
using namespace Gdiplus;

#define SAFE_DELETE(p) {if(p){delete (p);(p)=NULL;}}

class CStaticCx : public CStatic
{
	DECLARE_DYNAMIC(CStaticCx);

	static const int HighContrast         = 0x0001;
	static const int SystemDraw           = 0x0002;
	static const int OwnerDrawImage       = 0x0004;
	static const int OwnerDrawGlass       = 0x0008;
	static const int OwnerDrawTransparent = 0x0010;

	static const int FT_AUTO              = 0x0000;
	static const int FT_GDI               = 0x0001;
	static const int FT_GDI_PLUS          = 0x0002;
	static const int FT_D_WRITE           = 0x0003;

// 基本
public:
	CStaticCx();
	virtual ~CStaticCx();

// 外部関数
public:
	// 初期化
	BOOL InitControl(int x, int y, int width, int height, double zoomRatio, LPCWSTR imagePath, UINT imageCount, UINT textAlign = SS_LEFT, UINT renderMode = OwnerDrawImage);
	BOOL ReloadImage(LPCWSTR imagePath, UINT imageCount);

	// 画像関係
	BOOL LoadBitmap(UINT nIDResource, LPCTSTR pResourceType);// リソースからビットマップを割り当てる。
	BOOL LoadBitmap(LPCTSTR pFname);				// ファイルからビットマップ割り当てる。
	BOOL LoadBitmap(HBITMAP hbm);					// HBITMAPからビットマップを割り当てる。
	CSize GetSize(void);							// コントロールサイズを得る。
	void SetBgReload(void);							// 背景のリロードを設定(内部に取り込んでいるため)
	void SetAlpha(BYTE alpha);						// 画像用の Alpha 値
	void SetDrawFrame(BOOL bDrawFrame);				// フレームを描画するかどうかを制御する。
	// ツールチップ関係
	void SetToolTipText(LPCTSTR pText);				// ツールチップのメッセージを設定する。
	void SetToolTipActivate(BOOL bActivate = TRUE);	// ツールチップの有効無効を切り替えます。
	void SetToolTipWindowText(LPCTSTR pText);		// コントロールとツールチップのメッセージを設定する。
	// フォント関係
	void SetFontEx(CString face, int size, double zoomRatio, BYTE textAlpha = 255, COLORREF textColor = RGB(0, 0, 0), LONG fontWeight = FW_NORMAL, int fontType = FT_GDI); // フォントを設定します。
	void SetMargin(int top, int left, int bottom, int right, double zoomRatio); // テキスト描画用のマージンを設定します。
	// カーソル関係
	void SetHandCursor(BOOL bHandCuror = TRUE);	// カーソルモードを設定します。

// 内部変数。
private:
	// ホバー制御
	BOOL m_bHover;			// ホバー中のフラグ。
	BOOL m_bFocas;			// フォーカス中のフラグ。
	BOOL m_bTrackingNow;	// トラッキング中のフラグ。
	// 画像関連
	CBitmap m_CtrlBitmap;	// コントロールの画像(ビットマップ)。
	CImage m_CtrlImage;		// コントロールの画像(イメージ)。
	CSize m_CtrlSize;		// コントロールのサイズ。
	// ボタン背景
	CBitmap m_BgBitmap;		// コントロールの背景(ビットマップ)
	BOOL m_bBgBitmapInit;	// コントロールの背景(ビットマップ)の初期化済み
	BOOL m_bBgLoad;			// コントロールの背景(ビットマップ)の取り込み済みフラグ
	// ツールチップ関係
	CToolTipCtrl m_ToolTip;	// ツールチップ。
	CString m_ToolTipText;  // ツールチップ用テキスト
	// フォント関係
	CFont m_Font;
	// GDI+ 関連
	Gdiplus::Font *m_GpFont;
	Gdiplus::SolidBrush *m_GpBrush;
	Gdiplus::StringFormat *m_GpStringformat;

	BOOL m_bHighContrast;	// ハイコントラスト用のフラグ
	UINT m_RenderMode;		// レンダリングモード
	UINT m_TextAlign;		// テキストアラインモード

	UINT m_ImageCount;		// 保持イメージ数
	CString m_ImagePath;	// イメージのパス

	BOOL m_bHandCursor;		// ハンドカーソルモード
	CRect m_Margin;			// テキスト描画マージン
	BYTE m_Alpha;			// 画像用のアルファ値
	int m_FontType;		    // 描画フォントの使用テクノロジー

// 内部関数。
private:
	void InitToolTip();					// ツールチップの初期化。
	BOOL SetBitmap(CBitmap &cbitmap);	// ビットマップデータのセットアップ
	void LoadCtrlBg(CDC *drawDC);		// 背景のロード
	void DrawControl(CDC *drawDC, LPDRAWITEMSTRUCT lpDrawItemStruct, CBitmap &CtrlBitmap, CBitmap &BgBitmap, int no);	// ボタンの描画
	virtual void DrawString(CDC *drawDC, LPDRAWITEMSTRUCT lpDrawItemStruct);

// オーバーライド
private:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnKillfocus();
	afx_msg void OnSetfocus();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	DECLARE_MESSAGE_MAP()
};
