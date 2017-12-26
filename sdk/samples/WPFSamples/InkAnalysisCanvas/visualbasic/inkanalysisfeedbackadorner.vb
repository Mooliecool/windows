Imports System
Imports System.Globalization
Imports System.Windows
Imports System.Windows.Ink
Imports System.Windows.Controls

''' <summary>
''' InkAnalysisFeedbackAdorner is an adorner that gets added to the InkAnalysisCanvas's
''' visual tree to render feedback about the parsing and recognition results from 
''' the InkAnalyzer
''' </summary>
Public Class InkAnalysisFeedbackAdorner
    Inherits Adorner

    Private _inkAnalyzer As InkAnalyzer

    ''' <summary>
    ''' Constructor
    ''' </summary>
    Public Sub New(ByVal adornedElement As UIElement, ByVal inkAnalyzer As InkAnalyzer)

        MyBase.New(adornedElement)
        _inkAnalyzer = inkAnalyzer

    End Sub

    ''' <summary>
    ''' OnRender
    ''' </summary>
    Protected Overrides Sub OnRender(ByVal drawingContext As System.Windows.Media.DrawingContext)

        drawingContext.PushOpacity(0.45D)
        ' recurse through the tree of results
        DrawFeedback(drawingContext, _inkAnalyzer.RootNode)

    End Sub

    ''' <summary>
    ''' InkAnalysis results form a tree, this method is called recursively
    ''' to render each node in the tree.
    ''' </summary>
    Private Sub DrawFeedback(ByVal drawingContext As DrawingContext, ByVal contextNode As ContextNode)

        ' see what type of ContextNode this is

        Dim nodeBounds As Rect = contextNode.Strokes.GetBounds()

        If TypeOf contextNode Is InkWordNode Then

            Dim inkWordNode As InkWordNode = CType(contextNode, InkWordNode)
            drawingContext.DrawRoundedRectangle(Nothing, New Pen(Brushes.Blue, 1D), nodeBounds, 1D, 1D)
            drawingContext.DrawText(New FormattedText(inkWordNode.GetRecognizedString(), _
                                                        CultureInfo.CurrentCulture, _
                                                        System.Windows.FlowDirection.LeftToRight, _
                                                        New Typeface("Verdana"), _
                                                        9D, _
                                                        Brushes.Black), _
                                    nodeBounds.BottomLeft)
            GoTo recurse

        End If

        If TypeOf contextNode Is InkDrawingNode Then

            Dim inkDrawingNode As InkDrawingNode = CType(contextNode, InkDrawingNode)
            drawingContext.DrawRoundedRectangle(Nothing, New Pen(Brushes.Purple, 1D), nodeBounds, 1D, 1D)
            drawingContext.DrawText(New FormattedText("Drawing: " + inkDrawingNode.GetShapeName(), _
                                                        CultureInfo.CurrentCulture, _
                                                        System.Windows.FlowDirection.LeftToRight, _
                                                        New Typeface("Verdana"), _
                                                        9D, _
                                                        Brushes.Black), _
                                    nodeBounds.BottomLeft)
            GoTo recurse

        End If

        If TypeOf contextNode Is InkBulletNode Then

            Dim inkBulletNode As InkBulletNode = CType(contextNode, InkBulletNode)
            drawingContext.DrawRoundedRectangle(Nothing, New Pen(Brushes.Green, 1D), nodeBounds, 1D, 1D)
            drawingContext.DrawText(New FormattedText(inkBulletNode.GetRecognizedString(), _
                                        CultureInfo.CurrentCulture, _
                                        System.Windows.FlowDirection.LeftToRight, _
                                        New Typeface("Verdana"), _
                                        9D, _
                                        Brushes.Black), _
                                    nodeBounds.BottomLeft)
            GoTo recurse

        End If

        If TypeOf contextNode Is WritingRegionNode Then

            nodeBounds.Inflate(3D, 3D)
            drawingContext.DrawRoundedRectangle(Nothing, New Pen(Brushes.Black, 1D), nodeBounds, 1D, 1D)
            drawingContext.DrawText(New FormattedText("Writing Region", _
                                        CultureInfo.CurrentCulture, _
                                        System.Windows.FlowDirection.LeftToRight, _
                                        New Typeface("Verdana"), _
                                        9D, _
                                        Brushes.Black), _
                                    nodeBounds.BottomLeft + New Vector(0, 3))
            GoTo recurse

        End If

        If TypeOf contextNode Is ParagraphNode Then

            nodeBounds.Inflate(2D, 2D) ' inflate so this will be visible outside the line node
            drawingContext.DrawRoundedRectangle(Nothing, New Pen(Brushes.Red, 1D), nodeBounds, 1D, 1D)
            GoTo recurse

        End If

        If TypeOf contextNode Is LineNode Then

            nodeBounds.Inflate(1D, 1D) ' inflate so this will be visible outside the word node
            drawingContext.DrawRoundedRectangle(Nothing, New Pen(Brushes.Orange, 1D), nodeBounds, 1D, 1D)
            GoTo recurse

        End If

recurse:
        For Each subNode As ContextNode In contextNode.SubNodes

            DrawFeedback(drawingContext, subNode)

        Next
    End Sub
End Class
