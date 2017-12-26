using System;

namespace Swarm_Logic
{
    /// <summary>
    /// This class implements an exponintial random generator with mean.
    /// </summary>
    public class ExponintialRandom : NumberGenerator
    {
        // Represents the mean of the exponintial random generator.
        private double Mean;

        // Represents the pseudo-random number generator used to generate random numbers.
        private Random UniformRandom;

        /// <summary>
        /// Creates a new exponintial random generator with the specified mean.
        /// </summary>
        /// <param name="Mean">Represents the mean of the exponintial random generator.</param>
        public ExponintialRandom(double Mean)
        {
            this.Mean = Mean;
            UniformRandom = new Random();
        }

        /// <summary>
        /// Creates a new exponintial random generator with the specified mean and with the specified seed.
        /// </summary>
        /// <param name="Mean">Represents the mean of the exponintial random generator.</param>
        /// <param name="Seed">Represents the seed used for the internal pseudo-random number generator.</param>
        public ExponintialRandom(double Mean, int Seed)
        {
            this.Mean = Mean;
            UniformRandom = new Random(Seed);
        }

        /// <summary>
        /// This function returns the next exponintial-randomly generated double number.
        /// </summary>
        /// <returns>The next exponintial-random double number.</returns>
        public double NextDouble()
        {
            return -Math.Log(UniformRandom.NextDouble()) * Mean;
        }
    }

}
