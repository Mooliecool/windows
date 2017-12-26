' <SnippetCallingPageDefaultCODEBEHIND1>
Imports System.Windows ' RoutedEventArgs, RoutedEventHandler, Visibility
Imports System.Windows.Controls ' Page
Imports System.Windows.Navigation ' ReturnEventArgs

Namespace StructuredNavigationSample

Public Class CallingPage
    Inherits Page
    Public Sub New()
        Me.InitializeComponent()
        AddHandler Me.pageFunctionHyperlink.Click, New RoutedEventHandler(AddressOf Me.pageFunctionHyperlink_Click)
    End Sub
    Private Sub pageFunctionHyperlink_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Instantiate and navigate to page function
        Dim calledPageFunction As New CalledPageFunction("Initial Data Item Value")
        AddHandler calledPageFunction.Return, New ReturnEventHandler(Of String)(AddressOf Me.calledPageFunction_Return)
        MyBase.NavigationService.Navigate(calledPageFunction)
    End Sub
    Private Sub calledPageFunction_Return(ByVal sender As Object, ByVal e As ReturnEventArgs(Of String))

        Me.pageFunctionResultsTextBlock.Visibility = Windows.Visibility.Visible

        ' Display result
        Me.pageFunctionResultsTextBlock.Text = IIf((Not e Is Nothing), "Accepted", "Canceled")

        ' If page function returned, display result and data
        If (Not e Is Nothing) Then
            Me.pageFunctionResultsTextBlock.Text = (Me.pageFunctionResultsTextBlock.Text & ChrW(10) & e.Result)
        End If

    End Sub
End Class

End Namespace
' </SnippetCallingPageDefaultCODEBEHIND3>