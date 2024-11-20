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

BOOL InitializeJMS56X(HMODULE* hModule);
BOOL DeinitializeJMS56X(HMODULE* hModule);

BOOL InitializeJMB39X(HMODULE* hModule);
BOOL DeinitializeJMB39X(HMODULE* hModule);

BOOL InitializeJMS586_20(HMODULE* hModule);
BOOL DeinitializeJMS586_20(HMODULE* hModule);

BOOL InitializeJMS586_40(HMODULE* hModule);
BOOL DeinitializeJMS586_40(HMODULE* hModule);

//-----------------------------------------------------------------------------
//
// Funtions
//
//-----------------------------------------------------------------------------

_GetDllVersionJMS56X pGetDllVersionJMS56X = NULL;
_GetControllerCountJMS56X pGetControllerCountJMS56X = NULL;
_GetSmartInfoJMS56X pGetSmartInfoJMS56X = NULL;
_GetIdentifyInfoJMS56X pGetIdentifyInfoJMS56X = NULL;

_GetDllVersionJMB39X pGetDllVersionJMB39X = NULL;
_GetControllerCountJMB39X pGetControllerCountJMB39X = NULL;
_GetSmartInfoJMB39X pGetSmartInfoJMB39X = NULL;
_GetIdentifyInfoJMB39X pGetIdentifyInfoJMB39X = NULL;

_GetDllVersionJMS586_20 pGetDllVersionJMS586_20 = NULL;
_GetControllerCountJMS586_20 pGetControllerCountJMS586_20 = NULL;
_GetSmartInfoJMS586_20 pGetSmartInfoJMS586_20 = NULL;
_GetIdentifyInfoJMS586_20 pGetIdentifyInfoJMS586_20 = NULL;
_GetNVMePortInfoJMS586_20 pGetNVMePortInfoJMS586_20 = NULL;
_GetNVMeSmartInfoJMS586_20 pGetNVMeSmartInfoJMS586_20 = NULL;

_GetDllVersionJMS586_40 pGetDllVersionJMS586_40 = NULL;
_GetControllerCountJMS586_40 pGetControllerCountJMS586_40 = NULL;
_GetSmartInfoJMS586_40 pGetSmartInfoJMS586_40 = NULL;
_GetIdentifyInfoJMS586_40 pGetIdentifyInfoJMS586_40 = NULL;
_GetNVMePortInfoJMS586_40 pGetNVMePortInfoJMS586_40 = NULL;
_GetNVMeSmartInfoJMS586_40 pGetNVMeSmartInfoJMS586_40 = NULL;
_GetNVMeIdInfoJMS586_40 pGetNVMeIdInfoJMS586_40 = NULL;
_ControllerSerialNum2IdJMS586_40 pControllerSerialNum2IdJMS586_40 = NULL;

//-----------------------------------------------------------------------------
//
// Initialize
//
//-----------------------------------------------------------------------------

BOOL InitializeJMS56X(HMODULE* hModule)
{
	TCHAR fullPath[MAX_PATH] = {};
	TCHAR drive[MAX_PATH] = {};
	TCHAR path[MAX_PATH] = {};
	TCHAR dllPath[MAX_PATH] = {};

	GetModuleFileNameW(NULL, fullPath, MAX_PATH);
	_wsplitpath_s(fullPath, drive, MAX_PATH, path, MAX_PATH, NULL, 0, NULL, 0);
	wcscat_s(dllPath, MAX_PATH, drive);
	wcscat_s(dllPath, MAX_PATH, path);
	wcscat_s(dllPath, MAX_PATH, DLL_DIR);

#ifdef _M_ARM64
	wcscat_s(dllPath, MAX_PATH, L"JMS56xA64.dll");
#elif _M_X64
	wcscat_s(dllPath, MAX_PATH, L"JMS56x64.dll");
#else
	wcscat_s(dllPath, MAX_PATH, L"JMS56x86.dll");
#endif

	if (!CheckCodeSign(CERTNAME_JMS56X, dllPath)) { return FALSE; }
	* hModule = LoadLibraryW(dllPath);

	if (*hModule == NULL)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------------
	// GetProcAddress
	//-----------------------------------------------------------------------------
	pGetDllVersionJMS56X = (_GetDllVersionJMS56X)GetProcAddress(*hModule, "GetDllVersion");
	pGetControllerCountJMS56X = (_GetControllerCountJMS56X)GetProcAddress(*hModule, "GetControllerCount");
	pGetSmartInfoJMS56X = (_GetSmartInfoJMS56X)GetProcAddress(*hModule, "GetSmartInfoFx");
	pGetIdentifyInfoJMS56X = (_GetIdentifyInfoJMS56X)GetProcAddress(*hModule, "GetIdentifyInfoFx");

	//-----------------------------------------------------------------------------
	// Check Functions
	//-----------------------------------------------------------------------------
	if (!( pGetDllVersionJMS56X
		&& pGetControllerCountJMS56X
		&& pGetSmartInfoJMS56X
		&& pGetIdentifyInfoJMS56X
		))
	{
		FreeLibrary(*hModule);
		*hModule = NULL;
		return FALSE;
	}

	return TRUE;
}

BOOL InitializeJMB39X(HMODULE* hModule)
{
	TCHAR fullPath[MAX_PATH] = {};
	TCHAR drive[MAX_PATH] = {};
	TCHAR path[MAX_PATH] = {};
	TCHAR dllPath[MAX_PATH] = {};

	GetModuleFileNameW(NULL, fullPath, MAX_PATH);
	_wsplitpath_s(fullPath, drive, MAX_PATH, path, MAX_PATH, NULL, 0, NULL, 0);
	wcscat_s(dllPath, MAX_PATH, drive);
	wcscat_s(dllPath, MAX_PATH, path);
	wcscat_s(dllPath, MAX_PATH, DLL_DIR);

#ifdef _M_ARM64
	wcscat_s(dllPath, MAX_PATH, L"JMB39xA64.dll");
#elif _M_X64
	wcscat_s(dllPath, MAX_PATH, L"JMB39x64.dll");
#else
	wcscat_s(dllPath, MAX_PATH, L"JMB39x86.dll");
#endif

	if (!CheckCodeSign(CERTNAME_JMB39X, dllPath)) { return FALSE; }
	*hModule = LoadLibraryW(dllPath);

	if (*hModule == NULL)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------------
	// GetProcAddress
	//-----------------------------------------------------------------------------
	pGetDllVersionJMB39X = (_GetDllVersionJMB39X)GetProcAddress(*hModule, "GetDllVersion");
	pGetControllerCountJMB39X = (_GetControllerCountJMB39X)GetProcAddress(*hModule, "GetControllerCount");
	pGetSmartInfoJMB39X = (_GetSmartInfoJMB39X)GetProcAddress(*hModule, "GetSmartInfoFx");
	pGetIdentifyInfoJMB39X = (_GetIdentifyInfoJMB39X)GetProcAddress(*hModule, "GetIdentifyInfoFx");

	//-----------------------------------------------------------------------------
	// Check Functions
	//-----------------------------------------------------------------------------
	if (!( pGetDllVersionJMB39X
		&& pGetControllerCountJMB39X
		&& pGetSmartInfoJMB39X
		&& pGetIdentifyInfoJMB39X
		))
	{
		FreeLibrary(*hModule);
		*hModule = NULL;
		return FALSE;
	}

	return TRUE;
}

BOOL InitializeJMS586_20(HMODULE* hModule)
{
	TCHAR fullPath[MAX_PATH] = {};
	TCHAR drive[MAX_PATH] = {};
	TCHAR path[MAX_PATH] = {};
	TCHAR dllPath[MAX_PATH] = {};

	GetModuleFileNameW(NULL, fullPath, MAX_PATH);
	_wsplitpath_s(fullPath, drive, MAX_PATH, path, MAX_PATH, NULL, 0, NULL, 0);
	wcscat_s(dllPath, MAX_PATH, drive);
	wcscat_s(dllPath, MAX_PATH, path);
	wcscat_s(dllPath, MAX_PATH, DLL_DIR);

#ifdef _M_ARM64
	wcscat_s(dllPath, MAX_PATH, L"JMS586A64.dll");
#elif _M_X64
	wcscat_s(dllPath, MAX_PATH, L"JMS586x64.dll");
#else
	wcscat_s(dllPath, MAX_PATH, L"JMS586x86.dll");
#endif

	if (!CheckCodeSign(CERTNAME_JMS586_20, dllPath)) { return FALSE; }
	*hModule = LoadLibraryW(dllPath);

	if (*hModule == NULL)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------------
	// GetProcAddress
	//-----------------------------------------------------------------------------
	pGetDllVersionJMS586_20 = (_GetDllVersionJMS586_20)GetProcAddress(*hModule, "GetDllVersion");
	pGetControllerCountJMS586_20 = (_GetControllerCountJMS586_20)GetProcAddress(*hModule, "GetControllerCount");
	pGetSmartInfoJMS586_20 = (_GetSmartInfoJMS586_20)GetProcAddress(*hModule, "GetSmartInfoFx");
	pGetIdentifyInfoJMS586_20 = (_GetIdentifyInfoJMS586_20)GetProcAddress(*hModule, "GetIdentifyInfoFx");
	pGetNVMePortInfoJMS586_20 = (_GetNVMePortInfoJMS586_20)GetProcAddress(*hModule, "GetNVMePortInfoFx");
	pGetNVMeSmartInfoJMS586_20 = (_GetNVMeSmartInfoJMS586_20)GetProcAddress(*hModule, "GetNVMeSmartInfoFx");

	//-----------------------------------------------------------------------------
	// Check Functions
	//-----------------------------------------------------------------------------
	if (!(pGetDllVersionJMS586_20
		&& pGetControllerCountJMS586_20
		&& pGetSmartInfoJMS586_20
		&& pGetIdentifyInfoJMS586_20
		&& pGetNVMePortInfoJMS586_20
		&& pGetNVMeSmartInfoJMS586_20
		))
	{
		FreeLibrary(*hModule);
		*hModule = NULL;
		return FALSE;
	}

	return TRUE;
}

BOOL InitializeJMS586_40(HMODULE* hModule)
{
	TCHAR fullPath[MAX_PATH] = {};
	TCHAR drive[MAX_PATH] = {};
	TCHAR path[MAX_PATH] = {};
	TCHAR dllPath[MAX_PATH] = {};

	GetModuleFileNameW(NULL, fullPath, MAX_PATH);
	_wsplitpath_s(fullPath, drive, MAX_PATH, path, MAX_PATH, NULL, 0, NULL, 0);
	wcscat_s(dllPath, MAX_PATH, drive);
	wcscat_s(dllPath, MAX_PATH, path);
	wcscat_s(dllPath, MAX_PATH, DLL_DIR);

#ifdef _M_ARM64
	wcscat_s(dllPath, MAX_PATH, L"JMS586_40A64.dll");
#elif _M_X64
	wcscat_s(dllPath, MAX_PATH, L"JMS586_40x64.dll");
#else
	wcscat_s(dllPath, MAX_PATH, L"JMS586_40x86.dll");
#endif

	if (!CheckCodeSign(CERTNAME_JMS586_40, dllPath)) { return FALSE; }
	*hModule = LoadLibraryW(dllPath);

	if (*hModule == NULL)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------------
	// GetProcAddress
	//-----------------------------------------------------------------------------
	pGetDllVersionJMS586_40 = (_GetDllVersionJMS586_40)GetProcAddress(*hModule, "GetDllVersion");
	pGetControllerCountJMS586_40 = (_GetControllerCountJMS586_40)GetProcAddress(*hModule, "GetControllerCount");
	pGetSmartInfoJMS586_40 = (_GetSmartInfoJMS586_40)GetProcAddress(*hModule, "GetSmartInfoFx");
	pGetIdentifyInfoJMS586_40 = (_GetIdentifyInfoJMS586_40)GetProcAddress(*hModule, "GetIdentifyInfoFx");
	pGetNVMePortInfoJMS586_40 = (_GetNVMePortInfoJMS586_40)GetProcAddress(*hModule, "GetNVMePortInfoFx");
	pGetNVMeSmartInfoJMS586_40 = (_GetNVMeSmartInfoJMS586_40)GetProcAddress(*hModule, "GetNVMeSmartInfoFx");
	pGetNVMeIdInfoJMS586_40 = (_GetNVMeIdInfoJMS586_40)GetProcAddress(*hModule, "GetNVMeIdInfoFx");
	pControllerSerialNum2IdJMS586_40 = (_ControllerSerialNum2IdJMS586_40)GetProcAddress(*hModule, "ControllerSerialNum2IdFx");

	//-----------------------------------------------------------------------------
	// Check Functions
	//-----------------------------------------------------------------------------
	if (!(pGetDllVersionJMS586_40
		&& pGetControllerCountJMS586_40
		&& pGetSmartInfoJMS586_40
		&& pGetIdentifyInfoJMS586_40
		&& pGetNVMePortInfoJMS586_40
		&& pGetNVMeSmartInfoJMS586_40
		&& pGetNVMeIdInfoJMS586_40
		&& pControllerSerialNum2IdJMS586_40
		))
	{
		FreeLibrary(*hModule);
		*hModule = NULL;
		return FALSE;
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
//
// Deinitialize
//
//-----------------------------------------------------------------------------

BOOL DeinitializeJMS56X(HMODULE* hModule)
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

BOOL DeinitializeJMB39X(HMODULE* hModule)
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

BOOL DeinitializeJMS586_20(HMODULE* hModule)
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

BOOL DeinitializeJMS586_40(HMODULE* hModule)
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