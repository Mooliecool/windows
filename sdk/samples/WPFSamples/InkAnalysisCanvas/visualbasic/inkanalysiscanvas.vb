Imports System.Collections.ObjectModel
Imports System.Windows.Controls
Imports System.Windows.Ink

''' <summary>
''' InkAnalysisCanvas demostrates the correct way to 
''' interact with the InkAnalyzer in WPF
''' </summary>
Public Class InkAnalysisCanvas
    Inherits InkCanvas

    Public Sub New()

        _inkAnalyzer = New InkAnalyzer(Me.Dispatcher)

        ' Add a listener to ResultsUpdated event.
        AddHandler _inkAnalyzer.ResultsUpdated, AddressOf OnInkAnalyzerResultsUpdated

        ' Add a listener to StrokesChanged event of InkAnalysis.Strokes collection.
        AddHandler Me.Strokes.StrokesChanged, AddressOf OnStrokesChanged

        Me.ShowInkAnalysisFeedback = True

    End Sub

    ''' <summary>
    ''' InkAnalyzer this InkAnalysisCanvas is updating
    ''' </summary>
    Public Property InkAnalyzer() As InkAnalyzer
        Get
            Return _inkAnalyzer
        End Get
        Set(ByVal value As InkAnalyzer)

        End Set
    End Property

    ''' <summary>
    ''' Set to true to display the parse structure and recognition results
    ''' overlayed on the InkAnalysisCanvas
    ''' </summary>
    Public Property ShowInkAnalysisFeedback() As Boolean
        Get
            Return _showInkAnalysisFeedback
        End Get
        Set(ByVal value As Boolean)

            _showInkAnalysisFeedback = value

            If _adornerDecorator Is Nothing Then

                ' We want to adorn the InkCanvas's inner canvas with an adorner 
                ' that we use to display the parse and recognition results
                _adornerDecorator = CType(GetVisualChild(0), AdornerDecorator)
                Dim inkPresenter As DependencyObject = VisualTreeHelper.GetChild(_adornerDecorator, 0)
                Dim innerCanvas As DependencyObject = VisualTreeHelper.GetChild(inkPresenter, 0)

                _feedbackAdorner = New InkAnalysisFeedbackAdorner(CType(innerCanvas, UIElement), _inkAnalyzer)
                _adornerDecorator.AdornerLayer.Add(_feedbackAdorner)

            End If

            If _showInkAnalysisFeedback Then
                _feedbackAdorner.Visibility = Windows.Visibility.Visible
            Else
                _feedbackAdorner.Visibility = Windows.Visibility.Collapsed
            End If
        End Set
    End Property

    ''' <summary>
    ''' Protected override called when the InkAnalysisCanvas.Strokes StrokeCollection is replaced
    ''' </summary>
    Protected Overrides Sub OnStrokesReplaced(ByVal e As InkCanvasStrokesReplacedEventArgs)

        ' update the InkAnalyzer and unsubscribe / subscribe to StrokeChanged events
        _inkAnalyzer.RemoveStrokes(e.PreviousStrokes)
        RemoveHandler e.PreviousStrokes.StrokesChanged, AddressOf OnStrokesChanged
        For Each stroke As Stroke In e.PreviousStrokes

            ' we're removing this stroke so we don't need to listen 
            ' to StylusPointsChanged anymore
            RemoveHandler stroke.StylusPointsChanged, AddressOf OnStrokeStylusPointsChanged
        Next

        _inkAnalyzer.AddStrokes(e.NewStrokes)
        AddHandler e.NewStrokes.StrokesChanged, AddressOf OnStrokesChanged
        System.Diagnostics.Debug.Assert(e.NewStrokes.Equals(Me.Strokes))
        For Each stroke As Stroke In e.NewStrokes

            ' listen for StylusPointsChanged, which can happen
            ' during move and resize operations
            AddHandler stroke.StylusPointsChanged, AddressOf OnStrokeStylusPointsChanged

        Next

        _inkAnalyzer.BackgroundAnalyze()

    End Sub

    ''' <summary>
    ''' called when a gesture is detected, we use this to detect a scratchout gesture
    ''' and erase strokes
    ''' </summary>
    Protected Overrides Sub OnGesture(ByVal e As InkCanvasGestureEventArgs)
        MyBase.OnGesture(e)

        Dim results As ReadOnlyCollection(Of GestureRecognitionResult) = _
                       e.GetGestureRecognitionResults()

        If results.Count > 0 And _
           results(0).ApplicationGesture = ApplicationGesture.ScratchOut And _
           results(0).RecognitionConfidence = RecognitionConfidence.Strong Then

            ' hit test the InkAnalysisCanvas's Strokes (note that the '1' denotes percent)
            Dim hitStrokes As StrokeCollection = Me.Strokes.HitTest(e.Strokes.GetBounds(), 1)

            ' remove any hit strokes from the InkAnalysisCanvas
            If hitStrokes.Count > 0 Then

                ' OnStrokesChanged will be called after this and 
                ' remove the erased strokes from the InkAnalyzer
                Me.Strokes.Remove(hitStrokes)
            End If

            e.Cancel = False

        Else

            ' cancel the event, it wasn't a scratchout
            e.Cancel = True

        End If
    End Sub

    ''' <summary>
    ''' OnStrokesChanged - A handler for InkCanvas.Strokes.StrokesChanged event
    ''' </summary>
    Private Sub OnStrokesChanged(ByVal sender As Object, ByVal e As StrokeCollectionChangedEventArgs)

        ' flag we set when we change the StrokeCollection within this method
        ' to prevent recursion
        If _ignoreStrokesChanged Then

            _ignoreStrokesChanged = False
            Return

        End If

        ' special case: when point erasing and you trim the end 
        ' of a single stroke, the InkCanvas will replace it with a shorter stroke
        ' to improve recognition, we want to keep the original stroke but to update
        ' it's stroke data
        If Me.ActiveEditingMode = InkCanvasEditingMode.EraseByPoint And _
           e.Removed.Count = 1 And e.Added.Count = 1 Then

            _ignoreStrokesChanged = True

            ' get the index the stroke was replaced at
            Dim addedIndex As Int32 = Me.Strokes.IndexOf(e.Added(0))
            Debug.Assert(addedIndex >= 0)

            Dim removedStroke As Stroke = e.Removed(0)
            removedStroke.StylusPoints = e.Added(0).StylusPoints.Clone()

            ' let the InkAnalyzer know that we're about to invalidate 
            ' the StylusPoints on removedStroke
            _inkAnalyzer.ClearStrokeData(removedStroke)

            ' update the dirty region
            _inkAnalyzer.DirtyRegion.Union(e.Added(0).GetBounds())
            _inkAnalyzer.DirtyRegion.Union(e.Removed(0).GetBounds())

            'replace the added stroke with the removed stroke (which has had it's StylusPoints updated)
            Me.Strokes(addedIndex) = removedStroke

        Else

            ' Update the ink data of the ink analyzer.
            If e.Removed.Count > 0 Then
                For Each stroke As Stroke In e.Removed

                    ' we're removing this stroke so we don't need to listen 
                    ' to StylusPointsChanged anymore
                    RemoveHandler stroke.StylusPointsChanged, AddressOf OnStrokeStylusPointsChanged

                Next

                _inkAnalyzer.RemoveStrokes(e.Removed)

            End If

            If e.Added.Count > 0 Then

                For Each stroke As Stroke In e.Added

                    ' listen for StylusPointsChanged, which can happen
                    ' during move and resize operations
                    AddHandler stroke.StylusPointsChanged, AddressOf OnStrokeStylusPointsChanged

                Next

                _inkAnalyzer.AddStrokes(e.Added)
                _inkAnalyzer.SetStrokesType(e.Added, StrokeType.Unspecified)

            End If
        End If

        _inkAnalyzer.BackgroundAnalyze()

    End Sub

    ''' <summary>
    ''' OnSelectionChanging raised when selection is changing on the InkAnalysisCanvas
    ''' we use this to select entire nodes instead of just a stroke
    ''' </summary>
    Protected Overrides Sub OnSelectionChanging(ByVal e As InkCanvasSelectionChangingEventArgs)

        Dim currentSelectedStrokes As StrokeCollection = e.GetSelectedStrokes()
        If currentSelectedStrokes.Count > 0 Then

            Dim nodes As ContextNodeCollection = _inkAnalyzer.FindInkLeafNodes(e.GetSelectedStrokes())

            If nodes.Count > 0 Then

                Dim expandedSelection As StrokeCollection = New StrokeCollection()

                ' add all related strokes if they aren't part of selectedStrokes already
                For Each node As ContextNode In nodes
                    expandedSelection.Add(node.Strokes)
                Next

                ' modify the collection
                e.SetSelectedStrokes(expandedSelection)

            End If
        End If

        MyBase.OnSelectionChanging(e)

    End Sub

    ''' <summary>
    ''' Called when the StylusPoints on a Stroke are changed
    ''' we use this to invalidate the stroke data in the analyzer
    ''' </summary>
    Private Sub OnStrokeStylusPointsChanged(ByVal sender As Object, _
                                            ByVal e As EventArgs)

        Dim changedStroke As Stroke = CType(sender, Stroke)

        ' a stroke's StylusPoints have changed we need to find
        ' all affected contextNodes's and mark the dirty region with them
        Dim strokesThatChanged As StrokeCollection = New StrokeCollection()
        strokesThatChanged.Add(changedStroke)
        Dim dirtyNodes As ContextNodeCollection = _
               _inkAnalyzer.FindInkLeafNodes(strokesThatChanged)

        For Each dirtyNode As ContextNode In dirtyNodes

            ' let the analyzer know that where the stroke previously 
            ' existed is now dirty
            _inkAnalyzer.DirtyRegion.Union(dirtyNode.Location.GetBounds())

        Next

        ' let the analyzer know that the stroke data is no longer valid
        _inkAnalyzer.ClearStrokeData(changedStroke)

        ' finally, make the region where the stroke now exists dirty also
        _inkAnalyzer.DirtyRegion.Union(changedStroke.GetBounds())

        _inkAnalyzer.BackgroundAnalyze()

    End Sub

    ''' <summary>
    ''' OnInkAnalyzerResultsUpdated - A handler for InkAnalyzer.ResultsUpdated event
    ''' which is called when BackgroundAnalyze completes
    ''' </summary>
    Private Sub OnInkAnalyzerResultsUpdated(ByVal sender As System.Object, _
                                            ByVal e As ResultsUpdatedEventArgs)

        If Not _feedbackAdorner Is Nothing Then

            ' cause the feedback adorner to repaint itself
            _feedbackAdorner.InvalidateVisual()

        End If

        ' If the user has made edits while analysis was being performed, trigger
        ' BackgroundAnalyze again to analyze these changes
        If Not _inkAnalyzer.DirtyRegion.IsEmpty Then
            _inkAnalyzer.BackgroundAnalyze()
        End If
    End Sub

    ''' <summary>
    ''' our private InkAnalyzer
    ''' </summary>
    Private _inkAnalyzer As InkAnalyzer
    ''' <summary>
    ''' Flag to prevent recursion in OnStrokesChanged
    ''' </summary>
    Private _ignoreStrokesChanged As Boolean = False
    ''' <summary>
    ''' Flag set via ShowInkAnalysisFeedback that determines if 
    ''' we should show parsing structure feedback and analysis results 
    ''' overlayed on the strokes
    ''' </summary>
    Private _showInkAnalysisFeedback As Boolean = True
    ''' <summary>
    ''' The private AdornerDecorator InkCanvas uses to render selection feedback.
    ''' We use it to display feedback for InkAnalysis
    ''' </summary>
    Private _adornerDecorator As System.Windows.Documents.AdornerDecorator = Nothing
    Private _feedbackAdorner As InkAnalysisFeedbackAdorner = Nothing

End Class
