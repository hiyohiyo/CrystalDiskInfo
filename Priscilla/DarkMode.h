/*---------------------------------------------------------------------------*/
//       Author : Richard Yu
//          Web : https://github.com/ysc3839/win32-darkmode
//      License : The MIT License
//                https://github.com/ysc3839/win32-darkmode/blob/master/LICENSE
/*---------------------------------------------------------------------------*/

#pragma once

BOOL SetDarkMode(HWND hWnd);
void UnsetDarkMode(HWND hWnd);
void SetDarkModeControl(HWND hWnd, BOOL bDarkMode);

void FixDarkScrollBar();
bool AllowDarkModeForWindow(HWND hWnd, bool allow);

// BOOL InitDarkMode();
// void RefreshTitleBarThemeColor(HWND hWnd);
