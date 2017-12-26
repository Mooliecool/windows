
namespace Swarm_Logic
{
    /// <summary>
    /// An interface for the classes that model the intensity of radiation source(s).
    /// It must contain a function that takes the coordinates of a position and returns the intensity of the radiation as measured by a sensor at that position.
    /// It must also contain a function that takes the coordinates of a position and returns true if the position is near radiation source(s) and returns false otherwise.
    /// </summary>
    public interface RadiationSource
    {
        /// <summary>
        /// This function takes the coordinates of an agent position and returns the intensity of the radiation as measured by a sensor at that position.
        /// </summary>
        /// <param name="PX">Represents the X coordinate of the agent.</param>
        /// <param name="PY">Represents the Y coordinate of the agent.</param>
        /// <returns>The intensity of radiation as measured by the agent sensor at that position.</returns>
        double GetRadiation(double PX, double PY);

        /// <summary>
        /// This function takes the coordinates of an agent position and returns true if the position is near radiation source(s) and returns false otherwise.
        /// </summary>
        /// <param name="PX">Represents the X coordinate of the agent.</param>
        /// <param name="PY">Represents the Y coordinate of the agent.</param>
        /// <returns>true if the position is near radiation source(s) and returns false otherwise.</returns>
        bool IsNearASource(double PX, double PY);
    }

}
