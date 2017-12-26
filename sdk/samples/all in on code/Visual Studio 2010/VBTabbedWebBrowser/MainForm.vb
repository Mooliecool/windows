'*************************** Module Header ******************************\
' Module Name:  MainForm.vb
' Project:	    VBTabbedWebBrowser
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
'**************************************************************************


Partial Public Class MainForm
    Inherits Form

    Public Sub New()
        InitializeComponent()

        ' Initialize the checkbox.
        chkEnableTab.Checked = TabbedWebBrowserContainer.IsTabEnabled

        AddHandler chkEnableTab.CheckedChanged, AddressOf chkEnableTab_CheckedChanged

    End Sub

    ''' <summary>
    ''' Handle the KeyDown event of tbUrl. When the key is Enter, then navigate
    ''' to the url in the tbUrl.
    ''' </summary>
    Private Sub tbUrl_KeyDown(ByVal sender As Object, ByVal e As KeyEventArgs) _
        Handles tbUrl.KeyDown
        If e.KeyCode = Keys.Enter Then
            e.Handled = True
            webBrowserContainer.Navigate(tbUrl.Text)
        End If
    End Sub

    ''' <summary>
    ''' Handle the event when Back button is clicked.
    ''' </summary>
    Private Sub btnBack_Click(ByVal sender As Object, ByVal e As EventArgs) _
        Handles btnBack.Click
        webBrowserContainer.GoBack()
    End Sub

    ''' <summary>
    ''' Handle the event when Forward button is clicked.
    ''' </summary>
    Private Sub btnForward_Click(ByVal sender As Object, ByVal e As EventArgs) _
        Handles btnForward.Click
        webBrowserContainer.GoForward()
    End Sub

    ''' <summary>
    ''' Handle the event when Refresh button is clicked.
    ''' </summary>
    Private Sub btnRefresh_Click(ByVal sender As Object, ByVal e As EventArgs) _
        Handles btnRefresh.Click
        webBrowserContainer.RefreshWebBrowser()
    End Sub

    ''' <summary>
    ''' Handle the event when New Tab button is clicked.
    ''' </summary>
    Private Sub btnNewTab_Click(ByVal sender As Object, ByVal e As EventArgs) _
        Handles btnNewTab.Click
        webBrowserContainer.NewTab("about:blank")
    End Sub

    ''' <summary>
    ''' Handle the event when Close Tab button is clicked.
    ''' </summary>
    Private Sub btnCloseTab_Click(ByVal sender As Object, ByVal e As EventArgs) _
        Handles btnCloseTab.Click
        webBrowserContainer.CloseActiveTab()
    End Sub

    ''' <summary>
    ''' Handle the CheckedChanged event of chkEnableTab.
    ''' </summary>
    Private Sub chkEnableTab_CheckedChanged(ByVal sender As Object, ByVal e As EventArgs)
        TabbedWebBrowserContainer.IsTabEnabled = chkEnableTab.Checked
        MessageBox.Show("The context menu ""Open in new tab"" will take effect" _
                        & " after the application is restarted.")
        Application.Restart()
    End Sub
End Class
