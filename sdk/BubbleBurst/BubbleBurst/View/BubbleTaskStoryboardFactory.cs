using BubbleBurst.ViewModel;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Animation;
using Thriple.Easing;

namespace BubbleBurst.View
{
    public class BubbleTaskStoryboardFactory
    {
        private BubbleCanvas _bubbleCanvas;

        public BubbleTaskStoryboardFactory(BubbleCanvas _bubbleCanvas)
        {
            this._bubbleCanvas = _bubbleCanvas;
        }

        internal Storyboard CreateStoryboard(BubbleTaskGroup taskGroup)
        {
            if (taskGroup.Count() == 0)
            {
                return null;
            }

            int millisecondsPerUnit;
            Func<ContentPresenter, double> getTo;
            DependencyProperty animatedProperty;
            IEnumerable<BubbleViewModel> bubbles;

            this.GetStoryboardCreationData(
                taskGroup,
                out millisecondsPerUnit,
                out getTo,
                out animatedProperty,
                out bubbles);

            var storyboard = new Storyboard();
            var targetProperty = new PropertyPath(animatedProperty);
            var beginTime = TimeSpan.FromMilliseconds(0);
            var beginTimeIncrement = TimeSpan.FromMilliseconds(millisecondsPerUnit / bubbles.Count());

            foreach (ContentPresenter presenter in this.GetBubblePresenters(bubbles))
            {
                var bubble = presenter.DataContext as BubbleViewModel;
                var anim = new EasingDoubleAnimation
                {
                    BeginTime = beginTime,
                    Duration = CalculateDuration(taskGroup, bubble, millisecondsPerUnit),
                    Equation = EasingEquation.CubicEaseIn,
                    To = getTo(presenter),
                };

                Storyboard.SetTarget(anim, presenter);
                Storyboard.SetTargetProperty(anim, targetProperty);

                if (IsTaskStaggered(taskGroup.TaskType))
                {
                    beginTime = beginTime.Add(beginTimeIncrement);
                }

                storyboard.Children.Add(anim);
            }

            return storyboard;
        }

        void GetStoryboardCreationData(
          BubbleTaskGroup taskGroup,
          out int millisecondsPerUnit,
          out Func<ContentPresenter, double> getTo,
          out DependencyProperty animatedProperty,
          out IEnumerable<BubbleViewModel> bubbles)
        {
            switch (taskGroup.TaskType)
            {
                case BubbleTaskType.Burst:
                    millisecondsPerUnit = 100;
                    getTo = cp => 0.0;
                    animatedProperty = UIElement.OpacityProperty;
                    bubbles = taskGroup.Select(t => t.Bubble);
                    break;

                case BubbleTaskType.Add:
                    millisecondsPerUnit = 100;
                    getTo = cp => 1.0;
                    animatedProperty = UIElement.OpacityProperty;
                    bubbles = taskGroup.Select(t => t.Bubble);
                    break;

                case BubbleTaskType.MoveDown:
                    millisecondsPerUnit = 50;
                    getTo = _bubbleCanvas.CalculateTop;
                    animatedProperty = Canvas.TopProperty;

                    // Sort the bubbles to ensure that the columns move 
                    // in sync with each other in an appealing way.
                    bubbles = taskGroup.Select(t => t.Bubble)
                        .OrderByDescending(b => b.Row)
                        .ThenByDescending(b => b.Column);
                    break;

                case BubbleTaskType.MoveRight:
                    millisecondsPerUnit = 50;
                    getTo = _bubbleCanvas.CalculateLeft;
                    animatedProperty = Canvas.LeftProperty;

                    // Sort the bubbles to ensure that the rows move 
                    // in sync with each other in an appealing way.
                    bubbles = taskGroup.Select(t => t.Bubble)
                        .OrderByDescending(b => b.Row)
                        .ThenByDescending(b => b.Column);
                    break;

                default:
                    throw new ArgumentException("Unrecognized BubblesTaskType: " + taskGroup.TaskType);
            }
        }

        IEnumerable<ContentPresenter> GetBubblePresenters(IEnumerable<BubbleViewModel> bubbles)
        {
            var bubblePresenters = new List<ContentPresenter>();
            var contentPresenters = _bubbleCanvas.Children.Cast<ContentPresenter>().ToArray();
            foreach (BubbleViewModel bubble in bubbles)
            {
                var bubblePresenter = contentPresenters.FirstOrDefault(cp => cp.DataContext == bubble);
                if (bubblePresenter != null)
                {
                    bubblePresenters.Add(bubblePresenter);
                }
            }
            return bubblePresenters;
        }

        static Duration CalculateDuration(BubbleTaskGroup taskGroup, BubbleViewModel bubble, int millisecondsPerUnit)
        {
            var bubbleTask = taskGroup.SingleOrDefault(t => t.Bubble == bubble);

            int totalMilliseconds;
            switch (taskGroup.TaskType)
            {
                case BubbleTaskType.Burst:
                case BubbleTaskType.Add:
                    totalMilliseconds = millisecondsPerUnit;
                    break;

                case BubbleTaskType.MoveDown:
                    totalMilliseconds = millisecondsPerUnit * Math.Abs(bubbleTask.MoveDistance);
                    break;

                case BubbleTaskType.MoveRight:
                    totalMilliseconds = millisecondsPerUnit * Math.Abs(bubbleTask.MoveDistance);
                    break;

                default:
                    throw new ArgumentException("Unrecognized BubblesTaskType value: " + taskGroup.TaskType, "taskType");
            }

            return new Duration(TimeSpan.FromMilliseconds(totalMilliseconds));
        }

        static bool IsTaskStaggered(BubbleTaskType taskType)
        {
            return taskType != BubbleTaskType.Burst;
        }
    }
}
