/******************************** Module Header ********************************\
Module Name:  Program.cs
Project:      CSRemotingClient
Copyright (c) Microsoft Corporation.

.NET remoting provides an abstract approach to interprocess communication that 
separates the remotable object from a specific client or server application 
domain and from a specific mechanism of communication. 

.NET Remoting allows an application to make a remotable object available across 
remoting boundaries, which includes different appdomains, processes or even 
different computers connected by a network. .NET Remoting makes a reference of a 
remotable object available to a client application, which then instantiates and 
uses a remotable object as if it were a local object. However, the actual code 
execution happens at the server-side. Any requests to the remotable object are 
proxied by the .NET Remoting runtime over Channel objects, that encapsulate the 
actual transport mode, including TCP streams, HTTP streams and named pipes. As a 
result, by instantiating proper Channel objects, a .NET Remoting application can 
be made to support different communication protocols without recompiling the 
application. The runtime itself manages the act of serialization and marshalling 
of objects across the client and server appdomains.

CSRemotingClient is a .NET Remoting client project. It accesses the remote objects 
(SingleCall objects or Singleton objects or client-activated objects) exposed by 
the .NET Remoting server project CSRemotingServer.

There are generally two ways to create the .NET Remoting client: using a 
configuration file or writing codes. The AccessRemotingServerByConfig method 
demonstrates the former, and the AccessRemotingServerByCode method illustrates 
the latter method.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directives
using System;
using System.Collections;
using System.Runtime.Remoting.Channels.Tcp;
using System.Runtime.Remoting.Channels;
using System.Runtime.Remoting;
using System.Runtime.Serialization.Formatters;
using System.Runtime.InteropServices;
using System.Net.Sockets;

using RemotingShared;
using System.Diagnostics;
#endregion


namespace CSRemotingClient
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length > 0 && (args[0].StartsWith("-") || args[0].StartsWith("/")))
            {
                string cmd = args[0].Substring(1);

                if (cmd.Equals("configfile", StringComparison.OrdinalIgnoreCase))
                {
                    // Access the .NET Remoting server using a configuration file.
                    AccessRemotingServerByConfig();
                }
                else if (cmd.Equals("code", StringComparison.OrdinalIgnoreCase))
                {
                    // Access the .NET Remoting server using code.
                    AccessRemotingServerByCode();
                }
                else
                {
                    PrintInstructions();
                }
            }
            else
            {
                // By default, access the .NET Remoting server using a 
                // configuration file.
                AccessRemotingServerByConfig();
            }
        }


        static void PrintInstructions()
        {
            Console.WriteLine("CSRemotingClient Instructions:");
            Console.WriteLine("Access the .NET Remoting server");
            Console.WriteLine(" -configfile  using a config file");
            Console.WriteLine(" -code        using code");
        }


        #region Access the .NET Remoting server using a configuration file.

        /// <summary>
        /// Access the .NET Remoting server using a configuration file.
        /// </summary>
        static void AccessRemotingServerByConfig()
        {
            // Read the configuration file and configure the remoting 
            // infrastructure for the client project.

            // The format for .NET Remoting configuration file:
            // http://msdn.microsoft.com/en-us/library/ms973907.aspx
            RemotingConfiguration.Configure("CSRemotingClient.exe.config", true);

            try
            {
                //
                // Create a remotable object.
                //

                // Create a SingleCall server-activated proxy.
                SingleCallObject remoteObj = new SingleCallObject();
                Console.WriteLine("A SingleCall server-activated proxy is created");

                // [-or-] Create a Singleton server-activated proxy.
                //SingletonObject remoteObj = new SingletonObject();
                //Console.WriteLine("A Singleton server-activated proxy is created");

                // [-or-] Create a client-activated object.
                //ClientActivatedObject remoteObj = new ClientActivatedObject();
                //Console.WriteLine("A client-activated object is created");

                //
                // Use the remotable object as if it were a local object.
                //

                string remoteType = remoteObj.GetRemoteObjectType();
                Console.WriteLine("Call GetRemoteObjectType => {0}", remoteType);

                Console.WriteLine("The client process and thread: {0}, {1}",
                    Process.GetCurrentProcess().Id, GetCurrentThreadId());

                uint processId;
                uint threadId;
                remoteObj.GetProcessThreadID(out processId, out threadId);
                Console.WriteLine("Call GetProcessThreadID => {0} {1}", processId, threadId);

                Console.WriteLine("Set FloatProperty += {0}", 1.2f);
                remoteObj.FloatProperty += 1.2f;

                Console.WriteLine("Get FloatProperty = {0}", remoteObj.FloatProperty);
            }
            catch (SocketException ex)
            {
                Console.WriteLine(ex.Message);
            }
        }

        #endregion


        #region Access the .NET Remoting server using code.

        /// <summary>
        /// Access the .NET Remoting server using code.
        /// </summary>
        static void AccessRemotingServerByCode()
        {
            // Create and register a channel (TCP channel in this example) that 
            // is used to transport messages across the remoting boundary.

            // Set the properties of the channel.
            IDictionary props = new Hashtable();
            props["typeFilterLevel"] = TypeFilterLevel.Full;

            // Set the formatters of the messages for delivery.
            BinaryClientFormatterSinkProvider clientProvider = new BinaryClientFormatterSinkProvider();
            BinaryServerFormatterSinkProvider serverProvider = new BinaryServerFormatterSinkProvider();
            serverProvider.TypeFilterLevel = TypeFilterLevel.Full;

            // Create a TCP channel.
            TcpChannel tcpChannel = new TcpChannel(props, clientProvider, serverProvider);

            // Register the TCP channel.
            ChannelServices.RegisterChannel(tcpChannel, true);

            try
            {
                //
                // Create a remotable object.
                //

                // Create a SingleCall server-activated proxy.
                SingleCallObject remoteObj = (SingleCallObject)Activator.GetObject(
                    typeof(SingleCallObject),
                    "tcp://localhost:6100/SingleCallService");
                Console.WriteLine("A SingleCall server-activated proxy is created");

                // [-or-] Create a Singleton server-activated proxy
                //SingletonObject remoteObj = (SingletonObject)Activator.GetObject(
                //    typeof(SingletonObject),
                //    "tcp://localhost:6100/SingletonService");
                //Console.WriteLine("A Singleton server-activated proxy is created");

                // [-or-] Create a client-activated object
                //RemotingConfiguration.RegisterActivatedClientType(
                //    typeof(ClientActivatedObject),
                //    "tcp://localhost:6100/RemotingService");
                //ClientActivatedObject remoteObj = new ClientActivatedObject();
                //Console.WriteLine("A client-activated object is created");

                //
                // Use the remotable object as if it were a local object.
                //

                string remoteType = remoteObj.GetRemoteObjectType();
                Console.WriteLine("Call GetRemoteObjectType => {0}", remoteType);

                Console.WriteLine("The client process and thread: {0}, {1}",
                    Process.GetCurrentProcess().Id, GetCurrentThreadId());

                uint processId;
                uint threadId;
                remoteObj.GetProcessThreadID(out processId, out threadId);
                Console.WriteLine("Call GetProcessThreadID => {0} {1}", processId, threadId);

                Console.WriteLine("Set FloatProperty += {0}", 1.2f);
                remoteObj.FloatProperty += 1.2f;

                Console.WriteLine("Get FloatProperty = {0}", remoteObj.FloatProperty);
            }
            catch (SocketException ex)
            {
                Console.WriteLine(ex.Message);
            }
        }

        #endregion


        /// <summary>
        /// Get current thread ID.
        /// </summary>
        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        internal static extern uint GetCurrentThreadId();
    }
}