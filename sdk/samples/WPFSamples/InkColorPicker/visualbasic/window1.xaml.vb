Imports System.Windows.Ink

''' <summary>
''' Interaction logic for Window1.xaml
''' </summary>

Partial Public Class Window1
    Inherits System.Windows.Window

    Dim _colorPicker As ColorPicker
    Dim _markerDA, _highlightDA As DrawingAttributes

    ''' <summary>
    ''' Initialization.
    ''' </summary>
    Public Sub New()
        InitializeComponent()

        inkcanvas.PreferredPasteFormats = New InkCanvasClipboardFormat() {InkCanvasClipboardFormat.InkSerializedFormat}
        _markerDA = New DrawingAttributes()
        _markerDA.Color = Color.FromRgb(200, 0, 200)
        _markerDA.StylusTip = StylusTip.Ellipse
        _markerDA.Width = 3
        _markerDA.Height = 3

        inkcanvas.DefaultDrawingAttributes = _markerDA

        _highlightDA = New DrawingAttributes()
        _highlightDA.IsHighlighter = True
        _highlightDA.Color = Color.FromRgb(255, 255, 0)
        _highlightDA.Width = 10
        _highlightDA.Height = 25
        _highlightDA.StylusTip = StylusTip.Rectangle

        _colorPicker = New ColorPicker()
        _colorPicker.SelectedDrawingAttributes = _markerDA
        color_panel.Children.Add(_colorPicker)

    End Sub

#Region "Event Handlers"

    ''' <summary>
    ''' Switch to inking mode.
    ''' </summary>
    Private Sub Ink_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

        _colorPicker.SelectedDrawingAttributes = _markerDA
        inkcanvas.DefaultDrawingAttributes = _markerDA

        inkcanvas.EditingMode = InkCanvasEditingMode.Ink

    End Sub

    ''' <summary>
    ''' Switch to highlighting mode.
    ''' </summary>
    Private Sub Highlight_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

        _colorPicker.SelectedDrawingAttributes = _highlightDA
        inkcanvas.DefaultDrawingAttributes = _highlightDA

        inkcanvas.EditingMode = InkCanvasEditingMode.Ink

    End Sub

    ''' <summary>
    ''' Switch to erase mode.
    ''' </summary>
    Private Sub Erase_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

        inkcanvas.EditingMode = InkCanvasEditingMode.EraseByStroke

    End Sub


    ''' <summary>
    ''' Clear all strokes from the inkcanvas.
    ''' </summary>
    Private Sub Clear_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

        inkcanvas.Strokes.Clear()

    End Sub

#End Region

End Class
