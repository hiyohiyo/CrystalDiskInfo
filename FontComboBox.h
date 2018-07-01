/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once

// CFontComboBox

class CFontComboBox : public CComboBoxCx
{
	DECLARE_DYNAMIC(CFontComboBox)

public:
	CFontComboBox();
	virtual ~CFontComboBox();

protected:
	DECLARE_MESSAGE_MAP()

	INT m_FontHeight;
public:
	void SetFontHeight(int height, double zoomRatio);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
};


