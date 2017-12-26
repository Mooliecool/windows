// This sample shows how to drag and drop objects on the screen. 
// To visually indicate that the object is being dragged, a simple adorner 
// is applied to an object as you drag it.
namespace SDKSample
{
    #region Namespaces.
    using System;
    using System.Windows;
    using System.Windows.Controls;
    using System.Windows.Documents;
    using System.Windows.Media;
    using System.Windows.Threading;
    using System.Windows.Shapes;
    using System.Windows.Media.Animation;

    #endregion Namespaces.

    public partial class AdornerDragDropExample : Page
    {


        public void OnPageLoad(Object sender, RoutedEventArgs e)
        {
            MyCanvas = new Canvas();

            Rectangle rect1 = new Rectangle();
            rect1.Height = rect1.Width = 32;
            rect1.Fill = Brushes.Blue;

            Canvas.SetTop(rect1, 8);
            Canvas.SetLeft(rect1, 8);

            
            TextBox tb = new TextBox();
            tb.Text = "This is a TextBox. Drag and drop me";
            Canvas.SetTop(tb, 100);
            Canvas.SetLeft(tb, 100);
            
            MyCanvas.Children.Add(rect1);
            MyCanvas.Children.Add(tb);

            MyCanvas.PreviewMouseLeftButtonDown += new System.Windows.Input.MouseButtonEventHandler(MyCanvas_PreviewMouseLeftButtonDown);
            MyCanvas.PreviewMouseMove += new System.Windows.Input.MouseEventHandler(MyCanvas_PreviewMouseMove);
            MyCanvas.PreviewMouseLeftButtonUp += new System.Windows.Input.MouseButtonEventHandler(MyCanvas_PreviewMouseLeftButtonUp);
            this.PreviewKeyDown += new System.Windows.Input.KeyEventHandler(window1_PreviewKeyDown);

            myStackPanel.Children.Add(MyCanvas);
            
        }


        void window1_PreviewKeyDown(object sender, System.Windows.Input.KeyEventArgs e)
        {
            if (e.Key == System.Windows.Input.Key.Escape && _isDragging)
            {
                DragFinished(true);
            }
        }

        void MyCanvas_PreviewMouseLeftButtonUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            if (_isDown)
            {
                DragFinished(false);
                e.Handled = true;
            }
        }

        private void DragFinished(bool cancelled)
        {
            System.Windows.Input.Mouse.Capture(null);
            if (_isDragging)
            {
                AdornerLayer.GetAdornerLayer(_overlayElement.AdornedElement).Remove(_overlayElement);

                if (cancelled == false)
                {
                    Canvas.SetTop(_originalElement, _originalTop + _overlayElement.TopOffset);
                    Canvas.SetLeft(_originalElement, _originalLeft + _overlayElement.LeftOffset);
                }
                _overlayElement = null;

            }
            _isDragging = false;
            _isDown = false;
        }

        void MyCanvas_PreviewMouseMove(object sender, System.Windows.Input.MouseEventArgs e)
        {
            if (_isDown)
            {
                if ((_isDragging == false) && ((Math.Abs(e.GetPosition(MyCanvas).X - _startPoint.X) > SystemParameters.MinimumHorizontalDragDistance) ||
                    (Math.Abs(e.GetPosition(MyCanvas).Y - _startPoint.Y) > SystemParameters.MinimumVerticalDragDistance)))
                {
                    DragStarted();
                }
                if (_isDragging)
                {
                    DragMoved();
                }
            }
        }

        private void DragStarted()
        {
            _isDragging = true;
            _originalLeft = Canvas.GetLeft(_originalElement);
            _originalTop = Canvas.GetTop(_originalElement);

            _overlayElement = new SimpleCircleAdorner(_originalElement);
            AdornerLayer layer = AdornerLayer.GetAdornerLayer(_originalElement);
            layer.Add(_overlayElement);

        }

        private void DragMoved()
        {
            Point CurrentPosition = System.Windows.Input.Mouse.GetPosition(MyCanvas);

            _overlayElement.LeftOffset = CurrentPosition.X - _startPoint.X;
            _overlayElement.TopOffset = CurrentPosition.Y - _startPoint.Y;

        }

        void MyCanvas_PreviewMouseLeftButtonDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            if (e.Source == MyCanvas)
            {
            }
            else
            {
                _isDown = true;
                _startPoint = e.GetPosition(MyCanvas);
                _originalElement = e.Source as UIElement;
                MyCanvas.CaptureMouse();
                e.Handled = true;
            }
        }

        private Point _startPoint;
        private double _originalLeft;
        private double _originalTop;
        private bool _isDown;
        private bool _isDragging;
        private UIElement _originalElement;
        private SimpleCircleAdorner _overlayElement;

        Canvas MyCanvas ;

    }

    // Adorners must subclass the abstract base class Adorner.
    public class SimpleCircleAdorner : Adorner
    {
        // Be sure to call the base class constructor.
        public SimpleCircleAdorner(UIElement adornedElement)
            : base(adornedElement)
        {
            VisualBrush _brush = new VisualBrush(adornedElement);

            _child = new Rectangle();
            _child.Width = adornedElement.RenderSize.Width;
            _child.Height = adornedElement.RenderSize.Height;
           

            DoubleAnimation animation = new DoubleAnimation(0.3, 1, new Duration(TimeSpan.FromSeconds(1)));
            animation.AutoReverse = true;
            animation.RepeatBehavior =  System.Windows.Media.Animation.RepeatBehavior.Forever;
            _brush.BeginAnimation(System.Windows.Media.Brush.OpacityProperty, animation);

            _child.Fill = _brush;
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
            drawingContext.DrawRectangle(renderBrush, renderPen, adornedElementRect);
            drawingContext.DrawEllipse(renderBrush, renderPen, adornedElementRect.TopLeft, renderRadius, renderRadius);
            drawingContext.DrawEllipse(renderBrush, renderPen, adornedElementRect.TopRight, renderRadius, renderRadius);
            drawingContext.DrawEllipse(renderBrush, renderPen, adornedElementRect.BottomLeft, renderRadius, renderRadius);
            drawingContext.DrawEllipse(renderBrush, renderPen, adornedElementRect.BottomRight, renderRadius, renderRadius);
        }

        protected override Size MeasureOverride(Size constraint)
        {
            _child.Measure(constraint);
            return _child.DesiredSize;
        }

        protected override Size ArrangeOverride(Size finalSize)
        {
            _child.Arrange(new Rect(finalSize));
            return finalSize;
        }

        protected override Visual GetVisualChild(int index)
        {
            return _child;
        }

        protected override int VisualChildrenCount
        {
            get
            {
                return 1;
            }
        }

        public double LeftOffset
        {
            get
            {
                return _leftOffset;
            }
            set
            {
                _leftOffset = value;
                UpdatePosition();
            }
        }

        public double TopOffset
        {
            get
            {
                return _topOffset;
            }
            set
            {
                _topOffset = value;
                UpdatePosition();

            }
        }

        private void UpdatePosition()
        {
            AdornerLayer adornerLayer = this.Parent as AdornerLayer;
            if (adornerLayer != null)
            {
                adornerLayer.Update(AdornedElement);
            }
        }

        public override GeneralTransform GetDesiredTransform(GeneralTransform transform)
        {
            GeneralTransformGroup result = new GeneralTransformGroup();
            result.Children.Add(base.GetDesiredTransform(transform));
            result.Children.Add(new TranslateTransform(_leftOffset,_topOffset));
            return result;
        }

        private Rectangle _child = null;
        private double _leftOffset=0;
        private double _topOffset = 0;
    }
}



