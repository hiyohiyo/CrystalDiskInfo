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

class CAboutDlg : public CDialogCx
{
	DECLARE_DYNCREATE(CAboutDlg)

#ifdef SUISHO_SHIZUKU_SUPPORT
	static const int SIZE_X = 640;
	static const int SIZE_Y = 660;
#else
	static const int SIZE_X = 640;
	static const int SIZE_Y = 170;
#endif
	static const int SIZE_HC_Y = 170; // for HighContrast Mode

public:
	CAboutDlg(CWnd* pParent = NULL);
	virtual ~CAboutDlg();

	enum { IDD = IDD_ABOUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void UpdateDialogSize();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCrystalDewWorld();
	afx_msg void OnBnClickedProjectSite1();
	afx_msg void OnBnClickedProjectSite2();
	afx_msg void OnBnClickedProjectSite3();
	afx_msg void OnBnClickedProjectSite4();
	afx_msg void OnBnClickedProjectSite5();

	CButtonCx m_CtrlCrystalDewWorld;
	CButtonCx m_CtrlSecretVoice;
	CButtonCx m_CtrlProjectSite1;
	CButtonCx m_CtrlProjectSite2;
	CButtonCx m_CtrlProjectSite3;
	CButtonCx m_CtrlProjectSite4;
	CButtonCx m_CtrlProjectSite5;

	CStaticCx m_CtrlVersion;
	CStaticCx m_CtrlEdition;
	CStaticCx m_CtrlRelease;
	CStaticCx m_CtrlCopyright;
	CStaticCx m_CtrlLicense;

#ifdef SUISHO_SHIZUKU_SUPPORT
	afx_msg void OnSecretVoice();
#endif

};
