/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
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

LRESULT CDiskInfoDlg::TempIcon(int index, WPARAM wParam, LPARAM lParam)
{
	if (lParam == WM_LBUTTONDOWN)
	{
		ShowWindowEx(SW_RESTORE);
		SelectDrive(index);
	}
	else if (lParam == WM_RBUTTONDOWN)
	{
		TaskTrayRightMenu(index);
	}
	return 0;
}

LRESULT CDiskInfoDlg::OnTempIcon0(WPARAM w, LPARAM l) { return TempIcon(0, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon1(WPARAM w, LPARAM l) { return TempIcon(1, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon2(WPARAM w, LPARAM l) { return TempIcon(2, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon3(WPARAM w, LPARAM l) { return TempIcon(3, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon4(WPARAM w, LPARAM l) { return TempIcon(4, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon5(WPARAM w, LPARAM l) { return TempIcon(5, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon6(WPARAM w, LPARAM l) { return TempIcon(6, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon7(WPARAM w, LPARAM l) { return TempIcon(7, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon8(WPARAM w, LPARAM l) { return TempIcon(8, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon9(WPARAM w, LPARAM l) { return TempIcon(9, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon10(WPARAM w, LPARAM l) { return TempIcon(10, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon11(WPARAM w, LPARAM l) { return TempIcon(11, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon12(WPARAM w, LPARAM l) { return TempIcon(12, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon13(WPARAM w, LPARAM l) { return TempIcon(13, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon14(WPARAM w, LPARAM l) { return TempIcon(14, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon15(WPARAM w, LPARAM l) { return TempIcon(15, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon16(WPARAM w, LPARAM l) { return TempIcon(16, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon17(WPARAM w, LPARAM l) { return TempIcon(17, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon18(WPARAM w, LPARAM l) { return TempIcon(18, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon19(WPARAM w, LPARAM l) { return TempIcon(19, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon20(WPARAM w, LPARAM l) { return TempIcon(20, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon21(WPARAM w, LPARAM l) { return TempIcon(21, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon22(WPARAM w, LPARAM l) { return TempIcon(22, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon23(WPARAM w, LPARAM l) { return TempIcon(23, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon24(WPARAM w, LPARAM l) { return TempIcon(24, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon25(WPARAM w, LPARAM l) { return TempIcon(25, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon26(WPARAM w, LPARAM l) { return TempIcon(26, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon27(WPARAM w, LPARAM l) { return TempIcon(27, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon28(WPARAM w, LPARAM l) { return TempIcon(28, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon29(WPARAM w, LPARAM l) { return TempIcon(29, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon30(WPARAM w, LPARAM l) { return TempIcon(30, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon31(WPARAM w, LPARAM l) { return TempIcon(31, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon32(WPARAM w, LPARAM l) { return TempIcon(32, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon33(WPARAM w, LPARAM l) { return TempIcon(33, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon34(WPARAM w, LPARAM l) { return TempIcon(34, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon35(WPARAM w, LPARAM l) { return TempIcon(35, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon36(WPARAM w, LPARAM l) { return TempIcon(36, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon37(WPARAM w, LPARAM l) { return TempIcon(37, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon38(WPARAM w, LPARAM l) { return TempIcon(38, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon39(WPARAM w, LPARAM l) { return TempIcon(39, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon40(WPARAM w, LPARAM l) { return TempIcon(40, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon41(WPARAM w, LPARAM l) { return TempIcon(41, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon42(WPARAM w, LPARAM l) { return TempIcon(42, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon43(WPARAM w, LPARAM l) { return TempIcon(43, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon44(WPARAM w, LPARAM l) { return TempIcon(44, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon45(WPARAM w, LPARAM l) { return TempIcon(45, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon46(WPARAM w, LPARAM l) { return TempIcon(46, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon47(WPARAM w, LPARAM l) { return TempIcon(47, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon48(WPARAM w, LPARAM l) { return TempIcon(48, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon49(WPARAM w, LPARAM l) { return TempIcon(49, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon50(WPARAM w, LPARAM l) { return TempIcon(50, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon51(WPARAM w, LPARAM l) { return TempIcon(51, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon52(WPARAM w, LPARAM l) { return TempIcon(52, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon53(WPARAM w, LPARAM l) { return TempIcon(53, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon54(WPARAM w, LPARAM l) { return TempIcon(54, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon55(WPARAM w, LPARAM l) { return TempIcon(55, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon56(WPARAM w, LPARAM l) { return TempIcon(56, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon57(WPARAM w, LPARAM l) { return TempIcon(57, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon58(WPARAM w, LPARAM l) { return TempIcon(58, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon59(WPARAM w, LPARAM l) { return TempIcon(59, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon60(WPARAM w, LPARAM l) { return TempIcon(60, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon61(WPARAM w, LPARAM l) { return TempIcon(61, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon62(WPARAM w, LPARAM l) { return TempIcon(62, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon63(WPARAM w, LPARAM l) { return TempIcon(63, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon64(WPARAM w, LPARAM l) { return TempIcon(64, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon65(WPARAM w, LPARAM l) { return TempIcon(65, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon66(WPARAM w, LPARAM l) { return TempIcon(66, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon67(WPARAM w, LPARAM l) { return TempIcon(67, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon68(WPARAM w, LPARAM l) { return TempIcon(68, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon69(WPARAM w, LPARAM l) { return TempIcon(69, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon70(WPARAM w, LPARAM l) { return TempIcon(70, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon71(WPARAM w, LPARAM l) { return TempIcon(71, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon72(WPARAM w, LPARAM l) { return TempIcon(72, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon73(WPARAM w, LPARAM l) { return TempIcon(73, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon74(WPARAM w, LPARAM l) { return TempIcon(74, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon75(WPARAM w, LPARAM l) { return TempIcon(75, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon76(WPARAM w, LPARAM l) { return TempIcon(76, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon77(WPARAM w, LPARAM l) { return TempIcon(77, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon78(WPARAM w, LPARAM l) { return TempIcon(78, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon79(WPARAM w, LPARAM l) { return TempIcon(79, w, l); }
/*
LRESULT CDiskInfoDlg::OnTempIcon80(WPARAM w, LPARAM l) { return TempIcon(80, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon81(WPARAM w, LPARAM l) { return TempIcon(81, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon82(WPARAM w, LPARAM l) { return TempIcon(82, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon83(WPARAM w, LPARAM l) { return TempIcon(83, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon84(WPARAM w, LPARAM l) { return TempIcon(84, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon85(WPARAM w, LPARAM l) { return TempIcon(85, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon86(WPARAM w, LPARAM l) { return TempIcon(86, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon87(WPARAM w, LPARAM l) { return TempIcon(87, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon88(WPARAM w, LPARAM l) { return TempIcon(88, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon89(WPARAM w, LPARAM l) { return TempIcon(89, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon90(WPARAM w, LPARAM l) { return TempIcon(90, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon91(WPARAM w, LPARAM l) { return TempIcon(91, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon92(WPARAM w, LPARAM l) { return TempIcon(92, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon93(WPARAM w, LPARAM l) { return TempIcon(93, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon94(WPARAM w, LPARAM l) { return TempIcon(94, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon95(WPARAM w, LPARAM l) { return TempIcon(95, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon96(WPARAM w, LPARAM l) { return TempIcon(96, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon97(WPARAM w, LPARAM l) { return TempIcon(97, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon98(WPARAM w, LPARAM l) { return TempIcon(98, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon99(WPARAM w, LPARAM l) { return TempIcon(99, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon100(WPARAM w, LPARAM l) { return TempIcon(100, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon101(WPARAM w, LPARAM l) { return TempIcon(101, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon102(WPARAM w, LPARAM l) { return TempIcon(102, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon103(WPARAM w, LPARAM l) { return TempIcon(103, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon104(WPARAM w, LPARAM l) { return TempIcon(104, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon105(WPARAM w, LPARAM l) { return TempIcon(105, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon106(WPARAM w, LPARAM l) { return TempIcon(106, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon107(WPARAM w, LPARAM l) { return TempIcon(107, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon108(WPARAM w, LPARAM l) { return TempIcon(108, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon109(WPARAM w, LPARAM l) { return TempIcon(109, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon110(WPARAM w, LPARAM l) { return TempIcon(110, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon111(WPARAM w, LPARAM l) { return TempIcon(111, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon112(WPARAM w, LPARAM l) { return TempIcon(112, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon113(WPARAM w, LPARAM l) { return TempIcon(113, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon114(WPARAM w, LPARAM l) { return TempIcon(114, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon115(WPARAM w, LPARAM l) { return TempIcon(115, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon116(WPARAM w, LPARAM l) { return TempIcon(116, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon117(WPARAM w, LPARAM l) { return TempIcon(117, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon118(WPARAM w, LPARAM l) { return TempIcon(118, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon119(WPARAM w, LPARAM l) { return TempIcon(119, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon120(WPARAM w, LPARAM l) { return TempIcon(120, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon121(WPARAM w, LPARAM l) { return TempIcon(121, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon122(WPARAM w, LPARAM l) { return TempIcon(122, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon123(WPARAM w, LPARAM l) { return TempIcon(123, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon124(WPARAM w, LPARAM l) { return TempIcon(124, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon125(WPARAM w, LPARAM l) { return TempIcon(125, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon126(WPARAM w, LPARAM l) { return TempIcon(126, w, l); }
LRESULT CDiskInfoDlg::OnTempIcon127(WPARAM w, LPARAM l) { return TempIcon(127, w, l); }
*/
LRESULT CDiskInfoDlg::OnRegMessage(WPARAM wParam, LPARAM lParam)
{
	if(lParam == WM_LBUTTONDOWN && m_bResidentMinimize)
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
	if(m_bResident)
	{
		for(int i = 0; i < m_Ata.vars.GetCount(); i++)
		{
			if(m_bTrayTemperatureIcon[i])
			{
				m_bTrayTemperatureIcon[i] = FALSE; // Force Add Temperature Icon
				AddTemperatureIcon(i);
			}
		}
		if(m_bTrayMainIcon)
		{
			m_bTrayMainIcon = FALSE; // Force Add Main Icon
			AddTrayMainIcon();
		}
	}

	UpdateToolTip();

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
			if(m_bFahrenheit)
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
		else if(m_bTrayTemperatureIcon[i])
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

		if(m_bAutoRefreshTarget[i])
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
		cstr.Format(_T("(%d) %s"), i + 1, m_Ata.vars[i].Model.GetString());
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
		if(m_bFahrenheit)
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

	if(m_bShowTemperatureIconOnly)
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

	if(m_bResidentMinimize)
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
	if(m_Ata.vars[i].Temperature <= 0 || m_bTrayTemperatureIcon[i] == TRUE)
	{
		return FALSE;
	}

	CString cstr;
	CString diskStatus;
	diskStatus = GetDiskStatus(m_Ata.vars[i].DiskStatus);
	cstr.Format(_T("(%d) %s [%s]\r\n"), i + 1, m_Ata.vars[i].Model.GetString(), diskStatus.GetString());

	cstr += GetLogicalDriveInfo(i, 128);
	cstr.TrimRight();

	INT TemperatureOffset = 0;
	if (m_Ata.vars[i].Temperature >= m_Ata.vars[i].AlarmTemperature)
	{
		TemperatureOffset = 200;
	}
	else if (m_bGreenMode)
	{
		TemperatureOffset = 100;
	}

	if(m_hTempIcon[m_bFahrenheit][TemperatureOffset + m_Ata.vars[i].Temperature] == NULL)
	{
		m_hTempIcon[m_bFahrenheit][TemperatureOffset + m_Ata.vars[i].Temperature] = 
		(HICON)::LoadImageW(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_TEMP_00 + m_bFahrenheit * 300 + TemperatureOffset + m_Ata.vars[i].Temperature), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
	}

	cstr.Format(L"CrystalDiskInfo_%2d", i + 1);

	if(AddTaskTray(TRAY_TEMPERATURE_ICON_BASE + i, m_TempIconIndex[i], m_hTempIcon[m_bFahrenheit][TemperatureOffset + m_Ata.vars[i].Temperature], cstr))
	{
		m_bTrayTemperatureIcon[i] = TRUE;

		return TRUE;
	}
	return FALSE;
}

BOOL CDiskInfoDlg::RemoveTemperatureIcon(DWORD i)
{
	if(RemoveTaskTray(TRAY_TEMPERATURE_ICON_BASE + i))
	{
		m_bTrayTemperatureIcon[i] = FALSE;
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
	cstr.Format(_T("(%d) %s [%s]\r\n"), i + 1, m_Ata.vars[i].Model.GetString(), diskStatus.GetString());
	cstr += GetLogicalDriveInfo(i, 128);
	cstr.TrimRight();

	INT TemperatureOffset = 0;
	if (m_Ata.vars[i].Temperature >= m_Ata.vars[i].AlarmTemperature)
	{
		TemperatureOffset = 200;
	}
	else if (m_bGreenMode)
	{
		TemperatureOffset = 100;
	}

	if (m_hTempIcon[m_bFahrenheit][TemperatureOffset + m_Ata.vars[i].Temperature] == NULL)
	{
		m_hTempIcon[m_bFahrenheit][TemperatureOffset + m_Ata.vars[i].Temperature] =
			(HICON)::LoadImageW(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_TEMP_00 + m_bFahrenheit * 300 + TemperatureOffset + m_Ata.vars[i].Temperature), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
	}

	if(ModifyTaskTray(TRAY_TEMPERATURE_ICON_BASE + i, m_hTempIcon[m_bFahrenheit][TemperatureOffset + m_Ata.vars[i].Temperature], cstr))
	{
		return TRUE;
	}
	return FALSE;
}

void CDiskInfoDlg::CheckTrayTemperatureIcon()
{
	for(int i = (int)m_Ata.vars.GetCount() - 1; i >= 0; i--)
	{
		if(GetPrivateProfileIntFx(_T("TemperatureIcon"), m_Ata.vars[i].ModelSerial, 1, m_Ini))
		{
			if(m_Ata.vars[i].Temperature > -300)
			{
				AddTemperatureIcon(i);
			}
		}
	}
	UpdateToolTip();
}

void CDiskInfoDlg::UpdateTrayTemperatureIcon(BOOL flagForce)
{
	static int preFlagFahrenheit = -1;
	if(preFlagFahrenheit != m_bFahrenheit)
	{
		flagForce = TRUE;
		preFlagFahrenheit = m_bFahrenheit;
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
	if(m_bResident)
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
				if(m_bFahrenheit)
				{
					if(m_Ata.vars[i].TotalDiskSize >= 1000)
					{
						cstr.Format(_T("(%d) %s %.1f GB [%s] %d F\r\n"), i + 1, m_Ata.vars[i].Model.GetString(),
							m_Ata.vars[i].TotalDiskSize / 1000.0, diskStatus.GetString(), m_Ata.vars[i].Temperature * 9 / 5 + 32);
					}
					else
					{
						cstr.Format(_T("(%d) %s %d MB [%s] %d F\r\n"), i + 1, m_Ata.vars[i].Model.GetString(),
							m_Ata.vars[i].TotalDiskSize, diskStatus.GetString(), m_Ata.vars[i].Temperature * 9 / 5 + 32);
					}
					tipFull += cstr;
					cstr.Format(_T("(%d) %s %d F\r\n"), i + 1, m_Ata.vars[i].Model.GetString(), m_Ata.vars[i].Temperature * 9 / 5 + 32);
					tipMid += cstr;
					cstr.Format(_T("(%d)%dF\r\n"), i + 1, m_Ata.vars[i].Temperature * 9 / 5 + 32);
					tipShort += cstr;
				}
				else
				{
					if(m_Ata.vars[i].TotalDiskSize >= 1000)
					{
						cstr.Format(_T("(%d) %s %.1f GB [%s] %d C\r\n"), i + 1, m_Ata.vars[i].Model.GetString(),
							m_Ata.vars[i].TotalDiskSize / 1000.0, diskStatus.GetString(), m_Ata.vars[i].Temperature);
					}
					else
					{
						cstr.Format(_T("(%d) %s %d MB [%s] %d C\r\n"), i + 1, m_Ata.vars[i].Model.GetString(),
							m_Ata.vars[i].TotalDiskSize, diskStatus.GetString(), m_Ata.vars[i].Temperature);
					}
					tipFull += cstr;
					cstr.Format(_T("(%d) %s %d C\r\n"), i + 1, m_Ata.vars[i].Model.GetString(), m_Ata.vars[i].Temperature);
					tipMid += cstr;
					cstr.Format(_T("(%d)%dC\r\n"), i + 1, m_Ata.vars[i].Temperature);
					tipShort += cstr;
				}
			}
			else
			{
				if(m_Ata.vars[i].TotalDiskSize >= 1000)
				{
					cstr.Format(_T("(%d) %s %.1f GB [%s]\r\n"), i + 1, m_Ata.vars[i].Model.GetString(),
						m_Ata.vars[i].TotalDiskSize / 1000.0, diskStatus.GetString());
				}
				else
				{
					cstr.Format(_T("(%d) %s %d MB [%s]\r\n"), i + 1, m_Ata.vars[i].Model.GetString(),
						m_Ata.vars[i].TotalDiskSize, diskStatus.GetString());
				}
				tipFull += cstr;
				cstr.Format(_T("(%d) %s [%s]\r\n"), i + 1, m_Ata.vars[i].Model.GetString(), diskStatus.GetString());
				tipMid += cstr;
				cstr.Format(_T("(%d)[%s]\r\n"), i + 1, diskStatus.GetString());
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
			if(m_bTrayTemperatureIcon[i])
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
		m_bResident = FALSE;
		CMenu *menu = GetMenu();
		menu->CheckMenuItem(ID_RESIDENT, MF_UNCHECKED);
		SetMenu(menu);
		DrawMenuBar();
	}
	else
	{
		m_bResident = TRUE;
		CheckTrayTemperatureIcon();

		if(! m_bShowTemperatureIconOnly || ! IsTemperatureIconExist())
		{
			AddTrayMainIcon();
		}
		else
		{
			for(int i = 0; i < m_Ata.vars.GetCount(); i++)
			{
				if(m_bTrayTemperatureIcon[i])
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
	m_bWindowMinimizeOnce = FALSE;
	if(m_bResident)
	{
		CMenu *menu = GetMenu();
		menu->CheckMenuItem(ID_RESIDENT, MF_UNCHECKED);
		SetMenu(menu);
		DrawMenuBar();

		RemoveTrayMainIcon();
		for(int i = 0; i < m_Ata.vars.GetCount(); i++)
		{
			if(m_bTrayTemperatureIcon[i])
			{
				RemoveTemperatureIcon(i);
			}
		}
		m_bResident = FALSE;
		WritePrivateProfileStringFx(_T("Setting"), _T("Resident"), _T("0"), m_Ini);
	}
	else
	{
		m_bResident = TRUE;

		CMenu *menu = GetMenu();
		menu->CheckMenuItem(ID_RESIDENT, MF_CHECKED);
		SetMenu(menu);
		DrawMenuBar();
		WritePrivateProfileStringFx(_T("Setting"), _T("Resident"), _T("1"), m_Ini);
		CheckTrayTemperatureIcon();

		if(! m_bShowTemperatureIconOnly || ! IsTemperatureIconExist())
		{
			AddTrayMainIcon();
		}
	}
}

void CDiskInfoDlg::ShowTemperatureIconOnly()
{
	if(m_bShowTemperatureIconOnly)
	{
		m_bShowTemperatureIconOnly = FALSE;
		WritePrivateProfileStringFx(_T("Setting"), _T("ShowTemperatureIconOnly"), _T("0"), m_Ini);

		AddTrayMainIcon();
	}
	else
	{
		m_bShowTemperatureIconOnly = TRUE;
		WritePrivateProfileStringFx(_T("Setting"), _T("ShowTemperatureIconOnly"), _T("1"), m_Ini);

		if(IsTemperatureIconExist())
		{
			if(RemoveTrayMainIcon())
			{
				for(int i = 0; i < m_Ata.vars.GetCount(); i++)
				{
					if(m_bTrayTemperatureIcon[i])
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
	if(m_bTrayMainIcon)
	{
		return TRUE;
	}
	else if(AddTaskTray(gRegIconId, gRegMessageId, m_hIconMini, L"CrystalDiskInfo"))
	{
		m_bTrayMainIcon = TRUE;
		m_MainIconId = gRegIconId;
	}
	return FALSE;
}

BOOL CDiskInfoDlg::RemoveTrayMainIcon()
{
	if(! m_bTrayMainIcon)
	{
		return TRUE;
	}
	else if(RemoveTaskTray(gRegIconId))
	{
		m_bTrayMainIcon = FALSE;
		return TRUE;
	}
	return FALSE;
}

BOOL CDiskInfoDlg::IsTemperatureIconExist()
{
	for(int j = 0; j < m_Ata.vars.GetCount(); j++)
	{
		if(m_bTrayTemperatureIcon[j])
		{
			return TRUE;
		}
	}
	return FALSE;
}