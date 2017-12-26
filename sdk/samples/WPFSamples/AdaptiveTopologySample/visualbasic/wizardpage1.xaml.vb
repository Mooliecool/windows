Imports System
Imports System.ComponentModel
Imports System.Diagnostics
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Markup
Imports System.Windows.Navigation

Namespace AdaptiveTopologySample
    Public Class WizardPage1
        Inherits PageFunction(Of WizardResult)

        Public Sub New(ByVal WizardData As Object)
            Me.InitializeComponent()

            ' Bind Wizard state to UI
            MyBase.DataContext = WizardData

            AddHandler MyBase.Loaded, New RoutedEventHandler(AddressOf Me.WizardPage_Loaded)
        End Sub

        Private Sub backButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
            ' Go to previous Wizard page
            MyBase.NavigationService.GoBack()
        End Sub

        Private Sub cancelButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
            ' Cancel the Wizard and don't return any data
            Me.OnReturn(New ReturnEventArgs(Of WizardResult)(WizardResult.Canceled))
        End Sub

        Private Sub finishButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
            ' Finish the Wizard and return bound data to calling page
            Me.OnReturn(New ReturnEventArgs(Of WizardResult)(WizardResult.Finished))
        End Sub

        Private Sub nextButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
            ' Go to next Wizard page
            If MyBase.NavigationService.CanGoForward Then
                MyBase.NavigationService.GoForward()
            Else
                Dim nextPage As PageFunction(Of WizardResult) = WizardNavigationHub.Current.GetNextWizardPage(Me)
                AddHandler nextPage.Return, New ReturnEventHandler(Of WizardResult)(AddressOf Me.WizardPage_Return)
                MyBase.NavigationService.Navigate(nextPage)
            End If
        End Sub

        Public Sub WizardPage_Return(ByVal sender As Object, ByVal e As ReturnEventArgs(Of WizardResult))
            ' This is called when the next page returns, so return what they are returning
            Me.OnReturn(e)
        End Sub

        Private Sub WizardPage_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)

            ' Enable buttons based on position
            Me.backButton.IsEnabled = WizardNavigationHub.Current.CanGoBack(Me)
            Me.nextButton.IsEnabled = WizardNavigationHub.Current.CanGoNext(Me)
            Me.nextButton.IsDefault = WizardNavigationHub.Current.CanGoNext(Me)
            Me.finishButton.IsEnabled = WizardNavigationHub.Current.CanFinish(Me)
            Me.finishButton.IsDefault = WizardNavigationHub.Current.CanFinish(Me)
        End Sub

    End Class
End Namespace