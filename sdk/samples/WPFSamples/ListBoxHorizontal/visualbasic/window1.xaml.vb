Imports System
Imports System.ComponentModel
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Documents
Imports System.Windows.Navigation
Imports System.Windows.Shapes
Imports System.Windows.Data

	public partial class Window1 
inherits Window

    Public Sub Window1()

        InitializeComponent()
    End Sub


    Private Sub PrintText(ByVal Sender As Object, ByVal e As SelectionChangedEventArgs)

        If Not (TypeOf Sender Is ListBox) Then
            Return
        End If

        Dim lb As ListBox = CType(Sender, ListBox)

        If Not (TypeOf lb.SelectedItem Is ListBoxItem) Then
            Return
        End If

        Dim lbi As ListBoxItem = CType(lb.SelectedItem, ListBoxItem)

        label1.Content = "You chose " + lbi.Content.ToString() + "."

    End Sub
End Class
     
    
    
