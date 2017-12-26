'*************************** Module Header ******************************'
' Module Name:  WebBrowser2EventHelper.vb
' Project:	    VBTabbedWebBrowser
' Copyright (c) Microsoft Corporation.
' 
' The class WebBrowser2EventHelper is used to handle the NewWindow3 event 
' from the underlying ActiveX control by raising the NewWindow3 event 
' defined in class WebBrowserEx. 
' 
' Because of the protected method WebBrowserEx.OnNewWindow3, this
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
    Private Class DWebBrowserEvent2Helper
        Inherits StandardOleMarshalObject
        Implements DWebBrowserEvents2

        Private _parent As WebBrowserEx

        Public Sub New(ByVal parent As WebBrowserEx)
            Me._parent = parent
        End Sub

        ''' <summary>
        ''' Raise the NewWindow3 event.
        ''' If an instance of WebBrowser2EventHelper is associated with the underlying
        ''' ActiveX control, this method will be called When the NewWindow3 event was
        ''' fired in the ActiveX control.
        ''' </summary>
        Public Sub NewWindow3(ByRef ppDisp As Object, ByRef Cancel As Boolean,
                              ByVal dwFlags As UInteger, ByVal bstrUrlContext As String,
                              ByVal bstrUrl As String) Implements DWebBrowserEvents2.NewWindow3
            Dim e = New WebBrowserNewWindowEventArgs(bstrUrl, Cancel)
            Me._parent.OnNewWindow3(e)
            Cancel = e.Cancel
        End Sub
    End Class
End Class

