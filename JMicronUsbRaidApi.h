#pragma once

/******************************************************************************
**
** DLL Information
**
******************************************************************************/

//-----------------------------------------------------------------------------
// GetDllStatus
//-----------------------------------------------------------------------------
/*DWORD // DLL Status
WINAPI GetDllStatus();
*/
//-----------------------------------------------------------------------------
// GetDllVersion
//-----------------------------------------------------------------------------
extern "C"
{
	DWORD // DLL Version
		WINAPI GetDllVersion(
			PBYTE major,		// major version
			PBYTE minor,		// minor version
			PBYTE revision,		// revision
			PBYTE release		// release/build
		);

	//-----------------------------------------------------------------------------
	// GetControllerCount
	//-----------------------------------------------------------------------------
	int // Controller Count
		WINAPI GetControllerCount();


	//-----------------------------------------------------------------------------
	// GetControllerInfoFx
	//-----------------------------------------------------------------------------
	/*
	DWORD // Controller Count
	WINAPI GetControllerInfoFx(int index, CONTROLLER* controller);
	*/

	BOOL
		WINAPI GetSmartInfoFx(int index, unsigned char port, UNION_SMART_ATTRIBUTE* attribute, UNION_SMART_THRESHOLD* threshold);

	BOOL
		WINAPI GetIdentifyInfoFx(int index, unsigned char port, UNION_IDENTIFY_DEVICE* identify);


}