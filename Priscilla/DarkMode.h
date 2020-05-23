/*---------------------------------------------------------------------------*/
//       Author : Richard Yu
//          Web : https://github.com/ysc3839/win32-darkmode
//      License : The MIT License
//                https://github.com/ysc3839/win32-darkmode/blob/master/LICENSE
/*---------------------------------------------------------------------------*/

#pragma once

BOOL InitDarkMode();
bool AllowDarkModeForWindow(HWND hWnd, bool allow);
void RefreshTitleBarThemeColor(HWND hWnd);