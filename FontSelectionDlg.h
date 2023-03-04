/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#pragma once

#include "afxwin.h"
#include "ButtonFx.h"
#include "ComboBoxFx.h"
#include "FontComboBoxFx.h"

class CFontSelectionDlg : public CDialogFx
{
	DECLARE_DYNAMIC(CFontSelectionDlg)

	static const int SIZE_X = 480;
	static const int SIZE_Y = 204;
	enum { IDD = IDD_FONT };

public:
	CFontSelectionDlg(CWnd* pParent = NULL);
	virtual ~CFontSelectionDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void UpdateDialogSize();

	void SetDefaultFont(CString fontFace);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSetDefault();
	afx_msg void OnOk();

	CStaticFx m_LabelFontFace;
	CStaticFx m_LabelFontScale;
	CStaticFx m_LabelFontRender;
	CButtonFx m_CtrlOk;
	CButtonFx m_CtrlDefault;

	CFontComboBox m_CtrlFontFace;
	CComboBoxFx   m_CtrlFontScale;
	CComboBoxFx   m_CtrlFontRender;

};
