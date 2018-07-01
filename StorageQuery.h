#pragma once

namespace StorageQuery {
	typedef enum {
		StorageDeviceProperty = 0,
		StorageAdapterProperty,
		StorageDeviceIdProperty,
		StorageDeviceUniqueIdProperty,
		StorageDeviceWriteCacheProperty,
		StorageMiniportProperty,
		StorageAccessAlignmentProperty,
		StorageDeviceSeekPenaltyProperty,
		StorageDeviceTrimProperty,
		StorageDeviceWriteAggregationProperty,
		StorageDeviceDeviceTelemetryProperty,
		StorageDeviceLBProvisioningProperty,
		StorageDevicePowerProperty,
		StorageDeviceCopyOffloadProperty,
		StorageDeviceResiliencyProperty,
		StorageDeviceMediumProductType,
		StorageDeviceRpmbProperty,
		StorageDeviceIoCapabilityProperty = 48,
		StorageAdapterProtocolSpecificProperty,
		StorageDeviceProtocolSpecificProperty,
		StorageAdapterTemperatureProperty,
		StorageDeviceTemperatureProperty,
		StorageAdapterPhysicalTopologyProperty,
		StorageDevicePhysicalTopologyProperty,
		StorageDeviceAttributesProperty,
	} TStoragePropertyId;

	typedef enum {
		PropertyStandardQuery = 0,
		PropertyExistsQuery,
		PropertyMaskQuery,
		PropertyQueryMaxDefined
	} TStorageQueryType;

	typedef struct {
		TStoragePropertyId PropertyId;
		TStorageQueryType QueryType;
	} TStoragePropertyQuery;

	typedef enum {
		ProtocolTypeUnknown = 0x00,
		ProtocolTypeScsi,
		ProtocolTypeAta,
		ProtocolTypeNvme,
		ProtocolTypeSd,
		ProtocolTypeProprietary = 0x7E,
		ProtocolTypeMaxReserved = 0x7F
	} TStroageProtocolType;

	typedef struct {
		TStroageProtocolType ProtocolType;
		DWORD   DataType;
		DWORD   ProtocolDataRequestValue;
		DWORD   ProtocolDataRequestSubValue;
		DWORD   ProtocolDataOffset;
		DWORD   ProtocolDataLength;
		DWORD   FixedProtocolReturnData;
		DWORD   Reserved[3];
	} TStorageProtocolSpecificData;

	typedef enum {
		NVMeDataTypeUnknown = 0,
		NVMeDataTypeIdentify,
		NVMeDataTypeLogPage,
		NVMeDataTypeFeature,
	} TStorageProtocolNVMeDataType;

	typedef struct {
		TStoragePropertyQuery Query;
		TStorageProtocolSpecificData ProtocolSpecific;
		BYTE Buffer[4096];
	} TStorageQueryWithBuffer;
}
