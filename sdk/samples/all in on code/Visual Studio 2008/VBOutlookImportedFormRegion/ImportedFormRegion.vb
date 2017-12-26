'**************************** Module Header ********************************'
' Module Name:  ImportedFormRegion.vb
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

#Region "Imports directives"
Imports System.Windows.Forms
#End Region

Public Class ImportedFormRegion

#Region "Form Region Factory"

    <Microsoft.Office.Tools.Outlook.FormRegionMessageClass("IPM.Note.ImportedFormRegion")> _
    <Microsoft.Office.Tools.Outlook.FormRegionName("VBOutlookImportedFormRegion.ImportedFormRegion")> _
    Partial Public Class ImportedFormRegionFactory

        Private Sub InitializeManifest()
            Dim resources As System.Resources.ResourceManager = New System.Resources.ResourceManager(GetType(ImportedFormRegion))
            Me.Manifest.FormRegionType = Microsoft.Office.Tools.Outlook.FormRegionType.ReplaceAll
            Me.Manifest.Title = resources.GetString("Title")
            Me.Manifest.FormRegionName = resources.GetString("FormRegionName")
            Me.Manifest.Description = resources.GetString("Description")
            Me.Manifest.ShowInspectorCompose = True
            Me.Manifest.ShowInspectorRead = True
            Me.Manifest.ShowReadingPane = True

        End Sub

        ' Occurs before the form region is initialized.
        ' To prevent the form region from appearing, set e.Cancel to true.
        ' Use e.OutlookItem to get a reference to the current Outlook item.
        Private Sub ImportedFormRegionFactory_FormRegionInitializing(ByVal sender As Object, ByVal e As Microsoft.Office.Tools.Outlook.FormRegionInitializingEventArgs) Handles Me.FormRegionInitializing

        End Sub

    End Class

#End Region

    ' Occurs before the form region is displayed.
    ' Use Me.OutlookItem to get a reference to the current Outlook item.
    ' Use Me.OutlookFormRegion to get a reference to the form region.
    Private Sub ImportedFormRegion_FormRegionShowing(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.FormRegionShowing

    End Sub

#Region "Event Handlers"

    Private Sub btnSend_Click() Handles btnSend.Click
        Globals.ThisAddIn.mail.Send()
    End Sub

    Private Sub btnRemove_Click() Handles btnRemove.Click
        If comboBox1.ListIndex = -1 Then

            MessageBox.Show("Please choose an item first")
            Return
        End If
        comboBox1.RemoveItem(comboBox1.ListIndex)
    End Sub

    Private Sub btnAdd_Click() Handles btnAdd.Click
        Dim i As Integer = comboBox1.ListCount
        comboBox1.AddItem("Item" + i.ToString(), i)
    End Sub

    Private Sub btnDisable_Click() Handles btnDisable.Click
        If (btnTo.Enabled) Then
            btnDisable.Caption = "Enable Recipient Box"
            btnTo.Enabled = False
            _RecipientControl2.Enabled = 0
        Else
            btnDisable.Caption = "Disable Recipient Box"
            btnTo.Enabled = True
            _RecipientControl2.Enabled = -1
        End If
    End Sub

#End Region

    ' Occurs when the form region is closed.
    ' Use Me.OutlookItem to get a reference to the current Outlook item.
    ' Use Me.OutlookFormRegion to get a reference to the form region.
    Private Sub ImportedFormRegion_FormRegionClosed(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.FormRegionClosed

    End Sub

End Class
