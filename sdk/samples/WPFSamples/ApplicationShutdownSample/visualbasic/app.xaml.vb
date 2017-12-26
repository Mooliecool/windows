Imports System
Imports System.Diagnostics
Imports System.Windows

Public Class App

    Inherits Application

    Public Sub New()
        MyBase.ShutdownMode = System.Windows.ShutdownMode.OnExplicitShutdown
    End Sub

    Private Sub App_Exit(ByVal sender As Object, ByVal e As ExitEventArgs)
        Console.WriteLine("Exit")
    End Sub

End Class