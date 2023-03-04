/*---------------------------------------------------------------------------*/
//       Author : Minkyu Kim
//          Web : http://naraeon.net/
//                https://github.com/ebangin127/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "SlotSpeedGetter.h"
#include <wbemcli.h>
#include <comutil.h>
#include <Setupapi.h>
#include <Cfgmgr32.h>
//#pragma comment(lib, "Cfgmgr32.lib")
#pragma comment(lib, "Setupapi.lib")
#pragma comment(lib, "wbemuuid.lib")
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

DEFINE_GUID(GUID_DEVCLASS_SCSIADAPTER, 0x4D36E97B, 0xE325, 0x11CE, 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18);


typedef BOOL(WINAPI *FN_SetupDiGetDeviceProperty)(
	__in       HDEVINFO DeviceInfoSet,
	__in       PSP_DEVINFO_DATA DeviceInfoData,
	__in       const DEVPROPKEY *PropertyKey,
	__out      DEVPROPTYPE *PropertyType,
	__out_opt  PBYTE PropertyBuffer,
	__in       DWORD PropertyBufferSize,
	__out_opt  PDWORD RequiredSize,
	__in       DWORD Flags
	);

CString GetStringValueFromQuery(IWbemServices* pIWbemServices, const CString query, const CString valuename)
{
	IEnumWbemClassObject* pEnumCOMDevs = NULL;
	IWbemClassObject* pCOMDev = NULL;
	ULONG uReturned = 0;
	CString	result = L"";

	try
	{
		if (SUCCEEDED(pIWbemServices->ExecQuery(_bstr_t(L"WQL"),
			_bstr_t(query), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumCOMDevs)))
		{
			while (pEnumCOMDevs && SUCCEEDED(pEnumCOMDevs->Next(10000, 1, &pCOMDev, &uReturned)) && uReturned == 1)
			{
				VARIANT pVal;
				VariantInit(&pVal);

				if (pCOMDev->Get(L"DeviceID", 0L, &pVal, NULL, NULL) == WBEM_S_NO_ERROR && pVal.vt > VT_NULL)
				{
					result = pVal.bstrVal;
					VariantClear(&pVal);
				}
				VariantInit(&pVal);
			}
		}
	}
	catch (...)
	{
		result = L"";
	}

	SAFE_RELEASE(pCOMDev);
	SAFE_RELEASE(pEnumCOMDevs);
	return result;
}

CString GetDeviceIDFromPhysicalDriveID(const INT physicalDriveId, const BOOL IsKernelVerEqualOrOver6)
{
	const CString query2findstorage = L"ASSOCIATORS OF {Win32_DiskDrive.DeviceID='\\\\.\\PhysicalDrive%d'} WHERE ResultClass=Win32_PnPEntity";
	const CString query2findcontroller = L"ASSOCIATORS OF {Win32_PnPEntity.DeviceID='%s'} WHERE AssocClass=Win32_SCSIControllerDevice";
	CString query;
	CString result;
	IWbemLocator* pIWbemLocator = NULL;
	IWbemServices* pIWbemServices = NULL;
	//BOOL flag = FALSE;

	try
	{
		if (SUCCEEDED(CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER,
			IID_IWbemLocator, (LPVOID *)&pIWbemLocator)))
		{
			long securityFlag = 0;
			if (IsKernelVerEqualOrOver6) { securityFlag = WBEM_FLAG_CONNECT_USE_MAX_WAIT; }
			if (SUCCEEDED(pIWbemLocator->ConnectServer(_bstr_t(L"\\\\.\\root\\cimv2"),
				NULL, NULL, 0L, securityFlag, NULL, NULL, &pIWbemServices)))
			{
				if (SUCCEEDED(CoSetProxyBlanket(pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
					NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE)))
				{
					query.Format(query2findstorage, physicalDriveId);
					CString StorageID = GetStringValueFromQuery(pIWbemServices, query, L"DeviceID");
					if(!StorageID.IsEmpty())
					{
						query.Format(query2findcontroller.GetString(), StorageID.GetString());
						CString ControllerID = GetStringValueFromQuery(pIWbemServices, query, L"DeviceID");
						result = ControllerID;
					}
					else
					{
						result = L"";
					}
				}
			}
		}
	}
	catch (...)
	{
		result = L"";
	}
	SAFE_RELEASE(pIWbemServices);
	SAFE_RELEASE(pIWbemLocator);
	return result;
}

SlotMaxCurrSpeed ConvertOSResult(const OSSlotMaxCurrSpeed OSLevelResult)
{
	SlotMaxCurrSpeed result{};
	result.Current.LinkWidth = PCIeDataWidth(OSLevelResult.Current.LinkWidth);
	result.Current.SpecVersion = PCIeSpecification(OSLevelResult.Current.SpecVersion);
	result.Maximum.LinkWidth = PCIeDataWidth(OSLevelResult.Maximum.LinkWidth);
	result.Maximum.SpecVersion = PCIeSpecification(OSLevelResult.Maximum.SpecVersion);
	return result;
}

SlotMaxCurrSpeed GetSlotMaxCurrSpeedFromDeviceID(const CString DeviceID)
{
	DWORD CurrentDevice = 0;
	OSSlotMaxCurrSpeed OSLevelResult{};
	GUID SCSIAdapterGUID = GUID_DEVCLASS_SCSIADAPTER;
	HDEVINFO ClassDeviceInformations = SetupDiGetClassDevs(&SCSIAdapterGUID, nullptr, 0, DIGCF_PRESENT);

	BOOL LastResult{};
	do
	{
		SP_DEVINFO_DATA DeviceInfoData = {sizeof(SP_DEVINFO_DATA)};
		LastResult = SetupDiEnumDeviceInfo(ClassDeviceInformations, CurrentDevice, &DeviceInfoData);

		BOOL DeviceIDFound = FALSE;	
		TCHAR DeviceIDBuffer[MAX_PATH] = {};
		BOOL GetDeviceResult = CM_Get_Device_ID(DeviceInfoData.DevInst, DeviceIDBuffer, MAX_PATH, 0);
		DeviceIDFound = (GetDeviceResult == ERROR_SUCCESS) && (DeviceID.Compare(DeviceIDBuffer) == 0);
		if (LastResult && DeviceIDFound)
		{
			DEVPROPTYPE PropertyType{};
			BYTE ResultBuffer[1024]{};
			DWORD RequiredSize{};

			const HMODULE hMod = LoadLibrary(TEXT("Setupapi.dll"));
			if (hMod && hMod != INVALID_HANDLE_VALUE) {
				FN_SetupDiGetDeviceProperty SetupDiGetDeviceProperty =
					(FN_SetupDiGetDeviceProperty)GetProcAddress(hMod, "SetupDiGetDevicePropertyW");
				//Compatible with pre-vista era windows.
				if (SetupDiGetDeviceProperty) {
					SetupDiGetDeviceProperty(ClassDeviceInformations, &DeviceInfoData,
						&DEVPKEY_PciDevice_MaxLinkWidth, &PropertyType, ResultBuffer,
						sizeof(ResultBuffer), &RequiredSize, 0);
					OSLevelResult.Maximum.LinkWidth = ResultBuffer[0];
					SetupDiGetDeviceProperty(ClassDeviceInformations, &DeviceInfoData,
						&DEVPKEY_PciDevice_MaxLinkSpeed, &PropertyType, ResultBuffer,
						sizeof(ResultBuffer), &RequiredSize, 0);
					OSLevelResult.Maximum.SpecVersion = ResultBuffer[0];

					SetupDiGetDeviceProperty(ClassDeviceInformations, &DeviceInfoData,
						&DEVPKEY_PciDevice_CurrentLinkWidth, &PropertyType, ResultBuffer,
						sizeof(ResultBuffer), &RequiredSize, 0);
					OSLevelResult.Current.LinkWidth = ResultBuffer[0];
					SetupDiGetDeviceProperty(ClassDeviceInformations, &DeviceInfoData,
						&DEVPKEY_PciDevice_CurrentLinkSpeed, &PropertyType, ResultBuffer,
						sizeof(ResultBuffer), &RequiredSize, 0);
					OSLevelResult.Current.SpecVersion = ResultBuffer[0];
				}
			}
			break;
		}

		++CurrentDevice;
	} while (LastResult);

	return ConvertOSResult(OSLevelResult);
}

SlotMaxCurrSpeed GetPCIeSlotSpeed(const INT physicalDriveId, const BOOL IsKernelVerEqualOrOver6)
{
	CString DeviceID = GetDeviceIDFromPhysicalDriveID(physicalDriveId, IsKernelVerEqualOrOver6);
	return GetSlotMaxCurrSpeedFromDeviceID(DeviceID);
}

CString SlotSpeedToString(SlotSpeed speedtoconv)
{
	CString result = L"";
	if (speedtoconv.SpecVersion == 0 || speedtoconv.LinkWidth == 0)
	{
		result.Format(L"----");
	}
	else
	{
		result.Format(L"PCIe %d.0 x%d", speedtoconv.SpecVersion, speedtoconv.LinkWidth);
	}
	return result;
}