'*************************** Module Header ******************************'
' Module Name:  IOleCommandTarget.vb
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
    <ComImport()>
    <InterfaceType(ComInterfaceType.InterfaceIsIUnknown)>
    <ComVisible(True)>
    <Guid("B722BCCB-4E68-101B-A2BC-00AA00404770")>
    Public Interface IOleCommandTarget
        <PreserveSig()>
        Function QueryStatus(ByRef pguidCmdGroup As Guid,
                             ByVal cCmds As Integer,
                             <[In](), Out()> ByVal prgCmds As NativeMethods.OLECMD,
                             <[In](), Out()> ByVal pCmdText As IntPtr) _
                         As <MarshalAs(UnmanagedType.I4)> Integer

        <PreserveSig()>
        Function Exec(ByRef pguidCmdGroup As Guid,
                      ByVal nCmdID As Integer,
                      ByVal nCmdexecopt As Integer,
                      <[In](), MarshalAs(UnmanagedType.LPArray)> ByVal pvaIn() As Object,
                      ByVal pvaOut As Integer) As <MarshalAs(UnmanagedType.I4)> Integer
    End Interface
End Namespace

