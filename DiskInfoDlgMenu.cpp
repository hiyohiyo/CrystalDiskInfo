/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DiskInfo.h"
#include "DiskInfoDlg.h"
#include <complex>

//  Include the task header file.
#include <taskschd.h>
# pragma comment(lib, "taskschd.lib")
# pragma comment(lib, "comsupp.lib")
# pragma comment(lib, "credui.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

CString CDiskInfoDlg::Encode10X(DWORD value)
{
	CString cstr;
	cstr.Format(_T("%010d%08X"), value, value);

	return cstr;
}

DWORD CDiskInfoDlg::Decode10X(CString cstr)
{
	DWORD d, x;
	TCHAR *endPtr;
	d = _tcstol(cstr.Left(10), &endPtr, 10);
	x = _tcstol(cstr.Mid(10, 8), &endPtr, 16);

	if(d == x)
	{
		return d;
	}
	else
	{
		return 0;
	}
}

void CDiskInfoDlg::OnAbout()
{
	m_AboutDlg = new CAboutDlg(this);
	m_AboutDlg->Create(CAboutDlg::IDD, m_AboutDlg, ID_ABOUT, this);
}

void CDiskInfoDlg::OnCustomize()
{
	m_OptionDlg = new COptionDlg(this);
	m_OptionDlg->Create(COptionDlg::IDD, m_OptionDlg, ID_CUSTOMIZE, this);
}

void CDiskInfoDlg::OnAamApm()
{
	m_SettingDlg = new CSettingDlg(this);
	m_SettingDlg->Create(CSettingDlg::IDD, m_SettingDlg, ID_AAM_APM, this);
}

void CDiskInfoDlg::OnTemperature()
{
	m_TemperatureDlg = new CTemperatureDlg(this);
	m_TemperatureDlg->Create(CTemperatureDlg::IDD, m_TemperatureDlg, ID_TEMPERATURE, this);
}

void CDiskInfoDlg::OnHealthStatus()
{
	CMenu *menu = GetMenu();
	if(! (menu->GetMenuState(ID_HEALTH_STATUS, MF_BYCOMMAND) & MF_GRAYED))
	{
		m_HealthDlg = new CHealthDlg(this);
		m_HealthDlg->Create(CHealthDlg::IDD, m_HealthDlg, ID_HEALTH_STATUS, this);
	}
}

/*
void CDiskInfoDlg::OnAlarmHistory()
{
	m_AlarmHistoryDlg = new CAlarmHistoryDlg(this);
	m_AlarmHistoryDlg->Create(CAlarmHistoryDlg::IDD, m_AlarmHistoryDlg, ID_ALARM_HISTORY, this);
}
*/

void CDiskInfoDlg::OnSoundSetting()
{
	m_SoundSettingDlg = new CSoundSettingDlg(this);
	m_SoundSettingDlg->Create(CSoundSettingDlg::IDD, m_SoundSettingDlg, ID_SOUND_SETTINGS, this);
}

void CDiskInfoDlg::OnGraph()
{
	ShowGraphDlg(-1);
}

void CDiskInfoDlg::ShowGraphDlg(int index)
{
//	m_GraphDlg = new CGraphDlg(this);
//	m_GraphDlg->Create(CGraphDlg::IDD, m_GraphDlg, ID_GRAPH, this);

	CreateExchangeInfo();

	TCHAR path[MAX_PATH];
	CString cstr;
	GetModuleFileName(NULL, path, MAX_PATH);

	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi = {0};
	si.cb			= sizeof(STARTUPINFO);
	si.dwFlags		= STARTF_USESHOWWINDOW;
	si.wShowWindow	= SW_SHOWNORMAL;
	cstr.Format(_T("\"%s\" /Earthlight %d"), path, index); 
	::CreateProcess(NULL, (LPWSTR)cstr.GetString(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	m_GraphProcessId.Add(pi.dwProcessId);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}

void CDiskInfoDlg::CreateExchangeInfo()
{
	CString cstr;
	cstr.Format(_T("%d"), m_Ata.vars.GetCount());
	WritePrivateProfileString(_T("EXCHANGE"), _T("DetectedDisk"), cstr, m_SmartDir + EXCHANGE_INI);

	for(int i = 0; i < m_Ata.vars.GetCount(); i++)
	{
		cstr.Format(_T("%d"), i);
		WritePrivateProfileString(_T("MODEL"), cstr, m_Ata.vars[i].Model, m_SmartDir + EXCHANGE_INI);
		WritePrivateProfileString(_T("SERIAL"), cstr, m_Ata.vars[i].SerialNumber, m_SmartDir + EXCHANGE_INI);
		WritePrivateProfileString(_T("DRIVE"), cstr, m_Ata.vars[i].DriveMap, m_SmartDir + EXCHANGE_INI);
	}
}

void CDiskInfoDlg::OnHideSmartInfo()
{
	CMenu *menu = GetMenu();		
	if(m_bHideSmartInfo)
	{
		m_SizeX = SIZE_SMART_X;
		m_SizeY = SIZE_SMART_Y;
		SetClientSize(m_SizeX, m_SizeY, m_ZoomRatio);
		menu->CheckMenuItem(ID_HIDE_SMART_INFO, MF_UNCHECKED);
		m_bHideSmartInfo = FALSE;
		WritePrivateProfileStringW(_T("Setting"), _T("HideSmartInfo"), _T("0"), m_Ini);
	}
	else
	{
		m_SizeX = SIZE_X;
		m_SizeY = SIZE_Y;
		SetClientSize(m_SizeX, m_SizeY, m_ZoomRatio);
		menu->CheckMenuItem(ID_HIDE_SMART_INFO, MF_CHECKED);
		m_bHideSmartInfo = TRUE;
		WritePrivateProfileStringW(_T("Setting"), _T("HideSmartInfo"), _T("1"), m_Ini);
	}

	CString cstr;
	cstr.Format(_T("%d"), m_SizeY);
	WritePrivateProfileString(_T("Setting"), _T("Height"), cstr, m_Ini);

	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnGreenMode()
{
	CMenu *menu = GetMenu();
	if (menu->GetMenuState(ID_GREEN_MODE, MF_BYCOMMAND) & MFS_CHECKED)
	{
		menu->CheckMenuItem(ID_GREEN_MODE, MF_UNCHECKED);
		m_bGreenMode = FALSE;
		WritePrivateProfileStringW(_T("Setting"), _T("GreenMode"), _T("0"), m_Ini);
	}
	else
	{
		menu->CheckMenuItem(ID_GREEN_MODE, MF_CHECKED);
		m_bGreenMode = TRUE;
		WritePrivateProfileStringW(_T("Setting"), _T("GreenMode"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();

	UpdateTrayTemperatureIcon(TRUE);

	UpdateDialogSize();
}

void CDiskInfoDlg::CheckHideSerialNumber()
{
	CMenu *menu = GetMenu();
	if(GetPrivateProfileInt(_T("Setting"), _T("HideSerialNumber"), 0, m_Ini))
	{
		m_bHideSerialNumber = TRUE;
		menu->CheckMenuItem(ID_HIDE_SERIAL_NUMBER, MF_CHECKED);
	}
	else
	{
		m_bHideSerialNumber = FALSE;
		menu->CheckMenuItem(ID_HIDE_SERIAL_NUMBER, MF_UNCHECKED);
	}
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnHideSerialNumber()
{
	CMenu *menu = GetMenu();		
	if(m_bHideSerialNumber)
	{
		m_bHideSerialNumber = FALSE;
		menu->CheckMenuItem(ID_HIDE_SERIAL_NUMBER, MF_UNCHECKED);
		WritePrivateProfileStringW(_T("Setting"), _T("HideSerialNumber"), _T("0"), m_Ini);
	}
	else
	{
		m_bHideSerialNumber = TRUE;
		menu->CheckMenuItem(ID_HIDE_SERIAL_NUMBER, MF_CHECKED);
		WritePrivateProfileStringW(_T("Setting"), _T("HideSerialNumber"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();

	ChangeDisk(m_SelectDisk);
}

void CDiskInfoDlg::OnCrystalDewWorld()
{
	if(GetUserDefaultLCID() == 0x0411) // Japanese
	{
		OpenUrl(URL_CRYSTAL_DEW_WORLD_JA);
	}
	else // Other Language
	{
		OpenUrl(URL_CRYSTAL_DEW_WORLD_EN);
	}	
}

void CDiskInfoDlg::OnHelp()
{
	if(GetUserDefaultLCID() == 0x0411) // Japanese
	{
		OpenUrl(URL_HTML_HELP_JA);
	}
	else // Other Language
	{
		OpenUrl(URL_HTML_HELP_EN);
	}	
}

void CDiskInfoDlg::OnRefresh()
{
	Refresh(TRUE);
}

void CDiskInfoDlg::OnRescan()
{
	CWaitCursor wait;
	BOOL flagChangeDisk = FALSE;

	InitAta(TRUE, m_bAdvancedDiskSearch, &flagChangeDisk, m_bWorkaroundHD204UI, m_bWorkaroundAdataSsd);

	if(flagChangeDisk)
	{
		// Update Menu and Dialog
		m_SelectDisk = 0;
		m_DriveMenuPage = 0;
		ChangeLang(m_CurrentLang);
	}
	else
	{
		RebuildListHeader(m_SelectDisk, TRUE);
		Refresh(TRUE);
	}

	if(m_bResident && flagChangeDisk)
	{
		for(int i = 0; i < CAtaSmart::MAX_DISK; i++)
		{
			RemoveTemperatureIcon(i);
		}
		CheckTrayTemperatureIcon();
	}
}

void CDiskInfoDlg::OnHelpAboutSmart()
{
	TCHAR url[256];
	GetPrivateProfileString(_T("Url"), _T("WIKIPEDIA_SMART"), _T("http://en.wikipedia.org/wiki/Self-Monitoring%2C_Analysis%2C_and_Reporting_Technology"), url, 256, m_CurrentLangPath);

	OpenUrl(url);
}

void CDiskInfoDlg::OnAutoRefreshDisable(){	CheckRadioAutoRefresh(ID_AUTO_REFRESH_DISABLE, 0);}
void CDiskInfoDlg::OnAutoRefresh01Min(){	CheckRadioAutoRefresh(ID_AUTO_REFRESH_01_MIN, 1);}
void CDiskInfoDlg::OnAutoRefresh03Min(){	CheckRadioAutoRefresh(ID_AUTO_REFRESH_03_MIN, 3);}
void CDiskInfoDlg::OnAutoRefresh05Min(){	CheckRadioAutoRefresh(ID_AUTO_REFRESH_05_MIN, 5);}
void CDiskInfoDlg::OnAutoRefresh10Min(){	CheckRadioAutoRefresh(ID_AUTO_REFRESH_10_MIN, 10);}
void CDiskInfoDlg::OnAutoRefresh30Min(){	CheckRadioAutoRefresh(ID_AUTO_REFRESH_30_MIN, 30);}
void CDiskInfoDlg::OnAutoRefresh60Min(){	CheckRadioAutoRefresh(ID_AUTO_REFRESH_60_MIN, 60);}
void CDiskInfoDlg::OnAutoRefresh120Min(){ CheckRadioAutoRefresh(ID_AUTO_REFRESH_120_MIN, 120); }
void CDiskInfoDlg::OnAutoRefresh180Min(){ CheckRadioAutoRefresh(ID_AUTO_REFRESH_180_MIN, 180); }
void CDiskInfoDlg::OnAutoRefresh360Min(){ CheckRadioAutoRefresh(ID_AUTO_REFRESH_360_MIN, 360); }
void CDiskInfoDlg::OnAutoRefresh720Min(){ CheckRadioAutoRefresh(ID_AUTO_REFRESH_720_MIN, 720); }
void CDiskInfoDlg::OnAutoRefresh1440Min(){ CheckRadioAutoRefresh(ID_AUTO_REFRESH_1440_MIN, 1440); }

void CDiskInfoDlg::CheckRadioAutoRefresh(int id, int value)
{
	CMenu *menu = GetMenu();
	menu->CheckMenuRadioItem(ID_AUTO_REFRESH_DISABLE, ID_AUTO_REFRESH_1440_MIN, id, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();

	m_AutoRefreshStatus = value;

	CString cstr;
	cstr.Format(_T("%d"), value);
	WritePrivateProfileString(_T("Setting"), _T("AutoRefresh"), cstr, m_Ini);

	if(value == 0)
	{
		KillTimer(TIMER_AUTO_REFRESH);
	}
	else
	{
		KillTimer(TIMER_AUTO_REFRESH);
		SetTimer(TIMER_AUTO_REFRESH, 1000 * 60 * value, 0);
	}
}

void CDiskInfoDlg::CheckRadioAutoRefresh()
{
	int id = ID_AUTO_REFRESH_DISABLE;

	switch(m_AutoRefreshStatus)
	{
	case    1: id = ID_AUTO_REFRESH_01_MIN; break;
	case    3: id = ID_AUTO_REFRESH_03_MIN; break;
	case    5: id = ID_AUTO_REFRESH_05_MIN; break;
	case   10: id = ID_AUTO_REFRESH_10_MIN; break;
	case   30: id = ID_AUTO_REFRESH_30_MIN; break;
	case   60: id = ID_AUTO_REFRESH_60_MIN; break;
	case  120: id = ID_AUTO_REFRESH_120_MIN; break;
	case  180: id = ID_AUTO_REFRESH_180_MIN; break;
	case  360: id = ID_AUTO_REFRESH_360_MIN; break;
	case  720: id = ID_AUTO_REFRESH_720_MIN; break;
	case 1440: id = ID_AUTO_REFRESH_1440_MIN; break;

	default: id = ID_AUTO_REFRESH_DISABLE; break;
	}

	CMenu *menu = GetMenu();
	menu->CheckMenuRadioItem(ID_AUTO_REFRESH_DISABLE, ID_AUTO_REFRESH_1440_MIN, id, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
}


void CDiskInfoDlg::OnWait0Sec()
{
	CheckRadioWaitTime(ID_WAIT_0_SEC, 0);
}

void CDiskInfoDlg::OnWait5Sec(){	CheckRadioWaitTime(ID_WAIT_5_SEC, 5);	}
void CDiskInfoDlg::OnWait10Sec(){	CheckRadioWaitTime(ID_WAIT_10_SEC, 10);	}
void CDiskInfoDlg::OnWait15Sec(){	CheckRadioWaitTime(ID_WAIT_15_SEC, 15);	}
void CDiskInfoDlg::OnWait20Sec(){	CheckRadioWaitTime(ID_WAIT_20_SEC, 20);	}
void CDiskInfoDlg::OnWait30Sec(){	CheckRadioWaitTime(ID_WAIT_30_SEC, 30);	}
void CDiskInfoDlg::OnWait40Sec(){	CheckRadioWaitTime(ID_WAIT_40_SEC, 40);	}
void CDiskInfoDlg::OnWait50Sec(){	CheckRadioWaitTime(ID_WAIT_50_SEC, 50);	}
void CDiskInfoDlg::OnWait60Sec(){	CheckRadioWaitTime(ID_WAIT_60_SEC, 60);	}
void CDiskInfoDlg::OnWait90Sec(){	CheckRadioWaitTime(ID_WAIT_90_SEC, 90);	}
void CDiskInfoDlg::OnWait120Sec(){	CheckRadioWaitTime(ID_WAIT_120_SEC, 120);}
void CDiskInfoDlg::OnWait150Sec(){	CheckRadioWaitTime(ID_WAIT_150_SEC, 150);}
void CDiskInfoDlg::OnWait180Sec(){	CheckRadioWaitTime(ID_WAIT_180_SEC, 180);}
void CDiskInfoDlg::OnWait210Sec(){	CheckRadioWaitTime(ID_WAIT_210_SEC, 210);}
void CDiskInfoDlg::OnWait240Sec(){	CheckRadioWaitTime(ID_WAIT_240_SEC, 240);}

void CDiskInfoDlg::CheckRadioWaitTime(int id, int value)
{
	CMenu *menu = GetMenu();
	menu->CheckMenuRadioItem(ID_WAIT_0_SEC, ID_WAIT_240_SEC, id, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();

	m_WaitTimeStatus = value;

	CString cstr;
	cstr.Format(_T("%d"), value);
	WritePrivateProfileString(_T("Setting"), _T("StartupWaitTime"), cstr, m_Ini);
}

void CDiskInfoDlg::CheckRadioWaitTime()
{
	int id = ID_WAIT_0_SEC;

	switch(m_WaitTimeStatus)
	{
	case   0: id = ID_WAIT_0_SEC;	break;
	case   5: id = ID_WAIT_5_SEC;	break;
	case  10: id = ID_WAIT_10_SEC;	break;
	case  15: id = ID_WAIT_15_SEC;	break;
	case  20: id = ID_WAIT_20_SEC;	break;
	case  30: id = ID_WAIT_30_SEC;	break;
	case  40: id = ID_WAIT_40_SEC;	break;
	case  50: id = ID_WAIT_50_SEC;	break;
	case  60: id = ID_WAIT_60_SEC;	break;
	case  90: id = ID_WAIT_90_SEC;	break;
	case 120: id = ID_WAIT_120_SEC;	break;
	case 150: id = ID_WAIT_150_SEC;	break;
	case 180: id = ID_WAIT_180_SEC;	break;
	case 210: id = ID_WAIT_210_SEC;	break;
	case 240: id = ID_WAIT_240_SEC;	break;
	default:  id = ID_WAIT_0_SEC;	break;
	}

	CMenu *menu = GetMenu();
	menu->CheckMenuRadioItem(ID_WAIT_0_SEC, ID_WAIT_240_SEC, id, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnRawValues16()
{
	CheckRadioRawValues(ID_RAW_VALUES_16, 0);
}

void CDiskInfoDlg::OnRawValues10All()
{
	CheckRadioRawValues(ID_RAW_VALUES_10_ALL, 1);
}

void CDiskInfoDlg::OnRawValues2byte()
{
	CheckRadioRawValues(ID_RAW_VALUES_2BYTE, 2);
}

void CDiskInfoDlg::OnRawValues1byte()
{
	CheckRadioRawValues(ID_RAW_VALUES_1BYTE, 3);
}

void CDiskInfoDlg::CheckRadioRawValues(int id, int value)
{
	CMenu *menu = GetMenu();
	menu->CheckMenuRadioItem(ID_RAW_VALUES_16, ID_RAW_VALUES_1BYTE, id, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();

	m_RawValues = value;

	CString cstr;
	cstr.Format(_T("%d"), value);
	WritePrivateProfileString(_T("Setting"), _T("RawValues"), cstr, m_Ini);

	Refresh(TRUE);
}

void CDiskInfoDlg::CheckRadioRawValues()
{
	int id = ID_RAW_VALUES_16;

	switch(m_RawValues)
	{
	case   0: id = ID_RAW_VALUES_16;	break;
	case   1: id = ID_RAW_VALUES_10_ALL;break;
	case   2: id = ID_RAW_VALUES_2BYTE;	break;
	case   3: id = ID_RAW_VALUES_1BYTE;	break;
	default:  id = ID_RAW_VALUES_16;	break;
	}

	CMenu *menu = GetMenu();
	menu->CheckMenuRadioItem(ID_RAW_VALUES_16, ID_RAW_VALUES_1BYTE, id, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnRecordRawValues()
{
	m_RecordRawValues = !((BOOL)GetPrivateProfileInt(_T("Setting"), _T("RecordRawValues"), 0, m_Ini));

	CMenu* menu = GetMenu();
	menu->CheckMenuItem(ID_RECORD_RAW_VALUES, m_RecordRawValues ? MF_CHECKED : MF_UNCHECKED);
	SetMenu(menu);

	CString cstr;
	cstr.Format(_T("%d"), (INT)m_RecordRawValues);
	WritePrivateProfileString(_T("Setting"), _T("RecordRawValues"), cstr, m_Ini);

	DrawMenuBar();
	Refresh(TRUE); // SaveSmartInfo(i) does not work due to SAVE_SMART_PERIOD when initial stage
}

void CDiskInfoDlg::OnOpenDiskManagement()
{
	ShellExecute(NULL, NULL, _T("diskmgmt.msc"), NULL, NULL, SW_SHOWNORMAL);	
}

void CDiskInfoDlg::OnOpenDeviceManager()
{
	ShellExecute(NULL, NULL, _T("devmgmt.msc"), NULL, NULL, SW_SHOWNORMAL);	
}

void CDiskInfoDlg::OnAdvancedDiskSearch()
{
	CWaitCursor wait;
	BOOL flagChangeDisk = FALSE;

	if(m_bAdvancedDiskSearch)
	{
		m_bAdvancedDiskSearch = FALSE;
		InitAta(TRUE, m_bAdvancedDiskSearch, &flagChangeDisk, m_bWorkaroundHD204UI, m_bWorkaroundAdataSsd);

		if(flagChangeDisk)
		{
			// Update Menu and Dialog
			ChangeLang(m_CurrentLang);
		}
		WritePrivateProfileString(_T("Setting"), _T("AdvancedDiskSearch"), _T("0"), m_Ini);

		CMenu *menu = GetMenu();
		menu->CheckMenuItem(ID_ADVANCED_DISK_SEARCH, MF_UNCHECKED);
		SetMenu(menu);
		DrawMenuBar();
	}
	else
	{
		m_bAdvancedDiskSearch = TRUE;
		InitAta(TRUE, m_bAdvancedDiskSearch, &flagChangeDisk, m_bWorkaroundHD204UI, m_bWorkaroundAdataSsd);

		if(flagChangeDisk)
		{
			// Update Menu and Dialog
			ChangeLang(m_CurrentLang);
		}
		WritePrivateProfileString(_T("Setting"), _T("AdvancedDiskSearch"), _T("1"), m_Ini);

		CMenu *menu = GetMenu();
		menu->CheckMenuItem(ID_ADVANCED_DISK_SEARCH, MF_CHECKED);
		SetMenu(menu);
		DrawMenuBar();
	}

	if(m_bResident && flagChangeDisk)
	{
		for(int i = 0; i < CAtaSmart::MAX_DISK; i++)
		{
			RemoveTemperatureIcon(i);
		}
		CheckTrayTemperatureIcon();
	}
}

void CDiskInfoDlg::OnWorkaroundHD204UI()
{
	CWaitCursor wait;
	BOOL flagChangeDisk = FALSE;

	if(m_bWorkaroundHD204UI)
	{
		m_bWorkaroundHD204UI = FALSE;
		InitAta(TRUE, m_bAdvancedDiskSearch, &flagChangeDisk, m_bWorkaroundHD204UI, m_bWorkaroundAdataSsd);

		if(flagChangeDisk)
		{
			// Update Menu and Dialog
			ChangeLang(m_CurrentLang);
		}
		WritePrivateProfileString(_T("Workaround"), _T("HD204UI"), _T("0"), m_Ini);

		CMenu *menu = GetMenu();
		menu->CheckMenuItem(ID_WORKAROUND_HD204UI, MF_UNCHECKED);
		SetMenu(menu);
		DrawMenuBar();
	}
	else
	{
		m_bWorkaroundHD204UI = TRUE;
		InitAta(TRUE, m_bAdvancedDiskSearch, &flagChangeDisk, m_bWorkaroundHD204UI, m_bWorkaroundAdataSsd);

		if(flagChangeDisk)
		{
			// Update Menu and Dialog
			ChangeLang(m_CurrentLang);
		}
		WritePrivateProfileString(_T("Workaround"), _T("HD204UI"), _T("1"), m_Ini);

		CMenu *menu = GetMenu();
		menu->CheckMenuItem(ID_WORKAROUND_HD204UI, MF_CHECKED);
		SetMenu(menu);
		DrawMenuBar();
	}

	if(m_bResident && flagChangeDisk)
	{
		for(int i = 0; i < CAtaSmart::MAX_DISK; i++)
		{
			RemoveTemperatureIcon(i);
		}
		CheckTrayTemperatureIcon();
	}
}

void CDiskInfoDlg::OnWorkaroundIE8MODE()
{
	CWaitCursor wait;
	BOOL flagChangeDisk = FALSE;

	if(m_bWorkaroundIE8MODE)
	{
		m_bWorkaroundIE8MODE = FALSE;
		WritePrivateProfileString(_T("Workaround"), _T("IE8MODE"), _T("0"), m_Ini);
		CMenu *menu = GetMenu();
		menu->CheckMenuItem(ID_WORKAROUND_IE8MODE, MF_UNCHECKED);
		SetMenu(menu);
		DrawMenuBar();
	}
	else
	{
		m_bWorkaroundIE8MODE = TRUE;
		WritePrivateProfileString(_T("Workaround"), _T("IE8MODE"), _T("1"), m_Ini);
		CMenu *menu = GetMenu();
		menu->CheckMenuItem(ID_WORKAROUND_IE8MODE, MF_CHECKED);
		SetMenu(menu);
		DrawMenuBar();
	}
}

void CDiskInfoDlg::OnWorkaroundAdataSsd()
{
	CWaitCursor wait;
	BOOL flagChangeDisk = FALSE;

	if(m_bWorkaroundAdataSsd)
	{
		m_bWorkaroundAdataSsd = FALSE;
		InitAta(TRUE, m_bAdvancedDiskSearch, &flagChangeDisk, m_bWorkaroundHD204UI, m_bWorkaroundAdataSsd);

		if(flagChangeDisk)
		{
			// Update Menu and Dialog
			ChangeLang(m_CurrentLang);
		}
		WritePrivateProfileString(_T("Workaround"), _T("AdataSsd"), _T("0"), m_Ini);

		CMenu *menu = GetMenu();
		menu->CheckMenuItem(ID_WORKAROUND_ADATA_SSD, MF_UNCHECKED);
		SetMenu(menu);
		DrawMenuBar();
	}
	else
	{
		m_bWorkaroundAdataSsd = TRUE;
		InitAta(TRUE, m_bAdvancedDiskSearch, &flagChangeDisk, m_bWorkaroundHD204UI, m_bWorkaroundAdataSsd);

		if(flagChangeDisk)
		{
			// Update Menu and Dialog
			ChangeLang(m_CurrentLang);
		}
		WritePrivateProfileString(_T("Workaround"), _T("AdataSsd"), _T("1"), m_Ini);

		CMenu *menu = GetMenu();
		menu->CheckMenuItem(ID_WORKAROUND_ADATA_SSD, MF_CHECKED);
		SetMenu(menu);
		DrawMenuBar();
	}

	if(m_bResident && flagChangeDisk)
	{
		for(int i = 0; i < CAtaSmart::MAX_DISK; i++)
		{
			RemoveTemperatureIcon(i);
		}
		CheckTrayTemperatureIcon();
	}
}

void CDiskInfoDlg::OnWorkaroundIgnoreC4()
{
	CWaitCursor wait;
	BOOL flagChangeDisk = FALSE;

	if (m_bWorkaroundIgnoreC4)
	{
		m_bWorkaroundIgnoreC4 = FALSE;
		WritePrivateProfileString(_T("Workaround"), _T("IgnoreC4"), _T("0"), m_Ini);
		CMenu *menu = GetMenu();
		menu->CheckMenuItem(ID_WORKAROUND_IGNORE_C4, MF_UNCHECKED);
		SetMenu(menu);
		DrawMenuBar();
	}
	else
	{
		m_bWorkaroundIgnoreC4 = TRUE;
		WritePrivateProfileString(_T("Workaround"), _T("IgnoreC4"), _T("1"), m_Ini);
		CMenu *menu = GetMenu();
		menu->CheckMenuItem(ID_WORKAROUND_IGNORE_C4, MF_CHECKED);
		SetMenu(menu);
		DrawMenuBar();
	}
}

void CDiskInfoDlg::OnAtaPassThroughSmart()
{
	CMenu *menu = GetMenu();
	if(m_bAtaPassThroughSmart)
	{
		m_bAtaPassThroughSmart = FALSE;
		m_Ata.SetAtaPassThroughSmart(FALSE);
		menu->CheckMenuItem(ID_ATA_PASS_THROUGH_SMART, MF_UNCHECKED);
		WritePrivateProfileString(_T("Setting"), _T("AtaPassThroughSmart"), _T("0"), m_Ini);
	}
	else
	{
		m_bAtaPassThroughSmart = TRUE;
		m_Ata.SetAtaPassThroughSmart(TRUE);
		menu->CheckMenuItem(ID_ATA_PASS_THROUGH_SMART, MF_CHECKED);
		WritePrivateProfileString(_T("Setting"), _T("AtaPassThroughSmart"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnEventLog()
{
	CMenu *menu = GetMenu();
	if(m_bEventLog)
	{
		m_bEventLog = FALSE;
		menu->CheckMenuItem(ID_EVENT_LOG, MF_UNCHECKED);
		WritePrivateProfileString(_T("Setting"), _T("EventLog"), _T("0"), m_Ini);

	//	if(! m_bUseEventCreate)
	//	{
			UninstallEventSource();
	//	}
	}
	else
	{
		m_bEventLog = TRUE;
		menu->CheckMenuItem(ID_EVENT_LOG, MF_CHECKED);
		WritePrivateProfileString(_T("Setting"), _T("EventLog"), _T("1"), m_Ini);

	//	if(m_bUseEventCreate)
	//	{
			InstallEventSource();
	//	}
	}
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnAlertMail()
{
	CMenu *menu = GetMenu();
	if(m_bAlertMail)
	{
		m_bAlertMail = FALSE;
		menu->CheckMenuItem(ID_ALERT_MAIL, MF_UNCHECKED);
		WritePrivateProfileString(_T("Setting"), _T("AlertMail"), _T("0"), m_Ini);
	}
	else
	{
		m_bAlertMail = TRUE;
		menu->CheckMenuItem(ID_ALERT_MAIL, MF_CHECKED);
		WritePrivateProfileString(_T("Setting"), _T("AlertMail"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnAlertSound()
{
	CMenu *menu = GetMenu();
	if(m_bAlertSound)
	{
		m_bAlertSound = FALSE;
		menu->CheckMenuItem(ID_ALERT_SOUND, MF_UNCHECKED);
		WritePrivateProfileString(_T("Setting"), _T("AlertSound"), _T("0"), m_Ini);
		AlertSound(0, AS_SET_SOUND_ID);
	}
	else
	{
		m_bAlertSound = TRUE;
		menu->CheckMenuItem(ID_ALERT_SOUND, MF_CHECKED);
		WritePrivateProfileString(_T("Setting"), _T("AlertSound"), _T("1"), m_Ini);
		AlertSound(0, AS_SET_SOUND_ID);
	}
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnHideNoSmartDisk()
{
	CMenu *menu = GetMenu();
	if(m_bHideNoSmartDisk)
	{
		m_bHideNoSmartDisk = FALSE;
		menu->CheckMenuItem(ID_HIDE_NO_SMART_DISK, MF_UNCHECKED);
		WritePrivateProfileString(_T("Setting"), _T("HideNoSmartDisk"), _T("0"), m_Ini);
	}
	else
	{
		m_bHideNoSmartDisk = TRUE;
		menu->CheckMenuItem(ID_HIDE_NO_SMART_DISK, MF_CHECKED);
		WritePrivateProfileString(_T("Setting"), _T("HideNoSmartDisk"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();

	OnRescan();
}

void CDiskInfoDlg::OnGadgetSupport()
{
	CMenu *menu = GetMenu();
	if(m_bGadget)
	{
		m_bGadget = FALSE;
		menu->CheckMenuItem(ID_GADGET_SUPPORT, MF_UNCHECKED);
		WritePrivateProfileString(_T("Setting"), _T("Gadget"), _T("0"), m_Ini);
		DeleteShareInfo();
	}
	else
	{
		m_bGadget = TRUE;
		menu->CheckMenuItem(ID_GADGET_SUPPORT, MF_CHECKED);
		WritePrivateProfileString(_T("Setting"), _T("Gadget"), _T("1"), m_Ini);
		UpdateShareInfo();
	}
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnMailSettings()
{
	if(! m_AlertMailPath.IsEmpty())
	{
		ShellExecuteW(NULL, NULL, m_AlertMailPath, _T("/Settings"), NULL, SW_SHOW);
	}
}

void CDiskInfoDlg::CheckStartup()
{
	if(GetPrivateProfileInt(_T("Setting"), _T("Startup"), 0, m_Ini) == 1)
	{
		m_bStartup = TRUE;
		CMenu *menu = GetMenu();
		menu->CheckMenuItem(ID_STARTUP, MF_CHECKED);
		SetMenu(menu);
		DrawMenuBar();

		if(GetPrivateProfileInt(_T("Setting"), _T("StartupFixed"), 0, m_Ini) != 1)
		{
			RegisterStartup();
			// Fixed in 5.2.2  
			WritePrivateProfileString(_T("Setting"), _T("StartupFixed"), _T("1"), m_Ini);
		}
	}
	else
	{
		m_bStartup = FALSE;
		WritePrivateProfileString(_T("Setting"), _T("StartupFixed"), _T("1"), m_Ini);
	}
}

void CDiskInfoDlg::OnStartup()
{
	CMenu *menu = GetMenu();
	if(m_bStartup)
	{
		UnregisterStartup();
		m_bStartup = FALSE;
		menu->CheckMenuItem(ID_STARTUP, MF_UNCHECKED);
		WritePrivateProfileString(_T("Setting"), _T("Startup"), _T("0"), m_Ini);
	}
	else
	{
		RegisterStartup();
		m_bStartup = TRUE;
		menu->CheckMenuItem(ID_STARTUP, MF_CHECKED);
		WritePrivateProfileString(_T("Setting"), _T("Startup"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();
}

BOOL CDiskInfoDlg::RegisterStartup()
{
	OSVERSIONINFOEX osvi;
	BOOL bosVersionInfoEx;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if(!(bosVersionInfoEx = GetVersionEx((OSVERSIONINFO *)&osvi)))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx((OSVERSIONINFO *)&osvi);
	}

	TCHAR path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	CString cstr;

	if(osvi.dwMajorVersion >= 6)
	{
		STARTUPINFO si = {0};
		PROCESS_INFORMATION pi = {0};
		si.cb			= sizeof(STARTUPINFO);
		si.dwFlags		= STARTF_USESHOWWINDOW;
		si.wShowWindow	= SW_HIDE;
		cstr.Format(_T("schtasks.exe /Create /tn CrystalDiskInfo /tr \"\\\"%s\\\" \"/Startup\"\" /sc ONLOGON /RL HIGHEST /F"), path);
		::CreateProcess(NULL, (LPWSTR)cstr.GetString(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, 1000);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);

		// Change "StopIfGoingOnBatteries" and "DisallowStartIfOnBatteries"
		HRESULT hr = S_OK;

		ITaskService *pService = NULL;
		ITaskFolder *pRootFolder = NULL;
		IRegisteredTask *pRegisteredTask = NULL;
		ITaskDefinition *pDefinition = NULL;
		ITaskSettings *pSettings = NULL;
		LPCWSTR wszTaskName = L"CrystalDiskInfo";

		hr = CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (void **) &pService);
		if(SUCCEEDED(hr))
		{
			hr = pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());			
			if(SUCCEEDED(hr))
			{
				hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
				if(SUCCEEDED(hr))
				{
					hr = pRootFolder->GetTask(_bstr_t(wszTaskName), &pRegisteredTask);
					if(SUCCEEDED(hr))
					{						
						hr = pRegisteredTask->get_Definition(&pDefinition);
						if(SUCCEEDED(hr))
						{
							VARIANT_BOOL flag = FALSE;
							pDefinition->get_Settings(&pSettings);
							pSettings->put_DisallowStartIfOnBatteries(flag);
							pSettings->put_StopIfGoingOnBatteries(flag);
							pSettings->put_Priority(5);

							SAFE_RELEASE(pRegisteredTask);
							hr = pRootFolder->RegisterTaskDefinition(
								_bstr_t(wszTaskName), pDefinition, TASK_CREATE_OR_UPDATE, 
								_variant_t(), _variant_t(), TASK_LOGON_INTERACTIVE_TOKEN,
								_variant_t(L""), &pRegisteredTask);
						}
					}
				}
			}
		}
		SAFE_RELEASE(pSettings);
		SAFE_RELEASE(pDefinition);
		SAFE_RELEASE(pRegisteredTask);
		SAFE_RELEASE(pRootFolder);
		SAFE_RELEASE(pService);
	}
	else
	{
		HKEY hKey;
		DWORD disposition;
		LONG result;
		result = ::RegCreateKeyEx(HKEY_CURRENT_USER, 
			_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, _T(""),
			REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &disposition);
		if(result == ERROR_SUCCESS)
		{
			cstr.Format(_T("\"%s\" /Startup"), path);
			::RegSetValueEx(hKey, _T("CrystalDiskInfo"), 0, REG_SZ,
				(CONST BYTE*)(LPCTSTR)cstr,
				((DWORD)_tcslen(cstr) + 3) * 2);
			::RegCloseKey(hKey);
		}
	}

	return TRUE;
}

BOOL CDiskInfoDlg::UnregisterStartup()
{
	OSVERSIONINFOEX osvi;
	BOOL bosVersionInfoEx;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if(!(bosVersionInfoEx = GetVersionEx((OSVERSIONINFO *)&osvi)))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx((OSVERSIONINFO *)&osvi);
	}

	if(osvi.dwMajorVersion >= 6)
	{
		CString cstr;

		STARTUPINFO si = {0};
		PROCESS_INFORMATION pi = {0};
		si.cb			= sizeof(STARTUPINFO);
		si.dwFlags		= STARTF_USESHOWWINDOW;
		si.wShowWindow	= SW_HIDE;
		cstr.Format(_T("schtasks.exe /Delete /tn CrystalDiskInfo /F")); 
		::CreateProcess(NULL, (LPWSTR)cstr.GetString(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, 1000);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	else
	{
		HKEY hKey;
		DWORD disposition;
		LONG result;
		result = ::RegCreateKeyEx(HKEY_CURRENT_USER, 
			_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, _T(""),
			REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &disposition);
		if(result == ERROR_SUCCESS)
		{
			::RegDeleteValue(hKey, _T("CrystalDiskInfo"));
			::RegCloseKey(hKey);
		}
	}

	return TRUE;
}

void CDiskInfoDlg::OnCelsius()
{
	CMenu *menu = GetMenu();
	menu->CheckMenuRadioItem(ID_CELSIUS, ID_FAHRENHEIT, ID_CELSIUS, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
	m_bFahrenheit = FALSE;
	WritePrivateProfileString(_T("Setting"), _T("Temperature"), _T("0"), m_Ini);

	SelectDrive(m_SelectDisk);
}

void CDiskInfoDlg::OnFahrenheit()
{
	CMenu *menu = GetMenu();
	menu->CheckMenuRadioItem(ID_CELSIUS, ID_FAHRENHEIT, ID_FAHRENHEIT, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
	m_bFahrenheit = TRUE;
	WritePrivateProfileString(_T("Setting"), _T("Temperature"), _T("1"), m_Ini);

	SelectDrive(m_SelectDisk);
}

void CDiskInfoDlg::OnAutoAamApm()
{
	CMenu *menu = GetMenu();
	if(m_bAutoAamApm)
	{
		m_bAutoAamApm = FALSE;
		menu->CheckMenuItem(ID_AUTO_AAM_APM, MF_UNCHECKED);
		WritePrivateProfileString(_T("Setting"), _T("AutoAamApm"), _T("0"), m_Ini);
	}
	else
	{
		m_bAutoAamApm = TRUE;
		menu->CheckMenuItem(ID_AUTO_AAM_APM, MF_CHECKED);
		WritePrivateProfileString(_T("Setting"), _T("AutoAamApm"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnUsbSat()
{
	CMenu *menu = GetMenu();
	if(m_Ata.FlagUsbSat)
	{
		m_Ata.FlagUsbSat = FALSE;
		menu->CheckMenuItem(ID_USB_SAT, MF_UNCHECKED);
		WritePrivateProfileString(_T("USB"), _T("SAT"), _T("0"), m_Ini);
	}
	else
	{
		m_Ata.FlagUsbSat = TRUE;
		menu->CheckMenuItem(ID_USB_SAT, MF_CHECKED);
		WritePrivateProfileString(_T("USB"), _T("SAT"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnUsbIodata()
{
	CMenu *menu = GetMenu();
	if(m_Ata.FlagUsbIodata)
	{
		m_Ata.FlagUsbIodata = FALSE;
		menu->CheckMenuItem(ID_USB_IODATA, MF_UNCHECKED);
		WritePrivateProfileString(_T("USB"), _T("IODATA"), _T("0"), m_Ini);
	}
	else
	{
		m_Ata.FlagUsbIodata = TRUE;
		menu->CheckMenuItem(ID_USB_IODATA, MF_CHECKED);
		WritePrivateProfileString(_T("USB"), _T("IODATA"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnUsbSunplus()
{
	CMenu *menu = GetMenu();
	if(m_Ata.FlagUsbSunplus)
	{
		m_Ata.FlagUsbSunplus = FALSE;
		menu->CheckMenuItem(ID_USB_SUNPLUS, MF_UNCHECKED);
		WritePrivateProfileString(_T("USB"), _T("Sunplus"), _T("0"), m_Ini);
	}
	else
	{
		m_Ata.FlagUsbSunplus = TRUE;
		menu->CheckMenuItem(ID_USB_SUNPLUS, MF_CHECKED);
		WritePrivateProfileString(_T("USB"), _T("Sunplus"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnUsbLogitec()
{
	CMenu *menu = GetMenu();
	if(m_Ata.FlagUsbLogitec)
	{
		m_Ata.FlagUsbLogitec = FALSE;
		menu->CheckMenuItem(ID_USB_LOGITEC, MF_UNCHECKED);
		WritePrivateProfileString(_T("USB"), _T("Logitec"), _T("0"), m_Ini);
	}
	else
	{
		m_Ata.FlagUsbLogitec = TRUE;
		menu->CheckMenuItem(ID_USB_LOGITEC, MF_CHECKED);
		WritePrivateProfileString(_T("USB"), _T("Logitec"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnUsbProlific()
{
	CMenu *menu = GetMenu();
	if (m_Ata.FlagUsbProlific)
	{
		m_Ata.FlagUsbProlific = FALSE;
		menu->CheckMenuItem(ID_USB_PROLIFIC, MF_UNCHECKED);
		WritePrivateProfileString(_T("USB"), _T("Prolific"), _T("0"), m_Ini);
	}
	else
	{
		m_Ata.FlagUsbProlific = TRUE;
		menu->CheckMenuItem(ID_USB_PROLIFIC, MF_CHECKED);
		WritePrivateProfileString(_T("USB"), _T("Prolific"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnUsbJmicron()
{
	CMenu *menu = GetMenu();
	if(m_Ata.FlagUsbJmicron)
	{
		m_Ata.FlagUsbJmicron = FALSE;
		menu->CheckMenuItem(ID_USB_JMICRON, MF_UNCHECKED);
		WritePrivateProfileString(_T("USB"), _T("JMicron"), _T("0"), m_Ini);
	}
	else
	{
		m_Ata.FlagUsbJmicron = TRUE;
		menu->CheckMenuItem(ID_USB_JMICRON, MF_CHECKED);
		WritePrivateProfileString(_T("USB"), _T("JMicron"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnUsbCypress()
{
	CMenu *menu = GetMenu();
	if(m_Ata.FlagUsbCypress)
	{
		m_Ata.FlagUsbCypress = FALSE;
		menu->CheckMenuItem(ID_USB_CYPRESS, MF_UNCHECKED);
		WritePrivateProfileString(_T("USB"), _T("Cypress"), _T("0"), m_Ini);
	}
	else
	{
		m_Ata.FlagUsbCypress = TRUE;
		menu->CheckMenuItem(ID_USB_CYPRESS, MF_CHECKED);
		WritePrivateProfileString(_T("USB"), _T("Cypress"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnUsbMemory()
{
	CMenu *menu = GetMenu();
	if(m_Ata.FlagUsbMemory)
	{
		m_Ata.FlagUsbMemory = FALSE;
		menu->CheckMenuItem(ID_USB_MEMORY, MF_UNCHECKED);
		WritePrivateProfileString(_T("USB"), _T("UsbMemory"), _T("0"), m_Ini);
	}
	else
	{
		m_Ata.FlagUsbMemory = TRUE;
		menu->CheckMenuItem(ID_USB_MEMORY, MF_CHECKED);
		WritePrivateProfileString(_T("USB"), _T("UsbMemory"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();
}

/*
void CDiskInfoDlg::OnUsbSat16()
{
	CMenu* menu = GetMenu();
	if (m_Ata.FlagUsbSat16)
	{
		m_Ata.FlagUsbSat16 = FALSE;
		menu->CheckMenuItem(ID_USB_SAT16, MF_UNCHECKED);
		WritePrivateProfileString(_T("USB"), _T("UsbSAT16"), _T("0"), m_Ini);
	}
	else
	{
		m_Ata.FlagUsbSat16 = TRUE;
		menu->CheckMenuItem(ID_USB_SAT16, MF_CHECKED);
		WritePrivateProfileString(_T("USB"), _T("UsbSAT16"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();
}
*/

void CDiskInfoDlg::OnUsbNVMeJMicron()
{
	CMenu *menu = GetMenu();
	if (m_Ata.FlagUsbNVMeJMicron)
	{
		m_Ata.FlagUsbNVMeJMicron = FALSE;
		menu->CheckMenuItem(ID_USB_NVME_JMICRON, MF_UNCHECKED);
		WritePrivateProfileString(_T("USB"), _T("NVMeJMicron"), _T("0"), m_Ini);
	}
	else
	{
		m_Ata.FlagUsbNVMeJMicron = TRUE;
		menu->CheckMenuItem(ID_USB_NVME_JMICRON, MF_CHECKED);
		WritePrivateProfileString(_T("USB"), _T("NVMeJMicron"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnUsbNVMeASMedia()
{
	CMenu *menu = GetMenu();
	if (m_Ata.FlagUsbNVMeASMedia)
	{
		m_Ata.FlagUsbNVMeASMedia = FALSE;
		menu->CheckMenuItem(ID_USB_NVME_ASMEDIA, MF_UNCHECKED);
		WritePrivateProfileString(_T("USB"), _T("NVMeASMedia"), _T("0"), m_Ini);
	}
	else
	{
		m_Ata.FlagUsbNVMeASMedia = TRUE;
		menu->CheckMenuItem(ID_USB_NVME_ASMEDIA, MF_CHECKED);
		WritePrivateProfileString(_T("USB"), _T("NVMeASMedia"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnUsbNVMeRealtek()
{
	CMenu *menu = GetMenu();
	if (m_Ata.FlagUsbNVMeRealtek)
	{
		m_Ata.FlagUsbNVMeRealtek = FALSE;
		menu->CheckMenuItem(ID_USB_NVME_REALTEK, MF_UNCHECKED);
		WritePrivateProfileString(_T("USB"), _T("NVMeRealtek"), _T("0"), m_Ini);
	}
	else
	{
		m_Ata.FlagUsbNVMeRealtek = TRUE;
		menu->CheckMenuItem(ID_USB_NVME_REALTEK, MF_CHECKED);
		WritePrivateProfileString(_T("USB"), _T("NVMeRealtek"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnMegaRAID()
{
	CMenu *menu = GetMenu();
	if (m_Ata.FlagMegaRAID)
	{
		m_Ata.FlagMegaRAID = FALSE;
		menu->CheckMenuItem(ID_MEGA_RAID, MF_UNCHECKED);
		WritePrivateProfileString(_T("Setting"), _T("MegaRAID"), _T("0"), m_Ini);
	}
	else
	{
		m_Ata.FlagMegaRAID = TRUE;
		menu->CheckMenuItem(ID_MEGA_RAID, MF_CHECKED);
		WritePrivateProfileString(_T("Setting"), _T("MegaRAID"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnUsbASM1352R()
{
	CMenu* menu = GetMenu();
	if (m_Ata.FlagUsbASM1352R)
	{
		m_Ata.FlagUsbASM1352R = FALSE;
		menu->CheckMenuItem(ID_USB_ASM1352R, MF_UNCHECKED);
		WritePrivateProfileString(_T("USB"), _T("ASM1352R"), _T("0"), m_Ini);
	}
	else
	{
		m_Ata.FlagUsbASM1352R = TRUE;
		menu->CheckMenuItem(ID_USB_ASM1352R, MF_CHECKED);
		WritePrivateProfileString(_T("USB"), _T("ASM1352R"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnUsbEnableAll()
{
	m_Ata.FlagUsbSat     = FALSE;
	m_Ata.FlagUsbIodata  = FALSE;
	m_Ata.FlagUsbSunplus = FALSE;
	m_Ata.FlagUsbLogitec = FALSE;
	m_Ata.FlagUsbProlific = FALSE;
	m_Ata.FlagUsbJmicron = FALSE;
	m_Ata.FlagUsbCypress = FALSE;
	m_Ata.FlagUsbASM1352R = FALSE;
	m_Ata.FlagUsbMemory  = FALSE;
//	m_Ata.FlagUsbSat16 = FALSE;
	m_Ata.FlagUsbNVMeJMicron = FALSE;
	m_Ata.FlagUsbNVMeASMedia = FALSE;
	m_Ata.FlagUsbNVMeRealtek = FALSE;

	OnUsbSat();
	OnUsbIodata();
	OnUsbSunplus();
	OnUsbLogitec();
	OnUsbProlific();
	OnUsbJmicron();
	OnUsbCypress();
	OnUsbMemory();
//	OnUsbSat16();
	OnUsbASM1352R();
	OnUsbNVMeJMicron();
	OnUsbNVMeASMedia();
	OnUsbNVMeRealtek();
}

void CDiskInfoDlg::OnUsbDisableAll()
{
	m_Ata.FlagUsbSat     = TRUE;
	m_Ata.FlagUsbIodata  = TRUE;
	m_Ata.FlagUsbSunplus = TRUE;
	m_Ata.FlagUsbLogitec = TRUE;
	m_Ata.FlagUsbProlific = TRUE;
	m_Ata.FlagUsbJmicron = TRUE;
	m_Ata.FlagUsbCypress = TRUE;
	m_Ata.FlagUsbASM1352R = TRUE;
	m_Ata.FlagUsbMemory  = TRUE;
//	m_Ata.FlagUsbSat16 = FALSE;
	m_Ata.FlagUsbNVMeJMicron = TRUE;
	m_Ata.FlagUsbNVMeASMedia = TRUE;
	m_Ata.FlagUsbNVMeRealtek = TRUE;

	OnUsbSat();
	OnUsbIodata();
	OnUsbSunplus();
	OnUsbLogitec();
	OnUsbProlific();
	OnUsbJmicron();
	OnUsbCypress();
	OnUsbMemory();
//	OnUsbSat16();
	OnUsbASM1352R();
	OnUsbNVMeJMicron();
	OnUsbNVMeASMedia();
	OnUsbNVMeRealtek();
}

void CDiskInfoDlg::OnDumpIdentifyDevice()
{
	CMenu *menu = GetMenu();
	if(m_bDumpIdentifyDevice)
	{
		m_bDumpIdentifyDevice = FALSE;
		menu->CheckMenuItem(ID_DUMP_IDENTIFY_DEVICE, MF_UNCHECKED);
		WritePrivateProfileString(_T("Setting"), _T("DumpIdentifyDevice"), _T("0"), m_Ini);
	}
	else
	{
		m_bDumpIdentifyDevice = TRUE;
		menu->CheckMenuItem(ID_DUMP_IDENTIFY_DEVICE, MF_CHECKED);
		WritePrivateProfileString(_T("Setting"), _T("DumpIdentifyDevice"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnDumpSmartReadData()
{
	CMenu *menu = GetMenu();
	if(m_bDumpSmartReadData)
	{
		m_bDumpSmartReadData = FALSE;
		menu->CheckMenuItem(ID_DUMP_SMART_READ_DATA, MF_UNCHECKED);
		WritePrivateProfileString(_T("Setting"), _T("DumpSmartReadData"), _T("0"), m_Ini);
	}
	else
	{
		m_bDumpSmartReadData = TRUE;
		menu->CheckMenuItem(ID_DUMP_SMART_READ_DATA, MF_CHECKED);
		WritePrivateProfileString(_T("Setting"), _T("DumpSmartReadData"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnDumpSmartReadThreshold()
{
	CMenu *menu = GetMenu();
	if(m_bDumpSmartReadThreshold)
	{
		m_bDumpSmartReadThreshold = FALSE;
		menu->CheckMenuItem(ID_DUMP_SMART_READ_THRESHOLD, MF_UNCHECKED);
		WritePrivateProfileString(_T("Setting"), _T("DumpSmartReadThreshold"), _T("0"), m_Ini);
	}
	else
	{
		m_bDumpSmartReadThreshold = TRUE;
		menu->CheckMenuItem(ID_DUMP_SMART_READ_THRESHOLD, MF_CHECKED);
		WritePrivateProfileString(_T("Setting"), _T("DumpSmartReadThreshold"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();
}


void CDiskInfoDlg::OnAsciiView()
{
	CMenu *menu = GetMenu();
	if(m_bAsciiView)
	{
		m_bAsciiView = FALSE;
		menu->CheckMenuItem(ID_ASCII_VIEW, MF_UNCHECKED);
		WritePrivateProfileString(_T("Setting"), _T("AsciiView"), _T("0"), m_Ini);
	}
	else
	{
		m_bAsciiView = TRUE;
		menu->CheckMenuItem(ID_ASCII_VIEW, MF_CHECKED);
		WritePrivateProfileString(_T("Setting"), _T("AsciiView"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnSmartEnglish()
{
	CMenu *menu = GetMenu();
	if(m_bSmartEnglish)
	{
		m_bSmartEnglish = FALSE;
		menu->CheckMenuItem(ID_SMART_ENGLISH, MF_UNCHECKED);
		WritePrivateProfileString(_T("Setting"), _T("SmartEnglish"), _T("0"), m_Ini);
	}
	else
	{
		m_bSmartEnglish = TRUE;
		menu->CheckMenuItem(ID_SMART_ENGLISH, MF_CHECKED);
		WritePrivateProfileString(_T("Setting"), _T("SmartEnglish"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();

	UpdateListCtrl(m_SelectDisk);
}

void CDiskInfoDlg::OnFontSetting()
{
	CFontSelectionDlg fontSelection(this);
	if (fontSelection.DoModal() == IDOK)
	{
		m_FontFace = fontSelection.GetFontFace();
		m_FontScale = fontSelection.GetFontScale();
		m_FontRatio = m_FontScale / 100.0;
		m_FontRender = fontSelection.GetFontRender();

		CString cstr;
		WritePrivateProfileString(L"Setting", L"FontFace", L"\"" + m_FontFace + L"\"", m_Ini);
		cstr.Format(L"%d", m_FontScale);
		WritePrivateProfileString(L"Setting", L"FontScale", cstr, m_Ini);
		cstr.Format(L"%d", m_FontRender);
		WritePrivateProfileString(L"Setting", L"FontRender", cstr, m_Ini);

		UpdateDialogSize();
	}
}


void CDiskInfoDlg::OnResidentHide()
{
	CMenu *menu = GetMenu();
	menu->CheckMenuRadioItem(ID_RESIDENT_HIDE, ID_RESIDENT_MINIMIZE, ID_RESIDENT_HIDE, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
	m_bResidentMinimize = FALSE;
	WritePrivateProfileString(_T("Setting"), _T("ResidentMinimize"), _T("0"), m_Ini);
}

void CDiskInfoDlg::OnResidentMinimize()
{
	CMenu *menu = GetMenu();
	menu->CheckMenuRadioItem(ID_RESIDENT_HIDE, ID_RESIDENT_MINIMIZE, ID_RESIDENT_MINIMIZE, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
	m_bResidentMinimize = TRUE;
	WritePrivateProfileString(_T("Setting"), _T("ResidentMinimize"), _T("1"), m_Ini);
}

void CDiskInfoDlg::OnZoom100()
{
	if(CheckRadioZoomType(ID_ZOOM_100, 100))
	{
	//	ReExecute();
		UpdateDialogSize();
	}
}

void CDiskInfoDlg::OnZoom125()
{
	if(CheckRadioZoomType(ID_ZOOM_125, 125))
	{
	//	ReExecute();
		UpdateDialogSize();
	}
}

void CDiskInfoDlg::OnZoom150()
{
	if(CheckRadioZoomType(ID_ZOOM_150, 150))
	{
	//	ReExecute();
		UpdateDialogSize();
	}
}

void CDiskInfoDlg::OnZoom200()
{
	if(CheckRadioZoomType(ID_ZOOM_200, 200))
	{
	//	ReExecute();
		UpdateDialogSize();
	}
}

void CDiskInfoDlg::OnZoom250()
{
	if (CheckRadioZoomType(ID_ZOOM_250, 250))
	{
		//	ReExecute();
		UpdateDialogSize();
	}
}

void CDiskInfoDlg::OnZoom300()
{
	if(CheckRadioZoomType(ID_ZOOM_300, 300))
	{
	//	ReExecute();
		UpdateDialogSize();
	}
}


void CDiskInfoDlg::OnZoomAuto()
{
	if(CheckRadioZoomType(ID_ZOOM_AUTO, 0))
	{
	//	ReExecute();
		UpdateDialogSize();
	}
}

BOOL CDiskInfoDlg::CheckRadioZoomType(int id, int value)
{
	if(m_ZoomType == value)
	{
		return FALSE;
	}

	CMenu *menu = GetMenu();
	menu->CheckMenuRadioItem(ID_ZOOM_100, ID_ZOOM_AUTO, id, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();

	m_ZoomType = value;

	CString cstr;
	cstr.Format(_T("%d"), value);
	WritePrivateProfileString(_T("Setting"), _T("ZoomType"), cstr, m_Ini);

	ChangeZoomType(m_ZoomType);

	return TRUE;
}

void CDiskInfoDlg::CheckRadioZoomType()
{
	int id = ID_ZOOM_AUTO;

	switch(m_ZoomType)
	{
	case 100: id = ID_ZOOM_100;	break;
	case 125: id = ID_ZOOM_125;	break;
	case 150: id = ID_ZOOM_150;	break;
	case 200: id = ID_ZOOM_200;	break;
	case 250: id = ID_ZOOM_250;	break;
	case 300: id = ID_ZOOM_300;	break;
	default:  id = ID_ZOOM_AUTO;	break;
	}

	CMenu *menu = GetMenu();
	menu->CheckMenuRadioItem(ID_ZOOM_100, ID_ZOOM_AUTO, id, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::CheckRadioAutoDetection()
{
	int id = ID_AUTO_DETECTION_DISABLE;

	switch(m_AutoDetectionStatus)
	{
	case   5: id = ID_AUTO_DETECTION_05_SEC;	break;
	case  10: id = ID_AUTO_DETECTION_10_SEC;	break;
	case  20: id = ID_AUTO_DETECTION_20_SEC;	break;
	case  30: id = ID_AUTO_DETECTION_30_SEC;	break;
	default:  id = ID_AUTO_DETECTION_DISABLE;	break;
	}

	CMenu *menu = GetMenu();
	menu->CheckMenuRadioItem(ID_AUTO_DETECTION_05_SEC, ID_AUTO_DETECTION_DISABLE, id, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnAutoDetectionDisable()
{
	CheckRadioAutoDetection(ID_AUTO_DETECTION_DISABLE, 0);
}

void CDiskInfoDlg::OnAutoDetection05Sec(){	CheckRadioAutoDetection(ID_AUTO_DETECTION_05_SEC, 5);}
void CDiskInfoDlg::OnAutoDetection10Sec(){	CheckRadioAutoDetection(ID_AUTO_DETECTION_10_SEC, 10);	}
void CDiskInfoDlg::OnAutoDetection20Sec(){	CheckRadioAutoDetection(ID_AUTO_DETECTION_20_SEC, 20);	}
void CDiskInfoDlg::OnAutoDetection30Sec(){	CheckRadioAutoDetection(ID_AUTO_DETECTION_30_SEC, 30);	}

void CDiskInfoDlg::CheckRadioAutoDetection(int id, int value)
{
	CMenu *menu = GetMenu();
	menu->CheckMenuRadioItem(ID_AUTO_DETECTION_05_SEC, ID_AUTO_DETECTION_DISABLE, id, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();

	m_AutoDetectionStatus = value;

	CString cstr;
	cstr.Format(_T("%d"), value);
	WritePrivateProfileString(_T("Setting"), _T("AutoDetection"), cstr, m_Ini);
}

void CDiskInfoDlg::OnCsmiDisable()
{
	CheckRadioCsmiType(ID_CSMI_DISABLE, 0);
}

void CDiskInfoDlg::OnCsmiEnableAuto()
{
	CheckRadioCsmiType(ID_CSMI_ENABLE_AUTO, 1);
}

void CDiskInfoDlg::OnCsmiEnableRaid()
{
	CheckRadioCsmiType(ID_CSMI_ENABLE_RAID, 2);
}

void CDiskInfoDlg::OnCsmiEnableAll()
{
	CheckRadioCsmiType(ID_CSMI_ENABLE_ALL, 3);
}

void CDiskInfoDlg::CheckRadioCsmiType(int id, int value)
{
	CMenu *menu = GetMenu();
	menu->CheckMenuRadioItem(ID_CSMI_DISABLE, ID_CSMI_ENABLE_ALL, id, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();

	m_Ata.CsmiType = value;

	CString cstr;
	cstr.Format(_T("%d"), value);
	WritePrivateProfileString(_T("Setting"), _T("CsmiType"), cstr, m_Ini);

	OnRescan();
}

void CDiskInfoDlg::CheckRadioCsmiType()
{
	int id = ID_CSMI_DISABLE;

	switch(m_Ata.CsmiType)
	{
	case   0: id = ID_CSMI_DISABLE;	break;
	case   1: id = ID_CSMI_ENABLE_AUTO;	break;
	case   2: id = ID_CSMI_ENABLE_RAID;	break;
	case   3: id = ID_CSMI_ENABLE_ALL;	break;
	default:  id = ID_CSMI_ENABLE_AUTO;	break;
	}

	CMenu *menu = GetMenu();
	menu->CheckMenuRadioItem(ID_CSMI_DISABLE, ID_CSMI_ENABLE_ALL, id, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
}

void CDiskInfoDlg::OnInstallGadget()
{
	ShellExecute(NULL, _T("open"), m_GadgetDir, NULL, NULL, SW_SHOWNORMAL);
}
