Imports System
Imports System.Runtime.CompilerServices
Imports System.Windows.Navigation

Public Class WizardLauncher
    Inherits PageFunction(Of WizardResult)

    Public Event WizardReturn As WizardReturnEventHandler

    Public Sub New()
        Me.wizardData = New WizardData
    End Sub

    Protected Overrides Sub Start()
        MyBase.Start()
        MyBase.KeepAlive = True
        Dim firstPage As New WizardPage1(Me.wizardData)
        AddHandler firstPage.Return, New ReturnEventHandler(Of WizardResult)(AddressOf Me.wizardPage_Return)
        MyBase.NavigationService.Navigate(firstPage)
    End Sub

    Public Sub wizardPage_Return(ByVal sender As Object, ByVal e As ReturnEventArgs(Of WizardResult))
        RaiseEvent WizardReturn(Me, New WizardReturnEventArgs(e.Result, Me.wizardData))
        Me.OnReturn(Nothing)
    End Sub

    Private wizardData As WizardData

End Class


