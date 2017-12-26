Public Class CallingPage
    Inherits Page

    Public Sub New()
        Me.InitializeComponent()
    End Sub

    Private Sub startWizardHyperlink_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Dim WizardLauncher As New WizardLauncher
        AddHandler WizardLauncher.Return, New ReturnEventHandler(Of WizardContext)(AddressOf Me.Wizard_Return)
        MyBase.NavigationService.Navigate(WizardLauncher)
    End Sub

    Public Sub Wizard_Return(ByVal sender As Object, ByVal e As ReturnEventArgs(Of WizardContext))
        Dim WizardContext As WizardContext = e.Result
        Me.WizardResultsTextBlock.Visibility = System.Windows.Visibility.Visible
        Me.WizardResultsTextBlock.Text = (Me.WizardResultsTextBlock.Text & ChrW(10) & WizardContext.Result.ToString)
        If (WizardContext.Result = WizardResult.Finished) Then
            Me.WizardResultsTextBlock.Text = (Me.WizardResultsTextBlock.Text & ChrW(10) & "Data Item 1: " & DirectCast(WizardContext.Data, WizardData).DataItem1)
            Me.WizardResultsTextBlock.Text = (Me.WizardResultsTextBlock.Text & ChrW(10) & "Data Item 2: " & DirectCast(WizardContext.Data, WizardData).DataItem2)
            Me.WizardResultsTextBlock.Text = (Me.WizardResultsTextBlock.Text & ChrW(10) & "Data Item 3: " & DirectCast(WizardContext.Data, WizardData).DataItem3)
            Me.WizardResultsTextBlock.Text = (Me.WizardResultsTextBlock.Text & ChrW(10) & "Data Item 4: " & DirectCast(WizardContext.Data, WizardData).DataItem4)
        End If
    End Sub

End Class