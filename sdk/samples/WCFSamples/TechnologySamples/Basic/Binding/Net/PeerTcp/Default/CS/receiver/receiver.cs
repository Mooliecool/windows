
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.Configuration;
using System.ServiceModel;
using System.ServiceModel.Dispatcher;

// M:N broadcast application that enables senders to send announcements to multiple receivers
// using Peer Channel (a multi-party channel). The sender is implemented by a different program.

// If you are unfamiliar with WCF concepts used in this sample, refer to the WCF Basic\GettingStarted sample.


namespace Microsoft.ServiceModel.Samples
{
    // Define a service contract.
    [ServiceContract(Namespace="http://Microsoft.ServiceModel.Samples")]
    public interface IBroadcast
    {
        [OperationContract(IsOneWay = true)]
        void Announce(string msg);
    }

    // Implements the receiver of M:N broadcast app
    [ServiceBehavior(InstanceContextMode = InstanceContextMode.Single)]
    public class BroadcastReceiver : IBroadcast
    {
        // IBroadcast implementation
        public void Announce(string msg)
        {
            Console.WriteLine("Received {0}", msg);
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

        // Host the receiver within this EXE console application.
        public static void Main()
        {
            // Create a ServiceHost for the BroadcastReceiver type
            using (ServiceHost serviceHost = new ServiceHost(typeof(BroadcastReceiver)))
            {
                serviceHost.Open();

                // Open the ServiceHostBase to create listeners and start listening for messages.
                EndpointAddress lookFor = new EndpointAddress(serviceHost.BaseAddresses[0].ToString() + "announcements");
                for (int i=0; i<serviceHost.ChannelDispatchers.Count; ++i)
                {
                    ChannelDispatcher channelDispatcher = serviceHost.ChannelDispatchers[i] as ChannelDispatcher;
                    if (channelDispatcher != null)
                    {
                        for (int j=0; j<channelDispatcher.Endpoints.Count; ++j)
                        {
                            EndpointDispatcher endpointDispatcher = channelDispatcher.Endpoints[j];
                            if (endpointDispatcher.EndpointAddress == lookFor)
                            {
                                IOnlineStatus ostat = (IOnlineStatus)channelDispatcher.Listener.GetProperty<IOnlineStatus>();
                                if (ostat != null)
                                {
                                    ostat.Online += OnOnline;
                                    ostat.Offline += OnOffline;
                                    if (ostat.IsOnline)
                                    {
                                        Console.WriteLine("**  Online");
                                    }
                                }
                            }
                        }
                    }
                }

                // The receiver can now receive broadcast announcements
                Console.WriteLine("**  The receiver is ready");
                Console.WriteLine("Press <ENTER> to terminate receiver.");
                Console.ReadLine();
            }
        }
    }
}

