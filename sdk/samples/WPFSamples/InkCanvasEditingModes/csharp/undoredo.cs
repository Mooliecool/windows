using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Ink;

namespace InkCanvasEditingModes
{
    sealed class CommandStack
    {
        /// <summary>
        /// Initialization.
        /// </summary>
        /// <param name="strokes"></param>
        public CommandStack(StrokeCollection strokes)
        {
            if (strokes == null)
            {
                throw new ArgumentNullException("strokes");
            }
            _strokeCollection = strokes;
            _undoStack = new Stack<CommandItem>();
            _redoStack = new Stack<CommandItem>();
            _disableChangeTracking = false;
        }

        /// <summary>
        /// StrokeCollection to track changes for
        /// </summary>
        public StrokeCollection StrokeCollection
        {
            get
            {
                return _strokeCollection;
            }
        }

        /// <summary>
        /// Only undo if there are more items in the stack to step back into.
        /// </summary>
        public bool CanUndo
        {
            get { return (_undoStack.Count > 0); }
        }

        /// <summary>
        /// Only undo if one or more steps back in the stack.
        /// </summary>
        public bool CanRedo
        {
            get { return (_redoStack.Count > 0); }
        }

        /// <summary>
        /// Add an item to the top of the command stack
        /// </summary>
        public void Undo()
        {
            if (!CanUndo) throw new InvalidOperationException("No actions to undo");

            CommandItem item = _undoStack.Pop();

            // Invoke the undo operation, with change-tracking temporarily suspended.
            _disableChangeTracking = true;
            try
            {
                item.Undo();
            }
            finally
            { 
                _disableChangeTracking = false; 
            }
            
            //place this item on the redo stack
            _redoStack.Push(item);
        }

        /// <summary>
        /// Take the top item off the command stack.
        /// </summary>
        public void Redo()
        {
            if (!CanRedo) throw new InvalidOperationException();

            CommandItem item = _redoStack.Pop();

            // Invoke the redo operation, with change-tracking temporarily suspended.
            _disableChangeTracking = true;
            try
            {
                item.Redo();
            }
            finally
            {
                _disableChangeTracking = false;
            }

            //place this item on the undo stack
            _undoStack.Push(item);
        }

        /// <summary>
        /// Add a command item to the stack.
        /// </summary>
        /// <param name="item"></param>
        public void Enqueue(CommandItem item)
        {
            if (item == null)
            {
                throw new ArgumentNullException("item");
            }

            // Ensure we don't enqueue new items if we're being changed programmatically.
            if (_disableChangeTracking)
            {
                return;
            }

            // Check to see if this new item can be merged with previous.
            bool merged = false;
            if (_undoStack.Count > 0)
            {
                CommandItem prev = _undoStack.Peek();
                merged = prev.Merge(item);
            }

            // If not, append the new command item
            if (!merged)
            {
                _undoStack.Push(item);
            }

            //clear the redo stack
            if (_redoStack.Count > 0)
            {
                _redoStack.Clear();
            }
        }

        /// <summary>
        /// Implementation
        /// </summary>
        private StrokeCollection _strokeCollection;

        private Stack<CommandItem> _undoStack;
        private Stack<CommandItem> _redoStack;


        bool _disableChangeTracking; // reentrancy guard: disables tracking of programmatic changes 
        // (eg, in response to undo/redo ops)
    }

    /// <summary>
    /// Derive from this class for every undoable/redoable operation you wish to support.
    /// </summary>
    abstract class CommandItem
    {

        // Interface
        public abstract void Undo();
        public abstract void Redo();


        // Allows multiple subsequent commands of the same type to roll-up into one 
        // logical undoable/redoable command -- return false if newitem is incompatable.
        public abstract bool Merge(CommandItem newitem);

        // Implementation
        protected CommandStack _commandStack;

        protected CommandItem(CommandStack commandStack)
        {
            _commandStack = commandStack;
        }
    }

    /// <summary>
    /// This operation covers collecting new strokes, stroke-erase, and point-erase.
    /// </summary>
    class StrokesAddedOrRemovedCI : CommandItem
    {
        InkCanvasEditingMode _editingMode;
        StrokeCollection _added, _removed;
        int _editingOperationCount;

        public StrokesAddedOrRemovedCI(CommandStack commandStack, InkCanvasEditingMode editingMode, StrokeCollection added, StrokeCollection removed, int editingOperationCount)
            : base(commandStack)
        {
            _editingMode = editingMode;

            _added = added;
            _removed = removed;

            _editingOperationCount = editingOperationCount;
        }

        public override void Undo()
        {
            _commandStack.StrokeCollection.Remove(_added);
            _commandStack.StrokeCollection.Add(_removed);
        }

        public override void Redo()
        {
            _commandStack.StrokeCollection.Add(_added);
            _commandStack.StrokeCollection.Remove(_removed);
        }

        public override bool Merge(CommandItem newitem)
        {
            StrokesAddedOrRemovedCI newitemx = newitem as StrokesAddedOrRemovedCI;

            if (newitemx == null || 
                newitemx._editingMode != _editingMode ||
                newitemx._editingOperationCount != _editingOperationCount)
            {
                return false;
            }

            // We only implement merging for repeated point-erase operations.
            if (_editingMode != InkCanvasEditingMode.EraseByPoint) return false;
            if (newitemx._editingMode != InkCanvasEditingMode.EraseByPoint) return false;

            // Note: possible for point-erase to have hit intersection of >1 strokes!
            // For each newly hit stroke, merge results into this command item.
            foreach (Stroke doomed in newitemx._removed)
            {
                if (_added.Contains(doomed))
                {
                    _added.Remove(doomed);
                }
                else
                {
                    _removed.Add(doomed);
                }
            }
            _added.Add(newitemx._added);

            return true;
        }
    }

    /// <summary>
    /// This operation covers move and resize operations.
    /// </summary>
    class SelectionMovedOrResizedCI : CommandItem
    {
        StrokeCollection _selection;
        Rect _newrect, _oldrect;
        int _editingOperationCount;

        public SelectionMovedOrResizedCI(CommandStack commandStack, StrokeCollection selection, Rect newrect, Rect oldrect, int editingOperationCount)
            : base(commandStack)
        {
            _selection = selection;
            _newrect = newrect;
            _oldrect = oldrect;
            _editingOperationCount = editingOperationCount;
        }

        public override void Undo()
        {
            Matrix m = GetTransformFromRectToRect(_newrect, _oldrect);
            _selection.Transform(m, false);
        }

        public override void Redo()
        {
            Matrix m = GetTransformFromRectToRect(_oldrect, _newrect);
            _selection.Transform(m, false);
        }

        public override bool Merge(CommandItem newitem)
        {
            SelectionMovedOrResizedCI newitemx = newitem as SelectionMovedOrResizedCI;

            // Ensure items are of the same type.
            if (newitemx == null || 
                newitemx._editingOperationCount != _editingOperationCount ||
                !StrokeCollectionsAreEqual(newitemx._selection, _selection))
            {
                return false;
            }

            // Keep former oldrect, latter newrect.
            _newrect = newitemx._newrect;

            return true;
        }

        static Matrix GetTransformFromRectToRect(Rect src, Rect dst)
        {
            Matrix m = Matrix.Identity;
            m.Translate(-src.X, -src.Y);
            m.Scale(dst.Width / src.Width, dst.Height / src.Height);
            m.Translate(+dst.X, +dst.Y);
            return m;
        }

        static bool StrokeCollectionsAreEqual(StrokeCollection a, StrokeCollection b)
        {
            if (a == null && b == null) return true;
            if (a == null || b == null) return false;
            if (a.Count != b.Count) return false;

            for (int i = 0; i < a.Count; ++i)
                if (a[i] != b[i]) return false;

            return true;
        }
    }
}
