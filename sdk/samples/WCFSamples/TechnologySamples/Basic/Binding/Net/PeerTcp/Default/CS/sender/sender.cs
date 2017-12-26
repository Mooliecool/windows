
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.ServiceModel.Channels;
using System.Configuration;
using System.ServiceModel;


// M:N broadcast application that enables senders to send announcements to multiple receivers
// using Peer Channel (a multi-party channel). The receiver is implemented by a different program.

// If you are unfamiliar with WCF concepts used in this sample, refer to the WCF Basic\GettingStarted sample.

namespace Microsoft.ServiceModel.Samples
{
    // Service contract for Broadcast. It's manually generated since Peer Transport currently does not
    // support WSDL generation.
    [ServiceContract(Namespace = "http://Microsoft.ServiceModel.Samples")]
    public interface IBroadcast
    {
        [OperationContract(IsOneWay = true)]
        void Announce(string msg);
    }

    public interface IBroadcastChannel : IBroadcast, IClientChannel
    {
    }

    // Sender implementation code.
    // Host the sender within this EXE console application.
    public static class BroadcastSender
    {
        public static void Main()
        {
            // Get the sender ID from configuration
            string senderId = ConfigurationManager.AppSettings["sender"];

            // Create the sender with the given endpoint configuration
            // Sender is an instance of the sender side of the broadcast application that has opened a channel to mesh
            ChannelFactory<IBroadcastChannel> factory = new ChannelFactory<IBroadcastChannel>("BroadcastEndpoint");

            IBroadcastChannel sender = factory.CreateChannel();

            // Retrieve the PeerNode associated with the sender and register for online/offline events
            // PeerNode represents a node in the mesh. Mesh is the named collection of connected nodes.
            IOnlineStatus ostat = sender.GetProperty<IOnlineStatus>();
            ostat.Online += new EventHandler(OnOnline);
            ostat.Offline += new EventHandler(OnOffline);

            // Open the sender so that the instance can join the mesh even before the first message is sent
            sender.Open();

            Console.WriteLine("{0} is ready", senderId);
            Console.WriteLine("Press <ENTER> to send annoucements after starting the receivers and going Online");
            Console.ReadLine();
            Console.WriteLine("Sending Announcement 1");
            sender.Announce("Announcement 1");
            Console.WriteLine("Sending Announcement 2");
            sender.Announce("Announcement 2");
            Console.WriteLine("Sending Announcement 3");
            sender.Announce("Announcement 3");
            Console.WriteLine("Press <ENTER> to terminate the sender.");
            Console.ReadLine();

            sender.Close();

            factory.Close();
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
