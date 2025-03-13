/*---------------------------------------------------------------------------*/
//       Author : Minkyu Kim
//          Web : http://naraeon.net/
//                https://github.com/ebangin127/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#include "StdAfx.h"
#include "nvmeinterpreter.h"

void AddToATASmartBuf(SMART_ATTRIBUTE_LIST* ATASmartBuf, INT IdxInBuf, SMART_ATTRIBUTE AttrToAdd)
{
	(*ATASmartBuf)[IdxInBuf] = AttrToAdd;
}

SMART_ATTRIBUTE SeperateCriticalWarningFrom(UCHAR* NVMeSmartBuf)
{
	SMART_ATTRIBUTE attr = {};
	attr.Id = 1;
	attr.RawValue[0] = NVMeSmartBuf[0];
	return attr;
}

SMART_ATTRIBUTE SeperateTemperatureFrom(UCHAR* NVMeSmartBuf)
{
	const int TemperatureStart = 1;
	SMART_ATTRIBUTE attr = {};
	attr.Id = 2;
	attr.RawValue[0] = NVMeSmartBuf[TemperatureStart];
	attr.RawValue[1] = NVMeSmartBuf[TemperatureStart + 1];
	return attr;
}

SMART_ATTRIBUTE SeperateAvailableSpareFrom(UCHAR* NVMeSmartBuf)
{
	const int AvailableSpareStart = 3;
	SMART_ATTRIBUTE attr = {};
	attr.Id = 3;
	attr.RawValue[0] = NVMeSmartBuf[AvailableSpareStart];
	return attr;
}

SMART_ATTRIBUTE SeperateAvailableSpareThresholdFrom(UCHAR* NVMeSmartBuf)
{
	const int AvailableSpareThresholdStart = 4;
	SMART_ATTRIBUTE attr = {};
	attr.Id = 4;
	attr.RawValue[0] = NVMeSmartBuf[AvailableSpareThresholdStart];
	return attr;
}

SMART_ATTRIBUTE SeperatePercentageUsedFrom(UCHAR* NVMeSmartBuf)
{
	const int PercentageUsedStart = 5;
	SMART_ATTRIBUTE attr = {};
	attr.Id = 5;
	attr.RawValue[0] = NVMeSmartBuf[PercentageUsedStart];
	return attr;
}

SMART_ATTRIBUTE SeperateDataUnitsReadFrom(UCHAR* NVMeSmartBuf)
{
	const int DataUnitsReadStart = 32;
	SMART_ATTRIBUTE attr = {};
	attr.Id = 6;
	memcpy(attr.RawValue, &NVMeSmartBuf[DataUnitsReadStart], sizeof(attr.RawValue));
	return attr;
}

SMART_ATTRIBUTE SeperateDataUnitsWrittenFrom(UCHAR* NVMeSmartBuf)
{
	const int DataUnitsWrittenStart = 48;
	SMART_ATTRIBUTE attr = {};
	attr.Id = 7;
	memcpy(attr.RawValue, &NVMeSmartBuf[DataUnitsWrittenStart], sizeof(attr.RawValue));
	return attr;
}

SMART_ATTRIBUTE SeperateHostReadCommandsFrom(UCHAR* NVMeSmartBuf)
{
	const int ReadStart = 64;
	SMART_ATTRIBUTE attr = {};
	attr.Id = 8;
	memcpy(attr.RawValue, &NVMeSmartBuf[ReadStart], sizeof(attr.RawValue));
	return attr;
}

SMART_ATTRIBUTE SeperateHostWriteCommandsFrom(UCHAR* NVMeSmartBuf)
{
	const int WriteStart = 80;
	SMART_ATTRIBUTE attr = {};
	attr.Id = 9;
	memcpy(attr.RawValue, &NVMeSmartBuf[WriteStart], sizeof(attr.RawValue));
	return attr;
}

SMART_ATTRIBUTE SeperateControllerBusyTimeFrom(UCHAR* NVMeSmartBuf)
{
	const int BusyTimeStart = 96;
	SMART_ATTRIBUTE attr = {};
	attr.Id = 10;
	memcpy(attr.RawValue, &NVMeSmartBuf[BusyTimeStart], sizeof(attr.RawValue));
	return attr;
}

SMART_ATTRIBUTE SeperatePowerCyclesFrom(UCHAR* NVMeSmartBuf)
{
	const int PowerCycleStart = 112;
	SMART_ATTRIBUTE attr = {};
	attr.Id = 11;
	memcpy(attr.RawValue, &NVMeSmartBuf[PowerCycleStart], sizeof(attr.RawValue));
	return attr;
}

SMART_ATTRIBUTE SeperatePowerOnHoursFrom(UCHAR* NVMeSmartBuf)
{
	const int PowerOnHoursStart = 128;
	SMART_ATTRIBUTE attr = {};
	attr.Id = 12;
	memcpy(attr.RawValue, &NVMeSmartBuf[PowerOnHoursStart], sizeof(attr.RawValue));
	return attr;
}

SMART_ATTRIBUTE SeperateUnsafeShutdownsFrom(UCHAR* NVMeSmartBuf)
{
	const int UnsafeShutdownsStart = 144;
	SMART_ATTRIBUTE attr = {};
	attr.Id = 13;
	memcpy(attr.RawValue, &NVMeSmartBuf[UnsafeShutdownsStart], sizeof(attr.RawValue));
	return attr;
}

SMART_ATTRIBUTE SeperateMediaErrorsFrom(UCHAR* NVMeSmartBuf)
{
	const int MediaErrorsStart = 160;
	SMART_ATTRIBUTE attr = {};
	attr.Id = 14;
	memcpy(attr.RawValue, &NVMeSmartBuf[MediaErrorsStart], sizeof(attr.RawValue));
	return attr;
}

SMART_ATTRIBUTE SeperateNumberOfErrorsFrom(UCHAR* NVMeSmartBuf)
{
	const int NumberOfErrorsStart = 176;
	SMART_ATTRIBUTE attr = {};
	attr.Id = 15;
	memcpy(attr.RawValue, &NVMeSmartBuf[NumberOfErrorsStart], sizeof(attr.RawValue));
	return attr;
}

SMART_ATTRIBUTE SeperateWarningCompositeTemperatureTime(UCHAR* NVMeSmartBuf)
{
	const int TemperatureTimeStart = 192;
	SMART_ATTRIBUTE attr = {};
	attr.Id = 16;
	memcpy(attr.RawValue, &NVMeSmartBuf[TemperatureTimeStart], 4);
	return attr;
}

SMART_ATTRIBUTE SeperateCriticalCompositeTemperatureTime(UCHAR* NVMeSmartBuf)
{
	const int TemperatureTimeStart = 196;
	SMART_ATTRIBUTE attr = {};
	attr.Id = 17;
	memcpy(attr.RawValue, &NVMeSmartBuf[TemperatureTimeStart], 4);
	return attr;
}

void NVMeSmartToATASmart(UCHAR* NVMeSmartBuf, void* ATASmartBufUncasted)
{
	SMART_ATTRIBUTE_LIST* ATASmartBuf = (SMART_ATTRIBUTE_LIST*) ATASmartBufUncasted;
	int IdxInBuf = 0;
	AddToATASmartBuf(ATASmartBuf, IdxInBuf++, SeperateCriticalWarningFrom(NVMeSmartBuf));
	AddToATASmartBuf(ATASmartBuf, IdxInBuf++, SeperateTemperatureFrom(NVMeSmartBuf));
	AddToATASmartBuf(ATASmartBuf, IdxInBuf++, SeperateAvailableSpareFrom(NVMeSmartBuf));
	AddToATASmartBuf(ATASmartBuf, IdxInBuf++, SeperateAvailableSpareThresholdFrom(NVMeSmartBuf));
	AddToATASmartBuf(ATASmartBuf, IdxInBuf++, SeperatePercentageUsedFrom(NVMeSmartBuf));
	AddToATASmartBuf(ATASmartBuf, IdxInBuf++, SeperateDataUnitsReadFrom(NVMeSmartBuf));
	AddToATASmartBuf(ATASmartBuf, IdxInBuf++, SeperateDataUnitsWrittenFrom(NVMeSmartBuf));
	AddToATASmartBuf(ATASmartBuf, IdxInBuf++, SeperateHostReadCommandsFrom(NVMeSmartBuf));
	AddToATASmartBuf(ATASmartBuf, IdxInBuf++, SeperateHostWriteCommandsFrom(NVMeSmartBuf));
	AddToATASmartBuf(ATASmartBuf, IdxInBuf++, SeperateControllerBusyTimeFrom(NVMeSmartBuf));
	AddToATASmartBuf(ATASmartBuf, IdxInBuf++, SeperatePowerCyclesFrom(NVMeSmartBuf));
	AddToATASmartBuf(ATASmartBuf, IdxInBuf++, SeperatePowerOnHoursFrom(NVMeSmartBuf));
	AddToATASmartBuf(ATASmartBuf, IdxInBuf++, SeperateUnsafeShutdownsFrom(NVMeSmartBuf));
	AddToATASmartBuf(ATASmartBuf, IdxInBuf++, SeperateMediaErrorsFrom(NVMeSmartBuf));
	AddToATASmartBuf(ATASmartBuf, IdxInBuf++, SeperateNumberOfErrorsFrom(NVMeSmartBuf));
	NVMeTemperatureSensorSmartToATASmart(NVMeSmartBuf, ATASmartBuf);
}

void NVMeCompositeTemperatureSmartToATASmart(UCHAR* NVMeSmartBuf, void* ATASmartBufUncasted) {
	SMART_ATTRIBUTE_LIST* ATASmartBuf = (SMART_ATTRIBUTE_LIST*)ATASmartBufUncasted;
	int IdxInBuf = 15;
	AddToATASmartBuf(ATASmartBuf, IdxInBuf++, SeperateWarningCompositeTemperatureTime(NVMeSmartBuf));
	AddToATASmartBuf(ATASmartBuf, IdxInBuf++, SeperateCriticalCompositeTemperatureTime(NVMeSmartBuf));
}

void NVMeTemperatureSensorSmartToATASmart(UCHAR* NVMeSmartBuf, void* ATASmartBufUncasted) {
	SMART_ATTRIBUTE_LIST* ATASmartBuf = (SMART_ATTRIBUTE_LIST*)ATASmartBufUncasted;
	int IdxInBuf = 17;
	const int TemperatureSensorStart = 200;
	const int MaxSensors = 8;

	for (int i = 0; i < MaxSensors; i++) {
		SMART_ATTRIBUTE attr = {};
		attr.Id = ++IdxInBuf;
		memcpy(attr.RawValue, &NVMeSmartBuf[TemperatureSensorStart + i*2], 2);
		if (attr.RawValue[0] || attr.RawValue[1]) {
			AddToATASmartBuf(ATASmartBuf, IdxInBuf, attr);
		}
	}
}

void NVMeThermalManagementTemperatureSmartToATASmart(UCHAR* NVMeSmartBuf, void* ATASmartBufUncasted) {
	SMART_ATTRIBUTE_LIST* ATASmartBuf = (SMART_ATTRIBUTE_LIST*)ATASmartBufUncasted;
	int IdxInBuf = 25;
	const int TemperatureSensorStart = 216;
	const int MaxEntries = 4;

	for (int i = 0; i < MaxEntries; i++) {
		SMART_ATTRIBUTE attr = {};
		attr.Id = ++IdxInBuf;
		memcpy(attr.RawValue, &NVMeSmartBuf[TemperatureSensorStart + i*4], 4);
		AddToATASmartBuf(ATASmartBuf, IdxInBuf, attr);
	}
}