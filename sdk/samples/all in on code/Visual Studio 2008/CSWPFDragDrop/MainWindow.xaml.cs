/****************************** Module Header ******************************\
*  Module Name:  <MainWindow.xaml.cs>
*  Project:	<CSWPFDragDrop>
* Copyright (c) Microsoft Corporation.
* 
*  The MainWindow.xaml.cs file contains the code logic that implements the drag & drop
*  operation in a TabControl and a Grid.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

namespace CSWPFDragDrop
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        # region constructor

        public MainWindow()
        {
            InitializeComponent();
        }

        # endregion

        #region private fields used by dragging items in the TabControl

        bool isDragStarted = false;

        # endregion              

        # region private fields used by dragging elements in the Grid
        
        List<double> rowsheight = new List<double>();
        List<double> columnswidth = new List<double>();
        Rect[,] rects = null;
        bool IsMouseDown = false;
        bool IsDraggingStarted = false;
        DependencyObject ClickedElement = null;

        # endregion

        # region Initialization

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {           
            CalculateRectsRepresentingGridCells();           
        }

        # endregion

        # region logic to implement drag&drop in the TabControl           

        void tabitem_MouseMove(object sender, MouseEventArgs e)
        {
            if (isDragStarted)
                return;
                      
            //If the Mouse left button is pressed and the dragging hasn't started,
            // begin a drag&drop operation.            
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                // Set the isDragStarted variable to true to indicate a new drag&drop operation is 
                // started.      
                isDragStarted = true;
                TabItem item = sender as TabItem;                            
                DragDrop.DoDragDrop(item, item, DragDropEffects.Move);
            }
        }
             
        void tabitem_QueryContinueDrag(object sender, QueryContinueDragEventArgs e)
        {  
            // If the Mouse left button is released, cancel the drag&drop operation.        
            if (Mouse.LeftButton == MouseButtonState.Released)
            {
                e.Action = DragAction.Cancel;
                isDragStarted = false;
            }
        }

        void tabcontrol_DragEnter(object sender, DragEventArgs e)
        {           
            // If the dragged item is a TabItem, set the effect to move.         
            if (e.Data.GetDataPresent(typeof(TabItem)))
            {
                e.Effects = DragDropEffects.Move;
            }
        }

        void tabcontrol_Drop(object sender, DragEventArgs e)
        {
            TabControl tabcontrol = sender as TabControl;          
            TabItem draggeditem = e.Data.GetData(typeof(TabItem)) as TabItem;
          
            // Get the dropped point relative to the TabControl.
            Point droppedPoint = e.GetPosition(tabcontrol);
                      
            // Try to find which TabItem contains the dropped point.
            GeneralTransform transform;
            int index = -1;
            for (int i = 0; i < tabcontrol.Items.Count; i++)
            {
                TabItem item = tabcontrol.Items[i] as TabItem;
                transform = item.TransformToVisual(tabcontrol);
                Rect rect = transform.TransformBounds(new Rect() { X = 0, Y = 0, 
                    Width = item.ActualWidth, Height = item.ActualHeight });
                if (rect.Contains(droppedPoint))
                {
                    if (!item.Equals(draggeditem))
                    {
                        index = i;
                    }
                    break;
                }
            }
          
            // If the TabItem that contains the dropped point is found, re-arrange the dragged 
            // TabItem to the index of the dropped TabItem.
            if (index != -1)
            {
                tabcontrol.Items.Remove(draggeditem);
                tabcontrol.Items.Insert(index, draggeditem);
            }
            // Set the isDragStarted variable to false to indicate this drag&drop operation is 
            // completed.
            isDragStarted = false;
        }

       
        # endregion

        # region logic to implement drag&drop in the Grid
               
        /// <summary>
        /// This CalculateRectsRepresentingGridCells method calculate rects representing 
        /// cells in the Grid.
        /// </summary>
        private void CalculateRectsRepresentingGridCells()
        {
            rowsheight.Clear();
            columnswidth.Clear();
            
            double rowstarno = 0;
            double absoluteRowHeightSum = 0;

            // Get the sum of all star on the rows and the sum of all absolute row height.
            for (int i = 0; i < grid.RowDefinitions.Count; i++)
            {
                if (grid.RowDefinitions[i].Height.IsStar)
                {
                    rowstarno += grid.RowDefinitions[i].Height.Value;
                }
                if (grid.RowDefinitions[i].Height.IsAbsolute)
                {
                    absoluteRowHeightSum += grid.RowDefinitions[i].Height.Value;
                }
            }
          
            // Set the rowsheight array.           
            for (int i = 0; i < grid.RowDefinitions.Count; i++)
            {
                GridLength length = grid.RowDefinitions[i].Height;                
                if (length.IsAbsolute)
                {
                    rowsheight.Add(length.Value);
                }
                else if (length.IsStar)
                {
                    double rowheight = (grid.RowDefinitions[i].Height.Value / rowstarno) * (grid.ActualHeight - absoluteRowHeightSum);
                    rowsheight.Add(rowheight);
                }
            }

            double columnstarno = 0;
            double absoluteColumnWidthSum = 0;

            // Get the sum of all star on the columns and the sum of all absolute column width.
            for (int i = 0; i < grid.ColumnDefinitions.Count; i++)
            {
                if (grid.ColumnDefinitions[i].Width.IsStar)
                {
                    columnstarno += grid.ColumnDefinitions[i].Width.Value;
                }
                if (grid.ColumnDefinitions[i].Width.IsAbsolute)
                {
                    absoluteColumnWidthSum += grid.ColumnDefinitions[i].Width.Value;
                }
            }
          
            // Set the columnswidth array.
            for (int i = 0; i < grid.ColumnDefinitions.Count; i++)
            {
                GridLength length = grid.ColumnDefinitions[i].Width;
                if (length.IsAbsolute)
                {
                    columnswidth.Add(length.Value);
                }
                else if (length.IsStar)
                {
                    double columnheight = (grid.ColumnDefinitions[i].Width.Value / columnstarno) * (grid.ActualWidth - absoluteColumnWidthSum);
                    columnswidth.Add(columnheight);
                }
            }
            
            // Set the rect array according to the rowsheight and columnwidth arrays.            
            rects = new Rect[rowsheight.Count, columnswidth.Count];
            double yvalue = 0;
            for (int i = 0; i < rowsheight.Count; i++)
            {
                for (int j = 0; j < columnswidth.Count; j++)
                {
                    rects[i, j] = new Rect() { Y = yvalue, Height = rowsheight[i] };
                }
                yvalue += rowsheight[i];
            }
            double xvalue = 0;
            for (int j = 0; j < columnswidth.Count; j++)
            {
                for (int i = 0; i < rowsheight.Count; i++)
                {
                    rects[i, j].X = xvalue;
                    rects[i, j].Width = columnswidth[j];
                }
                xvalue += columnswidth[j];
            }
        }
          
        private void Grid_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {            
            // If a child element is clicked within the Grid, set the ClickedElement and IsMouseDown
            // variable.            
            if (!e.Source.Equals(sender))
            {
                ClickedElement = e.Source as DependencyObject;
                IsMouseDown = true;
            }
        }

        private void Grid_PreviewMouseMove(object sender, MouseEventArgs e)
        {        
            // If a drag&drop operation hasn't started and the IsMouseDown variable is true, begin a 
            // new drag&drop operation.
            if (!IsDraggingStarted && IsMouseDown)
            {                
                IsDraggingStarted = true;
                DragDrop.DoDragDrop(sender as DependencyObject, ClickedElement, DragDropEffects.Move);              
            }
        }
        
        private void grid_Drop(object sender, DragEventArgs e)
        {          
            // Get the dropped point relative to the Grid.         
            Point droppedpoint = e.GetPosition(sender as IInputElement);         

            // Try to find the rect that contains the dropped point.            
            bool cellfound = false;
            for (int i = 0; i < rects.GetLength(0); i++)
            {
                for (int j = 0; j < rects.GetLength(1); j++)
                {                    
                    // If the rect that contains the dropped point is found, change the Grid.Row and 
                    // Grid.Column attached property value on the dragged element based on the 
                    // indexes of the rect.                    
                    if (rects[i, j].Contains(droppedpoint))
                    {
                        ClickedElement.SetValue(Grid.RowProperty, i);
                        ClickedElement.SetValue(Grid.ColumnProperty, j);
                        cellfound = true;
                        break;
                    }
                }                
                // If the rect is found, don't need to loop any more.                
                if (cellfound)
                {
                    break;
                }
            }
          
            // Set the IsDraggingStarted variable to false to indicate this drag&drop operation is 
            // completed.            
            IsDraggingStarted = false;            
            IsMouseDown = false;
        }
        
        // If the Grid is resized, calculate the rects representing the cells in the Grid again.  
        private void grid_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            CalculateRectsRepresentingGridCells();
        }

        # endregion
    }
}
