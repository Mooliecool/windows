'*************************** Module Header ******************************'
' Module Name:  MSG.vb
' Project:	    VBIEExplorerBar
' Copyright (c) Microsoft Corporation.
' 
' The strcut MSG specifies the message to be translated.
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
    <StructLayout(LayoutKind.Sequential)>
    Public Structure MSG
        Public hwnd As IntPtr
        Public message As UInteger
        Public wParam As UInteger
        Public lParam As Integer
        Public time As UInteger
        Public pt As POINT
    End Structure
End Namespace
