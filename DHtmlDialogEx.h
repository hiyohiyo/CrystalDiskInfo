/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once

class CDHtmlDialogEx : public CDHtmlDialog
{
public:
	CDHtmlDialogEx(UINT dlgResouce, UINT dlgHtml, CWnd* pParent = NULL);
	virtual ~CDHtmlDialogEx();

	void InitDialogEx(DWORD sizeX, DWORD sizeY, CString dlgPath);
	void SetClientRect(DWORD sizeX, DWORD sizeY, DWORD menuLine = 0);
	void ShowWindowEx(int nCmdShow);

	virtual BOOL Create(UINT nIDTemplate, CWnd* dlgWnd, UINT menuId, CWnd* pParentWnd = NULL);

	CString m_CurrentLangPath;
	CString m_DefaultLangPath;

	enum ZOOM_TYPE
	{
		ZOOM_TYPE_AUTO = 0,
		ZOOM_TYPE_100 = 100,
		ZOOM_TYPE_125 = 125,
		ZOOM_TYPE_150 = 150,
		ZOOM_TYPE_200 = 200,
	};

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	static const int TIMER_INIT_DHTML_DIALOG = 0x1000;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	HACCEL m_hAccelerator;
	BOOL m_FlagShowWindow;
	BOOL m_FlagModelessDlg;
	CWnd* m_ParentWnd;
	CWnd* m_DlgWnd;
	UINT m_MenuId;
	TCHAR m_Ini[MAX_PATH];

	double m_ZoomRatio;
	DWORD m_ZoomType;

	void SetClassName(LPCTSTR szElementId, CString className);
	void SetTitle(LPCTSTR szElementId, CString title);

	void SetElementPropertyEx(LPCTSTR szElementId, DISPID dispid, CString className);
	void SetElementOuterHtmlEx(LPCTSTR szElementId, CString outerHtml);
	void SetElementInnerHtmlEx(LPCTSTR szElementId, CString innerHtml);
	INT CallScript(CString funcition, CString argument);
	void EnableDpiAware();
	DWORD ChangeZoomType(DWORD zoomType);
	double GetZoomRatio();
	CString i18n(CString section, CString key, BOOL inEnglish = FALSE);
	void SetLabel(CString &label, CString element, CString title);
	void OpenUrl(CString url);
	BOOL ClickCheck();
	BOOL IsLoadHtmlCompleted();

	void HideControl(int nIDDlgItem);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
	virtual void PostNcDestroy();
	virtual BOOL OnAmbientProperty(COleControlSite* pSite, DISPID dispid, VARIANT* pvar);
	virtual STDMETHODIMP GetOptionKeyPath(LPOLESTR *pchKey, DWORD dw);
	virtual STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
	virtual void InitDialogComplete();

	DECLARE_MESSAGE_MAP()

	DECLARE_DHTML_EVENT_MAP()

};
