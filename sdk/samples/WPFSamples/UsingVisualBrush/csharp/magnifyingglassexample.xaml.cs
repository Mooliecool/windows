using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Media;
using System.Windows.Input;

namespace Microsoft.Samples.Graphics.UsingVisualBrush
{


    public partial class MagnifyingGlassExample : Page
    {

        private static readonly double distanceFromMouse = 5;
        
        public MagnifyingGlassExample()
        {
        }
        
        
        private void updateMagnifyingGlass(object sender, MouseEventArgs args)
        {
            Mouse.SetCursor(Cursors.Cross);
            
            // Get the current position of the mouse pointer.
            Point currentMousePosition = args.GetPosition(this);
            
            // Determine whether the magnifying glass should be shown to the
            // the left or right of the mouse pointer.
            if (this.ActualWidth - currentMousePosition.X > magnifyingGlassEllipse.Width + distanceFromMouse)
            {
                Canvas.SetLeft(magnifyingGlassEllipse, currentMousePosition.X + distanceFromMouse);
            }
            else
            {
                Canvas.SetLeft(magnifyingGlassEllipse, currentMousePosition.X - distanceFromMouse - magnifyingGlassEllipse.Width);
            }
            
            // Determine whether the magnifying glass should be shown 
            // above or below the mouse pointer.
            if (this.ActualHeight - currentMousePosition.Y > magnifyingGlassEllipse.Height + distanceFromMouse)
            {
                Canvas.SetTop(magnifyingGlassEllipse, currentMousePosition.Y + distanceFromMouse);
            }
            else
            {
                Canvas.SetTop(magnifyingGlassEllipse, currentMousePosition.Y - distanceFromMouse - magnifyingGlassEllipse.Height);
            }
            
            
            // Update the visual brush's Viewbox to magnify a 20 by 20 rectangle,
            // centered on the current mouse position.
            myVisualBrush.Viewbox = 
                new Rect(currentMousePosition.X - 10, currentMousePosition.Y  - 10, 20, 20);
            
        }
        
    }
}