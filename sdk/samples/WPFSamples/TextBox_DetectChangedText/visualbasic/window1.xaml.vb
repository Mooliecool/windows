Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Documents

Namespace SDKSample

    Partial Class Window1

        ' This is a counter for the number of times the TextChanged fires
        ' for the tbCountingChanges TextBox.
        Private uiChanges As Integer = 0

        Public Sub New()
            InitializeComponent()
        End Sub

        ' Event handler for TextChanged Event.
        Private Sub textChangedEventHandler(ByVal sender As Object, ByVal args As TextChangedEventArgs)

            uiChanges += 1
            tbCounterText.Text = uiChanges.ToString()

        End Sub

    End Class

End Namespace