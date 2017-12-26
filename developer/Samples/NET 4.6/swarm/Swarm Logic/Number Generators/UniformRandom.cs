using System;

namespace Swarm_Logic
{
    /// <summary>
    /// This class implements a uniform random generator with a minimum and maximum values.
    /// </summary>
    public class UniformRandom : NumberGenerator
    {
        // Represents the minimum value reteurned by this uniform random generator.
        private double Min;

        // Represents the maximum value reteurned by this uniform random generator.
        private double Max;

        // Represents the pseudo-random number generator used to generate random numbers.
        private Random MyUniformRandom;

        /// <summary>
        /// Creates a new uniform random generator with the specified minimum and maximum values.
        /// </summary>
        /// <param name="Min">Represents the minimum value reteurned by this uniform random generator.</param>
        /// <param name="Max">Represents the maximum value reteurned by this uniform random generator.</param>
        public UniformRandom(double Min, double Max)
        {
            this.Min = Min;
            this.Max = Max;
            MyUniformRandom = new Random();
        }

        /// <summary>
        /// Creates a new uniform random generator with the specified minimum and maximum values and with the specified seed.
        /// </summary>
        /// <param name="Min">Represents the minimum value reteurned by this uniform random generator.</param>
        /// <param name="Max">Represents the maximum value reteurned by this uniform random generator.</param>
        /// <param name="Seed">Represents the seed used for the internal pseudo-random number generator.</param>
        public UniformRandom(double Min, double Max, int Seed)
        {
            this.Min = Min;
            this.Max = Max;
            MyUniformRandom = new Random(Seed);
        }

        /// <summary>
        /// This function returns the next uniform-randomly generated double number.
        /// </summary>
        /// <returns>The next uniform-random double number.</returns>
        public double NextDouble()
        {
            return MyUniformRandom.NextDouble() * (Max - Min) + Min;
        }
    }
}
