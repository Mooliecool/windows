Imports System     
Imports System.Windows     
Imports System.Windows.Controls     

Namespace SDKSample

    '@ <summary>
    '@ Interaction logic for Window1.xaml
    '@ </summary>

    Partial Public Class Window1
        Inherits Window

        Public Sub TextSelectionToDataObject(ByVal sender As Object, ByVal args As RoutedEventArgs)
            'Create a new data object using one of the overloaded constructors.  This particular
            'overload accepts a string specifying the data format (provided by the DataFormats class),
            'and an Object (in this case a string) that represents the data to be stored in the data object.
            Dim dataObject As New DataObject(DataFormats.Text, sourceTextBox.SelectedText)
            dataObjectInfoTextBox.Clear()

            'Get and display the native data formats (filtering out auto-convertable data formats).
            dataObjectInfoTextBox.Text = Environment.NewLine + "Native data formats present:" + Environment.NewLine
            For Each format As String In dataObject.GetFormats(False)
                dataObjectInfoTextBox.Text += format + Environment.NewLine
            Next
            'Display the data in the data object.
            dataObjectInfoTextBox.Text += Environment.NewLine + "Data contents:" + Environment.NewLine
            dataObjectInfoTextBox.Text += dataObject.GetData(DataFormats.Text, False).ToString()
        End Sub
    End Class
End Namespace
