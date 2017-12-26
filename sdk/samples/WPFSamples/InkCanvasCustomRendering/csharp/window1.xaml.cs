using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Input.StylusPlugIns;
using System.Windows.Media;


namespace InkCanvasCustomRenderingSample
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class Window1 : Window
    {
        public Window1()
        {
            InitializeComponent();
        }
    }

    /// <summary>
    /// Custom InkCanvas for rendering our custom ImageStrokes
    /// </summary>
    public class ImageInkCanvas : InkCanvas
    {
        protected override void OnInitialized(EventArgs e)
        {
            base.OnInitialized(e);

            // Replace standard DynamicRenderer with custom one
            DynamicRenderer dr = new ImageRenderer();
            dr.DrawingAttributes = this.DefaultDrawingAttributes;
            this.DynamicRenderer = dr;
        }

        //  When a Stroke is collected, replace it with an object of derived type, 
        // ImageStroke. ImageStroke contains pieces of an image as opposed to 
        // a traditional stroke which contains ink.
        protected override void OnStrokeCollected(InkCanvasStrokeCollectedEventArgs e)
        {
            base.OnStrokeCollected(e);

            StylusPointCollection spc = e.Stroke.StylusPoints.Clone();
            ImageStroke imgStroke = new ImageStroke(spc, e.Stroke.DrawingAttributes);

            this.Strokes.Remove(e.Stroke);
            this.Strokes.Add(imgStroke);
        }
    }

    /// <summary>
    /// Custom Stroke for painting strokes containing image information
    /// </summary>
    public class ImageStroke : Stroke
    {
        // All we need is one brush instance for use by the main UI thread.
        static ImageBrush myImageBrush = ImageRenderer.CreateImageBrush();

        public ImageStroke(StylusPointCollection points, DrawingAttributes da)
            : base(points, da)
        { }

        protected override void DrawCore(DrawingContext drawingContext, DrawingAttributes drawingAttributes)
        {
            // Get geometry for given drawingAttributes.
            Geometry geo = base.GetGeometry(drawingAttributes);

            // Paint the stroke with the imagebrush.
            drawingContext.DrawGeometry(myImageBrush, null, geo);
        }
    }

    /// <summary>
    /// Custom DynamicRenderer for rendering image information on the InkCanvas
    /// </summary>
    public class ImageRenderer : DynamicRenderer
    {
        ImageBrush renderImageBrush;

        public ImageRenderer()
        {
            // We do this here, on the app thread - the brush is frozen so it can be used from 
            // multiple threads later (OnDraw).
            renderImageBrush = CreateImageBrush();
        }

        internal static ImageBrush CreateImageBrush()
        {
            ImageBrush myimageBrush = new ImageBrush(App.myImageSource);

            myimageBrush.TileMode = TileMode.None;
            myimageBrush.Stretch = Stretch.None;
            myimageBrush.AlignmentX = AlignmentX.Left;
            myimageBrush.AlignmentY = AlignmentY.Top;

            // Map output to the whole positive x/y quadrant.
            myimageBrush.ViewportUnits = BrushMappingMode.Absolute;
            myimageBrush.Viewport = new Rect(0d, 0d, Double.PositiveInfinity, Double.PositiveInfinity);

            // Freeze it, for best performance.
            myimageBrush.Freeze();
            return myimageBrush;
        }

        protected override void OnDraw(DrawingContext drawingContext, StylusPointCollection stylusPoints, Geometry geometry, Brush fillBrush)
        {
            // Draw the image for the current packet(s).
            drawingContext.DrawGeometry(renderImageBrush, null, geometry);
        }
    }
}