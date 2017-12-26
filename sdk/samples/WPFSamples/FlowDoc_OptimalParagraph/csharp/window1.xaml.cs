using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Shapes;


namespace SDKSample
{
    public partial class Window1 : Window
    {

        void ToggleHyphenation(Object sender, RoutedEventArgs args)
        {
            flowDoc.IsHyphenationEnabled = ((CheckBox)sender).IsChecked.Value;
        }

        void ToggleOptimalParagraph(Object sender, RoutedEventArgs args)
        {
            flowDoc.IsOptimalParagraphEnabled = ((CheckBox)sender).IsChecked.Value;            
        }

        void ToggleColumnFlex(Object sender, RoutedEventArgs args)
        {
            flowDoc.IsColumnWidthFlexible = ((CheckBox)sender).IsChecked.Value;
        }

        void ChangeColumnWidth(Object sender, RoutedEventArgs args)
        {
            if (myGrid.Children.Contains(flowReader))
            {
                if (columnWidthSlider.Value == 0) 
                {
                    flowDoc.ColumnWidth = 100;
                }
                else if (columnWidthSlider.Value == 1)
                {
                    flowDoc.ColumnWidth = 200;
                }
                else if (columnWidthSlider.Value == 2)
                {
                    flowDoc.ColumnWidth = 300;
                }
                else if (columnWidthSlider.Value == 3)
                { 
                    flowDoc.ColumnWidth = 400; 
                }
                else if (columnWidthSlider.Value == 4)
                { 
                    flowDoc.ColumnWidth = 500; 
                }
            }
            else
            {
                return;
            }
        }
        void ChangeColumnGap(Object sender, RoutedEventArgs args)
        {
            if (myGrid.Children.Contains(flowReader))
            {
                if (columnGapSlider.Value == 0)
                {
                    flowDoc.ColumnGap = 5;
                }
                else if (columnGapSlider.Value == 1)
                {
                    flowDoc.ColumnGap = 10;
                }
                else if (columnGapSlider.Value == 2)
                {
                    flowDoc.ColumnGap = 15;
                }
                else if (columnGapSlider.Value == 3)
                {
                    flowDoc.ColumnGap = 20;
                }
                else if (columnGapSlider.Value == 4)
                {
                    flowDoc.ColumnGap = 25;
                }
            }
            else
            {
                return;
            }
        }
    }
}