/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : http://crystalmark.info/
//      License : The modified BSD license
//
//                                Copyright 2012 hiyohiyo. All rights reserved.
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DiskInfo.h"
#include "AlarmHistoryDlg.h"

IMPLEMENT_DYNCREATE(CAlarmHistoryDlg, CDHtmlDialog)

CAlarmHistoryDlg::CAlarmHistoryDlg(CWnd* pParent /*=NULL*/)
	: CDHtmlDialogEx(CAlarmHistoryDlg::IDD, CAlarmHistoryDlg::IDH, pParent)
{
	m_CurrentLangPath = ((CDHtmlMainDialog*)pParent)->m_CurrentLangPath;
	m_DefaultLangPath = ((CDHtmlMainDialog*)pParent)->m_DefaultLangPath;
	m_ZoomType = ((CDHtmlMainDialog*)pParent)->GetZoomType();
}

CAlarmHistoryDlg::~CAlarmHistoryDlg()
{
}

void CAlarmHistoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ALARM_LIST, m_List);
}

BOOL CAlarmHistoryDlg::OnInitDialog()
{
	CDHtmlDialogEx::OnInitDialog();

	CString cstr;
	cstr = i18n(_T("WindowTitle"), _T("ALARM_HISTORY"));
	SetWindowText(i18n(_T("WindowTitle"), _T("ALARM_HISTORY")));

	EnableDpiAware();
	InitDHtmlDialog(SIZE_X, SIZE_Y, ((CDiskInfoApp*)AfxGetApp())->m_AlarmHistoryDlgPath);

	m_List.InsertColumn(0, _T(""), LVCFMT_CENTER, 25, 0);
	m_List.InsertColumn(1, _T("Time"), LVCFMT_CENTER, (int)(64 * m_ZoomRatio), 0);
	m_List.InsertColumn(2, _T("ID"), LVCFMT_CENTER, (int)(32 * m_ZoomRatio), 0);
	m_List.InsertColumn(3, _T("Disk"), LVCFMT_CENTER, (int)(128 * m_ZoomRatio), 0);
	m_List.InsertColumn(4, _T("Message"), LVCFMT_CENTER, (int)(256 * m_ZoomRatio), 0);

	return TRUE;
}

void CAlarmHistoryDlg::OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
	CString cstr;
	cstr = szUrl;
	if(cstr.Find(_T("html")) != -1 || cstr.Find(_T("dlg")) != -1)
	{
		ChangeZoomType(m_ZoomType);
		SetClientRect((DWORD)(SIZE_X * m_ZoomRatio), (DWORD)(SIZE_Y * m_ZoomRatio), 0);

		UpdateData(FALSE);
		CenterWindow();
		ShowWindow(SW_SHOW);
	}
}

BEGIN_MESSAGE_MAP(CAlarmHistoryDlg, CDHtmlDialogEx)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CAlarmHistoryDlg)
END_DHTML_EVENT_MAP()
