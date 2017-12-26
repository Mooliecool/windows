'****************************** Module Header ******************************'
' Module Name:  MainWindow.xaml.vb
' Project:      VBWPFAutoCompleteTextBox
' Copyright (c) Microsoft Corporation.
' 
' This example demonstrates how to achieve AutoComplete TextBox in WPF 
' Application.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'


''' <summary> 
''' Interaction logic for MainWindow.xaml 
''' </summary> 
Partial Public Class MainWindow
    Inherits Window
    ''' <summary> 
    ''' Initializes a new instance of the <see cref="MainWindow"/> class. 
    ''' </summary> 
    Public Sub New()
        InitializeComponent()
        ' set dataSource for AutoComplete TextBox 
        ConstructAutoCompletionSource()
    End Sub

    ''' <summary> 
    ''' Constructs the auto completion source. 
    ''' </summary> 
    Private Sub ConstructAutoCompletionSource()

        Me.textBox.AutoSuggestionList.Add("Hello world")
        Me.textBox.AutoSuggestionList.Add("Hey buddy")
        Me.textBox.AutoSuggestionList.Add("Halo world")
        Me.textBox.AutoSuggestionList.Add("apple")
        Me.textBox.AutoSuggestionList.Add("apple tree")
        Me.textBox.AutoSuggestionList.Add("appaaaaa")
        Me.textBox.AutoSuggestionList.Add("arrange")
        For i As Integer = 0 To 99
            Me.textBox.AutoSuggestionList.Add("AA" & i)
        Next
    End Sub
End Class
