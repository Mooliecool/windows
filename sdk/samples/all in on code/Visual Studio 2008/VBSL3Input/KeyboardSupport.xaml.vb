'****************************** Module Header ******************************'
' Module Name:              KeyboardSupport.xaml.vb
' Project:                       VBSL3Input
' Copyright (c) Microsoft Corporation.
' 
' The KeyboardSupport UserControl's codebheind file, implemented Keyboard 
' event register function.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' * 7/28/2009 6:05 PM Mog Liang Created
' * 7/29/2009 6:25 PM Jialiang Ge Reviewed
'***************************************************************************'


Partial Public Class KeyboardSupport
    Inherits UserControl

    Public Sub New()
        Me.InitializeComponent()
        AddHandler MyBase.Loaded, New RoutedEventHandler(AddressOf Me.KeyboardSupport_Loaded)
    End Sub


    Private Sub Button_KeyDown(ByVal sender As Object, ByVal e As KeyEventArgs)
        Dim button As Button = TryCast(sender, Button)

        ' Determine moving direction and offset
        Dim step1 As Double = 5
        Dim x As Double = Canvas.GetLeft(button)
        Dim y As Double = Canvas.GetTop(button)
        Select Case e.Key
            Case Key.Left
                x = x - step1
                Exit Select
            Case Key.Up
                y = y - step1
                Exit Select
            Case Key.Right
                x = x + step1
                Exit Select
            Case Key.Down
                y = y + step1
                Exit Select
        End Select
        Canvas.SetLeft(button, x)
        Canvas.SetTop(button, y)

        ' Check if ctrl+z combination is pressed
        If (((Keyboard.Modifiers And ModifierKeys.Control) = ModifierKeys.Control) AndAlso (e.Key = Key.Z)) Then
            Canvas.SetLeft(button, 0)
            Canvas.SetTop(button, 0)
        End If
        button.Content = ("Press Key: " & e.Key.ToString())

    End Sub


    Private Sub Button_KeyUp(ByVal sender As Object, ByVal e As KeyEventArgs)
        ' Reset button content when key up.
        DirectCast(sender, Button).Content = "Press Key"
    End Sub


    Private Sub KeyboardSupport_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)

        ' When usercontrol loaded, set focus to button, then button could 
        ' catch key event.
        Me.btn1.Focus()

    End Sub

End Class
