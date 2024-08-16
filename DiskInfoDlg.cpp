/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DiskInfo.h"
#include "DiskInfoDlg.h"
#include "AtaSmart.h"

#include "UtilityFx.h"
#include "OsInfoFx.h"

#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

#include "locale.h"
#include <complex>

#include "digitalv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Global
	// Task Tray
	UINT gRegMessageId = ::RegisterWindowMessage(_T("CrystalDiskInfo"));
	UINT gRegIconId = ::RegisterWindowMessage(_T("CrystalDiskInfoIcon"));
	UINT gTempIcon[CAtaSmart::MAX_DISK];

	extern const GUID StrageGUID = { 0x53F56307, 0xB6BF, 0x11D0, 
					  0x94,0xF2,0x00,0xA0,0xC9,0x1E,0xFB,0x8B };

CDiskInfoDlg::CDiskInfoDlg(CWnd* pParent /*=NULL*/, BOOL flagStartupExit)
	: CMainDialogFx(CDiskInfoDlg::IDD, pParent)
{
	DebugPrint(L"CDiskInfoDlg::CDiskInfoDlg - START");

	m_Ini = ((CDiskInfoApp*)AfxGetApp())->m_Ini;
	m_OffsetX = 0;

#ifdef SUISHO_AOI_SUPPORT
	m_DefaultTheme = L"Aoi";
	m_RecommendTheme = L"Aoi";
	m_ThemeKeyName = L"ThemeAoi";
#elif MSI_MEI_SUPPORT
	m_DefaultTheme = L"MSIMei";
	m_RecommendTheme = L"MSIMei";
	m_ThemeKeyName = L"ThemeMSIMei";
#elif KUREI_KEI_SUPPORT
	m_DefaultTheme = L"KureiKei";
	m_RecommendTheme = L"KureiKeiRecoding";
	m_ThemeKeyName = L"ThemeKureiKei";
#elif SUISHO_SHIZUKU_SUPPORT
	m_DefaultTheme = L"Shizuku";
	m_RecommendTheme = L"ShizukuIdol";
	m_ThemeKeyName = L"ThemeShizuku";
#else
	m_DefaultTheme = L"Default";
	m_RecommendTheme = L"Default";
	m_ThemeKeyName = L"Theme";
#endif

	m_RandomThemeLabel = L"Random";
	m_BackgroundName = L"Background";

	m_hMenu = NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hIconMini = AfxGetApp()->LoadIcon(IDI_MINI_ICON);
	m_MainIconId = gRegIconId;

	m_SmartDir = ((CDiskInfoApp*)AfxGetApp())->m_SmartDir;
//	m_GadgetDir = ((CDiskInfoApp*)AfxGetApp())->m_GadgetDir;
	m_ExeDir = ((CDiskInfoApp*)AfxGetApp())->m_ExeDir;
	m_AlertMailPath = ((CDiskInfoApp*)AfxGetApp())->m_AlertMailPath;
	m_OpusDecPath = ((CDiskInfoApp*)AfxGetApp())->m_OpusDecPath;

#ifdef SUISHO_SHIZUKU_SUPPORT
	m_VoicePath = ((CDiskInfoApp*) AfxGetApp())->m_VoicePath;
#endif
#ifdef SUISHO_AOI_SUPPORT
	m_VoiceLanguage = ((CDiskInfoApp*)AfxGetApp())->m_VoiceLanguage;
#endif

	TCHAR tempPath[MAX_PATH];
	GetTempPath(MAX_PATH, tempPath);
	m_TempFilePathOpus = tempPath;
	m_TempFilePathOpus += _T("CrystalDiskInfo.opus");
	m_TempFilePathWave = tempPath;
	m_TempFilePathWave += _T("CrystalDiskInfo.wav");

	m_bStartupExit = flagStartupExit;

	m_AboutDlg = NULL;
	m_SettingDlg = NULL;
	m_HealthDlg = NULL;
	m_OptionDlg = NULL;
//	m_AlarmHistoryDlg = NULL;

	// Set Default Locale for CStdioFile
	_tsetlocale(LC_ALL, _T(""));

	for(int i = 0; i < 300; i++)
	{
		m_hTempIcon[0][i] = NULL;
		m_hTempIcon[1][i] = NULL;
	}

	CString cstr;
	for (int i = 0; i < CAtaSmart::MAX_DISK; i++)
	{
		cstr.Format(L"TempIcon%d", i);
		gTempIcon[i] = ::RegisterWindowMessage(cstr);
		m_TempIconIndex[i] = gTempIcon[i];
	}

	m_bTrayMainIcon = FALSE;
	for(int i = 0; i < CAtaSmart::MAX_DISK; i++)
	{
		m_PreTemp[i] = 0;
		m_bTrayTemperatureIcon[i] = FALSE;
	}

	m_SelectDisk = 0;
	m_DriveMenuPage = 0;
	m_AutoRefreshStatus = 0;
	m_WaitTimeStatus = 0;
	m_AutoDetectionStatus = 0;
	m_RawValues = 0;
	
	m_NowDetectingUnitPowerOnHours = FALSE;
	m_bInitializing = TRUE;

	DebugPrint(L"CDiskInfoDlg::CDiskInfoDlg - CENTER");

	m_ImageList.Create(16, 16, ILC_COLOR32|ILC_MASK, 3, 1);
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_GOOD));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_GOOD_GREEN));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_CAUTION));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_BAD));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_UNKNOWN));

	m_bAdvancedDiskSearch = (BOOL)GetPrivateProfileInt(_T("Setting"), _T("AdvancedDiskSearch"), 0, m_Ini);
	m_bWorkaroundHD204UI =  (BOOL)GetPrivateProfileInt(_T("Workaround"), _T("HD204UI"), 0, m_Ini);
	m_bWorkaroundIE8MODE =  (BOOL)GetPrivateProfileInt(_T("Workaround"), _T("IE8MODE"), 0, m_Ini);
	m_bWorkaroundAdataSsd =  (BOOL)GetPrivateProfileInt(_T("Workaround"), _T("AdataSsd"), 1, m_Ini);
	m_bWorkaroundIgnoreC4 = (BOOL) GetPrivateProfileInt(_T("Workaround"), _T("IgnoreC4"), 1, m_Ini);
	m_bEventLog = (BOOL)GetPrivateProfileInt(_T("Setting"), _T("EventLog"), 0, m_Ini);
	m_bAlertMail = (BOOL)GetPrivateProfileInt(_T("Setting"), _T("AlertMail"), 0, m_Ini);
	m_bAtaPassThroughSmart = (BOOL)GetPrivateProfileInt(_T("Setting"), _T("AtaPassThroughSmart"), 1, m_Ini);
	m_bAutoAamApm = (BOOL)GetPrivateProfileInt(_T("Setting"), _T("AutoAamApm"), 0, m_Ini);
	m_bDumpIdentifyDevice = (BOOL)GetPrivateProfileInt(_T("Setting"), _T("DumpIdentifyDevice"), 1, m_Ini);			// Default = Enabled
	m_bDumpSmartReadData = (BOOL)GetPrivateProfileInt(_T("Setting"), _T("DumpSmartReadData"), 1, m_Ini);
	m_bDumpSmartReadThreshold = (BOOL)GetPrivateProfileInt(_T("Setting"), _T("DumpSmartReadThreshold"), 1, m_Ini);
	m_bResidentMinimize = (BOOL)GetPrivateProfileInt(_T("Setting"), _T("ResidentMinimize"), 0, m_Ini);
	m_bSortDriveLetter = (BOOL)GetPrivateProfileInt(_T("Setting"), _T("SortDriveLetter"), 1, m_Ini);

	m_bShowTemperatureIconOnly = (BOOL)GetPrivateProfileInt(_T("Setting"), _T("ShowTemperatureIconOnly"), 0, m_Ini);
	m_bAsciiView = (BOOL)GetPrivateProfileInt(_T("Setting"), _T("AsciiView"), 0, m_Ini);
	m_bSmartEnglish = (BOOL)GetPrivateProfileInt(_T("Setting"), _T("SmartEnglish"), 0, m_Ini);
	m_bAlertSound = (BOOL)GetPrivateProfileInt(_T("Setting"), _T("AlertSound"), 1, m_Ini);
	m_bHideNoSmartDisk = (BOOL)GetPrivateProfileInt(_T("Setting"), _T("HideNoSmartDisk"), 1, m_Ini);
	m_bGreenMode = (BOOL) GetPrivateProfileInt(_T("Setting"), _T("GreenMode"), 0, m_Ini);
	m_bForceDisableDarkMode = (BOOL)GetPrivateProfileInt(_T("Setting"), _T("ForceDisableDarkMode"), 0, m_Ini);
	m_bNarrowDriveMenu = (BOOL)GetPrivateProfileInt(_T("Setting"), _T("NarrowDriveMenu"), 0, m_Ini);

#ifdef SUISHO_SHIZUKU_SUPPORT
	#ifdef MSI_MEI_SUPPORT
	m_bStartupVoice = (BOOL)GetPrivateProfileInt(_T("Setting"), _T("StartupVoice"), 1, m_Ini);
	#else
	m_bStartupVoice = (BOOL)GetPrivateProfileInt(_T("Setting"), _T("StartupVoice"), 0, m_Ini);
	#endif
#endif

	if((BOOL)GetPrivateProfileInt(_T("Workaround"), _T("ExecFailed"), 0, m_Ini))
	{
		m_bAtaPassThroughSmart = FALSE;
		WritePrivateProfileStringFx(_T("Setting"), _T("AtaPassThroughSmart"), _T("0"), m_Ini);
	}

	// Added 2013/04/12 - Workaround for Exec Failed
	WritePrivateProfileStringFx(_T("Workaround"), _T("ExecFailed"), _T("1"), m_Ini);
	
	TCHAR str[256];
	GetPrivateProfileStringFx(_T("Setting"), _T("AlertSoundPath"), _T(""), str, 256, m_Ini);
	m_AlertSoundPath = str;

	m_bGadget = (BOOL)GetPrivateProfileInt(_T("Setting"), _T("Gadget"), 0, m_Ini);

	m_AutoDetectionStatus = GetPrivateProfileInt(_T("Setting"), _T("AutoDetection"), 0, m_Ini);
	if(m_AutoDetectionStatus < 0)
	{
		m_AutoDetectionStatus = 0;
	}

	m_RawValues = GetPrivateProfileInt(_T("Setting"), _T("RawValues"), 0, m_Ini);
	if(m_RawValues < 0 || m_RawValues > 3)
	{
		m_RawValues = 0;
	}

	m_ZoomType = GetPrivateProfileInt(_T("Setting"), _T("ZoomType"), ZoomTypeAuto, m_Ini);

	// Setting
	SAVE_SMART_PERIOD = GetPrivateProfileInt(_T("Setting"), _T("SAVE_SMART_PERIOD"), 150, m_Ini);
	ALARM_TEMPERATURE_PERIOD = GetPrivateProfileInt(_T("Setting"), _T("ALARM_TEMPERATURE_PERIOD"), 60 * 60, m_Ini);

	if(m_bEventLog)
	{
		InstallEventSource();
	}
	else
	{
		UninstallEventSource();
	}

	if(m_bAtaPassThroughSmart)
	{
		m_Ata.SetAtaPassThroughSmart(TRUE);
	}
	else
	{
		m_Ata.SetAtaPassThroughSmart(FALSE);
	}
	m_Ata.CsmiType = GetPrivateProfileInt(_T("Setting"), _T("CsmiType"), m_Ata.CSMI_TYPE_ENABLE_AUTO, m_Ini);

//	m_BrushDlg.CreateHatchBrush(HS_BDIAGONAL, RGB(0xF0, 0xF0, 0xF0));
//	m_BrushDlg.CreatePatternBrush(&m_BitmapBk);

#ifdef SUISHO_AOI_SUPPORT
	m_BackgroundName = L"AoiBackground";
#elif MSI_MEI_SUPPORT
	m_BackgroundName = L"Background";
#elif KUREI_KEI_SUPPORT
	m_BackgroundName = L"KureiKeiBackground";
#elif SUISHO_SHIZUKU_SUPPORT
	m_BackgroundName = L"ShizukuBackground";
#else
	m_BackgroundName = L"Background";
#endif

#ifdef SUISHO_SHIZUKU_SUPPORT
	m_hVoice = LoadLibrary(m_VoicePath);
	if (m_hVoice != NULL)
	{
		DebugPrint(L"m_hVoice != NULL");
	}
	else
	{
		DebugPrint(L"m_hVoice == NULL");
	}
#endif
	DebugPrint(L"CDiskInfoDlg::CDiskInfoDlg - END");
}

CDiskInfoDlg::~CDiskInfoDlg()
{
	if(m_hMenu != NULL)
	{
		DestroyMenu(m_hMenu);
	}

	AlertSound(-1, AS_DEINIT);
	DeleteShareInfo();

	DeleteFile(m_TempFilePathOpus);
	DeleteFile(m_TempFilePathWave);

#ifdef SUISHO_SHIZUKU_SUPPORT
	if (m_hVoice)
	{
		FreeLibrary(m_hVoice);
	}
#endif
}

void CDiskInfoDlg::OnCancel()
{
	if(m_bResidentMinimize)
	{
		ShowWindow(SW_MINIMIZE);
	}
	else
	{
		ShowWindow(SW_HIDE);
	}
	if(! m_bResident)
	{
		SavePos();

		KillGraphDlg();
		if(m_hDevNotify)
		{
			UnregisterDeviceNotification(m_hDevNotify);
		}
		CMainDialogFx::OnCancel();
	}
}

void CDiskInfoDlg::OnSaveText()
{
	CString path;
	SYSTEMTIME st;
	GetLocalTime(&st);
	path.Format(L"%s_%04d%02d%02d%02d%02d%02d", PRODUCT_NAME, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	CString filter = L"TEXT (*.txt)|*.txt||";
	CFileDialog save(FALSE, L"txt", path, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER, filter);

	if (save.DoModal() == IDOK)
	{
		SaveText(save.GetPathName());
	}	
}

void CDiskInfoDlg::OnSaveImage()
{
	SaveImage();
}

void CDiskInfoDlg::OnExit()
{
	SavePos();

	ShowWindow(SW_HIDE);
	RemoveTrayMainIcon();
	for(int i = 0; i < m_Ata.vars.GetCount(); i++)
	{
		RemoveTemperatureIcon(i);
	}
	KillGraphDlg();
	CMainDialogFx::OnCancel();
}

void CDiskInfoDlg::OnOK()
{

}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);

void CDiskInfoDlg::KillGraphDlg()
{
	//static HWND hWnd{};
	EnumWindows(EnumWindowsProc, (LPARAM)&m_GraphProcessId);
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	CArray<DWORD, DWORD> *id = (CArray<DWORD, DWORD>*)lParam;

	TCHAR str[1024];
	GetWindowText(hWnd, str, 1024);
	if(str[0] == 0)
	{
		return TRUE;
	}

	CString cstr;
	cstr = str;

	if(cstr.Find(_T("CrystalDiskInfo - ")) == 0 && cstr.Find(_T(" - Powered by Flot")) > 0)
	{
		for(int i = 0; i < id->GetCount(); i++)
		{
			DWORD processId = 0;
			::GetWindowThreadProcessId(hWnd, &processId);
			if(processId == id->GetAt(i))
			{
				PostMessage(hWnd, WM_QUIT, NULL, NULL);
			}
		}
	}

	return TRUE;
}

void CDiskInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CMainDialogFx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Control(pDX, IDC_BUTTON_DISK0, m_ButtonDisk[0]);
	DDX_Control(pDX, IDC_BUTTON_DISK1, m_ButtonDisk[1]);
	DDX_Control(pDX, IDC_BUTTON_DISK2, m_ButtonDisk[2]);
	DDX_Control(pDX, IDC_BUTTON_DISK3, m_ButtonDisk[3]);
	DDX_Control(pDX, IDC_BUTTON_DISK4, m_ButtonDisk[4]);
	DDX_Control(pDX, IDC_BUTTON_DISK5, m_ButtonDisk[5]);
	DDX_Control(pDX, IDC_BUTTON_DISK6, m_ButtonDisk[6]);
	DDX_Control(pDX, IDC_BUTTON_DISK7, m_ButtonDisk[7]);
	DDX_Control(pDX, IDC_BUTTON_DISK8, m_ButtonDisk[8]);
	DDX_Control(pDX, IDC_BUTTON_DISK9, m_ButtonDisk[9]);
	DDX_Control(pDX, IDC_BUTTON_DISK10, m_ButtonDisk[10]);
	DDX_Control(pDX, IDC_BUTTON_DISK11, m_ButtonDisk[11]);
	DDX_Control(pDX, IDC_BUTTON_DISK12, m_ButtonDisk[12]);
	DDX_Control(pDX, IDC_BUTTON_DISK13, m_ButtonDisk[13]);
	DDX_Control(pDX, IDC_BUTTON_DISK14, m_ButtonDisk[14]);
	DDX_Control(pDX, IDC_BUTTON_DISK15, m_ButtonDisk[15]);

#ifdef MAX_DRIVE_20
	DDX_Control(pDX, IDC_BUTTON_DISK16, m_ButtonDisk[16]);
	DDX_Control(pDX, IDC_BUTTON_DISK17, m_ButtonDisk[17]);
	DDX_Control(pDX, IDC_BUTTON_DISK18, m_ButtonDisk[18]);
	DDX_Control(pDX, IDC_BUTTON_DISK19, m_ButtonDisk[19]);
#endif

	DDX_Control(pDX, IDC_BUTTON_PRE_DISK, m_CtrlButtonPreDisk);
	DDX_Control(pDX, IDC_BUTTON_NEXT_DISK, m_CtrlButtonNextDisk);

	DDX_Control(pDX, IDC_LABEL_FIRMWARE, m_CtrlLabelFirmware);
	DDX_Control(pDX, IDC_LABEL_SERIAL_NUMBER, m_CtrlLabelSerialNumber);
	DDX_Control(pDX, IDC_LABEL_INTERFACE, m_CtrlLabelInterface);
	DDX_Control(pDX, IDC_LABEL_TRANSFER_MODE, m_CtrlLabelTransferMode);
	DDX_Control(pDX, IDC_LABEL_DRIVE_MAP, m_CtrlLabelDriveMap);
	DDX_Control(pDX, IDC_LABEL_BUFFER_SIZE, m_CtrlLabelBufferSize);
	DDX_Control(pDX, IDC_LABEL_NV_CACHE_SIZE, m_CtrlLabelNvCacheSize);
	DDX_Control(pDX, IDC_LABEL_ROTATION_RATE, m_CtrlLabelRotationRate);
	DDX_Control(pDX, IDC_LABEL_POWER_ON_COUNT, m_CtrlLabelPowerOnCount);
	DDX_Control(pDX, IDC_LABEL_POWER_ON_HOURS, m_CtrlLabelPowerOnHours);
	DDX_Control(pDX, IDC_LABEL_ATA_ATAPI, m_CtrlLabelAtaAtapi);
	DDX_Control(pDX, IDC_LABEL_FEATURE, m_CtrlLabelFeature);
	DDX_Control(pDX, IDC_LABEL_HEALTH_STATUS, m_CtrlLabelDiskStatus);
	DDX_Control(pDX, IDC_LABEL_TEMPERATURE, m_CtrlLabelTemperature);

	DDX_Control(pDX, IDC_VALUE_MODEL, m_CtrlModel);
	DDX_Control(pDX, IDC_VALUE_FIRMWARE, m_CtrlFirmware);
	DDX_Control(pDX, IDC_VALUE_SERIAL_NUMBER, m_CtrlSerialNumber);
	DDX_Control(pDX, IDC_VALUE_INTERFACE, m_CtrlInterface);
	DDX_Control(pDX, IDC_VALUE_TRANSFER_MODE, m_CtrlTransferMode);
	DDX_Control(pDX, IDC_VALUE_DRIVE_MAP, m_CtrlDriveMap);
	DDX_Control(pDX, IDC_VALUE_BUFFER_SIZE, m_CtrlBufferSize);
	DDX_Control(pDX, IDC_VALUE_NV_CACHE_SIZE, m_CtrlNvCacheSize);
	DDX_Control(pDX, IDC_VALUE_ROTATION_RATE, m_CtrlRotationRate);
	DDX_Control(pDX, IDC_VALUE_POWER_ON_COUNT, m_CtrlPowerOnCount);
	DDX_Control(pDX, IDC_VALUE_POWER_ON_HOURS, m_CtrlPowerOnHours);
	DDX_Control(pDX, IDC_VALUE_ATA_ATAPI, m_CtrlAtaAtapi);
	DDX_Control(pDX, IDC_VALUE_FEATURE, m_CtrlFeature);
	DDX_Control(pDX, IDC_BUTTON_HEALTH_STATUS, m_CtrlDiskStatus);
	DDX_Control(pDX, IDC_BUTTON_TEMPERATURE, m_CtrlTemperature);
	DDX_Control(pDX, IDC_BUTTON_LIFE, m_CtrlLife);

	DDX_Text(pDX, IDC_BUTTON_DISK0, m_LiDisk[0]);
	DDX_Text(pDX, IDC_BUTTON_DISK1, m_LiDisk[1]);
	DDX_Text(pDX, IDC_BUTTON_DISK2, m_LiDisk[2]);
	DDX_Text(pDX, IDC_BUTTON_DISK3, m_LiDisk[3]);
	DDX_Text(pDX, IDC_BUTTON_DISK4, m_LiDisk[4]);
	DDX_Text(pDX, IDC_BUTTON_DISK5, m_LiDisk[5]);
	DDX_Text(pDX, IDC_BUTTON_DISK6, m_LiDisk[6]);
	DDX_Text(pDX, IDC_BUTTON_DISK7, m_LiDisk[7]);
	DDX_Text(pDX, IDC_BUTTON_DISK8, m_LiDisk[8]);
	DDX_Text(pDX, IDC_BUTTON_DISK9, m_LiDisk[9]);
	DDX_Text(pDX, IDC_BUTTON_DISK10, m_LiDisk[10]);
	DDX_Text(pDX, IDC_BUTTON_DISK11, m_LiDisk[11]);
	DDX_Text(pDX, IDC_BUTTON_DISK12, m_LiDisk[12]);
	DDX_Text(pDX, IDC_BUTTON_DISK13, m_LiDisk[13]);
	DDX_Text(pDX, IDC_BUTTON_DISK14, m_LiDisk[14]);
	DDX_Text(pDX, IDC_BUTTON_DISK15, m_LiDisk[15]);

#ifdef MAX_DRIVE_20
	DDX_Text(pDX, IDC_BUTTON_DISK16, m_LiDisk[16]);
	DDX_Text(pDX, IDC_BUTTON_DISK17, m_LiDisk[17]);
	DDX_Text(pDX, IDC_BUTTON_DISK18, m_LiDisk[18]);
	DDX_Text(pDX, IDC_BUTTON_DISK19, m_LiDisk[19]);
#endif

	DDX_Text(pDX, IDC_LABEL_FIRMWARE, m_LabelFirmware);
	DDX_Text(pDX, IDC_LABEL_SERIAL_NUMBER, m_LabelSerialNumber);
	DDX_Text(pDX, IDC_LABEL_INTERFACE, m_LabelInterface);
	DDX_Text(pDX, IDC_LABEL_TRANSFER_MODE, m_LabelTransferMode);
	DDX_Text(pDX, IDC_LABEL_DRIVE_MAP, m_LabelDriveMap);
	DDX_Text(pDX, IDC_LABEL_BUFFER_SIZE, m_LabelBufferSize);
	DDX_Text(pDX, IDC_LABEL_NV_CACHE_SIZE, m_LabelNvCacheSize);
	DDX_Text(pDX, IDC_LABEL_ROTATION_RATE, m_LabelRotationRate);
	DDX_Text(pDX, IDC_LABEL_POWER_ON_COUNT, m_LabelPowerOnCount);
	DDX_Text(pDX, IDC_LABEL_POWER_ON_HOURS, m_LabelPowerOnHours);
	DDX_Text(pDX, IDC_LABEL_ATA_ATAPI, m_LabelAtaAtapi);
	DDX_Text(pDX, IDC_LABEL_FEATURE, m_LabelFeature);
	DDX_Text(pDX, IDC_LABEL_HEALTH_STATUS, m_LabelDiskStatus);
	DDX_Text(pDX, IDC_LABEL_TEMPERATURE, m_LabelTemperature);

	DDX_Text(pDX, IDC_VALUE_MODEL, m_ModelCapacity);
	DDX_Text(pDX, IDC_VALUE_FIRMWARE, m_Firmware);
	DDX_Text(pDX, IDC_VALUE_SERIAL_NUMBER, m_SerialNumber);
	DDX_Text(pDX, IDC_VALUE_INTERFACE, m_Interface);
	DDX_Text(pDX, IDC_VALUE_TRANSFER_MODE, m_TransferMode);
	DDX_Text(pDX, IDC_VALUE_DRIVE_MAP, m_DriveMap);
	DDX_Text(pDX, IDC_VALUE_BUFFER_SIZE, m_BufferSize);
	DDX_Text(pDX, IDC_VALUE_NV_CACHE_SIZE, m_NvCacheSize);
	DDX_Text(pDX, IDC_VALUE_ROTATION_RATE, m_RotationRate);
	DDX_Text(pDX, IDC_VALUE_POWER_ON_COUNT, m_PowerOnCount);
	DDX_Text(pDX, IDC_VALUE_POWER_ON_HOURS, m_PowerOnHours);
	DDX_Text(pDX, IDC_VALUE_ATA_ATAPI, m_AtaAtapi);
	DDX_Text(pDX, IDC_VALUE_FEATURE, m_Feature);
	DDX_Text(pDX, IDC_BUTTON_HEALTH_STATUS, m_DiskStatus);
	DDX_Text(pDX, IDC_BUTTON_TEMPERATURE, m_Temperature);
	DDX_Text(pDX, IDC_BUTTON_LIFE, m_Life);

	DDX_Control(pDX, IDC_BUTTON_VOICE, m_CtrlVoice);
	DDX_Control(pDX, IDC_BUTTON_VOICE_HIDE, m_CtrlVoiceHide);
	DDX_Control(pDX, IDC_BUTTON_COPYRIGHT, m_CtrlCopyright);
}

BEGIN_MESSAGE_MAP(CDiskInfoDlg, CMainDialogFx)
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_COMMAND(ID_SAVE_TEXT, &CDiskInfoDlg::OnSaveText)
	ON_COMMAND(ID_SAVE_IMAGE, &CDiskInfoDlg::OnSaveImage)
	ON_COMMAND(ID_EXIT, &CDiskInfoDlg::OnExit)
	ON_COMMAND(ID_ABOUT, &CDiskInfoDlg::OnAbout)
	ON_COMMAND(ID_HIDE_SMART_INFO, &CDiskInfoDlg::OnHideSmartInfo)
	ON_COMMAND(ID_HIDE_SERIAL_NUMBER, &CDiskInfoDlg::OnHideSerialNumber)
	ON_COMMAND(ID_COPY, &CDiskInfoDlg::OnCopy)
	ON_COMMAND(ID_CRYSTALDEWWORLD, &CDiskInfoDlg::OnCrystalDewWorld)
	ON_COMMAND(ID_REFRESH, &CDiskInfoDlg::OnRefresh)
	ON_COMMAND(ID_HELP_ABOUT_SMART, &CDiskInfoDlg::OnHelpAboutSmart)
	ON_COMMAND(ID_AUTO_REFRESH_DISABLE, &CDiskInfoDlg::OnAutoRefreshDisable)
	ON_COMMAND(ID_AUTO_REFRESH_01_MIN, &CDiskInfoDlg::OnAutoRefresh01Min)
	ON_COMMAND(ID_AUTO_REFRESH_03_MIN, &CDiskInfoDlg::OnAutoRefresh03Min)
	ON_COMMAND(ID_AUTO_REFRESH_05_MIN, &CDiskInfoDlg::OnAutoRefresh05Min)
	ON_COMMAND(ID_AUTO_REFRESH_10_MIN, &CDiskInfoDlg::OnAutoRefresh10Min)
	ON_COMMAND(ID_AUTO_REFRESH_30_MIN, &CDiskInfoDlg::OnAutoRefresh30Min)
	ON_COMMAND(ID_AUTO_REFRESH_60_MIN, &CDiskInfoDlg::OnAutoRefresh60Min)
	ON_COMMAND(ID_AUTO_REFRESH_120_MIN, &CDiskInfoDlg::OnAutoRefresh120Min)
	ON_COMMAND(ID_AUTO_REFRESH_180_MIN, &CDiskInfoDlg::OnAutoRefresh180Min)
	ON_COMMAND(ID_AUTO_REFRESH_360_MIN, &CDiskInfoDlg::OnAutoRefresh360Min)
	ON_COMMAND(ID_AUTO_REFRESH_720_MIN, &CDiskInfoDlg::OnAutoRefresh720Min)
	ON_COMMAND(ID_AUTO_REFRESH_1440_MIN, &CDiskInfoDlg::OnAutoRefresh1440Min)
	ON_COMMAND(ID_OPEN_DISK_MANAGEMENT, &CDiskInfoDlg::OnOpenDiskManagement)
	ON_COMMAND(ID_OPEN_DEVICE_MANAGER, &CDiskInfoDlg::OnOpenDeviceManager)
	ON_COMMAND(ID_ADVANCED_DISK_SEARCH, &CDiskInfoDlg::OnAdvancedDiskSearch)
	ON_COMMAND(ID_WORKAROUND_HD204UI, &CDiskInfoDlg::OnWorkaroundHD204UI)
	ON_COMMAND(ID_WORKAROUND_IE8MODE, &CDiskInfoDlg::OnWorkaroundIE8MODE)
	ON_COMMAND(ID_GREEN_MODE, &CDiskInfoDlg::OnGreenMode)
	ON_COMMAND(ID_DISABLE_DARK_MODE, &CDiskInfoDlg::OnDisableDarkMode)

	ON_COMMAND(ID_WORKAROUND_ADATA_SSD, &CDiskInfoDlg::OnWorkaroundAdataSsd)
	ON_COMMAND(ID_WORKAROUND_IGNORE_C4, &CDiskInfoDlg::OnWorkaroundIgnoreC4)
	ON_COMMAND(ID_RESIDENT, &CDiskInfoDlg::OnResident)

	ON_MESSAGE(WM_POWERBROADCAST, &CDiskInfoDlg::OnPowerBroadcast)
	ON_MESSAGE(WM_DEVICECHANGE, &CDiskInfoDlg::OnDeviceChange)

	ON_MESSAGE(WM_QUERYENDSESSION, &CDiskInfoDlg::OnQueryEndSession)


	// Task Tray
	ON_REGISTERED_MESSAGE(gRegMessageId, OnRegMessage)
	ON_REGISTERED_MESSAGE(wmTaskbarCreated, OnTaskbarCreated)

	ON_REGISTERED_MESSAGE(gTempIcon[0], OnTempIcon0)
	ON_REGISTERED_MESSAGE(gTempIcon[1], OnTempIcon1)
	ON_REGISTERED_MESSAGE(gTempIcon[2], OnTempIcon2)
	ON_REGISTERED_MESSAGE(gTempIcon[3], OnTempIcon3)
	ON_REGISTERED_MESSAGE(gTempIcon[4], OnTempIcon4)
	ON_REGISTERED_MESSAGE(gTempIcon[5], OnTempIcon5)
	ON_REGISTERED_MESSAGE(gTempIcon[6], OnTempIcon6)
	ON_REGISTERED_MESSAGE(gTempIcon[7], OnTempIcon7)
	ON_REGISTERED_MESSAGE(gTempIcon[8], OnTempIcon8)
	ON_REGISTERED_MESSAGE(gTempIcon[9], OnTempIcon9)
	ON_REGISTERED_MESSAGE(gTempIcon[10], OnTempIcon10)
	ON_REGISTERED_MESSAGE(gTempIcon[11], OnTempIcon11)
	ON_REGISTERED_MESSAGE(gTempIcon[12], OnTempIcon12)
	ON_REGISTERED_MESSAGE(gTempIcon[13], OnTempIcon13)
	ON_REGISTERED_MESSAGE(gTempIcon[14], OnTempIcon14)
	ON_REGISTERED_MESSAGE(gTempIcon[15], OnTempIcon15)
	ON_REGISTERED_MESSAGE(gTempIcon[16], OnTempIcon16)
	ON_REGISTERED_MESSAGE(gTempIcon[17], OnTempIcon17)
	ON_REGISTERED_MESSAGE(gTempIcon[18], OnTempIcon18)
	ON_REGISTERED_MESSAGE(gTempIcon[19], OnTempIcon19)
	ON_REGISTERED_MESSAGE(gTempIcon[20], OnTempIcon20)
	ON_REGISTERED_MESSAGE(gTempIcon[21], OnTempIcon21)
	ON_REGISTERED_MESSAGE(gTempIcon[22], OnTempIcon22)
	ON_REGISTERED_MESSAGE(gTempIcon[23], OnTempIcon23)
	ON_REGISTERED_MESSAGE(gTempIcon[24], OnTempIcon24)
	ON_REGISTERED_MESSAGE(gTempIcon[25], OnTempIcon25)
	ON_REGISTERED_MESSAGE(gTempIcon[26], OnTempIcon26)
	ON_REGISTERED_MESSAGE(gTempIcon[27], OnTempIcon27)
	ON_REGISTERED_MESSAGE(gTempIcon[28], OnTempIcon28)
	ON_REGISTERED_MESSAGE(gTempIcon[29], OnTempIcon29)
	ON_REGISTERED_MESSAGE(gTempIcon[30], OnTempIcon30)
	ON_REGISTERED_MESSAGE(gTempIcon[31], OnTempIcon31)
	ON_REGISTERED_MESSAGE(gTempIcon[32], OnTempIcon32)
	ON_REGISTERED_MESSAGE(gTempIcon[33], OnTempIcon33)
	ON_REGISTERED_MESSAGE(gTempIcon[34], OnTempIcon34)
	ON_REGISTERED_MESSAGE(gTempIcon[35], OnTempIcon35)
	ON_REGISTERED_MESSAGE(gTempIcon[36], OnTempIcon36)
	ON_REGISTERED_MESSAGE(gTempIcon[37], OnTempIcon37)
	ON_REGISTERED_MESSAGE(gTempIcon[38], OnTempIcon38)
	ON_REGISTERED_MESSAGE(gTempIcon[39], OnTempIcon39)
	ON_REGISTERED_MESSAGE(gTempIcon[40], OnTempIcon40)
	ON_REGISTERED_MESSAGE(gTempIcon[41], OnTempIcon41)
	ON_REGISTERED_MESSAGE(gTempIcon[42], OnTempIcon42)
	ON_REGISTERED_MESSAGE(gTempIcon[43], OnTempIcon43)
	ON_REGISTERED_MESSAGE(gTempIcon[44], OnTempIcon44)
	ON_REGISTERED_MESSAGE(gTempIcon[45], OnTempIcon45)
	ON_REGISTERED_MESSAGE(gTempIcon[46], OnTempIcon46)
	ON_REGISTERED_MESSAGE(gTempIcon[47], OnTempIcon47)
	ON_REGISTERED_MESSAGE(gTempIcon[48], OnTempIcon48)
	ON_REGISTERED_MESSAGE(gTempIcon[49], OnTempIcon49)
	ON_REGISTERED_MESSAGE(gTempIcon[50], OnTempIcon50)
	ON_REGISTERED_MESSAGE(gTempIcon[51], OnTempIcon51)
	ON_REGISTERED_MESSAGE(gTempIcon[52], OnTempIcon52)
	ON_REGISTERED_MESSAGE(gTempIcon[53], OnTempIcon53)
	ON_REGISTERED_MESSAGE(gTempIcon[54], OnTempIcon54)
	ON_REGISTERED_MESSAGE(gTempIcon[55], OnTempIcon55)
	ON_REGISTERED_MESSAGE(gTempIcon[56], OnTempIcon56)
	ON_REGISTERED_MESSAGE(gTempIcon[57], OnTempIcon57)
	ON_REGISTERED_MESSAGE(gTempIcon[58], OnTempIcon58)
	ON_REGISTERED_MESSAGE(gTempIcon[59], OnTempIcon59)
	ON_REGISTERED_MESSAGE(gTempIcon[60], OnTempIcon60)
	ON_REGISTERED_MESSAGE(gTempIcon[61], OnTempIcon61)
	ON_REGISTERED_MESSAGE(gTempIcon[62], OnTempIcon62)
	ON_REGISTERED_MESSAGE(gTempIcon[63], OnTempIcon63)
	ON_REGISTERED_MESSAGE(gTempIcon[64], OnTempIcon64)
	ON_REGISTERED_MESSAGE(gTempIcon[65], OnTempIcon65)
	ON_REGISTERED_MESSAGE(gTempIcon[66], OnTempIcon66)
	ON_REGISTERED_MESSAGE(gTempIcon[67], OnTempIcon67)
	ON_REGISTERED_MESSAGE(gTempIcon[68], OnTempIcon68)
	ON_REGISTERED_MESSAGE(gTempIcon[69], OnTempIcon69)
	ON_REGISTERED_MESSAGE(gTempIcon[70], OnTempIcon70)
	ON_REGISTERED_MESSAGE(gTempIcon[71], OnTempIcon71)
	ON_REGISTERED_MESSAGE(gTempIcon[72], OnTempIcon72)
	ON_REGISTERED_MESSAGE(gTempIcon[73], OnTempIcon73)
	ON_REGISTERED_MESSAGE(gTempIcon[74], OnTempIcon74)
	ON_REGISTERED_MESSAGE(gTempIcon[75], OnTempIcon75)
	ON_REGISTERED_MESSAGE(gTempIcon[76], OnTempIcon76)
	ON_REGISTERED_MESSAGE(gTempIcon[77], OnTempIcon77)
	ON_REGISTERED_MESSAGE(gTempIcon[78], OnTempIcon78)
	ON_REGISTERED_MESSAGE(gTempIcon[79], OnTempIcon79)
/*
	ON_REGISTERED_MESSAGE(gTempIcon[80], OnTempIcon80)
	ON_REGISTERED_MESSAGE(gTempIcon[81], OnTempIcon81)
	ON_REGISTERED_MESSAGE(gTempIcon[82], OnTempIcon82)
	ON_REGISTERED_MESSAGE(gTempIcon[83], OnTempIcon83)
	ON_REGISTERED_MESSAGE(gTempIcon[84], OnTempIcon84)
	ON_REGISTERED_MESSAGE(gTempIcon[85], OnTempIcon85)
	ON_REGISTERED_MESSAGE(gTempIcon[86], OnTempIcon86)
	ON_REGISTERED_MESSAGE(gTempIcon[87], OnTempIcon87)
	ON_REGISTERED_MESSAGE(gTempIcon[88], OnTempIcon88)
	ON_REGISTERED_MESSAGE(gTempIcon[89], OnTempIcon89)
	ON_REGISTERED_MESSAGE(gTempIcon[90], OnTempIcon90)
	ON_REGISTERED_MESSAGE(gTempIcon[91], OnTempIcon91)
	ON_REGISTERED_MESSAGE(gTempIcon[92], OnTempIcon92)
	ON_REGISTERED_MESSAGE(gTempIcon[93], OnTempIcon93)
	ON_REGISTERED_MESSAGE(gTempIcon[94], OnTempIcon94)
	ON_REGISTERED_MESSAGE(gTempIcon[95], OnTempIcon95)
	ON_REGISTERED_MESSAGE(gTempIcon[96], OnTempIcon96)
	ON_REGISTERED_MESSAGE(gTempIcon[97], OnTempIcon97)
	ON_REGISTERED_MESSAGE(gTempIcon[98], OnTempIcon98)
	ON_REGISTERED_MESSAGE(gTempIcon[99], OnTempIcon99)
	ON_REGISTERED_MESSAGE(gTempIcon[100], OnTempIcon100)
	ON_REGISTERED_MESSAGE(gTempIcon[101], OnTempIcon101)
	ON_REGISTERED_MESSAGE(gTempIcon[102], OnTempIcon102)
	ON_REGISTERED_MESSAGE(gTempIcon[103], OnTempIcon103)
	ON_REGISTERED_MESSAGE(gTempIcon[104], OnTempIcon104)
	ON_REGISTERED_MESSAGE(gTempIcon[105], OnTempIcon105)
	ON_REGISTERED_MESSAGE(gTempIcon[106], OnTempIcon106)
	ON_REGISTERED_MESSAGE(gTempIcon[107], OnTempIcon107)
	ON_REGISTERED_MESSAGE(gTempIcon[108], OnTempIcon108)
	ON_REGISTERED_MESSAGE(gTempIcon[109], OnTempIcon109)
	ON_REGISTERED_MESSAGE(gTempIcon[110], OnTempIcon110)
	ON_REGISTERED_MESSAGE(gTempIcon[111], OnTempIcon111)
	ON_REGISTERED_MESSAGE(gTempIcon[112], OnTempIcon112)
	ON_REGISTERED_MESSAGE(gTempIcon[113], OnTempIcon113)
	ON_REGISTERED_MESSAGE(gTempIcon[114], OnTempIcon114)
	ON_REGISTERED_MESSAGE(gTempIcon[115], OnTempIcon115)
	ON_REGISTERED_MESSAGE(gTempIcon[116], OnTempIcon116)
	ON_REGISTERED_MESSAGE(gTempIcon[117], OnTempIcon117)
	ON_REGISTERED_MESSAGE(gTempIcon[118], OnTempIcon118)
	ON_REGISTERED_MESSAGE(gTempIcon[119], OnTempIcon119)
	ON_REGISTERED_MESSAGE(gTempIcon[120], OnTempIcon120)
	ON_REGISTERED_MESSAGE(gTempIcon[121], OnTempIcon121)
	ON_REGISTERED_MESSAGE(gTempIcon[122], OnTempIcon122)
	ON_REGISTERED_MESSAGE(gTempIcon[123], OnTempIcon123)
	ON_REGISTERED_MESSAGE(gTempIcon[124], OnTempIcon124)
	ON_REGISTERED_MESSAGE(gTempIcon[125], OnTempIcon125)
	ON_REGISTERED_MESSAGE(gTempIcon[126], OnTempIcon126)
	ON_REGISTERED_MESSAGE(gTempIcon[127], OnTempIcon127)
*/

	ON_COMMAND(ID_GRAPH, &CDiskInfoDlg::OnGraph)
	ON_COMMAND(ID_HELP, &CDiskInfoDlg::OnHelp)
	ON_COMMAND(ID_CUSTOMIZE, &CDiskInfoDlg::OnCustomize)
	ON_COMMAND(ID_STARTUP, &CDiskInfoDlg::OnStartup)
	ON_COMMAND(ID_WAIT_0_SEC, &CDiskInfoDlg::OnWait0Sec)
	ON_COMMAND(ID_WAIT_5_SEC, &CDiskInfoDlg::OnWait5Sec)
	ON_COMMAND(ID_WAIT_10_SEC, &CDiskInfoDlg::OnWait10Sec)
	ON_COMMAND(ID_WAIT_15_SEC, &CDiskInfoDlg::OnWait15Sec)
	ON_COMMAND(ID_WAIT_20_SEC, &CDiskInfoDlg::OnWait20Sec)
	ON_COMMAND(ID_WAIT_30_SEC, &CDiskInfoDlg::OnWait30Sec)
	ON_COMMAND(ID_WAIT_40_SEC, &CDiskInfoDlg::OnWait40Sec)
	ON_COMMAND(ID_WAIT_50_SEC, &CDiskInfoDlg::OnWait50Sec)
	ON_COMMAND(ID_WAIT_60_SEC, &CDiskInfoDlg::OnWait60Sec)
	ON_COMMAND(ID_WAIT_90_SEC, &CDiskInfoDlg::OnWait90Sec)
	ON_COMMAND(ID_WAIT_120_SEC, &CDiskInfoDlg::OnWait120Sec)
	ON_COMMAND(ID_WAIT_150_SEC, &CDiskInfoDlg::OnWait150Sec)
	ON_COMMAND(ID_WAIT_180_SEC, &CDiskInfoDlg::OnWait180Sec)
	ON_COMMAND(ID_WAIT_210_SEC, &CDiskInfoDlg::OnWait210Sec)
	ON_COMMAND(ID_WAIT_240_SEC, &CDiskInfoDlg::OnWait240Sec)

	ON_COMMAND(ID_AUTO_DETECTION_05_SEC, &CDiskInfoDlg::OnAutoDetection05Sec)
	ON_COMMAND(ID_AUTO_DETECTION_10_SEC, &CDiskInfoDlg::OnAutoDetection10Sec)
	ON_COMMAND(ID_AUTO_DETECTION_20_SEC, &CDiskInfoDlg::OnAutoDetection20Sec)
	ON_COMMAND(ID_AUTO_DETECTION_30_SEC, &CDiskInfoDlg::OnAutoDetection30Sec)
	ON_COMMAND(ID_AUTO_DETECTION_DISABLE, &CDiskInfoDlg::OnAutoDetectionDisable)

	ON_COMMAND(ID_EVENT_LOG, &CDiskInfoDlg::OnEventLog)
	ON_COMMAND(ID_ATA_PASS_THROUGH_SMART, &CDiskInfoDlg::OnAtaPassThroughSmart)
	ON_COMMAND(ID_GADGET_SUPPORT, &CDiskInfoDlg::OnGadgetSupport)
	ON_COMMAND(ID_CELSIUS, &CDiskInfoDlg::OnCelsius)
	ON_COMMAND(ID_FAHRENHEIT, &CDiskInfoDlg::OnFahrenheit)
	ON_COMMAND(ID_AAM_APM, &CDiskInfoDlg::OnAamApm)
	ON_COMMAND(ID_TEMPERATURE, &CDiskInfoDlg::OnTemperature)
	ON_COMMAND(ID_AUTO_AAM_APM, &CDiskInfoDlg::OnAutoAamApm)
	ON_COMMAND(ID_RESCAN, &CDiskInfoDlg::OnRescan)
	ON_COMMAND(ID_USB_SAT, &CDiskInfoDlg::OnUsbSat)
	ON_COMMAND(ID_USB_IODATA, &CDiskInfoDlg::OnUsbIodata)
	ON_COMMAND(ID_USB_SUNPLUS, &CDiskInfoDlg::OnUsbSunplus)
	ON_COMMAND(ID_USB_LOGITEC, &CDiskInfoDlg::OnUsbLogitec)
	ON_COMMAND(ID_USB_PROLIFIC, &CDiskInfoDlg::OnUsbProlific)
	ON_COMMAND(ID_USB_JMICRON, &CDiskInfoDlg::OnUsbJmicron)
	ON_COMMAND(ID_USB_CYPRESS, &CDiskInfoDlg::OnUsbCypress)
	ON_COMMAND(ID_USB_ASM1352R, &CDiskInfoDlg::OnUsbASM1352R)
	ON_COMMAND(ID_USB_REALTEK9220DP, &CDiskInfoDlg::OnUsbRealtek9220DP)
	ON_COMMAND(ID_USB_MEMORY, &CDiskInfoDlg::OnUsbMemory)
	//ON_COMMAND(ID_USB_SAT16, &CDiskInfoDlg::OnUsbSat16)
	ON_COMMAND(ID_USB_NVME_JMICRON3, &CDiskInfoDlg::OnUsbNVMeJMicron3)
	ON_COMMAND(ID_USB_NVME_JMICRON, &CDiskInfoDlg::OnUsbNVMeJMicron)
	ON_COMMAND(ID_USB_NVME_ASMEDIA, &CDiskInfoDlg::OnUsbNVMeASMedia)
	ON_COMMAND(ID_USB_NVME_REALTEK, &CDiskInfoDlg::OnUsbNVMeRealtek)
	ON_COMMAND(ID_AMD_RC2T7, &CDiskInfoDlg::OnAMD_RC2)
	ON_COMMAND(ID_MEGA_RAID, &CDiskInfoDlg::OnMegaRAID)
	ON_COMMAND(ID_INTEL_VROC, &CDiskInfoDlg::OnIntelVROC)
#ifdef JMICRON_USB_RAID_SUPPORT
	ON_COMMAND(ID_USB_JMS56X, &CDiskInfoDlg::OnUsbJMS56X)
	ON_COMMAND(ID_USB_JMB39X, &CDiskInfoDlg::OnUsbJMB39X)
	ON_COMMAND(ID_USB_JMS586_20, &CDiskInfoDlg::OnUsbJMS586_20)
	ON_COMMAND(ID_USB_JMS586_40, &CDiskInfoDlg::OnUsbJMS586_40)

#endif
	ON_COMMAND(ID_USB_ENABLE_ALL, &CDiskInfoDlg::OnUsbEnableAll)
	ON_COMMAND(ID_USB_DISABLE_ALL, &CDiskInfoDlg::OnUsbDisableAll)
	ON_COMMAND(ID_HEALTH_STATUS, &CDiskInfoDlg::OnHealthStatus)
	ON_COMMAND(ID_SOUND_SETTINGS, &CDiskInfoDlg::OnSoundSetting)
	ON_COMMAND(ID_DUMP_IDENTIFY_DEVICE, &CDiskInfoDlg::OnDumpIdentifyDevice)
	ON_COMMAND(ID_DUMP_SMART_READ_DATA, &CDiskInfoDlg::OnDumpSmartReadData)
	ON_COMMAND(ID_DUMP_SMART_READ_THRESHOLD, &CDiskInfoDlg::OnDumpSmartReadThreshold)
	ON_COMMAND(ID_RESIDENT_HIDE, &CDiskInfoDlg::OnResidentHide)
	ON_COMMAND(ID_RESIDENT_MINIMIZE, &CDiskInfoDlg::OnResidentMinimize)
	ON_COMMAND(ID_SORT_PHYSICAL_DRIVE_ID, &CDiskInfoDlg::OnSortPhysicalDriveId)
	ON_COMMAND(ID_SORT_DRIVE_LETTER, &CDiskInfoDlg::OnSortDriveLetter)
	ON_COMMAND(ID_DRIVE_MENU_8, &CDiskInfoDlg::OnDriveMenu8)
	ON_COMMAND(ID_DRIVE_MENU_16, &CDiskInfoDlg::OnDriveMenu16)
#ifdef MAX_DRIVE_20
	ON_COMMAND(ID_DRIVE_MENU_10, &CDiskInfoDlg::OnDriveMenu10)
	ON_COMMAND(ID_DRIVE_MENU_20, &CDiskInfoDlg::OnDriveMenu20)
#endif
	ON_COMMAND(ID_ZOOM_100, &CDiskInfoDlg::OnZoom100)
	ON_COMMAND(ID_ZOOM_125, &CDiskInfoDlg::OnZoom125)
	ON_COMMAND(ID_ZOOM_150, &CDiskInfoDlg::OnZoom150)
	ON_COMMAND(ID_ZOOM_200, &CDiskInfoDlg::OnZoom200)
	ON_COMMAND(ID_ZOOM_250, &CDiskInfoDlg::OnZoom250)
	ON_COMMAND(ID_ZOOM_300, &CDiskInfoDlg::OnZoom300)
	ON_COMMAND(ID_ZOOM_AUTO, &CDiskInfoDlg::OnZoomAuto)
	ON_COMMAND(ID_RAW_VALUES_16, &CDiskInfoDlg::OnRawValues16)
	ON_COMMAND(ID_RAW_VALUES_10_ALL, &CDiskInfoDlg::OnRawValues10All)
	ON_COMMAND(ID_RAW_VALUES_2BYTE, &CDiskInfoDlg::OnRawValues2byte)
	ON_COMMAND(ID_RAW_VALUES_1BYTE, &CDiskInfoDlg::OnRawValues1byte)
	ON_COMMAND(ID_ASCII_VIEW, &CDiskInfoDlg::OnAsciiView)
	ON_COMMAND(ID_ALERT_MAIL, &CDiskInfoDlg::OnAlertMail)
	ON_COMMAND(ID_MAIL_SETTINGS, &CDiskInfoDlg::OnMailSettings)
	ON_COMMAND(ID_SMART_ENGLISH, &CDiskInfoDlg::OnSmartEnglish)
#ifdef SUISHO_SHIZUKU_SUPPORT
		ON_COMMAND(ID_STARTUP_VOICE, &CDiskInfoDlg::OnStartupVoice)
#endif
#ifdef SUISHO_AOI_SUPPORT
		ON_COMMAND(ID_VOICE_ENGLISH, &CDiskInfoDlg::OnVoiceEnglish)
		ON_COMMAND(ID_VOICE_JAPANESE, &CDiskInfoDlg::OnVoiceJapanese)
#endif
	ON_COMMAND(ID_FONT_SETTING, &CDiskInfoDlg::OnFontSetting)
	ON_COMMAND(ID_CSMI_DISABLE, &CDiskInfoDlg::OnCsmiDisable)
	ON_COMMAND(ID_CSMI_ENABLE_AUTO, &CDiskInfoDlg::OnCsmiEnableAuto)
	ON_COMMAND(ID_CSMI_ENABLE_RAID, &CDiskInfoDlg::OnCsmiEnableRaid)
	ON_COMMAND(ID_CSMI_ENABLE_ALL, &CDiskInfoDlg::OnCsmiEnableAll)
//	ON_COMMAND(ID_INSTALL_GADGET, &CDiskInfoDlg::OnInstallGadget)
//	ON_COMMAND(ID_ALARM_HISTORY, &CDiskInfoDlg::OnAlarmHistory)
	ON_COMMAND(ID_ALERT_SOUND, &CDiskInfoDlg::OnAlertSound)
	ON_COMMAND(ID_HIDE_NO_SMART_DISK, &CDiskInfoDlg::OnHideNoSmartDisk)

	ON_MESSAGE(MY_PLAY_ALERT_SOUND, OnPlayAlertSound)
	ON_BN_CLICKED(IDC_BUTTON_DISK0, &CDiskInfoDlg::OnBnClickedButtonDisk0)
	ON_BN_CLICKED(IDC_BUTTON_DISK1, &CDiskInfoDlg::OnBnClickedButtonDisk1)
	ON_BN_CLICKED(IDC_BUTTON_DISK2, &CDiskInfoDlg::OnBnClickedButtonDisk2)
	ON_BN_CLICKED(IDC_BUTTON_DISK3, &CDiskInfoDlg::OnBnClickedButtonDisk3)
	ON_BN_CLICKED(IDC_BUTTON_DISK4, &CDiskInfoDlg::OnBnClickedButtonDisk4)
	ON_BN_CLICKED(IDC_BUTTON_DISK5, &CDiskInfoDlg::OnBnClickedButtonDisk5)
	ON_BN_CLICKED(IDC_BUTTON_DISK6, &CDiskInfoDlg::OnBnClickedButtonDisk6)
	ON_BN_CLICKED(IDC_BUTTON_DISK7, &CDiskInfoDlg::OnBnClickedButtonDisk7)
	ON_BN_CLICKED(IDC_BUTTON_DISK8, &CDiskInfoDlg::OnBnClickedButtonDisk8)
	ON_BN_CLICKED(IDC_BUTTON_DISK9, &CDiskInfoDlg::OnBnClickedButtonDisk9)
	ON_BN_CLICKED(IDC_BUTTON_DISK10, &CDiskInfoDlg::OnBnClickedButtonDisk10)
	ON_BN_CLICKED(IDC_BUTTON_DISK11, &CDiskInfoDlg::OnBnClickedButtonDisk11)
	ON_BN_CLICKED(IDC_BUTTON_DISK12, &CDiskInfoDlg::OnBnClickedButtonDisk12)
	ON_BN_CLICKED(IDC_BUTTON_DISK13, &CDiskInfoDlg::OnBnClickedButtonDisk13)
	ON_BN_CLICKED(IDC_BUTTON_DISK14, &CDiskInfoDlg::OnBnClickedButtonDisk14)
	ON_BN_CLICKED(IDC_BUTTON_DISK15, &CDiskInfoDlg::OnBnClickedButtonDisk15)
#ifdef MAX_DRIVE_20
	ON_BN_CLICKED(IDC_BUTTON_DISK16, &CDiskInfoDlg::OnBnClickedButtonDisk16)
	ON_BN_CLICKED(IDC_BUTTON_DISK17, &CDiskInfoDlg::OnBnClickedButtonDisk17)
	ON_BN_CLICKED(IDC_BUTTON_DISK18, &CDiskInfoDlg::OnBnClickedButtonDisk18)
	ON_BN_CLICKED(IDC_BUTTON_DISK19, &CDiskInfoDlg::OnBnClickedButtonDisk19)
#endif

	ON_BN_CLICKED(IDC_BUTTON_PRE_DISK, &CDiskInfoDlg::OnBnClickedButtonPreDisk)
	ON_BN_CLICKED(IDC_BUTTON_NEXT_DISK, &CDiskInfoDlg::OnBnClickedButtonNextDisk)
	ON_BN_CLICKED(IDC_BUTTON_HEALTH_STATUS, &CDiskInfoDlg::OnBnClickedButtonHealthStatus)
	ON_BN_CLICKED(IDC_BUTTON_TEMPERATURE, &CDiskInfoDlg::OnBnClickedButtonTemperature)
	ON_BN_CLICKED(IDC_BUTTON_VOICE, &CDiskInfoDlg::OnBnClickedButtonVoice)
	ON_BN_CLICKED(IDC_BUTTON_COPYRIGHT, &CDiskInfoDlg::OnBnClickedButtonCopyright)
	ON_BN_CLICKED(IDC_BUTTON_LIFE, &CDiskInfoDlg::OnBnClickedButtonLife)
	ON_WM_SHOWWINDOW()
	ON_WM_NCCREATE()
END_MESSAGE_MAP()

LRESULT CDiskInfoDlg::OnPlayAlertSound(WPARAM wParam, LPARAM lParam)
{
	static int id = 0;
	static int idlist[21] = {1, 2, 3, 4, 5, 6, 7, 8, 601, 602, 603, 604, 605, 606, 607, 701, 702, 703, 704, 705, 707};

	if(wParam == NULL)
	{
		id = idlist[rand() % 21];
	}
	else
	{
		id = (int)wParam;
	}

	TCHAR str[256];
	GetPrivateProfileStringFx(_T("Setting"), _T("AlertSoundPath"), _T(""), str, 256, m_Ini);
	m_AlertSoundPath = str;
	if (m_AlertSoundPath.Compare(_T("")) != 0)
	{
		id = 601;
	}

	AlertSound(id, AS_SET_SOUND_ID);
	AlertSound(1000, AS_PLAY_SOUND);

	return 0;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CDiskInfoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
	//	CPaintDC dc(this);
    //  dc.DrawState(CPoint(0, 0), CSize((int)(1000 * m_ZoomRatio), (int)(1000 * m_ZoomRatio)), m_BitmapBk, DST_BITMAP);
		CMainDialogFx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDiskInfoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL CDiskInfoDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if(WM_LANGUAGE_ID <= wParam && wParam < (WPARAM)WM_LANGUAGE_ID + m_MenuArrayLang.GetSize())
	{
#ifdef _UNICODE
		CMenu menu;
		CMenu subMenu;
		CMenu subMenuAN;
		CMenu subMenuOZ;
		menu.Attach(GetMenu()->GetSafeHmenu());
		subMenu.Attach(menu.GetSubMenu(MENU_LANG_INDEX)->GetSafeHmenu());
		subMenuAN.Attach(subMenu.GetSubMenu(0)->GetSafeHmenu());
		subMenuOZ.Attach(subMenu.GetSubMenu(1)->GetSafeHmenu());

		m_CurrentLang = m_MenuArrayLang.GetAt(wParam - WM_LANGUAGE_ID);
		ChangeLang(m_MenuArrayLang.GetAt(wParam - WM_LANGUAGE_ID));
		subMenuAN.CheckMenuRadioItem(WM_LANGUAGE_ID, WM_LANGUAGE_ID + (UINT)m_MenuArrayLang.GetSize(),
									(UINT)wParam, MF_BYCOMMAND);
		subMenuOZ.CheckMenuRadioItem(WM_LANGUAGE_ID, WM_LANGUAGE_ID + (UINT)m_MenuArrayLang.GetSize(),
									(UINT)wParam, MF_BYCOMMAND);

		subMenuOZ.Detach();
		subMenuAN.Detach();
		subMenu.Detach();
		menu.Detach();
#else
		CMenu menu;
		CMenu subMenu;
		menu.Attach(GetMenu()->GetSafeHmenu());
		subMenu.Attach(menu.GetSubMenu(MENU_LANG_INDEX)->GetSafeHmenu());

		m_CurrentLang = m_MenuArrayLang.GetAt(wParam - WM_LANGUAGE_ID);
		ChangeLang(m_MenuArrayLang.GetAt(wParam - WM_LANGUAGE_ID));
		subMenu.CheckMenuRadioItem(WM_LANGUAGE_ID, WM_LANGUAGE_ID + (UINT)m_MenuArrayLang.GetSize(),
									(UINT)wParam, MF_BYCOMMAND);
		subMenu.Detach();
		menu.Detach();
#endif
	}
	// Task Tray Menu
	else if(wParam == MY_EXIT)
	{
		SavePos();
		RemoveTrayMainIcon();
		for(int i = 0; i < m_Ata.vars.GetCount(); i++)
		{
			RemoveTemperatureIcon(i);
		}
		KillGraphDlg();
		CMainDialogFx::OnCancel();
	}
	else if(wParam == MY_SHOW_MAIN_DIALOG && m_bResidentMinimize)
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
	else if(wParam == MY_SHOW_MAIN_DIALOG)
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
	else if(wParam == MY_SHOW_TEMPERATURE_ICON_ONLY)
	{
		ShowTemperatureIconOnly();
	}
	else if(wParam == (WPARAM)SHOW_GRAPH_BASE + CAtaSmart::MAX_DISK)
	{
		ShowGraphDlg(-1); // Using "GraphHideDisk" option
	}
	else if(SHOW_GRAPH_BASE <= wParam && wParam < (WPARAM)SHOW_GRAPH_BASE + CAtaSmart::MAX_DISK)
	{
		ShowGraphDlg((int)wParam - SHOW_GRAPH_BASE);
	}
	else if(ALARM_SETTING_HEALTH_STATUS_BASE <= wParam && wParam <= (WPARAM)ALARM_SETTING_HEALTH_STATUS_BASE + CAtaSmart::MAX_DISK + 1)
	{
		int i = (int)(wParam - ALARM_SETTING_HEALTH_STATUS_BASE);

		if(i == CAtaSmart::MAX_DISK + 1) // Disable All
		{
			for(int j = 0; j < m_Ata.vars.GetCount(); j++)
			{
				m_Ata.vars[j].AlarmHealthStatus = FALSE;
				WritePrivateProfileStringFx(_T("AlarmHealthStatus"), m_Ata.vars[j].ModelSerial, _T("0"), m_Ini);
			}
		}
		else if(i == CAtaSmart::MAX_DISK) // Enable All
		{
			for(int j = 0; j < m_Ata.vars.GetCount(); j++)
			{
				m_Ata.vars[j].AlarmHealthStatus = TRUE;
				WritePrivateProfileStringFx(_T("AlarmHealthStatus"), m_Ata.vars[j].ModelSerial, _T("1"), m_Ini);
			}
		}
		else
		{
			CString alarm;
			if(m_Ata.vars[i].AlarmHealthStatus)
			{
				m_Ata.vars[i].AlarmHealthStatus = FALSE;
				alarm.Format(_T("%d"), FALSE);
			}
			else
			{
				m_Ata.vars[i].AlarmHealthStatus = TRUE;
				alarm.Format(_T("%d"), TRUE);
			}
			WritePrivateProfileStringFx(_T("AlarmHealthStatus"), m_Ata.vars[i].ModelSerial, alarm, m_Ini);
		}
	}
	else if(ALARM_SETTING_TEMPERATURE_BASE <= wParam && wParam <= (WPARAM)ALARM_SETTING_TEMPERATURE_BASE + ((WPARAM)CAtaSmart::MAX_DISK + 1) * 100)
	{
		int i = (int)(wParam - ALARM_SETTING_TEMPERATURE_BASE) / 100;
		int j = (int)(wParam - ALARM_SETTING_TEMPERATURE_BASE) % 100;

		if(i == CAtaSmart::MAX_DISK)
		{
			for(int k = 0; k < m_Ata.vars.GetCount(); k++)
			{
				m_Ata.vars[k].AlarmTemperature = j;
				CString temperature;
				temperature.Format(_T("%d"), j);
				WritePrivateProfileStringFx(_T("AlarmTemperature"), m_Ata.vars[k].Model + m_Ata.vars[k].SerialNumber, temperature, m_Ini);
			}
		}
		else
		{
			m_Ata.vars[i].AlarmTemperature = j;
			CString temperature;
			temperature.Format(_T("%d"), j);
			WritePrivateProfileStringFx(_T("AlarmTemperature"), m_Ata.vars[i].ModelSerial, temperature, m_Ini);
		}
	}
	else if(TRAY_TEMPERATURE_ICON_BASE <= wParam && wParam <= (WPARAM)TRAY_TEMPERATURE_ICON_BASE + CAtaSmart::MAX_DISK + 1)
	{
		int i = (int)(wParam - TRAY_TEMPERATURE_ICON_BASE);

		if(i == CAtaSmart::MAX_DISK + 1) // Hide All
		{
			for(int j = 0; j < m_Ata.vars.GetCount(); j++)
			{
				if(m_bTrayTemperatureIcon[j])
				{
					if(RemoveTemperatureIcon(j))
					{
						CString cstr;
						cstr.Format(_T("%d"), 0);
						WritePrivateProfileStringFx(_T("TemperatureIcon"), m_Ata.vars[j].ModelSerial, cstr, m_Ini);
					}
				}
			}
			if(m_bShowTemperatureIconOnly && ! IsTemperatureIconExist())
			{
				AddTrayMainIcon();
			}
		}
		else if(i == CAtaSmart::MAX_DISK) // Show All
		{
			int max = gRegIconId;
			for(int j = (int)m_Ata.vars.GetCount() -1; j >= 0; j--)
			{
				if(! m_bTrayTemperatureIcon[j])
				{
					if(AddTemperatureIcon(j))
					{
						CString cstr;
						cstr.Format(_T("%d"), 1);
						WritePrivateProfileStringFx(_T("TemperatureIcon"), m_Ata.vars[j].ModelSerial, cstr, m_Ini);
						max = TRAY_TEMPERATURE_ICON_BASE + j;
					}
				}
			}
			if(m_bShowTemperatureIconOnly && IsTemperatureIconExist())
			{
				if(RemoveTrayMainIcon())
				{
					m_MainIconId = max;
				}
			}
			else
			{
				AddTrayMainIcon();
			}
		}
		else
		{
			if(m_bTrayTemperatureIcon[i])
			{
				if(RemoveTemperatureIcon(i))
				{
					CString cstr;
					cstr.Format(_T("%d"), 0);
					WritePrivateProfileStringFx(_T("TemperatureIcon"), m_Ata.vars[i].ModelSerial, cstr, m_Ini);
					
					if(! IsTemperatureIconExist())
					{
						AddTrayMainIcon();
					}
				}
			}
			else if(AddTemperatureIcon(i))
			{
				CString cstr;
				cstr.Format(_T("%d"), 1);
				WritePrivateProfileStringFx(_T("TemperatureIcon"), m_Ata.vars[i].ModelSerial, cstr, m_Ini);

				if(m_bShowTemperatureIconOnly && IsTemperatureIconExist())
				{
					if(RemoveTrayMainIcon())
					{
						m_MainIconId = TRAY_TEMPERATURE_ICON_BASE + i;
					}
				}
			}
		}
	}
	else if(SELECT_DISK_BASE <= wParam && wParam < (WPARAM)SELECT_DISK_BASE + CAtaSmart::MAX_DISK)
	{
		int i = (int)(wParam - SELECT_DISK_BASE);
		m_DriveMenuPage = i / m_DriveMenuNumber;
		SelectDrive(i);
	}
	else if(AUTO_REFRESH_TARGET_BASE <= wParam && wParam <= (WPARAM)AUTO_REFRESH_TARGET_BASE + CAtaSmart::MAX_DISK + 1)
	{
		int i = (int)(wParam - AUTO_REFRESH_TARGET_BASE);
		// Target All Disk : AUTO_REFRESH_TARGET_BASE + CAtaSmart::MAX_DISK
		// Unarget All Disk : AUTO_REFRESH_TARGET_BASE + CAtaSmart::MAX_DISK
		CMenu *menu = GetMenu();
		if(i == CAtaSmart::MAX_DISK) // Target All Disk
		{
			for(int j = 0; j < m_Ata.vars.GetCount(); j++)
			{
				m_bAutoRefreshTarget[j] = TRUE;
				menu->CheckMenuItem(AUTO_REFRESH_TARGET_BASE + j, MF_CHECKED);
				WritePrivateProfileStringFx(_T("AutoRefreshTarget"), m_Ata.vars[j].ModelSerial, _T("1"), m_Ini);
			}
		}
		else if(i == CAtaSmart::MAX_DISK + 1) // Unarget All Disk
		{
			for(int j = 0; j < m_Ata.vars.GetCount(); j++)
			{
				m_bAutoRefreshTarget[j] = FALSE;
				menu->CheckMenuItem(AUTO_REFRESH_TARGET_BASE + j, MF_UNCHECKED);
				WritePrivateProfileStringFx(_T("AutoRefreshTarget"), m_Ata.vars[j].ModelSerial, _T("0"), m_Ini);
			}
		}
		else
		{
			if(m_bAutoRefreshTarget[i])
			{
				m_bAutoRefreshTarget[i] = FALSE;
				menu->CheckMenuItem(AUTO_REFRESH_TARGET_BASE + i, MF_UNCHECKED);
				WritePrivateProfileStringFx(_T("AutoRefreshTarget"), m_Ata.vars[i].ModelSerial, _T("0"), m_Ini);
			}
			else
			{
				m_bAutoRefreshTarget[i] = TRUE;
				menu->CheckMenuItem(AUTO_REFRESH_TARGET_BASE + i, MF_CHECKED);
				WritePrivateProfileStringFx(_T("AutoRefreshTarget"), m_Ata.vars[i].ModelSerial, _T("1"), m_Ini);
			}
		}
		SetMenu(menu);
		DrawMenuBar();
	}
	else if(WM_THEME_ID <= wParam && wParam < (WPARAM)WM_THEME_ID + m_MenuArrayTheme.GetSize())
	{
#ifndef SUISHO_SHIZUKU_SUPPORT
		CMenu *menu = GetMenu();
		if (menu->GetMenuState(ID_GREEN_MODE, MF_BYCOMMAND) & MFS_CHECKED)
		{
			m_bGreenMode = TRUE;
		}
		else
		{
			m_bGreenMode = FALSE;
		}
#endif
		CMainDialogFx::OnCommand(wParam, lParam);

#ifndef SUISHO_SHIZUKU_SUPPORT
		if(m_CurrentTheme.Compare(_T("Simplicity")) == 0)
		{
			m_bGreenMode = TRUE;
		}
#endif
		UpdateListCtrl(m_SelectDisk);
		return TRUE;
	}

	return CMainDialogFx::OnCommand(wParam, lParam);
}

void CDiskInfoDlg::UpdateDialogSize()
{
	CDialogFx::UpdateDialogSize();

	if (GetPrivateProfileInt(_T("Setting"), _T("HideSmartInfo"), 0, m_Ini))
	{
		m_SizeX = SIZE_X;
		m_SizeY = SIZE_MIN_Y;
		m_bHideSmartInfo = TRUE;
		SetClientSize(m_SizeX, m_SizeY, m_ZoomRatio);

		CMenu *menu = GetMenu();
		menu->CheckMenuItem(ID_HIDE_SMART_INFO, MF_CHECKED);
		SetMenu(menu);
		DrawMenuBar();
	}
	else
	{
		m_SizeX = SIZE_SMART_X;
		int y = GetPrivateProfileInt(_T("Setting"), _T("Height"), SIZE_SMART_Y, m_Ini);
		if (y > 0)
		{
			m_SizeY = y;
		}

		if (m_SizeY < SIZE_MIN_Y)
		{
			m_SizeY = SIZE_MIN_Y;
		}
		else if (m_SizeY > SIZE_MAX_Y)
		{
			m_SizeY = SIZE_MAX_Y;
		}

		SetClientSize(m_SizeX, m_SizeY, m_ZoomRatio);
		m_bHideSmartInfo = FALSE;
		CMenu *menu = GetMenu();
		menu->CheckMenuItem(ID_HIDE_SMART_INFO, MF_UNCHECKED);
		SetMenu(menu);
		DrawMenuBar();
	}

	UpdateBackground(TRUE, FALSE);
	SetControlFont();

#ifdef SUISHO_SHIZUKU_SUPPORT
	int positionX = 0;
	if (m_CharacterPosition == 0)
	{
		m_OffsetX = OFFSET_X;
		positionX = 0;
	}
	else
	{
		m_OffsetX = 0;
		positionX = SIZE_X - OFFSET_X;
	}
	m_CtrlVoice.InitControl(positionX, 48, OFFSET_X, m_SizeY - 24 - 48, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlVoiceHide.InitControl(positionX, 0, OFFSET_X, 48, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlVoice.SetHandCursor();

	if (m_CurrentLang.Find(L"Japanese") == 0)
	{
		/* 
		SYSTEMTIME systime;
		GetLocalTime(&systime);
		if ((systime.wYear == 2015 && systime.wMonth == 12 && systime.wDay <= 17)
			|| (systime.wYear == 2015 && systime.wMonth == 12 && systime.wDay == 18 && systime.wHour < 20))
		{
			m_CtrlCopyright.InitControl(0, m_SizeY - 24, OFFSET_X, 24, m_ZoomRatio, IP(L"ShizukuAkibaMoe"), 1, SS_CENTER, CButtonCx::OwnerDrawImage);
		}
		else 
		{		}
		*/
		m_CtrlCopyright.InitControl((int)(positionX * m_ZoomRatio), (int)(m_SizeY * m_ZoomRatio) - (int)(24 * m_ZoomRatio), (int)(OFFSET_X * m_ZoomRatio), (int)(24 * m_ZoomRatio), 1.0, &m_BkDC, IP(PROJECT_COPYRIGHT), 1, BS_CENTER, OwnerDrawImage, FALSE, FALSE, FALSE);
	}
	else
	{
		m_CtrlCopyright.InitControl((int)(positionX * m_ZoomRatio), (int)(m_SizeY * m_ZoomRatio) - (int)(24 * m_ZoomRatio), (int)(OFFSET_X * m_ZoomRatio), (int)(24 * m_ZoomRatio), 1.0, &m_BkDC, IP(PROJECT_COPYRIGHT), 1, BS_CENTER, OwnerDrawImage, FALSE, FALSE, FALSE);
	//	m_CtrlCopyright.InitControl(positionX, m_SizeY - 24, OFFSET_X, 24, m_ZoomRatio, &m_BkDC, IP(PROJECT_COPYRIGHT), 1, BS_CENTER, OwnerDrawImage, FALSE, FALSE);
	}
	
	m_CtrlCopyright.SetHandCursor();
#else
	m_CtrlVoice.ShowWindow(SW_HIDE);
	m_CtrlVoiceHide.ShowWindow(SW_HIDE);
	m_CtrlCopyright.ShowWindow(SW_HIDE);
#endif

	int buttonDiskHeight = 48;

	if(m_bHighContrast){ buttonDiskHeight = 56;}
	for(int i = 0; i < MAX_DRIVE_MENU; i++)
	{
		m_ButtonDisk[i].InitControl(DRIVE_MENU_SIZE / 2 * i + m_OffsetX, 0, DRIVE_MENU_SIZE / 2, buttonDiskHeight, m_ZoomRatio, &m_BkDC, IP(L"noDisk"), 1, BS_CENTER, OwnerDrawImage, FALSE, FALSE, FALSE);
		m_ButtonDisk[i].SetMargin(0, 0, 3, 0, m_ZoomRatio);
		m_ButtonDisk[i].SetHandCursor(TRUE);
	}

	if (m_bHalfDriveMenu)
	{
		for (int i = 0; i < m_DriveMenuNumber; i++)
		{
			m_ButtonDisk[i].InitControl(DRIVE_MENU_SIZE / 2 * i + m_OffsetX, 0, DRIVE_MENU_SIZE / 2, buttonDiskHeight, m_ZoomRatio, &m_BkDC, IP(L"noDiskMini"), 1, BS_CENTER, OwnerDrawImage, FALSE, FALSE, FALSE);
			m_ButtonDisk[i].SetMargin(3, 0, 3, 0, m_ZoomRatio);
			m_ButtonDisk[i].SetHandCursor(TRUE);
		}
	}
	else
	{
		for (int i = 0; i < m_DriveMenuNumber; i++)
		{
			m_ButtonDisk[i].InitControl(DRIVE_MENU_SIZE * i + m_OffsetX, 0, DRIVE_MENU_SIZE, buttonDiskHeight, m_ZoomRatio, &m_BkDC, IP(L"noDisk"), 1, BS_CENTER, OwnerDrawImage, FALSE, FALSE, FALSE);
			m_ButtonDisk[i].SetMargin(3, 0, 3, 0, m_ZoomRatio);
			m_ButtonDisk[i].SetHandCursor(TRUE);
		}
	}

	InitDriveList();

	m_CtrlButtonPreDisk.SetHandCursor(TRUE);
	m_CtrlButtonNextDisk.SetHandCursor(TRUE);

	if (m_bHighContrast)
	{
		m_CtrlModel.InitControl(40 + m_OffsetX, 56, 592, 32, m_ZoomRatio, &m_BkDC, NULL, 0, ES_CENTER, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
		m_CtrlButtonPreDisk.InitControl (  8 + m_OffsetX, 60, 24, 24, m_ZoomRatio, &m_BkDC, IP(L"preDisk"), 2, BS_CENTER, OwnerDrawImage, FALSE, FALSE, FALSE);
		m_CtrlButtonNextDisk.InitControl(640 + m_OffsetX, 60, 24, 24, m_ZoomRatio, &m_BkDC, IP(L"nextDisk"), 2, BS_CENTER, OwnerDrawImage, FALSE, FALSE, FALSE);

		m_CtrlButtonPreDisk.SetWindowTextW(L"");
		m_CtrlButtonNextDisk.SetWindowTextW(L"");
	}
	else
	{
		m_CtrlModel.InitControl(32 + m_OffsetX, 52, 608, 32, m_ZoomRatio, &m_BkDC, NULL, 0, ES_CENTER, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
		m_CtrlButtonPreDisk.InitControl(8 + m_OffsetX, 56, 24, 24, m_ZoomRatio, &m_BkDC, IP(L"preDisk"), 2, BS_CENTER, OwnerDrawImage, FALSE, FALSE, FALSE);
		m_CtrlButtonNextDisk.InitControl(640 + m_OffsetX, 56, 24, 24, m_ZoomRatio, &m_BkDC, IP(L"nextDisk"), 2, BS_CENTER, OwnerDrawImage, FALSE, FALSE, FALSE);

		m_CtrlButtonPreDisk.SetWindowTextW(L"");
		m_CtrlButtonNextDisk.SetWindowTextW(L"");
	}
	m_CtrlModel.Adjust();

	CString className;
	if (m_Ata.vars.GetCount())
	{
		className = GetDiskStatusClass(m_Ata.vars[m_SelectDisk].DiskStatus);
	}
	else
	{
		className = L"diskStatusUnknown";
	}

	int labelWidth = 128;
	if (m_bHighContrast) { labelWidth = 124; }
#ifdef SUISHO_SHIZUKU_SUPPORT
	m_CtrlLabelDiskStatus.InitControl(128 + m_OffsetX, 260, labelWidth, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlLabelTemperature.InitControl(436 + m_OffsetX, 260, labelWidth, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);

	m_CtrlLabelDiskStatus.SetMargin(0, 0, 0, 4, m_ZoomRatio);
	m_CtrlLabelTemperature.SetMargin(0, 0, 0, 4, m_ZoomRatio);

	m_CtrlDiskStatus.InitControl       (256 + m_OffsetX, 256, 180, 28, m_ZoomRatio, &m_BkDC, IP(className), 1, BS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
//	m_CtrlDiskStatus.SetAlpha(IMAGE_ALPHA);

	className.Replace(L"Green", L"");
	if (m_Ata.vars.GetCount() && className.Find(L"Good") != -1 && m_Ata.vars[m_SelectDisk].Life == 100)
	{
		className += L"100";
	}
	m_CtrlLife.InitControl             (m_OffsetX, 88, 128, 192, m_ZoomRatio, &m_BkDC, IP(L"SD" + className), 1, BS_CENTER, OwnerDrawImage, FALSE, FALSE, FALSE);
	m_CtrlLife.SetHandCursor(TRUE);
#else
	m_CtrlLife.ShowWindow(SW_HIDE);
	m_CtrlLabelDiskStatus.InitControl  (8 + m_OffsetX,  88, 100, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);
	m_CtrlLabelTemperature.InitControl (8 + m_OffsetX, 184, 100, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, FALSE, FALSE, FALSE);

	m_CtrlLabelDiskStatus.SetMargin(0, 0, 0, 4, m_ZoomRatio);
	m_CtrlLabelTemperature.SetMargin(0, 0, 0, 4, m_ZoomRatio);

	m_CtrlDiskStatus.InitControl       (8 + m_OffsetX, 112, 100, 60, m_ZoomRatio, &m_BkDC, IP(className), 1, BS_CENTER, OwnerDrawImage, FALSE, FALSE, FALSE);
	m_CtrlDiskStatus.SetMargin(4, 0, 4, 0, m_ZoomRatio);
#endif
	m_CtrlDiskStatus.SetHandCursor(TRUE);

	if (m_Ata.vars.GetCount() && (m_Ata.vars[m_SelectDisk].IsSmartCorrect || m_Ata.vars[m_SelectDisk].DiskVendorId == m_Ata.SSD_VENDOR_NVME))
	{
		className = GetTemperatureClass(m_Ata.vars[m_SelectDisk].Temperature, m_Ata.vars[m_SelectDisk].AlarmTemperature);
	}
	else
	{
		className = _T("temperatureUnknown");
	}
#ifdef SUISHO_SHIZUKU_SUPPORT
	m_CtrlTemperature.InitControl      (564 + m_OffsetX, 256, 100, 28, m_ZoomRatio, &m_BkDC, IP(className), 1, BS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
#else
	m_CtrlTemperature.InitControl      (8 + m_OffsetX, 208, 100, 40, m_ZoomRatio, &m_BkDC, IP(className), 1, BS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
#endif

	m_CtrlDiskStatus.SetHandCursor(TRUE);
	m_CtrlTemperature.SetHandCursor(TRUE);

	m_CtrlLabelFirmware.SetMargin(0, 0, 0, 4, m_ZoomRatio);
	m_CtrlLabelSerialNumber.SetMargin(0, 0, 0, 4, m_ZoomRatio);
	m_CtrlLabelInterface.SetMargin(0, 0, 0, 4, m_ZoomRatio);
	m_CtrlLabelTransferMode.SetMargin(0, 0, 0, 4, m_ZoomRatio);
	m_CtrlLabelDriveMap.SetMargin(0, 0, 0, 4, m_ZoomRatio);
	m_CtrlLabelAtaAtapi.SetMargin(0, 0, 0, 4, m_ZoomRatio);
	m_CtrlLabelFeature.SetMargin(0, 0, 0, 4, m_ZoomRatio);

	m_CtrlLabelFirmware.InitControl(128 + m_OffsetX, 88, labelWidth, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlLabelSerialNumber.InitControl(128 + m_OffsetX, 112, labelWidth, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlLabelInterface.InitControl(128 + m_OffsetX, 136, labelWidth, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlLabelTransferMode.InitControl(128 + m_OffsetX, 160, labelWidth, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlLabelDriveMap.InitControl(128 + m_OffsetX, 184, labelWidth, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlLabelAtaAtapi.InitControl(128 + m_OffsetX, 208, labelWidth, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlLabelFeature.InitControl(128 + m_OffsetX, 232, labelWidth, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);

	m_CtrlFirmware.SetGlassColor(m_Glass, m_GlassAlpha);
	m_CtrlSerialNumber.SetGlassColor(m_Glass, m_GlassAlpha);
	m_CtrlInterface.SetGlassColor(m_Glass, m_GlassAlpha);
	m_CtrlTransferMode.SetGlassColor(m_Glass, m_GlassAlpha);
	m_CtrlDriveMap.SetGlassColor(m_Glass, m_GlassAlpha);
	m_CtrlAtaAtapi.SetGlassColor(m_Glass, m_GlassAlpha);
	m_CtrlFeature.SetGlassColor(m_Glass, m_GlassAlpha);

	m_CtrlFirmware.SetMargin(0, 4, 0, 0, m_ZoomRatio);
	m_CtrlSerialNumber.SetMargin(0, 4, 0, 0, m_ZoomRatio);
	m_CtrlInterface.SetMargin(0, 4, 0, 0, m_ZoomRatio);
	m_CtrlTransferMode.SetMargin(0, 4, 0, 0, m_ZoomRatio);
	m_CtrlDriveMap.SetMargin(0, 4, 0, 0, m_ZoomRatio);
	m_CtrlAtaAtapi.SetMargin(0, 4, 0, 0, m_ZoomRatio);
	m_CtrlFeature.SetMargin(0, 4, 0, 0, m_ZoomRatio);

	m_CtrlFirmware.InitControl    (256 + m_OffsetX,  88, 180, 20, m_ZoomRatio, &m_BkDC, NULL, 0, ES_LEFT, OwnerDrawGlass, m_bHighContrast, FALSE, TRUE);
	m_CtrlSerialNumber.InitControl(256 + m_OffsetX, 112, 180, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawGlass, m_bHighContrast, FALSE, TRUE);
	m_CtrlInterface.InitControl   (256 + m_OffsetX, 136, 180, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawGlass, m_bHighContrast, FALSE, TRUE);
	m_CtrlTransferMode.InitControl(256 + m_OffsetX, 160, 180, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawGlass, m_bHighContrast, FALSE, TRUE);
	m_CtrlDriveMap.InitControl    (256 + m_OffsetX, 184, 180, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawGlass, m_bHighContrast, FALSE, TRUE);
	m_CtrlAtaAtapi.InitControl    (256 + m_OffsetX, 208, 408, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawGlass, m_bHighContrast, FALSE, TRUE);
	m_CtrlFeature.InitControl     (256 + m_OffsetX, 232, 408, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawGlass, m_bHighContrast, FALSE, TRUE);

	m_CtrlLabelBufferSize.SetMargin(0, 0, 0, 4, m_ZoomRatio);
	m_CtrlLabelNvCacheSize.SetMargin(0, 0, 0, 4, m_ZoomRatio);
	m_CtrlLabelRotationRate.SetMargin(0, 0, 0, 4, m_ZoomRatio);
	m_CtrlLabelPowerOnCount.SetMargin(0, 0, 0, 4, m_ZoomRatio);
	m_CtrlLabelPowerOnHours.SetMargin(0, 0, 0, 4, m_ZoomRatio);

	m_CtrlLabelBufferSize.InitControl(436 + m_OffsetX, 88, labelWidth, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlLabelNvCacheSize.InitControl(436 + m_OffsetX, 112, labelWidth, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlLabelRotationRate.InitControl(436 + m_OffsetX, 136, labelWidth, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlLabelPowerOnCount.InitControl(436 + m_OffsetX, 160, labelWidth, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlLabelPowerOnHours.InitControl(436 + m_OffsetX, 184, labelWidth, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);

	m_CtrlBufferSize.SetGlassColor(m_Glass, m_GlassAlpha);
	m_CtrlNvCacheSize.SetGlassColor(m_Glass, m_GlassAlpha);
	m_CtrlRotationRate.SetGlassColor(m_Glass, m_GlassAlpha);
	m_CtrlPowerOnCount.SetGlassColor(m_Glass, m_GlassAlpha);
	m_CtrlPowerOnHours.SetGlassColor(m_Glass, m_GlassAlpha);

	m_CtrlBufferSize.SetMargin(0, 0, 0, 4, m_ZoomRatio);
	m_CtrlNvCacheSize.SetMargin(0, 0, 0, 4, m_ZoomRatio);
	m_CtrlRotationRate.SetMargin(0, 0, 0, 4, m_ZoomRatio);
	m_CtrlPowerOnCount.SetMargin(0, 0, 0, 4, m_ZoomRatio);
	m_CtrlPowerOnHours.SetMargin(0, 0, 0, 4, m_ZoomRatio);

	m_CtrlBufferSize.InitControl  (564 + m_OffsetX,  88, 100, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawGlass, m_bHighContrast, FALSE, TRUE);
	m_CtrlNvCacheSize.InitControl (564 + m_OffsetX, 112, 100, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawGlass, m_bHighContrast, FALSE, TRUE);
	m_CtrlRotationRate.InitControl(564 + m_OffsetX, 136, 100, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawGlass, m_bHighContrast, FALSE, TRUE);
	m_CtrlPowerOnCount.InitControl(564 + m_OffsetX, 160, 100, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawGlass, m_bHighContrast, FALSE, TRUE);
	m_CtrlPowerOnHours.InitControl(564 + m_OffsetX, 184, 100, 20, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawGlass, m_bHighContrast, FALSE, TRUE);

	m_CtrlFirmware.SetDrawFrame(TRUE);
	m_CtrlSerialNumber.SetDrawFrame(TRUE);
	m_CtrlInterface.SetDrawFrame(TRUE);
	m_CtrlTransferMode.SetDrawFrame(TRUE);
	m_CtrlDriveMap.SetDrawFrame(TRUE);
	m_CtrlAtaAtapi.SetDrawFrame(TRUE);
	m_CtrlFeature.SetDrawFrame(TRUE);

	m_CtrlBufferSize.SetDrawFrame(TRUE);
	m_CtrlNvCacheSize.SetDrawFrame(TRUE);
	m_CtrlRotationRate.SetDrawFrame(TRUE);
	m_CtrlPowerOnCount.SetDrawFrame(TRUE);
	m_CtrlPowerOnHours.SetDrawFrame(TRUE);

	m_CtrlFirmware.Adjust();
	m_CtrlSerialNumber.Adjust();
	m_CtrlInterface.Adjust();
	m_CtrlTransferMode.Adjust();
	m_CtrlDriveMap.Adjust();
	m_CtrlAtaAtapi.Adjust();
	m_CtrlFeature.Adjust();

	m_CtrlBufferSize.Adjust();
	m_CtrlNvCacheSize.Adjust();
	m_CtrlRotationRate.Adjust();
	m_CtrlPowerOnCount.Adjust();
	m_CtrlPowerOnHours.Adjust();

	CRect rect;
	GetClientRect(&rect);

	m_List.SetTextColor1(m_ListText1);
	m_List.SetTextColor2(m_ListText2);
	m_List.SetTextSelected(m_ListTextSelected);
	m_List.SetBkColor1(m_ListBk1);
	m_List.SetBkColor2(m_ListBk2);
	m_List.SetBkSelected(m_ListBkSelected);
	m_List.SetLineColor1(m_ListLine1);
	m_List.SetLineColor2(m_ListLine2);
	m_List.SetGlassColor(m_Glass, m_GlassAlpha);

#ifdef SUISHO_SHIZUKU_SUPPORT
	m_List.InitControl(8 + m_OffsetX, SIZE_Y, 672 - 16, (int)(rect.Height() / m_ZoomRatio - SIZE_Y - 8), 672 - 16, 1000 - SIZE_Y - 8, m_ZoomRatio, &m_BkDC, OwnerDrawGlass, m_bHighContrast, FALSE);
#else
	m_List.InitControl(8 + m_OffsetX, SIZE_Y, 672 - 16, (int)(rect.Height() / m_ZoomRatio - SIZE_Y - 8), 672 - 16, 1000 - SIZE_Y - 8, m_ZoomRatio, &m_BkDC, SystemDraw, m_bHighContrast, FALSE);
#endif

	RebuildListHeader(m_SelectDisk, TRUE);
	UpdateListCtrl(m_SelectDisk);

	Invalidate();
}


void CDiskInfoDlg::OnSize(UINT nType, int cx, int cy)
{
	CMainDialogFx::OnSize(nType, cx, cy);

	static BOOL flag = FALSE;

	if(flag)
	{
#ifdef SUISHO_SHIZUKU_SUPPORT
		int positionX = 0;
		if (m_CharacterPosition == 0)
		{
			positionX = 0;
		}
		else
		{
			positionX = SIZE_X - OFFSET_X;
		}
		m_List.MoveWindow((int)((8 + m_OffsetX) * m_ZoomRatio), (int)(SIZE_Y * m_ZoomRatio), (int)((672 - 16) * m_ZoomRatio), (int)(cy - ((int)(SIZE_Y + 8) * m_ZoomRatio)));
		m_CtrlVoice.MoveWindow((int)(positionX * m_ZoomRatio), (int)(48 * m_ZoomRatio), (int)(OFFSET_X * m_ZoomRatio), (int)(cy - (int)((24 + 48) * m_ZoomRatio)));
		// m_CtrlCopyright.MoveWindow(0, (int)(cy - (24 * m_ZoomRatio)), (int)(m_OffsetX * m_ZoomRatio), (int)(24 * m_ZoomRatio));
		m_CtrlCopyright.InitControl((int)(positionX * m_ZoomRatio), (int)(cy - (24 * m_ZoomRatio)), (int)(OFFSET_X * m_ZoomRatio), (int)(24 * m_ZoomRatio), 1.0, &m_BkDC, IP(PROJECT_COPYRIGHT), 1, BS_CENTER, OwnerDrawImage, FALSE, FALSE, FALSE);
#else
		m_List.MoveWindow((int)((8.0 + m_OffsetX) * m_ZoomRatio), (int)(SIZE_Y * m_ZoomRatio), (int)((672.0 - 16.0) * m_ZoomRatio), (int)(cy - ((SIZE_Y + 8.0) * m_ZoomRatio)));
#endif
	}
	flag = TRUE;
	
	if(m_bHideSmartInfo == FALSE && m_bInitializing == FALSE && m_bDpiChanging == FALSE && cy > 0)
	{
		CString cstr;
		m_SizeY = (int)(cy / m_ZoomRatio);
		cstr.Format(_T("%d"), m_SizeY);
		WritePrivateProfileStringFx(_T("Setting"), _T("Height"), cstr, m_Ini);
	}
}

void CDiskInfoDlg::SetClientSize(int sizeX, int sizeY, double zoomRatio)
{
	RECT rw, rc;
	GetWindowRect(&rw);
	GetClientRect(&rc);

	if (rc.right != 0)
	{
		int ncaWidth = (rw.right - rw.left) - (rc.right - rc.left);
		int ncaHeight = (rw.bottom - rw.top) - (rc.bottom - rc.top);

		m_MinSizeX = (int)(sizeX * zoomRatio) + ncaWidth;
		m_MaxSizeX = m_MinSizeX;
		m_MinSizeY = (int)(SIZE_MIN_Y * m_ZoomRatio + ncaHeight);
		m_MaxSizeY = (int)(SIZE_MAX_Y * m_ZoomRatio + ncaHeight);

		SetWindowPos(NULL, 0, 0, (int)(sizeX * zoomRatio) + ncaWidth, (int)(sizeY * zoomRatio) + ncaHeight, SWP_NOMOVE | SWP_NOZORDER);
	}
}

void CDiskInfoDlg::SavePos() const
{
	WINDOWPLACEMENT place = { sizeof(WINDOWPLACEMENT) };
	//place.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(&place);

	CString x, y;
	x.Format(_T("%d"), place.rcNormalPosition.left);
	y.Format(_T("%d"), place.rcNormalPosition.top);
	WritePrivateProfileStringFx(_T("Setting"), _T("X"), x, m_Ini);
	WritePrivateProfileStringFx(_T("Setting"), _T("Y"), y, m_Ini);
}

void CDiskInfoDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = m_MinSizeX;
	lpMMI->ptMinTrackSize.y = m_MinSizeY;

	lpMMI->ptMaxTrackSize.x = m_MaxSizeX;
	lpMMI->ptMaxTrackSize.y = m_MaxSizeY;

	CMainDialogFx::OnGetMinMaxInfo(lpMMI);
}

void CDiskInfoDlg::AlarmHealthStatus(DWORD i, CString dir, CString disk)
{
	TCHAR str[256];
	CString cstr, alarm, name, title, id;
	DWORD niifType = NIIF_INFO;
	int pre = -1;
	
	name.Format(_T("(%d) %s / %s / %s\r\n"), i + 1, m_Ata.vars[i].Model.GetString(), m_Ata.vars[i].SerialNumber.GetString(), m_Ata.vars[i].DriveMap.GetString());
	title.Format(_T("(%d) %s / %s / %s"), i + 1, m_Ata.vars[i].Model.GetString(), m_Ata.vars[i].SerialNumber.GetString(), m_Ata.vars[i].DriveMap.GetString());

	GetPrivateProfileStringFx(disk, _T("HealthStatus"), _T("0"), str, 256, dir + _T("\\") + SMART_INI);
	pre = _tstoi(str);

	if(m_Ata.vars[i].DiskStatus == m_Ata.DISK_STATUS_UNKNOWN)
	{

	}
	else if(m_Ata.vars[i].DiskStatus > (DWORD)pre && m_Ata.vars[i].DiskStatus != m_Ata.DISK_STATUS_GOOD)
	{
		cstr.Format(_T("%s: [%s] -> [%s]\r\n"), i18n(_T("Dialog"), _T("HEALTH_STATUS")).GetString(),
					GetDiskStatus(pre).GetString(), GetDiskStatus(m_Ata.vars[i].DiskStatus).GetString());
		alarm += cstr;
		niifType = NIIF_WARNING;
		AddEventLog(601, 2, name + cstr);
		SendMail(601, title, cstr);
		AddAlarmHistory(601, title, cstr);
		AlertSound(601, AS_SET_SOUND_ID);
	}
	else if(m_Ata.vars[i].DiskStatus < (DWORD)pre)
	{
		cstr.Format(_T("%s: [%s] -> [%s]\r\n"), i18n(_T("Dialog"), _T("HEALTH_STATUS")).GetString(),
					GetDiskStatus(pre).GetString(), GetDiskStatus(m_Ata.vars[i].DiskStatus).GetString());
		alarm += cstr;
		niifType = NIIF_INFO;
		AddEventLog(701, 4, name + cstr);
		SendMail(701, title, cstr);
		AddAlarmHistory(701, title, cstr);
		AlertSound(701, AS_SET_SOUND_ID);
	}


	GetPrivateProfileStringFx(disk, _T("Life"), _T("-1"), str, 256, dir + _T("\\") + SMART_INI);
	pre = _tstoi(str);

	if(m_Ata.vars[i].Life == -1 || pre == -1)
	{

	}
	else if(m_Ata.vars[i].Life < pre)
	{
		cstr.Format(_T("%s: [%d] -> [%d]\r\n"), i18n(_T("SmartSsd"), _T("FF")).GetString(), pre, m_Ata.vars[i].Life);
		alarm += cstr;
		niifType = NIIF_WARNING;
		AddEventLog(607, 2, name + cstr);
		SendMail(607, title, cstr);
		AddAlarmHistory(607, title, cstr);
		AlertSound(607, AS_SET_SOUND_ID);
	}
	else if(m_Ata.vars[i].Life > pre)
	{
		cstr.Format(_T("%s: [%d] -> [%d]\r\n"), i18n(_T("SmartSsd"), _T("FF")).GetString(), pre, m_Ata.vars[i].Life);
		alarm += cstr;
		niifType = NIIF_INFO;
		AddEventLog(707, 4, name + cstr);
		SendMail(707, title, cstr);
		AddAlarmHistory(707, title, cstr);
		AlertSound(707, AS_SET_SOUND_ID);
	}

	for(DWORD j = 0; j < m_Ata.vars[i].AttributeCount; j++)
	{
		if(( m_Ata.vars[i].Attribute[j].Id == 0x05 // Reallocated Sectors Count
		|| (m_Ata.vars[i].Attribute[j].Id == 0xC4 && ! m_bWorkaroundIgnoreC4)// Reallocation Event Count
		||  m_Ata.vars[i].Attribute[j].Id == 0xC5 // Current Pending Sector Count
		||  m_Ata.vars[i].Attribute[j].Id == 0xC6 // Off-Line Scan Uncorrectable Sector Count
		) && ! m_Ata.vars[i].IsSsd)
		{
			CString target;
			DWORD eventId = 0;
			if(m_Ata.vars[i].Attribute[j].Id == 0x05)
			{
				target = _T("ReallocatedSectorsCount");
				eventId = 602;
			}
			else if(m_Ata.vars[i].Attribute[j].Id == 0xC4)
			{
				target = _T("ReallocationEventCount");
				eventId = 603;
			}
			else if(m_Ata.vars[i].Attribute[j].Id == 0xC5)
			{
				target = _T("CurrentPendingSectorCount");
				eventId = 604;
			}
			else if(m_Ata.vars[i].Attribute[j].Id == 0xC6)
			{
				target = _T("UncorrectableSectorCount");
				eventId = 605;
			}

			GetPrivateProfileStringFx(disk, target, _T("-1"), str, 256, dir + _T("\\") + SMART_INI);
			pre = _tstoi(str);
			id.Format(_T("%02X"), m_Ata.vars[i].Attribute[j].Id);
			int rawValue = MAKEWORD(m_Ata.vars[i].Attribute[j].RawValue[0], m_Ata.vars[i].Attribute[j].RawValue[1]);
			if(rawValue > pre && pre != -1)
			{
				cstr.Format(_T("%s: (%02X) %s [%d->%d]\r\n"), i18n(_T("Alarm"), _T("DEGRADATION")).GetString(),
					m_Ata.vars[i].Attribute[j].Id, i18n(_T("Smart"), id).GetString(), pre, rawValue);
				alarm += cstr;
				niifType = NIIF_WARNING;
				AddEventLog(eventId, 2, name + cstr);
				SendMail(eventId, title, cstr);
				AddAlarmHistory(eventId, title, cstr);
				AlertSound(eventId, AS_SET_SOUND_ID);
			}
			else if(rawValue < pre && pre != -1)
			{
				cstr.Format(_T("%s: (%02X) %s [%d->%d]\r\n"), i18n(_T("Alarm"), _T("RECOVERY")).GetString(),
					m_Ata.vars[i].Attribute[j].Id, i18n(_T("Smart"), id).GetString(), pre, rawValue);
				alarm += cstr;
				niifType = NIIF_INFO;
				AddEventLog(eventId + 100, 4, name + cstr);
				SendMail(eventId + 100, title, cstr);
				AddAlarmHistory(eventId + 100, title, cstr);
				AlertSound(eventId + 100, AS_SET_SOUND_ID);
			}
		}

		static CTime preTime[CAtaSmart::MAX_DISK] = {0};
		if(m_Ata.vars[i].AlarmTemperature > 0 && m_Ata.vars[i].Temperature >= m_Ata.vars[i].AlarmTemperature)
		{
			if(CTime::GetTickCount() - preTime[i] > ALARM_TEMPERATURE_PERIOD)
			{
				if(m_bFahrenheit)
				{
					cstr.Format(_T("%s: %d F\r\n"), i18n(_T("Alarm"), _T("ALARM_TEMPERATURE")).GetString(), m_Ata.vars[i].Temperature * 9 / 5 + 32);
				}
				else
				{
					cstr.Format(_T("%s: %d C\r\n"), i18n(_T("Alarm"), _T("ALARM_TEMPERATURE")).GetString(), m_Ata.vars[i].Temperature);
				}
				AddEventLog(606, 2, name + cstr);
				SendMail(606, title, cstr);
				AddAlarmHistory(606, title, cstr);
				AlertSound(606, AS_SET_SOUND_ID);

				niifType = NIIF_WARNING;
				preTime[i] = CTime::GetTickCount();
			}
		}
	}

	if(! alarm.IsEmpty())
	{
		cstr.Format(_T("(%d) %s\n"), i + 1, m_Ata.vars[i].Model.GetString());
		if(niifType == NIIF_WARNING)
		{
			ShowBalloon(m_MainIconId, niifType, i18n(_T("Alarm"), _T("ALARM_HEALTH_STATUS")), cstr + alarm);
		}
		else
		{
			ShowBalloon(m_MainIconId, niifType, i18n(_T("Alarm"), _T("INFO_HEALTH_STATUS")), cstr + alarm);
		}
	}
}

BOOL CDiskInfoDlg::AddEventLog(DWORD eventId, WORD eventType, CString message)
{
	if(! m_bEventLog)
	{
		return FALSE;
	}

	return WriteEventLog(eventId, eventType, _T("CrystalDiskInfo"), message);
}

BOOL CDiskInfoDlg::SendMail(DWORD eventId, CString title, CString message)
{
	if(! m_bAlertMail || m_AlertMailPath.IsEmpty())
	{
		return FALSE;
	}

	TCHAR computer[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD length = MAX_COMPUTERNAME_LENGTH + 1;
	GetComputerName(computer, &length);

	CString subject, body, option;

	subject.Format(_T("[CDI] %s %s [%d]"), computer, title.GetString(), eventId);
	body.Format(_T("%s %s\r\n[%d] %s"), computer, title.GetString(), eventId, message.GetString());

	option.Format(_T("Subject=\"%s\" Body=\"%s\""), subject.GetString(), body.GetString());
	if((INT_PTR)(ShellExecute(NULL, NULL, m_AlertMailPath, option, NULL, SW_SHOW)) > 32)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CDiskInfoDlg::AddAlarmHistory(DWORD eventId, CString disk, CString message)
{
	CString cstr;
	cstr.Format(L"[Alarm] EventID=%d, Disk=%s, Message=%s", eventId, disk.GetString(), message.GetString());
	cstr.Replace(L"\n", L"");
	DebugPrint(cstr);
	return FALSE;
// 2012/5/26 - 
/*
	CTime time = CTime::GetTickCount();

	CString line;
	line.Format(_T("%s,%d,%s,%s"), time.Format(_T("%Y/%m/%d %H:%M:%S")), eventId, disk, message);
	line.TrimRight();
	line += _T("\n");

	CStdioFile outFile;
	if(outFile.Open(m_SmartDir + ALARM_HISTORY_CSV,
		CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::typeText))
	{
		outFile.SeekToEnd();
		outFile.WriteString(line);
		outFile.Close();
		return TRUE;
	}

	return FALSE;
*/
}

int ExecAndWait(TCHAR *pszCmd, BOOL bNoWindow)
{
	DWORD Code;
	BOOL bSuccess;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if (bNoWindow) {
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
	}
	bSuccess = CreateProcess(
		NULL,	// lpApplicationName
		pszCmd,	// lpCommandLine
		NULL,	// lpProcessAttributes
		NULL,	// lpThreadAttributes
		FALSE,	// bInheritHandle
		0,		// dwCreationFlag
		NULL,	// lpEnvironment
		NULL,	// lpCurrentDirectory
		&si,	// lpStartupInfo
		&pi		// lpProcessInformation
	);
	if (bSuccess != TRUE) return 0;

	WaitForInputIdle(pi.hProcess, INFINITE);
	WaitForSingleObject(pi.hProcess, INFINITE);

	GetExitCodeProcess(pi.hProcess, &Code);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	return Code;
}


BOOL CDiskInfoDlg::AlertSound(DWORD eventId, DWORD mode)
{
	if(mode != AS_SET_SOUND_ID && eventId != 1000 && ! m_bAlertSound)
	{
		return FALSE;
	}

	static DWORD soundId = 0;

	if(mode == AS_SET_SOUND_ID)
	{
		if(eventId == 0 || soundId == 0 || soundId > eventId)
		{
			soundId = eventId;
		}
		return TRUE;
	}
	if(soundId == 0)
	{
		return TRUE;
	}

#ifdef SUISHO_SHIZUKU_SUPPORT
	if (600 <= soundId && soundId < 800)
	{
		m_bStartupVoiceDisabled = TRUE;
	}
#endif

	if (600 <= soundId && soundId < 800 && IsFileExist(m_AlertSoundPath))
	{
		if (mode == AS_DEINIT)
		{
			return AlertSound(_T(""));
		}

		// mode == AS_PLAY_SOUND
		// Play
		if (AlertSound(m_AlertSoundPath) == FALSE)
		{
			return FALSE;
		}
	}
	else
	{
		// mode == AS_PLAY_SOUND

		HRSRC hrs;
		// Choose OPUS resource
		CString resource;
		HMODULE hModule = nullptr;

#ifdef SUISHO_SHIZUKU_SUPPORT
	#ifdef SUISHO_AOI_SUPPORT
			if (m_hVoice != nullptr)
			{
				hModule = m_hVoice;
				resource.Format(_T("CDI_VOICE_%03d"), soundId);
			}
			else
			{
				resource = _T("CDI_SOUND_001");
			}		
	#else
			// For Japanese
			if (m_CurrentLang.Find(_T("Japanese")) == 0 || GetUserDefaultLCID() == 0x0411)
			{
				if (m_hVoice != nullptr)
				{
					hModule = m_hVoice;
				}
				else
				{
					return FALSE;
				}
				resource.Format(_T("CDI_VOICE_%03d"), soundId);
			}
			else
			{
				resource = _T("CDI_SOUND_001");
			}
	#endif
#else
		resource = _T("CDI_SOUND_001");
#endif
		hrs = FindResource(hModule, resource, _T("OPUS"));
		// Resource to TempFile
		if (hrs == nullptr)
		{
			return FALSE;
		}
		const HGLOBAL hOpus = LoadResource(hModule, hrs);
		if (!hOpus)
		{
			return FALSE;
		}
		const LPBYTE lpOpus = static_cast<LPBYTE>(LockResource(hOpus));
		if (!lpOpus)
		{
			return FALSE;
		}
		DWORD dwWrite = 0;

		const HANDLE hFile = CreateFile(m_TempFilePathOpus, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, nullptr);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			if (WriteFile(hFile, lpOpus, SizeofResource(hModule, hrs), &dwWrite, nullptr) == 0)
			{
				CloseHandle(hFile);
				return FALSE;
			}
			CloseHandle(hFile);
		}

		// Play
		if (AlertSound(m_TempFilePathOpus) == FALSE)
		{
			return FALSE;
		}
	}
	soundId = 0;

	return TRUE;
}

BOOL CDiskInfoDlg::AlertSound(const CString& alertSoundPath) const
{
	static MCI_OPEN_PARMS mop = { 0 };
	static MCI_PLAY_PARMS mpp = { 0 };
	static MCI_GENERIC_PARMS mgp = { 0 };
	MCIERROR error = 0;

	if (mop.wDeviceID != 0)
	{
		// Stop and close
		error = mciSendCommandW(mop.wDeviceID, MCI_STOP, MCI_WAIT, (DWORD_PTR)&mop);
		error = mciSendCommandW(mop.wDeviceID, MCI_CLOSE, MCI_WAIT, (DWORD_PTR)&mop);
		mop.wDeviceID = 0;
		ZeroMemory(&mop, sizeof(MCI_OPEN_PARMS));
		if (error)
		{
			return FALSE;
		}
	}

	if (alertSoundPath.Right(5).MakeUpper().Compare(_T(".OPUS")) == 0)
	{
		// Convert Opus to WAV
		CString option;
		option.Format(_T("\"%s\" \"%s\" \"%s\""), m_OpusDecPath.GetString(), alertSoundPath.GetString(), m_TempFilePathWave.GetString());
		ExecAndWait(const_cast<TCHAR*>(option.GetString()), TRUE);

		return AlertSound(m_TempFilePathWave);
	}

	if (alertSoundPath.Compare(_T("")) == 0) {
		// mode == AS_DEINIT
		// Close
		error = mciSendCommandW(mop.wDeviceID, MCI_CLOSE, 0, reinterpret_cast<DWORD_PTR>(&mop));
		if (error)
		{
			return FALSE;
		}
		return TRUE;
	}

	// Open
	mop.lpstrDeviceType = _T("MPEGVideo");
	mop.lpstrElementName = alertSoundPath;
	error = mciSendCommandW(NULL, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, reinterpret_cast<DWORD_PTR>(&mop));
	if (error)
	{
		return FALSE;
	}

	// Set volume
	int volume = GetPrivateProfileInt(_T("Setting"), _T("AlertSoundVolume"), 80, m_Ini);
	if (volume < 0 || volume > 100) volume = 80;

	MCI_DGV_SETAUDIO_PARMS parms = { 0 };
	parms.dwItem = MCI_DGV_SETAUDIO_VOLUME;
	parms.dwValue = volume * 10; // 0-1000
	error = mciSendCommand(mop.wDeviceID, MCI_SETAUDIO, MCI_DGV_SETAUDIO_ITEM | MCI_DGV_SETAUDIO_VALUE, (DWORD_PTR)&parms);

	// Seek
	error = mciSendCommand(mop.wDeviceID, MCI_SEEK, MCI_SEEK_TO_START, reinterpret_cast<DWORD_PTR>(&mgp));
	if (error)
	{
		return FALSE;
	}

	// Play
	error = mciSendCommandW(mop.wDeviceID, MCI_PLAY, 0, reinterpret_cast<DWORD_PTR>(&mpp));
	if (error)
	{
		return FALSE;
	}
	
	return TRUE;
}

void CDiskInfoDlg::AlarmOverheat()
{
	CString cstr;
	CString overheat;
	CString diskStatus;
	for(int i = 0; i < m_Ata.vars.GetCount(); i++)
	{
		if(m_Ata.vars[i].AlarmTemperature > 0 && m_Ata.vars[i].Temperature >= m_Ata.vars[i].AlarmTemperature)
		{
			diskStatus = GetDiskStatus(m_Ata.vars[i].DiskStatus);
			
			if(m_bFahrenheit)
			{
				cstr.Format(_T("(%d) %s [%s] %d F\r\n"), i + 1, m_Ata.vars[i].Model.GetString(), diskStatus.GetString(), m_Ata.vars[i].Temperature * 9 / 5 + 32);
			}
			else
			{
				cstr.Format(_T("(%d) %s [%s] %d C\r\n"), i + 1, m_Ata.vars[i].Model.GetString(), diskStatus.GetString(), m_Ata.vars[i].Temperature);
			}
			
			overheat += cstr;
		}
	}
	overheat.Trim();
	if(! overheat.IsEmpty())
	{
		ShowBalloon(m_MainIconId, NIIF_WARNING, i18n(_T("Alarm"), _T("ALARM_TEMPERATURE")), overheat);
	}
}

void CDiskInfoDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == TIMER_SET_POWER_ON_UNIT)
	{
		KillTimer(TIMER_SET_POWER_ON_UNIT);

		SetWindowTitle(_T(""));
		m_NowDetectingUnitPowerOnHours = FALSE;
		if(m_Ata.MeasuredTimeUnit())
		{
			for(int i = 0; i < m_Ata.vars.GetCount(); i++)
			{
				m_Ata.vars[i].MeasuredPowerOnHours = m_Ata.GetPowerOnHoursEx(i, m_Ata.vars[i].MeasuredTimeUnitType);
				
				CString cstr;
				cstr.Format(_T("%d"), m_Ata.vars[i].MeasuredTimeUnitType);
				WritePrivateProfileStringFx(_T("PowerOnUnit"), m_Ata.vars[i].ModelSerial, cstr, m_Ini);
				SaveSmartInfo(i);
			}

			AlertSound(0, AS_PLAY_SOUND);

			if(m_SelectDisk < (DWORD)m_Ata.vars.GetCount() && m_Ata.vars[m_SelectDisk].IsSmartCorrect && m_Ata.vars[m_SelectDisk].MeasuredPowerOnHours > 0)
			{
				CString IsMinutesT, title;
				if(m_Ata.vars[m_SelectDisk].MeasuredTimeUnitType == CAtaSmart::POWER_ON_MINUTES && m_Ata.vars[m_SelectDisk].IsMaxtorMinute)
				{
					IsMinutesT = _T(" (?)");
				}
				else
				{
					IsMinutesT = _T("");
				}

				const int years = m_Ata.vars[m_SelectDisk].MeasuredPowerOnHours / (365 * 24);
				const int days = (m_Ata.vars[m_SelectDisk].MeasuredPowerOnHours - 365 * 24 * years) / 24;
				const int hours = m_Ata.vars[m_SelectDisk].MeasuredPowerOnHours % 24;
				if (years > 0)
				{
					title.Format(_T("%d %s %d %s %d %s%s"),
						years, i18n(_T("Dialog"), _T("POWER_ON_YEARS_UNIT")).GetString(),
						days, i18n(_T("Dialog"), _T("POWER_ON_DAYS_UNIT")).GetString(),
						hours, i18n(_T("Dialog"), _T("POWER_ON_HOURS_UNIT")).GetString(),
						IsMinutesT.GetString());
				}
				else
				{
					title.Format(_T("%d %s %d %s%s"),
						days, i18n(_T("Dialog"), _T("POWER_ON_DAYS_UNIT")).GetString(),
						hours, i18n(_T("Dialog"), _T("POWER_ON_HOURS_UNIT")).GetString(),
						IsMinutesT.GetString());
				}
				m_CtrlPowerOnHours.SetToolTipText(title + L"  ");
			}
			else
			{
				m_CtrlPowerOnHours.SetToolTipText(L"");
			}
		}
	}
	else if(nIDEvent == TIMER_AUTO_REFRESH)
	{
		Refresh(FALSE);
	}
	else if(nIDEvent == TIMER_FORCE_REFRESH)
	{
		KillTimer(TIMER_FORCE_REFRESH);
		Refresh(FALSE);

		AutoAamApmAdaption();
	}
	else if(nIDEvent == TIMER_AUTO_DETECT)
	{
		CWaitCursor wait;
		BOOL flagChangeDisk = FALSE;
		KillTimer(TIMER_AUTO_DETECT);

		InitAta(TRUE, m_bAdvancedDiskSearch, &flagChangeDisk, m_bWorkaroundHD204UI, m_bWorkaroundAdataSsd);

		if(flagChangeDisk)
		{
			// Update Menu and Dialog
			m_SelectDisk = 0;
			m_DriveMenuPage = 0;
			ChangeLang(m_CurrentLang);
			if(m_bResident)
			{
				for(int i = 0; i < CAtaSmart::MAX_DISK; i++)
				{
					RemoveTemperatureIcon(i);
				}
				CheckTrayTemperatureIcon();
			}

			if(m_SettingDlg != NULL)
			{
				::SendMessage(m_SettingDlg->m_hWnd, WM_CLOSE, 0, 0);
			}
			if(m_HealthDlg != NULL)
			{
				::SendMessage(m_HealthDlg->m_hWnd, WM_CLOSE, 0, 0);
			}
		}
		else
		{
			Refresh(TRUE);
		}
	}
	else if(nIDEvent == TIMER_UPDATE_TRAY_ICON)
	{
		KillTimer(TIMER_UPDATE_TRAY_ICON);
		for(int i = 0; i < CAtaSmart::MAX_DISK; i++)
		{
			RemoveTemperatureIcon(i);
		}
		CheckTrayTemperatureIcon();
		ShowWindow(SW_SHOW);
	}

	CMainDialogFx::OnTimer(nIDEvent);
}

LRESULT CDiskInfoDlg::OnPowerBroadcast(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case PBT_APMRESUMESUSPEND:
		SetTimer(TIMER_FORCE_REFRESH, 5 * 1000, 0);
//		MessageBox(_T("PBT_APMRESUMESUSPEND"));
		break;
//	case PBT_APMSUSPEND:
//		MessageBox(_T("PBT_APMSUSPEND"));
//		break;
	default:
		break;
	}

	return TRUE;
}

LRESULT CDiskInfoDlg::OnQueryEndSession(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

LRESULT CDiskInfoDlg::OnDeviceChange(WPARAM wParam, LPARAM lParam)
{
	if(m_AutoDetectionStatus <= 0)
	{
		return FALSE;
	}

	//CString cstr;
	switch(wParam)
	{
	case DBT_DEVICEARRIVAL:
		{
			//cstr.Format(_T("DBT_DEVICEARRIVAL: LPARAM=%08X\n"), lParam);
			PDEV_BROADCAST_HDR pdbh = (PDEV_BROADCAST_HDR)lParam;
			switch(pdbh->dbch_devicetype)
			{
			case DBT_DEVTYP_DEVICEINTERFACE:
				{
					//cstr += _T("DBT_DEVTYP_DEVICEINTERFACE");
					PDEV_BROADCAST_DEVICEINTERFACE pdbd = (PDEV_BROADCAST_DEVICEINTERFACE)lParam;
					if(pdbd->dbcc_classguid == StrageGUID)
					{
					//	AfxMessageBox(pdbd->dbcc_name);
					
					// Disabled 2008/11/20 (This feature will be enabled on future release...)
						KillTimer(TIMER_AUTO_DETECT);
						SetTimer(TIMER_AUTO_DETECT, m_AutoDetectionStatus * 1000, 0);
					}
				}
				break;
			case DBT_DEVTYP_HANDLE:
				// cstr += _T("DBT_DEVTYP_HANDLE");
				break;
			case DBT_DEVTYP_OEM:
				// cstr += _T("DBT_DEVTYP_OEM");
				break;
			case DBT_DEVTYP_PORT:
				// cstr += _T("DBT_DEVTYP_PORT");
				break;
			case DBT_DEVTYP_VOLUME:
				// cstr += _T("DBT_DEVTYP_VOLUME\n");
				// PDEV_BROADCAST_VOLUME pdbv = (PDEV_BROADCAST_VOLUME)lParam;
				// CString temp;
				// temp.Format(_T("Flags=%d, UnitMask=%08X"), pdbv->dbcv_flags, pdbv->dbcv_unitmask);
				// cstr += temp;
				break;
			}
		//	AfxMessageBox(cstr);
		}
		break;
	case DBT_DEVICEREMOVECOMPLETE:
		{
			// cstr.Format(_T("DBT_DEVICEREMOVECOMPLETE: LPARAM=%08X\n"), lParam);
			PDEV_BROADCAST_HDR pdbh = (PDEV_BROADCAST_HDR)lParam;
			switch(pdbh->dbch_devicetype)
			{
			case DBT_DEVTYP_DEVICEINTERFACE:
				{
				//	cstr += _T("DBT_DEVTYP_DEVICEINTERFACE");
					PDEV_BROADCAST_DEVICEINTERFACE pdbd = (PDEV_BROADCAST_DEVICEINTERFACE)lParam;
					if(pdbd->dbcc_classguid == StrageGUID)
					{
					//	AfxMessageBox(pdbd->dbcc_name);
					// Disabled 2008/11/20 (This feature will be enabled on future release...)
						KillTimer(TIMER_AUTO_DETECT);
						SetTimer(TIMER_AUTO_DETECT, m_AutoDetectionStatus * 1000, 0);
					}
				}
				break;
			case DBT_DEVTYP_HANDLE:
			//	cstr += _T("DBT_DEVTYP_HANDLE");
				break;
			case DBT_DEVTYP_OEM:
			//	cstr += _T("DBT_DEVTYP_OEM");
				break;
			case DBT_DEVTYP_PORT:
			//	cstr += _T("DBT_DEVTYP_PORT");
				break;
			case DBT_DEVTYP_VOLUME:
			//	cstr += _T("DBT_DEVTYP_VOLUME\n");
			//	PDEV_BROADCAST_VOLUME pdbv = (PDEV_BROADCAST_VOLUME)lParam;
			//	CString temp;
			//	temp.Format(_T("Flags=%d, UnitMask=%08X"), pdbv->dbcv_flags, pdbv->dbcv_unitmask);
			//	cstr += temp;
				break;
			}
		//	AfxMessageBox(cstr);
		}
		break;
	default:
	//	cstr.Format(_T("WPARAM=%08X, LPARAM=%08X"), wParam, lParam);
	//	AfxMessageBox(cstr);
		break;
	}
	return TRUE;
}

void CDiskInfoDlg::AutoAamApmAdaption()
{
	if(! m_bAutoAamApm)
	{
		return ;
	}

	int status = 0;
	int value = 0;

	//DEBUG
	DebugPrint(_T("AutoAamApmAdaption"));
	// AAM
	for(int i = 0; i < m_Ata.vars.GetCount(); i++)
	{
		if(! m_Ata.vars[i].IsAamSupported)
		{
			continue;
		}

		status = GetPrivateProfileIntFx(_T("AamStatus"), m_Ata.vars[i].ModelSerial, -1, m_Ini);
		value =  GetPrivateProfileIntFx(_T("AamValue"), m_Ata.vars[i].ModelSerial, -1, m_Ini);
		if(status == 1 /* Enabled */ && value != -1)
		{
			m_Ata.EnableAam(i, (BYTE)value);
			m_Ata.UpdateIdInfo(i);
			DebugPrint(_T("m_Ata.EnableAam"));
		}
		else if(status == 0 /* Disabled */ )
		{
			m_Ata.DisableAam(i);
			m_Ata.UpdateIdInfo(i);
			DebugPrint(_T("m_Ata.DisableAam"));
		}
	}

	// APM
	for(int i = 0; i < m_Ata.vars.GetCount(); i++)
	{
		if(! m_Ata.vars[i].IsApmSupported)
		{
			continue;
		}

		status = GetPrivateProfileIntFx(_T("ApmStatus"), m_Ata.vars[i].ModelSerial, -1, m_Ini);
		value  = GetPrivateProfileIntFx(_T("ApmValue"), m_Ata.vars[i].ModelSerial, -1, m_Ini);
		if(status == 1 /* Enabled */ && value != -1)
		{
			m_Ata.EnableApm(i, (BYTE)value);
			m_Ata.UpdateIdInfo(i);
			DebugPrint(_T("m_Ata.EnableApm"));
		}
		else if(status == 0 /* Disabled */ )
		{
			m_Ata.DisableApm(i);
			m_Ata.UpdateIdInfo(i);
			DebugPrint(_T("m_Ata.DisableApm"));
		}
	}
}

void CDiskInfoDlg::ReExecute()
{
	// Added 2013/04/12 - Workaround for Exec Failed
	WritePrivateProfileStringFx(_T("Workaround"), _T("ExecFailed"), _T("0"), m_Ini);

	ShowWindow(SW_HIDE);
	RemoveTrayMainIcon();
	for(int i = 0; i < m_Ata.vars.GetCount(); i++)
	{
		RemoveTemperatureIcon(i);
	}
	KillGraphDlg();
	EndDialog(RE_EXEC);
}

DWORD CDiskInfoDlg::GetSelectDisk()
{
	return m_SelectDisk;
}

void CDiskInfoDlg::OnBnClickedButtonDisk0(){SelectDrive(0 + m_DriveMenuPage * m_DriveMenuNumber);}
void CDiskInfoDlg::OnBnClickedButtonDisk1(){SelectDrive(1 + m_DriveMenuPage * m_DriveMenuNumber);}
void CDiskInfoDlg::OnBnClickedButtonDisk2(){SelectDrive(2 + m_DriveMenuPage * m_DriveMenuNumber);}
void CDiskInfoDlg::OnBnClickedButtonDisk3(){SelectDrive(3 + m_DriveMenuPage * m_DriveMenuNumber);}
void CDiskInfoDlg::OnBnClickedButtonDisk4(){SelectDrive(4 + m_DriveMenuPage * m_DriveMenuNumber);}
void CDiskInfoDlg::OnBnClickedButtonDisk5(){SelectDrive(5 + m_DriveMenuPage * m_DriveMenuNumber);}
void CDiskInfoDlg::OnBnClickedButtonDisk6(){SelectDrive(6 + m_DriveMenuPage * m_DriveMenuNumber);}
void CDiskInfoDlg::OnBnClickedButtonDisk7(){SelectDrive(7 + m_DriveMenuPage * m_DriveMenuNumber);}
void CDiskInfoDlg::OnBnClickedButtonDisk8() { SelectDrive(8 + m_DriveMenuPage * m_DriveMenuNumber); }
void CDiskInfoDlg::OnBnClickedButtonDisk9() { SelectDrive(9 + m_DriveMenuPage * m_DriveMenuNumber); }
void CDiskInfoDlg::OnBnClickedButtonDisk10() { SelectDrive(10 + m_DriveMenuPage * m_DriveMenuNumber); }
void CDiskInfoDlg::OnBnClickedButtonDisk11() { SelectDrive(11 + m_DriveMenuPage * m_DriveMenuNumber); }
void CDiskInfoDlg::OnBnClickedButtonDisk12() { SelectDrive(12 + m_DriveMenuPage * m_DriveMenuNumber); }
void CDiskInfoDlg::OnBnClickedButtonDisk13() { SelectDrive(13 + m_DriveMenuPage * m_DriveMenuNumber); }
void CDiskInfoDlg::OnBnClickedButtonDisk14() { SelectDrive(14 + m_DriveMenuPage * m_DriveMenuNumber); }
void CDiskInfoDlg::OnBnClickedButtonDisk15() { SelectDrive(15 + m_DriveMenuPage * m_DriveMenuNumber); }

#ifdef MAX_DRIVE_20
void CDiskInfoDlg::OnBnClickedButtonDisk16() { SelectDrive(16 + m_DriveMenuPage * m_DriveMenuNumber); }
void CDiskInfoDlg::OnBnClickedButtonDisk17() { SelectDrive(17 + m_DriveMenuPage * m_DriveMenuNumber); }
void CDiskInfoDlg::OnBnClickedButtonDisk18() { SelectDrive(18 + m_DriveMenuPage * m_DriveMenuNumber); }
void CDiskInfoDlg::OnBnClickedButtonDisk19() { SelectDrive(19 + m_DriveMenuPage * m_DriveMenuNumber); }
#endif

void CDiskInfoDlg::SetControlFont()
{
#ifdef SUISHO_SHIZUKU_SUPPORT
	//BYTE textAlpha = TEXT_ALPHA;
//	COLORREF textColor = RGB(77, 77, 77);
	//COLORREF textColor = m_LabelText;
#else
	///BYTE textAlpha = 255;
	//COLORREF textColor = m_LabelText;
#endif

	m_List.SetFontEx(m_FontFace, 12, m_ZoomRatio, m_FontRatio, FW_NORMAL, m_FontRender);

	for(int i = 0; i < MAX_DRIVE_MENU; i++)
	{
		m_ButtonDisk[i].SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	}

	if (m_bHighContrast)
	{
		m_CtrlModel.SetFontEx(m_FontFace, 20, 20, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	}
	else
	{
		m_CtrlModel.SetFontEx(m_FontFace, 24, 24, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	}

	m_CtrlButtonPreDisk.SetFontEx(m_FontFace, 24, 24, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlButtonNextDisk.SetFontEx(m_FontFace, 24, 24, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	
	m_CtrlLabelFirmware.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio,m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlLabelSerialNumber.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlLabelInterface.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlLabelTransferMode.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlLabelDriveMap.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlLabelBufferSize.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlLabelNvCacheSize.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlLabelRotationRate.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlLabelPowerOnCount.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlLabelPowerOnHours.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlLabelAtaAtapi.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlLabelFeature.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlLabelDiskStatus.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlLabelTemperature.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);

	m_CtrlFirmware.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlSerialNumber.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlInterface.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlTransferMode.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlDriveMap.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlBufferSize.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlNvCacheSize.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlRotationRate.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlPowerOnCount.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlPowerOnHours.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlAtaAtapi.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);
	m_CtrlFeature.SetFontEx(m_FontFace, 12, 12, m_ZoomRatio, m_FontRatio, m_LabelText, FW_NORMAL, m_FontRender);

#ifdef SUISHO_SHIZUKU_SUPPORT
	m_CtrlDiskStatus.SetFontEx(m_FontFace, 16, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD, m_FontRender);
	m_CtrlTemperature.SetFontEx(m_FontFace, 18, 18, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD, m_FontRender);
	m_CtrlLife.SetFontEx(m_FontFace, 18, 18, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_NORMAL, m_FontRender);
#else
	m_CtrlDiskStatus.SetFontEx(m_FontFace, 18, 18, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD, m_FontRender);
	m_CtrlTemperature.SetFontEx(m_FontFace, 20, 20, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD, m_FontRender);
#endif
}

void CDiskInfoDlg::OnBnClickedButtonPreDisk()
{
	SelectDrive(m_SelectDisk - 1);
}

void CDiskInfoDlg::OnBnClickedButtonNextDisk()
{
	SelectDrive(m_SelectDisk + 1);
}

void CDiskInfoDlg::OnBnClickedButtonHealthStatus()
{
	CMenu *menu = GetMenu();
	if(! (menu->GetMenuState(ID_HEALTH_STATUS, MF_BYCOMMAND) & MF_GRAYED))
	{
		m_HealthDlg = new CHealthDlg(this);
		m_HealthDlg->Create(CHealthDlg::IDD, m_HealthDlg, ID_HEALTH_STATUS, this);
	}
}

void CDiskInfoDlg::OnBnClickedButtonTemperature()
{
	CMenu* menu = GetMenu();
	if (!(menu->GetMenuState(ID_TEMPERATURE, MF_BYCOMMAND) & MF_GRAYED))
	{
		m_TemperatureDlg = new CTemperatureDlg(this);
		m_TemperatureDlg->Create(CTemperatureDlg::IDD, m_TemperatureDlg, ID_TEMPERATURE, this);
	}
}

void CDiskInfoDlg::OnBnClickedButtonVoice()
{
#ifdef SUISHO_SHIZUKU_SUPPORT
	DWORD id;
	if(GetTickCountFx() % 2)
	{
		id = 7;
	}
	else
	{
		id = 8;
	}
	AlertSound(id, AS_SET_SOUND_ID);
	AlertSound(1000, AS_PLAY_SOUND);
#endif
}


void CDiskInfoDlg::OnBnClickedButtonCopyright()
{
#ifdef SUISHO_SHIZUKU_SUPPORT
	/*
	if (m_CurrentLang.Find(L"Japanese") == 0)
	{
		SYSTEMTIME systime;
		GetLocalTime(&systime);
		if ((systime.wYear == 2015 && systime.wMonth == 12 && systime.wDay <= 17)
			|| (systime.wYear == 2015 && systime.wMonth == 12 && systime.wDay == 18 && systime.wHour < 20))
		{
			OpenUrl(L"http://akiba-pc.watch.impress.co.jp/docs/sp/20151207_733688.html");
		}
	}
	*/
	UINT themeIndex = rand() % (UINT)m_MenuArrayTheme.GetSize();
	SendMessage(WM_COMMAND, WM_THEME_ID + themeIndex);
#endif
}


void CDiskInfoDlg::OnBnClickedButtonLife()
{
#ifdef SUISHO_SHIZUKU_SUPPORT
	DWORD id;
	if (m_Ata.vars.GetCount() > 0)
	{
		switch(m_Ata.vars[m_SelectDisk].DiskStatus)
		{
		case  CAtaSmart::DISK_STATUS_GOOD:
			if(m_Ata.vars[m_SelectDisk].Life == 100)
			{
				id = 2;
			}
			else
			{
				id = 3;
			}
			break;
		case  CAtaSmart::DISK_STATUS_CAUTION:
			id = 4;
			break;
		case CAtaSmart::DISK_STATUS_BAD:
			id = 5;
			break;
		case CAtaSmart::DISK_STATUS_UNKNOWN:
		default:
			id = 6;
			break;
		}
	}
	else
	{
		id = 6;
	}

	AlertSound(id, AS_SET_SOUND_ID);
	AlertSound(1000, AS_PLAY_SOUND);
#endif
}

void CDiskInfoDlg::SetLabel(CStaticFx& ctrl, CString& label, CString title)
{
	ctrl.SetToolTipText(title);
	label = title;
}

void CDiskInfoDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CMainDialogFx::OnShowWindow(bShow, nStatus);
}

void CDiskInfoDlg::ShowWindowEx(int nCmdShow)
{
	if (nCmdShow != SW_HIDE)
	{
		m_bShowWindow = TRUE;
		ShowWindow(nCmdShow);
		SetForegroundWindow();
	}
	else
	{
		ShowWindow(nCmdShow);
		m_bShowWindow = FALSE;
	}
}

BOOL CDiskInfoDlg::CheckThemeEdition(CString name)
{
#ifdef SUISHO_AOI_SUPPORT
	if (name.Find(L"Aoi") == 0) { return TRUE; }
#elif MSI_MEI_SUPPORT
	if (name.Find(L"MSIMei") == 0) { return TRUE; }
#elif KUREI_KEI_SUPPORT
	if (name.Find(L"KureiKei") == 0) { return TRUE; }
#elif SUISHO_SHIZUKU_SUPPORT
	if (name.Find(L"Shizuku") == 0) { return TRUE; }
#else
	if (name.Find(L"Shizuku") != 0 && name.Find(L"KureiKei") != 0 && name.Find(L"Aoi") != 0 && name.Find(L"MSIMei") != 0 && name.Find(L".") != 0) { return TRUE; }
#endif

	return FALSE;
}
