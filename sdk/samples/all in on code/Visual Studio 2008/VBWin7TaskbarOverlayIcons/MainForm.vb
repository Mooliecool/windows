'****************************** Module Header ******************************\
' Module Name:  MainForm.vb
' Project:      VBWin7TaskbarOverlayIcons
' Copyright (c) Microsoft Corporation.
' 
' Windows 7 Taskbar introduces Overlay Icons, which makes your application can 
' provide contextual status information to the user even if the application’s 
' window is not shown.  The user doesn’t even have to look at the thumbnail 
' or the live preview of your app – the taskbar button itself can reveal 
' whether you have any interesting status updates..
' 
' VBWin7TaskbarOverlayIcons example demostrates how to set and clear Taskbar
' Overlay Icons using Taskbar related APIs in Windows API Code Pack.
' 
' This MainForm enable the user select whether to show the Taskbar Overlay
' Icon and which icon to be displayed. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

#Region "Imports directive"
Imports Microsoft.WindowsAPICodePack.Taskbar
#End Region

Public Class MainForm

    ' Check the Windows version, if the system is not Windows 7 or
    ' Windows Server 2008 R2, exit the current process.
    Private Sub MainForm_Load(ByVal sender As System.Object, ByVal e As  _
                              System.EventArgs) Handles MyBase.Load
        If Not TaskbarManager.IsPlatformSupported Then
            MessageBox.Show("Overlay Icon is not supported in your " & _
                            "operation system!" & vbNewLine & "Please launch" & _
                            " the application in Windows 7 or " & _
                            "Windows Server 2008 R2 systems.")

            ' Exit the current process
            Application.Exit()
        End If
    End Sub


    ' Status changed, the Overlay Icon should be updated
    Private Sub showIconCheckBox_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles showIconCheckBox.CheckedChanged
        ShowOrHideOverlayIcon()
    End Sub


    ' Whether to show Overlay Icon is changed
    Private Sub statusComboBox_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles statusComboBox.SelectedIndexChanged
        ShowOrHideOverlayIcon()
    End Sub


    ''' <summary>
    ''' Show, hide and modify Taskbar button Overlay Icons
    ''' </summary>
    Private Sub ShowOrHideOverlayIcon()

        ' Show or hide the Overlay Icon
        If showIconCheckBox.Checked Then
            Dim icon As Icon = Nothing

            ' Select Overlay Icon image based on the selected status
            Select Case statusComboBox.SelectedIndex
                Case 0
                    icon = My.Resources.Available
                Case 1
                    icon = My.Resources.Away
                Case 2
                    icon = My.Resources.Offline
                Case Else
                    MessageBox.Show("Please set the Status to show the Overlay Icon!")
            End Select

            ' Set the Taskbar Overlay Icon
            TaskbarManager.Instance.SetOverlayIcon(icon, statusComboBox.SelectedIndex.ToString())
        Else
            ' Hide the Taskbar Overlay Icon
            TaskbarManager.Instance.SetOverlayIcon(Nothing, Nothing)
        End If
    End Sub
End Class
