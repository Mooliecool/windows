========================================================================
    ACTIVEX CONTROL DLL : MFCSafeActiveX Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

By default, MFC ActiveX controls (e.g. the MFCActiveX sample) are not marked 
as Safe for Scripting and Safe for Initialization. This becomes apparent when 
the control is hosted in Internet Explorer with the security level set to 
medium or high. In either of these modes, warnings may be displayed that the 
control's data is not safe or that the control may not be safe for scripts to 
use. 

There are two methods to mark an ActiveX control as safe for scripting and 
initialization, and eliminate these warnings and errors.

1. Implement the IObjectSafety interface in the control.

2. Modify the control's DllRegisterServer function to mark the control "safe"
in the registry. It results in these entries in the registry:

	HKEY_CLASSES_ROOT\Component Categories\
	{7DD95801-9882-11CF-9FA9-00AA006C42C4}  // CATID_SafeForScripting
   
	HKEY_CLASSES_ROOT\Component Categories\
	{7DD95802-9882-11CF-9FA9-00AA006C42C4}  // CATID_SafeForInitializing

	// register the control as safe for scripting
	HKEY_CLASSES_ROOT\CLSID\{"your controls GUID"}\Implemented Categories\
	{7DD95801-9882-11CF-9FA9-00AA006C42C4}  

	// register the control as safe for initialization
	HKEY_CLASSES_ROOT\CLSID\{"your controls GUID"}\Implemented Categories\
	{7DD95802-9882-11CF-9FA9-00AA006C42C4}  

This sample convers the second of these methods.

MFCSafeActiveX exposes the following items:

1. A MFC ActiveX control short-named MFCSafeActiveX that is safe for 
scripting and initialization

Program ID: MFCSAFEACTIVEX.MFCSafeActiveXCtrl.1
CLSID_MFCSafeActiveX: 1EBAE592-7515-43C2-A6F1-CDEEDF3FD82B
DIID__DMFCSafeActiveX: 6267760D-4EDC-430A-A94F-1181971ABA02
DIID__DMFCSafeActiveXEvents: 050C9E59-ADA3-440A-92B4-59AE97009569
LIBID_MFCSafeActiveXLib: 098DB52D-2AAE-499B-B959-A430BA0FF357

Dialogs:
// The main dialog of the control
IDD_MAINDIALOG
// The property page of the control
IDD_PROPPAGE_MFCACTIVEX

Properties:
// With both get and set accessor methods
FLOAT FloatProperty

Methods:
// HelloWorld returns a BSTR "HelloWorld"
BSTR HelloWorld(void);
// GetProcessThreadID outputs the running process ID and thread ID
void GetProcessThreadID(LONG* pdwProcessId, LONG* pdwThreadId);

Events:
// FloatPropertyChanging is fired before new value is set to the 
// FloatProperty property. The Cancel parameter allows the client to cancel 
// the change of FloatProperty.
void FloatPropertyChanging(FLOAT NewValue, VARIANT_BOOL* Cancel);


/////////////////////////////////////////////////////////////////////////////
Project Relation:

HTMLEmbedActiveX -> MFCSafeActiveX
HTMLEmbedActiveX demonstrates the use of the safe MFC ActiveX control. 


/////////////////////////////////////////////////////////////////////////////
Build:

To build MFCSafeActiveX, 1. run Visual Studio as administrator because the 
control needs to be registered into HKCR. 2. Be sure to build the 
MFCSafeActiveX project using the Release configuration.


/////////////////////////////////////////////////////////////////////////////
Creation:

A. Creating the project

Step1. Create a Visual C++ / MFC / MFC ActiveX Control project named 
MFCSafeActiveX in Visual Studio 2008.

Step2. In the page Control Settings, select "Create control based on" as 
STATIC. Under "Additional features", check "Activates when visible" and 
"Flicker-free activation", and un-check "Has an About box dialog".

B. Adding a main dialog to the control

Step1. In Resource View, insert a new dialog resource and change the control 
ID to IDD_MAINDIALOG.

Step2. Change the default properties of the dialog to Border - None, 
Style - Child, System Menu - False, Visible - True.

Step3. Create a class for the dialog, by right clicking on the dialog and 
selecting Add Class. Name the class CMainDialog, with the base class CDialog.

Step4. Add the member variable m_MainDialog of the type CMainDialog to the 
class CMFCSafeActiveXCtrl.

Step5. Select the class CMFCSafeActiveXCtrl in Class View. In the Properties 
sheet, select the Messages icon. Add OnCreate for the WM_CREATE message. 

Step6. Open MFCSafeActiveXCtrl.cpp, and add the following code to the 
OnCreate method to create the main dialog.

	m_MainDialog.Create(IDD_MAINDIALOG, this);

Step7. Add the following code to the OnDraw method to size the main dialog 
window and fill the background.

	m_MainDialog.MoveWindow(rcBounds, TRUE);
	CBrush brBackGnd(TranslateColor(AmbientBackColor()));
	pdc->FillRect(rcBounds, &brBackGnd);

C. Adding Properties to the ActiveX control

Step1. In Class View, expand the element MFCSafeActiveXLib. Right click on 
_DMFCSafeActiveX, and click on Add, Add Property. In the Add Property Wizard 
dialog, select FLOAT for Property type, and enter "FloatProperty" for 
property name. Select "Get/Set methods" to create the methods 
GetFloatProperty and SetFloatProperty.

Step2. In the class CMFCSafeActiveXCtrl, add a member variable, FLOAT 
m_FloatField. In the class's contructor, set the variable's default value to 
0.0f.

Step3. Associate the Get/Set methods of FloatProperty with m_FloatField.

D. Adding Methods to the ActiveX control

Step1. In Class View, expand the element MFCSafeActiveXLib. Right click on 
_DMFCSafeActiveX, and click on Add, Add Method. In the Add Method Wizard 
dialog, select BSTR for the return type, and enter "HelloWorld" for Method 
name.

With the almost same steps, the method GetProcessThreadID is added to get the
executing process ID and thread ID:

	void GetProcessThreadID(LONG* pdwProcessId, LONG* pdwThreadId);

E. Adding Events to the ActiveX control

Step1. In Class View, right click on CMFCSafeActiveXCtrl, select Add, Add 
Event. In the Add Event Wizard, enter "FloatPropertyChanging" for Event name 
and add two parameters: FLOAT NewValue, VARIANT_BOOL* Cancel. 

Step2. The event "FloatPropertyChanging" is fired in SetFloatProperty:

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
			m_MainDialog.m_StaticFloatProperty.SetWindowTextW(strFloatProp);
		}
		// else, do nothing.
	}

F. Marking the control as Safe for Scripting and Initialization

Step1. Implement the CreateComponentCategory, RegisterCLSIDInCategory, and 
UnRegisterCLSIDInCategory helper functions in cathelp.h/cpp files.

Step2. Modify MFCSafeActiveX.cpp to add CLSID_SafeItem at the beginning of 
the file. The value of CLSID_SafeItem is taken from IMPLEMENT_OLECREATE_EX 
in MFCSafeActiveXCtrl.cpp. It is actually the CLSID of the ActiveX control.

	const CATID CLSID_SafeItem =
	{ 0x1ebae592, 0x7515, 0x43c2,
	{ 0xa6, 0xf1, 0xcd, 0xee, 0xdf, 0x3f, 0xd8, 0x2b}};

Step3. Modify the DllRegisterServer method in MFCSafeActiveX.cpp, to mark the
control as safe for initialization and scripting using the helper functions
in cathelp.h.

	// Mark the control as safe for initializing. (Added)

	hr = CreateComponentCategory(CATID_SafeForInitializing, 
		L"Controls safely initializable from persistent data!");
	if (FAILED(hr))
		return hr;

	hr = RegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForInitializing);
	if (FAILED(hr))
		return hr;

	// Mark the control as safe for scripting. (Added)

	hr = CreateComponentCategory(CATID_SafeForScripting,
		L"Controls safely  scriptable!");
	if (FAILED(hr))
		return hr;

	hr = RegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForScripting);
	if (FAILED(hr))
		return hr;

Step4. Modify the DllUnregisterServer method in MFCSafeActiveX.cpp, to remove
the entries from the registry using the helper functions in cathelp.h.

	// Remove entries from the registry.

	hr = UnRegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForInitializing);
	if (FAILED(hr))
		return hr;

	hr = UnRegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForScripting);
	if (FAILED(hr))
		return hr;


/////////////////////////////////////////////////////////////////////////////
References:

How to mark MFC ActiveX controls as Safe for Scripting and Initialization
http://support.microsoft.com/kb/161873

SafeCtl.exe implements IObjectSafety in ActiveX control
http://support.microsoft.com/kb/164119

The ABCs of MFC ActiveX Controls
http://msdn.microsoft.com/en-us/library/ms968497.aspx

Signing and Marking ActiveX Controls
http://msdn.microsoft.com/en-us/library/ms974305.aspx

A Complete ActiveX Web Control Tutorial
http://www.codeproject.com/KB/COM/CompleteActiveX.aspx

Packaging ActiveX Controls
http://msdn.microsoft.com/en-us/library/aa751974.aspx


/////////////////////////////////////////////////////////////////////////////
