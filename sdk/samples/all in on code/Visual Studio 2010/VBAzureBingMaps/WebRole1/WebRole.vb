Imports System.Collections.Generic
Imports System.Linq
Imports Microsoft.WindowsAzure
Imports Microsoft.WindowsAzure.Diagnostics
Imports Microsoft.WindowsAzure.ServiceRuntime

Public Class WebRole
    Inherits RoleEntryPoint
    Public Overrides Function OnStart() As Boolean

        ' For information on handling configuration changes
        ' see the MSDN topic at http://go.microsoft.com/fwlink/?LinkId=166357.
        AddHandler RoleEnvironment.Changing, AddressOf RoleEnvironmentChanging

        Return MyBase.OnStart()
    End Function

    Private Sub RoleEnvironmentChanging(ByVal sender As Object, ByVal e As RoleEnvironmentChangingEventArgs)
        ' If a configuration setting is changing
        If e.Changes.Any(Function(change) TypeOf change Is RoleEnvironmentConfigurationSettingChange) Then
            ' Set e.Cancel to true to restart this role instance
            e.Cancel = True
        End If
    End Sub
End Class