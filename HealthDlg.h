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

class CHealthDlg : public CDialogFx
{
	DECLARE_DYNAMIC(CHealthDlg)

	static const int SIZE_X = 400;
	static const int SIZE_Y = 304;

public:
	CHealthDlg(CWnd* pParent = NULL);
	virtual ~CHealthDlg();

	enum { IDD = IDD_HEALTH };

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
	CString m_Value05;
	CString m_ValueC5;
	CString m_ValueC6;
	CString m_ValueFF;
	CString m_Value05X;
	CString m_ValueC5X;
	CString m_ValueC6X;
	CString m_ValueFFX;
	CString m_Label05;
	CString m_LabelC5;
	CString m_LabelC6;
	CString m_LabelFF;

	CScrollBar m_CtrlScrollbar05;
	CScrollBar m_CtrlScrollbarC5;
	CScrollBar m_CtrlScrollbarC6;
	CScrollBar m_CtrlScrollbarFF;
	CStaticFx m_CtrlValue05;
	CStaticFx m_CtrlValueC5;
	CStaticFx m_CtrlValueC6;
	CStaticFx m_CtrlValueFF;
	CStaticFx m_CtrlValue05X;
	CStaticFx m_CtrlValueC5X;
	CStaticFx m_CtrlValueC6X;
	CStaticFx m_CtrlValueFFX;
	CStaticFx m_CtrlLabel05;
	CStaticFx m_CtrlLabelC5;
	CStaticFx m_CtrlLabelC6;
	CStaticFx m_CtrlLabelFF;
	CComboBoxFx m_CtrlSelectDisk;
	CButtonFx m_CtrlApply;
	CButtonFx m_CtrlDefault; 
};
