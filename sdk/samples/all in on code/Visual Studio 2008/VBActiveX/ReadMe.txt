========================================================================
    ACTIVEX CONTROL DLL : VBActiveX Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The sample demonstrates an ActiveX control written in VB.NET. ActiveX 
controls (formerly known as OLE controls) are small program building blocks 
that can work in a variety of different containers, ranging from software 
development tools to end-user productivity tools. For example, it can be used 
to create distributed applications that work over the Internet through web 
browsers. ActiveX controls can be written in MFC, ATL, C++, C#, Borland  
Delphi and Visual Basic. In this sample, we focus on writing an ActiveX 
control using VB.NET. We will go through the basic steps of adding UI, 
properties, methods, and events to the control. Please note that ActiveX 
controls or COM components written in .NET languages cannot be referenced by 
.NET applications in the form of interop assemblies. If you "add reference" 
to such a TLB, or drag & drop such an ActiveX control to your .NET 
application, you will get an error "The ActiveX type library 'XXXXX.tlb' was 
exported from a .NET assembly and cannot be added as a reference.". The 
correct approach is to add a reference to the .NET assembly directly.

VBActiveX exposes the following items:

1. A VB.NET ActiveX control.

Program ID: VBActiveX.VBActiveXCtrl
CLSID_VBActiveXCtrl: 81F71529-0F1C-3905-94E5-82ADBB9DFB5B
DIID__VBActiveXCtrl: 916F51ED-11DA-317B-988E-427C2B5032C5
DIID___VBActiveXCtrl: 2994FDB9-EBD0-4708-9197-D2B90E261E4F (EventID)
LIBID_VBActiveX: 418E907A-3490-4879-8C15-FFC464374407

UI:
' The main UI of the control
VBActiveXCtrl

Properties:
' Typical control properties
Visible As Boolean
Enabled As Boolean
ForeColor As Integer
BackColor As Integer
' Custom properties
FloatProperty As Single

Methods:
' Typical control methods
Sub Refresh()
' Custom methods
Function HelloWorld() As String

Events:
' Typical control events
Event Click()
' Custom events
' FloatPropertyChanging is fired before new value is set to the FloatProperty
' property. The Cancel parameter allows the client to cancel the change of
' FloatProperty.
Event FloatPropertyChanging(ByVal NewValue As Single, ByRef Cancel As Boolean)


/////////////////////////////////////////////////////////////////////////////
Project Relation:

VBActiveX - MFCActiveX - CSActiveX
These samples expose the same UI and the same set of properties, methods, and
events, but they are implemented in different languages.


/////////////////////////////////////////////////////////////////////////////
Creation:

A. Creating the project

Step1. Create a Visual Basic / Class Library project named VBActiveX in 
Visual Studio 2008. Delete the default Class1.vb file.

Step2. In order to make the .NET assembly COM-visible, first, open the 
property of the project. Click the Assembly Information button in the 
Application page, and select the "Make Assembly COM-Visible" box. This 
corresponds to the assembly attribute ComVisible in AssemblyInfo.vb:

	<Assembly: ComVisible(True)> 

The GUID value in the dialog is the libid of the component:

	<Assembly: Guid("418e907a-3490-4879-8c15-ffc464374407")> 

Second, in the Compile page of the project's property, select the option 
"Register for COM interop". This option specifies whether your managed 
application will expose a COM object (a COM-callable wrapper) that allows a 
COM object to interact with your managed application.

B. Adding the ActiveXCtrlHelper class

ActiveXCtrlHelper provides the helper functions to register/unregister an 
ActiveX control, and helps to handle the focus and tabbing across the 
container and the .NET controls.

C. Adding a user control and expose it as an ActiveX control

Step1. Right-click the project and choose Add / User Control in the context 
menu. Name the control as VBActiveXCtrl. Next, double-click the control to 
open its design view and design the UI.

Step2. Inside the VBActiveXCtrl class, define Class ID, Interface ID, and 
Event ID:

	ClassId As String = "81F71529-0F1C-3905-94E5-82ADBB9DFB5B"
    InterfaceId As String = "916F51ED-11DA-317B-988E-427C2B5032C5"
    EventsId As String = "2994FDB9-EBD0-4708-9197-D2B90E261E4F"

Attach ComClassAttribute to the class VBActiveXCtrl, and specify its 
_ClassID, _InterfaceID, and _EventID to be the above const values:

	<ComClass(VBActiveXCtrl.ClassId, VBActiveXCtrl.InterfaceId, _
			  VBActiveXCtrl.EventsId)> _
	Public Class VBActiveXCtrl

The Microsoft.VisualBasic.ComClassAttribute attribute instructs the compiler 
to add metadata that allows a class to be exposed as a COM object in an easy
way, when compared with the use of COM interface and the ComSourceInterfaces 
attribute in C# (see: CSActiveX). 

D. ActiveX Control Registration

Additional registry keys/values are required to be set for ActiveX controls 
when compared with ordinary COM components. The default COM registration 
routine does not meet the need. Inside VBActiveXCtrl, add the functions 
Register, Unregister and decorate them with ComRegisterFunctionAttribute, 
ComUnregisterFunctionAttribute. The custom routine gets called after Regasm 
finishes the default behaviors. The Register and Unregister functions call 
the helper methods in ActiveXCtrlHelper.

E. Adding Properties to the ActiveX control

Step1. Inside the VBActiveXCtrl class, add a public property. All public 
properties are exposed from the control. For example, 

	Public Property FloatProperty() As Single
		Get
			Return Me.fField
		End Get
		Set(ByVal value As Single)
			Me.fField = value
		End Set
	End Property

F. Adding Methods to the ActiveX control

Step1. Inside the VBActiveXCtrl class, add a public method. All public 
methods are exposed from the control. For example, 

	Public Function HelloWorld() As String
		Return "HelloWorld"
	End Function

G. Adding Events to the ActiveX control

Step1. Inside the VBActiveXCtrl class, add a public event. For example, 

	Public Event FloatPropertyChanging(ByVal NewValue As Single, _
	                                   ByRef Cancel As Boolean)

Then raise the event in the proper places. For example, 

	Dim cancel As Boolean = False
	RaiseEvent FloatPropertyChanging(value, cancel)

H. Adding the ToolBox Bitmap resource

The ToolBox bitmap resource is specified in the regsitry key:
HKCR\CLSID\{CLSID of the control}\ToolBoxBitmap32\
(see the RegisterControl method in ActiveXCtrlHelper)
ToolBoxBitmap32 is used to identify the module name and the resource ID for a 
16 x 16 bitmap as the toolbar button face. Each specified icon must be 
embedded as a win32 resource in the assembly. In order to embed the bitmap 
CSActiveX.bmp into the assembly as a win32 resource, we need to 

Step1. Place the VBActiveX.bmp file in the root folder of the project.

Step2. Add a .rc file (VBActiveX.rc) to the project with the content: 

	101 BITMAP VBActiveX.bmp

101 is the resource ID, BITMAP is the resource type, and VBActiveX.bmp is the
resource name. 

Step3. Open Project Properties, turn to the Compile page and click the Build 
Events button. In Pre-build event command line, enter this command:

	@echo.
	IF EXIST "$(DevEnvDir)..\..\..\Microsoft SDKs\Windows\v6.0A\bin\rc.exe" 
	("$(DevEnvDir)..\..\..\Microsoft SDKs\Windows\v6.0A\bin\rc.exe" /r 
	"$(ProjectDir)VBActiveX.rc") 
	ELSE (IF EXIST "$(DevEnvDir)..\..\SDK\v2.0\Bin\rc.exe" 
	("$(DevEnvDir)..\..\SDK\v2.0\Bin\rc.exe"/r "$(ProjectDir)VBActiveX.rc") 
	ELSE (IF EXIST "$(DevEnvDir)..\Tools\Bin\rc.exe" 
	("$(DevEnvDir)..\Tools\Bin\rc.exe"/r "$(ProjectDir)VBActiveX.rc") 
	ELSE (IF EXIST "$(DevEnvDir)..\..\VC\Bin\rc.exe" 
	("$(DevEnvDir)..\..\VC\Bin\rc.exe"/r "$(ProjectDir)VBActiveX.rc") 
	ELSE (@Echo Unable to find rc.exe, using default manifest instead))))
	@echo.

The command searches for the Resource Compiler (rc.exe), and use the tool to 
compile the resource definition file and the resource files (binary files 
such as icon, bitmap, and cursor files) into a binary resource (.RES) file: 
VBActiveX.RES.

Step4. Open the project file (VBActiveX.vbproj) in Notepad, and add the below 
XML element under <PropertyGroup>.

	<Win32Resource>VBActiveX.res</Win32Resource>

This specifies the VBActiveX.res file generated in step 3 to be the win32 
resource of the assembly.


/////////////////////////////////////////////////////////////////////////////
References:

Interop Forms Toolkit 2.0
http://msdn.microsoft.com/en-us/vbasic/bb419144.aspx


/////////////////////////////////////////////////////////////////////////////