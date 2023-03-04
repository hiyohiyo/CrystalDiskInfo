/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
//
//                                Copyright 2008 hiyohiyo. All rights reserved.
/*---------------------------------------------------------------------------*/

#pragma once
#include "afxwin.h"
#include "DialogFx.h"

#include "StaticFx.h"
#include "ButtonFx.h"
#include "ComboBoxFx.h"

class CSettingDlg : public CDialogFx
{
	DECLARE_DYNCREATE(CSettingDlg)

	static const int SIZE_X = 400;
	static const int SIZE_Y = 304;

public:
	CSettingDlg(CWnd* pParent = NULL);
	virtual ~CSettingDlg();

	enum { IDD = IDD_SETTING };

protected:
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
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCbnSelchangeSelectDisk();

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
	CString m_EnableAam;
	CString m_DisableAam;
	CString m_EnableApm;
	CString m_DisableApm;

	CStaticFx m_CtrlAamStatus;
	CStaticFx m_CtrlApmStatus;
	CStaticFx m_CtrlCurrentAam;
	CStaticFx m_CtrlCurrentApm;
	CStaticFx m_CtrlRecommendAam;
	CStaticFx m_CtrlLabelAam;
	CStaticFx m_CtrlLabelApm;
	CStaticFx m_CtrlLabelAamLow;
	CStaticFx m_CtrlLabelAamHigh;
	CStaticFx m_CtrlLabelAamRecommend;
	CStaticFx m_CtrlLabelApmLow;
	CStaticFx m_CtrlLabelApmHigh;

	CButtonFx m_CtrlEnableAam;
	CButtonFx m_CtrlDisableAam;
	CButtonFx m_CtrlEnableApm;
	CButtonFx m_CtrlDisableApm;
	CComboBoxFx	m_CtrlSelectDisk;
	CScrollBar m_AamScrollbar;
	CScrollBar m_ApmScrollbar;
};
