/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DiskInfo.h"
#include "DiskInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern UINT gRegMessageId;
extern UINT gRegIconId;

void CDiskInfoDlg::TaskTrayRightMenu(DWORD index)
{
	POINT pt;
	CreateMainMenu(index);
	SetForegroundWindow();
	GetCursorPos(&pt);
	TrackPopupMenu(m_hMenu, TPM_RIGHTALIGN|TPM_BOTTOMALIGN, pt.x, pt.y, 0, m_hWnd, NULL);
	PostMessage(WM_NULL);
}

LRESULT CDiskInfoDlg::OnTempIcon0(WPARAM wParam, LPARAM lParam)
{
	if(lParam == WM_LBUTTONDOWN)
	{
		ShowWindowEx(SW_RESTORE);
		SelectDrive(0);
	}
	else if(lParam == WM_RBUTTONDOWN)
	{
		TaskTrayRightMenu(0);
	}
	return 0;
}

LRESULT CDiskInfoDlg::OnTempIcon1(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(1);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(1);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon2(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(2);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(2);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon3(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(3);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(3);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon4(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(4);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(4);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon5(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(5);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(5);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon6(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(6);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(6);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon7(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(7);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(7);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon8(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(8);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(8);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon9(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(9);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(9);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon10(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(10);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(10);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon11(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(11);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(11);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon12(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(12);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(12);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon13(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(13);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(13);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon14(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(14);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(14);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon15(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(15);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(15);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon16(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(16);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(16);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon17(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(17);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(17);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon18(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(18);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(18);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon19(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(19);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(19);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon20(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(20);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(20);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon21(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(21);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(21);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon22(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(22);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(22);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon23(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(23);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(23);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon24(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(24);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(24);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon25(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(25);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(25);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon26(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(26);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(26);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon27(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(27);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(27);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon28(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(28);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(28);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon29(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(29);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(29);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon30(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(30);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(30);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon31(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(31);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(31);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon32(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(32);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(32);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon33(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(33);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(33);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon34(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(34);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(34);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon35(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(35);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(35);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon36(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(36);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(36);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon37(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(37);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(37);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon38(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(38);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(38);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon39(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(39);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(39);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon40(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(40);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(40);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon41(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(41);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(41);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon42(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(42);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(42);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon43(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(43);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(43);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon44(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(44);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(44);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon45(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(45);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(45);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon46(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(46);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(46);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon47(WPARAM w, LPARAM l){if(l == WM_LBUTTONDOWN){SelectDrive(47);ShowWindowEx(SW_RESTORE);}else if(l == WM_RBUTTONDOWN){TaskTrayRightMenu(47);}return 0;}
LRESULT CDiskInfoDlg::OnTempIcon48(WPARAM w, LPARAM l){ if (l == WM_LBUTTONDOWN){ SelectDrive(48); ShowWindowEx(SW_RESTORE); } else if (l == WM_RBUTTONDOWN){ TaskTrayRightMenu(48); }return 0; }
LRESULT CDiskInfoDlg::OnTempIcon49(WPARAM w, LPARAM l){ if (l == WM_LBUTTONDOWN){ SelectDrive(49); ShowWindowEx(SW_RESTORE); } else if (l == WM_RBUTTONDOWN){ TaskTrayRightMenu(49); }return 0; }
LRESULT CDiskInfoDlg::OnTempIcon50(WPARAM w, LPARAM l){ if (l == WM_LBUTTONDOWN){ SelectDrive(50); ShowWindowEx(SW_RESTORE); } else if (l == WM_RBUTTONDOWN){ TaskTrayRightMenu(50); }return 0; }
LRESULT CDiskInfoDlg::OnTempIcon51(WPARAM w, LPARAM l){ if (l == WM_LBUTTONDOWN){ SelectDrive(51); ShowWindowEx(SW_RESTORE); } else if (l == WM_RBUTTONDOWN){ TaskTrayRightMenu(51); }return 0; }
LRESULT CDiskInfoDlg::OnTempIcon52(WPARAM w, LPARAM l){ if (l == WM_LBUTTONDOWN){ SelectDrive(52); ShowWindowEx(SW_RESTORE); } else if (l == WM_RBUTTONDOWN){ TaskTrayRightMenu(52); }return 0; }
LRESULT CDiskInfoDlg::OnTempIcon53(WPARAM w, LPARAM l){ if (l == WM_LBUTTONDOWN){ SelectDrive(53); ShowWindowEx(SW_RESTORE); } else if (l == WM_RBUTTONDOWN){ TaskTrayRightMenu(53); }return 0; }
LRESULT CDiskInfoDlg::OnTempIcon54(WPARAM w, LPARAM l){ if (l == WM_LBUTTONDOWN){ SelectDrive(54); ShowWindowEx(SW_RESTORE); } else if (l == WM_RBUTTONDOWN){ TaskTrayRightMenu(54); }return 0; }
LRESULT CDiskInfoDlg::OnTempIcon55(WPARAM w, LPARAM l){ if (l == WM_LBUTTONDOWN){ SelectDrive(55); ShowWindowEx(SW_RESTORE); } else if (l == WM_RBUTTONDOWN){ TaskTrayRightMenu(55); }return 0; }
LRESULT CDiskInfoDlg::OnTempIcon56(WPARAM w, LPARAM l){ if (l == WM_LBUTTONDOWN){ SelectDrive(56); ShowWindowEx(SW_RESTORE); } else if (l == WM_RBUTTONDOWN){ TaskTrayRightMenu(56); }return 0; }
LRESULT CDiskInfoDlg::OnTempIcon57(WPARAM w, LPARAM l){ if (l == WM_LBUTTONDOWN){ SelectDrive(57); ShowWindowEx(SW_RESTORE); } else if (l == WM_RBUTTONDOWN){ TaskTrayRightMenu(57); }return 0; }
LRESULT CDiskInfoDlg::OnTempIcon58(WPARAM w, LPARAM l){ if (l == WM_LBUTTONDOWN){ SelectDrive(58); ShowWindowEx(SW_RESTORE); } else if (l == WM_RBUTTONDOWN){ TaskTrayRightMenu(58); }return 0; }
LRESULT CDiskInfoDlg::OnTempIcon59(WPARAM w, LPARAM l){ if (l == WM_LBUTTONDOWN){ SelectDrive(59); ShowWindowEx(SW_RESTORE); } else if (l == WM_RBUTTONDOWN){ TaskTrayRightMenu(59); }return 0; }
LRESULT CDiskInfoDlg::OnTempIcon60(WPARAM w, LPARAM l){ if (l == WM_LBUTTONDOWN){ SelectDrive(60); ShowWindowEx(SW_RESTORE); } else if (l == WM_RBUTTONDOWN){ TaskTrayRightMenu(60); }return 0; }
LRESULT CDiskInfoDlg::OnTempIcon61(WPARAM w, LPARAM l){ if (l == WM_LBUTTONDOWN){ SelectDrive(61); ShowWindowEx(SW_RESTORE); } else if (l == WM_RBUTTONDOWN){ TaskTrayRightMenu(61); }return 0; }
LRESULT CDiskInfoDlg::OnTempIcon62(WPARAM w, LPARAM l){ if (l == WM_LBUTTONDOWN){ SelectDrive(62); ShowWindowEx(SW_RESTORE); } else if (l == WM_RBUTTONDOWN){ TaskTrayRightMenu(62); }return 0; }
LRESULT CDiskInfoDlg::OnTempIcon63(WPARAM w, LPARAM l){ if (l == WM_LBUTTONDOWN){ SelectDrive(63); ShowWindowEx(SW_RESTORE); } else if (l == WM_RBUTTONDOWN){ TaskTrayRightMenu(63); }return 0; }


LRESULT CDiskInfoDlg::OnRegMessage(WPARAM wParam, LPARAM lParam)
{
	if(lParam == WM_LBUTTONDOWN && m_FlagResidentMinimize)
	{
		if(! IsIconic())
		{
			ShowWindowEx(SW_MINIMIZE);
		}
		else
		{
			ShowWindowEx(SW_RESTORE);
		}
	}
	else if(lParam == WM_LBUTTONDOWN)
	{
		if(IsWindowVisible())
		{
			ShowWindowEx(SW_HIDE);
		}
		else
		{
			ShowWindowEx(SW_RESTORE);
		}
	}
	else if(lParam == WM_RBUTTONDOWN)
	{
		TaskTrayRightMenu(CAtaSmart::MAX_DISK);
	}
	return 0;
}

LRESULT CDiskInfoDlg::OnTaskbarCreated(WPARAM, LPARAM) 
{
	if(m_FlagResident)
	{
		for(int i = 0; i < m_Ata.vars.GetCount(); i++)
		{
			if(m_FlagTrayTemperatureIcon[i])
			{
				m_FlagTrayTemperatureIcon[i] = FALSE; // Force Add Temperature Icon
				AddTemperatureIcon(i);
			}
		}
		if(m_FlagTrayMainIcon)
		{
			m_FlagTrayMainIcon = FALSE; // Force Add Main Icon
			AddTrayMainIcon();
		}
	}

	return 0;
}

void CDiskInfoDlg::CreateMainMenu(DWORD index)
{
	CString cstr;

	if(m_hMenu != NULL)
	{
		DestroyMenu(m_hMenu);
	}

	m_hMenu = CreatePopupMenu();
	HMENU hSubMenu = CreateMenu();


	HMENU hDiskTemp[CAtaSmart::MAX_DISK + 1];
	HMENU hDiskMenu[CAtaSmart::MAX_DISK + 1];
	HMENU hDiskHealth;

	MENUITEMINFO menuInfo;
	ZeroMemory(&menuInfo, sizeof(MENUITEMINFO));
	menuInfo.cbSize = sizeof(MENUITEMINFO);
	menuInfo.fMask = MIIM_CHECKMARKS|MIIM_TYPE|MIIM_STATE|MIIM_ID|MIIM_SUBMENU;
	menuInfo.fType = MFT_RADIOCHECK;
	menuInfo.hbmpChecked = NULL;
	menuInfo.fState = MFS_UNCHECKED;

	MENUITEMINFO subMenuInfo;
	ZeroMemory(&subMenuInfo, sizeof(MENUITEMINFO));
	subMenuInfo.cbSize = sizeof(MENUITEMINFO);
	subMenuInfo.fMask = MIIM_CHECKMARKS|MIIM_TYPE|MIIM_STATE|MIIM_ID|MIIM_SUBMENU;
	subMenuInfo.fType = MFT_RADIOCHECK;
	subMenuInfo.hbmpChecked = NULL;
	subMenuInfo.fState = MFS_UNCHECKED;
	subMenuInfo.hSubMenu = NULL;

	MENUITEMINFO subSubMenuInfo;
	ZeroMemory(&subSubMenuInfo, sizeof(MENUITEMINFO));
	subSubMenuInfo.cbSize = sizeof(MENUITEMINFO);
	subSubMenuInfo.fMask = MIIM_CHECKMARKS|MIIM_TYPE|MIIM_STATE|MIIM_ID|MIIM_SUBMENU;
	subSubMenuInfo.fType = MFT_RADIOCHECK;
	subSubMenuInfo.hbmpChecked = NULL;
	subSubMenuInfo.fState = MFS_UNCHECKED;
	subSubMenuInfo.hSubMenu = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////////
// Alarm Temperature & Temperature Icon
/////////////////////////////////////////////////////////////////////////////////////////////////
	for(int i = 0; i < m_Ata.vars.GetCount(); i++)
	{
		hDiskTemp[i] = CreateMenu();
		hDiskMenu[i] = CreateMenu();
		for(int j = 20; j <= 80; j++)
		{
			if(m_FlagFahrenheit)
			{
				cstr.Format(_T("%d C (%d F)"), j, j * 9 / 5 + 32);
			}
			else
			{
				cstr.Format(_T("%d C"), j);
			}

			subSubMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
			subSubMenuInfo.wID = ALARM_SETTING_TEMPERATURE_BASE + i * 100 + j;
			if(m_Ata.vars[i].AlarmTemperature == j)
			{
				subSubMenuInfo.fState = MFS_CHECKED;
			}
			else
			{
				subSubMenuInfo.fState = MFS_UNCHECKED;
			}
			InsertMenuItem(hDiskTemp[i], -1, TRUE, &subSubMenuInfo);
		}
		cstr = i18n(_T("TrayMenu"), _T("DISABLE"));
		subSubMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
		subSubMenuInfo.wID = ALARM_SETTING_TEMPERATURE_BASE + i * 100 + 0;
		if(m_Ata.vars[i].AlarmTemperature == 0)
		{
			subSubMenuInfo.fState = MFS_CHECKED;
		}
		else
		{
			subSubMenuInfo.fState = MFS_UNCHECKED;
		}
		InsertMenuItem(hDiskTemp[i], -1, TRUE, &subSubMenuInfo);

		cstr = i18n(_T("TrayMenu"), _T("ALARM_SETTING_TEMPERATURE"));
		subMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
		subMenuInfo.wID= 0;
		if(m_Ata.vars[i].Temperature <= 0)
		{
			subMenuInfo.fState = MFS_DISABLED;
		}
		else
		{
			subMenuInfo.fState = MFS_UNCHECKED;
		}
		subMenuInfo.hSubMenu = hDiskTemp[i];
		InsertMenuItem(hDiskMenu[i], -1, TRUE, &subMenuInfo);

		subMenuInfo.fType = 0;
		cstr = i18n(_T("TrayMenu"), _T("ALARM_SETTING_HEALTH_STATUS"));
		subMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
		if(m_Ata.vars[i].AlarmHealthStatus)
		{
			subMenuInfo.fState = MFS_CHECKED;
		}
		else
		{
			subMenuInfo.fState = MFS_UNCHECKED;
		}
		subMenuInfo.wID = ALARM_SETTING_HEALTH_STATUS_BASE + i;
		subMenuInfo.hSubMenu = NULL;
		InsertMenuItem(hDiskMenu[i], -1, TRUE, &subMenuInfo);

		cstr = i18n(_T("TrayMenu"), _T("SHOW_TEMPERATURE_ICON"));
		subMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
		subMenuInfo.fType = 0;

		if(m_Ata.vars[i].Temperature <= 0)
		{
			subMenuInfo.fState = MFS_DISABLED;
		}
		else if(m_FlagTrayTemperatureIcon[i])
		{
			subMenuInfo.fState = MFS_CHECKED;
		}
		else
		{
			subMenuInfo.fState = MFS_UNCHECKED;
		}
		subMenuInfo.wID = TRAY_TEMPERATURE_ICON_BASE + i;
		subMenuInfo.hSubMenu = NULL;
		InsertMenuItem(hDiskMenu[i], -1, TRUE, &subMenuInfo);
		
		cstr = i18n(_T("TrayMenu"), _T("AUTO_REFRESH_TARGET"));
		subMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
		subMenuInfo.fType = 0;

		if(m_FlagAutoRefreshTarget[i])
		{
			subMenuInfo.fState = MFS_CHECKED;
		}
		else
		{
			subMenuInfo.fState = MFS_UNCHECKED;
		}
		subMenuInfo.wID = AUTO_REFRESH_TARGET_BASE + i;
		subMenuInfo.hSubMenu = NULL;
		InsertMenuItem(hDiskMenu[i], -1, TRUE, &subMenuInfo);

		CString cstr;
		cstr.Format(_T("(%d) %s"), i + 1, m_Ata.vars[i].Model);
		menuInfo.dwTypeData = (LPWSTR)cstr.GetString();
		menuInfo.wID = 0;
		menuInfo.hSubMenu = hDiskMenu[i];
		InsertMenuItem(m_hMenu, -1, TRUE, &menuInfo);
	}

	//	Collective Setting
	hDiskTemp[CAtaSmart::MAX_DISK] = CreateMenu();
	hDiskMenu[CAtaSmart::MAX_DISK] = CreateMenu();
	hDiskHealth = CreateMenu();
	for(int j = 20; j <= 80; j++)
	{
		if(m_FlagFahrenheit)
		{
			cstr.Format(_T("%d C (%d F)"), j, j * 9 / 5 + 32);
		}
		else
		{
			cstr.Format(_T("%d C"), j);
		}
		subSubMenuInfo.fState = MFS_UNCHECKED;
		subSubMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
		subSubMenuInfo.wID = ALARM_SETTING_TEMPERATURE_BASE + CAtaSmart::MAX_DISK * 100 + j;
		InsertMenuItem(hDiskTemp[CAtaSmart::MAX_DISK], -1, TRUE, &subSubMenuInfo);
	}
	subMenuInfo.fState = MFS_UNCHECKED;
	cstr = i18n(_T("TrayMenu"), _T("DISABLE"));
	subSubMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	subSubMenuInfo.wID = ALARM_SETTING_TEMPERATURE_BASE + CAtaSmart::MAX_DISK * 100 + 0;
	InsertMenuItem(hDiskTemp[CAtaSmart::MAX_DISK], -1, TRUE, &subSubMenuInfo);

	cstr = i18n(_T("TrayMenu"), _T("ALARM_SETTING_TEMPERATURE"));
	subMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	subMenuInfo.wID= 0;
	subMenuInfo.hSubMenu = hDiskTemp[CAtaSmart::MAX_DISK];
	InsertMenuItem(hDiskMenu[CAtaSmart::MAX_DISK], -1, TRUE, &subMenuInfo);

	cstr = i18n(_T("TrayMenu"), _T("ENABLE_ALL"));
	subSubMenuInfo.fState = MFS_UNCHECKED;
	subSubMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	subSubMenuInfo.wID = ALARM_SETTING_HEALTH_STATUS_BASE + CAtaSmart::MAX_DISK;
	InsertMenuItem(hDiskHealth, -1, TRUE, &subSubMenuInfo);

	cstr = i18n(_T("TrayMenu"), _T("DISABLE_ALL"));
	subSubMenuInfo.fState = MFS_UNCHECKED;
	subSubMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	subSubMenuInfo.wID = ALARM_SETTING_HEALTH_STATUS_BASE + CAtaSmart::MAX_DISK + 1;
	InsertMenuItem(hDiskHealth, -1, TRUE, &subSubMenuInfo);

	// All Alarm Health Status == CAtaSmart::MAX_DISK
	cstr = i18n(_T("TrayMenu"), _T("ALARM_SETTING_HEALTH_STATUS"));
	subMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	subMenuInfo.fType = 0;
	subMenuInfo.wID = 0;//ALARM_SETTING_HEALTH_STATUS_BASE + CAtaSmart::MAX_DISK;
	subMenuInfo.hSubMenu = hDiskHealth;
	InsertMenuItem(hDiskMenu[CAtaSmart::MAX_DISK], -1, TRUE, &subMenuInfo);

	// Show All == CAtaSmart::MAX_DISK
	cstr = i18n(_T("TrayMenu"), _T("SHOW_ALL_TEMPERATURE_ICON"));
	subMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	subMenuInfo.fType = 0;
	subMenuInfo.wID = TRAY_TEMPERATURE_ICON_BASE + CAtaSmart::MAX_DISK;
	subMenuInfo.hSubMenu = NULL;
	InsertMenuItem(hDiskMenu[CAtaSmart::MAX_DISK], -1, TRUE, &subMenuInfo);

	// Hide All == CAtaSmart::MAX_DISK + 1
	cstr = i18n(_T("TrayMenu"), _T("HIDE_ALL_TEMPERATURE_ICON"));
	subMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	subMenuInfo.fType = 0;
	subMenuInfo.wID = TRAY_TEMPERATURE_ICON_BASE + CAtaSmart::MAX_DISK + 1;
	subMenuInfo.hSubMenu = NULL;
	InsertMenuItem(hDiskMenu[CAtaSmart::MAX_DISK], -1, TRUE, &subMenuInfo);

	// Target Auto Refresh All Disk == CAtaSmart::MAX_DISK
	cstr = i18n(_T("TrayMenu"), _T("AUTO_REFRESH_TARGET_ALL_DISK"));
	subMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	subMenuInfo.fType = 0;
	subMenuInfo.wID= AUTO_REFRESH_TARGET_BASE + CAtaSmart::MAX_DISK;
	subMenuInfo.hSubMenu = NULL;
	InsertMenuItem(hDiskMenu[CAtaSmart::MAX_DISK], -1, TRUE, &subMenuInfo);

	// Untarget Auto Refresh All Disk == CAtaSmart::MAX_DISK + 1
	cstr = i18n(_T("TrayMenu"), _T("AUTO_REFRESH_UNTARGET_ALL_DISK"));
	subMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	subMenuInfo.fType = 0;
	subMenuInfo.wID= AUTO_REFRESH_TARGET_BASE + CAtaSmart::MAX_DISK + 1;
	subMenuInfo.hSubMenu = NULL;
	InsertMenuItem(hDiskMenu[CAtaSmart::MAX_DISK], -1, TRUE, &subMenuInfo);

	cstr = i18n(_T("TrayMenu"), _T("COLLECTIVE_SETTING"));
	menuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	menuInfo.wID = 0;
	menuInfo.hSubMenu = hDiskMenu[CAtaSmart::MAX_DISK];
	InsertMenuItem(m_hMenu, -1, TRUE, &menuInfo);

	// Separator
	menuInfo.fType = MFT_SEPARATOR;
	InsertMenuItem(m_hMenu, -1, TRUE, &menuInfo);
	menuInfo.fType = MFT_RADIOCHECK;

/////////////////////////////////////////////////////////////////////////////////////////////////
// Auto Refresh
/////////////////////////////////////////////////////////////////////////////////////////////////
	subMenuInfo.fType = MFT_RADIOCHECK;
	cstr = _T("1 ") + i18n(_T("Menu"), _T("MINUTE"));
	subMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	subMenuInfo.wID= ID_AUTO_REFRESH_01_MIN;
	if(m_AutoRefreshStatus == 1){subMenuInfo.fState = MFS_CHECKED;}else{subMenuInfo.fState = MFS_UNCHECKED;}
	InsertMenuItem(hSubMenu, -1, TRUE, &subMenuInfo);

	cstr = _T("3 ") + i18n(_T("Menu"), _T("MINUTE"));
	subMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	subMenuInfo.wID= ID_AUTO_REFRESH_03_MIN;
	if(m_AutoRefreshStatus == 3){subMenuInfo.fState = MFS_CHECKED;}else{subMenuInfo.fState = MFS_UNCHECKED;}
	InsertMenuItem(hSubMenu, -1, TRUE, &subMenuInfo);

	cstr = _T("5 ") + i18n(_T("Menu"), _T("MINUTE"));
	subMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	subMenuInfo.wID= ID_AUTO_REFRESH_05_MIN;
	if(m_AutoRefreshStatus == 5){subMenuInfo.fState = MFS_CHECKED;}else{subMenuInfo.fState = MFS_UNCHECKED;}
	InsertMenuItem(hSubMenu, -1, TRUE, &subMenuInfo);

	cstr = _T("10 ") + i18n(_T("Menu"), _T("MINUTE"));
	subMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	subMenuInfo.wID= ID_AUTO_REFRESH_10_MIN;
	if(m_AutoRefreshStatus == 10){subMenuInfo.fState = MFS_CHECKED;}else{subMenuInfo.fState = MFS_UNCHECKED;}
	InsertMenuItem(hSubMenu, -1, TRUE, &subMenuInfo);

	cstr = _T("30 ") + i18n(_T("Menu"), _T("MINUTE"));
	subMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	subMenuInfo.wID= ID_AUTO_REFRESH_30_MIN;
	if(m_AutoRefreshStatus == 30){subMenuInfo.fState = MFS_CHECKED;}else{subMenuInfo.fState = MFS_UNCHECKED;}
	InsertMenuItem(hSubMenu, -1, TRUE, &subMenuInfo);

	cstr = _T("60 ") + i18n(_T("Menu"), _T("MINUTE"));
	subMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	subMenuInfo.wID= ID_AUTO_REFRESH_60_MIN;
	if(m_AutoRefreshStatus == 60){subMenuInfo.fState = MFS_CHECKED;}else{subMenuInfo.fState = MFS_UNCHECKED;}
	InsertMenuItem(hSubMenu, -1, TRUE, &subMenuInfo);

	cstr = _T("120 ") + i18n(_T("Menu"), _T("MINUTE"));
	subMenuInfo.dwTypeData = (LPWSTR) cstr.GetString();
	subMenuInfo.wID = ID_AUTO_REFRESH_120_MIN;
	if (m_AutoRefreshStatus == 120){ subMenuInfo.fState = MFS_CHECKED; }
	else{ subMenuInfo.fState = MFS_UNCHECKED; }
	InsertMenuItem(hSubMenu, -1, TRUE, &subMenuInfo);

	cstr = _T("180 ") + i18n(_T("Menu"), _T("MINUTE"));
	subMenuInfo.dwTypeData = (LPWSTR) cstr.GetString();
	subMenuInfo.wID = ID_AUTO_REFRESH_180_MIN;
	if (m_AutoRefreshStatus == 180){ subMenuInfo.fState = MFS_CHECKED; }
	else{ subMenuInfo.fState = MFS_UNCHECKED; }
	InsertMenuItem(hSubMenu, -1, TRUE, &subMenuInfo);

	cstr = _T("360 ") + i18n(_T("Menu"), _T("MINUTE"));
	subMenuInfo.dwTypeData = (LPWSTR) cstr.GetString();
	subMenuInfo.wID = ID_AUTO_REFRESH_360_MIN;
	if (m_AutoRefreshStatus == 360){ subMenuInfo.fState = MFS_CHECKED; }
	else{ subMenuInfo.fState = MFS_UNCHECKED; }
	InsertMenuItem(hSubMenu, -1, TRUE, &subMenuInfo);

	cstr = _T("720 ") + i18n(_T("Menu"), _T("MINUTE"));
	subMenuInfo.dwTypeData = (LPWSTR) cstr.GetString();
	subMenuInfo.wID = ID_AUTO_REFRESH_720_MIN;
	if (m_AutoRefreshStatus == 720){ subMenuInfo.fState = MFS_CHECKED; }
	else{ subMenuInfo.fState = MFS_UNCHECKED; }
	InsertMenuItem(hSubMenu, -1, TRUE, &subMenuInfo);

	cstr = _T("1440 ") + i18n(_T("Menu"), _T("MINUTE"));
	subMenuInfo.dwTypeData = (LPWSTR) cstr.GetString();
	subMenuInfo.wID = ID_AUTO_REFRESH_1440_MIN;
	if (m_AutoRefreshStatus == 1440){ subMenuInfo.fState = MFS_CHECKED; }
	else{ subMenuInfo.fState = MFS_UNCHECKED; }
	InsertMenuItem(hSubMenu, -1, TRUE, &subMenuInfo);

	cstr = i18n(_T("TrayMenu"), _T("DISABLE"));
	subMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	subMenuInfo.wID= ID_AUTO_REFRESH_DISABLE;
	if(m_AutoRefreshStatus == 0){subMenuInfo.fState = MFS_CHECKED;}else{subMenuInfo.fState = MFS_UNCHECKED;}
	InsertMenuItem(hSubMenu, -1, TRUE, &subMenuInfo);

/////////////////////////////////////////////////////////////////////////////////////////////////
// Main Menu
/////////////////////////////////////////////////////////////////////////////////////////////////
	menuInfo.fType = MFT_STRING;
	cstr = i18n(_T("TrayMenu"), _T("AUTO_REFRESH"));
	menuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	menuInfo.wID = ID_AUTO_REFRESH;
	menuInfo.hSubMenu = hSubMenu;
	InsertMenuItem(m_hMenu, -1, TRUE, &menuInfo);

	menuInfo.hSubMenu = NULL;

	cstr = i18n(_T("TrayMenu"), _T("REFRESH"));
	menuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	menuInfo.wID = ID_REFRESH;
	InsertMenuItem(m_hMenu, -1, TRUE, &menuInfo);

	menuInfo.fType = MFT_STRING;
	cstr = i18n(_T("TrayMenu"), _T("RESCAN"));
	menuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	menuInfo.wID = ID_RESCAN;

	InsertMenuItem(m_hMenu, -1, TRUE, &menuInfo);
	menuInfo.fType = MFT_SEPARATOR;
	InsertMenuItem(m_hMenu, -1, TRUE, &menuInfo);

	menuInfo.fType = MFT_STRING;
	cstr = i18n(_T("TrayMenu"), _T("GRAPH"));
	menuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	menuInfo.wID = SHOW_GRAPH_BASE + index;
	InsertMenuItem(m_hMenu, -1, TRUE, &menuInfo);

	menuInfo.fType = MFT_SEPARATOR;
	InsertMenuItem(m_hMenu, -1, TRUE, &menuInfo);

	menuInfo.fType = MFT_STRING;

	if(m_FlagShowTemperatureIconOnly)
	{
		menuInfo.fState = MFS_CHECKED;
	}
	else
	{
		menuInfo.fState = MFS_UNCHECKED;
	}
	if(!IsTemperatureIconExist())
	{
		menuInfo.fState |= MFS_DISABLED;
	}

	cstr = i18n(_T("TrayMenu"), _T("SHOW_TEMPERATURE_ICON_ONLY"));
	menuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	menuInfo.wID = MY_SHOW_TEMPERATURE_ICON_ONLY;
	InsertMenuItem(m_hMenu, -1, TRUE, &menuInfo);

	if(m_FlagResidentMinimize)
	{
		if(! IsIconic())
		{
			menuInfo.fState = MFS_CHECKED;
		}
		else
		{
			menuInfo.fState = MFS_UNCHECKED;
		}
	}
	else
	{
		if(IsWindowVisible())
		{
			menuInfo.fState = MFS_CHECKED;
		}
		else
		{
			menuInfo.fState = MFS_UNCHECKED;
		}
	}
	cstr = i18n(_T("TrayMenu"), _T("SHOW_MAIN_WINDOW"));
	menuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	menuInfo.wID = MY_SHOW_MAIN_DIALOG;
	InsertMenuItem(m_hMenu, -1, TRUE, &menuInfo);

	menuInfo.fType = MFT_SEPARATOR;
	InsertMenuItem(m_hMenu, -1, TRUE, &menuInfo);

	cstr = i18n(_T("TrayMenu"), _T("EXIT"));
	menuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	menuInfo.wID = MY_EXIT;
	menuInfo.fType = MFT_STRING;
	menuInfo.fState = MFS_UNCHECKED;
	InsertMenuItem(m_hMenu, -1, TRUE, &menuInfo);
}


BOOL CDiskInfoDlg::AddTemperatureIcon(DWORD i)
{
	if(m_Ata.vars[i].Temperature <= 0 || m_FlagTrayTemperatureIcon[i] == TRUE)
	{
		return FALSE;
	}

	CString cstr;
	CString diskStatus;
	diskStatus = GetDiskStatus(m_Ata.vars[i].DiskStatus);
	cstr.Format(_T("(%d) %s [%s]\r\n"), i + 1, m_Ata.vars[i].Model, diskStatus);

	cstr += GetLogicalDriveInfo(i, 128);
	cstr.TrimRight();
	if(m_hTempIcon[m_FlagFahrenheit][m_Ata.vars[i].Temperature] == NULL)
	{
		m_hTempIcon[m_FlagFahrenheit][m_Ata.vars[i].Temperature]
		= AfxGetApp()->LoadIcon(IDI_TEMP00 + m_FlagFahrenheit * 100 + m_Ata.vars[i].Temperature);
	}

	if(AddTaskTray(TRAY_TEMPERATURE_ICON_BASE + i, m_TempIconIndex[i], m_hTempIcon[m_FlagFahrenheit][m_Ata.vars[i].Temperature], cstr))
	{
		m_FlagTrayTemperatureIcon[i] = TRUE;

		return TRUE;
	}
	return FALSE;
}

BOOL CDiskInfoDlg::RemoveTemperatureIcon(DWORD i)
{
	if(RemoveTaskTray(TRAY_TEMPERATURE_ICON_BASE + i))
	{
		m_FlagTrayTemperatureIcon[i] = FALSE;
		return TRUE;
	}
	return FALSE;
}

BOOL CDiskInfoDlg::ModifyTemperatureIcon(DWORD i)
{
	if(m_Ata.vars[i].Temperature <= 0)
	{
		return FALSE;
	}

	CString cstr;
	CString diskStatus;
	diskStatus = GetDiskStatus(m_Ata.vars[i].DiskStatus);
	cstr.Format(_T("(%d) %s [%s]\r\n"), i + 1, m_Ata.vars[i].Model, diskStatus);
	cstr += GetLogicalDriveInfo(i, 128);

	cstr.TrimRight();
	if(m_hTempIcon[m_FlagFahrenheit][m_Ata.vars[i].Temperature] == NULL)
	{
		m_hTempIcon[m_FlagFahrenheit][m_Ata.vars[i].Temperature] = 
			AfxGetApp()->LoadIcon(IDI_TEMP00 + m_FlagFahrenheit * 100 + m_Ata.vars[i].Temperature);
	}

	if(ModifyTaskTray(TRAY_TEMPERATURE_ICON_BASE + i, m_hTempIcon[m_FlagFahrenheit][m_Ata.vars[i].Temperature], cstr))
	{
		return TRUE;
	}
	return FALSE;
}

void CDiskInfoDlg::CheckTrayTemperatureIcon()
{
	for(int i = (int)m_Ata.vars.GetCount() - 1; i >= 0; i--)
	{
		if(GetPrivateProfileInt(_T("TemperatureIcon"), m_Ata.vars[i].ModelSerial, 1, m_Ini))
		{
			if(m_Ata.vars[i].Temperature > -300)
			{
				AddTemperatureIcon(i);
			}
		}
	}
}

void CDiskInfoDlg::UpdateTrayTemperatureIcon(BOOL flagForce)
{
	static int preFlagFahrenheit = -1;
	if(preFlagFahrenheit != m_FlagFahrenheit)
	{
		flagForce = TRUE;
		preFlagFahrenheit = m_FlagFahrenheit;
	}
	for(int i = 0; i < m_Ata.vars.GetCount(); i++)
	{
		if(m_Ata.vars[i].Temperature > -300 && (flagForce || m_Ata.vars[i].Temperature != m_PreTemp[i]))
		{
			m_PreTemp[i] = m_Ata.vars[i].Temperature;
			ModifyTemperatureIcon(i);
		}
	}
}

void CDiskInfoDlg::UpdateToolTip()
{
	if(m_FlagResident)
	{
		CString cstr;
		CString tipFull, tipMid, tipShort;
		for(int i = 0; i < m_Ata.vars.GetCount(); i++)
		{
			CString diskStatus;
			diskStatus = GetDiskStatus(m_Ata.vars[i].DiskStatus);

			// Tooltip
			if(m_Ata.vars[i].IsSmartEnabled && m_Ata.vars[i].Temperature > -300)
			{
				if(m_FlagFahrenheit)
				{
					if(m_Ata.vars[i].TotalDiskSize >= 1000)
					{
						cstr.Format(_T("(%d) %s %.1f GB [%s] %d F\r\n"), i + 1, m_Ata.vars[i].Model,
							m_Ata.vars[i].TotalDiskSize / 1000.0, diskStatus, m_Ata.vars[i].Temperature * 9 / 5 + 32);
					}
					else
					{
						cstr.Format(_T("(%d) %s %d MB [%s] %d F\r\n"), i + 1, m_Ata.vars[i].Model,
							m_Ata.vars[i].TotalDiskSize, diskStatus, m_Ata.vars[i].Temperature * 9 / 5 + 32);
					}
					tipFull += cstr;
					cstr.Format(_T("(%d) %s %d F\r\n"), i + 1, m_Ata.vars[i].Model, m_Ata.vars[i].Temperature * 9 / 5 + 32);
					tipMid += cstr;
					cstr.Format(_T("(%d)%dF\r\n"), i + 1, m_Ata.vars[i].Temperature * 9 / 5 + 32);
					tipShort += cstr;
				}
				else
				{
					if(m_Ata.vars[i].TotalDiskSize >= 1000)
					{
						cstr.Format(_T("(%d) %s %.1f GB [%s] %d C\r\n"), i + 1, m_Ata.vars[i].Model,
							m_Ata.vars[i].TotalDiskSize / 1000.0, diskStatus, m_Ata.vars[i].Temperature);
					}
					else
					{
						cstr.Format(_T("(%d) %s %d MB [%s] %d C\r\n"), i + 1, m_Ata.vars[i].Model,
							m_Ata.vars[i].TotalDiskSize, diskStatus, m_Ata.vars[i].Temperature);
					}
					tipFull += cstr;
					cstr.Format(_T("(%d) %s %d C\r\n"), i + 1, m_Ata.vars[i].Model, m_Ata.vars[i].Temperature);
					tipMid += cstr;
					cstr.Format(_T("(%d)%dC\r\n"), i + 1, m_Ata.vars[i].Temperature);
					tipShort += cstr;
				}
			}
			else
			{
				if(m_Ata.vars[i].TotalDiskSize >= 1000)
				{
					cstr.Format(_T("(%d) %s %.1f GB [%s]\r\n"), i + 1, m_Ata.vars[i].Model,
						m_Ata.vars[i].TotalDiskSize / 1000.0, diskStatus);
				}
				else
				{
					cstr.Format(_T("(%d) %s %d MB [%s]\r\n"), i + 1, m_Ata.vars[i].Model,
						m_Ata.vars[i].TotalDiskSize, diskStatus);
				}
				tipFull += cstr;
				cstr.Format(_T("(%d) %s [%s]\r\n"), i + 1, m_Ata.vars[i].Model, diskStatus);
				tipMid += cstr;
				cstr.Format(_T("(%d)[%s]\r\n"), i + 1, diskStatus);
				tipShort += cstr;
			}
		}

		tipFull.Trim();
		tipMid.Trim();
		tipShort.Trim();
		if(tipFull.GetLength() < 128)
		{
			m_StatusTip = tipFull;
		}
		else if(tipMid.GetLength() < 128)
		{
			m_StatusTip = tipMid;
		}
		else
		{
			m_StatusTip = tipShort;
		}	

		ModifyTaskTrayTip(gRegIconId, m_StatusTip);
		for(int i = 0; i < m_Ata.vars.GetCount(); i++)
		{
			if(m_FlagTrayTemperatureIcon[i])
			{
				ModifyTemperatureIcon(i);
			}
		}
	}
}

void CDiskInfoDlg::CheckResident()
{
	if(GetPrivateProfileInt(_T("Setting"), _T("Resident"), 0, m_Ini) == 0)
	{
		m_FlagResident = FALSE;
		CMenu *menu = GetMenu();
		menu->CheckMenuItem(ID_RESIDENT, MF_UNCHECKED);
		SetMenu(menu);
		DrawMenuBar();
	}
	else
	{
		m_FlagResident = TRUE;
		CheckTrayTemperatureIcon();

		if(! m_FlagShowTemperatureIconOnly || ! IsTemperatureIconExist())
		{
			AddTrayMainIcon();
		}
		else
		{
			for(int i = 0; i < m_Ata.vars.GetCount(); i++)
			{
				if(m_FlagTrayTemperatureIcon[i])
				{
					m_MainIconId = TRAY_TEMPERATURE_ICON_BASE + i;
				}
			}
		}

		CMenu *menu = GetMenu();
		menu->CheckMenuItem(ID_RESIDENT, MF_CHECKED);
		SetMenu(menu);
		DrawMenuBar();
	}
}

void CDiskInfoDlg::OnResident()
{
	m_FlagWindoowMinimizeOnce = FALSE;
	if(m_FlagResident)
	{
		CMenu *menu = GetMenu();
		menu->CheckMenuItem(ID_RESIDENT, MF_UNCHECKED);
		SetMenu(menu);
		DrawMenuBar();

		RemoveTrayMainIcon();
		for(int i = 0; i < m_Ata.vars.GetCount(); i++)
		{
			if(m_FlagTrayTemperatureIcon[i])
			{
				RemoveTemperatureIcon(i);
			}
		}
		m_FlagResident = FALSE;
		WritePrivateProfileString(_T("Setting"), _T("Resident"), _T("0"), m_Ini);
	}
	else
	{
		m_FlagResident = TRUE;

		CMenu *menu = GetMenu();
		menu->CheckMenuItem(ID_RESIDENT, MF_CHECKED);
		SetMenu(menu);
		DrawMenuBar();
		WritePrivateProfileString(_T("Setting"), _T("Resident"), _T("1"), m_Ini);
		CheckTrayTemperatureIcon();

		if(! m_FlagShowTemperatureIconOnly || ! IsTemperatureIconExist())
		{
			AddTrayMainIcon();
		}
	}
}

void CDiskInfoDlg::ShowTemperatureIconOnly()
{
	if(m_FlagShowTemperatureIconOnly)
	{
		m_FlagShowTemperatureIconOnly = FALSE;
		WritePrivateProfileString(_T("Setting"), _T("ShowTemperatureIconOnly"), _T("0"), m_Ini);

		AddTrayMainIcon();
	}
	else
	{
		m_FlagShowTemperatureIconOnly = TRUE;
		WritePrivateProfileString(_T("Setting"), _T("ShowTemperatureIconOnly"), _T("1"), m_Ini);

		if(IsTemperatureIconExist())
		{
			if(RemoveTrayMainIcon())
			{
				for(int i = 0; i < m_Ata.vars.GetCount(); i++)
				{
					if(m_FlagTrayTemperatureIcon[i])
					{
						m_MainIconId = TRAY_TEMPERATURE_ICON_BASE + i;
					}
				}
			}
		}
	}
}

BOOL CDiskInfoDlg::AddTrayMainIcon()
{
	if(m_FlagTrayMainIcon)
	{
		return TRUE;
	}
	else if(AddTaskTray(gRegIconId, gRegMessageId, m_hIconMini, m_StatusTip))
	{
		m_FlagTrayMainIcon = TRUE;
		m_MainIconId = gRegIconId;
	}
	return FALSE;
}

BOOL CDiskInfoDlg::RemoveTrayMainIcon()
{
	if(! m_FlagTrayMainIcon)
	{
		return TRUE;
	}
	else if(RemoveTaskTray(gRegIconId))
	{
		m_FlagTrayMainIcon = FALSE;
		return TRUE;
	}
	return FALSE;
}

BOOL CDiskInfoDlg::IsTemperatureIconExist()
{
	for(int j = 0; j < m_Ata.vars.GetCount(); j++)
	{
		if(m_FlagTrayTemperatureIcon[j])
		{
			return TRUE;
		}
	}
	return FALSE;
}