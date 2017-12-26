'*************************** Module Header ******************************'
' Module Name:  MainForm.vb
' Project:	    VBRichTextBoxSyntaxHighlighting
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

        ' Initialize the XMLViewerSettings.
        Dim viewerSetting As XMLViewerSettings =
            New XMLViewerSettings With {
                .AttributeKey = Color.Red,
                .AttributeValue = Color.Blue,
                .Tag = Color.Blue,
                .Element = Color.DarkRed,
                .Value = Color.Black}

        viewer.Settings = viewerSetting

    End Sub

    ''' <summary>
    ''' Handle the Click event of the button "btnProcess".
    ''' </summary>
    Private Sub btnProcess_Click(ByVal sender As Object, ByVal e As EventArgs) _
        Handles btnProcess.Click
        Try
            viewer.Process(True)
        Catch appException As ApplicationException
            MessageBox.Show(appException.Message, "ApplicationException")
        Catch ex As Exception
            MessageBox.Show(ex.Message, "Exception")
        End Try

    End Sub

End Class
