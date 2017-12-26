========================================================================
    ACTIVE TEMPLATE LIBRARY : ATLCOMService Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

Acitve Template Library (ATL) is designed to simplify the process of creating
efficient, flexible, lightweight COM components. ATLCOMService provides the 
our-of-process server objects that run in a Windows Service. 

ATLCOMService exposes the following items:

1. An ATL STA Simple Object short-named SimpleObject.

Program ID: ATLCOMService.SimpleObject
CLSID_SimpleObject: 388F1C82-ED00-4966-9590-02F6B9CCA41B
IID_ISimpleObject: 1B877090-76CD-4EDE-8115-EC4CCD9676F3
DIID__ISimpleObjectEvents: 7DACF5E9-2885-4E4E-83DD-CA6CC3A88B6D
LIBID_ATLExeCOMServerLib: CC2CA6F0-2220-4D77-BA46-4BCB62156A28
AppID: 5CDE0403-41B3-45F9-8B6F-49E3193B5425

Properties: 
// With both get and put accessor methods
FLOAT FloatProperty

Methods: 
// HelloWorld returns a BSTR "HelloWorld"
HRESULT HelloWorld([out,retval] BSTR* pRet);
// GetProcessThreadID outputs the running process ID and thread ID
HRESULT GetProcessThreadID([out] LONG* pdwProcessId, [out] LONG* pdwThreadId);

Events:
// FloatPropertyChanging is fired before new value is set to the 
// FloatProperty property. The [in, out] parameter Cancel allows the client
// to cancel the change of FloatProperty.
void FloatPropertyChanging(
	[in] FLOAT NewValue, [in, out] VARIANT_BOOL* Cancel);


/////////////////////////////////////////////////////////////////////////////
Project Relation:

ATLCOMService - ATLExeCOMServer - ATLDllCOMServer
All are COM components written in ATL. ATLDllCOMServer is an in-process
component in the form of DLL, ATLExeCOMServer is an out-of-process component
in the form of EXE (Local Server), and ATLCOMService is an out-of-process 
component in the form of EXE (Local Service).


/////////////////////////////////////////////////////////////////////////////
Deployment:

A. Setup

ATLCOMService.exe /Service
It registers your server in the system registry and as a service. 

B. Cleanup

ATLCOMService.exe /Unregserver
It not only removes the server from the system registry, but also deletes it 
as a service. 


/////////////////////////////////////////////////////////////////////////////
Creation:

A. Creating the project

Step1. Create a Visual C++ / ATL / ATL Project named ATLCOMService in Visual
Studio 2008.

Step2. In the page "Application Settings" of ATL Project Wizard, select the 
server type as Service (EXE). It generates the main project: ATLCOMService, 
and the proxy/stub project: ATLCOMServicePS.

B. Customizing the service in its Overrides

Some famous Overrides are exposed by the service module. For example, 
InitializeSecurity - Override to set security options for the service via 
CoInitializeSecurity. ServiceMain - Called when the service is started.
Handler - Called whenever a control request is received from the service 
control manager. To override these methods, 

Step1. Switch to the Class View of the project, and select the service module 
CATLCOMServiceModule.

Step2. In the Properties dialog, turn to the Overrides page. Select the 
overridable methods in the list, and add the overrides. The code is generated 
in ATLCOMService.cpp. For example, 

	void CATLCOMServiceModule::ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
	{
		// TODO: Add your specialized code here and/or call the base class
		CAtlServiceModuleT<CATLCOMServiceModule,100>::ServiceMain(
			dwArgc, lpszArgv);
	}

C. Adding an ATL Simple Object

Step1. In Solution Explorer, add a new ATL / ATL Simple Object class to the 
project.

Step2. In ATL Simple Object Wizard, specify the short name as 
SimpleObject, and select the threading model as Apartment (corresponding
to STA), select Interface as Dual that supports both IDispatch (late binding)
and vtable binding (early binding). Last, select the Connection points check 
box. This creates the _ISimpleObjectEvents interface in the file 
ATLCOMService.idl. The Connection points option is the prerequisite for the 
component to supporting events.

D. Adding Properties to the ATL Simple Object

Step1. In Class View, find the interface ISimpleObject. Right click it 
and select Add / Add Property in the menu. 

Step2. In Add Property Wizard, specify the property type as FLOAT, property 
name as FloatProperty. Select both Get function and Put function. 
SimpleObject therefore exposes FloatProperty with the get&put accessor 
methods: get_FloatProperty, put_FloatProperty.

Step3. Add a float field, m_fField, to the class CSimpleObject:

	protected:
		// Used by FloatProperty
		float m_fField;
		
Implement the get&put accessor methods of FloatProperty to access m_fField.

	STDMETHODIMP CSimpleObject::get_FloatProperty(FLOAT* pVal)
	{
		*pVal = m_fField;
		return S_OK;
	}

	STDMETHODIMP CSimpleObject::put_FloatProperty(FLOAT newVal)
	{
		m_fField = newVal;
		return S_OK;
	}

E. Adding Methods to the ATL Simple Object

Step1. In Class View, find the interface ISimpleObject. Right-click it
and select Add / Add Method in the menu.

Step2. In Add Method Wizard, specify the method name as HelloWorld. Add a 
parameter whose parameter attributes = retval, parameter type = BSTR*, 
and parameter name = pRet.

Step3. Write the body of HelloWorld as this:

	STDMETHODIMP CSimpleObject::HelloWorld(BSTR* pRet)
	{
		// Allocate memory for the string: 
		*pRet = ::SysAllocString(L"HelloWorld");
		if (pRet == NULL)
			return E_OUTOFMEMORY;

		// The client is now responsible for freeing pbstr
		return S_OK;
	}

With the almost same steps, the method GetProcessThreadID is added to get the
executing process ID and thread ID.

HRESULT GetProcessThreadID([out] LONG* pdwProcessId, [out] LONG* pdwThreadId);

F. Adding Events to the ATL Simple Object

The Connection points option in B/Step2 is the prerequisite for the component
to supporting events.

Step1. In Class View, expand ATLExeCOMServer and ATLExeCOMServerLib to display
_ISimpleObjectEvents.

Step2. Right-click _ISimpleObjectEvents. In the menu, click Add, and 
then click Add Method.

Step3. Select a Return Type of void, enter FloatPropertyChanging in the
Method name box, and add an [in] parameter FLOAT NewValue, and an [in, out] 
parameter VARIANT_BOOL* Cancel. After clicking Finish, 
_ISimpleObjectEvents dispinterface in the ATLExeCOMServer.idl file 
should look like this: 

	dispinterface _ISimpleObjectEvents
	{
		properties:
		methods:
			[id(1), helpstring("method FloatPropertyChanging")] void 
			FloatPropertyChanging(
			[in] FLOAT NewValue, [in,out] VARIANT_BOOL* Cancel);
	};

Step4. Generate the type library by rebuilding the project or by 
right-clicking the ATLCOMService.idl file in Solution Explorer and clicking
Compile on the shortcut menu. Please note: We must compile the IDL file 
BEFORE setting up a connection point.

Step5. Use the Implement Connection Point Wizard to implement the Connection
Point interface: In Class View, right-click the component's implementation 
class CSimpleObject. On the shortcut menu, click Add, and then click 
Add Connection Point. Select _ISimpleObjectEvents from the Source 
Interfaces list and double-click it to add it to the Implement connection 
points column. Click Finish. A proxy class for the connection point will be 
generated (ie. CProxy_ISimpleObjectEvents in this sample) in the file 
_ISimpleObjectEvents_CP.h. This also creates a function with the name
Fire_[eventname] which can be called to raise events in the client. 

Step6. Fire the event in put_FloatProperty:

	STDMETHODIMP CSimpleObject::put_FloatProperty(FLOAT newVal)
	{
		// Fire the event, FloatPropertyChanging
		VARIANT_BOOL cancel = VARIANT_FALSE; 
		Fire_FloatPropertyChanging(newVal, &cancel);

		if (cancel == VARIANT_FALSE)
		{
			m_fField = newVal;	// Save the new value
		} // else, do nothing
		return S_OK;
	}

G. Configuring and building the project as an ATL COM service

Step1. Open SimpleObject.rgs and add the following value the 
ForceRemove {388F1C82-ED00-4966-9590-02F6B9CCA41B} registry key.

	val AppID = s '%APPID%'

The result is like

	ForceRemove {388F1C82-ED00-4966-9590-02F6B9CCA41B} = 
		s 'SimpleObject Class'
	{
		ProgID = s 'ATLCOMService.SimpleObject.1'
		VersionIndependentProgID = s 'ATLCOMService.SimpleObject'
		ForceRemove 'Programmable'
		LocalServer32 = s '%MODULE%'
		val AppID = s '%APPID%'
		'TypeLib' = s '{CC2CA6F0-2220-4D77-BA46-4BCB62156A28}'
	}

This is a known product issue of ATL Simple template in Visual Studio 2008. 
Without the AppID entry, the calls of CoCreateInstance on the client sides 
return 0x80080005. Jigar Mehta's article "CoCreateInstance returns 0x80080005 
for Visual Studio 2008 based ATL service" illustrates the details of the 
issue.

Step2. Right-click the ATLCOMService project and select Properties to open 
its Property Pages. Turn to Build Events / Post Build Event, and change the 
Command Line value from 

	"$(TargetPath)" /RegServer

to 

	"$(TargetPath)" /Service

This registers the server in the system registry and as a service, instead of 
as just a plain EXE.

H. Configuring DCOM-specific settings

DCOM-specific settings of the ATL service can be configured in the DCOMCNFG 
utility.
http://msdn.microsoft.com/en-us/library/wdyy0xsw.aspx


/////////////////////////////////////////////////////////////////////////////
References:

ATL Services
http://msdn.microsoft.com/en-us/library/74y2334x.aspx

CoCreateInstance returns 0x80080005 for Visual Studio 2008 based ATL service
http://blogs.msdn.com/jigarme/archive/2008/05/08/cocreateinstance-returns-0x80080005-for-visual-studio-2008-based-atl-service.aspx


/////////////////////////////////////////////////////////////////////////////
