using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace GridSplitter_Example
{
  public partial class Window1 : Window
  {
  public Window1()
  {
    InitializeComponent();
  }

    private void KeyboardIncrementChanged(object sender, RoutedEventArgs e)
    {
      if ((Boolean)KeyboardIncrementAuto.IsChecked)
        myGridSplitter.KeyboardIncrement = 1;
      else if ((Boolean)KeyboardIncrementCols.IsChecked)
        myGridSplitter.KeyboardIncrement = 20;
      else if ((Boolean)KeyboardIncrementRows.IsChecked)
        myGridSplitter.KeyboardIncrement = 50;
      else if ((Boolean)KeyboardIncrementBoth.IsChecked)
        myGridSplitter.KeyboardIncrement = 100;
    }
    
    private void DragIncrementChanged(object sender, RoutedEventArgs e)
    {
      if ((Boolean)DragIncrementAuto.IsChecked)
        myGridSplitter.DragIncrement = 1;
      else if ((Boolean)DragIncrementCols.IsChecked)
        myGridSplitter.DragIncrement = 20;
      else if ((Boolean)DragIncrementRows.IsChecked)
        myGridSplitter.DragIncrement = 50;
    }
  
    
    private void ResizeBehaviorChanged(object sender, RoutedEventArgs e)
    {
      if ((Boolean)BehaviorBasedOnAlignment.IsChecked)
        myGridSplitter.ResizeBehavior = GridResizeBehavior.BasedOnAlignment;
      else if ((Boolean)BehaviorCurrentAndNext.IsChecked)
        myGridSplitter.ResizeBehavior = GridResizeBehavior.CurrentAndNext;
      else if ((Boolean)BehaviorPreviousAndCurrent.IsChecked)
        myGridSplitter.ResizeBehavior = GridResizeBehavior.PreviousAndCurrent;
      else if ((Boolean)BehaviorPreviousAndNext.IsChecked)
        myGridSplitter.ResizeBehavior = GridResizeBehavior.PreviousAndNext;
    }

    private void ResizeDirectionChanged(object sender, RoutedEventArgs e)
    {
      if ((Boolean)ResizeDirectionAuto.IsChecked)
        myGridSplitter.ResizeDirection = GridResizeDirection.Auto;
      else if ((Boolean)ResizeDirectionCols.IsChecked)
        myGridSplitter.ResizeDirection = GridResizeDirection.Columns;
      else if ((Boolean)ResizeDirectionRows.IsChecked)
        myGridSplitter.ResizeDirection = GridResizeDirection.Rows;
    }

    private void ShowsPreviewChanged(object sender, RoutedEventArgs e)
    {
      if ((Boolean)ShowsPreviewFalse.IsChecked)
        myGridSplitter.ShowsPreview = false;
      else if ((Boolean)ShowsPreviewTrue.IsChecked)
        myGridSplitter.ShowsPreview = true;
    }

    private void VerticalAlignmentChanged(object sender, RoutedEventArgs e)
    {
      if ((Boolean)VerticalAlignmentTop.IsChecked)
        myGridSplitter.VerticalAlignment = VerticalAlignment.Top;
      else if ((Boolean)VerticalAlignmentBottom.IsChecked)
        myGridSplitter.VerticalAlignment = VerticalAlignment.Bottom;
      else if ((Boolean)VerticalAlignmentCenter.IsChecked)
        myGridSplitter.VerticalAlignment = VerticalAlignment.Center;
      else if ((Boolean)(Boolean)VerticalAlignmentStretch.IsChecked)
        myGridSplitter.VerticalAlignment = VerticalAlignment.Stretch;
    }

    private void HorizontalAlignmentChanged(object sender, RoutedEventArgs e)
    {
      if ((Boolean)HorizontalAlignmentLeft.IsChecked)
        myGridSplitter.HorizontalAlignment = HorizontalAlignment.Left;
      else if ((Boolean)HorizontalAlignmentRight.IsChecked)
        myGridSplitter.HorizontalAlignment = HorizontalAlignment.Right;
      else if ((Boolean)HorizontalAlignmentCenter.IsChecked)
        myGridSplitter.HorizontalAlignment = HorizontalAlignment.Center;
      else if ((Boolean)HorizontalAlignmentStretch.IsChecked)
        myGridSplitter.HorizontalAlignment = HorizontalAlignment.Stretch;
    }
  }
}
