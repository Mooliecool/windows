using ReactiveUI;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reactive;
using System.Reactive.Linq;
using System.Reactive.Subjects;
using System.Text;
using System.Threading.Tasks;

namespace BubbleBurst.ViewModel
{
    public class BubbleTaskManager : ReactiveObject
    {
        readonly BubbleMatrixViewModel _bubbleMatrix;
        readonly BubbleTaskFactory _taskFactory;
        readonly Stack<IEnumerable<BubbleTaskGroup>> _undoStack;

        /// <summary>
        /// Returns true if an undo operation can be performed at this time.
        /// </summary>
        internal bool CanUndo { get { return _undoStack.Any(); } }

        Subject<BubbleTaskGroup> _pendingTaskGroups = new Subject<BubbleTaskGroup>();
        public IObservable<BubbleTaskGroup> PendingTaskGroups { get { return _pendingTaskGroups.AsObservable(); } }

        internal BubbleTaskManager(BubbleMatrixViewModel bubbleMatrix)
        {
            _bubbleMatrix = bubbleMatrix;
            _taskFactory = new BubbleTaskFactory(bubbleMatrix);
            _undoStack = new Stack<IEnumerable<BubbleTaskGroup>>();
        }

        internal async void BurstBubbleGroup(BubbleViewModel[] bubblesInGroup)
        {
            var taskTypes = new[] { 
                BubbleTaskType.Burst,
                BubbleTaskType.MoveDown,
                BubbleTaskType.MoveRight,
            };

            var tasksToArchive = new List<BubbleTaskGroup>();

            _bubbleMatrix.IsIdle = false;

            foreach (var taskType in taskTypes)
            {
                var group = _taskFactory.CreateTaskGroup(taskType, bubblesInGroup);
                _pendingTaskGroups.OnNext(group);
                tasksToArchive.Add(group);
                // LastOrDefaultAsync used here because OnComplete Observable may be Completed 
                // before this line is executed.  This results in a runtime error if we don't guard against it.
                await group.OnComplete.LastOrDefaultAsync();
            }

            ArchiveTaskGroups(tasksToArchive);
            _bubbleMatrix.IsIdle = true;
            _bubbleMatrix.TryToEndGame();
        }

        public async void Undo()
        {
            var taskGroups = _undoStack.Pop().Reverse().ToList();
            this.raisePropertyChanged("CanUndo");

            _bubbleMatrix.IsIdle = false;

            foreach (var taskGroup in taskGroups)
            {
                var group = _taskFactory.CreateUndoTaskGroup(taskGroup);
                _pendingTaskGroups.OnNext(group);
                await group.OnComplete.LastOrDefaultAsync();
            }

            _bubbleMatrix.IsIdle = true;
        }

        void ArchiveTaskGroups(IEnumerable<BubbleTaskGroup> taskGroups)
        {
            _undoStack.Push(taskGroups);
            this.raisePropertyChanged("CanUndo");
        }

        internal void Reset()
        {
            _pendingTaskGroups.OnCompleted();
            _pendingTaskGroups = new Subject<BubbleTaskGroup>();
            this.raisePropertyChanged("PendingTaskGroups");

            _undoStack.Clear();
            this.raisePropertyChanged("CanUndo");
        }
    }
}
