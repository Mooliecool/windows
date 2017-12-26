using System;
using System.Collections.Generic;

namespace Swarm_Logic
{
    /// <summary>
    /// A class that represents the enviroment where the agents move and interact. It is responsible for initialization of agents, inter-communication between them,
    /// detecting collisions with barriers, validating agents decisions and radiation source(s).
    /// </summary>
    public class Environment
    {
        /// <summary>
        /// Represents the radiation source(s).
        /// </summary>
        public RadiationSource Source;

        /// <summary>
        /// Represents a list of all agents in the environment.
        /// </summary>
        public List<Agent> Agents;

        /// <summary>
        /// Represents a list of all barriers in the environment.
        /// </summary>
        public List<Barrier2> Barriers;

        /// <summary>
        ///  This function decides if an agent will receive a meesage sent from another agent or not.
        /// </summary>
        /// <param name="SendingAgent">Represents the agent who is sending.</param>
        /// <param name="ReceivingAgent">Represents the agent who is receiving.</param>
        /// <returns>true if the second agent will receive the message from the first agent and returns false otherwise.</returns>
        private bool WillTheAgentReceiveTheMessage(Agent SendingAgent, Agent ReceivingAgent)
        {
            // If the sending agent is the same as the receiving one return false
            // If the distance between the sending agent and the receiving one is less than or equal GeneralParameters.ReceiveRange then return true
            // Otherwise return false
            return (SendingAgent != ReceivingAgent) &&
                (SendingAgent.PX - ReceivingAgent.PX) * (SendingAgent.PX - ReceivingAgent.PX) + (SendingAgent.PY - ReceivingAgent.PY) * (SendingAgent.PY - ReceivingAgent.PY) <= GeneralParameters.ReceiveRange * GeneralParameters.ReceiveRange;
        }

        /// <summary>
        /// Represents an event called on the end of each iteration done by the Run function.
        /// Can be used to update a graphical user interface.
        /// </summary>
        public event Action OnIterationEnd;

        /// <summary>
        /// Creates a new environment with the specified number of agents, environment dimensions, list of barriers,
        /// and radiation source(s).
        /// </summary>
        /// <param name="NumberOfAgents">Represents the number of all agents in the environment.</param>
        /// <param name="Width">Represents the width of the enviroment.</param>
        /// <param name="Height">Represents the height of the enviroment.</param>
        /// <param name="Barriers">Represents all barriers in the enviroment.</param>
        /// <param name="Source">Represents the source(s) of radiation in the environment.</param>
        public Environment(int NumberOfAgents, double Width, double Height, List<Barrier2> Barriers, RadiationSource Source)
        {
            //A uniform random generator for generating the X coordinate of agents' positions.
            NumberGenerator PXRandomGenerator = new UniformRandom(0.0, Width, (int)(DateTime.Now.Ticks));

            //A uniform random generator for generating the Y coordinate of agents' positions.
            NumberGenerator PYRandomGenerator = new UniformRandom(0.0, Height, (int)(DateTime.Now.Ticks + 1));

            //A uniform random generator for generating the X coordinate of agents' velocities.
            NumberGenerator VXRandomGenerator = new UniformRandom(-Width, Width, (int)(DateTime.Now.Ticks + 2));

            //A uniform random generator for generating the Y coordinate of agents' velocities.
            NumberGenerator VYRandomGenerator = new UniformRandom(-Height, Height, (int)(DateTime.Now.Ticks + 3));

            this.Source = Source;
            this.Barriers = Barriers;

            // Populate the list of agents with random positions and velocities.
            Agents = new List<Agent>(NumberOfAgents);
            for (int i = 0; i < NumberOfAgents; i++)
            {
                Agents.Add(new Agent(
                    PXRandomGenerator.NextDouble(),
                    PYRandomGenerator.NextDouble(),
                    VXRandomGenerator.NextDouble(),
                    VYRandomGenerator.NextDouble(),
                    Source.GetRadiation,
                    Send
                    ));
            }
        }

        /// <summary>
        /// Creates a new environment with the specified list of agents, environment dimensions, list of barriers,
        /// and radiation source(s).
        /// </summary>
        /// <param name="Agents">Represents all agents in the enviroment.</param>
        /// <param name="Width">Represents the width of the enviroment.</param>
        /// <param name="Height">Represents the height of the enviroment.</param>
        /// <param name="Barriers">Represents all barriers in the enviroment.</param>
        /// <param name="Source">Represents the source(s) of radiation in the environment.</param>
        public Environment(List<Agent> Agents, double Width, double Height, List<Barrier2> Barriers, RadiationSource Source)
        {
            //A uniform random generator for generating the X coordinate of agents' positions.
            NumberGenerator PXRandomGenerator = new UniformRandom(0.0, Width, (int)(DateTime.Now.Ticks));

            //A uniform random generator for generating the Y coordinate of agents' positions.
            NumberGenerator PYRandomGenerator = new UniformRandom(0.0, Height, (int)(DateTime.Now.Ticks + 1));

            //A uniform random generator for generating the X coordinate of agents' velocities.
            NumberGenerator VXRandomGenerator = new UniformRandom(-Width, Width, (int)(DateTime.Now.Ticks + 2));

            //A uniform random generator for generating the Y coordinate of agents' velocities.
            NumberGenerator VYRandomGenerator = new UniformRandom(-Height, Height, (int)(DateTime.Now.Ticks + 3));

            this.Source = Source;
            this.Barriers = Barriers;
            this.Agents = Agents;

            // Ensure that the agents are initialized properly.
            foreach (Agent a in Agents)
            {
                a.RadiationFunction = Source.GetRadiation;
                a.MyBestValue = a.RadiationFunction(a.PX, a.PY);
                a.SendMessage = Send;
            }
        }

        /// <summary>
        /// This function is used by all the agents to send messages to eachother.
        /// </summary>
        /// <param name="SendingAgent">Represents the agent who is sending.</param>
        /// <param name="Message">Represents the message which is being sent.</param>
        private void Send(Agent SendingAgent, AgentMessage Message)
        {
            foreach (Agent a in Agents)
            {
                // Check if an agent will receive the message sent by the SendingAgent.
                if (WillTheAgentReceiveTheMessage(SendingAgent, a))
                {
                    // Send the message to the agent.
                    a.Receive(Message);
                }
            }
        }

        /// <summary>
        /// This function executes only one iteration of the Particle Swarm Optimization algorithm.
        /// </summary>
        public void Update()
        {
            foreach (Agent agent in Agents)
            {

                // Make the agent calculates the next action.
                agent.CalculateNextAction();

                // Current position of the agent.
                double startX = agent.PX;
                double startY = agent.PY;

                // The expected position of the agent after executing the calculated action.
                double endX = agent.PX + agent.VX;
                double endY = agent.PY + agent.VY;

                // The number of invalid decisions each agent is allowed to take.
                int Try = 4;

                // A flag to indicate whether to restart the small foreach loop or not.
                bool Restart;

                do
                {
                    Restart = false;

                    // Ensure that executing the action will not cross any of the barriers.
                    foreach (Barrier2 barrier in Barriers)
                    {
                        // Executing the action will cross a barrier.
                        if (barrier.IsIntersected(startX, startY, endX, endY))
                        {
                            // If the number of remaining trials is greater than zero.
                            if (Try > 0)
                            {
                                // Allow the agent to take another action.
                                agent.TakeRandomDecision();

                                // Re-calculates the expected position of the agent after executing the calculated action.
                                endX = agent.PX + agent.VX;
                                endY = agent.PY + agent.VY;

                                // Decrease the number of remaining trials.
                                Try--;

                                // Restart the loop checking for barries.
                                // Make the loop checks for all the barries again after the decision is changed.
                                Restart = true;
                                break;
                            }
                            else
                            {
                                // The agent has finished all the remaining trials.
                                // It will not be allowed to move in this iteration.
                                endX = agent.PX;
                                endY = agent.PY;
                                agent.VX = 0;
                                agent.VY = 0;

                                break;
                            }
                        }
                    }

                } while (Restart);

                // Execute the calculated action -if it was valid.
                agent.PX = endX;
                agent.PY = endY;

                // Check if the agent became near a source after executing the decesion.
                if (Source.IsNearASource(agent.PX, agent.PY))
                    agent.FoundSource = true;

                // Update agent parameters and communicate them to other agents if needed.
                agent.AfterMoving();
            }
        }

        /// <summary>
        /// This function executes the specified number of iterations of the Particle Swarm Optimization algorithm.
        /// </summary>
        /// <param name="NumberOfIterations">The number of iterations to be executed.</param>
        public void Run(int NumberOfIterations)
        {
            for (int i = 0; i < NumberOfIterations; i++)
            {
                // Perform one iteration.
                Update();

                // Call the event after finishing one iteration.
                if (OnIterationEnd != null)
                    OnIterationEnd();
            }
        }

    }
}
