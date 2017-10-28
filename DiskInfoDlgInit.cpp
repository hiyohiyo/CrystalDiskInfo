/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DiskInfo.h"
#include "DiskInfoDlg.h"
#include "GetFileVersion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef BOOL(WINAPI *FuncEnableNonClientDpiScaling) (HWND hWnd);

extern const GUID StrageGUID;

int CALLBACK EnumFontFamExProcMeiryo(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, int FontType, LPARAM lParam)
{
	if(_tcscmp(lpelfe->elfLogFont.lfFaceName, _T("メイリオ")) == 0)
	{
		BOOL *flag = (BOOL*)lParam;
		*flag = TRUE;
	}
    return TRUE;
}

BOOL CDiskInfoDlg::OnInitDialog()
{
	BOOL result = FALSE;

	CMainDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIconMini, FALSE);		// Set small icon

	InitThemeLang();
	InitMenu();

	// メイン画面をレイヤードウィンドウにする。
	::SetWindowLong(m_hWnd, GWL_EXSTYLE, ::GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	::SetLayeredWindowAttributes(m_hWnd, 0, 255, LWA_ALPHA);

//	m_BackgroundName = L"ShizukuOriginal";

	TCHAR str[256];
	
	CClientDC dc(this);
    LOGFONT logfont;
	CString defaultFontFace;
	BOOL hasMeiryo = FALSE;
    ZeroMemory(&logfont, sizeof(LOGFONT)); 
    logfont.lfCharSet = ANSI_CHARSET;
    ::EnumFontFamiliesExW(dc.m_hDC, &logfont, (FONTENUMPROC)EnumFontFamExProcMeiryo, (INT_PTR)(&hasMeiryo), 0);
	
	if(hasMeiryo)
	{
		defaultFontFace = _T("メイリオ");
	}
	else
	{
		defaultFontFace = _T("Tahoma");
	}

	GetPrivateProfileString(_T("Setting"), _T("FontFace"), defaultFontFace, str, 256, m_Ini);
	m_FontFace = str;

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

	// USB/IEEE1394 Support
	m_Ata.FlagUsbSat     = ! GetPrivateProfileInt(_T("USB"), _T("SAT"), 1, m_Ini);
	m_Ata.FlagUsbIodata  = ! GetPrivateProfileInt(_T("USB"), _T("IODATA"), 1, m_Ini);
	m_Ata.FlagUsbSunplus = ! GetPrivateProfileInt(_T("USB"), _T("Sunplus"), 1, m_Ini);
	m_Ata.FlagUsbLogitec1 = ! GetPrivateProfileInt(_T("USB"), _T("Logitec"), 1, m_Ini);
	m_Ata.FlagUsbLogitec2 = !GetPrivateProfileInt(_T("USB"), _T("Logitec2"), 1, m_Ini);
	m_Ata.FlagUsbJmicron = ! GetPrivateProfileInt(_T("USB"), _T("JMicron"), 1, m_Ini);
	m_Ata.FlagUsbCypress = ! GetPrivateProfileInt(_T("USB"), _T("Cypress"), 1, m_Ini);
	m_Ata.FlagUsbMemory  = ! GetPrivateProfileInt(_T("USB"), _T("UsbMemory"), 0, m_Ini);

	OnUsbSat();
	OnUsbIodata();
	OnUsbSunplus();
	OnUsbLogitec1();
	OnUsbLogitec2();
	OnUsbJmicron();
	OnUsbCypress();
	OnUsbMemory();

	DebugPrint(_T("InitAta"));
	InitAta((BOOL)GetPrivateProfileInt(_T("Setting"), _T("UseWMI"), 1, m_Ini), m_FlagAdvancedDiskSearch, NULL, m_FlagWorkaroundHD204UI, m_FlagWorkaroundAdataSsd);

	if(m_FlagStartupExit)
	{
		// Added 2013/04/12 - Workaround for Exec Failed
		WritePrivateProfileString(_T("Workaround"), _T("ExecFailed"), _T("0"), m_Ini);
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



	return TRUE; 
}

void CDiskInfoDlg::InitDialogComplete()
{
	DebugPrint(_T("InitDialogComplete"));
	static BOOL once = FALSE;
	if(! once)
	{
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
			
		m_FlagShowWindow = TRUE;
		DebugPrint(_T("CenterWindow()"));
		CenterWindow();

		if(m_FlagResident)
		{
			DebugPrint(_T("AlarmOverheat()"));
			AlarmOverheat();
		//	CheckTrayTemperatureIcon();
			if(! m_FlagResidentMinimize)
			{
				m_FlagShowWindow = FALSE;
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
		m_FlagInitializing = FALSE;


		// Added 2013/04/12 - Workaround for Exec Failed
		WritePrivateProfileString(_T("Workaround"), _T("ExecFailed"), _T("0"), m_Ini);

		if(! ((CDiskInfoApp*)AfxGetApp())->m_SaveAsText.IsEmpty())
		{
			CopySave(((CDiskInfoApp*)AfxGetApp())->m_SaveAsText);
		}

		if(((CDiskInfoApp*)AfxGetApp())->m_FlagCopyExit)
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

	m_Ata.Init(useWmi, advancedDiskSearch, flagChangeDisk, workaroundHD204UI, workaroundAdataSsd, m_FlagHideNoSmartDisk);
	
	if(! once)
	{
		DebugPrint(_T("CheckResident()"));
		CheckResident();
		once = TRUE;
	}

	DWORD errorCount = 0;
	for(int i = 0; i < m_Ata.vars.GetCount(); i++)
	{
		int unitType = GetPrivateProfileInt(_T("PowerOnUnit"), m_Ata.vars[i].ModelSerial, -1, m_Ini);
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

		m_FlagAutoRefreshTarget[i] = GetPrivateProfileInt(_T("AutoRefreshTarget"), m_Ata.vars[i].ModelSerial, 1, m_Ini);;
		m_Ata.vars[i].AlarmTemperature = GetPrivateProfileInt(_T("AlarmTemperature"), m_Ata.vars[i].ModelSerial, 50, m_Ini);
		m_Ata.vars[i].AlarmHealthStatus = GetPrivateProfileInt(_T("AlarmHealthStatus"), m_Ata.vars[i].ModelSerial, 1, m_Ini);

		m_Ata.vars[i].Threshold05     = GetPrivateProfileInt(_T("ThreasholdOfCaution05"), m_Ata.vars[i].ModelSerial, 1, m_Ini);
		m_Ata.vars[i].ThresholdC5     = GetPrivateProfileInt(_T("ThreasholdOfCautionC5"), m_Ata.vars[i].ModelSerial, 1, m_Ini);
		m_Ata.vars[i].ThresholdC6     = GetPrivateProfileInt(_T("ThreasholdOfCautionC6"), m_Ata.vars[i].ModelSerial, 1, m_Ini);
		m_Ata.vars[i].ThresholdFF     = GetPrivateProfileInt(_T("ThreasholdOfCautionFF"), m_Ata.vars[i].ModelSerial, 10, m_Ini);

		m_Ata.vars[i].DiskStatus = m_Ata.CheckDiskStatus(i);
		DebugPrint(_T("SaveSmartInfo(i)"));
		SaveSmartInfo(i);
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

	// このウィンドウに WS_EX_LAYERED を設定する 
#ifdef SUISHO_SHIZUKU_SUPPORT
	if (m_LayeredListCtrl)
	{
		::SetWindowLong(m_List.m_hWnd, GWL_EXSTYLE, ::GetWindowLong(m_List.m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		if (m_IsHighContrast)
		{
			::SetLayeredWindowAttributes(m_List.m_hWnd, 0, 255, LWA_ALPHA);
		}
		else
		{
			::SetLayeredWindowAttributes(m_List.m_hWnd, 0, LIST_CTL_ALPHA, LWA_ALPHA);
		}
	}	
#endif

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
		if (m_FlagGreenMode)
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

CString CDiskInfoDlg::GetTemperatureClass(INT temperature)
{
	if(temperature >= 55)
	{
		return _T("temperatureBad");
	}
	else if(temperature >= 50)
	{
		return _T("temperatureCaution");
	}
	else if(temperature == -1000)
	{
		return _T("temperatureUnknown");
	}
	else
	{
		if (m_FlagGreenMode)
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
			else
			if((m_Ata.vars[index].Attribute[j].Id == 0xE8 && (m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_INTEL || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_PLEXTOR || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_SANDISK))
			|| (m_Ata.vars[index].Attribute[j].Id == 0xBB && m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_MTRON)
			||((m_Ata.vars[index].Attribute[j].Id == 0xB4 || m_Ata.vars[index].Attribute[j].Id == 0xB3) && m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_SAMSUNG)
			|| (m_Ata.vars[index].Attribute[j].Id == 0xD1 && m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_INDILINX)
			|| (m_Ata.vars[index].Attribute[j].Id == 0xE7 && m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_SANDFORCE)
			|| (m_Ata.vars[index].Attribute[j].Id == 0xAA && m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_JMICRON && ! m_Ata.vars[index].IsRawValues8
			|| (m_Ata.vars[index].Attribute[j].Id == 0xCA && m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_MICRON)
			|| (m_Ata.vars[index].Attribute[j].Id == 0xE9 && (m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_OCZ || m_Ata.vars[index].DiskVendorId == m_Ata.SSD_VENDOR_OCZ_VECTOR))
			)
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
	ULARGE_INTEGER freeBytesAvailableToCaller;
	ULARGE_INTEGER totalNumberOfBytes;
	ULARGE_INTEGER totalNumberOfFreeBytes;

	for(int j = 0; j < 26; j++)
	{
		if(map & (1 << j))
		{
			CString cstr;
			CString letter;
			TCHAR volumeNameBuffer[256];

			letter.Format(_T("%C:\\"), j + 'A');
			GetDiskFreeSpaceEx(letter, &freeBytesAvailableToCaller,
				&totalNumberOfBytes, &totalNumberOfFreeBytes);
			GetVolumeInformation(letter,  volumeNameBuffer, 256, NULL, NULL, NULL, NULL, 0);

			cstr.Format(_T("%C: %s [%.1f/%.1f GB (%.1f %%)]\r\n"), 
				j + 'A', volumeNameBuffer,
				totalNumberOfFreeBytes.QuadPart / 1024 / 1024 / 1024.0,
				totalNumberOfBytes.QuadPart  / 1024 / 1024 / 1024.0,
				(double)totalNumberOfFreeBytes.QuadPart / (double)totalNumberOfBytes.QuadPart * 100);
		
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
	if (m_Ata.vars.GetCount() / 8 == m_DriveMenuPage)
	{
		for(int i = m_Ata.vars.GetCount() % 8; i < 8; i++)
		{
			m_LiDisk[i] = _T("");
			m_ButtonDisk[i].ReloadImage(IP(L"noDisk"), 1);
			m_ButtonDisk[i].SetToolTipText(L"");
		//	m_ButtonDisk[i].EnableWindow(FALSE);
			m_ButtonDisk[i].SetHandCursor(FALSE);
		}
	}

	for (int i = 0; i < 8; i++)
	{
		m_ButtonDisk[i].SetSelected(FALSE);
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

		if(m_SelectDisk == i)
		{
			m_ButtonDisk[i % 8].SetSelected(TRUE);
		}

		// DriveMenu
		if(i / 8 == m_DriveMenuPage)
		{
			CString targetDisk;
			targetDisk.Format(_T("Disk%d"), i % 8);
			if (m_Ata.vars[i].InterfaceType == m_Ata.INTERFACE_TYPE_NVME && m_Ata.vars[i].Temperature > -300)
			{
				if (m_FlagFahrenheit)
				{
					m_LiDisk[i % 8].Format(_T("%s%s%d °F%s%s"),
						diskStatus, delimiter, m_Ata.vars[i].Temperature * 9 / 5 + 32, delimiter, driveLetter);
				}
				else
				{
					m_LiDisk[i % 8].Format(_T("%s%s%d °C%s%s"),
						diskStatus, delimiter, m_Ata.vars[i].Temperature, delimiter, driveLetter);
				}
			}
			else if(m_Ata.vars[i].IsSmartEnabled && m_Ata.vars[i].Temperature > -300)
			{
				if(m_FlagFahrenheit)
				{
					m_LiDisk[i % 8].Format(_T("%s%s%d °F%s%s"), 
								diskStatus, delimiter, m_Ata.vars[i].Temperature * 9 / 5 + 32, delimiter, driveLetter);
				}
				else
				{
					m_LiDisk[i % 8].Format(_T("%s%s%d °C%s%s"), 
								diskStatus, delimiter, m_Ata.vars[i].Temperature, delimiter, driveLetter);
				}
			}
			else if(m_Ata.vars[i].IsSmartEnabled)
			{
				if(m_FlagFahrenheit)
				{
					m_LiDisk[i % 8].Format(_T("%s%s-- °F%s%s"), diskStatus, delimiter, delimiter, driveLetter);
				}
				else
				{
					m_LiDisk[i % 8].Format(_T("%s%s-- °C%s%s"), diskStatus, delimiter, delimiter, driveLetter);
				}
			}
			else
			{
				if(m_FlagFahrenheit)
				{
					m_LiDisk[i % 8].Format(_T("----%s-- °F%s%s"), delimiter, delimiter, driveLetter);
				}
				else
				{
					m_LiDisk[i % 8].Format(_T("----%s-- °C%s%s"), delimiter, delimiter, driveLetter);
				}
			}

			if(m_Ata.vars[i].PhysicalDriveId >= 0)
			{
				cstr.Format(_T("Disk %d : %s %.1f GB\n%s"), m_Ata.vars[i].PhysicalDriveId, m_Ata.vars[i].Model, m_Ata.vars[i].TotalDiskSize / 1000.0, GetLogicalDriveInfo(i));
			}
			else
			{
				cstr.Format(_T("Disk -- : %s %.1f GB\n%s"), m_Ata.vars[i].Model, m_Ata.vars[i].TotalDiskSize / 1000.0, GetLogicalDriveInfo(i));
			}
			
			m_ButtonDisk[i % 8].SetToolTipText(cstr);
			className.Replace(L"Status", L"");
			m_ButtonDisk[i % 8].ReloadImage(IP(className), 4);
			m_ButtonDisk[i % 8].SetHandCursor(TRUE);
		//	m_ButtonDisk[i % 8].EnableWindow(TRUE);
		}
	}

	UpdateData(FALSE);
}
