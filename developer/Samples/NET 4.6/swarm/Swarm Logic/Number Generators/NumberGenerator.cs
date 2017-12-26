
namespace Swarm_Logic
{
    /// <summary>
    /// An interface for the classes that implement random number generators.
    /// It must contain a function that returns the next random double number.
    /// </summary>
    public interface NumberGenerator
    {
        /// <summary>
        /// This function returns the next randomly generated double number.
        /// </summary>
        /// <returns>The next random double number.</returns>
        double NextDouble();
    }

}
