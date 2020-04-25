/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once
#include "DialogFx.h"
#include "StaticCx.h"
#include "ButtonCx.h"
#include "ComboBoxCx.h"

class CTemperatureDlg : public CDialogFx
{
	DECLARE_DYNAMIC(CTemperatureDlg)

	static const int SIZE_X = 400;
	static const int SIZE_Y = 120;

public:
	CTemperatureDlg(CWnd* pParent = NULL);
	virtual ~CTemperatureDlg();

	enum { IDD = IDD_TEMPERATURE };

protected:
	DWORD m_DiskIndex;

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void UpdateDialogSize();

	void InitSelectDisk();
	void InitLang();
	void UpdateSelectDisk(DWORD index);

	DECLARE_MESSAGE_MAP()
public:
	CString m_ValueTemperature;
	CString m_ValueTemperatureF;
//	CString m_LabelTemperature;

	CScrollBar m_CtrlScrollbarTemperature;
	CStaticCx m_CtrlValueTemperature;
	CStaticCx m_CtrlValueTemperatureF;
//	CStaticCx m_CtrlLabelTemperature;
	CComboBoxCx m_CtrlSelectDisk;

	CButtonCx m_CtrlApply;
	CButtonCx m_CtrlDefault;

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	afx_msg void OnBnClickedApply();
	afx_msg void OnBnClickedDefault();
	afx_msg void OnCbnSelchangeSelectDisk();
};
