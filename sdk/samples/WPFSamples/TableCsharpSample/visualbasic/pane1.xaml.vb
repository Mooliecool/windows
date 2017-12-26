Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Media
Imports System.Windows.Navigation
Imports System.Windows.Documents

Namespace SDKSample

    Partial Class Pane1

        Private Sub onInit(ByVal sender As Object, ByVal args As RoutedEventArgs)

            'Create a new Table Element
            Dim table1 As New System.Windows.Documents.Table
            table1.CellSpacing = 10

            'Add TableColumns to the Table
            Dim col1 As New System.Windows.Documents.TableColumn
            Dim col2 As New System.Windows.Documents.TableColumn
            col2.Background = System.Windows.Media.Brushes.LightSteelBlue
            Dim col3 As New System.Windows.Documents.TableColumn
            col3.Background = System.Windows.Media.Brushes.Beige
            Dim col4 As New System.Windows.Documents.TableColumn
            col4.Background = System.Windows.Media.Brushes.LightSteelBlue
            Dim col5 As New System.Windows.Documents.TableColumn
            col5.Background = System.Windows.Media.Brushes.Beige
            Dim col6 As New System.Windows.Documents.TableColumn
            col6.Background = System.Windows.Media.Brushes.LightSteelBlue

            table1.Columns.Add(col1)
            table1.Columns.Add(col2)
            table1.Columns.Add(col3)
            table1.Columns.Add(col4)
            table1.Columns.Add(col5)
            table1.Columns.Add(col6)

            Dim trg1 As New System.Windows.Documents.TableRowGroup
            table1.RowGroups.Add(trg1)

            Dim row1 As New System.Windows.Documents.TableRow
            row1.Background = System.Windows.Media.Brushes.Silver
            trg1.Rows.Add(row1)

            Dim para1 As New System.Windows.Documents.Paragraph
            para1.Inlines.Add(New Run("2004 Sales Projection"))
            Dim cell1 As New System.Windows.Documents.TableCell(para1)
            cell1.ColumnSpan = 6
            cell1.FontSize = 40
            cell1.FontWeight = System.Windows.FontWeights.Bold
            row1.Cells.Add(cell1)

            Dim row2 As New System.Windows.Documents.TableRow
            trg1.Rows.Add(row2)

            Dim para2 As New System.Windows.Documents.Paragraph
            para2.Inlines.Add(New Run("Product"))
            Dim cell2 As New System.Windows.Documents.TableCell(para2)
            cell2.FontSize = 18
            cell2.FontWeight = System.Windows.FontWeights.Bold
            row2.Cells.Add(cell2)

            Dim para3 As New System.Windows.Documents.Paragraph
            para3.Inlines.Add(New Run("Quarter 1"))
            Dim cell3 As New System.Windows.Documents.TableCell(para3)
            cell3.FontSize = 18
            cell3.FontWeight = System.Windows.FontWeights.Bold
            row2.Cells.Add(cell3)

            Dim para4 As New System.Windows.Documents.Paragraph
            para4.Inlines.Add(New Run("2004 Sales Projection"))
            Dim cell4 As New System.Windows.Documents.TableCell(para4)
            cell4.FontSize = 18
            cell4.FontWeight = System.Windows.FontWeights.Bold
            row2.Cells.Add(cell4)

            Dim para5 As New System.Windows.Documents.Paragraph
            para5.Inlines.Add(New Run("Quarter 3"))
            Dim cell5 As New System.Windows.Documents.TableCell(para5)
            cell5.FontSize = 18
            cell5.FontWeight = System.Windows.FontWeights.Bold
            row2.Cells.Add(cell5)

            Dim para6 As New System.Windows.Documents.Paragraph
            para6.Inlines.Add(New Run("Quarter 4"))
            Dim cell6 As New System.Windows.Documents.TableCell(para6)
            cell6.FontSize = 18
            cell6.FontWeight = System.Windows.FontWeights.Bold
            row2.Cells.Add(cell6)

            Dim para7 As New System.Windows.Documents.Paragraph
            para7.Inlines.Add(New Run("Total"))
            Dim cell7 As New System.Windows.Documents.TableCell(para7)
            cell7.FontSize = 18
            cell7.FontWeight = System.Windows.FontWeights.Bold
            row2.Cells.Add(cell7)

            Dim row3 As New System.Windows.Documents.TableRow
            trg1.Rows.Add(row3)

            Dim para8 As New System.Windows.Documents.Paragraph
            para8.Inlines.Add(New Run("Widgets"))
            Dim cell8 As New System.Windows.Documents.TableCell(para8)
            cell8.FontSize = 12
            cell8.FontWeight = System.Windows.FontWeights.Bold
            row3.Cells.Add(cell8)

            Dim para9 As New System.Windows.Documents.Paragraph
            para9.Inlines.Add(New Run("$50,000"))
            Dim cell9 As New System.Windows.Documents.TableCell(para9)
            cell9.FontSize = 12
            cell9.FontWeight = System.Windows.FontWeights.Normal
            row3.Cells.Add(cell9)

            Dim para10 As New System.Windows.Documents.Paragraph
            para10.Inlines.Add(New Run("$55,000"))
            Dim cell10 As New System.Windows.Documents.TableCell(para10)
            cell10.FontSize = 12
            cell10.FontWeight = System.Windows.FontWeights.Normal
            row3.Cells.Add(cell10)

            Dim para11 As New System.Windows.Documents.Paragraph
            para11.Inlines.Add(New Run("$60,000"))
            Dim cell11 As New System.Windows.Documents.TableCell(para11)
            cell11.FontSize = 12
            cell11.FontWeight = System.Windows.FontWeights.Normal
            row3.Cells.Add(cell11)

            Dim para12 As New System.Windows.Documents.Paragraph
            para12.Inlines.Add(New Run("$65,000"))
            Dim cell12 As New System.Windows.Documents.TableCell(para12)
            cell12.FontSize = 12
            cell12.FontWeight = System.Windows.FontWeights.Normal
            row3.Cells.Add(cell12)

            Dim para13 As New System.Windows.Documents.Paragraph
            para13.Inlines.Add(New Run("$230,000"))
            Dim cell13 As New System.Windows.Documents.TableCell(para13)
            cell13.FontSize = 12
            cell13.FontWeight = System.Windows.FontWeights.Normal
            row3.Cells.Add(cell13)

            Dim row4 As New System.Windows.Documents.TableRow
            trg1.Rows.Add(row4)

            Dim para14 As New System.Windows.Documents.Paragraph
            para14.Inlines.Add(New Run("Wickets"))
            Dim cell14 As New System.Windows.Documents.TableCell(para14)
            cell14.FontSize = 12
            cell14.FontWeight = System.Windows.FontWeights.Bold
            row4.Cells.Add(cell14)

            Dim para15 As New System.Windows.Documents.Paragraph
            para15.Inlines.Add(New Run("$100,000"))
            Dim cell15 As New System.Windows.Documents.TableCell(para15)
            cell15.FontSize = 12
            cell15.FontWeight = System.Windows.FontWeights.Normal
            row4.Cells.Add(cell15)

            Dim para16 As New System.Windows.Documents.Paragraph
            para16.Inlines.Add(New Run("$120,000"))
            Dim cell16 As New System.Windows.Documents.TableCell(para16)
            cell16.FontSize = 12
            cell16.FontWeight = System.Windows.FontWeights.Normal
            row4.Cells.Add(cell16)

            Dim para17 As New System.Windows.Documents.Paragraph
            para17.Inlines.Add(New Run("$160,000"))
            Dim cell17 As New System.Windows.Documents.TableCell(para17)
            cell17.FontSize = 12
            cell17.FontWeight = System.Windows.FontWeights.Normal
            row4.Cells.Add(cell17)

            Dim para18 As New System.Windows.Documents.Paragraph
            para18.Inlines.Add(New Run("$200,000"))
            Dim cell18 As New System.Windows.Documents.TableCell(para18)
            cell18.FontSize = 12
            cell18.FontWeight = System.Windows.FontWeights.Normal
            row4.Cells.Add(cell18)

            Dim para19 As New System.Windows.Documents.Paragraph
            para19.Inlines.Add(New Run("$580,000"))
            Dim cell19 As New System.Windows.Documents.TableCell(para19)
            cell19.FontSize = 12
            cell19.FontWeight = System.Windows.FontWeights.Normal
            row4.Cells.Add(cell19)

            Dim row5 As New System.Windows.Documents.TableRow
            row5.Background = System.Windows.Media.Brushes.Silver
            trg1.Rows.Add(row5)

            Dim para20 As New System.Windows.Documents.Paragraph
            para20.Inlines.Add(New Run("Projected 2004 Revenue: $810,000"))
            Dim cell20 As New System.Windows.Documents.TableCell(para20)
            cell20.ColumnSpan = 6
            cell20.FontSize = 18
            cell20.FontWeight = System.Windows.FontWeights.Normal
            row5.Cells.Add(cell20)

            myFlowDocViewer.Document = New FlowDocument()
            myFlowDocViewer.Document.Blocks.Add(table1)

        End Sub

    End Class

End Namespace