'****************************** Module Header ******************************\
' Module Name:  SubForm.vb
' Project:      VBWin7TaskbarAppID
' Copyright (c) Microsoft Corporation.
' 
' This SubForm remove its form object reference from the MainForm's list when
' this SubForm is closing. 
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Public Class SubForm

    ' Remove the current SubForm reference from the MainForm's list
    Private Sub SubForm_FormClosing(ByVal sender As System.Object, ByVal e _
                                    As FormClosingEventArgs) _
                                    Handles MyBase.FormClosing
        ' Find the MainForm reference
        Dim mainForm = TryCast(Application.OpenForms("MainForm"), MainForm)
        If Not mainForm Is Nothing Then
            ' Remove the current SubForm reference from the list
            mainForm.SubFormList.Remove(Me)
        End If
    End Sub
End Class