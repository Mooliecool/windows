'*************************** Module Header ******************************'
' Module Name:  MainForm.vb
' Project:	    VBWebBrowserWithProxy
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
    ' Get the current proxy.
    Private ReadOnly Property CurrentProxy() As InternetProxy
        Get
            If radNoProxy.Checked Then
                Return InternetProxy.NoProxy
            Else
                Return TryCast(cmbProxy.SelectedItem, InternetProxy)
            End If
        End Get
    End Property

    <PermissionSetAttribute(SecurityAction.LinkDemand, Name:="FullTrust")> _
    Public Sub New()
        InitializeComponent()
    End Sub

    Private Sub MainForm_Load(ByVal sender As Object, ByVal e As EventArgs) _
        Handles MyBase.Load


        ' Data bind cmbProxy to display the ProxyList.
        cmbProxy.DisplayMember = "ProxyName"
        cmbProxy.DataSource = InternetProxy.ProxyList
        cmbProxy.SelectedIndex = 0

        AddHandler wbcSample.StatusTextChanged, AddressOf wbcSample_StatusTextChanged

    End Sub

    ''' <summary>
    ''' Handle btnNavigate_Click event.
    ''' The method Goto of WebBrowserControl class wraps the Navigate method of
    ''' WebBrowser class to set the Proxy-Authorization header if needed.
    ''' </summary>
    Private Sub btnNavigate_Click(ByVal sender As Object, ByVal e As EventArgs) _
        Handles btnNavigate.Click
        Try
            wbcSample.Proxy = CurrentProxy
            wbcSample.Goto(tbUrl.Text)
        Catch e1 As ArgumentException
            MessageBox.Show("Please maske sure that the url is valid.")
        End Try
    End Sub

    Private Sub wbcSample_ProgressChanged(ByVal sender As Object, _
                                          ByVal e As WebBrowserProgressChangedEventArgs) _
                                      Handles wbcSample.ProgressChanged

        prgBrowserProcess.Value = CInt(Fix(e.CurrentProgress))
        AddHandler wbcSample.StatusTextChanged, AddressOf wbcSample_StatusTextChanged
    End Sub

    Private Sub wbcSample_StatusTextChanged(ByVal sender As Object, ByVal e As EventArgs)
        lbStatus.Text = wbcSample.StatusText
    End Sub
End Class

