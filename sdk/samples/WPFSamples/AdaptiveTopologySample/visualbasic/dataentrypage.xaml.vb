Imports System
Imports System.ComponentModel
Imports System.Diagnostics
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Markup
Imports System.Windows.Navigation

Namespace AdaptiveTopologySample
    Public Class DataEntryPage
        Inherits PageFunction(Of WizardContext)

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        Private Sub cancelButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
            ' Cancel the Wizard and don't return any data
            Me.OnReturn(New ReturnEventArgs(Of WizardContext)(New WizardContext(WizardResult.Canceled, Nothing)))
        End Sub

        Private Sub okButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
            ' Go to next Wizard - as determined by the navigation hub
            Dim direction1 As WizardNavigationDirection = IIf(Me.forwardsRadioButton.IsChecked.Value, WizardNavigationDirection.Forwards, WizardNavigationDirection.Reverse)
            Me.OnReturn(New ReturnEventArgs(Of WizardContext)(New WizardContext(WizardResult.Finished, direction1)))
        End Sub

    End Class
End Namespace