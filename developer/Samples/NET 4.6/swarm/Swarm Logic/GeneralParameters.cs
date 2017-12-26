
namespace Swarm_Logic
{
    /// <summary>
    /// A static class that contains various parameters for the Particle Swarm Optimization algorithm,
    /// communication range, minimum and maximum velocities.
    /// </summary>
    public static class GeneralParameters
    {
        /// <summary>
        /// Represents the ratio of the agent's current velocity that affects the next velocity decesion.
        /// </summary>
        public static double W = 0.3925;

        /// <summary>
        /// Represents the ratio of the agent's distance to the best encountered position that affects the next velocity decesion.
        /// </summary>
        public static double P = 2.5586;

        /// <summary>
        /// Represents the ratio of the agent's distance to the best known position that affects the next velocity decesion.
        /// </summary>
        public static double G = 1.3358;

        /// <summary>
        /// Represents the range in which agents can communicate with each other.
        /// </summary>
        public static double ReceiveRange = 50.0;

        /// <summary>
        /// Represents the maximum velocity for every agent.
        /// </summary>
        public static double MaxVelocity = 5.0;

        /// <summary>
        /// Represents the minimum velocity for every agent.
        /// </summary>
        public static double MinVelocity = 2.0;

        /// <summary>
        /// An agent is considered reached a radiation source, if it is at distance NearDistance or less to the radiation source.
        /// </summary>
        public static double NearDistance = 5.0;
    }
}
