using System;
using System.IO;
using System.Resources;
using System.Windows.Markup;
using System.Threading;
using System.Windows;
using System.Windows.Controls;

namespace SDKSample
{
    public partial class Window1 : Window
    {
        void OnStartup(object sender, RoutedEventArgs e)
        {

            Slider childrenCountSlider = (Slider)LogicalTreeHelper.FindLogicalNode(myWindow, "ChildrenCountSlider");
            childrenCountSlider.ValueChanged += new RoutedPropertyChangedEventHandler<double>(OnChildrenCountChanged);

            Slider columnCountSlider = (Slider)LogicalTreeHelper.FindLogicalNode(myWindow, "ColumnCountSlider");
            columnCountSlider.ValueChanged += new RoutedPropertyChangedEventHandler<double>(OncolumnCountChanged);

        }




        /////////////////// Event Handlers


        private void OnChildrenCountChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            int childrenCount = (int)Math.Floor(e.NewValue + 0.5);

            //  Update the children count...
            AutoIndexingGrid g = (AutoIndexingGrid)LogicalTreeHelper.FindLogicalNode(myWindow, "TargetGrid");
            while (g.Children.Count < childrenCount)
            {
                Control c = new Control();
                g.Children.Add(c);
                c.Style = (Style)c.FindResource("ImageWithBorder");
            }
            while (g.Children.Count > childrenCount)
            {
                g.Children.Remove(g.Children[g.Children.Count - 1]);
            }


            //  Update TextBlock element displaying the count...
            TextBlock t = (TextBlock)LogicalTreeHelper.FindLogicalNode(myWindow, "ChildrenCountDisplay");
            t.Text = childrenCount.ToString();
        }

        private void OncolumnCountChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            int columnCount = (int)Math.Floor(e.NewValue + 0.5);

            //  Update column count...
            AutoIndexingGrid g = (AutoIndexingGrid)LogicalTreeHelper.FindLogicalNode(myWindow, "TargetGrid");
            while (g.ColumnDefinitions.Count < columnCount)
            {
                g.ColumnDefinitions.Add(new ColumnDefinition());
            }
            while (g.ColumnDefinitions.Count > columnCount)
            {
                g.ColumnDefinitions.Remove(g.ColumnDefinitions[g.ColumnDefinitions.Count - 1]);
            }

            //  Update TextBlock element displaying the count...
            TextBlock t = (TextBlock)LogicalTreeHelper.FindLogicalNode(myWindow, "ColumnCountDisplay");
            t.Text = columnCount.ToString();
        }


    }


     /// <summary>
    /// AutoIndexingGrid - sample implementation of auto indexing, row primary grid
    /// </summary>
    
    public class AutoIndexingGrid : Grid
    {
        protected override Size MeasureOverride(Size constraint)
        {
            if (    _updateChildenIndices
                ||  _columnCount != base.ColumnDefinitions.Count    )
            {
                _updateChildenIndices = false;
                _columnCount = base.ColumnDefinitions.Count;

                //  Update the number of rows
                int newRowCount = ((base.Children.Count - 1) / _columnCount + 1);
                while (base.RowDefinitions.Count < newRowCount)
                {
                    base.RowDefinitions.Add(new RowDefinition());
                }
                if (base.RowDefinitions.Count > newRowCount)
                {
                    base.RowDefinitions.RemoveRange(newRowCount, base.RowDefinitions.Count - newRowCount);
                }

                //  Update children indices
                for (int i = 0, childrenCount = base.Children.Count; i < childrenCount; ++i)
                {
                    UIElement child = base.Children[i];
                    Grid.SetColumn(child, i % _columnCount);
                    Grid.SetRow(child, i / _columnCount);
                }
            }

            return (base.MeasureOverride(constraint));
        }

        protected override void OnVisualChildrenChanged(DependencyObject visualAdded, DependencyObject visualRemoved)
        {
            //  Remember that children collection has changed 
            _updateChildenIndices = true;

            base.OnVisualChildrenChanged(visualAdded, visualRemoved);
        }

        private bool _updateChildenIndices = true;  // A value of "true" forces children to be re-indexed at the next oportunity
        private int _columnCount;
    }
}


