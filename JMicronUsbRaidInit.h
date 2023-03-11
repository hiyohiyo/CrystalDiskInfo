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

//-----------------------------------------------------------------------------
//
// Initialize
//
//-----------------------------------------------------------------------------

BOOL InitializeJMS56X(HMODULE* hModule)
{
#ifdef _M_X64
	*hModule = LoadLibraryW(DLL_DIR L"JMS56x64.dll");
#else
	*hModule = LoadLibraryW(DLL_DIR L"JMS56x32.dll");
#endif

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
#ifdef _M_X64
	* hModule = LoadLibraryW(DLL_DIR L"JMB39X64.dll");
#else
	* hModule = LoadLibraryW(DLL_DIR L"JMB39X32.dll");
#endif

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
