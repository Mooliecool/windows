'*************************** Module Header ******************************'
' Module Name:  IOleInPlaceFrame.vb
' Project:      VBCustomIEContextMenu
' Copyright (c) Microsoft Corporation.
' 
' This interface is used by object applications to control the display and 
' placement of composite menus, keystroke accelerator translation, 
' context-sensitive help mode, and modeless dialog boxes.
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
    <ComImport(), InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid("00000116-0000-0000-C000-000000000046")>
    Public Interface IOleInPlaceFrame
        Function GetWindow() As IntPtr

        <PreserveSig()>
        Function ContextSensitiveHelp(ByVal fEnterMode As Integer) As Integer

        <PreserveSig()>
        Function GetBorder(<Out()> ByVal lprectBorder As NativeMethods.COMRECT) As Integer

        <PreserveSig()>
        Function RequestBorderSpace(<[In]()> ByVal pborderwidths As NativeMethods.COMRECT) As Integer

        <PreserveSig()>
        Function SetBorderSpace(<[In]()> ByVal pborderwidths As NativeMethods.COMRECT) As Integer

        <PreserveSig()>
        Function SetActiveObject(<[In](), MarshalAs(UnmanagedType.Interface)> ByVal pActiveObject As IOleInPlaceActiveObject,
                                 <[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal pszObjName As String) As Integer

        <PreserveSig()>
        Function InsertMenus(<[In]()> ByVal hmenuShared As IntPtr,
                             <[In](), Out()> ByVal lpMenuWidths As NativeMethods.tagOleMenuGroupWidths) As Integer

        <PreserveSig()>
        Function SetMenu(<[In]()> ByVal hmenuShared As IntPtr,
                         <[In]()> ByVal holemenu As IntPtr,
                         <[In]()> ByVal hwndActiveObject As IntPtr) As Integer

        <PreserveSig()>
        Function RemoveMenus(<[In]()> ByVal hmenuShared As IntPtr) As Integer

        <PreserveSig()>
        Function SetStatusText(<[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal pszStatusText As String) As Integer

        <PreserveSig()>
        Function EnableModeless(ByVal fEnable As Boolean) As Integer

        <PreserveSig()>
        Function TranslateAccelerator(<[In]()> ByRef lpmsg As NativeMethods.MSG,
                                      <[In](), MarshalAs(UnmanagedType.U2)> ByVal wID As Short) As Integer
    End Interface
End Namespace

