' Interaction logic for SampleWindow.xaml
Imports System
Imports System.Collections
Imports System.Collections.Generic
Imports System.Collections.ObjectModel
Imports System.Text
Imports System.IO
Imports System.Xml
Imports System.Windows
Imports System.Windows.Ink
Imports System.Windows.Input
Imports System.Windows.Controls
Imports System.Windows.Media
Imports System.Windows.Shapes
Imports System.Windows.Markup
Imports System.Windows.Data
Imports System.Globalization

Namespace InkCanvasDragAndDropSample

    '@ <summary>
    '@ Interaction logic for SampleWindow.xaml
    '@ </summary>
    Partial Public Class SampleWindow
        Inherits Window

        Public Sub New()
            InitializeComponent()
        End Sub

#Region "Drag and Drop Handlers"

        '@ <summary>
        '@ OnPreviewMouseDown is called before InkCanvas sees the Down.  We use this 
        '@ handler to hit test any selection and initiate a drag and drop operation
        '@ if the user clicks on the selection to move it.
        '@ </summary>
        Sub OnPreviewMouseDown(ByVal sender As Object, ByVal e As MouseButtonEventArgs)

            Dim inkCanvasSender As InkCanvas = sender

            Dim downPosition As Point = e.GetPosition(inkCanvasSender)

            'see if we hit the selection, and not a grab handle
            If (inkCanvasSender.HitTestSelection(downPosition) = InkCanvasSelectionHitResult.Selection) Then

                'clone the selected strokes so we can transform them without affecting the actual selection
                Dim selectedStrokes As StrokeCollection
                selectedStrokes = inkCanvasSender.GetSelectedStrokes()

                Dim selectedElements As New List(Of UIElement)(inkCanvasSender.GetSelectedElements())
                Dim dataObject As New DataObject()

                'copy ink to the clipboard if we have any.  we do this even if there are no
                'strokes since clonedStrokes will be used by the Xaml clipboard code below

                Dim clonedStrokes As StrokeCollection
                clonedStrokes = selectedStrokes.Clone()

                If (clonedStrokes.Count > 0) Then

                    'translate the strokes relative to the down position
                    Dim translation As New Matrix()
                    translation.Translate(-downPosition.X, -downPosition.Y)
                    clonedStrokes.Transform(translation, False)

                    'save the strokes to a dataobject to use during the dragdrop operation
                    Dim ms As New MemoryStream()
                    clonedStrokes.Save(ms)
                    ms.Position = 0
                    dataObject.SetData(StrokeCollection.InkSerializedFormat, ms)
                    'we don't close the MemoryStream here, we'll do it in OnDrop
                End If


                'Now we're going to add Xaml to the dragdrop dataobject.  We'll create an 
                'InkCanvas and add any selected strokes and elements in the selection to it
                Dim inkCanvasForDragDrop As New InkCanvas()
                For Each childElement As UIElement In selectedElements
                    'we can't add elements in the selection to the InkCanvas that 
                    'represents selection (since they are already parented to the
                    'inkCanvas that has the selection) so we need to clone them.
                    'To clone each element, we need to convert it to Xaml and back again
                    Dim childXaml As String = XamlWriter.Save(childElement)
                    Dim clonedChildElement As UIElement = XamlReader.Load(New XmlTextReader(New StringReader(childXaml)))

                    'adjust top and left relative to the down position
                    Dim childLeft As Double = InkCanvas.GetLeft(clonedChildElement)
                    InkCanvas.SetLeft(clonedChildElement, childLeft - downPosition.X)

                    Dim childTop As Double = InkCanvas.GetTop(clonedChildElement)
                    InkCanvas.SetTop(clonedChildElement, childTop - downPosition.Y)

                    InkCanvas.GetLeft(clonedChildElement)
                    inkCanvasForDragDrop.Children.Add(clonedChildElement)

                Next

                'last, add the cloned strokes in case our drop location only supports Xaml
                'this preserves both the ink and the selected elements
                inkCanvasForDragDrop.Strokes = clonedStrokes

                'now copy the Xaml for the InkCanvas that represents selection to the clipboard
                Dim inkCanvasXaml As String = XamlWriter.Save(inkCanvasForDragDrop)
                dataObject.SetData(DataFormats.Xaml, inkCanvasXaml)

                'The call to DragDrop.DoDragDrop will block until the drag and drop operation is completed
                'once it does, 'effects' will have been updated by OnDragOver getting called
                'if we're moving the strokes and elements, we'll remove them.  If we're copying them
                '(the CTRL key is pressed) we won't remove them.
                Dim effects As DragDropEffects = _
                        DragDrop.DoDragDrop(inkCanvasSender, dataObject, DragDropEffects.Move Or DragDropEffects.Copy)

                If (effects = DragDropEffects.Move) Then

                    inkCanvasSender.Strokes.Remove(selectedStrokes)
                    For Each childElement As UIElement In selectedElements
                        inkCanvasSender.Children.Remove(childElement)
                    Next
                End If
            End If
        End Sub

        '@ <summary>
        '@ Called during the drag operation, we set e.Effects both to update
        '@ the cursor and to inform the OnPreviewMouseDown method if it should 
        '@ remove the strokes.
        '@ </summary>
        Sub OnDragOver(ByVal sender As Object, ByVal e As DragEventArgs)
            If (e.Data.GetDataPresent(StrokeCollection.InkSerializedFormat) Or _
                e.Data.GetDataPresent(DataFormats.Xaml)) Then
                SetDragDropEffects(e)
                e.Handled = True
            End If
        End Sub

        '@ <summary>
        '@ SetDragDropEffects changes the cursor based on key and mouse state
        '@ as well as what effects are allowed
        '@ </summary>
        Sub SetDragDropEffects(ByVal e As DragEventArgs)

            e.Effects = DragDropEffects.None

            If ((e.AllowedEffects And DragDropEffects.Move) <> 0) Then
                e.Effects = DragDropEffects.Move
            End If

            'if the CTRL key is down, treat this as a copy
            If ((e.KeyStates And DragDropKeyStates.ControlKey) <> 0 And _
                (e.AllowedEffects And DragDropEffects.Copy) <> 0) Then
                e.Effects = DragDropEffects.Copy
            End If
        End Sub

        '@ <summary>
        '@ OnQueryContinueDrag is called to see if we should continue the drag and drop
        '@ operation.  If the escape key is pressed, we cancel it.
        '@ </summary>
        Sub OnQueryContinueDrag(ByVal sender As Object, ByVal e As QueryContinueDragEventArgs)
            If (e.EscapePressed) Then
                e.Action = DragAction.Cancel
                e.Handled = True
            End If
        End Sub



        '@ <summary>
        '@ OnDrop - called when drag and drop contents are dropped on an InkCanvas
        '@ </summary>
        Sub OnDrop(ByVal sender As Object, ByVal e As DragEventArgs)

            Dim inkCanvasSender As InkCanvas = sender

            'see if ISF is present in the drag and drop IDataObject
            If (e.Data.GetDataPresent(StrokeCollection.InkSerializedFormat) Or _
                e.Data.GetDataPresent(DataFormats.Xaml)) Then

                SetDragDropEffects(e)

                If ((e.Effects = DragDropEffects.Move) Or (e.Effects = DragDropEffects.Copy)) Then

                    Dim dropPosition As Point = e.GetPosition(inkCanvasSender)

                    'after we drop, we need to select the elements and strokes that were
                    'copied into the inkCanvas
                    Dim elementsToSelect As New List(Of UIElement)

                    If (e.Data.GetDataPresent(DataFormats.Xaml)) Then

                        'paste Xaml
                        Dim xamlData As String = e.Data.GetData(DataFormats.Xaml)
                        If (Not String.IsNullOrEmpty(xamlData)) Then
                            Dim element As UIElement = _
                            XamlReader.Load(New XmlTextReader(New StringReader(xamlData)))

                            If (TypeOf element Is UIElement) Then

                                'check to see if this is an InkCanvas
                                If (TypeOf element Is InkCanvas) Then

                                    Dim inkCanvasFromDragDrop As InkCanvas = element
                                    'we assume this was put on the data object by us.
                                    'remove the children and add them to the drop inkCanvas.
                                    While (inkCanvasFromDragDrop.Children.Count > 0)

                                        Dim childElement As UIElement = inkCanvasFromDragDrop.Children(0)

                                        Dim childLeft As Double = InkCanvas.GetLeft(childElement)
                                        InkCanvas.SetLeft(childElement, childLeft + dropPosition.X)

                                        Dim childTop As Double = InkCanvas.GetTop(childElement)
                                        InkCanvas.SetTop(childElement, childTop + dropPosition.Y)

                                        inkCanvasFromDragDrop.Children.Remove(childElement)

                                        inkCanvasSender.Children.Add(childElement)
                                        elementsToSelect.Add(childElement)
                                    End While
                                Else

                                    'just add the element, it wasn't another InkCanvas that we
                                    'added to the dataobject as a container
                                    inkCanvasSender.Children.Add(element)
                                    elementsToSelect.Add(element)
                                End If
                            End If
                        End If
                    End If


                    'now check to see if we have ISF as well
                    Dim strokesToSelect As New StrokeCollection()

                    If (e.Data.GetDataPresent(StrokeCollection.InkSerializedFormat)) Then

                        'ISF is present
                        Dim ms As MemoryStream = e.Data.GetData(StrokeCollection.InkSerializedFormat)
                        Dim sc As New StrokeCollection(ms)
                        ms.Close()

                        'translate the strokes back from the origin to the current position
                        Dim translation As New Matrix()
                        translation.Translate(dropPosition.X, dropPosition.Y)
                        sc.Transform(translation, False)

                        'add the strokes from the IDataObject to the inkCanvas and select them.
                        inkCanvasSender.Strokes.Add(sc)
                        strokesToSelect.Add(sc)
                    End If

                    'now that we're done, we select
                    If (elementsToSelect.Count > 0 Or strokesToSelect.Count > 0) Then

                        inkCanvasSender.Select(strokesToSelect, elementsToSelect)
                        e.Handled = True
                    End If
                End If
            End If
        End Sub

#End Region

#Region "File Menu Handlers"

        '@ <summary>
        '@ clears strokes and elements from inkCanvas1 and inkCanvas2
        '@ </summary>
        Sub OnClearInkAndElements(ByVal sender As Object, ByVal e As RoutedEventArgs)
            inkCanvas1.Strokes.Clear()
            inkCanvas1.Children.Clear()

            inkCanvas2.Strokes.Clear()
            inkCanvas2.Children.Clear()
        End Sub

        '@ <summary>
        '@ closes the application
        '@ </summary>
        Sub OnExit(ByVal sender As Object, ByVal e As RoutedEventArgs)
            Me.Close()
        End Sub

#End Region

#Region "InkCanvas Menu Handlers"

        '@ <summary>
        '@ OnInkCanvasSelectChecked
        '@ </summary>
        Sub OnInkCanvasSelectChecked(ByVal sender As Object, ByVal e As RoutedEventArgs)
            Dim menuItem As MenuItem = sender
            If (menuItem.Tag = "inkCanvas1") Then
                inkCanvas1.EditingMode = InkCanvasEditingMode.Select
                miInkCanvas1EditingModeSelect.IsChecked = True
            Else
                inkCanvas2.EditingMode = InkCanvasEditingMode.Select
                miInkCanvas2EditingModeSelect.IsChecked = True
            End If
        End Sub

        '@ <summary>
        '@ OnInkCanvasInkChecked
        '@ </summary>
        Sub OnInkCanvasInkChecked(ByVal sender As Object, ByVal e As RoutedEventArgs)
            Dim menuItem As MenuItem = sender
            If (menuItem.Tag = "inkCanvas1") Then
                inkCanvas1.EditingMode = InkCanvasEditingMode.Ink
                miInkCanvas1EditingModeInk.IsChecked = True
            Else
                inkCanvas2.EditingMode = InkCanvasEditingMode.Ink
                miInkCanvas2EditingModeInk.IsChecked = True
            End If
        End Sub


#End Region

#Region "Insert Menu Handlers"

        '@ <summary>
        '@ inserts a new TextBox into InkCanvas
        '@ </summary>
        Sub OnInsertTextBox(ByVal sender As Object, ByVal e As RoutedEventArgs)
            Dim inkCanvas As InkCanvas
            Dim menuItem As MenuItem = sender
            If (menuItem.Tag = "inkCanvas1") Then
                inkCanvas = inkCanvas1
            Else
                inkCanvas = inkCanvas2
            End If

            Dim textBoxToInsert As New TextBox()
            textBoxToInsert.Text = "New TextBox"
            textBoxToInsert.AcceptsReturn = True
            textBoxToInsert.SetValue(inkCanvas.TopProperty, inkCanvas.ActualHeight / 2D)
            textBoxToInsert.SetValue(inkCanvas.LeftProperty, inkCanvas.ActualWidth / 2D)

            inkCanvas.Children.Add(textBoxToInsert)
        End Sub

        '@ <summary>
        '@ inserts a new Label into InkCanvas
        '@ </summary>
        Sub OnInsertLabel(ByVal sender As Object, ByVal e As RoutedEventArgs)
            Dim inkCanvas As InkCanvas
            Dim menuItem As MenuItem = sender
            If (menuItem.Tag = "inkCanvas1") Then
                inkCanvas = inkCanvas1
            Else
                inkCanvas = inkCanvas2
            End If

            Dim labelToInsert As New Label()
            labelToInsert.Content = "New Label"
            labelToInsert.Background = Brushes.LightBlue
            labelToInsert.SetValue(inkCanvas.TopProperty, inkCanvas.ActualHeight / 2D)
            labelToInsert.SetValue(inkCanvas.LeftProperty, inkCanvas.ActualWidth / 2D)

            inkCanvas.Children.Add(labelToInsert)

        End Sub

        '@ <summary>
        '@ inserts a new Ellipse shape into InkCanvas
        '@ </summary>
        Sub OnInsertShape(ByVal sender As Object, ByVal e As RoutedEventArgs)

            Dim inkCanvas As InkCanvas
            Dim menuItem As MenuItem = sender
            If (menuItem.Tag = "inkCanvas1") Then
                inkCanvas = inkCanvas1
            Else
                inkCanvas = inkCanvas2
            End If

            Dim ellipseToInsert As New Ellipse()
            ellipseToInsert.Width = 150D
            ellipseToInsert.Height = 100D
            ellipseToInsert.Fill = New LinearGradientBrush(Colors.Goldenrod, Colors.HotPink, 20D)
            ellipseToInsert.SetValue(inkCanvas.TopProperty, inkCanvas.ActualHeight / 2D)
            ellipseToInsert.SetValue(inkCanvas.LeftProperty, inkCanvas.ActualWidth / 2D)

            inkCanvas.Children.Add(ellipseToInsert)
        End Sub

#End Region

#Region "Enable / Disable Drag Drop Handlers"

        '@ <summary>
        '@ Enables drag and drop
        '@ </summary>
        Sub OnDragAndDropEnabled(ByVal sender As Object, ByVal e As RoutedEventArgs)

            'enable drag-drop
            inkCanvas1.AllowDrop = True
            AddHandler inkCanvas1.QueryContinueDrag, AddressOf OnQueryContinueDrag
            AddHandler inkCanvas1.Drop, AddressOf OnDrop
            AddHandler inkCanvas1.DragOver, AddressOf OnDragOver
            AddHandler inkCanvas1.PreviewMouseDown, AddressOf OnPreviewMouseDown

            inkCanvas2.AllowDrop = True
            AddHandler inkCanvas2.QueryContinueDrag, AddressOf OnQueryContinueDrag
            AddHandler inkCanvas2.Drop, AddressOf OnDrop
            AddHandler inkCanvas2.DragOver, AddressOf OnDragOver
            AddHandler inkCanvas2.PreviewMouseDown, AddressOf OnPreviewMouseDown

        End Sub


        '@ <summary>
        '@ Disables drag and drop
        '@ </summary>
        Sub OnDragAndDropDisabled(ByVal sender As Object, ByVal e As RoutedEventArgs)

            'enable drag-drop
            inkCanvas1.AllowDrop = False
            RemoveHandler inkCanvas1.QueryContinueDrag, AddressOf OnQueryContinueDrag
            RemoveHandler inkCanvas1.Drop, AddressOf OnDrop
            RemoveHandler inkCanvas1.DragOver, AddressOf OnDragOver
            RemoveHandler inkCanvas1.PreviewMouseDown, AddressOf OnPreviewMouseDown

            inkCanvas2.AllowDrop = True
            RemoveHandler inkCanvas2.QueryContinueDrag, AddressOf OnQueryContinueDrag
            RemoveHandler inkCanvas2.Drop, AddressOf OnDrop
            RemoveHandler inkCanvas2.DragOver, AddressOf OnDragOver
            RemoveHandler inkCanvas2.PreviewMouseDown, AddressOf OnPreviewMouseDown

        End Sub
#End Region
    End Class
End Namespace







