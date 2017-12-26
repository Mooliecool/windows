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
    AdornerLayer alSingle, alChildren;
    
    public Window1()
    {
      InitializeComponent();
    }

    private void WindowLoaded(object sender, RoutedEventArgs e) 
    {
      alSingle = AdornerLayer.GetAdornerLayer(loneTextBox);
      alChildren = AdornerLayer.GetAdornerLayer(elementsGrid);
    }

    private void ClickAdornMe(object sender, RoutedEventArgs e)
    {
      alSingle.Add(new SimpleCircleAdorner(loneTextBox));

      btUnAdornMe.IsEnabled = !(btAdornMe.IsEnabled = btUnAdornMe.IsEnabled); 
    }

    private void ClickUnAdornMe(object sender, RoutedEventArgs e)
    {
      // GetAdorners returns an array of adorners in the adorner layer.
      // In this case, since we know we've only added a single adorner to the
      // layer, call the Remove method on the first (and only) adorner in
      // the array returned by GetAdorners.
      alSingle.Remove((alSingle.GetAdorners(loneTextBox))[0]);

      btUnAdornMe.IsEnabled = !(btAdornMe.IsEnabled = btUnAdornMe.IsEnabled);
    }
    
    private void ClickAdornUs(object sender, RoutedEventArgs e) 
    {
      foreach (UIElement toAdorn in elementsGrid.Children)
        alChildren.Add(new SimpleCircleAdorner(toAdorn));

      btUnAdornUs.IsEnabled = !(btAdornUs.IsEnabled = btUnAdornUs.IsEnabled); 
    }

    private void ClickUnAdornUs(object sender, RoutedEventArgs e) 
    {
      foreach (UIElement toUnAdorn in elementsGrid.Children)
        alChildren.Remove((alChildren.GetAdorners(toUnAdorn))[0]);

      btUnAdornUs.IsEnabled = !(btAdornUs.IsEnabled = btUnAdornUs.IsEnabled); 
    }
  }

  // Adorners must subclass the abstract base class Adorner.
  public class SimpleCircleAdorner : Adorner
  {
    // Be sure to call the base class constructor.
    public SimpleCircleAdorner(UIElement adornedElement)
      : base(adornedElement)
    {
      // Any constructor implementation...
    }

    // A common way to implement an adorner's rendering behavior is to override the OnRender
    // method, which is called by the layout subsystem as part of a rendering pass.
    protected override void OnRender(DrawingContext drawingContext)
    {
      // Get a rectangle that represents the desired size of the rendered element
      // after the rendering pass.  This will be used to draw at the corners of the 
      // adorned element.
      Rect adornedElementRect = new Rect(this.AdornedElement.DesiredSize);

      // Some arbitrary drawing implements.
      SolidColorBrush renderBrush = new SolidColorBrush(Colors.Green);
      renderBrush.Opacity = 0.2;
      Pen renderPen = new Pen(new SolidColorBrush(Colors.Navy), 1.5);
      double renderRadius = 5.0;

      // Just draw a circle at each corner.
      drawingContext.DrawEllipse(renderBrush, renderPen, adornedElementRect.TopLeft, renderRadius, renderRadius);
      drawingContext.DrawEllipse(renderBrush, renderPen, adornedElementRect.TopRight, renderRadius, renderRadius);
      drawingContext.DrawEllipse(renderBrush, renderPen, adornedElementRect.BottomLeft, renderRadius, renderRadius);
      drawingContext.DrawEllipse(renderBrush, renderPen, adornedElementRect.BottomRight, renderRadius, renderRadius);
    }
  }
}