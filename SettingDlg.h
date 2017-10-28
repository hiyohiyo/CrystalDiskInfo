/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
//
//                                Copyright 2008 hiyohiyo. All rights reserved.
/*---------------------------------------------------------------------------*/

#pragma once
#include "afxwin.h"
#include "DialogCx.h"

#include "StaticCx.h"
#include "ButtonCx.h"
#include "ComboBoxCx.h"

class CSettingDlg : public CDialogCx
{
	DECLARE_DYNCREATE(CSettingDlg)

	static const int SIZE_X = 400;
	static const int SIZE_Y = 304;

public:
	CSettingDlg(CWnd* pParent = NULL);
	virtual ~CSettingDlg();

	enum { IDD = IDD_SETTING };

protected:
	DWORD m_DiskIndex;

	CString m_SelectDisk;
	CString m_AamLow;
	CString m_AamHigh;
	CString m_AamRecommend;
	CString m_ApmLow;
	CString m_ApmHigh;

	CString m_AamStatus;
	CString m_ApmStatus;

	CString m_LabelAam;
	CString m_LabelApm;

	CString m_CurrentAam;
	CString m_RecommendAam;
	CString m_CurrentApm;
//	CString m_RecommendApm;

	CString m_EnableAam;
	CString m_DisableAam;
	CString m_EnableApm;
	CString m_DisableApm;

	CStaticCx m_CtrlAamStatus;
	CStaticCx m_CtrlApmStatus;
	CStaticCx m_CtrlCurrentAam;
	CStaticCx m_CtrlCurrentApm;
	CStaticCx m_CtrlRecommendAam;

	CStaticCx m_CtrlLabelAam;
	CStaticCx m_CtrlLabelApm;

	CStaticCx m_CtrlLabelAamLow;
	CStaticCx m_CtrlLabelAamHigh;
	CStaticCx m_CtrlLabelAamRecommend;
	CStaticCx m_CtrlLabelApmLow;
	CStaticCx m_CtrlLabelApmHigh;

	CButtonCx m_CtrlEnableAam;
	CButtonCx m_CtrlDisableAam;
	CButtonCx m_CtrlEnableApm;
	CButtonCx m_CtrlDisableApm;

	CComboBoxCx	m_CtrlSelectDisk;
	CScrollBar m_AamScrollbar;
	CScrollBar m_ApmScrollbar;

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void UpdateDialogSize();

	void InitSelectDisk();
	void InitLang();

	void OnEnableAam();
	void OnDisableAam();
	void OnEnableApm();
	void OnDisableApm();

	void UpdateSelectDisk(DWORD index);

	DECLARE_MESSAGE_MAP()

public:

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCbnSelchangeSelectDisk();
};
