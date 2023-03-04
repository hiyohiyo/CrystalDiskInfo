/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#pragma once
#include "DialogFx.h"
#include "StaticFx.h"
#include "ButtonFx.h"
#include "ComboBoxFx.h"

class CTemperatureDlg : public CDialogFx
{
	DECLARE_DYNAMIC(CTemperatureDlg)

	static const int SIZE_X = 400;
	static const int SIZE_Y = 112;

public:
	CTemperatureDlg(CWnd* pParent = NULL);
	virtual ~CTemperatureDlg();

	enum { IDD = IDD_TEMPERATURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void UpdateDialogSize();

	void InitSelectDisk();
	void InitLang();
	void UpdateSelectDisk(DWORD index);
	
	DECLARE_MESSAGE_MAP()
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnApply();
	afx_msg void OnDefault();
	afx_msg void OnCbnSelchangeSelectDisk();

	DWORD m_DiskIndex;
	CString m_ValueTemperature;
	CString m_ValueTemperatureF;
	CScrollBar m_CtrlScrollbarTemperature;
	CStaticFx m_CtrlValueTemperature;
	CStaticFx m_CtrlValueTemperatureF;
	CComboBoxFx m_CtrlSelectDisk;

	CButtonFx m_CtrlApply;
	CButtonFx m_CtrlDefault;
};
