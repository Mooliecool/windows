Imports System.Windows.Forms

' Interaction logic for App.xaml
Partial Public Class App
    Inherits System.Windows.Application

    Private isActive As Boolean
    Private mailDispatcher As MailDispatcher
    Private newMailNotifyIcon As NotifyIcon

    Public Sub New()
        Me.mailDispatcher = New MailDispatcher
        Me.newMailNotifyIcon = New NotifyIcon
    End Sub

    Private Sub App_Activated(ByVal sender As Object, ByVal e As EventArgs)
        Me.isActive = True
        Me.newMailNotifyIcon.Visible = False
    End Sub

    Private Sub App_Deactivated(ByVal sender As Object, ByVal e As EventArgs)
        Me.isActive = False
    End Sub

    Private Sub App_Exit(ByVal sender As Object, ByVal e As ExitEventArgs)
        Me.mailDispatcher.Stop()
    End Sub


    Private Sub App_Startup(ByVal sender As Object, ByVal e As StartupEventArgs)
        AddHandler Me.mailDispatcher.MailDispatched, New EventHandler(AddressOf Me.mailDispatcher_MailDispatched)
        Me.mailDispatcher.Start()
        Me.newMailNotifyIcon.Icon = ActivationSample.My.Resources.NewMailIcon
    End Sub

    Private Sub mailDispatcher_MailDispatched(ByVal sender As Object, ByVal e As EventArgs)
        DirectCast(MyBase.MainWindow, MainWindow).AddMailItem(DateTime.Now.ToString)
        If Not (Me.isActive OrElse Me.newMailNotifyIcon.Visible) Then
            Me.newMailNotifyIcon.Visible = True
        End If
    End Sub

End Class
