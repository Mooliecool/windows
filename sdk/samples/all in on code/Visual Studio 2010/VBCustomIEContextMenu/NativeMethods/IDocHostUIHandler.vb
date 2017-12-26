'*************************** Module Header ******************************'
' Module Name:  IDocHostUIHandler.vb
' Project:      VBCustomIEContextMenu
' Copyright (c) Microsoft Corporation.
' 
' The interface IDocHostUIHandler enables an application that is hosting the 
' WebBrowser Control or automating Windows Internet Explorer to replace the 
' menus, toolbars, and context menus used by MSHTML.
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
Imports System.Runtime.InteropServices.ComTypes

Namespace NativeMethods
    <ComImport()>
    <Guid("BD3F23C0-D43E-11CF-893B-00AA00BDCE1A")>
    <InterfaceType(ComInterfaceType.InterfaceIsIUnknown)>
    <ComVisible(True)>
    Public Interface IDocHostUIHandler

        <PreserveSig()>
        Function ShowContextMenu(
                                <[In](), MarshalAs(UnmanagedType.U4)> ByVal dwID As Integer,
                                <[In]()> ByVal pt As NativeMethods.POINT,
                                <[In](), MarshalAs(UnmanagedType.Interface)> ByVal pcmdtReserved As Object,
                                <[In](), MarshalAs(UnmanagedType.Interface)> ByVal pdispReserved As Object) _
                            As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()>
        Function GetHostInfo(<[In](), Out()> ByVal info As NativeMethods.DOCHOSTUIINFO) _
        As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()>
        Function ShowUI(
                       <[In](), MarshalAs(UnmanagedType.I4)> ByVal dwID As Integer,
                       <[In]()> ByVal activeObject As NativeMethods.IOleInPlaceActiveObject,
                       <[In]()> ByVal commandTarget As NativeMethods.IOleCommandTarget,
                       <[In]()> ByVal frame As NativeMethods.IOleInPlaceFrame,
                       <[In]()> ByVal doc As NativeMethods.IOleInPlaceUIWindow) _
                   As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()>
        Function HideUI() As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()>
        Function UpdateUI() As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()>
        Function EnableModeless(<[In](), MarshalAs(UnmanagedType.Bool)> ByVal fEnable As Boolean) _
        As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()>
        Function OnDocWindowActivate(<[In](), MarshalAs(UnmanagedType.Bool)> ByVal fActivate As Boolean) _
        As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()>
        Function OnFrameWindowActivate(<[In](), MarshalAs(UnmanagedType.Bool)> ByVal fActivate As Boolean) _
        As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()>
        Function ResizeBorder(
                             <[In]()> ByVal rect As NativeMethods.COMRECT,
                             <[In]()> ByVal doc As NativeMethods.IOleInPlaceUIWindow,
                             ByVal fFrameWindow As Boolean) _
                         As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()>
        Function TranslateAccelerator(
                                     <[In]()> ByRef msg As NativeMethods.MSG,
                                     <[In]()> ByRef group As Guid,
                                     <[In](), MarshalAs(UnmanagedType.I4)> ByVal nCmdID As Integer) _
                                 As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()>
        Function GetOptionKeyPath(
                                 <Out(), MarshalAs(UnmanagedType.LPArray)> ByVal pbstrKey() As String,
                                 <[In](), MarshalAs(UnmanagedType.U4)> ByVal dw As Integer) _
                             As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()>
        Function GetDropTarget(
                              <[In](), MarshalAs(UnmanagedType.Interface)> ByVal pDropTarget As IOleDropTarget,
                              <Out(), MarshalAs(UnmanagedType.Interface)> ByRef ppDropTarget As IOleDropTarget) _
                          As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()>
        Function GetExternal(<Out(), MarshalAs(UnmanagedType.Interface)> ByRef ppDispatch As Object) _
        As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()>
        Function TranslateUrl(
                             <[In](), MarshalAs(UnmanagedType.U4)> ByVal dwTranslate As Integer,
                             <[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal strURLIn As String,
                             <Out(), MarshalAs(UnmanagedType.LPWStr)> ByRef pstrURLOut As String) _
                         As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()>
        Function FilterDataObject(
                                 ByVal pDO As System.Runtime.InteropServices.ComTypes.IDataObject,
                                 <Out()> ByRef ppDORet As System.Runtime.InteropServices.ComTypes.IDataObject) _
                             As <MarshalAs(UnmanagedType.I4)> Integer
    End Interface
End Namespace

