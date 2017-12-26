'*************************** Module Header ******************************'
' Module Name:  NativeMethods.vb
' Project:	    VBCheckEXEType
' Copyright (c) Microsoft Corporation.
' 
' This class wraps the GetAssemblyIdentityFromFile method in clr.dll. 
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.Runtime.InteropServices
Imports System.Security

Namespace Fusion

    Friend Class NativeMethods
        <DllImport("clr.dll", CharSet:=CharSet.Auto, SetLastError:=True,
            ExactSpelling:=True, PreserveSig:=False)>
        Friend Shared Function GetAssemblyIdentityFromFile(
                                                          <[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal filePath As String,
                                                          <[In]()> ByRef riid As Guid) _
                                                      As <MarshalAs(UnmanagedType.IUnknown)> Object
        End Function

        <SecurityCritical()>
        <DllImport("clr.dll", CharSet:=CharSet.Auto, SetLastError:=True, PreserveSig:=False)>
        Friend Shared Function GetIdentityAuthority() As <MarshalAs(UnmanagedType.Interface)> IIdentityAuthority
        End Function

        Friend Shared ReferenceIdentityGuid As New Guid("6eaf5ace-7917-4f3c-b129-e046a9704766")

        Friend Shared DefinitionIdentityGuid As New Guid("587bf538-4d90-4a3c-9ef1-58a200a8a9e7")

        Friend Shared IdentityAuthorityGuid As New Guid("261a6983-c35d-4d0d-aa5b-7867259e77bc")
    End Class

End Namespace

