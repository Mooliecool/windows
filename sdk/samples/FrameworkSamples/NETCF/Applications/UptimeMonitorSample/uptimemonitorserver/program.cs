//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------
namespace Microsoft.Samples.WCF.UptimeMonitor.Server {

    using System;
    using System.ServiceModel;
    using System.ServiceModel.Channels;

    class Program {
        static void Main() {
            Binding binding = new BasicHttpBinding();
            Uri address = new Uri("http://localhost:" + 
                UptimeRequest.ListeningPort);
            DateTime started = DateTime.Now;

            Console.WriteLine("Opening listener at {0}.", address);
            Console.WriteLine("Be sure your firewall allows incoming " +
                "requests on this port.");
            var listener = 
                binding.BuildChannelListener<IReplyChannel>(address);
            listener.Open();
            try {
                var channel = listener.AcceptChannel();
                channel.Open();
                try {
                    while (true) {
                        Console.WriteLine("Waiting for uptime requests.  Press Ctrl+C to stop server.");
                        // Block until a request from the client comes in.
                        var req = channel.ReceiveRequest(TimeSpan.MaxValue);
                        // The request doesn't have anything interesting in 
                        // our case, so we won't bother deserializing it.
                        // var uptimeRequest = req.RequestMessage.
                        //     GetBody<UptimeRequest>(
                        //        MessageSerializer.RequestSerializer);
                        
                        // Formulate our response.
                        var uptime = new UptimeResponse() { 
                            Uptime = DateTime.Now - started
                        };
                        Console.WriteLine("Uptime request received.  " +
                            "Sending {0}.", uptime.Uptime);
                        req.Reply(Message.CreateMessage(
                            binding.MessageVersion, "urn:UptimeResponse",
                            uptime, MessageSerializer.ResponseSerializer
                        ));
                    }
                } finally {
                    channel.Close();
                }
            } finally {
                listener.Close();
            }
        }
    }
}
