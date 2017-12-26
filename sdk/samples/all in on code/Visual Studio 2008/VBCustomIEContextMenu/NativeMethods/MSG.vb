'*************************** Module Header ******************************'
' Module Name:  MSG.vb
' Project:      VBCustomIEContextMenu
' Copyright (c) Microsoft Corporation.
' 
' The struct MSG specifies the message to be translated.
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

Namespace NativeMethods
    <Serializable(), StructLayout(LayoutKind.Sequential)> _
    Public Structure MSG
        Public hwnd As IntPtr
        Public message As Integer
        Public wParam As IntPtr
        Public lParam As IntPtr
        Public time As Integer
        Public pt_x As Integer
        Public pt_y As Integer
    End Structure
End Namespace
