'************************** Module Header ******************************\
' Module Name:  DWebBrowserEvents2Helper.vb
' Project:      VBWebBrowserLoadComplete
' Copyright (c) Microsoft Corporation.
' 
' The class DWebBrowserEvents2Helper is used to handles the BeforeNavigate2 and 
' DocumentComplete events from the underlying ActiveX control by raising the 
' StartNavigating and LoadCompleted events defined in class WebBrowserEx. 
' 
' If the WebBrowser control is hosting a normal html page without frame, the 
' DocumentComplete event is fired once after everything is done.
' 
' If the WebBrowser control is hosting a frameset. DocumentComplete gets 
' fired multiple times. The DocumentComplete event has a pDisp parameter, which
' is the IDispatch of the frame (shdocvw) for which DocumentComplete is fired. 
' 
' Then we could check if the pDisp parameter of the DocumentComplete is the same
' as the ActiveXInstance of the WebBrowser.
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


Partial Public Class WebBrowserEx
    Private Class DWebBrowserEvents2Helper
        Inherits StandardOleMarshalObject
        Implements DWebBrowserEvents2

        Private parent As WebBrowserEx

        Public Sub New(ByVal parent As WebBrowserEx)
            Me.parent = parent
        End Sub

        ''' <summary>
        ''' Fires when a document is completely loaded and initialized.
        ''' If the frame is the top-level frame / window element, then the page is
        ''' done loading.
        ''' 
        ''' Then reset the glpDisp to null after the WebBrowser is done loading.
        ''' </summary>
        Public Sub DocumentComplete(ByVal pDisp As Object, ByRef URL As Object) _
             Implements DWebBrowserEvents2.DocumentComplete

            Dim _url As String = TryCast(URL, String)

            If String.IsNullOrEmpty(_url) OrElse _
                _url.Equals("about:blank", StringComparison.OrdinalIgnoreCase) Then
                Return
            End If

            If pDisp IsNot Nothing AndAlso pDisp.Equals(parent.ActiveXInstance) Then
                Dim e = New WebBrowserDocumentCompletedEventArgs(New Uri(_url))

                parent.OnLoadCompleted(e)
            End If
        End Sub

        ''' <summary>
        ''' Fires before navigation occurs in the given object 
        ''' (on either a window element or a frameset element).
        ''' 
        ''' </summary>
        Public Sub BeforeNavigate2(ByVal pDisp As Object,
                                   ByRef URL As Object,
                                   ByRef flags As Object,
                                   ByRef targetFrameName As Object,
                                   ByRef postData As Object,
                                   ByRef headers As Object,
                                   ByRef cancel As Boolean) _
                               Implements DWebBrowserEvents2.BeforeNavigate2

            Dim _url As String = TryCast(URL, String)

            If String.IsNullOrEmpty(_url) OrElse _
                _url.Equals("about:blank", StringComparison.OrdinalIgnoreCase) Then
                Return
            End If

            If pDisp IsNot Nothing AndAlso pDisp.Equals(parent.ActiveXInstance) Then
                Dim e As New WebBrowserNavigatingEventArgs(
                    New Uri(_url), TryCast(targetFrameName, String))

                parent.OnStartNavigating(e)
            End If
        End Sub


    End Class
End Class

