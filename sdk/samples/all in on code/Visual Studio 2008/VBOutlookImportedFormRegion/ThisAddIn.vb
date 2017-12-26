'**************************** Module Header ********************************'
' Module Name:  ThisAddIn.vb
' Project:      VBOutlookImportedFormRegion
' Copyright (c) Microsoft Corporation.
' 
' The VBOutlookImportedFormRegion provides the examples on how to built a
' Outlook Custom Form with Outlook style look&feel.
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE. 
'***************************************************************************'

Public Class ThisAddIn

    Private btnCreateCustomItem As Office.CommandBarButton
    Friend mail As Outlook.MailItem

    Private Sub ThisAddIn_Startup(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Startup

        'Create a new button on Explorer to show the form region.
        Dim CustomBar As Office.CommandBar = Application.ActiveExplorer().CommandBars.Add("CSImportedFormRegion", Office.MsoBarPosition.msoBarTop, False, True)
        CustomBar.Visible = True
        btnCreateCustomItem = CustomBar.Controls.Add(Office.MsoControlType.msoControlButton, Temporary:=True)
        btnCreateCustomItem.Caption = "Create A Custom Item"
        AddHandler btnCreateCustomItem.Click, AddressOf btnCreateCustomItem_Click
    End Sub

    Private Sub btnCreateCustomItem_Click(ByVal Ctrl As Microsoft.Office.Core.CommandBarButton, ByRef CancelDefault As Boolean)
        mail = Application.CreateItem(Microsoft.Office.Interop.Outlook.OlItemType.olMailItem)
        mail.MessageClass = "IPM.Note.ImportedFormRegion"
        mail.Display()
    End Sub

    Private Sub ThisAddIn_Shutdown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shutdown

    End Sub

End Class
