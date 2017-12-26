========================================================================
    ACTIVEX CONTROL DLL : CSActiveX Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The sample demonstrates an ActiveX control written in C#. ActiveX controls
(formerly known as OLE controls) are small program building blocks that can 
work in a variety of different containers, ranging from software development 
tools to end-user productivity tools. For example, it can be used to create 
distributed applications that work over the Internet through web browsers. 
ActiveX controls can be written in MFC, ATL, C++, C#, Borland Delphi and 
Visual Basic. In this sample, we focus on writing an ActiveX control using 
C#. We will go through the basic steps of adding UI, properties, methods, and 
events to the control. Please note that ActiveX controls or COM components 
written in .NET languages cannot be referenced by .NET applications in the 
form of interop assemblies. If you "add reference" to such a TLB, or drag & 
drop such an ActiveX control to your .NET application, you will get an error 
"The ActiveX type library 'XXXXX.tlb' was exported from a .NET assembly and 
cannot be added as a reference.". The correct approach is to add a reference 
to the .NET assembly directly.

CSActiveX exposes the following items:

1. A C# ActiveX control.

  Program ID: CSActiveX.CSActiveXCtrl
  CLSID_CSActiveXCtrl: 80B59B58-98EA-303C-BE83-D26E5D8D6794
  DIID_AxCSActiveXCtrl: D4B8539E-3839-3913-8B1A-C551A9930864
  DIID_AxCSActiveXCtrlEvents: 901EE2A0-C47C-43EC-B433-985C020051D5
  LIBID_CSActiveX: 361188E4-99EB-4E43-A72F-C89451E756DD

  UI:
    // The main UI of the control
    CSActiveXCtrl

  Properties:
    // Typical control properties
    bool Visible
    bool Enabled
    int ForeColor
    int BackColor
    // Custom properties
    float FloatProperty

  Methods:
    // Typical control methods
    void Refresh()
    // Custom methods
    string HelloWorld()

  Events:
    // Typical control events
    void Click();
    // Custom events
    // FloatPropertyChanging is fired before new value is set to the 
    // FloatProperty property. The Cancel parameter allows the client to 
    // cancel the change of FloatProperty.
    void FloatPropertyChanging(float NewValue, ref bool Cancel);


/////////////////////////////////////////////////////////////////////////////
Project Relation:

CSActiveX - VBActiveX - MFCActiveX
These samples expose the same UI and the same set of properties, methods, and
events, but they are implemented in different languages.


/////////////////////////////////////////////////////////////////////////////
Creation:

A. Creating the project

Step1. Create a Visual C# / Class Library project named CSActiveX in Visual 
Studio 2008. Delete the default Class1.cs file.

Step2. In order to make the .NET assembly COM-visible, first, open the 
property of the project. Click the Assembly Information button in the 
Application page, and select the "Make Assembly COM-Visible" box. This 
corresponds to the assembly attribute ComVisible in AssemblyInfo.cs:

	[assembly: ComVisible(true)]

The GUID value in the dialog is the libid of the component:

	[assembly: Guid("361188e4-99eb-4e43-a72f-c89451e756dd")]

Second, in the Build page of the project's property, select the option 
"Register for COM interop". This option specifies whether your managed 
application will expose a COM object (a COM-callable wrapper) that allows a 
COM object to interact with your managed application.

B. Adding the ActiveXCtrlHelper class

ActiveXCtrlHelper provides the helper functions to register/unregister an 
ActiveX control, and helps to handle the focus and tabbing across the 
container and the .NET controls.

C. Adding a user control and expose it as an ActiveX control

Step1. Right-click the project and choose Add / User Control in the context 
menu. Name the control as CSActiveXCtrl. Next, double-click the control to 
open its design view and design the UI.

Step2. In the code view of CSActiveXCtrl.cs, add a public interface named 
AxCSActiveXCtrl to describe the COM interface of the coclass CSActiveXCtrl. 
Inside the interface, define the properties and methods to be exposed by the 
ActiveX control. Attach the GuidAttribute to the interface. The GUID value 
can be generated using Tools / Create GUID, and is used as the interface ID.

	[Guid("D4B8539E-3839-3913-8B1A-C551A9930864")]

The detailed steps of adding properties and methods are documented below.

Step3. In CSActiveXCtrl.cs, add a public interface, AxCSActiveXCtrlEvents, to 
describe the events of the control. Inside the interface, define the 
prototype of the events and assign DISPID to each. For example, 

	[DispId(2)]
	void FloatPropertyChanging(float NewValue, ref bool Cancel);

The detailed steps of adding events are documented below.

Please note that DISPID must be explicitly defined for each event, otherwise, 
the callback address cannot be found when the event is fired. 

Next, assign a GUID to the interface as the event ID, and define the 
interface type as InterfaceIsIDispatch.

	[Guid("901EE2A0-C47C-43ec-B433-985C020051D5")]
	[InterfaceType(ComInterfaceType.InterfaceIsIDispatch)]

Step4. Set the class CSActiveXCtrl to implement the interface AxCSActiveXCtrl 
and attach [ComSourceInterfaces(typeof(AxCSActiveXCtrlEvents))] to the class.
ComSourceInterfacesAttribute identifies the interface exposed as COM event 
sources for the attributed class. Next, add the attribute 
[ClassInterface(ClassInterfaceType.None)] to the class, which tells the 
type-library generation tools that we do not require a Class Interface. This 
ensures that the AxCSActiveXCtrl interface is the default interface. In 
addition, specify the GUID of the class, aka CLSID, using the Guid attribute:

	[Guid("80B59B58-98EA-303C-BE83-D26E5D8D6794")]

D. ActiveX Control Registration

Additional registry keys/values are required to be set for ActiveX controls 
when compared with ordinary COM components. The default COM registration 
routine does not meet the need. Inside CSActiveXCtrl, add the functions 
Register, Unregister and decorate them with ComRegisterFunctionAttribute, 
ComUnregisterFunctionAttribute. The custom routine gets called after Regasm 
finishes the default behaviors. The Register and Unregister functions call 
the helper methods in ActiveXCtrlHelper.

E. Adding Properties to the ActiveX control

Step1. Add the prototype of property to the COM interface AxCSActiveXCtrl. 
For example, 

	float FloatProperty { get; set; }

Step2. Add the implementation of the property to the coclass CSActiveXCtrl. 
For example, 

	private float fField = 0;
	public float FloatProperty
	{
		get { return this.fField; }
		set { this.fField = value; }
	}

F. Adding Methods to the ActiveX control

Step1. Add the prototype of method to the COM interface AxCSActiveXCtrl. 
For example, 

	string HelloWorld();

Step2. Add the implementation of the method to the coclass CSActiveXCtrl. 
For example, 

	public string HelloWorld()
	{
		return "HelloWorld";
	}

G. Adding Events to the ActiveX control

Step1. Add the prototype of event to the event interface 
AxCSActiveXCtrlEvents, and explicitly assign a DISPID to it. For example, 

	[DispId(2)]
	void FloatPropertyChanging(float NewValue, ref bool Cancel);

Step2. Inside the coclass CSActiveXCtrl, add a delegate with the above 
prototype as the event handler. Set the delegate to be [ComVisible(false)]. 
For example, 

	[ComVisible(false)]
	public delegate void FloatPropertyChangingEventHandler
	(float NewValue, ref bool Cancel);

Next, add a public event:

	public event FloatPropertyChangingEventHandler FloatPropertyChanging;

And raise the event in the proper places. Please check null on the event 
before raising it. For example, 

	if (null != FloatPropertyChanging)
		FloatPropertyChanging(value, ref cancel);

H. Adding the ToolBox Bitmap resource

The ToolBox bitmap resource is specified in the regsitry key:
HKCR\CLSID\{CLSID of the control}\ToolBoxBitmap32\
(see the RegisterControl method in ActiveXCtrlHelper)
ToolBoxBitmap32 is used to identify the module name and the resource ID for a 
16 x 16 bitmap as the toolbar button face. Each specified icon must be 
embedded as a win32 resource in the assembly. In order to embed the bitmap 
CSActiveX.bmp into the assembly as a win32 resource, we need to 

Step1. Place the CSActiveX.bmp file in the root folder of the project.

Step2. Add a .rc file (CSActiveX.rc) to the project with the content: 

	101 BITMAP CSActiveX.bmp

101 is the resource ID, BITMAP is the resource type, and CSActiveX.bmp is the
resource name. 

Step3. Open Project Properties, and turn to the Build Events page. In 
Pre-build event command line, enter this command:

	@echo.
	IF EXIST "$(DevEnvDir)..\..\..\Microsoft SDKs\Windows\v6.0A\bin\rc.exe" 
	("$(DevEnvDir)..\..\..\Microsoft SDKs\Windows\v6.0A\bin\rc.exe" /r 
	"$(ProjectDir)CSActiveX.rc") 
	ELSE (IF EXIST "$(DevEnvDir)..\..\SDK\v2.0\Bin\rc.exe" 
	("$(DevEnvDir)..\..\SDK\v2.0\Bin\rc.exe"/r "$(ProjectDir)CSActiveX.rc") 
	ELSE (IF EXIST "$(DevEnvDir)..\Tools\Bin\rc.exe" 
	("$(DevEnvDir)..\Tools\Bin\rc.exe"/r "$(ProjectDir)CSActiveX.rc") 
	ELSE (IF EXIST "$(DevEnvDir)..\..\VC\Bin\rc.exe" 
	("$(DevEnvDir)..\..\VC\Bin\rc.exe"/r "$(ProjectDir)CSActiveX.rc") 
	ELSE (@Echo Unable to find rc.exe, using default manifest instead))))
	@echo.

The command searches for the Resource Compiler (rc.exe), and use the tool to 
compile the resource definition file and the resource files (binary files 
such as icon, bitmap, and cursor files) into a binary resource (.RES) file: 
CSActiveX.RES.

Step4. Turn to the Application page of Project Properties. In the section 
"Specify how application resources will be managed", select "Resource File", 
and enter the full path of CSActiveX.RES that is generated in step 3.

Step5. (Optional) If you perfer a relative path to the full path of 
CSActiveX.RES, you need to modify the project file (CSActiveX.csproj). The 
"Resource File" value corresponds to the XML element:

	<Win32Resource>CSActiveX.res</Win32Resource>


/////////////////////////////////////////////////////////////////////////////
References:

Interop Forms Toolkit 2.0
http://msdn.microsoft.com/en-us/vbasic/bb419144.aspx


/////////////////////////////////////////////////////////////////////////////