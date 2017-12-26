Imports System

Public Class WizardDialogBox
    Inherits NavigationWindow

    Public Sub New()
        MyBase.InitializeComponent()
        Dim launcher As New WizardLauncher
        AddHandler launcher.WizardReturn, New WizardReturnEventHandler(AddressOf Me.wizardLauncher_WizardReturn)
        MyBase.Navigate(launcher)
    End Sub

    Private Sub wizardLauncher_WizardReturn(ByVal sender As Object, ByVal e As WizardReturnEventArgs)
        Me._wizardData = TryCast(e.Data, WizardData)
        If Not MyBase.DialogResult.HasValue Then
            MyBase.DialogResult = New Nullable(Of Boolean)((e.Result = WizardResult.Finished))
        End If
    End Sub

    Public ReadOnly Property WizardData() As WizardData
        Get
            Return Me._wizardData
        End Get
    End Property

    Private _wizardData As WizardData

End Class


