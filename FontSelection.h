/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once

#include "afxwin.h"
#include "DialogFx.h"
#include "StaticFx.h"
#include "ButtonFx.h"
#include "ComboBoxFx.h"
#include "FontComboBox.h"

class CFontSelection : public CDialogFx
{
	DECLARE_DYNAMIC(CFontSelection)

	static const int SIZE_X = 520;
	static const int SIZE_Y = 228;
	enum { IDD = IDD_FONT };

public:
	CFontSelection(CWnd* pParent = NULL);
	virtual ~CFontSelection();

	CString GetFontFace();
	int GetFontScale();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	CButtonFx m_CtrlOk;
	void OnSetDefault();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void UpdateDialogSize();

	CStaticFx m_LabelFontFace;
	CStaticFx m_LabelFontScale;

	CFontComboBox m_FontComboBox;
	CComboBoxFx m_FontScaleComboBox;
	CButtonFx m_ButtonSetDefault;
	afx_msg void OnBnClickedOk();
};
