using System;

namespace Swarm_Logic
{
    /// <summary>
    /// A class that represents an agent of the Particle Swarm Optimization algorithm. It is responsible for taking -not executing- decisions,
    /// communicating with other agents, and searching fr the radiation source.
    /// </summary>
    public class Agent
    {
        // Represents the pseudo-random number generator used to generate random numbers for the Particle Swarm Optimization algorithm.
        private static Random r = new Random();
        
        /// <summary>
        /// A boolean that indicates if the agent has found a radiation source before.
        /// </summary>
        public bool FoundSource { get; set; }

        /// <summary>
        /// A boolean that indicates if the agent was sending information in the last iteration.
        /// That boolean can be used by a graphical user interface, to indicate when each agent had sent information.
        /// </summary>
        public bool WasSending { get; set; }

        /// <summary>
        /// Represents the X coordinate of the agent position.
        /// </summary>
        public double PX { set; get; }

        /// <summary>
        /// Represents the Y coordinate of the agent position.
        /// </summary>
        public double PY { set; get; }

        /// <summary>
        /// Represents the X coordinate of the agent velocity.
        /// </summary>
        public double VX { set; get; }

        /// <summary>
        /// Represents the Y coordinate of the agent velocity.
        /// </summary>
        public double VY { set; get; }

        /// <summary>
        /// Represents the X coordinate of the visited-by-the-agent position with the most radiation intensity.
        /// </summary>
        public double MyBestX { set; get; }

        /// <summary>
        /// Represents the Y coordinate of the visited-by-the-agent position with the most radiation intensity.
        /// </summary>
        public double MyBestY { set; get; }

        /// <summary>
        /// Represents the radiation intensity of the visited-by-the-agent position with the most radiation intensity.
        /// </summary>
        public double MyBestValue { set; get; }

        /// <summary>
        /// Represents the X coordinate of the known position with the most radiation intensity.
        /// </summary>
        public double OthersBestX { set; get; }

        /// <summary>
        /// Represents the Y coordinate of the known position with the most radiation intensity.
        /// </summary>
        public double OthersBestY { set; get; }

        /// <summary>
        /// Represents the radiation intensity of the known position with the most radiation intensity.
        /// </summary>
        public double OthersBestValue { set; get; }

        /// <summary>
        /// Represents the function used by the agent to calculate the radiation intensity as measured by a sensor at any given position.
        /// </summary>
        public PositionFunction RadiationFunction { set; get; }

        /// <summary>
        /// Represents the function used by the agent to send messages to other nearby agents. 
        /// </summary>
        public SendMessageFunction SendMessage{set;get;}

        /// <summary>
        /// The delegate of the function used by the agent to calculate the radiation intensity as measured by a sensor at any given position.
        /// </summary>
        /// <param name="PX">Represents the X coordinate of the agent position.</param>
        /// <param name="PY">Represents the Y coordinate of the agent position.</param>
        /// <returns>The intensity of radiation as measured by the agent sensor at that position.</returns>
        public delegate double PositionFunction(double PX, double PY);

        /// <summary>
        /// The delegate of the function used by the agent to send messages to other nearby agents.
        /// </summary>
        /// <param name="SendingAgent">Represents the agent who is sending.</param>
        /// <param name="Message">Represents the message which is being sent.</param>
        public delegate void SendMessageFunction(Agent SendingAgent, AgentMessage Message);

        /// <summary>
        /// This function calculates a random decision.
        /// It is used after the agent had found a radiation source, in order to wander around telling everyone else with the source position.
        /// </summary>
        private void TakeTotalyRandomDecision()
        {
            VX = (r.NextDouble() - 0.5) * 2 * GeneralParameters.MaxVelocity;
            VY = (r.NextDouble() - 0.5) * 2 * GeneralParameters.MaxVelocity;

            // Calculate the magnitude of the calculated velocity.
            double V = Math.Sqrt(VX * VX + VY * VY);

            // If magnitude is zero, then make the dominator 1.0 to avoid division by zero.
            if (V == 0)
                V = 1.0;
            
            VX = VX / V;
            VY = VY / V;

            // Ensure that the velocity is between MinVelocity and MaxVelocity inclusively.
            V = Math.Min(V, GeneralParameters.MaxVelocity);
            V = Math.Max(V, GeneralParameters.MinVelocity);

            VX *= V;
            VY *= V;
        }

        /// <summary>
        /// This function calculates a random decision related to the direction opposite to the current velocity.
        /// </summary>
        private void TakeRandomOppositeDecision()
        {
            VX = -(r.NextDouble()) * VX;
            VY = -(r.NextDouble()) * VY;

            // Calculate the magnitude of the calculated velocity.
            double V = Math.Sqrt(VX * VX + VY * VY);

            // If magnitude is zero, then make the dominator 1.0 to avoid division by zero.
            if (V == 0)
                V = 1.0;
            
            VX = VX / V;
            VY = VY / V;

            // Ensure that the velocity is between MinVelocity and MaxVelocity inclusively.
            V = Math.Min(V, GeneralParameters.MaxVelocity);
            V = Math.Max(V, GeneralParameters.MinVelocity);

            VX *= V;
            VY *= V;
        }

        /// <summary>
        /// This function calculates a random decision related to the direction tangent -perpendicular- to the current velocity.
        /// </summary>
        private void TakeRandomTangentDecision()
        {
            // VX = -(r.NextDouble()) * VX;
            // VY = -(r.NextDouble()) * VY;

            double dx = VX - PX;
            double dy = VY - PY;

            double rdir = new Random().NextDouble();

            double theta = Math.Atan(dy / dx);

            if (rdir > 0.5)
                theta *= 1;

            double mag = Math.Sqrt((dx * dx) + (dy * dy));

            double dxn = -dy;
            double dyn = dx;

            VX = PX + (dxn * mag);
            VY = PY + (dyn * mag);

            // Calculate the magnitude of the calculated velocity.
            double V = Math.Sqrt(VX * VX + VY * VY);

            // If magnitude is zero, then make the dominator 1.0 to avoid division by zero.
            if (V == 0)
                V = 1.0;
            
            VX = VX / V;
            VY = VY / V;

            // Ensure that the velocity is between MinVelocity and MaxVelocity inclusively.
            V = Math.Min(V, GeneralParameters.MaxVelocity);
            V = Math.Max(V, GeneralParameters.MinVelocity);

            VX *= V;
            VY *= V;

        }

        /// <summary>
        /// Creates a new agent with the specified position, velocity, radiation function and send message function.
        /// </summary>
        /// <param name="PX">Represents the X coordinate of the agent position.</param>
        /// <param name="PY">Represents the Y coordinate of the agent position.</param>
        /// <param name="VX">Represents the X coordinate of the agent velocity.</param>
        /// <param name="VY">Represents the Y coordinate of the agent velocity.</param>
        /// <param name="RadiationFunction">Represents the function used by the agent to calculate the radiation intensity as measured by a sensor at any given position.</param>
        /// <param name="SendMessage">// Represents the function used by the agent to send messages to other nearby agents.</param>
        public Agent(double PX, double PY, double VX, double VY, PositionFunction RadiationFunction, SendMessageFunction SendMessage)
        {
            this.PX = PX;
            this.PY = PY;

            this.VX = VX;
            this.VY = VY;

            this.MyBestX = PX;
            this.MyBestY = PY;

            this.RadiationFunction = RadiationFunction;

            // Evaluate the radiation intensity at te initial position as the best visited-by-the-agent position til now.
            this.MyBestValue = RadiationFunction(PX, PY);
            
            this.SendMessage = SendMessage;
        }

        /// <summary>
        /// This function is called -by the environment- when this agent receives a message from another agent.
        /// It updates the best known position with the most radiation intensity.
        /// </summary>
        /// <param name="Message">The message received from the other agent.</param>
        public void Receive(AgentMessage Message)
        {
            // If the other agent knows a better spot, update the best known position.
            if (Message.Value >= OthersBestValue)
            {
                OthersBestValue = Message.Value;
                OthersBestX = Message.PX;
                OthersBestY = Message.PY;
            }
        }

        /// <summary>
        /// This function calculates the next velocities according to the Particle Swarm Optimization algorithm.
        /// It stores the output in VX and VY.
        /// </summary>
        public void CalculateNextAction()
        {
            if (FoundSource)
            {
                // If this agent had already found a radiation source, it will take a random action,
                // in order to wander around telling everyone else with the source position. 
                TakeTotalyRandomDecision();
            }
            else
            {
                // Else calculate the next velocities according to the Particle Swarm Optimization algorithm.
                VX = GeneralParameters.W * VX + r.NextDouble() * GeneralParameters.P * (MyBestX - PX) + r.NextDouble() * GeneralParameters.G * (OthersBestX - PX);
                VY = GeneralParameters.W * VY + r.NextDouble() * GeneralParameters.P * (MyBestY - PY) + r.NextDouble() * GeneralParameters.G * (OthersBestY - PY);

                // Calculate the magnitude of the calculated velocity.
                double V = Math.Sqrt(VX * VX + VY * VY);

                // If magnitude is zero, then make the dominator 1.0 to avoid division by zero.
                if (V == 0.0)
                    V = 1.0;

                VX = VX / V;
                VY = VY / V;

                // Ensure that the velocity is between MinVelocity and MaxVelocity inclusively.
                V = Math.Min(V, GeneralParameters.MaxVelocity);
                V = Math.Max(V, GeneralParameters.MinVelocity);
                
                VX *= V;
                VY *= V;
            }
        }

        /// <summary>
        /// This function calculates a random decision, either parallel to or opposite to the current velocity direction,
        /// With some randomness employed.
        /// </summary>
        public void TakeRandomDecision()
        {
            if (r.NextDouble() < 0.5)
                TakeRandomOppositeDecision();
            else
                TakeRandomTangentDecision();
        }

        /// <summary>
        /// This function updates agent parameters and communicate them to other agents
        /// after executing the decisions -if needed.
        /// </summary>
        public void AfterMoving()
        {
            // If the agent had found a radiation source before, it will always send his information to nearby robots.
            bool WillAgentSend = FoundSource;

            if (!FoundSource)
            {
                // If this agent encountered a better spot, then update the best visited-by-the-agent position.
                double CurrentRadiation = RadiationFunction(PX, PY);
                if (CurrentRadiation > MyBestValue)
                {
                    MyBestX = PX;
                    MyBestY = PY;
                    MyBestValue = CurrentRadiation;
                    WillAgentSend = true;
                }

                // If the best visited-by-the-agent position is better than the best known position, then update the best known position.
                if (MyBestValue > OthersBestValue)
                {
                    OthersBestValue = MyBestValue;
                    OthersBestX = MyBestX;
                    OthersBestY = MyBestY;
                    WillAgentSend = true;
                }
            }

            // Update the WasSending boolean.
            WasSending = WillAgentSend;

            // If the agent is supposed to send in this iteration, then send the information to nearby agents.
            if (WillAgentSend)
            {
                SendMessage(this, new AgentMessage(OthersBestX, OthersBestY, OthersBestValue));
            }


        }

    }
}
