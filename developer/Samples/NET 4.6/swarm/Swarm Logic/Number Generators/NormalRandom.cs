using System;

namespace Swarm_Logic
{
    /// <summary>
    /// This class implements a normal random generator with a mean and a variance.
    /// </summary>
    public class NormalRandom : NumberGenerator
    {
        // Represents the mean of the normal random generator.
        private double Mean;

        // Represents the variance of the normal random generator.
        private double Variance;

        // Represents the standard deviation of the normal random generator.
        private double StandardDeviation;

        // Represents the first pseudo-random number generator used to generate random numbers.
        private Random UniformRandom1;

        // Represents the second pseudo-random number generator used to generate random numbers.
        private Random UniformRandom2;

        /// <summary>
        /// Creates a new normal random generator with the specified mean and variance.
        /// </summary>
        /// <param name="Mean"> Represents the mean of the normal random generator.</param>
        /// <param name="Variance">Represents the variance of the normal random generator.</param>
        [Obsolete("Danger of producing unvalid disribution, consider providing 2 seeds to the constructor instead to solve this problem...")]
        public NormalRandom(double Mean, double Variance)
        {
            this.Mean = Mean;
            this.Variance = Variance;
            this.StandardDeviation = Math.Sqrt(Variance);
            UniformRandom1 = new Random();
            UniformRandom2 = new Random();
        }

        /// <summary>
        /// Creates a new normal random generator with the specified mean and variance and with the specified seeds.
        /// </summary>
        /// <param name="Mean"> Represents the mean of the normal random generator.</param>
        /// <param name="Variance">Represents the variance of the normal random generator.</param>
        /// <param name="Seed1">>Represents the seed used for the first internal pseudo-random number generator.</param>
        /// <param name="Seed2">>Represents the seed used for the second internal pseudo-random number generator.</param>
        public NormalRandom(double Mean, double Variance, int Seed1, int Seed2)
        {
            this.Mean = Mean;
            this.Variance = Variance;
            this.StandardDeviation = Math.Sqrt(Variance);
            UniformRandom1 = new Random(Seed1);
            UniformRandom2 = new Random(Seed2);
        }

        /// <summary>
        /// This function returns the next normal-randomly generated double number.
        /// </summary>
        /// <returns>The next normal-randomly double number.</returns>
        public double NextDouble()
        {
            return StandardDeviation * Math.Sqrt(-2 * Math.Log(UniformRandom1.NextDouble())) * Math.Cos(2 * Math.PI * UniformRandom2.NextDouble()) + Mean;
        }
    }
}
