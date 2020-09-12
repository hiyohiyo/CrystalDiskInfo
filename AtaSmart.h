/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/
// Reference : http://www.usefullcode.net/2007/02/hddsmart.html (ja)

#pragma once

#include "winioctl.h"
#include "SPTIUtil.h"
#include "SlotSpeedGetter.h"
#include "NVMeInterpreter.h"
#include "StorageQuery.h"

static const TCHAR* commandTypeString[] =
{
	_T("un"),
	_T("pd"),
	_T("sm"),
	_T("si"),
	_T("sa"),
	_T("sp"),
	_T("io"),
	_T("lo"),
	_T("pr"),
	_T("jm"),
	_T("cy"),
	_T("pr"),
	_T("cs"),
	_T("cp"),
	_T("wm"),
	_T("ns"), // NVMe Samsung
	_T("ni"), // NVMe Intel
	_T("sq"), // NVMe Storage Query
	_T("nj"), // NVMe JMicron
	_T("na"), // NVMe ASMedia
	_T("nr"), // NVMe Realtek
	_T("nt"), // NVMe Intel RST
	_T("mr"), // MegaRAID SAS
};

static const TCHAR* ssdVendorString[] =
{
	_T(""),
	_T(""),
	_T("mt"), // MTron
	_T("ix"), // Indilinx
	_T("jm"), // JMicron
	_T("il"), // Intel
	_T("sg"), // SAMSUNG
	_T("sf"), // SandForce
	_T("mi"), // Micron
	_T("oz"), // OCZ
	_T("st"), // SEAGATE
	_T("wd"), // WDC
	_T("px"), // PLEXTOR
	_T("sd"), // SanDisk
	_T("oz"), // OCZ Vector
	_T("to"), // TOSHIABA
	_T("co"), // Corsair
	_T("ki"), // Kingston
	_T("m2"), // Micron MU02
	_T("nv"), // NVMe
	_T("re"), // Realtek
	_T("sk"), // SKhynix
	_T("ki"), // KIOXIA
	_T("ss"), // SSSTC
	_T("id"), // Intel DC
	_T("ap"), // Apacer
	_T("sm"), // SiliconMotion
	_T("ph"), // Phison
	_T("ma"), // Marvell
	_T("mk"), // Maxiotek
};

static const TCHAR* attributeString[] =
{
	_T("Smart"),
	_T("SmartSsd"),
	_T("SmartMtron"),
	_T("SmartIndlinx"),
	_T("SmartJMicron"),
	_T("SmartIntel"),
	_T("SmartSamsung"),
	_T("SmartSandForce"),
	_T("SmartMicron"),
	_T("SmartOcz"),
	_T("SmartSeagate"),
	_T("SmartWdc"),
	_T("SmartPlextor"),
	_T("SmartSanDisk"),
	_T("SmartOczVector"),
	_T("SmartToshiba"),
	_T("SmartCorsair"),
	_T("SmartKingston"),
	_T("SmartMicronMU02"),
	_T("SmartNVMe"),
	_T("SmartRealtek"),
	_T("SmartSKhynix"),
	_T("SmartKioxia"),
	_T("SmartSsstc"),
	_T("SmartIntelDc"),
	_T("SmartApacer"),
	_T("SmartSiliconMotion"),
	_T("SmartPhison"),
	_T("SmartMarvell"),
	_T("SmartMaxiotek"),
};

static const TCHAR* deviceFormFactorString[] =
{
	_T(""),
	_T("5.25 inch"),
	_T("3.5 inch"),
	_T("2.5 inch"),
	_T("1.8 inch"),
	_T("< 1.8 inch")
};

typedef struct _VOLUME_DISK_EXTENTS_LX {
    DWORD       NumberOfDiskExtents;
    DISK_EXTENT Extents[4];
} VOLUME_DISK_EXTENTS_LX, *PVOLUME_DISK_EXTENTS_LX;

class CAtaSmart
{

public:
	static const int MAX_DISK = 64;// FIX
	static const int MAX_ATTRIBUTE = 30; // FIX
	static const int MAX_SEARCH_PHYSICAL_DRIVE = 56;
	static const int MAX_SEARCH_SCSI_PORT = 16;
	static const int MAX_SEARCH_SCSI_TARGET_ID = 8;

	static const int SCSI_MINIPORT_BUFFER_SIZE = 512;

public:
	CAtaSmart();
	virtual ~CAtaSmart();


	enum VENDOR_ID
	{
		HDD_GENERAL = 0,
		SSD_GENERAL = 1,
		SSD_VENDOR_MTRON = 2,
		SSD_VENDOR_INDILINX = 3,
		SSD_VENDOR_JMICRON = 4,
		SSD_VENDOR_INTEL = 5,
		SSD_VENDOR_SAMSUNG = 6,
		SSD_VENDOR_SANDFORCE = 7,
		SSD_VENDOR_MICRON = 8,
		SSD_VENDOR_OCZ = 9,
		SSD_VENDOR_SEAGATE = 10,
		SSD_VENDOR_WDC = 11,
		SSD_VENDOR_PLEXTOR = 12,
		SSD_VENDOR_SANDISK = 13,
		SSD_VENDOR_OCZ_VECTOR = 14,
		SSD_VENDOR_TOSHIBA = 15,
		SSD_VENDOR_CORSAIR = 16,
		SSD_VENDOR_KINGSTON = 17,
		SSD_VENDOR_MICRON_MU02 = 18,
		SSD_VENDOR_NVME = 19,
		SSD_VENDOR_REALTEK = 20,
		SSD_VENDOR_SKHYNIX = 21,
		SSD_VENDOR_KIOXIA = 22,
		SSD_VENDOR_SSSTC = 23,
		SSD_VENDOR_INTEL_DC = 24,
		SSD_VENDOR_APACER = 25,
		SSD_VENDOR_SILICONMOTION = 26,
		SSD_VENDOR_PHISON = 27,
		SSD_VENDOR_MARVELL = 28,
		SSD_VENDOR_MAXIOTEK = 29,
		SSD_VENDOR_MAX = 99,

		VENDOR_UNKNOWN = 0x0000,
		USB_VENDOR_BUFFALO = 0x0411,
		USB_VENDOR_IO_DATA = 0x04BB,
		USB_VENDOR_LOGITEC = 0x0789,
		USB_VENDOR_INITIO = 0x13FD,
		USB_VENDOR_SUNPLUS = 0x04FC,
		USB_VENDOR_JMICRON = 0x152D,
		USB_VENDOR_CYPRESS = 0x04B4,
		USB_VENDOR_OXFORD = 0x0928,
		USB_VENDOR_PROLIFIC = 0x067B,
		USB_VENDOR_REALTEK = 0x0BDA,
		USB_VENDOR_ALL = 0xFFFF,
	};

	enum SMART_STATUS
	{
		SMART_STATUS_NO_CHANGE = 0,
		SMART_STATUS_MINOR_CHANGE,
		SMART_STATUS_MAJOR_CHANGE
	};

	enum TRANSFER_MODE
	{
		TRANSFER_MODE_UNKNOWN = 0,
		TRANSFER_MODE_PIO,
		TRANSFER_MODE_PIO_DMA,
		TRANSFER_MODE_ULTRA_DMA_16,
		TRANSFER_MODE_ULTRA_DMA_25,
		TRANSFER_MODE_ULTRA_DMA_33,
		TRANSFER_MODE_ULTRA_DMA_44,
		TRANSFER_MODE_ULTRA_DMA_66,
		TRANSFER_MODE_ULTRA_DMA_100,
		TRANSFER_MODE_ULTRA_DMA_133,
		TRANSFER_MODE_SATA_150,
		TRANSFER_MODE_SATA_300,
		TRANSFER_MODE_SATA_600
	};

	enum DISK_STATUS
	{
		DISK_STATUS_UNKNOWN = 0,
		DISK_STATUS_GOOD,
		DISK_STATUS_CAUTION,
		DISK_STATUS_BAD
	};

	enum POWER_ON_HOURS_UNIT
	{
		POWER_ON_UNKNOWN = 0,
		POWER_ON_HOURS,
		POWER_ON_MINUTES,
		POWER_ON_HALF_MINUTES,
		POWER_ON_SECONDS,
		POWER_ON_10_MINUTES,
		POWER_ON_MILLI_SECONDS,
	};

	enum HOST_READS_WRITES_UNIT
	{
		HOST_READS_WRITES_UNKNOWN = 0,
		HOST_READS_WRITES_512B,
		HOST_READS_WRITES_1MB,
		HOST_READS_WRITES_32MB,
		HOST_READS_WRITES_GB,
	};

	enum COMMAND_TYPE
	{
		CMD_TYPE_UNKNOWN = 0,
		CMD_TYPE_PHYSICAL_DRIVE,
		CMD_TYPE_SCSI_MINIPORT,
		CMD_TYPE_SILICON_IMAGE,
		CMD_TYPE_SAT,			// SAT = SCSI_ATA_TRANSLATION
		CMD_TYPE_SUNPLUS,
		CMD_TYPE_IO_DATA,
		CMD_TYPE_LOGITEC,
		CMD_TYPE_PROLIFIC,
		CMD_TYPE_JMICRON,
		CMD_TYPE_CYPRESS,
		CMD_TYPE_SAT_ASM1352R,	// AMS1352 2nd drive
		CMD_TYPE_CSMI,				// CSMI = Common Storage Management Interface
		CMD_TYPE_CSMI_PHYSICAL_DRIVE, // CSMI = Common Storage Management Interface 
		CMD_TYPE_WMI,
		CMD_TYPE_NVME_SAMSUNG,
		CMD_TYPE_NVME_INTEL,
		CMD_TYPE_NVME_STORAGE_QUERY,
		CMD_TYPE_NVME_JMICRON,
		CMD_TYPE_NVME_ASMEDIA,
		CMD_TYPE_NVME_REALTEK,
		CMD_TYPE_NVME_INTEL_RST,
		CMD_TYPE_MEGARAID,
		CMD_TYPE_DEBUG
	};

	enum CSMI_TYPE
	{
		CSMI_TYPE_DISABLE = 0,
		CSMI_TYPE_ENABLE_AUTO,
		CSMI_TYPE_ENABLE_RAID,
		CSMI_TYPE_ENABLE_ALL,
	};

	enum SMART_WMI_TYPE
	{
		WMI_SMART_DATA = 0,
		WMI_SMART_THRESHOLD
	};

	enum INTERFACE_TYPE
	{
		INTERFACE_TYPE_UNKNOWN = 0,
		INTERFACE_TYPE_PATA,
		INTERFACE_TYPE_SATA,
		INTERFACE_TYPE_USB,
		INTERFACE_TYPE_IEEE1394,
	//	INTERFACE_TYPE_UASP,
		INTERFACE_TYPE_SCSI, 
		INTERFACE_TYPE_NVME,
	//	INTERFACE_TYPE_USB_NVME,
	};

protected:
	enum IO_CONTROL_CODE
	{
		DFP_SEND_DRIVE_COMMAND	= 0x0007C084,
		DFP_RECEIVE_DRIVE_DATA	= 0x0007C088,
		IOCTL_SCSI_MINIPORT     = 0x0004D008,
		IOCTL_IDE_PASS_THROUGH  = 0x0004D028, // 2000 or later
		IOCTL_ATA_PASS_THROUGH  = 0x0004D02C, // XP SP2 and 2003 or later
	};

#pragma pack(push,1)

	typedef	struct _IDENTIFY_DEVICE_OUTDATA
	{
		SENDCMDOUTPARAMS	SendCmdOutParam;
		BYTE				Data[IDENTIFY_BUFFER_SIZE - 1];
	} IDENTIFY_DEVICE_OUTDATA, *PIDENTIFY_DEVICE_OUTDATA;

	typedef	struct _SMART_READ_DATA_OUTDATA
	{
		SENDCMDOUTPARAMS	SendCmdOutParam;
		BYTE				Data[READ_ATTRIBUTE_BUFFER_SIZE - 1];
	} SMART_READ_DATA_OUTDATA, *PSMART_READ_DATA_OUTDATA;

	typedef struct _CMD_IDE_PATH_THROUGH
	{
		IDEREGS	reg;
		DWORD   length;
		BYTE    buffer[1];
	} CMD_IDE_PATH_THROUGH, *PCMD_IDE_PATH_THROUGH;

	static const int ATA_FLAGS_DRDY_REQUIRED = 0x01;
	static const int ATA_FLAGS_DATA_IN       = 0x02;
	static const int ATA_FLAGS_DATA_OUT      = 0x04;
	static const int ATA_FLAGS_48BIT_COMMAND = 0x08;

	typedef struct _ATA_PASS_THROUGH_EX
	{
		WORD    Length;
		WORD    AtaFlags;
		BYTE    PathId;
		BYTE    TargetId;
		BYTE    Lun;
		BYTE    ReservedAsUchar;
		DWORD   DataTransferLength;
		DWORD   TimeOutValue;
		DWORD   ReservedAsUlong;
	//	DWORD   DataBufferOffset;
#ifdef _WIN64
		DWORD	padding;
#endif
		DWORD_PTR   DataBufferOffset;
		IDEREGS PreviousTaskFile;
		IDEREGS CurrentTaskFile;
	} ATA_PASS_THROUGH_EX, *PCMD_ATA_PASS_THROUGH_EX;

	typedef struct
	{
		ATA_PASS_THROUGH_EX Apt;
		DWORD Filer;
		BYTE  Buf[512];
	} ATA_PASS_THROUGH_EX_WITH_BUFFERS;

	typedef	struct _SMART_THRESHOLD
	{
		BYTE	Id;
		BYTE	ThresholdValue;
		BYTE	Reserved[10];
	} SMART_THRESHOLD;

	typedef struct _SRB_IO_CONTROL
	{
	   ULONG	HeaderLength;
	   UCHAR	Signature[8];
	   ULONG	Timeout;
	   ULONG	ControlCode;
	   ULONG	ReturnCode;
	   ULONG	Length;
	} SRB_IO_CONTROL;

	typedef struct _SRB_IO_COMMAND
	{
		SRB_IO_CONTROL	Cntrol;
		IDEREGS			IdeRegs;
		BYTE			Data[512];
	} SRB_IO_COMMAND;

	typedef struct {
		SRB_IO_CONTROL sic ;
		USHORT port ;
		USHORT maybe_always1 ;
		ULONG unknown[5] ;
		//IDENTIFY_DEVICE id_data ;
		WORD id_data[256] ;
	} SilIdentDev ;

	struct BIN_IDENTIFY_DEVICE
	{
		BYTE		Bin[4096];
	};

	struct NVME_IDENTIFY_DEVICE
	{
		CHAR		Reserved1[4];
		CHAR		SerialNumber[20];
		CHAR		Model[40];
		CHAR		FirmwareRev[8];
		CHAR		Reserved2[9];
		CHAR		MinorVersion;
		SHORT		MajorVersion;
		CHAR		Reserved3[428];
		CHAR		Reserved4[3584];
	};

	struct ATA_IDENTIFY_DEVICE
	{
		WORD		GeneralConfiguration;					//0
		WORD		LogicalCylinders;						//1	Obsolete
		WORD		SpecificConfiguration;					//2
		WORD		LogicalHeads;							//3 Obsolete
		WORD		Retired1[2];							//4-5
		WORD		LogicalSectors;							//6 Obsolete
		DWORD		ReservedForCompactFlash;				//7-8
		WORD		Retired2;								//9
		CHAR		SerialNumber[20];						//10-19
		WORD		Retired3;								//20
		WORD		BufferSize;								//21 Obsolete
		WORD		Obsolute4;								//22
		CHAR		FirmwareRev[8];							//23-26
		CHAR		Model[40];								//27-46
		WORD		MaxNumPerInterupt;						//47
		WORD		Reserved1;								//48
		WORD		Capabilities1;							//49
		WORD		Capabilities2;							//50
		DWORD		Obsolute5;								//51-52
		WORD		Field88and7064;							//53
		WORD		Obsolute6[5];							//54-58
		WORD		MultSectorStuff;						//59
		DWORD		TotalAddressableSectors;				//60-61
		WORD		Obsolute7;								//62
		WORD		MultiWordDma;							//63
		WORD		PioMode;								//64
		WORD		MinMultiwordDmaCycleTime;				//65
		WORD		RecommendedMultiwordDmaCycleTime;		//66
		WORD		MinPioCycleTimewoFlowCtrl;				//67
		WORD		MinPioCycleTimeWithFlowCtrl;			//68
		WORD		Reserved2[6];							//69-74
		WORD		QueueDepth;								//75
		WORD		SerialAtaCapabilities;					//76
		WORD		SerialAtaAdditionalCapabilities;		//77
		WORD		SerialAtaFeaturesSupported;				//78
		WORD		SerialAtaFeaturesEnabled;				//79
		WORD		MajorVersion;							//80
		WORD		MinorVersion;							//81
		WORD		CommandSetSupported1;					//82
		WORD		CommandSetSupported2;					//83
		WORD		CommandSetSupported3;					//84
		WORD		CommandSetEnabled1;						//85
		WORD		CommandSetEnabled2;						//86
		WORD		CommandSetDefault;						//87
		WORD		UltraDmaMode;							//88
		WORD		TimeReqForSecurityErase;				//89
		WORD		TimeReqForEnhancedSecure;				//90
		WORD		CurrentPowerManagement;					//91
		WORD		MasterPasswordRevision;					//92
		WORD		HardwareResetResult;					//93
		WORD		AcoustricManagement;					//94
		WORD		StreamMinRequestSize;					//95
		WORD		StreamingTimeDma;						//96
		WORD		StreamingAccessLatency;					//97
		DWORD		StreamingPerformance;					//98-99
		ULONGLONG	MaxUserLba;								//100-103
		WORD		StremingTimePio;						//104
		WORD		Reserved3;								//105
		WORD		SectorSize;								//106
		WORD		InterSeekDelay;							//107
		WORD		IeeeOui;								//108
		WORD		UniqueId3;								//109
		WORD		UniqueId2;								//110
		WORD		UniqueId1;								//111
		WORD		Reserved4[4];							//112-115
		WORD		Reserved5;								//116
		DWORD		WordsPerLogicalSector;					//117-118
		WORD		Reserved6[8];							//119-126
		WORD		RemovableMediaStatus;					//127
		WORD		SecurityStatus;							//128
		WORD		VendorSpecific[31];						//129-159
		WORD		CfaPowerMode1;							//160
		WORD		ReservedForCompactFlashAssociation[7];	//161-167
		WORD		DeviceNominalFormFactor;				//168
		WORD		DataSetManagement;						//169
		WORD		AdditionalProductIdentifier[4];			//170-173
		WORD		Reserved7[2];							//174-175
		CHAR		CurrentMediaSerialNo[60];				//176-205
		WORD		SctCommandTransport;					//206
		WORD		ReservedForCeAta1[2];					//207-208
		WORD		AlignmentOfLogicalBlocks;				//209
		DWORD		WriteReadVerifySectorCountMode3;		//210-211
		DWORD		WriteReadVerifySectorCountMode2;		//212-213
		WORD		NvCacheCapabilities;					//214
		DWORD		NvCacheSizeLogicalBlocks;				//215-216
		WORD		NominalMediaRotationRate;				//217
		WORD		Reserved8;								//218
		WORD		NvCacheOptions1;						//219
		WORD		NvCacheOptions2;						//220
		WORD		Reserved9;								//221
		WORD		TransportMajorVersionNumber;			//222
		WORD		TransportMinorVersionNumber;			//223
		WORD		ReservedForCeAta2[10];					//224-233
		WORD		MinimumBlocksPerDownloadMicrocode;		//234
		WORD		MaximumBlocksPerDownloadMicrocode;		//235
		WORD		Reserved10[19];							//236-254
		WORD		IntegrityWord;							//255
	};
#pragma	pack(pop)


///////////////////////////////////////////////////
// from http://naraeon.net/en/archives/1126
///////////////////////////////////////////////////

#define NVME_STORPORT_DRIVER 0xE000
#define NVME_PASS_THROUGH_SRB_IO_CODE \
	CTL_CODE( NVME_STORPORT_DRIVER, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define NVME_SIG_STR "NvmeMini"
#define NVME_SIG_STR_LEN 8
#define NVME_FROM_DEV_TO_HOST 2
#define NVME_IOCTL_VENDOR_SPECIFIC_DW_SIZE 6
#define NVME_IOCTL_CMD_DW_SIZE 16
#define NVME_IOCTL_COMPLETE_DW_SIZE 4
#define NVME_PT_TIMEOUT 40

#define IOCTL_SCSI_GET_ADDRESS \
	CTL_CODE(IOCTL_SCSI_BASE, 0x0406, METHOD_BUFFERED, FILE_ANY_ACCESS)



	typedef struct _SCSI_ADDRESS {
		ULONG Length;
		UCHAR PortNumber;
		UCHAR PathId;
		UCHAR TargetId;
		UCHAR Lun;
	} SCSI_ADDRESS, *PSCSI_ADDRESS;

	struct NVME_PASS_THROUGH_IOCTL {
		SRB_IO_CONTROL SrbIoCtrl;
		DWORD          VendorSpecific[NVME_IOCTL_VENDOR_SPECIFIC_DW_SIZE];
		DWORD          NVMeCmd[NVME_IOCTL_CMD_DW_SIZE];
		DWORD          CplEntry[NVME_IOCTL_COMPLETE_DW_SIZE];
		DWORD          Direction;
		DWORD          QueueId;
		DWORD          DataBufferLen;
		DWORD          MetaDataLen;
		DWORD          ReturnBufferLen;
		UCHAR          DataBuffer[4096];
	};

///////////////////
// from csmisas.h
///////////////////

// IOCTL Control Codes
// (IoctlHeader.ControlCode)

// Control Codes requiring CSMI_ALL_SIGNATURE

#define CC_CSMI_SAS_GET_DRIVER_INFO    1
#define CC_CSMI_SAS_GET_CNTLR_CONFIG   2
#define CC_CSMI_SAS_GET_CNTLR_STATUS   3
#define CC_CSMI_SAS_FIRMWARE_DOWNLOAD  4

// Control Codes requiring CSMI_RAID_SIGNATURE

#define CC_CSMI_SAS_GET_RAID_INFO      10
#define CC_CSMI_SAS_GET_RAID_CONFIG    11
#define CC_CSMI_SAS_GET_RAID_FEATURES  12
#define CC_CSMI_SAS_SET_RAID_CONTROL   13
#define CC_CSMI_SAS_GET_RAID_ELEMENT   14
#define CC_CSMI_SAS_SET_RAID_OPERATION 15

// Control Codes requiring CSMI_SAS_SIGNATURE

#define CC_CSMI_SAS_GET_PHY_INFO       20
#define CC_CSMI_SAS_SET_PHY_INFO       21
#define CC_CSMI_SAS_GET_LINK_ERRORS    22
#define CC_CSMI_SAS_SMP_PASSTHRU       23
#define CC_CSMI_SAS_SSP_PASSTHRU       24
#define CC_CSMI_SAS_STP_PASSTHRU       25
#define CC_CSMI_SAS_GET_SATA_SIGNATURE 26
#define CC_CSMI_SAS_GET_SCSI_ADDRESS   27
#define CC_CSMI_SAS_GET_DEVICE_ADDRESS 28
#define CC_CSMI_SAS_TASK_MANAGEMENT    29
#define CC_CSMI_SAS_GET_CONNECTOR_INFO 30
#define CC_CSMI_SAS_GET_LOCATION       31

// Control Codes requiring CSMI_PHY_SIGNATURE

#define CC_CSMI_SAS_PHY_CONTROL        60

#define IOCTL_HEADER SRB_IO_CONTROL
#define PIOCTL_HEADER PSRB_IO_CONTROL

/*************************************************************************/
/* OS INDEPENDENT CODE                                                   */
/*************************************************************************/

/* * * * * * * * * * Class Independent IOCTL Constants * * * * * * * * * */

// Return codes for all IOCTL's regardless of class
// (IoctlHeader.ReturnCode)

#define CSMI_SAS_STATUS_SUCCESS              0
#define CSMI_SAS_STATUS_FAILED               1
#define CSMI_SAS_STATUS_BAD_CNTL_CODE        2
#define CSMI_SAS_STATUS_INVALID_PARAMETER    3
#define CSMI_SAS_STATUS_WRITE_ATTEMPTED      4

// Signature value
// (IoctlHeader.Signature)

#define CSMI_ALL_SIGNATURE    "CSMIALL"

// Timeout value default of 60 seconds
// (IoctlHeader.Timeout)

#define CSMI_ALL_TIMEOUT      60

//  Direction values for data flow on this IOCTL
// (IoctlHeader.Direction, Linux only)
#define CSMI_SAS_DATA_READ    0
#define CSMI_SAS_DATA_WRITE   1

// I/O Bus Types
// ISA and EISA bus types are not supported
// (bIoBusType)

#define CSMI_SAS_BUS_TYPE_PCI       3
#define CSMI_SAS_BUS_TYPE_PCMCIA    4

// Controller Status
// (uStatus)

#define CSMI_SAS_CNTLR_STATUS_GOOD     1
#define CSMI_SAS_CNTLR_STATUS_FAILED   2
#define CSMI_SAS_CNTLR_STATUS_OFFLINE  3
#define CSMI_SAS_CNTLR_STATUS_POWEROFF 4

// Offline Status Reason
// (uOfflineReason)

#define CSMI_SAS_OFFLINE_REASON_NO_REASON             0
#define CSMI_SAS_OFFLINE_REASON_INITIALIZING          1
#define CSMI_SAS_OFFLINE_REASON_BACKSIDE_BUS_DEGRADED 2
#define CSMI_SAS_OFFLINE_REASON_BACKSIDE_BUS_FAILURE  3

// Controller Class
// (bControllerClass)

#define CSMI_SAS_CNTLR_CLASS_HBA    5

// Controller Flag bits
// (uControllerFlags)

#define CSMI_SAS_CNTLR_SAS_HBA          0x00000001
#define CSMI_SAS_CNTLR_SAS_RAID         0x00000002
#define CSMI_SAS_CNTLR_SATA_HBA         0x00000004
#define CSMI_SAS_CNTLR_SATA_RAID        0x00000008
#define CSMI_SAS_CNTLR_SMART_ARRAY      0x00000010

// for firmware download
#define CSMI_SAS_CNTLR_FWD_SUPPORT      0x00010000
#define CSMI_SAS_CNTLR_FWD_ONLINE       0x00020000
#define CSMI_SAS_CNTLR_FWD_SRESET       0x00040000
#define CSMI_SAS_CNTLR_FWD_HRESET       0x00080000
#define CSMI_SAS_CNTLR_FWD_RROM         0x00100000

// for RAID configuration supported
#define CSMI_SAS_CNTLR_RAID_CFG_SUPPORT 0x01000000

// Download Flag bits
// (uDownloadFlags)
#define CSMI_SAS_FWD_VALIDATE       0x00000001
#define CSMI_SAS_FWD_SOFT_RESET     0x00000002
#define CSMI_SAS_FWD_HARD_RESET     0x00000004

// Firmware Download Status
// (usStatus)
#define CSMI_SAS_FWD_SUCCESS        0
#define CSMI_SAS_FWD_FAILED         1
#define CSMI_SAS_FWD_USING_RROM     2
#define CSMI_SAS_FWD_REJECT         3
#define CSMI_SAS_FWD_DOWNREV        4

// Firmware Download Severity
// (usSeverity>
#define CSMI_SAS_FWD_INFORMATION    0
#define CSMI_SAS_FWD_WARNING        1
#define CSMI_SAS_FWD_ERROR          2
#define CSMI_SAS_FWD_FATAL          3

/* * * * * * * * * * SAS RAID Class IOCTL Constants  * * * * * * * * */

// Return codes for the RAID IOCTL's regardless of class
// (IoctlHeader.ReturnCode)

#define CSMI_SAS_RAID_SET_OUT_OF_RANGE       1000
#define CSMI_SAS_RAID_SET_BUFFER_TOO_SMALL   1001
#define CSMI_SAS_RAID_SET_DATA_CHANGED       1002

// Signature value
// (IoctlHeader.Signature)

#define CSMI_RAID_SIGNATURE    "CSMIARY"

// Timeout value default of 60 seconds
// (IoctlHeader.Timeout)

#define CSMI_RAID_TIMEOUT      60

// RAID Types
// (bRaidType)
#define CSMI_SAS_RAID_TYPE_NONE     0
#define CSMI_SAS_RAID_TYPE_0        1
#define CSMI_SAS_RAID_TYPE_1        2
#define CSMI_SAS_RAID_TYPE_10       3
#define CSMI_SAS_RAID_TYPE_5        4
#define CSMI_SAS_RAID_TYPE_15       5
#define CSMI_SAS_RAID_TYPE_6        6
#define CSMI_SAS_RAID_TYPE_50       7
#define CSMI_SAS_RAID_TYPE_VOLUME   8
#define CSMI_SAS_RAID_TYPE_1E       9
#define CSMI_SAS_RAID_TYPE_OTHER    255
// the last value 255 was already defined for other
// so end is defined as 254
#define CSMI_SAS_RAID_TYPE_END      254

// RAID Status
// (bStatus)
#define CSMI_SAS_RAID_SET_STATUS_OK             0
#define CSMI_SAS_RAID_SET_STATUS_DEGRADED       1
#define CSMI_SAS_RAID_SET_STATUS_REBUILDING     2
#define CSMI_SAS_RAID_SET_STATUS_FAILED         3
#define CSMI_SAS_RAID_SET_STATUS_OFFLINE        4
#define CSMI_SAS_RAID_SET_STATUS_TRANSFORMING   5
#define CSMI_SAS_RAID_SET_STATUS_QUEUED_FOR_REBUILD         6
#define CSMI_SAS_RAID_SET_STATUS_QUEUED_FOR_TRANSFORMATION  7

// RAID Drive Count
// (bDriveCount, 0xF1 to 0xFF are reserved)
#define CSMI_SAS_RAID_DRIVE_COUNT_TOO_BIG   0xF1
#define CSMI_SAS_RAID_DRIVE_COUNT_SUPRESSED 0xF2

// RAID Data Type
// (bDataType)
#define CSMI_SAS_RAID_DATA_DRIVES           0
#define CSMI_SAS_RAID_DATA_DEVICE_ID        1
#define CSMI_SAS_RAID_DATA_ADDITIONAL_DATA  2

// RAID Drive Status
// (bDriveStatus)
#define CSMI_SAS_DRIVE_STATUS_OK          0
#define CSMI_SAS_DRIVE_STATUS_REBUILDING  1
#define CSMI_SAS_DRIVE_STATUS_FAILED      2
#define CSMI_SAS_DRIVE_STATUS_DEGRADED    3
#define CSMI_SAS_DRIVE_STATUS_OFFLINE     4
#define CSMI_SAS_DRIVE_STATUS_QUEUED_FOR_REBUILD 5

// RAID Drive Usage
// (bDriveUsage)
#define CSMI_SAS_DRIVE_CONFIG_NOT_USED      0
#define CSMI_SAS_DRIVE_CONFIG_MEMBER        1
#define CSMI_SAS_DRIVE_CONFIG_SPARE         2
#define CSMI_SAS_DRIVE_CONFIG_SPARE_ACTIVE  3

// RAID Drive Type
// (bDriveType)
#define CSMI_SAS_DRIVE_TYPE_UNKNOWN         0
#define CSMI_SAS_DRIVE_TYPE_SINGLE_PORT_SAS 1
#define CSMI_SAS_DRIVE_TYPE_DUAL_PORT_SAS   2
#define CSMI_SAS_DRIVE_TYPE_SATA            3
#define CSMI_SAS_DRIVE_TYPE_SATA_PS         4
#define CSMI_SAS_DRIVE_TYPE_OTHER           255

// RAID Write Protect
// (bWriteProtect)
#define CSMI_SAS_RAID_SET_WRITE_PROTECT_UNKNOWN     0
#define CSMI_SAS_RAID_SET_WRITE_PROTECT_UNCHANGED   0
#define CSMI_SAS_RAID_SET_WRITE_PROTECT_ENABLED     1
#define CSMI_SAS_RAID_SET_WRITE_PROTECT_DISABLED    2

// RAID Cache Setting
// (bCacheSetting)
#define CSMI_SAS_RAID_SET_CACHE_UNKNOWN             0
#define CSMI_SAS_RAID_SET_CACHE_UNCHANGED           0
#define CSMI_SAS_RAID_SET_CACHE_ENABLED             1
#define CSMI_SAS_RAID_SET_CACHE_DISABLED            2
#define CSMI_SAS_RAID_SET_CACHE_CORRUPT             3

// RAID Features
// (uFeatures)
#define CSMI_SAS_RAID_FEATURE_TRANSFORMATION    0x00000001
#define CSMI_SAS_RAID_FEATURE_REBUILD           0x00000002
#define CSMI_SAS_RAID_FEATURE_SPLIT_MIRROR      0x00000004
#define CSMI_SAS_RAID_FEATURE_MERGE_MIRROR      0x00000008
#define CSMI_SAS_RAID_FEATURE_LUN_RENUMBER      0x00000010
#define CSMI_SAS_RAID_FEATURE_SURFACE_SCAN      0x00000020
#define CSMI_SAS_RAID_FEATURE_SPARES_SHARED     0x00000040

// RAID Priority
// (bDefaultTransformPriority, etc.)
#define CSMI_SAS_PRIORITY_UNKNOWN   0
#define CSMI_SAS_PRIORITY_UNCHANGED 0
#define CSMI_SAS_PRIORITY_AUTO      1
#define CSMI_SAS_PRIORITY_OFF       2
#define CSMI_SAS_PRIORITY_LOW       3
#define CSMI_SAS_PRIORITY_MEDIUM    4
#define CSMI_SAS_PRIORITY_HIGH      5

// RAID Transformation Rules
// (uRaidSetTransformationRules)
#define CSMI_SAS_RAID_RULE_AVAILABLE_MEMORY     0x00000001
#define CSMI_SAS_RAID_RULE_OVERLAPPED_EXTENTS   0x00000002

// RAID Cache Ratios Supported
// (bCacheRatiosSupported)
// from 0 to 100 defines the write to read ratio, 0 is 100% write
#define CSMI_SAS_RAID_CACHE_RATIO_RANGE     101
#define CSMI_SAS_RAID_CACHE_RATIO_FIXED     102
#define CSMI_SAS_RAID_CACHE_RATIO_AUTO      103
#define CSMI_SAS_RAID_CACHE_RATIO_END       255

// RAID Cache Ratio Flag
// (bCacheRatioFlag)
#define CSMI_SAS_RAID_CACHE_RATIO_DISABLE   0
#define CSMI_SAS_RAID_CACHE_RATIO_ENABLE    1

// RAID Clear Configuration Signature
// (bClearConfiguration)
#define CSMI_SAS_RAID_CLEAR_CONFIGURATION_SIGNATURE "RAIDCLR"

// RAID Failure Codes
// (uFailureCode)
#define CSMI_SAS_FAIL_CODE_OK                           0
#define CSMI_SAS_FAIL_CODE_PARAMETER_INVALID            1000
#define CSMI_SAS_FAIL_CODE_TRANSFORM_PRIORITY_INVALID   1001
#define CSMI_SAS_FAIL_CODE_REBUILD_PRIORITY_INVALID     1002
#define CSMI_SAS_FAIL_CODE_CACHE_RATIO_INVALID          1003
#define CSMI_SAS_FAIL_CODE_SURFACE_SCAN_INVALID         1004
#define CSMI_SAS_FAIL_CODE_CLEAR_CONFIGURATION_INVALID  1005
#define CSMI_SAS_FAIL_CODE_ELEMENT_INDEX_INVALID        1006
#define CSMI_SAS_FAIL_CODE_SUBELEMENT_INDEX_INVALID     1007
#define CSMI_SAS_FAIL_CODE_EXTENT_INVALID               1008
#define CSMI_SAS_FAIL_CODE_BLOCK_COUNT_INVALID          1009
#define CSMI_SAS_FAIL_CODE_DRIVE_INDEX_INVALID          1010
#define CSMI_SAS_FAIL_CODE_EXISTING_LUN_INVALID         1011
#define CSMI_SAS_FAIL_CODE_RAID_TYPE_INVALID            1012
#define CSMI_SAS_FAIL_CODE_STRIPE_SIZE_INVALID          1013
#define CSMI_SAS_FAIL_CODE_TRANSFORMATION_INVALID       1014
#define CSMI_SAS_FAIL_CODE_CHANGE_COUNT_INVALID         1015
#define CSMI_SAS_FAIL_CODE_ENUMERATION_TYPE_INVALID     1016

#define CSMI_SAS_FAIL_CODE_EXCEEDED_RAID_SET_COUNT      2000
#define CSMI_SAS_FAIL_CODE_DUPLICATE_LUN                2001

#define CSMI_SAS_FAIL_CODE_WAIT_FOR_OPERATION           3000

// RAID Enumeration Types
// (uEnumerationType)
#define CSMI_SAS_RAID_ELEMENT_TYPE_DRIVE                0
#define CSMI_SAS_RAID_ELEMENT_TYPE_MODULE               1
#define CSMI_SAS_RAID_ELEMENT_TYPE_DRIVE_RAID_SET       2
#define CSMI_SAS_RAID_ELEMENT_TYPE_EXTENT_DRIVE         3

// RAID Extent Types
// (bExtentType)
#define CSMI_SAS_RAID_EXTENT_RESERVED       0
#define CSMI_SAS_RAID_EXTENT_METADATA       1
#define CSMI_SAS_RAID_EXTENT_ALLOCATED      2
#define CSMI_SAS_RAID_EXTENT_UNALLOCATED    3

// RAID Operation Types
// (uOperationType)
#define CSMI_SAS_RAID_SET_CREATE            0
#define CSMI_SAS_RAID_SET_LABEL             1
#define CSMI_SAS_RAID_SET_TRANSFORM         2
#define CSMI_SAS_RAID_SET_DELETE            3
#define CSMI_SAS_RAID_SET_WRITE_PROTECT     4
#define CSMI_SAS_RAID_SET_CACHE             5
#define CSMI_SAS_RAID_SET_ONLINE_STATE      6
#define CSMI_SAS_RAID_SET_SPARE             7

// RAID Transform Types
// (bTransformType)
#define CSMI_SAS_RAID_SET_TRANSFORM_SPLIT_MIRROR    0
#define CSMI_SAS_RAID_SET_TRANSFORM_MERGE_RAID_0    1
#define CSMI_SAS_RAID_SET_TRANSFORM_LUN_RENUMBER    2
#define CSMI_SAS_RAID_SET_TRANSFORM_RAID_SET        3

// RAID Online State
// (bOnlineState)
#define CSMI_SAS_RAID_SET_STATE_UNKNOWN     0
#define CSMI_SAS_RAID_SET_STATE_ONLINE      1
#define CSMI_SAS_RAID_SET_STATE_OFFLINE     2

/* * * * * * * * * * SAS HBA Class IOCTL Constants * * * * * * * * * */

// Return codes for SAS IOCTL's
// (IoctlHeader.ReturnCode)

#define CSMI_SAS_PHY_INFO_CHANGED            CSMI_SAS_STATUS_SUCCESS
#define CSMI_SAS_PHY_INFO_NOT_CHANGEABLE     2000
#define CSMI_SAS_LINK_RATE_OUT_OF_RANGE      2001

#define CSMI_SAS_PHY_DOES_NOT_EXIST          2002
#define CSMI_SAS_PHY_DOES_NOT_MATCH_PORT     2003
#define CSMI_SAS_PHY_CANNOT_BE_SELECTED      2004
#define CSMI_SAS_SELECT_PHY_OR_PORT          2005
#define CSMI_SAS_PORT_DOES_NOT_EXIST         2006
#define CSMI_SAS_PORT_CANNOT_BE_SELECTED     2007
#define CSMI_SAS_CONNECTION_FAILED           2008

#define CSMI_SAS_NO_SATA_DEVICE              2009
#define CSMI_SAS_NO_SATA_SIGNATURE           2010
#define CSMI_SAS_SCSI_EMULATION              2011
#define CSMI_SAS_NOT_AN_END_DEVICE           2012
#define CSMI_SAS_NO_SCSI_ADDRESS             2013
#define CSMI_SAS_NO_DEVICE_ADDRESS           2014

// Signature value
// (IoctlHeader.Signature)

#define CSMI_SAS_SIGNATURE    "CSMISAS"

// Timeout value default of 60 seconds
// (IoctlHeader.Timeout)

#define CSMI_SAS_TIMEOUT      60

// Device types
// (bDeviceType)

#define CSMI_SAS_PHY_UNUSED               0x00
#define CSMI_SAS_NO_DEVICE_ATTACHED       0x00
#define CSMI_SAS_END_DEVICE               0x10
#define CSMI_SAS_EDGE_EXPANDER_DEVICE     0x20
#define CSMI_SAS_FANOUT_EXPANDER_DEVICE   0x30

// Protocol options
// (bInitiatorPortProtocol, bTargetPortProtocol)

#define CSMI_SAS_PROTOCOL_SATA   0x01
#define CSMI_SAS_PROTOCOL_SMP    0x02
#define CSMI_SAS_PROTOCOL_STP    0x04
#define CSMI_SAS_PROTOCOL_SSP    0x08

// Negotiated and hardware link rates
// (bNegotiatedLinkRate, bMinimumLinkRate, bMaximumLinkRate)

#define CSMI_SAS_LINK_RATE_UNKNOWN  0x00
#define CSMI_SAS_PHY_DISABLED       0x01
#define CSMI_SAS_LINK_RATE_FAILED   0x02
#define CSMI_SAS_SATA_SPINUP_HOLD   0x03
#define CSMI_SAS_SATA_PORT_SELECTOR 0x04
#define CSMI_SAS_LINK_RATE_1_5_GBPS 0x08
#define CSMI_SAS_LINK_RATE_3_0_GBPS 0x09
#define CSMI_SAS_LINK_VIRTUAL       0x10

// Discover state
// (bAutoDiscover)

#define CSMI_SAS_DISCOVER_NOT_SUPPORTED   0x00
#define CSMI_SAS_DISCOVER_NOT_STARTED     0x01
#define CSMI_SAS_DISCOVER_IN_PROGRESS     0x02
#define CSMI_SAS_DISCOVER_COMPLETE        0x03
#define CSMI_SAS_DISCOVER_ERROR           0x04

// Phy features

#define CSMI_SAS_PHY_VIRTUAL_SMP          0x01

// Programmed link rates
// (bMinimumLinkRate, bMaximumLinkRate)
// (bProgrammedMinimumLinkRate, bProgrammedMaximumLinkRate)

#define CSMI_SAS_PROGRAMMED_LINK_RATE_UNCHANGED 0x00
#define CSMI_SAS_PROGRAMMED_LINK_RATE_1_5_GBPS  0x08
#define CSMI_SAS_PROGRAMMED_LINK_RATE_3_0_GBPS  0x09

// Link rate
// (bNegotiatedLinkRate in CSMI_SAS_SET_PHY_INFO)

#define CSMI_SAS_LINK_RATE_NEGOTIATE      0x00
#define CSMI_SAS_LINK_RATE_PHY_DISABLED   0x01

// Signal class
// (bSignalClass in CSMI_SAS_SET_PHY_INFO)

#define CSMI_SAS_SIGNAL_CLASS_UNKNOWN     0x00
#define CSMI_SAS_SIGNAL_CLASS_DIRECT      0x01
#define CSMI_SAS_SIGNAL_CLASS_SERVER      0x02
#define CSMI_SAS_SIGNAL_CLASS_ENCLOSURE   0x03

// Link error reset
// (bResetCounts)

#define CSMI_SAS_LINK_ERROR_DONT_RESET_COUNTS   0x00
#define CSMI_SAS_LINK_ERROR_RESET_COUNTS        0x01

// Phy identifier
// (bPhyIdentifier)

#define CSMI_SAS_USE_PORT_IDENTIFIER   0xFF

// Port identifier
// (bPortIdentifier)

#define CSMI_SAS_IGNORE_PORT           0xFF

// Programmed link rates
// (bConnectionRate)

#define CSMI_SAS_LINK_RATE_NEGOTIATED  0x00
#define CSMI_SAS_LINK_RATE_1_5_GBPS    0x08
#define CSMI_SAS_LINK_RATE_3_0_GBPS    0x09

// Connection status
// (bConnectionStatus)

#define CSMI_SAS_OPEN_ACCEPT                          0
#define CSMI_SAS_OPEN_REJECT_BAD_DESTINATION          1
#define CSMI_SAS_OPEN_REJECT_RATE_NOT_SUPPORTED       2
#define CSMI_SAS_OPEN_REJECT_NO_DESTINATION           3
#define CSMI_SAS_OPEN_REJECT_PATHWAY_BLOCKED          4
#define CSMI_SAS_OPEN_REJECT_PROTOCOL_NOT_SUPPORTED   5
#define CSMI_SAS_OPEN_REJECT_RESERVE_ABANDON          6
#define CSMI_SAS_OPEN_REJECT_RESERVE_CONTINUE         7
#define CSMI_SAS_OPEN_REJECT_RESERVE_INITIALIZE       8
#define CSMI_SAS_OPEN_REJECT_RESERVE_STOP             9
#define CSMI_SAS_OPEN_REJECT_RETRY                    10
#define CSMI_SAS_OPEN_REJECT_STP_RESOURCES_BUSY       11
#define CSMI_SAS_OPEN_REJECT_WRONG_DESTINATION        12

// SSP Status
// (bSSPStatus)

#define CSMI_SAS_SSP_STATUS_UNKNOWN     0x00
#define CSMI_SAS_SSP_STATUS_WAITING     0x01
#define CSMI_SAS_SSP_STATUS_COMPLETED   0x02
#define CSMI_SAS_SSP_STATUS_FATAL_ERROR 0x03
#define CSMI_SAS_SSP_STATUS_RETRY       0x04
#define CSMI_SAS_SSP_STATUS_NO_TAG      0x05

// SSP Flags
// (uFlags)

#define CSMI_SAS_SSP_READ           0x00000001
#define CSMI_SAS_SSP_WRITE          0x00000002
#define CSMI_SAS_SSP_UNSPECIFIED    0x00000004

#define CSMI_SAS_SSP_TASK_ATTRIBUTE_SIMPLE         0x00000000
#define CSMI_SAS_SSP_TASK_ATTRIBUTE_HEAD_OF_QUEUE  0x00000010
#define CSMI_SAS_SSP_TASK_ATTRIBUTE_ORDERED        0x00000020
#define CSMI_SAS_SSP_TASK_ATTRIBUTE_ACA            0x00000040

// SSP Data present
// (bDataPresent)

#define CSMI_SAS_SSP_NO_DATA_PRESENT         0x00
#define CSMI_SAS_SSP_RESPONSE_DATA_PRESENT   0x01
#define CSMI_SAS_SSP_SENSE_DATA_PRESENT      0x02

// STP Flags
// (uFlags)

#define CSMI_SAS_STP_READ           0x00000001
#define CSMI_SAS_STP_WRITE          0x00000002
#define CSMI_SAS_STP_UNSPECIFIED    0x00000004
#define CSMI_SAS_STP_PIO            0x00000010
#define CSMI_SAS_STP_DMA            0x00000020
#define CSMI_SAS_STP_PACKET         0x00000040
#define CSMI_SAS_STP_DMA_QUEUED     0x00000080
#define CSMI_SAS_STP_EXECUTE_DIAG   0x00000100
#define CSMI_SAS_STP_RESET_DEVICE   0x00000200

// Task Management Flags
// (uFlags)

#define CSMI_SAS_TASK_IU               0x00000001
#define CSMI_SAS_HARD_RESET_SEQUENCE   0x00000002
#define CSMI_SAS_SUPPRESS_RESULT       0x00000004

// Task Management Functions
// (bTaskManagement)

#define CSMI_SAS_SSP_ABORT_TASK           0x01
#define CSMI_SAS_SSP_ABORT_TASK_SET       0x02
#define CSMI_SAS_SSP_CLEAR_TASK_SET       0x04
#define CSMI_SAS_SSP_LOGICAL_UNIT_RESET   0x08
#define CSMI_SAS_SSP_CLEAR_ACA            0x40
#define CSMI_SAS_SSP_QUERY_TASK           0x80

// Task Management Information
// (uInformation)

#define CSMI_SAS_SSP_TEST           1
#define CSMI_SAS_SSP_EXCEEDED       2
#define CSMI_SAS_SSP_DEMAND         3
#define CSMI_SAS_SSP_TRIGGER        4

// Connector Pinout Information
// (uPinout)

#define CSMI_SAS_CON_UNKNOWN              0x00000001
#define CSMI_SAS_CON_SFF_8482             0x00000002
#define CSMI_SAS_CON_SFF_8470_LANE_1      0x00000100
#define CSMI_SAS_CON_SFF_8470_LANE_2      0x00000200
#define CSMI_SAS_CON_SFF_8470_LANE_3      0x00000400
#define CSMI_SAS_CON_SFF_8470_LANE_4      0x00000800
#define CSMI_SAS_CON_SFF_8484_LANE_1      0x00010000
#define CSMI_SAS_CON_SFF_8484_LANE_2      0x00020000
#define CSMI_SAS_CON_SFF_8484_LANE_3      0x00040000
#define CSMI_SAS_CON_SFF_8484_LANE_4      0x00080000

// Connector Location Information
// (bLocation)

// same as uPinout above...
// #define CSMI_SAS_CON_UNKNOWN              0x01
#define CSMI_SAS_CON_INTERNAL             0x02
#define CSMI_SAS_CON_EXTERNAL             0x04
#define CSMI_SAS_CON_SWITCHABLE           0x08
#define CSMI_SAS_CON_AUTO                 0x10
#define CSMI_SAS_CON_NOT_PRESENT          0x20
#define CSMI_SAS_CON_NOT_CONNECTED        0x80

// Device location identification
// (bIdentify)

#define CSMI_SAS_LOCATE_UNKNOWN           0x00
#define CSMI_SAS_LOCATE_FORCE_OFF         0x01
#define CSMI_SAS_LOCATE_FORCE_ON          0x02

// Location Valid flags
// (uLocationFlags)

#define CSMI_SAS_LOCATE_SAS_ADDRESS_VALID           0x00000001
#define CSMI_SAS_LOCATE_SAS_LUN_VALID               0x00000002
#define CSMI_SAS_LOCATE_ENCLOSURE_IDENTIFIER_VALID  0x00000004
#define CSMI_SAS_LOCATE_ENCLOSURE_NAME_VALID        0x00000008
#define CSMI_SAS_LOCATE_BAY_PREFIX_VALID            0x00000010
#define CSMI_SAS_LOCATE_BAY_IDENTIFIER_VALID        0x00000020
#define CSMI_SAS_LOCATE_LOCATION_STATE_VALID        0x00000040

/* * * * * * * * SAS Phy Control Class IOCTL Constants * * * * * * * * */

// Return codes for SAS Phy Control IOCTL's
// (IoctlHeader.ReturnCode)

// Signature value
// (IoctlHeader.Signature)

#define CSMI_PHY_SIGNATURE    "CSMIPHY"

// Phy Control Functions
// (bFunction)

// values 0x00 to 0xFF are consistent in definition with the SMP PHY CONTROL
// function defined in the SAS spec
#define CSMI_SAS_PC_NOP                   0x00000000
#define CSMI_SAS_PC_LINK_RESET            0x00000001
#define CSMI_SAS_PC_HARD_RESET            0x00000002
#define CSMI_SAS_PC_PHY_DISABLE           0x00000003
// 0x04 to 0xFF reserved...
#define CSMI_SAS_PC_GET_PHY_SETTINGS      0x00000100

// Link Flags
#define CSMI_SAS_PHY_ACTIVATE_CONTROL     0x00000001
#define CSMI_SAS_PHY_UPDATE_SPINUP_RATE   0x00000002
#define CSMI_SAS_PHY_AUTO_COMWAKE         0x00000004

// Device Types for Phy Settings
// (bType)
#define CSMI_SAS_UNDEFINED 0x00
#define CSMI_SAS_SATA      0x01
#define CSMI_SAS_SAS       0x02

// Transmitter Flags
// (uTransmitterFlags)
#define CSMI_SAS_PHY_PREEMPHASIS_DISABLED    0x00000001

// Receiver Flags
// (uReceiverFlags)
#define CSMI_SAS_PHY_EQUALIZATION_DISABLED   0x00000001

// Pattern Flags
// (uPatternFlags)
// #define CSMI_SAS_PHY_ACTIVATE_CONTROL     0x00000001
#define CSMI_SAS_PHY_DISABLE_SCRAMBLING      0x00000002
#define CSMI_SAS_PHY_DISABLE_ALIGN           0x00000004
#define CSMI_SAS_PHY_DISABLE_SSC             0x00000008

#define CSMI_SAS_PHY_FIXED_PATTERN           0x00000010
#define CSMI_SAS_PHY_USER_PATTERN            0x00000020

// Fixed Patterns
// (bFixedPattern)
#define CSMI_SAS_PHY_CJPAT                   0x00000001
#define CSMI_SAS_PHY_ALIGN                   0x00000002

// Type Flags
// (bTypeFlags)
#define CSMI_SAS_PHY_POSITIVE_DISPARITY      0x01
#define CSMI_SAS_PHY_NEGATIVE_DISPARITY      0x02
#define CSMI_SAS_PHY_CONTROL_CHARACTER       0x04

// Miscellaneous
#define SLOT_NUMBER_UNKNOWN   0xFFFF

#pragma pack(8)

	typedef struct _CSMI_SAS_DRIVER_INFO
	{
		UCHAR  szName[81];
		UCHAR  szDescription[81];
		USHORT usMajorRevision;
		USHORT usMinorRevision;
		USHORT usBuildRevision;
		USHORT usReleaseRevision;
		USHORT usCSMIMajorRevision;
		USHORT usCSMIMinorRevision;
	} CSMI_SAS_DRIVER_INFO, *PCSMI_SAS_DRIVER_INFO;

	typedef struct _CSMI_SAS_DRIVER_INFO_BUFFER
	{
		SRB_IO_CONTROL IoctlHeader;
		CSMI_SAS_DRIVER_INFO Information;
	} CSMI_SAS_DRIVER_INFO_BUFFER, *PCSMI_SAS_DRIVER_INFO_BUFFER;

	typedef struct _CSMI_SAS_IDENTIFY
	{
		UCHAR bDeviceType;
		UCHAR bRestricted;
		UCHAR bInitiatorPortProtocol;
		UCHAR bTargetPortProtocol;
		UCHAR bRestricted2[8];
		UCHAR bSASAddress[8];
		UCHAR bPhyIdentifier;
		UCHAR bSignalClass;
		UCHAR bReserved[6];
	} CSMI_SAS_IDENTIFY, *PCSMI_SAS_IDENTIFY;

	typedef struct _CSMI_SAS_PHY_ENTITY
	{
		CSMI_SAS_IDENTIFY Identify;
		UCHAR bPortIdentifier;
		UCHAR bNegotiatedLinkRate;
		UCHAR bMinimumLinkRate;
		UCHAR bMaximumLinkRate;
		UCHAR bPhyChangeCount;
		UCHAR bAutoDiscover;
		UCHAR bPhyFeatures;
		UCHAR bReserved;
		CSMI_SAS_IDENTIFY Attached;
	} CSMI_SAS_PHY_ENTITY, *PCSMI_SAS_PHY_ENTITY;

	typedef struct _CSMI_SAS_PHY_INFO
	{
		UCHAR bNumberOfPhys;
		UCHAR bReserved[3];
		CSMI_SAS_PHY_ENTITY Phy[32];
	} CSMI_SAS_PHY_INFO, *PCSMI_SAS_PHY_INFO;

	typedef struct _CSMI_SAS_PHY_INFO_BUFFER
	{
		IOCTL_HEADER IoctlHeader;
		CSMI_SAS_PHY_INFO Information;
	} CSMI_SAS_PHY_INFO_BUFFER, *PCSMI_SAS_PHY_INFO_BUFFER;

	typedef struct _CSMI_SAS_STP_PASSTHRU
	{
		UCHAR bPhyIdentifier;
		UCHAR bPortIdentifier;
		UCHAR bConnectionRate;
		UCHAR bReserved;
		UCHAR bDestinationSASAddress[8];
		UCHAR bReserved2[4];
		UCHAR bCommandFIS[20];
		ULONG uFlags;
		ULONG uDataLength;
	} CSMI_SAS_STP_PASSTHRU, *PCSMI_SAS_STP_PASSTHRU;
	
	typedef struct _CSMI_SAS_STP_PASSTHRU_STATUS
	{
		UCHAR bConnectionStatus;
		UCHAR bReserved[3];
		UCHAR bStatusFIS[20];
		ULONG uSCR[16];
		ULONG uDataBytes;
	} CSMI_SAS_STP_PASSTHRU_STATUS, *PCSMI_SAS_STP_PASSTHRU_STATUS;
	
	typedef struct _CSMI_SAS_STP_PASSTHRU_BUFFER
	{
		SRB_IO_CONTROL IoctlHeader;
		CSMI_SAS_STP_PASSTHRU Parameters;
		CSMI_SAS_STP_PASSTHRU_STATUS Status;
		UCHAR bDataBuffer[1];
	} CSMI_SAS_STP_PASSTHRU_BUFFER, *PCSMI_SAS_STP_PASSTHRU_BUFFER;

// CC_CSMI_SAS_RAID_INFO

	typedef struct _CSMI_SAS_RAID_INFO
	{
		UINT uNumRaidSets;
		UINT uMaxDrivesPerSet;
		UINT uMaxRaidSets;
		UCHAR  bMaxRaidTypes;
		UCHAR  bReservedByteFields[7];
		struct
		{
			UINT uLowPart;
			UINT uHighPart;
		} ulMinRaidSetBlocks;
		struct
		{
			UINT uLowPart;
			UINT uHighPart;
		} ulMaxRaidSetBlocks;
		UINT uMaxPhysicalDrives;
		UINT uMaxExtents;
		UINT uMaxModules;
		UINT uMaxTransformationMemory;
		UINT uChangeCount;
		UCHAR  bReserved[44];
	} CSMI_SAS_RAID_INFO, *PCSMI_SAS_RAID_INFO;

	typedef struct _CSMI_SAS_RAID_INFO_BUFFER
	{
		IOCTL_HEADER IoctlHeader;
		CSMI_SAS_RAID_INFO Information;
	} CSMI_SAS_RAID_INFO_BUFFER, *PCSMI_SAS_RAID_INFO_BUFFER;

// CC_CSMI_SAS_GET_RAID_CONFIG

	typedef struct _CSMI_SAS_RAID_DRIVES
	{
		UCHAR  bModel[40];
		UCHAR  bFirmware[8];
		UCHAR  bSerialNumber[40];
		UCHAR  bSASAddress[8];
		UCHAR  bSASLun[8];
		UCHAR  bDriveStatus;
		UCHAR  bDriveUsage;
		USHORT usBlockSize;
		UCHAR  bDriveType;
		UCHAR  bReserved[15];
		UINT uDriveIndex;
	   struct
	   {
		  UINT uLowPart;
		  UINT uHighPart;
	   } ulTotalUserBlocks;
	} CSMI_SAS_RAID_DRIVES,
	  *PCSMI_SAS_RAID_DRIVES;

typedef struct _CSMI_SAS_RAID_DEVICE_ID {
   UCHAR  bDeviceIdentificationVPDPage[1];
} CSMI_SAS_RAID_DEVICE_ID,
  *PCSMI_SAS_RAID_DEVICE_ID;

typedef struct _CSMI_SAS_RAID_SET_ADDITIONAL_DATA {
   UCHAR  bLabel[16];
   UCHAR  bRaidSetLun[8];
   UCHAR  bWriteProtection;
   UCHAR  bCacheSetting;
   UCHAR  bCacheRatio;
   USHORT usBlockSize;
   UCHAR  bReservedBytes[11];
   struct
   {
	  UINT uLowPart;
	  UINT uHighPart;
   } ulRaidSetExtentOffset;
   struct
   {
	  UINT uLowPart;
	  UINT uHighPart;
   } ulRaidSetBlocks;
   UINT uStripeSizeInBlocks;
   UINT uSectorsPerTrack;
   UCHAR  bApplicationScratchPad[16];
   UINT uNumberOfHeads;
   UINT uNumberOfTracks;
   UCHAR  bReserved[24];
} CSMI_SAS_RAID_SET_ADDITIONAL_DATA,
  *PCSMI_SAS_RAID_SET_ADDITIONAL_DATA;

typedef struct _CSMI_SAS_RAID_CONFIG {
   UINT uRaidSetIndex;
   UINT uCapacity;
   UINT uStripeSize;
   UCHAR  bRaidType;
   UCHAR  bStatus;
   UCHAR  bInformation;
   UCHAR  bDriveCount;
   UCHAR  bDataType;
   UCHAR  bReserved[11];
   UINT uFailureCode;
   UINT uChangeCount;
   union {
	  CSMI_SAS_RAID_DRIVES Drives[1];
	  CSMI_SAS_RAID_DEVICE_ID DeviceId[1];
	  CSMI_SAS_RAID_SET_ADDITIONAL_DATA Data[1];
   };
} CSMI_SAS_RAID_CONFIG,
   *PCSMI_SAS_RAID_CONFIG;

typedef struct _CSMI_SAS_RAID_CONFIG_BUFFER {
   IOCTL_HEADER IoctlHeader;
   CSMI_SAS_RAID_CONFIG Configuration;
} CSMI_SAS_RAID_CONFIG_BUFFER,
  *PCSMI_SAS_RAID_CONFIG_BUFFER;

#pragma pack()

//////////////////////////////////////////////////////////////////
// for Intel RST NVMe
//////////////////////////////////////////////////////////////////
#pragma pack(push, 1)

typedef union
{
	struct
	{
		ULONG Opcode : 8;
		ULONG FUSE : 2;
		ULONG _Rsvd : 4;
		ULONG PSDT : 2;
		ULONG CID : 16;
	} DUMMYSTRUCTNAME;
	ULONG AsDWord;
} NVME_CDW0, * PNVME_CDW0;

// NVMe Command Format
// See NVMe specification 1.3c Section 4.2, Figure 10
typedef union
{
	struct
	{
		ULONG   CNS : 2;
		ULONG   _Rsvd : 30;
	} DUMMYSTRUCTNAME;
	ULONG AsDWord;
} NVME_IDENTIFY_CDW10, * PNVME_IDENTIFY_CDW10;

// NVMe Specification < 1.3
typedef union
{
	struct
	{
		ULONG   LID : 8;
		ULONG   _Rsvd1 : 8;
		ULONG   NUMD : 12;
		ULONG   _Rsvd2 : 4;
	} DUMMYSTRUCTNAME;
	ULONG   AsDWord;
} NVME_GET_LOG_PAGE_CDW10, * PNVME_GET_LOG_PAGE_CDW10;

// NVMe Specification >= 1.3
typedef union
{
	struct
	{
		ULONG   LID : 8;
		ULONG   LSP : 4;
		ULONG   Reserved0 : 3;
		ULONG   RAE : 1;
		ULONG   NUMDL : 16;
	} DUMMYSTRUCTNAME;
	ULONG   AsDWord;
} NVME_GET_LOG_PAGE_CDW10_V13, * PNVME_GET_LOG_PAGE_CDW10_V13;

typedef struct
{
	// Common fields for all commands
	NVME_CDW0           CDW0;

	ULONG               NSID;
	ULONG               _Rsvd[2];
	ULONGLONG           MPTR;
	ULONGLONG           PRP1;
	ULONGLONG           PRP2;

	// Command independent fields from CDW10 to CDW15
	union
	{
		// Admin Command: Identify (6)
		struct
		{
			NVME_IDENTIFY_CDW10 CDW10;
			ULONG   CDW11;
			ULONG   CDW12;
			ULONG   CDW13;
			ULONG   CDW14;
			ULONG   CDW15;
		} IDENTIFY;

		// Admin Command: Get Log Page (2)
		struct
		{
			NVME_GET_LOG_PAGE_CDW10 CDW10;
			//NVME_GET_LOG_PAGE_CDW10_V13 CDW10;
			ULONG   CDW11;
			ULONG   CDW12;
			ULONG   CDW13;
			ULONG   CDW14;
			ULONG   CDW15;
		} GET_LOG_PAGE;
	} u;
} NVME_CMD, * PNVME_CMD;

typedef struct _INTEL_NVME_PAYLOAD
{
	BYTE    Version;        // 0x001C
	BYTE    PathId;         // 0x001D
	BYTE    TargetID;       // 0x001E
	BYTE    Lun;            // 0x001F
	NVME_CMD Cmd;           // 0x0020 ~ 0x005F
	DWORD   CplEntry[4];    // 0x0060 ~ 0x006F
	DWORD   QueueId;        // 0x0070 ~ 0x0073
	DWORD   ParamBufLen;    // 0x0074
	DWORD   ReturnBufferLen;// 0x0078
	BYTE    __rsvd2[0x28];  // 0x007C ~ 0xA3
} INTEL_NVME_PAYLOAD, * PINTEL_NVME_PAYLOAD;

typedef struct _INTEL_NVME_PASS_THROUGH
{
	SRB_IO_CONTROL SRB;     // 0x0000 ~ 0x001B
	INTEL_NVME_PAYLOAD Payload;
	BYTE DataBuffer[0x1000];
} INTEL_NVME_PASS_THROUGH, * PINTEL_NVME_PASS_THROUGH;
#pragma pack(pop)

#define IOCTL_INTEL_NVME_PASS_THROUGH CTL_CODE(0xf000, 0xA02, METHOD_BUFFERED, FILE_ANY_ACCESS);

//////////////////////////////////////////////////////////////////
// for MegaRAID SAS
//////////////////////////////////////////////////////////////////
#pragma pack(push, 1)

#define MAX_SYS_PDS               240

struct MEGARAID_PASS_THROUGH
{
	UCHAR Cmd;
	UCHAR SenseLength;
	UCHAR CmdStatus;
	UCHAR ScsiStatus;

	UCHAR TargetId;
	UCHAR Lun;
	UCHAR CdbLength;
	UCHAR SenseInfoLength;

	ULONG Context;
	ULONG Padding0;

	USHORT Flags;
	USHORT TimeOutValue;
	ULONG DataTransferLength;

	ULONG SenseInfoOffsetLo;
	ULONG SenseInfoOffsetHi;

	UCHAR Cdb[16];
};

struct MEGARAID_PASS_THROUGH_IOCTL
{
	SRB_IO_CONTROL    SrbIoCtrl;
	MEGARAID_PASS_THROUGH Mpt;
	UCHAR             SenseBuf[112];
	UCHAR             DataBuf[4096];
};

struct MEGARAID_DCOMD
{
	UCHAR Cmd;
	UCHAR Reserved0;
	UCHAR CmdStatus;
	UCHAR Reserved1[4];
	UCHAR SenseInfoLength;

	ULONG Context;
	ULONG Padding0;

	USHORT Flags;
	USHORT TimeOutValue;

	ULONG DataTransferLength;
	ULONG Opcode;

	UCHAR Mbox[12];
};

struct MEGARAID_DCOMD_IOCTL
{
	SRB_IO_CONTROL SrbIoCtrl;
	MEGARAID_DCOMD Mpt;
	UCHAR          SenseBuf[120];
	UCHAR          DataBuf[4096];
};

struct MEGARAID_PHYSICAL_DRIVE_ADDRESS
{
	USHORT DeviceId;
	USHORT EnclDeviceId;
	UCHAR  EnclIndex;
	UCHAR  SlotNumber;
	UCHAR  ScsiDevType;
	UCHAR  ConnectPortBitmap;
	UINT64 SasAddr[2];
};

struct MEGARAID_PHYSICAL_DRIVE_LIST
{
	ULONG Size;
	ULONG Count;
	MEGARAID_PHYSICAL_DRIVE_ADDRESS Addr[MAX_SYS_PDS];
};
#pragma pack(pop)

#define MFI_CMD_PD_SCSI_IO        0x04
#define MFI_CMD_DCMD              0x05

#define MFI_STAT_OK               0x00

#define MFI_DCMD_PD_GET_LIST      0x02010000

#define MFI_MBOX_SIZE             12
#define MFI_FRAME_DIR_NONE        0x0000
#define MFI_FRAME_DIR_WRITE       0x0008
#define MFI_FRAME_DIR_READ        0x0010
#define MFI_FRAME_DIR_BOTH        0x0018


public:
	DWORD UpdateSmartInfo(DWORD index);
	BOOL UpdateIdInfo(DWORD index);
	BYTE GetAamValue(DWORD index);
	BYTE GetApmValue(DWORD index);
	BOOL EnableAam(DWORD index, BYTE param);
	BOOL EnableApm(DWORD index, BYTE param);
	BOOL DisableAam(DWORD index);
	BOOL DisableApm(DWORD index);
	BYTE GetRecommendAamValue(DWORD index);
	BYTE GetRecommendApmValue(DWORD index);

	VOID Init(BOOL useWmi, BOOL advancedDiskSearch, PBOOL flagChangeDisk, BOOL workaroundHD204UI, BOOL workaroundAdataSsd, BOOL flagHideNoSmartDisk);
	BOOL MeasuredTimeUnit();
	DWORD GetPowerOnHours(DWORD rawValue, DWORD timeUnitType);
	DWORD GetPowerOnHoursEx(DWORD index, DWORD timeUnitType);

	VOID SetAtaPassThroughSmart(BOOL flag);

	struct DISK_POSITION
	{
		INT					PhysicalDriveId;
		INT					ScsiPort;
		INT					ScsiTargetId;
		CSMI_SAS_PHY_ENTITY sasPhyEntity;
	};

	union IDENTIFY_DEVICE
	{
		ATA_IDENTIFY_DEVICE	 A;
		NVME_IDENTIFY_DEVICE N;
		BIN_IDENTIFY_DEVICE	 B;
	};

	struct ATA_SMART_INFO
	{
		IDENTIFY_DEVICE		IdentifyDevice;
		BYTE				SmartReadData[512];
		BYTE				SmartReadThreshold[512];
		SMART_ATTRIBUTE		Attribute[MAX_ATTRIBUTE];
		SMART_THRESHOLD		Threshold[MAX_ATTRIBUTE];

		BOOL				IsSmartEnabled;
		BOOL				IsIdInfoIncorrect;
		BOOL				IsSmartCorrect;
		BOOL				IsThresholdCorrect;
		BOOL				IsCheckSumError;
		BOOL				IsWord88;
		BOOL				IsWord64_76;
		BOOL				IsRawValues8;
		BOOL				IsRawValues7;
		BOOL				Is9126MB;
		BOOL				IsThresholdBug;

		BOOL				IsSmartSupported;
		BOOL				IsLba48Supported;
		BOOL				IsAamSupported;
		BOOL				IsApmSupported;
		BOOL				IsAamEnabled;
		BOOL				IsApmEnabled;
		BOOL				IsNcqSupported;
		BOOL				IsNvCacheSupported;
		BOOL				IsDeviceSleepSupported;
		BOOL				IsMaxtorMinute;
		BOOL				IsSsd;
		BOOL				IsTrimSupported;
		BOOL				IsVolatileWriteCachePresent;

		BOOL				IsNVMe;
		BOOL				IsUasp;

		INT					PhysicalDriveId;
		INT					ScsiPort;
		INT					ScsiTargetId;
		INT					ScsiBus;
		INT					SiliconImageType;
//		INT					AccessType;

		DWORD				TotalDiskSize;
		DWORD				Cylinder;
		DWORD				Head;
		DWORD				Sector;
		DWORD				Sector28;
		ULONGLONG			Sector48;
		ULONGLONG			NumberOfSectors;
		DWORD				DiskSizeChs;
		DWORD				DiskSizeLba28;
		DWORD				DiskSizeLba48;
		DWORD				LogicalSectorSize;
		DWORD				PhysicalSectorSize;
		DWORD				DiskSizeWmi;
		DWORD				BufferSize;
		ULONGLONG			NvCacheSize;
		DWORD				TransferModeType;
		DWORD				DetectedTimeUnitType;
		DWORD				MeasuredTimeUnitType;
		DWORD				AttributeCount;
		INT					DetectedPowerOnHours;
		INT					MeasuredPowerOnHours;
		INT					PowerOnRawValue;
		INT					PowerOnStartRawValue;
		DWORD				PowerOnCount;
		INT					Temperature;
		double				TemperatureMultiplier;
		DWORD				NominalMediaRotationRate;
//		double				Speed;
		INT					HostWrites;
		INT					HostReads;
		INT					GBytesErased;
		INT					NandWrites;
		INT					WearLevelingCount;

//		INT					PlextorNandWritesUnit;

		INT					Life;
		BOOL				FlagLifeRawValue;
		BOOL				FlagLifeRawValueIncrement;
		BOOL				FlagLifeSanDisk0_1;
		BOOL				FlagLifeSanDisk1;
		BOOL				FlagLifeSanDiskLenovo;

		DWORD				Major;
		DWORD				Minor;

		DWORD				DiskStatus;
		DWORD				DriveLetterMap;
		// 
		INT 				AlarmTemperature;
		BOOL				AlarmHealthStatus;

		INTERFACE_TYPE		InterfaceType;
		COMMAND_TYPE		CommandType;
		HOST_READS_WRITES_UNIT HostReadsWritesUnit;

		DWORD				DiskVendorId;
		DWORD				UsbVendorId;
		DWORD				UsbProductId;
		BYTE				Target;

		WORD				Threshold05;
		WORD				ThresholdC5;
		WORD				ThresholdC6;
		WORD				ThresholdFF;

		CSMI_SAS_PHY_ENTITY sasPhyEntity;

		CString				SerialNumber;
		CString				SerialNumberReverse;
		CString				FirmwareRev;
		CString				FirmwareRevReverse;
		CString				Model;
		CString				ModelReverse;
		CString				ModelWmi;
		CString				ModelSerial;
		CString				DriveMap;
		CString				MaxTransferMode;
		CString				CurrentTransferMode;
		CString				MajorVersion;
		CString				MinorVersion;
		CString				Interface;
		CString				Enclosure;
		CString				CommandTypeString;
		CString				SsdVendorString;
		CString				DeviceNominalFormFactor;
		CString				PnpDeviceId;

		CString				SmartKeyName;
	};

	struct EXTERNAL_DISK_INFO
	{
		CString Enclosure;
		DWORD	UsbVendorId;
		DWORD	UsbProductId;
	};

	CArray<ATA_SMART_INFO, ATA_SMART_INFO> vars;
	CArray<EXTERNAL_DISK_INFO, EXTERNAL_DISK_INFO> externals;

	CStringArray m_IdeController;
	CStringArray m_ScsiController;
	CStringArray m_UsbController;
	CString m_ControllerMap;
	CStringArray m_BlackIdeController;
	CStringArray m_BlackScsiController;
	CStringArray m_SiliconImageController;
	CStringArray m_UASPController;
	CArray<DWORD, DWORD> m_SiliconImageControllerType;
	CArray<INT, INT> m_BlackPhysicalDrive;

	BOOL IsAdvancedDiskSearch;
	BOOL IsEnabledWmi;

	BOOL IsWorkaroundHD204UI;
	BOOL IsWorkaroundAdataSsd;

	ULONGLONG MeasuredGetTickCount;

	BOOL FlagNvidiaController;
	BOOL FlagMarvellController;
	BOOL FlagUsbSat;
	BOOL FlagUsbSunplus;
	BOOL FlagUsbIodata;
	BOOL FlagUsbLogitec;
	BOOL FlagUsbProlific;
	BOOL FlagUsbJmicron;
	BOOL FlagUsbCypress;
	BOOL FlagUsbMemory;
	BOOL FlagUsbNVMeJMicron;
	BOOL FlagUsbNVMeASMedia;
	BOOL FlagUsbNVMeRealtek;
	BOOL FlagMegaRAID;
	BOOL FlagUsbASM1352R;

	DWORD CsmiType;

	DWORD CheckDiskStatus(DWORD index);

protected:
	OSVERSIONINFOEX m_Os;
	CString m_SerialNumberA_Z[26];
	BOOL m_bAtaPassThrough;
	BOOL m_bAtaPassThroughSmart;
	BOOL m_bNVMeStorageQuery;

	BOOL GetDiskInfo(INT physicalDriveId, INT scsiPort, INT scsiTargetId, INTERFACE_TYPE interfaceType, COMMAND_TYPE commandType, VENDOR_ID vendorId, DWORD productId = 0, INT scsiBus = -1, DWORD siliconImageId = 0, BOOL FlagNvidiaController = 0, BOOL FlagMarvellController = 0, CString pnpDeviceId = _T(""), BOOL FlagNVMe = FALSE, BOOL FlagUasp = FALSE);
	BOOL AddDisk(INT PhysicalDriveId, INT ScsiPort, INT scsiTargetId, INT scsiBus, BYTE target, COMMAND_TYPE commandType, IDENTIFY_DEVICE* identify, INT siliconImageType = -1, PCSMI_SAS_PHY_ENTITY sasPhyEntity = NULL, CString pnpDeviceId = _T(""));
	DWORD CheckSmartAttributeUpdate(DWORD index, SMART_ATTRIBUTE* pre, SMART_ATTRIBUTE* cur);

	BOOL CheckSmartAttributeCorrect(ATA_SMART_INFO* asi1, ATA_SMART_INFO* asi2);

	VOID WakeUp(INT physicalDriveId);
//	VOID InitAtaInfo();
//	VOID InitAtaInfoByWmi();
//	VOID InitStruct();
	VOID ChangeByteOrder(PCHAR str, DWORD length);
	BOOL CheckAsciiStringError(PCHAR str, DWORD length);
	HANDLE GetIoCtrlHandle(BYTE index);
	HANDLE GetIoCtrlHandle(INT scsiPort, DWORD siliconImageType);
	HANDLE GetIoCtrlHandleCsmi(INT scsiPort);
	HANDLE GetIoCtrlHandleMegaRAID(INT scsiPort);
	BOOL SendAtaCommand(DWORD i, BYTE main, BYTE sub, BYTE param);

	BOOL DoIdentifyDevicePd(INT physicalDriveId, BYTE target, IDENTIFY_DEVICE* identify);
	BOOL GetSmartAttributePd(INT physicalDriveId, BYTE target, ATA_SMART_INFO* asi);
	BOOL GetSmartThresholdPd(INT physicalDriveId, BYTE target, ATA_SMART_INFO* asi);
	BOOL ControlSmartStatusPd(INT physicalDriveId, BYTE target, BYTE command);
	BOOL SendAtaCommandPd(INT physicalDriveId, BYTE target, BYTE main, BYTE sub, BYTE param, PBYTE data, DWORD dataSize);
	BOOL ReadLogExtPd(INT physicalDriveId, BYTE target, BYTE logAddress, BYTE logPage, PBYTE data, DWORD dataSize);

	BOOL AddDiskNVMe(INT PhysicalDriveId, INT ScsiPort, INT scsiTargetId, INT scsiBus, BYTE target, COMMAND_TYPE commandType, IDENTIFY_DEVICE* identify, DWORD* diskSize = NULL, CString pnpDeviceId = _T(""));

	BOOL DoIdentifyDeviceNVMeJMicron(INT physicalDriveId, INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* identify);
	BOOL GetSmartAttributeNVMeJMicron(INT physicalDriveId, INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi);

	BOOL DoIdentifyDeviceNVMeASMedia(INT physicalDriveId, INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* identify);
	BOOL GetSmartAttributeNVMeASMedia(INT physicalDriveId, INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi);
	
	BOOL DoIdentifyDeviceNVMeRealtek(INT physicalDriveId, INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* data);
	BOOL GetSmartAttributeNVMeRealtek(INT physicalDriveId, INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi);

	BOOL DoIdentifyDeviceNVMeSamsung(INT physicalDriveId, INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* identify);
	BOOL GetSmartAttributeNVMeSamsung(INT physicalDriveId, INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi);
	BOOL GetSmartAttributeNVMeSamsung951(INT physicalDriveId, INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi);

	CString GetScsiPath(const TCHAR* Path);
	BOOL DoIdentifyDeviceNVMeIntel(INT physicalDriveId, INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* data);
	BOOL GetSmartAttributeNVMeIntel(INT physicalDriveId, INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi);

	BOOL GetScsiAddress(const TCHAR* Path, BYTE* PortNumber, BYTE* PathId, BYTE* TargetId, BYTE* Lun);
	BOOL DoIdentifyDeviceNVMeIntelRst(INT physicalDriveId, INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* data, DWORD* diskSize);
	BOOL GetSmartAttributeNVMeIntelRst(INT physicalDriveId, INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi);

	BOOL DoIdentifyDeviceNVMeStorageQuery(INT physicalDriveId, INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* data);
	BOOL GetSmartAttributeNVMeStorageQuery(INT physicalDriveId, INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi);

	BOOL DoIdentifyDeviceScsi(INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* identify);
	BOOL GetSmartAttributeScsi(INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi);
	BOOL GetSmartThresholdScsi(INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi);
	BOOL ControlSmartStatusScsi(INT scsiPort, INT scsiTargetId, BYTE command);
	BOOL SendAtaCommandScsi(INT scsiPort, INT scsiTargetId, BYTE main, BYTE sub, BYTE param);

	BOOL DoIdentifyDeviceSat(INT physicalDriveId, BYTE target, IDENTIFY_DEVICE* identify, COMMAND_TYPE commandType);
	BOOL GetSmartAttributeSat(INT physicalDriveId, BYTE target, ATA_SMART_INFO* asi);
	BOOL GetSmartThresholdSat(INT physicalDriveId, BYTE target, ATA_SMART_INFO* asi);
	BOOL ControlSmartStatusSat(INT physicalDriveId, BYTE target, BYTE command, COMMAND_TYPE commandType);
	BOOL SendAtaCommandSat(INT physicalDriveId, BYTE target, BYTE main, BYTE sub, BYTE param, COMMAND_TYPE commandType);
	BOOL ReadLogExtSat(INT physicalDriveId, BYTE target, BYTE logAddress, BYTE logPage, PBYTE data, DWORD dataSize, COMMAND_TYPE type);

	BOOL DoIdentifyDeviceSi(INT physicalDriveId, INT scsiPort, INT scsiBus, DWORD siliconImageId, IDENTIFY_DEVICE* identify);
	BOOL GetSmartAttributeSi(INT physicalDriveId, ATA_SMART_INFO* asi);
	BOOL GetSmartThresholdSi(INT physicalDriveId, ATA_SMART_INFO* asi);

	BOOL GetSmartAttributeWmi(ATA_SMART_INFO* asi);
	BOOL GetSmartThresholdWmi(ATA_SMART_INFO* asi);
	BOOL GetSmartInfoWmi(DWORD type, ATA_SMART_INFO* asi);

	BOOL AddDiskCsmi(INT scsiPort);
//	BOOL GetPhyInfo(INT scsiPort, CSMI_SAS_PHY_INFO & phyInfo);
	BOOL CsmiIoctl(HANDLE hHandle, UINT code, SRB_IO_CONTROL *csmiBuf, UINT csmiBufSize);
	BOOL DoIdentifyDeviceCsmi(INT scsiPort, PCSMI_SAS_PHY_ENTITY sasPhyEntity, IDENTIFY_DEVICE* identify);
	BOOL GetSmartAttributeCsmi(INT scsiPort, PCSMI_SAS_PHY_ENTITY sasPhyEntity, ATA_SMART_INFO* asi);
	BOOL GetSmartThresholdCsmi(INT scsiPort, PCSMI_SAS_PHY_ENTITY sasPhyEntity, ATA_SMART_INFO* asi);
	BOOL ControlSmartStatusCsmi(INT scsiPort, PCSMI_SAS_PHY_ENTITY sasPhyEntity, BYTE command);
	BOOL SendAtaCommandCsmi(INT scsiPort, PCSMI_SAS_PHY_ENTITY sasPhyEntity, BYTE main, BYTE sub, BYTE param, PBYTE data, DWORD dataSize);

	BOOL AddDiskMegaRAID(INT scsiPort);
	BOOL DoIdentifyDeviceMegaRAID(INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* identify);
	BOOL GetSmartAttributeMegaRAID(INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi);
	BOOL GetSmartThresholdMegaRAID(INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi);
	BOOL ControlSmartStatusMegaRAID(INT scsiPort, INT scsiTargetId, BYTE command);
	BOOL SendAtaCommandMegaRAID(INT scsiPort, INT scsiTargetId, BYTE main, BYTE sub, BYTE param);
	BOOL SendDCommandMegaRAID(HANDLE hHandle, ULONG opcode, void* buf, size_t bufsize, BYTE* mbox, size_t mboxlen);
	BOOL SendPassThroughCommandMegaRAID(INT scsiPort, INT scsiTargetId, void* buf, size_t bufsize, const UCHAR Cdb[], UCHAR CdbLength);

	DWORD GetTransferMode(WORD w63, WORD w76, WORD w77, WORD w88, CString &currentTransferMode, CString &maxTransferMode, CString &Interface, INTERFACE_TYPE *interfaceType);
	VOID GetTransferModePCIe(CString & current, CString & max, SlotMaxCurrSpeed slotspeed);
	DWORD GetTimeUnitType(CString model, CString firmware, DWORD major, DWORD transferMode);
	DWORD GetAtaMajorVersion(WORD w80, CString &majorVersion);
	VOID  GetAtaMinorVersion(WORD w81, CString &minor);
//	DWORD GetMaxtorPowerOnHours(DWORD currentValue, DWORD rawValue);

	BOOL FillSmartData(ATA_SMART_INFO* asi);
	BOOL FillSmartThreshold(ATA_SMART_INFO* asi);
	
	VOID CheckSsdSupport(ATA_SMART_INFO &asi);
	BOOL IsSsdOld(ATA_SMART_INFO &asi);
	BOOL IsSsdMtron(ATA_SMART_INFO &asi);
	BOOL IsSsdIndlinx(ATA_SMART_INFO &asi);
	BOOL IsSsdJMicron60x(ATA_SMART_INFO &asi);
	BOOL IsSsdJMicron61x(ATA_SMART_INFO &asi);
	BOOL IsSsdJMicron66x(ATA_SMART_INFO& asi);
	BOOL IsSsdIntel(ATA_SMART_INFO &asi);
	BOOL IsSsdIntelDc(ATA_SMART_INFO& asi);
	BOOL IsSsdSamsung(ATA_SMART_INFO &asi);
	BOOL IsSsdSandForce(ATA_SMART_INFO &asi);
	BOOL IsSsdMicronMU02(ATA_SMART_INFO &asi);
	BOOL IsSsdMicron(ATA_SMART_INFO &asi);
	BOOL IsSsdOcz(ATA_SMART_INFO &asi);
	BOOL IsSsdSsstc(ATA_SMART_INFO &asi);
	BOOL IsSsdPlextor(ATA_SMART_INFO& asi);
	BOOL IsSsdSanDisk(ATA_SMART_INFO &asi);
	BOOL IsSsdKingston(ATA_SMART_INFO &asi);
	BOOL IsSsdOczVector(ATA_SMART_INFO &asi);
	BOOL IsSsdToshiba(ATA_SMART_INFO &asi);
	BOOL IsSsdRealtek(ATA_SMART_INFO &asi);
	BOOL IsSsdSKhynix(ATA_SMART_INFO& asi);
	BOOL IsSsdKioxia(ATA_SMART_INFO& asi);
	BOOL IsSsdApacer(ATA_SMART_INFO& asi);
	BOOL IsSsdSiliconMotion(ATA_SMART_INFO& asi);
	BOOL IsSsdPhison(ATA_SMART_INFO& asi);
	BOOL IsSsdSeagate(ATA_SMART_INFO& asi);
	BOOL IsSsdMarvell(ATA_SMART_INFO& asi);
	BOOL IsSsdMaxiotek(ATA_SMART_INFO& asi);
	BOOL IsSsdGeneral(ATA_SMART_INFO& asi);

//	INT CheckPlextorNandWritesUnit(ATA_SMART_INFO &asi);

	static int Compare(const void *p1, const void *p2);

	CString GetModelSerial(CString &model, CString &serialNumber);
};
