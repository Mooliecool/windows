using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Animation;
using BubbleBurst.ViewModel;
using ReactiveUI;
using System.Reactive;
using System.Reactive.Subjects;
using System.Reactive.Linq;

namespace BubbleBurst.View
{
    /// <summary>
    /// Displays a fixed-size grid of bubbles.
    /// </summary>
    public partial class BubbleMatrixView : ItemsControl, IViewFor<BubbleMatrixViewModel>
    {
        public BubbleMatrixViewModel ViewModel { get; set; }
        object IViewFor.ViewModel { get { return ViewModel; } set { ViewModel = (BubbleMatrixViewModel)value; } }

        BubbleCanvas _bubbleCanvas;
        BubbleTaskStoryboardFactory _storyboardFactory;

        public BubbleMatrixView()
        {
            InitializeComponent();

            var dataContextChanged = Observable.FromEventPattern<DependencyPropertyChangedEventHandler, DependencyPropertyChangedEventArgs>(
                h => base.DataContextChanged += h,
                h => base.DataContextChanged -= h);

            dataContextChanged.Subscribe(ev => { ViewModel = base.DataContext as BubbleMatrixViewModel; });

            // Hook the event raised after a bubble group bursts and a series
            // of animations need to run to advance the game state.
            var taskGroups = this.WhenAnyObservable(x => x.ViewModel.TaskManager.PendingTaskGroups);
            taskGroups.Subscribe(tg => this.ProcessTaskGroup(tg));
        }

        void HandleBubbleCanvasLoaded(object sender, RoutedEventArgs e)
        {
            // Store a reference to the panel that contains the bubbles.
            _bubbleCanvas = sender as BubbleCanvas;

            // Create the factory that makes Storyboards used after a bubble group bursts.
            _storyboardFactory = new BubbleTaskStoryboardFactory(_bubbleCanvas);

            // Ready to Start the Game
            ViewModel.StartNewGame();
        }

        void ProcessTaskGroup(BubbleTaskGroup taskGroup)
        {
            var storyboard = _storyboardFactory.CreateStoryboard(taskGroup);
            this.PerformTask(taskGroup, storyboard);
        }

        void PerformTask(BubbleTaskGroup taskGroup, Storyboard storyboard)
        {
            if (storyboard != null)
            {
                // There are some bubbles that need to be animated, so we must
                // wait until the Storyboard finishs before completing the task.
                storyboard.Completed += delegate { taskGroup.RaiseComplete(); };

                // Freeze the Storyboard to improve perf.
                storyboard.Freeze();

                // Start animating the bubbles associated with the task.
                storyboard.Begin(this);
            }
            else
            {
                // There are no bubbles associated with this task,
                // so immediately move to the task completion phase.
                taskGroup.RaiseComplete();
            }
        }
    }
}