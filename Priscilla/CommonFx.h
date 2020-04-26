/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once

//------------------------------------------------
// Naming Conventions
//------------------------------------------------
// BOOL              bXxxxYyyy
// HANDLE            hXxxxYyyy
// Pointer           pXxxxYyyy
// Function          SampleFunction
// Variable          sampleVariable
// Const Value       ConstVaiable
// Member Variable   m_XxxxYyyy

//------------------------------------------------
// Order for C*****Fx Control
//------------------------------------------------
// Control > Draw Control > Image > Font > Mouse > ToolTip
// 

//------------------------------------------------
// Utility Macros
//------------------------------------------------

#define SAFE_DELETE(p) {if(p){delete (p);(p)=NULL;}}

//------------------------------------------------
// WM_APP
//------------------------------------------------
// WM_APP + 0x0000-0x0FFF: User Application
// WM_APP + 0x1000-0x1FFF: Project Priscilla
// WM_APP + 0x2000-0x2FFF: Themes
// WM_APP + 0x3000-0x3FFF: Languages

#define WM_UPDATE_DIALOG_SIZE	(WM_APP + 0x1000)
#define WM_THEME_ID				(WM_APP + 0x2000)
#define WM_LANGUAGE_ID			(WM_APP + 0x3000)

//------------------------------------------------
// TIMER ID
//------------------------------------------------

static const int TimerUpdateDialogSize = 0x2001;

//------------------------------------------------
// Const Values
//------------------------------------------------

static const int ControlImageNormal   = 0x0000;
static const int ControlImageHover    = 0x0001;
static const int ControlImageFocus    = 0x0002;
static const int ControlImageSelected = 0x0003;
static const int ControlImageDisabled = 0x0004;

static const int HighContrast         = 0x0001;
static const int SystemDraw           = 0x0002;
static const int OwnerDrawImage       = 0x0004;
static const int OwnerDrawGlass       = 0x0008;
static const int OwnerDrawTransparent = 0x0010;

static const int ZoomTypeAuto =  0;
static const int ZoomType100 = 100;
static const int ZoomType125 = 125;
static const int ZoomType150 = 150;
static const int ZoomType200 = 200;
static const int ZoomType250 = 250;
static const int ZoomType300 = 300;