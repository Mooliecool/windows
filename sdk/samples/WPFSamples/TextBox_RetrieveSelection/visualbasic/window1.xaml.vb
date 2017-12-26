Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Documents

Namespace SDKSample

    Partial Class Window1

        Public Sub New()
            InitializeComponent()
        End Sub

        Sub OnClick(ByVal sender As Object, ByVal e As RoutedEventArgs)
            Dim sSelectedText As String = tbSelectSomeText.SelectedText
            tbDisplaySelectedText.Text = tbSelectSomeText.SelectedText
        End Sub


    End Class

End Namespace