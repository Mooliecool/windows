using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using BubbleBurst.ViewModel.Internal;
using ReactiveUI;
using System.Reactive;
using System.Reactive.Linq;
using System.Reactive.Subjects;

namespace BubbleBurst.ViewModel
{
    /// <summary>
    /// Represents the matrix of bubbles and contains 
    /// logic that drives a game to completion.
    /// </summary>
    public class BubbleMatrixViewModel : ReactiveObject
    {
        readonly BubbleGroupHelper _bubbleGroupHelper;
        readonly Stack<int> _bubbleGroupSizeStack;

        // The Matrix
        public int RowCount { get; private set; }
        public int ColumnCount { get; private set; }

        // Bubbles
        readonly IReactiveList<BubbleViewModel> _bubblesInternal;
        public IReactiveDerivedList<BubbleViewModel> Bubbles { get; private set; }

        BubbleViewModel _focusedBubble;
        public BubbleViewModel FocusedBubble
        {
            get { return _focusedBubble; }
            internal set { this.RaiseAndSetIfChanged(ref _focusedBubble, value); }
        }

        IEnumerable<BubbleViewModel> ActiveBubbleGroup { get { return Bubbles.Where(b => b.IsActive); } }

        internal int MostBubblesPoppedAtOnce { get { return _bubbleGroupSizeStack.Max(); } }

        // Tasks
        bool _isIdle;
        /// <summary>
        /// Represents whether the application is currently processing something that
        /// requires the user interface to ignore user interactions until it finishes.
        /// </summary>
        public bool IsIdle
        {
            get { return _isIdle; }
            internal set { this.RaiseAndSetIfChanged(ref _isIdle, value); }
        }

        BubbleTaskManager _taskManager;
        /// <summary>
        /// Represents whether the application is currently processing something that
        /// requires the user interface to ignore user interactions until it finishes.
        /// </summary>
        public BubbleTaskManager TaskManager
        {
            get { return _taskManager; }
            internal set { this.RaiseAndSetIfChanged(ref _taskManager, value); }
        }

        public IReactiveCommand UndoCommand { get; private set; }

        /// <summary>
        /// Raised when there are no more bubble groups left to burst.
        /// </summary>
        readonly Subject<Unit> _gameEnded = new Subject<Unit>();
        public IObservable<Unit> GameEnded { get { return _gameEnded.AsObservable(); } }

        internal BubbleMatrixViewModel(int rowCount, int columnCount)
        {
            RowCount = rowCount;
            ColumnCount = columnCount;

            _bubblesInternal = new ReactiveList<BubbleViewModel>();
            this.Bubbles = _bubblesInternal.CreateDerivedCollection(x => x);

            this.TaskManager = new BubbleTaskManager(this);

            _bubbleGroupHelper = new BubbleGroupHelper(this.Bubbles);

            _bubbleGroupSizeStack = new Stack<int>();

            _isIdle = true;

            var canUndo = this.WhenAny(x => x.IsIdle, x => x.TaskManager.CanUndo, (i, cu) => i.Value && cu.Value);
            UndoCommand = new ReactiveCommand(canUndo);
            UndoCommand.Subscribe(x => Undo());

            this.WhenAnyObservable(t => t.TaskManager.PendingTaskGroups).Subscribe(tg => ExecuteTaskGroup(tg));

            this.WhenAnyValue(t => t.FocusedBubble).Subscribe(b => ActivateBubbleGroup(b));
        }

        private void ExecuteTaskGroup(BubbleTaskGroup taskGroup)
        {
            foreach (var task in taskGroup)
            {
                var bubble = task.Bubble;
                var moveDistance = task.MoveDistance;

                switch (taskGroup.TaskType)
                {
                    case BubbleTaskType.Burst:
                        RemoveBubble(bubble);
                        break;
                    case BubbleTaskType.Add:
                        AddBubble(bubble);
                        break;
                    case BubbleTaskType.MoveDown:
                        bubble.MoveTo(bubble.Row + moveDistance, bubble.Column);
                        break;
                    case BubbleTaskType.MoveRight:
                        bubble.MoveTo(bubble.Row, bubble.Column + moveDistance);
                        break;
                }
            }
        }

        public void StartNewGame()
        {
            // Reset game state.
            this.IsIdle = true;
            _bubbleGroupHelper.Reset();
            _bubbleGroupSizeStack.Clear();
            this.TaskManager.Reset();

            InitializeBubbles();
        }

        void InitializeBubbles()
        {
            // Create a new matrix of bubbles.
            _bubblesInternal.Clear();
            ((ReactiveList<BubbleViewModel>)_bubblesInternal).AddRange(
                from row in Enumerable.Range(0, RowCount)
                from col in Enumerable.Range(0, ColumnCount)
                select new BubbleViewModel(this, row, col));
        }

        internal void AddBubble(BubbleViewModel bubble)
        {
            if (bubble == null)
                throw new ArgumentNullException("bubble");

            _bubblesInternal.Add(bubble);
        }

        internal void BurstBubbleGroup()
        {
            if (!this.IsIdle)
                throw new InvalidOperationException("Cannot burst a bubble group when not idle.");

            var bubblesInGroup = ActiveBubbleGroup.ToArray();
            if (!bubblesInGroup.Any())
                return;

            _bubbleGroupSizeStack.Push(bubblesInGroup.Length);
            
            this.TaskManager.BurstBubbleGroup(bubblesInGroup);
        }

        internal void RemoveBubble(BubbleViewModel bubble)
        {
            if (bubble == null)
                throw new ArgumentNullException("bubble");

            _bubblesInternal.Remove(bubble);
        }

        internal void TryToEndGame()
        {
            bool groupExists = _bubbleGroupHelper.AnyGroupsExist();
            if (!groupExists)
            {
                this.IsIdle = false;
                _gameEnded.OnNext(Unit.Default);
            }
        }

        internal void ActivateBubbleGroup(BubbleViewModel bubble)
        {
            _bubbleGroupHelper.Deactivate();
            if (bubble != null)
            {
                _bubbleGroupHelper.FindBubbleGroup(bubble).Activate();
            }
        }

        /// <summary>
        /// Reverts the game state to how it was before 
        /// the most recent group of bubbles was burst.
        /// </summary>
        void Undo()
        {
            // Throw away the last bubble group size, 
            // since that burst is about to be undone.
            _bubbleGroupSizeStack.Pop();

            this.TaskManager.Undo();
        }
    }
}