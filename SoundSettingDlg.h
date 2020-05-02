/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once
#include "DialogFx.h"
#include "ButtonFx.h"
#include "StaticFX.h"

class CSoundSettingDlg : public CDialogFx
{
	DECLARE_DYNCREATE(CSoundSettingDlg)

	static const int SIZE_X = 488;
	static const int SIZE_Y = 72;

public:
	CSoundSettingDlg(CWnd* pParent = NULL);
	virtual ~CSoundSettingDlg();

	enum { IDD = IDD_SOUND_SETTINGS};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void UpdateDialogSize();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSelectFile();
	afx_msg void OnPlay();
	afx_msg void OnDefault();
	afx_msg void OnOk();

	CString m_FilePath;
	CStaticFx m_CtrlFilePath;
	CButtonFx m_CtrlSelectFile;
	CButtonFx m_CtrlPlay;
	CButtonFx m_CtrlDefault;
	CButtonFx m_CtrlOk;
};
