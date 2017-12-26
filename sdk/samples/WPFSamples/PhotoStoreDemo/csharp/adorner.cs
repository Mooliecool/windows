#define VISUALCHILD

using System;
using System.IO;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Documents;

namespace PhotoDemo
{
#if VISUALCHILD

    public class RubberbandAdorner : Adorner
    {
        private Window1 _window;
        private RectangleGeometry _geometry;
        private System.Windows.Shapes.Path _rubberband;
        private UIElement _adornedElement;
        private Rect _selectRect;
        public Rect SelectRect { get { return _selectRect; } }
        public System.Windows.Shapes.Path Rubberband { get { return _rubberband; } }
        protected override int VisualChildrenCount { get { return 1; } }
        public Window1 Window  { set { _window = value; } }
        private System.Windows.Point _anchorPoint;
                
        public RubberbandAdorner(UIElement adornedElement)
            : base(adornedElement)
        {
            _adornedElement = adornedElement;
            _selectRect = new Rect();
            _geometry = new RectangleGeometry();
            _rubberband = new System.Windows.Shapes.Path();
            _rubberband.Data = _geometry;
            _rubberband.StrokeThickness = 2;
            _rubberband.Stroke = System.Windows.Media.Brushes.Yellow;
            _rubberband.Opacity = .6;
            _rubberband.Visibility = Visibility.Hidden;
            AddVisualChild(_rubberband);
            MouseMove += new MouseEventHandler(DrawSelection);
            MouseUp += new MouseButtonEventHandler(EndSelection);
        }

        protected override System.Windows.Size ArrangeOverride(System.Windows.Size size)
        {
            System.Windows.Size finalSize = base.ArrangeOverride(size);
            ((UIElement)GetVisualChild(0)).Arrange(new Rect(new System.Windows.Point(), finalSize));
            return finalSize;
        }

        public void StartSelection(System.Windows.Point anchorPoint)
        {
            _anchorPoint = anchorPoint;
            _selectRect.Size = new System.Windows.Size(10, 10);
            _selectRect.Location = _anchorPoint;
            _geometry.Rect = _selectRect; 
            if (Visibility.Visible != _rubberband.Visibility)
                _rubberband.Visibility = Visibility.Visible;
        }

        private void DrawSelection(object sender, MouseEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                System.Windows.Point mousePosition = e.GetPosition(_adornedElement);
                if (mousePosition.X < _anchorPoint.X)
                    _selectRect.X = mousePosition.X;
                else
                    _selectRect.X = _anchorPoint.X;
                if (mousePosition.Y < _anchorPoint.Y)
                    _selectRect.Y = mousePosition.Y;
                else
                    _selectRect.Y = _anchorPoint.Y;
                _selectRect.Width = Math.Abs(mousePosition.X - _anchorPoint.X);
                _selectRect.Height = Math.Abs(mousePosition.Y - _anchorPoint.Y);
                _geometry.Rect = _selectRect; 
                AdornerLayer layer = AdornerLayer.GetAdornerLayer(_adornedElement); 
                layer.InvalidateArrange();
            }
        }

        private void EndSelection(object sender, MouseButtonEventArgs e)
        {
            if (3 >= _selectRect.Width || 3 >= _selectRect.Height)
                _rubberband.Visibility = Visibility.Hidden;
            else
                _window.CropButton.IsEnabled = true;
            ReleaseMouseCapture();
        }
    
        protected override Visual GetVisualChild(int index)
        {
            return _rubberband;
        }
    }
#endif

#if NoVISUALCHILD


    public class RubberbandAdorner : Adorner
    {

        private UIElement _adornedElement;
        private bool _showRect;
        private Window1 _window;
        SolidColorBrush _brush;
        Pen _pen;
        private Rect _selectRect;
        public Rect SelectRect { get { return _selectRect; } set { _selectRect = value; } }
        public bool ShowRect { get { return _showRect; } set { _showRect = value; } }
        public Window1 Window { set { _window = value; } }

        public RubberbandAdorner(UIElement adornedElement)
            : base(adornedElement)
        {
            _adornedElement = adornedElement;
            _selectRect = new Rect();
            _brush = new SolidColorBrush();
            _brush.Color = Colors.Yellow;
            _brush.Opacity = .6;
            _pen = new Pen();
            _pen.Thickness = 2;
            _pen.Brush = _brush;
            _showRect = false;
            MouseMove += new MouseEventHandler(DrawSelection);
            MouseUp += new MouseButtonEventHandler(EndSelection);
        }

        public void StartSelection(Point anchorPoint)
        {
            _anchorPoint = anchorPoint;
            _selectRect.Size = new Size(0, 0);
            _selectRect.Location = _anchorPoint;
            if (!_showRect)
                _showRect = true;
        }

        private void DrawSelection(object sender, MouseEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                Point mousePosition = e.GetPosition(_adornedElement);
                if (mousePosition.X < _anchorPoint.X)
                    _selectRect.X = mousePosition.X;
                else
                    _selectRect.X = _anchorPoint.X;
                if (mousePosition.Y < _anchorPoint.Y)
                    _selectRect.Y = mousePosition.Y;
                else
                    _selectRect.Y = _anchorPoint.Y;
                _selectRect.Width = Math.Abs(mousePosition.X - _anchorPoint.X);
                _selectRect.Height = Math.Abs(mousePosition.Y - _anchorPoint.Y);
                InvalidateArrange();
                AdornerLayer layer = AdornerLayer.GetAdornerLayer(_adornedElement);
                layer.InvalidateArrange();
            }
        }

        private void EndSelection(object sender, MouseButtonEventArgs e)
        {
            if (3 >= _selectRect.Width || 3 >= _selectRect.Height)
                ShowRect = false;
            else
                _window.CropButton.IsEnabled = true;
            ReleaseMouseCapture();
        }

        protected override void OnRender(DrawingContext drawingContext)
        {
            if (_showRect)
            {
                base.OnRender(drawingContext);
                drawingContext.DrawRectangle(Brushes.Transparent, _pen, _selectRect);
            }
            else
                return;
        }
    }
#endif

}
