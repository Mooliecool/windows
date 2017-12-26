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
    <ComImport()> _
    <Guid("BD3F23C0-D43E-11CF-893B-00AA00BDCE1A")> _
    <InterfaceType(ComInterfaceType.InterfaceIsIUnknown)> _
    <ComVisible(True)> _
    Public Interface IDocHostUIHandler

        <PreserveSig()> _
        Function ShowContextMenu( _
                                <[In](), MarshalAs(UnmanagedType.U4)> ByVal dwID As Integer, _
                                <[In]()> ByVal pt As NativeMethods.POINT, _
                                <[In](), MarshalAs(UnmanagedType.Interface)> ByVal pcmdtReserved As Object, _
                                <[In](), MarshalAs(UnmanagedType.Interface)> ByVal pdispReserved As Object) _
                            As <MarshalAs(UnmanagedType.I4)> Integer

    <PreserveSig()> _
        Function GetHostInfo(<[In](), Out()> ByVal info As NativeMethods.DOCHOSTUIINFO) _
        As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()> _
        Function ShowUI( _
                       <[In](), MarshalAs(UnmanagedType.I4)> ByVal dwID As Integer, _
                       <[In]()> ByVal activeObject As NativeMethods.IOleInPlaceActiveObject, _
                       <[In]()> ByVal commandTarget As NativeMethods.IOleCommandTarget, _
                       <[In]()> ByVal frame As NativeMethods.IOleInPlaceFrame, _
                       <[In]()> ByVal doc As NativeMethods.IOleInPlaceUIWindow) _
                   As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()> _
        Function HideUI() As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()> _
        Function UpdateUI() As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()> _
        Function EnableModeless(<[In](), MarshalAs(UnmanagedType.Bool)> ByVal fEnable As Boolean) _
        As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()> _
        Function OnDocWindowActivate(<[In](), MarshalAs(UnmanagedType.Bool)> ByVal fActivate As Boolean) _
        As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()> _
        Function OnFrameWindowActivate(<[In](), MarshalAs(UnmanagedType.Bool)> ByVal fActivate As Boolean) _
        As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()> _
        Function ResizeBorder( _
                             <[In]()> ByVal rect As NativeMethods.COMRECT, _
                             <[In]()> ByVal doc As NativeMethods.IOleInPlaceUIWindow, _
                             ByVal fFrameWindow As Boolean) _
                         As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()> _
        Function TranslateAccelerator( _
                                     <[In]()> ByRef msg As NativeMethods.MSG, _
                                     <[In]()> ByRef group As Guid, _
                                     <[In](), MarshalAs(UnmanagedType.I4)> ByVal nCmdID As Integer) _
                                 As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()> _
        Function GetOptionKeyPath( _
                                 <Out(), MarshalAs(UnmanagedType.LPArray)> ByVal pbstrKey() As String, _
                                 <[In](), MarshalAs(UnmanagedType.U4)> ByVal dw As Integer) _
                             As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()> _
        Function GetDropTarget( _
                              <[In](), MarshalAs(UnmanagedType.Interface)> ByVal pDropTarget As IOleDropTarget, _
                              <Out(), MarshalAs(UnmanagedType.Interface)> ByRef ppDropTarget As IOleDropTarget) _
                          As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()> _
    Function GetExternal(<Out(), MarshalAs(UnmanagedType.Interface)> ByRef ppDispatch As Object) _
    As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()> _
        Function TranslateUrl( _
                             <[In](), MarshalAs(UnmanagedType.U4)> ByVal dwTranslate As Integer, _
                             <[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal strURLIn As String, _
                             <Out(), MarshalAs(UnmanagedType.LPWStr)> ByRef pstrURLOut As String) _
                         As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()> _
        Function FilterDataObject( _
                                 ByVal pDO As System.Runtime.InteropServices.ComTypes.IDataObject, _
                                 <Out()> ByRef ppDORet As System.Runtime.InteropServices.ComTypes.IDataObject) _
                             As <MarshalAs(UnmanagedType.I4)> Integer
    End Interface
End Namespace

