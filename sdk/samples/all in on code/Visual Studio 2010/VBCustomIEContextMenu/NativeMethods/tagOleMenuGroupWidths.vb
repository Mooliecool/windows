'*************************** Module Header ******************************'
' Module Name:  tagOleMenuGroupWidths.vb
' Project:      VBCustomIEContextMenu
' Copyright (c) Microsoft Corporation.
' 
' The class tagOleMenuGroupWidths is used by IOleInPlaceFrame.
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
    Public NotInheritable Class tagOleMenuGroupWidths
        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)>
        Public widths(5) As Integer
    End Class
End Namespace
