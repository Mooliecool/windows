using System;

namespace Swarm_Logic
{
    /// <summary>
    /// This class model the intensity of a gaussian radiation source.
    /// </summary>
    public class GaussianFunctionSource : RadiationSource
    {
        // Represents the X coordinate of the gaussian radiation source.
        private double SourceX { get; set; }

        // Represents the Y coordinate of the gaussian radiation source.
        private double SourceY { get; set; }

        // Represents the sigma square value of the gaussian radiation source.
        private double Source1B { set; get; }

        /// <summary>
        /// Creates a new gaussian radiation source with the specified position and sigma square value.
        /// </summary>
        /// <param name="SourceX">Represents the X coordinate of the gaussian radiation source.</param>
        /// <param name="SourceY">Represents the Y coordinate of the gaussian radiation source.</param>
        /// <param name="Source1B">Represents the sigma square value of the gaussian radiation source.</param>
        public GaussianFunctionSource(double SourceX, double SourceY, double Source1B)
        {
            this.SourceX = SourceX;
            this.SourceY = SourceY;
            this.Source1B = Source1B;
        }

        /// <summary>
        /// This function takes the coordinates of an agent position and returns the intensity of the gaussian radiation source as measured by a sensor at that position.
        /// </summary>
        /// <param name="PX">Represents the X coordinate of the agent.</param>
        /// <param name="PY">Represents the Y coordinate of the agent.</param>
        /// <returns>The intensity of the gaussian radiation source as measured by the agent sensor at that position.</returns>
        public double GetRadiation(double PX, double PY)
        {
            return Math.Exp(-0.5 * ((SourceX - PX) * (SourceX - PX) / Source1B + (SourceY - PY) * (SourceY - PY) / Source1B));
        }

        /// <summary>
        /// This function takes the coordinates of an agent position and returns true if the position is near gaussian radiation source and returns false otherwise.
        /// </summary>
        /// <param name="PX">Represents the X coordinate of the agent.</param>
        /// <param name="PY">Represents the Y coordinate of the agent.</param>
        /// <returns>true if the position is near gaussian radiation source and returns false otherwise.</returns>
        public bool IsNearASource(double PX, double PY)
        {
            return Math.Sqrt((SourceX - PX) * (SourceX - PX) + (SourceY - PY) * (SourceY - PY)) <= GeneralParameters.NearDistance;
        }
    }
}
