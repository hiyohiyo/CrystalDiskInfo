/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once

//-----------------------------------------------------------------------------
//
// Prototypes
//
//-----------------------------------------------------------------------------

BOOL InitializeDll(HMODULE* hModule);
BOOL DeinitializeDll(HMODULE* hModule);

//-----------------------------------------------------------------------------
//
// Funtions
//
//-----------------------------------------------------------------------------

// DLL
//_GetDllStatus GetDllStatus = NULL;
_GetDllVersion GetDllVersion = NULL;
_GetControllerCount GetControllerCount = NULL;
// _GetControllerInfoFx GetControllerInfoFx = NULL;
_GetSmartInfoFx GetSmartInfoFx = NULL;
_GetIdentifyInfoFx GetIdentifyInfoFx = NULL;

//-----------------------------------------------------------------------------
//
// Initialize
//
//-----------------------------------------------------------------------------

BOOL InitializeDll(HMODULE* hModule)
{
#ifdef _M_X64
	*hModule = LoadLibraryA("JMicronUsbRaid64.dll");
#else
	*hModule = LoadLibraryA("JMicronUsbRaid32.dll");
#endif

	if (*hModule == NULL)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------------
	// GetProcAddress
	//-----------------------------------------------------------------------------
	// DLL
//	GetDllStatus = (_GetDllStatus)GetProcAddress(*hModule, "GetDllStatus");
	GetDllVersion = (_GetDllVersion)GetProcAddress(*hModule, "GetDllVersion");
	GetControllerCount = (_GetControllerCount)GetProcAddress(*hModule, "GetControllerCount");
//	GetControllerInfoFx = (_GetControllerInfoFx)GetProcAddress(*hModule, "GetControllerInfoFx");
	GetSmartInfoFx = (_GetSmartInfoFx)GetProcAddress(*hModule, "GetSmartInfoFx");
	GetIdentifyInfoFx = (_GetIdentifyInfoFx)GetProcAddress(*hModule, "GetIdentifyInfoFx");

	//-----------------------------------------------------------------------------
	// Check Functions
	//-----------------------------------------------------------------------------
	if (!(
//		GetDllStatus
		GetDllVersion
		&& GetControllerCount
//		&& GetControllerInfoFx
		&& GetSmartInfoFx
		&& GetIdentifyInfoFx
		))
	{
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
//
// Deinitialize
//
//-----------------------------------------------------------------------------

BOOL DeinitializeDll(HMODULE* hModule)
{
	BOOL result = FALSE;

	if (*hModule == NULL)
	{
		return TRUE;
	}
	else
	{
		result = FreeLibrary(*hModule);
		*hModule = NULL;

		return result;
	}
}
