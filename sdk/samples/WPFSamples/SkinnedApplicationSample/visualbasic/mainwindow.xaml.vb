Imports System
Imports System.Windows

Public Class MainWindow
    Inherits Window

    Public Sub New()
        Me.InitializeComponent()
        Me.skinComboBox.Items.Add("Blue")
        Me.skinComboBox.Items.Add("Yellow")
        Me.skinComboBox.SelectedIndex = 0
        Application.Current.Resources = DirectCast(Application.Current.Properties.Item("Blue"), ResourceDictionary)
        AddHandler Me.skinComboBox.SelectionChanged, New SelectionChangedEventHandler(AddressOf Me.skinComboBox_SelectionChanged)
    End Sub

    Private Sub newChildWindowButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Dim window As ChildWindow = New ChildWindow
        window.Show()
    End Sub

    Private Sub skinComboBox_SelectionChanged(ByVal sender As Object, ByVal e As SelectionChangedEventArgs)
        Dim skin As String = CStr(e.AddedItems.Item(0))
        Application.Current.Resources = DirectCast(Application.Current.Properties.Item(skin), ResourceDictionary)
    End Sub
End Class