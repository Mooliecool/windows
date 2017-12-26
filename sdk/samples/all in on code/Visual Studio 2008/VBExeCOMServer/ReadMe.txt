========================================================================
    WINDOWS APPLICATION : VBExeCOMServer Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

VBExeCOMServer demonstrates an out-of-process COM server in the form of local 
server (EXE), which is implemented entirely in Visual Basic.NET 

VBExeCOMServer exposes the following item:

1. SimpleObject

  Program ID: VBExeCOMServer.SimpleObject
  CLSID_SimpleObject: 3CCB29D4-9466-4f3c-BCB2-F5F0A62C2C3C
  IID__SimpleObject: 5EECE765-6416-467c-8D5E-C227F69E7EB7
  DIID___SimpleObjectEvents: 10C862E3-37E6-4e36-96FE-3106477235F1

  Properties:
    ' With both get and set accessor methods
    FloatProperty As Single

  Methods:
    ' HelloWorld returns a string "HelloWorld"
    Function HelloWorld() As String
    ' GetProcessThreadID outputs the running process ID and thread ID
    Sub GetProcessThreadID(ByRef processId As UInteger, 
                           ByRef threadId As UInteger)

  Events:
    ' FloatPropertyChanging is fired before new value is set to the 
    ' FloatProperty property. The Cancel parameter allows the client to 
    ' cancel the change of FloatProperty.
    Event FloatPropertyChanging(ByVal NewValue As Single, 
                                ByRef Cancel As Boolean)


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

VBExeCOMServer - CSExeCOMServer - ATLExeCOMServer
VBExeCOMServer, CSExeCOMServer and ATLExeCOMServer, implemented in different 
languages, are our-of-process COM components in the form of local server (EXE).

VBExeCOMServer - VBCOMService - VBDllCOMServer
All are COM components written in Visual Basic.NET. VBExeCOMServer is an 
out-of-process component in the form of local server. VBCOMService is an 
out-of-process component in the form of Windows Service. VBDllCOMServer is an 
in-process component in the form of DLL.


/////////////////////////////////////////////////////////////////////////////
Deployment:

A. Setup

Regasm.exe VBExeCOMServer.exe
It registers the types that are COM-visible in VBExeCOMServer.exe.

B. Cleanup

Regasm.exe /u VBExeCOMServer.exe
It unregisters the types that are COM-visible in VBExeCOMServer.exe.


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

Step1. Create a Visual Basic / Windows / Console Application project named 
VBExeCOMServer in Visual Studio 2008.

Step2. Open the project's Properties page, and change the output type to 
"Windows Forms Application" in the Application tab. This avoids the console 
window when the executable file is started.

B. Adding the COMHelper class

COMHelper provides the helper functions to register/unregister COM servers 
and encapsulates the native COM APIs to be used in .NET.

C. Adding the ExeCOMServer class

ExeCOMServer encapsulates the skeleton of an out-of-process COM server in 
VB.NET. The class implements the singleton design pattern and it's thread-safe. 
To start the server, call VBExeCOMServer.Instance.Run(). If the server is 
running, the function returns directly. Inside the Run method, it registers 
the class factories for the COM classes to be exposed from the COM server, 
and starts the message loop to wait for the drop of lock count to zero. When 
lock count equals zero, it revokes the registrations and quits the server.

The lock count of the server is incremented when a COM object is created, and 
it's decremented when the object is released (GC-ed). In order that the COM 
objects can be GC-ed in time, ExeCOMServer triggers GC every 5 seconds by 
running a Timer after the server is started.

D. Adding the component SimpleObject

Step1. Add a public class SimpleObject. 

Step2. Inside the SimpleObject class, define Class ID, Interface ID, and 
Event ID:

	ClassId As String = "805303FE-B5A6-308D-9E4F-BF500978AEEB"
    InterfaceId As String = "90E0BCEA-7AFA-362A-A75E-6D07C1C6FC4B"
    EventsId As String = "72D3EFB2-0D88-4ba7-A26B-8FFDB92FEBED"

Step3. Attach ComClassAttribute to the class SimpleObject, and specify its 
_ClassID, _InterfaceID, and _EventID to be the above const values:

    <ComClass(SimpleObject.ClassId, SimpleObject.InterfaceId, _
        SimpleObject.EventsId), ComVisible(True)> _
    Public Class SimpleObject

Step4. Declare the class ReferenceCountedObject. The class is responsible for 
incrementing the lock count of the COM server in the constructor, and 
decrementing the lock count in the Finalize. Inherit SimpleObject from 
ReferenceCountedObject. 

Step5. Add one property (FloatProperty), two methods (HelloWorld, 
GetProcessThreadID) and one event (FloatPropertyChanging) to the component.

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

''' <summary>
''' Class factory for the class SimpleObject.
''' </summary>
Friend Class SimpleObjectClassFactory
    Implements IClassFactory

    Public Function CreateInstance(ByVal pUnkOuter As IntPtr, ByRef riid As Guid, _
                                   <Out()> ByRef ppvObject As IntPtr) As Integer _
                                   Implements IClassFactory.CreateInstance
        ppvObject = IntPtr.Zero

        If (pUnkOuter <> IntPtr.Zero) Then
            ' The pUnkOuter parameter was non-NULL and the object does 
            ' not support aggregation.
            Marshal.ThrowExceptionForHR(COMNative.CLASS_E_NOAGGREGATION)
        End If

        If ((riid = New Guid(SimpleObject.ClassId)) OrElse _
            (riid = New Guid(COMNative.GuidIUnknown))) Then
            ' Create the instance of the .NET object
            ppvObject = Marshal.GetComInterfaceForObject( _
            New SimpleObject, GetType(SimpleObject).GetInterface("_SimpleObject"))
        Else
            ' The object that ppvObject points to does not support the 
            ' interface identified by riid.
            Marshal.ThrowExceptionForHR(COMNative.E_NOINTERFACE)
        End If

        Return 0  ' S_OK
    End Function


    Public Function LockServer(ByVal fLock As Boolean) As Integer _
    Implements IClassFactory.LockServer
        Return 0  ' S_OK
    End Function

End Class

Step2. Register the class factory of SimpleObject using the standard  
CoRegisterClassObject API when the server starts (In the PreMessageLoop 
method of ExeCOMServer). Please note that PInvoking CoRegisterClassObject to 
register COM objects is a technique which is not supported.

	// Register the SimpleObject class object on start
	Dim hResult As Integer = COMNative.CoRegisterClassObject( _
    clsidSimpleObj, New SimpleObjectClassFactory, CLSCTX.LOCAL_SERVER, _
    REGCLS.SUSPENDED Or REGCLS.MULTIPLEUSE, Me._cookieSimpleObj)

Step3. Revoke the registration of SimpleObject using the 
CoRevokeClassObject API when the server stops (In the PostMessageLopp method 
of ExeCOMServer). 

	COMNative.CoRevokeClassObject(Me._cookieSimpleObj)

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
