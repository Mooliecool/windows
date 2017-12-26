'************************************* Module Header **************************************'
' Module Name:  VstoClass.vb
' Project:      VBVstoVBAInterop
' Copyright (c) Microsoft Corporation.
' 
' The VBVstoVBAInterop project demonstrates how to interop with VBA project object model in 
' VSTO projects. Including how to programmatically add Macros (or VBA UDF in Excel) into an
' Office document; how to call Macros / VBA UDFs from VSTO code; and how to call VSTO code
' from VBA code. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
' EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
' MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'******************************************************************************************'

#Region "Import directives"
Imports System.Reflection
Imports System.Runtime.InteropServices
#End Region


''' <summary>
''' Interface for VstoClass
''' </summary>
<InterfaceType(ComInterfaceType.InterfaceIsIDispatch)> _
Public Interface IVstoClass
    Function GetAsmInfo() As String
End Interface

''' <summary>
''' The implementation of IVstoClass interface.
''' </summary>
<ComVisible(True), ClassInterface(ClassInterfaceType.None)> _
Public Class VstoClass
    Implements IVstoClass

    Public Function GetAsmInfo() As String Implements IVstoClass.GetAsmInfo
        Return Assembly.GetExecutingAssembly().ToString()
    End Function
End Class
