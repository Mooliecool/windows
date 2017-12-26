'*************************** Module Header ******************************'
' Module Name:  DOCHOSTUIINFO.vb
' Project:      VBCustomIEContextMenu
' Copyright (c) Microsoft Corporation.
' 
' The class DOCHOSTUIINFO is used by the IDocHostUIHandler::GetHostInfo method 
' to allow MSHTML to retrieve information about the host's UI requirements.
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

Namespace NativeMethods
    <StructLayout(LayoutKind.Sequential), ComVisible(True)>
    Public Class DOCHOSTUIINFO
        <MarshalAs(UnmanagedType.U4)>
        Public cbSize As Integer = Marshal.SizeOf(GetType(NativeMethods.DOCHOSTUIINFO))
        <MarshalAs(UnmanagedType.I4)>
        Public dwFlags As Integer
        <MarshalAs(UnmanagedType.I4)>
        Public dwDoubleClick As Integer
        <MarshalAs(UnmanagedType.I4)>
        Public dwReserved1 As Integer
        <MarshalAs(UnmanagedType.I4)>
        Public dwReserved2 As Integer
    End Class
End Namespace

