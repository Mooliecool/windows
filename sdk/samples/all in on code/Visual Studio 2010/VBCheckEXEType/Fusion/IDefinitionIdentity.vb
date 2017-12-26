'*************************** Module Header ******************************'
' Module Name:  IDefinitionIdentity.vb
' Project:	    VBCheckEXEType
' Copyright (c) Microsoft Corporation.
' 
' Represents the unique signature of the code that defines the application 
' in the current scope.
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

Imports System
Imports System.Runtime.InteropServices
Imports System.Security

Namespace Fusion

    <ComImport()>
    <Guid("587bf538-4d90-4a3c-9ef1-58a200a8a9e7")>
    <InterfaceType(ComInterfaceType.InterfaceIsIUnknown)>
    Friend Interface IDefinitionIdentity

        <SecurityCritical()>
        Function GetAttribute(<[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal [Namespace] As String,
                              <[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal Name As String) _
                          As <MarshalAs(UnmanagedType.LPWStr)> String

        <SecurityCritical()>
        Sub SetAttribute(<[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal [Namespace] As String,
                         <[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal Name As String,
                         <[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal Value As String)

        <SecurityCritical()>
        Function EnumAttributes() As IEnumIDENTITY_ATTRIBUTE

        <SecurityCritical()>
        Function Clone(<[In]()> ByVal cDeltas As IntPtr,
                       <[In](), MarshalAs(UnmanagedType.LPArray)> ByVal Deltas() As IDENTITY_ATTRIBUTE) _
                   As IDefinitionIdentity

    End Interface
End Namespace