'****************************** Module Header ******************************'
' Module Name:  AutoCompleteTextBox.vb
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


Imports System.Collections.ObjectModel

'Namespace UserControls

' Achieve AutoComplete TextBox or ComboBox 
Public Class AutoCompleteTextBox
    Inherits ComboBox
    ''' <summary> 
    ''' Initializes a new instance of the <see cref="AutoCompleteTextBox"/> class. 
    ''' </summary> 
    Public Sub New()
        'load and apply style to the ComboBox. 
        Dim rd As New ResourceDictionary()
        rd.Source = New Uri("/" & Me.[GetType]().Assembly.GetName().Name & ";component/UserControls/AutoCompleteComboBoxStyle.xaml", UriKind.Relative)
        Me.Resources = rd
        'disable default Text Search Function 
        Me.IsTextSearchEnabled = False
    End Sub

    ''' <summary> 
    ''' override OnApplyTemplate method 
    ''' get TextBox control out of Combobox control, and hook up TextChanged event. 
    ''' </summary> 
    Public Overloads Overrides Sub OnApplyTemplate()
        MyBase.OnApplyTemplate()
        'get the textbox control in the ComboBox control 
        Dim textBox As TextBox = TryCast(Me.Template.FindName("PART_EditableTextBox", Me), TextBox)
        If textBox IsNot Nothing Then
            'disable Autoword selection of the TextBox 
            textBox.AutoWordSelection = False
            'handle TextChanged event to dynamically add Combobox items. 
            AddHandler textBox.TextChanged, AddressOf textBox_TextChanged
        End If
    End Sub

    'The autosuggestionlist source. 
    Private _autoSuggestionList As New ObservableCollection(Of String)()

    ''' <summary> 
    ''' Gets or sets the auto suggestion list. 
    ''' </summary> 
    ''' <value>The auto suggestion list.</value> 
    Public Property AutoSuggestionList() As ObservableCollection(Of String)
        Get
            Return _autoSuggestionList
        End Get
        Set(ByVal value As ObservableCollection(Of String))
            _autoSuggestionList = value
        End Set
    End Property


    ''' <summary> 
    ''' main logic to generate auto suggestion list. 
    ''' </summary> 
    ''' <param name="sender">The source of the event.</param> 
    ''' <param name="e">The <see cref="System.Windows.Controls.TextChangedEventArgs"/> 
    ''' instance containing the event data.</param> 
    Private Sub textBox_TextChanged(ByVal sender As Object, ByVal e As TextChangedEventArgs)
        Dim textBox As TextBox = TryCast(sender, TextBox)
        textBox.AutoWordSelection = False
        ' if the word in the textbox is selected, then don't change item collection 
        If (textBox.SelectionStart <> 0 OrElse textBox.Text.Length = 0) Then
            Me.Items.Clear()
            'add new filtered items according the current TextBox input 
            If Not String.IsNullOrEmpty(textBox.Text) Then
                For Each s As String In Me._autoSuggestionList
                    If s.StartsWith(textBox.Text, StringComparison.InvariantCultureIgnoreCase) Then

                        Dim unboldpart As String = s.Substring(textBox.Text.Length)
                        Dim boldpart As String = s.Substring(0, textBox.Text.Length)
                        'construct AutoCompleteEntry and add to the ComboBox 
                        Dim entry As New AutoCompleteEntry(s, boldpart, unboldpart)
                        Me.Items.Add(entry)
                    End If
                Next
            End If
        End If
        ' open or close dropdown of the ComboBox according to whether there are items in the 
        ' fitlered result. 
        Me.IsDropDownOpen = Me.HasItems

        'avoid auto selection 
        textBox.Focus()

        textBox.SelectionStart = textBox.Text.Length
    End Sub
End Class

'class derived from ComboBoxItem. 
''' <summary> 
''' Extended ComboBox Item 
''' </summary> 
Public Class AutoCompleteEntry
    Inherits ComboBoxItem
    Private tb As TextBlock

    'text of the item 
    Private _text As String

    ''' <summary> 
    ''' Contrutor of AutoCompleteEntry class 
    ''' </summary> 
    ''' <param name="text">All the Text of the item </param> 
    ''' <param name="bold">The already entered part of the Text</param> 
    ''' <param name="unbold">The remained part of the Text</param> 
    Public Sub New(ByVal text As String, ByVal bold As String, ByVal unbold As String)
        _text = text
        tb = New TextBlock()
        'highlight the current input Text 
        tb.Inlines.Add(New Run() With {.Text = bold, .FontWeight = FontWeights.Bold, .Foreground = New SolidColorBrush(Colors.RoyalBlue)})
        tb.Inlines.Add(New Run() With {.Text = unbold})
        Me.Content = tb
    End Sub

    ''' <summary> 
    ''' Gets the text. 
    ''' </summary> 
    Public ReadOnly Property Text() As String
        Get
            Return _text
        End Get
    End Property
End Class
'End Namespace