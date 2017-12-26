'************************** Module Header ******************************'
' Module Name:  IDownloadManager.vb
' Project:      VBIEDownloadManager
' Copyright (c) Microsoft Corporation.
' 
' Provides access to the method of a custom download manager object that Windows
' Internet Explorer and WebBrowser applications use to download a file. See
' http://msdn.microsoft.com/en-us/library/aa753613(VS.85).aspx
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'************************************************************************'

Imports System.Runtime.InteropServices
Imports System.Runtime.InteropServices.ComTypes

Namespace NativeMethods
    <ComVisible(False), ComImport(), Guid("988934A4-064B-11D3-BB80-00104B35E7F9"), _
    InterfaceType(ComInterfaceType.InterfaceIsIUnknown)> _
    Public Interface IDownloadManager
        <PreserveSig()> _
        Function Download(<[In](), MarshalAs(UnmanagedType.Interface)> ByVal pmk As IMoniker, _
                          <[In](), MarshalAs(UnmanagedType.Interface)> ByVal pbc As IBindCtx, _
                          <[In](), MarshalAs(UnmanagedType.U4)> ByVal dwBindVerb As UInt32, _
                          <[In]()> ByVal grfBINDF As Integer, _
                          <[In]()> ByVal pBindInfo As IntPtr, _
                          <[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal pszHeaders As String, _
                          <[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal pszRedir As String, _
                          <[In](), MarshalAs(UnmanagedType.U4)> ByVal uiCP As UInteger) _
                      As <MarshalAs(UnmanagedType.I4)> Integer
    End Interface
End Namespace
