/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "resource.h"
#include "DHtmlDialogEx.h"
#include "GetOsInfo.h"
#include <Shlwapi.h>

#include <strsafe.h>

CDHtmlDialogEx::CDHtmlDialogEx(UINT dlgResouce, UINT dlgHtml, CWnd* pParent)
				:CDHtmlDialog(dlgResouce, dlgHtml, pParent)
{
	m_FlagShowWindow = FALSE;
	m_FlagModelessDlg = FALSE;
	m_ParentWnd = NULL;
	m_DlgWnd = NULL;
	m_MenuId = 0;

	m_ZoomRatio = 1.0;
	m_ZoomType = ZOOM_TYPE_AUTO;
}

CDHtmlDialogEx::~CDHtmlDialogEx()
{
}

void CDHtmlDialogEx::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BOOL CDHtmlDialogEx::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();

	m_hAccelerator = ::LoadAccelerators(AfxGetInstanceHandle(),
		                                MAKEINTRESOURCE(IDR_ACCELERATOR));
	return TRUE;
}

BEGIN_MESSAGE_MAP(CDHtmlDialogEx, CDHtmlDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CDHtmlDialogEx)
END_DHTML_EVENT_MAP()

BOOL CDHtmlDialogEx::PreTranslateMessage(MSG* pMsg) 
{
	if(m_hAccelerator != NULL)
	{
		if(::TranslateAccelerator(m_hWnd, m_hAccelerator, pMsg) != 0)
		{
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CDHtmlDialogEx::OnAmbientProperty(COleControlSite* pSite, DISPID dispid, VARIANT* pvar)
{
	if(dispid == DISPID_AMBIENT_DLCONTROL)
	{
		pvar->vt	= VT_I4;
		pvar->lVal |= DLCTL_DLIMAGES;

		return TRUE;
    }
	return CDHtmlDialog::OnAmbientProperty(pSite, dispid, pvar);
}

/*
void CDHtmlDialogEx::ChangeAmbient(void)
{
    LPOLECONTROL  pOleControl = NULL;

    m_pBrowserApp.QueryInterface(&pOleControl);
    pOleControl->OnAmbientPropertyChange(DISPID_AMBIENT_DLCONTROL);

    if(pOleControl)pOleControl->Release();
}
*/

void CDHtmlDialogEx::OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
	DebugPrint(_T("OnDocumentComplete"));
	CString cstr = szUrl;
	if(cstr.Find(_T("html")) != -1 || cstr.Find(_T("dlg")) != -1)
	{
		InitDialogComplete();
	}
}

void CDHtmlDialogEx::InitDialogComplete()
{
	DebugPrint(_T("InitDialogComplete"));
	UpdateData(FALSE);
	m_FlagShowWindow = TRUE;
	ShowWindow(SW_SHOW);
	DebugPrint(_T("InitDialogComplete - once"));
}

double CDHtmlDialogEx::GetZoomRatio()
{
	return m_ZoomRatio;
}

#ifndef DOCHOSTUIFLAG_DPI_AWARE
#define DOCHOSTUIFLAG_DPI_AWARE 0x40000000
#endif

void CDHtmlDialogEx::EnableDpiAware()
{
	if(GetIeVersion() >= 800)
	{
		DOCHOSTUIINFO info;
		info.cbSize = sizeof(info);
		GetHostInfo(&info);
		SetHostFlags(info.dwFlags | DOCHOSTUIFLAG_DIALOG | DOCHOSTUIFLAG_THEME | DOCHOSTUIFLAG_DPI_AWARE);
	}
}

DWORD CDHtmlDialogEx::ChangeZoomType(DWORD zoomType)
{
	if(GetIeVersion() < 800)
	{
		return (DWORD)-1;
	}

	DWORD current;
	VARIANT zoom;
	VariantInit(&zoom);
	zoom.vt	= VT_I4;
	m_pBrowserApp->ExecWB(OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DODEFAULT, NULL, &zoom);
	current = zoom.lVal;

	if(zoomType == ZOOM_TYPE_AUTO)
	{
		if(current >= 200)
		{
			zoomType = ZOOM_TYPE_200;
		}
		else if(current >= 150)
		{
			zoomType = ZOOM_TYPE_150;
		}
		else if(current >= 125)
		{
			zoomType = ZOOM_TYPE_125;
		}
		else
		{
			zoomType = ZOOM_TYPE_100;
		}
	}

	// Force reset Zoom value
	zoom.lVal = 10;
	m_pBrowserApp->ExecWB(OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DODEFAULT, &zoom, NULL);
	zoom.lVal = zoomType;
	m_pBrowserApp->ExecWB(OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DODEFAULT, &zoom, NULL);

	m_ZoomRatio = zoomType / 100.0;

	VariantClear(&zoom);

	return zoomType;
}

void CDHtmlDialogEx::InitDialogEx(DWORD sizeX, DWORD sizeY, CString dialogPath)
{
// Enabled Visual Style
	DOCHOSTUIINFO info;
	info.cbSize = sizeof(info);
	GetHostInfo(&info);
	SetHostFlags(info.dwFlags | DOCHOSTUIFLAG_DIALOG | DOCHOSTUIFLAG_THEME);

// ReSize Dialog
	SetClientRect((DWORD)(sizeX * m_ZoomRatio), (DWORD)(sizeY * m_ZoomRatio));
	CenterWindow();

// Navigate
//	ChangeAmbient();
//	Navigate(_T("file://") + dialogPath, navNoHistory);
	TCHAR url[MAX_PATH * 3];
	DWORD length = MAX_PATH * 3;
	UrlCreateFromPath(dialogPath, url, &length, NULL);
	Navigate(url, navNoHistory);

//	DebugPrint(_T("SetTimer(TIMER_INIT_DHTML_DIALOG,"));
//	SetTimer(TIMER_INIT_DHTML_DIALOG, 100, 0);
}

// 2008/1/19 //
void CDHtmlDialogEx::SetClientRect(DWORD sizeX, DWORD sizeY, DWORD menuLine)
{
	RECT rc;
	RECT clientRc;
	rc.left = 0;
	rc.top = 0;
	rc.right = sizeX;
	rc.bottom = sizeY;

	GetClientRect(&clientRc);
	if(clientRc.bottom - clientRc.top == sizeY && clientRc.right - clientRc.left == sizeX)
	{
		return;
	}

	WINDOWINFO wi = {0};
	wi.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(&wi);

// 0x94CE004C

	AdjustWindowRect(&rc, wi.dwStyle, TRUE);
	SetWindowPos(&CWnd::wndTop, -1, -1,
		rc.right - rc.left,
		rc.bottom - rc.top + GetSystemMetrics(SM_CYMENU) * menuLine,
		SWP_NOMOVE);

	GetClientRect(&clientRc);
	if(clientRc.bottom - clientRc.top != sizeY)
	{
		SetWindowPos(&CWnd::wndTop , -1, -1, 
			rc.right - rc.left,
			rc.bottom - rc.top + GetSystemMetrics(SM_CYMENU) * menuLine + sizeY - (clientRc.bottom - clientRc.top), SWP_NOMOVE);	
	}
}

BOOL CDHtmlDialogEx::Create(UINT nIDTemplate, CWnd* pDlgWnd, UINT menuId, CWnd* pParentWnd)
{
	m_FlagModelessDlg = TRUE;
	m_ParentWnd = pParentWnd;
	m_DlgWnd = pDlgWnd;
	m_MenuId = menuId;

	if(m_MenuId != 0 && m_ParentWnd != NULL)
	{
		CMenu *menu = m_ParentWnd->GetMenu();
		menu->EnableMenuItem(m_MenuId, MF_GRAYED);
		m_ParentWnd->SetMenu(menu);
		m_ParentWnd->DrawMenuBar();
	}

	return CDialog::Create(nIDTemplate, pParentWnd);
}

void CDHtmlDialogEx::OnCancel() 
{
	if(m_FlagModelessDlg)
	{
		if(m_MenuId != 0 && m_ParentWnd != NULL)
		{
			CMenu *menu = m_ParentWnd->GetMenu();
			menu->EnableMenuItem(m_MenuId, MF_ENABLED);
			m_ParentWnd->SetMenu(menu);
			m_ParentWnd->DrawMenuBar();
		}
		CWnd::DestroyWindow();
	}
	else
	{
		CDialog::OnCancel();
	}
}

void CDHtmlDialogEx::PostNcDestroy()
{
	if(m_FlagModelessDlg)
	{
		m_DlgWnd = NULL;
		delete this;
	}
}

void CDHtmlDialogEx::OnOK()
{
}

void CDHtmlDialogEx::ShowWindowEx(int nCmdShow)
{
	m_FlagShowWindow = TRUE;
	ShowWindow(nCmdShow);
	SetForegroundWindow();
}

void CDHtmlDialogEx::SetElementPropertyEx(LPCTSTR szElementId, DISPID dispid, CString className)
{
	CComPtr<IDispatch> spdispElem;

	GetElement(szElementId, &spdispElem);
	VARIANT v;
	VariantInit(&v);
	v.vt = VT_BSTR;
	v.bstrVal = CComBSTR(className);

	if(spdispElem)
	{
		DISPPARAMS dispparams = {NULL, NULL, 1, 1};
		dispparams.rgvarg = &v;
		DISPID dispidPut = DISPID_PROPERTYPUT;
		dispparams.rgdispidNamedArgs = &dispidPut;

		spdispElem->Invoke(dispid, IID_NULL,
				LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT,
				&dispparams, NULL, NULL, NULL);
	}
	VariantClear(&v);
}

void CDHtmlDialogEx::SetElementOuterHtmlEx(LPCTSTR szElementId, CString outerHtml)
{
	CComPtr<IHTMLElement> sphtmlElem;
	GetElement(szElementId, &sphtmlElem);
	if(sphtmlElem)
	{
		sphtmlElem->put_outerHTML(CComBSTR(outerHtml));
	}
}

void CDHtmlDialogEx::SetElementInnerHtmlEx(LPCTSTR szElementId, CString innerHtml)
{
	CComPtr<IHTMLElement> sphtmlElem;
	GetElement(szElementId, &sphtmlElem);
	if(sphtmlElem)
	{
		sphtmlElem->put_innerHTML(CComBSTR(innerHtml));
	}
}

INT CDHtmlDialogEx::CallScript(CString function, CString argument)
{
	CComPtr<IHTMLDocument2> pDocument;
	HRESULT hr = GetDHtmlDocument(&pDocument);
	if(hr != S_OK){DebugPrint(_T("CallScript - GetDHtmlDocument"));}
	ASSERT( hr == S_OK );

	CComPtr<IDispatch> script;
	hr = pDocument->get_Script(&script);
	if(hr != S_OK){DebugPrint(_T("CallScript - pDocument->get_Script"));}
	ASSERT( hr == S_OK );

	CComBSTR name = CComBSTR(function);

	DISPID dispid;
	hr = script->GetIDsOfNames(
			IID_NULL,
			&name,
			1,
			LOCALE_SYSTEM_DEFAULT,
			&dispid);
	ASSERT( hr == S_OK );
	if(hr != S_OK){DebugPrint(_T("CallScript - script->GetIDsOfNames"));}

	CComVariant arg1 = CComVariant(argument);
	DISPPARAMS params = {&arg1, NULL, 1, 0};

	VARIANT ret;
	VariantInit(&ret);

	EXCEPINFO exp;

	hr = script->Invoke(
			dispid,
			IID_NULL,
			LOCALE_SYSTEM_DEFAULT,
			DISPATCH_METHOD,
			&params,
			&ret,
			&exp,
			NULL);
	ASSERT( hr == S_OK );
	if(hr != S_OK){DebugPrint(_T("CallScript - script->Invoke"));}

	INT result = 0;

	if(ret.vt == VT_I4)
	{
		result = ret.intVal;
	}
	VariantClear(&ret);

	return result;
}

CString CDHtmlDialogEx::i18n(CString section, CString key, BOOL inEnglish)
{
	TCHAR str[256];
	CString cstr;

	if(inEnglish)
	{
		GetPrivateProfileString(section, key, _T(""), str, 256, m_DefaultLangPath);
		cstr = str;
	}
	else
	{
		GetPrivateProfileString(section, key, _T(""), str, 256, m_CurrentLangPath);
		cstr = str;
		if(cstr.IsEmpty())
		{
			GetPrivateProfileString(section, key, _T(""), str, 256, m_DefaultLangPath);
			cstr = str;
		}
	}

	return cstr;
}

/////////////////////////////////////////////////////////////////////////
// Added 2010/10/3 http://msdn.microsoft.com/ja-jp/library/aa753258.aspx
/////////////////////////////////////////////////////////////////////////

STDMETHODIMP CDHtmlDialogEx::GetOptionKeyPath(LPOLESTR *pchKey, DWORD dw)
{
    HRESULT hr;
    WCHAR* szKey = L"Software";
	
    size_t cbLength;
    hr = StringCbLengthW(szKey, 1280, &cbLength);
    //  TODO: Add error handling code here.
    
    if (pchKey)
    {
        *pchKey = (LPOLESTR)CoTaskMemAlloc(cbLength + sizeof(WCHAR));
        if (*pchKey)
            hr = StringCbCopyW(*pchKey, cbLength + sizeof(WCHAR), szKey);
    }
    else
        hr = E_INVALIDARG;

    return hr;
}

void CDHtmlDialogEx::SetLabel(CString& label, CString element, CString title)
{
	label = title;
	SetElementPropertyEx(element, DISPID_IHTMLELEMENT_TITLE, title);
}

// 2012/5/6
// http://msdn.microsoft.com/ja-jp/magazine/cc163834(en-us).aspx
void CDHtmlDialogEx::OpenUrl(CString url)
{
	INT_PTR result = 0;
	result = (INT_PTR)ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_SHOWNORMAL);
	if(result <= 32)
	{
		result = (INT_PTR)ShellExecuteW(NULL, _T("open"), _T("iexplore.exe"), url, NULL, SW_SHOWNORMAL);
		if(result <= 32)
		{
			CString args;
			args.Format(_T("url.dll,FileProtocolHandler %s"), url.GetString());
			ShellExecuteW(NULL, _T("open"), _T("rundll32.exe"), args, NULL, SW_SHOWNORMAL);
		}
	}
}

// 2012/10/12 Workaround for Duplicate Click Message

BOOL CDHtmlDialogEx::ClickCheck()
{
	static DWORD preTime = 0;
	DWORD currentTime = GetTickCount();

	// CString cstr;
	// cstr.Format(_T("Cur=%d Pre=%d Sabun=%d"), currentTime, preTime, currentTime - preTime);
	// AfxMessageBox(cstr);

	if(currentTime >= preTime + 100)
	{
		preTime = currentTime;
		return TRUE;
	}
	return FALSE;
}

STDMETHODIMP CDHtmlDialogEx::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	if(dispIdMember == DISPID_HTMLELEMENTEVENTS_ONCLICK && ClickCheck())
	{
		return CDHtmlEventSink::Invoke(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
	}
	return S_FALSE;
}

void CDHtmlDialogEx::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == TIMER_INIT_DHTML_DIALOG)
	{
		if(IsLoadHtmlCompleted())
		{
			DebugPrint(_T("KillTimer(TIMER_INIT_DHTML_DIALOG)"));
			KillTimer(TIMER_INIT_DHTML_DIALOG);
			InitDialogComplete();
		}
	}

	return CDHtmlDialog::OnTimer(nIDEvent);
}

BOOL CDHtmlDialogEx::IsLoadHtmlCompleted()
{
	CComPtr<IHTMLElement> sphtmlElem = NULL;
	GetElement(_T("complete"), &sphtmlElem);
	if(sphtmlElem)
	{
		DebugPrint(_T("IsLoadHtmlCompleted - TRUE"));
		return TRUE;
	}
	else
	{
	//	DebugPrint(_T("IsLoadHtmlCompleted - FALSE"));
		return FALSE;
	}
}

void CDHtmlDialogEx::HideControl(int nIDDlgItem)
{
	GetDlgItem(nIDDlgItem)->ShowWindow(SW_HIDE);
}