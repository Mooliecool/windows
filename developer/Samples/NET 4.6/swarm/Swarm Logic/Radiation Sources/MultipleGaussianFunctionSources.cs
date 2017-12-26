using System;

namespace Swarm_Logic
{
    /// <summary>
    /// This class model the intensity of multiple gaussian radiation sources.
    /// </summary>
    public class MultipleGaussianFunctionSources : RadiationSource
    {
        // Represents the X coordinates of all the gaussian radiation sources.
        private double[] SourceXs;

        // Represents the Y coordinates of all the gaussian radiation sources.
        private double[] SourceYs;

        // Represents the amplitude values of all the gaussian radiation sources.
        private double[] SourceAs;

        // Represents the sigma square values of all the gaussian radiation sources.
        private double[] SourceBs;

        /// <summary>
        /// Creates new multiple gaussian radiation sources with the specified positions, amplitudes and sigma square values.
        /// </summary>
        /// <param name="SourceXs">Represents the X coordinates of all the gaussian radiation sources.</param>
        /// <param name="SourceYs">Represents the Y coordinates of all the gaussian radiation sources.</param>
        /// <param name="SourceAs">Represents the amplitude values of all the gaussian radiation sources.</param>
        /// <param name="SourceBs">Represents the sigma square values of all the gaussian radiation sources.</param>
        public MultipleGaussianFunctionSources(double[] SourceXs, double[] SourceYs, double[] SourceAs, double[] SourceBs)
        {
            this.SourceXs = SourceXs;
            this.SourceYs = SourceYs;
            this.SourceAs = SourceAs;
            this.SourceBs = SourceBs;
        }

        /// <summary>
        /// This function takes the coordinates of an agent position and returns the intensity of the multiple gaussian radiation sources as measured by a sensor at that position.
        /// </summary>
        /// <param name="PX">Represents the X coordinate of the agent.</param>
        /// <param name="PY">Represents the Y coordinate of the agent.</param>
        /// <returns>The intensity of the multiple gaussian radiation sources as measured by the agent sensor at that position.</returns>
        public double GetRadiation(double PX, double PY)
        {
            double Sum = 0;
            for (int i = 0; i < SourceXs.Length; i++)
            {
                double Magnitude = SourceAs[i] * Math.Exp(-0.5 * ((SourceXs[i] - PX) * (SourceXs[i] - PX) + (SourceYs[i] - PY) * (SourceYs[i] - PY)) / SourceBs[i]);
                Sum += Magnitude;
            }
            return Sum;
        }

        /// <summary>
        /// This function takes the coordinates of an agent position and returns true if the position is near any of the multiple gaussian radiation sources and returns false otherwise.
        /// </summary>
        /// <param name="PX">Represents the X coordinate of the agent.</param>
        /// <param name="PY">Represents the Y coordinate of the agent.</param>
        /// <returns>true if the position is near any of the multiple gaussian radiation sources and returns false otherwise.</returns>
        public bool IsNearASource(double PX, double PY)
        {
            for (int i = 0; i < SourceXs.Length; i++)
            {
                if (Math.Sqrt((SourceXs[i] - PX) * (SourceXs[i] - PX) + (SourceYs[i] - PY) * (SourceYs[i] - PY)) <= GeneralParameters.NearDistance)
                    return true;
            }
            return false;
        }

    }
}
