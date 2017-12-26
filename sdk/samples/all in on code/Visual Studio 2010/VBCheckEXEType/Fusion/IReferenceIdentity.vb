'*************************** Module Header ******************************'
' Module Name:  IReferenceIdentity.vb
' Project:	    VBCheckEXEType
' Copyright (c) Microsoft Corporation.
' 
' Represents a reference to the unique signature of a code object.
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
    <ComImport(), Guid("6eaf5ace-7917-4f3c-b129-e046a9704766"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)>
    Friend Interface IReferenceIdentity
        ''' <summary>
        ''' Get an assembly attribute.
        ''' </summary>
        ''' <param name="attributeNamespace">Attribute namespace.</param>
        ''' <param name="attributeName">Attribute name.</param>
        ''' <returns>The assembly attribute.</returns>
        <SecurityCritical()>
        Function GetAttribute(<[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal attributeNamespace As String,
                          <[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal attributeName As String) As <MarshalAs(UnmanagedType.LPWStr)> String

        ''' <summary>
        ''' Set an assembly attribute.
        ''' </summary>
        ''' <param name="attributeNamespace">Attribute namespace.</param>
        ''' <param name="attributeName">Attribute name.</param>
        ''' <param name="attributeValue">Attribute value.</param>
        <SecurityCritical()>
        Sub SetAttribute(<[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal attributeNamespace As String,
                     <[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal attributeName As String,
                     <[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal attributeValue As String)

        ''' <summary>
        ''' Get an iterator for the assembly's attributes.
        ''' </summary>
        ''' <returns>Assembly attribute enumerator.</returns>
        <SecurityCritical()>
        Function EnumAttributes() As IEnumIDENTITY_ATTRIBUTE

        ''' <summary>
        ''' Clone an IReferenceIdentity.
        ''' </summary>
        ''' <param name="countOfDeltas">Count of deltas.</param>
        ''' <param name="deltas">The deltas.</param>
        ''' <returns>Cloned IReferenceIdentity.</returns>
        <SecurityCritical()>
        Function Clone(<[In]()> ByVal countOfDeltas As IntPtr,
                   <[In](), MarshalAs(UnmanagedType.LPArray)> ByVal deltas() As IDENTITY_ATTRIBUTE) As IReferenceIdentity
    End Interface
End Namespace