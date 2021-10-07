/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include <afxpriv.h>
#include "stdafx.h"
#include "AtaSmart.h"
#include "DiskInfo.h"
#include "GraphDlg.h"
#include "DiskInfoDlg.h"

#include "OsInfoFx.h"
#include "AtaSmart.h"

#define SMART_TEMPERATURE                  300
#define SMART_POWER_ON_HOURS               301
#define SMART_POWER_ON_COUNT               302
#define SMART_REALLOCATED_SECTORS_COUNT    303
#define SMART_REALLOCATED_EVENT_COUNT      304
#define SMART_CURRENT_PENDING_SECTOR_COUNT 305
#define SMART_UNCORRECTABLE_SECTOR_COUNT   306
#define SMART_HOST_WRITES                  307
#define SMART_HOST_READS                   308
#define SMART_NAND_WRITES                  309
#define SMART_NAND_ERASED                  310
#define SMART_WEAR_LEVELING_COUNT          311
#define SMART_LIFE                         312
#define NAND_WAF                           400

IMPLEMENT_DYNCREATE(CGraphDlg, CDHtmlDialog)

CGraphDlg::CGraphDlg(CWnd* pParent /*=NULL*/, int defaultDisk)
	: CDHtmlMainDialog(CGraphDlg::IDD, CGraphDlg::IDH,
	((CDiskInfoApp*)AfxGetApp())->m_ThemeDir,
	((CDiskInfoApp*)AfxGetApp())->m_ThemeIndex,
	((CDiskInfoApp*)AfxGetApp())->m_LangDir,
	((CDiskInfoApp*)AfxGetApp())->m_LangIndex,
	pParent)
{
	CString cstr;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hIconMini = AfxGetApp()->LoadIcon(IDI_MINI_ICON);
	m_SmartDir = ((CDiskInfoApp*)AfxGetApp())->m_SmartDir;
	_tcscpy_s(m_Ini, MAX_PATH, ((CDiskInfoApp*)AfxGetApp())->m_Ini.GetString());
	m_bSmartEnglish = (BOOL)GetPrivateProfileInt(_T("Setting"), _T("SmartEnglish"), 0, m_Ini);

	InitVars(defaultDisk);

	m_ZoomType = GetZoomType();
}

CGraphDlg::~CGraphDlg()
{
	CString index, value;
	for(int i = 0; i < m_DetectedDisk; i++)
	{
		index.Format(_T("Disk%d"), i);
		value.Format(_T("%d"), ! m_bGraph[i]);
		WritePrivateProfileString(_T("GraphHideDisk"), index, value, m_Ini);
	}
}

void CGraphDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlMainDialog::DoDataExchange(pDX);

	DDX_DHtml_ElementInnerHtml(pDX, _T("Title"),  m_Title);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk0"),  m_LiDisk[0]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk1"),  m_LiDisk[1]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk2"),  m_LiDisk[2]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk3"),  m_LiDisk[3]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk4"),  m_LiDisk[4]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk5"),  m_LiDisk[5]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk6"),  m_LiDisk[6]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk7"),  m_LiDisk[7]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk8"),  m_LiDisk[8]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk9"),  m_LiDisk[9]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk10"), m_LiDisk[10]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk11"), m_LiDisk[11]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk12"), m_LiDisk[12]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk13"), m_LiDisk[13]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk14"), m_LiDisk[14]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk15"), m_LiDisk[15]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk16"), m_LiDisk[16]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk17"), m_LiDisk[17]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk18"), m_LiDisk[18]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk19"), m_LiDisk[19]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk20"), m_LiDisk[20]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk21"), m_LiDisk[21]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk22"), m_LiDisk[22]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk23"), m_LiDisk[23]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk24"), m_LiDisk[24]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk25"), m_LiDisk[25]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk26"), m_LiDisk[26]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk27"), m_LiDisk[27]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk28"), m_LiDisk[28]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk29"), m_LiDisk[29]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk30"), m_LiDisk[30]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk31"), m_LiDisk[31]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk32"), m_LiDisk[32]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk33"), m_LiDisk[33]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk34"), m_LiDisk[34]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk35"), m_LiDisk[35]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk36"), m_LiDisk[36]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk37"), m_LiDisk[37]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk38"), m_LiDisk[38]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk39"), m_LiDisk[39]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk40"), m_LiDisk[40]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk41"), m_LiDisk[41]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk42"), m_LiDisk[42]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk43"), m_LiDisk[43]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk44"), m_LiDisk[44]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk45"), m_LiDisk[45]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk46"), m_LiDisk[46]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk47"), m_LiDisk[47]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk48"), m_LiDisk[48]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk49"), m_LiDisk[49]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk50"), m_LiDisk[50]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk51"), m_LiDisk[51]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk52"), m_LiDisk[52]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk53"), m_LiDisk[53]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk54"), m_LiDisk[54]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk55"), m_LiDisk[55]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk56"), m_LiDisk[56]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk57"), m_LiDisk[57]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk58"), m_LiDisk[58]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk59"), m_LiDisk[59]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk60"), m_LiDisk[60]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk61"), m_LiDisk[61]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk62"), m_LiDisk[62]);
	DDX_DHtml_ElementInnerHtml(pDX, _T("Disk63"), m_LiDisk[63]);

	DDX_DHtml_SelectValue(pDX, _T("SelectAttributeId"), m_SelectAttributeId);
	DDX_DHtml_SelectIndex(pDX, _T("SelectAttributeId"), m_SelectAttributeIdCtrl);
}

BOOL CGraphDlg::OnInitDialog()
{
	CDHtmlMainDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIconMini, FALSE);	// Set small icon

	EnableDpiAware();

	InitThemeLang();
	InitMenu();

	SetWindowText(_T("CrystalDiskInfo - ") + i18n(_T("WindowTitle"), _T("GRAPH")) + _T(" - Powered by Flot"));

	m_IeVersion = GetIeVersion();

	InitDialogEx(m_SizeX, m_SizeY, ((CDiskInfoApp*)AfxGetApp())->m_GraphDlgPath);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CGraphDlg, CDHtmlMainDialog)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXIT, &CGraphDlg::OnExit)
	ON_COMMAND(ID_NORTH_WEST, &CGraphDlg::OnNorthWest)
	ON_COMMAND(ID_NORTH_EAST, &CGraphDlg::OnNorthEast)
	ON_COMMAND(ID_SOUTH_WEST, &CGraphDlg::OnSouthWest)
	ON_COMMAND(ID_SOUTH_EAST, &CGraphDlg::OnSouthEast)
	ON_COMMAND(ID_PAINT_WEEKEND, &CGraphDlg::OnPaintWeekend)
	ON_COMMAND(ID_POINT_100, &CGraphDlg::OnPoint100)
	ON_COMMAND(ID_POINT_200, &CGraphDlg::OnPoint200)
	ON_COMMAND(ID_POINT_300, &CGraphDlg::OnPoint300)
	ON_COMMAND(ID_POINT_400, &CGraphDlg::OnPoint400)
	ON_COMMAND(ID_POINT_500, &CGraphDlg::OnPoint500)
	ON_COMMAND(ID_POINT_600, &CGraphDlg::OnPoint600)
	ON_COMMAND(ID_POINT_700, &CGraphDlg::OnPoint700)
	ON_COMMAND(ID_POINT_800, &CGraphDlg::OnPoint800)
	ON_COMMAND(ID_POINT_900, &CGraphDlg::OnPoint900)
	ON_COMMAND(ID_POINT_1000, &CGraphDlg::OnPoint1000)
	ON_COMMAND(ID_POINT_2000, &CGraphDlg::OnPoint2000)
	ON_COMMAND(ID_POINT_3000, &CGraphDlg::OnPoint3000)
	ON_COMMAND(ID_POINT_4000, &CGraphDlg::OnPoint4000)
	ON_COMMAND(ID_POINT_5000, &CGraphDlg::OnPoint5000)
	ON_COMMAND(ID_POINT_ALL, &CGraphDlg::OnPointAll)
	ON_COMMAND(ID_CUSTOMIZE, &CGraphDlg::OnCustomize)
	ON_MESSAGE(MY_UPDATE_BG_IMAGE, OnUpdateBkImage)
	ON_MESSAGE(MY_UPDATE_LINE_COLOR, OnUpdateLineColor)
	ON_COMMAND(ID_MDHM, &CGraphDlg::OnMdhm)
	ON_COMMAND(ID_MD, &CGraphDlg::OnMd)
	ON_COMMAND(ID_YMDHM, &CGraphDlg::OnYmdhm)
	ON_COMMAND(ID_YMD, &CGraphDlg::OnYmd)
	ON_COMMAND(ID_DMYHM, &CGraphDlg::OnDmyhm)
	ON_COMMAND(ID_DMY, &CGraphDlg::OnDmy)
	ON_COMMAND(ID_DMYHM2, &CGraphDlg::OnDmyhm2)
	ON_COMMAND(ID_DMY2, &CGraphDlg::OnDmy2)
	ON_COMMAND(ID_HDD, &CGraphDlg::OnHdd)
	ON_COMMAND(ID_SSD, &CGraphDlg::OnSsd)
	ON_COMMAND(ID_SSD_MTRON, &CGraphDlg::OnSsdMtron)
	ON_COMMAND(ID_SSD_INDILINX, &CGraphDlg::OnSsdIndilinx)
	ON_COMMAND(ID_SSD_JMICRON, &CGraphDlg::OnSsdJmicron)
	ON_COMMAND(ID_SSD_INTEL, &CGraphDlg::OnSsdIntel)
	ON_COMMAND(ID_SSD_SAMSUNG, &CGraphDlg::OnSsdSamsung)
	ON_COMMAND(ID_SSD_SANDFORCE, &CGraphDlg::OnSsdSandforce)
	ON_COMMAND(ID_SSD_MICRON, &CGraphDlg::OnSsdMicron)
	ON_COMMAND(ID_SSD_OCZ, &CGraphDlg::OnSsdOcz)
	ON_COMMAND(ID_SSD_PLEXTOR, &CGraphDlg::OnSsdPlextor)
	ON_COMMAND(ID_SSD_SANDISK, &CGraphDlg::OnSsdSanDisk)
	ON_COMMAND(ID_SSD_OCZ_VECTOR, &CGraphDlg::OnSsdOczVector)
	ON_COMMAND(ID_SSD_REALTEK, &CGraphDlg::OnSsdRealtek)
	ON_COMMAND(ID_SSD_NVME, &CGraphDlg::OnSsdNVMe)

END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CGraphDlg)
	DHTML_EVENT_ONCLICK(_T("SelectAttributeId"), OnSelectAttributeId)
	DHTML_EVENT_ONCLICK(_T("AllOn"), OnAllOn)
	DHTML_EVENT_ONCLICK(_T("AllOff"), OnAllOff)
	DHTML_EVENT_ONCLICK(_T("Reset"), OnReset)

	DHTML_EVENT_ONCLICK(_T("Disk0"),  OnDisk0)
	DHTML_EVENT_ONCLICK(_T("Disk1"),  OnDisk1)
	DHTML_EVENT_ONCLICK(_T("Disk2"),  OnDisk2)
	DHTML_EVENT_ONCLICK(_T("Disk3"),  OnDisk3)
	DHTML_EVENT_ONCLICK(_T("Disk4"),  OnDisk4)
	DHTML_EVENT_ONCLICK(_T("Disk5"),  OnDisk5)
	DHTML_EVENT_ONCLICK(_T("Disk6"),  OnDisk6)
	DHTML_EVENT_ONCLICK(_T("Disk7"),  OnDisk7)
	DHTML_EVENT_ONCLICK(_T("Disk8"),  OnDisk8)
	DHTML_EVENT_ONCLICK(_T("Disk9"),  OnDisk9)
	DHTML_EVENT_ONCLICK(_T("Disk10"), OnDisk10)
	DHTML_EVENT_ONCLICK(_T("Disk11"), OnDisk11)
	DHTML_EVENT_ONCLICK(_T("Disk12"), OnDisk12)
	DHTML_EVENT_ONCLICK(_T("Disk13"), OnDisk13)
	DHTML_EVENT_ONCLICK(_T("Disk14"), OnDisk14)
	DHTML_EVENT_ONCLICK(_T("Disk15"), OnDisk15)
	DHTML_EVENT_ONCLICK(_T("Disk16"), OnDisk16)
	DHTML_EVENT_ONCLICK(_T("Disk17"), OnDisk17)
	DHTML_EVENT_ONCLICK(_T("Disk18"), OnDisk18)
	DHTML_EVENT_ONCLICK(_T("Disk19"), OnDisk19)
	DHTML_EVENT_ONCLICK(_T("Disk20"), OnDisk20)
	DHTML_EVENT_ONCLICK(_T("Disk21"), OnDisk21)
	DHTML_EVENT_ONCLICK(_T("Disk22"), OnDisk22)
	DHTML_EVENT_ONCLICK(_T("Disk23"), OnDisk23)
	DHTML_EVENT_ONCLICK(_T("Disk24"), OnDisk24)
	DHTML_EVENT_ONCLICK(_T("Disk25"), OnDisk25)
	DHTML_EVENT_ONCLICK(_T("Disk26"), OnDisk26)
	DHTML_EVENT_ONCLICK(_T("Disk27"), OnDisk27)
	DHTML_EVENT_ONCLICK(_T("Disk28"), OnDisk28)
	DHTML_EVENT_ONCLICK(_T("Disk29"), OnDisk29)
	DHTML_EVENT_ONCLICK(_T("Disk30"), OnDisk30)
	DHTML_EVENT_ONCLICK(_T("Disk31"), OnDisk31)
	DHTML_EVENT_ONCLICK(_T("Disk32"), OnDisk32)
	DHTML_EVENT_ONCLICK(_T("Disk33"), OnDisk33)
	DHTML_EVENT_ONCLICK(_T("Disk34"), OnDisk34)
	DHTML_EVENT_ONCLICK(_T("Disk35"), OnDisk35)
	DHTML_EVENT_ONCLICK(_T("Disk36"), OnDisk36)
	DHTML_EVENT_ONCLICK(_T("Disk37"), OnDisk37)
	DHTML_EVENT_ONCLICK(_T("Disk38"), OnDisk38)
	DHTML_EVENT_ONCLICK(_T("Disk39"), OnDisk39)
	DHTML_EVENT_ONCLICK(_T("Disk40"), OnDisk40)
	DHTML_EVENT_ONCLICK(_T("Disk41"), OnDisk41)
	DHTML_EVENT_ONCLICK(_T("Disk42"), OnDisk42)
	DHTML_EVENT_ONCLICK(_T("Disk43"), OnDisk43)
	DHTML_EVENT_ONCLICK(_T("Disk44"), OnDisk44)
	DHTML_EVENT_ONCLICK(_T("Disk45"), OnDisk45)
	DHTML_EVENT_ONCLICK(_T("Disk46"), OnDisk46)
	DHTML_EVENT_ONCLICK(_T("Disk47"), OnDisk47)
	DHTML_EVENT_ONCLICK(_T("Disk48"), OnDisk48)
	DHTML_EVENT_ONCLICK(_T("Disk49"), OnDisk49)
	DHTML_EVENT_ONCLICK(_T("Disk50"), OnDisk50)
	DHTML_EVENT_ONCLICK(_T("Disk51"), OnDisk51)
	DHTML_EVENT_ONCLICK(_T("Disk52"), OnDisk52)
	DHTML_EVENT_ONCLICK(_T("Disk53"), OnDisk53)
	DHTML_EVENT_ONCLICK(_T("Disk54"), OnDisk54)
	DHTML_EVENT_ONCLICK(_T("Disk55"), OnDisk55)
	DHTML_EVENT_ONCLICK(_T("Disk56"), OnDisk56)
	DHTML_EVENT_ONCLICK(_T("Disk57"), OnDisk57)
	DHTML_EVENT_ONCLICK(_T("Disk58"), OnDisk58)
	DHTML_EVENT_ONCLICK(_T("Disk59"), OnDisk59)
	DHTML_EVENT_ONCLICK(_T("Disk60"), OnDisk60)
	DHTML_EVENT_ONCLICK(_T("Disk61"), OnDisk61)
	DHTML_EVENT_ONCLICK(_T("Disk62"), OnDisk62)
	DHTML_EVENT_ONCLICK(_T("Disk63"), OnDisk63)
END_DHTML_EVENT_MAP()

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGraphDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDHtmlMainDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGraphDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGraphDlg::InitDialogComplete()
{
	DebugPrint(_T("InitDialogComplete"));
	static BOOL once = FALSE;
	if(! once)
	{
		m_bShowWindow = TRUE;

		InitMenuBar();
		UpdateBkImage();

		for(int i = 0; i < m_DetectedDisk; i++)
		{
			if(m_DefaultDisk[i])
			{
				CheckDisk(i);
			}
		}
		m_bInitializing = FALSE;

		UpdateData(TRUE);
		m_AttributeId = _tstoi(m_SelectAttributeId);

		UpdateDialogSize();
		UpdateGraph();
		CenterWindow();
		ShowWindow(SW_SHOW);
		once = TRUE;
	}
}

void CGraphDlg::InitVars(int defaultDisk)
{
	TCHAR str[256];
	CString cstr;
	int temp;

	GetPrivateProfileString(_T("EXCHANGE"), _T("DetectedDisk"), _T("-1"), str, 256, m_SmartDir + EXCHANGE_INI);
	temp = _tstoi(str);
	if(0 < temp && temp <= CAtaSmart::MAX_DISK)
	{
		m_DetectedDisk = temp;
	}
	else
	{
		m_DetectedDisk = 0;
	}

	for(int i = 0; i < CAtaSmart::MAX_DISK; i++)
	{
		m_bGraph[i] = FALSE;
		m_DefaultDisk[i] = FALSE;
	}

	if(0 <= defaultDisk && defaultDisk < m_DetectedDisk)
	{
		m_DefaultDisk[defaultDisk] = TRUE;
	}
	else
	{
		for(int i = 0; i < m_DetectedDisk; i++)
		{
			cstr.Format(_T("Disk%d"), i);
			if(GetPrivateProfileInt(_T("GraphHideDisk"), cstr, 0, m_Ini) == 0)
			{
				m_DefaultDisk[i] = TRUE;
			}
		}
	}

	for(int i = 0; i < m_DetectedDisk; i++)
	{
		cstr.Format(_T("%d"), i);
		GetPrivateProfileString(_T("MODEL"), cstr, _T(""), str, 256, m_SmartDir + EXCHANGE_INI);
		m_Model[i] = str;
		m_ModelEscape[i] = str;
		m_ModelEscape[i].Replace(_T("\""), _T("\\\""));
		GetPrivateProfileString(_T("SERIAL"), cstr, _T(""), str, 256, m_SmartDir + EXCHANGE_INI);
		m_Serial[i] = str;
		GetPrivateProfileString(_T("DRIVE"), cstr, _T(""), str, 256, m_SmartDir + EXCHANGE_INI);
		m_Drive[i] = str;
	}

	// LegendPosition
	GetPrivateProfileString(_T("Setting"), _T("LegendPosition"), _T("sw"), str, 256, m_Ini);
	cstr = str;
	if(cstr.Compare(_T("nw")) == 0 || cstr.Compare(_T("ne")) == 0 || cstr.Compare(_T("sw")) == 0 || cstr.Compare(_T("se")) == 0)
	{
		m_LegendPositon = str;
	}
	else
	{
		m_LegendPositon = _T("sw");
	}

	// Date Style
	GetPrivateProfileString(_T("Setting"), _T("TimeFormat"), _T("mdhm"), str, 256, m_Ini);
	cstr = str;
	if(cstr.Compare(_T("%m/%d %H:%M")) == 0
	|| cstr.Compare(_T("%m/%d")) == 0
	|| cstr.Compare(_T("%y/%m/%d %H:%M")) == 0
	|| cstr.Compare(_T("%y/%m/%d")) == 0
	|| cstr.Compare(_T("%d/%m/%y")) == 0
	|| cstr.Compare(_T("%d/%m/%y %H:%M")) == 0
	|| cstr.Compare(_T("%d.%m.%y")) == 0
	|| cstr.Compare(_T("%d.%m.%y %H:%M")) == 0)
	{
		m_TimeFormat = str;
	}
	else
	{
		m_TimeFormat = _T("%m/%d %H:%M");
	}

	// Paint Weekend
	GetPrivateProfileString(_T("Setting"), _T("PaintWeekend"), _T("0"), str, 256, m_Ini);
	if(_tstoi(str) > 0)
	{
		m_bPaintWeekend = TRUE;
	}
	else
	{
		m_bPaintWeekend = FALSE;
	}

	GetPrivateProfileString(_T("Setting"), _T("MaxPlotPoint"), _T("100"), str, 256, m_Ini);
	if(_tstoi(str) > 0)
	{
		m_MaxPlotPoint = _tstoi(str);
	}
	else
	{
		m_MaxPlotPoint = 0;
	}

	GetPrivateProfileString(_T("Setting"), _T("Attribute"), _T("0"), str, 256, m_Ini);
	if(_tstoi(str) > 0)
	{
		m_Attribute = _tstoi(str);
	}
	else
	{
		m_Attribute = 0;
	}

	// Graph Color
	m_DefaultLineColor[0] = 0xff9797;
	m_DefaultLineColor[1] = 0x40c2ed;
	m_DefaultLineColor[2] = 0x4b4bcb;
	m_DefaultLineColor[3] = 0xc6ac8c;
	m_DefaultLineColor[4] = 0x4da74d;
	m_DefaultLineColor[5] = 0xaa1c4a;
	m_DefaultLineColor[6] = 0x1191bb;
	m_DefaultLineColor[7] = 0x47189e;
	m_DefaultLineColor[8] = 0xc17453;
	m_DefaultLineColor[9] = 0x1a7612;
	m_DefaultLineColor[10] = 0xff6c35;
	m_DefaultLineColor[11] = 0x24676f;
	m_DefaultLineColor[12] = 0x0b29a4;
	m_DefaultLineColor[13] = 0xd2958c;
	m_DefaultLineColor[14] = 0x575700;
	m_DefaultLineColor[15] = 0x808080;
	m_DefaultLineColor[16] = 0x787878;
	m_DefaultLineColor[17] = 0x707070;
	m_DefaultLineColor[18] = 0x686868;
	m_DefaultLineColor[19] = 0x606060;
	m_DefaultLineColor[20] = 0x585858;
	m_DefaultLineColor[21] = 0x505050;
	m_DefaultLineColor[22] = 0x484848;
	m_DefaultLineColor[23] = 0x404040;
	m_DefaultLineColor[24] = 0x383838;
	m_DefaultLineColor[25] = 0x303030;
	m_DefaultLineColor[26] = 0x282828;
	m_DefaultLineColor[27] = 0x202020;
	m_DefaultLineColor[28] = 0x181818;
	m_DefaultLineColor[29] = 0x101010;
	m_DefaultLineColor[30] = 0x080808;
	m_DefaultLineColor[31] = 0x000000;
	m_DefaultLineColor[32] = 0x000000;
	m_DefaultLineColor[33] = 0x000000;
	m_DefaultLineColor[34] = 0x000000;
	m_DefaultLineColor[35] = 0x000000;
	m_DefaultLineColor[36] = 0x000000;
	m_DefaultLineColor[37] = 0x000000;
	m_DefaultLineColor[38] = 0x000000;
	m_DefaultLineColor[39] = 0x000000;
	m_DefaultLineColor[40] = 0x000000;
	m_DefaultLineColor[41] = 0x000000;
	m_DefaultLineColor[42] = 0x000000;
	m_DefaultLineColor[43] = 0x000000;
	m_DefaultLineColor[44] = 0x000000;
	m_DefaultLineColor[45] = 0x000000;
	m_DefaultLineColor[46] = 0x000000;
	m_DefaultLineColor[47] = 0x000000;

	m_DefaultLineColor[CAtaSmart::MAX_DISK] = 0x4a24ff;	// Threshold

	switch(GetPrivateProfileInt(_T("Setting"), _T("Temperature"), 0, m_Ini))
	{
	case   1:	m_bFahrenheit = TRUE; break;
	default:	m_bFahrenheit = FALSE;break;
	}

	m_bInitializing = TRUE;

	UpdateColor();
}

void CGraphDlg::UpdateColor()
{
	for(int i = 0; i < m_DetectedDisk; i++)
	{
		m_LineColor[i] = GetLineColor(i);
	}
	// Threshold
	m_LineColor[CAtaSmart::MAX_DISK] = GetLineColor(CAtaSmart::MAX_DISK);
}

COLORREF CGraphDlg::GetLineColor(DWORD index)
{
	TCHAR str[256];
	CString cstr;
	cstr.Format(_T("%d"), index);
	GetPrivateProfileString(_T("LineColor"), cstr, _T("-1"), str, 256, m_Ini);

	if(str[0] == '#')
	{
		int r, g, b;
		cstr = str;
		r = _tcstol(cstr.Mid(1,2), NULL, 16);
		g = _tcstol(cstr.Mid(3,2), NULL, 16);
		b = _tcstol(cstr.Mid(5,2), NULL, 16);
		return RGB(r, g, b);
	}
	else if(_tstoi(str) <= -1 || _tstoi(str) > 0x00FFFFFF)
	{
		return m_DefaultLineColor[index];
	}
	else
	{
		return _tstoi(str);
	}
}

HRESULT CGraphDlg::OnSelectAttributeId(IHTMLElement* /*pElement*/)
{
	UpdateData(TRUE);

	if(m_AttributeId != _tstoi(m_SelectAttributeId))
	{
		m_AttributeId = _tstoi(m_SelectAttributeId);
		UpdateGraph();
		CString cstr;
		cstr.Format(_T("%d"), m_AttributeId);
		WritePrivateProfileString(_T("Setting"), _T("SelectedAttributeId"), cstr, m_Ini);
	}

	return S_FALSE;
}

HRESULT CGraphDlg::OnDisk0(IHTMLElement* /*pElement*/){CheckDisk(0);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk1(IHTMLElement* /*pElement*/){CheckDisk(1);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk2(IHTMLElement* /*pElement*/){CheckDisk(2);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk3(IHTMLElement* /*pElement*/){CheckDisk(3);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk4(IHTMLElement* /*pElement*/){CheckDisk(4);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk5(IHTMLElement* /*pElement*/){CheckDisk(5);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk6(IHTMLElement* /*pElement*/){CheckDisk(6);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk7(IHTMLElement* /*pElement*/){CheckDisk(7);UpdateGraph();return S_FALSE;}

HRESULT CGraphDlg::OnDisk8(IHTMLElement* /*pElement*/){CheckDisk(8);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk9(IHTMLElement* /*pElement*/){CheckDisk(9);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk10(IHTMLElement* /*pElement*/){CheckDisk(10);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk11(IHTMLElement* /*pElement*/){CheckDisk(11);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk12(IHTMLElement* /*pElement*/){CheckDisk(12);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk13(IHTMLElement* /*pElement*/){CheckDisk(13);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk14(IHTMLElement* /*pElement*/){CheckDisk(14);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk15(IHTMLElement* /*pElement*/){CheckDisk(15);UpdateGraph();return S_FALSE;}

HRESULT CGraphDlg::OnDisk16(IHTMLElement* /*pElement*/){CheckDisk(16);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk17(IHTMLElement* /*pElement*/){CheckDisk(17);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk18(IHTMLElement* /*pElement*/){CheckDisk(18);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk19(IHTMLElement* /*pElement*/){CheckDisk(19);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk20(IHTMLElement* /*pElement*/){CheckDisk(20);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk21(IHTMLElement* /*pElement*/){CheckDisk(21);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk22(IHTMLElement* /*pElement*/){CheckDisk(22);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk23(IHTMLElement* /*pElement*/){CheckDisk(23);UpdateGraph();return S_FALSE;}

HRESULT CGraphDlg::OnDisk24(IHTMLElement* /*pElement*/){CheckDisk(24);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk25(IHTMLElement* /*pElement*/){CheckDisk(25);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk26(IHTMLElement* /*pElement*/){CheckDisk(26);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk27(IHTMLElement* /*pElement*/){CheckDisk(27);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk28(IHTMLElement* /*pElement*/){CheckDisk(28);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk29(IHTMLElement* /*pElement*/){CheckDisk(29);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk30(IHTMLElement* /*pElement*/){CheckDisk(30);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk31(IHTMLElement* /*pElement*/){CheckDisk(31);UpdateGraph();return S_FALSE;}

HRESULT CGraphDlg::OnDisk32(IHTMLElement* /*pElement*/){CheckDisk(32);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk33(IHTMLElement* /*pElement*/){CheckDisk(33);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk34(IHTMLElement* /*pElement*/){CheckDisk(34);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk35(IHTMLElement* /*pElement*/){CheckDisk(35);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk36(IHTMLElement* /*pElement*/){CheckDisk(36);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk37(IHTMLElement* /*pElement*/){CheckDisk(37);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk38(IHTMLElement* /*pElement*/){CheckDisk(38);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk39(IHTMLElement* /*pElement*/){CheckDisk(39);UpdateGraph();return S_FALSE;}

HRESULT CGraphDlg::OnDisk40(IHTMLElement* /*pElement*/){CheckDisk(40);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk41(IHTMLElement* /*pElement*/){CheckDisk(41);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk42(IHTMLElement* /*pElement*/){CheckDisk(42);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk43(IHTMLElement* /*pElement*/){CheckDisk(43);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk44(IHTMLElement* /*pElement*/){CheckDisk(44);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk45(IHTMLElement* /*pElement*/){CheckDisk(45);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk46(IHTMLElement* /*pElement*/){CheckDisk(46);UpdateGraph();return S_FALSE;}
HRESULT CGraphDlg::OnDisk47(IHTMLElement* /*pElement*/){CheckDisk(47);UpdateGraph();return S_FALSE;}

HRESULT CGraphDlg::OnDisk48(IHTMLElement* /*pElement*/){ CheckDisk(48); UpdateGraph(); return S_FALSE; }
HRESULT CGraphDlg::OnDisk49(IHTMLElement* /*pElement*/){ CheckDisk(49); UpdateGraph(); return S_FALSE; }
HRESULT CGraphDlg::OnDisk50(IHTMLElement* /*pElement*/){ CheckDisk(50); UpdateGraph(); return S_FALSE; }
HRESULT CGraphDlg::OnDisk51(IHTMLElement* /*pElement*/){ CheckDisk(51); UpdateGraph(); return S_FALSE; }
HRESULT CGraphDlg::OnDisk52(IHTMLElement* /*pElement*/){ CheckDisk(52); UpdateGraph(); return S_FALSE; }
HRESULT CGraphDlg::OnDisk53(IHTMLElement* /*pElement*/){ CheckDisk(53); UpdateGraph(); return S_FALSE; }
HRESULT CGraphDlg::OnDisk54(IHTMLElement* /*pElement*/){ CheckDisk(54); UpdateGraph(); return S_FALSE; }
HRESULT CGraphDlg::OnDisk55(IHTMLElement* /*pElement*/){ CheckDisk(55); UpdateGraph(); return S_FALSE; }

HRESULT CGraphDlg::OnDisk56(IHTMLElement* /*pElement*/){ CheckDisk(56); UpdateGraph(); return S_FALSE; }
HRESULT CGraphDlg::OnDisk57(IHTMLElement* /*pElement*/){ CheckDisk(57); UpdateGraph(); return S_FALSE; }
HRESULT CGraphDlg::OnDisk58(IHTMLElement* /*pElement*/){ CheckDisk(58); UpdateGraph(); return S_FALSE; }
HRESULT CGraphDlg::OnDisk59(IHTMLElement* /*pElement*/){ CheckDisk(59); UpdateGraph(); return S_FALSE; }
HRESULT CGraphDlg::OnDisk60(IHTMLElement* /*pElement*/){ CheckDisk(60); UpdateGraph(); return S_FALSE; }
HRESULT CGraphDlg::OnDisk61(IHTMLElement* /*pElement*/){ CheckDisk(61); UpdateGraph(); return S_FALSE; }
HRESULT CGraphDlg::OnDisk62(IHTMLElement* /*pElement*/){ CheckDisk(62); UpdateGraph(); return S_FALSE; }
HRESULT CGraphDlg::OnDisk63(IHTMLElement* /*pElement*/){ CheckDisk(63); UpdateGraph(); return S_FALSE; }


HRESULT CGraphDlg::OnAllOn(IHTMLElement* /*pElement*/)
{
	CString cstr;
	for(int i = 0; i < m_DetectedDisk; i++)
	{
		cstr.Format(_T("Disk%d"), i);
		m_bGraph[i] = TRUE;
		SetElementPropertyEx(cstr, DISPID_IHTMLELEMENT_CLASSNAME, _T("selected"));
	}
	UpdateGraph();

	return S_FALSE;
}

HRESULT CGraphDlg::OnAllOff(IHTMLElement* /*pElement*/)
{
	CString cstr;
	for(int i = 0; i < m_DetectedDisk; i++)
	{
		cstr.Format(_T("Disk%d"), i);
		m_bGraph[i] = FALSE;
		SetElementPropertyEx(cstr, DISPID_IHTMLELEMENT_CLASSNAME, _T(""));
	}
	UpdateGraph();

	return S_FALSE;
}

HRESULT CGraphDlg::OnReset(IHTMLElement* /*pElement*/)
{
	UpdateGraph();
	return S_FALSE;
}

BOOL CGraphDlg::CheckDisk(DWORD disk)
{
	CString cstr;

	m_bGraph[disk] = ! m_bGraph[disk];

	cstr.Format(_T("Disk%d"), disk);
	if(m_bGraph[disk])
	{
		SetElementPropertyEx(cstr, DISPID_IHTMLELEMENT_CLASSNAME, _T("selected"));
	}
	else
	{
		SetElementPropertyEx(cstr, DISPID_IHTMLELEMENT_CLASSNAME, _T(""));
	}

	return TRUE;
}

void CGraphDlg::InitMenuBar()
{
	CString cstr, temp, space;

	int counter = 0, index = 0;
	m_SelectAttributeIdCtrl = 0;
	DWORD SelectedAttributeId = GetPrivateProfileInt(_T("Setting"), _T("SelectedAttributeId"), 0, m_Ini);

	for(int i = 0; i < CAtaSmart::MAX_DISK; i++)
	{
		m_LiDisk[i] = _T("");
		cstr.Format(_T("Disk%d"), i);
		SetElementPropertyEx(cstr, DISPID_IHTMLELEMENT_CLASSNAME, _T("hidden"));
	}

	for(int i = 0; i < m_DetectedDisk; i++)
	{
		cstr.Format(_T("%d"), i + 1);
		m_LiDisk[i] = cstr;
		cstr.Format(_T("Disk%d"), i);
		SetElementPropertyEx(cstr, DISPID_IHTMLELEMENT_CLASSNAME, _T("visible"));
		
		temp.Format(_T("%s\r\n%s"), m_Model[i], m_Serial[i]);
		if(! m_Drive[i].IsEmpty())
		{
			temp += (_T("\r\n") + m_Drive[i]);
		}
		SetElementPropertyEx(cstr, DISPID_IHTMLELEMENT_TITLE, temp);
	}

	SetElementPropertyEx(_T("AllOn"), DISPID_IHTMLELEMENT_TITLE, i18n(_T("Graph"), _T("ALL_ON")));
	SetElementPropertyEx(_T("AllOff"), DISPID_IHTMLELEMENT_TITLE, i18n(_T("Graph"), _T("ALL_OFF")));
	SetElementPropertyEx(_T("Reset"), DISPID_IHTMLELEMENT_TITLE, i18n(_T("Graph"), _T("RESET")));

	CString select;

	cstr.Format(_T("<select id=\"SelectAttributeId\" title=\"%s\" onchange=\"this.click()\">"), i18n(_T("Graph"), _T("PLEASE_SELECT_ITEM"), m_bSmartEnglish));
	select = cstr;

	if(m_IeVersion >= 700)
	{
		cstr.Format(_T("<optgroup label=\"%s\">"), i18n(_T("Graph"), _T("ACTUAL_VALUE"), m_bSmartEnglish));
		select += cstr;
	}
	else
	{
		cstr.Format(_T("<option value=\"513\">%s</option>"), i18n(_T("Graph"), _T("ACTUAL_VALUE"), m_bSmartEnglish));
		select += cstr;
		space = _T("&nbsp;&nbsp;");
		if(SelectedAttributeId == 513)
		{
			index = counter;
		}
		counter++;
	}
	
	BOOL flagAttribute[256] = {0};
	CString dir, disk;
	TCHAR str[256];
	for(int i = 0; i < m_DetectedDisk; i++)
	{
		dir = m_SmartDir;
		disk = GetModelSerial(m_Model[i], m_Serial[i]);
		dir += disk;
		for(int j = 1; j < 255; j++)
		{
			cstr.Format(_T("%02X"), j);
			GetPrivateProfileString(disk, cstr, _T("-1"), str, 256, dir + _T("\\") + SMART_INI);
			if(_tstoi(str) >= 0)
			{
				flagAttribute[j] = TRUE;
			}
		}
	}

	if (m_Attribute != CAtaSmart::SSD_VENDOR_NVME)
	{
		// Reallocated Sectors Count
		cstr.Format(_T("<option value=\"%d\" selected=\"selected\">[05] %s</option>"), SMART_REALLOCATED_SECTORS_COUNT, i18n(_T("Smart"), _T("05"), m_bSmartEnglish));
		select += cstr; if (SelectedAttributeId == SMART_REALLOCATED_SECTORS_COUNT) { index = counter; }counter++;
	}

	// PowerOnHours
	cstr.Format(_T("<option value=\"%d\" selected=\"selected\">[09] %s</option>"), SMART_POWER_ON_HOURS, i18n(_T("Smart"), _T("09"), m_bSmartEnglish));
	select += cstr;if(SelectedAttributeId == SMART_POWER_ON_HOURS){index = counter;}counter++;

	// PowerOnCount
	cstr.Format(_T("<option value=\"%d\" selected=\"selected\">[0C] %s</option>"), SMART_POWER_ON_COUNT, i18n(_T("Smart"), _T("0C"), m_bSmartEnglish));
	select += cstr;if(SelectedAttributeId == SMART_POWER_ON_COUNT){index = counter;}counter++;

	// Temperature
	if(m_bFahrenheit)
	{
		cstr.Format(_T("<option value=\"%d\" selected=\"selected\">[C2] %s (&deg;F)</option>"), SMART_TEMPERATURE, i18n(_T("Smart"), _T("C2"), m_bSmartEnglish));
	}
	else
	{
		cstr.Format(_T("<option value=\"%d\" selected=\"selected\">[C2] %s (&deg;C)</option>"), SMART_TEMPERATURE, i18n(_T("Smart"), _T("C2"), m_bSmartEnglish));
	}
	select += cstr;if(SelectedAttributeId == SMART_TEMPERATURE){index = counter;}counter++;
	
	if (m_Attribute != CAtaSmart::SSD_VENDOR_NVME)
	{
		// Reallocation Event Count
		cstr.Format(_T("<option value=\"%d\" selected=\"selected\">[C4] %s</option>"), SMART_REALLOCATED_EVENT_COUNT, i18n(_T("Smart"), _T("C4"), m_bSmartEnglish));
		select += cstr; if (SelectedAttributeId == SMART_REALLOCATED_EVENT_COUNT) { index = counter; }counter++;

		// Current Pending Sector Count
		cstr.Format(_T("<option value=\"%d\" selected=\"selected\">[C5] %s</option>"), SMART_CURRENT_PENDING_SECTOR_COUNT, i18n(_T("Smart"), _T("C5"), m_bSmartEnglish));
		select += cstr; if (SelectedAttributeId == SMART_CURRENT_PENDING_SECTOR_COUNT) { index = counter; }counter++;

		// Off-Line Scan Uncorrectable Sector Count
		cstr.Format(_T("<option value=\"%d\" selected=\"selected\">[C6] %s</option>"), SMART_UNCORRECTABLE_SECTOR_COUNT, i18n(_T("Smart"), _T("C6"), m_bSmartEnglish));
		select += cstr; if (SelectedAttributeId == SMART_UNCORRECTABLE_SECTOR_COUNT) { index = counter; }counter++;
	}

	// Life
	cstr.Format(_T("<option value=\"%d\" selected=\"selected\">[XX] %s</option>"), SMART_LIFE, i18n(_T("SmartSsd"), _T("FF"), m_bSmartEnglish));
	select += cstr;if(SelectedAttributeId == SMART_LIFE){index = counter;}counter++;
	
	// HostWrites
	cstr.Format(_T("<option value=\"%d\" selected=\"selected\">[XX] %s (GB)</option>"), SMART_HOST_WRITES, i18n(_T("Dialog"), _T("TOTAL_HOST_WRITES"), m_bSmartEnglish));
	select += cstr;if(SelectedAttributeId == SMART_HOST_WRITES){index = counter;}counter++;
	
	// HostReads
	cstr.Format(_T("<option value=\"%d\" selected=\"selected\">[XX] %s (GB)</option>"), SMART_HOST_READS, i18n(_T("Dialog"), _T("TOTAL_HOST_READS"), m_bSmartEnglish));
	select += cstr;if(SelectedAttributeId == SMART_HOST_READS){index = counter;}counter++;

	if (m_Attribute != CAtaSmart::SSD_VENDOR_NVME)
	{
		// NandWrites
		cstr.Format(_T("<option value=\"%d\" selected=\"selected\">[XX] %s (GB)</option>"), SMART_NAND_WRITES, i18n(_T("Dialog"), _T("TOTAL_NAND_WRITES"), m_bSmartEnglish));
		select += cstr; if (SelectedAttributeId == SMART_NAND_WRITES) { index = counter; }counter++;


		// GBytes Erased
		cstr.Format(_T("<option value=\"%d\" selected=\"selected\">[XX] %s (GB)</option>"), SMART_NAND_ERASED, i18n(_T("SmartSandForce"), _T("64"), m_bSmartEnglish));
		select += cstr; if (SelectedAttributeId == SMART_NAND_ERASED) { index = counter; }counter++;

		// Wear Leveling Count for Micron
		cstr.Format(_T("<option value=\"%d\" selected=\"selected\">[XX] %s</option>"), SMART_WEAR_LEVELING_COUNT, i18n(_T("Dialog"), _T("WEAR_LEVELING_COUNT"), m_bSmartEnglish));
		select += cstr; if (SelectedAttributeId == SMART_WEAR_LEVELING_COUNT) { index = counter; }counter++;
	}

	// NAND WAF based on the computed Host writes and NAND writes.
	cstr.Format(_T("<option value=\"%d\" selected=\"selected\">[WAF] %s</option>"), NAND_WAF, i18n(_T("SmartSsd"), _T("WAF")  , m_bSmartEnglish));
	select += cstr; if (SelectedAttributeId == NAND_WAF) { index = counter; }counter++;

	if (m_Attribute != CAtaSmart::SSD_VENDOR_NVME)
	{
		if (m_IeVersion >= 700)
		{
			cstr.Format(_T("<optgroup label=\"%s\">"), i18n(_T("Graph"), _T("NORMALIZED_VALUE"), m_bSmartEnglish));
			select += cstr;
		}
		else
		{
			cstr.Format(_T("<option value=\"514\">%s</option>"), i18n(_T("Graph"), _T("NORMALIZED_VALUE"), m_bSmartEnglish));
			select += cstr;
			if (SelectedAttributeId == 514)
			{
				index = counter;
			}
			counter++;
		}

		CString sectionName = attributeString[m_Attribute];

		for (int i = 1; i < 255; i++)
		{
			if (flagAttribute[i])
			{
				cstr.Format(_T("%02X"), i);
				if (i18n(sectionName, cstr).IsEmpty())
				{
					cstr.Format(_T("<option value=\"%d\">%s(%02X) %s</option>"), i, space, i, i18n(_T("Smart"), _T("UNKNOWN"), m_bSmartEnglish));
				}
				else
				{
					cstr.Format(_T("<option value=\"%d\">%s(%02X) %s</option>"), i, space, i, i18n(sectionName, cstr, m_bSmartEnglish));
				}
				if (SelectedAttributeId == i)
				{
					index = counter;
				}
				select += cstr;
				counter++;
			}
		}
	}

	select += _T("</select>");
	SetElementOuterHtmlEx(_T("SelectAttributeId"), select);

	m_SelectAttributeIdCtrl = index;
	UpdateData(FALSE);
}

BOOL CGraphDlg::UpdateGraph()
{
	CString cstr, line, values, thresholds, maxMin, points;
	CString smartFile, fileName;
	CStdioFile inFile;
	DWORD threshold = 0;
	int max = -1, min = -1, tickDecimals = 0;

	if(m_AttributeId > 0x200)
	{
		CallScript(_T("updateData"), _T("[]"));
	//	CallScript(_T("reDraw"), NULL);
		CallScript(_T("changeSize"), NULL); // Redraw
		return FALSE;
	}
	else if(m_AttributeId < 0x100)
	{
		fileName.Format(_T("%02X"), m_AttributeId);
		max = 255;
		min = 0;
	}
	else if(m_AttributeId >= 0x100)
	{
		switch(m_AttributeId)
		{
		case SMART_TEMPERATURE:                  fileName = _T("Temperature");				min = 0;break;
		case SMART_POWER_ON_HOURS:               fileName = _T("PowerOnHours");				break;
		case SMART_POWER_ON_COUNT:               fileName = _T("PowerOnCount");				break;
		case SMART_REALLOCATED_SECTORS_COUNT:    fileName = _T("ReallocatedSectorsCount");	min = 0; break;
		case SMART_REALLOCATED_EVENT_COUNT:      fileName = _T("ReallocationEventCount");	min = 0; break;
		case SMART_CURRENT_PENDING_SECTOR_COUNT: fileName = _T("CurrentPendingSectorCount");min = 0; break;
		case SMART_UNCORRECTABLE_SECTOR_COUNT:   fileName = _T("UncorrectableSectorCount");	min = 0; break;
		case SMART_HOST_WRITES:                  fileName = _T("HostWrites");				min = 0; break;
		case SMART_HOST_READS:                   fileName = _T("HostReads");				min = 0; break;
		case SMART_NAND_WRITES:                  fileName = _T("NandWrites");				min = 0; break;
		case SMART_NAND_ERASED:                  fileName = _T("GBytesErased");				min = 0; break;
		case SMART_WEAR_LEVELING_COUNT:          fileName = _T("WearLevelingCount");		min = 0; break;
		case SMART_LIFE:                         fileName = _T("Life");						max = 100;	min = 0; break;
		case NAND_WAF:                           fileName = _T("WAF");						tickDecimals = 2; break;
		default:
			return FALSE;
			break;
		}
	}
	else
	{
		return FALSE;
	}

	// Update Time Zone Information
	GetTimeZoneInformation(&m_TimeZoneInformation);

	UpdateColor();

	DWORD value = 0;
	double value_d = 0;
	time_t dateTime;
	ULONGLONG startTime = 0;
	int count = 0;
	int drive = 0;
	int index = 0; // Only One Disk
	CString arg;

	arg = _T("[");

	for(int i = 0; i < m_DetectedDisk; i++)
	{
		if(m_bGraph[i] == FALSE)
		{
			continue;
		}
		count = 0;
		values = _T("");

		smartFile = m_SmartDir;
		smartFile += GetModelSerial(m_Model[i], m_Serial[i]);
		smartFile += _T("\\") + fileName + _T(".csv");
		if(! inFile.Open(smartFile, CFile::modeRead | CFile::typeText))
		{
			continue;
		}
		else
		{
			int end = 0;
			int start = 0;
			int count = 0;
			CStringArray lines;
			/*
			while(inFile.ReadString(line) != NULL)
			{
				if(line.GetLength() > 0)
				{
					count++;
				}
			}
			*/

			// 20140602 - Skip null
			while (inFile.GetPosition() < inFile.GetLength())
			{
				while (inFile.ReadString(line) != NULL)
				{
					if (line.GetLength() > 0)
					{
						count++;
					}
				}
			}

			if(count > (int)m_MaxPlotPoint && m_MaxPlotPoint != 0)
			{
				start = count - (int)m_MaxPlotPoint;
				end = m_MaxPlotPoint;
			}
			else
			{
				start = 0;
				end = count;
			}
			inFile.SeekToBegin();
			count = 0;
			while (inFile.GetPosition() < inFile.GetLength())
			{
				while (inFile.ReadString(line) != NULL)
				{
					if (line.GetLength() > 0)
					{
						count++;
						if (start < count && count <= start + end)
						{
							lines.Add(line);
							DebugPrint(line);
						}
					}
				}	
			}
			inFile.Close();

			count = 0;
			if(m_AttributeId == 0x1C3) // Fahrenheit
			{
				for(int i = 0; i < end; i++)
				{
					line = lines.GetAt(i);
					dateTime = GetTimeT(line.Left(19));
					if(count != 0)
					{
						cstr.Format(_T("[%I64d, %d], "), dateTime * 1000, value * 9 / 5 + 32); 
						values += cstr;
					}
					if(count == 0)
					{
						startTime = dateTime * 1000;
					}

					value = _tstoi(line.Mid(20));
					cstr.Format(_T("[%I64d, %d], "), dateTime * 1000, value * 9 / 5 + 32); 
					values += cstr;
					count++;
				}
			}
			else if (m_AttributeId == NAND_WAF)
			{
				for (int i = 0; i < end; i++)
				{
					line = lines.GetAt(i);
					dateTime = GetTimeT(line.Left(19));

					value_d = _tstof(line.Mid(20));
					cstr.Format(_T("[%I64d, %2f], "), dateTime * 1000, value_d);

					values += cstr;
					count++;
				}
			}
			else
			{
				for(int i = 0; i < end; i++)
				{
					line = lines.GetAt(i);
					dateTime = GetTimeT(line.Left(19));
					if(count != 0)
					{
						cstr.Format(_T("[%I64d, %d], "), dateTime * 1000, value); 
						values += cstr;
					}
					if(count == 0)
					{
						startTime = dateTime * 1000;
					}

					value = _tstoi(line.Mid(20));
					cstr.Format(_T("[%I64d, %d], "), dateTime * 1000, value); 
					values += cstr;
					count++;
				}
			}
		}
		drive++;
		index = i;

		// Latest
		_tzset();
		if(m_AttributeId == 0x1C3) // Fahrenheit
		{
			cstr.Format(_T("[%I64d, %d]"), (ULONGLONG)(time(&dateTime) - m_TimeZoneInformation.Bias * 60) * 1000, value * 9 / 5 + 32);
		}
		else if (m_AttributeId == NAND_WAF)
		{
			cstr.Format(_T("[%I64d, %2f], "), dateTime * 1000, value_d);
		}
		else
		{
			cstr.Format(_T("[%I64d, %d]"), (ULONGLONG)(time(&dateTime) - m_TimeZoneInformation.Bias * 60) * 1000, value);
		}
		values += cstr;
/*		cstr.Format(_T("{label: \"(%d) %s\", data:[%s]}, "),
			i + 1, m_Model[i],
			values);
*/
		if(m_Drive[i].IsEmpty())
		{
			cstr.Format(_T("{label: \"(%d) %s\", color: \"rgb(%d, %d, %d)\", data:[%s]}, "),
				i + 1, m_ModelEscape[i], 
				GetRValue(m_LineColor[i]), GetGValue(m_LineColor[i]), GetBValue(m_LineColor[i]), values);
		}
		else
		{
			cstr.Format(_T("{label: \"(%d) %s [%s]\", color: \"rgb(%d, %d, %d)\", data:[%s]}, "),
				i + 1, m_ModelEscape[i], m_Drive[i],
				GetRValue(m_LineColor[i]), GetGValue(m_LineColor[i]), GetBValue(m_LineColor[i]), values);
		}

	//	cstr.Format(_T("{label: \"%s\", color: \"rgb(%d, %d, %d)\", data:[%s]}, "),
	//		m_Ata->vars[i].Model, rand() % 256, rand() % 256, rand() % 256, values);
		arg += cstr;
	}

	if(m_AttributeId < 0x100 && drive == 1)
	{
		CString dir, disk;
		dir = m_SmartDir;
		disk = GetModelSerial(m_Model[index], m_Serial[index]);
		dir += disk;
		TCHAR str[256];
		cstr.Format(_T("%02X"), m_AttributeId);
		GetPrivateProfileString(disk + _T("THRESHOLD"), cstr, _T("-1"), str, 256, dir + _T("\\") + SMART_INI);
		threshold = _tstoi(str);
		if(threshold >= 0)
		{
			cstr.Format(_T("{label: \"%s\", color: \"rgb(%d, %d, %d)\", data:["),
				i18n(_T("Dialog"), _T("LIST_THRESHOLD"), m_bSmartEnglish),
				GetRValue(m_LineColor[CAtaSmart::MAX_DISK]),
				GetGValue(m_LineColor[CAtaSmart::MAX_DISK]),
				GetBValue(m_LineColor[CAtaSmart::MAX_DISK])
				);
			arg += cstr;
			cstr.Format(_T("[%I64d, %d], "), startTime, threshold); 
			arg += cstr; 
			cstr.Format(_T("[%I64d, %d]"), (ULONGLONG)(time(&dateTime) - m_TimeZoneInformation.Bias * 60) * 1000, threshold); 
			arg += cstr;
			arg += _T("]}, ");
		}
	}

	if(arg.GetLength() > 3)
	{
		arg.Delete(arg.GetLength() - 2, 2);
	}
	arg += _T("]");

	CString options, overViewOptions, grid;

	if(m_bPaintWeekend)
	{
		grid = _T("grid: { hoverable: true, clickable: true, coloredAreas: weekendAreas },");
	}
	else
	{
		grid = _T("grid: { hoverable: true, clickable: true },");
	}

	// Options
	if (tickDecimals > 0) // WAF
	{
		options.Format(_T("{\
xaxis: { mode: \"time\", ticks: 4, timeformat: \"%s\", minTickSize: [60, \"minute\"]},\
yaxis: { minTickSize: 0.01, min: null, max: null, tickDecimals: %d },\
selection: { mode: \"xy\" },\
%s\
legend: { position: \"%s\", margin: 20 },\
lines: { show: true }\
}"), m_TimeFormat, tickDecimals, grid, m_LegendPositon);
	}
	else if(max == -1 && min == -1) // Auto
	{
	options.Format(_T("{\
xaxis: { mode: \"time\", ticks: 4, timeformat: \"%s\", minTickSize: [60, \"minute\"]},\
yaxis: { minTickSize: 1, min: null, max: null },\
selection: { mode: \"xy\" },\
%s\
legend: { position: \"%s\", margin: 20 },\
lines: { show: true }\
}"), m_TimeFormat, grid, m_LegendPositon);
	}
	else if(max == -1)
	{
	options.Format(_T("{\
xaxis: { mode: \"time\", ticks: 4, timeformat: \"%s\", minTickSize: [60, \"minute\"]},\
yaxis: { minTickSize: 1, min: %d, max: null },\
selection: { mode: \"xy\" },\
%s\
legend: { position: \"%s\", margin: 20 },\
lines: { show: true }\
%s\
}"), m_TimeFormat, min, grid, m_LegendPositon, points);
	}
	else
	{
	options.Format(_T("{\
xaxis: { mode: \"time\", ticks: 4, timeformat: \"%s\", minTickSize: [60, \"minute\"]},\
yaxis: { minTickSize: 1, min: %d, max: %d },\
selection: { mode: \"xy\" },\
%s\
legend: { position: \"%s\", margin: 20 },\
lines: { show: true }\
%s\
}"), m_TimeFormat, min, max, grid, m_LegendPositon, points);
	}

	// overViewOptions
	if(max == -1 && min == -1)
	{
	overViewOptions.Format(_T("{\
xaxis: { ticks: [], mode: \"time\" },\
yaxis: { ticks: [], min: 0, max: null  },\
selection: { mode: \"x\" },\
%s\
legend: { show: false },\
lines: { show: true }\
}"), grid);
	}
	else if(max == -1)
	{
	overViewOptions.Format(_T("{\
xaxis: { ticks: [], mode: \"time\" },\
yaxis: { ticks: [], min: 0, max: null  },\
selection: { mode: \"x\" },\
%s\
legend: { show: false },\
lines: { show: true }\
}"), grid);
	}
	else
	{
	overViewOptions.Format(_T("{\
xaxis: { ticks: [], mode: \"time\" },\
yaxis: { ticks: [], min: %d, max: %d },\
selection: { mode: \"x\" },\
%s\
legend: { show: false },\
lines: { show: true }\
}"), min, max, grid);
	}
	CallScript(_T("updateData"), arg);
	CallScript(_T("updateMainViewOptions"), options);
	CallScript(_T("updateOverViewOptions"), overViewOptions);
//	CallScript(_T("reDraw"), NULL);
	CallScript(_T("changeSize"), NULL); // Redraw

	if(m_AttributeId >= 0x100)
	{
		cstr.Format(_T("%02X"), m_AttributeId % 256);
		m_Title.Format(_T("%s"), i18n(_T("Smart"), cstr, m_bSmartEnglish));
	}
	else
	{
		cstr.Format(_T("%02X"), m_AttributeId);
		m_Title.Format(_T("(%02X) %s"), m_AttributeId, i18n(_T("Smart"), cstr, m_bSmartEnglish));
	}

	UpdateData(FALSE);
	return TRUE;
}


void CGraphDlg::InitMenu()
{
	CString cstr, temp;
	CMenu *menu = GetMenu();
	CMenu subMenu;

	cstr = i18n(_T("Menu"), _T("FILE"));
	menu->ModifyMenu(0, MF_BYPOSITION | MF_STRING, 0, cstr);
	cstr = i18n(_T("Menu"), _T("OPTION"));
	menu->ModifyMenu(1, MF_BYPOSITION | MF_STRING, 1, cstr);

	cstr = i18n(_T("Menu"), _T("CUSTOMIZE"));
	menu->ModifyMenu(ID_CUSTOMIZE, MF_STRING, ID_CUSTOMIZE, cstr);
 	cstr = i18n(_T("Menu"), _T("EXIT"));
	menu->ModifyMenu(ID_EXIT, MF_STRING, ID_EXIT, cstr);
	cstr = i18n(_T("Menu"), _T("PAINT_WEEKEND"));
	menu->ModifyMenu(ID_PAINT_WEEKEND, MF_STRING, ID_PAINT_WEEKEND, cstr);
	cstr = i18n(_T("Menu"), _T("ALL"));
	menu->ModifyMenu(ID_POINT_ALL, MF_STRING, ID_POINT_ALL, cstr);
	cstr = i18n(_T("Menu"), _T("NORTH_WEST"));
	menu->ModifyMenu(ID_NORTH_WEST, MF_STRING, ID_NORTH_WEST, cstr);
	cstr = i18n(_T("Menu"), _T("NORTH_EAST"));
	menu->ModifyMenu(ID_NORTH_EAST, MF_STRING, ID_NORTH_EAST, cstr);
	cstr = i18n(_T("Menu"), _T("SOUTH_WEST"));
	menu->ModifyMenu(ID_SOUTH_WEST, MF_STRING, ID_SOUTH_WEST, cstr);
	cstr = i18n(_T("Menu"), _T("SOUTH_EAST"));
	menu->ModifyMenu(ID_SOUTH_EAST, MF_STRING, ID_SOUTH_EAST, cstr);

	subMenu.Attach(menu->GetSubMenu(1)->GetSafeHmenu());
	cstr = i18n(_T("Menu"), _T("LEGEND_POSITION"));
	subMenu.ModifyMenu(0, MF_BYPOSITION, 0, cstr);
	cstr = i18n(_T("Menu"), _T("MAX_PLOT_POINT"));
	subMenu.ModifyMenu(1, MF_BYPOSITION, 1, cstr);
	cstr = i18n(_T("Menu"), _T("TIME_FORMAT"));
	subMenu.ModifyMenu(2, MF_BYPOSITION, 2, cstr);
	cstr = i18n(_T("Menu"), _T("ATTRIBUTE"));
	subMenu.ModifyMenu(3, MF_BYPOSITION, 3, cstr);

	subMenu.Detach();

	if(m_bPaintWeekend)
	{
		menu->CheckMenuItem(ID_PAINT_WEEKEND, MF_CHECKED);
	}

	if(m_LegendPositon.Compare(_T("nw")) == 0)
	{
		menu->CheckMenuRadioItem(ID_NORTH_WEST, ID_SOUTH_EAST, ID_NORTH_WEST, MF_BYCOMMAND);
	}
	else if(m_LegendPositon.Compare(_T("ne")) == 0)
	{
		menu->CheckMenuRadioItem(ID_NORTH_WEST, ID_SOUTH_EAST, ID_NORTH_EAST, MF_BYCOMMAND);
	}
	else if(m_LegendPositon.Compare(_T("sw")) == 0)
	{
		menu->CheckMenuRadioItem(ID_NORTH_WEST, ID_SOUTH_EAST, ID_SOUTH_WEST, MF_BYCOMMAND);
	}
	else
	{
		menu->CheckMenuRadioItem(ID_NORTH_WEST, ID_SOUTH_EAST, ID_SOUTH_EAST, MF_BYCOMMAND);
	}

	if(m_TimeFormat.Compare(_T("%m/%d")) == 0)
	{
		menu->CheckMenuRadioItem(ID_MDHM, ID_DMY2, ID_MD, MF_BYCOMMAND);
	}
	else if(m_TimeFormat.Compare(_T("%y/%m/%d %H:%M")) == 0)
	{
		menu->CheckMenuRadioItem(ID_MDHM, ID_DMY2, ID_YMDHM, MF_BYCOMMAND);
	}
	else if(m_TimeFormat.Compare(_T("%y/%m/%d")) == 0)
	{
		menu->CheckMenuRadioItem(ID_MDHM, ID_DMY2, ID_YMD, MF_BYCOMMAND);
	}
	else if(m_TimeFormat.Compare(_T("%d/%m/%y")) == 0)
	{
		menu->CheckMenuRadioItem(ID_MDHM, ID_DMY2, ID_DMY, MF_BYCOMMAND);
	}
	else if(m_TimeFormat.Compare(_T("%d/%m/%y %H:%M")) == 0)
	{
		menu->CheckMenuRadioItem(ID_MDHM, ID_DMY2, ID_DMYHM, MF_BYCOMMAND);
	}
	else if(m_TimeFormat.Compare(_T("%d.%m.%y %H:%M")) == 0)
	{
		menu->CheckMenuRadioItem(ID_MDHM, ID_DMY2, ID_DMYHM2, MF_BYCOMMAND);
	}
	else if(m_TimeFormat.Compare(_T("%d.%m.%y")) == 0)
	{
		menu->CheckMenuRadioItem(ID_MDHM, ID_DMY2, ID_DMY2, MF_BYCOMMAND);
	}
	else
	{
		menu->CheckMenuRadioItem(ID_MDHM, ID_DMY2, ID_MDHM, MF_BYCOMMAND);
	}

	switch(m_MaxPlotPoint)
	{
	case 100:menu->CheckMenuRadioItem(ID_POINT_100, ID_POINT_ALL, ID_POINT_100, MF_BYCOMMAND);break;
	case 200:menu->CheckMenuRadioItem(ID_POINT_100, ID_POINT_ALL, ID_POINT_200, MF_BYCOMMAND);break;
	case 300:menu->CheckMenuRadioItem(ID_POINT_100, ID_POINT_ALL, ID_POINT_300, MF_BYCOMMAND);break;
	case 400:menu->CheckMenuRadioItem(ID_POINT_100, ID_POINT_ALL, ID_POINT_400, MF_BYCOMMAND);break;
	case 500:menu->CheckMenuRadioItem(ID_POINT_100, ID_POINT_ALL, ID_POINT_500, MF_BYCOMMAND);break;
	case 600:menu->CheckMenuRadioItem(ID_POINT_100, ID_POINT_ALL, ID_POINT_600, MF_BYCOMMAND);break;
	case 700:menu->CheckMenuRadioItem(ID_POINT_100, ID_POINT_ALL, ID_POINT_700, MF_BYCOMMAND);break;
	case 800:menu->CheckMenuRadioItem(ID_POINT_100, ID_POINT_ALL, ID_POINT_800, MF_BYCOMMAND);break;
	case 900:menu->CheckMenuRadioItem(ID_POINT_100, ID_POINT_ALL, ID_POINT_900, MF_BYCOMMAND);break;
	case 1000:menu->CheckMenuRadioItem(ID_POINT_100, ID_POINT_ALL, ID_POINT_1000, MF_BYCOMMAND);break;
	case 2000:menu->CheckMenuRadioItem(ID_POINT_100, ID_POINT_ALL, ID_POINT_2000, MF_BYCOMMAND);break;
	case 3000:menu->CheckMenuRadioItem(ID_POINT_100, ID_POINT_ALL, ID_POINT_3000, MF_BYCOMMAND);break;
	case 4000:menu->CheckMenuRadioItem(ID_POINT_100, ID_POINT_ALL, ID_POINT_4000, MF_BYCOMMAND);break;
	case 5000:menu->CheckMenuRadioItem(ID_POINT_100, ID_POINT_ALL, ID_POINT_5000, MF_BYCOMMAND);break;
	default:
		m_MaxPlotPoint = 0;
		menu->CheckMenuRadioItem(ID_POINT_100, ID_POINT_ALL, ID_POINT_ALL, MF_BYCOMMAND);
		break;
	}

	switch(m_Attribute)
	{
	case CAtaSmart::HDD_GENERAL: menu->CheckMenuRadioItem(ID_HDD, ID_SSD_MAX, ID_HDD, MF_BYCOMMAND);break;
	case CAtaSmart::SSD_GENERAL: menu->CheckMenuRadioItem(ID_HDD, ID_SSD_MAX, ID_SSD, MF_BYCOMMAND);break;
	case CAtaSmart::SSD_VENDOR_MTRON: menu->CheckMenuRadioItem(ID_HDD, ID_SSD_MAX, ID_SSD_MTRON, MF_BYCOMMAND);break;
	case CAtaSmart::SSD_VENDOR_INDILINX: menu->CheckMenuRadioItem(ID_HDD, ID_SSD_MAX, ID_SSD_INDILINX, MF_BYCOMMAND);break;
	case CAtaSmart::SSD_VENDOR_JMICRON: menu->CheckMenuRadioItem(ID_HDD, ID_SSD_MAX, ID_SSD_JMICRON, MF_BYCOMMAND);break;
	case CAtaSmart::SSD_VENDOR_INTEL: menu->CheckMenuRadioItem(ID_HDD, ID_SSD_MAX, ID_SSD_INTEL, MF_BYCOMMAND);break;
	case CAtaSmart::SSD_VENDOR_SAMSUNG: menu->CheckMenuRadioItem(ID_HDD, ID_SSD_MAX, ID_SSD_SAMSUNG, MF_BYCOMMAND);break;
	case CAtaSmart::SSD_VENDOR_SANDFORCE: menu->CheckMenuRadioItem(ID_HDD, ID_SSD_MAX, ID_SSD_SANDFORCE, MF_BYCOMMAND);break;
	case CAtaSmart::SSD_VENDOR_MICRON: menu->CheckMenuRadioItem(ID_HDD, ID_SSD_MAX, ID_SSD_MICRON, MF_BYCOMMAND);break;
	case CAtaSmart::SSD_VENDOR_OCZ: menu->CheckMenuRadioItem(ID_HDD, ID_SSD_MAX, ID_SSD_OCZ, MF_BYCOMMAND);break;
	case CAtaSmart::SSD_VENDOR_PLEXTOR: menu->CheckMenuRadioItem(ID_HDD, ID_SSD_MAX, ID_SSD_PLEXTOR, MF_BYCOMMAND);break;
	case CAtaSmart::SSD_VENDOR_SANDISK: menu->CheckMenuRadioItem(ID_HDD, ID_SSD_MAX, ID_SSD_SANDISK, MF_BYCOMMAND);break;
	case CAtaSmart::SSD_VENDOR_OCZ_VECTOR: menu->CheckMenuRadioItem(ID_HDD, ID_SSD_MAX, ID_SSD_OCZ_VECTOR, MF_BYCOMMAND);break;
	case CAtaSmart::SSD_VENDOR_REALTEK: menu->CheckMenuRadioItem(ID_HDD, ID_SSD_MAX, ID_SSD_REALTEK, MF_BYCOMMAND); break;
	case CAtaSmart::SSD_VENDOR_NVME: menu->CheckMenuRadioItem(ID_HDD, ID_SSD_MAX, ID_SSD_NVME, MF_BYCOMMAND); break;

	default:
		m_Attribute = CAtaSmart::HDD_GENERAL;
		menu->CheckMenuRadioItem(ID_HDD, ID_SSD_MAX, ID_HDD, MF_BYCOMMAND);
		break;
	}

	SetMenu(menu);
	DrawMenuBar();
}

BOOL CGraphDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if(GRAPH_DISK_INDEX <= wParam && wParam <= GRAPH_DISK_INDEX + (CAtaSmart::MAX_DISK + 1) * 100)
	{
		int i = (int)(wParam - GRAPH_DISK_INDEX) / 100;
		int j = (int)(wParam - GRAPH_DISK_INDEX) % 100;

		UpdateGraph();
	}

	return CDHtmlDialogEx::OnCommand(wParam, lParam);
}

time_t CGraphDlg::GetTimeT(CString time)
{
	CString token;
	int currentPosition = 0;
	int index = 0;
	tm t = {0};

	token = time.Tokenize(_T(" :/"), currentPosition);
	while(token != _T(""))
	{
		switch(index)
		{
		case 0:
			t.tm_year = _ttoi(token) - 1900;
			break;
		case 1:
			t.tm_mon = _ttoi(token) - 1;
			break;
		case 2:
			t.tm_mday = _ttoi(token);
			break;
		case 3:
			t.tm_hour = _ttoi(token);
			break;
		case 4:
			t.tm_min = _ttoi(token);
			break;
		case 5:
			t.tm_sec = _ttoi(token);
			break;
		default:
			break;
		}
		token = time.Tokenize(_T(" :/"), currentPosition);
		index++;
	}

	return mktime(&t) - m_TimeZoneInformation.Bias * 60;
}

void CGraphDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = (LONG)(SIZE_X * m_ZoomRatio) + GetSystemMetrics(SM_CXFRAME) * 2;
	lpMMI->ptMinTrackSize.y = (LONG)(SIZE_Y * m_ZoomRatio) + GetSystemMetrics(SM_CYMENU) + GetSystemMetrics(SM_CYSIZEFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION);

	CDHtmlDialogEx::OnGetMinMaxInfo(lpMMI);
}

void CGraphDlg::UpdateDialogSize()
{
	ChangeZoomType(m_ZoomType);

	if(GetPrivateProfileInt(_T("Setting"), _T("GraphHeight"), 0, m_Ini) > 0)
	{
		m_SizeY = GetPrivateProfileInt(_T("Setting"), _T("GraphHeight"), 0, m_Ini);
	}
	else
	{
		m_SizeY = SIZE_Y;
	}
	if(GetPrivateProfileInt(_T("Setting"), _T("GraphWidth"), 0, m_Ini) > 0)
	{
		m_SizeX = GetPrivateProfileInt(_T("Setting"), _T("GraphWidth"), 0, m_Ini);
	}
	else
	{
		m_SizeX = SIZE_X;
	}

	SetClientRect((DWORD)(m_SizeX * m_ZoomRatio), (DWORD)(m_SizeY * m_ZoomRatio), 1);
}

void CGraphDlg::OnSize(UINT nType, int cx, int cy)
{
	CDHtmlMainDialog::OnSize(nType, cx, cy);

	if(m_bInitializing == FALSE)
	{
		RECT rect;
		CString cstr;
		GetClientRect(&rect);
		cstr.Format(_T("%d"), (DWORD)((rect.bottom - rect.top) / m_ZoomRatio));
		WritePrivateProfileString(_T("Setting"), _T("GraphHeight"), cstr, m_Ini);	
		cstr.Format(_T("%d"), (DWORD)((rect.right - rect.left) / m_ZoomRatio));
		WritePrivateProfileString(_T("Setting"), _T("GraphWidth"), cstr, m_Ini);
	}
}

void CGraphDlg::OnExit()
{
	OnCancel();
}

// Legend Position
void CGraphDlg::OnNorthWest(){SetLegendPosition(ID_NORTH_WEST, _T("nw"));}
void CGraphDlg::OnNorthEast(){SetLegendPosition(ID_NORTH_EAST, _T("ne"));}
void CGraphDlg::OnSouthWest(){SetLegendPosition(ID_SOUTH_WEST, _T("sw"));}
void CGraphDlg::OnSouthEast(){SetLegendPosition(ID_SOUTH_EAST, _T("se"));}

void CGraphDlg::SetLegendPosition(DWORD id, CString position)
{
	WritePrivateProfileString(_T("Setting"), _T("LegendPosition"), position, m_Ini);
	m_LegendPositon = position;
	UpdateGraph();

	CMenu *menu = GetMenu();
	menu->CheckMenuRadioItem(ID_NORTH_WEST, ID_SOUTH_EAST, id, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
}

void CGraphDlg::OnMdhm()  {SetTimeFormat(ID_MDHM,   _T("%m/%d %H:%M"));}
void CGraphDlg::OnMd()    {SetTimeFormat(ID_MD,     _T("%m/%d"));}
void CGraphDlg::OnYmdhm() {SetTimeFormat(ID_YMDHM,  _T("%y/%m/%d %H:%M"));}
void CGraphDlg::OnYmd()   {SetTimeFormat(ID_YMD,    _T("%y/%m/%d"));}
void CGraphDlg::OnDmyhm() {SetTimeFormat(ID_DMYHM,  _T("%d/%m/%y %H:%M"));}
void CGraphDlg::OnDmy()   {SetTimeFormat(ID_DMY,    _T("%d/%m/%y"));}
void CGraphDlg::OnDmyhm2(){SetTimeFormat(ID_DMYHM2, _T("%d.%m.%y %H:%M"));}
void CGraphDlg::OnDmy2()  {SetTimeFormat(ID_DMY2,   _T("%d.%m.%y"));}

void CGraphDlg::SetTimeFormat(DWORD id, CString format)
{
	WritePrivateProfileString(_T("Setting"), _T("TimeFormat"), format, m_Ini);
	m_TimeFormat = format;
	UpdateGraph();

	CMenu *menu = GetMenu();
	menu->CheckMenuRadioItem(ID_MDHM, ID_DMY2, id, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
}

void CGraphDlg::OnPaintWeekend()
{
	if(m_bPaintWeekend)
	{
		m_bPaintWeekend = FALSE;
		WritePrivateProfileString(_T("Setting"), _T("PaintWeekend"), _T("0"), m_Ini);

		CMenu *menu = GetMenu();
		menu->CheckMenuItem(ID_PAINT_WEEKEND, MF_UNCHECKED);
		SetMenu(menu);
		DrawMenuBar();
	}
	else
	{
		m_bPaintWeekend = TRUE;
		WritePrivateProfileString(_T("Setting"), _T("PaintWeekend"), _T("1"), m_Ini);

		CMenu *menu = GetMenu();
		menu->CheckMenuItem(ID_PAINT_WEEKEND, MF_CHECKED);
		SetMenu(menu);
		DrawMenuBar();
	}
	UpdateGraph();
}

void CGraphDlg::OnPoint100(){SetPlotPoint(ID_POINT_100, 100);}
void CGraphDlg::OnPoint200(){SetPlotPoint(ID_POINT_200, 200);}
void CGraphDlg::OnPoint300(){SetPlotPoint(ID_POINT_300, 300);}
void CGraphDlg::OnPoint400(){SetPlotPoint(ID_POINT_400, 400);}
void CGraphDlg::OnPoint500(){SetPlotPoint(ID_POINT_500, 500);}
void CGraphDlg::OnPoint600(){SetPlotPoint(ID_POINT_600, 600);}
void CGraphDlg::OnPoint700(){SetPlotPoint(ID_POINT_700, 700);}
void CGraphDlg::OnPoint800(){SetPlotPoint(ID_POINT_800, 800);}
void CGraphDlg::OnPoint900(){SetPlotPoint(ID_POINT_900, 900);}
void CGraphDlg::OnPoint1000(){SetPlotPoint(ID_POINT_1000, 1000);}
void CGraphDlg::OnPoint2000(){SetPlotPoint(ID_POINT_2000, 2000);}
void CGraphDlg::OnPoint3000(){SetPlotPoint(ID_POINT_3000, 3000);}
void CGraphDlg::OnPoint4000(){SetPlotPoint(ID_POINT_4000, 4000);}
void CGraphDlg::OnPoint5000(){SetPlotPoint(ID_POINT_5000, 5000);}
void CGraphDlg::OnPointAll(){SetPlotPoint(ID_POINT_ALL, 0);}

void CGraphDlg::SetPlotPoint(DWORD id, DWORD point)
{
	CString cstr;
	cstr.Format(_T("%d"), point);
	WritePrivateProfileString(_T("Setting"), _T("MaxPlotPoint"), cstr, m_Ini);
	m_MaxPlotPoint = point;
	UpdateGraph();

	CMenu *menu = GetMenu();
	menu->CheckMenuRadioItem(ID_POINT_100, ID_POINT_ALL, id, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
}

void CGraphDlg::OnCustomize()
{
	m_OptionDlg = new COptionDlg(this);
	m_OptionDlg->Create(COptionDlg::IDD, m_OptionDlg, ID_CUSTOMIZE, this);
}

void CGraphDlg::UpdateBkImage()
{
	TCHAR str[256];
	CString cstr;
	GetPrivateProfileString(_T("Customize"), _T("GraphBkImage"), _T(""), str, 256, m_Ini);
	cstr = str;
	cstr.Replace(_T("\\"), _T("/"));
	if(cstr.IsEmpty())
	{
		cstr = _T("image/background.png");
	}
	CallScript(_T("changeBackgroundImage"), _T("url(") + cstr + _T(")"));
}

LRESULT CGraphDlg::OnUpdateBkImage(WPARAM wParam, LPARAM lParam)
{
	UpdateBkImage();
	return 0;
}

LRESULT CGraphDlg::OnUpdateLineColor(WPARAM wParam, LPARAM lParam)
{
	UpdateGraph();
	return 0;
}

void CGraphDlg::OnHdd()
{
	SetAttribute(ID_HDD, CAtaSmart::HDD_GENERAL);
}

void CGraphDlg::OnSsd()
{
	SetAttribute(ID_SSD, CAtaSmart::SSD_GENERAL);
}

void CGraphDlg::OnSsdMtron()
{
	SetAttribute(ID_SSD_MTRON, CAtaSmart::SSD_VENDOR_MTRON);
}

void CGraphDlg::OnSsdIndilinx()
{
	SetAttribute(ID_SSD_INDILINX, CAtaSmart::SSD_VENDOR_INDILINX);
}

void CGraphDlg::OnSsdJmicron()
{
	SetAttribute(ID_SSD_JMICRON, CAtaSmart::SSD_VENDOR_JMICRON);
}

void CGraphDlg::OnSsdIntel()
{
	SetAttribute(ID_SSD_INTEL, CAtaSmart::SSD_VENDOR_INTEL);
}

void CGraphDlg::OnSsdSamsung()
{
	SetAttribute(ID_SSD_SAMSUNG, CAtaSmart::SSD_VENDOR_SAMSUNG);
}


void CGraphDlg::OnSsdSandforce()
{
	SetAttribute(ID_SSD_SANDFORCE, CAtaSmart::SSD_VENDOR_SANDFORCE);
}

void CGraphDlg::OnSsdMicron()
{
	SetAttribute(ID_SSD_MICRON, CAtaSmart::SSD_VENDOR_MICRON);
}

void CGraphDlg::OnSsdOcz()
{
	SetAttribute(ID_SSD_OCZ, CAtaSmart::SSD_VENDOR_OCZ);
}

void CGraphDlg::OnSsdPlextor()
{
	SetAttribute(ID_SSD_PLEXTOR, CAtaSmart::SSD_VENDOR_PLEXTOR);
}

void CGraphDlg::OnSsdSanDisk()
{
	SetAttribute(ID_SSD_SANDISK, CAtaSmart::SSD_VENDOR_SANDISK);
}

void CGraphDlg::OnSsdOczVector()
{
	SetAttribute(ID_SSD_OCZ_VECTOR, CAtaSmart::SSD_VENDOR_OCZ_VECTOR);
}

void CGraphDlg::OnSsdRealtek()
{
	SetAttribute(ID_SSD_REALTEK, CAtaSmart::SSD_VENDOR_REALTEK);
}

void CGraphDlg::OnSsdNVMe()
{
	SetAttribute(ID_SSD_NVME, CAtaSmart::SSD_VENDOR_NVME);
}

void CGraphDlg::SetAttribute(DWORD id, DWORD type)
{
	CString cstr;
	cstr.Format(_T("%d"), type);
	WritePrivateProfileString(_T("Setting"), _T("Attribute"), cstr, m_Ini);
	if(type <= CAtaSmart::SSD_VENDOR_MAX)
	{
		m_Attribute = type;
	}
	else
	{
		m_Attribute = CAtaSmart::HDD_GENERAL;
	}

	CMenu *menu = GetMenu();
	menu->CheckMenuRadioItem(ID_HDD, ID_SSD_MAX, id, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();

	InitMenuBar();
}

CString CGraphDlg::GetModelSerial(CString &model, CString &serialNumber)
{
	CString modelSerial;
	modelSerial = model + serialNumber;
	modelSerial.Replace(_T("\\"), _T(""));
	modelSerial.Replace(_T("/"), _T(""));
	modelSerial.Replace(_T(":"), _T(""));
	modelSerial.Replace(_T("*"), _T(""));
	modelSerial.Replace(_T("?"), _T(""));
	modelSerial.Replace(_T("\""), _T(""));
	modelSerial.Replace(_T("<"), _T(""));
	modelSerial.Replace(_T(">"), _T(""));
	modelSerial.Replace(_T("|"), _T(""));

	return modelSerial;
}