/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once

class CHeaderCtrlFx : public CHeaderCtrl
{
	DECLARE_DYNAMIC(CHeaderCtrlFx)

public:
	CHeaderCtrlFx();
	virtual ~CHeaderCtrlFx();

protected:
	// Draw Control
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};


