Imports System
Imports System.Reflection
Imports System.Runtime.InteropServices
Imports System.EnterpriseServices

' General Information about an assembly is controlled through the following 
' set of attributes. Change these attribute values to modify the information
' associated with an assembly.

' Review the values of the assembly attributes

<Assembly: AssemblyTitle("VBServicedComponent")> 
<Assembly: AssemblyDescription("")> 
<Assembly: AssemblyCompany("Microsoft Corporation")> 
<Assembly: AssemblyProduct("VBServicedComponent")> 
<Assembly: AssemblyCopyright("Copyright © Microsoft 2010")> 
<Assembly: AssemblyTrademark("")> 

<Assembly: ComVisible(True)> 

'The following GUID is for the ID of the typelib if this project is exposed to COM
<Assembly: Guid("6b8e2f67-6e10-43a6-b8ee-7561e8e71a9e")> 

' Version information for an assembly consists of the following four values:
'
'      Major Version
'      Minor Version 
'      Build Number
'      Revision
'
' You can specify all the values or you can default the Build and Revision Numbers 
' by using the '*' as shown below:
' <Assembly: AssemblyVersion("1.0.*")> 

<Assembly: AssemblyVersion("1.0.0.0")> 
<Assembly: AssemblyFileVersion("1.0.0.0")> 


' The ActivationOption attribute indicates whether the component will be 
' activated within the caller's process. You can set Activation.Option to 
' Library or to Server.
<Assembly: ApplicationActivation(ActivationOption.Server)> 

' The ApplicationName attribute is the name that appears for the COM+  
' application in the COM+ Catalog and the Component Services Administration 
' console.
<Assembly: ApplicationName("All-In-One Code Framework")> 

' Specify the application ID.
'<Assembly: ApplicationID("11F3EE74-29A6-4773-82C6-274A67961FB4")>

' The Description attribute provides a description for the COM+ application 
' in the COM+ Catalog and Component Services Administration console.
<Assembly: Description("COM+ examples of All-In-One Code Framework")> 

' COM+ security setting
<Assembly: ApplicationAccessControl(True, _
    AccessChecksLevel:=AccessChecksLevelOption.ApplicationComponent, _
    Authentication:=AuthenticationOption.Packet, _
    ImpersonationLevel:=ImpersonationLevelOption.Identify)> 

<Assembly: SecurityRole("Tester", SetEveryoneAccess:=True)> 
