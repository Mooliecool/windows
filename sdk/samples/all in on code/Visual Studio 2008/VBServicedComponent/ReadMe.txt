========================================================================
    LIBRARY APPLICATION : VBServicedComponent Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

VBServicedComponent demonstrates a serviced component written in VB.NET. A 
serviced component is a class that is authored in a CLS-compliant language 
and that derives directly or indirectly from the System.EnterpriseServices.
ServicedComponent class. Classes configured in this way can be hosted in a 
COM+ application and can use COM+ services by way of the EnterpriseServices 
namespace. 

VBServicedComponent exposes the following component:

1. SimpleObject

Program ID: VBServicedComponent.SimpleObject
CLSID_SimpleObject: 53B70923-7796-4c6e-8E19-03DA58D51AB0
IID_ISimpleObject: 3CBE3348-E59D-4ce6-8B46-AE0119E4B871
DIID_ISimpleObjectEvents: C265CEA3-7A1C-479c-BFFC-05EC03F7D24B (EventID)
LIBID_CSServicedComponent: 6B8E2f67-6E10-43A6-B8EE-7561E8E71A9E

Properties:
' With both get and set accessor methods
FloatProperty As Single

Methods:
' HelloWorld returns a string "HelloWorld"
Function HelloWorld() As String
' GetProcessThreadID outputs the running process ID and thread ID
Sub GetProcessThreadID(ByRef processId As UInteger, ByRef threadId As UInteger)
' Transactional operation
void DoTransaction();

Events:
' FloatPropertyChanging is fired before new value is set to the FloatProperty
' property. The Cancel parameter allows the client to cancel the change of
' FloatProperty.
Event FloatPropertyChanging(ByVal NewValue As Single, ByRef Cancel As Boolean)


/////////////////////////////////////////////////////////////////////////////
Project Relation:

VBServicedComponent - CSServicedComponent
These Serviced Component examples expose the same set of components but they 
are implemented in different .NET languages.


/////////////////////////////////////////////////////////////////////////////
Deployment:

A. Setup

Regsvcs.exe VBServicedComponent.dll
It adds your serviced components to a COM+ application and configures them 
according to the default COM+ settings or according to their attributes (if 
present in the code).

B. Cleanup

Regsvcs.exe /u VBServicedComponent.dll
It uninstalls the target serviced components.


/////////////////////////////////////////////////////////////////////////////
Creation:

A. Creating the project

Step1. Create a Visual Basic / Class Library project named 
VBServicedComponent in Visual Studio 2008.

Step2. Add the reference to System.EnterpriseServices.

Step3. In the properties page of the project, sign the assembly with a strong 
name.

Step4. Open the property of the project. Click the Assembly Information  
button in the page, Application, and select the "Make Assembly COM-Visible" 
box. This corresponds to the assembly attribute ComVisible in AssemblyInfo.cs:

	<Assembly: ComVisible(True)>

Step5. Add the following command to the post-build event of the project.

	RegSvcs.exe "$(TargetPath)"

This makes sure that the component is added to a COM+ application after the 
build in Visual Studio.

B. Adding a serviced component

Step1. Add a public class SimpleObject that inherits from
System.EnterpriseServices.ServicedComponent.

Step2. Inside the SimpleObject class, define Class ID, Interface ID, and 
Event ID:

	ClassId As String = "53B70923-7796-4c6e-8E19-03DA58D51AB0"
	InterfaceId As String = "3CBE3348-E59D-4ce6-8B46-AE0119E4B871"
	EventsId As String = "C265CEA3-7A1C-479c-BFFC-05EC03F7D24B"

Step3. Attach ComClassAttribute to the class SimpleObject, and specify its 
_ClassID, _InterfaceID, and _EventID to be the above const values:

	<ComClass(SimpleObject.ClassId, SimpleObject.InterfaceId, _
		SimpleObject.EventsId)> _
	Public Class SimpleObject
		Inherits ServicedComponent

Step4. Adding public properties, methods and events to the component.

C. Configuring the serviced component

Step1. Specify the name of the COM+ application in AssemblyInfo.vb:

	<Assembly: ApplicationName("All-In-One Code Framework")> 

The System.EnterpriseServices.ApplicationName attribute specifies the name of 
the COM+ application you would like your components to be part of. If the 
assembly attribute is not provided, the assembly name is used as the name of 
the COM+ application by default.

Step2. Optionally specify the application ID in AssemblyInfo.vb:

	<Assembly: ApplicationID("11F3EE74-29A6-4773-82C6-274A67961FB4")>

When registration occurs, the components in the assembly are installed in an 
application with the given ID. 

Step3. Specify the application activation type in AssemblyInfo.vb:

	<Assembly: ApplicationActivation(ActivationOption.Server)> 

The ActivationOption attribute indicates whether the component will be 
activated within the caller's process. You can set Activation.Option to 
Library or to Server. If you do not provide the ApplicationActivation 
attribute, then .NET uses a library activation type by default.

Step4. Add the Description attribute. 

	<Assembly: Description("COM+ examples of All-In-One Code Framework")> 

The Description attribute allows you to add text to the description field on 
the General Properties tab of an application, component, interface, or method 
in DCOMCNFG.

Step5. Configure the COM+ security

The COM+ security corresponds to the Roles folder and the Security tab of the 
application', components', interfaces', methods' properties in DCOMCNFG. It 
allows us to configure their access security. Improper configuration may 
cause unexpected "permission denied" error on the client side, so you need to 
be careful to set the security.

	<Assembly: ApplicationAccessControl(True, _
		AccessChecksLevel:=AccessChecksLevelOption.ApplicationComponent, _
		Authentication:=AuthenticationOption.Packet, _
		ImpersonationLevel:=ImpersonationLevelOption.Identify)> 

The ApplicationAccessControl attribute programmatically configures the 
Security tab of the Appliction's properties dialog in DCOMCNFG. When 
ApplicationAccessControl(False), the "Enforce access checks for this 
application" option is off, and the components inside the application allow 
full accesses from clients. ApplicationAccessControl(True) enables access 
checks. You should select the appropriate security level. (See 
http://msdn.microsoft.com/en-us/library/ms684382.aspx) Also, you must be sure 
to define roles and add them to the application. If access checks are enabled 
but no roles have been added, all calls to the application will fail. (See 
http://msdn.microsoft.com/en-us/library/ms678849.aspx and 
http://support.microsoft.com/kb/810153). Roles can be defined 
using attributes too:

	<Assembly: SecurityRole("Tester", SetEveryoneAccess:=True)> 

Step6. Optionally specify the COM+ object pooling for serviced components:

	<ObjectPooling(MinPoolSize:=2, MaxPoolSize:=10, CreationTimeout:=20)> _
	Public Class SimpleObject
		Inherits ServicedComponent

The ObjectPooling attribute is used to configure the component's object 
pooling. It can enable or disables object pooling and set the min. or max. 
pool size and object creation timeout. 

Step7. Specify the COM+ transactions for the serviced components:

	<Transaction(TransactionOption.Required)> _
	Public Class SimpleObject
		Inherits ServicedComponent

You can configure the serviced component to use one of the five available 
COM+ transaction support options by using the Transaction attribute:

	Disabled,		' Ignores any transaction in the current context
	NotSupported,	' Creates in a context without governing transaction
	Supported,		' Shares a transaction if one exists
	Required,		' Shares a transaction if one exists, and creates a new 
					' transaction if necessary
	RequiresNew		' Creates the component with a new transaction, 
					' regardless of the state of the current context.

The five options correspond to the Transactions tab of the components' 
properties sheet in DCOMCNFG. 

When performing transactional operations, you can use ContextUtil.SetComplete 
to commit the changes, or call ContextUtil.SetAbort to rollback on exceptions. 

	Try
		' Operate on the resource managers like DBMS
		' ...

		ContextUtil.SetComplete()   ' Commit
	Catch ex As Exception
		ContextUtil.SetAbort()      ' Rollback
		Throw ex
	End Try

The KB article http://support.microsoft.com/kb/816141 is an example of COM+ 
transaction. Please note that transactional operations must happen to resource 
managers. A resource (such as a database management system) that can 
participate in a COM+ transaction is called a resource manager. A resource 
manager knows how to conduct itself properly in the scope of a COM+ 
transaction. 


/////////////////////////////////////////////////////////////////////////////
References:

Writing Serviced Components
http://msdn.microsoft.com/en-us/library/3x7357ez(VS.80).aspx

HOW TO: Create a Serviced .NET Component in Visual C# .NET
http://support.microsoft.com/kb/306296

Chapter 10 .NET Serviced Components of 'COM and .NET Component Services'
http://oreilly.com/catalog/comdotnetsvs/chapter/ch10.html

How to use COM+ transactions in a Visual C# component
http://support.microsoft.com/kb/816141

COM+ Integration: How .NET Enterprise Services Can Help You Build Distributed 
Applications
http://msdn.microsoft.com/en-us/magazine/cc301491.aspx


/////////////////////////////////////////////////////////////////////////////

