/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : http://crystalmark.info/
//      License : The modified BSD license
//
//                                Copyright 2012 hiyohiyo. All rights reserved.
/*---------------------------------------------------------------------------*/

#pragma once

#include "CompileOptions.h"
#include "afxcmn.h"
#include "ListCtrlEx.h"

class CAlarmHistoryDlg : public CDHtmlDialogEx
{
	DECLARE_DYNCREATE(CAlarmHistoryDlg)

	static const int SIZE_X = 640;
	static const int SIZE_Y = 480;

public:
	CAlarmHistoryDlg(CWnd* pParent = NULL);
	virtual ~CAlarmHistoryDlg();

	enum { IDD = IDD_ALARM_HISTORY, IDH = IDR_HTML_DUMMY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
public:
	CListCtrlEx m_List;
};
