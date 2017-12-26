========================================================================
    WINDOWS APPLICATION : CSExeCOMServer Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

CSExeCOMServer demonstrates an out-of-process COM server in the form of local 
server (EXE), which is implemented entirely in Visual C#. 

CSExeCOMServer exposes the following item:

1. SimpleObject

  (Please generate new GUIDs when you are writing your own COM server)
  Program ID: CSExeCOMServer.SimpleObject
  CLSID_SimpleObject: DB9935C1-19C5-4ed2-ADD2-9A57E19F53A3
  IID_ISimpleObject: 941D219B-7601-4375-B68A-61E23A4C8425
  DIID_ISimpleObjectEvents: 014C067E-660D-4d20-9952-CD973CE50436

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
    // FloatProperty property. The Cancel parameter allows the client to 
    // cancel the change of FloatProperty.
    void FloatPropertyChanging(float NewValue, ref bool Cancel);


NOTE: If you are going to deploy this out-of-process COM server to a x64  
operating sytem, you must build the sample project with "Platform target" 
explicitly set to x64 or x86 in the project properties. If you use the 
default "Any CPU", you will see your client application hang while creating 
the COM object for about 2 mins, and give the error: 

"Retrieving the COM class factory for component with CLSID {<clsid>} failed 
due to the following error: 80080005."

The cause is that targeting "Any CPU" confuses the COM activation path on x64 
systems. The on-disk binary has its IMAGE_NT_HEADERS.FileHeader.Machine field 
in the PE header set to IMAGE_FILE_MACHINE_I386. COM would therefore expect 
the launched process to be 32bit. However, the running process is actually 
64bit.


/////////////////////////////////////////////////////////////////////////////
Project Relation:

CSExeCOMServer - VBExeCOMServer - ATLExeCOMServer
CSExeCOMServer, VBExeCOMServer and ATLExeCOMServer, implemented in different 
languages, are our-of-process COM components in the form of local server (EXE).

CSExeCOMServer - CSCOMService - CSDllCOMServer
All are COM components written in Visual C#. CSExeCOMServer is an out-of-
process component in the form of local server. CSCOMService is an out-of-
process component in the form of Windows Service. CSDllCOMServer is an in-
process component in the form of DLL.


/////////////////////////////////////////////////////////////////////////////
Deployment:

A. Setup

Regasm.exe CSExeCOMServer.exe
It registers the types that are COM-visible in CSExeCOMServer.exe.

B. Cleanup

Regasm.exe /u CSExeCOMServer.exe
It unregisters the types that are COM-visible in CSExeCOMServer.exe.


NOTE: If you are going to deploy this out-of-process COM server to a x64  
operating sytem, you must build the sample project with "Platform target" 
explicitly set to x64 or x86 in the project properties. If you use the 
default "Any CPU", you will see your client application hang while creating 
the COM object for about 2 mins, and give the error: 

"Retrieving the COM class factory for component with CLSID {<clsid>} failed 
due to the following error: 80080005."


/////////////////////////////////////////////////////////////////////////////
Creation:

A. Creating the project

Step1. Create a Visual C# / Windows / Console Application project named 
CSExeCOMServer in Visual Studio 2008.

Step2. Open the project's Properties page, and change the output type to 
"Windows Application" in the Application tab. This avoids the console window 
when the executable file is started.

B. Adding the COMHelper class

COMHelper provides the helper functions to register/unregister COM servers 
and encapsulates the native COM APIs to be used in .NET.

C. Adding the ExeCOMServer class

ExeCOMServer encapsulates the skeleton of an out-of-process COM server in C#. 
The class implements the singleton design pattern and it's thread-safe. To 
start the server, call CSExeCOMServer.Instance.Run(). If the server is 
running, the function returns directly. Inside the Run method, it registers 
the class factories for the COM classes to be exposed from the COM server, 
and starts the message loop to wait for the drop of lock count to zero. When 
lock count equals zero, it revokes the registrations and quits the server.

The lock count of the server is incremented when a COM object is created, and 
it's decremented when the object is released (GC-ed). In order that the COM 
objects can be GC-ed in time, ExeCOMServer triggers GC every 5 seconds by 
running a Timer after the server is started.

D. Adding the COM-visible class SimpleObject

Step1. Define a "public" COM-visible interface ISimpleObject to describe 
the COM interface of the coclass. Specify its GUID, aka IID, using 
GuidAttribute: 

	[Guid("941D219B-7601-4375-B68A-61E23A4C8425"), ComVisible(true)]

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

	[Guid("014C067E-660D-4d20-9952-CD973CE50436"), ComVisible(true)]

Decorate the interface as an IDispatch interface:

	[InterfaceType(ComInterfaceType.InterfaceIsIDispatch)]

Step4. Inside the interface ISimpleObjectEvents, define the prototype of
the events to be exported.

Step5. Declare the class ReferenceCountedObject. The class is responsible for 
incrementing the lock count of the COM server in the constructor, and 
decrementing the lock count in the Finalize.

Step6. Define a "public" COM-visible class SimpleObject that implements 
the interface ISimpleObject, and inherits from ReferenceCountedObject. 
Attach the attribute [ClassInterface(ClassInterfaceType.None)] to it, which 
tells the type-library generation tools that we do not require a Class 
Interface. This ensures that the ISimpleObject interface is the default 
interface. In addition, specify the GUID of the class, aka CLSID, using the 
Guid attribute:

	[Guid("DB9935C1-19C5-4ed2-ADD2-9A57E19F53A3"), ComVisible(true)]

In this way, CLSID of the COM object is a fixed value. Last, decorate the 
class with a ComSourceInterface attribute:

	[ComSourceInterfaces(typeof(ISimpleObjectEvents))]

ComSourceInterfaces identifies a list of interfaces that are exposed as	COM 
event sources for the attributed class.

Step7. Make sure that the constructor of the class SimpleObject is not 
private (we can either add a public constructor or use the default one), so 
that the COM object is creatable from the COM aware clients.

Step8. Inside SimpleObject, implement the interface ISimpleObject by 
writing the body of the property FloatProperty and the methods HelloWorld,  
GetProcessThreadID.

E. Registering SimpleObject in the registry

Additional registry keys and values are required for the COM server. The 
default COM registration routine in Regasm.exe only works for InprocServer 
in the form of DLL. In order to register the LocalServer, we need to 
customize the registration routine to change InprocServer32 to LocalServer 
appropriately.

Step1. Inside SimpleObject, add the functions Register and Unregister, and 
decorate them with ComRegisterFunctionAttribute and 
ComUnregisterFunctionAttribute. The custom routine gets called after Regasm 
finishes the default behaviors. The Register and Unregister functions call 
the helper methods in COMHelper.

F. Registering ClassFactory of SimpleObject

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
CoRegisterClassObject API when the server starts (In the PreMessageLoop 
method of ExeCOMServer). Please note that PInvoking CoRegisterClassObject to 
register COM objects is a technique which is not supported.

	// Register the SimpleObject class object on start
	int hResult = COMNative.CoRegisterClassObject(
		ref clsidSimpleObj,                 // CLSID to be registered
		new SimpleObjectClassFactory(),   // Class factory
		CLSCTX.LOCAL_SERVER,                // Context to run
		REGCLS.MULTIPLEUSE, 
		out _cookie);

Step3. Revoke the registration of SimpleObject using the 
CoRevokeClassObject API when the server stops (In the PostMessageLopp method 
of ExeCOMServer). 

	COMNative.CoRevokeClassObject(_cookie);

G. Configuring and building the project as a COM local server

Step1. Open the Properties page of the project and turn to Build Events.

Step2. In Post-build event command line, enter the commands:

	Regasm.exe "$(TargetPath)"

The commands register the COM-visible types (e.g. SimpleObject) in the 
registry.


/////////////////////////////////////////////////////////////////////////////
References:

Building COM Servers in .NET 
http://www.codeproject.com/KB/COM/BuildCOMServersInDotNet.aspx


/////////////////////////////////////////////////////////////////////////////
