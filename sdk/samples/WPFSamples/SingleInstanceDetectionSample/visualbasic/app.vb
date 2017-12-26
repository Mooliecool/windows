Imports Microsoft.VisualBasic.ApplicationServices
Imports System.Windows

Public Class EntryPoint
    <STAThread()> _
    Public Shared Sub Main(ByVal args As String())
        Dim manager As SingleInstanceManager = New SingleInstanceManager
        manager.Run(args)
    End Sub
End Class

' Using VB bits to detect single instances and process accordingly:
'  * OnStartup is fired when the first instance loads
'  * OnStartupNextInstance is fired when the application is re-run again
'    NOTE: it is redirected to this instance thanks to IsSingleInstance
Public Class SingleInstanceManager
    Inherits WindowsFormsApplicationBase

    Public Sub New()
        MyBase.IsSingleInstance = True
    End Sub

    Protected Overrides Function OnStartup(ByVal e As ApplicationServices.StartupEventArgs) As Boolean
        ' First time app is launched
        Me.app = New SingleInstanceApplication
        Me.app.Run()
        Return False
    End Function

    Protected Overrides Sub OnStartupNextInstance(ByVal eventArgs As StartupNextInstanceEventArgs)
        ' Subsequent launches
        MyBase.OnStartupNextInstance(eventArgs)
        Me.app.Activate()
    End Sub

    Private app As SingleInstanceApplication

End Class

Public Class SingleInstanceApplication
    Inherits Application

    Public Sub Activate()
        ' Reactivate application's main window
        MyBase.MainWindow.Activate()
    End Sub

    Protected Overrides Sub OnStartup(ByVal e As Windows.StartupEventArgs)
        MyBase.OnStartup(e)
        ' Create and show the application's main window
        Dim mainWindow As MainWindow = New MainWindow
        mainWindow.Show()
    End Sub

End Class