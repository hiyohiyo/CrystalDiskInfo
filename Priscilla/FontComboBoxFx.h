/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once

#include "ComboBoxFx.h"

class CFontComboBox : public CComboBoxFx
{
	DECLARE_DYNAMIC(CFontComboBox)

public:
	CFontComboBox();
	virtual ~CFontComboBox();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	HBRUSH m_Brush;
};


