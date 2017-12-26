Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Media
Imports System.Windows.Navigation

Namespace SDKSample

    Partial Class Pane1

        Sub AddRow(ByVal sender As Object, ByVal e As System.Windows.RoutedEventArgs)
            Dim row As New System.Windows.Documents.TableRow
            trg1.Rows.Add(row)
            Dim para As New System.Windows.Documents.Paragraph
            para.Inlines.Add("A new Row and Cell have been Added to the Table")
            Dim cell As New System.Windows.Documents.TableCell(para)
            row.Cells.Add(cell)
        End Sub

    End Class

End Namespace