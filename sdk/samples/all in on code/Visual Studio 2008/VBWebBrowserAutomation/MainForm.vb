'*************************** Module Header ******************************'
' Module Name:  MainForm.vb
' Project:	    VBWebBrowserAutomation
' Copyright (c) Microsoft Corporation.
' 
' This is the main form of this application. It is used to initialize the UI and 
' handle the events.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.Security.Permissions

Partial Public Class MainForm
    Inherits Form

    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
    Public Sub New()
        InitializeComponent()

        ' Register the events.
        AddHandler webBrowser.Navigating, AddressOf webBrowser_Navigating
        AddHandler webBrowser.DocumentCompleted, AddressOf webBrowser_DocumentCompleted
    End Sub

    ''' <summary>
    ''' Disable the btnAutoComplete when webBrowser is navigating.
    ''' </summary>
    Private Sub webBrowser_Navigating(ByVal sender As Object, _
                                      ByVal e As WebBrowserNavigatingEventArgs)
        btnAutoComplete.Enabled = False
    End Sub

    ''' <summary>
    ''' Refresh the UI after the web page is loaded.
    ''' </summary>
    Private Sub webBrowser_DocumentCompleted(ByVal sender As Object, _
                                             ByVal e As WebBrowserDocumentCompletedEventArgs)
        btnAutoComplete.Enabled = webBrowser.CanAutoComplete
        tbUrl.Text = e.Url.ToString()
    End Sub

    ''' <summary>
    ''' Handle the Click event of btnAutoComplete_
    ''' </summary>
    Private Sub btnAutoComplete_Click(ByVal sender As Object, _
                                      ByVal e As EventArgs) Handles btnAutoComplete.Click
        Try
            webBrowser.AutoComplete()
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try
    End Sub

    ''' <summary>
    ''' Hanle the Click event of the btnGo.
    ''' </summary>
    Private Sub btnGo_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnGo.Click
        Try
            webBrowser.Navigate(tbUrl.Text)
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try
    End Sub
End Class
