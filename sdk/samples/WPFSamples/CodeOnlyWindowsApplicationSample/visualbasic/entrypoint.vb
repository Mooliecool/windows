Imports System

Namespace CodeOnlyWindowsApplicationSample
    Public Class EntryPoint

        ' Entry point function

        ' WPF applications must run in a single-threaded apartment
        <STAThread()> _
        Public Shared Sub Main()
            ' Start the WPF application
            Dim _app As New App
            _app.Run()
        End Sub

    End Class
End Namespace


