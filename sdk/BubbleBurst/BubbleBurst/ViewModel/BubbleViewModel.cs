using System;
using System.Windows.Input;
using BubbleBurst.ViewModel.Internal;
using ReactiveUI;

namespace BubbleBurst.ViewModel
{
    /// <summary>
    /// Represents a bubble in the bubble matrix.
    /// </summary>
    public class BubbleViewModel : ReactiveObject
    {
        readonly BubbleMatrixViewModel _bubbleMatrix;

        static readonly Random _random = new Random(DateTime.Now.Millisecond);

        public BubbleType BubbleType { get; private set; }

        public int Row { get; set; }
        public int Column { get; set; }

        bool _isActive;
        /// <summary>
        /// Returns true if this bubble is a member of the 
        /// currently active bubble group in the user interface.
        /// </summary>
        public bool IsActive
        {
            get { return _isActive; }
            internal set { this.RaiseAndSetIfChanged(ref _isActive, value); }
        }

        /// <summary>
        /// Returns the command used to burst the bubble group in which this bubble exists.
        /// </summary>
        public IReactiveCommand BurstBubbleGroupCommand { get; private set; }

        internal BubbleViewModel(BubbleMatrixViewModel bubbleMatrix, int row, int column)
        {
            if (bubbleMatrix == null)
                throw new ArgumentNullException("bubbleMatrix");

            if (row < 0 || bubbleMatrix.RowCount <= row)
                throw new ArgumentOutOfRangeException("row");

            if (column < 0 || bubbleMatrix.ColumnCount <= column)
                throw new ArgumentOutOfRangeException("column");

            _bubbleMatrix = bubbleMatrix;

            MoveTo(row, column);

            this.BubbleType = GetRandomBubbleType();

            BurstBubbleGroupCommand = new ReactiveCommand();
            BurstBubbleGroupCommand.Subscribe(x => _bubbleMatrix.BurstBubbleGroup());
        }

        /// <summary>
        /// Causes the bubble to evaluate whether or not it is in a bubble group.
        /// </summary>
        /// <param name="isMouseOver">
        /// True if the mouse cursor is currently over this bubble.
        /// </param>
        public void SetFocus(bool isMouseOver)
        {
            _bubbleMatrix.FocusedBubble = isMouseOver ? this : null;
        }

        internal void MoveTo(int row, int column)
        {
            Row = row;
            Column = column;
        }

        static BubbleType GetRandomBubbleType()
        {
            var bubbleTypeValues = Enum.GetValues(typeof(BubbleType)) as BubbleType[];
            return bubbleTypeValues[_random.Next(bubbleTypeValues.Length)];
        }

        // Easier Viewing for Debugging
        public override string ToString()
        {
            return String.Format("{0}: {1},{2}", BubbleType, Row, Column);
        }
    }
}