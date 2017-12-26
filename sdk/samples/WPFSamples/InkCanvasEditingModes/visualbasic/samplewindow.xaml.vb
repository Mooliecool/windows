' Interaction logic for SampleWindow.xaml
Imports System.Collections.ObjectModel, _
        System.Windows.Data, _
        System.Windows.Documents, _
        System.Windows.Ink

''' <summary>
''' EditingMode Ids
''' </summary>
''' <remarks></remarks>
Public Enum EditingModeId
    Pen1
    Pen2
    Pen3
    Pen4
    Pen5
    Highlighter1
    Highlighter2
    Highlighter3
    Highlighter4
    Highlighter5
    PointEraser1
    PointEraser2
    PointEraser3
    CurrentPen
    CurrentHighlighter
    CurrentEraser
    SelectionTool
    StrokeEraser
    None
End Enum
''' <summary>
''' Option Ids
''' </summary>
''' <remarks></remarks>
Public Enum OptionId
    InkAndGesture
    Ink
    GestureOnly
End Enum
''' <summary>
''' A converter used by Option menu items.
''' If the current ink mode is equal to the value expected by an item, Convert method will return true.
''' Then the item's check mark can be shown or hidden accordingly
''' </summary>
''' <remarks></remarks>
Public Class InkEditingModeOptionIsCheckedConverter
    Implements IValueConverter

#Region "IValueConverter Members"

    Public Function Convert(ByVal value As Object, ByVal targetType As System.Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.Convert
        If value Is Nothing Then Return Nothing

        ' Get the current ink mode which is one of the values - OptionId.Ink, OptionId.GestureOnly and OptionId.InkAndGesture
        Dim currentOptionId As OptionId = value
        ' Get the expected Id which the menu item expects.
        Dim expectedOptionId As OptionId = parameter

        ' return true if the active option id is same as the item's
        If currentOptionId = expectedOptionId Then
            Return True
        Else
            Return False
        End If
    End Function

    Public Function ConvertBack(ByVal value As Object, ByVal targetType As System.Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.ConvertBack
        Return Nothing
    End Function

#End Region

End Class

''' <summary>
''' A converter used by the various controls (Tools items, Selection button, Pen/Hight/Eraser DropDownButton and items
''' If the bound SampleWindow property equals to the value expected by the control, Convert method will return true.
''' Then the item's check mark can be shown or hidden accordingly
''' </summary>
''' <remarks></remarks>
Public Class EditingModeIsCheckedConverter
    Implements IValueConverter

#Region "IValueConverter Members"

    Public Function Convert(ByVal value As Object, ByVal targetType As System.Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.Convert
        If value Is Nothing Then Return Nothing

        ' Get the bound property value.
        Dim currentEditingModeId As EditingModeId = value
        ' Get the id which the control expects
        Dim expectedEditingModeId As EditingModeId = parameter

        If expectedEditingModeId = currentEditingModeId Then
            ' Returns true if the two values are same.
            Return True
        ElseIf expectedEditingModeId = EditingModeId.CurrentPen _
                And (currentEditingModeId >= EditingModeId.Pen1 And currentEditingModeId <= EditingModeId.Pen5) Then
            ' The pen toolbar button should be checked whenever any pen mode is active.
            Return True
        ElseIf expectedEditingModeId = EditingModeId.CurrentHighlighter _
           And (currentEditingModeId >= EditingModeId.Highlighter1 And currentEditingModeId <= EditingModeId.Highlighter5) Then
            ' The highlighter toolbar button should be checked whenever any highlighter mode is active.
            Return True
        ElseIf expectedEditingModeId = EditingModeId.CurrentEraser _
            And ((currentEditingModeId >= EditingModeId.PointEraser1 And currentEditingModeId <= EditingModeId.PointEraser3) _
                Or currentEditingModeId = EditingModeId.StrokeEraser) Then
            ' The eraser toolbar button should be checked whenever any eraser mode is active.
            Return True
        End If

        Return False
    End Function

    Public Function ConvertBack(ByVal value As Object, ByVal targetType As System.Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.ConvertBack
        ' No reverse conversion. Don't update the source
        Return Binding.DoNothing
    End Function

#End Region

End Class
''' <summary>
''' A converter which returns the corresponding DrawingAttributes object based on a pen/highlighter id.
''' The DrawingAttributes object can be visualized by our DataTemplates.
''' </summary>
''' <remarks></remarks>
Public Class EditingModeIdToDrawingAttributesConverter
    Implements System.Windows.Data.IValueConverter

#Region "IValueConverter Members"

    Public Function Convert(ByVal value As Object, ByVal targetType As System.Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.Convert
        If value Is Nothing Then Return Nothing

        ' Get the pre-defined DrawingAttributes collection
        Dim predefinedDrawingAttributes As ObservableCollection(Of DrawingAttributes) = value
        ' Get the pen/highlighter id associated to this control.
        Dim expectedEditingModeId As EditingModeId = parameter

        If expectedEditingModeId >= EditingModeId.Pen1 And expectedEditingModeId <= EditingModeId.Highlighter5 Then
            ' return the specified DrawingAttributes
            Return predefinedDrawingAttributes(expectedEditingModeId)
        End If

        Return Nothing
    End Function

    Public Function ConvertBack(ByVal value As Object, ByVal targetType As System.Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.ConvertBack
        ' Don't update the source
        Return Binding.DoNothing
    End Function

#End Region

End Class
''' <summary>
''' A converter which returns the string name of a DrawingAttributes. This string doesn't contain the color name.
''' </summary>
''' <remarks></remarks>
Public Class DrawingAttributesDropDownItemCaptionConverter
    Implements IValueConverter

#Region "IValueConverter Members"

    Public Overridable Function Convert(ByVal value As Object, ByVal targetType As System.Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.Convert
        If value Is Nothing Then Return Nothing

        ' Get the DrawingAttributes object
        Dim da As DrawingAttributes = value
        ' Returns the built-in string name.
        Dim name As String = String.Empty

        If Not da.IsHighlighter Then
            Dim strSize As String = String.Empty
            Dim strShape As String = String.Empty

            If da.Height = 1.44 Then
                strSize = "Fine"
            ElseIf da.Height = 0.96 Then
                strSize = "Very Fine"
            ElseIf da.Height = 2.88 Then
                strSize = "Medium"
            ElseIf da.Height = 8.64 Then
                strSize = "Marker (2mm)"
            End If

            If da.StylusTip = StylusTip.Rectangle Then
                strShape = "Chisel"
            Else
                strShape = "Point"
            End If

            Return String.Format("{0} {1}", strSize, strShape)
        Else
            Dim strSize As String
            strSize = String.Empty

            If da.Height = 26.4 Then
                strSize = "Medium"
            ElseIf da.Height = 39.648 Then
                strSize = "Thick"
            ElseIf da.Height = 47.232 Then
                strSize = "Extra Thick"
            End If

            Return strSize

        End If
    End Function

    Public Function ConvertBack(ByVal value As Object, ByVal targetType As System.Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.ConvertBack
        ' Don't update the source
        Return Binding.DoNothing
    End Function

#End Region

End Class
''' <summary>
''' A converter which returns the string name of a DrawingAttributes. This string also contains the color name.
''' </summary>
''' <remarks></remarks>
Public Class DrawingAttributesMenuCaptionConverter
    Inherits DrawingAttributesDropDownItemCaptionConverter

    Public Overrides Function Convert(ByVal value As Object, ByVal targetType As System.Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object
        If value Is Nothing Then Return Nothing

        ' Call base to get the string name
        Dim caption As String = MyBase.Convert(value, targetType, parameter, culture)

        ' Add the color suffix
        Dim da As DrawingAttributes = value

        If da.Color = Colors.Black Then
            caption += " (Black)"
        ElseIf da.Color = Colors.Indigo Then
            caption += " (Indigo)"
        ElseIf da.Color = Colors.Red Then
            caption += " (Red)"
        ElseIf da.Color = Colors.Yellow Then
            caption += " (Yellow)"
        ElseIf (da.Color = Colors.LightGreen) Then
            caption += " (Bright Green)"
        ElseIf (da.Color = Colors.Turquoise) Then
            caption += " (Turquoise)"
        ElseIf (da.Color = Colors.Pink) Then
            caption += " (Pink)"
        ElseIf da.Color = Color.FromArgb(&HFF, &HFF, &H99, &H0) Then
            caption += " (Light Orange)"
        Else
            caption += " (" + da.Color.ToString() + ")"
        End If

        Return caption
    End Function
End Class
''' <summary>
''' A converter which returns the string name for a given eraser mode id.
''' </summary>
''' <remarks></remarks>
Public Class EraserNameConverter
    Implements IValueConverter

#Region "IValueConverter Members"

    Public Function Convert(ByVal value As Object, ByVal targetType As System.Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.Convert
        If value Is Nothing Then Return Nothing

        Dim name As String = String.Empty
        Dim modeId As EditingModeId = value

        ' Returns the string for a given eraser id.
        Select Case modeId
            Case EditingModeId.StrokeEraser
                name = "Stroke"
            Case EditingModeId.PointEraser1
                name = "Small"
            Case EditingModeId.PointEraser2
                name = "Medium"
            Case EditingModeId.PointEraser3
                name = "Large"
        End Select

        Return name
    End Function

    Public Function ConvertBack(ByVal value As Object, ByVal targetType As System.Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.ConvertBack
        Return Nothing
    End Function

#End Region

End Class
''' <summary>
''' A multivalue converter which returns the DrawingAttributes of the current active pen or the active highlighter.
''' The DrawingAttributes will be visualized on the pen toolbar button or the highlighter toolbar button.
''' </summary>
''' <remarks></remarks>
Public Class CurrentDrawingAttributesConverter
    Implements IMultiValueConverter

    Public Function Convert(ByVal values() As Object, ByVal targetType As System.Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IMultiValueConverter.Convert
        If (values Is Nothing Or values.Length = 0) Then Return Nothing

        For Each value As Object In values
            ' Some source binding could have no value being set yet. If it's the case, return UnsetValue
            ' to indicate no value produced by the converter.
            If value Is DependencyProperty.UnsetValue Then
                Return DependencyProperty.UnsetValue
            End If
        Next

        ' Get the active pen or highlighter id
        Dim currentActivePenOrHighlighterModeId As EditingModeId = values(0)
        ' Get the collection.
        Dim predefinedDrawingAttributes As ObservableCollection(Of DrawingAttributes) = values(1)

        ' Return the DrawingAttributes object.
        Return predefinedDrawingAttributes(currentActivePenOrHighlighterModeId)

    End Function

    Public Function ConvertBack(ByVal value As Object, ByVal targetTypes() As System.Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object() Implements System.Windows.Data.IMultiValueConverter.ConvertBack
        Return Nothing
    End Function
End Class
''' <summary>
''' Interaction logic for SampleWindow.xaml
''' </summary>
''' <remarks></remarks>
Partial Public Class SampleWindow
    Inherits System.Windows.Window

    ''' <summary>
    ''' Initialization.
    ''' </summary>
    Protected Overrides Sub OnInitialized(ByVal e As System.EventArgs)
        MyBase.OnInitialized(e)
        _cmdStack = New CommandStack(MyInkCanvas.Strokes)
    End Sub


#Region "Constructor"

    ''' <summary>
    ''' Constructor
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub New()
        InitializeComponent()
        ' Initialize Ink Mode option list
        _inkModeOptions = New List(Of InkCanvasEditingMode)( _
                            New InkCanvasEditingMode() { _
                                    InkCanvasEditingMode.InkAndGesture, _
                                    InkCanvasEditingMode.Ink, _
                                    InkCanvasEditingMode.GestureOnly})

        ' Initiliaze the InkCanvas
        ChangeToInkMode(EditingModeId.Pen1)

        ' Setup the command bindings
        Me.CommandBindings.Add(New CommandBinding(ApplicationCommands.Close, _
                                New ExecutedRoutedEventHandler(AddressOf OnExecutedCommands), _
                                New CanExecuteRoutedEventHandler(AddressOf OnCanExecutedCommands)))

        Me.CommandBindings.Add(New CommandBinding(SampleWindow.ClearCommand, _
                                    New ExecutedRoutedEventHandler(AddressOf OnExecutedCommands), _
                                    New CanExecuteRoutedEventHandler(AddressOf OnCanExecutedCommands)))

        Me.CommandBindings.Add(New CommandBinding(SampleWindow.EditingCommand, _
                                    New ExecutedRoutedEventHandler(AddressOf OnExecutedCommands), _
                                    New CanExecuteRoutedEventHandler(AddressOf OnCanExecutedCommands)))

        Me.CommandBindings.Add(New CommandBinding(SampleWindow.OptionCommand, _
                                    New ExecutedRoutedEventHandler(AddressOf OnExecutedCommands), _
                                    New CanExecuteRoutedEventHandler(AddressOf OnCanExecutedCommands)))

        Me.CommandBindings.Add(New CommandBinding(ApplicationCommands.Undo, _
                                    New ExecutedRoutedEventHandler(AddressOf OnExecutedCommands), _
                                    New CanExecuteRoutedEventHandler(AddressOf OnCanExecutedCommands)))

        Me.CommandBindings.Add(New CommandBinding(ApplicationCommands.Redo, _
                                    New ExecutedRoutedEventHandler(AddressOf OnExecutedCommands), _
                                    New CanExecuteRoutedEventHandler(AddressOf OnCanExecutedCommands)))


        AddHandler MyInkCanvas.Gesture, New InkCanvasGestureEventHandler(AddressOf OnInkCanvasGesture)
        AddHandler MyHyperline.Click, New RoutedEventHandler(AddressOf OnHyperlinkClick)

        AddHandler MyInkCanvas.Strokes.StrokesChanged, New StrokeCollectionChangedEventHandler(AddressOf Strokes_StrokesChanged)
        AddHandler MyInkCanvas.SelectionMoving, New InkCanvasSelectionEditingEventHandler(AddressOf MyInkCanvas_SelectionMovingOrResizing)
        AddHandler MyInkCanvas.SelectionResizing, New InkCanvasSelectionEditingEventHandler(AddressOf MyInkCanvas_SelectionMovingOrResizing)
        AddHandler MyInkCanvas.MouseUp, New MouseButtonEventHandler(AddressOf MyInkCanvas_MouseUp)

    End Sub

#End Region

#Region "Public Commands"

    ''' <summary>
    ''' The Clear Command
    ''' </summary>
    Public Shared ReadOnly ClearCommand As RoutedCommand = New RoutedCommand("Clear", GetType(SampleWindow))

    ''' <summary>
    ''' The Editing Command
    ''' </summary>
    Public Shared ReadOnly EditingCommand As RoutedCommand = New RoutedCommand("Editing", GetType(SampleWindow))

    ''' <summary>
    ''' The Option Command
    ''' </summary>
    Public Shared ReadOnly OptionCommand As RoutedCommand = New RoutedCommand("Option", GetType(SampleWindow))

#End Region

#Region "Public Properties"

    ''' <summary>
    ''' Returns the built-in DrawingAttributes collection
    ''' </summary>
    Public ReadOnly Property DrawingAttributesCollection() As ObservableCollection(Of DrawingAttributes)
        Get
            If _predefinedDrawingAttributes Is Nothing Then
                ' Initialize the DrawingAttributes list
                _predefinedDrawingAttributes = New ObservableCollection(Of DrawingAttributes)()

                Dim drawingAttributesList As DrawingAttributes() = Me.FindResource("MyPenDrawingAttributes")
                Dim count As Integer = drawingAttributesList.Length

                For i As Integer = 0 To count - 1
                    _predefinedDrawingAttributes.Add(drawingAttributesList(i))
                Next

                drawingAttributesList = Me.FindResource("MyHighlighterDrawingAttributes")
                count = drawingAttributesList.Length
                For i As Integer = 0 To count - 1
                    _predefinedDrawingAttributes.Add(drawingAttributesList(i))
                Next
            End If

            Return _predefinedDrawingAttributes
        End Get
    End Property

    ''' <summary>
    ''' Returns the built-in PointEraser Collection
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property PointEraserShapeCollection() As ObservableCollection(Of StylusShape)
        Get
            If _predefinedStylusShapes Is Nothing Then
                ' Initialize EraserShape list
                _predefinedStylusShapes = New ObservableCollection(Of StylusShape)()

                ' Small
                _predefinedStylusShapes.Add(New RectangleStylusShape(6, 6))

                ' Medium
                _predefinedStylusShapes.Add(New RectangleStylusShape(18, 18))

                ' Large
                _predefinedStylusShapes.Add(New RectangleStylusShape(32, 32))
            End If
            Return _predefinedStylusShapes
        End Get
    End Property

    ''' <summary>
    ''' The DependencyProperty for the EditingMode property.
    ''' </summary>
    Public Shared ReadOnly EditingModeProperty As DependencyProperty = _
            DependencyProperty.Register( _
                    "EditingMode", _
                    GetType(EditingModeId), _
                    GetType(SampleWindow), _
                    New FrameworkPropertyMetadata( _
                            EditingModeId.Pen1, _
                            New PropertyChangedCallback(AddressOf OnEditingModeChanged)))

    ''' <summary>
    ''' Gets/Sets the EditingMode property.
    ''' </summary>
    Public Property EditingMode() As EditingModeId
        Get
            Return GetValue(EditingModeProperty)
        End Get
        Set(ByVal value As EditingModeId)
            SetValue(EditingModeProperty, value)
        End Set
    End Property

    ''' <summary>
    ''' EditingMode property change callback handler 
    ''' </summary>
    ''' <param name="d"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Shared Sub OnEditingModeChanged(ByVal d As DependencyObject, ByVal e As DependencyPropertyChangedEventArgs)
        Dim myWindow As SampleWindow = d
        myWindow.ChangeEditingMode(e.NewValue)
    End Sub

    ''' <summary>
    ''' A private DependencyPropertyKey is used for the read-only CurrentInkModeOption DependencyProperty
    ''' </summary>
    Private Shared ReadOnly CurrentInkModeOptionPropertyKey As DependencyPropertyKey = _
            DependencyProperty.RegisterReadOnly("CurrentInkModeOption", _
                GetType(OptionId), _
                GetType(SampleWindow), _
                New FrameworkPropertyMetadata(OptionId.InkAndGesture))

    ''' <summary>
    ''' The read-only CurrentInkModeOption Dependency Property
    ''' </summary>
    Public Shared ReadOnly CurrentInkModeOptionProperty As DependencyProperty = _
            CurrentInkModeOptionPropertyKey.DependencyProperty

    ''' <summary>
    ''' CLR getter for CurrentPenMode
    ''' </summary>
    Public ReadOnly Property CurrentInkModeOption() As OptionId
        Get
            Return GetValue(CurrentInkModeOptionProperty)
        End Get
    End Property

    ''' <summary>
    ''' A private DependencyPropertyKey is used for the read-only CurrentPenMode DependencyProperty
    ''' </summary>
    Private Shared ReadOnly CurrentPenModePropertyKey As DependencyPropertyKey = _
            DependencyProperty.RegisterReadOnly("CurrentPenMode", _
                GetType(EditingModeId), _
                GetType(SampleWindow), _
                New FrameworkPropertyMetadata(EditingModeId.Pen1))

    ''' <summary>
    ''' The read-only CurrentPenMode Dependency Property
    ''' </summary>
    Public Shared ReadOnly CurrentPenModeProperty As DependencyProperty = _
                CurrentPenModePropertyKey.DependencyProperty

    ''' <summary>
    ''' CLR getter for CurrentPenMode
    ''' </summary>
    Public ReadOnly Property CurrentPenMode() As EditingModeId
        Get
            Return GetValue(CurrentPenModeProperty)
        End Get
    End Property

    ''' <summary>
    ''' A private DependencyPropertyKey is used for the read-only CurrentHighlighterMode DependencyProperty
    ''' </summary>
    Private Shared ReadOnly CurrentHighlighterModePropertyKey As DependencyPropertyKey = _
            DependencyProperty.RegisterReadOnly("CurrentHighlighterMode", _
                GetType(EditingModeId), _
                GetType(SampleWindow), _
                New FrameworkPropertyMetadata(EditingModeId.Highlighter1))

    ''' <summary>
    ''' The read-only CurrentHighlighterMode Dependency Property
    ''' </summary>
    Public Shared ReadOnly CurrentHighlighterModeProperty As DependencyProperty = _
            CurrentHighlighterModePropertyKey.DependencyProperty

    ''' <summary>
    ''' CLR getter for CurrentHighlighterMode
    ''' </summary>
    Public ReadOnly Property CurrentHighlighterMode() As EditingModeId
        Get
            Return GetValue(CurrentHighlighterModeProperty)
        End Get
    End Property

    ''' <summary>
    ''' A private DependencyPropertyKey is used for the read-only CurrentEraserMode DependencyProperty
    ''' </summary>
    Private Shared ReadOnly CurrentEraserModePropertyKey As DependencyPropertyKey = _
            DependencyProperty.RegisterReadOnly("CurrentEraserMode", _
                GetType(EditingModeId), _
                GetType(SampleWindow), _
                New FrameworkPropertyMetadata(EditingModeId.StrokeEraser))

    ''' <summary>
    ''' The read-only CurrentEraserMode Dependency Property
    ''' </summary>
    Public Shared ReadOnly CurrentEraserModeProperty As DependencyProperty = _
                CurrentEraserModePropertyKey.DependencyProperty

    ''' <summary>
    ''' CLR getter for CurrentEraserMode
    ''' </summary>
    Public ReadOnly Property CurrentEraserMode() As EditingModeId
        Get
            Return GetValue(CurrentEraserModeProperty)
        End Get
    End Property

#End Region

#Region "Private Methods"

    ''' <summary>
    ''' A class handler which handles the various commands.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Shared Sub OnExecutedCommands(ByVal sender As Object, ByVal e As ExecutedRoutedEventArgs)
        Dim myWindow As SampleWindow = sender

        If e.Command Is ApplicationCommands.Close Then
            ' Close the main window.
            myWindow.Close()
        ElseIf e.Command Is SampleWindow.ClearCommand Then
            ' Clear the current strokes.
            myWindow.ClearStrokes()
        ElseIf e.Command Is SampleWindow.EditingCommand Then
            Dim newEditingMode As EditingModeId = e.Parameter

            If newEditingMode = EditingModeId.CurrentPen Then
                ' The Pen toolbar button is clicked. 
                ' We will switch to the mode with the active pen setting.
                newEditingMode = myWindow.CurrentPenMode
            ElseIf newEditingMode = EditingModeId.CurrentHighlighter Then
                ' The Highlighter toolbar button is clicked. 
                ' We will switch to the mode with the active highlighter setting.
                newEditingMode = myWindow.CurrentHighlighterMode
            ElseIf newEditingMode = EditingModeId.CurrentEraser Then
                ' The Eraser toolbar button is clicked. 
                ' We will switch to the mode with the active eraser setting.
                newEditingMode = myWindow.CurrentEraserMode
            End If

            ' Switch to the specified mode.
            myWindow.EditingMode = newEditingMode
        ElseIf e.Command Is SampleWindow.OptionCommand Then
            ' Switch to the specified ink mode (Ink, GestureOnly or InkAndGesture).
            myWindow.ChangeInkModeOption(e.Parameter)
        ElseIf e.Command Is ApplicationCommands.Undo Then
            myWindow.Undo(sender, e)
        ElseIf e.Command Is ApplicationCommands.Redo Then
            myWindow.Redo(sender, e)
        End If
    End Sub

    ''' <summary>
    ''' A handler which handles the enabled status for a command.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Shared Sub OnCanExecutedCommands(ByVal sender As Object, ByVal e As CanExecuteRoutedEventArgs)
        Dim myWindow As SampleWindow = sender

        ' By default, enable all commands.
        e.CanExecute = True

        If e.Command Is SampleWindow.ClearCommand Then
            ' Enable Clear command only if there is a non-empty stroke collection.
            e.CanExecute = IIf(myWindow.MyInkCanvas.Strokes.Count <> 0, True, False)
        ElseIf e.Command Is ApplicationCommands.Undo Then
            ' Enable only if there are items on the command stack
            e.CanExecute = myWindow._cmdStack.CanUndo
        ElseIf e.Command Is ApplicationCommands.Redo Then
            ' Enable only if there are items on the command stack
            e.CanExecute = myWindow._cmdStack.CanRedo
        End If
    End Sub

    ''' <summary>
    ''' A handler which handles the Gesture events.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub OnInkCanvasGesture(ByVal sender As Object, ByVal e As InkCanvasGestureEventArgs)
        Dim results As ReadOnlyCollection(Of GestureRecognitionResult) = e.GetGestureRecognitionResults()

        If results.Count <> 0 And results(0).RecognitionConfidence = RecognitionConfidence.Strong Then
            If CurrentInkModeOption = OptionId.GestureOnly Then
                ' Show gesture feedback in the GestureOnly mode
                GestureResultAdorner.ShowMessage(results(0).ApplicationGesture.ToString(), e.Strokes.GetBounds().TopLeft)
            Else
                ' In InkAndGesture mode, and if not using a highlighter, if a ScratchOut 
                ' gesture is detected then remove the underlying strokes
                If results(0).ApplicationGesture = ApplicationGesture.ScratchOut AndAlso _
                   Not e.Strokes(0).DrawingAttributes.IsHighlighter Then
                    Dim strokesToRemove As StrokeCollection = MyInkCanvas.Strokes.HitTest(e.Strokes.GetBounds(), 10)
                    If strokesToRemove.Count <> 0 Then
                        MyInkCanvas.Strokes.Remove(strokesToRemove)
                    End If
                Else
                    ' Otherwise cancel the gesture.
                    e.Cancel = True
                End If
            End If
        Else
            ' Cancel the gesture.
            e.Cancel = True
        End If
    End Sub

    ''' <summary>
    ''' Hyperlink Click event handler
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub OnHyperlinkClick(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Launch the brower with the gesture information url.
        System.Diagnostics.Process.Start(GestureInfoUrl)
    End Sub

    ''' <summary>
    ''' Clear the stroke collection on the InkCanvas
    ''' </summary>
    Private Sub ClearStrokes()
        MyInkCanvas.Strokes.Clear()
    End Sub

    ''' <summary>
    ''' Change the editing mode.
    ''' </summary>
    ''' <param name="modeId"></param>
    Private Sub ChangeEditingMode(ByVal modeId As EditingModeId)
        Select Case modeId
            Case EditingModeId.Pen1 To EditingModeId.Pen5, _
                 EditingModeId.Highlighter1 To EditingModeId.Highlighter5
                ' Change to the ink mode with the correct pen or highlighter setting.
                ChangeToInkMode(modeId)
            Case EditingModeId.PointEraser1 To EditingModeId.PointEraser3, _
                 EditingModeId.StrokeEraser
                ' Change to the erase mode with the correct eraser.
                ChangeToEraseMode(modeId)
            Case EditingModeId.SelectionTool
                ' Change to the select mode.
                EnsureEditingMode(InkCanvasEditingMode.Select)
            Case EditingModeId.None
                ' Change to the none mode.
                EnsureEditingMode(InkCanvasEditingMode.None)
        End Select
    End Sub

    ''' <summary>
    ''' Change the ink mode option
    ''' </summary>
    ''' <param name="modeOption"></param>
    Private Sub ChangeInkModeOption(ByVal modeOption As OptionId)
        ' Change the option setting.
        SetValue(CurrentInkModeOptionPropertyKey, modeOption)

        ' If the InkCanvas is under one of any inking mode, we need to update the new inking mode at once.
        Dim currentEditingMode As InkCanvasEditingMode = MyInkCanvas.EditingMode
        If currentEditingMode = InkCanvasEditingMode.Ink _
            Or currentEditingMode = InkCanvasEditingMode.InkAndGesture _
            Or currentEditingMode = InkCanvasEditingMode.GestureOnly Then
            EnsureEditingMode(_inkModeOptions(CurrentInkModeOption - OptionId.InkAndGesture))
        End If
    End Sub

    ''' <summary>
    ''' Change to the ink mode with a pen/highlighter setting.
    ''' </summary>
    ''' <param name="editingModeId"></param>
    Private Sub ChangeToInkMode(ByVal editingModeId As EditingModeId)
        ' Ensure the ink Mode based on the current option.
        Dim currentInkMode As InkCanvasEditingMode = _inkModeOptions(CurrentInkModeOption - OptionId.InkAndGesture)
        EnsureEditingMode(currentInkMode)

        ' Get the Drawing Attributes which is associated to the pen/highlighter setting.
        ' Then update the InkCanvas' DefaultDrawingAttributes property.
        Dim da As DrawingAttributes = DrawingAttributesCollection(editingModeId - editingModeId.Pen1)
        MyInkCanvas.DefaultDrawingAttributes = da

        ' Update the read-only DependencyProperties so that the UI can react to the change correspondingly.
        If editingModeId >= editingModeId.Pen1 And editingModeId <= editingModeId.Pen5 Then
            SetValue(SampleWindow.CurrentPenModePropertyKey, editingModeId)
        ElseIf editingModeId >= editingModeId.Highlighter1 And editingModeId <= editingModeId.Highlighter5 Then
            SetValue(SampleWindow.CurrentHighlighterModePropertyKey, editingModeId)
        End If
    End Sub

    ''' <summary>
    ''' Change to the ink mode with an eraser setting.
    ''' </summary>
    ''' <param name="modeId"></param>
    Private Sub ChangeToEraseMode(ByVal modeId As EditingModeId)
        Select Case modeId
            Case EditingModeId.PointEraser1 To EditingModeId.PointEraser3
                ' Change to the point erase mode
                Dim newSS As StylusShape = PointEraserShapeCollection(modeId - EditingModeId.PointEraser1)
                ChangeToPointErase(newSS)
            Case EditingModeId.StrokeEraser
                ' Change to the stroke erase mode
                EnsureEditingMode(InkCanvasEditingMode.EraseByStroke)
        End Select
        ' Update the readonly dependency property.
        SetValue(SampleWindow.CurrentEraserModePropertyKey, modeId)
    End Sub

    ''' <summary>
    ''' Change to the point erase mode
    ''' </summary>
    ''' <param name="ss"></param>
    Private Sub ChangeToPointErase(ByVal ss As StylusShape)
        ' Update InkCanvas EditingMode and the point eraser shape.
        EnsureEditingMode(InkCanvasEditingMode.EraseByPoint)
        MyInkCanvas.EraserShape = ss
        MyInkCanvas.RenderTransform = New MatrixTransform()
    End Sub

    ''' <summary>
    ''' Ensure the new InkCanvasEditingMode on the InkCanvas
    ''' </summary>
    ''' <param name="mode"></param>
    Private Sub EnsureEditingMode(ByVal mode As InkCanvasEditingMode)
        If MyInkCanvas.EditingMode <> mode Then
            MyInkCanvas.EditingMode = mode
        End If

        If (mode = InkCanvasEditingMode.EraseByPoint Or mode = InkCanvasEditingMode.EraseByStroke) _
            And MyInkCanvas.EditingModeInverted <> mode Then
            ' We update the EditingModeInverted if the new mode is one of the erase modes.
            MyInkCanvas.EditingModeInverted = mode
        End If
    End Sub

    ''' <summary>
    ''' Enabling undo/redo for changes to the strokes collection.
    ''' </summary>
    Sub Strokes_StrokesChanged(ByVal sender As Object, ByVal e As StrokeCollectionChangedEventArgs)

        Dim added As StrokeCollection = New StrokeCollection(e.Added)
        Dim removed As StrokeCollection = New StrokeCollection(e.Removed)

        Dim item As CommandItem = New StrokesAddedOrRemovedCI(_cmdStack, MyInkCanvas.EditingMode, added, removed, _editingOperationCount)
        _cmdStack.Enqueue(item)

    End Sub

    ''' <summary>
    ''' Enabling undo/redo for selections moving or resizing.
    ''' </summary>
    Sub MyInkCanvas_SelectionMovingOrResizing(ByVal sender As Object, ByVal e As InkCanvasSelectionEditingEventArgs)

        ' Enforce stroke bounds to positive territory.
        Dim newRect As Rect = e.NewRectangle
        Dim oldRect As Rect = e.OldRectangle

        If newRect.Top < 0D Or newRect.Left < 0D Then

            Dim newRect2 As Rect = _
                         New Rect(IIf(newRect.Left < 0D, 0D, newRect.Left), _
                                  IIf(newRect.Top < 0D, 0D, newRect.Top), _
                                  newRect.Width, _
                                  newRect.Height)

            e.NewRectangle = newRect2

        End If

        Dim item As CommandItem = New SelectionMovedOrResizedCI(_cmdStack, MyInkCanvas.GetSelectedStrokes(), newRect, oldRect, _editingOperationCount)
        _cmdStack.Enqueue(item)

    End Sub

    ''' <summary>
    ''' Track when mouse or stylus goes up to increment the editingOperationCount for undo / redo
    ''' </summary>
    Sub MyInkCanvas_mouseUp(ByVal sender As Object, ByVal e As MouseButtonEventArgs)
        _editingOperationCount += 1
    End Sub

    ''' <summary>
    ''' Undo the last edit.
    ''' </summary>
    Private Sub Undo(ByVal sender As Object, ByVal e As RoutedEventArgs)
        _cmdStack.Undo()
    End Sub

    ''' <summary>
    ''' Redo the last edit.
    ''' </summary>
    Private Sub Redo(ByVal sender As Object, ByVal e As RoutedEventArgs)
        _cmdStack.Redo()
    End Sub

#End Region

#Region "Private Properties"

    ''' <summary>
    ''' An Adorner which is used for displaying the gesture feeback.
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private ReadOnly Property GestureResultAdorner() As MessageAdorner
        Get
            ' Initialize the MessageAdorner if it isn't created yet.
            If _gestureResultAdorner Is Nothing Then
                _gestureResultAdorner = New MessageAdorner(MyInkCanvas)

                ' The MessageAdorner only needs to be rendered. Disable the HitTest on it.
                _gestureResultAdorner.IsHitTestVisible = False

                Dim al As AdornerLayer = AdornerLayer.GetAdornerLayer(MyInkCanvas)
                al.Add(_gestureResultAdorner)
            End If

            Return _gestureResultAdorner
        End Get
    End Property

#End Region

#Region "Private Fields"

    Private _cmdStack As CommandStack

    Private _predefinedDrawingAttributes As ObservableCollection(Of DrawingAttributes)
    Private _predefinedStylusShapes As ObservableCollection(Of StylusShape)
    Private _inkModeOptions As List(Of InkCanvasEditingMode)
    Private _gestureResultAdorner As MessageAdorner
    Private Const GestureInfoUrl As String = "http://msdn.microsoft.com/library/default.asp?url=/library/en-us/tpcsdk10/lonestar/whitepapers/designguide/tbconusingapplicationgesturesandtheirsemantics.asp"
    Private _editingOperationCount As Int32

#End Region

End Class
