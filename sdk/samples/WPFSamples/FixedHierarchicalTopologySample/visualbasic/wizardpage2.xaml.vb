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
        If Me.youngRadioButton.IsChecked.Value Then
            DirectCast(MyBase.DataContext, WizardData).DataItem4 = Nothing
            Dim nextPageYoung As New WizardPage3(DirectCast(MyBase.DataContext, WizardData))
            AddHandler nextPageYoung.Return, New ReturnEventHandler(Of WizardResult)(AddressOf Me.WizardPage_Return)
            MyBase.NavigationService.Navigate(nextPageYoung)
        Else
            DirectCast(MyBase.DataContext, WizardData).DataItem3 = Nothing
            Dim nextPageOld As New WizardPage4(DirectCast(MyBase.DataContext, WizardData))
            AddHandler nextPageOld.Return, New ReturnEventHandler(Of WizardResult)(AddressOf Me.WizardPage_Return)
            MyBase.NavigationService.Navigate(nextPageOld)
        End If
    End Sub

    Public Sub WizardPage_Return(ByVal sender As Object, ByVal e As ReturnEventArgs(Of WizardResult))
        Me.OnReturn(e)
    End Sub

End Class