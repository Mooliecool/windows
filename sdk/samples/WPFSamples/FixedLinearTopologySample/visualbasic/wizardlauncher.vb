Public Class WizardLauncher
    Inherits PageFunction(Of WizardContext)

    Public Sub New()
        Me.WizardData = New WizardData
    End Sub

    Protected Overrides Sub Start()
        MyBase.Start()
        MyBase.KeepAlive = True
        Dim firstPage As New WizardPage1(Me.WizardData)
        AddHandler firstPage.Return, New ReturnEventHandler(Of WizardResult)(AddressOf Me.WizardPage_Return)
        MyBase.NavigationService.Navigate(firstPage)
    End Sub

    Public Sub WizardPage_Return(ByVal sender As Object, ByVal e As ReturnEventArgs(Of WizardResult))
        Me.OnReturn(New ReturnEventArgs(Of WizardContext)(New WizardContext(e.Result, Me.WizardData)))
    End Sub

    Private WizardData As WizardData

End Class