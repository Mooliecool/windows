Imports System.Windows.Ink

NotInheritable Class CommandStack

    ''' <summary>
    ''' Initialization.
    ''' </summary>
    ''' <param name="strokes"></param>
    Public Sub New(ByVal strokes As StrokeCollection)

        If strokes Is Nothing Then Throw New ArgumentNullException("strokes")

        _strokeCollection = strokes
        _undoStack = New Stack(Of CommandItem)()
        _redoStack = New Stack(Of CommandItem)()
        _disableChangeTracking = False
    End Sub

    ''' <summary>
    ''' StrokeCollection to track changes for
    ''' </summary>
    Public ReadOnly Property StrokeCollection() As StrokeCollection
        Get
            Return _strokeCollection
        End Get
    End Property

    ''' <summary>
    ''' Only undo if there are more items in the stack to step back into.
    ''' </summary>
    Public ReadOnly Property CanUndo() As Boolean
        Get
            Return (_undoStack.Count > 0)
        End Get
    End Property

    ''' <summary>
    ''' Only undo if one or more steps back in the stack.
    ''' </summary>
    Public ReadOnly Property CanRedo() As Boolean
        Get
            Return (_redoStack.Count > 0)
        End Get
    End Property

    ''' <summary>
    ''' Add an item to the top of the command stack
    ''' </summary>
    Public Sub Undo()

        If Not CanUndo() Then Throw New InvalidOperationException("No actions to undo")

        Dim item As CommandItem = _undoStack.Pop()

        ' Invoke the undo operation, with change-tracking temporarily suspended.
        _disableChangeTracking = True

        Try
            item.Undo()

        Finally
            _disableChangeTracking = False

        End Try

        ' place this item on the redo stack
        _redoStack.Push(item)

    End Sub

    ''' <summary>
    ''' Take the top item off the command stack.
    ''' </summary>
    Public Sub Redo()

        If Not CanRedo() Then Throw New InvalidOperationException()

        Dim item As CommandItem = _redoStack.Pop()

        ' Invoke the redo operation, with change-tracking temporarily suspended.
        _disableChangeTracking = True

        Try
            item.Redo()

        Finally
            _disableChangeTracking = False

        End Try

        ' place this item on the undo stack
        _undoStack.Push(item)

    End Sub

    ''' <summary>
    ''' Add a command item to the stack.
    ''' </summary>
    ''' <param name="item"></param>
    Public Sub Enqueue(ByVal item As CommandItem)

        If item Is Nothing Then Throw New ArgumentNullException("item")

        ' Ensure we don't enqueue new items if we're being changed programmatically.
        If _disableChangeTracking Then Return

        ' Check to see if this new item can be merged with previous.
        Dim merged As Boolean = False
        If _undoStack.Count > 0 Then

            Dim prev As CommandItem = _undoStack.Peek()
            merged = prev.Merge(item)

        End If

        ' If not, append the new command item
        If Not merged Then _undoStack.Push(item)

        ' clear the redo stack
        If _redoStack.Count > 0 Then _redoStack.Clear()

    End Sub

    ''' <summary>
    ''' Implementation
    ''' </summary>
    Private _strokeCollection As StrokeCollection

    Private _undoStack As Stack(Of CommandItem)
    Private _redoStack As Stack(Of CommandItem)

    Dim _disableChangeTracking As Boolean ' reentrancy guard: disables tracking of programmatic changes 
    ' (eg, in response to undo/redo ops)

End Class

''' <summary>
''' Derive from this class for every undoable/redoable operation you wish to support.
''' </summary>
MustInherit Class CommandItem

    ' Interface
    Public MustOverride Sub Undo()
    Public MustOverride Sub Redo()

    ' Allows multiple subsequent commands of the same type to roll-up into one 
    ' logical undoable/redoable command -- return false if newitem is incompatable.
    Public MustOverride Function Merge(ByVal newitem As CommandItem) As Boolean

    ' Implementation
    Protected _commandStack As CommandStack

    Protected Sub New(ByVal commandStack As CommandStack)

        _commandStack = commandStack

    End Sub
End Class

''' <summary>
''' This operation covers collecting new strokes, stroke-erase, and point-erase.
''' </summary>
Class StrokesAddedOrRemovedCI
    Inherits CommandItem

    Dim _editingMode As InkCanvasEditingMode
    Dim _added, _removed As StrokeCollection
    Dim _editingOperationCount As Int32

    Public Sub New(ByVal commandStack As CommandStack, ByVal editingMode As InkCanvasEditingMode, ByVal added As StrokeCollection, ByVal removed As StrokeCollection, ByVal editingOperationCount As Int32)
        MyBase.New(commandStack)

        _editingMode = editingMode

        _added = added
        _removed = removed

        _editingOperationCount = editingOperationCount
    End Sub


    Public Overrides Sub Undo()

        _commandStack.StrokeCollection.Remove(_added)
        _commandStack.StrokeCollection.Add(_removed)

    End Sub

    Public Overrides Sub Redo()

        _commandStack.StrokeCollection.Add(_added)
        _commandStack.StrokeCollection.Remove(_removed)

    End Sub

    Public Overrides Function Merge(ByVal newitem As CommandItem) As Boolean

        Dim newitemx As StrokesAddedOrRemovedCI = TryCast(newitem, StrokesAddedOrRemovedCI)

        If newitemx Is Nothing OrElse _
           Not newitemx._editingMode = _editingMode OrElse _
           Not newitemx._editingOperationCount = _editingOperationCount Then

            Return False

        End If

        ' We only implement merging for repeated point-erase operations.
        If Not _editingMode = InkCanvasEditingMode.EraseByPoint Then Return False
        If Not newitemx._editingMode = InkCanvasEditingMode.EraseByPoint Then Return False

        ' Note: possible for point-erase to have hit intersection of >1 strokes!
        ' For each newly hit stroke, merge results into this command item.
        For Each doomed As Stroke In newitemx._removed

            If _added.Contains(doomed) Then
                _added.Remove(doomed)
            Else
                _removed.Add(doomed)
            End If
        Next

        _added.Add(newitemx._added)

        Return True

    End Function
End Class

''' <summary>
''' This operation covers move and resize operations.
''' </summary>
Class SelectionMovedOrResizedCI
    Inherits CommandItem

    Dim _selection As StrokeCollection
    Dim _newrect, _oldrect As Rect
    Dim _editingOperationCount As Int32

    Public Sub New(ByVal commandStack As CommandStack, ByVal selection As StrokeCollection, ByVal newrect As Rect, ByVal oldrect As Rect, ByVal editingOperationCount As Int32)
        MyBase.New(commandStack)

        _selection = selection
        _newrect = newrect
        _oldrect = oldrect
        _editingOperationCount = editingOperationCount

    End Sub

    Public Overrides Sub Undo()

        Dim m As Matrix = GetTransformFromRectToRect(_newrect, _oldrect)
        _selection.Transform(m, False)

    End Sub

    Public Overrides Sub Redo()

        Dim m As Matrix = GetTransformFromRectToRect(_oldrect, _newrect)
        _selection.Transform(m, False)

    End Sub

    Public Overrides Function Merge(ByVal newitem As CommandItem) As Boolean

        Dim newitemx As SelectionMovedOrResizedCI = TryCast(newitem, SelectionMovedOrResizedCI)

        ' Ensure items are of the same type.
        If newitemx Is Nothing OrElse _
           Not newitemx._editingOperationCount = _editingOperationCount OrElse _
           Not StrokeCollectionsAreEqual(newitemx._selection, _selection) Then

            Return False

        End If

        ' Keep former oldrect, latter newrect.
        _newrect = newitemx._newrect

        Return True

    End Function

    Shared Function GetTransformFromRectToRect(ByVal src As Rect, ByVal dst As Rect) As Matrix

        Dim m As Matrix = Matrix.Identity
        m.Translate(-src.X, -src.Y)
        m.Scale(dst.Width / src.Width, dst.Height / src.Height)
        m.Translate(+dst.X, +dst.Y)
        Return m

    End Function

    Shared Function StrokeCollectionsAreEqual(ByVal a As StrokeCollection, ByVal b As StrokeCollection) As Boolean

        If a Is Nothing AndAlso b Is Nothing Then Return True
        If a Is Nothing OrElse b Is Nothing Then Return False
        If Not a.Count = b.Count Then Return False

        For i As Int32 = 0 To (a.Count - 1)
            If Not a(i) Is b(i) Then Return False
        Next

        Return True

    End Function
End Class