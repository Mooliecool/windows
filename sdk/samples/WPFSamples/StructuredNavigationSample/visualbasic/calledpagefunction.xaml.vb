'<SnippetCalledPageFunctionCODEBEHIND1>
Imports System ' String
Imports System.Windows ' RoutedEventArgs, RoutedEventHandler
Imports System.Windows.Navigation ' PageFunction

Namespace StructuredNavigationSample

Public Class CalledPageFunction
    Inherits PageFunction(Of String)
    Public Sub New()
        Me.InitializeComponent()
    End Sub
    Public Sub New(ByVal initialDataItem1Value As String)
        Me.InitializeComponent()
        AddHandler Me.okButton.Click, New RoutedEventHandler(AddressOf Me.okButton_Click)
        AddHandler Me.cancelButton.Click, New RoutedEventHandler(AddressOf Me.cancelButton_Click)
        ' Set initial value
        Me.dataItem1TextBox.Text = initialDataItem1Value
    End Sub
    Private Sub okButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Accept when Ok button is clicked
        Me.OnReturn(New ReturnEventArgs(Of String)(Me.dataItem1TextBox.Text))
    End Sub

    Private Sub cancelButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Cancel
        Me.OnReturn(Nothing)
    End Sub
End Class

End Namespace
'</SnippetCalledPageFunctionCODEBEHIND2>