Imports System.Windows.Threading

Public Class MailDispatcher

    Private timer As DispatcherTimer

    Public Event MailDispatched As EventHandler

    Public Sub Start()
        Me.timer = New DispatcherTimer
        AddHandler Me.timer.Tick, New EventHandler(AddressOf Me.timer_Tick)
        Me.timer.IsEnabled = True
        Me.timer.Interval = New TimeSpan(0, 0, 5)
    End Sub

    Public Sub [Stop]()
        Me.timer.IsEnabled = False
        RemoveHandler Me.timer.Tick, New EventHandler(AddressOf Me.timer_Tick)
        Me.timer = Nothing
    End Sub


    Private Sub timer_Tick(ByVal sender As Object, ByVal e As EventArgs)
        RaiseEvent MailDispatched(Me, EventArgs.Empty)
    End Sub

End Class

