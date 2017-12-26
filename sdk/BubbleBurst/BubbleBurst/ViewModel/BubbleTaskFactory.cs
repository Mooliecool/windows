using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BubbleBurst.ViewModel
{
    public class BubbleTaskFactory
    {
        readonly BubbleMatrixViewModel _bubbleMatrix;

        internal BubbleTaskFactory(BubbleMatrixViewModel bubbleMatrix)
        {
            _bubbleMatrix = bubbleMatrix;
        }

        public BubbleTaskGroup CreateTaskGroup(BubbleTaskType taskType, BubbleViewModel[] bubblesInGroup)
        {
            BubbleTaskGroup taskGroup = null;

            switch (taskType)
            {
                case BubbleTaskType.Burst:
                    taskGroup = new BubbleTaskGroup { TaskType = BubbleTaskType.Burst };
                    foreach (var bubble in bubblesInGroup)
                    {
                        taskGroup.Add(new BubbleTask { Bubble = bubble });
                    }
                    break;
                case BubbleTaskType.MoveDown:
                    taskGroup = MoveBubblesDown(bubblesInGroup);
                    break;
                case BubbleTaskType.MoveRight:
                    taskGroup = MoveBubblesRight();
                    break;
            }

            return taskGroup;
        }

        public BubbleTaskGroup CreateUndoTaskGroup(BubbleTaskGroup taskGroup)
        {
            taskGroup.ResetTask();

            switch (taskGroup.TaskType)
            {
                case BubbleTaskType.Burst:
                    taskGroup.TaskType = BubbleTaskType.Add;
                    break;
                case BubbleTaskType.MoveRight:
                case BubbleTaskType.MoveDown:
                    foreach (var task in taskGroup)
                    {
                        // Move the same distance in the opposite direction
                        task.MoveDistance = -task.MoveDistance;
                    }
                    break;
            }

            return taskGroup;
        }

        BubbleTaskGroup MoveBubblesDown(BubbleViewModel[] bubblesInGroup)
        {
            var taskGroup = new BubbleTaskGroup { TaskType = BubbleTaskType.MoveDown };

            int[] affectedColumns = bubblesInGroup.Select(b => b.Column).Distinct().ToArray();

            foreach (int affectedColumn in affectedColumns)
            {
                var bubblesInColumn = _bubbleMatrix.Bubbles.Where(b => b.Column == affectedColumn).ToArray();

                // If Empty Column
                if (bubblesInColumn.Length == 0)
                    continue;

                var moveDistance = 0;
                for (var rowIndex = _bubbleMatrix.RowCount - 1; rowIndex >= 0; rowIndex--)
                {
                    var bubble = bubblesInColumn.SingleOrDefault(b => b.Row == rowIndex);

                    if (bubble != null)
                    {
                        if (moveDistance > 0)
                        {
                            taskGroup.Add(new BubbleTask { Bubble = bubble, MoveDistance = moveDistance });
                        }
                    }
                    else
                    {
                        moveDistance++;
                    }
                }
            }

            return taskGroup;
        }

        BubbleTaskGroup MoveBubblesRight()
        {
            var taskGroup = new BubbleTaskGroup { TaskType = BubbleTaskType.MoveRight };

            for (int rowIndex = _bubbleMatrix.RowCount - 1; rowIndex >= 0; rowIndex--)
            {
                var bubblesInRow = _bubbleMatrix.Bubbles.Where(b => b.Row == rowIndex).ToArray();

                // Skip empty rows and full rows.
                if (bubblesInRow.Length == 0 ||
                    bubblesInRow.Length >= _bubbleMatrix.ColumnCount)
                    continue;

                var moveDistance = 0;
                for (int colIndex = _bubbleMatrix.ColumnCount - 1; colIndex >= 0; colIndex--)
                {
                    var bubble = bubblesInRow.SingleOrDefault(b => b.Column == colIndex);
                    if (bubble != null)
                    {
                        if (moveDistance > 0)
                        {
                            taskGroup.Add(new BubbleTask { Bubble = bubble, MoveDistance = moveDistance });
                        }
                    }
                    else
                    {
                        moveDistance++;
                    }
                }
            }

            return taskGroup;
        }
    }
}
