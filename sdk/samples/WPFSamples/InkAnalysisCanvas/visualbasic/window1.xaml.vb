Imports System.Windows.Controls.Primitives
Imports System.Windows.Ink

' Interaction logic for Window1.xaml
Partial Public Class Window1
    Inherits System.Windows.Window

    ''' <summary>
    ''' Used to keep track of which results to highlight
    ''' </summary>
    Private _resultIndex As Int32 = 0
    Private _searchText As String = ""

    Public Sub New()
        InitializeComponent()
    End Sub

    ''' <summary>
    ''' Button click event handlers
    ''' </summary>
    Private Sub btnInk_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

        inkAnalysisCanvas.EditingMode = InkCanvasEditingMode.InkAndGesture
        SetToggleButtonState(CType(sender, ToggleButton))

    End Sub

    Private Sub btnEraseByStroke_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

        inkAnalysisCanvas.EditingMode = InkCanvasEditingMode.EraseByStroke
        SetToggleButtonState(CType(sender, ToggleButton))

    End Sub

    Private Sub btnEraseByPoint_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

        inkAnalysisCanvas.EditingMode = InkCanvasEditingMode.EraseByPoint
        SetToggleButtonState(CType(sender, ToggleButton))

    End Sub

    Private Sub btnSelect_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

        inkAnalysisCanvas.EditingMode = InkCanvasEditingMode.Select
        SetToggleButtonState(CType(sender, ToggleButton))

    End Sub

    ''' <summary>
    ''' Sets the toggle button state to off for all ToggleButtons other than the one passed in
    ''' </summary>
    Private Sub SetToggleButtonState(ByVal button As ToggleButton)

        If Not button.Equals(btnInk) Then
            btnInk.IsChecked = False
        End If

        If Not button.Equals(btnEraseByStroke) Then
            btnEraseByStroke.IsChecked = False
        End If

        If Not button.Equals(btnEraseByPoint) Then
            btnEraseByPoint.IsChecked = False
        End If

        If Not button.Equals(btnSelect) Then
            btnSelect.IsChecked = False
        End If
    End Sub

    ''' <summary>
    ''' Handler for turning feedback on / off
    ''' </summary>
    Private Sub ToggleFeedback_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

        inkAnalysisCanvas.ShowInkAnalysisFeedback = Not inkAnalysisCanvas.ShowInkAnalysisFeedback

    End Sub

    Private Sub tbFind_gotFocus(ByVal sender As Object, ByVal e As RoutedEventArgs)

        tbFind.Text = ""

    End Sub

    ''' <summary>
    ''' Handles searching for text in the document
    ''' </summary>
    Private Sub Find_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

        If Not _searchText = tbFind.Text Then
            _resultIndex = 0
        End If

        _searchText = tbFind.Text
        Dim searchResults As StrokeCollection() = inkAnalysisCanvas.InkAnalyzer.Search(_searchText)

        If searchResults.Length = 0 Then

            MessageBox.Show("Could not find the text '" + _searchText + "'")

        Else

            ' switch to selection mode and select the strokes
            ' we found
            inkAnalysisCanvas.EditingMode = InkCanvasEditingMode.Select
            SetToggleButtonState(btnSelect)

            If _resultIndex >= searchResults.Length Then

                ' we wrapped around to the beginning
                _resultIndex = 0

            End If

            inkAnalysisCanvas.Select(searchResults(_resultIndex))

            ' increment the result index so if the user clicks find more 
            ' than once, they advance to the next result
            _resultIndex += 1

        End If
    End Sub
End Class
