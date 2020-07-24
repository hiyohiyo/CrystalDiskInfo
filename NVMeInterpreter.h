/*---------------------------------------------------------------------------*/
//       Author : Minkyu Kim
//          Web : http://naraeon.net/
//                https://github.com/ebangin127/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once
#include <windows.h>

static const int MAX_ATTRIBUTE = 30;
static const int NVME_ATTRIBUTE = 15;

#pragma pack(push,1)
typedef	struct _SMART_ATTRIBUTE
{
	BYTE	Id;
	WORD	StatusFlags;
	BYTE	CurrentValue;
	BYTE	WorstValue;
	BYTE	RawValue[6];
	BYTE	Reserved;
} SMART_ATTRIBUTE;
#pragma	pack(pop)

typedef SMART_ATTRIBUTE SMART_ATTRIBUTE_LIST[MAX_ATTRIBUTE];

void NVMeSmartToATASmart(UCHAR* NVMeSmartBuf, void* ATASmartBufUncasted);