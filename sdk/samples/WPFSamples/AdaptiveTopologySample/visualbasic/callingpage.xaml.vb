Imports System
Imports System.ComponentModel
Imports System.Diagnostics
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Documents
Imports System.Windows.Markup
Imports System.Windows.Navigation

Namespace AdaptiveTopologySample
    Public Class CallingPage
        Inherits Page

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        ' Determine how the Wizard completed and, if accepted, process the collected Wizard state
        Public Sub navigationHub_Return(ByVal sender As Object, ByVal e As ReturnEventArgs(Of WizardContext))
            ' Get Wizard state
            Dim WizardContext As WizardContext = e.Result

            Me.WizardResultsTextBlock.Visibility = Windows.Visibility.Visible

            ' How did the Wizard end?
            Me.WizardResultsTextBlock.Text = WizardContext.Result.ToString

            ' If the Wizard completed by being accpeted, display Wizard data
            If (WizardContext.Result = WizardResult.Finished) Then
                Me.WizardResultsTextBlock.Text = (Me.WizardResultsTextBlock.Text & ChrW(10) & "Data Item 1: " & DirectCast(WizardContext.Data, WizardData).DataItem1)
                Me.WizardResultsTextBlock.Text = (Me.WizardResultsTextBlock.Text & ChrW(10) & "Data Item 2: " & DirectCast(WizardContext.Data, WizardData).DataItem2)
                Me.WizardResultsTextBlock.Text = (Me.WizardResultsTextBlock.Text & ChrW(10) & "Data Item 3: " & DirectCast(WizardContext.Data, WizardData).DataItem3)
            End If

        End Sub

        Private Sub startWizardHyperlink_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

            ' Launch the Wizard
            Dim WizardNavigationHub As New WizardNavigationHub
            AddHandler WizardNavigationHub.Return, New ReturnEventHandler(Of WizardContext)(AddressOf Me.navigationHub_Return)
            MyBase.NavigationService.Navigate(WizardNavigationHub)

        End Sub

    End Class
End Namespace


