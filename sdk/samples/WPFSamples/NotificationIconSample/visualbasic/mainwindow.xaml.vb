Imports System
Imports System.Drawing
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Forms

Public Class MainWindow
    Inherits Window

    Public Sub New()
        Me.InitializeComponent()
    End Sub

    Private Sub click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me._notifyIcon = New NotifyIcon
        Me._notifyIcon.BalloonTipText = "Hello, NotifyIcon!"
        Me._notifyIcon.Text = "Hello, NotifyIcon!"
        Me._notifyIcon.Icon = New Icon("NotifyIcon.ico")
        Me._notifyIcon.Visible = True
        Me._notifyIcon.ShowBalloonTip(1000)
    End Sub

    Private _notifyIcon As NotifyIcon

End Class