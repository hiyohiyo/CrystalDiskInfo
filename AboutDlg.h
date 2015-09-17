/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : http://crystalmark.info/
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
#ifdef SUISHO_SHIZUKU_SUPPORT
	afx_msg void OnSecretVoice();
	afx_msg void OnBnClickedProjectShizukuKirinokasumu();
	afx_msg void OnBnClickedProjectShizukuLinuxhajapan();
	afx_msg void OnBnClickedProjectShizukuOpensourcechannel();
	afx_msg void OnBnClickedProjectShizukuBellche();
#endif
	CButtonCx m_CtrlCrystalDewWorld;
	CButtonCx m_CtrlProjectShizukuKirinokasumu;
	CButtonCx m_CtrlProjectShizukuLinuxhajapan;
	CButtonCx m_CtrlProjectShizukuOpensourcechannel;
	CButtonCx m_CtrlProjectShizukuBellche;
	CButtonCx m_CtrlSecretVoice;

	CStaticCx m_CtrlVersion;
	CStaticCx m_CtrlEdition;
	CStaticCx m_CtrlRelease;
	CStaticCx m_CtrlCopyright;
	CStaticCx m_CtrlLicense;

};
