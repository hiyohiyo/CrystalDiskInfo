/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef WINVER
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT              
#define _WIN32_WINNT 0x0501
#endif						

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif

#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard component
#include <afxext.h>         // Extended MFC

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC IE4 Common Control support
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC Windows Common Control support
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxdhtml.h>			// HTML Dialogs

#include "DHtmlDialogEx.h"		// CDHtmlDialogEx by hiyohiyo
#include "DHtmlMainDialog.h"	// CDHtmlMainDialog by hiyohiyo

#include <comutil.h>

#include "CommonFx.h"
#include "UtilityFx.h"
#include "CompileOptions.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma warning(disable : 4996)

//------------------------------------------------
// Global Sttings
//------------------------------------------------

// Version Information
#define PROJECT_NAME			_T("CrystalDiskInfo")
#define PRODUCT_NAME			_T("CrystalDiskInfo")

#if KUREI_KEI_SUPPORT
#define PRODUCT_SHORT_NAME		_T("CDI 9 KKE")

#ifdef _M_ARM
#define PRODUCT_EDITION			_T("Kurei Kei Edition ARM32")
#elif _M_ARM64
#define PRODUCT_EDITION			_T("Kurei Kei Edition ARM64")
#elif _M_X64
#define PRODUCT_EDITION			_T("Kurei Kei Edition x64")
#else
#define PRODUCT_EDITION			_T("Kurei Kei Edition x86")
#endif
#elif SUISHO_SHIZUKU_SUPPORT
#define PRODUCT_SHORT_NAME		_T("CDI 9 SE")
#ifdef _M_ARM32
#define PRODUCT_EDITION			_T("Shizuku Edition ARM32")
#elif _M_ARM64
#define PRODUCT_EDITION			_T("Shizuku Edition ARM64")
#elif _M_X64
#define PRODUCT_EDITION			_T("Shizuku Edition x64")
#else
#define PRODUCT_EDITION			_T("Shizuku Edition x86")
#endif
#else
#define PRODUCT_SHORT_NAME		_T("CDI 9")
#ifdef _M_ARM
#define PRODUCT_EDITION			_T("ARM32")
#elif _M_ARM64
#define PRODUCT_EDITION			_T("ARM64")
#elif _M_X64
#define PRODUCT_EDITION			_T("x64")
#else
#define PRODUCT_EDITION			_T("x86")
#endif
#endif

#define PRODUCT_VERSION				L"9.0.0 RC3"
#define PRODUCT_RELEASE				L"2023/05/21"
#define PRODUCT_COPY_YEAR			L"2008-2023"
#define PRODUCT_LICENSE				L"MIT License"

#ifdef KUREI_KEI_SUPPORT
#define PRODUCT_COPYRIGHT_1         L"© 2008-2023 hiyohiyo"
#define PRODUCT_COPYRIGHT_2         L"© 2017-2023 Pronama LLC"
#elif SUISHO_SHIZUKU_SUPPORT
#define PRODUCT_COPYRIGHT_1         L"© 2008-2023 hiyohiyo"
#define PRODUCT_COPYRIGHT_2         L"© 2012-2023 kirino kasumu"
#else
#define PRODUCT_COPYRIGHT_1			L"© 2008-2023 hiyohiyo"
#define PRODUCT_COPYRIGHT_2			L""
#endif

#define DEFAULT_FONT_FACE_1			L"Segoe UI"
#define DEFAULT_FONT_FACE_2			L"Tahoma"

#define THEME_DIR					L"CdiResource\\themes\\"
#define LANGUAGE_DIR				L"CdiResource\\language\\"

#define MENU_THEME_INDEX			3
#define MENU_LANG_INDEX				6

#define DEFAULT_THEME				L"Default"
#define DEFAULT_LANGUAGE			L"English"

#define TIMER_UPDATE_DIALOG			500

#define URL_CRYSTAL_DEW_WORLD_JA	L"https://crystalmark.info/ja/"
#define URL_CRYSTAL_DEW_WORLD_EN 	L"https://crystalmark.info/en/"

#define	URL_PROJECT_SHIZUKU_JA		L"https://suishoshizuku.com/ja/"
#define	URL_PROJECT_SHIZUKU_EN		L"https://suishoshizuku.com/en/"

#define URL_PRONAMA	                L"https://pronama.jp/2017/08/04/crystaldiskinfo-kurei-kei-edition-release/"

#ifdef MAX_DRIVE_20
#define MAX_DRIVE_MENU              20
#else
#define MAX_DRIVE_MENU              16
#endif

#define CERTNAME_JMS56X				L"Open Source Developer, Noriyuki Miyazaki"
#define CERTNAME_JMB39X				L"Open Source Developer, Noriyuki Miyazaki"


#ifdef SUISHO_SHIZUKU_SUPPORT

#ifdef KUREI_KEI_SUPPORT
#define	URL_PROJECT_SITE_1		    L"https://pronama.jp/crystaldiskinfo"
#define URL_PROJECT_SITE_2		    L"https://pronama.jp/ixy"
#define URL_PROJECT_SITE_3	        L"https://pronama.jp/uesakasumire"
#define URL_PROJECT_SITE_4			L"https://pronama.jp/crystaldiskinfo"
#define URL_PROJECT_SITE_5			L""
#else
#define	URL_PROJECT_SITE_1		    L"https://twitter.com/kirinokasumu"
#define URL_PROJECT_SITE_2		    L"https://linux-ha.osdn.jp/wp/"
#define URL_PROJECT_SITE_3	        L"https://ch.nicovideo.jp/oss"
#define URL_PROJECT_SITE_4			L"https://twitter.com/bellche"
#define URL_PROJECT_SITE_5			L"https://suishoshizuku.com/"
#endif

#endif

#define	URL_VERSION_JA				L"https://crystalmark.info/ja/software/crystaldiskinfo/crystaldiskinfo-history/"
#define	URL_VERSION_EN				L"https://crystalmark.info/en/software/crystaldiskinfo/crystaldiskinfo-history/"
#define	URL_LICENSE_JA				L"https://crystalmark.info/ja/software/crystaldiskinfo/crystaldiskinfo-license/"
#define	URL_LICENSE_EN				L"https://crystalmark.info/en/software/crystaldiskinfo/crystaldiskinfo-license/"

#define URL_HTML_HELP_JA			L"https://crystalmark.info/ja/software/crystaldiskinfo/"
#define URL_HTML_HELP_EN 			L"https://crystalmark.info/en/software/crystaldiskinfo/"

#define URL_AMD_RC2T7_JA			L"https://thilmera.com/project/AMD_RC2t7/?lang=ja"
#define URL_AMD_RC2T7_EN 			L"https://thilmera.com/project/AMD_RC2t7/?lang=en"

// Command
static const int TRAY_TEMPERATURE_ICON_BASE = WM_APP + 0x1200;
static const int SELECT_DISK_BASE = WM_APP + 0x1300;
static const int AUTO_REFRESH_TARGET_BASE = WM_APP + 0x1400;
static const int SHOW_GRAPH_BASE = WM_APP + 0x1500;
static const int ALARM_SETTING_HEALTH_STATUS_BASE = WM_APP + 0x1900;
static const int ALARM_SETTING_TEMPERATURE_BASE = WM_APP + 0x2000; // Main Only
static const int GRAPH_DISK_INDEX = WM_APP + 0x2000; // Graph Only

static const int RE_EXEC = 5963;