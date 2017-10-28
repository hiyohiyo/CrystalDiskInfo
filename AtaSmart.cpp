/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/
// Reference : http://www.usefullcode.net/2007/02/hddsmart.html (ja)

#include "stdafx.h"
#include "AtaSmart.h"
#include <wbemcli.h>

#include "DnpService.h"
#include "GetOsInfo.h"

#pragma comment(lib, "wbemuuid.lib")
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

static const TCHAR *commandTypeString[] = 
{
	_T("un"),
	_T("pd"),
	_T("sm"),
	_T("si"),
	_T("sa"),
	_T("sp"),
	_T("io"),
	_T("l1"), // Logitec 1
	_T("l2"), // Logitec 2
	_T("jm"),
	_T("cy"),
	_T("pr"),
	_T("cs"),
	_T("cp"),
	_T("wm"),
	_T("ns"), // NVMe Samsung
	_T("ni"), // NVMe Intel
	_T("sq"), // NVMe Storage Query
};

static const TCHAR *ssdVendorString[] = 
{
	_T(""),
	_T(""),
	_T("mt"), // MTron
	_T("ix"), // Indilinx
	_T("jm"), // JMicron
	_T("il"), // Intel
	_T("sg"), // SAMSUNG
	_T("sf"), // SandForce
	_T("mi"), // Micron
	_T("oz"), // OCZ
	_T("st"), // SEAGATE
	_T("wd"), // WDC
	_T("px"), // PLEXTOR
	_T("sd"), // SanDisk
	_T("oz"), // OCZ Vector
	_T("to"), // TOSHIABA
	_T("co"), // Corsair
	_T("ki"), // Kingston
	_T("m2"), // Micron MU02
	_T("nv"), // NVMe
};

static const TCHAR *deviceFormFactorString[] = 
{
	_T(""),
	_T("5.25 inch"),
	_T("3.5 inch"),
	_T("2.5 inch"),
	_T("1.8 inch"),
	_T("< 1.8 inch")
};

CAtaSmart::CAtaSmart()
{
	BOOL bosVersionInfoEx;
	ZeroMemory(&m_Os, sizeof(OSVERSIONINFOEX));
	m_Os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if(!(bosVersionInfoEx = GetVersionEx((OSVERSIONINFO *)&m_Os)))
	{
		m_Os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx((OSVERSIONINFO *)&m_Os);
	}

	m_FlagAtaPassThrough = FALSE;
	m_FlagAtaPassThroughSmart = FALSE;
	m_FlagNVMeStorageQuery = FALSE;

	if (m_Os.dwMajorVersion >= 10)
	{
		m_FlagAtaPassThrough = TRUE;
		m_FlagAtaPassThroughSmart = TRUE;
		m_FlagNVMeStorageQuery = TRUE;
	}
	else if(m_Os.dwMajorVersion >= 6 || (m_Os.dwMajorVersion == 5 && m_Os.dwMinorVersion == 2))
	{
		m_FlagAtaPassThrough = TRUE;
		m_FlagAtaPassThroughSmart = TRUE;
	}
	else if(m_Os.dwMajorVersion == 5 && m_Os.dwMinorVersion == 1)
	{
		CString cstr;
		cstr = m_Os.szCSDVersion;
		cstr.Replace(_T("Service Pack "), _T(""));
		if(_tstoi(cstr) >= 2)
		{
			m_FlagAtaPassThrough = TRUE;
			m_FlagAtaPassThroughSmart = TRUE;
		}
	}
}

CAtaSmart::~CAtaSmart()
{
}

/* PUBLIC FUNCTION */
VOID CAtaSmart::SetAtaPassThroughSmart(BOOL flag)
{
	m_FlagAtaPassThroughSmart = flag;
}

/* PUBLIC FUNCTION */
DWORD CAtaSmart::UpdateSmartInfo(DWORD i)
{
	if (vars.GetCount() == 0)
	{
		return SMART_STATUS_NO_CHANGE;
	}

	static SMART_ATTRIBUTE attribute[MAX_DISK][MAX_ATTRIBUTE] = {0};

	if (vars[i].InterfaceType == INTERFACE_TYPE_NVME)
	{
		NVMeSmartToATASmart(vars[i].SmartReadData, &(vars[i].Attribute));

		if ((m_FlagNVMeStorageQuery && vars[i].CommandType == CMD_TYPE_NVME_STORAGE_QUERY && GetSmartAttributeNVMeStorageQuery(vars[i].PhysicalDriveId, vars[i].ScsiPort, vars[i].ScsiTargetId, &(vars[i])))
		||  (vars[i].CommandType == CMD_TYPE_NVME_INTEL && GetSmartAttributeNVMeIntel(vars[i].PhysicalDriveId, vars[i].ScsiPort, vars[i].ScsiTargetId, &(vars[i])))
		||  (vars[i].CommandType == CMD_TYPE_NVME_SAMSUNG && GetSmartAttributeNVMeSamsung(vars[i].PhysicalDriveId, vars[i].ScsiPort, vars[i].ScsiTargetId, &(vars[i])))
		||  (vars[i].CommandType == CMD_TYPE_NVME_SAMSUNG && GetSmartAttributeNVMeSamsung951(vars[i].PhysicalDriveId, vars[i].ScsiPort, vars[i].ScsiTargetId, &(vars[i])))
			)
		{
			vars[i].Temperature = vars[i].SmartReadData[0x2] * 256 + vars[i].SmartReadData[0x1] - 273;
			vars[i].Life = vars[i].SmartReadData[0x03];

			vars[i].HostReads = (ULONG64)
				(((ULONG64)vars[i].SmartReadData[0x27] << 56)
					+ ((ULONG64)vars[i].SmartReadData[0x26] << 48)
					+ ((ULONG64)vars[i].SmartReadData[0x25] << 40)
					+ ((ULONG64)vars[i].SmartReadData[0x24] << 32)
					+ ((ULONG64)vars[i].SmartReadData[0x23] << 24)
					+ ((ULONG64)vars[i].SmartReadData[0x22] << 16)
					+ ((ULONG64)vars[i].SmartReadData[0x21] << 8)
					+ ((ULONG64)vars[i].SmartReadData[0x20])) * 512 / 1024 / 1024;

			vars[i].HostWrites = (ULONG64)
				(((ULONG64)vars[i].SmartReadData[0x37] << 56)
					+ ((ULONG64)vars[i].SmartReadData[0x36] << 48)
					+ ((ULONG64)vars[i].SmartReadData[0x35] << 40)
					+ ((ULONG64)vars[i].SmartReadData[0x34] << 32)
					+ ((ULONG64)vars[i].SmartReadData[0x33] << 24)
					+ ((ULONG64)vars[i].SmartReadData[0x32] << 16)
					+ ((ULONG64)vars[i].SmartReadData[0x31] << 8)
					+ ((ULONG64)vars[i].SmartReadData[0x30])) * 512 / 1024 / 1024;

			vars[i].PowerOnCount = (ULONG64)
				(((ULONG64)vars[i].SmartReadData[0x77] << 56)
					+ ((ULONG64)vars[i].SmartReadData[0x76] << 48)
					+ ((ULONG64)vars[i].SmartReadData[0x75] << 40)
					+ ((ULONG64)vars[i].SmartReadData[0x74] << 32)
					+ ((ULONG64)vars[i].SmartReadData[0x73] << 24)
					+ ((ULONG64)vars[i].SmartReadData[0x72] << 16)
					+ ((ULONG64)vars[i].SmartReadData[0x71] << 8)
					+ ((ULONG64)vars[i].SmartReadData[0x70]));

			vars[i].MeasuredPowerOnHours = vars[i].DetectedPowerOnHours = (ULONG64)
				(((ULONG64)vars[i].SmartReadData[0x87] << 56)
					+ ((ULONG64)vars[i].SmartReadData[0x86] << 48)
					+ ((ULONG64)vars[i].SmartReadData[0x85] << 40)
					+ ((ULONG64)vars[i].SmartReadData[0x84] << 32)
					+ ((ULONG64)vars[i].SmartReadData[0x83] << 24)
					+ ((ULONG64)vars[i].SmartReadData[0x82] << 16)
					+ ((ULONG64)vars[i].SmartReadData[0x81] << 8)
					+ ((ULONG64)vars[i].SmartReadData[0x80]));
		}

		vars[i].DiskStatus = CheckDiskStatus(i);
		return SMART_STATUS_MAJOR_CHANGE;
	}

	if(vars[i].IsSmartEnabled && vars[i].IsSmartCorrect)
	{
		switch(vars[i].CommandType)
		{
		case CMD_TYPE_PHYSICAL_DRIVE:
			if(! GetSmartAttributePd(vars[i].PhysicalDriveId, vars[i].Target, &(vars[i])))
			{
				WakeUp(vars[i].PhysicalDriveId);
				if(! GetSmartAttributePd(vars[i].PhysicalDriveId, vars[i].Target, &(vars[i])))
				{
					return SMART_STATUS_NO_CHANGE;
				}
			}
			vars[i].DiskStatus = CheckDiskStatus(i);
			break;
		case CMD_TYPE_SCSI_MINIPORT:
			if(! GetSmartAttributeScsi(vars[i].ScsiPort, vars[i].ScsiTargetId, &(vars[i])))
			{
				return SMART_STATUS_NO_CHANGE;
			}
			vars[i].DiskStatus = CheckDiskStatus(i);
			break;
		case CMD_TYPE_SILICON_IMAGE:
			if(! GetSmartAttributeSi(vars[i].PhysicalDriveId, &(vars[i])))
			{
				return SMART_STATUS_NO_CHANGE;
			}
			vars[i].DiskStatus = CheckDiskStatus(i);
			break;
		case CMD_TYPE_CSMI:
			if(! GetSmartAttributeCsmi(vars[i].ScsiPort, &(vars[i].sasPhyEntity), &(vars[i])))
			{
				return SMART_STATUS_NO_CHANGE;
			}
			vars[i].DiskStatus = CheckDiskStatus(i);
			break;
		case CMD_TYPE_CSMI_PHYSICAL_DRIVE:
			if(! GetSmartAttributePd(vars[i].PhysicalDriveId, vars[i].Target, &(vars[i])))
			{
				WakeUp(vars[i].PhysicalDriveId);
				if(! GetSmartAttributePd(vars[i].PhysicalDriveId, vars[i].Target, &(vars[i])))
				{
					if(! GetSmartAttributeCsmi(vars[i].ScsiPort, &(vars[i].sasPhyEntity), &(vars[i])))
					{
						return SMART_STATUS_NO_CHANGE;
					}
					else
					{
						vars[i].CommandType = CMD_TYPE_CSMI;
					}
				}
			}
			vars[i].DiskStatus = CheckDiskStatus(i);
			break;
		
		case CMD_TYPE_SAT:
		case CMD_TYPE_SUNPLUS:
		case CMD_TYPE_IO_DATA:
		case CMD_TYPE_LOGITEC1:
		case CMD_TYPE_LOGITEC2:
		case CMD_TYPE_JMICRON:
		case CMD_TYPE_CYPRESS:
			WakeUp(vars[i].PhysicalDriveId);
			if(! GetSmartAttributeSat(vars[i].PhysicalDriveId, vars[i].Target, &(vars[i])))
			{
				return SMART_STATUS_NO_CHANGE;
			}
			vars[i].DiskStatus = CheckDiskStatus(i);
			break;
		case CMD_TYPE_WMI:	
			if(! GetSmartAttributeWmi(&(vars[i])))
			{
				return SMART_STATUS_NO_CHANGE;
			}
			vars[i].DiskStatus = CheckDiskStatus(i);
			break;
		default:
			return SMART_STATUS_NO_CHANGE;
		}
		return CheckSmartAttributeUpdate(i, attribute[i], vars[i].Attribute);
	}

	return SMART_STATUS_NO_CHANGE;
}

/* PUBLIC FUNCTION */
BOOL CAtaSmart::UpdateIdInfo(DWORD i)
{
	BOOL flag = FALSE;
	switch(vars[i].CommandType)
	{
	case CMD_TYPE_PHYSICAL_DRIVE:
	case CMD_TYPE_WMI:
	case CMD_TYPE_CSMI_PHYSICAL_DRIVE:
		flag =  DoIdentifyDevicePd(vars[i].PhysicalDriveId, vars[i].Target, &(vars[i].IdentifyDevice));
		break;
	case CMD_TYPE_SCSI_MINIPORT:
		flag =  DoIdentifyDeviceScsi(vars[i].ScsiPort, vars[i].ScsiTargetId, &(vars[i].IdentifyDevice));
		break;
	case CMD_TYPE_SILICON_IMAGE:
		flag =  DoIdentifyDeviceSi(vars[i].PhysicalDriveId, vars[i].ScsiPort, vars[i].ScsiBus, vars[i].SiliconImageType, &(vars[i].IdentifyDevice));
		break;
	case CMD_TYPE_CSMI:
		flag = DoIdentifyDeviceCsmi(vars[i].ScsiPort, &(vars[i].sasPhyEntity), &(vars[i].IdentifyDevice));
		break;
	case CMD_TYPE_SAT:
	case CMD_TYPE_SUNPLUS:
	case CMD_TYPE_IO_DATA:
	case CMD_TYPE_LOGITEC1:
	case CMD_TYPE_LOGITEC2:
	case CMD_TYPE_JMICRON:
	case CMD_TYPE_CYPRESS:
		flag = DoIdentifyDeviceSat(vars[i].PhysicalDriveId, vars[i].Target, &(vars[i].IdentifyDevice), vars[i].CommandType);
		break;
	default:
		return FALSE;
		break;
	}

	if(vars[i].Major >= 3 && vars[i].IdentifyDevice.A.CommandSetSupported2 & (1 << 3))
	{
		vars[i].IsApmSupported = TRUE;
		if(vars[i].IdentifyDevice.A.CommandSetEnabled2 & (1 << 3))
		{
			vars[i].IsApmEnabled = TRUE;
		}
		else
		{
			vars[i].IsApmEnabled = FALSE;
		}
	}
	if(vars[i].Major >= 5 && vars[i].IdentifyDevice.A.CommandSetSupported2 & (1 << 9))
	{
		vars[i].IsAamSupported = TRUE;
		if(vars[i].IdentifyDevice.A.CommandSetEnabled2 & (1 << 9))
		{
			vars[i].IsAamEnabled = TRUE;
		}
		else
		{
			vars[i].IsAamEnabled = FALSE;
		}
	}

	return flag;
}

/* PUBLIC FUNCTION */
BYTE CAtaSmart::GetAamValue(DWORD i)
{
	return LOBYTE(vars[i].IdentifyDevice.A.AcoustricManagement);
}

/* PUBLIC FUNCTION */
BYTE CAtaSmart::GetApmValue(DWORD i)
{
	return LOBYTE(vars[i].IdentifyDevice.A.CurrentPowerManagement);
}

/* PUBLIC FUNCTION */
BYTE CAtaSmart::GetRecommendAamValue(DWORD i)
{
	return HIBYTE(vars[i].IdentifyDevice.A.AcoustricManagement);
}

/* PUBLIC FUNCTION */
BYTE CAtaSmart::GetRecommendApmValue(DWORD i)
{
	return HIBYTE(vars[i].IdentifyDevice.A.CurrentPowerManagement);
}

/* PUBLIC FUNCTION */
BOOL CAtaSmart::EnableAam(DWORD i, BYTE param)
{
	return SendAtaCommand(i, 0xEF, 0x42, param);
}

/* PUBLIC FUNCTION */
BOOL CAtaSmart::DisableAam(DWORD i)
{
	return SendAtaCommand(i, 0xEF, 0xC2, 0);
}

/* PUBLIC FUNCTION */
BOOL CAtaSmart::EnableApm(DWORD i, BYTE param)
{
	return SendAtaCommand(i, 0xEF, 0x05, param);
}

/* PUBLIC FUNCTION */
BOOL CAtaSmart::DisableApm(DWORD i)
{
	return SendAtaCommand(i, 0xEF, 0x85, 0);
}

BOOL CAtaSmart::SendAtaCommand(DWORD i, BYTE main, BYTE sub, BYTE param)
{
	WakeUp(vars[i].PhysicalDriveId);
	switch(vars[i].CommandType)
	{
	case CMD_TYPE_PHYSICAL_DRIVE:
	case CMD_TYPE_WMI:
	case CMD_TYPE_CSMI_PHYSICAL_DRIVE:
		return SendAtaCommandPd(vars[i].PhysicalDriveId, vars[i].Target, main, sub, param, NULL, 0);
		break;
	case CMD_TYPE_SCSI_MINIPORT:
		return SendAtaCommandScsi(vars[i].ScsiPort, vars[i].ScsiTargetId, main, sub, param);
		break;
	case CMD_TYPE_SILICON_IMAGE:
		return FALSE;
		break;
	case CMD_TYPE_CSMI:
		if(vars[i].PhysicalDriveId >= 0 && main == 0xEF) // Enable/Disable AAM/APM
		{
			return SendAtaCommandPd(vars[i].PhysicalDriveId, vars[i].Target, main, sub, param, NULL, 0);
		}
		else
		{
			return SendAtaCommandCsmi(vars[i].ScsiPort, &(vars[i].sasPhyEntity), main, sub, param, NULL, 0);
		}
		break;
	case CMD_TYPE_SAT:
	case CMD_TYPE_SUNPLUS:
	case CMD_TYPE_IO_DATA:
	case CMD_TYPE_LOGITEC1:
	case CMD_TYPE_LOGITEC2:
	case CMD_TYPE_JMICRON:
	case CMD_TYPE_CYPRESS:
		return SendAtaCommandSat(vars[i].PhysicalDriveId, vars[i].Target, main, sub, param, vars[i].CommandType);
		break;
	default:
		return FALSE;
		break;
	}
	return FALSE;
}

DWORD CAtaSmart::CheckSmartAttributeUpdate(DWORD index, SMART_ATTRIBUTE* pre, SMART_ATTRIBUTE* cur)
{
	if(! vars[index].IsSmartCorrect)
	{
		return SMART_STATUS_NO_CHANGE;
	}

	if(memcmp(pre, cur, sizeof(SMART_ATTRIBUTE) * MAX_ATTRIBUTE) == 0)
	{
		return SMART_STATUS_NO_CHANGE;
	}
	else
	{
		for(int i = 0; i < MAX_ATTRIBUTE; i++)
		{
			switch(cur[i].Id)
			{
			case 0x09: // Power on Hours
				{
					DWORD preRawValue = MAKELONG(
						MAKEWORD(pre[i].RawValue[0], pre[i].RawValue[1]),
						MAKEWORD(pre[i].RawValue[2], pre[i].RawValue[3])
						);
					DWORD curRawValue = MAKELONG(
						MAKEWORD(cur[i].RawValue[0], cur[i].RawValue[1]),
						MAKEWORD(cur[i].RawValue[2], cur[i].RawValue[3])
						);

					if(vars[index].DiskVendorId == SSD_VENDOR_INDILINX)
					{
						preRawValue = pre[i].WorstValue * 256 + pre[i].CurrentValue;
						curRawValue = cur[i].WorstValue * 256 + cur[i].CurrentValue;
					}

					if(GetPowerOnHours(preRawValue, vars[index].DetectedTimeUnitType)
					!= GetPowerOnHours(curRawValue, vars[index].DetectedTimeUnitType))
					{
						memcpy(pre, cur, sizeof(SMART_ATTRIBUTE) * MAX_ATTRIBUTE);
						return SMART_STATUS_MAJOR_CHANGE;
					}
					if(GetPowerOnHours(preRawValue, vars[index].MeasuredTimeUnitType)
					!= GetPowerOnHours(curRawValue, vars[index].MeasuredTimeUnitType))
					{
						memcpy(pre, cur, sizeof(SMART_ATTRIBUTE) * MAX_ATTRIBUTE);
						return SMART_STATUS_MAJOR_CHANGE;
					}
				}
				break;
			case 0x0C: // Power On Count
				{
					DWORD preRawValue = MAKELONG(
						MAKEWORD(pre[i].RawValue[0], pre[i].RawValue[1]),
						MAKEWORD(pre[i].RawValue[2], pre[i].RawValue[3])
						);
					DWORD curRawValue = MAKELONG(
						MAKEWORD(cur[i].RawValue[0], cur[i].RawValue[1]),
						MAKEWORD(cur[i].RawValue[2], cur[i].RawValue[3])
						);
					if(vars[index].DiskVendorId == SSD_VENDOR_INDILINX)
					{
						preRawValue = pre[i].WorstValue * 256 + pre[i].CurrentValue;
						curRawValue = cur[i].WorstValue * 256 + cur[i].CurrentValue;
					}

					if(preRawValue != curRawValue)
					{
						memcpy(pre, cur, sizeof(SMART_ATTRIBUTE) * MAX_ATTRIBUTE);
						return SMART_STATUS_MAJOR_CHANGE;
					}
				}
				break;
			case 0xBE: // 3.9.4 or later
			case 0xC2: // Temperature
				if(pre[i].RawValue[0] != cur[i].RawValue[0]
				|| pre[i].CurrentValue != cur[i].CurrentValue)
				{
					memcpy(pre, cur, sizeof(SMART_ATTRIBUTE) * MAX_ATTRIBUTE);
					return SMART_STATUS_MAJOR_CHANGE;
				}
				break;
			default:
				break;
			}
		}
		// 3.4.0 - 
		return SMART_STATUS_MAJOR_CHANGE;
		// return SMART_STATUS_MINOR_CHANGE;
	}
}

/* PUBLIC FUNCTION */
BOOL CAtaSmart::MeasuredTimeUnit()
{
	DWORD getTickCount = GetTickCount();
	if(getTickCount > MeasuredGetTickCount + 155000 || MeasuredGetTickCount + 125000 > getTickCount)
	{
		return FALSE;
	}

	for(int i = 0; i < vars.GetCount(); i++)
	{
		if(vars[i].PowerOnRawValue < 0)
		{
			continue;
		}
		UpdateSmartInfo(i);

		DWORD test = vars[i].PowerOnRawValue - vars[i].PowerOnStartRawValue;

		if(vars[i].DetectedTimeUnitType == POWER_ON_MILLI_SECONDS)
		{
			vars[i].MeasuredTimeUnitType = POWER_ON_MILLI_SECONDS;
		}
		else if(vars[i].DetectedTimeUnitType == POWER_ON_10_MINUTES)
		{
			vars[i].MeasuredTimeUnitType = POWER_ON_10_MINUTES;
		}
		else if(vars[i].DiskVendorId == SSD_VENDOR_INDILINX)
		{
			vars[i].MeasuredTimeUnitType = POWER_ON_HOURS;
		}
		else if(vars[i].Model.Find(_T("SAMSUNG")) == 0)
		{
			if(test >= 2)
			{
				vars[i].MeasuredTimeUnitType = POWER_ON_HALF_MINUTES;
			}
			else
			{
				vars[i].MeasuredTimeUnitType = POWER_ON_HOURS;
			}
		}
		else if(vars[i].Model.Find(_T("FUJITSU")) == 0)
		{
			if(test >= 6)
			{
				vars[i].MeasuredTimeUnitType = POWER_ON_SECONDS;
			}
			else if(test >= 4)
			{
				vars[i].MeasuredTimeUnitType = POWER_ON_HALF_MINUTES;
			}
			else if(test >= 2)
			{
				vars[i].MeasuredTimeUnitType = POWER_ON_MINUTES;
			}
			else
			{
				vars[i].MeasuredTimeUnitType = POWER_ON_HOURS;
			}
		}
		else if(vars[i].Model.Find(_T("MAXTOR")) == 0)
		{
			if(test >= 2)
			{
				vars[i].MeasuredTimeUnitType = POWER_ON_MINUTES;
				vars[i].IsMaxtorMinute = TRUE;
			}
			else
			{
				vars[i].MeasuredTimeUnitType = POWER_ON_HOURS;
				vars[i].IsMaxtorMinute = FALSE;
			}
		}
		else
		{
			if(test >= 2)
			{
				vars[i].MeasuredTimeUnitType = POWER_ON_MINUTES;
			}
			else
			{
				vars[i].MeasuredTimeUnitType = POWER_ON_HOURS;
			}
		}
	}

	return TRUE;	
}

/* PUBLIC FUNCTION */
VOID CAtaSmart::Init(BOOL useWmi, BOOL advancedDiskSearch, PBOOL flagChangeDisk, BOOL workaroundHD204UI, BOOL workaroundAdataSsd, BOOL flagHideNoSmartDisk)
{
	// Debug
	// useWmi = FALSE;

	IsAdvancedDiskSearch = advancedDiskSearch;
	IsEnabledWmi = FALSE;
	IsWorkaroundHD204UI = workaroundHD204UI;
	IsWorkaroundAdataSsd = workaroundAdataSsd;

	FlagNvidiaController = FALSE;
	FlagMarvellController = FALSE;

	CArray<DISK_POSITION, DISK_POSITION> previous;
	
	if(flagChangeDisk != NULL)
	{
		*flagChangeDisk = FALSE;
		for(int i = 0; i < vars.GetCount(); i++)
		{
			DISK_POSITION dp;
			dp.PhysicalDriveId = vars[i].PhysicalDriveId;
			dp.ScsiTargetId = vars[i].ScsiTargetId;
			dp.ScsiPort = vars[i].ScsiPort;
			memcpy(&(dp.sasPhyEntity), &(vars[i].sasPhyEntity), sizeof(CSMI_SAS_PHY_ENTITY));

			previous.Add(dp);
		}
	}

	// Init
	vars.RemoveAll();
	externals.RemoveAll();
	m_ControllerMap = _T("");
	m_BlackIdeController.RemoveAll();
	m_BlackScsiController.RemoveAll();
	m_SiliconImageController.RemoveAll();
	m_UASPController.RemoveAll();
	m_SiliconImageControllerType.RemoveAll();

	BOOL detectUASPdisks = FALSE;
	BOOL detectUSBMemory = FALSE;

	m_BlackPhysicalDrive.RemoveAll();
	DWORD driveLetterMap[256] = {0};

	if(useWmi)
	{
		HRESULT hRes = S_OK;
		ULONG   uReturned = 1;

		IWbemLocator*			pIWbemLocator = NULL;
		IWbemServices*			pIWbemServices = NULL;
		IEnumWbemClassObject*	pEnumCOMDevs = NULL;
		IEnumWbemClassObject*	pEnumCOMDevs2 = NULL;
		IWbemClassObject*		pCOMDev = NULL;

		
		DebugPrint(_T("CAtaSmart::Init WMI on - Start"));

		bool initWmi = true;
		CDnpService	cService;
	
		if(! cService.IsServiceRunning(_T("Winmgmt")))
		{
			DebugPrint(_T("Waiting... Winmgmt"));
			initWmi = cService.EasyStart(_T("Winmgmt"));
		}

		if(initWmi)
		{
			try
			{
			//	DebugPrint(_T("CoInitialize()"));
			//	CoInitialize(NULL);
				DebugPrint(_T("CoInitializeSecurity()"));
				CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
					RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
				DebugPrint(_T("CoCreateInstance()"));
				//CLSID_WbemAdministrativeLocator / 
				if(FAILED(CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER,
					IID_IWbemLocator, (LPVOID *)&pIWbemLocator)))
				{
				//	CoUninitialize();
					DebugPrint(_T("NG:WMI Init 1"));
				}
				else 
				{
					long securityFlag = 0;
					if( m_Os.dwMajorVersion >= 6 // Vista or later
				//	|| (m_Os.dwMajorVersion == 5 && m_Os.dwMinorVersion >= 1) // XP or later
					)
					{
						securityFlag = WBEM_FLAG_CONNECT_USE_MAX_WAIT;
					}

					DebugPrint(_T("ConnectServer()"));
					if (FAILED(pIWbemLocator->ConnectServer(_bstr_t(L"\\\\.\\root\\cimv2"),
						NULL, NULL, 0L,
						securityFlag,
						NULL, NULL, &pIWbemServices)))
					{
					//	CoUninitialize();
						DebugPrint(_T("NG:WMI Init 2"));
					}
					else
					{
						DebugPrint(_T("CoSetProxyBlanket()"));
						hRes = CoSetProxyBlanket(pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
							NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
						if(FAILED(hRes))
						{
						//	CoUninitialize();
							CString cstr;
							cstr.Format(_T("NG:WMI Init - %08X"), hRes);
							DebugPrint(cstr);
						}
						else
						{
							IsEnabledWmi = TRUE;
							DebugPrint(_T("OK:WMI Init"));
						}
					}
					SAFE_RELEASE(pIWbemLocator);
				}
			}
			catch(...)
			{
				DebugPrint(_T("EX:WMI Init"));
			}
		}
		else
		{
			DebugPrint(_T("NG:WMI Init 3"));
		}

		if(IsEnabledWmi)
		{
			CStringArray csa;
			CString temp, cstr, cstr1, cstr2;
			try
			{// Win32_IDEController
				hRes = pIWbemServices->ExecQuery(_bstr_t(L"WQL"),
					_bstr_t(L"select Name, DeviceID from Win32_IDEController"), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumCOMDevs);
				if(FAILED(hRes))
				{
					goto safeRelease;
				}

				int workaround = 0;
				while (pEnumCOMDevs && SUCCEEDED(pEnumCOMDevs->Next(10000, 1, &pCOMDev, &uReturned)) && uReturned == 1 && workaround < 256)
				{
					workaround++;
					BOOL flagBlackList = FALSE;
					VARIANT  pVal;
					VariantInit(&pVal);
					CString name1, deviceId, channel;
					if(pCOMDev->Get(L"DeviceID", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
					{
						deviceId = pVal.bstrVal;
						if(deviceId.Find(_T("PCIIDE\\IDECHANNEL")) == 0)
						{
							channel = deviceId.Right(1);
						}
						deviceId.Replace(_T("\\"), _T("\\\\"));
						VariantClear(&pVal);
					}
					if(pCOMDev->Get(L"Name", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
					{
						name1 = pVal.bstrVal;
						if(! channel.IsEmpty())
						{
							name1 += _T(" (") + channel + _T(")");
						}
						m_IdeController.Add(name1);
						VariantClear(&pVal);

						// Black List
					//	if(name1.Find(_T("")) == 0)
					//	{
					//		flagBlackList = TRUE;
					//	}
					}
					SAFE_RELEASE(pCOMDev);

					if(cstr.Find(name1) == -1 || cstr.Find(name1 + _T(" [ATA]")) >= 0)
					{
						csa.Add(cstr);
						cstr = _T("%%%") + name1 + _T(" [ATA]");
						cstr += _T("\r\n");
					}

					int workaroundDevice = 0;
					CString mapping;
					mapping.Format(_T("ASSOCIATORS OF {Win32_IDEController.DeviceID=\"%s\"} WHERE AssocClass = Win32_IDEControllerDevice"), deviceId);
					pIWbemServices->ExecQuery(_bstr_t(L"WQL"), _bstr_t(mapping), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumCOMDevs2);
					while (pEnumCOMDevs2 && SUCCEEDED(pEnumCOMDevs2->Next(10000, 1, &pCOMDev, &uReturned)) && uReturned == 1 && workaroundDevice < 256)
					{
						workaroundDevice++;
						VARIANT pVal;
						VariantInit(&pVal);
						CString name2, deviceId, channel;
						if(pCOMDev->Get(L"DeviceID", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
						{
							deviceId = pVal.bstrVal;
							if(deviceId.Find(_T("PCIIDE\\IDECHANNEL")) == 0)
							{
								channel = deviceId.Right(1);
							}
							else if(flagBlackList)
							{
								m_BlackIdeController.Add(deviceId);
							}
							VariantClear(&pVal);
						}
						if(pCOMDev->Get(L"Name", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
						{
							name2 = pVal.bstrVal;
							if(! channel.IsEmpty())
							{
								name2 += _T(" (") + channel + _T(")");
							}
							VariantClear(&pVal);
						}
						SAFE_RELEASE(pCOMDev);

						// NVIDIA ATA Controller
						if (name1.Find(_T("NVIDIA")) == 0)
						{
							FlagNvidiaController = TRUE;
						}
						// Marvell ATA Controller
						if (name1.Find(_T("Marvell")) == 0)
						{
							FlagMarvellController = TRUE;
						}
						

						if(cstr.Find(_T("   - ") + name1) >= 0 || cstr.Find(_T("   + ") + name1) >= 0)
						{
							cstr1 = _T("- ") + name1;
							cstr2 = _T("+ ") + name1;
							cstr.Replace(cstr1, cstr2);

							cstr1 = name1 + _T("\r\n     - ") + name2;
							cstr.Replace(name1, cstr1);
						}
						else
						{
							cstr += _T("   - ") + name2 + _T("\r\n");
						}
						cstr.Replace(_T("%%%"), _T(" + "));
					}
					cstr.Replace(_T("%%%"), _T(" - "));
					SAFE_RELEASE(pEnumCOMDevs2);
				}
				csa.Add(cstr);
				SAFE_RELEASE(pEnumCOMDevs);
				DebugPrint(_T("OK:Win32_IDEController"));
			}
			catch(...)
			{
				DebugPrint(_T("EX:Win32_IDEController"));
			}

			try
			{
				cstr = _T("");
				hRes = pIWbemServices->ExecQuery(_bstr_t(L"WQL"),
					_bstr_t(L"select Name, DeviceID from Win32_SCSIController"), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumCOMDevs);
				if(FAILED(hRes))
				{
					goto safeRelease;
				}

				int workaroundController = 0;
				while (pEnumCOMDevs && SUCCEEDED(pEnumCOMDevs->Next(10000, 1, &pCOMDev, &uReturned)) && uReturned == 1 && workaroundController < 256)
				{
					workaroundController++;
					BOOL flagUASP = FALSE;
					BOOL flagBlackList = FALSE;
					BOOL flagSiliconImage = FALSE;
					DWORD siliconImageType = 0;

					VARIANT  pVal;
					VariantInit(&pVal);
					CString name1, deviceId;
					if(pCOMDev->Get(L"DeviceID", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
					{
						deviceId = pVal.bstrVal;
						deviceId.Replace(_T("\\"), _T("\\\\"));
						VariantClear(&pVal);
					}
					if(pCOMDev->Get(L"Name", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
					{
						name1 = pVal.bstrVal;
						m_ScsiController.Add(name1);
						VariantClear(&pVal);

						// UASP List
						if(name1.Find(_T("USB")) >= 0 || name1.Find(_T("UAS")) >= 0)
						{
							flagUASP = TRUE;
						}

						// Black List
						if(! IsAdvancedDiskSearch 
						&& (name1.Find(_T("VIA VT6410")) == 0)
						)
						{
							flagBlackList = TRUE;
						}
						else if (!IsAdvancedDiskSearch
							&& (name1.Find(_T("ITE IT8212")) == 0)
							)
						{
							flagBlackList = TRUE;
						}
						
						// NVIDIA SCSI Controller
						if (name1.Find(_T("NVIDIA")) == 0)
						{
							FlagNvidiaController = TRUE;
						}
						// Marvell SCSI Controller
						if (name1.Find(_T("Marvell")) == 0)
						{
							FlagMarvellController = TRUE;
						}

						// Workaround for DVDFab Virtual Drive
						if (name1.Find(_T("DVDFab Virtual Drive")) == 0)
						{
							continue;
						}

						// Silicon Image Controller
						if(name1.Find(_T("Silicon Image SiI ")) == 0)
						{
							flagSiliconImage = TRUE;
							CString cstr;
							cstr = name1;
							cstr.Replace(_T("Silicon Image SiI "), _T(""));
							siliconImageType = _tstoi(cstr);
						}
						// https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=836;id=diskinfo#836
						else if(name1.Find(_T("BUFFALO IFC-PCI2ES")) == 0)
						{
							flagSiliconImage = TRUE;
							siliconImageType = 3112;
						}
						// https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=1270;id=diskinfo#1270
						// http://dream-drive.net/archives/2010/01/ifc-pcie2sawo.html
						else if(name1.Find(_T("BUFFALO IFC-PCIE2SA")) == 0)
						{
							flagSiliconImage = TRUE;
							siliconImageType = 3132;
						}
					}
					SAFE_RELEASE(pCOMDev);
					if(cstr.Find(name1) == -1 || cstr.Find(name1 + _T(" [SCSI]")) >= 0)
					{
						csa.Add(cstr);
						cstr = _T("%%%") + name1 + _T(" [SCSI]");			
						cstr += _T("\r\n");
					}

					int workaroundDevice = 0;
					CString mapping;
					mapping.Format(_T("ASSOCIATORS OF {Win32_SCSIController.DeviceID=\"%s\"} WHERE AssocClass = Win32_SCSIControllerDevice"), deviceId);
					pIWbemServices->ExecQuery(_bstr_t(L"WQL"), _bstr_t(mapping), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumCOMDevs2);
					while (pEnumCOMDevs2 && SUCCEEDED(pEnumCOMDevs2->Next(10000, 1, &pCOMDev, &uReturned)) && uReturned == 1 && workaroundDevice < 256)
					{
						workaroundDevice++;
						VARIANT pVal;
						VariantInit(&pVal);
						CString name2, deviceId;
						if(pCOMDev->Get(L"DeviceID", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
						{
							deviceId = pVal.bstrVal;
							VariantClear(&pVal);

							if(flagUASP)
							{
								m_UASPController.Add(deviceId);
							}

							if(flagBlackList)
							{
								m_BlackScsiController.Add(deviceId);
							}

							if(flagSiliconImage)
							{
								m_SiliconImageController.Add(deviceId);
								m_SiliconImageControllerType.Add(siliconImageType);
							}
						}

						if(pCOMDev->Get(L"Name", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
						{
							name2 = pVal.bstrVal;
							VariantClear(&pVal);
						}
						SAFE_RELEASE(pCOMDev);

						if(cstr.Find(_T("   - ") + name1) >= 0 || cstr.Find(_T("   + ") + name1) >= 0)
						{
							cstr1 = _T("- ") + name1;
							cstr2 = _T("+ ") + name1;
							cstr.Replace(cstr1, cstr2);

							cstr1 = name1 + _T("\r\n     - ") + name2;
							cstr.Replace(name1, cstr1);
						}
						else
						{
							cstr += _T("   - ") + name2 + _T("\r\n");
						}
						cstr.Replace(_T("%%%"), _T(" + "));
					}
					cstr.Replace(_T("%%%"), _T(" - "));
					SAFE_RELEASE(pEnumCOMDevs2);
				}
				csa.Add(cstr);
				SAFE_RELEASE(pEnumCOMDevs);
				DebugPrint(_T("OK:Win32_SCSIController"));
			}
			catch(...)
			{
				DebugPrint(_T("EX:Win32_SCSIController"));
			}

			for(int i = 0; i < csa.GetCount(); i++)
			{
				m_ControllerMap += csa.GetAt(i);
			}

			DebugPrint(_T("--ControllerMap--\r\n") + m_ControllerMap);

			try
			{// Win32_USBController
				hRes = pIWbemServices->ExecQuery(_bstr_t(L"WQL"),
					_bstr_t(L"select Name, DeviceID from Win32_USBController"), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumCOMDevs);
				if(FAILED(hRes))
				{
					goto safeRelease;
				}

				while(pEnumCOMDevs && SUCCEEDED(pEnumCOMDevs->Next(10000, 1, &pCOMDev, &uReturned)) && uReturned == 1)
				{
					VARIANT  pVal;
					VariantInit(&pVal);
					CString deviceId, channel;
					if(pCOMDev->Get(L"DeviceID", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
					{
						deviceId = pVal.bstrVal;
						deviceId.Replace(_T("\\"), _T("\\\\"));
						VariantClear(&pVal);
					}
					SAFE_RELEASE(pCOMDev);

					CString mapping, enclosure;
					mapping.Format(_T("ASSOCIATORS OF {Win32_USBController.DeviceID=\"%s\"} WHERE AssocClass = Win32_USBControllerDevice"), deviceId);
					pIWbemServices->ExecQuery(_bstr_t(L"WQL"), _bstr_t(mapping), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumCOMDevs2);
					while(pEnumCOMDevs2 && SUCCEEDED(pEnumCOMDevs2->Next(10000, 1, &pCOMDev, &uReturned)) && uReturned == 1)
					{
						VARIANT pVal;
						VariantInit(&pVal);
						if(pCOMDev->Get(L"DeviceID", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
						{
							cstr = pVal.bstrVal;
							VariantClear(&pVal);
							if(cstr.Find(_T("USBSTOR")) >= 0)
							{
								EXTERNAL_DISK_INFO edi = {0};
								int curPos= 0;
								CString resToken;
								resToken = deviceId.Tokenize(_T("\\&"), curPos);
								while(resToken != _T(""))
								{
									if(resToken.Replace(_T("VID_"), _T("")) > 0)
									{
										edi.UsbVendorId = _tcstol(resToken, NULL, 16);
									}
									else if(resToken.Replace(_T("PID_"), _T("")) > 0)
									{
										edi.UsbProductId = _tcstol(resToken, NULL, 16);
									}
									resToken = deviceId.Tokenize(_T("\\&"), curPos);
								};

								if(pCOMDev->Get(L"Name", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
								{
									edi.Enclosure = pVal.bstrVal;
									VariantClear(&pVal);
								}
								externals.Add(edi);
							}
							deviceId = cstr;
						}
						SAFE_RELEASE(pCOMDev);
					}
					SAFE_RELEASE(pEnumCOMDevs2);
				}
				SAFE_RELEASE(pEnumCOMDevs);
				DebugPrint(_T("OK:Win32_USBController"));

				for(int i = 0; i < externals.GetCount(); i++)
				{
					cstr.Format(_T("VID=%04Xh, PID=%04Xh"), externals[i].UsbVendorId, externals[i].UsbProductId);
					DebugPrint(cstr);
				}
			}
			catch(...)
			{
				DebugPrint(_T("EX:Win32_USBController"));
			}
/*
			try
			{// Win32_1394Controller
				pIWbemServices->ExecQuery(_bstr_t(L"WQL"),
					_bstr_t(L"select Name, DeviceID from Win32_1394Controller"), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumCOMDevs);
				while(pEnumCOMDevs && SUCCEEDED(pEnumCOMDevs->Next(10000, 1, &pCOMDev, &uReturned)) && uReturned == 1)
				{
					VARIANT  pVal;
					VariantInit(&pVal);
					CString deviceId, channel;
					if(pCOMDev->Get(L"DeviceID", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
					{
						deviceId = pVal.bstrVal;
						deviceId.Replace(_T("\\"), _T("\\\\"));
						VariantClear(&pVal);
					}
					SAFE_RELEASE(pCOMDev);

					CString mapping, enclosure;
					mapping.Format(_T("ASSOCIATORS OF {Win32_1394Controller.DeviceID=\"%s\"} WHERE AssocClass = Win32_1394ControllerDevice"), deviceId);
					pIWbemServices->ExecQuery(_bstr_t(L"WQL"), _bstr_t(mapping), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumCOMDevs2);
					while(pEnumCOMDevs2 && SUCCEEDED(pEnumCOMDevs2->Next(10000, 1, &pCOMDev, &uReturned)) && uReturned == 1)
					{
						VARIANT pVal;
						VariantInit(&pVal);
						if(pCOMDev->Get(L"DeviceID", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
						{
							deviceId = pVal.bstrVal;
							VariantClear(&pVal);
						}
						SAFE_RELEASE(pCOMDev);
					}
					SAFE_RELEASE(pEnumCOMDevs2);
				}
				SAFE_RELEASE(pEnumCOMDevs);
				DebugPrint(_T("OK:Win32_1394Controller"));
			}
			catch(...)
			{
				DebugPrint(_T("EX:Win32_1394Controller"));
			}
*/
			/* DEBUG
			for(int i = 0; i < externals.GetCount(); i++)
			{
				CString cstr;
				cstr.Format(_T("Enclosure=%s, VID=%04X, PID=%04X"),
					externals.GetAt(i).Enclosure,
					externals.GetAt(i).VendorId,
					externals.GetAt(i).ProductId);

				AfxMessageBox(cstr);
			}
			*/

			for(int i = 0; i < MAX_SEARCH_PHYSICAL_DRIVE; i++)
			{
				BOOL	bRet;
				HANDLE	hIoCtrl;
				DWORD	dwReturned;
				DISK_GEOMETRY dg = {0};
				CAtaSmart::INTERFACE_TYPE interfaceType = INTERFACE_TYPE_UNKNOWN;
				CAtaSmart::VENDOR_ID vendor = VENDOR_UNKNOWN;
							
				hIoCtrl = GetIoCtrlHandle(i);
				if(hIoCtrl == INVALID_HANDLE_VALUE)
				{
				///	DebugPrint(_T("INVALID_HANDLE_VALUE - continue"));
					continue;
				}
				///	DebugPrint(_T("DeviceIoControl"));
				bRet = ::DeviceIoControl(hIoCtrl, IOCTL_DISK_GET_DRIVE_GEOMETRY, 
					NULL, 0, &dg, sizeof(DISK_GEOMETRY),
					&dwReturned, NULL);
				if(dg.MediaType == FixedMedia)
				{
					CString cstr;
					cstr.Format(_T("WakeUp(%d)"), i);
					DebugPrint(cstr);
					WakeUp(i);
				}
				::CloseHandle(hIoCtrl);
			}

			///////////////////////////////
			// Intel RAID support
			///////////////////////////////

			for(int i = 0; i < MAX_SEARCH_SCSI_PORT; i++)
			{
				AddDiskCsmi(i);
			}

			try
			{
				DebugPrint(_T("DO:SELECT * FROM Win32_DiskDrive"));
				hRes = pIWbemServices->ExecQuery(_bstr_t(L"WQL"), 
					_bstr_t(L"SELECT * FROM Win32_DiskDrive"), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumCOMDevs);
				if(FAILED(hRes))
				{
					goto safeRelease;
				}
				DebugPrint(_T("OK1:SELECT * FROM Win32_DiskDrive"));

				if(! pEnumCOMDevs)
				{
					DebugPrint(_T("pEnumCOMDevs == NULL"));
					goto safeRelease;
				}

				while(SUCCEEDED(pEnumCOMDevs->Next(10000, 1, &pCOMDev, &uReturned)))
				{
					if(uReturned != 1)
					{
						DebugPrint(_T("uReturned != 1"));
						break;
					}
					DebugPrint(_T("while(pEnumCOMDevs ..."));
					CString mapping1, mapping2;
					CString model, deviceId, diskSize, mediaType, interfaceTypeWmi, pnpDeviceId, firmware;
					INT physicalDriveId = -1, scsiPort = -1, scsiTargetId = -1, scsiBus = -1;
					BOOL flagTarget = FALSE;
					BOOL flagBlackList = FALSE;
					BOOL flagInterfaceTypeUASP = FALSE;
					DWORD siliconImageType = 0;
					
				try
				{
					VARIANT pVal;
					VariantInit(&pVal);
					if(pCOMDev->Get(L"Size", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
					{
						diskSize = pVal.bstrVal;
						DebugPrint(_T("diskSize:") + diskSize);
						VariantClear(&pVal);
					}

					if(pCOMDev->Get(L"DeviceID", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
					{
						deviceId = pVal.bstrVal;
						DebugPrint(_T("deviceId:") + deviceId);
						deviceId.Replace(_T("\\"), _T("\\\\"));
						if(_ttoi(deviceId.Right(2)) >= 10)
						{
							physicalDriveId = _ttoi(deviceId.Right(2));
						}
						else
						{
							physicalDriveId = _ttoi(deviceId.Right(1));
						}
						VariantClear(&pVal);
					}
					if(pCOMDev->Get(L"Model", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
					{
						model = pVal.bstrVal;
						DebugPrint(_T("model:") + model);
						VariantClear(&pVal);
					}
					if(pCOMDev->Get(L"FirmwareRevision", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
					{
						firmware = pVal.bstrVal;
						VariantClear(&pVal);
					}
					if(pCOMDev->Get(L"SCSIPort", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
					{
						scsiPort = pVal.intVal;
						VariantClear(&pVal);
					}			
					if(pCOMDev->Get(L"SCSITargetId", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
					{
						scsiTargetId = pVal.intVal;
						VariantClear(&pVal);
					}
					if(pCOMDev->Get(L"SCSIBus", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
					{
						scsiBus = pVal.intVal;
						VariantClear(&pVal);
					}
					if(pCOMDev->Get(L"MediaType", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
					{
						mediaType = pVal.bstrVal;
						DebugPrint(_T("mediaType:") + mediaType);
						mediaType.MakeLower();
						VariantClear(&pVal);

						// https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=994;id=diskinfo#994
						if(model.Find(_T("SanDisk Extreme")) == 0)
						{
							flagTarget = TRUE;
							detectUSBMemory = TRUE;
						}
						// https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=1198;id=diskinfo#1198
						else if(model.Find(_T("Kingston DT Ultimate")) == 0)
						{
							flagTarget = TRUE;
							detectUSBMemory = TRUE;
						}
						else if(FlagUsbMemory)
						{
							flagTarget = TRUE;
							detectUSBMemory = TRUE;
						}
						else if(mediaType.Find(_T("removable")) >= 0 || mediaType.IsEmpty())
						{
							flagTarget = FALSE;
						}
						else
						{
							flagTarget = TRUE;
						}
					}
					if(pCOMDev->Get(L"InterfaceType", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
					{
						interfaceTypeWmi = pVal.bstrVal;
						DebugPrint(_T("interfaceTypeWmi:") + interfaceTypeWmi);
						VariantClear(&pVal);
					}

					// Added 3.3.1 (Controller Black List Support)
					if(pCOMDev->Get(L"PNPDeviceID", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
					{
						pnpDeviceId = pVal.bstrVal;
						DebugPrint(_T("pnpDeviceId:") + pnpDeviceId);
						pnpDeviceId.MakeUpper();
						VariantClear(&pVal);

						// Is UAS Controller (5.2.0-)
						for(int i = 0; i < m_UASPController.GetCount(); i++)
						{
							if(m_UASPController.GetAt(i).Find(pnpDeviceId) >= 0)
							{
								DebugPrint(_T("UASPController:") + pnpDeviceId);
								flagInterfaceTypeUASP = TRUE;
							}
						}

						// Is Silicon Image Controller
						for(int i = 0; i < m_SiliconImageController.GetCount(); i++)
						{
							if(m_SiliconImageController.GetAt(i).Find(pnpDeviceId) >= 0)
							{
								DebugPrint(_T("SiliconImageController:") + pnpDeviceId);
								siliconImageType = m_SiliconImageControllerType.GetAt(i);
							}
						}

						for(int i = 0; i < m_BlackIdeController.GetCount(); i++)
						{
							if(m_BlackIdeController.GetAt(i).Find(pnpDeviceId) >= 0)
							{
								DebugPrint(_T("BlackIdeController:") + pnpDeviceId);
								m_BlackPhysicalDrive.Add(physicalDriveId);
								flagBlackList = TRUE;
							}
						}
						for(int i = 0; i < m_BlackScsiController.GetCount(); i++)
						{
							if(m_BlackScsiController.GetAt(i).Find(pnpDeviceId) >= 0)
							{
								DebugPrint(_T("BlackScsiController:") + pnpDeviceId);
								flagBlackList = TRUE;
							}
						}
					}

					SAFE_RELEASE(pCOMDev);

					if(! flagBlackList)
					{
						// GetDiskInfo
						CString cstr;
						cstr.Format(_T("DO:GetDiskInfo pd=%d, sp=%d, st=%d, mt=%s"), physicalDriveId, scsiPort, scsiTargetId, mediaType);
						DebugPrint(cstr);

						INTERFACE_TYPE interfaceType = INTERFACE_TYPE_UNKNOWN;
						COMMAND_TYPE commandType = CMD_TYPE_UNKNOWN;
						VENDOR_ID usbVendorId = VENDOR_UNKNOWN;
						DWORD usbProductId = 0;

						if(flagInterfaceTypeUASP)
						{
							DebugPrint(_T("INTERFACE_TYPE_UASP"));
							interfaceType = INTERFACE_TYPE_UASP;
						}
						else if (model.Find(_T("NVMe")) >= 0 || pnpDeviceId.Find(_T("NVME")) >= 0)
						{
							DebugPrint(_T("INTERFACE_TYPE_NVME"));
							interfaceType = INTERFACE_TYPE_NVME;
						}
						/*
						else if (interfaceTypeWmi.Find(_T("SCSI")) >= 0)
						{
							DebugPrint(_T("INTERFACE_TYPE_SCSI"));
							interfaceType = INTERFACE_TYPE_SCSI;
						}*/
						else if(interfaceTypeWmi.Find(_T("1394")) >= 0 || model.Find(_T(" IEEE 1394 SBP2 Device")) > 0)
						{
							DebugPrint(_T("INTERFACE_TYPE_IEEE1394"));
							interfaceType = INTERFACE_TYPE_IEEE1394;
						}
						else if(interfaceTypeWmi.Find(_T("USB")) >= 0 || model.Find(_T(" USB Device")) > 0)
						{
							DebugPrint(_T("INTERFACE_TYPE_USB"));
							interfaceType = INTERFACE_TYPE_USB;
						}
						else
						{
							flagTarget = TRUE;
						}
						
						for(int i = 0; i < externals.GetCount(); i++)
						{
							if(model.Find(externals.GetAt(i).Enclosure) == 0)
							{
								usbVendorId = (VENDOR_ID)externals[i].UsbVendorId;
								usbProductId = externals[i].UsbProductId;
								cstr.Format(_T("usbVendorId=%04X, usbProductId=%04X"), usbVendorId, usbProductId);
								DebugPrint(cstr);
							}
						}

						if(IsAdvancedDiskSearch && mediaType.IsEmpty())
						{
							flagTarget = TRUE;
						}

						// [2010/12/05] Workaround for SAMSUNG HD204UI
						// http://sourceforge.net/apps/trac/smartmontools/wiki/SamsungF4EGBadBlocks
						if((model.Find(_T("SAMSUNG HD155UI")) == 0 || model.Find(_T("SAMSUNG HD204UI")) == 0) && firmware.Find(_T("1AQ10003")) != 0 && IsWorkaroundHD204UI)
						{
							flagTarget = FALSE;
						}

						DebugPrint(_T("flagTarget && GetDiskInfo"));
						if (flagTarget && GetDiskInfo(physicalDriveId, scsiPort, scsiTargetId, interfaceType, commandType, usbVendorId, usbProductId, scsiBus, siliconImageType, FlagNvidiaController, FlagMarvellController, pnpDeviceId))
						{
							DebugPrint(_T("int index = (int)vars.GetCount() - 1;"));
							int index = (int)vars.GetCount() - 1;
							if(! diskSize.IsEmpty())
							{
								vars[index].DiskSizeWmi = (DWORD)(_ttoi64(diskSize) / 1000 / 1000 - 49);
								if(0 < vars[index].TotalDiskSize && vars[index].TotalDiskSize < 1000) // < 1GB
								{
								//	vars[index].TotalDiskSize == vars[index].DiskSizeChs;
								}
								else if(vars[index].TotalDiskSize < 10 * 1000) // < 10GB
								{
									vars[index].TotalDiskSize = vars[index].DiskSizeWmi;
								}
								else if(vars[index].TotalDiskSize < vars[index].DiskSizeWmi)
								{
								//	vars[index].TotalDiskSize = vars[index].DiskSizeWmi;
								}
							}

							BOOL flagSkipModelCheck = FALSE;

							vars[index].ModelWmi = model;
							// Model
							model.Replace(_T(" SCSI Disk Device"), _T(""));
							model.Replace(_T(" SATA Disk Device"), _T(""));
							model.Replace(_T(" ATA Disk Device"), _T(""));
							model.Replace(_T(" SCSI Device"), _T(""));
							model.Replace(_T(" SATA Device"), _T(""));
							model.Replace(_T(" ATA Device"), _T(""));
							model.Replace(_T("NVMe "), _T(""));

							if(interfaceType == INTERFACE_TYPE_UASP)
							{
								flagSkipModelCheck = TRUE;
								cstr.Format(_T("UASP (%s)"), vars[index].Interface);
								vars[index].Interface = cstr;
								vars[index].InterfaceType = INTERFACE_TYPE_UASP;
								
								detectUASPdisks = TRUE;
							}
							else if(model.Replace(_T(" USB Device"), _T("")) > 0 || interfaceTypeWmi.Find(_T("USB")) >= 0)
							{
								flagSkipModelCheck = TRUE;
								cstr.Format(_T("USB (%s)"), vars[index].Interface);
								vars[index].Interface = cstr;
								vars[index].InterfaceType = INTERFACE_TYPE_USB;

								for(int i = 0; i < externals.GetCount(); i++)
								{
									if(externals.GetAt(i).Enclosure.Find(vars[index].ModelWmi) == 0)
									{
										vars[index].Enclosure = externals.GetAt(i).Enclosure;
										vars[index].UsbVendorId  = externals.GetAt(i).UsbVendorId;
										vars[index].UsbProductId = externals.GetAt(i).UsbProductId;
									}
								}
							}
							else if(model.Replace(_T(" IEEE 1394 SBP2 Device"), _T("")) > 0 || interfaceTypeWmi.Find(_T("1394")) >= 0)
							{
								flagSkipModelCheck = TRUE;
								cstr.Format(_T("IEEE 1394 (%s)"), vars[index].Interface);
								vars[index].Interface = cstr;
								vars[index].InterfaceType = INTERFACE_TYPE_IEEE1394;
								for(int i = 0; i < externals.GetCount(); i++)
								{
									if(externals.GetAt(i).Enclosure.Find(vars[index].ModelWmi) == 0)
									{
										vars[index].Enclosure = externals.GetAt(i).Enclosure;
										vars[index].UsbVendorId  = externals.GetAt(i).UsbVendorId;
										vars[index].UsbProductId = externals.GetAt(i).UsbProductId;
									}
								}
							}
														
							CString cmp, cmp1, cmp2, cmp3;
							cmp = model;
							cmp.Replace(_T(" "), _T(""));
							cmp1 = cmp.Left(8);

							cmp = vars[index].Model;
							cmp.Replace(_T(" "), _T(""));
							cmp2 = cmp.Left(8);

							cmp = vars[index].ModelReverse;
							cmp.Replace(_T(" "), _T(""));
							cmp3 = cmp.Left(8);
							
							if(vars[index].Model.IsEmpty())
							{
								DebugPrint(_T("WmiModel: ") + model);
								DebugPrint(_T("SerialNumber: ") + vars[index].SerialNumber);
								DebugPrint(_T("vars.RemoveAt(index) - 1"));
								vars.RemoveAt(index);
							}
							else if(flagSkipModelCheck)
							{
								// None
							}
							else if(model.IsEmpty() || cmp1.Compare(cmp2) == 0)
							{
								// None
							}
							else if(cmp1.Compare(cmp3) == 0)
							{
								vars[index].SerialNumber = vars[index].SerialNumberReverse;
								vars[index].FirmwareRev = vars[index].FirmwareRevReverse;
								vars[index].Model = vars[index].ModelReverse;
								vars[index].ModelSerial = GetModelSerial(vars[index].Model, vars[index].SerialNumber);
							}
							else if(vars[index].InterfaceType == INTERFACE_TYPE_USB || vars[index].InterfaceType == INTERFACE_TYPE_UASP)
							{
								// None
							}
							else
							{
								DebugPrint(_T("WmiModel: ") + model);
								DebugPrint(_T("Model: ") + vars[index].Model);
								DebugPrint(_T("SerialNumber: ") + vars[index].SerialNumber);
								DebugPrint(_T("DISABLED: vars.RemoveAt(index) - 2"));
							//	vars.RemoveAt(index);
							}

							// DEBUG
							// vars[index].VendorId = VENDOR_MTRON;
							DebugPrint(_T("OK:Check Model Name"));

						}
					}
				}
				catch(...)
				{
					DebugPrint(_T("EX:while(pEnumCOMDevs"));
				}
				}
				SAFE_RELEASE(pEnumCOMDevs);
				DebugPrint(_T("OK2:SELECT * FROM Win32_DiskDrive"));
			}
			catch(...)
			{
				DebugPrint(_T("EX:SELECT * FROM Win32_DiskDrive"));
			}

			// Drive Letter Mapping Start
			/*
			try
			{
				DebugPrint(_T("DO:SELECT * FROM Win32_DiskPartition"));
				hRes = pIWbemServices->ExecQuery(_bstr_t(L"WQL"), 
					_bstr_t(L"SELECT * FROM Win32_DiskPartition"), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumCOMDevs);
				if(FAILED(hRes))
				{
					DebugPrint(_T("NG:SELECT * FROM Win32_DiskPartition"));
					goto safeRelease;
				}
				DebugPrint(_T("OK:SELECT * FROM Win32_DiskPartition"));

				while(pEnumCOMDevs && SUCCEEDED(pEnumCOMDevs->Next(10000, 1, &pCOMDev, &uReturned)) && uReturned == 1)
				{
					DWORD physicalDriveId = 0;
					CString partition, drive, mapping, cstr;
					VARIANT pVal;
					VariantInit(&pVal);
					if(pCOMDev->Get(L"DeviceID", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
					{
						partition = pVal.bstrVal;
						VariantClear(&pVal);
					}
					cstr = partition;
					cstr.Replace(_T("Disk #"), _T(""));
					physicalDriveId = _ttoi(cstr);
					SAFE_RELEASE(pCOMDev);

					hRes = pIWbemServices->ExecQuery(_bstr_t(L"WQL"), 
						_bstr_t(L"SELECT * FROM Win32_LogicalDisk Where DriveType = 3"), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumCOMDevs2);
					if(FAILED(hRes))
					{
						DebugPrint(_T("NG:SELECT * FROM Win32_LogicalDisk Where DriveType = 3"));
						goto safeRelease;
					}

					while(pEnumCOMDevs2 && SUCCEEDED(pEnumCOMDevs2->Next(100000, 1, &pCOMDev, &uReturned)) && uReturned == 1)
					{
						VariantClear(&pVal);
						if(pCOMDev->Get(L"DeviceID", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
						{
							drive = pVal.bstrVal;
							VariantClear(&pVal);
						}
						SAFE_RELEASE(pCOMDev);

						IWbemContext *pCtx = 0;
						IWbemCallResult *pResult = 0;
						mapping.Format(_T("Win32_LogicalDiskToPartition.Antecedent=\"Win32_DiskPartition.DeviceID=\\\"%s\\\"\",Dependent=\"Win32_LogicalDisk.DeviceID=\\\"%s\\\"\""),
							partition, drive);
						
						BSTR bstr;
						bstr = mapping.AllocSysString();
						pIWbemServices->GetObject(bstr, 0, pCtx, &pCOMDev, &pResult);
						SysFreeString(bstr);
						if(pCOMDev)
						{
							driveLetterMap[physicalDriveId] |= 1 << (drive.GetAt(0) - 'A');
						}
						SAFE_RELEASE(pCOMDev);
					}
					SAFE_RELEASE(pEnumCOMDevs2);
				}
				SAFE_RELEASE(pEnumCOMDevs);

				DebugPrint(_T("OK:Drive Letter Mapping"));

			}
			catch(...)
			{
				DebugPrint(_T("EX:Drive Letter Mapping"));
			}
		*/
safeRelease:

			SAFE_RELEASE(pCOMDev);
			SAFE_RELEASE(pEnumCOMDevs);
			SAFE_RELEASE(pEnumCOMDevs2);
			SAFE_RELEASE(pIWbemServices);
		//	CoUninitialize();
		//  DebugPrint(_T("OK:CoUninitialize()"));
		}
	}
	else
	{
		DebugPrint(_T("CAtaSmart::Init WMI off - Start"));
	}
	
	// \\\\.\\PhysicalDrive%d
	for(int i = 0; i < MAX_SEARCH_PHYSICAL_DRIVE; i++)
	{
		BOOL	flagChecked = FALSE;
		BOOL	bRet;
		HANDLE	hIoCtrl;
		DWORD	dwReturned;
		DISK_GEOMETRY dg;
		CAtaSmart::INTERFACE_TYPE interfaceType = INTERFACE_TYPE_UNKNOWN;
		CAtaSmart::COMMAND_TYPE commandType = CMD_TYPE_UNKNOWN;
		CAtaSmart::VENDOR_ID vendor = VENDOR_UNKNOWN;

		for(int j = 0; j < vars.GetCount(); j++)
		{
			if(i == vars[j].PhysicalDriveId)
			{
				flagChecked = TRUE;
			}
		}

		for(int j = 0; j < m_BlackPhysicalDrive.GetCount(); j++)
		{
			if(i == m_BlackPhysicalDrive.GetAt(j))
			{
				flagChecked = TRUE;
			}
		}

		if(flagChecked)
		{
///			DebugPrint(_T("flagChecked - continue"));
			continue;
		}

///		DebugPrint(_T("GetIoCtrlHandle"));
		hIoCtrl = GetIoCtrlHandle(i);
		if(hIoCtrl == INVALID_HANDLE_VALUE)
		{
///			DebugPrint(_T("INVALID_HANDLE_VALUE - continue"));
			continue;
		}
///		DebugPrint(_T("DeviceIoControl"));
		bRet = ::DeviceIoControl(hIoCtrl, IOCTL_DISK_GET_DRIVE_GEOMETRY, 
			NULL, 0, &dg, sizeof(DISK_GEOMETRY),
			&dwReturned, NULL);
		if(bRet == FALSE || dwReturned != sizeof(DISK_GEOMETRY) || dg.MediaType != FixedMedia)
		{
///			DebugPrint(_T("CloseHandle - continue"));
			::CloseHandle(hIoCtrl);
			continue;
		}
		// [2010/12/05] Workaround for SAMSUNG HD204UI
		// http://sourceforge.net/apps/trac/smartmontools/wiki/SamsungF4EGBadBlocks

		DWORD	dwRet;
		DWORD	dwLen;
		BYTE*	pcbData;
		STORAGE_DEVICE_DESCRIPTOR*	pDescriptor;
		STORAGE_PROPERTY_QUERY		sQuery;
		CString model, firmware;

		dwLen = 4096;
		pcbData = new BYTE[dwLen];
		if(pcbData == NULL)
		{
			continue;
		}
		ZeroMemory(pcbData, dwLen);
		sQuery.PropertyId	= StorageDeviceProperty;
		sQuery.QueryType	= PropertyStandardQuery;
		sQuery.AdditionalParameters[0] = NULL;
		
		bRet = ::DeviceIoControl(hIoCtrl, IOCTL_STORAGE_QUERY_PROPERTY, &sQuery, sizeof(STORAGE_PROPERTY_QUERY), pcbData,dwLen,&dwRet,NULL);
		if(bRet == FALSE)
		{
			delete [] pcbData;
			::CloseHandle(hIoCtrl);
			continue;
		}

		pDescriptor = (STORAGE_DEVICE_DESCRIPTOR*)pcbData;
		if(pDescriptor->ProductIdOffset)
		{
			model	= (char*)pDescriptor + pDescriptor->ProductIdOffset;
		}
		if(pDescriptor->ProductRevisionOffset)
		{
			firmware	= (char*)pDescriptor + pDescriptor->ProductRevisionOffset;
		}

		delete [] pcbData;

		if((model.Find(_T("SAMSUNG HD155UI")) == 0 || model.Find(_T("SAMSUNG HD204UI")) == 0) && firmware.Find(_T("1AQ10003")) != 0 && IsWorkaroundHD204UI)
		{
			continue;
		}

		// [2010/12/05] Workaround for SAMSUNG HD204UI
		// http://sourceforge.net/apps/trac/smartmontools/wiki/SamsungF4EGBadBlocks
		
		// USB-HDD Check
		// if(! IsEnabledWmi)
		{
			DebugPrint(_T("USB-HDD Check"));

			BYTE cbData[4096];
			ZeroMemory(cbData, 4096);

			STORAGE_DEVICE_DESCRIPTOR*	pDescriptor;
			STORAGE_PROPERTY_QUERY		sQuery;
			DWORD	dwRet;

			sQuery.PropertyId	= StorageDeviceProperty;
			sQuery.QueryType	= PropertyStandardQuery;
			sQuery.AdditionalParameters[0] = NULL;

///			DebugPrint(_T("DeviceIoControl"));
			bRet = ::DeviceIoControl(hIoCtrl, IOCTL_STORAGE_QUERY_PROPERTY, &sQuery,
				sizeof(STORAGE_PROPERTY_QUERY), &cbData, 4096, &dwRet, NULL);

			if(bRet != FALSE)
			{
				DebugPrint(_T("Check Bus Type"));
				pDescriptor = (STORAGE_DEVICE_DESCRIPTOR*)&cbData;
				if(pDescriptor->BusType == BusTypeUsb)
				{
					DebugPrint(_T("Bus Type = USB"));
					interfaceType = INTERFACE_TYPE_USB;
					vendor = USB_VENDOR_ALL;
				}
			}
		}
///		DebugPrint(_T("CloseHandle"));
		::CloseHandle(hIoCtrl);

		DebugPrint(_T("DO:GetDiskInfo"));
		if(GetDiskInfo(i, -1, -1, interfaceType, commandType, vendor))
		{
			DebugPrint(_T("OK:GetDiskInfo"));
			int index = (int)vars.GetCount() - 1;
			
			CString cmp, cstr;

///			cstr.Format(_T("index: %d"), index);
			cmp = vars[index].Model;
			DebugPrint(_T("Check Reverse"));
			if(cmp.Find(_T("DW C")) == 0 // WDC 
			|| cmp.Find(_T("iHat")) == 0 // Hitachi
			|| cmp.Find(_T("ASSM")) == 0 // SAMSUNG
			|| cmp.Find(_T("aMtx")) == 0 // Maxtor
			|| cmp.Find(_T("OTHS")) == 0 // TOSHIBA
			|| cmp.Find(_T("UFIJ")) == 0 // FUJITSU
			)	
			{
				DebugPrint(_T("Reverse"));
				vars[index].SerialNumber = vars[index].SerialNumberReverse;
				vars[index].FirmwareRev = vars[index].FirmwareRevReverse;
				vars[index].Model = vars[index].ModelReverse;
				vars[index].ModelSerial = GetModelSerial(vars[index].Model, vars[index].SerialNumber);
			}

			if(interfaceType == INTERFACE_TYPE_USB)
			{
				cstr.Format(_T("USB (%s)"), vars[index].Interface);
				vars[index].Interface = cstr;
			}
		}
	}

	DebugPrint(_T("OK:GetDiskInfo - PhysicalDrive"));
	// Sort
	ATA_SMART_INFO* p = vars.GetData();
	qsort(p, vars.GetCount(), sizeof(ATA_SMART_INFO), Compare);

	DebugPrint(_T("OK:qsort"));

	// Advanced Disk Search
	if(IsAdvancedDiskSearch)
	{
		// \\\\.\\Scsi%d:
		for(int i = 0; i < MAX_SEARCH_SCSI_PORT; i++)
		{
			for(int j = 0; j < MAX_SEARCH_SCSI_TARGET_ID; j++)
			{
				if(GetDiskInfo(-1, i, j, INTERFACE_TYPE_UNKNOWN, CMD_TYPE_UNKNOWN, VENDOR_UNKNOWN))
				{
					int index = (int)vars.GetCount() - 1;

					CString cmp;
					cmp = vars[index].Model;
					if(cmp.Find(_T("DW C")) == 0 // WDC 
					|| cmp.Find(_T("iHat")) == 0 // Hitachi
					|| cmp.Find(_T("ASSM")) == 0 // SAMSUNG
					|| cmp.Find(_T("aMtx")) == 0 // Maxtor
					|| cmp.Find(_T("OTHS")) == 0 // TOSHIBA
					|| cmp.Find(_T("UFIJ")) == 0 // FUJITSU
					)	
					{
						vars[index].SerialNumber = vars[index].SerialNumberReverse;
						vars[index].FirmwareRev = vars[index].FirmwareRevReverse;
						vars[index].Model = vars[index].ModelReverse;
						vars[index].ModelSerial = GetModelSerial(vars[index].Model, vars[index].SerialNumber);
					}
				}
			}
		}
		DebugPrint(_T("OK:GetDiskInfo - Scsi"));
	}

	DebugPrint(_T("Drive Letter Mapping - Start"));

	// Drive Letter Mapping http://www.cplusplus.com/forum/windows/12196/
	for(TCHAR c = 'A'; c <= 'Z'; c++)
	{
		CString cstr;
		TCHAR szPath[MAX_PATH] = {0};
		wsprintf(szPath, _T("%c:\\"), c);
		cstr = szPath;
		DebugPrint(cstr);

		if(GetDriveType(cstr) == DRIVE_FIXED)
		{
			// None
		}
		else if(detectUSBMemory && GetDriveType(cstr) == DRIVE_REMOVABLE && c >= 'C')
		{
			// None
		}
		else
		{
			DebugPrint(_T("Drive Letter Mapping - != DRIVE_FIXED"));
			continue;
		}
		
		wsprintf(szPath, _T("\\\\.\\%c:"), c);
		cstr = szPath;
		DebugPrint(cstr);

		HANDLE hHandle = CreateFile(szPath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_WRITE|FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hHandle == INVALID_HANDLE_VALUE)
		{
			DebugPrint(_T("Drive Letter Mapping - INVALID_HANDLE_VALUE"));
			continue;
		}
		VOLUME_DISK_EXTENTS_LX volumeDiskExtents;
		DWORD dwBytesReturned = 0;
		BOOL bResult = DeviceIoControl(hHandle, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0,
			&volumeDiskExtents, sizeof(volumeDiskExtents), &dwBytesReturned, NULL);
		CloseHandle(hHandle);
		if(!bResult)
		{
			DebugPrint(_T("Drive Letter Mapping - bResult == FALSE"));
			continue;
		}

		cstr.Format(_T("volumeDiskExtents.NumberOfDiskExtents = %d"), volumeDiskExtents.NumberOfDiskExtents);
		DebugPrint(cstr);

		for (DWORD n = 0; n < volumeDiskExtents.NumberOfDiskExtents && volumeDiskExtents.NumberOfDiskExtents < 4; ++n)
		{
			PDISK_EXTENT pDiskExtent = &volumeDiskExtents.Extents[n];

			// Workaround for RamPhantom EX 1.1
			// https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=1178;id=diskinfo#1178
			if(pDiskExtent->ExtentLength.QuadPart == 0)
			{
				continue;
			}
			if(0 <= pDiskExtent->DiskNumber && pDiskExtent->DiskNumber < 256)
			{
				driveLetterMap[pDiskExtent->DiskNumber] |= 1 << (c - 'A');
			}
			cstr.Format(_T("n = %d, pDiskExtent->DiskNumber = %d"), n, pDiskExtent->DiskNumber);
			DebugPrint(cstr);
		}
	}
	DebugPrint(_T("Drive Letter Mapping - End"));

	for(int i = 0; i < vars.GetCount(); i++)
	{
		if(vars[i].PhysicalDriveId < 0)
		{
			continue;
		}

		CString driveLetter = _T("");
		for(int j = 0; j < 26; j++)
		{
			if(driveLetterMap[vars[i].PhysicalDriveId] & (1 << j))
			{
				CString cstr;
				cstr.Format(_T("%C"), j + 'A'); 
				driveLetter += cstr + _T(": ");
				vars[i].DriveLetterMap += (1 << j);
				DebugPrint(cstr);
			}
		}
		vars[i].DriveMap.Append(driveLetter);
		vars[i].DriveMap.TrimRight();
	}
	// Drive Letter Mapping2

	MeasuredGetTickCount = GetTickCount();
	DebugPrint(_T("CAtaSmart::Init - Complete"));

	if(flagChangeDisk != NULL)
	{
		if(vars.GetCount() != previous.GetCount())
		{
			*flagChangeDisk = TRUE;
		}
		else
		{
			for(int i = 0; i < vars.GetCount(); i++)
			{
				if(vars.GetAt(i).PhysicalDriveId != previous.GetAt(i).PhysicalDriveId
				|| vars.GetAt(i).ScsiTargetId != previous.GetAt(i).ScsiTargetId
				|| vars.GetAt(i).ScsiPort != previous.GetAt(i).ScsiPort
				|| memcmp(&(vars.GetAt(i).sasPhyEntity), &(previous.GetAt(i).sasPhyEntity), sizeof(CSMI_SAS_PHY_ENTITY)) != 0
				)
				{
					*flagChangeDisk = TRUE;
					break;
				}
			}
		}
	}

	// HideNoSmartDisk
	if(flagHideNoSmartDisk)
	{
		int count = (int)vars.GetCount();
		if(count > 0)
		{
			for(int i = count - 1; i >= 0; i--)
			{
				if(vars[i].AttributeCount == 0)
				{
					vars.RemoveAt(i);
				}
			}
		}
	}
}

int CAtaSmart::Compare(const void *p1, const void *p2)
{
	if(((ATA_SMART_INFO*)p1)->PhysicalDriveId == -1 && ((ATA_SMART_INFO*)p2)->PhysicalDriveId == -1)
	{
		return ((ATA_SMART_INFO*)p1)->sasPhyEntity.bPortIdentifier - ((ATA_SMART_INFO*)p2)->sasPhyEntity.bPortIdentifier;
	}
	/*
	else if(((ATA_SMART_INFO*)p1)->PhysicalDriveId == -1)
	{
		return 1;
	}
	else if(((ATA_SMART_INFO*)p2)->PhysicalDriveId == -1)
	{
		return -1;
	}
	*/
	else
	{
		return ((ATA_SMART_INFO*)p1)->PhysicalDriveId - ((ATA_SMART_INFO*)p2)->PhysicalDriveId;
	}
}

BOOL CAtaSmart::AddDisk(INT physicalDriveId, INT scsiPort, INT scsiTargetId, INT scsiBus, BYTE target, COMMAND_TYPE commandType, IDENTIFY_DEVICE* identify, INT siliconImageType, PCSMI_SAS_PHY_ENTITY sasPhyEntity, CString pnpDeviceId)
{
	if(vars.GetCount() >= MAX_DISK)
	{
		return FALSE;
	}

	ATA_SMART_INFO asi = {0};
	ATA_SMART_INFO asiCheck = {0};

	memcpy(&(asi.IdentifyDevice), identify, sizeof(IDENTIFY_DEVICE));
	asi.PhysicalDriveId = physicalDriveId;
	asi.ScsiBus = scsiBus;
	asi.ScsiPort =  scsiPort;
	asi.ScsiTargetId = scsiTargetId;
	asi.SiliconImageType = siliconImageType;
	asi.CommandType = commandType;
	asiCheck.CommandType = commandType;
	asi.SsdVendorString = _T("");

	if(sasPhyEntity != NULL)
	{
		memcpy(&(asi.sasPhyEntity), sasPhyEntity, sizeof(CSMI_SAS_PHY_ENTITY));
	}

	if(commandType == CMD_TYPE_PHYSICAL_DRIVE || CMD_TYPE_SAT <= commandType && commandType <= CMD_TYPE_PROLIFIC)
	{
		if(target == 0xB0)
		{
			asi.CommandTypeString.Format(_T("%s2"), commandTypeString[commandType]);
		}
		else
		{
			asi.CommandTypeString.Format(_T("%s1"), commandTypeString[commandType]);
		}
	}
	else
	{
		asi.CommandTypeString = commandTypeString[commandType];
	}

	for(int i = 0; i < MAX_ATTRIBUTE; i++)
	{
		::ZeroMemory(&(asi.Attribute[i]), sizeof(SMART_ATTRIBUTE));
		::ZeroMemory(&(asi.Threshold[i]), sizeof(SMART_THRESHOLD));
	}

	for(int i = 0; i < 512; i++)
	{
		asi.SmartReadData[i] = 0x00;
		asi.SmartReadThreshold[i] = 0x00;
	}

	asi.IsSmartEnabled = FALSE;
	asi.IsIdInfoIncorrect = FALSE;
	asi.IsSmartCorrect = FALSE;
	asi.IsThresholdCorrect = FALSE;
	asi.IsWord88 = FALSE;
	asi.IsWord64_76 = FALSE;
	asi.IsCheckSumError = FALSE;
	asi.IsRawValues8 = FALSE;
	asi.IsRawValues7 = FALSE;
	asi.Is9126MB = FALSE;
	asi.IsThresholdBug = FALSE;

	asi.IsSmartSupported = FALSE;
	asi.IsLba48Supported = FALSE;
	asi.IsNcqSupported = FALSE;
	asi.IsAamSupported = FALSE;
	asi.IsApmSupported = FALSE;
	asi.IsAamEnabled = FALSE;
	asi.IsApmEnabled = FALSE;
	asi.IsNvCacheSupported = FALSE;
	asi.IsDeviceSleepSupported = FALSE;
	asi.IsMaxtorMinute = FALSE;
	asi.IsSsd = FALSE;
	asi.IsTrimSupported = FALSE;
	
	asi.TotalDiskSize = 0;
	asi.Cylinder = 0;
	asi.Head = 0;
	asi.Sector = 0;
	asi.Sector28 = 0;
	asi.Sector48 = 0;
	asi.NumberOfSectors = 0;
	asi.DiskSizeChs = 0;
	asi.DiskSizeLba28 = 0;
	asi.DiskSizeLba48 = 0;
	asi.DiskSizeWmi = 0;
	asi.BufferSize = 0;
	asi.NvCacheSize = 0;
	asi.TransferModeType = 0;
	asi.DetectedTimeUnitType = 0;
	asi.MeasuredTimeUnitType = 0;
	asi.AttributeCount = 0;
	asi.DetectedPowerOnHours = -1;
	asi.MeasuredPowerOnHours = -1;
	asi.PowerOnRawValue = -1;
	asi.PowerOnStartRawValue = -1;
	asi.PowerOnCount = 0;
	asi.Temperature = -1000;
	asi.TemperatureMultiplier = 1.0;
	asi.NominalMediaRotationRate = 0;
//	asi.Speed = 0.0;
	asi.Life = -1;
	asi.HostWrites = -1;
	asi.HostReads = -1;
	asi.GBytesErased = -1;
	asi.NandWrites = -1;
	asi.WearLevelingCount = -1;
//	asi.PlextorNandWritesUnit = 0;

	asi.Major = 0;
	asi.Minor = 0;

	asi.DiskStatus = 0;
	asi.DriveLetterMap = 0;

	asi.AlarmTemperature = 0;

	asi.InterfaceType = INTERFACE_TYPE_UNKNOWN;
	asi.HostReadsWritesUnit = HOST_READS_WRITES_UNKNOWN;

	asi.DiskVendorId = VENDOR_UNKNOWN;
	asi.UsbVendorId = VENDOR_UNKNOWN;
	asi.UsbProductId = 0;
	asi.Target = target;
	
	asi.SerialNumber = _T("");
	asi.FirmwareRev = _T("");
	asi.Model = _T("");
	asi.ModelReverse = _T("");
	asi.ModelWmi = _T("");
	asi.ModelSerial = _T("");
	asi.DriveMap = _T("");
	asi.MaxTransferMode = _T("");
	asi.CurrentTransferMode = _T("");
	asi.MajorVersion = _T("");
	asi.MinorVersion = _T("");
	asi.Interface = _T("");
	asi.Enclosure = _T("");
	asi.DeviceNominalFormFactor = _T("");
	asi.PnpDeviceId = pnpDeviceId;
	asi.MinorVersion = _T("");

	CHAR buf[64];

	// Check Sum Error
	BYTE sum = 0;
	BYTE checkSum[IDENTIFY_BUFFER_SIZE];
	memcpy(checkSum, (void *)identify, IDENTIFY_BUFFER_SIZE);
	for(int j = 0; j < IDENTIFY_BUFFER_SIZE; j++)
	{
		sum += checkSum[j];
	}
	if(sum != 0)
	{
		asi.IsCheckSumError = TRUE;
	}
	
	if(CheckAsciiStringError(identify->A.SerialNumber, sizeof(identify->A.SerialNumber))
	|| CheckAsciiStringError(identify->A.FirmwareRev, sizeof(identify->A.FirmwareRev))
	|| CheckAsciiStringError(identify->A.Model, sizeof(identify->A.Model)))
	{
		asi.IsIdInfoIncorrect = TRUE;
	//	DebugPrint(_T("CheckAsciiStringError"));
		return FALSE;
	}

	// Reverse
	strncpy_s(buf, 64, identify->A.SerialNumber, sizeof(identify->A.SerialNumber));
	asi.SerialNumberReverse = buf;
	asi.SerialNumberReverse.TrimLeft();
	asi.SerialNumberReverse.TrimRight();
	strncpy_s(buf, 64, identify->A.FirmwareRev, sizeof(identify->A.FirmwareRev));
	asi.FirmwareRevReverse = buf;
	asi.FirmwareRevReverse.TrimLeft();
	asi.FirmwareRevReverse.TrimRight();
	strncpy_s(buf, 64, identify->A.Model, sizeof(identify->A.Model));
	asi.ModelReverse = buf;
	asi.ModelReverse.TrimLeft();
	asi.ModelReverse.TrimRight();

	ChangeByteOrder(identify->A.SerialNumber, sizeof(identify->A.SerialNumber));
	ChangeByteOrder(identify->A.FirmwareRev, sizeof(identify->A.FirmwareRev));
	ChangeByteOrder(identify->A.Model, sizeof(identify->A.Model));

	// Normal
	strncpy_s(buf, 64, identify->A.SerialNumber, sizeof(identify->A.SerialNumber));
	asi.SerialNumber = buf;
	asi.SerialNumber.TrimLeft();
	asi.SerialNumber.TrimRight();
	strncpy_s(buf, 64, identify->A.FirmwareRev, sizeof(identify->A.FirmwareRev));
	asi.FirmwareRev = buf;
	asi.FirmwareRev.TrimLeft();
	asi.FirmwareRev.TrimRight();
	strncpy_s(buf, 64, identify->A.Model, sizeof(identify->A.Model));
	asi.Model = buf;
	asi.Model.TrimLeft();
	asi.Model.TrimRight();

	if(asi.Model.IsEmpty() || asi.FirmwareRev.IsEmpty())
	{
		DebugPrint(_T("asi.Model.IsEmpty() || asi.FirmwareRev.IsEmpty()"));
		asi.IsIdInfoIncorrect = TRUE;
		return FALSE;
	}

	INT	duplicatedId = -1;
	// Check duplicate device
	for(int i = 0; i < vars.GetCount(); i++)
	{
		if(asi.Model.Compare(vars[i].Model) == 0 && asi.SerialNumber.Compare(vars[i].SerialNumber) == 0)
		{
			// for CSMI devices
			if(vars[i].PhysicalDriveId == -1)
			{
				vars[i].PhysicalDriveId = asi.PhysicalDriveId;
				if(CsmiType == CSMI_TYPE_ENABLE_AUTO)
				{
					duplicatedId = i;
					DebugPrint(_T("vars[i].CommandType = CMD_TYPE_CSMI_PHYSICAL_DRIVE"));
					vars[i].CommandType = CMD_TYPE_CSMI_PHYSICAL_DRIVE;
					vars[i].CommandTypeString = commandTypeString[vars[i].CommandType];
					break;
				}
				else
				{
					return FALSE;
				}
			}
			else if (vars[i].ModelWmi.IsEmpty())
			{
				duplicatedId = i;
			}
			else if (asi.ModelWmi.IsEmpty())
			{
				return FALSE;
			}
		}
	}

	CString firmwareRevInt = asi.FirmwareRev;
	firmwareRevInt.Replace(_T("."), _T(""));
	if(asi.Model.Find(_T("ADATA SSD")) == 0 && _wtoi(firmwareRevInt) == 346)
	{
		asi.TemperatureMultiplier = 0.5;
	}

// DEBUG
//	asi.Model = _T(" MTRON ") + asi.Model; 

	asi.ModelSerial = GetModelSerial(asi.Model, asi.SerialNumber);
	asi.Major = GetAtaMajorVersion(identify->A.MajorVersion, asi.MajorVersion);
	GetAtaMinorVersion(identify->A.MinorVersion, asi.MinorVersion);
	asi.TransferModeType = GetTransferMode(identify->A.MultiWordDma, identify->A.SerialAtaCapabilities,
						identify->A.SerialAtaAdditionalCapabilities,
						identify->A.UltraDmaMode, asi.CurrentTransferMode, asi.MaxTransferMode,
						asi.Interface, &asi.InterfaceType);
	
	asi.DetectedTimeUnitType = GetTimeUnitType(asi.Model, asi.FirmwareRev, asi.Major, asi.TransferModeType);

	if(asi.DetectedTimeUnitType == POWER_ON_MILLI_SECONDS)
	{
		asi.MeasuredTimeUnitType = POWER_ON_MILLI_SECONDS;
	}
	else if(asi.DetectedTimeUnitType == POWER_ON_10_MINUTES)
	{
		asi.MeasuredTimeUnitType = POWER_ON_10_MINUTES;
	}

	// Feature
	if(asi.Major >= 3 && asi.IdentifyDevice.A.CommandSetSupported1 & (1 << 0))
	{
		asi.IsSmartSupported = TRUE;
	}
	if(asi.Major >= 3 && asi.IdentifyDevice.A.CommandSetSupported2 & (1 << 3))
	{
		asi.IsApmSupported = TRUE;
		if(asi.IdentifyDevice.A.CommandSetEnabled2 & (1 << 3))
		{
			asi.IsApmEnabled = TRUE;
		}
	}
	if(asi.Major >= 5 && asi.IdentifyDevice.A.CommandSetSupported2 & (1 << 9))
	{
		asi.IsAamSupported = TRUE;
		if(asi.IdentifyDevice.A.CommandSetEnabled2 & (1 << 9))
		{
			asi.IsAamEnabled = TRUE;
		}
	}

	if(asi.Major >= 5 && asi.IdentifyDevice.A.CommandSetSupported2 & (1 << 10))
	{
		asi.IsLba48Supported = TRUE;
	}

	if(asi.Major >= 6 && asi.IdentifyDevice.A.SerialAtaCapabilities & (1 << 8))
	{
		asi.IsNcqSupported = TRUE;
	}

	if(asi.Major >= 7 && asi.IdentifyDevice.A.NvCacheCapabilities & (1 << 0))
	{
		asi.IsNvCacheSupported = TRUE;
	}

	if(asi.Major >= 7 && asi.IdentifyDevice.A.DataSetManagement & (1 << 0))
	{
		asi.IsTrimSupported = TRUE;
	}

	// http://ascii.jp/elem/000/000/203/203345/img.html
	// "NominalMediaRotationRate" is supported by ATA8-ACS but a part of ATA/ATAPI-7 devices support this field.
	if(asi.Major >= 7 && asi.IdentifyDevice.A.NominalMediaRotationRate == 0x01)
	{
		asi.IsSsd = TRUE;
		asi.NominalMediaRotationRate = 1;
	}

	if(asi.Major >= 7 && (asi.IdentifyDevice.A.NominalMediaRotationRate >= 0x401
	&& asi.IdentifyDevice.A.NominalMediaRotationRate < 0xFFFF))
	{
		asi.NominalMediaRotationRate = asi.IdentifyDevice.A.NominalMediaRotationRate;
	}

	if(asi.Major >= 7 
	&&(asi.IdentifyDevice.A.DeviceNominalFormFactor & 0xF) > 0
	&&(asi.IdentifyDevice.A.DeviceNominalFormFactor & 0xF) <= 5
	)
	{
		asi.DeviceNominalFormFactor.Format(_T("%s"),  
			deviceFormFactorString[asi.IdentifyDevice.A.DeviceNominalFormFactor & 0xF]);
	//	AfxMessageBox(asi.DeviceNominalFormFactor);
	}

	if(asi.Major >= 7 && asi.IdentifyDevice.A.SerialAtaFeaturesSupported & (1 << 8))
	{
		asi.IsDeviceSleepSupported = TRUE;
	}

	CString model = asi.Model;
	model.MakeUpper();
	if(model.Find(_T("MAXTOR")) == 0 && asi.DetectedTimeUnitType == POWER_ON_MINUTES)
	{
		asi.IsMaxtorMinute = TRUE;
	}

	// DiskSize & BufferSize
	if(identify->A.LogicalCylinders > 16383)
	{
		identify->A.LogicalCylinders = 16383;
		asi.IsIdInfoIncorrect = TRUE;
	}
	if(identify->A.LogicalHeads > 16)
	{
		identify->A.LogicalHeads = 16;
		asi.IsIdInfoIncorrect = TRUE;
	}
	if(identify->A.LogicalSectors > 63)
	{
		identify->A.LogicalSectors = 63;
		asi.IsIdInfoIncorrect = TRUE;
	}

	asi.Cylinder = identify->A.LogicalCylinders;
	asi.Head = identify->A.LogicalHeads;
	asi.Sector = identify->A.LogicalSectors;
	asi.Sector28 = 0x0FFFFFFF & identify->A.TotalAddressableSectors;
	asi.Sector48 = 0x0000FFFFFFFFFFFF & identify->A.MaxUserLba;

	if(identify->A.TotalAddressableSectors == 0x01100003) // 9126807040 bytes
	{
		asi.Is9126MB = TRUE;
	}

	if(identify->A.LogicalCylinders == 0 || identify->A.LogicalHeads == 0 || identify->A.LogicalSectors == 0)
	{
		return FALSE;
	//	asi.DiskSizeChs   = 0;
	}
	else if(((ULONGLONG)(identify->A.LogicalCylinders * identify->A.LogicalHeads * identify->A.LogicalSectors * 512) / 1000 / 1000) > 1000)
	{
		asi.DiskSizeChs   = (DWORD)(((ULONGLONG)identify->A.LogicalCylinders * identify->A.LogicalHeads * identify->A.LogicalSectors * 512) / 1000 / 1000 - 49);
	}
	else
	{
		asi.DiskSizeChs   = (DWORD)(((ULONGLONG)identify->A.LogicalCylinders * identify->A.LogicalHeads * identify->A.LogicalSectors * 512) / 1000 / 1000);
	}

	asi.NumberOfSectors = (ULONGLONG)identify->A.LogicalCylinders * identify->A.LogicalHeads * identify->A.LogicalSectors;
	if(asi.Sector28 > 0 && ((ULONGLONG)asi.Sector28 * 512) / 1000 / 1000 > 49)
	{
		asi.DiskSizeLba28 = (DWORD)(((ULONGLONG)asi.Sector28 * 512) / 1000 / 1000 - 49);
		asi.NumberOfSectors = asi.Sector28;
	}
	else
	{
		asi.DiskSizeLba28 = 0;
	}

	if(asi.IsLba48Supported && (asi.Sector48 * 512) / 1000 / 1000 > 49)
	{
		asi.DiskSizeLba48 = (DWORD)((asi.Sector48 * 512) / 1000 / 1000 - 49);
		asi.NumberOfSectors = asi.Sector48;
	}
	else
	{
		asi.DiskSizeLba48 = 0;
	}

	asi.BufferSize = identify->A.BufferSize * 512;
	if(asi.IsNvCacheSupported)
	{
		asi.NvCacheSize = identify->A.NvCacheSizeLogicalBlocks * 512;
	}

	if(identify->A.TotalAddressableSectors > 0x0FFFFFFF)
	{
		asi.TotalDiskSize = 0;
	}
	else if(asi.DiskSizeChs == 0)
	{
		asi.TotalDiskSize = 0;
	}
	else if(asi.DiskSizeLba48 > asi.DiskSizeLba28)
	{
		asi.TotalDiskSize = asi.DiskSizeLba48;
	}
	else if(asi.DiskSizeLba28 > asi.DiskSizeChs)
	{
		asi.TotalDiskSize = asi.DiskSizeLba28;
	}
	else
	{
	//	asi.TotalDiskSize = 0;
		asi.TotalDiskSize = asi.DiskSizeChs;
	}

	// Error Check for External ATA Controller
	if(asi.IsLba48Supported && (identify->A.TotalAddressableSectors < 268435455 && asi.DiskSizeLba28 != asi.DiskSizeLba48))
	{
		asi.DiskSizeLba48 = 0;
	}

	CString debug;
	// Check S.M.A.R.T. Enabled or Diabled
	if(asi.IsSmartSupported || asi.Is9126MB || IsAdvancedDiskSearch)
	{
		switch(asi.CommandType)
		{
		case CMD_TYPE_PHYSICAL_DRIVE:
			debug.Format(_T("GetSmartAttributePd(%d) - 1"), physicalDriveId);
			DebugPrint(debug);
			if(GetSmartAttributePd(physicalDriveId, asi.Target, &asi))
			{
				CheckSsdSupport(asi);
				GetSmartAttributePd(physicalDriveId, asi.Target, &asiCheck);
				if(CheckSmartAttributeCorrect(&asi, &asiCheck))
				{
					debug.Format(_T("GetSmartAttributePd(%d) - 1A"), physicalDriveId);
					asi.IsSmartCorrect = TRUE;
				}

				if(GetSmartThresholdPd(physicalDriveId, asi.Target, &asi))
				{
					asi.IsThresholdCorrect = TRUE;
				}
			//	asi.DiskStatus = CheckDiskStatus(asi.Attribute, asi.Threshold, asi.AttributeCount, asi.DiskVendorId, asi.IsSmartCorrect, asi.IsSsd);
				asi.IsSmartEnabled = TRUE;
			}
			
			if(! asi.IsSmartCorrect && ControlSmartStatusPd(physicalDriveId, asi.Target, ENABLE_SMART))
			{
				debug.Format(_T("GetSmartAttributePd(%d) - 2"), physicalDriveId);
				DebugPrint(debug);
				if(GetSmartAttributePd(physicalDriveId, asi.Target, &asi))
				{
					CheckSsdSupport(asi);
					GetSmartAttributePd(physicalDriveId, asi.Target, &asiCheck);
					if(CheckSmartAttributeCorrect(&asi, &asiCheck))
					{
						debug.Format(_T("GetSmartAttributePd(%d) - 2A"), physicalDriveId);
						asi.IsSmartCorrect = TRUE;
					}
					if(GetSmartThresholdPd(physicalDriveId, asi.Target, &asi))
					{
						asi.IsThresholdCorrect = TRUE;
					}
				//	asi.DiskStatus = CheckDiskStatus(asi.Attribute, asi.Threshold, asi.AttributeCount, asi.DiskVendorId, asi.IsSmartCorrect, asi.IsSsd);
					asi.IsSmartEnabled = TRUE;
				}
			}

			// 2012/9/12 - https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=821;id=diskinfo#821
			// 2013/12/2 - https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=1330;id=diskinfo#1330
			if (memcmp(asi.SmartReadData, asi.SmartReadThreshold, 512) == 0 && asi.DiskVendorId != SSD_VENDOR_INDILINX)
			{
				DebugPrint(_T("asi.SmartReadData == asi.SmartReadThreshold"));
				asi.IsSmartCorrect = FALSE;
				asi.IsThresholdCorrect = FALSE;
				asi.IsSmartEnabled = FALSE;

				/* 2013/04/12 Disabled
				m_FlagAtaPassThroughSmart = TRUE; // Force Enable ATA_PASS_THROUGH
				
				debug.Format(_T("GetSmartAttributePd(%d) - 1"), physicalDriveId);
				DebugPrint(debug);
				if(GetSmartAttributePd(physicalDriveId, asi.Target, &asi))
				{
					CheckSsdSupport(asi);
					GetSmartAttributePd(physicalDriveId, asi.Target, &asiCheck);
					if(CheckSmartAttributeCorrect(&asi, &asiCheck))
					{
						debug.Format(_T("GetSmartAttributePd(%d) - 1A"), physicalDriveId);
						asi.IsSmartCorrect = TRUE;
					}

					if(GetSmartThresholdPd(physicalDriveId, asi.Target, &asi))
					{
						asi.IsThresholdCorrect = TRUE;
					}
				//	asi.DiskStatus = CheckDiskStatus(asi.Attribute, asi.Threshold, asi.AttributeCount, asi.DiskVendorId, asi.IsSmartCorrect, asi.IsSsd);
					asi.IsSmartEnabled = TRUE;
				}
			
				if(! asi.IsSmartCorrect && ControlSmartStatusPd(physicalDriveId, asi.Target, ENABLE_SMART))
				{
					debug.Format(_T("GetSmartAttributePd(%d) - 2"), physicalDriveId);
					DebugPrint(debug);
					if(GetSmartAttributePd(physicalDriveId, asi.Target, &asi))
					{
						CheckSsdSupport(asi);
						GetSmartAttributePd(physicalDriveId, asi.Target, &asiCheck);
						if(CheckSmartAttributeCorrect(&asi, &asiCheck))
						{
							debug.Format(_T("GetSmartAttributePd(%d) - 2A"), physicalDriveId);
							asi.IsSmartCorrect = TRUE;
						}
						if(GetSmartThresholdPd(physicalDriveId, asi.Target, &asi))
						{
							asi.IsThresholdCorrect = TRUE;
						}
					//	asi.DiskStatus = CheckDiskStatus(asi.Attribute, asi.Threshold, asi.AttributeCount, asi.DiskVendorId, asi.IsSmartCorrect, asi.IsSsd);
						asi.IsSmartEnabled = TRUE;
					}
				}
				*/
			}
			break;
		case CMD_TYPE_SCSI_MINIPORT:
			if(GetSmartAttributeScsi(scsiPort, scsiTargetId, &asi))
			{
				CheckSsdSupport(asi);
				GetSmartAttributeScsi(scsiPort, scsiTargetId, &asiCheck);
				if(CheckSmartAttributeCorrect(&asi, &asiCheck))
				{
					asi.IsSmartCorrect = TRUE;
				}
				if(GetSmartThresholdScsi(scsiPort, scsiTargetId, &asi))
				{
					asi.IsThresholdCorrect = TRUE;
				}
			//	asi.DiskStatus = CheckDiskStatus(asi.Attribute, asi.Threshold, asi.AttributeCount, asi.DiskVendorId, asi.IsSmartCorrect, asi.IsSsd);
				asi.IsSmartEnabled = TRUE;
			}
			
			if(! asi.IsSmartCorrect && ControlSmartStatusScsi(scsiPort, scsiTargetId, ENABLE_SMART))
			{
				if(GetSmartAttributeScsi(scsiPort, scsiTargetId, &asi))
				{
					CheckSsdSupport(asi);
					GetSmartAttributeScsi(scsiPort, scsiTargetId, &asiCheck);
					if(CheckSmartAttributeCorrect(&asi, &asiCheck))
					{
						asi.IsSmartCorrect = TRUE;
					}
					if(GetSmartThresholdScsi(scsiPort, scsiTargetId, &asi))
					{
						asi.IsThresholdCorrect = TRUE;
					}
				//	asi.DiskStatus = CheckDiskStatus(asi.Attribute, asi.Threshold, asi.AttributeCount, asi.DiskVendorId, asi.IsSmartCorrect, asi.IsSsd);
					asi.IsSmartEnabled = TRUE;
				}
			}
			break;
		case CMD_TYPE_SILICON_IMAGE:
			DebugPrint(_T("GetSmartAttributeSi(physicalDriveId, &asi)"));
			if(GetSmartAttributeSi(physicalDriveId, &asi))
			{
				CheckSsdSupport(asi);
				DebugPrint(_T("GetSmartAttributeSi(physicalDriveId, &asiCheck)"));
				GetSmartAttributeSi(physicalDriveId, &asiCheck);
				DebugPrint(_T("CheckSmartAttributeCorrect(&asi, &asiCheck) - 1"));
				if(CheckSmartAttributeCorrect(&asi, &asiCheck))
				{
					asi.IsSmartCorrect = TRUE;
					// Compare Si and Pd 
					GetSmartAttributePd(physicalDriveId, 0xA0, &asiCheck);
					DebugPrint(_T("CheckSmartAttributeCorrect(&asi, &asiCheck) - 2"));
					if(CheckSmartAttributeCorrect(&asi, &asiCheck))
					{
						DebugPrint(_T("GetSmartThresholdPd"));
						// Does not support GetSmartThresholdSi
						GetSmartThresholdPd(physicalDriveId, 0xA0, &asi);
						asi.IsThresholdCorrect = TRUE;
					}
				}

			//	asi.DiskStatus = CheckDiskStatus(asi.Attribute, asi.Threshold, asi.AttributeCount, asi.DiskVendorId, asi.IsSmartCorrect, asi.IsSsd);
				asi.IsSmartEnabled = TRUE;
			}
			break;
		case CMD_TYPE_CSMI:
			if(GetSmartAttributeCsmi(scsiPort, sasPhyEntity, &asi))
			{
				CheckSsdSupport(asi);
				GetSmartAttributeCsmi(scsiPort, sasPhyEntity, &asiCheck);
				if(CheckSmartAttributeCorrect(&asi, &asiCheck))
				{
					asi.IsSmartCorrect = TRUE;
				}
				if(GetSmartThresholdCsmi(scsiPort, sasPhyEntity, &asi))
				{
					asi.IsThresholdCorrect = TRUE;
				}
				asi.IsSmartEnabled = TRUE;
			}
			
			if(! asi.IsSmartCorrect && ControlSmartStatusCsmi(scsiPort, sasPhyEntity, ENABLE_SMART))
			{
				if(GetSmartAttributeCsmi(scsiPort, sasPhyEntity, &asi))
				{
					CheckSsdSupport(asi);
					GetSmartAttributeCsmi(scsiPort, sasPhyEntity, &asiCheck);
					if(CheckSmartAttributeCorrect(&asi, &asiCheck))
					{
						asi.IsSmartCorrect = TRUE;
					}
					if(GetSmartThresholdCsmi(scsiPort, sasPhyEntity, &asi))
					{
						asi.IsThresholdCorrect = TRUE;
					}
					asi.IsSmartEnabled = TRUE;
				}
			}
			break;
		case CMD_TYPE_CSMI_PHYSICAL_DRIVE:
			debug.Format(_T("GetSmartAttributePd(%d) - 1 CSMI"), physicalDriveId);
			DebugPrint(debug);
			if(GetSmartAttributePd(physicalDriveId, asi.Target, &asi))
			{
				CheckSsdSupport(asi);
				GetSmartAttributePd(physicalDriveId, asi.Target, &asiCheck);
				if(CheckSmartAttributeCorrect(&asi, &asiCheck))
				{
					debug.Format(_T("GetSmartAttributePd(%d) - 1A CSMI"), physicalDriveId);
					asi.IsSmartCorrect = TRUE;
				}

				if(GetSmartThresholdPd(physicalDriveId, asi.Target, &asi))
				{
					asi.IsThresholdCorrect = TRUE;
				}
				asi.IsSmartEnabled = TRUE;
			}

			if(! asi.IsSmartEnabled || ! asi.IsSmartCorrect || ! asi.IsThresholdCorrect)
			{
				debug.Format(_T("GetSmartAttributeCsmi - 1 CSMI"));
				DebugPrint(debug);
				if(GetSmartAttributeCsmi(scsiPort, sasPhyEntity, &asi))
				{
					CheckSsdSupport(asi);
					GetSmartAttributeCsmi(scsiPort, sasPhyEntity, &asiCheck);
					if(CheckSmartAttributeCorrect(&asi, &asiCheck))
					{
						asi.IsSmartCorrect = TRUE;
					}
					if(GetSmartThresholdCsmi(scsiPort, sasPhyEntity, &asi))
					{
						asi.IsThresholdCorrect = TRUE;
					}
					asi.IsSmartEnabled = TRUE;
				}

				if(asi.IsSmartEnabled && asi.IsSmartCorrect && asi.IsThresholdCorrect)
				{
					asi.CommandType = CMD_TYPE_CSMI;
				}
			
				if(! asi.IsSmartCorrect && ControlSmartStatusCsmi(scsiPort, sasPhyEntity, ENABLE_SMART))
				{
					if(GetSmartAttributeCsmi(scsiPort, sasPhyEntity, &asi))
					{
						CheckSsdSupport(asi);
						GetSmartAttributeCsmi(scsiPort, sasPhyEntity, &asiCheck);
						if(CheckSmartAttributeCorrect(&asi, &asiCheck))
						{
							asi.IsSmartCorrect = TRUE;
						}
						if(GetSmartThresholdCsmi(scsiPort, sasPhyEntity, &asi))
						{
							asi.IsThresholdCorrect = TRUE;
						}
						asi.IsSmartEnabled = TRUE;
						asi.CommandType = CMD_TYPE_CSMI;
					}
				}
			}
			break;

		case CMD_TYPE_SAT:
		case CMD_TYPE_SUNPLUS:
		case CMD_TYPE_IO_DATA:
		case CMD_TYPE_LOGITEC1:
		case CMD_TYPE_LOGITEC2:
		case CMD_TYPE_JMICRON:
		case CMD_TYPE_CYPRESS:
			debug.Format(_T("GetSmartAttributeSat(%d) - 1 [%s]"), physicalDriveId, commandTypeString[asi.CommandType]);
			DebugPrint(debug);
			if(GetSmartAttributeSat(physicalDriveId, asi.Target, &asi))
			{
				CheckSsdSupport(asi);
				DebugPrint(_T("GetSmartAttributeSat - 1A"));
				GetSmartAttributeSat(physicalDriveId, asi.Target, &asiCheck);
				if(CheckSmartAttributeCorrect(&asi, &asiCheck))
				{
					asi.IsSmartCorrect = TRUE;
				}
				if(GetSmartThresholdSat(physicalDriveId, asi.Target, &asi))
				{
					asi.IsThresholdCorrect = TRUE;
				}
			//	asi.DiskStatus = CheckDiskStatus(asi.Attribute, asi.Threshold, asi.AttributeCount, asi.DiskVendorId, asi.IsSmartCorrect, asi.IsSsd);
				asi.IsSmartEnabled = TRUE;
			}
			
			if(! asi.IsSmartCorrect && ControlSmartStatusSat(physicalDriveId, asi.Target, ENABLE_SMART, asi.CommandType))
			{
				DebugPrint(_T("GetSmartAttributeSat - 2"));
				if(GetSmartAttributeSat(physicalDriveId, asi.Target, &asi))
				{
					CheckSsdSupport(asi);
					DebugPrint(_T("GetSmartAttributeSat - 2A"));
					GetSmartAttributeSat(physicalDriveId, asi.Target, &asiCheck);
					if(CheckSmartAttributeCorrect(&asi, &asiCheck))
					{
						asi.IsSmartCorrect = TRUE;
					}
					if(GetSmartThresholdSat(physicalDriveId, asi.Target, &asi))
					{
						asi.IsThresholdCorrect = TRUE;
					}
				//	asi.DiskStatus = CheckDiskStatus(asi.Attribute, asi.Threshold, asi.AttributeCount, asi.DiskVendorId, asi.IsSmartCorrect, asi.IsSsd);
					asi.IsSmartEnabled = TRUE;
				}
			}
			break;
		case CMD_TYPE_WMI:
			asi.IsSmartCorrect = GetSmartAttributeWmi(&asi);
			asi.IsThresholdCorrect = GetSmartThresholdWmi(&asi);
			if(asi.IsSmartCorrect)
			{
				CheckSsdSupport(asi);
				asi.IsSmartEnabled = TRUE;
			}
			break;
		default:
			return FALSE;
			break;
		}
	}

	// OCZ-VERTEX3 2.02 Firmware Bug
	// https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=303;id=diskinfo#303
	// http://www.ocztechnologyforum.com/forum/showthread.php?88540-Vertex-3-Issues-Errors-and-Slow-speeds.
	// OCZ-VERTEX2 1.27 Firmware Bug
	// http://sourceforge.jp/ticket/browse.php?group_id=4394&tid=28153
	if(asi.DiskVendorId == SSD_VENDOR_SANDFORCE 
	&&((asi.Model.Find(_T("OCZ-VERTEX3")) == 0 && asi.FirmwareRev.Find(_T("2.02")) == 0) 
	|| (asi.Model.Find(_T("OCZ-VERTEX2")) == 0 && asi.FirmwareRev.Find(_T("1.27")) == 0))
	)
	{
		asi.IsThresholdBug = TRUE;
	}
	// SSD G2 Serieas Firmware Bug
	// http://hardforum.com/showthread.php?t=1732629
	else if(asi.Model.Find(_T("SSD G2 Series")) == 0 && asi.FirmwareRev.Find(_T("3.6.5")) == 0)
	{
		asi.IsThresholdBug = TRUE;
	}
	
	// DEBUG
	// asi.IsSmartCorrect = rand() %2;

	asi.PowerOnStartRawValue = asi.PowerOnRawValue;

	if(! asi.IsSmartCorrect)
	{
		asi.DetectedPowerOnHours = -1;
		asi.MeasuredPowerOnHours = -1;
		asi.PowerOnRawValue = -1;
		asi.PowerOnStartRawValue = -1;
		asi.PowerOnCount = 0;
		asi.Temperature = -1000;
		asi.DiskStatus = DISK_STATUS_UNKNOWN;
	}

	// Workaround for Intel SSD
	if (asi.Model.Find(_T("Intel")) == 0 && asi.MeasuredPowerOnHours > 0x0DA753)
	{
		asi.PowerOnRawValue -= 0x0DA753;
		asi.DetectedPowerOnHours -= 0x0DA753;
		asi.MeasuredPowerOnHours -= 0x0DA753;
	}

	if(asi.IsIdInfoIncorrect && (! IsAdvancedDiskSearch || commandType >= CMD_TYPE_SAT))
	{
		return FALSE;
	}

	// for CSMI devices & RAM Drive
	if(duplicatedId != -1)
	{
		if(asi.IsSmartCorrect)
		{
			// Replace Disk
			vars.RemoveAt(duplicatedId);
		}
		else
		{
			// None (Not Add Disk)
			return FALSE;
		}
	}

	vars.Add(asi);

	return TRUE;
}


BOOL CAtaSmart::AddDiskNVMe(INT physicalDriveId, INT scsiPort, INT scsiTargetId, INT scsiBus, BYTE target, COMMAND_TYPE commandType, IDENTIFY_DEVICE* identify, CString pnpDeviceId)
{
	if (vars.GetCount() >= MAX_DISK)
	{
		return FALSE;
	}

	ATA_SMART_INFO asi = { 0 };

	memcpy(&(asi.IdentifyDevice), identify, sizeof(IDENTIFY_DEVICE));
	asi.PhysicalDriveId = physicalDriveId;
	asi.ScsiBus = scsiBus;
	asi.ScsiPort = scsiPort;
	asi.ScsiTargetId = scsiTargetId;
	asi.CommandType = commandType;
	asi.SsdVendorString = _T("");
	asi.CommandTypeString = commandTypeString[commandType];

	asi.IsSmartEnabled = TRUE;
	asi.IsIdInfoIncorrect = FALSE;
	asi.IsSmartCorrect = TRUE;
	asi.IsThresholdCorrect = TRUE;
	asi.IsWord88 = FALSE;
	asi.IsWord64_76 = FALSE;
	asi.IsCheckSumError = FALSE;
	asi.IsRawValues8 = FALSE;
	asi.IsRawValues7 = FALSE;
	asi.Is9126MB = FALSE;
	asi.IsThresholdBug = FALSE;

	asi.IsSmartSupported = TRUE;
	asi.IsLba48Supported = FALSE;
	asi.IsNcqSupported = FALSE;
	asi.IsAamSupported = FALSE;
	asi.IsApmSupported = FALSE;
	asi.IsAamEnabled = FALSE;
	asi.IsApmEnabled = FALSE;
	asi.IsNvCacheSupported = FALSE;
	asi.IsDeviceSleepSupported = FALSE;
	asi.IsMaxtorMinute = FALSE;
	asi.IsSsd = TRUE;
	asi.IsTrimSupported = FALSE;

	asi.TotalDiskSize = 0;
	asi.Cylinder = 0;
	asi.Head = 0;
	asi.Sector = 0;
	asi.Sector28 = 0;
	asi.Sector48 = 0;
	asi.NumberOfSectors = 0;
	asi.DiskSizeChs = 0;
	asi.DiskSizeLba28 = 0;
	asi.DiskSizeLba48 = 0;
	asi.DiskSizeWmi = 0;
	asi.BufferSize = 0;
	asi.NvCacheSize = 0;
	asi.TransferModeType = 0;
	asi.DetectedTimeUnitType = 0;
	asi.MeasuredTimeUnitType = 0;
	asi.AttributeCount = 0;
	asi.DetectedPowerOnHours = -1;
	asi.MeasuredPowerOnHours = -1;
	asi.PowerOnRawValue = -1;
	asi.PowerOnStartRawValue = -1;
	asi.PowerOnCount = 0;
	asi.Temperature = -1000;
	asi.TemperatureMultiplier = 1.0;
	asi.NominalMediaRotationRate = 1;
	//	asi.Speed = 0.0;
	asi.Life = -1;
	asi.HostWrites = -1;
	asi.HostReads = -1;
	asi.GBytesErased = -1;
	asi.NandWrites = -1;
	asi.WearLevelingCount = -1;
	//	asi.PlextorNandWritesUnit = 0;

	asi.Major = 0;
	asi.Minor = 0;

	asi.DiskStatus = 0;
	asi.DriveLetterMap = 0;

	asi.AlarmTemperature = 0;

	asi.InterfaceType = INTERFACE_TYPE_NVME;
	asi.HostReadsWritesUnit = HOST_READS_WRITES_512B;

	asi.DiskVendorId = VENDOR_UNKNOWN;
	asi.UsbVendorId = VENDOR_UNKNOWN;
	asi.UsbProductId = 0;
	asi.Target = target;

	asi.SerialNumber = _T("");
	asi.FirmwareRev = _T("");
	asi.Model = _T("");
	asi.ModelReverse = _T("");
	asi.ModelWmi = _T("");
	asi.ModelSerial = _T("");
	asi.DriveMap = _T("");
	asi.MaxTransferMode = _T("");
	asi.CurrentTransferMode = _T("");
	asi.MajorVersion = _T("");
	asi.MinorVersion = _T("");
	asi.Interface = _T("");
	asi.Enclosure = _T("");
	asi.DeviceNominalFormFactor = _T("");
	asi.PnpDeviceId = pnpDeviceId;
	asi.MinorVersion = _T("");

	asi.Model = asi.IdentifyDevice.N.Model;
	asi.Model = asi.Model.Mid(0, 40);
	asi.Model.TrimRight();

	asi.SerialNumber = asi.IdentifyDevice.N.SerialNumber;
	asi.SerialNumber = asi.SerialNumber.Mid(0, 20);
	asi.SerialNumber.TrimRight();

	asi.FirmwareRev = asi.IdentifyDevice.N.FirmwareRev;
	asi.FirmwareRev = asi.FirmwareRev.Mid(0, 8);
	asi.FirmwareRev.TrimRight();

	asi.ModelSerial = GetModelSerial(asi.Model, asi.SerialNumber);

	/*
	ULONG64 totalDiskSizeByte = 
		(ULONG64)
		( (ULONG64)(asi.IdentifyDevice.B.Bin[ 7] << 56)
		+ (ULONG64)(asi.IdentifyDevice.B.Bin[ 6] << 48)
		+ (ULONG64)(asi.IdentifyDevice.B.Bin[ 5] << 40)
		+ (ULONG64)(asi.IdentifyDevice.B.Bin[ 4] << 32)
		+ (ULONG64)(asi.IdentifyDevice.B.Bin[ 3] << 24)
		+ (ULONG64)(asi.IdentifyDevice.B.Bin[ 2] << 16)
		+ (ULONG64)(asi.IdentifyDevice.B.Bin[ 1] <<  8)
		+ (ULONG64)(asi.IdentifyDevice.B.Bin[ 0]))
		* 512 / 1000;
	*/
	/*	
	asi.NumberOfSectors = (ULONG64)
	          ((ULONG64)(asi.IdentifyDevice.B.Bin[17]) << 56)
			+ ((ULONG64)(asi.IdentifyDevice.B.Bin[16]) << 48)
			+ ((ULONG64)(asi.IdentifyDevice.B.Bin[15]) << 40)
			+ ((ULONG64)(asi.IdentifyDevice.B.Bin[14]) << 32)
			+ ((ULONG64)(asi.IdentifyDevice.B.Bin[13]) << 24)
			+ ((ULONG64)(asi.IdentifyDevice.B.Bin[12]) << 16)
			+ ((ULONG64)(asi.IdentifyDevice.B.Bin[11]) << 8)
			+ ((ULONG64)(asi.IdentifyDevice.B.Bin[10]));
	*/

	if (
		(m_FlagNVMeStorageQuery && commandType == CMD_TYPE_NVME_STORAGE_QUERY && GetSmartAttributeNVMeStorageQuery(physicalDriveId, scsiPort, scsiTargetId, &asi))
	||  (commandType == CMD_TYPE_NVME_INTEL && GetSmartAttributeNVMeIntel(physicalDriveId, scsiPort, scsiTargetId, &asi))
	||  (commandType == CMD_TYPE_NVME_SAMSUNG && GetSmartAttributeNVMeSamsung(physicalDriveId, scsiPort, scsiTargetId, &asi))
	||  (commandType == CMD_TYPE_NVME_SAMSUNG && GetSmartAttributeNVMeSamsung951(physicalDriveId, scsiPort, scsiTargetId, &asi))
		)
	{
		asi.IsSmartSupported = TRUE;
		asi.Temperature = asi.SmartReadData[0x2] * 256 + asi.SmartReadData[0x1] - 273;
		asi.Life = asi.SmartReadData[0x03];

		asi.HostReads = (ULONG64)
		         (((ULONG64)asi.SmartReadData[0x27] << 56)
				+ ((ULONG64)asi.SmartReadData[0x26] << 48)
				+ ((ULONG64)asi.SmartReadData[0x25] << 40)
				+ ((ULONG64)asi.SmartReadData[0x24] << 32)
				+ ((ULONG64)asi.SmartReadData[0x23] << 24)
				+ ((ULONG64)asi.SmartReadData[0x22] << 16)
				+ ((ULONG64)asi.SmartReadData[0x21] << 8)
				+ ((ULONG64)asi.SmartReadData[0x20])) * 512 / 1024 / 1024;

		asi.HostWrites = (ULONG64)
		         (((ULONG64)asi.SmartReadData[0x37] << 56)
				+ ((ULONG64)asi.SmartReadData[0x36] << 48)
				+ ((ULONG64)asi.SmartReadData[0x35] << 40)
				+ ((ULONG64)asi.SmartReadData[0x34] << 32)
				+ ((ULONG64)asi.SmartReadData[0x33] << 24)
				+ ((ULONG64)asi.SmartReadData[0x32] << 16)
				+ ((ULONG64)asi.SmartReadData[0x31] << 8)
				+ ((ULONG64)asi.SmartReadData[0x30])) * 512 / 1024 / 1024;

		asi.PowerOnCount = (ULONG64)
			     (((ULONG64)asi.SmartReadData[0x77] << 56)
				+ ((ULONG64)asi.SmartReadData[0x76] << 48)
				+ ((ULONG64)asi.SmartReadData[0x75] << 40)
				+ ((ULONG64)asi.SmartReadData[0x74] << 32)
				+ ((ULONG64)asi.SmartReadData[0x73] << 24)
				+ ((ULONG64)asi.SmartReadData[0x72] << 16)
				+ ((ULONG64)asi.SmartReadData[0x71] << 8)
				+ ((ULONG64)asi.SmartReadData[0x70]));

		asi.MeasuredPowerOnHours = asi.DetectedPowerOnHours = (ULONG64)
				 (((ULONG64)asi.SmartReadData[0x87] << 56)
				+ ((ULONG64)asi.SmartReadData[0x86] << 48)
				+ ((ULONG64)asi.SmartReadData[0x85] << 40)
				+ ((ULONG64)asi.SmartReadData[0x84] << 32)
				+ ((ULONG64)asi.SmartReadData[0x83] << 24)
				+ ((ULONG64)asi.SmartReadData[0x82] << 16)
				+ ((ULONG64)asi.SmartReadData[0x81] << 8)
				+ ((ULONG64)asi.SmartReadData[0x80]));

		NVMeSmartToATASmart(asi.SmartReadData, &asi.Attribute);
		GetTransferModePCIe(asi.CurrentTransferMode, asi.MaxTransferMode, GetPCIeSlotSpeed(physicalDriveId, true));
		asi.AttributeCount = NVME_ATTRIBUTE;

		asi.SmartKeyName = _T("SmartNVMe");
		asi.DiskVendorId = SSD_VENDOR_NVME;
		asi.SsdVendorString = ssdVendorString[asi.DiskVendorId];
		asi.IsSmartSupported = TRUE;
		asi.Interface = _T("NVM Express"); 
		if (asi.IdentifyDevice.N.MajorVersion == 0)
		{
			asi.MajorVersion = _T("NVM Express 1.0/1.1");
		}
		else
		{
			asi.MajorVersion.Format(_T("NVM Express %d.%d"), asi.IdentifyDevice.N.MajorVersion, asi.IdentifyDevice.N.MinorVersion);
		}
	}

	vars.Add(asi);

	return TRUE;
}

VOID CAtaSmart::CheckSsdSupport(ATA_SMART_INFO &asi)
{
	// Old SSD Detection
	if(IsSsdOld(asi))
	{
		asi.IsSsd = TRUE;
	}

	if(asi.Model.Find(_T("STT")) != 0 && asi.Model.Find(_T("ST")) == 0)
	{
		if(asi.IsSsd)
		{
			asi.SmartKeyName = _T("SmartSsd");
			asi.DiskVendorId = HDD_SSD_VENDOR_SEAGATE;
		}
		else
		{
			asi.SmartKeyName = _T("Smart");
			asi.DiskVendorId = HDD_SSD_VENDOR_SEAGATE;
		}
		asi.SsdVendorString = ssdVendorString[asi.DiskVendorId];
	}
	else if(asi.Model.Find(_T("WDC ")) == 0 && ! asi.IsSsd)
	{
		asi.SmartKeyName = _T("Smart");
		asi.DiskVendorId = HDD_VENDOR_WESTERN_DIGITAL;
		asi.SsdVendorString = ssdVendorString[asi.DiskVendorId];
	}
	else if (IsSsdMtron(asi))
	{
		asi.SmartKeyName = _T("SmartMtron");
		asi.DiskVendorId = SSD_VENDOR_MTRON;
		asi.SsdVendorString = ssdVendorString[asi.DiskVendorId];
		asi.IsSsd = TRUE;
	}
	else if(IsSsdJMicron60x(asi))
	{
		asi.SmartKeyName = _T("SmartJMicron60x");
		asi.DiskVendorId = SSD_VENDOR_JMICRON;
		asi.SsdVendorString = ssdVendorString[asi.DiskVendorId];
		asi.IsSsd = TRUE;
		asi.IsRawValues8 = TRUE;
	}
	else if(IsSsdJMicron61x(asi))
	{
		asi.SmartKeyName = _T("SmartJMicron61x");
		asi.DiskVendorId = SSD_VENDOR_JMICRON;
		asi.SsdVendorString = ssdVendorString[asi.DiskVendorId];
		asi.IsSsd = TRUE;
	}
	else if(IsSsdIndlinx(asi))
	{
		asi.SmartKeyName = _T("SmartIndlinx");
		asi.DiskVendorId = SSD_VENDOR_INDILINX;
		asi.SsdVendorString = ssdVendorString[asi.DiskVendorId];
		asi.IsSsd = TRUE;
		asi.IsRawValues8 = TRUE;
	}
	else if(IsSsdIntel(asi))
	{
		asi.SmartKeyName = _T("SmartIntel");
		asi.DiskVendorId = SSD_VENDOR_INTEL;
		asi.SsdVendorString = ssdVendorString[asi.DiskVendorId];
		asi.IsSsd = TRUE;
	}
	else if(IsSsdSamsung(asi))
	{
		asi.SmartKeyName = _T("SmartSamsung");
		asi.DiskVendorId = SSD_VENDOR_SAMSUNG;
		asi.SsdVendorString = ssdVendorString[asi.DiskVendorId];
		asi.IsSsd = TRUE;
	}
	else if (IsSsdMicronMU02(asi))
	{
		asi.SmartKeyName = _T("SmartMicronMU02");
		asi.DiskVendorId = SSD_VENDOR_MICRON_MU02;
		asi.SsdVendorString = ssdVendorString[asi.DiskVendorId];
		asi.IsSsd = TRUE;
	}
	else if (IsSsdMicron(asi))
	{
		asi.SmartKeyName = _T("SmartMicron");
		asi.DiskVendorId = SSD_VENDOR_MICRON;
		asi.SsdVendorString = ssdVendorString[asi.DiskVendorId];
		asi.IsSsd = TRUE;
	}
	else if(IsSsdSandForce(asi))
	{
		asi.SmartKeyName = _T("SmartSandForce");
		asi.DiskVendorId = SSD_VENDOR_SANDFORCE;
		asi.SsdVendorString = ssdVendorString[asi.DiskVendorId];
		asi.IsSsd = TRUE;
		asi.IsRawValues7 = TRUE;
	}
	else if(IsSsdOcz(asi))
	{
		asi.SmartKeyName = _T("SmartOcz");
		asi.DiskVendorId = SSD_VENDOR_OCZ;
		asi.SsdVendorString = ssdVendorString[asi.DiskVendorId];
		asi.IsSsd = TRUE;
	}
	else if(IsSsdOczVector(asi))
	{
		asi.SmartKeyName = _T("SmartOczVector");
		asi.DiskVendorId = SSD_VENDOR_OCZ_VECTOR;
		asi.SsdVendorString = ssdVendorString[asi.DiskVendorId];
		asi.IsSsd = TRUE;
	}

	else if(IsSsdPlextor(asi))
	{
		asi.SmartKeyName = _T("SmartPlextor");
		asi.DiskVendorId = SSD_VENDOR_PLEXTOR;
		asi.SsdVendorString = ssdVendorString[asi.DiskVendorId];
		asi.IsSsd = TRUE;

	//	asi.PlextorNandWritesUnit = CheckPlextorNandWritesUnit(asi);
	}
	else if(IsSsdSanDisk(asi))
	{
		asi.DiskVendorId = SSD_VENDOR_SANDISK;
		asi.SsdVendorString = ssdVendorString[asi.DiskVendorId];
		asi.IsSsd = TRUE;
	}
	else if (IsSsdKingston(asi))
	{
		asi.SmartKeyName = _T("SmartKingston");
		asi.DiskVendorId = SSD_VENDOR_KINGSTON;
		asi.SsdVendorString = ssdVendorString[asi.DiskVendorId];
	}
	else if(asi.Model.Find(_T("TOSHIBA")) == 0 && asi.IsSsd)
	{
		asi.SmartKeyName = _T("SmartToshiba");
		asi.DiskVendorId = HDD_SSD_VENDOR_TOSHIBA;
		asi.SsdVendorString = ssdVendorString[asi.DiskVendorId];
	}
	else if (asi.Model.Find(_T("Corsair")) == 0)
	{
		asi.SmartKeyName = _T("SmartCorsair");
		asi.DiskVendorId = SSD_VENDOR_CORSAIR;
		asi.SsdVendorString = ssdVendorString[asi.DiskVendorId];
	}

	else if(asi.IsSsd)
	{
		asi.SmartKeyName = _T("SmartSsd");
		return ;
	}
	else
	{
		asi.SmartKeyName = _T("Smart");
		return ;
	}

// Update Life
	for(DWORD j = 0; j < asi.AttributeCount; j++)
	{
		switch(asi.Attribute[j].Id)
		{
		case 0xBB:
			if(asi.DiskVendorId == SSD_VENDOR_MTRON)
			{
				if(asi.Attribute[j].CurrentValue <= 100)
				{
					asi.Life = asi.Attribute[j].CurrentValue;
				}
			}
			break;
		case 0xCA:
			if (asi.DiskVendorId == SSD_VENDOR_MICRON)
			{
				if (asi.Attribute[j].CurrentValue <= 100)
				{
					asi.Life = asi.Attribute[j].CurrentValue;
				}
			}
			break;
		case 0xD1:
			if(asi.DiskVendorId == SSD_VENDOR_INDILINX)
			{
				if(asi.Attribute[j].CurrentValue <= 100)
				{
					asi.Life = asi.Attribute[j].CurrentValue;
				}
			}
			break;
		case 0xE8:
			if(asi.DiskVendorId == SSD_VENDOR_INTEL || asi.DiskVendorId == SSD_VENDOR_PLEXTOR || asi.DiskVendorId == SSD_VENDOR_SANDISK)
			{
				if(asi.Attribute[j].CurrentValue <= 100)
				{
					asi.Life = asi.Attribute[j].CurrentValue;
				}
			}
			else if(asi.DiskVendorId == SSD_VENDOR_OCZ)
			{
				asi.HostWrites  = (INT)(
					(UINT64)
					( (UINT64)asi.Attribute[j].RawValue[5] * 256 * 256 * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[4] * 256 * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[3] * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[2] * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[1] * 256
					+ (UINT64)asi.Attribute[j].RawValue[0])
					* 512 / 1024 / 1024 / 1024);
			}
			break;
		case 0xE9:
			if(asi.DiskVendorId == SSD_VENDOR_OCZ || asi.DiskVendorId == SSD_VENDOR_OCZ_VECTOR)
			{
				if(asi.Attribute[j].CurrentValue <= 100)
				{
					asi.Life = asi.Attribute[j].CurrentValue;
				}
			}
			else if (asi.DiskVendorId == SSD_VENDOR_SANDISK && asi.HostReadsWritesUnit == HOST_READS_WRITES_GB)
			{
				asi.NandWrites = (INT) (MAKELONG(
					MAKEWORD(asi.Attribute[j].RawValue[0], asi.Attribute[j].RawValue[1]),
					MAKEWORD(asi.Attribute[j].RawValue[2], asi.Attribute[j].RawValue[3])
					));
			}
			else if (asi.DiskVendorId == SSD_VENDOR_PLEXTOR || asi.DiskVendorId == SSD_VENDOR_KINGSTON)
			{
				asi.NandWrites = (INT) (MAKELONG(
					MAKEWORD(asi.Attribute[j].RawValue[0], asi.Attribute[j].RawValue[1]),
					MAKEWORD(asi.Attribute[j].RawValue[2], asi.Attribute[j].RawValue[3])
					));
			}
			break;
		case 0xE1:
			if(asi.DiskVendorId == SSD_VENDOR_INTEL)
			{
				asi.HostWrites  = (INT)(MAKELONG(
					MAKEWORD(asi.Attribute[j].RawValue[0], asi.Attribute[j].RawValue[1]),
					MAKEWORD(asi.Attribute[j].RawValue[2], asi.Attribute[j].RawValue[3])
					) * 0.03125); //  0.03125 = 65536 * 512 / 1024 / 1024 / 1024;
			}
			break;
		case 0xF1:
			if (asi.DiskVendorId == HDD_SSD_VENDOR_TOSHIBA && asi.HostReadsWritesUnit == HOST_READS_WRITES_GB)
			{
				asi.HostWrites = (INT) (MAKELONG(
					MAKEWORD(asi.Attribute[j].RawValue[0], asi.Attribute[j].RawValue[1]),
					MAKEWORD(asi.Attribute[j].RawValue[2], asi.Attribute[j].RawValue[3])
					));
			}
			else if(asi.DiskVendorId == SSD_VENDOR_INTEL || asi.DiskVendorId == HDD_SSD_VENDOR_TOSHIBA)
			{
				asi.HostWrites  = (INT)(MAKELONG(
					MAKEWORD(asi.Attribute[j].RawValue[0], asi.Attribute[j].RawValue[1]),
					MAKEWORD(asi.Attribute[j].RawValue[2], asi.Attribute[j].RawValue[3])
					) * 0.03125); //  0.03125 = 65536 * 512 / 1024 / 1024 / 1024;
			}
			else if (asi.DiskVendorId == SSD_VENDOR_SANDFORCE || asi.DiskVendorId == SSD_VENDOR_OCZ_VECTOR || asi.DiskVendorId == SSD_VENDOR_CORSAIR || asi.DiskVendorId == SSD_VENDOR_KINGSTON)
			{
				asi.HostWrites  = (INT)(MAKELONG(
					MAKEWORD(asi.Attribute[j].RawValue[0], asi.Attribute[j].RawValue[1]),
					MAKEWORD(asi.Attribute[j].RawValue[2], asi.Attribute[j].RawValue[3])
					));
			}
			else if (asi.DiskVendorId == SSD_VENDOR_SAMSUNG && asi.HostReadsWritesUnit == HOST_READS_WRITES_GB)
			{
				asi.HostWrites = (INT) (MAKELONG(
					MAKEWORD(asi.Attribute[j].RawValue[0], asi.Attribute[j].RawValue[1]),
					MAKEWORD(asi.Attribute[j].RawValue[2], asi.Attribute[j].RawValue[3])
					));
			}
			else if (asi.DiskVendorId == SSD_VENDOR_SAMSUNG)
			{
				asi.HostWrites  = (INT)(
					(UINT64)
					( (UINT64)asi.Attribute[j].RawValue[5] * 256 * 256 * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[4] * 256 * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[3] * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[2] * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[1] * 256
					+ (UINT64)asi.Attribute[j].RawValue[0])
					* 512 / 1024 / 1024 / 1024);
			}
			else if(asi.DiskVendorId == HDD_VENDOR_WESTERN_DIGITAL)
			{
				asi.HostWrites  = (INT)(
					(UINT64)
					( (UINT64)asi.Attribute[j].RawValue[5] * 256 * 256 * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[4] * 256 * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[3] * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[2] * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[1] * 256
					+ (UINT64)asi.Attribute[j].RawValue[0])
					* 512 / 1024 / 1024 / 1024);
			}
			else if(asi.DiskVendorId == SSD_VENDOR_PLEXTOR || asi.DiskVendorId == SSD_VENDOR_MICRON_MU02)
			{
				asi.HostWrites  = (INT)(MAKELONG(
					MAKEWORD(asi.Attribute[j].RawValue[0], asi.Attribute[j].RawValue[1]),
					MAKEWORD(asi.Attribute[j].RawValue[2], asi.Attribute[j].RawValue[3])
					)) * 32 / 1024;
			}
			else if (asi.DiskVendorId == SSD_VENDOR_SANDISK && asi.HostReadsWritesUnit == HOST_READS_WRITES_GB)
			{
				asi.HostWrites = (INT) (MAKELONG(
					MAKEWORD(asi.Attribute[j].RawValue[0], asi.Attribute[j].RawValue[1]),
					MAKEWORD(asi.Attribute[j].RawValue[2], asi.Attribute[j].RawValue[3])
					));
			}
			else if (asi.DiskVendorId == SSD_VENDOR_SANDISK)
			{
				asi.HostWrites = (INT) (
					(UINT64)
					((UINT64) asi.Attribute[j].RawValue[5] * 256 * 256 * 256 * 256 * 256
					+ (UINT64) asi.Attribute[j].RawValue[4] * 256 * 256 * 256 * 256
					+ (UINT64) asi.Attribute[j].RawValue[3] * 256 * 256 * 256
					+ (UINT64) asi.Attribute[j].RawValue[2] * 256 * 256
					+ (UINT64) asi.Attribute[j].RawValue[1] * 256
					+ (UINT64) asi.Attribute[j].RawValue[0])
					* 512 / 1024 / 1024 / 1024);
			}
			/*
			else if(asi.DiskVendorId == HDD_SSD_VENDOR_SEAGATE)
			{
				asi.HostWrites  = (INT)(
					(UINT64)
					( (UINT64)asi.Attribute[j].RawValue[5] * 256 * 256 * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[4] * 256 * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[3] * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[2] * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[1] * 256
					+ (UINT64)asi.Attribute[j].RawValue[0])
					* 512 / 1024 / 1024 / 1024);
			}
			*/
			break;
		case 0xF2:
			if (asi.DiskVendorId == HDD_SSD_VENDOR_TOSHIBA && asi.HostReadsWritesUnit == HOST_READS_WRITES_GB)
			{
				asi.HostReads = (INT) (MAKELONG(
					MAKEWORD(asi.Attribute[j].RawValue[0], asi.Attribute[j].RawValue[1]),
					MAKEWORD(asi.Attribute[j].RawValue[2], asi.Attribute[j].RawValue[3])
					));
			}
			else if(asi.DiskVendorId == SSD_VENDOR_INTEL || asi.DiskVendorId == HDD_SSD_VENDOR_TOSHIBA)
			{
				asi.HostReads  = (INT)(MAKELONG(
					MAKEWORD(asi.Attribute[j].RawValue[0], asi.Attribute[j].RawValue[1]),
					MAKEWORD(asi.Attribute[j].RawValue[2], asi.Attribute[j].RawValue[3])
					) * 0.03125); //  0.03125 = 65536 * 512 / 1024 / 1024 / 1024;
			}
			else if (asi.DiskVendorId == SSD_VENDOR_SANDFORCE || asi.DiskVendorId == SSD_VENDOR_OCZ_VECTOR || asi.DiskVendorId == SSD_VENDOR_CORSAIR || asi.DiskVendorId == SSD_VENDOR_KINGSTON)
			{
				asi.HostReads  = (INT)(MAKELONG(
					MAKEWORD(asi.Attribute[j].RawValue[0], asi.Attribute[j].RawValue[1]),
					MAKEWORD(asi.Attribute[j].RawValue[2], asi.Attribute[j].RawValue[3])
					));
			}
			else if (asi.DiskVendorId == SSD_VENDOR_SAMSUNG && asi.HostReadsWritesUnit == HOST_READS_WRITES_GB)
			{
				asi.HostReads = (INT) (MAKELONG(
					MAKEWORD(asi.Attribute[j].RawValue[0], asi.Attribute[j].RawValue[1]),
					MAKEWORD(asi.Attribute[j].RawValue[2], asi.Attribute[j].RawValue[3])
					));
			}
			else if(asi.DiskVendorId == SSD_VENDOR_SAMSUNG)
			{
				asi.HostReads  = (INT)(
					(UINT64)
					( (UINT64)asi.Attribute[j].RawValue[5] * 256 * 256 * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[4] * 256 * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[3] * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[2] * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[1] * 256
					+ (UINT64)asi.Attribute[j].RawValue[0])
					* 512 / 1024 / 1024 / 1024);
			}
			else if(asi.DiskVendorId == HDD_VENDOR_WESTERN_DIGITAL)
			{
				asi.HostReads  = (INT)(
					(UINT64)
					( (UINT64)asi.Attribute[j].RawValue[5] * 256 * 256 * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[4] * 256 * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[3] * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[2] * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[1] * 256
					+ (UINT64)asi.Attribute[j].RawValue[0])
					* 512 / 1024 / 1024 / 1024);
			}
			else if(asi.DiskVendorId == SSD_VENDOR_PLEXTOR || asi.DiskVendorId == SSD_VENDOR_MICRON_MU02)
			{
				asi.HostReads  = (INT)(MAKELONG(
					MAKEWORD(asi.Attribute[j].RawValue[0], asi.Attribute[j].RawValue[1]),
					MAKEWORD(asi.Attribute[j].RawValue[2], asi.Attribute[j].RawValue[3])
					)) * 32 / 1024;
			}
			else if (asi.DiskVendorId == SSD_VENDOR_SANDISK && asi.HostReadsWritesUnit == HOST_READS_WRITES_GB)
			{
				asi.HostReads = (INT) (MAKELONG(
					MAKEWORD(asi.Attribute[j].RawValue[0], asi.Attribute[j].RawValue[1]),
					MAKEWORD(asi.Attribute[j].RawValue[2], asi.Attribute[j].RawValue[3])
					));
			}
			else if(asi.DiskVendorId == SSD_VENDOR_SANDISK)
			{
				asi.HostReads  = (INT)(
					(UINT64)
					( (UINT64)asi.Attribute[j].RawValue[5] * 256 * 256 * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[4] * 256 * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[3] * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[2] * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[1] * 256
					+ (UINT64)asi.Attribute[j].RawValue[0])
					* 512 / 1024 / 1024 / 1024);
			}
			else if (asi.DiskVendorId == SSD_VENDOR_MICRON)
			{
				if (asi.Attribute[j].CurrentValue <= 100)
				{
					asi.Life = asi.Attribute[j].CurrentValue;
				}
			}
			/*
			else if(asi.DiskVendorId == HDD_SSD_VENDOR_SEAGATE)
			{
				asi.HostReads  = (INT)(
					(UINT64)
					( (UINT64)asi.Attribute[j].RawValue[5] * 256 * 256 * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[4] * 256 * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[3] * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[2] * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[1] * 256
					+ (UINT64)asi.Attribute[j].RawValue[0])
					* 512 / 1024 / 1024 / 1024);
			}
			*/
			break;
		case 0xF9:
			if(asi.DiskVendorId == SSD_VENDOR_INTEL)
			{
				asi.NandWrites  = (INT)(MAKELONG(
					MAKEWORD(asi.Attribute[j].RawValue[0], asi.Attribute[j].RawValue[1]),
					MAKEWORD(asi.Attribute[j].RawValue[2], asi.Attribute[j].RawValue[3])
					));
			}
			else if(asi.DiskVendorId == SSD_VENDOR_OCZ_VECTOR)
			{
				asi.NandWrites  = (INT)(
					(UINT64)
					( (UINT64)asi.Attribute[j].RawValue[5] * 256 * 256 * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[4] * 256 * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[3] * 256 * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[2] * 256 * 256
					+ (UINT64)asi.Attribute[j].RawValue[1] * 256
					+ (UINT64)asi.Attribute[j].RawValue[0])
					* 16 / 1024 / 1024);
			}
			break;
		case 0x64:
			if(asi.DiskVendorId == SSD_VENDOR_SANDFORCE)
			{
				asi.GBytesErased  = (INT)MAKELONG(
					MAKEWORD(asi.Attribute[j].RawValue[0], asi.Attribute[j].RawValue[1]),
					MAKEWORD(asi.Attribute[j].RawValue[2], asi.Attribute[j].RawValue[3])
					);
			}
			break;
		case 0xAD:
			if(asi.DiskVendorId == SSD_VENDOR_MICRON)
			{
				asi.WearLevelingCount  = (INT)MAKELONG(
					MAKEWORD(asi.Attribute[j].RawValue[0], asi.Attribute[j].RawValue[1]),
					MAKEWORD(asi.Attribute[j].RawValue[2], asi.Attribute[j].RawValue[3])
					);
			}
			break;
		case 0xB1:
			if(asi.DiskVendorId == SSD_VENDOR_SAMSUNG)
			{
				asi.WearLevelingCount  = (INT)MAKELONG(
					MAKEWORD(asi.Attribute[j].RawValue[0], asi.Attribute[j].RawValue[1]),
					MAKEWORD(asi.Attribute[j].RawValue[2], asi.Attribute[j].RawValue[3])
					);
			}
			/*
			else if(asi.DiskVendorId == SSD_VENDOR_PLEXTOR)
			{
				asi.NandWrites  = (INT)(MAKELONG(
					MAKEWORD(asi.Attribute[j].RawValue[0], asi.Attribute[j].RawValue[1]),
					MAKEWORD(asi.Attribute[j].RawValue[2], asi.Attribute[j].RawValue[3])
					)) * asi.PlextorNandWritesUnit / 1024;
			}
			*/
			break;
		case 0xB3:
		case 0xB4:
			if(asi.DiskVendorId == SSD_VENDOR_SAMSUNG)
			{
				if(asi.Attribute[j].CurrentValue <= 100)
				{
					asi.Life = asi.Attribute[j].CurrentValue;
				}
			}
			break;
		case 0xE7:
			if (asi.DiskVendorId == SSD_VENDOR_SANDFORCE || asi.DiskVendorId == SSD_VENDOR_CORSAIR || asi.DiskVendorId == SSD_VENDOR_KINGSTON)
			{
				if(asi.Attribute[j].CurrentValue <= 100)
				{
					asi.Life = asi.Attribute[j].CurrentValue;
				}
			}
			break;
		case 0xAA:
			if(asi.DiskVendorId == SSD_VENDOR_JMICRON && ! asi.IsRawValues8)
			{
				if(asi.Attribute[j].CurrentValue <= 100)
				{
					asi.Life = asi.Attribute[j].CurrentValue;
				}
			}
			break;
		case 0xA9:
			if(asi.DiskVendorId == SSD_VENDOR_MICRON_MU02)
			{
				if(asi.Attribute[j].CurrentValue <= 100)
				{
					asi.Life = asi.Attribute[j].CurrentValue;
				}
			}
			break;
		case 0xC6:
			if(asi.DiskVendorId == SSD_VENDOR_OCZ_VECTOR)
			{
				asi.HostReads = (INT)(MAKELONG(
					MAKEWORD(asi.Attribute[j].RawValue[0], asi.Attribute[j].RawValue[1]),
					MAKEWORD(asi.Attribute[j].RawValue[2], asi.Attribute[j].RawValue[3])
					));
			}
			break;
		case 0xC7:
			if(asi.DiskVendorId == SSD_VENDOR_OCZ_VECTOR)
			{
				asi.HostWrites = (INT)(MAKELONG(
					MAKEWORD(asi.Attribute[j].RawValue[0], asi.Attribute[j].RawValue[1]),
					MAKEWORD(asi.Attribute[j].RawValue[2], asi.Attribute[j].RawValue[3])
					));
			}
			break;
		case 0xF5:
			// NAND Page Size = 8KBytes
			// http://www.overclock.net/t/1145150/official-crucial-ssd-owners-club/1290
			if (asi.DiskVendorId == SSD_VENDOR_MICRON)
			{
				asi.NandWrites = (INT) (
					(UINT64)
					((UINT64) asi.Attribute[j].RawValue[5] * 256 * 256 * 256 * 256 * 256
					+ (UINT64) asi.Attribute[j].RawValue[4] * 256 * 256 * 256 * 256
					+ (UINT64) asi.Attribute[j].RawValue[3] * 256 * 256 * 256
					+ (UINT64) asi.Attribute[j].RawValue[2] * 256 * 256
					+ (UINT64) asi.Attribute[j].RawValue[1] * 256
					+ (UINT64) asi.Attribute[j].RawValue[0])
					* 8 / 1024 / 1024);
			}
			else if (asi.DiskVendorId == SSD_VENDOR_MICRON_MU02)
			{
				asi.NandWrites = (INT)(MAKELONG(
					MAKEWORD(asi.Attribute[j].RawValue[0], asi.Attribute[j].RawValue[1]),
					MAKEWORD(asi.Attribute[j].RawValue[2], asi.Attribute[j].RawValue[3])
					)) * 32 / 1024;
			}
			break;
		case 0xF6:
			if (asi.DiskVendorId == SSD_VENDOR_MICRON)
			{
				asi.HostWrites = (INT) (
					(UINT64)
					((UINT64) asi.Attribute[j].RawValue[5] * 256 * 256 * 256 * 256 * 256
					+ (UINT64) asi.Attribute[j].RawValue[4] * 256 * 256 * 256 * 256
					+ (UINT64) asi.Attribute[j].RawValue[3] * 256 * 256 * 256
					+ (UINT64) asi.Attribute[j].RawValue[2] * 256 * 256
					+ (UINT64) asi.Attribute[j].RawValue[1] * 256
					+ (UINT64) asi.Attribute[j].RawValue[0])
					* 512 / 1024 / 1024 / 1024);
			}
			break;
		default:
			break;
		}
	}
}

/*
INT CAtaSmart::CheckPlextorNandWritesUnit(ATA_SMART_INFO &asi)
{
	CString model = asi.Model;
	CString serial = asi.SerialNumber;
	INT unit = 0;
	INT capacity = 0;
	model.Replace(L"PLEXTOR PX-", L"");
	capacity = _wtoi(model);

	// Block Size: 2MB
	if (model.Find(L"M2") >= 0 || model.Find(L"M3") >= 0
	|| ((model.Find(L"M5P") >= 0 || model.Find(L"M5S")) && _wtoi(serial.Mid(1, 5)) <= 2247)
	)
	{
		if (capacity > 0)
		{
			unit = capacity / 2;
		}
	}
	// Block Size: 4MB
	else
	{
		if (capacity > 0)
		{
			unit = capacity;
		}
	}
	
	return unit;
}
*/

BOOL CAtaSmart::IsSsdOld(ATA_SMART_INFO &asi)
{
	return asi.Model.Find(_T("OCZ")) == 0 
		|| asi.Model.Find(_T("SPCC")) == 0
		|| asi.Model.Find(_T("PATRIOT")) == 0
		|| asi.Model.Find(_T("Solid")) >= 0
		|| asi.Model.Find(_T("SSD")) >= 0
		|| asi.Model.Find(_T("SiliconHardDisk")) >= 0
		|| asi.Model.Find(_T("PHOTOFAST")) == 0
		|| asi.Model.Find(_T("STT_FTM")) == 0
		|| asi.Model.Find(_T("Super Talent")) == 0
		;
}

BOOL CAtaSmart::IsSsdMtron(ATA_SMART_INFO &asi)
{
	return ((asi.Attribute[ 0].Id == 0xBB && asi.AttributeCount == 1) || asi.Model.Find(_T("MTRON")) == 0);
}

BOOL CAtaSmart::IsSsdJMicron60x(ATA_SMART_INFO &asi)
{
	BOOL flagSmartType = FALSE;

	if(asi.Attribute[ 0].Id == 0x0C
	&& asi.Attribute[ 1].Id == 0x09
	&& asi.Attribute[ 2].Id == 0xC2
	&& asi.Attribute[ 3].Id == 0xE5
	&& asi.Attribute[ 4].Id == 0xE8
	&& asi.Attribute[ 5].Id == 0xE9
//	&& asi.Attribute[ 6].Id == 0xEA
//	&& asi.Attribute[ 7].Id == 0xEB
	)
	{
		flagSmartType = TRUE;
	}

	return flagSmartType;
}

BOOL CAtaSmart::IsSsdJMicron61x(ATA_SMART_INFO &asi)
{
	BOOL flagSmartType = FALSE;

	if(asi.Attribute[ 0].Id == 0x01
	&& asi.Attribute[ 1].Id == 0x02
	&& asi.Attribute[ 2].Id == 0x03
	&& asi.Attribute[ 3].Id == 0x05
	&& asi.Attribute[ 4].Id == 0x07
	&& asi.Attribute[ 5].Id == 0x08
	&& asi.Attribute[ 6].Id == 0x09
	&& asi.Attribute[ 7].Id == 0x0A
	&& asi.Attribute[ 8].Id == 0x0C
	&& asi.Attribute[ 9].Id == 0xA8
	&& asi.Attribute[10].Id == 0xAF
	&& asi.Attribute[11].Id == 0xC0
	&& asi.Attribute[12].Id == 0xC2
//	&& asi.Attribute[13].Id == 0xF0
//	&& asi.Attribute[14].Id == 0xAA
//	&& asi.Attribute[15].Id == 0xAD
	)
	{
		flagSmartType = TRUE;
	}

	return flagSmartType;
}

BOOL CAtaSmart::IsSsdIndlinx(ATA_SMART_INFO &asi)
{
	BOOL flagSmartType = FALSE;

	if(asi.Attribute[ 0].Id == 0x01
	&& asi.Attribute[ 1].Id == 0x09
	&& asi.Attribute[ 2].Id == 0x0C
	&& asi.Attribute[ 3].Id == 0xB8
	&& asi.Attribute[ 4].Id == 0xC3
	&& asi.Attribute[ 5].Id == 0xC4
//	&& asi.Attribute[ 6].Id == 0xC5
//	&& asi.Attribute[ 7].Id == 0xC6
//	&& asi.Attribute[ 8].Id == 0xC7
//	&& asi.Attribute[ 9].Id == 0xC8
//	&& asi.Attribute[10].Id == 0xC9
//	&& asi.Attribute[11].Id == 0xCA
//	&& asi.Attribute[12].Id == 0xCB
//	&& asi.Attribute[13].Id == 0xCC
//	&& asi.Attribute[14].Id == 0xCD
//	&& asi.Attribute[15].Id == 0xCE
//	&& asi.Attribute[16].Id == 0xCF
//	&& asi.Attribute[17].Id == 0xD0
//	&& asi.Attribute[18].Id == 0xD1
//	&& asi.Attribute[19].Id == 0xD2
//	&& asi.Attribute[20].Id == 0xD3
	)
	{
		flagSmartType = TRUE;
	}

	/*
		asi.Model.Find(_T("OCZ-VERTEX")) == 0
	|| asi.Model.Find(_T("G-Monster-V3")) == 0
	|| asi.Model.Find(_T("G-Monster-V5")) == 0 
	|| (asi.Model.Find(_T("STT_FTM")) == 0 && asi.Model.Find(_T("GX")) > 0)
	|| asi.Model.Find(_T("Solidata")) == 0
	*/

	return flagSmartType;

}

BOOL CAtaSmart::IsSsdIntel(ATA_SMART_INFO &asi)
{
	BOOL flagSmartType = FALSE;

	if(asi.Attribute[ 0].Id == 0x03
	&& asi.Attribute[ 1].Id == 0x04
	&& asi.Attribute[ 2].Id == 0x05
	&& asi.Attribute[ 3].Id == 0x09
	&& asi.Attribute[ 4].Id == 0x0C
	)
	{
		if(asi.Attribute[ 5].Id == 0xC0 && asi.Attribute[ 6].Id == 0xE8 && asi.Attribute[ 7].Id == 0xE9)
		{
			flagSmartType = TRUE;
		}
		else if(asi.Attribute[ 5].Id == 0xC0 && asi.Attribute[ 6].Id == 0xE1)
		{
			flagSmartType = TRUE;
		}
		else if(asi.Attribute[ 5].Id == 0xAA && asi.Attribute[ 6].Id == 0xAB && asi.Attribute[ 7].Id == 0xAC)
		{
			flagSmartType = TRUE;
		}
	}

	return (asi.Model.Find(_T("INTEL")) >= 0 || flagSmartType == TRUE);
}


// http://www.samsung.com/us/business/oem-solutions/pdfs/General_NSSD_25_SATA_III_Spec_0.2.pdf
BOOL CAtaSmart::IsSsdSamsung(ATA_SMART_INFO &asi)
{
	BOOL flagSmartType = FALSE;

	// SM951
	if (asi.Attribute[0].Id == 0x05
		&& asi.Attribute[1].Id == 0x09
		&& asi.Attribute[2].Id == 0x0C
		&& asi.Attribute[3].Id == 0xAA
		&& asi.Attribute[4].Id == 0xAB
		&& asi.Attribute[5].Id == 0xAC
		&& asi.Attribute[6].Id == 0xAD
		&& asi.Attribute[7].Id == 0xAE
		&& asi.Attribute[8].Id == 0xB2
		&& asi.Attribute[9].Id == 0xB4
		)
	{
		flagSmartType = TRUE;
		asi.HostReadsWritesUnit = HOST_READS_WRITES_GB;
	}
	else if(asi.Attribute[ 0].Id == 0x09
	&& asi.Attribute[ 1].Id == 0x0C
	&& asi.Attribute[ 2].Id == 0xB2
	&& asi.Attribute[ 3].Id == 0xB3
	&& asi.Attribute[ 4].Id == 0xB4
	)
	{
		flagSmartType = TRUE;
	}
	else
	if(asi.Attribute[ 0].Id == 0x09
	&& asi.Attribute[ 1].Id == 0x0C
	&& asi.Attribute[ 2].Id == 0xB1
	&& asi.Attribute[ 3].Id == 0xB2
	&& asi.Attribute[ 4].Id == 0xB3
	&& asi.Attribute[ 5].Id == 0xB4
	&& asi.Attribute[ 6].Id == 0xB7
	)
	{
		flagSmartType = TRUE;
	}
	else 
	if(asi.Attribute[ 0].Id == 0x09
	&& asi.Attribute[ 1].Id == 0x0C
	&& asi.Attribute[ 2].Id == 0xAF
	&& asi.Attribute[ 3].Id == 0xB0
	&& asi.Attribute[ 4].Id == 0xB1
	&& asi.Attribute[ 5].Id == 0xB2
	&& asi.Attribute[ 6].Id == 0xB3
	&& asi.Attribute[ 7].Id == 0xB4
	)
	{
		flagSmartType = TRUE;
	}
	else 
	if(asi.Attribute[ 0].Id == 0x05
	&& asi.Attribute[ 1].Id == 0x09
	&& asi.Attribute[ 2].Id == 0x0C
	&& asi.Attribute[ 3].Id == 0xB1
	&& asi.Attribute[ 4].Id == 0xB3
	&& asi.Attribute[ 5].Id == 0xB5
	&& asi.Attribute[ 6].Id == 0xB6
	)
	{
		flagSmartType = TRUE;
	}

	return ((asi.Model.Find(_T("SAMSUNG")) >= 0 && asi.IsSsd) || (asi.Model.Find(_T("MZ-")) >= 0 && asi.IsSsd) || flagSmartType == TRUE);
}

BOOL CAtaSmart::IsSsdSandForce(ATA_SMART_INFO &asi)
{
	BOOL flagSmartType = FALSE;

	if(asi.Attribute[ 0].Id == 0x01
	&& asi.Attribute[ 1].Id == 0x05
	&& asi.Attribute[ 2].Id == 0x09
	&& asi.Attribute[ 3].Id == 0x0C
	&& asi.Attribute[ 4].Id == 0x0D
	&& asi.Attribute[ 5].Id == 0x64
	&& asi.Attribute[ 6].Id == 0xAA
	)
	{
		flagSmartType = TRUE;
	}

	if(asi.Attribute[ 0].Id == 0x01
	&& asi.Attribute[ 1].Id == 0x05
	&& asi.Attribute[ 2].Id == 0x09
	&& asi.Attribute[ 3].Id == 0x0C
	&& asi.Attribute[ 4].Id == 0xAB
	&& asi.Attribute[ 5].Id == 0xAC
	)
	{
		flagSmartType = TRUE;
	}

	// TOSHIBA + SandForce
	// https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=1116;id=diskinfo#1116
	// https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=1136;id=diskinfo#1136
	if(asi.Attribute[ 0].Id == 0x01
	&& asi.Attribute[ 1].Id == 0x02
	&& asi.Attribute[ 2].Id == 0x03
	&& asi.Attribute[ 3].Id == 0x05
	&& asi.Attribute[ 4].Id == 0x07
	&& asi.Attribute[ 5].Id == 0x08
	&& asi.Attribute[ 6].Id == 0x09
	&& asi.Attribute[ 7].Id == 0x0A
	&& asi.Attribute[ 8].Id == 0x0C
	&& asi.Attribute[ 9].Id == 0xA7
	&& asi.Attribute[10].Id == 0xA8
	&& asi.Attribute[11].Id == 0xA9
	&& asi.Attribute[12].Id == 0xAA
	&& asi.Attribute[13].Id == 0xAD
	&& asi.Attribute[14].Id == 0xAF
	&& asi.Attribute[15].Id == 0xB1
	)
	{
		flagSmartType = TRUE;
	}

	return (asi.Model.Find(_T("SandForce")) >= 0 || flagSmartType);
}

// Micron Crucial
BOOL CAtaSmart::IsSsdMicronMU02(ATA_SMART_INFO &asi)
{
	BOOL flagSmartType = FALSE;
	CString modelUpper = asi.Model;
	modelUpper.MakeUpper();


	// Crucial BX100 MU02 2015/11/26
	if (asi.Attribute[0].Id == 0x01
		&& asi.Attribute[1].Id == 0x05
		&& asi.Attribute[2].Id == 0x09
		&& asi.Attribute[3].Id == 0x0C
		&& asi.Attribute[4].Id == 0xA0
		&& asi.Attribute[5].Id == 0xA1
		&& asi.Attribute[6].Id == 0xA3
		&& asi.Attribute[7].Id == 0xA4
		&& asi.Attribute[8].Id == 0xA5
		&& asi.Attribute[9].Id == 0xA6
		&& asi.Attribute[10].Id == 0xA7
		)
	{
		flagSmartType = TRUE;
	}

	// Crucial BX200 MU01.4 2015/11/26
	if (asi.Attribute[0].Id == 0x01
		&& asi.Attribute[1].Id == 0x05
		&& asi.Attribute[2].Id == 0x09
		&& asi.Attribute[3].Id == 0x0C
		&& asi.Attribute[4].Id == 0xA0
		&& asi.Attribute[5].Id == 0xA1
		&& asi.Attribute[6].Id == 0xA3
		&& asi.Attribute[7].Id == 0x94
		&& asi.Attribute[8].Id == 0x95
		&& asi.Attribute[9].Id == 0x96
		&& asi.Attribute[10].Id == 0x97
		)
	{
		flagSmartType = TRUE;
	}

	return flagSmartType;
}

// Micron RealSSD & Crucial
BOOL CAtaSmart::IsSsdMicron(ATA_SMART_INFO &asi)
{
	BOOL flagSmartType = FALSE;
	CString modelUpper = asi.Model;
	modelUpper.MakeUpper();

	if(asi.Attribute[ 0].Id == 0x01
	&& asi.Attribute[ 1].Id == 0x05
	&& asi.Attribute[ 2].Id == 0x09
	&& asi.Attribute[ 3].Id == 0x0C
	&& asi.Attribute[ 4].Id == 0xAA
	&& asi.Attribute[ 5].Id == 0xAB
	&& asi.Attribute[ 6].Id == 0xAC
	&& asi.Attribute[ 7].Id == 0xAD
	&& asi.Attribute[ 8].Id == 0xAE
	&& asi.Attribute[ 9].Id == 0xB5
	&& asi.Attribute[10].Id == 0xB7
	)
	{
		flagSmartType = TRUE;
	}

	return modelUpper.Find(_T("P600")) == 0 || modelUpper.Find(_T("C600")) == 0
		|| modelUpper.Find(_T("M6-")) == 0 || modelUpper.Find(_T("M600")) == 0
		|| modelUpper.Find(_T("P500")) == 0 || modelUpper.Find(_T("C500")) == 0
		|| modelUpper.Find(_T("M5-")) == 0 || modelUpper.Find(_T("M500")) == 0
		|| modelUpper.Find(_T("P400")) == 0 || modelUpper.Find(_T("C400")) == 0
		|| modelUpper.Find(_T("M4-")) == 0 || modelUpper.Find(_T("M400")) == 0
		|| modelUpper.Find(_T("P300")) == 0 || modelUpper.Find(_T("C300")) == 0
		|| modelUpper.Find(_T("M3-")) == 0 || modelUpper.Find(_T("M300")) == 0
		|| modelUpper.Find(_T("CRUCIAL")) == 0 || modelUpper.Find(_T("MICRON")) == 0
		|| flagSmartType;
}

BOOL CAtaSmart::IsSsdOcz(ATA_SMART_INFO &asi)
{
	BOOL flagSmartType = FALSE;
	CString modelUpper = asi.Model;
	modelUpper.MakeUpper();

	// OCZ-TRION100 2015/11/25
	if (modelUpper.Find(_T("OCZ-TRION")) == 0)
	{
		flagSmartType = TRUE;
	}
	// 2012/3/11
	// OCZ-PETROL - https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=553;id=diskinfo#553
	// OCZ-OCTANE S2 - https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=577;id=diskinfo#577
	// OCZ-VERTEX 4 - http://imageshack.us/a/img269/7506/ocz2.png
	if(asi.Attribute[ 0].Id == 0x01
	&& asi.Attribute[ 1].Id == 0x03
	&& asi.Attribute[ 2].Id == 0x04
	&& asi.Attribute[ 3].Id == 0x05
	&& asi.Attribute[ 4].Id == 0x09
	&& asi.Attribute[ 5].Id == 0x0C
	&& asi.Attribute[ 6].Id == 0xE8
	&& asi.Attribute[ 7].Id == 0xE9
	)
	{
		flagSmartType = TRUE;
	}
	
	return (modelUpper.Find(_T("OCZ")) == 0 && flagSmartType);
}

BOOL CAtaSmart::IsSsdOczVector(ATA_SMART_INFO &asi)
{
	BOOL flagSmartType = FALSE;
	CString modelUpper = asi.Model;
	modelUpper.MakeUpper();

	/*
	// 2013/1/19
	// OCZ-VECTOR - https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=1031;id=diskinfo#1031
	if (asi.Attribute[0].Id == 0x05
		&& asi.Attribute[1].Id == 0x09
		&& asi.Attribute[2].Id == 0x0C
		&& asi.Attribute[3].Id == 0xAB
		&& asi.Attribute[4].Id == 0xAE
		&& asi.Attribute[5].Id == 0xBB
		&& asi.Attribute[6].Id == 0xC3
		&& asi.Attribute[7].Id == 0xC4
		)
	{
		flagSmartType = TRUE;
	}
	// 2013/3/24
	// OCZ-VECTOR - FW 2.0
	// https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=1185;id=diskinfo#1185
	if (asi.Attribute[0].Id == 0x05
		&& asi.Attribute[1].Id == 0x09
		&& asi.Attribute[2].Id == 0x0C
		&& asi.Attribute[3].Id == 0xAB
		&& asi.Attribute[4].Id == 0xAE
		&& asi.Attribute[5].Id == 0xC3
		&& asi.Attribute[6].Id == 0xC4
		)
	{
		flagSmartType = TRUE;
	}
	*/

	// 2015/11/25
	// PANASONIC RP-SSB240GAK
	// https://crystalmark.info/board/c-board.cgi?cmd=one;no=500;id=#500
	if (asi.Attribute[0].Id == 0x05
		&& asi.Attribute[1].Id == 0x09
		&& asi.Attribute[2].Id == 0x0C
		&& asi.Attribute[3].Id == 0xAB
		&& asi.Attribute[4].Id == 0xAE
		&& asi.Attribute[5].Id == 0xC3
		&& asi.Attribute[6].Id == 0xC4
		&& asi.Attribute[7].Id == 0xC5
		&& asi.Attribute[8].Id == 0xC6
		)
	{
		flagSmartType = TRUE;
	}
	if (modelUpper.Find(_T("PANASONIC RP-SSB")) == 0)
	{
		flagSmartType = TRUE;
	}
	
	return (modelUpper.Find(_T("OCZ")) == 0 || flagSmartType);
}

BOOL CAtaSmart::IsSsdPlextor(ATA_SMART_INFO &asi)
{
	BOOL flagSmartType = FALSE;

	// 2012/10/10
	// https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=739;id=diskinfo#739
	// https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=829;id=diskinfo#829
	if(asi.Attribute[ 0].Id == 0x01
	&& asi.Attribute[ 1].Id == 0x05
	&& asi.Attribute[ 2].Id == 0x09
	&& asi.Attribute[ 3].Id == 0x0C
	&& asi.Attribute[ 4].Id == 0xB1
	&& asi.Attribute[ 5].Id == 0xB2
	&& asi.Attribute[ 6].Id == 0xB5
	&& asi.Attribute[ 7].Id == 0xB6
	)
	{
		flagSmartType = TRUE;
	}

	// Added CFD's SSD
	return 	asi.Model.Find(_T("PLEXTOR")) == 0 || asi.Model.Find(_T("CSSD-S6T128NM3PQ")) == 0 || asi.Model.Find(_T("CSSD-S6T256NM3PQ")) == 0
		|| flagSmartType;
}

BOOL CAtaSmart::IsSsdSanDisk(ATA_SMART_INFO &asi)
{
	BOOL flagSmartType = FALSE;

	// 2013/10/7
	// https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=1425;id=diskinfo#1425
	if (asi.Model.Find(_T("SanDisk")) >= 0)
	{
		flagSmartType = TRUE;
		asi.SmartKeyName = _T("SmartSanDisk");

		for (int i = 0; i < 30; i++)
		{
			if (asi.Attribute[i].Id == 0xE9) // Total GB Written To NAND
			{
				asi.HostReadsWritesUnit = HOST_READS_WRITES_GB;
				asi.SmartKeyName = _T("SmartSanDiskGb");
			}
		}
	}

	return flagSmartType;
}

BOOL CAtaSmart::IsSsdKingston(ATA_SMART_INFO &asi)
{
	BOOL flagSmartType = FALSE;

	// 2015/11/29
	if (asi.Attribute[0].Id == 0x01
		&& asi.Attribute[1].Id == 0x02
		&& asi.Attribute[2].Id == 0x03
		&& asi.Attribute[3].Id == 0x05
		&& asi.Attribute[4].Id == 0x07
		&& asi.Attribute[5].Id == 0x08
		&& asi.Attribute[6].Id == 0x09
		&& asi.Attribute[7].Id == 0x0A
		&& asi.Attribute[8].Id == 0x0C
		&& asi.Attribute[9].Id == 0xA8
		)
	{
		flagSmartType = TRUE;
		asi.HostReadsWritesUnit = HOST_READS_WRITES_GB;
	}

	return flagSmartType;
}

BOOL CAtaSmart::IsSsdToshiba(ATA_SMART_INFO &asi)
{
	BOOL flagSmartType = FALSE;

	if (asi.Model.Find(_T("Toshiba")) >= 0 && asi.IsSsd)
	{
		flagSmartType = TRUE;
		if (asi.Model.Find(_T("THNSNC")) >= 0)
		{
			// TOSHIBA HG3
			// https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=1106;id=diskinfo#1106
		}
		else
		{
			asi.HostReadsWritesUnit = HOST_READS_WRITES_GB;
		}
	}

	return flagSmartType;
}

BOOL CAtaSmart::CheckSmartAttributeCorrect(ATA_SMART_INFO* asi1, ATA_SMART_INFO* asi2)
{
	if(asi1->AttributeCount != asi2->AttributeCount)
	{
		DebugPrint(_T("asi1->AttributeCount != asi2->AttributeCount"));
		return FALSE;
	}
	
	for(DWORD i = 0; i < asi1->AttributeCount; i++)
	{
		if(asi1->Attribute[i].Id != asi2->Attribute[i].Id)
		{
			DebugPrint(_T("asi1->Attribute[i].Id != asi2->Attribute[i].Id"));
			return FALSE;
		}
	}

	return TRUE; // Correct
}

VOID CAtaSmart::WakeUp(INT physicalDriveId)
{
	HANDLE hFile = INVALID_HANDLE_VALUE;
	CString	strDevice;
	if(physicalDriveId < 0)
	{
		return ;
	}

	strDevice.Format(_T("\\\\.\\PhysicalDrive%d"), physicalDriveId);
	hFile = ::CreateFile(strDevice, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		BYTE buf[512];
		const DWORD bufSize = 512;
		DWORD readSize = 0;
		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
		ReadFile(hFile, buf, bufSize, &readSize, NULL);
		CloseHandle(hFile);
	}
}

BOOL CAtaSmart::GetDiskInfo(INT physicalDriveId, INT scsiPort, INT scsiTargetId, 
	INTERFACE_TYPE interfaceType, COMMAND_TYPE commandType, VENDOR_ID usbVendorId, DWORD productId, INT scsiBus, DWORD siliconImageType, BOOL FlagNvidiaController, BOOL FlagMarvellController, CString pnpDeviceId
	)
{
	DebugPrint(_T("GetDiskInfo"));
	if(vars.GetCount() > MAX_DISK)
	{
		DebugPrint(_T("GetDiskInfo - FALSE0"));
		return FALSE;
	}
	// Check overlap
	for(int i = 0; i < vars.GetCount(); i++)
	{
		if(physicalDriveId >= 0 && vars[i].PhysicalDriveId == physicalDriveId)
		{
			DebugPrint(_T("GetDiskInfo - FALSE1"));
			return FALSE;
		}
		/*
		else if(scsiPort >= 0 && scsiTargetId >= 0
			&& vars[i].ScsiPort == scsiPort && vars[i].ScsiTargetId == scsiTargetId
			&& vars[i].ScsiBus == scsiBus
			)
		{
			DebugPrint(_T("GetDiskInfo - FALSE2"));
			return FALSE;
		}
		*/
	}

	IDENTIFY_DEVICE identify = {0};

	CString debug;
	if(interfaceType == INTERFACE_TYPE_UNKNOWN || interfaceType == INTERFACE_TYPE_PATA || interfaceType == INTERFACE_TYPE_SATA)
	{
		if(siliconImageType)
		{
			WakeUp(physicalDriveId);
			debug.Format(_T("physicalDriveId=%d, scsiPort=%d, scsiBus=%d, scsiTargetId=%d"), physicalDriveId, scsiPort, scsiBus, scsiTargetId);
			DebugPrint(debug);
			if(DoIdentifyDeviceSi(physicalDriveId, scsiPort, scsiBus, siliconImageType, &identify))
			{
				DebugPrint(_T("AddDisk - Si"));
				if(AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xA0, CMD_TYPE_SILICON_IMAGE, &identify, siliconImageType, NULL, pnpDeviceId))
				{
					return TRUE;
				}
				return FALSE;
			}
		}

		if(physicalDriveId >= 0)
		{
			debug.Format(_T("DoIdentifyDevicePd(%d, 0xA0) - 1"), physicalDriveId);
			DebugPrint(debug);
			if(! DoIdentifyDevicePd(physicalDriveId, 0xA0, &identify))
			{
				debug.Format(_T("WakeUp(%d)"), physicalDriveId);
				DebugPrint(debug);
				WakeUp(physicalDriveId);

				debug.Format(_T("DoIdentifyDevicePd(%d, 0xA0) - 2"), physicalDriveId);
				DebugPrint(debug);
				if(! DoIdentifyDevicePd(physicalDriveId, 0xA0, &identify))
				{
					debug.Format(_T("DoIdentifyDevicePd(%d, 0xB0) - 3"), physicalDriveId);
					DebugPrint(debug);

					if(! DoIdentifyDevicePd(physicalDriveId, 0xB0, &identify))
					{
						debug.Format(_T("DoIdentifyDeviceScsi(%d, %d) - 4"), scsiPort, scsiTargetId);
						DebugPrint(debug);

						if ((FlagNvidiaController || FlagMarvellController || IsAdvancedDiskSearch) && scsiPort >= 0 && scsiTargetId >= 0 && DoIdentifyDeviceScsi(scsiPort, scsiTargetId, &identify))
						{
							debug.Format(_T("AddDisk(%d, %d, %d) - 5"), physicalDriveId, scsiPort, scsiTargetId);
							DebugPrint(debug);
							return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xA0, CMD_TYPE_SCSI_MINIPORT, &identify, siliconImageType, NULL, pnpDeviceId);
						}
						else
						{
							return FALSE;
						}
					}
				}
			}
			debug.Format(_T("AddDisk(%d, %d, %d) - 6"), physicalDriveId, scsiPort, scsiTargetId);
			DebugPrint(debug);
			return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xA0, CMD_TYPE_PHYSICAL_DRIVE, &identify, siliconImageType, NULL, pnpDeviceId);
		}
		else
		{
			debug.Format(_T("DoIdentifyDeviceScsi(%d, %d) - 7"), scsiPort, scsiTargetId);
			DebugPrint(debug);
			if(scsiPort >= 0 && scsiTargetId >= 0 && DoIdentifyDeviceScsi(scsiPort, scsiTargetId, &identify))
			{
				debug.Format(_T("AddDisk(%d, %d, %d) - 8"), physicalDriveId, scsiPort, scsiTargetId);
				DebugPrint(debug);
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xA0, CMD_TYPE_SCSI_MINIPORT, &identify, siliconImageType, NULL, pnpDeviceId);
			}
		}
	}
	else if (interfaceType == INTERFACE_TYPE_NVME)
	{

		debug.Format(_T("DoIdentifyDeviceNVMeStorageQuery"));
		DebugPrint(debug);
		if (m_FlagNVMeStorageQuery && DoIdentifyDeviceNVMeStorageQuery(physicalDriveId, scsiPort, scsiTargetId, &identify))
		{
			debug.Format(_T("AddDiskNVMe - CMD_TYPE_NVME_STORAGE_QUERY"));
			DebugPrint(debug);
			return AddDiskNVMe(physicalDriveId, scsiPort, scsiTargetId, scsiBus, scsiTargetId, CMD_TYPE_NVME_STORAGE_QUERY, &identify);
		}

		debug.Format(_T("DoIdentifyDeviceNVMeSamsung"));
		DebugPrint(debug);
		if (DoIdentifyDeviceNVMeSamsung(physicalDriveId, scsiPort, scsiTargetId, &identify))
		{
			debug.Format(_T("AddDiskNVMe - CMD_TYPE_NVME_SAMSUNG"));
			DebugPrint(debug);
			return AddDiskNVMe(physicalDriveId, scsiPort, scsiTargetId, scsiBus, scsiTargetId, CMD_TYPE_NVME_SAMSUNG, &identify);
		}

		debug.Format(_T("DoIdentifyDeviceNVMeIntel"));
		DebugPrint(debug);

		if (DoIdentifyDeviceNVMeIntel(physicalDriveId, scsiPort, scsiTargetId, &identify))
		{
			debug.Format(_T("AddDiskNVMe - CMD_TYPE_NVME_INTEL"));
			DebugPrint(debug);
			return AddDiskNVMe(physicalDriveId, scsiPort, scsiTargetId, scsiBus, scsiTargetId, CMD_TYPE_NVME_INTEL, &identify);
		}
	}
	
	if(physicalDriveId >= 0)
	{
		/** DEBUG
		if(TRUE)
		{
			DoIdentifyDeviceSat(physicalDriveId, &identify, CMD_TYPE_DEBUG);
		}
		else
		*/

		WakeUp(physicalDriveId);

		if(interfaceType == INTERFACE_TYPE_USB && usbVendorId == USB_VENDOR_LOGITEC && productId == 0x00D9)
		{
			DebugPrint(_T("FALSE: usbVendorId == USB_VENDOR_LOGITEC && productId == 0x00D9"));
			return FALSE;
		}

		// 2009/7/30, 2009/8/21
		// IO-DATA HDPS-U  https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=2918;id=report#2918
		//                 https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=2978;id=report#2978
		//                 https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=2985;id=report#2985
		// 2012/3/20
		// StoreJet         http://btmn.jp/2012/02/25/storejet-320gb-crystal-diskinfo/
		if(interfaceType == INTERFACE_TYPE_USB && ((usbVendorId == USB_VENDOR_IO_DATA && productId == 0x0122) || usbVendorId == USB_VENDOR_JMICRON))
		{
			DebugPrint(_T("(usbVendorId == USB_VENDOR_IO_DATA && productId == 0x0122) || usbVendorId == USB_VENDOR_JMICRON"));
			if(FlagUsbJmicron && DoIdentifyDeviceSat(physicalDriveId, 0xA0, &identify,  CMD_TYPE_JMICRON))
			{
				DebugPrint(_T("AddDisk - USB0A"));
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xA0,  CMD_TYPE_JMICRON, &identify, siliconImageType, NULL, pnpDeviceId);
			}
			else if(FlagUsbSat && DoIdentifyDeviceSat(physicalDriveId, 0xA0, &identify, CMD_TYPE_SAT))
			{
				DebugPrint(_T("AddDisk - USB1A"));
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xA0, CMD_TYPE_SAT, &identify, siliconImageType, NULL, pnpDeviceId);
			}
			if(FlagUsbJmicron && DoIdentifyDeviceSat(physicalDriveId, 0xB0, &identify,  CMD_TYPE_JMICRON))
			{
				DebugPrint(_T("AddDisk - USB0B"));
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xB0,  CMD_TYPE_JMICRON, &identify, siliconImageType, NULL, pnpDeviceId);
			}
			else if(FlagUsbSat && DoIdentifyDeviceSat(physicalDriveId, 0xB0, &identify, CMD_TYPE_SAT))
			{
				DebugPrint(_T("AddDisk - USB1B"));
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xB0, CMD_TYPE_SAT, &identify, siliconImageType, NULL, pnpDeviceId);
			}
			else
			{
				DebugPrint(_T("FALSE - USB0"));
				return FALSE;
			}
		}

		if(interfaceType == INTERFACE_TYPE_USB && usbVendorId == USB_VENDOR_IO_DATA)
		{
			DebugPrint(_T("usbVendorId == USB_VENDOR_IO_DATA"));
			if(FlagUsbIodata && DoIdentifyDeviceSat(physicalDriveId, 0xA0, &identify, CMD_TYPE_IO_DATA))
			{
				DebugPrint(_T("AddDisk - USB2"));
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xA0, CMD_TYPE_IO_DATA, &identify, siliconImageType, NULL, pnpDeviceId);
			}
			else if(FlagUsbIodata && DoIdentifyDeviceSat(physicalDriveId, 0xB0, &identify, CMD_TYPE_IO_DATA))
			{
				DebugPrint(_T("AddDisk - USB3"));
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xB0, CMD_TYPE_IO_DATA, &identify, siliconImageType, NULL, pnpDeviceId);
			}
		}

/*
		else if(interfaceType == INTERFACE_TYPE_USB && vendorId == USB_VENDOR_LOGITEC && productId != 0x00D9)
		{
			if(DoIdentifyDeviceSat(physicalDriveId, &identify, CMD_TYPE_LOGITEC))
			{
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, CMD_TYPE_LOGITEC, &identify);
			}
		}
*/

		if(interfaceType == INTERFACE_TYPE_USB && usbVendorId == USB_VENDOR_SUNPLUS)
		{
			DebugPrint(_T("usbVendorId == USB_VENDOR_SUNPLUS"));
			if(FlagUsbSunplus && DoIdentifyDeviceSat(physicalDriveId, 0xA0, &identify, CMD_TYPE_SUNPLUS))
			{
				DebugPrint(_T("AddDisk - USB4"));
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xA0, CMD_TYPE_SUNPLUS, &identify, siliconImageType, NULL, pnpDeviceId);
			}
			else if(FlagUsbSunplus && DoIdentifyDeviceSat(physicalDriveId, 0xB0, &identify, CMD_TYPE_SUNPLUS))
			{
				DebugPrint(_T("AddDisk - USB5"));
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xB0, CMD_TYPE_SUNPLUS, &identify, siliconImageType, NULL, pnpDeviceId);
			}
		}
		else if(interfaceType == INTERFACE_TYPE_USB && usbVendorId == USB_VENDOR_CYPRESS)
		{
			DebugPrint(_T("usbVendorId == USB_VENDOR_CYPRESS"));
			if(FlagUsbCypress && DoIdentifyDeviceSat(physicalDriveId, 0xA0, &identify, CMD_TYPE_CYPRESS))
			{
				DebugPrint(_T("AddDisk - USB6"));
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xA0, CMD_TYPE_CYPRESS, &identify, siliconImageType, NULL, pnpDeviceId);
			}
			else if(FlagUsbCypress && DoIdentifyDeviceSat(physicalDriveId, 0xB0, &identify, CMD_TYPE_CYPRESS))
			{
				DebugPrint(_T("AddDisk - USB7"));
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xB0, CMD_TYPE_CYPRESS, &identify, siliconImageType, NULL, pnpDeviceId);
			}
		}
		else if(interfaceType == INTERFACE_TYPE_USB && 
			(usbVendorId == USB_VENDOR_INITIO || usbVendorId == USB_VENDOR_OXFORD)
			)
		{
			DebugPrint(_T("usbVendorId == USB_VENDOR_INITIO || usbVendorId == USB_VENDOR_OXFORD"));

			if(FlagUsbSat && DoIdentifyDeviceSat(physicalDriveId, 0xA0, &identify, CMD_TYPE_SAT))
			{
				DebugPrint(_T("AddDisk - USB8"));
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xA0, CMD_TYPE_SAT, &identify, siliconImageType, NULL, pnpDeviceId);
			}
			else if(FlagUsbSat && DoIdentifyDeviceSat(physicalDriveId, 0xB0, &identify, CMD_TYPE_SAT))
			{
				DebugPrint(_T("AddDisk - USB9"));
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xB0, CMD_TYPE_SAT, &identify, siliconImageType, NULL, pnpDeviceId);
			}
		}
		else
		{
			DebugPrint(_T("else (USB-HDD)"));

			if(FlagUsbSat && DoIdentifyDeviceSat(physicalDriveId, 0xA0, &identify, CMD_TYPE_SAT))
			{
				DebugPrint(_T("AddDisk - USB10"));
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xA0, CMD_TYPE_SAT, &identify, siliconImageType, NULL, pnpDeviceId);
			}
			else if(FlagUsbJmicron && DoIdentifyDeviceSat(physicalDriveId, 0xA0, &identify, CMD_TYPE_JMICRON))
			{
				DebugPrint(_T("AddDisk - USB11"));
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xA0, CMD_TYPE_JMICRON, &identify, siliconImageType, NULL, pnpDeviceId);
			}
			else if(FlagUsbSunplus && DoIdentifyDeviceSat(physicalDriveId, 0xA0, &identify, CMD_TYPE_SUNPLUS))
			{
				DebugPrint(_T("AddDisk - USB12"));
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xA0, CMD_TYPE_SUNPLUS, &identify, siliconImageType, NULL, pnpDeviceId);
			}
			else if(FlagUsbCypress && DoIdentifyDeviceSat(physicalDriveId, 0xA0, &identify, CMD_TYPE_CYPRESS))
			{
				DebugPrint(_T("AddDisk - USB13"));
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xA0, CMD_TYPE_CYPRESS, &identify, siliconImageType, NULL, pnpDeviceId);
			}
			else if(FlagUsbLogitec1 && DoIdentifyDeviceSat(physicalDriveId, 0xA0, &identify, CMD_TYPE_LOGITEC1))
			{
				DebugPrint(_T("AddDisk - USB14"));
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xA0, CMD_TYPE_LOGITEC1, &identify, siliconImageType, NULL, pnpDeviceId);
			}
			else if (FlagUsbLogitec2 && DoIdentifyDeviceSat(physicalDriveId, 0xA0, &identify, CMD_TYPE_LOGITEC2))
			{
				DebugPrint(_T("AddDisk - USB14"));
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xA0, CMD_TYPE_LOGITEC2, &identify, siliconImageType, NULL, pnpDeviceId);
			}
			else if(FlagUsbSat && DoIdentifyDeviceSat(physicalDriveId, 0xB0, &identify, CMD_TYPE_SAT))
			{
				DebugPrint(_T("AddDisk - USB15"));
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xB0, CMD_TYPE_SAT, &identify, siliconImageType, NULL, pnpDeviceId);
			}
			else if(FlagUsbJmicron && DoIdentifyDeviceSat(physicalDriveId, 0xB0, &identify, CMD_TYPE_JMICRON))
			{
				DebugPrint(_T("AddDisk - USB16"));
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xB0, CMD_TYPE_JMICRON, &identify, siliconImageType, NULL, pnpDeviceId);
			}
			else if(FlagUsbSunplus && DoIdentifyDeviceSat(physicalDriveId, 0xB0, &identify, CMD_TYPE_SUNPLUS))
			{
				DebugPrint(_T("AddDisk - USB17"));
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xB0, CMD_TYPE_SUNPLUS, &identify, siliconImageType, NULL, pnpDeviceId);
			}
			else if(FlagUsbCypress && DoIdentifyDeviceSat(physicalDriveId, 0xB0, &identify, CMD_TYPE_CYPRESS))
			{
				DebugPrint(_T("AddDisk - USB18"));
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xB0, CMD_TYPE_CYPRESS, &identify, siliconImageType, NULL, pnpDeviceId);
			}
			else if(FlagUsbLogitec1 && DoIdentifyDeviceSat(physicalDriveId, 0xB0, &identify, CMD_TYPE_LOGITEC1))
			{
				DebugPrint(_T("AddDisk - USB19"));
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xB0, CMD_TYPE_LOGITEC1, &identify, siliconImageType, NULL, pnpDeviceId);
			}
			else if (FlagUsbLogitec2 && DoIdentifyDeviceSat(physicalDriveId, 0xB0, &identify, CMD_TYPE_LOGITEC2))
			{
				DebugPrint(_T("AddDisk - USB20"));
				return AddDisk(physicalDriveId, scsiPort, scsiTargetId, scsiBus, 0xB0, CMD_TYPE_LOGITEC2, &identify, siliconImageType, NULL, pnpDeviceId);
			}
		}
	}

	return FALSE;
}

/*---------------------------------------------------------------------------*/
// \\\\.\\PhysicalDriveX
/*---------------------------------------------------------------------------*/
HANDLE CAtaSmart::GetIoCtrlHandle(BYTE index)
{
	CString	strDevice;
	strDevice.Format(_T("\\\\.\\PhysicalDrive%d"), index);

	return ::CreateFile(strDevice, GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, 0, NULL);
}

BOOL CAtaSmart::DoIdentifyDevicePd(INT physicalDriveId, BYTE target, IDENTIFY_DEVICE* data)
{
	BOOL	bRet = FALSE;
	HANDLE	hIoCtrl;
	DWORD	dwReturned;
	CString cstr;

	IDENTIFY_DEVICE_OUTDATA	sendCmdOutParam;
	SENDCMDINPARAMS	sendCmd;

	if(data == NULL)
	{
		return	FALSE;
	}

	if(m_FlagAtaPassThrough && m_FlagAtaPassThroughSmart)
	{
		DebugPrint(_T("SendAtaCommandPd - IDENTIFY_DEVICE (ATA_PASS_THROUGH)"));
		bRet = SendAtaCommandPd(physicalDriveId, target, 0xEC, 0x00, 0x00, (PBYTE)data, sizeof(IDENTIFY_DEVICE));
		cstr = data->A.Model;
	}

	if(bRet == FALSE || cstr.IsEmpty())
	{
		::ZeroMemory(data, sizeof(IDENTIFY_DEVICE));
		hIoCtrl = GetIoCtrlHandle(physicalDriveId);
		if(hIoCtrl == INVALID_HANDLE_VALUE)
		{
			return	FALSE;
		}
		::ZeroMemory(&sendCmdOutParam, sizeof(IDENTIFY_DEVICE_OUTDATA));
		::ZeroMemory(&sendCmd, sizeof(SENDCMDINPARAMS));

		sendCmd.irDriveRegs.bCommandReg			= ID_CMD;
		sendCmd.irDriveRegs.bSectorCountReg		= 1;
		sendCmd.irDriveRegs.bSectorNumberReg	= 1;
		sendCmd.irDriveRegs.bDriveHeadReg		= target;
		sendCmd.cBufferSize						= IDENTIFY_BUFFER_SIZE;

		DebugPrint(_T("SendAtaCommandPd - IDENTIFY_DEVICE"));
		bRet = ::DeviceIoControl(hIoCtrl, DFP_RECEIVE_DRIVE_DATA, 
			&sendCmd, sizeof(SENDCMDINPARAMS),
			&sendCmdOutParam, sizeof(IDENTIFY_DEVICE_OUTDATA),
			&dwReturned, NULL);

		::CloseHandle(hIoCtrl);
		
		if(bRet == FALSE || dwReturned != sizeof(IDENTIFY_DEVICE_OUTDATA))
		{
			return	FALSE;
		}

		memcpy_s(data, sizeof(IDENTIFY_DEVICE), sendCmdOutParam.SendCmdOutParam.bBuffer, sizeof(IDENTIFY_DEVICE));
	}

	return	TRUE;
}

BOOL CAtaSmart::GetSmartAttributePd(INT physicalDriveId, BYTE target, ATA_SMART_INFO* asi)
{
	BOOL	bRet = FALSE;
	HANDLE	hIoCtrl;
	DWORD	dwReturned;

	SMART_READ_DATA_OUTDATA	sendCmdOutParam;
	SENDCMDINPARAMS	sendCmd;

	if(m_FlagAtaPassThrough && m_FlagAtaPassThroughSmart)
	{
		DebugPrint(_T("SendAtaCommandPd - SMART_READ_DATA (ATA_PASS_THROUGH)"));
		bRet = SendAtaCommandPd(physicalDriveId, target, SMART_CMD, READ_ATTRIBUTES, 0x00, 
		(PBYTE)&(asi->SmartReadData), sizeof(asi->SmartReadData));
	}

	if(! bRet)
	{
		hIoCtrl = GetIoCtrlHandle(physicalDriveId);
		if(hIoCtrl == INVALID_HANDLE_VALUE)
		{
			return	FALSE;
		}

		::ZeroMemory(&sendCmdOutParam, sizeof(SMART_READ_DATA_OUTDATA));
		::ZeroMemory(&sendCmd, sizeof(SENDCMDINPARAMS));

		sendCmd.irDriveRegs.bFeaturesReg	= READ_ATTRIBUTES;
		sendCmd.irDriveRegs.bSectorCountReg = 1;
		sendCmd.irDriveRegs.bSectorNumberReg= 1;
		sendCmd.irDriveRegs.bCylLowReg		= SMART_CYL_LOW;
		sendCmd.irDriveRegs.bCylHighReg		= SMART_CYL_HI;
		sendCmd.irDriveRegs.bDriveHeadReg	= target;
		sendCmd.irDriveRegs.bCommandReg		= SMART_CMD;
		sendCmd.cBufferSize					= READ_ATTRIBUTE_BUFFER_SIZE;

		DebugPrint(_T("SendAtaCommandPd - SMART_READ_DATA"));
		bRet = ::DeviceIoControl(hIoCtrl, DFP_RECEIVE_DRIVE_DATA, 
			&sendCmd, sizeof(SENDCMDINPARAMS),
			&sendCmdOutParam, sizeof(SMART_READ_DATA_OUTDATA),
			&dwReturned, NULL);

		::CloseHandle(hIoCtrl);
	
		if(bRet == FALSE || dwReturned != sizeof(SMART_READ_DATA_OUTDATA))
		{
			return	FALSE;
		}

		memcpy_s(&(asi->SmartReadData), 512, &(sendCmdOutParam.SendCmdOutParam.bBuffer), 512);
	}

	return FillSmartData(asi);
}

BOOL CAtaSmart::GetSmartThresholdPd(INT physicalDriveId, BYTE target, ATA_SMART_INFO* asi)
{
	BOOL	bRet = FALSE;
	HANDLE	hIoCtrl = INVALID_HANDLE_VALUE;
	DWORD	dwReturned;

	SMART_READ_DATA_OUTDATA	sendCmdOutParam;
	SENDCMDINPARAMS	sendCmd;

	if(m_FlagAtaPassThrough && m_FlagAtaPassThroughSmart)
	{
		DebugPrint(_T("SendAtaCommandPd - SMART_READ_THRESHOLDS (ATA_PASS_THROUGH)"));
		bRet = SendAtaCommandPd(physicalDriveId, target, SMART_CMD, READ_THRESHOLDS, 0x00, 
			(PBYTE)&(asi->SmartReadThreshold), sizeof(asi->SmartReadThreshold));
	}

	if(! bRet)
	{
		hIoCtrl = GetIoCtrlHandle(physicalDriveId);
		if(hIoCtrl == INVALID_HANDLE_VALUE)
		{
			return	FALSE;
		}

		::ZeroMemory(&sendCmdOutParam, sizeof(SMART_READ_DATA_OUTDATA));
		::ZeroMemory(&sendCmd, sizeof(SENDCMDINPARAMS));

		sendCmd.irDriveRegs.bFeaturesReg	= READ_THRESHOLDS;
		sendCmd.irDriveRegs.bSectorCountReg = 1;
		sendCmd.irDriveRegs.bSectorNumberReg= 1;
		sendCmd.irDriveRegs.bCylLowReg		= SMART_CYL_LOW;
		sendCmd.irDriveRegs.bCylHighReg		= SMART_CYL_HI;
		sendCmd.irDriveRegs.bDriveHeadReg	= target;
		sendCmd.irDriveRegs.bCommandReg		= SMART_CMD;
		sendCmd.cBufferSize					= READ_THRESHOLD_BUFFER_SIZE;

		DebugPrint(_T("SendAtaCommandPd - SMART_READ_THRESHOLDS"));
		bRet = ::DeviceIoControl(hIoCtrl, DFP_RECEIVE_DRIVE_DATA, 
			&sendCmd, sizeof(SENDCMDINPARAMS),
			&sendCmdOutParam, sizeof(SMART_READ_DATA_OUTDATA),
			&dwReturned, NULL);

		::CloseHandle(hIoCtrl);
	
		if(bRet == FALSE || dwReturned != sizeof(SMART_READ_DATA_OUTDATA))
		{
			return	FALSE;
		}

		memcpy_s(&(asi->SmartReadThreshold), 512, &(sendCmdOutParam.SendCmdOutParam.bBuffer), 512);
	}
	
	return FillSmartThreshold(asi);
}

BOOL CAtaSmart::ControlSmartStatusPd(INT physicalDriveId, BYTE target, BYTE command)
{
	BOOL	bRet = FALSE;
	HANDLE	hIoCtrl;
	DWORD	dwReturned;

	SENDCMDINPARAMS		sendCmd;
	SENDCMDOUTPARAMS	sendCmdOutParam;

	if(m_FlagAtaPassThrough && m_FlagAtaPassThroughSmart)
	{
		DebugPrint(_T("SendAtaCommandPd - SMART_CONTROL_STATUS (ATA_PASS_THROUGH)"));
		bRet = SendAtaCommandPd(physicalDriveId, target, SMART_CMD, command, 0x00, NULL, 0);
	}

	if(! bRet)
	{
		DebugPrint(_T("SendAtaCommandPd - SMART_CONTROL_STATUS"));
		hIoCtrl = GetIoCtrlHandle(physicalDriveId);
		if(hIoCtrl == INVALID_HANDLE_VALUE)
		{
			return	FALSE;
		}

		::ZeroMemory(&sendCmd, sizeof(SENDCMDINPARAMS));
		::ZeroMemory(&sendCmdOutParam, sizeof(SENDCMDOUTPARAMS));

		sendCmd.irDriveRegs.bFeaturesReg	= command;
		sendCmd.irDriveRegs.bSectorCountReg = 1;
		sendCmd.irDriveRegs.bSectorNumberReg= 1;
		sendCmd.irDriveRegs.bCylLowReg		= SMART_CYL_LOW;
		sendCmd.irDriveRegs.bCylHighReg		= SMART_CYL_HI;
		sendCmd.irDriveRegs.bDriveHeadReg	= target;
		sendCmd.irDriveRegs.bCommandReg		= SMART_CMD;
		sendCmd.cBufferSize					= 0;

		bRet = ::DeviceIoControl(hIoCtrl, DFP_SEND_DRIVE_COMMAND, 
			&sendCmd, sizeof(SENDCMDINPARAMS) - 1,
			&sendCmdOutParam, sizeof(SENDCMDOUTPARAMS) -1,
			&dwReturned, NULL);

		::CloseHandle(hIoCtrl);
	}
	
	return	bRet;
}

BOOL CAtaSmart::SendAtaCommandPd(INT physicalDriveId, BYTE target, BYTE main, BYTE sub, BYTE param, PBYTE data, DWORD dataSize)
{
	BOOL	bRet;
	HANDLE	hIoCtrl;
	DWORD	dwReturned;

	hIoCtrl = GetIoCtrlHandle(physicalDriveId);
	if(hIoCtrl == INVALID_HANDLE_VALUE)
	{
		return	FALSE;
	}

	if(m_FlagAtaPassThrough)
	{
		ATA_PASS_THROUGH_EX_WITH_BUFFERS ab;
		::ZeroMemory(&ab, sizeof(ab));
		ab.Apt.Length = sizeof(ATA_PASS_THROUGH_EX);
		ab.Apt.TimeOutValue = 2;
		DWORD size = offsetof(ATA_PASS_THROUGH_EX_WITH_BUFFERS, Buf);
		ab.Apt.DataBufferOffset = size;

		if(dataSize > 0)
		{
			if(dataSize > sizeof(ab.Buf))
			{
				return FALSE;
			}
			ab.Apt.AtaFlags = ATA_FLAGS_DATA_IN;
			ab.Apt.DataTransferLength = dataSize;
			ab.Buf[0] = 0xCF; // magic number
			size += dataSize;
		}

		ab.Apt.CurrentTaskFile.bFeaturesReg = sub;
		ab.Apt.CurrentTaskFile.bSectorCountReg = param;
		ab.Apt.CurrentTaskFile.bDriveHeadReg = target;
		ab.Apt.CurrentTaskFile.bCommandReg = main;

		if(main == SMART_CMD)
		{
			ab.Apt.CurrentTaskFile.bCylLowReg		= SMART_CYL_LOW;
			ab.Apt.CurrentTaskFile.bCylHighReg		= SMART_CYL_HI;
			ab.Apt.CurrentTaskFile.bSectorCountReg  = 1;
			ab.Apt.CurrentTaskFile.bSectorNumberReg = 1;
		}

		bRet = ::DeviceIoControl(hIoCtrl, IOCTL_ATA_PASS_THROUGH,
			&ab, size, &ab, size, &dwReturned, NULL);
		::CloseHandle(hIoCtrl);
		if(bRet && dataSize && data != NULL)
		{
			memcpy_s(data, dataSize, ab.Buf, dataSize);
		}
	}
	else if(m_Os.dwMajorVersion <= 4)
	{
		return FALSE;
	}
	else
	{
		DWORD size = sizeof(CMD_IDE_PATH_THROUGH) - 1 + dataSize;
		CMD_IDE_PATH_THROUGH* buf = (CMD_IDE_PATH_THROUGH*)VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);

		buf->reg.bFeaturesReg		= sub;
		buf->reg.bSectorCountReg	= param;
		buf->reg.bSectorNumberReg	= 0;
		buf->reg.bCylLowReg			= 0;
		buf->reg.bCylHighReg		= 0;
		buf->reg.bDriveHeadReg		= target;
		buf->reg.bCommandReg		= main;
		buf->reg.bReserved		    = 0;
		buf->length					= dataSize;

		bRet = ::DeviceIoControl(hIoCtrl, IOCTL_IDE_PASS_THROUGH,
			buf, size, buf, size, &dwReturned, NULL);
		::CloseHandle(hIoCtrl);
		if(bRet && dataSize && data != NULL)
		{
			memcpy_s(data, dataSize, buf->buffer, dataSize);
		}
		VirtualFree(buf, 0, MEM_RELEASE);
	}

	return	bRet;
}

/*---------------------------------------------------------------------------*/
//  NVMe SAMSUNG
/*---------------------------------------------------------------------------*/

BOOL CAtaSmart::DoIdentifyDeviceNVMeSamsung(INT physicalDriveId, INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* data)
{
	BOOL	bRet;
	HANDLE	hIoCtrl;
	DWORD	dwReturned;
	DWORD	length;

	SCSI_PASS_THROUGH_WITH_BUFFERS24 sptwb;

	if (data == NULL)
	{
		return	FALSE;
	}

	::ZeroMemory(data, sizeof(IDENTIFY_DEVICE));

	hIoCtrl = GetIoCtrlHandle(physicalDriveId);

	if (hIoCtrl == INVALID_HANDLE_VALUE)
	{
		return	FALSE;
	}

	::ZeroMemory(&sptwb, sizeof(SCSI_PASS_THROUGH_WITH_BUFFERS24));

	sptwb.Spt.Length = sizeof(SCSI_PASS_THROUGH);
	sptwb.Spt.PathId = 0;
	sptwb.Spt.TargetId = 0;
	sptwb.Spt.Lun = 0;
	sptwb.Spt.SenseInfoLength = 24;
	sptwb.Spt.DataIn = SCSI_IOCTL_DATA_IN;
	sptwb.Spt.DataTransferLength = IDENTIFY_BUFFER_SIZE;
	sptwb.Spt.TimeOutValue = 2;
	sptwb.Spt.DataBufferOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS24, DataBuf);
	sptwb.Spt.SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS24, SenseBuf);

	sptwb.Spt.CdbLength = 16;
	sptwb.Spt.Cdb[0] = 0xB5; // SECURITY PROTOCOL IN
	sptwb.Spt.Cdb[1] = 0xFE; // SAMSUNG PROTOCOL
	sptwb.Spt.Cdb[2] = 0;
	sptwb.Spt.Cdb[3] = 5;
	sptwb.Spt.Cdb[4] = 0;
	sptwb.Spt.Cdb[5] = 0;
	sptwb.Spt.Cdb[6] = 0;
	sptwb.Spt.Cdb[7] = 0;
	sptwb.Spt.Cdb[8] = 0;
	sptwb.Spt.Cdb[9] = 0x40;
	sptwb.Spt.DataIn = SCSI_IOCTL_DATA_OUT;
	sptwb.DataBuf[0] = 1;


	length = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS24, DataBuf) + sptwb.Spt.DataTransferLength;

	bRet = ::DeviceIoControl(hIoCtrl, IOCTL_SCSI_PASS_THROUGH,
		&sptwb, length,
		&sptwb, length, &dwReturned, NULL);

	if (bRet == FALSE)
	{
		::CloseHandle(hIoCtrl);
		return	FALSE;
	}

	sptwb.Spt.CdbLength = 16;
	sptwb.Spt.Cdb[0] = 0xA2; // SECURITY PROTOCOL OUT
	sptwb.Spt.Cdb[1] = 0xFE; // SAMSUNG PROTOCOL
	sptwb.Spt.Cdb[2] = 0;
	sptwb.Spt.Cdb[3] = 5;
	sptwb.Spt.Cdb[4] = 0;
	sptwb.Spt.Cdb[5] = 0;
	sptwb.Spt.Cdb[6] = 0;
	sptwb.Spt.Cdb[7] = 0;
	sptwb.Spt.Cdb[8] = 1;
	sptwb.Spt.Cdb[9] = 0;

	sptwb.Spt.DataIn = SCSI_IOCTL_DATA_IN;
	sptwb.DataBuf[0] = 0;

	bRet = ::DeviceIoControl(hIoCtrl, IOCTL_SCSI_PASS_THROUGH,
		&sptwb, length,
		&sptwb, length, &dwReturned, NULL);

	if (bRet == FALSE)
	{
		::CloseHandle(hIoCtrl);
		return	FALSE;
	}

	DWORD count = 0;
	for (int i = 0; i < 512; i++)
	{
		count += sptwb.DataBuf[i];
	}
	if(count == 0)
	{
		::CloseHandle(hIoCtrl);
		return	FALSE;
	}

	memcpy_s(data, sizeof(IDENTIFY_DEVICE), sptwb.DataBuf, sizeof(IDENTIFY_DEVICE));

	::CloseHandle(hIoCtrl);

	return TRUE;
}

BOOL CAtaSmart::GetSmartAttributeNVMeSamsung(INT physicalDriveId, INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi)
{
	BOOL	bRet;
	HANDLE	hIoCtrl;
	DWORD	dwReturned;
	DWORD	length;

	SCSI_PASS_THROUGH_WITH_BUFFERS24 sptwb;

	hIoCtrl = GetIoCtrlHandle(physicalDriveId);


	if (hIoCtrl == INVALID_HANDLE_VALUE)
	{
		return	FALSE;
	}

	::ZeroMemory(&sptwb, sizeof(SCSI_PASS_THROUGH_WITH_BUFFERS24));

	sptwb.Spt.Length = sizeof(SCSI_PASS_THROUGH);
	sptwb.Spt.PathId = 0;
	sptwb.Spt.TargetId = 0;
	sptwb.Spt.Lun = 0;
	sptwb.Spt.SenseInfoLength = 24;
	sptwb.Spt.DataIn = SCSI_IOCTL_DATA_IN;
	sptwb.Spt.DataTransferLength = IDENTIFY_BUFFER_SIZE;
	sptwb.Spt.TimeOutValue = 2;
	sptwb.Spt.DataBufferOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS24, DataBuf);
	sptwb.Spt.SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS24, SenseBuf);

	sptwb.Spt.CdbLength = 16;
	sptwb.Spt.Cdb[0] = 0xB5; // SECURITY PROTOCOL IN
	sptwb.Spt.Cdb[1] = 0xFE; // SAMSUNG PROTOCOL
	sptwb.Spt.Cdb[2] = 0;
	sptwb.Spt.Cdb[3] = 6;
	sptwb.Spt.Cdb[4] = 0;
	sptwb.Spt.Cdb[5] = 0;
	sptwb.Spt.Cdb[6] = 0;
	sptwb.Spt.Cdb[7] = 0;
	sptwb.Spt.Cdb[8] = 0;
	sptwb.Spt.Cdb[9] = 0x40;

	sptwb.Spt.DataIn = SCSI_IOCTL_DATA_OUT;
	sptwb.DataBuf[0] = 2;
	sptwb.DataBuf[4] = 0xFF;
	sptwb.DataBuf[5] = 0xFF;
	sptwb.DataBuf[6] = 0xFF;
	sptwb.DataBuf[7] = 0xFF;

	length = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS24, DataBuf) + sptwb.Spt.DataTransferLength;

	bRet = ::DeviceIoControl(hIoCtrl, IOCTL_SCSI_PASS_THROUGH,
		&sptwb, length,
		&sptwb, length, &dwReturned, NULL);

	if (bRet == FALSE)
	{
		::CloseHandle(hIoCtrl);
		return	FALSE;
	}

	sptwb.Spt.CdbLength = 16;
	sptwb.Spt.Cdb[0] = 0xA2; // SECURITY PROTOCOL OUT
	sptwb.Spt.Cdb[1] = 0xFE; // SAMSUNG PROTOCOL
	sptwb.Spt.Cdb[2] = 0;
	sptwb.Spt.Cdb[3] = 6;
	sptwb.Spt.Cdb[4] = 0;
	sptwb.Spt.Cdb[5] = 0;
	sptwb.Spt.Cdb[6] = 0;
	sptwb.Spt.Cdb[7] = 0;
	sptwb.Spt.Cdb[8] = 1;
	sptwb.Spt.Cdb[9] = 0;

	sptwb.Spt.DataIn = SCSI_IOCTL_DATA_IN;
	sptwb.DataBuf[0] = 0;

	bRet = ::DeviceIoControl(hIoCtrl, IOCTL_SCSI_PASS_THROUGH,
		&sptwb, length,
		&sptwb, length, &dwReturned, NULL);

	if (bRet == FALSE)
	{
		::CloseHandle(hIoCtrl);
		return	FALSE;
	}

	DWORD count = 0;
	for (int i = 0; i < 512; i++)
	{
		count += sptwb.DataBuf[i];
	}
	if (count == 0)
	{
		::CloseHandle(hIoCtrl);
		return	FALSE;
	}

	memcpy_s(&(asi->SmartReadData), 512, sptwb.DataBuf, 512);

	::CloseHandle(hIoCtrl);

	return TRUE;
}

BOOL CAtaSmart::GetSmartAttributeNVMeSamsung951(INT physicalDriveId, INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi)
{
	CString path;
	path.Format(L"\\\\.\\PhysicalDrive%d", physicalDriveId);

	HANDLE hIoCtrl = CreateFile(GetScsiPath((TCHAR*)path.GetString()), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	BOOL bRet = 0;
	NVME_PASS_THROUGH_IOCTL nptwb;
	DWORD length = sizeof(nptwb);
	DWORD dwReturned;

	ZeroMemory(&nptwb, sizeof(NVME_PASS_THROUGH_IOCTL));

	nptwb.SrbIoCtrl.ControlCode = NVME_PASS_THROUGH_SRB_IO_CODE;
	nptwb.SrbIoCtrl.HeaderLength = sizeof(SRB_IO_CONTROL);
	memcpy((UCHAR*)(&nptwb.SrbIoCtrl.Signature[0]), NVME_SIG_STR, NVME_SIG_STR_LEN);
	nptwb.SrbIoCtrl.Timeout = NVME_PT_TIMEOUT;
	nptwb.SrbIoCtrl.Length = length - sizeof(SRB_IO_CONTROL);
	nptwb.DataBufferLen = sizeof(nptwb.DataBuffer);
	nptwb.ReturnBufferLen = sizeof(nptwb);
	nptwb.Direction = NVME_FROM_DEV_TO_HOST;

	nptwb.NVMeCmd[0] = 2;  // GetLogPage
	nptwb.NVMeCmd[1] = 0xFFFFFFFF;  // GetLogPage
	nptwb.NVMeCmd[10] = 0x00000002; // S,M.A.R.T.

	bRet = DeviceIoControl(hIoCtrl, IOCTL_SCSI_MINIPORT,
		&nptwb, length, &nptwb, length, &dwReturned, NULL);

	if (bRet == FALSE)
	{
		::CloseHandle(hIoCtrl);
		return	FALSE;
	}

	DWORD count = 0;
	for (int i = 0; i < 512; i++)
	{
		count += nptwb.DataBuffer[i];
	}
	if (count == 0)
	{
		::CloseHandle(hIoCtrl);
		return	FALSE;
	}

	memcpy_s(&(asi->SmartReadData), 512, nptwb.DataBuffer, 512);

	CloseHandle(hIoCtrl);
	return bRet;
}


/*---------------------------------------------------------------------------*/
// NVMe Intel 
// Reference: http://naraeon.net/en/archives/1126
/*---------------------------------------------------------------------------*/

CString CAtaSmart::GetScsiPath(const TCHAR* Path)
{
	HANDLE hIoCtrl = CreateFile(Path, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	SCSI_ADDRESS sadr;
	BOOL bRet = 0;
	DWORD dwReturned;

	bRet = DeviceIoControl(hIoCtrl, IOCTL_SCSI_GET_ADDRESS,
		NULL, 0, &sadr, sizeof(sadr), &dwReturned, NULL);

	CString result;
	result.Format(L"\\\\.\\SCSI%d:", sadr.PortNumber);

	CloseHandle(hIoCtrl);
	return result;
}

BOOL CAtaSmart::DoIdentifyDeviceNVMeIntel(INT physicalDriveId, INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* data)
{
	CString path;
	path.Format(L"\\\\.\\PhysicalDrive%d", physicalDriveId);

	HANDLE hIoCtrl = CreateFile(GetScsiPath((TCHAR*)path.GetString()), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	BOOL bRet = 0;
	NVME_PASS_THROUGH_IOCTL nptwb;
	DWORD length = sizeof(nptwb);
	DWORD dwReturned;

	ZeroMemory(&nptwb, sizeof(NVME_PASS_THROUGH_IOCTL));

	nptwb.SrbIoCtrl.ControlCode = NVME_PASS_THROUGH_SRB_IO_CODE;
	nptwb.SrbIoCtrl.HeaderLength = sizeof(SRB_IO_CONTROL);
	memcpy((UCHAR*)(&nptwb.SrbIoCtrl.Signature[0]), NVME_SIG_STR, NVME_SIG_STR_LEN);
	nptwb.SrbIoCtrl.Timeout = NVME_PT_TIMEOUT;
	nptwb.SrbIoCtrl.Length = length - sizeof(SRB_IO_CONTROL);
	nptwb.DataBufferLen = sizeof(nptwb.DataBuffer);
	nptwb.ReturnBufferLen = sizeof(nptwb);
	nptwb.Direction = NVME_FROM_DEV_TO_HOST;

	nptwb.NVMeCmd[0] = 6;  // Identify
	nptwb.NVMeCmd[10] = 1; // Return to Host

	bRet = DeviceIoControl(hIoCtrl, IOCTL_SCSI_MINIPORT,
		&nptwb, length, &nptwb, length, &dwReturned, NULL);

	if (bRet == FALSE)
	{
		::CloseHandle(hIoCtrl);
		return	FALSE;
	}

	DWORD count = 0;
	for (int i = 0; i < 512; i++)
	{
		count += nptwb.DataBuffer[i];
	}
	if (count == 0)
	{
		::CloseHandle(hIoCtrl);
		return	FALSE;
	}

	memcpy_s(data, sizeof(IDENTIFY_DEVICE), nptwb.DataBuffer, sizeof(IDENTIFY_DEVICE));

	CloseHandle(hIoCtrl);
	return bRet;
}

BOOL CAtaSmart::GetSmartAttributeNVMeIntel(INT physicalDriveId, INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi)
{
	CString path;
	path.Format(L"\\\\.\\PhysicalDrive%d", physicalDriveId);

	HANDLE hIoCtrl = CreateFile(GetScsiPath((TCHAR*)path.GetString()), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	BOOL bRet = 0;
	NVME_PASS_THROUGH_IOCTL nptwb;
	DWORD length = sizeof(nptwb);
	DWORD dwReturned;

	ZeroMemory(&nptwb, sizeof(NVME_PASS_THROUGH_IOCTL));

	nptwb.SrbIoCtrl.ControlCode = NVME_PASS_THROUGH_SRB_IO_CODE;
	nptwb.SrbIoCtrl.HeaderLength = sizeof(SRB_IO_CONTROL);
	memcpy((UCHAR*)(&nptwb.SrbIoCtrl.Signature[0]), NVME_SIG_STR, NVME_SIG_STR_LEN);
	nptwb.SrbIoCtrl.Timeout = NVME_PT_TIMEOUT;
	nptwb.SrbIoCtrl.Length = length - sizeof(SRB_IO_CONTROL);
	nptwb.DataBufferLen = sizeof(nptwb.DataBuffer);
	nptwb.ReturnBufferLen = sizeof(nptwb);
	nptwb.Direction = NVME_FROM_DEV_TO_HOST;

	nptwb.NVMeCmd[0] = 2;  // GetLogPage
	nptwb.NVMeCmd[1] = 0xFFFFFFFF;  // GetLogPage
	nptwb.NVMeCmd[10] = 0x007f0002; // 

	bRet = DeviceIoControl(hIoCtrl, IOCTL_SCSI_MINIPORT,
		&nptwb, length, &nptwb, length, &dwReturned, NULL);

	if (bRet == FALSE)
	{
		::CloseHandle(hIoCtrl);
		return	FALSE;
	}

	DWORD count = 0;
	for (int i = 0; i < 512; i++)
	{
		count += nptwb.DataBuffer[i];
	}
	if (count == 0)
	{
		::CloseHandle(hIoCtrl);
		return	FALSE;
	}

	memcpy_s(&(asi->SmartReadData), 512, nptwb.DataBuffer, 512);

	CloseHandle(hIoCtrl);
	return bRet;
}

/*---------------------------------------------------------------------------*/
// NVMe Storage Query
// Reference: http://naraeon.net/en/archives/1338
/*---------------------------------------------------------------------------*/

BOOL CAtaSmart::DoIdentifyDeviceNVMeStorageQuery(INT physicalDriveId, INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* data)
{
	CString path;
	path.Format(L"\\\\.\\PhysicalDrive%d", physicalDriveId);

	HANDLE hIoCtrl = CreateFile(path, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	StorageQuery::TStorageQueryWithBuffer nptwb;
	BOOL bRet = 0;
	ZeroMemory(&nptwb, sizeof(nptwb));

	nptwb.ProtocolSpecific.ProtocolType = StorageQuery::ProtocolTypeNvme;
	nptwb.ProtocolSpecific.DataType = StorageQuery::NVMeDataTypeIdentify;
	nptwb.ProtocolSpecific.ProtocolDataOffset = sizeof(StorageQuery::TStorageProtocolSpecificData);
	nptwb.ProtocolSpecific.ProtocolDataLength = 4096;
	nptwb.Query.PropertyId = StorageQuery::StorageAdapterProtocolSpecificProperty;
	nptwb.Query.QueryType = StorageQuery::PropertyStandardQuery;
	DWORD dwReturned = 0;

	bRet = DeviceIoControl(hIoCtrl, IOCTL_STORAGE_QUERY_PROPERTY,
		&nptwb, sizeof(nptwb), &nptwb, sizeof(nptwb), &dwReturned, NULL);
	CloseHandle(hIoCtrl);

	memcpy_s(data, sizeof(IDENTIFY_DEVICE), nptwb.Buffer, sizeof(IDENTIFY_DEVICE));

	return bRet;
}

BOOL CAtaSmart::GetSmartAttributeNVMeStorageQuery(INT physicalDriveId, INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi)
{
	CString path;
	path.Format(L"\\\\.\\PhysicalDrive%d", physicalDriveId);

	HANDLE hIoCtrl = CreateFile(path, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	BOOL bRet = 0;

	StorageQuery::TStorageQueryWithBuffer nptwb;
	ZeroMemory(&nptwb, sizeof(nptwb));

	nptwb.ProtocolSpecific.ProtocolType = StorageQuery::ProtocolTypeNvme;
	nptwb.ProtocolSpecific.DataType = StorageQuery::NVMeDataTypeLogPage;
	nptwb.ProtocolSpecific.ProtocolDataRequestValue = 2; // SMART Health Information
	nptwb.ProtocolSpecific.ProtocolDataRequestSubValue = 0xFFFFFFFF;
	nptwb.ProtocolSpecific.ProtocolDataOffset = sizeof(StorageQuery::TStorageProtocolSpecificData);
	nptwb.ProtocolSpecific.ProtocolDataLength = 4096;
	nptwb.Query.PropertyId = StorageQuery::StorageAdapterProtocolSpecificProperty;
	nptwb.Query.QueryType = StorageQuery::PropertyStandardQuery;
	DWORD dwReturned = 0;

	bRet = DeviceIoControl(hIoCtrl, IOCTL_STORAGE_QUERY_PROPERTY,
		&nptwb, sizeof(nptwb), &nptwb, sizeof(nptwb), &dwReturned, NULL);
	CloseHandle(hIoCtrl);

	memcpy_s(&(asi->SmartReadData), 512, nptwb.Buffer, 512);

	return bRet;
}


/*---------------------------------------------------------------------------*/
//  \\\\.\\ScsiX
/*---------------------------------------------------------------------------*/

BOOL CAtaSmart::DoIdentifyDeviceScsi(INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* data)
{
	int done = FALSE;
	int controller = 0;
	int current = 0;
	HANDLE hScsiDriveIOCTL = 0;
	CString driveName;
	driveName.Format(_T("\\\\.\\Scsi%d:"), scsiPort);
	hScsiDriveIOCTL = CreateFile(driveName, GENERIC_READ | GENERIC_WRITE,
							FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if(hScsiDriveIOCTL != INVALID_HANDLE_VALUE)
	{
		BYTE buffer[sizeof(SRB_IO_CONTROL) + sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE];
		SRB_IO_CONTROL *p = (SRB_IO_CONTROL *)buffer;
		SENDCMDINPARAMS *pin = (SENDCMDINPARAMS *)(buffer + sizeof(SRB_IO_CONTROL));
		DWORD dummy;
		
		memset(buffer, 0, sizeof(buffer));
		p->HeaderLength = sizeof (SRB_IO_CONTROL);
		p->Timeout = 2;
		p->Length = sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE;
		p->ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
		memcpy((char *)p->Signature, "SCSIDISK", 8);
		pin->irDriveRegs.bCommandReg = ID_CMD;
		pin->bDriveNumber = scsiTargetId;
		
		if(DeviceIoControl(hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT, 
								buffer, sizeof(SRB_IO_CONTROL) + sizeof(SENDCMDINPARAMS) - 1,
								buffer, sizeof(SRB_IO_CONTROL) + sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE,
								&dummy, NULL))
		{
			SENDCMDOUTPARAMS *pOut = (SENDCMDOUTPARAMS *)(buffer + sizeof(SRB_IO_CONTROL));
			if(*(pOut->bBuffer) > 0)
			{
				done = TRUE;
				memcpy_s(data, sizeof(IDENTIFY_DEVICE), pOut->bBuffer, sizeof(IDENTIFY_DEVICE));
			}
		}
		CloseHandle(hScsiDriveIOCTL);
	}
	return done;
}

BOOL CAtaSmart::GetSmartAttributeScsi(INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi)
{
	HANDLE hScsiDriveIOCTL = 0;
	CString driveName;
	driveName.Format(_T("\\\\.\\Scsi%d:"), scsiPort);
	hScsiDriveIOCTL = CreateFile(driveName, GENERIC_READ | GENERIC_WRITE,
							FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if(hScsiDriveIOCTL != INVALID_HANDLE_VALUE)
	{
		BYTE buffer[sizeof(SRB_IO_CONTROL) + sizeof(SENDCMDOUTPARAMS) + READ_ATTRIBUTE_BUFFER_SIZE];
		SRB_IO_CONTROL *p = (SRB_IO_CONTROL *)buffer;
		SENDCMDINPARAMS *pin = (SENDCMDINPARAMS *)(buffer + sizeof(SRB_IO_CONTROL));
		DWORD dummy;
		memset(buffer, 0, sizeof(buffer));
		p->HeaderLength = sizeof(SRB_IO_CONTROL);
		p->Timeout = 2;
		p->Length = sizeof(SENDCMDOUTPARAMS) + READ_ATTRIBUTE_BUFFER_SIZE;
		p->ControlCode = IOCTL_SCSI_MINIPORT_READ_SMART_ATTRIBS;
		memcpy((char *)p->Signature, "SCSIDISK", 8);
		pin->irDriveRegs.bFeaturesReg		= READ_ATTRIBUTES;
		pin->irDriveRegs.bSectorCountReg	= 1;
		pin->irDriveRegs.bSectorNumberReg	= 1;
		pin->irDriveRegs.bCylLowReg			= SMART_CYL_LOW;
		pin->irDriveRegs.bCylHighReg		= SMART_CYL_HI;
		pin->irDriveRegs.bCommandReg		= SMART_CMD;
		pin->cBufferSize					= READ_ATTRIBUTE_BUFFER_SIZE;
		pin->bDriveNumber					= scsiTargetId;

		if(DeviceIoControl(hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT, 
								buffer, sizeof(SRB_IO_CONTROL) + sizeof(SENDCMDINPARAMS) - 1,
								buffer, sizeof(SRB_IO_CONTROL) + sizeof(SENDCMDOUTPARAMS) + READ_ATTRIBUTE_BUFFER_SIZE,
								&dummy, NULL))
		{
			SENDCMDOUTPARAMS *pOut = (SENDCMDOUTPARAMS *)(buffer + sizeof(SRB_IO_CONTROL));

			memcpy_s(&(asi->SmartReadData), 512, &(pOut->bBuffer), 512);
			CloseHandle(hScsiDriveIOCTL);
			return FillSmartData(asi);
		}
	}
	return FALSE;
}

BOOL CAtaSmart::GetSmartThresholdScsi(INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi)
{
	HANDLE hScsiDriveIOCTL = 0;
	CString driveName;
	driveName.Format(_T("\\\\.\\Scsi%d:"), scsiPort);
	hScsiDriveIOCTL = CreateFile(driveName, GENERIC_READ | GENERIC_WRITE,
							FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if(hScsiDriveIOCTL != INVALID_HANDLE_VALUE)
	{
		BYTE buffer[sizeof(SRB_IO_CONTROL) + sizeof(SENDCMDOUTPARAMS) + READ_THRESHOLD_BUFFER_SIZE];
		SRB_IO_CONTROL *p = (SRB_IO_CONTROL *)buffer;
		SENDCMDINPARAMS *pin = (SENDCMDINPARAMS *)(buffer + sizeof(SRB_IO_CONTROL));
		DWORD dummy;
		memset(buffer, 0, sizeof(buffer));
		p->HeaderLength = sizeof(SRB_IO_CONTROL);
		p->Timeout = 2;
		p->Length = sizeof(SENDCMDOUTPARAMS) + READ_THRESHOLD_BUFFER_SIZE;
		p->ControlCode = IOCTL_SCSI_MINIPORT_READ_SMART_THRESHOLDS;
		memcpy((char *)p->Signature, "SCSIDISK", 8);
		pin->irDriveRegs.bFeaturesReg		= READ_THRESHOLDS;
		pin->irDriveRegs.bSectorCountReg	= 1;
		pin->irDriveRegs.bSectorNumberReg	= 1;
		pin->irDriveRegs.bCylLowReg			= SMART_CYL_LOW;
		pin->irDriveRegs.bCylHighReg		= SMART_CYL_HI;
		pin->irDriveRegs.bCommandReg		= SMART_CMD;
		pin->cBufferSize					= READ_THRESHOLD_BUFFER_SIZE;
		pin->bDriveNumber					= scsiTargetId;

		if(DeviceIoControl(hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT, 
								buffer, sizeof(SRB_IO_CONTROL) + sizeof(SENDCMDINPARAMS) - 1,
								buffer, sizeof(SRB_IO_CONTROL) + sizeof(SENDCMDOUTPARAMS) + READ_THRESHOLD_BUFFER_SIZE,
								&dummy, NULL))
		{
			SENDCMDOUTPARAMS *pOut = (SENDCMDOUTPARAMS *)(buffer + sizeof(SRB_IO_CONTROL));
			if(*(pOut->bBuffer) > 0)
			{
				
				memcpy_s(&(asi->SmartReadThreshold), 512, &(pOut->bBuffer), 512);
				CloseHandle (hScsiDriveIOCTL);
				return FillSmartThreshold(asi);
			}
		}
	}
	return FALSE;
}

BOOL CAtaSmart::ControlSmartStatusScsi(INT scsiPort, INT scsiTargetId, BYTE command)
{
	BOOL	bRet;
	HANDLE hScsiDriveIOCTL = 0;
	CString driveName;
	driveName.Format(_T("\\\\.\\Scsi%d:"), scsiPort);
	hScsiDriveIOCTL = CreateFile(driveName, GENERIC_READ | GENERIC_WRITE,
							FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if(hScsiDriveIOCTL != INVALID_HANDLE_VALUE)
	{
		BYTE buffer[sizeof(SRB_IO_CONTROL) + sizeof(SENDCMDOUTPARAMS) + SCSI_MINIPORT_BUFFER_SIZE];
		SRB_IO_CONTROL *p = (SRB_IO_CONTROL *)buffer;
		SENDCMDINPARAMS *pin = (SENDCMDINPARAMS *)(buffer + sizeof(SRB_IO_CONTROL));
		DWORD dummy;
		memset(buffer, 0, sizeof(buffer));
		p->HeaderLength = sizeof(SRB_IO_CONTROL);
		p->Timeout = 2;
		p->Length = sizeof(SENDCMDOUTPARAMS) + SCSI_MINIPORT_BUFFER_SIZE;
		if(command == DISABLE_SMART)
		{
			p->ControlCode = IOCTL_SCSI_MINIPORT_DISABLE_SMART;
		}
		else
		{
			p->ControlCode = IOCTL_SCSI_MINIPORT_ENABLE_SMART;
		}
		memcpy((char *)p->Signature, "SCSIDISK", 8);
		pin->irDriveRegs.bFeaturesReg		= command;
		pin->irDriveRegs.bSectorCountReg	= 1;
		pin->irDriveRegs.bSectorNumberReg	= 1;
		pin->irDriveRegs.bCylLowReg			= SMART_CYL_LOW;
		pin->irDriveRegs.bCylHighReg		= SMART_CYL_HI;
		pin->irDriveRegs.bCommandReg		= SMART_CMD;
		pin->cBufferSize					= SCSI_MINIPORT_BUFFER_SIZE;
		pin->bDriveNumber					= scsiTargetId;

		bRet = DeviceIoControl(hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT, 
								buffer, sizeof(SRB_IO_CONTROL) + sizeof(SENDCMDINPARAMS) - 1,
								buffer, sizeof(SRB_IO_CONTROL) + sizeof(SENDCMDOUTPARAMS) + SCSI_MINIPORT_BUFFER_SIZE,
								&dummy, NULL);
		CloseHandle(hScsiDriveIOCTL);
		return bRet;
	}

	return FALSE;
}


BOOL CAtaSmart::SendAtaCommandScsi(INT scsiPort, INT scsiTargetId, BYTE main, BYTE sub, BYTE param)
{
/** Does not work...
	BOOL	bRet;
	HANDLE hScsiDriveIOCTL = 0;
	CString driveName;
	driveName.Format(_T("\\\\.\\Scsi%d:"), scsiPort);
	hScsiDriveIOCTL = CreateFile(driveName, GENERIC_READ | GENERIC_WRITE,
							FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if(hScsiDriveIOCTL != INVALID_HANDLE_VALUE)
	{
		DWORD dummy;
		CMD_ATA_PASS_THROUGH_WITH_BUFFERS capt;
		::ZeroMemory(&capt, sizeof(CMD_ATA_PASS_THROUGH_WITH_BUFFERS));
		capt.apt.Length = sizeof(CMD_ATA_PASS_THROUGH);
		capt.apt.PathId = 0;
		capt.apt.TargetId = 0;
		capt.apt.Lun = 0;
		capt.apt.TimeOutValue = 10;

		DWORD size = offsetof(CMD_ATA_PASS_THROUGH_WITH_BUFFERS, DataBuf);
		capt.apt.DataBufferOffset = size;

		capt.apt.AtaFlags = 0x02;
		capt.apt.DataTransferLength = 512;
		size += 512;
		capt.DataBuf[0] = 0xCF;

		capt.apt.CurrentTaskFile.bFeaturesReg= sub;
		capt.apt.CurrentTaskFile.bSectorCountReg = param;
		capt.apt.CurrentTaskFile.bDriveHeadReg = 0xA0;
		capt.apt.CurrentTaskFile.bCommandReg = main;

		bRet = DeviceIoControl(hScsiDriveIOCTL, IOCTL_ATA_PASS_THROUGH, 
								&capt, size,
								&capt, size,
								&dummy, NULL);
	}
	CloseHandle(hScsiDriveIOCTL);

	return bRet;
*/
	return FALSE;
}

/*---------------------------------------------------------------------------*/
//  SCSI / ATA Translation (SAT) 
/*---------------------------------------------------------------------------*/

BOOL CAtaSmart::DoIdentifyDeviceSat(INT physicalDriveId, BYTE target, IDENTIFY_DEVICE* data, COMMAND_TYPE type)
{
	BOOL	bRet;
	HANDLE	hIoCtrl;
	DWORD	dwReturned;
	DWORD	length;

	SCSI_PASS_THROUGH_WITH_BUFFERS sptwb;

	if (data == NULL)
	{
		return	FALSE;
	}

	::ZeroMemory(data, sizeof(IDENTIFY_DEVICE));

	hIoCtrl = GetIoCtrlHandle(physicalDriveId);
	if (hIoCtrl == INVALID_HANDLE_VALUE)
	{
		return	FALSE;
	}

	::ZeroMemory(&sptwb, sizeof(SCSI_PASS_THROUGH_WITH_BUFFERS));

	sptwb.Spt.Length = sizeof(SCSI_PASS_THROUGH);
	sptwb.Spt.PathId = 0;
	sptwb.Spt.TargetId = 0;
	sptwb.Spt.Lun = 0;
	sptwb.Spt.SenseInfoLength = 24;
	sptwb.Spt.DataIn = SCSI_IOCTL_DATA_IN;
	sptwb.Spt.DataTransferLength = IDENTIFY_BUFFER_SIZE;
	sptwb.Spt.TimeOutValue = 2;
	sptwb.Spt.DataBufferOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, DataBuf);
	sptwb.Spt.SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, SenseBuf);

	// DEBUG
	//	CString cstr;
	//	cstr.Format(_T("DoIdentifyDevice TYPE=%d"), (DWORD)type);
	//	DebugPrint(cstr);

	if (type == CMD_TYPE_SAT)
	{
		sptwb.Spt.CdbLength = 12;
		sptwb.Spt.Cdb[0] = 0xA1;//ATA PASS THROUGH(12) OPERATION CODE(A1h)
		sptwb.Spt.Cdb[1] = (4 << 1) | 0; //MULTIPLE_COUNT=0,PROTOCOL=4(PIO Data-In),Reserved
		sptwb.Spt.Cdb[2] = (1 << 3) | (1 << 2) | 2;//OFF_LINE=0,CK_COND=0,Reserved=0,T_DIR=1(ToDevice),BYTE_BLOCK=1,T_LENGTH=2
		sptwb.Spt.Cdb[3] = 0;//FEATURES (7:0)
		sptwb.Spt.Cdb[4] = 1;//SECTOR_COUNT (7:0)
		sptwb.Spt.Cdb[5] = 0;//LBA_LOW (7:0)
		sptwb.Spt.Cdb[6] = 0;//LBA_MID (7:0)
		sptwb.Spt.Cdb[7] = 0;//LBA_HIGH (7:0)
		sptwb.Spt.Cdb[8] = target;
		sptwb.Spt.Cdb[9] = ID_CMD;//COMMAND
	}
	else if (type == CMD_TYPE_SUNPLUS)
	{
		sptwb.Spt.CdbLength = 12;
		sptwb.Spt.Cdb[0] = 0xF8;
		sptwb.Spt.Cdb[1] = 0x00;
		sptwb.Spt.Cdb[2] = 0x22;
		sptwb.Spt.Cdb[3] = 0x10;
		sptwb.Spt.Cdb[4] = 0x01;
		sptwb.Spt.Cdb[5] = 0x00;
		sptwb.Spt.Cdb[6] = 0x01;
		sptwb.Spt.Cdb[7] = 0x00;
		sptwb.Spt.Cdb[8] = 0x00;
		sptwb.Spt.Cdb[9] = 0x00;
		sptwb.Spt.Cdb[10] = target;
		sptwb.Spt.Cdb[11] = 0xEC; // ID_CMD
	}
	else if (type == CMD_TYPE_IO_DATA)
	{
		sptwb.Spt.CdbLength = 12;
		sptwb.Spt.Cdb[0] = 0xE3;
		sptwb.Spt.Cdb[1] = 0x00;
		sptwb.Spt.Cdb[2] = 0x00;
		sptwb.Spt.Cdb[3] = 0x01;
		sptwb.Spt.Cdb[4] = 0x01;
		sptwb.Spt.Cdb[5] = 0x00;
		sptwb.Spt.Cdb[6] = 0x00;
		sptwb.Spt.Cdb[7] = target;
		sptwb.Spt.Cdb[8] = 0xEC;  // ID_CMD
		sptwb.Spt.Cdb[9] = 0x00;
		sptwb.Spt.Cdb[10] = 0x00;
		sptwb.Spt.Cdb[11] = 0x00;
	}
	else if (type == CMD_TYPE_LOGITEC1)
	{
		sptwb.Spt.CdbLength = 10;
		sptwb.Spt.Cdb[0] = 0xE0;
		sptwb.Spt.Cdb[1] = 0x00;
		sptwb.Spt.Cdb[2] = 0x00;
		sptwb.Spt.Cdb[3] = 0x00;
		sptwb.Spt.Cdb[4] = 0x00;
		sptwb.Spt.Cdb[5] = 0x00;
		sptwb.Spt.Cdb[6] = 0x00;
		sptwb.Spt.Cdb[7] = target;
		sptwb.Spt.Cdb[8] = 0xEC;  // ID_CMD
		sptwb.Spt.Cdb[9] = 0x4C;
	}
	else if (type == CMD_TYPE_LOGITEC2)
	{
		sptwb.Spt.CdbLength = 16;
		sptwb.Spt.Cdb[0] = 0xD8;
		sptwb.Spt.Cdb[1] = 0x15;
		sptwb.Spt.Cdb[2] = 0x00;
		sptwb.Spt.Cdb[3] = 0x00;
		sptwb.Spt.Cdb[4] = 0x06;
		sptwb.Spt.Cdb[5] = 0x7B;
		sptwb.Spt.Cdb[6] = 0x00;
		sptwb.Spt.Cdb[7] = 0x00;
		sptwb.Spt.Cdb[8] = 0x02;
		sptwb.Spt.Cdb[9] = 0x00;
		sptwb.Spt.Cdb[10] = 0x01;
		sptwb.Spt.Cdb[11] = 0x00;
		sptwb.Spt.Cdb[12] = 0x00;
		sptwb.Spt.Cdb[13] = 0x00;
		sptwb.Spt.Cdb[14] = target; // 
		sptwb.Spt.Cdb[15] = 0xEC; // ID_CMD
	}
	else if (type == CMD_TYPE_JMICRON)
	{
		sptwb.Spt.CdbLength = 12;
		sptwb.Spt.Cdb[0] = 0xDF;
		sptwb.Spt.Cdb[1] = 0x10;
		sptwb.Spt.Cdb[2] = 0x00;
		sptwb.Spt.Cdb[3] = 0x02;
		sptwb.Spt.Cdb[4] = 0x00;
		sptwb.Spt.Cdb[5] = 0x00;
		sptwb.Spt.Cdb[6] = 0x01;
		sptwb.Spt.Cdb[7] = 0x00;
		sptwb.Spt.Cdb[8] = 0x00;
		sptwb.Spt.Cdb[9] = 0x00;
		sptwb.Spt.Cdb[10] = target;
		sptwb.Spt.Cdb[11] = 0xEC; // ID_CMD
	}
	else if (type == CMD_TYPE_CYPRESS)
	{
		sptwb.Spt.CdbLength = 16;
		sptwb.Spt.Cdb[0] = 0x24;
		sptwb.Spt.Cdb[1] = 0x24;
		sptwb.Spt.Cdb[2] = 0x00;
		sptwb.Spt.Cdb[3] = 0xBE;
		sptwb.Spt.Cdb[4] = 0x01;
		sptwb.Spt.Cdb[5] = 0x00;
		sptwb.Spt.Cdb[6] = 0x00;
		sptwb.Spt.Cdb[7] = 0x01;
		sptwb.Spt.Cdb[8] = 0x00;
		sptwb.Spt.Cdb[9] = 0x00;
		sptwb.Spt.Cdb[10] = 0x00;
		sptwb.Spt.Cdb[11] = target;
		sptwb.Spt.Cdb[12] = 0xEC; // ID_CMD
		sptwb.Spt.Cdb[13] = 0x00;
		sptwb.Spt.Cdb[14] = 0x00;
		sptwb.Spt.Cdb[15] = 0x00;
	}
	/*
	else if(type == CMD_TYPE_DEBUG)
	{
	sptwb.Spt.CdbLength = 16;
	for(int i = 0xA0; i <= 0xFF; i++)
	{
	for(int j = 8; j < 16; j++)
	{
	::ZeroMemory(&sptwb.Spt.Cdb, 16);
	sptwb.Spt.Cdb[0] = i;
	sptwb.Spt.Cdb[j - 1] = 0xA0;
	sptwb.Spt.Cdb[j] = 0xEC;

	length = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, DataBuf) + sptwb.Spt.DataTransferLength;

	bRet = ::DeviceIoControl(hIoCtrl, IOCTL_SCSI_PASS_THROUGH,
	&sptwb, sizeof(SCSI_PASS_THROUGH),
	&sptwb, length,	&dwReturned, NULL);


	if(bRet == FALSE || dwReturned != length)
	{
	continue;
	}

	CString cstr;
	cstr.Format(_T("i = %d, j = %d"), i, j);
	AfxMessageBox(cstr);

	::CloseHandle(hIoCtrl);
	memcpy_s(data, sizeof(IDENTIFY_DEVICE), sptwb.DataBuf, sizeof(IDENTIFY_DEVICE));

	return TRUE;
	}
	}
	}
	*/
	else
	{
		return FALSE;
	}

	length = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, DataBuf) + sptwb.Spt.DataTransferLength;

	bRet = ::DeviceIoControl(hIoCtrl, IOCTL_SCSI_PASS_THROUGH,
		&sptwb, sizeof(SCSI_PASS_THROUGH),
		&sptwb, length, &dwReturned, NULL);

	::CloseHandle(hIoCtrl);

	if (bRet == FALSE || dwReturned != length)
	{
		return	FALSE;
	}

	memcpy_s(data, sizeof(IDENTIFY_DEVICE), sptwb.DataBuf, sizeof(IDENTIFY_DEVICE));

	return	TRUE;
}

BOOL CAtaSmart::GetSmartAttributeSat(INT PhysicalDriveId, BYTE target, ATA_SMART_INFO* asi)
{
	BOOL	bRet;
	HANDLE	hIoCtrl;
	DWORD	dwReturned;
	DWORD length;

	SCSI_PASS_THROUGH_WITH_BUFFERS sptwb;

	hIoCtrl = GetIoCtrlHandle(PhysicalDriveId);
	if(hIoCtrl == INVALID_HANDLE_VALUE)
	{
		return	FALSE;
	}

	::ZeroMemory(&sptwb,sizeof(SCSI_PASS_THROUGH_WITH_BUFFERS));

	sptwb.Spt.Length = sizeof(SCSI_PASS_THROUGH);
	sptwb.Spt.PathId = 0;
	sptwb.Spt.TargetId = 0;
	sptwb.Spt.Lun = 0;
	sptwb.Spt.SenseInfoLength = 24;
	sptwb.Spt.DataIn = SCSI_IOCTL_DATA_IN;
	sptwb.Spt.DataTransferLength = READ_ATTRIBUTE_BUFFER_SIZE;
	sptwb.Spt.TimeOutValue = 2;
	sptwb.Spt.DataBufferOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, DataBuf);
	sptwb.Spt.SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, SenseBuf);

// DEBUG
//	CString cstr;
//	cstr.Format(_T("SmartAttribute TYPE=%d"), asi->CommandType);
//	DebugPrint(cstr);

	COMMAND_TYPE type = asi->CommandType;
	if(type == CMD_TYPE_SAT)
	{
		sptwb.Spt.CdbLength = 12;
		sptwb.Spt.Cdb[0] = 0xA1;//ATA PASS THROUGH(12) OPERATION CODE(A1h)
		sptwb.Spt.Cdb[1] = (4 << 1) | 0; //MULTIPLE_COUNT=0,PROTOCOL=4(PIO Data-In),Reserved
		sptwb.Spt.Cdb[2] = (1 << 3) | (1 << 2) | 2;//OFF_LINE=0,CK_COND=0,Reserved=0,T_DIR=1(ToDevice),BYTE_BLOCK=1,T_LENGTH=2
		sptwb.Spt.Cdb[3] = READ_ATTRIBUTES;//FEATURES (7:0)
		sptwb.Spt.Cdb[4] = 1;//SECTOR_COUNT (7:0)
		sptwb.Spt.Cdb[5] = 1;//LBA_LOW (7:0)
		sptwb.Spt.Cdb[6] = SMART_CYL_LOW;//LBA_MID (7:0)
		sptwb.Spt.Cdb[7] = SMART_CYL_HI;//LBA_HIGH (7:0)
		sptwb.Spt.Cdb[8] = target;
		sptwb.Spt.Cdb[9] = SMART_CMD;//COMMAND
	}
	else if(type == CMD_TYPE_SUNPLUS)
	{
		sptwb.Spt.CdbLength = 12;
		sptwb.Spt.Cdb[0] = 0xF8;
		sptwb.Spt.Cdb[1] = 0x00;
		sptwb.Spt.Cdb[2] = 0x22;
		sptwb.Spt.Cdb[3] = 0x10;
		sptwb.Spt.Cdb[4] = 0x01;
		sptwb.Spt.Cdb[5] = 0xD0;  // READ_ATTRIBUTES
		sptwb.Spt.Cdb[6] = 0x01; 
		sptwb.Spt.Cdb[7] = 0x00; 
		sptwb.Spt.Cdb[8] = 0x4F;  // SMART_CYL_LOW 
		sptwb.Spt.Cdb[9] = 0xC2;  // SMART_CYL_HIGH
		sptwb.Spt.Cdb[10] = target; 
		sptwb.Spt.Cdb[11] = 0xB0; // SMART_CMD
	}
	else if(type == CMD_TYPE_IO_DATA)
	{
		sptwb.Spt.CdbLength = 12;
		sptwb.Spt.Cdb[0] = 0xE3;
		sptwb.Spt.Cdb[1] = 0x00; // ?
		sptwb.Spt.Cdb[2] = 0xD0; // READ_ATTRIBUTES
		sptwb.Spt.Cdb[3] = 0x00; // ?
		sptwb.Spt.Cdb[4] = 0x00; // ?
		sptwb.Spt.Cdb[5] = 0x4F; // SMART_CYL_LOW
		sptwb.Spt.Cdb[6] = 0xC2; // SMART_CYL_HIGH
		sptwb.Spt.Cdb[7] = target; // 
		sptwb.Spt.Cdb[8] = 0xB0; // SMART_CMD
		sptwb.Spt.Cdb[9] = 0x00;  
		sptwb.Spt.Cdb[10] = 0x00; 
		sptwb.Spt.Cdb[11] = 0x00;
	}
	else if(type == CMD_TYPE_LOGITEC1)
	{
		sptwb.Spt.CdbLength = 10;
		sptwb.Spt.Cdb[0] = 0xE0;
		sptwb.Spt.Cdb[1] = 0x00;
		sptwb.Spt.Cdb[2] = 0xD0; // READ_ATTRIBUTES
		sptwb.Spt.Cdb[3] = 0x00; 
		sptwb.Spt.Cdb[4] = 0x00;
		sptwb.Spt.Cdb[5] = 0x4F; // SMART_CYL_LOW
		sptwb.Spt.Cdb[6] = 0xC2; // SMART_CYL_HIGH
		sptwb.Spt.Cdb[7] = target; 
		sptwb.Spt.Cdb[8] = 0xB0; // SMART_CMD
		sptwb.Spt.Cdb[9] = 0x4C;
	}
	else if (type == CMD_TYPE_LOGITEC2)
	{
		sptwb.Spt.CdbLength = 16;
		sptwb.Spt.Cdb[0] = 0xD8;
		sptwb.Spt.Cdb[1] = 0x15;
		sptwb.Spt.Cdb[2] = 0x00;
		sptwb.Spt.Cdb[3] = 0xD0; // READ_ATTRIBUTES
		sptwb.Spt.Cdb[4] = 0x06;
		sptwb.Spt.Cdb[5] = 0x7B;
		sptwb.Spt.Cdb[6] = 0x00;
		sptwb.Spt.Cdb[7] = 0x00;
		sptwb.Spt.Cdb[8] = 0x02;
		sptwb.Spt.Cdb[9] = 0x00;
		sptwb.Spt.Cdb[10] = 0x01;
		sptwb.Spt.Cdb[11] = 0x00;
		sptwb.Spt.Cdb[12] = 0x4F; // SMART_CYL_LOW
		sptwb.Spt.Cdb[13] = 0xC2; // SMART_CYL_HIGH
		sptwb.Spt.Cdb[14] = target; // 
		sptwb.Spt.Cdb[15] = 0xB0;// SMART_CMD
	}
	else if(type == CMD_TYPE_JMICRON)
	{
		sptwb.Spt.CdbLength = 12;
		sptwb.Spt.Cdb[0] = 0xDF;
		sptwb.Spt.Cdb[1] = 0x10;
		sptwb.Spt.Cdb[2] = 0x00;
		sptwb.Spt.Cdb[3] = 0x02;
		sptwb.Spt.Cdb[4] = 0x00;
		sptwb.Spt.Cdb[5] = 0xD0;  // READ_ATTRIBUTES
		sptwb.Spt.Cdb[6] = 0x01; 
		sptwb.Spt.Cdb[7] = 0x01; 
		sptwb.Spt.Cdb[8] = 0x4F;  // SMART_CYL_LOW
		sptwb.Spt.Cdb[9] = 0xC2;  // SMART_CYL_HIGH
		sptwb.Spt.Cdb[10] = target; 
		sptwb.Spt.Cdb[11] = 0xB0; // SMART_CMD
	}
	else if(type == CMD_TYPE_CYPRESS)
	{
		sptwb.Spt.CdbLength = 16;
		sptwb.Spt.Cdb[0] = 0x24;
		sptwb.Spt.Cdb[1] = 0x24;
		sptwb.Spt.Cdb[2] = 0x00;
		sptwb.Spt.Cdb[3] = 0xBE;
		sptwb.Spt.Cdb[4] = 0x01;
		sptwb.Spt.Cdb[5] = 0x00; 
		sptwb.Spt.Cdb[6] = 0xD0;  // READ_ATTRIBUTES
		sptwb.Spt.Cdb[7] = 0x00; 
		sptwb.Spt.Cdb[8] = 0x00;
		sptwb.Spt.Cdb[9] = 0x4F;  // SMART_CYL_LOW
		sptwb.Spt.Cdb[10] = 0xC2; // SMART_CYL_HIGH
		sptwb.Spt.Cdb[11] = target;
		sptwb.Spt.Cdb[12] = 0xB0; // ID_CMD
		sptwb.Spt.Cdb[13] = 0x00;
		sptwb.Spt.Cdb[14] = 0x00;
		sptwb.Spt.Cdb[15] = 0x00;
	}
	else
	{
		return FALSE;
	}

	length = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, DataBuf) + sptwb.Spt.DataTransferLength;
	bRet = ::DeviceIoControl(hIoCtrl, IOCTL_SCSI_PASS_THROUGH, 
		&sptwb, sizeof(SCSI_PASS_THROUGH),
		&sptwb, length,	&dwReturned, NULL);

	::CloseHandle(hIoCtrl);
	
	if(bRet == FALSE || dwReturned != length)
	{
		return	FALSE;
	}

	memcpy_s(&(asi->SmartReadData), 512, &(sptwb.DataBuf), 512);
	return FillSmartData(asi);
}

BOOL CAtaSmart::GetSmartThresholdSat(INT physicalDriveId, BYTE target, ATA_SMART_INFO* asi)
{
	BOOL	bRet;
	HANDLE	hIoCtrl;
	DWORD	dwReturned;
	DWORD length;

	SCSI_PASS_THROUGH_WITH_BUFFERS sptwb;

	hIoCtrl = GetIoCtrlHandle(physicalDriveId);
	if(hIoCtrl == INVALID_HANDLE_VALUE)
	{
		return	FALSE;
	}

	::ZeroMemory(&sptwb,sizeof(SCSI_PASS_THROUGH_WITH_BUFFERS));

	sptwb.Spt.Length = sizeof(SCSI_PASS_THROUGH);
	sptwb.Spt.PathId = 0;
	sptwb.Spt.TargetId = 0;
	sptwb.Spt.Lun = 0;
	sptwb.Spt.SenseInfoLength = 24;
	sptwb.Spt.DataIn = SCSI_IOCTL_DATA_IN;
	sptwb.Spt.DataTransferLength = READ_THRESHOLD_BUFFER_SIZE;
	sptwb.Spt.TimeOutValue = 2;
	sptwb.Spt.DataBufferOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, DataBuf);
	sptwb.Spt.SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, SenseBuf);

// DEBUG
//	CString cstr;
//	cstr.Format(_T("SmartThreshold TYPE=%d"), asi->CommandType);
//	DebugPrint(cstr);

	COMMAND_TYPE type = asi->CommandType;
	if(type == CMD_TYPE_SAT)
	{
		sptwb.Spt.CdbLength = 12;
		sptwb.Spt.Cdb[0] = 0xA1; ////ATA PASS THROUGH(12) OPERATION CODE (A1h)
		sptwb.Spt.Cdb[1] = (4 << 1) | 0; //MULTIPLE_COUNT=0,PROTOCOL=4(PIO Data-In),Reserved
		sptwb.Spt.Cdb[2] = (1 << 3) | (1 << 2) | 2;//OFF_LINE=0,CK_COND=0,Reserved=0,T_DIR=1(ToDevice),BYTE_BLOCK=1,T_LENGTH=2
		sptwb.Spt.Cdb[3] = READ_THRESHOLDS;//FEATURES (7:0)
		sptwb.Spt.Cdb[4] = 1;//SECTOR_COUNT (7:0)
		sptwb.Spt.Cdb[5] = 1;//LBA_LOW (7:0)
		sptwb.Spt.Cdb[6] = SMART_CYL_LOW;//LBA_MID (7:0)
		sptwb.Spt.Cdb[7] = SMART_CYL_HI;//LBA_HIGH (7:0)
		sptwb.Spt.Cdb[8] = target;
		sptwb.Spt.Cdb[9] = SMART_CMD;//COMMAND
	}
	else if(type == CMD_TYPE_SUNPLUS)
	{
		sptwb.Spt.CdbLength = 12;
		sptwb.Spt.Cdb[0] = 0xF8;
		sptwb.Spt.Cdb[1] = 0x00;
		sptwb.Spt.Cdb[2] = 0x22;
		sptwb.Spt.Cdb[3] = 0x10;
		sptwb.Spt.Cdb[4] = 0x01;
		sptwb.Spt.Cdb[5] = 0xD1; // READ_THRESHOLD
		sptwb.Spt.Cdb[6] = 0x01; 
		sptwb.Spt.Cdb[7] = 0x01; 
		sptwb.Spt.Cdb[8] = 0x4F; // SMART_CYL_LOW
		sptwb.Spt.Cdb[9] = 0xC2; // SMART_CYL_HIGH
		sptwb.Spt.Cdb[10] = target; 
		sptwb.Spt.Cdb[11] = 0xB0;// SMART_CMD
	}
	else if(type == CMD_TYPE_IO_DATA)
	{
		sptwb.Spt.CdbLength = 12;
		sptwb.Spt.Cdb[0] = 0xE3;
		sptwb.Spt.Cdb[1] = 0x00; // ?
		sptwb.Spt.Cdb[2] = 0xD1; // READ_THRESHOLD
		sptwb.Spt.Cdb[3] = 0x00; // ?
		sptwb.Spt.Cdb[4] = 0x00; // ?
		sptwb.Spt.Cdb[5] = 0x4F; // SMART_CYL_LOW 
		sptwb.Spt.Cdb[6] = 0xC2; // SMART_CYL_HIGH
		sptwb.Spt.Cdb[7] = target; // 
		sptwb.Spt.Cdb[8] = 0xB0; // SMART_CMD
		sptwb.Spt.Cdb[9] = 0x00;  
		sptwb.Spt.Cdb[10] = 0x00; 
		sptwb.Spt.Cdb[11] = 0x00;
	}
	else if(type == CMD_TYPE_LOGITEC1)
	{
		sptwb.Spt.CdbLength = 10;
		sptwb.Spt.Cdb[0] = 0xE0;
		sptwb.Spt.Cdb[1] = 0x00;
		sptwb.Spt.Cdb[2] = 0xD1; // READ_THRESHOLD
		sptwb.Spt.Cdb[3] = 0x00;
		sptwb.Spt.Cdb[4] = 0x00;
		sptwb.Spt.Cdb[5] = 0x4F; // SMART_CYL_LOW
		sptwb.Spt.Cdb[6] = 0xC2; // SMART_CYL_HIGH
		sptwb.Spt.Cdb[7] = target; 
		sptwb.Spt.Cdb[8] = 0xB0; // SMART_CMD
		sptwb.Spt.Cdb[9] = 0x4C;
	}
	else if (type == CMD_TYPE_LOGITEC2)
	{
		sptwb.Spt.CdbLength = 16;
		sptwb.Spt.Cdb[0] = 0xD8;
		sptwb.Spt.Cdb[1] = 0x15;
		sptwb.Spt.Cdb[2] = 0x00;
		sptwb.Spt.Cdb[3] = 0xD1; // READ_THRESHOLD
		sptwb.Spt.Cdb[4] = 0x06;
		sptwb.Spt.Cdb[5] = 0x7B;
		sptwb.Spt.Cdb[6] = 0x00;
		sptwb.Spt.Cdb[7] = 0x00;
		sptwb.Spt.Cdb[8] = 0x02;
		sptwb.Spt.Cdb[9] = 0x00;
		sptwb.Spt.Cdb[10]= 0x01;
		sptwb.Spt.Cdb[11]= 0x01;
		sptwb.Spt.Cdb[12]= 0x4F; // SMART_CYL_LOW
		sptwb.Spt.Cdb[13]= 0xC2; // SMART_CYL_HIGH
		sptwb.Spt.Cdb[14]= target; // 
		sptwb.Spt.Cdb[15]= 0xB0;// SMART_CMD
	}
	else if(type == CMD_TYPE_JMICRON)
	{
		sptwb.Spt.CdbLength = 12;
		sptwb.Spt.Cdb[0] = 0xDF;
		sptwb.Spt.Cdb[1] = 0x10;
		sptwb.Spt.Cdb[2] = 0x00;
		sptwb.Spt.Cdb[3] = 0x02;
		sptwb.Spt.Cdb[4] = 0x00;
		sptwb.Spt.Cdb[5] = 0xD1;  // READ_THRESHOLD
		sptwb.Spt.Cdb[6] = 0x01; 
		sptwb.Spt.Cdb[7] = 0x01; 
		sptwb.Spt.Cdb[8] = 0x4F;  // SMART_CYL_LOW
		sptwb.Spt.Cdb[9] = 0xC2;  // SMART_CYL_HIGH
		sptwb.Spt.Cdb[10] = target; 
		sptwb.Spt.Cdb[11] = 0xB0; // SMART_CMD
	}
	else if(type == CMD_TYPE_CYPRESS)
	{
		sptwb.Spt.CdbLength = 16;
		sptwb.Spt.Cdb[0] = 0x24;
		sptwb.Spt.Cdb[1] = 0x24;
		sptwb.Spt.Cdb[2] = 0x00;
		sptwb.Spt.Cdb[3] = 0xBE;
		sptwb.Spt.Cdb[4] = 0x01;
		sptwb.Spt.Cdb[5] = 0x00; 
		sptwb.Spt.Cdb[6] = 0xD1;  // READ_THRESHOLD  
		sptwb.Spt.Cdb[7] = 0x00; 
		sptwb.Spt.Cdb[8] = 0x00;
		sptwb.Spt.Cdb[9] = 0x4F;  // SMART_CYL_LOW
		sptwb.Spt.Cdb[10] = 0xC2; // SMART_CYL_HIGH
		sptwb.Spt.Cdb[11] = target;
		sptwb.Spt.Cdb[12] = 0xB0; // ID_CMD
		sptwb.Spt.Cdb[13] = 0x00;
		sptwb.Spt.Cdb[14] = 0x00;
		sptwb.Spt.Cdb[15] = 0x00;
	}
	else
	{
		return FALSE;
	}

	length = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, DataBuf) + sptwb.Spt.DataTransferLength;
	bRet = ::DeviceIoControl(hIoCtrl, IOCTL_SCSI_PASS_THROUGH, 
		&sptwb, sizeof(SCSI_PASS_THROUGH),
		&sptwb, length,	&dwReturned, NULL);
	
	::CloseHandle(hIoCtrl);
	
	if(bRet == FALSE || dwReturned != length)
	{
		return	FALSE;
	}

	memcpy_s(&(asi->SmartReadThreshold), 512, &(sptwb.DataBuf), 512);
	return	FillSmartThreshold(asi);
}

BOOL CAtaSmart::ControlSmartStatusSat(INT physicalDriveId, BYTE target, BYTE command, COMMAND_TYPE type)
{
	BOOL	bRet;
	HANDLE	hIoCtrl;
	DWORD	dwReturned;

	SCSI_PASS_THROUGH_WITH_BUFFERS sptwb;

	hIoCtrl = GetIoCtrlHandle(physicalDriveId);
	if(hIoCtrl == INVALID_HANDLE_VALUE)
	{
		return	FALSE;
	}

	::ZeroMemory(&sptwb,sizeof(SCSI_PASS_THROUGH_WITH_BUFFERS));

	sptwb.Spt.Length = sizeof(SCSI_PASS_THROUGH);
	sptwb.Spt.PathId = 0;
	sptwb.Spt.TargetId = 0;
	sptwb.Spt.Lun = 0;
	sptwb.Spt.SenseInfoLength = 24;
	sptwb.Spt.DataIn = SCSI_IOCTL_DATA_IN;
	sptwb.Spt.DataTransferLength = 0;
	sptwb.Spt.TimeOutValue = 2;
	sptwb.Spt.DataBufferOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, DataBuf);
	sptwb.Spt.SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, SenseBuf);
	if(type == CMD_TYPE_SAT)
	{
		sptwb.Spt.CdbLength = 12;
		sptwb.Spt.Cdb[0] = 0xA1; //ATA PASS THROUGH (12) OPERATION CODE (A1h)
		sptwb.Spt.Cdb[1] = (3 << 1) | 0; //MULTIPLE_COUNT=0,PROTOCOL=3(Non-Data),Reserved
		sptwb.Spt.Cdb[2] = (1 << 3) | (1 << 2) | 2;//OFF_LINE=0,CK_COND=0,Reserved=0,T_DIR=1(ToDevice),BYTE_BLOCK=1,T_LENGTH=2
		sptwb.Spt.Cdb[3] = command;//FEATURES (7:0)
		sptwb.Spt.Cdb[4] = 0;//SECTOR_COUNT (7:0)
		sptwb.Spt.Cdb[5] = 1;//LBA_LOW (7:0)
		sptwb.Spt.Cdb[6] = SMART_CYL_LOW;//LBA_MID (7:0)
		sptwb.Spt.Cdb[7] = SMART_CYL_HI;//LBA_HIGH (7:0)
		sptwb.Spt.Cdb[8] = target;
		sptwb.Spt.Cdb[9] = SMART_CMD;//COMMAND
	}
	else if(type == CMD_TYPE_SUNPLUS)
	{
		sptwb.Spt.CdbLength = 12;
		sptwb.Spt.Cdb[0] = 0xF8;
		sptwb.Spt.Cdb[1] = 0x00;
		sptwb.Spt.Cdb[2] = 0x22;
		sptwb.Spt.Cdb[3] = 0x10;
		sptwb.Spt.Cdb[4] = 0x01;
		sptwb.Spt.Cdb[5] = command;
		sptwb.Spt.Cdb[6] = 0x01; 
		sptwb.Spt.Cdb[7] = 0x00; 
		sptwb.Spt.Cdb[8] = 0x4F;  // SMART_CYL_LOW 
		sptwb.Spt.Cdb[9] = 0xC2;  // SMART_CYL_HIGH
		sptwb.Spt.Cdb[10] = target; 
		sptwb.Spt.Cdb[11] = 0xB0; // SMART_CMD
	}
	else if(type == CMD_TYPE_IO_DATA)
	{
		sptwb.Spt.CdbLength = 12;
		sptwb.Spt.Cdb[0] = 0xE3;
		sptwb.Spt.Cdb[1] = 0x00; // ?
		sptwb.Spt.Cdb[2] = command;
		sptwb.Spt.Cdb[3] = 0x00; // ?
		sptwb.Spt.Cdb[4] = 0x00; // ?
		sptwb.Spt.Cdb[5] = 0x4F; // SMART_CYL_LOW
		sptwb.Spt.Cdb[6] = 0xC2; // SMART_CYL_HIGH
		sptwb.Spt.Cdb[7] = target; // 
		sptwb.Spt.Cdb[8] = 0xB0; // SMART_CMD
		sptwb.Spt.Cdb[9] = 0x00;  
		sptwb.Spt.Cdb[10] = 0x00; 
		sptwb.Spt.Cdb[11] = 0x00;
	}
	else if(type == CMD_TYPE_LOGITEC1)
	{
		sptwb.Spt.CdbLength = 10;
		sptwb.Spt.Cdb[0] = 0xE0;
		sptwb.Spt.Cdb[1] = 0x00;
		sptwb.Spt.Cdb[2] = command;
		sptwb.Spt.Cdb[3] = 0x00; 
		sptwb.Spt.Cdb[4] = 0x00;
		sptwb.Spt.Cdb[5] = 0x4F; // SMART_CYL_LOW
		sptwb.Spt.Cdb[6] = 0xC2; // SMART_CYL_HIGH
		sptwb.Spt.Cdb[7] = target; 
		sptwb.Spt.Cdb[8] = 0xB0; // SMART_CMD
		sptwb.Spt.Cdb[9] = 0x4C;
	}
	else if (type == CMD_TYPE_LOGITEC2) // ??????????????
	{
		sptwb.Spt.CdbLength = 16;
		sptwb.Spt.Cdb[0] = 0xD8;
		sptwb.Spt.Cdb[1] = 0x15;
		sptwb.Spt.Cdb[2] = 0x00;
		sptwb.Spt.Cdb[3] = command;
		sptwb.Spt.Cdb[4] = 0x06;
		sptwb.Spt.Cdb[5] = 0x7B;
		sptwb.Spt.Cdb[6] = 0x00;
		sptwb.Spt.Cdb[7] = 0x00;
		sptwb.Spt.Cdb[8] = 0x02;
		sptwb.Spt.Cdb[9] = 0x00;
		sptwb.Spt.Cdb[10] = 0x01;
		sptwb.Spt.Cdb[11] = 0x00;
		sptwb.Spt.Cdb[12] = 0x4F; // SMART_CYL_LOW
		sptwb.Spt.Cdb[13] = 0xC2; // SMART_CYL_HIGH
		sptwb.Spt.Cdb[14] = target; // 
		sptwb.Spt.Cdb[15] = 0xB0;// SMART_CMD
	}
	else if(type == CMD_TYPE_JMICRON)
	{
		sptwb.Spt.CdbLength = 12;
		sptwb.Spt.Cdb[0] = 0xDF;
		sptwb.Spt.Cdb[1] = 0x10;
		sptwb.Spt.Cdb[2] = 0x00;
		sptwb.Spt.Cdb[3] = 0x02;
		sptwb.Spt.Cdb[4] = 0x00;
		sptwb.Spt.Cdb[5] = command;
		sptwb.Spt.Cdb[6] = 0x01; 
		sptwb.Spt.Cdb[7] = 0x01; 
		sptwb.Spt.Cdb[8] = 0x4F;  // SMART_CYL_LOW
		sptwb.Spt.Cdb[9] = 0xC2;  // SMART_CYL_HIGH
		sptwb.Spt.Cdb[10] = target; 
		sptwb.Spt.Cdb[11] = 0xB0; // SMART_CMD
	}
	else if(type == CMD_TYPE_CYPRESS)
	{
		sptwb.Spt.CdbLength = 16;
		sptwb.Spt.Cdb[0] = 0x24;
		sptwb.Spt.Cdb[1] = 0x24;
		sptwb.Spt.Cdb[2] = 0x00;
		sptwb.Spt.Cdb[3] = 0xBE;
		sptwb.Spt.Cdb[4] = 0x00;
		sptwb.Spt.Cdb[5] = 0x00; 
		sptwb.Spt.Cdb[6] = command; 
		sptwb.Spt.Cdb[7] = 0x00; 
		sptwb.Spt.Cdb[8] = 0x00;
		sptwb.Spt.Cdb[9] = 0x4F;  // SMART_CYL_LOW
		sptwb.Spt.Cdb[10] = 0xC2; // SMART_CYL_HIGH
		sptwb.Spt.Cdb[11] = target;
		sptwb.Spt.Cdb[12] = 0xB0; // ID_CMD
		sptwb.Spt.Cdb[13] = 0x00;
		sptwb.Spt.Cdb[14] = 0x00;
		sptwb.Spt.Cdb[15] = 0x00;
	}
	else
	{
		return FALSE;
	}

	DWORD length = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, DataBuf) + sptwb.Spt.DataTransferLength;
	bRet = ::DeviceIoControl(hIoCtrl, IOCTL_SCSI_PASS_THROUGH, 
		&sptwb, sizeof(SCSI_PASS_THROUGH),
		&sptwb, length,	&dwReturned, NULL);

	::CloseHandle(hIoCtrl);

	return	bRet;
}

BOOL CAtaSmart::SendAtaCommandSat(INT physicalDriveId, BYTE target, BYTE main, BYTE sub, BYTE param, COMMAND_TYPE type)
{
	BOOL	bRet;
	HANDLE	hIoCtrl;
	DWORD	dwReturned;

	SCSI_PASS_THROUGH_WITH_BUFFERS sptwb;

	hIoCtrl = GetIoCtrlHandle(physicalDriveId);
	if(hIoCtrl == INVALID_HANDLE_VALUE)
	{
		return	FALSE;
	}

	::ZeroMemory(&sptwb,sizeof(SCSI_PASS_THROUGH_WITH_BUFFERS));

	sptwb.Spt.Length = sizeof(SCSI_PASS_THROUGH);
	sptwb.Spt.PathId = 0;
	sptwb.Spt.TargetId = 0;
	sptwb.Spt.Lun = 0;
	sptwb.Spt.SenseInfoLength = 24;
	sptwb.Spt.DataIn = SCSI_IOCTL_DATA_IN;
	sptwb.Spt.DataTransferLength = 0;
	sptwb.Spt.TimeOutValue = 2;
	sptwb.Spt.DataBufferOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, DataBuf);
	sptwb.Spt.SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, SenseBuf);
	if(type == CMD_TYPE_SAT)
	{
		sptwb.Spt.CdbLength = 12;
		sptwb.Spt.Cdb[0]  = 0xA1; //ATA PASS THROUGH (12) OPERATION CODE (A1h)
		sptwb.Spt.Cdb[1]  = (3 << 1) | 0; //MULTIPLE_COUNT=0,PROTOCOL=3(Non-Data),Reserved
		sptwb.Spt.Cdb[2]  = (1 << 3) | (1 << 2) | 2;//OFF_LINE=0,CK_COND=0,Reserved=0,T_DIR=1(ToDevice),BYTE_BLOCK=1,T_LENGTH=2
		sptwb.Spt.Cdb[3]  = sub;		//FEATURES (7:0)
		sptwb.Spt.Cdb[4]  = param;		//SECTOR_COUNT (7:0)
		sptwb.Spt.Cdb[5]  = 0x00;		//LBA_LOW (7:0)
		sptwb.Spt.Cdb[6]  = 0x00;		//LBA_MID (7:0)
		sptwb.Spt.Cdb[7]  = 0x00;		//LBA_HIGH (7:0)
		sptwb.Spt.Cdb[8]  = target;		//DEVICE_HEAD
		sptwb.Spt.Cdb[9]  = main;		//COMMAND
		sptwb.Spt.Cdb[10] = 0x00;
		sptwb.Spt.Cdb[11] = 0x00;
	}
	else if(type == CMD_TYPE_SUNPLUS)
	{
		sptwb.Spt.CdbLength = 12;
		sptwb.Spt.Cdb[0]  = 0xF8;
		sptwb.Spt.Cdb[1]  = 0x00;
		sptwb.Spt.Cdb[2]  = 0x22;
		sptwb.Spt.Cdb[3]  = 0x10;
		sptwb.Spt.Cdb[4]  = 0x01;
		sptwb.Spt.Cdb[5]  = sub;
		sptwb.Spt.Cdb[6]  = param; 
		sptwb.Spt.Cdb[7]  = 0x00; 
		sptwb.Spt.Cdb[8]  = 0x00;
		sptwb.Spt.Cdb[9]  = 0x00;
		sptwb.Spt.Cdb[10] = target; 
		sptwb.Spt.Cdb[11] = main;
	}
	else if(type == CMD_TYPE_IO_DATA)
	{
		sptwb.Spt.CdbLength = 12;
		sptwb.Spt.Cdb[0]  = 0xE3;
		sptwb.Spt.Cdb[1]  = 0x00;
		sptwb.Spt.Cdb[2]  = sub;
		sptwb.Spt.Cdb[3]  = param;
		sptwb.Spt.Cdb[4]  = 0x00;
		sptwb.Spt.Cdb[5]  = 0x00;
		sptwb.Spt.Cdb[6]  = 0x00;
		sptwb.Spt.Cdb[7]  = target;
		sptwb.Spt.Cdb[8]  = main;
		sptwb.Spt.Cdb[9]  = 0x00;  
		sptwb.Spt.Cdb[10] = 0x00; 
		sptwb.Spt.Cdb[11] = 0x00;
	}
	else if(type == CMD_TYPE_LOGITEC1)
	{
		sptwb.Spt.CdbLength = 10;
		sptwb.Spt.Cdb[0] = 0xE0;
		sptwb.Spt.Cdb[1] = 0x00;
		sptwb.Spt.Cdb[2] = sub;
		sptwb.Spt.Cdb[3] = param; 
		sptwb.Spt.Cdb[4] = 0x00;
		sptwb.Spt.Cdb[5] = 0x00;
		sptwb.Spt.Cdb[6] = 0x00;
		sptwb.Spt.Cdb[7] = target; 
		sptwb.Spt.Cdb[8] = main;
		sptwb.Spt.Cdb[9] = 0x4C;		// ?
	}
	else if (type == CMD_TYPE_LOGITEC2) // ????????????????
	{
		sptwb.Spt.CdbLength = 16;
		sptwb.Spt.Cdb[0] = 0xD8;
		sptwb.Spt.Cdb[1] = 0x15;
		sptwb.Spt.Cdb[2] = 0x00;
		sptwb.Spt.Cdb[3] = sub;
		sptwb.Spt.Cdb[4] = 0x06;
		sptwb.Spt.Cdb[5] = 0x7B;
		sptwb.Spt.Cdb[6] = 0x00;
		sptwb.Spt.Cdb[7] = 0x00;
		sptwb.Spt.Cdb[8] = 0x00;
		sptwb.Spt.Cdb[9] = 0x00;
		sptwb.Spt.Cdb[10] = param;
		sptwb.Spt.Cdb[11] = 0x00;
		sptwb.Spt.Cdb[12] = 0x00;
		sptwb.Spt.Cdb[13] = 0x00;
		sptwb.Spt.Cdb[14] = target;
		sptwb.Spt.Cdb[15] = main;
	}
	else if(type == CMD_TYPE_JMICRON)
	{
		sptwb.Spt.CdbLength = 12;
		sptwb.Spt.Cdb[0]  = 0xDF;
		sptwb.Spt.Cdb[1]  = 0x10;
		sptwb.Spt.Cdb[2]  = 0x00;
		sptwb.Spt.Cdb[3]  = 0x02;
		sptwb.Spt.Cdb[4]  = 0x00;
		sptwb.Spt.Cdb[5]  = sub;
		sptwb.Spt.Cdb[6]  = param; 
		sptwb.Spt.Cdb[7]  = 0x00; 
		sptwb.Spt.Cdb[8]  = 0x00;
		sptwb.Spt.Cdb[9]  = 0x00;
		sptwb.Spt.Cdb[10] = target;
		sptwb.Spt.Cdb[11] = main;
	}
	else if(type == CMD_TYPE_CYPRESS)
	{
		sptwb.Spt.CdbLength = 16;
		sptwb.Spt.Cdb[0]  = 0x24;
		sptwb.Spt.Cdb[1]  = 0x24;
		sptwb.Spt.Cdb[2]  = 0x00;
		sptwb.Spt.Cdb[3]  = 0xBE;
		sptwb.Spt.Cdb[4]  = 0x00;
		sptwb.Spt.Cdb[5]  = 0x00; 
		sptwb.Spt.Cdb[6]  = sub; 
		sptwb.Spt.Cdb[7]  = param; 
		sptwb.Spt.Cdb[8]  = 0x00;
		sptwb.Spt.Cdb[9]  = 0x00;
		sptwb.Spt.Cdb[10] = 0x00;
		sptwb.Spt.Cdb[11] = target;
		sptwb.Spt.Cdb[12] = main;
		sptwb.Spt.Cdb[13] = 0x00;
		sptwb.Spt.Cdb[14] = 0x00;
		sptwb.Spt.Cdb[15] = 0x00;
	}
	else
	{
		return FALSE;
	}

	DWORD length = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, DataBuf) + sptwb.Spt.DataTransferLength;
	bRet = ::DeviceIoControl(hIoCtrl, IOCTL_SCSI_PASS_THROUGH, 
		&sptwb, sizeof(SCSI_PASS_THROUGH),
		&sptwb, length,	&dwReturned, NULL);

	::CloseHandle(hIoCtrl);

	return	bRet;
}

/*---------------------------------------------------------------------------*/
// Silicon Image Support
/*---------------------------------------------------------------------------*/

HANDLE CAtaSmart::GetIoCtrlHandle(INT scsiPort, DWORD siliconImageType)
{
	HANDLE hScsiDriveIOCTL = 0;
	CString driveName;

	driveName.Format(_T("\\\\.\\Scsi%d:"), scsiPort);
	hScsiDriveIOCTL = CreateFile(driveName, GENERIC_READ | GENERIC_WRITE,
							FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if(hScsiDriveIOCTL == INVALID_HANDLE_VALUE)
	{
		TCHAR szBusPathBody[MAX_PATH];
		TCHAR szSilDeviceName[MAX_PATH];

		if(siliconImageType == 3512)
		{
			siliconImageType = 3112;
		}

		wsprintf(szBusPathBody, _T("tempBusSil%d"), siliconImageType);
		wsprintf(szSilDeviceName, _T("\\Device\\Scsi\\SI%d1"), siliconImageType) ;

		if(DefineDosDevice(DDD_RAW_TARGET_PATH, szBusPathBody, szSilDeviceName))
		{
			driveName.Format(_T("\\\\.\\%s"), szBusPathBody);
			return CreateFile(driveName, GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		}
	}
	return hScsiDriveIOCTL;
}

BOOL CAtaSmart::DoIdentifyDeviceSi(INT physicalDriveId, INT scsiPort, INT scsiBus, DWORD siliconImageType, IDENTIFY_DEVICE* data)
{
	int done = FALSE;
	int controller = 0;
	int current = 0;
	HANDLE hScsiDriveIOCTL = 0;

	hScsiDriveIOCTL = GetIoCtrlHandle(scsiPort, siliconImageType);

	if(hScsiDriveIOCTL != INVALID_HANDLE_VALUE)
	{
		SilIdentDev sid;
		memset(&sid, 0, sizeof(sid));
		
		sid.sic.HeaderLength = sizeof(SRB_IO_CONTROL);
		memcpy(sid.sic.Signature, "CMD_IDE ", 8);
		sid.sic.Timeout = 5;
		sid.sic.ControlCode = CTL_CODE(FILE_DEVICE_CONTROLLER, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS);
		sid.sic.ReturnCode = 0xffffffff;
		sid.sic.Length = sizeof(sid) - offsetof(SilIdentDev, port);
		sid.port = scsiBus;
		sid.maybe_always1 = 1 ;

		DWORD dwReturnBytes;
		if(DeviceIoControl(hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT, &sid, sizeof(sid), &sid, sizeof(sid), &dwReturnBytes, NULL))
		{
			done = TRUE;
			memcpy_s(data, sizeof(IDENTIFY_DEVICE), &sid.id_data, sizeof(IDENTIFY_DEVICE));
		}

		CloseHandle(hScsiDriveIOCTL);
	}
	return done;
}

BOOL CAtaSmart::GetSmartAttributeSi(INT physicalDriveId, ATA_SMART_INFO* asi)
{
//	return GetSmartAttributePd(physicalDriveId, 0xA0, asi);

	HANDLE hScsiDriveIOCTL = 0;

	hScsiDriveIOCTL = GetIoCtrlHandle(physicalDriveId);

	STORAGE_PREDICT_FAILURE spf = {0};
	DWORD dwRetBytes;

	if(hScsiDriveIOCTL != INVALID_HANDLE_VALUE)
	{
		if (DeviceIoControl(hScsiDriveIOCTL, IOCTL_STORAGE_PREDICT_FAILURE,
			&spf, sizeof(spf), &spf, sizeof(spf), &dwRetBytes, NULL))
		{
			memcpy_s(&(asi->SmartReadData), 512, &(spf.VendorSpecific), 512);
			CloseHandle(hScsiDriveIOCTL);
			return FillSmartData(asi);
		}
		CloseHandle(hScsiDriveIOCTL);
	}
	return GetSmartAttributeWmi(asi);
}

BOOL CAtaSmart::GetSmartThresholdSi(INT physicalDriveId, ATA_SMART_INFO* asi)
{
	return GetSmartThresholdWmi(asi);
}

/*---------------------------------------------------------------------------*/
// WMI
/*---------------------------------------------------------------------------*/

BOOL CAtaSmart::GetSmartAttributeWmi(ATA_SMART_INFO* asi)
{
	return GetSmartInfoWmi(WMI_SMART_DATA, asi);
}

BOOL CAtaSmart::GetSmartThresholdWmi(ATA_SMART_INFO* asi)
{
	if(m_Os.dwMajorVersion == 5 && m_Os.dwMinorVersion == 0)
	{
		return FALSE;
	}

	return GetSmartInfoWmi(WMI_SMART_THRESHOLD, asi);
}

BOOL CAtaSmart::GetSmartInfoWmi(DWORD type, ATA_SMART_INFO* asi)
{
	if(asi->PnpDeviceId.IsEmpty())
	{
		return FALSE;
	}

	CString query;

	if(type == WMI_SMART_DATA)
	{
		query = L"SELECT * FROM MSStorageDriver_FailurePredictData";
	}
	else
	{
		query = L"SELECT * FROM MSStorageDriver_FailurePredictThresholds";
	}
	
	IWbemLocator*			pIWbemLocator = NULL;
	IWbemServices*			pIWbemServices = NULL;
	IEnumWbemClassObject*	pEnumCOMDevs = NULL;
	IWbemClassObject*		pCOMDev = NULL;
	ULONG					uReturned = 0;
	BOOL					flag = FALSE;

	try
	{
		if(SUCCEEDED(CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER,
				IID_IWbemLocator, (LPVOID *)&pIWbemLocator)))
		{
			long securityFlag = 0;
			if(m_Os.dwMajorVersion >= 6){securityFlag = WBEM_FLAG_CONNECT_USE_MAX_WAIT;}
			if(SUCCEEDED(pIWbemLocator->ConnectServer(_bstr_t(L"\\\\.\\root\\WMI"), 
				NULL, NULL, 0L, securityFlag, NULL, NULL, &pIWbemServices)))
			{
				if(SUCCEEDED(CoSetProxyBlanket(pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
					NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE)))
				{
					if(SUCCEEDED(pIWbemServices->ExecQuery(_bstr_t(L"WQL"), 
						_bstr_t(query), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumCOMDevs)))
					{
						while(pEnumCOMDevs && SUCCEEDED(pEnumCOMDevs->Next(10000, 1, &pCOMDev, &uReturned)) && uReturned == 1)
						{
							CString name;
							VARIANT pVal;
							VariantInit(&pVal);
							
							if(pCOMDev->Get(L"InstanceName", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
							{
								name = pVal.bstrVal;
								name.MakeUpper();
								VariantClear(&pVal);
							}
							VariantInit(&pVal);

							
						
							if(name.Find(asi->PnpDeviceId) != 0)
							{
								continue;
							}

							if(pCOMDev->Get(L"VendorSpecific", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
							{
								if(type == WMI_SMART_DATA)
								{
									for(long i = 0; i < 512; i++)
									{
										SafeArrayGetElement(pVal.parray, &i, &(asi->SmartReadData[i]));
									}
									flag = FillSmartData(asi);
								}
								else
								{
									for(long i = 0; i < 512; i++)
									{
										SafeArrayGetElement(pVal.parray, &i, &(asi->SmartReadThreshold[i]));
									}
									flag = FillSmartThreshold(asi);
								}
								VariantClear(&pVal);
								break;
							}
						}
					}
				}
			}
		}
	}
	catch(...)
	{
		flag = FALSE;
	}
	SAFE_RELEASE(pCOMDev);
	SAFE_RELEASE(pEnumCOMDevs);
	SAFE_RELEASE(pIWbemServices);
	SAFE_RELEASE(pIWbemLocator);

	return flag;
}

/*---------------------------------------------------------------------------*/
// CSMI support (Intel RAID support)
/*---------------------------------------------------------------------------*/

HANDLE CAtaSmart::GetIoCtrlHandleCsmi(INT scsiPort)
{
	HANDLE hScsiDriveIOCTL = 0;
	CString driveName;

	driveName.Format(_T("\\\\.\\Scsi%d:"), scsiPort);
	hScsiDriveIOCTL = CreateFile(driveName, GENERIC_READ | GENERIC_WRITE,
							FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	return hScsiDriveIOCTL;
}

BOOL CAtaSmart::AddDiskCsmi(INT scsiPort)
{
	if(CsmiType == CSMI_TYPE_DISABLE)
	{
		return FALSE;
	}

	HANDLE hHandle = GetIoCtrlHandleCsmi(scsiPort);
	if(hHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	CSMI_SAS_DRIVER_INFO_BUFFER driverInfoBuf = {0};
	if(! CsmiIoctl(hHandle, CC_CSMI_SAS_GET_DRIVER_INFO, &driverInfoBuf.IoctlHeader, sizeof(driverInfoBuf)))
	{
		CloseHandle(hHandle);
		DebugPrint(_T("FAILED: CC_CSMI_SAS_GET_DRIVER_INFO"));
		return FALSE;
	}


	CSMI_SAS_RAID_INFO_BUFFER raidInfoBuf = {0};
	if(! CsmiIoctl(hHandle, CC_CSMI_SAS_GET_RAID_INFO, &raidInfoBuf.IoctlHeader, sizeof(raidInfoBuf)))
	{
		CloseHandle(hHandle);
		DebugPrint(_T("FAILED: CC_CSMI_SAS_GET_RAID_INFO"));
		return FALSE;
	}

//	CArray<CSMI_SAS_RAID_DRIVES, CSMI_SAS_RAID_DRIVES> raidDrives;
	CArray<UCHAR, UCHAR> raidDrives;

	DWORD size = sizeof(CSMI_SAS_RAID_CONFIG_BUFFER) + sizeof(CSMI_SAS_RAID_DRIVES) * raidInfoBuf.Information.uNumRaidSets * raidInfoBuf.Information.uMaxDrivesPerSet;
	PCSMI_SAS_RAID_CONFIG_BUFFER buf = (PCSMI_SAS_RAID_CONFIG_BUFFER)VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);
	for(UINT i = 0; i < raidInfoBuf.Information.uNumRaidSets; i++)
	{
		buf->Configuration.uRaidSetIndex = i;
		if(! CsmiIoctl(hHandle, CC_CSMI_SAS_GET_RAID_CONFIG, &(buf->IoctlHeader), size))
		{
			CloseHandle(hHandle);
			DebugPrint(_T("FAILED: CC_CSMI_SAS_GET_RAID_CONFIG"));
			VirtualFree(buf, 0, MEM_RELEASE);
			return FALSE;
		}
		else
		{
			for(UINT j = 0; j < raidInfoBuf.Information.uMaxDrivesPerSet; j++)
			{
				if(buf->Configuration.Drives[j].bModel[0] != '\0')
				{
				//	raidDrives.Add(buf->Configuration.Drives[j]);
					raidDrives.Add(buf->Configuration.Drives[j].bSASAddress[2]);
				}
			}
		}
	}
	VirtualFree(buf, 0, MEM_RELEASE);

	CSMI_SAS_PHY_INFO phyInfo = {0};
	CSMI_SAS_PHY_INFO_BUFFER phyInfoBuf = {0};
	if (! CsmiIoctl(hHandle, CC_CSMI_SAS_GET_PHY_INFO, &phyInfoBuf.IoctlHeader, sizeof(phyInfoBuf)))
	{
		CloseHandle(hHandle);
		DebugPrint(_T("FAILED: CC_CSMI_SAS_GET_PHY_INFO"));
		return FALSE;
	}
	memcpy(&phyInfo, &(phyInfoBuf.Information), sizeof(phyInfoBuf.Information));
	
	IDENTIFY_DEVICE identify = {0};	
	if(phyInfo.bNumberOfPhys <= sizeof(phyInfo.Phy)/sizeof(phyInfo.Phy[0]))
	{
		for(int i = 0; i < phyInfo.bNumberOfPhys; i++)
		{
			if(CsmiType == CSMI_TYPE_ENABLE_RAID)
			{
				for(int j = 0; j < raidDrives.GetCount(); j++)
				{
				//	PCSMI_SAS_RAID_DRIVES test;
				//	test = &(raidDrives.GetAt(j));
				//	if(memcmp(raidDrives.GetAt(j).bSASAddress, phyInfo.Phy[i].Attached.bSASAddress, 8) == 0)
				//	if(raidDrives.GetAt(j).bSASAddress[2] == phyInfo.Phy[i].Attached.bSASAddress[2])
					if(raidDrives.GetAt(j) == phyInfo.Phy[i].Attached.bSASAddress[2])
					{
						if(DoIdentifyDeviceCsmi(scsiPort, &(phyInfo.Phy[i]), &identify))
						{
							AddDisk(-1, scsiPort, -1, -1, 0xA0, CMD_TYPE_CSMI, &identify, FALSE, &(phyInfo.Phy[i]));
						}
						break;
					}
				}
			}
			else
			{
				if(DoIdentifyDeviceCsmi(scsiPort, &(phyInfo.Phy[i]), &identify))
				{
					AddDisk(-1, scsiPort, -1, -1, 0xA0, CMD_TYPE_CSMI, &identify, FALSE, &(phyInfo.Phy[i]));
				}
			}
		}
	}	

	CloseHandle(hHandle);
	return TRUE;
}

BOOL CAtaSmart::CsmiIoctl(HANDLE hHandle, UINT code, SRB_IO_CONTROL *csmiBuf, UINT csmiBufSize)
{
	// Determine signature
	const CHAR *sig;
	switch (code)
	{
		case CC_CSMI_SAS_GET_DRIVER_INFO:
			sig = CSMI_ALL_SIGNATURE;
			break;
		case CC_CSMI_SAS_GET_PHY_INFO:
		case CC_CSMI_SAS_STP_PASSTHRU:
			sig = CSMI_SAS_SIGNATURE;
			break;
		case CC_CSMI_SAS_GET_RAID_INFO:
		case CC_CSMI_SAS_GET_RAID_CONFIG:
			sig = CSMI_RAID_SIGNATURE;
			break;
		default:
			return FALSE;
	}

	// Set header
	csmiBuf->HeaderLength = sizeof(IOCTL_HEADER);
	strncpy_s((char *)csmiBuf->Signature, sizeof(csmiBuf->Signature), sig, sizeof(csmiBuf->Signature));
	csmiBuf->Timeout = CSMI_SAS_TIMEOUT;
	csmiBuf->ControlCode = code;
	csmiBuf->ReturnCode = 0;
	csmiBuf->Length = csmiBufSize - sizeof(IOCTL_HEADER);

	// Call function
	DWORD num_out = 0;
	if (!DeviceIoControl(hHandle, IOCTL_SCSI_MINIPORT, 
		csmiBuf, csmiBufSize, csmiBuf, csmiBufSize, &num_out, (OVERLAPPED*)0))
	{
		long err = GetLastError();
		
		if (err == ERROR_INVALID_FUNCTION || err == ERROR_NOT_SUPPORTED 
			|| err == ERROR_DEV_NOT_EXIST)
		{
			return FALSE;
		}
	}

	// Check result
	return TRUE;
}

BOOL CAtaSmart::DoIdentifyDeviceCsmi(INT scsiPort, PCSMI_SAS_PHY_ENTITY sasPhyEntity, IDENTIFY_DEVICE* data)
{
	BOOL flag = FALSE;
	DebugPrint(_T("DoIdentifyDeviceCsmi"));
	return SendAtaCommandCsmi(scsiPort, sasPhyEntity, 0xEC, 0x00, 0x00, (PBYTE)data, sizeof(IDENTIFY_DEVICE));
}

BOOL CAtaSmart::GetSmartAttributeCsmi(INT scsiPort, PCSMI_SAS_PHY_ENTITY sasPhyEntity, ATA_SMART_INFO* asi)
{
	DebugPrint(_T("GetSmartAttributeCsmi"));
	if(SendAtaCommandCsmi(scsiPort, sasPhyEntity, SMART_CMD, READ_ATTRIBUTES, 0x00, (PBYTE)asi->SmartReadData, sizeof(asi->SmartReadData)))
	{
		DebugPrint(_T("FillSmartData"));
		return FillSmartData(asi);
	}
	else
	{
		return FALSE;
	}
}

BOOL CAtaSmart::GetSmartThresholdCsmi(INT scsiPort, PCSMI_SAS_PHY_ENTITY sasPhyEntity, ATA_SMART_INFO* asi)
{
	DebugPrint(_T("GetSmartThresholdCsmi"));
	if(SendAtaCommandCsmi(scsiPort, sasPhyEntity, SMART_CMD, READ_THRESHOLDS, 0x00, (PBYTE)asi->SmartReadThreshold, sizeof(asi->SmartReadThreshold)))
	{
		DebugPrint(_T("FillSmartThreshold"));
		return FillSmartThreshold(asi);
	}
	else
	{
		return FALSE;
	}
}

BOOL CAtaSmart::ControlSmartStatusCsmi(INT scsiPort, PCSMI_SAS_PHY_ENTITY sasPhyEntity, BYTE command)
{
	DebugPrint(_T("ControlSmartStatusCsmi"));
	return SendAtaCommandCsmi(scsiPort, sasPhyEntity, SMART_CMD, command, 0x00, NULL, 0);
}

BOOL CAtaSmart::SendAtaCommandCsmi(INT scsiPort, PCSMI_SAS_PHY_ENTITY sasPhyEntity, BYTE main, BYTE sub, BYTE param, PBYTE data, DWORD dataSize)
{
	HANDLE hIoCtrl = GetIoCtrlHandleCsmi(scsiPort);
	if(hIoCtrl == INVALID_HANDLE_VALUE)
	{
		return	FALSE;
	}

	DWORD size = sizeof(CSMI_SAS_STP_PASSTHRU_BUFFER) + dataSize;
	CSMI_SAS_STP_PASSTHRU_BUFFER* buf = (CSMI_SAS_STP_PASSTHRU_BUFFER*)VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);

	buf->Parameters.bPhyIdentifier = sasPhyEntity->Attached.bPhyIdentifier;
	buf->Parameters.bPortIdentifier = sasPhyEntity->bPortIdentifier;
	memcpy(&(buf->Parameters.bDestinationSASAddress), sasPhyEntity->Attached.bSASAddress, sizeof(sasPhyEntity->Attached.bSASAddress));
	buf->Parameters.bConnectionRate = CSMI_SAS_LINK_RATE_NEGOTIATED;

	if(main == 0xEF) // AAM/APM
	{
		buf->Parameters.uFlags = CSMI_SAS_STP_UNSPECIFIED;
	}
	else
	{
		buf->Parameters.uFlags = CSMI_SAS_STP_PIO | CSMI_SAS_STP_READ;
	}
	buf->Parameters.uDataLength = dataSize;

	buf->Parameters.bCommandFIS[ 0] = 0x27; // Type: host-to-device FIS
	buf->Parameters.bCommandFIS[ 1] = 0x80; // Bit7: Update command register

	if(main == SMART_CMD)
	{
		buf->Parameters.bCommandFIS[ 2] = main;
		buf->Parameters.bCommandFIS[ 3] = sub;
		buf->Parameters.bCommandFIS[ 4] = 0;
		buf->Parameters.bCommandFIS[ 5] = SMART_CYL_LOW;
		buf->Parameters.bCommandFIS[ 6] = SMART_CYL_HI;
		buf->Parameters.bCommandFIS[ 7] = 0xA0; // target
		buf->Parameters.bCommandFIS[ 8] = 0;
		buf->Parameters.bCommandFIS[ 9] = 0;
		buf->Parameters.bCommandFIS[10] = 0;
		buf->Parameters.bCommandFIS[11] = 0;
		buf->Parameters.bCommandFIS[12] = param;
		buf->Parameters.bCommandFIS[13] = 0;
	}
	else
	{
		buf->Parameters.bCommandFIS[ 2] = main;
		buf->Parameters.bCommandFIS[ 3] = sub;
		buf->Parameters.bCommandFIS[ 4] = 0;
		buf->Parameters.bCommandFIS[ 5] = 0;
		buf->Parameters.bCommandFIS[ 6] = 0;
		buf->Parameters.bCommandFIS[ 7] = 0xA0; // target
		buf->Parameters.bCommandFIS[ 8] = 0;
		buf->Parameters.bCommandFIS[ 9] = 0;
		buf->Parameters.bCommandFIS[10] = 0;
		buf->Parameters.bCommandFIS[11] = 0;
		buf->Parameters.bCommandFIS[12] = param;
		buf->Parameters.bCommandFIS[13] = 0;
	}

	if(! CsmiIoctl(hIoCtrl, CC_CSMI_SAS_STP_PASSTHRU, &buf->IoctlHeader, size))
	{
		CloseHandle(hIoCtrl);
		VirtualFree(buf, 0, MEM_RELEASE);
		return FALSE;
	}

	if(main != 0xEF && buf->bDataBuffer && data != NULL)
	{
		memcpy_s(data, dataSize, buf->bDataBuffer, dataSize);
	}
	
	CloseHandle(hIoCtrl);
	VirtualFree(buf, 0, MEM_RELEASE);
	
	return	TRUE;
}

/*---------------------------------------------------------------------------*/
// Fill S.M.A.R.T. Information
/*---------------------------------------------------------------------------*/

BOOL CAtaSmart::FillSmartData(ATA_SMART_INFO* asi)
{
	CString str;
	asi->AttributeCount = 0;
	int j = 0;
	for(int i = 0; i < MAX_ATTRIBUTE; i++)
	{
		DWORD rawValue = 0;
		memcpy(	&(asi->Attribute[j]), 
			&(asi->SmartReadData[i * sizeof(SMART_ATTRIBUTE) + 2]), sizeof(SMART_ATTRIBUTE));

		if(asi->Attribute[j].Id != 0)
		{
			switch(asi->Attribute[j].Id)
			{
			case 0x09: // Power on Hours
				rawValue = MAKELONG(
					MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
					MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
					);
				if(asi->DiskVendorId == SSD_VENDOR_INDILINX)
				{
					rawValue = asi->Attribute[j].WorstValue * 256 + asi->Attribute[j].CurrentValue;
				}
				// Intel SSD 520 Series and etc...
				else if (
					(asi->DetectedTimeUnitType == POWER_ON_MILLI_SECONDS)
				||  (asi->DetectedTimeUnitType == POWER_ON_HOURS && rawValue >= 0x0DA000)
				|| (asi->Model.Find(_T("Intel")) == 0 && rawValue >= 0x0DA000)
				)
				{
					asi->MeasuredTimeUnitType = POWER_ON_MILLI_SECONDS;
					int value = 0; 
					rawValue = value = asi->Attribute[j].RawValue[2] * 256 * 256
									 + asi->Attribute[j].RawValue[1] * 256
									 + asi->Attribute[j].RawValue[0] - 0x0DA753; // https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=560;id=diskinfo#560
					if(value < 0)
					{
						rawValue = 0;
					}
				}

				asi->PowerOnRawValue = rawValue;
				asi->DetectedPowerOnHours = GetPowerOnHours(rawValue, asi->DetectedTimeUnitType);
				asi->MeasuredPowerOnHours = GetPowerOnHours(rawValue, asi->MeasuredTimeUnitType);
				break;
			case 0x0C: // Power On Count
				rawValue = MAKELONG(
					MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
					MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
					);
				if(asi->DiskVendorId == SSD_VENDOR_INDILINX)
				{
					rawValue = asi->Attribute[j].WorstValue * 256 + asi->Attribute[j].CurrentValue;
				}
				asi->PowerOnCount = rawValue;
				break;
			case 0xBE:
				if(asi->Attribute[j].RawValue[0] > 0 && asi->Attribute[j].RawValue[0] < 100)
				{
					asi->Temperature = asi->Attribute[j].RawValue[0];
				}
				break;
			case 0xC2: // Temperature
				if(asi->Model.Find(_T("SAMSUNG SV")) == 0 && (asi->Attribute[j].RawValue[1] != 0 || asi->Attribute[j].RawValue[0] > 70))
				{
					asi->Temperature = MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]) / 10;			
				}
				else if(asi->Attribute[j].RawValue[0] > 0 && asi->TemperatureMultiplier < 1.0)//(asi->DiskVendorId == SSD_VENDOR_SANDFORCE)
				{
					asi->Temperature = (DWORD)(asi->Attribute[j].RawValue[0] * asi->TemperatureMultiplier);
				}
				else if(asi->Attribute[j].RawValue[0] > 0)
				{
					asi->Temperature = asi->Attribute[j].RawValue[0];
				}

				if(asi->Temperature >= 100)
				{
					asi->Temperature = -1000;
				}
				break;
			case 0xBB:
				if(asi->DiskVendorId == SSD_VENDOR_MTRON)
				{
					if(asi->Attribute[j].CurrentValue <= 100)
					{
						asi->Life = asi->Attribute[j].CurrentValue;
					}
				}
				break;
			case 0xCA:
				if (asi->DiskVendorId == SSD_VENDOR_MICRON)
				{
					if (asi->Attribute[j].CurrentValue <= 100)
					{
						asi->Life = asi->Attribute[j].CurrentValue;
					}
				}
				break;
			case 0xD1:
				if(asi->DiskVendorId == SSD_VENDOR_INDILINX)
				{
					if(asi->Attribute[j].CurrentValue <= 100)
					{
						asi->Life = asi->Attribute[j].CurrentValue;
					}
				}
				break;
			case 0xF7:
				if (asi->DiskVendorId == SSD_VENDOR_KINGSTON)
				{
					if (asi->Attribute[j].CurrentValue <= 100)
					{
						asi->Life = asi->Attribute[j].CurrentValue;
					}
				}
				break;
			case 0xE8:
				if(asi->DiskVendorId == SSD_VENDOR_INTEL || asi->DiskVendorId == SSD_VENDOR_PLEXTOR || asi->DiskVendorId == SSD_VENDOR_SANDISK)
				{
					if(asi->Attribute[j].CurrentValue <= 100)
					{
						asi->Life = asi->Attribute[j].CurrentValue;
					}
				}
				else if(asi->DiskVendorId == SSD_VENDOR_OCZ)
				{
					asi->HostWrites  = (INT)(
						(UINT64)
						( (UINT64)asi->Attribute[j].RawValue[5] * 256 * 256 * 256 * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[4] * 256 * 256 * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[3] * 256 * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[2] * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[1] * 256
						+ (UINT64)asi->Attribute[j].RawValue[0])
						* 512 / 1024 / 1024 / 1024);
				}
				break;
			case 0xE9:
				if(asi->DiskVendorId == SSD_VENDOR_OCZ || asi->DiskVendorId == SSD_VENDOR_OCZ_VECTOR)
				{
					if(asi->Attribute[j].CurrentValue <= 100)
					{
						asi->Life = asi->Attribute[j].CurrentValue;
					}
				}
				else if (asi->DiskVendorId == SSD_VENDOR_SANDISK && asi->HostReadsWritesUnit == HOST_READS_WRITES_GB)
				{
					asi->NandWrites = (INT) (MAKELONG(
						MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
						MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
						));
				}
				else if (asi->DiskVendorId == SSD_VENDOR_PLEXTOR || asi->DiskVendorId == SSD_VENDOR_KINGSTON)
				{
					asi->NandWrites = (INT) (MAKELONG(
						MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
						MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
						));
				}
				break;
			case 0xE1:
				if(asi->DiskVendorId == SSD_VENDOR_INTEL)
				{
					asi->HostWrites  = (INT)(MAKELONG(
						MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
						MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
						) * 0.03125); //  0.03125 = 65536 * 512 / 1024 / 1024 / 1024;
				}
				break;
			case 0xF1:
				if (asi->DiskVendorId == HDD_SSD_VENDOR_TOSHIBA && asi->HostReadsWritesUnit == HOST_READS_WRITES_GB)
				{
					asi->HostWrites = (INT) (MAKELONG(
						MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
						MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
						));
				}
				else if(asi->DiskVendorId == SSD_VENDOR_INTEL || asi->DiskVendorId == HDD_SSD_VENDOR_TOSHIBA)
				{
					asi->HostWrites  = (INT)(MAKELONG(
						MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
						MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
						) * 0.03125); //  0.03125 = 65536 * 512 / 1024 / 1024 / 1024;
				}
				else if (asi->DiskVendorId == SSD_VENDOR_SANDFORCE || asi->DiskVendorId == SSD_VENDOR_OCZ_VECTOR || asi->DiskVendorId == SSD_VENDOR_CORSAIR || asi->DiskVendorId == SSD_VENDOR_KINGSTON)
				{
					asi->HostWrites  = (INT)(MAKELONG(
						MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
						MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
						));
				}
				else if (asi->DiskVendorId == SSD_VENDOR_SAMSUNG && asi->HostReadsWritesUnit == HOST_READS_WRITES_GB)
				{
					asi->HostWrites = (INT) (MAKELONG(
						MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
						MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
						));
				}
				else if(asi->DiskVendorId == SSD_VENDOR_SAMSUNG)
				{
					asi->HostWrites  = (INT)(
						(UINT64)
						( (UINT64)asi->Attribute[j].RawValue[5] * 256 * 256 * 256 * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[4] * 256 * 256 * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[3] * 256 * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[2] * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[1] * 256
						+ (UINT64)asi->Attribute[j].RawValue[0])
						* 512 / 1024 / 1024 / 1024);
				}
				else if(asi->DiskVendorId == HDD_VENDOR_WESTERN_DIGITAL)
				{
					asi->HostWrites  = (INT)(
						(UINT64)
						( (UINT64)asi->Attribute[j].RawValue[5] * 256 * 256 * 256 * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[4] * 256 * 256 * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[3] * 256 * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[2] * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[1] * 256
						+ (UINT64)asi->Attribute[j].RawValue[0])
						* 512 / 1024 / 1024 / 1024);
				}
				else if(asi->DiskVendorId == SSD_VENDOR_PLEXTOR || asi->DiskVendorId == SSD_VENDOR_MICRON_MU02)
				{
					asi->HostWrites  = (INT)(MAKELONG(
						MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
						MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
						)) * 32 / 1024;
				}
				else if(asi->DiskVendorId == SSD_VENDOR_SANDISK && asi->HostReadsWritesUnit == HOST_READS_WRITES_GB)
				{
					asi->HostWrites = (INT) (MAKELONG(
						MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
						MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
						));
				}
				else if (asi->DiskVendorId == SSD_VENDOR_SANDISK)
				{
					asi->HostWrites = (INT) (
						(UINT64)
						((UINT64) asi->Attribute[j].RawValue[5] * 256 * 256 * 256 * 256 * 256
						+ (UINT64) asi->Attribute[j].RawValue[4] * 256 * 256 * 256 * 256
						+ (UINT64) asi->Attribute[j].RawValue[3] * 256 * 256 * 256
						+ (UINT64) asi->Attribute[j].RawValue[2] * 256 * 256
						+ (UINT64) asi->Attribute[j].RawValue[1] * 256
						+ (UINT64) asi->Attribute[j].RawValue[0])
						* 512 / 1024 / 1024 / 1024);
				}
				/*
				else if(asi->DiskVendorId == HDD_SSD_VENDOR_SEAGATE)
				{
					asi->HostWrites  = (INT)(
						(UINT64)
						( (UINT64)asi->Attribute[j].RawValue[5] * 256 * 256 * 256 * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[4] * 256 * 256 * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[3] * 256 * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[2] * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[1] * 256
						+ (UINT64)asi->Attribute[j].RawValue[0])
						* 512 / 1024 / 1024 / 1024);
				}
				*/
				break;
			case 0xF2:
				if (asi->DiskVendorId == HDD_SSD_VENDOR_TOSHIBA && asi->HostReadsWritesUnit == HOST_READS_WRITES_GB)
				{
					asi->HostReads = (INT) (MAKELONG(
						MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
						MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
						));
				}
				else if(asi->DiskVendorId == SSD_VENDOR_INTEL || asi->DiskVendorId == HDD_SSD_VENDOR_TOSHIBA)
				{
					asi->HostReads  = (INT)(MAKELONG(
						MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
						MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
						) * 0.03125); //  0.03125 = 65536 * 512 / 1024 / 1024 / 1024;
				}
				else if (asi->DiskVendorId == SSD_VENDOR_SANDFORCE || asi->DiskVendorId == SSD_VENDOR_OCZ_VECTOR || asi->DiskVendorId == SSD_VENDOR_CORSAIR || asi->DiskVendorId == SSD_VENDOR_KINGSTON)
				{
					asi->HostReads  = (INT)(MAKELONG(
						MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
						MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
						));
				}
				else if (asi->DiskVendorId == SSD_VENDOR_SAMSUNG && asi->HostReadsWritesUnit == HOST_READS_WRITES_GB)
				{
					asi->HostReads = (INT) (MAKELONG(
						MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
						MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
						));
				}
				else if(asi->DiskVendorId == SSD_VENDOR_SAMSUNG)
				{
					asi->HostReads  = (INT)(
						(UINT64)
						( (UINT64)asi->Attribute[j].RawValue[5] * 256 * 256 * 256 * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[4] * 256 * 256 * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[3] * 256 * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[2] * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[1] * 256
						+ (UINT64)asi->Attribute[j].RawValue[0])
						* 512 / 1024 / 1024 / 1024);
				}
				else if(asi->DiskVendorId == HDD_VENDOR_WESTERN_DIGITAL)
				{
					asi->HostReads  = (INT)(
						(UINT64)
						( (UINT64)asi->Attribute[j].RawValue[5] * 256 * 256 * 256 * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[4] * 256 * 256 * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[3] * 256 * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[2] * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[1] * 256
						+ (UINT64)asi->Attribute[j].RawValue[0])
						* 512 / 1024 / 1024 / 1024);
				}
				else if(asi->DiskVendorId == SSD_VENDOR_PLEXTOR || asi->DiskVendorId == SSD_VENDOR_MICRON_MU02)
				{
					asi->HostReads  = (INT)(MAKELONG(
						MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
						MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
						)) * 32 / 1024;
				}
				else if(asi->DiskVendorId == SSD_VENDOR_SANDISK && asi->HostReadsWritesUnit == HOST_READS_WRITES_GB)
				{
					asi->HostReads = (INT) (MAKELONG(
						MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
						MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
						));
				}
				else if (asi->DiskVendorId == SSD_VENDOR_SANDISK)
				{
					asi->HostReads = (INT) (
						(UINT64)
						((UINT64) asi->Attribute[j].RawValue[5] * 256 * 256 * 256 * 256 * 256
						+ (UINT64) asi->Attribute[j].RawValue[4] * 256 * 256 * 256 * 256
						+ (UINT64) asi->Attribute[j].RawValue[3] * 256 * 256 * 256
						+ (UINT64) asi->Attribute[j].RawValue[2] * 256 * 256
						+ (UINT64) asi->Attribute[j].RawValue[1] * 256
						+ (UINT64) asi->Attribute[j].RawValue[0])
						* 512 / 1024 / 1024 / 1024);
				}
				else if (asi->DiskVendorId == SSD_VENDOR_MICRON)
				{
					if (asi->Attribute[j].CurrentValue <= 100)
					{
						asi->Life = asi->Attribute[j].CurrentValue;
					}
				}
				/*
				else if(asi->DiskVendorId == HDD_SSD_VENDOR_SEAGATE)
				{
					asi->HostReads  = (INT)(
						(UINT64)
						( (UINT64)asi->Attribute[j].RawValue[5] * 256 * 256 * 256 * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[4] * 256 * 256 * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[3] * 256 * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[2] * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[1] * 256
						+ (UINT64)asi->Attribute[j].RawValue[0])
						* 512 / 1024 / 1024 / 1024);
				}
				*/
				break;
			case 0xF9:
				if(asi->DiskVendorId == SSD_VENDOR_INTEL)
				{
					asi->NandWrites  = (INT)(MAKELONG(
						MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
						MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
						));
				}
				else if(asi->DiskVendorId == SSD_VENDOR_OCZ_VECTOR)
				{
					asi->NandWrites  = (INT)(
						(UINT64)
						( (UINT64)asi->Attribute[j].RawValue[5] * 256 * 256 * 256 * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[4] * 256 * 256 * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[3] * 256 * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[2] * 256 * 256
						+ (UINT64)asi->Attribute[j].RawValue[1] * 256
						+ (UINT64)asi->Attribute[j].RawValue[0])
						* 16 / 1024 / 1024);
				}
				break;
			case 0x64:
				if(asi->DiskVendorId == SSD_VENDOR_SANDFORCE)
				{
					asi->GBytesErased  = MAKELONG(
						MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
						MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
						);
				}
				break;
			case 0xAD:
				if(asi->DiskVendorId == SSD_VENDOR_MICRON)
				{
					asi->WearLevelingCount  = (INT)MAKELONG(
						MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
						MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
						);
				}
				break;
			case 0xB1:
				if(asi->DiskVendorId == SSD_VENDOR_SAMSUNG)
				{
					asi->WearLevelingCount  = (INT)MAKELONG(
						MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
						MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
						);
				}
				/*
				else if(asi->DiskVendorId == SSD_VENDOR_PLEXTOR)
				{
					asi->NandWrites  = (INT)(MAKELONG(
						MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
						MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
						)) * asi->PlextorNandWritesUnit / 1024;
				}
				*/
				break;
			case 0xB3:
			case 0xB4:
				if(asi->DiskVendorId == SSD_VENDOR_SAMSUNG)
				{
					if(asi->Attribute[j].CurrentValue <= 100)
					{
						asi->Life = asi->Attribute[j].CurrentValue;
					}
				}
				break;
			case 0xE7:
				if (asi->DiskVendorId == SSD_VENDOR_SANDFORCE || asi->DiskVendorId == SSD_VENDOR_CORSAIR || asi->DiskVendorId == SSD_VENDOR_KINGSTON)
				{
					if(asi->Attribute[j].CurrentValue <= 100)
					{
						asi->Life = asi->Attribute[j].CurrentValue;
					}
				}
				break;
			case 0xAA:
				if(asi->DiskVendorId == SSD_VENDOR_JMICRON && ! asi->IsRawValues8)
				{
					if(asi->Attribute[j].CurrentValue <= 100)
					{
						asi->Life = asi->Attribute[j].CurrentValue;
					}
				}
				break;
			case 0xA9:
				if(asi->DiskVendorId == SSD_VENDOR_MICRON_MU02)
				{
					if(asi->Attribute[j].CurrentValue <= 100)
					{
						asi->Life = asi->Attribute[j].CurrentValue;
					}
				}
				break;
			case 0xC6:
				if(asi->DiskVendorId == SSD_VENDOR_OCZ_VECTOR)
				{
					asi->HostReads  = (INT)(MAKELONG(
						MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
						MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
						));
				}
				break;
			case 0xC7:
				if(asi->DiskVendorId == SSD_VENDOR_OCZ_VECTOR)
				{
					asi->HostWrites  = (INT)(MAKELONG(
						MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
						MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
						));
				}
				break;
			case 0xF5:
				// NAND Page Size = 8KBytes
				// http://www.overclock.net/t/1145150/official-crucial-ssd-owners-club/1290
				if (asi->DiskVendorId == SSD_VENDOR_MICRON)
				{
					asi->NandWrites = (INT) ((UINT64)
						( (UINT64) asi->Attribute[j].RawValue[5] * 256 * 256 * 256 * 256 * 256
						+ (UINT64) asi->Attribute[j].RawValue[4] * 256 * 256 * 256 * 256
						+ (UINT64) asi->Attribute[j].RawValue[3] * 256 * 256 * 256
						+ (UINT64) asi->Attribute[j].RawValue[2] * 256 * 256
						+ (UINT64) asi->Attribute[j].RawValue[1] * 256
						+ (UINT64) asi->Attribute[j].RawValue[0])
						* 8 / 1024 / 1024);
				}
				else if (asi->DiskVendorId == SSD_VENDOR_MICRON_MU02)
				{
					asi->NandWrites = (INT)(MAKELONG(
						MAKEWORD(asi->Attribute[j].RawValue[0], asi->Attribute[j].RawValue[1]),
						MAKEWORD(asi->Attribute[j].RawValue[2], asi->Attribute[j].RawValue[3])
						)) * 32 / 1024;
				}
				break;
			case 0xF6:
				if (asi->DiskVendorId == SSD_VENDOR_MICRON)
				{
					asi->HostWrites = (INT) (
						(UINT64)
						((UINT64) asi->Attribute[j].RawValue[5] * 256 * 256 * 256 * 256 * 256
						+ (UINT64) asi->Attribute[j].RawValue[4] * 256 * 256 * 256 * 256
						+ (UINT64) asi->Attribute[j].RawValue[3] * 256 * 256 * 256
						+ (UINT64) asi->Attribute[j].RawValue[2] * 256 * 256
						+ (UINT64) asi->Attribute[j].RawValue[1] * 256
						+ (UINT64) asi->Attribute[j].RawValue[0])
						* 512 / 1024 / 1024 / 1024);
				}
				break;
			default:
				break;
			}
			j++;
		}
	}
	asi->AttributeCount = j;

	if(asi->AttributeCount > 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CAtaSmart::FillSmartThreshold(ATA_SMART_INFO* asi)
{
	// 2016/04/18
	// https://github.com/hiyohiyo/CrystalDiskInfo/issues/1
	int count = 0;
	for (int i = 0; i < MAX_ATTRIBUTE; i++)
	{
		SMART_THRESHOLD* pst = (SMART_THRESHOLD*)&(asi->SmartReadThreshold[i * sizeof(SMART_THRESHOLD) + 2]);
		if (pst->Id != 0)
		{
			for (DWORD j = 0; j < asi->AttributeCount; j++)
			{
				if (pst->Id == asi->Attribute[j].Id)
				{
					memcpy(&(asi->Threshold[j]), pst, sizeof(SMART_THRESHOLD));
					count++;
				}
			}
		}
	}

	// 2013/04/13 Added P400e SSD SMART Implementation support
	// Threshold = Attribute[].Reserved
	if(asi->DiskVendorId == SSD_VENDOR_MICRON && count == 0)
	{
		for(int i = 0; i < MAX_ATTRIBUTE; i++)
		{
			if(asi->Attribute[i].Reserved > 0)
			{
				asi->Threshold[i].Id = asi->Attribute[i].Id;
				asi->Threshold[i].ThresholdValue = asi->Attribute[i].Reserved;
				count++;
			}
		}
	}

	if(count > 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*---------------------------------------------------------------------------*/
// Support Functions
/*---------------------------------------------------------------------------*/

DWORD CAtaSmart::CheckDiskStatus(DWORD i)
{
	if (vars.GetCount() == 0)
	{
		return DISK_STATUS_UNKNOWN;
	}

	// NVMe
	if (vars[i].InterfaceType == INTERFACE_TYPE_NVME)
	{
		if (vars[i].Life > 10)
		{
			return DISK_STATUS_GOOD;
		}
		else if (vars[i].Life == 10)
		{
			return DISK_STATUS_CAUTION;
		}
		else if (vars[i].Life < 10)
		{
			return DISK_STATUS_BAD;
		}
	}

	if(! vars[i].IsSmartCorrect)
	{
		return DISK_STATUS_UNKNOWN;
	}

	// HDD
	if(! vars[i].IsSsd && ! vars[i].IsThresholdCorrect)
	{
		return DISK_STATUS_UNKNOWN;
	}

	if(vars[i].IsThresholdBug)
	{
		return DISK_STATUS_UNKNOWN;
	}

	// DEBUG //
	// vars[i].Attribute[3].RawValue[0] = rand() % 256;

	int error = 0;
	int caution = 0;
	BOOL flagUnknown = TRUE;

	for(DWORD j = 0; j < vars[i].AttributeCount; j++)
	{
		// Check overlap
		for(DWORD k = 0; k < j; k++)
		{
			if(vars[i].Attribute[k].Id != 0 && vars[i].Attribute[j].Id == vars[i].Attribute[k].Id)
			{
				return DISK_STATUS_UNKNOWN;
			}
		}

		// Read Error Rate Bug
		if(vars[i].DiskVendorId == SSD_VENDOR_SANDFORCE && vars[i].Attribute[j].Id == 0x01
			&& vars[i].Attribute[j].CurrentValue == 0 && vars[i].Attribute[j].RawValue[0] == 0 && vars[i].Attribute[j].RawValue[1] == 0)
		{
		}
		// Temperature Threshold Bug
		else if(vars[i].Attribute[j].Id == 0xC2)
		{
		}
		else if(vars[i].IsSsd && vars[i].IsRawValues8)
		{
		}
		else if(vars[i].IsSsd && ! vars[i].IsRawValues8
		&&	vars[i].Threshold[j].ThresholdValue != 0
		&& 	vars[i].Attribute[j].CurrentValue < vars[i].Threshold[j].ThresholdValue)
		{
			error++;
		}
		else if((
			(0x01 <= vars[i].Attribute[j].Id && vars[i].Attribute[j].Id <= 0x0D)
//		||	vars[i].Attribute[j].Id == 0xB8
		||	(0xBB <= vars[i].Attribute[j].Id && vars[i].Attribute[j].Id <= 0xBD)
		||	(0xBF <= vars[i].Attribute[j].Id && vars[i].Attribute[j].Id <= 0xC1)
		||	(0xC3 <= vars[i].Attribute[j].Id && vars[i].Attribute[j].Id <= 0xD1)
		||	(0xD3 <= vars[i].Attribute[j].Id && vars[i].Attribute[j].Id <= 0xD4)
		||	(0xDC <= vars[i].Attribute[j].Id && vars[i].Attribute[j].Id <= 0xE4)
		||	(0xE6 <= vars[i].Attribute[j].Id && vars[i].Attribute[j].Id <= 0xE7)
		||	vars[i].Attribute[j].Id == 0xF0
		||	vars[i].Attribute[j].Id == 0xFA
		||	vars[i].Attribute[j].Id == 0xFE
		)
		&&	vars[i].Threshold[j].ThresholdValue != 0
		&& 	vars[i].Attribute[j].CurrentValue < vars[i].Threshold[j].ThresholdValue)
		{
			error++;
		}

		if(vars[i].IsSsd && vars[i].Threshold[j].ThresholdValue != 0)
		{
			flagUnknown = FALSE;
		}

		if( vars[i].Attribute[j].Id == 0x05 // Reallocated Sectors Count
		||	vars[i].Attribute[j].Id == 0xC5 // Current Pending Sector Count
		||	vars[i].Attribute[j].Id == 0xC6 // Off-Line Scan Uncorrectable Sector Count
		)
		{
			if(vars[i].Attribute[j].RawValue[0] == 0xFF
			&& vars[i].Attribute[j].RawValue[1] == 0xFF
			&& vars[i].Attribute[j].RawValue[2] == 0xFF
			&& vars[i].Attribute[j].RawValue[3] == 0xFF)
			{
			}
			else
			{
				WORD raw = MAKEWORD(vars[i].Attribute[j].RawValue[0], vars[i].Attribute[j].RawValue[1]);
				WORD threshold; 
				switch(vars[i].Attribute[j].Id)
				{
				case 0x05:
					threshold = vars[i].Threshold05;
					break;
				case 0xC5:
					threshold = vars[i].ThresholdC5;
					break;
				case 0xC6:
					threshold = vars[i].ThresholdC6;
					break;
				}
				if(threshold > 0 && raw >= threshold && ! vars[i].IsSsd)
				{
					caution = 1;
				}
			}
			if(! vars[i].IsSsd)
			{
				flagUnknown = FALSE;
			}
		}
		else 
		if((vars[i].Attribute[j].Id == 0xE8 && (vars[i].DiskVendorId == SSD_VENDOR_INTEL || vars[i].DiskVendorId == SSD_VENDOR_PLEXTOR || vars[i].DiskVendorId == SSD_VENDOR_SANDISK))
		|| (vars[i].Attribute[j].Id == 0xBB && vars[i].DiskVendorId == SSD_VENDOR_MTRON)
		||((vars[i].Attribute[j].Id == 0xB4 || vars[i].Attribute[j].Id == 0xB3) && vars[i].DiskVendorId == SSD_VENDOR_SAMSUNG)
		|| (vars[i].Attribute[j].Id == 0xD1 && vars[i].DiskVendorId == SSD_VENDOR_INDILINX)
		|| (vars[i].Attribute[j].Id == 0xE7 && vars[i].DiskVendorId == SSD_VENDOR_SANDFORCE)
		|| (vars[i].Attribute[j].Id == 0xAA && vars[i].DiskVendorId == SSD_VENDOR_JMICRON && ! vars[i].IsRawValues8)
		|| (vars[i].Attribute[j].Id == 0xCA && vars[i].DiskVendorId == SSD_VENDOR_MICRON)
		|| (vars[i].Attribute[j].Id == 0xE9 && (vars[i].DiskVendorId == SSD_VENDOR_OCZ || vars[i].DiskVendorId == SSD_VENDOR_OCZ_VECTOR))
		)
		{
			flagUnknown = FALSE;
			if(vars[i].Attribute[j].CurrentValue == 0
			|| vars[i].Attribute[j].CurrentValue < vars[i].Threshold[j].ThresholdValue
			)
			{
				error = 1;
			}
			else if(vars[i].Attribute[j].CurrentValue <= vars[i].ThresholdFF)
			{
				caution = 1;
			}
		}
	}

	if(error > 0)
	{
		return DISK_STATUS_BAD;
	}
	else if(flagUnknown)
	{
		return DISK_STATUS_UNKNOWN;
	}
	else if(caution > 0)
	{
		return DISK_STATUS_CAUTION;
	}
	else
	{
		return DISK_STATUS_GOOD;
	}
}

VOID CAtaSmart::ChangeByteOrder(PCHAR str, DWORD length)
{
	CHAR	temp;
	for(DWORD i = 0; i < length; i += 2)
	{
		temp = str[i];
		str[i] = str[i+1];
		str[i+1] = temp;
	}
}

BOOL CAtaSmart::CheckAsciiStringError(PCHAR str, DWORD length)
{
	BOOL flag = FALSE;
	for(DWORD i = 0; i < length; i++)
	{
		if((0x00 < str[i] && str[i] < 0x20))
		{
			str[i] = 0x20;
			break;
		}
		else if(str[i] >= 0x7f)
		{ 
			flag = TRUE;
			break;
		}
	}
	return flag;
}

DWORD CAtaSmart::GetPowerOnHours(DWORD rawValue, DWORD timeUnitType)
{
	switch(timeUnitType)
	{
	case POWER_ON_UNKNOWN:
		return 0;
		break;
	case POWER_ON_HOURS:
		return rawValue;
		break;
	case POWER_ON_MINUTES:
		return rawValue / 60;
		break;
	case POWER_ON_HALF_MINUTES:
		return rawValue / 120;
		break;
	case POWER_ON_SECONDS:
		return rawValue / 60 / 60;
		break;
	case POWER_ON_10_MINUTES:
		return rawValue / 6;
		break;
	case POWER_ON_MILLI_SECONDS:
		return rawValue;
		break;
	default:
		return rawValue;
		break;
	}
}

DWORD CAtaSmart::GetPowerOnHoursEx(DWORD i, DWORD timeUnitType)
{
	DWORD rawValue = vars[i].PowerOnRawValue;
	switch(timeUnitType)
	{
	case POWER_ON_UNKNOWN:
		return 0;
		break;
	case POWER_ON_HOURS:
		return rawValue;
		break;
	case POWER_ON_MINUTES:
		return rawValue / 60;
		break;
	case POWER_ON_HALF_MINUTES:
		return rawValue / 120;
		break;
	case POWER_ON_SECONDS:
		return rawValue / 60 / 60;
		break;
	case POWER_ON_10_MINUTES:
		return rawValue / 6;
		break;
	case POWER_ON_MILLI_SECONDS:
		return rawValue;
		break;
	default:
		return rawValue;
		break;
	}
}

DWORD CAtaSmart::GetTransferMode(WORD w63, WORD w76, WORD w77, WORD w88, CString &current, CString &max, CString &type, INTERFACE_TYPE* interfaceType)
{
	DWORD tm = TRANSFER_MODE_PIO;
	current = max = _T("");
	type = _T("Parallel ATA");
	*interfaceType = INTERFACE_TYPE_PATA;

	// Multiword DMA or PIO
	if(w63 & 0x0700)
	{
		tm = TRANSFER_MODE_PIO_DMA;
		current = max = _T("PIO/DMA");
	}

	// Ultara DMA Max Transfer Mode
		 if(w88 & 0x0040){tm = TRANSFER_MODE_ULTRA_DMA_133; max = _T("UDMA/133");}
	else if(w88 & 0x0020){tm = TRANSFER_MODE_ULTRA_DMA_100; max = _T("UDMA/100");}
	else if(w88 & 0x0010){tm = TRANSFER_MODE_ULTRA_DMA_66;  max = _T("UDMA/66");}
	else if(w88 & 0x0008){tm = TRANSFER_MODE_ULTRA_DMA_44;  max = _T("UDMA/44");}
	else if(w88 & 0x0004){tm = TRANSFER_MODE_ULTRA_DMA_33;  max = _T("UDMA/33");}
	else if(w88 & 0x0002){tm = TRANSFER_MODE_ULTRA_DMA_25;  max = _T("UDMA/25");}
	else if(w88 & 0x0001){tm = TRANSFER_MODE_ULTRA_DMA_16;  max = _T("UDMA/16");}

	// Ultara DMA Current Transfer Mode
		 if(w88 & 0x4000){current = _T("UDMA/133");}
	else if(w88 & 0x2000){current = _T("UDMA/100");}
	else if(w88 & 0x1000){current = _T("UDMA/66");}
	else if(w88 & 0x0800){current = _T("UDMA/44");}
	else if(w88 & 0x0400){current = _T("UDMA/33");}
	else if(w88 & 0x0200){current = _T("UDMA/25");}
	else if(w88 & 0x0100){current = _T("UDMA/16");}

	// Serial ATA
	if(w76 != 0x0000 && w76 != 0xFFFF)
	{
		current = max = _T("SATA/150");
		type = _T("Serial ATA");
		*interfaceType = INTERFACE_TYPE_SATA;
	}

		 if(w76 & 0x0010){tm = TRANSFER_MODE_UNKNOWN; current = max = _T("----");}
	else if(w76 & 0x0008){tm = TRANSFER_MODE_SATA_600; current = _T("----"); max = _T("SATA/600");}
	else if(w76 & 0x0004){tm = TRANSFER_MODE_SATA_300; current = _T("----"); max = _T("SATA/300");}
	else if(w76 & 0x0002){tm = TRANSFER_MODE_SATA_150; current = _T("----"); max = _T("SATA/150");}

	// 2013/5/1 ACS-3
		 if(((w77 & 0x000E) >> 1) == 3){current = _T("SATA/600");}
	else if(((w77 & 0x000E) >> 1) == 2){current = _T("SATA/300");}
	else if(((w77 & 0x000E) >> 1) == 1){current = _T("SATA/150");}

	return tm;
}

VOID CAtaSmart::GetTransferModePCIe(CString &current, CString &max, SlotMaxCurrSpeed slotspeed)
{
	max = SlotSpeedToString(slotspeed.Maximum);
	current = SlotSpeedToString(slotspeed.Current);
}

DWORD CAtaSmart::GetTimeUnitType(CString model, CString firmware, DWORD major, DWORD transferMode)
{
	model.MakeUpper();

	if(model.Find(_T("FUJITSU")) == 0)
	{
		if(major >= 8)
		{
			return POWER_ON_HOURS;
		}
		else
		{
			return POWER_ON_SECONDS;
		}
	}
	else if(model.Find(_T("HITACHI_DK")) == 0)
	{
		return POWER_ON_MINUTES;
	}
	else if(model.Find(_T("MAXTOR")) == 0)
	{
		if(transferMode >= TRANSFER_MODE_SATA_300
		|| model.Find(_T("MAXTOR 6H")) == 0		// Maxtor DiamondMax 11 family
		|| model.Find(_T("MAXTOR 7H500")) == 0	// Maxtor MaXLine Pro 500 family
		|| model.Find(_T("MAXTOR 6L0")) == 0	// Maxtor DiamondMax Plus D740X family
		|| model.Find(_T("MAXTOR 4K")) == 0		// Maxtor DiamondMax D540X-4K family
		)
		{
			return POWER_ON_HOURS;
		}
		else
		{
			return POWER_ON_MINUTES;
		}
	}
	else if(model.Find(_T("SAMSUNG")) == 0)
	{
		if(transferMode >= TRANSFER_MODE_SATA_300)
		{
			return POWER_ON_HOURS;
		}
		else if(-23 >= _tstoi(firmware.Right(3)) && _tstoi(firmware.Right(3)) >= -39)
		{
			return POWER_ON_HALF_MINUTES;
		}
		else if(model.Find(_T("SAMSUNG SV")) == 0
		||		model.Find(_T("SAMSUNG SP")) == 0
		||		model.Find(_T("SAMSUNG HM")) == 0
		||		model.Find(_T("SAMSUNG MP")) == 0
		)
		{
			return POWER_ON_HALF_MINUTES;
		}
		else
		{
			return POWER_ON_HOURS;
		}
	}
	// 2012/1/15
	// https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=504;id=diskinfo#504
	// http://sourceforge.jp/ticket/browse.php?group_id=4394&tid=27443
	else if(
	   ((model.Find(_T("CFD_CSSD-S6TM128NMPQ")) == 0 || model.Find(_T("CFD_CSSD-S6TM256NMPQ")) == 0) && (firmware.Find(_T("VM21")) == 0 || firmware.Find(_T("VN21")) == 0))
	|| ((model.Find(_T("PX-128M2P")) != -1 || model.Find(_T("PX-256M2P")) != -1) && _tstof(firmware) < 1.059)
	|| (model.Find(_T("Corsair Performance Pro")) == 0 && _tstof(firmware) < 1.059)
	)
	{
		return POWER_ON_10_MINUTES;
	}
	// https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=1174;id=diskinfo#1174
	else if(
		   (model.Find(_T("INTEL SSDSC2CW")) == 0 && model.Find(_T("A3")) > 0) // Intel SSD 520 Series
		|| (model.Find(_T("INTEL SSDSC2BW")) == 0 && model.Find(_T("A3")) > 0) // Intel SSD 520 Series
		|| (model.Find(_T("INTEL SSDSC2CT")) == 0 && model.Find(_T("A3")) > 0) // Intel SSD 330 Series
		)
	{
		return POWER_ON_MILLI_SECONDS;
	}
	else
	{
		return POWER_ON_HOURS;
	}
}

DWORD CAtaSmart::GetAtaMajorVersion(WORD w80, CString &majorVersion)
{
	DWORD major = 0;

	if(w80 == 0x0000 || w80 == 0xFFFF)
	{
		return 0;
	}

	for(int i = 14; i > 0; i--)
	{
		if((w80 >> i) & 0x1)
		{
			major = i;
			break;
		}
	}

	if(major > 10)
	{
		majorVersion = _T("");
	}
	else if(major == 11)
	{
		majorVersion = _T("ACS-4");
	}
	else if(major == 10)
	{
		majorVersion = _T("ACS-3");
	}
	else if(major == 9)
	{
		majorVersion = _T("ACS-2");
	}
	else if(major == 8)
	{
		majorVersion = _T("ATA8-ACS");
	}
	else if(major >= 4)
	{
		majorVersion.Format(_T("ATA/ATAPI-%d"), major);
	}
	else if(major == 0)
	{
		majorVersion = _T("----");
	}
	else
	{
		majorVersion.Format(_T("ATA-%d"), major);
	}

	return major;
}

// Last Update : 2011/03/21
// Reference : http://www.t13.org/Documents/MinutesDefault.aspx?DocumentType=4&DocumentStage=2
//           - d2161r0-ATAATAPI_Command_Set_-_3.pdf
//           - d1153r18-ATA-ATAPI-4.pdf 
VOID CAtaSmart::GetAtaMinorVersion(WORD w81, CString &minor)
{
	switch(w81)
	{
	case 0x0000:
	case 0xFFFF:
				//	minor = _T("Not Reported");									break;
					minor = _T("----");											break;
	case 0x0001:	minor = _T("ATA (ATA-1) X3T9.2 781D prior to revision 4");	break;
	case 0x0002:	minor = _T("ATA-1 published, ANSI X3.221-1994");			break;
	case 0x0003:	minor = _T("ATA (ATA-1) X3T10 781D revision 4");			break;
	case 0x0004:	minor = _T("ATA-2 published, ANSI X3.279-1996");			break;
	case 0x0005:	minor = _T("ATA-2 X3T10 948D prior to revision 2k");		break;
	case 0x0006:	minor = _T("ATA-3 X3T10 2008D revision 1");					break;
	case 0x0007:	minor = _T("ATA-2 X3T10 948D revision 2k");					break;
	case 0x0008:	minor = _T("ATA-3 X3T10 2008D revision 0");					break;
	case 0x0009:	minor = _T("ATA-2 X3T10 948D revision 3");					break;
	case 0x000A:	minor = _T("ATA-3 published, ANSI X3.298-199x");			break;
	case 0x000B:	minor = _T("ATA-3 X3T10 2008D revision 6");					break;
	case 0x000C:	minor = _T("ATA-3 X3T13 2008D revision 7 and 7a");			break;
	case 0x000D:	minor = _T("ATA/ATAPI-4 X3T13 1153D version 6");			break;
	case 0x000E:	minor = _T("ATA/ATAPI-4 T13 1153D version 13");				break;
	case 0x000F:	minor = _T("ATA/ATAPI-4 X3T13 1153D version 7");			break;
	case 0x0010:	minor = _T("ATA/ATAPI-4 T13 1153D version 18");				break;
	case 0x0011:	minor = _T("ATA/ATAPI-4 T13 1153D version 15");				break;
	case 0x0012:	minor = _T("ATA/ATAPI-4 published, ANSI INCITS 317-1998");	break;
	case 0x0013:	minor = _T("ATA/ATAPI-5 T13 1321D version 3");				break;
	case 0x0014:	minor = _T("ATA/ATAPI-4 T13 1153D version 14");				break;
	case 0x0015:	minor = _T("ATA/ATAPI-5 T13 1321D version 1");				break;
	case 0x0016:	minor = _T("ATA/ATAPI-5 published, ANSI INCITS 340-2000");	break;
	case 0x0017:	minor = _T("ATA/ATAPI-4 T13 1153D version 17");				break;
	case 0x0018:	minor = _T("ATA/ATAPI-6 T13 1410D version 0");				break;
	case 0x0019:	minor = _T("ATA/ATAPI-6 T13 1410D version 3a");				break;
	case 0x001A:	minor = _T("ATA/ATAPI-7 T13 1532D version 1");				break;
	case 0x001B:	minor = _T("ATA/ATAPI-6 T13 1410D version 2");				break;
	case 0x001C:	minor = _T("ATA/ATAPI-6 T13 1410D version 1");				break;
	case 0x001D:	minor = _T("ATA/ATAPI-7 published ANSI INCITS 397-2005.");	break;
	case 0x001E:	minor = _T("ATA/ATAPI-7 T13 1532D version 0");				break;
	case 0x001F:	minor = _T("ACS-3 Revision 3b");							break;
	case 0x0021:	minor = _T("ATA/ATAPI-7 T13 1532D version 4a");				break;
	case 0x0022:	minor = _T("ATA/ATAPI-6 published, ANSI INCITS 361-2002");	break;
	case 0x0027:	minor = _T("ATA8-ACS version 3c");							break;
	case 0x0028:	minor = _T("ATA8-ACS version 6");							break;
	case 0x0029:	minor = _T("ATA8-ACS version 4");							break;
	case 0x0031:	minor = _T("ACS-2 Revision 2");								break;
	case 0x0033:	minor = _T("ATA8-ACS version 3e");							break;
	case 0x0039:	minor = _T("ATA8-ACS version 4c");							break;
	case 0x0042:	minor = _T("ATA8-ACS version 3f");							break;
	case 0x0052:	minor = _T("ATA8-ACS version 3b");							break;
	case 0x005E:	minor = _T("ACS-4 Revision 5");								break;
	case 0x006D:	minor = _T("ACS-3 Revision 4");								break;
	case 0x0082:	minor = _T("ACS-2 published, ANSI INCITS 482-2012");		break;
	case 0x0107:	minor = _T("ATA8-ACS version 2d");							break;
	case 0x010A:	minor = _T("ACS-3 published, ANSI INCITS 522-2014");		break;
	case 0x0110:	minor = _T("ACS-2 Revision 3");								break;
	case 0x011B:	minor = _T("ACS-3 Revision 4");								break;
	default:	//	minor.Format(_T("Reserved [%04Xh]"), w81);					break;
					minor.Format(_T("---- [%04Xh]"), w81);						break;
	}
}

CString CAtaSmart::GetModelSerial(CString &model, CString &serialNumber)
{
	CString modelSerial;
	modelSerial = model + serialNumber;
	modelSerial.Replace(_T("\\"), _T(""));
	modelSerial.Replace(_T("/"), _T(""));
	modelSerial.Replace(_T(":"), _T(""));
	modelSerial.Replace(_T("*"), _T(""));
	modelSerial.Replace(_T("?"), _T(""));
	modelSerial.Replace(_T("\""), _T(""));
	modelSerial.Replace(_T("<"), _T(""));
	modelSerial.Replace(_T(">"), _T(""));
	modelSerial.Replace(_T("|"), _T(""));

	return modelSerial;
}
