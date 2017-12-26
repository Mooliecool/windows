' This is a list of commonly used namespaces for a window.
Imports System     
Imports System.Windows     
Imports System.Windows.Controls     
Imports System.Windows.Documents     
Imports System.Windows.Navigation     


namespace Grid_Convert_Value

    Partial Public Class Window1
        Inherits Window

        Private Sub ChangeMargin(ByVal sender As Object, ByVal args As SelectionChangedEventArgs)

            Dim li As ListBoxItem = CType(CType(sender, ListBox).SelectedItem, ListBoxItem)
            Dim myThicknessConverter As ThicknessConverter = New ThicknessConverter()
            Dim th1 As Thickness = CType(myThicknessConverter.ConvertFromString(li.Content.ToString()), Thickness)
            text1.Margin = th1
            Dim st1 As String = CType(myThicknessConverter.ConvertToString(text1.Margin), String)
            gridVal.Text = "The Margin property is set to " + st1 + "."
        End Sub

    End Class
    End Namespace
