' Interaction logic for Window1.xaml
Imports System.Windows.Ink

Partial Public Class Window1
    Inherits System.Windows.Window

    Public Sub New()
        InitializeComponent()
    End Sub

    ''' <summary>
    ''' Prepare the InkCanvas for InkAndGesture (if gesture recognition is available).
    ''' </summary>
    Sub OnLoaded(ByVal sender As Object, ByVal e As RoutedEventArgs)

        '  After the window is loaded and laid out but before the user can interact with the application
        ' it is made sure that the user has gesture recognizers installed. If not, gesturing is not enabled 
        ' on the InkCanvas and a message to remedy the situation is displayed.
        ' 
        '  This check is absolutely necessary - if not done, a gesture on the InkAndGesture EditingMode
        ' InkCanvas will throw an exception. Please refer to the SDK for more information regarding 
        ' InkCanvas gestures.

        If (myInkCanvas.IsGestureRecognizerAvailable) Then

            myInkCanvas.EditingMode = InkCanvasEditingMode.InkAndGesture

            ' The gestures in which the application is interested are enabled here.
            Dim gestures() As ApplicationGesture = _
                                {ApplicationGesture.ScratchOut, _
                                 ApplicationGesture.Up, _
                                 ApplicationGesture.Down, _
                                 ApplicationGesture.Left, _
                                 ApplicationGesture.Right}

            myInkCanvas.SetEnabledGestures(gestures)

        Else
            myInkCanvas.EditingMode = InkCanvasEditingMode.None
            myInkTextBox.Text = String.Empty
            myInkLabel.Content = String.Empty
            myNoGestureRecoTextBlock.Visibility = Windows.Visibility.Visible
        End If
    End Sub

    ''' <summary>
    '''  This is the InkCanvas gesture event handler. Here certain gestures are received 
    ''' and acted upon accordingly.
    ''' </summary>

    Sub myInkCanvas_Gesture(ByVal sender As Object, _
        ByVal e As InkCanvasGestureEventArgs) Handles myInkCanvas.Gesture

        Dim topResult As GestureRecognitionResult
        topResult = e.GetGestureRecognitionResults()(0)

        If topResult.RecognitionConfidence = RecognitionConfidence.Strong Then

            Dim gesture As ApplicationGesture
            gesture = topResult.ApplicationGesture

            Select Case gesture
                Case ApplicationGesture.ScratchOut
                    Dim strokesToRemove As StrokeCollection
                    strokesToRemove = myInkCanvas.Strokes.HitTest(e.Strokes.GetBounds(), 10)
                    If strokesToRemove.Count > 0 Then
                        myInkCanvas.Strokes.Remove(strokesToRemove)
                    End If

                Case ApplicationGesture.Right
                    myScrollViewer.ScrollToHorizontalOffset(myScrollViewer.HorizontalOffset + 30)

                Case ApplicationGesture.Left
                    myScrollViewer.ScrollToHorizontalOffset(myScrollViewer.HorizontalOffset - 30)

                Case ApplicationGesture.Up
                    myScrollViewer.ScrollToVerticalOffset(myScrollViewer.VerticalOffset - 30)

                Case ApplicationGesture.Down
                    myScrollViewer.ScrollToVerticalOffset(myScrollViewer.VerticalOffset + 30)

            End Select
        End If
    End Sub
End Class
