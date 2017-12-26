'*************************** Module Header ******************************'
' Module Name:  IOleDropTarget.vb
' Project:      VBCustomIEContextMenu
' Copyright (c) Microsoft Corporation.
' 
' This interface enables objects and their containers to dispatch commands to 
' each other. For example, an object's toolbars may contain buttons for 
' commands such as Print, Print Preview, Save, New, and Zoom.
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
    <Guid("00000122-0000-0000-C000-000000000046")> _
    <InterfaceType(ComInterfaceType.InterfaceIsIUnknown)> _
    Public Interface IOleDropTarget

        <PreserveSig()> _
        Function OleDragEnter(<[In](), MarshalAs(UnmanagedType.Interface)> ByVal pDataObj As Object, _
                              <[In](), MarshalAs(UnmanagedType.U4)> ByVal grfKeyState As Integer, _
                              <[In](), MarshalAs(UnmanagedType.U8)> ByVal pt As Long, _
                              <[In](), Out()> ByRef pdwEffect As Integer) As Integer

        <PreserveSig()> _
        Function OleDragOver(<[In](), MarshalAs(UnmanagedType.U4)> ByVal grfKeyState As Integer, _
                             <[In](), MarshalAs(UnmanagedType.U8)> ByVal pt As Long, _
                             <[In](), Out()> ByRef pdwEffect As Integer) As Integer

        <PreserveSig()> _
    Function OleDragLeave() As Integer

        <PreserveSig()> _
        Function OleDrop(<[In](), MarshalAs(UnmanagedType.Interface)> ByVal pDataObj As Object, _
                         <[In](), MarshalAs(UnmanagedType.U4)> ByVal grfKeyState As Integer, _
                         <[In](), MarshalAs(UnmanagedType.U8)> ByVal pt As Long, _
                         <[In](), Out()> ByRef pdwEffect As Integer) As Integer
    End Interface
End Namespace
