#pragma once

//------------------------------------------------
// グラデーション http://www.geocities.jp/iooiau/tips/web20back.html
//------------------------------------------------
// 2色のグラデーションを描画する関数です
BOOL TwoColorsGradient(
    HDC hdc,            // 描画先のデバイスコンテキスト・ハンドルです
    const RECT *pRect,  // 描画する範囲の矩形です
    COLORREF Color1,    // 描画する一つ目の色です
    COLORREF Color2,    // 描画する二つ目の色です
    BOOL fHorizontal    // 水平のグラデーションを描画する場合は TRUE にします
);

// 二つの色を混ぜ合わせる関数です
COLORREF MixColor(COLORREF Color1, COLORREF Color2, BYTE Ratio = 128);

// テカったグラデーションを描画する関数です
void GlossyGradient(
    HDC hdc,                   // 描画先のデバイスコンテキスト・ハンドルです
    const RECT *pRect,         // 描画する範囲の矩形です
    COLORREF Color1,           // 描画する一つ目の色です
    COLORREF Color2,           // 描画する二つ目の色です
    BOOL fHorizontal = FALSE,  // 水平のグラデーションを描画する場合は TRUE にします
    BYTE GlossRatio1 = 96,     // 端のテカりの強さです
    BYTE GlossRatio2 = 48      // 中央のテカりの強さです
);