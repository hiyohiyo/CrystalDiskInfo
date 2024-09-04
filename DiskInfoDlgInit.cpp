/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DiskInfo.h"
#include "DiskInfoDlg.h"
#include "UtilityFx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef BOOL(WINAPI *FuncEnableNonClientDpiScaling) (HWND hWnd);

extern const GUID StrageGUID;

int CALLBACK EnumFontFamExProcDefaultFont(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, int FontType, LPARAM lParam)
{
	if(_tcscmp(lpelfe->elfLogFont.lfFaceName, DEFAULT_FONT_FACE_1) == 0)
	{
		*((BOOL*)lParam) = TRUE;
	}
    return TRUE;
}

BOOL CDiskInfoDlg::OnInitDialog()
{
	//BOOL result = FALSE;

	CMainDialogFx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIconMini, FALSE);	// Set small icon

	InitThemeLang();
	InitMenu();
//	ChangeTheme(m_CurrentTheme);
	UpdateThemeInfo();
//	ChangeLang(m_CurrentLang);

	TCHAR str[256];
	
	CClientDC dc(this);
    LOGFONT logfont;
	CString defaultFontFace;
	BOOL hasDefaultFont = FALSE;
    ZeroMemory(&logfont, sizeof(LOGFONT)); 
    logfont.lfCharSet = DEFAULT_CHARSET;
    ::EnumFontFamiliesExW(dc.m_hDC, &logfont, (FONTENUMPROC)EnumFontFamExProcDefaultFont, (INT_PTR)(&hasDefaultFont), 0);
	
	if(hasDefaultFont)
	{
		defaultFontFace = DEFAULT_FONT_FACE_1;
	}
	else
	{
		defaultFontFace = DEFAULT_FONT_FACE_2;
	}

	GetPrivateProfileStringFx(_T("Setting"), _T("FontFace"), defaultFontFace, str, 256, m_Ini);
	m_FontFace = str;
	m_FontScale = GetPrivateProfileInt(L"Setting", L"FontScale", 100, m_Ini);
	if (m_FontScale > 150 || m_FontScale < 50)
	{
		m_FontScale = 100;
		m_FontRatio = 1.0;
	}
	else
	{
		m_FontRatio = m_FontScale / 100.0;
	}

	m_FontRender = (BYTE)GetPrivateProfileInt(L"Setting", L"FontRender", CLEARTYPE_NATURAL_QUALITY, m_Ini);
	if (m_FontRender > CLEARTYPE_NATURAL_QUALITY)
	{
		m_FontRender = CLEARTYPE_NATURAL_QUALITY;
	}

	switch(GetPrivateProfileInt(_T("Setting"), _T("AutoRefresh"), 10, m_Ini))
	{
	case  1: OnAutoRefresh01Min(); break;
	case  3: OnAutoRefresh03Min(); break;
	case  5: OnAutoRefresh05Min(); break;
	case 10: OnAutoRefresh10Min(); break;
	case 30: OnAutoRefresh30Min(); break;
	case 60: OnAutoRefresh60Min(); break;
	case 120:OnAutoRefresh120Min(); break;
	case 180:OnAutoRefresh180Min(); break;
	case 360:OnAutoRefresh360Min(); break;
	case 720:OnAutoRefresh720Min(); break;
	case 1440:OnAutoRefresh1440Min(); break;
	default: OnAutoRefreshDisable(); break;
	}

	switch(GetPrivateProfileInt(_T("Setting"), _T("StartupWaitTime"), 30, m_Ini))
	{
	case   0: OnWait0Sec();   break;
	case   5: OnWait5Sec();   break;
	case  10: OnWait10Sec();  break;
	case  15: OnWait15Sec();  break;
	case  20: OnWait20Sec();  break;
	case  30: OnWait30Sec();  break;
	case  40: OnWait40Sec();  break;
	case  50: OnWait50Sec();  break;
	case  60: OnWait60Sec();  break;
	case  90: OnWait90Sec();  break;
	case 120: OnWait120Sec(); break;
	case 150: OnWait150Sec(); break;
	case 180: OnWait180Sec(); break;
	case 210: OnWait210Sec(); break;
	case 240: OnWait240Sec(); break;
	default:  OnWait0Sec();   break;
	}

	switch(GetPrivateProfileInt(_T("Setting"), _T("ZoomType"), 0, m_Ini))
	{
	case 100:  CheckRadioZoomType(ID_ZOOM_100, 100); break;
	case 125:  CheckRadioZoomType(ID_ZOOM_125, 125); break;
	case 150:  CheckRadioZoomType(ID_ZOOM_150, 150); break;
	case 200:  CheckRadioZoomType(ID_ZOOM_200, 200); break;
	case 250:  CheckRadioZoomType(ID_ZOOM_250, 250); break;
	case 300:  CheckRadioZoomType(ID_ZOOM_300, 300); break;
	default:   CheckRadioZoomType(ID_ZOOM_AUTO, 0); break;
	}

	switch(GetPrivateProfileInt(_T("Setting"), _T("Temperature"), 0, m_Ini))
	{
	case   1:	OnFahrenheit(); break;
	default:	OnCelsius();	break;
	}

	switch(GetPrivateProfileInt(_T("Setting"), _T("ResidentMinimize"), 0, m_Ini))
	{
	case   1:	OnResidentMinimize(); break;
	default:	OnResidentHide();	  break;
	}

	switch (GetPrivateProfileInt(_T("Setting"), _T("SortDriveLetter"), 1, m_Ini))
	{
	case   1:	OnSortDriveLetter();	 break;
	default:	OnSortPhysicalDriveId(); break;
	}

	switch (GetPrivateProfileInt(_T("Setting"), _T("DriveMenu"), 8, m_Ini))
	{
	case   16:	OnDriveMenu16();	break;
#ifdef MAX_DRIVE_20
	case   10:	OnDriveMenu10();	break;
	case   20:	OnDriveMenu20();	break;
#endif
	default:	OnDriveMenu8(); 	break;
	}

#ifdef SUISHO_SHIZUKU_SUPPORT
	if (m_bStartupVoice)
	{
		m_bStartupVoice = FALSE;
		OnStartupVoice();
	}
#endif

	// USB/IEEE1394 Support
	m_Ata.FlagUsbSat     = ! GetPrivateProfileInt(_T("USB"), _T("SAT"), 1, m_Ini);
	m_Ata.FlagUsbIodata  = ! GetPrivateProfileInt(_T("USB"), _T("IODATA"), 1, m_Ini);
	m_Ata.FlagUsbSunplus = ! GetPrivateProfileInt(_T("USB"), _T("Sunplus"), 1, m_Ini);
	m_Ata.FlagUsbLogitec = ! GetPrivateProfileInt(_T("USB"), _T("Logitec"), 1, m_Ini);
	m_Ata.FlagUsbProlific = !GetPrivateProfileInt(_T("USB"), _T("Prolific"), 1, m_Ini);
	m_Ata.FlagUsbJmicron = ! GetPrivateProfileInt(_T("USB"), _T("JMicron"), 1, m_Ini);
	m_Ata.FlagUsbCypress = ! GetPrivateProfileInt(_T("USB"), _T("Cypress"), 1, m_Ini);
	m_Ata.FlagUsbASM1352R = !GetPrivateProfileInt(_T("USB"), _T("ASM1352R"), 0, m_Ini);
	m_Ata.FlagUsbRealtek9220DP = !GetPrivateProfileInt(_T("USB"), _T("RTL9220DP"), 0, m_Ini);
	m_Ata.FlagUsbMemory  = ! GetPrivateProfileInt(_T("USB"), _T("UsbMemory"), 0, m_Ini);
//	m_Ata.FlagUsbSat16 = !GetPrivateProfileInt(_T("USB"), _T("UsbSAT16"), 1, m_Ini);
	m_Ata.FlagUsbNVMeJMicron3 = !GetPrivateProfileInt(_T("USB"), _T("NVMeJMicron3"), 0, m_Ini);
	m_Ata.FlagUsbNVMeJMicron = !GetPrivateProfileInt(_T("USB"), _T("NVMeJMicron"), 1, m_Ini);
	m_Ata.FlagUsbNVMeASMedia = !GetPrivateProfileInt(_T("USB"), _T("NVMeASMedia"), 1, m_Ini);
	m_Ata.FlagUsbNVMeRealtek = !GetPrivateProfileInt(_T("USB"), _T("NVMeRealtek"), 1, m_Ini);
	m_Ata.FlagAMD_RC2 = !GetPrivateProfileInt(_T("Setting"), _T("AMD_RC2"), 1, m_Ini);
	m_Ata.FlagMegaRAID = !GetPrivateProfileInt(_T("Setting"), _T("MegaRAID"), 1, m_Ini);
	m_Ata.FlagIntelVROC = !GetPrivateProfileInt(_T("Setting"), _T("IntelVROC"), 1, m_Ini);

#ifdef JMICRON_USB_RAID_SUPPORT
	m_Ata.FlagUsbJMS56X = !GetPrivateProfileInt(_T("Setting"), _T("JMS56X"), 0, m_Ini); // Default Off
	m_Ata.FlagUsbJMB39X = !GetPrivateProfileInt(_T("Setting"), _T("JMB39X"), 0, m_Ini); // Default Off
	m_Ata.FlagUsbJMS586_20 = !GetPrivateProfileInt(_T("Setting"), _T("JMS586_20"), 0, m_Ini); // Default Off
	m_Ata.FlagUsbJMS586_40 = !GetPrivateProfileInt(_T("Setting"), _T("JMS586_40"), 0, m_Ini); // Default Off
#endif

	OnUsbSat();
	OnUsbIodata();
	OnUsbSunplus();
	OnUsbLogitec();
	OnUsbProlific();
	OnUsbJmicron();
	OnUsbCypress();
	OnUsbASM1352R();
	OnUsbRealtek9220DP();
	OnUsbMemory();
//	OnUsbSat16();
	OnUsbNVMeJMicron3();
	OnUsbNVMeJMicron();
	OnUsbNVMeASMedia();
	OnUsbNVMeRealtek();
	OnAMD_RC2();
	OnMegaRAID();
	OnIntelVROC();
#ifdef JMICRON_USB_RAID_SUPPORT
	OnUsbJMS56X();
	OnUsbJMB39X();
	OnUsbJMS586_20();
	OnUsbJMS586_40();
#endif

	DebugPrint(_T("InitAta"));
	InitAta((BOOL)GetPrivateProfileInt(_T("Setting"), _T("UseWMI"), 1, m_Ini), m_bAdvancedDiskSearch, NULL, m_bWorkaroundHD204UI, m_bWorkaroundAdataSsd);

	if(m_bStartupExit)
	{
		// Added 2013/04/12 - Workaround for Exec Failed
		WritePrivateProfileStringFx(_T("Workaround"), _T("ExecFailed"), _T("0"), m_Ini);
		DebugPrint(_T("EndDialog(0)"));
		EndDialog(0);
		return FALSE;
	}

	m_SizeX = SIZE_X;
	m_SizeY = SIZE_Y;

	DEV_BROADCAST_DEVICEINTERFACE filter;
	filter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	filter.dbcc_classguid = StrageGUID;

	DebugPrint(_T("RegisterDeviceNotification"));
	m_hDevNotify = RegisterDeviceNotification(m_hWnd, &filter, DEVICE_NOTIFY_WINDOW_HANDLE);

	InitDialogComplete();

#ifdef SUISHO_SHIZUKU_SUPPORT
	if (m_bStartupVoice && ! m_bStartupVoiceDisabled)
	{
		AlertSound(1, AS_SET_SOUND_ID);
		AlertSound(1000, AS_PLAY_SOUND);
	}
#endif

	if (m_bResident)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void CDiskInfoDlg::RestorePos()
{
	const int x = GetPrivateProfileInt(_T("Setting"), _T("X"), INT_MIN, m_Ini);
	const int y = GetPrivateProfileInt(_T("Setting"), _T("Y"), INT_MIN, m_Ini);

	RECT rw, rc;
	GetWindowRect(&rw);

	rc.left = x;
	rc.top = y;
	rc.right = x + rw.right - rw.left;
	rc.bottom = y + rw.bottom - rw.top;

	HMONITOR hMonitor = MonitorFromRect(&rc, MONITOR_DEFAULTTONULL);
	if (hMonitor == nullptr)
	{
		DebugPrint(_T("CenterWindow()"));
		CenterWindow();
	}
	else
	{
		// Get Taskbar Size
		APPBARDATA	taskbarInfo = { 0 };
		taskbarInfo.cbSize = sizeof(APPBARDATA);
		taskbarInfo.hWnd = m_hWnd;
		SHAppBarMessage(ABM_GETTASKBARPOS, &taskbarInfo);
		CRect taskbarRect = taskbarInfo.rc;

		/// <summary>
		/// Debug Code
		/// </summary>
		/// CString cstr;
		/// cstr.Format(L"top=%d, bottom=%d, left=%d, right=%d", taskbarInfo.rc.top, taskbarInfo.rc.bottom, taskbarInfo.rc.left, taskbarInfo.rc.right);
		/// AfxMessageBox(cstr);

		if (taskbarInfo.rc.top <= 0 && taskbarInfo.rc.left <= 0) // Top Side or Left Side
		{
			if (taskbarRect.Height() > taskbarRect.Width()) // Left Side
			{
				if (x < taskbarRect.Width()) // Overlap
				{
					SetWindowPos(nullptr, taskbarRect.Width(), y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
				}
				else
				{
					SetWindowPos(nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
				}				
			}
			else // Top Side
			{
				if (y < taskbarRect.Height()) // Overlap
				{
					SetWindowPos(nullptr, x, taskbarRect.Height(), 0, 0, SWP_NOSIZE | SWP_NOZORDER);
				}
				else
				{
					SetWindowPos(nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
				}				
			}
		}
		else
		{
			SetWindowPos(nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}		
	}
}

void CDiskInfoDlg::InitDialogComplete()
{
	DebugPrint(_T("InitDialogComplete"));
	static BOOL once = FALSE;
	if(! once)
	{
		DebugPrint(_T("RestorePos()"));
		RestorePos();

		DebugPrint(_T("CheckStartup"));
		CheckStartup();
		// CheckResident();
		DebugPrint(_T("CheckHideSerialNumber"));
		CheckHideSerialNumber();
	//	DebugPrint(_T("ChangeTheme"));
	//	ChangeTheme(m_CurrentTheme);

		DebugPrint(_T("ChangeZoomType"));
		ChangeZoomType(m_ZoomType);
		DebugPrint(_T("UpdateDialogSize"));
		UpdateDialogSize();
		DebugPrint(_T("ChangeLang"));
		ChangeLang(m_CurrentLang);
		DebugPrint(_T("CheckPage()"));
		CheckPage();
			
		m_bShowWindow = TRUE;

		if(m_bResident)
		{
			DebugPrint(_T("AlarmOverheat()"));
			AlarmOverheat();
		//	CheckTrayTemperatureIcon();
			if(! m_bResidentMinimize)
			{
				m_bShowWindow = FALSE;
			}
		}
		else
		{
			DebugPrint(_T("ShowWindow(SW_SHOW)"));
			ShowWindow(SW_SHOW);
		}

		if(m_NowDetectingUnitPowerOnHours != TRUE)
		{
			DebugPrint(_T("SetWindowTitle()"));
			SetWindowTitle(_T(""));
		}

		once = TRUE;
		m_bInitializing = FALSE;


		// Added 2013/04/12 - Workaround for Exec Failed
		WritePrivateProfileStringFx(_T("Workaround"), _T("ExecFailed"), _T("0"), m_Ini);

		if(! ((CDiskInfoApp*)AfxGetApp())->m_SaveAsText.IsEmpty())
		{
			SaveText(((CDiskInfoApp*)AfxGetApp())->m_SaveAsText);
		}

		if(((CDiskInfoApp*)AfxGetApp())->m_bCopyExit)
		{
			DebugPrint(_T("EndDialog(0)"));
			EndDialog(0);
		}
	}
}

void CDiskInfoDlg::InitAta(BOOL useWmi, BOOL advancedDiskSearch, PBOOL flagChangeDisk, BOOL workaroundHD204UI, BOOL workaroundAdataSsd)
{
	static BOOL once = FALSE;

	KillTimer(TIMER_SET_POWER_ON_UNIT);
	SetWindowTitle(i18n(_T("Message"), _T("DETECT_DISK")));
	m_NowDetectingUnitPowerOnHours = FALSE;

	m_Ata.Init(useWmi, advancedDiskSearch, flagChangeDisk, workaroundHD204UI, workaroundAdataSsd, m_bHideNoSmartDisk, m_bSortDriveLetter);
	
	DWORD errorCount = 0;
	for(int i = 0; i < m_Ata.vars.GetCount(); i++)
	{
		int unitType = GetPrivateProfileIntFx(_T("PowerOnUnit"), m_Ata.vars[i].ModelSerial, -1, m_Ini);
		if(unitType >= 0)
		{
			if(m_Ata.vars[i].DetectedTimeUnitType == m_Ata.POWER_ON_MILLI_SECONDS)
			{
				unitType = m_Ata.POWER_ON_MILLI_SECONDS;
			}
			else if(m_Ata.vars[i].DetectedTimeUnitType == m_Ata.POWER_ON_10_MINUTES)
			{
				unitType = m_Ata.POWER_ON_10_MINUTES;
			}
			m_Ata.vars[i].MeasuredTimeUnitType = unitType;
			m_Ata.vars[i].MeasuredPowerOnHours = m_Ata.GetPowerOnHoursEx(i, unitType);
		}
		else if(m_Ata.vars[i].PowerOnRawValue > 0)
		{
			errorCount++;
		}

		m_bAutoRefreshTarget[i] = GetPrivateProfileIntFx(_T("AutoRefreshTarget"), m_Ata.vars[i].ModelSerial, 1, m_Ini);
		if (m_Ata.vars[i].IsSsd)
		{
			m_Ata.vars[i].AlarmTemperature = GetPrivateProfileIntFx(_T("AlarmTemperature"), m_Ata.vars[i].ModelSerial, 60, m_Ini);
		}
		else
		{
			m_Ata.vars[i].AlarmTemperature = GetPrivateProfileIntFx(_T("AlarmTemperature"), m_Ata.vars[i].ModelSerial, 50, m_Ini);
		}
		m_Ata.vars[i].AlarmHealthStatus = GetPrivateProfileIntFx(_T("AlarmHealthStatus"), m_Ata.vars[i].ModelSerial, 1, m_Ini);

		m_Ata.vars[i].Threshold05     = (WORD)GetPrivateProfileIntFx(_T("ThreasholdOfCaution05"), m_Ata.vars[i].ModelSerial, 1, m_Ini);
		m_Ata.vars[i].ThresholdC5     = (WORD)GetPrivateProfileIntFx(_T("ThreasholdOfCautionC5"), m_Ata.vars[i].ModelSerial, 1, m_Ini);
		m_Ata.vars[i].ThresholdC6     = (WORD)GetPrivateProfileIntFx(_T("ThreasholdOfCautionC6"), m_Ata.vars[i].ModelSerial, 1, m_Ini);
		m_Ata.vars[i].ThresholdFF     = (WORD)GetPrivateProfileIntFx(_T("ThreasholdOfCautionFF"), m_Ata.vars[i].ModelSerial, 10, m_Ini);

		m_Ata.vars[i].DiskStatus = m_Ata.CheckDiskStatus(i);
		DebugPrint(_T("SaveSmartInfo(i)"));
		SaveSmartInfo(i);
	}

	if (!once)
	{
		DebugPrint(_T("CheckResident()"));
		CheckResident();
		once = TRUE;
	}

	AlertSound(0, AS_PLAY_SOUND);
	if(errorCount)
	{
		DebugPrint(_T("SetTimer(TIMER_SET_POWER_ON_UNIT, 130000, 0)"));
		SetTimer(TIMER_SET_POWER_ON_UNIT, 130000, 0);
		m_NowDetectingUnitPowerOnHours = TRUE;
	}
	SetWindowTitle(_T(""));

	DebugPrint(_T("AutoAamApmAdaption()"));
	AutoAamApmAdaption();

	DebugPrint(_T("UpdateShareInfo()"));
	UpdateShareInfo();
}

void CDiskInfoDlg::InitListCtrl()
{
	DWORD style = ListView_GetExtendedListViewStyle(m_List.m_hWnd);
	style |= LVS_EX_FULLROWSELECT | /*LVS_EX_GRIDLINES |*/ LVS_EX_LABELTIP ;
	ListView_SetExtendedListViewStyle(m_List.m_hWnd, style);

	m_List.SetImageList(&m_ImageList, LVSIL_SMALL);
}

CString CDiskInfoDlg::GetDiskStatus(DWORD statusCode)
{
	switch(statusCode)
	{
	case CAtaSmart::DISK_STATUS_GOOD:
		return i18n(_T("DiskStatus"), _T("GOOD"));
		break;
	case CAtaSmart::DISK_STATUS_CAUTION:
		return i18n(_T("DiskStatus"), _T("CAUTION"));
		break;
	case CAtaSmart::DISK_STATUS_BAD:
		return i18n(_T("DiskStatus"), _T("BAD"));
		break;
	case CAtaSmart::DISK_STATUS_UNKNOWN:
	default:
		return i18n(_T("DiskStatus"), _T("UNKNOWN"));
		break;
	}
}

CString CDiskInfoDlg::GetDiskStatusClass(DWORD statusCode)
{
	switch(statusCode)
	{
	case CAtaSmart::DISK_STATUS_GOOD:
		if (m_bGreenMode)
		{
			return _T("diskStatusGoodGreen");
		}
		else
		{
			return _T("diskStatusGood");
		}		
		break;
	case CAtaSmart::DISK_STATUS_CAUTION:
		return _T("diskStatusCaution");
		break;
	case CAtaSmart::DISK_STATUS_BAD:
		return _T("diskStatusBad");
		break;
	case CAtaSmart::DISK_STATUS_UNKNOWN:
	default:
		return _T("diskStatusUnknown");
		break;
	}
}

CString CDiskInfoDlg::GetTemperatureClass(INT temperature, INT alarmTemperature)
{
	if(temperature >= alarmTemperature)
	{
		return _T("temperatureBad");
	}/*
	else if(temperature == alarmTemperature)
	{
		return _T("temperatureCaution");
	}*/
	else if(temperature == -1000)
	{
		return _T("temperatureUnknown");
	}
	else
	{
		if (m_bGreenMode)
		{
			return _T("temperatureGoodGreen");
		}
		else
		{
			return _T("temperatureGood");
		}
	}
}

CString CDiskInfoDlg::GetDiskStatusReason(DWORD index)
{
	CString result, cstr;
	DWORD value = 0;

	if (m_Ata.vars[index].IsNVMe)
	{
		BYTE cw = m_Ata.vars[index].SmartReadData[0];
		if (cw & 0x01) { result += L"NG: Available spare capacity has fallen below the threshold.\r\n"; };
		if (cw & 0x02) { result += L"NG: Temperature error (Overheat or Overcool)\r\n"; };
		if (cw & 0x04) { result += L"NG: NVM subsystem reliability has been degraded.\r\n"; };
		if (cw & 0x08) { result += L"NG: Media has been placed in Read Only Mode.\r\n"; };
		if (cw & 0x10) { result += L"NG: Volatile memory backup device has Failed.\r\n"; };
		if (cw & 0x20) { result += L"NG: Persistent memory region has become Read-Only.\r\n"; };

		result.TrimRight();
		return result;
	}
	
	if(m_Ata.vars[index].DiskStatus == CAtaSmart::DISK_STATUS_BAD)
	{
		if(m_Ata.vars[index].IsSsd && m_Ata.vars[index].IsRawValues8)
		{
		}
		else if(m_Ata.vars[index].IsSsd)
		{
			for(DWORD j = 0; j < m_Ata.vars[index].AttributeCount; j++)
			{
				if( m_Ata.vars[index].Threshold[j].ThresholdValue != 0
				&& 	m_Ata.vars[index].Attribute[j].CurrentValue < m_Ata.vars[index].Threshold[j].ThresholdValue)
				{
					cstr.Format(_T("%02X"), m_Ata.vars[index].Attribute[j].Id);
					result += i18n(_T("DiskStatus"), _T("BAD")) + _T(" (") + cstr + _T(") ")+ i18n(m_Ata.vars[index].SmartKeyName, cstr);
					cstr.Format(_T("\n"));
					result += cstr;
				}
			}
		}
		else
		{
			for(DWORD j = 0; j < m_Ata.vars[index].AttributeCount; j++)
			{
				if(((0x01 <= m_Ata.vars[index].Attribute[j].Id && m_Ata.vars[index].Attribute[j].Id <= 0x0D)
//				||	m_Ata.vars[index].Attribute[j].Id == 0xB8
				||	(0xBB <= m_Ata.vars[index].Attribute[j].Id && m_Ata.vars[index].Attribute[j].Id <= 0xC1)
				||	(0xC3 <= m_Ata.vars[index].Attribute[j].Id && m_Ata.vars[index].Attribute[j].Id <= 0xD1)
				||	(0xD3 <= m_Ata.vars[index].Attribute[j].Id && m_Ata.vars[index].Attribute[j].Id <= 0xD4)
				||	(0xDC <= m_Ata.vars[index].Attribute[j].Id && m_Ata.vars[index].Attribute[j].Id <= 0xE4)
				||	(0xE6 <= m_Ata.vars[index].Attribute[j].Id && m_Ata.vars[index].Attribute[j].Id <= 0xE7)
				||	m_Ata.vars[index].Attribute[j].Id == 0xF0
				||	m_Ata.vars[index].Attribute[j].Id == 0xFA
				||	m_Ata.vars[index].Attribute[j].Id == 0xFE
				)
				&&	m_Ata.vars[index].Threshold[j].ThresholdValue != 0
				&& 	m_Ata.vars[index].Attribute[j].CurrentValue < m_Ata.vars[index].Threshold[j].ThresholdValue)
				{
					cstr.Format(_T("%02X"), m_Ata.vars[index].Attribute[j].Id);
					result += i18n(_T("DiskStatus"), _T("BAD")) + _T(" (") + cstr + _T(") ")+ i18n(m_Ata.vars[index].SmartKeyName, cstr);
					cstr.Format(_T("\n"));
					result += cstr;
				}
			}
		}
	}

	if(m_Ata.vars[index].DiskStatus == CAtaSmart::DISK_STATUS_CAUTION
	|| m_Ata.vars[index].DiskStatus == CAtaSmart::DISK_STATUS_BAD)
	{
		for(DWORD j = 0; j < m_Ata.vars[index].AttributeCount; j++)
		{
			if(m_Ata.vars[index].Attribute[j].Id == 0x05 // Reallocated Sectors Count
			|| m_Ata.vars[index].Attribute[j].Id == 0xC5 // Current Pending Sector Count
			|| m_Ata.vars[index].Attribute[j].Id == 0xC6 // Off-Line Scan Uncorrectable Sector Count
			)
			{
				value = MAKELONG(
							MAKEWORD(
								m_Ata.vars[index].Attribute[j].RawValue[0],
								m_Ata.vars[index].Attribute[j].RawValue[1]),
							MAKEWORD(
								m_Ata.vars[index].Attribute[j].RawValue[2],
								m_Ata.vars[index].Attribute[j].RawValue[3])
							);
				if(value != 0xFFFFFFFF && value != 0x0 && ! m_Ata.vars[index].IsSsd)
				{
					cstr.Format(_T("%02X"), m_Ata.vars[index].Attribute[j].Id);
					result += i18n(_T("DiskStatus"), _T("CAUTION")) + _T(" [") + cstr + _T("] ")+ i18n(_T("Smart"), cstr);
					cstr.Format(_T(" : %d\n"), value);
					result += cstr;
				}
			}
			else if(
			   (m_Ata.vars[index].Attribute[j].Id == 0xA9 && (m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_REALTEK || (m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_KINGSTON && m_Ata.vars[index].HostReadsWritesUnit == m_Ata.HOST_READS_WRITES_32MB) || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_SILICONMOTION))
			|| (m_Ata.vars[index].Attribute[j].Id == 0xAD && (m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_TOSHIBA || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_KIOXIA))
			|| (m_Ata.vars[index].Attribute[j].Id == 0xB1 && m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_SAMSUNG)
			|| (m_Ata.vars[index].Attribute[j].Id == 0xBB && m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_MTRON)
			|| (m_Ata.vars[index].Attribute[j].Id == 0xCA && (m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_MICRON || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_MICRON_MU03 || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_INTEL_DC || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_SILICONMOTION_CVC))
			|| (m_Ata.vars[index].Attribute[j].Id == 0xD1 && m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_INDILINX)
			|| (m_Ata.vars[index].Attribute[j].Id == 0xE6 && (m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_WDC || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_SANDISK || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_SANDISK_LENOVO || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_SANDISK_DELL))
			|| (m_Ata.vars[index].Attribute[j].Id == 0xE8 && m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_PLEXTOR)
			|| (m_Ata.vars[index].Attribute[j].Id == 0xE7 && (m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_SANDFORCE || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_CORSAIR || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_KINGSTON || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_SKHYNIX
				                                          || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_REALTEK || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_SANDISK || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_SSSTC || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_APACER
				                                          || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_JMICRON || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_SEAGATE || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_MAXIOTEK || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_YMTC
				                                          || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_SCY || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_RECADATA || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_ADATA_INDUSTRIAL))
			|| (m_Ata.vars[index].Attribute[j].Id == 0xE9 && (m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_INTEL || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_OCZ || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_OCZ_VECTOR || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_SKHYNIX))
			|| (m_Ata.vars[index].Attribute[j].Id == 0xE9 && m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_SANDISK_LENOVO_HELEN_VENUS)
			|| (m_Ata.vars[index].Attribute[j].Id == 0xC9 && (m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_SANDISK_HP || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_SANDISK_HP_VENUS))
				)
			{
				cstr.Format(_T("%02X"), m_Ata.vars[index].Attribute[j].Id);
				if(m_Ata.vars[index].DiskStatus == CAtaSmart::DISK_STATUS_CAUTION)
				{
					result += i18n(_T("DiskStatus"), _T("CAUTION")) + _T(" [") + cstr + _T("] ") + i18n(m_Ata.vars[index].SmartKeyName, cstr);
				}
				else if(m_Ata.vars[index].DiskStatus == CAtaSmart::DISK_STATUS_BAD)
				{
					result += i18n(_T("DiskStatus"), _T("BAD")) + _T(" [") + cstr + _T("] ") + i18n(m_Ata.vars[index].SmartKeyName, cstr);
				}
				cstr.Format(_T(" : %d\n"), m_Ata.vars[index].Life);
				result += cstr;
			}
		}
	}

	result.TrimRight();
	return result;
}

CString CDiskInfoDlg::GetLogicalDriveInfo(DWORD index, INT maxLength)
{
	DWORD map = m_Ata.vars[index].DriveLetterMap;
	CString resultS, resultM, resultL;
	ULARGE_INTEGER freeBytesAvailableToCaller = {};
	ULARGE_INTEGER totalNumberOfBytes = {};
	ULARGE_INTEGER totalNumberOfFreeBytes = {};

	for(int j = 0; j < 26; j++)
	{
		if(map & (1 << j))
		{
			CString cstr;
			CString letter;
			TCHAR volumeNameBuffer[256] = {};

			letter.Format(_T("%C:\\"), j + 'A');
			GetDiskFreeSpaceEx(letter, &freeBytesAvailableToCaller,
				&totalNumberOfBytes, &totalNumberOfFreeBytes);
			GetVolumeInformation(letter,  volumeNameBuffer, 256, NULL, NULL, NULL, NULL, 0);

			if (totalNumberOfBytes.QuadPart == 0)
			{
				cstr.Format(_T("%C: %s\r\n"), j + 'A', volumeNameBuffer);
			}
			else if (totalNumberOfFreeBytes.QuadPart < totalNumberOfBytes.QuadPart)
			{
				cstr.Format(_T("%C: %s [%.1f/%.1f GB (%.1f %%)]\r\n"), 
					j + 'A', volumeNameBuffer,
					totalNumberOfFreeBytes.QuadPart / 1024 / 1024 / 1024.0,
					totalNumberOfBytes.QuadPart  / 1024 / 1024 / 1024.0,
					(double)totalNumberOfFreeBytes.QuadPart / (double)totalNumberOfBytes.QuadPart * 100);
			}
			else
			{
				cstr.Format(_T("%C: %s [%.1f/%.1f GB]\r\n"),
					j + 'A', volumeNameBuffer,
					totalNumberOfFreeBytes.QuadPart / 1024 / 1024 / 1024.0,
					totalNumberOfBytes.QuadPart / 1024 / 1024 / 1024.0);
			}
		
			resultL += cstr;

			cstr.Format(_T("%C: %s [%.1f GB]\r\n"), 
				j + 'A', volumeNameBuffer,
				totalNumberOfBytes.QuadPart  / 1024 / 1024 / 1024.0);

			resultM += cstr;

			cstr.Format(_T("%C: %s\r\n"), 
				j + 'A', volumeNameBuffer);

			resultS += cstr;
		}
	}
	resultL.TrimRight();
	resultM.TrimRight();
	resultS.TrimRight();

	if(resultL.GetLength() < maxLength)
	{
		return resultL;
	}
	else if(resultM.GetLength() < maxLength)
	{
		return resultM;
	}
	else
	{
		return resultS;
	}
}

void CDiskInfoDlg::InitDriveList()
{
	CString cstr;
	CString delimiter;
	CString mini = L"";

	if (m_bHalfDriveMenu)
	{
		mini = L"Mini";
	}

	if (m_Ata.vars.GetCount() / m_DriveMenuNumber == m_DriveMenuPage)
	{
		for(int i = m_Ata.vars.GetCount() % m_DriveMenuNumber; i < m_DriveMenuNumber; i++)
		{
			m_LiDisk[i] = _T("");
			m_ButtonDisk[i].ReloadImage(IP(L"noDisk" + mini), 1);
			m_ButtonDisk[i].SetToolTipText(L"");
		//	m_ButtonDisk[i].EnableWindow(FALSE);
			m_ButtonDisk[i].SetHandCursor(FALSE);
		}
	}

	for (int i = 0; i < m_DriveMenuNumber; i++)
	{
		m_ButtonDisk[i].ShowWindow(SW_SHOW);
		m_ButtonDisk[i].SetSelected(FALSE);
	}
	for (int i = m_DriveMenuNumber; i < MAX_DRIVE_MENU; i++)
	{
		m_ButtonDisk[i].ShowWindow(SW_HIDE);
	}

	delimiter = _T("\r\n");

	for(int i = 0; i < m_Ata.vars.GetCount(); i++)
	{
		CString className, driveLetter, diskStatus;
		if(m_Ata.vars[i].DriveMap.IsEmpty())
		{
			if(m_Ata.vars[i].PhysicalDriveId >= 0)
			{
				driveLetter.Format(_T("Disk %d"), m_Ata.vars[i].PhysicalDriveId);
			}
			else
			{
				driveLetter.Format(_T("Disk --"));
			}
		}
		else if(m_Ata.vars[i].DriveMap.GetLength() > 15)
		{
			driveLetter = m_Ata.vars[i].DriveMap.Left(11) + _T(" ...");
		}
		else
		{
			driveLetter = m_Ata.vars[i].DriveMap;
		}

		diskStatus = GetDiskStatus(m_Ata.vars[i].DiskStatus);
		className = GetDiskStatusClass(m_Ata.vars[i].DiskStatus);

		if (m_bHalfDriveMenu)
		{
			className += L"Mini";
			diskStatus = L" ";
			driveLetter.Replace(L":", L"");
			driveLetter.Replace(L" ", L"");	
		}

		if(m_SelectDisk == i)
		{
			m_ButtonDisk[i % m_DriveMenuNumber].SetSelected(TRUE);
		}

		// DriveMenu
		if(i / m_DriveMenuNumber == m_DriveMenuPage)
		{
			CString targetDisk;
			targetDisk.Format(_T("Disk%d"), i % m_DriveMenuNumber);
			if (m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_NVME && m_Ata.vars[i].Temperature > -300)
			{
				if (m_bFahrenheit)
				{
					m_LiDisk[i % m_DriveMenuNumber].Format(_T("%s%s%d °F%s%s"),
						diskStatus.GetString(), delimiter.GetString(), m_Ata.vars[i].Temperature * 9 / 5 + 32, delimiter.GetString(), driveLetter.GetString());
				}
				else
				{
					m_LiDisk[i % m_DriveMenuNumber].Format(_T("%s%s%d °C%s%s"),
						diskStatus.GetString(), delimiter.GetString(), m_Ata.vars[i].Temperature, delimiter.GetString(), driveLetter.GetString());
				}
			}
			else if(m_Ata.vars[i].IsSmartEnabled && m_Ata.vars[i].Temperature > -300)
			{
				if(m_bFahrenheit)
				{
					m_LiDisk[i % m_DriveMenuNumber].Format(_T("%s%s%d °F%s%s"),
								diskStatus.GetString(), delimiter.GetString(), m_Ata.vars[i].Temperature * 9 / 5 + 32, delimiter.GetString(), driveLetter.GetString());
				}
				else
				{
					m_LiDisk[i % m_DriveMenuNumber].Format(_T("%s%s%d °C%s%s"),
								diskStatus.GetString(), delimiter.GetString(), m_Ata.vars[i].Temperature, delimiter.GetString(), driveLetter.GetString());
				}
			}
			else if(m_Ata.vars[i].IsSmartEnabled)
			{
				if(m_bFahrenheit)
				{
					m_LiDisk[i % m_DriveMenuNumber].Format(_T("%s%s-- °F%s%s"), diskStatus.GetString(), delimiter.GetString(), delimiter.GetString(), driveLetter.GetString());
				}
				else
				{
					m_LiDisk[i % m_DriveMenuNumber].Format(_T("%s%s-- °C%s%s"), diskStatus.GetString(), delimiter.GetString(), delimiter.GetString(), driveLetter.GetString());
				}
			}
			else
			{
				if(m_bFahrenheit)
				{
					m_LiDisk[i % m_DriveMenuNumber].Format(_T("----%s-- °F%s%s"), delimiter.GetString(), delimiter.GetString(), driveLetter.GetString());
				}
				else
				{
					m_LiDisk[i % m_DriveMenuNumber].Format(_T("----%s-- °C%s%s"), delimiter.GetString(), delimiter.GetString(), driveLetter.GetString());
				}
			}

			if(m_Ata.vars[i].PhysicalDriveId >= 0)
			{
				cstr.Format(_T("Disk %d : %s : %.1f GB\n%s"), m_Ata.vars[i].PhysicalDriveId, m_Ata.vars[i].Model.GetString(), m_Ata.vars[i].TotalDiskSize / 1000.0, GetLogicalDriveInfo(i).GetString());
			}
			else
			{
				cstr.Format(_T("Disk -- : %s : %.1f GB\n%s"), m_Ata.vars[i].Model.GetString(), m_Ata.vars[i].TotalDiskSize / 1000.0, GetLogicalDriveInfo(i).GetString());
			}
			
			m_ButtonDisk[i % m_DriveMenuNumber].SetToolTipText(cstr);
			className.Replace(L"Status", L"");
			m_ButtonDisk[i % m_DriveMenuNumber].ReloadImage(IP(className), 4);
			m_ButtonDisk[i % m_DriveMenuNumber].SetHandCursor(TRUE);
		//	m_ButtonDisk[i % m_DriveMenuNumber].EnableWindow(TRUE);
		}
	}

	UpdateData(FALSE);
}
