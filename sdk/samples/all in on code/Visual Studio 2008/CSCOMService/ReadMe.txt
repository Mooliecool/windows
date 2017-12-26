========================================================================
    WINDOWS SERVICE : CSCOMService Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

CSCOMService demonstrates an out-of-process COM/DCOM service in the form of  
Windows Service (EXE), which is implemented entirely in Visual C#. 

CSCOMService exposes the following item:

1. SimpleObject

Program ID: CSCOMService.SimpleObject
CLSID_SimpleObject: E2EDB864-02DB-4130-BEE4-2E35B30BBF3B
IID_ISimpleObject: 83C40736-3189-44bc-AB0F-9FB3703EA172
DIID_ISimpleObjectEvents: 7A11E6DA-DD09-404c-8731-DB917E783501
AppID: 2E78BFC7-FDD9-4b87-BB6F-470D08399DD1

Properties:
// With both get and set accessor methods
float FloatProperty

Methods:
// HelloWorld returns a string "HelloWorld"
string HelloWorld();
// GetProcessThreadID outputs the running process ID and thread ID
void GetProcessThreadID(out uint processId, out uint threadId);

Events:
// FloatPropertyChanging is fired before new value is set to the 
// FloatProperty property. The Cancel parameter allows the client to cancel 
// the change of FloatProperty.
void FloatPropertyChanging(float NewValue, ref bool Cancel);


/////////////////////////////////////////////////////////////////////////////
Project Relation:

CSCOMService - ATLCOMService
CSCOMService and ATLCOMService, implemented in different languages, are our-
of-process COM components in the form of Windows Service.

CSCOMService - CSExeCOMServer - CSDllCOMServer
All are COM components written in Visual C#. CSCOMService is an out-of-
process component in the form of Windows Service. CSExeCOMServer is an out-
of-process component in the form of local server. CSDllCOMServer is an in-
process component in the form of DLL.


/////////////////////////////////////////////////////////////////////////////
Deployment:

A. Setup

Regasm.exe CSCOMService.exe
It registers the types that are COM-visible in CSCOMService.exe.

Installutil.exe CSCOMService.exe
It installs CSCOMService.exe into SCM as a Windows Service.

B. Cleanup

Regasm.exe /u CSCOMService.exe
It unregisters the types that are COM-visible in CSCOMService.exe.

Installutil.exe /u CSCOMService.exe
It uninstalls CSCOMService from SCM.


/////////////////////////////////////////////////////////////////////////////
Creation:

A. Creating the project

Step1. Create a Visual C# / Windows / Windows Service project named 
CSCOMService in Visual Studio 2008.

B. Adding the COMHelper class

COMHelper provides the helper functions to register/unregister COM servers 
and encapsulates the native COM APIs to be used in .NET.

C. Adding the Windows Service

Step1. Rename the existing Windows Service component as COMServie.

Step2. Double-click COMService.cs to open the component in the Visual Studio 
designer. In the Properties dialog, set the "ServiceName" property as 
"CSCOMService". 

Step3. In the Properties dialog, click the "Add Installer" button. This 
generates a Project Installer component. Rename it as COMServiceInstaller. 

Step4. Double-click COMServiceInstaller.cs to open the component in the 
Visual Studio designer. It contains a ServiceProcessInstaller, and a 
ServiceInstaller. Set the Account property of ServiceProcessInstaller as 
LocalSystem so that the service runs as the SYSTEM account. 

D. Configuring security access of the service.

Step1. In the contructor of the Service class, call CoInitializeSecurity to, 
for example, allow only administrators to call in.

	// Initialize COM security
	int hResult = COMNative.CoInitializeSecurity(
		IntPtr.Zero,    // Add your security descriptor here
		-1, IntPtr.Zero, IntPtr.Zero, RPC_C_AUTHN_LEVEL.PKT_PRIVACY,
		RPC_C_IMP_LEVEL.IDENTIFY, IntPtr.Zero, 
		EOLE_AUTHENTICATION_CAPABILITIES.DISABLE_AAA | 
		EOLE_AUTHENTICATION_CAPABILITIES.SECURE_REFS |
		EOLE_AUTHENTICATION_CAPABILITIES.NO_CUSTOM_MARSHAL,
        IntPtr.Zero);

E. Adding the COM-visible class SimpleObject

Step1. Define a "public" COM-visible interface ISimpleObject to describe 
the COM interface of the coclass. Specify its GUID, aka IID, using 
GuidAttribute: 

	[Guid("83C40736-3189-44bc-AB0F-9FB3703EA172"), ComVisible(true)]

In this way, IID of the COM object is a fixed value. By default, the 
interfaces used by a .NET Class are transformed to dual interfaces 
[InterfaceType(ComInterfaceType.InterfaceIsDual)] in the IDL. This allows the
client to get the best of both early binding and late binding. Other options  
are [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)] and 
[InterfaceType(ComInterfaceType.InterfaceIsIDispatch)].

Step2. Inside the interface ISimpleObject, define the prototypes of the 
properties and methods to be exported. 

Step3. Define a "public" COM-visible interface ISimpleObjectEvents to 
describe the events the coclass can sink. Specify its GUID, aka the Events Id, 
using GuidAttribute:

	[Guid("7A11E6DA-DD09-404c-8731-DB917E783501"), ComVisible(true)]

Decorate the interface as an IDispatch interface:

	[InterfaceType(ComInterfaceType.InterfaceIsIDispatch)]

Step4. Inside the interface ISimpleObjectEvents, define the prototype of
the events to be exported.

Step5. Define a "public" COM-visible class SimpleObject that implements 
the interface ISimpleObject. Attach the attribute 
[ClassInterface(ClassInterfaceType.None)] to it, which tells the type-library
generation tools that we do not require a Class Interface. This ensures that 
the ISimpleObject interface is the default interface. In addition, specify 
the GUID of the class, aka CLSID, using the Guid attribute:

	[Guid("E2EDB864-02DB-4130-BEE4-2E35B30BBF3B"), ComVisible(true)]

In this way, CLSID of the COM object is a fixed value. Last, decorate the 
class with a ComSourceInterface attribute:

	[ComSourceInterfaces(typeof(ISimpleObjectEvents))]

ComSourceInterfaces identifies a list of interfaces that are exposed as	COM 
event sources for the attributed class.

Step6. Make sure that the constructor of the class SimpleObject is not 
private (we can either add a public constructor or use the default one), so 
that the COM object is creatable from the COM aware clients.

Step7. Inside SimpleObject, implement the interface ISimpleObject by 
writing the body of the property FloatProperty and the methods HelloWorld,  
GetProcessThreadID.

F. Registering SimpleObject in the registry

Additional registry keys and values are required for the COM service. The 
default COM registration routine in Regasm.exe only works for InprocServer 
in the form of DLL. In order to register the LocalServer, and allow remote 
activation (DCOM), we need to customize the registration routine to change 
InprocServer32 to LocalServer, and to set AppID appropriately.

Step1. Inside SimpleObject, add the functions Register and Unregister, and 
decorate them with ComRegisterFunctionAttribute and 
ComUnregisterFunctionAttribute. The custom routine gets called after Regasm 
finishes the default behaviors. The Register and Unregister functions call 
the helper methods in COMHelper.

G. Registering ClassFactory of SimpleObject

Step1. Create a ClassFactory class that realizes the IClassFactory interface 
for SimpleObject.

	/// <summary>
	/// Class factory for the class SimpleObject.
	/// </summary>
	internal class SimpleObjectClassFactory : IClassFactory
	{
		public int CreateInstance(IntPtr pUnkOuter, ref Guid riid, 
			out IntPtr ppvObject)
		{
			ppvObject = IntPtr.Zero;

			if (pUnkOuter != IntPtr.Zero)
                Marshal.ThrowExceptionForHR(COMNative.CLASS_E_NOAGGREGATION);

			if (riid == new Guid(SimpleObject.ClassId) || 
				riid == new Guid(COMNative.GuidIUnknown))
				// Create the instance of the .NET object
				ppvObject = Marshal.GetComInterfaceForObject(
					new SimpleObject(), typeof(ISimpleObject));
			else
				// The object that ppvObject points to does not support the 
				// interface identified by riid.
				Marshal.ThrowExceptionForHR(COMNative.E_NOINTERFACE);
			return 0;   // S_OK
		}

		public int LockServer(bool fLock)
		{
			return 0;   // S_OK
		}
	}

Step2. Register the class factory of SimpleObject using the standard  
CoRegisterClassObject API when the service starts. Please note that PInvoking 
CoRegisterClassObject to register COM objects is a technique which is not 
supported.

	// Register the SimpleObject class object on start
	int hResult = COMNative.CoRegisterClassObject(
		ref clsidSimpleObj,                 // CLSID to be registered
		new SimpleObjectClassFactory(),   // Class factory
		CLSCTX.LOCAL_SERVER,                // Context to run
		REGCLS.MULTIPLEUSE, 
		out _cookie);

Step3. Revoke the registration of SimpleObject using the 
CoRevokeClassObject API when the service stops.

	COMNative.CoRevokeClassObject(_cookie);

H. Configuring and building the project as a COM service

Step1. Open the Properties page of the project and turn to Build Events.

Step2. In Post-build event command line, enter the commands:

	Regasm.exe "$(TargetPath)"
	Installutil.exe /u "$(TargetPath)"
	Installutil.exe "$(TargetPath)" 

The commands register the COM-visible types (e.g. SimpleObject) in the 
registry, and install CSCOMService into SCM as a Windows Service. 


/////////////////////////////////////////////////////////////////////////////
References:

How to write a DCOM server in C# 
http://blogs.msdn.com/adioltean/archive/2004/06/18/159479.aspx

Building COM Servers in .NET 
http://www.codeproject.com/KB/COM/BuildCOMServersInDotNet.aspx


/////////////////////////////////////////////////////////////////////////////