'************************** Module Header ******************************\
' Module Name:  MainForm.vb
' Project:      VBWebBrowserLoadComplete
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
'*************************************************************************

Imports System.Security.Permissions

<PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust"),
PermissionSetAttribute(SecurityAction.InheritanceDemand, Name:="FullTrust")>
Partial Public Class MainForm
    Inherits Form

    ' The count of how many times DocumentCompleted event were fired.
    Private documentCompletedCount As Integer = 0

    ' The count of how many times LoadCompleted event were fired.
    Private loadCompletedCount As Integer = 0

    Public Sub New()
        InitializeComponent()

        ' Register the events of the System.Windows.Forms.WebBrowser.
        AddHandler webEx.DocumentCompleted, AddressOf webEx_DocumentCompleted
        AddHandler webEx.Navigating, AddressOf webEx_Navigating
        AddHandler webEx.Navigated, AddressOf webEx_Navigated

        AddHandler webEx.LoadCompleted, AddressOf webEx_LoadCompleted
        AddHandler webEx.StartNavigating, AddressOf webEx_StartNavigating

        Me.tbURL.Text = String.Format("{0}\Resource\FramesPage.htm",
               Environment.CurrentDirectory)
    End Sub

    ''' <summary>
    ''' Navigate to an URL.
    ''' </summary>
    Private Sub btnGo_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnGo.Click
        Try
            Dim url As New Uri(tbURL.Text)
            webEx.Navigate(url)
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try
    End Sub


    Private Sub webEx_DocumentCompleted(ByVal sender As Object,
        ByVal e As WebBrowserDocumentCompletedEventArgs)
        documentCompletedCount += 1
        DisplayStatus("Document Completed : " & e.Url.ToString)
    End Sub

    Private Sub webEx_Navigating(ByVal sender As Object,
                                 ByVal e As WebBrowserNavigatingEventArgs)
        DisplayStatus("Navigating : " & e.Url.ToString)
    End Sub

    Private Sub webEx_Navigated(ByVal sender As Object,
                                ByVal e As WebBrowserNavigatedEventArgs)
        DisplayStatus("Navigated : " & e.Url.ToString)
    End Sub

    Private Sub webEx_LoadCompleted(ByVal sender As Object,
                                    ByVal e As WebBrowserDocumentCompletedEventArgs)
        loadCompletedCount += 1
        DisplayStatus("Load Completed : " & e.Url.ToString)
    End Sub

    Private Sub webEx_StartNavigating(ByVal sender As Object,
                                      ByVal e As WebBrowserNavigatingEventArgs)
        documentCompletedCount = 0
        loadCompletedCount = 0
        DisplayStatus("Start Navigating : " & e.Url.ToString)
    End Sub

    ''' <summary>
    ''' Display the message.
    ''' </summary>
    Private Sub DisplayStatus(ByVal msg As String)
        Dim now As Date = Date.Now

        lstActivities.Items.Insert(0, String.Format("{0:HH:mm:ss}:{1:000} {2}",
                                                    now, now.Millisecond, msg))

        lstActivities.SelectedIndex = 0

        Me.lbStatus.Text = String.Format(
            "DocumentCompleted:{0} LoadCompleted:{1}",
            documentCompletedCount, loadCompletedCount)

    End Sub

End Class
