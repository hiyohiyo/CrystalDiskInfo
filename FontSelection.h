/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once
#include "FontComboBox.h"
#include "DialogCx.h"
#include "ButtonCx.h"
#include "ComboBoxCx.h"
#include "afxwin.h"

class CFontSelection : public CDialogCx
{
	DECLARE_DYNAMIC(CFontSelection)

	static const int SIZE_X = 320;
	static const int SIZE_Y = 120;

public:
	CFontSelection(CWnd* pParent = NULL);
	virtual ~CFontSelection();

	CString GetFontFace();
	INT GetFontType();

	enum { IDD = IDD_FONT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

//	CString m_FontFace;
//	CString m_WindowTitle;

	CButtonCx m_CtrlOk;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void UpdateDialogSize();

	CFontComboBox m_FontComboBox;
	afx_msg void OnBnClickedOk();
	CComboBoxCx m_FontTypeComboBox;
};
