Public Class WizardPage3
    Inherits PageFunction(Of WizardResult)

    Public Sub New(ByVal WizardData As WizardData)
        Me.InitializeComponent()
        MyBase.DataContext = WizardData
    End Sub

    Private Sub backButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        MyBase.NavigationService.GoBack()
    End Sub

    Private Sub cancelButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me.OnReturn(New ReturnEventArgs(Of WizardResult)(WizardResult.Canceled))
    End Sub

    Private Sub finishButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Me.OnReturn(New ReturnEventArgs(Of WizardResult)(WizardResult.Finished))
    End Sub

End Class