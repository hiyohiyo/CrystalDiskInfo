/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once

#include "afxwin.h"
#include "ButtonFx.h"
#include "ComboBoxFx.h"
#include "FontComboBox.h"

class CFontSelectionDlg : public CDialogFx
{
	DECLARE_DYNAMIC(CFontSelectionDlg)

	static const int SIZE_X = 480;
	static const int SIZE_Y = 208;
	enum { IDD = IDD_FONT };

public:
	CFontSelectionDlg(CWnd* pParent = NULL);
	virtual ~CFontSelectionDlg();

	CString GetFontFace();
	int GetFontScale();

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
	CButtonFx m_CtrlOk;
	CButtonFx m_CtrlSetDefault;

	CFontComboBox m_CtrlFontFace;
	CComboBoxFx   m_CtrlFontScale;

};
