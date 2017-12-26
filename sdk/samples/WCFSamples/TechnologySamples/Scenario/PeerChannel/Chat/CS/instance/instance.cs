
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.Configuration;
using System.ServiceModel;
using System.ServiceModel.PeerResolvers;


// Multi-party chat application using Peer Channel (a multi-party channel)
// If you are unfamiliar with new concepts used in this sample, refer to the WCF Basic\GettingStarted sample.

namespace Microsoft.ServiceModel.Samples
{
    // Chat service contract
    // Applying [PeerBehavior] attribute on the service contract enables retrieval of PeerNode from IClientChannel.
    [ServiceContract(Namespace = "http://Microsoft.ServiceModel.Samples", CallbackContract = typeof(IChat))]
    public interface IChat
    {
        [OperationContract(IsOneWay = true)]
        void Join(string member);

        [OperationContract(IsOneWay = true)]
        void Chat(string member, string msg);

        [OperationContract(IsOneWay = true)]
        void Leave(string member);
    }

    public interface IChatChannel : IChat, IClientChannel
    {
    }

    public class ChatApp : IChat
    {
        // member id for this instance
        string member;

        public ChatApp(string member)
        {
            this.member = member;
        }

        // Host the chat instance within this EXE console application.
        public static void Main()
        {

            Console.WriteLine("Enter your nickname [default=DefaultName]: ");
            string member = Console.ReadLine();

            if (member == "") member = "DefaultName";

            // Construct InstanceContext to handle messages on callback interface. 
            // An instance of ChatApp is created and passed to the InstanceContext.
            InstanceContext instanceContext = new InstanceContext(new ChatApp(member));

            // Create the participant with the given endpoint configuration
            // Each participant opens a duplex channel to the mesh
            // participant is an instance of the chat application that has opened a channel to the mesh
            DuplexChannelFactory<IChatChannel> factory = new DuplexChannelFactory<IChatChannel>(instanceContext, "ChatEndpoint");

            IChatChannel participant = factory.CreateChannel();

            // Retrieve the PeerNode associated with the participant and register for online/offline events
            // PeerNode represents a node in the mesh. Mesh is the named collection of connected nodes.
            IOnlineStatus ostat = participant.GetProperty<IOnlineStatus>();
            ostat.Online += new EventHandler(OnOnline);
            ostat.Offline += new EventHandler(OnOffline);

            try
            {
                participant.Open();
            }
            catch (CommunicationException)
            {
                Console.WriteLine("Could not find resolver.  If you are using a custom resolver, please ensure");
                Console.WriteLine("that the service is running before executing this sample.  Refer to the readme");
                Console.WriteLine("for more details.");
                return;
            }
            

            Console.WriteLine("{0} is ready", member);
            Console.WriteLine("Type chat messages after going Online");
            Console.WriteLine("Press q<ENTER> to terminate this instance.");

            // Announce self to other participants
            participant.Join(member);

            // loop until the user quits
            while (true)
            {
                string line = Console.ReadLine();
                if (line == "q") break;
                participant.Chat(member, line);
            }
            // Leave the mesh
            participant.Leave(member);
            participant.Close();
            factory.Close();
        }

        // IChat implementation
        public void Join(string member)
        {
            Console.WriteLine("[{0} joined]", member);
        }

        public void Chat(string member, string msg)
        {
            // Comment out this if statement if you wish to echo chats from the same member
            if (member != this.member)
                Console.WriteLine("[{0}] {1}", member, msg);
        }

        public void Leave(string member)
        {
            Console.WriteLine("[{0} left]", member);
        }

        // PeerNode event handlers
        static void OnOnline(object sender, EventArgs e)
        {
            Console.WriteLine("**  Online");
        }

        static void OnOffline(object sender, EventArgs e)
        {
            Console.WriteLine("**  Offline");
        }
    }
}
