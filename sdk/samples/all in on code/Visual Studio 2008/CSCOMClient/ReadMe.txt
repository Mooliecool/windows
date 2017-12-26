========================================================================
    CONSOLE APPLICATION : CSCOMClient Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This Visual C# code example demonstrates the access of COM components from 
the .NET Framework by use of a runtime callable wrapper (RCW) or late binding, 
and the host of ActiveX control in Windows Form.

A. Early binding of COM

Early-binding means that the compiler must have prior knowledge about COM at 
compile time. Early-binding is supported by use of a runtime callable wrapper 
(RCW). The wrapper turns the COM interfaces exposed by the COM component into 
.NET Framework-compatible interfaces, and thus facilitates communication 
between COM and .NET.

The Interop Assembly that is made from a type library is a form of RCW. It 
defines managed interfaces that map to a COM-based type library and that a 
managed client can interact with. To use an interop assembly in Visual Studio, 
first add a reference to the corresponding COM component. Visual Studio will 
automatically generate a local copy of the interop assembly. Type Library 
Importer (Tlbimp.exe) is a standalone tool to convert the type definitions 
found within a COM type library into equivalent definitions in a common 
language runtime assembly. 

Without the use of the interop assembly, a developer may write a custom RCW 
and manually map the types exposed by the COM interface to .NET Framework-
compatible types. 

B. Late binding of COM

Late-binding means that the compiler does not have any prior knowledge about 
the methods and properties in the COM component and it is delayed until 
runtime. The advantage of late binding is that you do not have to use an RCW 
(or build/ship a custom Interop Assembly), and it is more version agnostic. 
The disadvantage is that it is more difficult to program this in Visual C# 
than it is in Visual Basic, and late binding still suffers the performance  
hit of having to find DISPIDs at runtime.

We do late binding in Visual C# through .NET reflection. Reflection is a way 
to determine the type or information about the classes or interfaces. We do 
not need to create RCW for the COM component in late binding as we did in 
early binding. We use System.Type.GetTypeFromProgID to get the type object 
for the COM object, then use System.Activator.CreateInstance to instantiate 
the COM object.

C. Host of ActiveX control

Hosting an ActiveX control requires a RCW of the AcitveX COM object, and a 
class that inherits from System.Windows.Forms.AxHost to wrap the ActiveX COM 
object and expose it as fully featured Windows Forms controls. The stuff can 
be automatically done using the Windows Forms ActiveX Control Importer 
(Aximp.exe) or Visual Studio Windows Forms design environment.

The ActiveX Control Importer generates a class that is derived from the AxHost
class, and compiles it into a library file (DLL) that can be added as a 
reference to your application. Alternatively, you can use the /source switch 
with the ActiveX Control Importer and a C# file is generated for your AxHost 
derived class. You can then make changes to the code and recompile it into a 
library file. 

If you are using Visual Studio as your Windows Forms design environment, you 
can make an ActiveX control available to your application by adding the 
ActiveX control to your Toolbox. To accomplish this, right-click the Toolbox, 
select Choose Items, then browse to the ActiveX control's .OCX or .DLL file. 


/////////////////////////////////////////////////////////////////////////////
Project Relation:
(Relation of the current sample and other samples in 
Microsoft All-In-One Code Framework http://cfx.codeplex.com)

CSCOMClient -> ATLDllCOMServer
CSCOMClient is the client application of the COM server ATLDllCOMServer.

CSCOMClient -> MFCActiveX
CSCOMClient hosts the MFCActiveX control in a Windows Form.

CSCOMClient - VBCOMClient
These samples demonstrate the use of COM objects and ActiveX controls in 
different .NET languages.


/////////////////////////////////////////////////////////////////////////////
Creation:

A. Early-binding to the ATLDllCOMServer component using an interop assembly

Step1. Add a reference to the ATLDllCOMServer COM component by right-clicking 
the project, selecting "Add Reference...", turning to the COM tab, and adding 
ATLDllCOMServer Type Library. Visual Studio will automatically generate a  
local copy of the interop assembly "Interop.ATLDllCOMServerLib.dll". If the 
ATLDllCOMServer Type Library is not in the list, please build and setup the 
ATLDllCOMServer sample first.

Step2. Create a thread whose thread apartement is set to STA, because we are 
going to demonstrate the instantiation and use of a STA COM object: 
ATLDllCOMServer.SimpleObject.

Step3. Create an ATLDllCOMServer.SimpleObject COM object.

	ATLDllCOMServerLib.SimpleObject simpleObj = 
		new ATLDllCOMServerLib.SimpleObject();

Step4. Register the events of the COM object,

	simpleObj.FloatPropertyChanging += new ATLDllCOMServerLib.
		_ISimpleObjectEvents_FloatPropertyChangingEventHandler(
		simpleObj_FloatPropertyChanging);

and define the event handler.

	static void simpleObj_FloatPropertyChanging(
		float NewValue, ref bool Cancel)
	{
	}

Step5. Consume the properties and the methods of the COM object. For example, 

	string strResult = simpleObj.HelloWorld();

Step6. Release the COM object. It is strongly recommended against using 
ReleaseComObject to manually release an RCW object that represents a COM 
component unless you absolutely have to. We should generally let CLR release 
the COM object in the garbage collector. 
Ref: http://blogs.msdn.com/yvesdolc/archive/2004/04/17/115379.aspx

	Marshal.FinalReleaseComObject(simpleObj);

B. Late-binding to the ATLDllCOMServer component through .NET reflection 

Step1. Create a thread whose thread apartement is set to STA, because we are 
going to demonstrate the instantiation and use of a STA COM object: 
ATLDllCOMServer.SimpleObject.

Step2. Create an ATLDllCOMServer.SimpleObject COM object. We use 
System.Type.GetTypeFromProgID to get the type object for the COM object,  
then use System.Activator.CreateInstance to instantiate the COM object. Note  
that the COM object is declared as Object (object simpleObj) without the  
useful type information.

	Type simpleObjType = Type.GetTypeFromProgID(
		"ATLDllCOMServer.SimpleObject");
	object simpleObj = Activator.CreateInstance(simpleObjType);

Step3. Consume the properties and the methods of the COM object through .NET 
reflection. For example, 

	string strResult = simpleObjType.InvokeMember("HelloWorld",
		BindingFlags.InvokeMethod, null, simpleObj, null) as string;

Step4. Release the COM object. It is strongly recommended against using 
ReleaseComObject to manually release an RCW object that represents a COM 
component unless you absolutely have to. We should generally let CLR release 
the COM object in the garbage collector. 
Ref: http://blogs.msdn.com/yvesdolc/archive/2004/04/17/115379.aspx

	Marshal.FinalReleaseComObject(simpleObj);

C. Hosting the MFCActiveX control in a Windows Form

Step1. Open FrmMain in Visual Studio Windows Forms design environment. Right-
click the Toolbox, select Choose Items, switch to the tab COM Components, 
and browse to the MFCActiveX control's .OCX file. The MFCActiveX control 
should appear in the Toolbox then. If MFCActiveX is not in the list, please 
build and setup the MFCActiveX sample first.

Step2. Drag and drop the MFCActiveX control to the WinForm designer. Name the 
control variable as axMFCActiveX1.

Step3. Access the properties and methods of the ActiveX control like this:

	float fProp = this.axMFCActiveX1.FloatProperty;

Step4. To access the events of the control (e.g. FloatPropertyChanging in 
MFCActiveX), select the ActiveX control in WinForm designer. In the Visual 
Studio Properties dialog, turn to the event list of the control. Double-click
the FloatPropertyChanging event in the list, which generates the event 
handler in code-behind:

	private void axMFCActiveX1_FloatPropertyChanging(object sender, 
		AxMFCActiveXLib._DMFCActiveXEvents_FloatPropertyChangingEvent e)
	{
	}

The NewValue and Cancel parameters are encapsulated in 
AxMFCActiveXLib._DMFCActiveXEvents_FloatPropertyChangingEvent e.

In the event handler, pop up a message box to allow selecting 
OK or Cancel, then pass the user's selection back to the control through the
e.cancel parameter.


/////////////////////////////////////////////////////////////////////////////
References:

COM Interop Part 1: C# Client Tutorial
http://msdn.microsoft.com/en-us/library/aa645736.aspx

Understanding Classic COM Interoperability With .NET Applications By Aravind
http://www.codeproject.com/KB/COM/cominterop.aspx

Calling COM Components from .NET Clients
http://msdn.microsoft.com/en-us/library/ms973800.aspx

Microsoft .NET/COM Migration and Interoperability
http://msdn.microsoft.com/en-us/library/ms978506.aspx

Type Library Importer (Tlbimp.exe)
http://msdn.microsoft.com/en-us/library/tt0cf3sx.aspx

Using early binding and late binding in Automation
http://support.microsoft.com/kb/245115

MSDN: AxHost Class
http://msdn.microsoft.com/en-us/library/system.windows.forms.axhost.aspx

Windows Forms ActiveX Control Importer (Aximp.exe)
http://msdn.microsoft.com/en-us/library/8ccdh774.aspx


/////////////////////////////////////////////////////////////////////////////
