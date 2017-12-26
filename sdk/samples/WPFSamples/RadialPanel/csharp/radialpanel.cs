using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
 
namespace MyCustomPanels
{
    public class app : Application
    {
        Window mainWindow;
        RadialPanel radialPanel1;
        Button btn1;
        Button btn2;
        Button btn3;
        Button btn4;
        Button btn5;
        Border border1;

        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);
            CreateAndShowMainWindow();
        }

        private void CreateAndShowMainWindow()
        {
            // Create the application's main Window and Border 
            // and instantiate a RadialPanel. RadialPanel is defined
            // below

            border1 = new Border();
            border1.VerticalAlignment = VerticalAlignment.Top;
            border1.HorizontalAlignment = HorizontalAlignment.Left;
            border1.BorderThickness = new Thickness(1);
            border1.BorderBrush = Brushes.Purple;

            radialPanel1 = new RadialPanel();
            radialPanel1.Width = 500;
            radialPanel1.Height = 500;
            radialPanel1.VerticalAlignment = VerticalAlignment.Stretch;
            radialPanel1.HorizontalAlignment = HorizontalAlignment.Stretch;

            // Add the Button Elements

            btn1 = new Button();
            btn1.Background = Brushes.RoyalBlue;
            btn1.Content = "Button 1";
            btn1.BorderBrush = Brushes.Black;

            btn2 = new Button();
            btn2.Background = Brushes.Red;
            btn2.Content = "Button 2";
            btn2.BorderBrush = Brushes.Black;

            btn3 = new Button();
            btn3.Background = Brushes.Green;
            btn3.Content = "Button 3";
            btn3.BorderBrush = Brushes.Black;

            btn4 = new Button();
            btn4.Background = Brushes.Purple;
            btn4.Content = "Button 4";
            btn4.BorderBrush = Brushes.Black;

            btn5 = new Button();
            btn5.Background = Brushes.Yellow;
            btn5.Content = "Button 5";
            btn5.BorderBrush = Brushes.Black;

            // Add the button elements defined above as
            // children of the RadialPanel

            radialPanel1.Children.Add(btn1);
            radialPanel1.Children.Add(btn2);
            radialPanel1.Children.Add(btn3);
            radialPanel1.Children.Add(btn4);
            radialPanel1.Children.Add(btn5);

            // Add the RadialPanel to the Border

            border1.Child = radialPanel1;

            // Add the RadialPanel as a Child of the 
            // MainWindow and show the Window

            mainWindow = new Window();
            mainWindow.Content = border1;
            mainWindow.Title = "Custom RadialPanel Sample";
            mainWindow.Show();
        }
    }
    public class RadialPanel : Panel
    {
        // This Panel lays its children out in a circle
        // keeping the angular distance from each child
        // equal; MeasureOverride is called before ArrangeOverride.

        double _maxChildHeight, _perimeter, _radius, _adjustFactor;

        protected override Size MeasureOverride(Size availableSize)

        {
            _perimeter = 0;
            _maxChildHeight = 0;
 
            // Find the tallest child and determine the perimeter
            // based on the width of all of the children after
            // measuring all of the them and letting them size
            // to content by passing Double.PositiveInfinity as
            // the available size.
 
            foreach (UIElement uie in Children)

            {
                uie.Measure(new Size(Double.PositiveInfinity, Double.PositiveInfinity));
                _perimeter += uie.DesiredSize.Width;
                _maxChildHeight = Math.Max(_maxChildHeight, uie.DesiredSize.Height);
            }
 
            // If the marginal angle is not 0, 90 or 180
            // then the adjustFactor is needed.

            if (Children.Count > 2 && Children.Count != 4)
                _adjustFactor = 10;
 
            // Determine the radius of the circle layout and determine
            // the RadialPanel's DesiredSize.

            _radius = _perimeter / (2 * Math.PI) + _adjustFactor;
            double _squareSize = 2 * (_radius + _maxChildHeight);
            return new Size(_squareSize, _squareSize);
        }

        // Perform arranging of children based on 
        // the final size.

        protected override Size ArrangeOverride(Size finalSize)
        {
            // Necessary variables.
            double _currentOriginX = 0,
                    _currentOriginY = 0,
                    _currentAngle = 0,
                    _centerX = 0,
                    _centerY = 0,
                    _marginalAngle = 0;
 
            // During measure, an adjustFactor was added to the radius
            // to account for rotated children that might fall outside
            // of the desired size.  Now, during arrange, that extra
            // space isn't needed

            _radius -= _adjustFactor;
 
            // Find center of the circle based on arrange size.
            // DesiredSize is not used because the Panel
            // is potentially being arranged across a larger
            // area from the default alignment values.

            _centerX = finalSize.Width / 2;
            _centerY = finalSize.Height / 2;
  
            // Determine the marginal angle, the angle between
            // each child on the circle.
 
            if (Children.Count != 0)
                _marginalAngle = 360 / Children.Count;

            foreach (UIElement uie in Children)
            {
                // Find origin from which to arrange 
                // each child of the RadialPanel (its top
                // left corner.)

                _currentOriginX = _centerX - uie.DesiredSize.Width / 2;
                _currentOriginY = _centerY - _radius - uie.DesiredSize.Height;

                // Apply a rotation on each child around the center of the
                // RadialPanel.

                uie.RenderTransform = new RotateTransform(_currentAngle);
                uie.Arrange(new Rect(new Point(_currentOriginX, _currentOriginY), new Size(uie.DesiredSize.Width, uie.DesiredSize.Height)));
 
                // Increment the _currentAngle by the _marginalAngle
                // to advance the next child to the appropriate position.

                _currentAngle += _marginalAngle;
            }
 
            // In this case, the Panel is sizing to the space
            // given, so, return the finalSize which will be used
            // to set the ActualHeight & ActualWidth and for rendering.

            return finalSize;
        }
    }

    // Run the application
    internal static class EntryClass
    {
        [System.STAThread()]
        private static void Main()
        {
            app app = new app();
            app.Run();
        }
    }
 }
