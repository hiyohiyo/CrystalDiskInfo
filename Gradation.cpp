#pragma once
#include "stdafx.h"
#include "Gradation.h"

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
)
{
    TRIVERTEX vert[2];
    GRADIENT_RECT rect = {0, 1};

    // 描画範囲と色を設定します
    vert[0].x = pRect->left;
    vert[0].y = pRect->top;
    vert[0].Red   = GetRValue(Color1) << 8;
    vert[0].Green = GetGValue(Color1) << 8;
    vert[0].Blue  = GetBValue(Color1) << 8;
    vert[0].Alpha = 0;
    vert[1].x = pRect->right;
    vert[1].y = pRect->bottom;
    vert[1].Red   = GetRValue(Color2) << 8;
    vert[1].Green = GetGValue(Color2) << 8;
    vert[1].Blue  = GetBValue(Color2) << 8;
    vert[1].Alpha = 0;
    return GradientFill(hdc, vert, 2, &rect, 1,
                        fHorizontal ? GRADIENT_FILL_RECT_H : GRADIENT_FILL_RECT_V);
}

// 二つの色を混ぜ合わせる関数です
COLORREF MixColor(COLORREF Color1, COLORREF Color2, BYTE Ratio)
{
    int Alpha = 255 - Ratio;
    return RGB((GetRValue(Color1) * Ratio + GetRValue(Color2) * Alpha) / 255,
               (GetGValue(Color1) * Ratio + GetGValue(Color2) * Alpha) / 255,
               (GetBValue(Color1) * Ratio + GetBValue(Color2) * Alpha) / 255);
}

// テカったグラデーションを描画する関数です
void GlossyGradient(
    HDC hdc,                   // 描画先のデバイスコンテキスト・ハンドルです
    const RECT *pRect,         // 描画する範囲の矩形です
    COLORREF Color1,           // 描画する一つ目の色です
    COLORREF Color2,           // 描画する二つ目の色です
    BOOL fHorizontal,          // 水平のグラデーションを描画する場合は TRUE にします
    BYTE GlossRatio1,          // 端のテカりの強さです
    BYTE GlossRatio2           // 中央のテカりの強さです
)
{
    // 中央の色を求めます
    COLORREF CenterColor = MixColor(Color1, Color2);

    RECT rc;

    // テカり部分の範囲を計算します(描画領域の半分です)
    rc.left = pRect->left;
    rc.top  = pRect->top;
    if (fHorizontal) {
        rc.right  = (rc.left + pRect->right) / 2;
        rc.bottom = pRect->bottom;
    } else {
        rc.right  = pRect->right;
        rc.bottom = (rc.top + pRect->bottom) / 2;
    }
    // テカり部分を描画します
    TwoColorsGradient(hdc, &rc,
                      MixColor(RGB(255, 255, 255), Color1, GlossRatio1),
                      MixColor(RGB(255, 255, 255), CenterColor, GlossRatio2),
                      fHorizontal);

    // テカり以外の部分を描画します
    if (fHorizontal) {
        rc.left  = rc.right;
        rc.right = pRect->right;
    } else {
        rc.top    = rc.bottom;
        rc.bottom = pRect->bottom;
    }
    TwoColorsGradient(hdc, &rc, CenterColor, Color2, fHorizontal);
//	TwoColorsGradient(hdc, &rc,
//                   MixColor(RGB(255, 255, 255), CenterColor, GlossRatio2),
//                    MixColor(RGB(255, 255, 255), Color1, GlossRatio1),
//                   fHorizontal);
}