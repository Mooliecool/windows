'****************************** Module Header ******************************\
' Module Name:  MainForm.vb
' Project:      VBWin7TaskbarProgressBar
' Copyright (c) Microsoft Corporation.
' 
' Windows 7 Taskbar introduces Taskbar Progress Bar, which makes your 
' application can provide contextual status information to the user even if 
' the application’s window is not shown.  The user doesn’t even have to 
' look at the thumbnail or the live preview of your app – the taskbar button 
' itself can reveal whether you have any interesting status updates..
' 
' VBWin7TaskbarProgressBar example demostrates how to set Taskbar Progress
' Bar state (normal, pause, indeterminate, error) and value, and flash window.  
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
Imports System.Runtime.InteropServices
#End Region

Public Class MainForm

    ' The max flash window count
    Const MAX_FLASH_WINDOW_COUNT As Integer = 6

    ' The max progress bar value
    Const MAX_PROGRESSBAR_VALUE As Integer = 100

    ' The current flash window count
    Private count As Integer = 0


    ' P/Invoke the Windows API FlashWindow to flash Taskbar button
    <DllImport("user32.dll")> _
    Private Shared Function FlashWindow(ByVal hwnd As IntPtr, ByVal bInvert _
                                        As Boolean) As Boolean
    End Function


    ' Check the Windows version, if the system is not Windows 7 or
    ' Windows Server 2008 R2, exit the current process.
    Private Sub MainForm_Load(ByVal sender As System.Object, ByVal e As  _
                              System.EventArgs) Handles MyBase.Load
        If Not TaskbarManager.IsPlatformSupported Then
            MessageBox.Show("Taskbar ProgressBar is not supported in your " & _
                            "operation system!" & vbNewLine & "Please launch " & _
                            "the application in Windows 7 or " & _
                            "Windows Server 2008 R2 systems.")

            '  Exit the current process
            Application.Exit()
        End If
    End Sub


    ' Start the timer to let the WinForm ProgressBar and Taskbar button
    ' Progress Bar
    Private Sub startButton_Click(ByVal sender As System.Object, ByVal e _
                                  As System.EventArgs) Handles startButton.Click
        progreeBarTimer.Enabled = True
    End Sub


    ' Update the WinForm Progress Bar and Taskbar button ProgressBar
    ' value every 100ms
    Private Sub progreeBarTimer_Tick(ByVal sender As System.Object, _
                                     ByVal e As System.EventArgs) Handles _
                                     progreeBarTimer.Tick
        UpdateProgress()
    End Sub

    
    ' Stop the timer to pause the WinForm ProgressBar and Taskbar
    ' button Progress Bar
    Private Sub pauseButton_Click(ByVal sender As System.Object, _
                                  ByVal e As System.EventArgs) Handles _
                                  pauseButton.Click
        ' Stop the timer
        progreeBarTimer.Enabled = False

        ' Set the Taskbar button Progress Bar state to Paused
        TaskbarManager.Instance.SetProgressState( _
                        TaskbarProgressBarState.Paused)

        ' Set the Taskbar button Progress Bar value
        TaskbarManager.Instance.SetProgressValue( _
                    progressBar.Value, MAX_PROGRESSBAR_VALUE)
    End Sub


    ' Stop the timer and to pause the WinForm ProgressBar and make the
    ' Taskbar Progress Bar state to Indeterminate
    Private Sub indeterminateButton_Click(ByVal sender As System.Object, _
                                          ByVal e As System.EventArgs) _
                                          Handles indeterminateButton.Click
        ' Stop the timer
        progreeBarTimer.Enabled = False

        ' Set the Taskbar button Progress Bar state to Indeterminate
        TaskbarManager.Instance.SetProgressState( _
                        TaskbarProgressBarState.Indeterminate)
    End Sub


    ' Stop the timer and to pause the WinForm ProgressBar and make the
    ' Taskbar Progress Bar state to Error
    Private Sub errorButton_Click(ByVal sender As System.Object, _
                                  ByVal e As System.EventArgs) _
                                  Handles errorButton.Click
        ' Stop the timer
        progreeBarTimer.Enabled = False

        ' Set the Taskbar button Progress Bar state to Error
        TaskbarManager.Instance.SetProgressState( _
                        TaskbarProgressBarState.Error)

        ' Set the Taskbar button Progress Bar value
        TaskbarManager.Instance.SetProgressValue( _
                    progressBar.Value, MAX_PROGRESSBAR_VALUE)
    End Sub


    ' Make the Taskbar button flash several times
    Private Sub flashButton_Click(ByVal sender As System.Object, _
                                  ByVal e As System.EventArgs) _
                                  Handles flashButton.Click
        flashWindowTimer.Enabled = True
    End Sub


    '  Make the Taskbar button flash (MAX_FLASH_WINDOW_COUNT / 2) times
    Private Sub flashWindowTimer_Tick(ByVal sender As System.Object, _
                                      ByVal e As System.EventArgs) _
                                      Handles flashWindowTimer.Tick
        If count < MAX_FLASH_WINDOW_COUNT Then
            count += 1

            ' Make the window flash or return to the original state
            FlashWindow(Me.Handle, (count Mod 2) = 0)
        Else
            ' Restore the count variable
            count = 0

            ' Stop the timer
            flashWindowTimer.Enabled = False
        End If
    End Sub


    ''' <summary>
    ''' Update the WinForm ProgressBar and Taskbar button Progress Bar
    ''' value
    ''' </summary>
    Private Sub UpdateProgress()
        If progressBar.Value < MAX_PROGRESSBAR_VALUE Then

            ' Increase the WinForm ProgressBar value
            progressBar.Value += 1
        Else

            ' Clear the WinForm ProgressBar value
            progressBar.Value = 0
        End If

        ' Set the Taskbar button Progress Bar state to Normal
        TaskbarManager.Instance.SetProgressState( _
                        TaskbarProgressBarState.Normal)

        ' Set the Taskbar button Progress Bar value
        TaskbarManager.Instance.SetProgressValue( _
                    progressBar.Value, MAX_PROGRESSBAR_VALUE)
    End Sub
End Class
