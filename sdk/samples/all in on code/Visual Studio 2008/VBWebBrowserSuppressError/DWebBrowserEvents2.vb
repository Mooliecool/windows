'*************************** Module Header ******************************'
' Module Name:  DWebBrowserEvents2.vb
' Project:	    VBWebBrowserSuppressError
' Copyright (c) Microsoft Corporation.
' 
' The interface DWebBrowserEvents2 designates an event sink interface that an
' application must implement to receive event notifications from a WebBrowser 
' control or from the Windows Internet Explorer application. The event 
' notifications include NavigateError event that will be used in this 
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
'*************************************************************************'

Imports System.Runtime.InteropServices

<ComImport(), TypeLibType(TypeLibTypeFlags.FHidden), _
InterfaceType(ComInterfaceType.InterfaceIsIDispatch), _
Guid("34A715A0-6587-11D0-924A-0020AFC7AC4D")> _
Public Interface DWebBrowserEvents2
    <DispId(271)> _
    Sub NavigateError(<[In](), MarshalAs(UnmanagedType.IDispatch)> ByVal pDisp As Object, _
                          <[In]()> ByRef url As Object, _
                          <[In]()> ByRef frame As Object, _
                          <[In]()> ByRef statusCode As Object, _
                          <[In](), Out()> ByRef cancel As Boolean)
End Interface