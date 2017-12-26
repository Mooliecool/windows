Imports System
Imports System.Reflection
Imports System.Runtime.CompilerServices
Imports System.Runtime.InteropServices
Imports System.Security.Permissions
Imports System.Management.Instrumentation

' General Information about an assembly is controlled through the following 
' set of attributes. Change these attribute values to modify the information
' associated with an assembly.

' Review the values of the assembly attributes

<Assembly: AssemblyTitle("service")> 
<Assembly: CLSCompliant(True)> 
<Assembly: SecurityPermissionAttribute(SecurityAction.RequestMinimum)> 
<Assembly: AssemblyConfiguration("")> 
<Assembly: AssemblyDescription("")> 
<Assembly: AssemblyCompany("Microsoft")> 
<Assembly: AssemblyProduct("")> 
<Assembly: AssemblyCopyright("Copyright @ Microsoft 2004")> 
<Assembly: AssemblyTrademark("")> 
<Assembly: AssemblyCulture("")> 

<Assembly: ComVisible(False)> 

'The following GUID is for the ID of the typelib if this project is exposed to COM
<Assembly: Guid("a51fe644-fec9-413f-ae63-91e8e697f6de")> 

' Version information for an assembly consists of the following four values:
'
'      Major Version
'      Minor Version 
'      Build Number
'      Revision
'
' You can specify all the values or you can default the Build and Revision Numbers 
' by using the '*' as shown below:

<Assembly: AssemblyVersion("1.0.0.0")> 
<Assembly: AssemblyFileVersion("1.0.0.0")> 
<Assembly: Instrumented("root/ServiceModel")> 