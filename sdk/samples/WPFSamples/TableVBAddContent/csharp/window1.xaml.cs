using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;

namespace SDKSample
{

	public partial class Window1 : Window
	{
        public void AddRow(object sender, RoutedEventArgs e)
        {
            TableRow row = new TableRow();
            trg1.Rows.Add(row);
            Paragraph para = new Paragraph();
            para.Inlines.Add("A new Row and Cell have been Added to the Table");
            TableCell cell = new TableCell(para);
            row.Cells.Add(cell);
        }
    }
}