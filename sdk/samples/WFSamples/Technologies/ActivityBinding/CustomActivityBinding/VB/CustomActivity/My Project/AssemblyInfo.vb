'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports System
Imports System.Reflection
Imports System.Runtime.InteropServices
Imports System.Security.Permissions

' General Information about an assembly is controlled through the following 
' set of attributes. Change these attribute values to modify the information
' associated with an assembly.

' Review the values of the assembly attributes

<Assembly: AssemblyTitle("CustomActivity")> 
<Assembly: AssemblyDescription("")> 
<Assembly: AssemblyCompany("Microsoft")> 
<Assembly: AssemblyProduct("CustomActivity")> 
<Assembly: AssemblyCopyright("Copyright © Microsoft 2006")> 
<Assembly: AssemblyTrademark("")> 

<Assembly: ComVisible(False)> 
<Assembly: CLSCompliant(True)> 
<Assembly: SecurityPermission(SecurityAction.RequestMinimum, Flags:=SecurityPermissionFlag.Execution)> 

'The following GUID is for the ID of the typelib if this project is exposed to COM
<Assembly: Guid("bec8493c-fcd7-4953-b176-fc719ed7a7fa")> 

' Version information for an assembly consists of the following four values:
'
'      Major Version
'      Minor Version 
'      Build Number
'      Revision
'
' You can specify all the values or you can default the Build and Revision Numbers 
' by using '*'.

<Assembly: AssemblyVersion("1.0.0.0")> 
<Assembly: AssemblyFileVersion("1.0.0.0")> 

'NOTE: When updating the namespaces in the project please add new or update existing the XmlnsDefinitionAttribute
'You can add additional attributes in order to map any additional namespaces you have in the project
'<Assembly: System.Workflow.ComponentModel.Serialization.XmlnsDefinition("http://schemas.com/CustomActivity", "CustomActivity")> 
