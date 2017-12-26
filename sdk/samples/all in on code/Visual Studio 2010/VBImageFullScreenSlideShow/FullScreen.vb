'******************************** Module Header ***********************************'
' Module Name:  FullScreen.vb
' Project:      VBImageFullScreenSlideShow
' Copyright (c) Microsoft Corporation.
'
' The class defines the helper methods to make a Windows Form enter full-screen and 
' leave full-screen.
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
' EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
' MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'**********************************************************************************'

Public Class FullScreen

    Private winState As FormWindowState
    Private brdStyle As FormBorderStyle
    Private topMost As Boolean
    Private bounds As Rectangle

    Public Sub New()
        IsFullScreen = False
    End Sub

    Public Property IsFullScreen() As Boolean

    ''' <summary>
    ''' Maximize the window to the full screen.
    ''' </summary>
    Public Sub EnterFullScreen(ByVal targetForm As Form)
        If Not IsFullScreen Then
            Save(targetForm) ' Save the original form state.

            targetForm.WindowState = FormWindowState.Maximized
            targetForm.FormBorderStyle = FormBorderStyle.None
            targetForm.TopMost = True
            targetForm.Bounds = Screen.GetBounds(targetForm)

            IsFullScreen = True
        End If
    End Sub

    ''' <summary>
    ''' Save the current Window state.
    ''' </summary>
    Private Sub Save(ByVal targetForm As Form)
        winState = targetForm.WindowState
        brdStyle = targetForm.FormBorderStyle
        topMost = targetForm.TopMost
        bounds = targetForm.Bounds
    End Sub

    ''' <summary>
    ''' Leave the full screen mode and restore the original window state.
    ''' </summary>
    Public Sub LeaveFullScreen(ByVal targetForm As Form)
        If IsFullScreen Then
            ' Restore the original Window state.
            targetForm.WindowState = winState
            targetForm.FormBorderStyle = brdStyle
            targetForm.TopMost = topMost
            targetForm.Bounds = bounds

            IsFullScreen = False
        End If
    End Sub
End Class
