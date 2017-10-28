/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once
#include "DialogCx.h"
#include "ButtonCx.h"
#include "StaticCX.h"

class CSoundSettingDlg : public CDialogCx
{
	DECLARE_DYNCREATE(CSoundSettingDlg)

	static const int SIZE_X = 488;
	static const int SIZE_Y = 76;

	static const int SIZE_X_SJ = 480;
	static const int SIZE_Y_SJ = 480;

public:
	CSoundSettingDlg(CWnd* pParent = NULL);
	virtual ~CSoundSettingDlg();

	enum { IDD = IDD_SOUND_SETTINGS};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void UpdateDialogSize();

	CString m_FilePath;

	CStaticCx m_CtrlFilePath;
	CButtonCx m_CtrlSelectFile;
	CButtonCx m_CtrlPlay;
	CButtonCx m_CtrlDefault;
	CButtonCx m_CtrlOk;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSelectFile();
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonDefault();
	afx_msg void OnBnClickedButtonOk();
};
