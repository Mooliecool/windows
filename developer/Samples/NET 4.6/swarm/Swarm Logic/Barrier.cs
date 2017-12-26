using System;

namespace Swarm_Logic
{
    /// <summary>
    /// A class that represents a barrier, that prevents moving from one side to another.
    /// It is responsible for detecting collisions with moving agents.
    /// </summary>
    public struct Barrier2
    {
        /// <summary>
        /// Represents the X coordinate of the first end of the barrier.
        /// </summary>
        public double X1 { set; get; }

        /// <summary>
        /// Represents the Y coordinate of the first end of the barrier.
        /// </summary>
        public double Y1 { set; get; }

        /// <summary>
        /// Represents the X coordinate of the second end of the barrier.
        /// </summary>
        public double X2 { set; get; }

        /// <summary>
        /// Represents the Y coordinate of the second end of the barrier.
        /// </summary>
        public double Y2 { set; get; }

        /// <summary>
        /// Creates a new barrier with the specified begining and ending coordinates.
        /// </summary>
        /// <param name="X1">Represents the X coordinate of the first end of the barrier.</param>
        /// <param name="Y1">Represents the Y coordinate of the first end of the barrier.</param>
        /// <param name="X2">Represents the X coordinate of the second end of the barrier.</param>
        /// <param name="Y2">Represents the Y coordinate of the second end of the barrier.</param>
        public Barrier2(double X1, double Y1, double X2, double Y2)
            : this()
        {
            this.X1 = X1;
            this.Y1 = Y1;
            this.X2 = X2;
            this.Y2 = Y2;
        }

        /// <summary>
        /// Checks if a moving agent crosses the barrier or not.
        /// </summary>
        /// <param name="startX">Represents the X coordinate of the inital position of agent before moving.</param>
        /// <param name="startY">Represents the Y coordinate of the inital position of agent before moving.</param>
        /// <param name="endX">Represents the X coordinate of the last position of agent after moving.</param>
        /// <param name="endY">Represents the Y coordinate of the last position of agent after moving.</param>
        /// <returns>true if the agent crosses the barrier and returns false otherwise.</returns>
        public bool IsIntersected(double startX, double startY, double endX, double endY)
        {
            double A1 = Y2 - Y1;
            double B1 = X1 - X2;
            double C1 = A1 * X1 + B1 * Y1;

            double A2 = endY - startY;
            double B2 = startX - endX;
            double C2 = A2 * startX + B2 * startY;

            // The determinant of the linear system of equations.
            double det = A1 * B2 - A2 * B1;

            if (Math.Abs(det) <= double.Epsilon)
                return false; // Zero determinant so parrallel lines. That means the agent is moving along the barrier, and that is OK.
            else
            {
                // Calculate the intersection points of the straight lines- not the line segments.
                double x = (B2 * C1 - B1 * C2) / det;
                double y = (A1 * C2 - A2 * C1) / det;

                // Check if the calculated point is the intersection points of the line segments.
                if (
                    x >= Math.Min(startX, endX) && x <= Math.Max(startX, endX) &&
                    x >= Math.Min(X1, X2) && x <= Math.Max(X1, X2) &&
                    y >= Math.Min(startY, endY) && y <= Math.Max(startY, endY) &&
                    y >= Math.Min(Y1, Y2) && y <= Math.Max(Y1, Y2)
                    )
                    return true; // There is an intersection.
                else
                    return false; // There is no intersection.
            }
        }
    }
}
