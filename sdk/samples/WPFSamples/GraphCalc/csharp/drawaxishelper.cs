using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Controls;
using System.Windows.Shapes;

namespace GraphCalc
{
    public class DrawAxisHelper
    {
        public DrawAxisHelper( Canvas screen, Size screenSize )
        {
            this.screen = screen;
            this.screenSize = screenSize;
        }

        public void DrawAxes( double xmin, double xmax, double ymin, double ymax )
        {
            this.xmin = xmin;
            this.xmax = xmax;
            this.ymin = ymin;
            this.ymax = ymax;

            double canvasWidth = screenSize.Width;
            double canvasHeight = screenSize.Height;
            double graphWidth = xmax-xmin;
            double graphHeight = ymax-ymin;
            double xOffset = ( xmin >= 0 ) ? 0.0 : ( xmax <= 0 ) ? 1.0 : -xmin / graphWidth;
            double yOffset = ( ymin >= 0 ) ? 1.0 : ( ymax <= 0 ) ? 0.0 : ymax / graphHeight;
            xOffset = Math.Floor( xOffset * ( canvasWidth - 0.0 ) );
            yOffset = Math.Floor( yOffset * ( canvasHeight - 0.0 ) );

            // X axis
            DrawLine( xOffset, 0.0, xOffset, canvasHeight, false );
            // Y axis
            DrawLine( 0.0, yOffset, canvasWidth, yOffset, false );
        }

        private void DrawLine( double x1, double y1, double x2, double y2, bool dotted )
        {
            Line line = new Line();
            line.X1 = x1;
            line.Y1 = y1;
            line.X2 = x2;
            line.Y2 = y2;
            line.Stroke = Brushes.Red;
            line.StrokeThickness = 1.0;
            if ( dotted )
            {
                DoubleCollection collection = new DoubleCollection();
                collection.Add( 3 );
                collection.Add( 3 );
                line.StrokeDashArray = collection;
            }
            screen.Children.Add( line );
        }

        private double xmin, xmax, ymin, ymax;
        private Canvas screen;
        private Size screenSize;
    }
}