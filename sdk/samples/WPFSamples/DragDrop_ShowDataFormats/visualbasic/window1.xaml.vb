Imports System     
Imports System.Windows     
Imports System.Windows.Controls     

Namespace SDKSample

    '@ <summary>
    '@ Interaction logic for Window1.xaml
    '@ </summary>

    Partial Public Class Window1
        Inherits Window

        Public Sub clickClear(ByVal sender As Object, ByVal args As RoutedEventArgs)
            tbDropTarget.Clear()
        End Sub

        Public Sub ehDragEnter(ByVal sender As Object, ByVal args As DragEventArgs)
            args.Effects = DragDropEffects.Copy
        End Sub

        Public Sub ehPreviewDrop(ByVal sender As Object, ByVal args As DragEventArgs)
            'TextBox includes native drop handling.  This is to let the TextBox know that we're handling
            'the Drop event, and we don't want the native handler to execute.  
            args.Handled = True
            ShowDataFormats(args)
        End Sub

        Public Sub ShowDataFormats(ByVal args As DragEventArgs)
            tbDropTarget.AppendText("The following data formats are present:" + Environment.NewLine)
            If cbAutoConvert.IsChecked.Value Then
                For Each format As String In args.Data.GetFormats(CType(cbAutoConvert.IsChecked, Boolean))
                    If args.Data.GetDataPresent(format, False) Then
                        tbDropTarget.AppendText("\t- " + format + " (native)" + Environment.NewLine)
                    Else
                        tbDropTarget.AppendText("\t- " + format + " (autoconvert)" + Environment.NewLine)
                    End If
                Next

            Else
                For Each format As String In args.Data.GetFormats(CType(cbAutoConvert.IsChecked, Boolean))
                    tbDropTarget.AppendText("\t- " + format + " (native)" + Environment.NewLine)
                Next
            End If

            tbDropTarget.AppendText(Environment.NewLine)
            tbDropTarget.ScrollToEnd()
        End Sub


    End Class
End Namespace
