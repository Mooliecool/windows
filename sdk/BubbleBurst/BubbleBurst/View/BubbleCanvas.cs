using System;
using System.Windows;
using System.Windows.Controls;
using BubbleBurst.ViewModel;

namespace BubbleBurst.View
{
    /// <summary>
    /// A Canvas panel that arranges bubbles into a matrix layout.
    /// </summary>
    public class BubbleCanvas : Canvas
    {
        internal int BubbleSize
        {
            get
            {
                var maxWidth = (int)Math.Floor(base.ActualWidth / ColumnCount);
                var maxHeight = (int)Math.Floor(base.ActualHeight / RowCount);
                return Math.Min(maxWidth, maxHeight);
            }
        }

        public static readonly DependencyProperty RowCountProperty = 
            DependencyProperty.Register("RowCount", typeof(int), typeof(BubbleCanvas), new PropertyMetadata(0));
        internal int RowCount { get { return (int)GetValue(RowCountProperty); } set { SetValue(RowCountProperty, value); } }

        public static readonly DependencyProperty ColumnCountProperty =
            DependencyProperty.Register("ColumnCount", typeof(int), typeof(BubbleCanvas), new PropertyMetadata(0));
        internal int ColumnCount { get { return (int)GetValue(ColumnCountProperty); } set { SetValue(ColumnCountProperty, value); } }

        internal double CalculateLeft(FrameworkElement bubbleContainer)
        {
            if (bubbleContainer == null)
                throw new ArgumentNullException("bubbleContainer");

            var bubble = bubbleContainer.DataContext as BubbleViewModel;
            if (bubble == null)
                throw new ArgumentException("Element does not have a BubbleViewModel as its DataContext.", "bubbleContainer");

            return this.CalculateLeft(bubble.Column);
        }

        internal double CalculateTop(FrameworkElement bubbleContainer)
        {
            if (bubbleContainer == null)
                throw new ArgumentNullException("bubbleContainer");

            var bubble = bubbleContainer.DataContext as BubbleViewModel;
            if (bubble == null)
                throw new ArgumentException("Element does not have a BubbleViewModel as its DataContext.", "bubbleContainer");

            return this.CalculateTop(bubble.Row);
        }

        protected override void OnVisualChildrenChanged(DependencyObject visualAdded, DependencyObject visualRemoved)
        {
            var contentPresenter = visualAdded as ContentPresenter;
            if (contentPresenter != null)
            {
                var bubble = contentPresenter.DataContext as BubbleViewModel;
                if (bubble != null)
                {
                    Canvas.SetLeft(contentPresenter, CalculateLeft(bubble.Column));
                    Canvas.SetTop(contentPresenter, CalculateTop(bubble.Row));

                    contentPresenter.Width = BubbleSize;
                    contentPresenter.Height = BubbleSize;
                }
            }

            base.OnVisualChildrenChanged(visualAdded, visualRemoved);
        }

        double CalculateLeft(int column)
        {
            double bubblesWidth = BubbleSize * this.ColumnCount;
            double horizOffset = (base.ActualWidth - bubblesWidth) / 2;
            return column * BubbleSize + horizOffset;
        }

        double CalculateTop(int row)
        {
            double bubblesHeight = BubbleSize * this.RowCount;
            double vertOffset = (base.ActualHeight - bubblesHeight) / 2;
            return row * BubbleSize + vertOffset;
        }
    }
}