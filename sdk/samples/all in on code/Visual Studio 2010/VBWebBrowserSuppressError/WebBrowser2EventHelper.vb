'*************************** Module Header ******************************'
' Module Name:  WebBrowser2EventHelper.vb
' Project:	    VBWebBrowserSuppressError
' Copyright (c) Microsoft Corporation.
' 
' The class WebBrowser2EventHelper is used to handles the NavigateError event 
' from the underlying ActiveX control by raising the NavigateError event 
' defined in class WebBrowserEx. 
' 
' Because of the protected method WebBrowserEx.OnNavigateError, this
' class is defined inside the class WebBrowserEx.
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

Partial Public Class WebBrowserEx

    Private Class WebBrowser2EventHelper
        Inherits StandardOleMarshalObject
        Implements DWebBrowserEvents2
        Private parent As WebBrowserEx

        Public Sub New(ByVal parent As WebBrowserEx)
            Me.parent = parent
        End Sub

        ''' <summary>
        ''' Raise the NavigateError event.
        ''' If a instance of WebBrowser2EventHelper is associated with the underlying
        ''' ActiveX control, this method will be called When NavigateError event was
        ''' fired in the ActiveX control.
        ''' </summary>
        Public Sub NavigateError(ByVal pDisp As Object, ByRef url As Object,
                                 ByRef frame As Object, ByRef statusCode As Object,
                                 ByRef cancel As Boolean) _
                             Implements DWebBrowserEvents2.NavigateError

            ' Raise the NavigateError event in WebBrowserEx class.
            Me.parent.OnNavigateError(
                New WebBrowserNavigateErrorEventArgs(
                    CType(url, String),
                    CType(frame, String),
                    CInt(Fix(statusCode)),
                    cancel))

        End Sub
    End Class
End Class
