' Interaction logic for Window1.xaml
Imports System.Windows.Input.StylusPlugIns
Imports System.Windows.Ink


Partial Public Class Window1
    Inherits System.Windows.Window

    Public Sub New()
        InitializeComponent()
    End Sub
End Class

''' <summary>
''' Custom InkCanvas for rendering our custom ImageStrokes
''' </summary>
Public Class ImageInkCanvas
    Inherits InkCanvas

    Protected Overrides Sub OnInitialized(ByVal e As System.EventArgs)

        MyBase.OnInitialized(e)

        ' Replace standard DynamicRenderer with custom one
        Dim dr As DynamicRenderer = New ImageRenderer()
        dr.DrawingAttributes = Me.DefaultDrawingAttributes
        Me.DynamicRenderer = dr

    End Sub

    '  When a Stroke is collected, replace it with an object of derived type, 
    ' ImageStroke. ImageStroke contains pieces of an image as opposed to 
    ' a traditional stroke which contains ink.
    Protected Overrides Sub OnStrokeCollected(ByVal e As System.Windows.Controls.InkCanvasStrokeCollectedEventArgs)

        MyBase.OnStrokeCollected(e)

        Dim spc As StylusPointCollection = e.Stroke.StylusPoints.Clone()
        Dim imgStroke As ImageStroke = New ImageStroke(spc, e.Stroke.DrawingAttributes)

        Me.Strokes.Remove(e.Stroke)
        Me.Strokes.Add(imgStroke)

    End Sub
End Class

''' <summary>
''' Custom Stroke for painting strokes containing image information
''' </summary>
Public Class ImageStroke
    Inherits Stroke

    ' All we need is one brush instance for use by the main UI thread.
    Shared myImageBrush As ImageBrush = ImageRenderer.CreateImageBrush()

    Public Sub New(ByVal points As StylusPointCollection, ByVal da As DrawingAttributes)
        MyBase.New(points, da)
    End Sub

    Protected Overrides Sub DrawCore(ByVal drawingContext As System.Windows.Media.DrawingContext, ByVal drawingAttributes As System.Windows.Ink.DrawingAttributes)

        ' Get geometry for given drawingAttributes.
        Dim geo As Geometry = MyBase.GetGeometry(drawingAttributes)

        ' Paint the stroke with the imagebrush.
        drawingContext.DrawGeometry(myImageBrush, Nothing, geo)

    End Sub
End Class

''' <summary>
''' Custom DynamicRenderer for rendering image information on the InkCanvas
''' </summary>
Public Class ImageRenderer
    Inherits DynamicRenderer

    Dim renderImageBrush As ImageBrush

    Public Sub New()

        '  We do this here, on the app thread - the brush is frozen so it can be used from 
        ' multiple threads later (OnDraw).
        renderImageBrush = CreateImageBrush()

    End Sub

    Friend Shared Function CreateImageBrush() As ImageBrush

        Dim myImageBrush As ImageBrush = New ImageBrush(App.myImageSource)

        myImageBrush.TileMode = TileMode.None
        myImageBrush.Stretch = Stretch.None
        myImageBrush.AlignmentX = AlignmentX.Left
        myImageBrush.AlignmentY = AlignmentY.Top

        ' Map output to the whole positive x/y quadrant.
        myImageBrush.ViewportUnits = BrushMappingMode.Absolute
        myImageBrush.Viewport = New Rect(0D, 0D, Double.PositiveInfinity, Double.PositiveInfinity)

        ' Freeze it, for best performance.
        myImageBrush.Freeze()
        Return myImageBrush

    End Function

    Protected Overrides Sub OnDraw(ByVal drawingContext As System.Windows.Media.DrawingContext, ByVal stylusPoints As System.Windows.Input.StylusPointCollection, ByVal geometry As System.Windows.Media.Geometry, ByVal fillBrush As System.Windows.Media.Brush)

        ' Draw the image for the current packet(s).
        drawingContext.DrawGeometry(renderImageBrush, Nothing, geometry)

    End Sub
End Class



