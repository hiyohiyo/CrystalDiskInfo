/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once
#include "DialogCx.h"
#include "StaticCx.h"
#include "ButtonCx.h"
#include "ComboBoxCx.h"

class CHealthDlg : public CDialogCx
{
	DECLARE_DYNAMIC(CHealthDlg)

	static const int SIZE_X = 400;
	static const int SIZE_Y = 304;

public:
	CHealthDlg(CWnd* pParent = NULL);
	virtual ~CHealthDlg();

	enum { IDD = IDD_HEALTH };

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
	CStaticCx m_CtrlValue05;
	CStaticCx m_CtrlValueC5;
	CStaticCx m_CtrlValueC6;
	CStaticCx m_CtrlValueFF;
	CStaticCx m_CtrlValue05X;
	CStaticCx m_CtrlValueC5X;
	CStaticCx m_CtrlValueC6X;
	CStaticCx m_CtrlValueFFX;
	CStaticCx m_CtrlLabel05;
	CStaticCx m_CtrlLabelC5;
	CStaticCx m_CtrlLabelC6;
	CStaticCx m_CtrlLabelFF;
	CComboBoxCx m_CtrlSelectDisk;

	CButtonCx m_CtrlApply;
	CButtonCx m_CtrlDefault; 

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	afx_msg void OnBnClickedApply();
	afx_msg void OnBnClickedDefault();
	afx_msg void OnCbnSelchangeSelectDisk();
};
