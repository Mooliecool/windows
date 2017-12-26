'*************************** Module Header ******************************'
' Module Name:  MainForm.vb
' Project:	    VBWebBrowserSuppressError
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

Partial Public Class MainForm
    Inherits Form

    Public Sub New()
        InitializeComponent()
    End Sub

    Private Sub MainForm_Load(ByVal sender As Object, ByVal e As EventArgs) _
        Handles MyBase.Load
        ' Initialize the properties ScriptErrorsSuppressed and HtmlElementErrorsSuppressed 
        ' of the browser.
        wbcSample.ScriptErrorsSuppressed = chkSuppressAllDialog.Checked
        wbcSample.HtmlElementErrorsSuppressed = chkSuppressHtmlElementError.Checked

        ' Add a handler to the NavigateError event of the web borwser control.
        AddHandler wbcSample.NavigateError, AddressOf wbcSample_NavigateError

        ' Get the current value of DisableJITDebugger in the key 
        ' HKEY_CURRENT_USER\Software\Microsoft\Internet Explorer\Main.
        chkSuppressJITDebugger.Checked =
            VBWebBrowserSuppressError.WebBrowserEx.JITDebuggerDisabled

        AddHandler chkSuppressJITDebugger.CheckedChanged,
            AddressOf chkSuppressJITDebugger_CheckedChanged
    End Sub


    ''' <summary>
    ''' Handle btnNavigate_Click event.
    ''' If the text of tbUrl is not empty, then navigate to the url, else navigate to
    ''' the build-in Error.htm.
    ''' </summary>
    Private Sub btnNavigate_Click(ByVal sender As Object, ByVal e As EventArgs) _
        Handles btnNavigate.Click
        Try
            If Not String.IsNullOrEmpty(tbUrl.Text.Trim()) Then
                wbcSample.Navigate(tbUrl.Text)
            Else
                wbcSample.Navigate(Environment.CurrentDirectory & "\HTMLPages\Error.htm")
            End If
        Catch e1 As ArgumentException
            MessageBox.Show("Please make sure that the url is valid.")
        End Try
    End Sub


    ''' <summary>
    ''' Enable or disable JITDebugger of the web browser if 
    ''' chkSuppressJITDebugger.Checked changed.
    ''' </summary>
    Private Sub chkSuppressJITDebugger_CheckedChanged(ByVal sender As Object,
                                                     ByVal e As EventArgs)
        WebBrowserEx.JITDebuggerDisabled = chkSuppressJITDebugger.Checked
        MessageBox.Show("To disable/enable script debugger, the application need" _
                        & " restart to take effect.")
    End Sub

    ''' <summary>
    ''' Set the property SuppressWindowScriptError of the web browser if 
    ''' chkSuppressScriptError.Checked changed.
    ''' </summary>
    Private Sub chkSuppressHtmlElementError_CheckedChanged(ByVal sender As Object, ByVal e As EventArgs) _
                                                 Handles chkSuppressHtmlElementError.CheckedChanged
        wbcSample.HtmlElementErrorsSuppressed = chkSuppressHtmlElementError.Checked
    End Sub

    ''' <summary>
    ''' Set the property ScriptErrorsSuppressed of the web browser if 
    ''' chkSuppressAllDialog.Checked changed.
    ''' </summary>
    Private Sub chkSuppressAllDialog_CheckedChanged(ByVal sender As Object, ByVal e As EventArgs) _
                                                Handles chkSuppressAllDialog.CheckedChanged
        wbcSample.ScriptErrorsSuppressed = chkSuppressAllDialog.Checked
    End Sub

    ''' <summary>
    ''' Handle the Navigation Error.
    ''' </summary>
    Private Sub wbcSample_NavigateError(ByVal sender As Object,
                                        ByVal e As WebBrowserNavigateErrorEventArgs)

        ' Navigate to the build-in 404.htm if the http status code is 404.
        If chkSuppressNavigationError.Checked AndAlso e.StatusCode = 404 Then
            wbcSample.Navigate(String.Format("{0}\HTMLPages\404.htm",
                                             Environment.CurrentDirectory))
        End If
    End Sub

End Class

