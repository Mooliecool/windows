
namespace Swarm_Logic
{
    /// <summary>
    /// A struct that represents the messages sent between different agents.
    /// It contains position coordinates and value of the radiation intensity at that position.
    /// </summary>
    public struct AgentMessage
    {
        /// <summary>
        /// Represents the X coordinate of the sent message.
        /// </summary>
        public double PX { set; get; }

        /// <summary>
        /// Represents the Y coordinate of the sent message.
        /// </summary>
        public double PY { set; get; }

        /// <summary>
        /// Represents radiation intensity at the position (PX,PY).
        /// </summary>
        public double Value { set; get; }

        /// <summary>
        /// Creates an new agent message with the specified coordinates and radiation intensity value.
        /// </summary>
        /// <param name="PX">Represents the X coordinate of the sent message.</param>
        /// <param name="PY">Represents the Y coordinate of the sent message.</param>
        /// <param name="Value">Represents radiation intensity at the position (PX,PY).</param>
        public AgentMessage(double PX, double PY, double Value)
            : this()
        {
            this.PX = PX;
            this.PY = PY;
            this.Value = Value;
        }
    }
}