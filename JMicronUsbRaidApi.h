#pragma once

extern "C"
{
	int  WINAPI GetControllerCount();
	BOOL WINAPI GetSmartInfoFx(int index, unsigned char port, UNION_SMART_ATTRIBUTE* attribute, UNION_SMART_THRESHOLD* threshold);
	BOOL WINAPI GetIdentifyInfoFx(int index, unsigned char port, UNION_IDENTIFY_DEVICE* identify);
}