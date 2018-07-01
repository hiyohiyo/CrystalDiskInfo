/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "ComboBoxCx.h"

IMPLEMENT_DYNAMIC(CComboBoxCx, CComboBox)

BOOL CComboBoxCx::IsHighContrast()
{
	HIGHCONTRAST hc;
	hc.cbSize = sizeof(HIGHCONTRAST);
	SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST), &hc, 0);
	return hc.dwFlags & HCF_HIGHCONTRASTON;
}

CComboBoxCx::CComboBoxCx()
{
}

CComboBoxCx::~CComboBoxCx()
{
	m_Font.DeleteObject();
}

BEGIN_MESSAGE_MAP(CComboBoxCx, CComboBox)
END_MESSAGE_MAP()

void CComboBoxCx::SetFontEx(CString face, int size, double zoomRatio)
{
	LOGFONT logFont = {0};
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfHeight = (LONG)(-1 * size * zoomRatio);
	logFont.lfQuality = 6;
	if(face.GetLength() < 32)
	{
		wsprintf(logFont.lfFaceName, _T("%s"), face.GetString());
	}
	else
	{
		wsprintf(logFont.lfFaceName, _T(""));
	}

	m_Font.DeleteObject();
	m_Font.CreateFontIndirect(&logFont);
	SetFont(&m_Font);

	SetItemHeight(-1, (UINT)(size * zoomRatio * 1.25));
}