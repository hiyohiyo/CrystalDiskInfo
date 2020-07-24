/*---------------------------------------------------------------------------*/
//       Author : Minkyu Kim
//          Web : http://naraeon.net/
//                https://github.com/ebangin127/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once
#include <initguid.h>
#include <devpkey.h>

typedef enum _PCIeSpecification  {
	PCIe1d0 = 1,
	PCIe2d0 = 2,
	PCIe3d0 = 3,
	PCIe4d0 = 4,
	PCIe5d0 = 5
} PCIeSpecification;

typedef enum _PCIeDataWidth {
	PCIex1 = 1,
	PCIex2 = 2,
	PCIex4 = 4,
	PCIex8 = 8,
	PCIex12 = 12,
	PCIex16 = 16,
	PCIex32 = 32
} PCIeDataWidth;

typedef struct _SlotSpeed  {
	PCIeSpecification SpecVersion;
	PCIeDataWidth LinkWidth;
} SlotSpeed;

typedef struct _SlotMaxCurrSpeed {
	SlotSpeed Maximum;
	SlotSpeed Current;
} SlotMaxCurrSpeed;

typedef struct _OSSlotSpeed {
	BYTE SpecVersion;
	BYTE LinkWidth;
} OSSlotSpeed;

typedef struct _OSSlotMaxCurrSpeed {
	OSSlotSpeed Maximum;
	OSSlotSpeed Current;
} OSSlotMaxCurrSpeed;

#define DEFINE_PCI_DEVICE_DEVPKEY(_DevPkeyName, _Pid) \
   DEFINE_DEVPROPKEY((_DevPkeyName), 0x3ab22e31, 0x8264, 0x4b4e, 0x9a, 0xf5, 0xa8, 0xd2, 0xd8, 0xe3, 0x3e, 0x62, (_Pid))

DEFINE_PCI_DEVICE_DEVPKEY(DEVPKEY_PciDevice_CurrentLinkSpeed, 9);
DEFINE_PCI_DEVICE_DEVPKEY(DEVPKEY_PciDevice_CurrentLinkWidth, 10);
DEFINE_PCI_DEVICE_DEVPKEY(DEVPKEY_PciDevice_MaxLinkSpeed, 11);
DEFINE_PCI_DEVICE_DEVPKEY(DEVPKEY_PciDevice_MaxLinkWidth, 12);

SlotMaxCurrSpeed GetPCIeSlotSpeed(const INT physicalDriveId, const BOOL IsKernelVerEqualOrOver6);
CString SlotSpeedToString(SlotSpeed speedtoconv);