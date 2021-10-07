/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/
#pragma once

#include "AtaSmart.h"
#include "AboutDlg.h"
#include "SettingDlg.h"
#include "HealthDlg.h"
#include "OptionDlg.h"
#include "FontSelectionDlg.h"
#include "SoundSettingDlg.h"
#include "GraphDlg.h"
#include "TemperatureDlg.h"
#include "EventLog.h"

#include "DialogFx.h"
#include "MainDialogFx.h"
#include "EditFx.h"
#include "ListCtrlFx.h"
#include "OsInfoFx.h"

#include <Dbt.h>

#define REGISTRY_PATH _T("software\\Crystal Dew World\\CrystalDiskInfo")

// Task Tray
enum
{
	MY_EXIT = (WM_APP + 0x1100),
	MY_SHOW_MAIN_DIALOG,
	MY_SHOW_TEMPERATURE_ICON_ONLY,
	MY_PLAY_ALERT_SOUND,
};

// CDiskInfoDlg dialog
class CDiskInfoDlg : public CMainDialogFx
{
// Construction
public:
	CDiskInfoDlg(CWnd* pParent /*=NULL*/, BOOL flagStarupExit);
	virtual ~CDiskInfoDlg();

	CAtaSmart m_Ata;

// Dialog Data
	enum { IDD = IDD_DISKINFO_DIALOG };

#ifdef SUISHO_SHIZUKU_SUPPORT
	static const int SIZE_X = 1000;
	static const int SIZE_SMART_X = 1000;
	static const int SIZE_SMART_Y = 640;
	static const int SIZE_MIN_Y = 500;
	static const int SIZE_Y = 288;
	static const int SIZE_MAX_Y = 1000;
	static const int OFFSET_X = 328;
	static const int IMAGE_ALPHA = 192;
	static const int TEXT_ALPHA = 255;
	static const int LIST_CTL_ALPHA = 192;
#else
	static const int SIZE_X = 672;
	static const int SIZE_SMART_X = 672;
	static const int SIZE_SMART_Y = 640;
	static const int SIZE_MIN_Y = 260;
	static const int SIZE_Y = 260;
	static const int SIZE_MAX_Y = 1000;
	static const int OFFSET_X = 0;
#endif

public:

#ifdef SUISHO_SHIZUKU_SUPPORT
	HMODULE m_hVoice;
#endif

	// Timer
	static const int TIMER_SET_POWER_ON_UNIT = 0x2001;
	static const int TIMER_AUTO_REFRESH      = 0x2002;
	static const int TIMER_FORCE_REFRESH     = 0x2003;
	static const int TIMER_AUTO_DETECT       = 0x2004;
	static const int TIMER_UPDATE_TRAY_ICON  = 0x2005;

	// Setting
	int SAVE_SMART_PERIOD;			// sec
	int ALARM_TEMPERATURE_PERIOD;	// sec

	// Icon
	enum
	{
		ICON_GOOD = 0,
		ICON_GOOD_GREEN,
		ICON_CAUTION,
		ICON_BAD,
		ICON_UNKNOWN
	};

	HMENU m_hMenu;
	void CreateMainMenu(DWORD index);

	DWORD GetSelectDisk();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;
	HICON m_hIconMini;
	HICON m_hTempIcon[2][300];
	UINT m_TempIconIndex[CAtaSmart::MAX_DISK];

	UINT m_MainIconId;

	CAboutDlg*			m_AboutDlg;
	CSettingDlg*		m_SettingDlg;
	CTemperatureDlg*	m_TemperatureDlg;
	CHealthDlg*			m_HealthDlg;
	COptionDlg*			m_OptionDlg;
//	CAlarmHistoryDlg*	m_AlarmHistoryDlg;
	CSoundSettingDlg*	m_SoundSettingDlg;
	CGraphDlg*			m_GraphDlg;
	CListCtrlFx			m_List;
	CImageList			m_ImageList;

	CButtonFx			m_ButtonDisk[8];

	CButtonFx			m_CtrlButtonPreDisk;
	CButtonFx			m_CtrlButtonNextDisk;

	CStaticFx			m_CtrlLabelFirmware;
	CStaticFx			m_CtrlLabelSerialNumber;
	CStaticFx			m_CtrlLabelInterface;
	CStaticFx			m_CtrlLabelTransferMode;
	CStaticFx			m_CtrlLabelDriveMap;
	CStaticFx			m_CtrlLabelBufferSize;
	CStaticFx			m_CtrlLabelNvCacheSize;
	CStaticFx			m_CtrlLabelRotationRate;
	CStaticFx			m_CtrlLabelPowerOnCount;
	CStaticFx			m_CtrlLabelPowerOnHours;
	CStaticFx			m_CtrlLabelWAF;
	CStaticFx			m_CtrlLabelAtaAtapi;
	CStaticFx			m_CtrlLabelFeature;
	CStaticFx			m_CtrlLabelDiskStatus;
	CStaticFx			m_CtrlLabelTemperature;

	CEditFx				m_CtrlModel;
	CEditFx				m_CtrlFirmware;
	CEditFx				m_CtrlSerialNumber;
	CEditFx				m_CtrlInterface;
	CEditFx				m_CtrlTransferMode;
	CEditFx				m_CtrlDriveMap;
	CEditFx				m_CtrlBufferSize;
	CEditFx				m_CtrlNvCacheSize;
	CEditFx				m_CtrlRotationRate;
	CEditFx				m_CtrlWAF;
	CEditFx				m_CtrlPowerOnCount;
	CEditFx				m_CtrlPowerOnHours;
	CEditFx				m_CtrlAtaAtapi;
	CEditFx				m_CtrlFeature;
	CButtonFx			m_CtrlDiskStatus;
	CButtonFx			m_CtrlTemperature;
	CButtonFx			m_CtrlLife;
	CButtonFx			m_CtrlVoice;
	CButtonFx			m_CtrlVoiceHide;
	CButtonFx			m_CtrlCopyright;

	HDEVNOTIFY m_hDevNotify;

	CString		m_SmartDir;
	CString		m_GadgetDir;
	CString		m_ExeDir;
	CString		m_AlertMailPath;
	CString		m_OpusDecPath;
	CString		m_AlertSoundPath;

#ifdef SUISHO_SHIZUKU_SUPPORT
	CString		m_VoicePath;
#endif

	DWORD m_SelectDisk;
	DWORD m_DriveMenuPage;
	DWORD m_AutoRefreshStatus;
	DWORD m_WaitTimeStatus;
	DWORD m_AutoDetectionStatus;
	DWORD m_RawValues;
	BOOL m_NowDetectingUnitPowerOnHours;

	CArray<DWORD, DWORD> m_GraphProcessId;

	int m_SizeX;
	int m_SizeY;

	int m_PreTemp[CAtaSmart::MAX_DISK];
	BOOL m_bTrayTemperatureIcon[CAtaSmart::MAX_DISK];
	BOOL m_bAutoRefreshTarget[CAtaSmart::MAX_DISK];
	BOOL m_bTrayMainIcon;

	// Options
	BOOL m_bStartupExit;
	BOOL m_bHideSmartInfo;
	BOOL m_bHideSerialNumber;
	BOOL m_bAdvancedDiskSearch;
	BOOL m_bWorkaroundHD204UI;
	BOOL m_bWorkaroundIE8MODE;
	BOOL m_bWorkaroundAdataSsd;
	BOOL m_bWorkaroundIgnoreC4;
	BOOL m_bEventLog;
	BOOL m_bAlertMail;
	BOOL m_bAtaPassThroughSmart;
//	BOOL m_bUseEventCreate;		// Use eventcreate.exe (XP Pro or later)
	BOOL m_bFahrenheit;
	BOOL m_bAutoAamApm;
	BOOL m_bDumpIdentifyDevice;
	BOOL m_bDumpSmartReadData;
	BOOL m_bDumpSmartReadThreshold;
	BOOL m_bShowTemperatureIconOnly;
	BOOL m_bAsciiView;
	BOOL m_bSmartEnglish;
	BOOL m_bGadget;
	BOOL m_bGreenMode;
	BOOL m_bAlertSound;
	BOOL m_bHideNoSmartDisk;

	BOOL AddTemperatureIcon(DWORD index);
	BOOL RemoveTemperatureIcon(DWORD index);
	BOOL ModifyTemperatureIcon(DWORD index);

	CString m_LiDisk[8];

	CString m_Model;
	CString m_ModelCapacity;
	CString m_Firmware;
	CString m_SerialNumber;
	CString m_Capacity;
	CString m_Temperature;
	CString m_Life;
	CString m_PowerOnHours;
	CString m_PowerOnCount;
	CString m_WAF;
	CString m_Feature;
	CString m_BufferSize;
	CString m_NvCacheSize;
	CString m_RotationRate;
	CString m_LbaSize;
	CString m_DriveMap;
	CString m_Interface;
	CString m_TransferMode;
	CString m_AtaAtapi;
	CString m_DiskStatus;
	CString m_SmartStatus;

	CString m_LabelFirmware;
	CString m_LabelSerialNumber;
	CString m_LabelCapacity;
	CString m_LabelTemperature;
	CString m_LabelPowerOnHours;
	CString m_LabelPowerOnCount;
	CString m_LabelWAF;
	CString m_LabelFeature;
	CString m_LabelBufferSize;
	CString m_LabelNvCacheSize;
	CString m_LabelRotationRate;
	CString m_LabelLbaSize;
	CString m_LabelDriveMap;
	CString m_LabelInterface;
	CString m_LabelTransferMode;
	CString m_LabelAtaAtapi;
	CString m_LabelDiskStatus;
	CString m_StatusTip;

	BOOL ChangeDisk(DWORD index);
	BOOL UpdateListCtrl(DWORD index);
	void SelectDrive(DWORD index);
	void InitDriveList();
	void InitListCtrl();
	void InitAta(BOOL useWmi, BOOL advancedDiskSearch, PBOOL flagChangeDisk, BOOL workaroundHD204UI, BOOL workaroundAdataSsd);

	void ChangeLang(CString LangName);
	void UpdateDialogSize();
	void CheckHideSerialNumber();
	void CheckResident();
	void CheckStartup();
	void AutoAamApmAdaption();
	void ShowTemperatureIconOnly();

	void SetControlFont();
	virtual void SetClientSize(int sizeX, int sizeY, double zoomRatio);

	void SavePos() const;
	void RestorePos();
	
	void UpdateShareInfo(); // For Sidebar Gadget Support
	void DeleteShareInfo();

	BOOL AddTrayMainIcon();
	BOOL RemoveTrayMainIcon();

	CString GetDiskStatus(DWORD statusCode);
	CString GetDiskStatusClass(DWORD statusCode);
	CString GetDiskStatusReason(DWORD index);
	CString GetTemperatureClass(INT temperature, INT alarmTemperature);
	CString GetLogicalDriveInfo(DWORD index, INT maxLength = 65535);

	CString Encode10X(DWORD value);
	DWORD Decode10X(CString cstr);

	void CheckRadioAutoRefresh(int id, int value);
	void CheckRadioWaitTime(int id, int value);
	BOOL CheckRadioZoomType(int id, int value);
	void CheckRadioRawValues(int id, int value);
	void CheckRadioAutoDetection(int id, int value);
	void CheckRadioAutoRefresh();
	void CheckRadioWaitTime();
	void CheckRadioZoomType();
	void CheckRadioRawValues();
	void CheckRadioAutoDetection();
	void CheckRadioCsmiType(int id, int value);
	void CheckRadioCsmiType();
	void ReExecute();
	void AlarmOverheat();
	void AlarmHealthStatus(DWORD i, CString dir, CString disk);
	void CheckPage();
	void CheckTrayTemperatureIcon();
	void UpdateTrayTemperatureIcon(BOOL flagForce);
	void UpdateToolTip();
	BOOL IsTemperatureIconExist();

	void TaskTrayRightMenu(DWORD index);
	void SaveSmartInfo(DWORD index);
	void ShowGraphDlg(int index);
	void CreateExchangeInfo();
	void KillGraphDlg();
	void SetLabel(CStaticFx& ctrl, CString& label, CString title);

	BOOL RegisterStartup();
	BOOL UnregisterStartup();

	CString __Number(DWORD value);
	CHAR AsciiFilter(BYTE c);

	void ShowWindowEx(int nCmdShow);
	virtual BOOL CheckThemeEdition(CString name);

	// Generated message map functions
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
//	virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
	virtual void InitDialogComplete();

	void OnDisk0();
	void OnDisk1();
	void OnDisk2();
	void OnDisk3();
	void OnDisk4();
	void OnDisk5();
	void OnDisk6();
	void OnDisk7();
	void OnPreDisk();
	void OnNextDisk();
	void OnDiskStatus();

	LRESULT OnPowerBroadcast(WPARAM wParam, LPARAM lParam);
	LRESULT OnDeviceChange(WPARAM wParam, LPARAM lParam);
	LRESULT OnQueryEndSession(WPARAM wParam, LPARAM lParam);

	void Refresh(DWORD flagForceUpdate);
	BOOL AppendLog(CString dir, CString disk, CString file, CTime time, int value, BOOL firstTime = FALSE, int threshold = 0);
	BOOL AppendLog_WAF(CString dir, CString disk, CString file, CTime time, double value, BOOL flagFirst, int threshold = 0);
	BOOL AddEventLog(DWORD eventId, WORD eventType, CString message);
	BOOL SendMail(DWORD eventId, CString title, CString message);
	BOOL AddAlarmHistory(DWORD eventId, CString disk, CString message);

#define AS_SET_SOUND_ID 1
#define AS_PLAY_SOUND   2
#define AS_DEINIT       3
	BOOL AlertSound(DWORD eventId, DWORD mode) const;
	BOOL AlertSound(const CString& alertSoundPath) const;
	CString m_TempFilePathWave;
	CString m_TempFilePathOpus;

	void RebuildListHeader(DWORD index, BOOL forceUpdate = FALSE);

	void SaveText(CString fileName);

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	int GetSelectedDrive();

	afx_msg void OnCopy();
	afx_msg void OnSaveText();
	afx_msg void OnSaveImage();
	afx_msg void OnExit();
	afx_msg void OnAbout();
	afx_msg void OnHideSmartInfo();
	afx_msg void OnHideSerialNumber();
	afx_msg void OnCrystalDewWorld();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRefresh();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnHelpAboutSmart();
	afx_msg void OnAutoRefreshDisable();
	afx_msg void OnAutoRefresh01Min();
	afx_msg void OnAutoRefresh03Min();
	afx_msg void OnAutoRefresh05Min();
	afx_msg void OnAutoRefresh10Min();
	afx_msg void OnAutoRefresh30Min();
	afx_msg void OnAutoRefresh60Min();
	afx_msg void OnAutoRefresh120Min();
	afx_msg void OnAutoRefresh180Min();
	afx_msg void OnAutoRefresh360Min();
	afx_msg void OnAutoRefresh720Min();
	afx_msg void OnAutoRefresh1440Min();
	afx_msg void OnOpenDiskManagement();
	afx_msg void OnOpenDeviceManager();
	afx_msg void OnAdvancedDiskSearch();
	afx_msg void OnWorkaroundHD204UI();
	afx_msg void OnWorkaroundIE8MODE();
	afx_msg void OnWorkaroundAdataSsd();
	afx_msg void OnWorkaroundIgnoreC4();
	afx_msg void OnGreenMode();
	afx_msg void OnResident();

	// Task Tray
	afx_msg LRESULT OnRegMessage(WPARAM, LPARAM);
	afx_msg LRESULT OnTaskbarCreated(WPARAM, LPARAM);
	afx_msg LRESULT OnTempIcon0(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon1(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon2(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon3(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon4(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon5(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon6(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon7(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon8(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon9(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon10(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon11(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon12(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon13(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon14(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon15(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon16(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon17(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon18(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon19(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon20(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon21(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon22(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon23(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon24(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon25(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon26(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon27(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon28(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon29(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon30(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon31(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon32(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon33(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon34(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon35(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon36(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon37(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon38(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon39(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon40(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon41(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon42(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon43(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon44(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon45(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon46(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon47(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTempIcon48(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTempIcon49(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTempIcon50(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTempIcon51(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTempIcon52(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTempIcon53(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTempIcon54(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTempIcon55(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTempIcon56(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTempIcon57(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTempIcon58(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTempIcon59(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTempIcon60(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTempIcon61(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTempIcon62(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTempIcon63(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnPlayAlertSound(WPARAM wParam, LPARAM lParam);

	afx_msg void OnGraph();
	afx_msg void OnHelp();
	afx_msg void OnCustomize();
	afx_msg void OnStartup();
	afx_msg void OnWait0Sec();
	afx_msg void OnWait5Sec();
	afx_msg void OnWait10Sec();
	afx_msg void OnWait15Sec();
	afx_msg void OnWait20Sec();
	afx_msg void OnWait30Sec();
	afx_msg void OnWait40Sec();
	afx_msg void OnWait50Sec();
	afx_msg void OnWait60Sec();
	afx_msg void OnWait90Sec();
	afx_msg void OnWait120Sec();
	afx_msg void OnWait150Sec();
	afx_msg void OnWait180Sec();
	afx_msg void OnWait210Sec();
	afx_msg void OnWait240Sec();
	afx_msg void OnAutoDetection05Sec();
	afx_msg void OnAutoDetection10Sec();
	afx_msg void OnAutoDetection20Sec();
	afx_msg void OnAutoDetection30Sec();
	afx_msg void OnAutoDetectionDisable();
	afx_msg void OnEventLog();
	afx_msg void OnAtaPassThroughSmart();
	afx_msg void OnGadgetSupport();
	afx_msg void OnCelsius();
	afx_msg void OnFahrenheit();
	afx_msg void OnAamApm();
	afx_msg void OnTemperature();
	afx_msg void OnAutoAamApm();
	afx_msg void OnRescan();
	afx_msg void OnUsbSat();
	afx_msg void OnUsbIodata();
	afx_msg void OnUsbSunplus();
	afx_msg void OnUsbLogitec();
	afx_msg void OnUsbProlific();
	afx_msg void OnUsbJmicron();
	afx_msg void OnUsbCypress();
	afx_msg void OnUsbMemory();
	afx_msg void OnUsbASM1352R();
//	afx_msg void OnUsbSat16();
	afx_msg void OnUsbNVMeJMicron();
	afx_msg void OnUsbNVMeASMedia();
	afx_msg void OnUsbNVMeRealtek();
	afx_msg void OnUsbEnableAll();
	afx_msg void OnUsbDisableAll();
	afx_msg void OnMegaRAID();
	afx_msg void OnHealthStatus();
	afx_msg void OnSoundSetting();
	afx_msg void OnDumpIdentifyDevice();
	afx_msg void OnDumpSmartReadData();
	afx_msg void OnDumpSmartReadThreshold();
	afx_msg void OnResidentMinimize();
	afx_msg void OnResidentHide();
	afx_msg void OnZoom100();
	afx_msg void OnZoom125();
	afx_msg void OnZoom150();
	afx_msg void OnZoom200();
	afx_msg void OnZoom250();
	afx_msg void OnZoom300();
	afx_msg void OnZoomAuto();
	afx_msg void OnRawValues16();
	afx_msg void OnRawValues10All();
	afx_msg void OnRawValues2byte();
	afx_msg void OnRawValues1byte();
//	afx_msg void OnAutoDetection();
	afx_msg void OnAsciiView();
	afx_msg void OnAlertMail();
	afx_msg void OnMailSettings();
	afx_msg void OnSmartEnglish();
	afx_msg void OnFontSetting();
	afx_msg void OnCsmiDisable();
	afx_msg void OnCsmiEnableAuto();
	afx_msg void OnCsmiEnableRaid();
	afx_msg void OnCsmiEnableAll();
	afx_msg void OnInstallGadget();
//	afx_msg void OnAlarmHistory();
	afx_msg void OnAlertSound();
	afx_msg void OnHideNoSmartDisk();
	afx_msg void OnBnClickedButtonDisk0();
	afx_msg void OnBnClickedButtonDisk1();
	afx_msg void OnBnClickedButtonDisk2();
	afx_msg void OnBnClickedButtonDisk3();
	afx_msg void OnBnClickedButtonDisk4();
	afx_msg void OnBnClickedButtonDisk5();
	afx_msg void OnBnClickedButtonDisk6();
	afx_msg void OnBnClickedButtonDisk7();
	afx_msg void OnBnClickedButtonPreDisk();
	afx_msg void OnBnClickedButtonNextDisk();
	afx_msg void OnBnClickedButtonHealthStatus();
	afx_msg void OnBnClickedButtonTemperature();
	afx_msg void OnBnClickedButtonVoice();
	afx_msg void OnBnClickedButtonCopyright();

	afx_msg void OnBnClickedButtonLife();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
//	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
};
