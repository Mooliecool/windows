Imports System
Imports System.Windows

Public Class MainWindow
    Inherits Window

    Public Sub New()
        Me.InitializeComponent()
    End Sub

    Private Sub runWizardButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Dim dlg As New WizardDialogBox
        Dim dialogResult As Nullable(Of Boolean) = dlg.ShowDialog
        If dialogResult.Value Then
            MessageBox.Show(String.Format("{0}" & ChrW(10) & "{1}" & ChrW(10) & "{2}", dlg.WizardData.DataItem1, dlg.WizardData.DataItem2, dlg.WizardData.DataItem3))
        Else
            MessageBox.Show("Canceled.")
        End If
    End Sub

End Class


