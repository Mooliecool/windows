Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Media
Imports System.Windows.Input

' Interaction logic for Window1.xaml
Namespace SDKSamples
    Partial Public Class Window1
        Inherits Window

        Public Sub New()
            InitializeComponent()
        End Sub
        Private Sub HandleButtonDown(ByVal sender As Object, ByVal e As MouseButtonEventArgs)

            ' Casting the source to a StackPanel
            Dim sourceStackPanel As StackPanel = CType(e.Source, StackPanel)

            ' If the button is pressed then make dimensions larger.
            If e.ButtonState = MouseButtonState.Pressed Then
                sourceStackPanel.Width = 200
                sourceStackPanel.Height = 200

                ' If the button is released then make dimensions smaller.
            ElseIf e.ButtonState = MouseButtonState.Released Then
                sourceStackPanel.Width = 100
                sourceStackPanel.Height = 100
            End If
        End Sub

    End Class

End Namespace

