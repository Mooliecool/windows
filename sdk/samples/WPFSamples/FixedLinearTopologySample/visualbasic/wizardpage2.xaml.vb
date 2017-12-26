Public Class WizardPage2
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

    Private Sub nextButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Dim nextPage As New WizardPage3(DirectCast(MyBase.DataContext, WizardData))
        AddHandler nextPage.Return, New ReturnEventHandler(Of WizardResult)(AddressOf Me.WizardPage_Return)
        MyBase.NavigationService.Navigate(nextPage)
    End Sub

    Public Sub WizardPage_Return(ByVal sender As Object, ByVal e As ReturnEventArgs(Of WizardResult))
        Me.OnReturn(e)
    End Sub

End Class