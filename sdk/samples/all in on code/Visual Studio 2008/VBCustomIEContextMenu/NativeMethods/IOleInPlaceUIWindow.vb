'*************************** Module Header ******************************'
' Module Name:  IOleInPlaceUIWindow.vb
' Project:      VBCustomIEContextMenu
' Copyright (c) Microsoft Corporation.
' 
' This interface is used by object applications to negotiate border space on
' the document or frame window when one of its objects is being activated,
' or to renegotiate border space if the size of the object changes.
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
    <ComImport()> _
    <Guid("00000115-0000-0000-C000-000000000046")> _
    <InterfaceType(ComInterfaceType.InterfaceIsIUnknown)> _
    Public Interface IOleInPlaceUIWindow
        Function GetWindow() As IntPtr

        <PreserveSig()> _
        Function ContextSensitiveHelp(ByVal fEnterMode As Integer) As Integer

        <PreserveSig()> _
        Function GetBorder(<Out()> ByVal lprectBorder As NativeMethods.COMRECT) As Integer

        <PreserveSig()> _
        Function RequestBorderSpace(<[In]()> ByVal pborderwidths As NativeMethods.COMRECT) As Integer

        <PreserveSig()> _
        Function SetBorderSpace(<[In]()> ByVal pborderwidths As NativeMethods.COMRECT) As Integer

        Sub SetActiveObject(<[In](), MarshalAs(UnmanagedType.Interface)> ByVal pActiveObject As IOleInPlaceActiveObject, _
                            <[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal pszObjName As String)
    End Interface
End Namespace

