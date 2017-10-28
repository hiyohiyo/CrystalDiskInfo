/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once

#include "AtaSmart.h"
#include "OptionDlg.h"

// Task Tray
enum
{
	MY_UPDATE_BG_IMAGE = (WM_APP + 0x1100),
	MY_UPDATE_LINE_COLOR,
};

class CGraphDlg : public CDHtmlMainDialog
{
	DECLARE_DYNCREATE(CGraphDlg)

	static const int SIZE_X = 640;
	static const int SIZE_Y = 480;

public:
	CGraphDlg(CWnd* pParent = NULL, int defaultDisk = -1);
	virtual ~CGraphDlg();

	CString	m_SmartDir;
	BOOL m_FlagSmartEnglish;

	int m_SizeX;
	int m_SizeY;
	
	BOOL m_FlagGraph[CAtaSmart::MAX_DISK];
	COLORREF m_LineColor[CAtaSmart::MAX_DISK + 1];
	COLORREF m_DefaultLineColor[CAtaSmart::MAX_DISK + 1];

	CString m_LiDisk[CAtaSmart::MAX_DISK];

	DWORD m_AttributeId;
	int m_DefaultDisk[CAtaSmart::MAX_DISK];
	int m_DetectedDisk;

	DWORD m_IeVersion;

	CString m_Model[CAtaSmart::MAX_DISK];
	CString m_ModelEscape[CAtaSmart::MAX_DISK];
	CString m_Serial[CAtaSmart::MAX_DISK];
	CString m_Drive[CAtaSmart::MAX_DISK];

	COptionDlg* m_OptionDlg;
	CString m_Title;
	CString m_SelectAttributeId;
	long m_SelectAttributeIdCtrl;

	CString m_LegendPositon;
	CString m_TimeFormat;

	enum { IDD = IDD_GRAPH, IDH = IDR_HTML_DUMMY };

protected:
	HICON m_hIcon;
	HICON m_hIconMini;
	time_t GetTimeT(CString time);
	TIME_ZONE_INFORMATION m_TimeZoneInformation;

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void InitDialogComplete();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	void UpdateDialogSize();

	HRESULT OnAllOn(IHTMLElement *pElement);
	HRESULT OnAllOff(IHTMLElement *pElement);
	HRESULT OnReset(IHTMLElement *pElement);

	HRESULT OnDisk0(IHTMLElement *pElement);
	HRESULT OnDisk1(IHTMLElement *pElement);
	HRESULT OnDisk2(IHTMLElement *pElement);
	HRESULT OnDisk3(IHTMLElement *pElement);
	HRESULT OnDisk4(IHTMLElement *pElement);
	HRESULT OnDisk5(IHTMLElement *pElement);
	HRESULT OnDisk6(IHTMLElement *pElement);
	HRESULT OnDisk7(IHTMLElement *pElement);
	HRESULT OnDisk8(IHTMLElement *pElement);
	HRESULT OnDisk9(IHTMLElement *pElement);
	HRESULT OnDisk10(IHTMLElement *pElement);
	HRESULT OnDisk11(IHTMLElement *pElement);
	HRESULT OnDisk12(IHTMLElement *pElement);
	HRESULT OnDisk13(IHTMLElement *pElement);
	HRESULT OnDisk14(IHTMLElement *pElement);
	HRESULT OnDisk15(IHTMLElement *pElement);
	HRESULT OnDisk16(IHTMLElement *pElement);
	HRESULT OnDisk17(IHTMLElement *pElement);
	HRESULT OnDisk18(IHTMLElement *pElement);
	HRESULT OnDisk19(IHTMLElement *pElement);
	HRESULT OnDisk20(IHTMLElement *pElement);
	HRESULT OnDisk21(IHTMLElement *pElement);
	HRESULT OnDisk22(IHTMLElement *pElement);
	HRESULT OnDisk23(IHTMLElement *pElement);
	HRESULT OnDisk24(IHTMLElement *pElement);
	HRESULT OnDisk25(IHTMLElement *pElement);
	HRESULT OnDisk26(IHTMLElement *pElement);
	HRESULT OnDisk27(IHTMLElement *pElement);
	HRESULT OnDisk28(IHTMLElement *pElement);
	HRESULT OnDisk29(IHTMLElement *pElement);
	HRESULT OnDisk30(IHTMLElement *pElement);
	HRESULT OnDisk31(IHTMLElement *pElement);
	HRESULT OnDisk32(IHTMLElement *pElement);
	HRESULT OnDisk33(IHTMLElement *pElement);
	HRESULT OnDisk34(IHTMLElement *pElement);
	HRESULT OnDisk35(IHTMLElement *pElement);
	HRESULT OnDisk36(IHTMLElement *pElement);
	HRESULT OnDisk37(IHTMLElement *pElement);
	HRESULT OnDisk38(IHTMLElement *pElement);
	HRESULT OnDisk39(IHTMLElement *pElement);
	HRESULT OnDisk40(IHTMLElement *pElement);
	HRESULT OnDisk41(IHTMLElement *pElement);
	HRESULT OnDisk42(IHTMLElement *pElement);
	HRESULT OnDisk43(IHTMLElement *pElement);
	HRESULT OnDisk44(IHTMLElement *pElement);
	HRESULT OnDisk45(IHTMLElement *pElement);
	HRESULT OnDisk46(IHTMLElement *pElement);
	HRESULT OnDisk47(IHTMLElement *pElement);
	HRESULT OnDisk48(IHTMLElement *pElement);
	HRESULT OnDisk49(IHTMLElement *pElement);
	HRESULT OnDisk50(IHTMLElement *pElement);
	HRESULT OnDisk51(IHTMLElement *pElement);
	HRESULT OnDisk52(IHTMLElement *pElement);
	HRESULT OnDisk53(IHTMLElement *pElement);
	HRESULT OnDisk54(IHTMLElement *pElement);
	HRESULT OnDisk55(IHTMLElement *pElement);
	HRESULT OnDisk56(IHTMLElement *pElement);
	HRESULT OnDisk57(IHTMLElement *pElement);
	HRESULT OnDisk58(IHTMLElement *pElement);
	HRESULT OnDisk59(IHTMLElement *pElement);
	HRESULT OnDisk60(IHTMLElement *pElement);
	HRESULT OnDisk61(IHTMLElement *pElement);
	HRESULT OnDisk62(IHTMLElement *pElement);
	HRESULT OnDisk63(IHTMLElement *pElement);


	HRESULT OnSelectAttributeId(IHTMLElement *pElement);

	void InitMenu();
	void InitMenuBar();
	BOOL UpdateGraph();
	BOOL CheckDisk(DWORD disk);
	void SetLegendPosition(DWORD id, CString position);
	void SetTimeFormat(DWORD id, CString format);
	void SetPlotPoint(DWORD id, DWORD point);
	void SetAttribute(DWORD id, DWORD type);
	void InitVars(int defaultDisk);
	void UpdateColor();
	void UpdateBgImage();

	CString GetModelSerial(CString &model, CString &serialNumber);

	BOOL m_FlagFahrenheit;
	BOOL m_FlagPaintWeekend;
	DWORD m_MaxPlotPoint;
	DWORD m_Attribute;

	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnUpdateBgImage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateLineColor(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	DECLARE_DHTML_EVENT_MAP()

public:
	COLORREF GetLineColor(DWORD index);

	afx_msg void OnExit();
	afx_msg void OnNorthWest();
	afx_msg void OnNorthEast();
	afx_msg void OnSouthWest();
	afx_msg void OnSouthEast();
	afx_msg void OnPaintWeekend();
	afx_msg void OnPoint100();
	afx_msg void OnPoint200();
	afx_msg void OnPoint300();
	afx_msg void OnPoint400();
	afx_msg void OnPoint500();
	afx_msg void OnPoint600();
	afx_msg void OnPoint700();
	afx_msg void OnPoint800();
	afx_msg void OnPoint900();
	afx_msg void OnPoint1000();
	afx_msg void OnPoint2000();
	afx_msg void OnPoint3000();
	afx_msg void OnPoint4000();
	afx_msg void OnPoint5000();
	afx_msg void OnPointAll();
	afx_msg void OnCustomize();
	afx_msg void OnMdhm();
	afx_msg void OnMd();
	afx_msg void OnYmdhm();
	afx_msg void OnYmd();
	afx_msg void OnDmyhm();
	afx_msg void OnDmy();
	afx_msg void OnDmyhm2();
	afx_msg void OnDmy2();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHdd();
	afx_msg void OnSsd();
	afx_msg void OnSsdMtron();
	afx_msg void OnSsdIndilinx();
	afx_msg void OnSsdJmicron();
	afx_msg void OnSsdIntel();
	afx_msg void OnSsdSamsung();
	afx_msg void OnSsdSandforce();
	afx_msg void OnSsdMicron();
	afx_msg void OnSsdOcz();
	afx_msg void OnSsdPlextor();
	afx_msg void OnSsdSanDisk();
	afx_msg void OnSsdOczVector();
	afx_msg void OnSsdNVMe();
};
