/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#pragma once
#include "DialogFx.h"
#include "ButtonFx.h"
#include "SliderCtrlFx.h"
#include "StaticFX.h"

class CSoundSettingDlg : public CDialogFx
{
	DECLARE_DYNCREATE(CSoundSettingDlg)

	static const int SIZE_X = 488;
	static const int SIZE_Y = 104;

public:
	CSoundSettingDlg(CWnd* pParent = NULL);
	virtual ~CSoundSettingDlg();

	enum { IDD = IDD_SOUND_SETTINGS};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void UpdateDialogSize();
	virtual void OnCancel();
	void SetVolumeImage();
	
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSelectFile();
	afx_msg void OnPlay();
	afx_msg void OnDefault();
	afx_msg void OnOk();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	CString m_FilePath;
	CStaticFx m_CtrlFilePath;
	CButtonFx m_CtrlSelectFile;
	CButtonFx m_CtrlPlay;
	int m_InitialVolume;
	int m_CurrentVolume;
	CSliderCtrlFx m_CtrlSlider;
	CButtonFx m_CtrlDefault;
	CButtonFx m_CtrlOk;
	CStaticFx m_CtrlVolume;
	CStaticFx m_CtrlValueVolume;
};
