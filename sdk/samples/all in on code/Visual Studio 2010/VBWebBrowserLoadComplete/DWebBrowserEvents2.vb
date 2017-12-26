'************************** Module Header ******************************\
' Module Name:  DWebBrowserEvents2.vb
' Project:      VBWebBrowserLoadComplete
' Copyright (c) Microsoft Corporation.
' 
' The interface DWebBrowserEvents2 designates an event sink interface that an
' application must implement to receive event notifications from a WebBrowser 
' control or from the Windows Internet Explorer application. The event 
' notifications include DocumentComplete event that will be used in this 
' application.
' 
' To get the full event list of DWebBrowserEvents2, see
' http://msdn.microsoft.com/en-us/library/aa768283(VS.85).aspx
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************

Imports System.Runtime.InteropServices


<ComImport(),
 TypeLibType(TypeLibTypeFlags.FHidden),
 InterfaceType(ComInterfaceType.InterfaceIsIDispatch),
 Guid("34A715A0-6587-11D0-924A-0020AFC7AC4D")>
Public Interface DWebBrowserEvents2
    ''' <summary>
    ''' Fires when a document is completely loaded and initialized.
    ''' </summary>
    <DispId(259)>
    Sub DocumentComplete(<[In](), MarshalAs(UnmanagedType.IDispatch)> ByVal pDisp As Object,
                         <[In]()> ByRef URL As Object)

    <DispId(250)>
    Sub BeforeNavigate2(<[In](), MarshalAs(UnmanagedType.IDispatch)> ByVal pDisp As Object,
                        <[In]()> ByRef URL As Object,
                        <[In]()> ByRef flags As Object,
                        <[In]()> ByRef targetFrameName As Object,
                        <[In]()> ByRef postData As Object,
                        <[In]()> ByRef headers As Object,
                        <[In](), Out()> ByRef cancel As Boolean)
End Interface

