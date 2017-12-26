Imports System.Windows.Ink
Imports System.Windows.Input
Imports System.Windows.Input.StylusPlugIns

Public Class HighContrastInkCanvas
    Inherits InkCanvas

    ''' <summary>
    ''' Custom InkCanvas element to collect
    ''' and render high contrast ink
    ''' </summary>
    Public Sub New()
        ' set Color, Width and Height for the inner stroke
        Me.DynamicRenderer.DrawingAttributes.Color = Colors.Chocolate
        Me.DynamicRenderer.DrawingAttributes.Width = 3D
        Me.DynamicRenderer.DrawingAttributes.Height = 3D

        ' create a second DynamicRenderer
        outerDynamicRenderer = New DynamicRenderer()
        outerDynamicRenderer.DrawingAttributes = Me.DefaultDrawingAttributes.Clone()
        outerDynamicRenderer.DrawingAttributes.Color = Colors.DarkGreen
        outerDynamicRenderer.DrawingAttributes.Width *= 1.4D
        outerDynamicRenderer.DrawingAttributes.Height *= 1.4D

        ' plug the new DynamicRenderer in, so that it can receive
        ' real-time stylus notifications
        Me.StylusPlugIns.Add(outerDynamicRenderer)

        ' re-order the RootVisuals of our DynamicRendererd, so that
        ' the inner stroke gets rendered on top of the outer stroke
        Me.InkPresenter.DetachVisuals(Me.DynamicRenderer.RootVisual)
        Me.InkPresenter.AttachVisuals(outerDynamicRenderer.RootVisual, outerDynamicRenderer.DrawingAttributes)
        Me.InkPresenter.AttachVisuals(Me.DynamicRenderer.RootVisual, Me.DynamicRenderer.DrawingAttributes)
    End Sub

    ''' <summary>
    ''' Occurs when a stroke drawn by the user is added to the Strokes property 
    ''' </summary>
    Protected Overrides Sub OnStrokeCollected(ByVal e As System.Windows.Controls.InkCanvasStrokeCollectedEventArgs)
        MyBase.OnStrokeCollected(e)

        ' create a clone of the collected strokes with new
        ' DrawingAttributes to represent the outline
        Dim daOutline As DrawingAttributes
        daOutline = GetOutlineDrawingAttributes(e.Stroke.DrawingAttributes)
        Dim newStroke As New Stroke(e.Stroke.StylusPoints.Clone(), daOutline)

        ' insert the new strokes at the proper location in
        ' the stroke collection so that it will be renderer
        ' behind the original stroke
        Dim index As Integer
        index = Me.Strokes.IndexOf(e.Stroke)
        Me.Strokes.Insert(index, newStroke)
    End Sub

    ''' <summary>
    ''' Occurs when the current editing mode changes 
    ''' </summary>
    Protected Overrides Sub OnActiveEditingModeChanged(ByVal e As System.Windows.RoutedEventArgs)
        MyBase.OnActiveEditingModeChanged(e)
        If (outerDynamicRenderer Is Nothing) Then
            Return
        End If

        If Me.ActiveEditingMode = InkCanvasEditingMode.Ink Or _
           Me.ActiveEditingMode = InkCanvasEditingMode.InkAndGesture Or _
           Me.ActiveEditingMode = InkCanvasEditingMode.GestureOnly Then
            outerDynamicRenderer.Enabled = True
        Else
            ' prevent dynamic ink rendering when ActiveEditingMode
            ' is not one of the inking modes
            outerDynamicRenderer.Enabled = False
        End If
    End Sub

    ''' <summary>
    ''' Creates a new DrawingAttributes instance to represent
    ''' the outline for given DrawingAttributes
    ''' </summary>
    Private Function GetOutlineDrawingAttributes(ByVal oirginalDrawingAttributes As DrawingAttributes) As DrawingAttributes
        Dim newDrawingAttributes As DrawingAttributes
        newDrawingAttributes = oirginalDrawingAttributes.Clone()

        ' set Color, Width and Height for the outer stroke
        newDrawingAttributes.Color = Colors.DarkGreen
        newDrawingAttributes.Width *= 1.4D
        newDrawingAttributes.Height *= 1.4D

        Return newDrawingAttributes
    End Function

    ''' <summary>
    ''' DynamicRender to render the stroke outline
    ''' </summary>
    Private outerDynamicRenderer As DynamicRenderer
End Class
