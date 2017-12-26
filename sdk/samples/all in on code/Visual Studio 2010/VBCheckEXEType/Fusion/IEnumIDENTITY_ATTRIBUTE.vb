'*************************** Module Header ******************************'
' Module Name:  IEnumIDENTITY_ATTRIBUTE.vb
' Project:	    VBCheckEXEType
' Copyright (c) Microsoft Corporation.
' 
' Serves as an enumerator for the attributes of the code object in the current 
' scope.
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
    <ComImport()>
    <Guid("9cdaae75-246e-4b00-a26d-b9aec137a3eb")>
    <InterfaceType(ComInterfaceType.InterfaceIsIUnknown)>
    Friend Interface IEnumIDENTITY_ATTRIBUTE
        ''' <summary>
        ''' Gets the next attributes.
        ''' </summary>
        ''' <param name="celt">Count of elements.</param>
        ''' <param name="rgAttributes">Array of attributes being returned.</param>
        ''' <returns>The next attribute.</returns>
        <SecurityCritical()>
        Function [Next](<[In]()> ByVal celt As UInteger,
                    <Out(), MarshalAs(UnmanagedType.LPArray)> ByVal rgAttributes() As IDENTITY_ATTRIBUTE) As UInteger


        ''' <summary>
        ''' Copy the current attribute into a buffer.
        ''' </summary>
        ''' <param name="available">Number of available bytes.</param>
        ''' <param name="data">Buffer into which attribute should be written.</param>
        ''' <returns>Pointer to buffer containing the attribute.</returns>
        <SecurityCritical()>
        Function CurrentIntoBuffer(<[In]()> ByVal Available As IntPtr,
                               <Out(), MarshalAs(UnmanagedType.LPArray)> ByVal Data() As Byte) As IntPtr


        ''' <summary>
        ''' Skip past a number of elements.
        ''' </summary>
        ''' <param name="celt">Count of elements to skip.</param>
        <SecurityCritical()>
        Sub Skip(<[In]()> ByVal celt As UInteger)


        ''' <summary>
        ''' Reset the enumeration to the beginning.
        ''' </summary>
        <SecurityCritical()>
        Sub Reset()


        ''' <summary>
        ''' Clone this attribute enumeration.
        ''' </summary>
        ''' <returns>Clone of a IEnumIDENTITY_ATTRIBUTE.</returns>
        <SecurityCritical()>
        Function Clone() As IEnumIDENTITY_ATTRIBUTE

    End Interface
End Namespace


