#include "stdafx.h"
#include "MFCSafeActiveX.h"
#include "MFCSafeActiveXCtrl.h"
#include "MFCSafeActiveXPropPage.h"

#include <mshtml.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CMFCSafeActiveXCtrl, COleControl)


// Message map
BEGIN_MESSAGE_MAP(CMFCSafeActiveXCtrl, COleControl)
	ON_MESSAGE(OCM_COMMAND, &CMFCSafeActiveXCtrl::OnOcmCommand)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// Dispatch map
BEGIN_DISPATCH_MAP(CMFCSafeActiveXCtrl, COleControl)
	DISP_FUNCTION_ID(CMFCSafeActiveXCtrl, "HelloWorld", dispidHelloWorld, HelloWorld, VT_BSTR, VTS_NONE)
	DISP_PROPERTY_EX_ID(CMFCSafeActiveXCtrl, "FloatProperty", dispidFloatProperty, GetFloatProperty, SetFloatProperty, VT_R4)
	DISP_FUNCTION_ID(CMFCSafeActiveXCtrl, "GetProcessThreadID", dispidGetProcessThreadID, GetProcessThreadID, VT_EMPTY, VTS_PI4 VTS_PI4)
END_DISPATCH_MAP()


// Event map
BEGIN_EVENT_MAP(CMFCSafeActiveXCtrl, COleControl)
	EVENT_CUSTOM_ID("FloatPropertyChanging", eventidFloatPropertyChanging, FloatPropertyChanging, VTS_R4 VTS_PBOOL)
END_EVENT_MAP()


// Property pages
// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CMFCSafeActiveXCtrl, 1)
	PROPPAGEID(CMFCSafeActiveXPropPage::guid)
END_PROPPAGEIDS(CMFCSafeActiveXCtrl)


// Initialize class factory and guid
IMPLEMENT_OLECREATE_EX(CMFCSafeActiveXCtrl, "MFCSAFEACTIVEX.MFCSafeActiveXCtrl.1",
	0x1ebae592, 0x7515, 0x43c2, 0xa6, 0xf1, 0xcd, 0xee, 0xdf, 0x3f, 0xd8, 0x2b)


// Type library ID and version
IMPLEMENT_OLETYPELIB(CMFCSafeActiveXCtrl, _tlid, _wVerMajor, _wVerMinor)


// Interface IDs
const IID BASED_CODE IID_DMFCSafeActiveX =
		{ 0x6267760D, 0x4EDC, 0x430A, { 0xA9, 0x4F, 0x11, 0x81, 0x97, 0x1A, 0xBA, 0x2 } };
const IID BASED_CODE IID_DMFCSafeActiveXEvents =
		{ 0x50C9E59, 0xADA3, 0x440A, { 0x92, 0xB4, 0x59, 0xAE, 0x97, 0x0, 0x95, 0x69 } };


// Control type information
static const DWORD BASED_CODE _dwMFCSafeActiveXOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CMFCSafeActiveXCtrl, IDS_MFCSAFEACTIVEX, _dwMFCSafeActiveXOleMisc)


// CMFCSafeActiveXCtrl::CMFCSafeActiveXCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CMFCSafeActiveXCtrl
BOOL CMFCSafeActiveXCtrl::CMFCSafeActiveXCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(AfxGetInstanceHandle(), m_clsid,
			m_lpszProgID, IDS_MFCSAFEACTIVEX, IDB_MFCSAFEACTIVEX, 
			afxRegApartmentThreading, _dwMFCSafeActiveXOleMisc, _tlid,
			_wVerMajor, _wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


// Licensing strings

static const TCHAR BASED_CODE _szLicFileName[] = _T("MFCSafeActiveX.lic");

static const WCHAR BASED_CODE _szLicString[] =
	L"Copyright (c) 2009 ";


// CMFCSafeActiveXCtrl::CMFCSafeActiveXCtrlFactory::VerifyUserLicense -
// Checks for existence of a user license
BOOL CMFCSafeActiveXCtrl::CMFCSafeActiveXCtrlFactory::VerifyUserLicense()
{
	return AfxVerifyLicFile(AfxGetInstanceHandle(), _szLicFileName,
		_szLicString);
}


// CMFCSafeActiveXCtrl::CMFCSafeActiveXCtrlFactory::GetLicenseKey -
// Returns a runtime licensing key
BOOL CMFCSafeActiveXCtrl::CMFCSafeActiveXCtrlFactory::GetLicenseKey(DWORD dwReserved,
	BSTR FAR* pbstrKey)
{
	if (pbstrKey == NULL)
		return FALSE;

	*pbstrKey = SysAllocString(_szLicString);
	return (*pbstrKey != NULL);
}


// CMFCSafeActiveXCtrl::CMFCSafeActiveXCtrl - Constructor
CMFCSafeActiveXCtrl::CMFCSafeActiveXCtrl() : m_fField(0.0f)
{
	InitializeIIDs(&IID_DMFCSafeActiveX, &IID_DMFCSafeActiveXEvents);

	m_pMainDialog = new CMainDialog(this);
}


// CMFCSafeActiveXCtrl::~CMFCSafeActiveXCtrl - Destructor
CMFCSafeActiveXCtrl::~CMFCSafeActiveXCtrl()
{
	// TODO: Cleanup your control's instance data here.

	if (m_pMainDialog)
		delete m_pMainDialog;
}


// CMFCSafeActiveXCtrl::OnDraw - Drawing function
void CMFCSafeActiveXCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (!pdc)
		return;

	// To size the main dialog window and fill the background
	m_pMainDialog->MoveWindow(rcBounds, TRUE);
	CBrush brBackGnd(TranslateColor(AmbientBackColor()));
	pdc->FillRect(rcBounds, &brBackGnd);

	DoSuperclassPaint(pdc, rcBounds);
}


// CMFCSafeActiveXCtrl::DoPropExchange - Persistence support
void CMFCSafeActiveXCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
}


// CMFCSafeActiveXCtrl::GetControlFlags -
// Flags to customize MFC's implementation of ActiveX controls.
DWORD CMFCSafeActiveXCtrl::GetControlFlags()
{
	DWORD dwFlags = COleControl::GetControlFlags();

	// The control will not be redrawn when making the transition
	// between the active and inactivate state.
	dwFlags |= noFlickerActivate;
	return dwFlags;
}


// CMFCSafeActiveXCtrl::OnResetState - Reset control to default state
void CMFCSafeActiveXCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


// CMFCSafeActiveXCtrl::PreCreateWindow - Modify parameters for CreateWindowEx
BOOL CMFCSafeActiveXCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.lpszClass = _T("STATIC");
	return COleControl::PreCreateWindow(cs);
}


// CMFCSafeActiveXCtrl::IsSubclassedControl - This is a subclassed control
BOOL CMFCSafeActiveXCtrl::IsSubclassedControl()
{
	return TRUE;
}


// CMFCSafeActiveXCtrl::OnOcmCommand - Handle command messages
LRESULT CMFCSafeActiveXCtrl::OnOcmCommand(WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32
	WORD wNotifyCode = HIWORD(wParam);
#else
	WORD wNotifyCode = HIWORD(lParam);
#endif

	// TODO: Switch on wNotifyCode here.
	return 0;
}


// CMFCSafeActiveXCtrl message handlers
int CMFCSafeActiveXCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (m_pMainDialog)
	{
		// To create the main dialog
		m_pMainDialog->Create(IDD_MAINDIALOG, this);
	}
	return 0;
}


BSTR CMFCSafeActiveXCtrl::HelloWorld(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;
	strResult = TEXT("HelloWorld");
	
	return strResult.AllocSysString();
}


FLOAT CMFCSafeActiveXCtrl::GetFloatProperty(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return this->m_fField;
}


void CMFCSafeActiveXCtrl::SetFloatProperty(FLOAT newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Fire the event, FloatPropertyChanging
	VARIANT_BOOL cancel = VARIANT_FALSE; 
	FloatPropertyChanging(newVal, &cancel);

	if (cancel == VARIANT_FALSE)
	{
		m_fField = newVal;	// Save the new value
		SetModifiedFlag();

		// Display the new value in the control UI
		CString strFloatProp;
		strFloatProp.Format(_T("%f"), m_fField);
		m_pMainDialog->m_StaticFloatProperty.SetWindowTextW(strFloatProp);
	}
	// else, do nothing.
}


void CMFCSafeActiveXCtrl::GetProcessThreadID(LONG* pdwProcessId, LONG* pdwThreadId)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pdwProcessId = GetCurrentProcessId();
	*pdwThreadId = GetCurrentThreadId();
}


void CMFCSafeActiveXCtrl::CallHtmlScript(LPTSTR pszScript)
{
	HRESULT hr;

	IOleClientSite *pOleClientSite = this->GetClientSite();
	ASSERT(pOleClientSite);

	IOleContainer *pOleContainer = NULL;
	hr = pOleClientSite->GetContainer(&pOleContainer);
	ASSERT(SUCCEEDED(hr) && pOleContainer);

	IHTMLDocument *pHtmlDoc = NULL;
	hr = pOleContainer->QueryInterface(IID_IHTMLDocument, (void**)&pHtmlDoc);
	ASSERT(SUCCEEDED(hr) && pHtmlDoc);

	// Get the script object (this returns the script object, NOT the script 
	// element(s) that the get_scripts method does).
	IDispatch* pDispScript = NULL;
	hr = pHtmlDoc->get_Script(&pDispScript);
	ASSERT(SUCCEEDED(hr) && pDispScript);
	COleDispatchDriver oddScript;
	oddScript.AttachDispatch(pDispScript);

	// Call the specified (strScript) script function.
	OLECHAR* pScriptName = T2OLE(pszScript);
	DISPID dispidScript;
	hr = pDispScript->GetIDsOfNames(IID_NULL, &pScriptName, 1, 
		LOCALE_SYSTEM_DEFAULT, &dispidScript);
	ASSERT(SUCCEEDED(hr));
	oddScript.InvokeHelper(dispidScript, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);

	// Cleanup
	if (pDispScript)
		pDispScript->Release();
	if (pHtmlDoc)
		pHtmlDoc->Release();
	if (pOleContainer)
		pOleContainer->Release();
	if (pOleClientSite)
		pOleClientSite->Release();
	oddScript.DetachDispatch();
}