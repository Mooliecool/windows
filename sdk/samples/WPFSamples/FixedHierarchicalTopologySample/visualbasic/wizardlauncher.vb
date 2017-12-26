Imports System
Imports System.Windows.Navigation

Public Class WizardLauncher
    Inherits PageFunction(Of WizardContext)

    Public Sub New()
        Me.WizardData = New WizardData
    End Sub

    Protected Overrides Sub Start()
        MyBase.Start()
        MyBase.KeepAlive = True
        Dim page1 As New WizardPage1(Me.WizardData)
        AddHandler page1.Return, New ReturnEventHandler(Of WizardResult)(AddressOf Me.WizardPage_Return)
        MyBase.NavigationService.Navigate(page1)
    End Sub

    Public Sub WizardPage_Return(ByVal sender As Object, ByVal e As ReturnEventArgs(Of WizardResult))
        Me.OnReturn(New ReturnEventArgs(Of WizardContext)(New WizardContext(e.Result, Me.WizardData)))
    End Sub

    Private WizardData As WizardData

End Class


