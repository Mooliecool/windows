'*************************** Module Header ******************************'
' Module Name:  IOleInPlaceActiveObject.vb
' Project:      VBCustomIEContextMenu
' Copyright (c) Microsoft Corporation.
' 
' This interface is implemented by object applications in order to provide
' support for their objects while they are active in-place.
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

Namespace NativeMethods
    <ComImport()>
    <InterfaceType(ComInterfaceType.InterfaceIsIUnknown)>
    <SuppressUnmanagedCodeSecurity()>
    <Guid("00000117-0000-0000-C000-000000000046")>
    Public Interface IOleInPlaceActiveObject
        <PreserveSig()>
        Function GetWindow(<Out()> ByRef hwnd As IntPtr) As Integer

        Sub ContextSensitiveHelp(ByVal fEnterMode As Integer)

        <PreserveSig()>
        Function TranslateAccelerator(<[In]()> ByRef lpmsg As NativeMethods.MSG) As Integer

        Sub OnFrameWindowActivate(ByVal fActivate As Boolean)

        Sub OnDocWindowActivate(ByVal fActivate As Integer)

        Sub ResizeBorder(<[In]()> ByVal prcBorder As NativeMethods.COMRECT,
                         <[In]()> ByVal pUIWindow As NativeMethods.IOleInPlaceUIWindow,
                         ByVal fFrameWindow As Boolean)

        Sub EnableModeless(ByVal fEnable As Integer)
    End Interface
End Namespace

