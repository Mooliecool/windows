'/************************************* Module Header **************************************\
'* Module Name:	MyTaskPane.vb
'* Project:		VBOutlookUIDesigner
'* Copyright (c) Microsoft Corporation.
'* 
'* The VBOutlookUIDesigner sample demonstrates how to use the Ribbon Designer to create
'* customized Ribbon for Outlook 2007 inspectors.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* *  9/18/2009 18:00 PM Li ZhenHao Created
'\******************************************************************************************/

#Region "Imports directives"
Imports System
Imports System.Collections.Generic
Imports System.ComponentModel
Imports System.Drawing
Imports System.Data
Imports System.Linq
Imports System.Text
Imports System.Windows.Forms
Imports Microsoft.Office.Tools
Imports Outlook = Microsoft.Office.Interop.Outlook
Imports Word = Microsoft.Office.Interop.Word
#End Region


Public Class MyTaskPane
    ''' <summary>
    ''' Gets or sets the parent CTP object.
    ''' </summary>
    Private _ParentCustomTaskPane As CustomTaskPane

    Public Property ParentCustomTaskPane() As CustomTaskPane

        Get
            Return Me._ParentCustomTaskPane
        End Get
        Set(ByVal value As CustomTaskPane)
            Me._ParentCustomTaskPane = value
        End Set
    End Property

    Private Sub txtAddress_TextChanged(ByVal sender As Object, ByVal e As EventArgs) Handles txtAddress.TextChanged

        ' Updates the Go button state.
        btnGo.Enabled = txtAddress.Text.Trim().Length > 0
    End Sub

    Private Sub txtAddress_KeyDown(ByVal sender As Object, ByVal e As KeyEventArgs)

        ' If Enter is pressed, do the navigation by clicking
        ' the Go button.
        If e.KeyCode = Keys.Enter And btnGo.Enabled Then
            btnGo.PerformClick()
        End If
    End Sub

    Private Sub btnGo_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnGo.Click

        ' Start navigation.
        WebBrowser.Navigate(txtAddress.Text)
    End Sub

    Private Sub WebBrowser_DocumentCompleted(ByVal sender As Object, ByVal e As WebBrowserDocumentCompletedEventArgs)

        'Update the address bar.
        txtAddress.Text = WebBrowser.Document.Url.ToString()
    End Sub

    Private Sub btnPaste_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnPaste.Click

        'Get current inspector.
        Dim inspector As Outlook.Inspector = Me.ParentCustomTaskPane.Window
        If Not inspector Is Nothing Then

            ' Copy the selected content into clipboard.
            WebBrowser.Document.ExecCommand("Copy", False, Nothing)
            ' Get the document reference.
            Dim doc As Word.Document = inspector.WordEditor
            ' Paste the content into the document.
            doc.Application.Selection.Paste()
        End If
    End Sub
End Class
