/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#ifndef __GET_OS_INFO_H__
#define __GET_OS_INFO_H__

void GetOsName(CString& OsFullName);
BOOL IsX64();
BOOL IsIa64();
BOOL IsArm32();
BOOL IsArm64();
BOOL IsWow64();
BOOL IsSidebar();
BOOL IsClassicSystem();
BOOL IsIe556();
BOOL IsDotNet2();
BOOL IsDotNet4();
DWORD GetIeVersion();

#define PRODUCT_UNLICENSED                       0xABCDABCD
#define PRODUCT_BUSINESS                         0x00000006 // Business Edition
#define PRODUCT_BUSINESS_N                       0x00000010 // Business Edition
#define PRODUCT_CLUSTER_SERVER                   0x00000012 // Cluster Server Edition
#define PRODUCT_DATACENTER_SERVER                0x00000008 // Server Datacenter Edition (full installation)
#define PRODUCT_DATACENTER_SERVER_CORE           0x0000000C // Server Datacenter Edition (core installation)
#define PRODUCT_DATACENTER_SERVER_CORE_V         0x00000027 // Server Datacenter Edition without Hyper-V (core installation)
#define PRODUCT_DATACENTER_SERVER_V              0x00000025 // Server Datacenter Edition without Hyper-V (full installation)
#define PRODUCT_ENTERPRISE                       0x00000004 // Enterprise Edition
#define PRODUCT_ENTERPRISE_N                     0x0000001B // Enterprise Edition
#define PRODUCT_ENTERPRISE_SERVER                0x0000000A // Server Enterprise Edition (full installation)
#define PRODUCT_ENTERPRISE_SERVER_CORE           0x0000000E // Server Enterprise Edition (core installation)
#define PRODUCT_ENTERPRISE_SERVER_CORE_V         0x00000029 // Server Enterprise Edition without Hyper-V (core installation)
#define PRODUCT_ENTERPRISE_SERVER_IA64           0x0000000F // Server Enterprise Edition for Itanium-based Systems
#define PRODUCT_ENTERPRISE_SERVER_V              0x00000026 // Server Enterprise Edition without Hyper-V (full installation)

#define PRODUCT_HOME_BASIC                       0x00000002 // Home Basic Edition
#define PRODUCT_HOME_BASIC_N                     0x00000005 // Home Basic Edition
#define PRODUCT_HOME_PREMIUM                     0x00000003 // Home Premium Edition
#define PRODUCT_HOME_PREMIUM_N                   0x0000001A // Home Premium Edition
#define PRODUCT_HOME_SERVER                      0x00000013 // Home Server Edition
#define PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT 0x0000001E // Windows Essential Business Server Management Server
#define PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING  0x00000020 // Windows Essential Business Server Messaging Server
#define PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY   0x0000001F // Windows Essential Business Server Security Server
#define PRODUCT_SERVER_FOR_SMALLBUSINESS         0x00000018 // Server for Small Business Edition
#define PRODUCT_SMALLBUSINESS_SERVER             0x00000009 // Small Business Server
#define PRODUCT_SMALLBUSINESS_SERVER_PREMIUM     0x00000019 // Small Business Server Premium Edition
#define PRODUCT_STANDARD_SERVER                  0x00000007 // Server Standard Edition (full installation)
#define PRODUCT_STANDARD_SERVER_CORE             0x0000000D // Server Standard Edition (core installation)
#define PRODUCT_STANDARD_SERVER_CORE_V           0x00000028 // Server Standard Edition without Hyper-V (core installation)
#define PRODUCT_STANDARD_SERVER_V                0x00000024 // Server Standard Edition without Hyper-V (full installation)
#define PRODUCT_STARTER                          0x0000000B // Starter Edition
#define PRODUCT_STORAGE_ENTERPRISE_SERVER        0x00000017 // Storage Server Enterprise Edition
#define PRODUCT_STORAGE_EXPRESS_SERVER           0x00000014 // Storage Server Express Edition
#define PRODUCT_STORAGE_STANDARD_SERVER          0x00000015 // Storage Server Standard Edition
#define PRODUCT_STORAGE_WORKGROUP_SERVER         0x00000016 // Storage Server Workgroup Edition
#define PRODUCT_UNDEFINED                        0x00000000 // An unknown product
#define PRODUCT_ULTIMATE                         0x00000001 // Ultimate Edition
#define PRODUCT_ULTIMATE_N                       0x0000001C // Ultimate Edition
#define PRODUCT_WEB_SERVER                       0x00000011 // Web Server Edition (full installation)
#define PRODUCT_WEB_SERVER_CORE                  0x0000001D // Web Server Edition (core installation)
#define PRODUCT_PROFESSIONAL					 0x00000030 // Professional
#define PRODUCT_PROFESSIONAL_N					 0x00000031 // Professional N

#define PRODUCT_CORE							 0x00000065 // Windows 8
#define PRODUCT_CORE_N                           0x00000062 // Windows 8 N
#define PRODUCT_CORE_COUNTRYSPECIFIC			 0x00000063 // Windows 8 China
#define PRODUCT_CORE_SINGLELANGUAGE				 0x00000064 // Windows 8 Single Language

#define RRODUCT_ESSENTIALBUSINESS_SERVER_MGMT	 0x0000003B // Windows Essential Server Solution Management
#define PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL	 0x0000003C // Windows Essential Server Solution Additional
#define PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC 0x0000003D // Windows Essential Server Solution Management SVC
#define PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC 0x0000003E // Windows Essential Server Solution Additional SVC



#define SM_TABLETPC             86
#define SM_MEDIACENTER          87
#define SM_STARTER              88
#define SM_SERVERR2             89

#endif
