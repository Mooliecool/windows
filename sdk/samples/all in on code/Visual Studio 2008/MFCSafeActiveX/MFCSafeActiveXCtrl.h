#pragma once
#include "maindialog.h"


class CMFCSafeActiveXCtrl : public COleControl
{
	DECLARE_DYNCREATE(CMFCSafeActiveXCtrl)

// Constructor
public:
	CMFCSafeActiveXCtrl();

// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual DWORD GetControlFlags();

// Implementation
protected:
	~CMFCSafeActiveXCtrl();

	BEGIN_OLEFACTORY(CMFCSafeActiveXCtrl)        // Class factory and guid
		virtual BOOL VerifyUserLicense();
		virtual BOOL GetLicenseKey(DWORD, BSTR FAR*);
	END_OLEFACTORY(CMFCSafeActiveXCtrl)

	DECLARE_OLETYPELIB(CMFCSafeActiveXCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CMFCSafeActiveXCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CMFCSafeActiveXCtrl)		 // Type name and misc status

	// Subclassed control support
	BOOL IsSubclassedControl();
	LRESULT OnOcmCommand(WPARAM wParam, LPARAM lParam);

// Message maps
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	DECLARE_DISPATCH_MAP()

// Event maps
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
		dispidGetProcessThreadID = 3L,
		eventidFloatPropertyChanging = 1L,
		dispidFloatProperty = 2,
		dispidHelloWorld = 1L
	};

	// Pointer to main dialog of the control
	CMainDialog * m_pMainDialog;

	// Call HTML script when the control is hosted in web browser
	void CallHtmlScript(LPTSTR pszScript);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

protected:
	// The float field used by FloatProperty
	FLOAT m_fField;
protected:
	BSTR HelloWorld(void);
	void GetProcessThreadID(LONG* pdwProcessId, LONG* pdwThreadId);
	FLOAT GetFloatProperty(void);
	void SetFloatProperty(FLOAT newVal);

	void FloatPropertyChanging(FLOAT NewValue, VARIANT_BOOL* Cancel)
	{
		FireEvent(eventidFloatPropertyChanging, EVENT_PARAM(VTS_R4 VTS_PBOOL), NewValue, Cancel);
	}
};