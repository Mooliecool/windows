using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;

//Implements a sample socket client to connect to the server implmented in the AsyncSocketServer project
//Usage: AsyncSocketClient.exe <destination IP address> <destination port number>

//Destination IP address: The IP Address of the server to connect to
//Destination Port Number: The port number to connect to

namespace AsyncSocketClient
{
    class Program
    {
        static ManualResetEvent clientDone = new ManualResetEvent(false);

        static void Main(string[] args)
        {
            IPAddress destinationAddr = null;          // IP Address of server to connect to
            int destinationPort = 0;                   // Port number of server
            SocketAsyncEventArgs socketEventArg = new SocketAsyncEventArgs();
            

            if (args.Length != 2)
            {
                Console.WriteLine("Usage: AsyncSocketClient.exe <destination IP address> <destination port number>");
            }
            try
            {
                destinationAddr = IPAddress.Parse(args[0]);
                destinationPort = int.Parse(args[1]);
                if (destinationPort <= 0){
                    throw new ArgumentException("Destination port number provided cannot be less than or equal to 0");
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                Console.WriteLine("Usage: AsyncSocketClient.exe <destination IP address> <destination port number>");              
            }

            // Create a socket and connect to the server
            Socket sock = new Socket(destinationAddr.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
            socketEventArg.Completed += new EventHandler<SocketAsyncEventArgs>(SocketEventArg_Completed);
            socketEventArg.RemoteEndPoint = new IPEndPoint(destinationAddr, destinationPort);
            socketEventArg.UserToken = sock;
            sock.ConnectAsync(socketEventArg);
            clientDone.WaitOne();
        }

        /// <summary>
        /// A single callback is used for all socket operations. This method forwards execution on to the correct handler 
        /// based on the type of completed operation
        /// </summary>
        static void SocketEventArg_Completed(object sender, SocketAsyncEventArgs e)
        {
            switch (e.LastOperation)
            {
                case SocketAsyncOperation.Connect:
                    ProcessConnect(e);
                    break;
                case SocketAsyncOperation.Receive:
                    ProcessReceive(e);
                    break;
                case SocketAsyncOperation.Send:
                    ProcessSend(e);
                    break;
                default:
                    throw new Exception("Invalid operation completed");
            }
        }

        /// <summary>
        /// Called when a ConnectAsync operation completes
        /// </summary>
        private static void ProcessConnect(SocketAsyncEventArgs e)
        {
            if (e.SocketError == SocketError.Success)
            {
                Console.WriteLine("Successfully connected to the server");

                // Send 'Hello World' to the server
                byte[] buffer = Encoding.UTF8.GetBytes("Hello World");
                e.SetBuffer(buffer, 0, buffer.Length);
                Socket sock = e.UserToken as Socket;
                bool willRaiseEvent = sock.SendAsync(e);
                if (!willRaiseEvent)
                {
                    ProcessSend(e);
                }
            }
            else
            {
                throw new SocketException((int)e.SocketError);
            }
        }

        /// <summary>
        /// Called when a ReceiveAsync operation completes
        /// </summary>
        private static void ProcessReceive(SocketAsyncEventArgs e)
        {
            if (e.SocketError == SocketError.Success)
            {
                Console.WriteLine("Received from server: {0}", Encoding.UTF8.GetString(e.Buffer, 0, e.BytesTransferred));
                
                // Data has now been sent and received from the server. Disconnect from the server
                Socket sock = e.UserToken as Socket;
                sock.Shutdown(SocketShutdown.Send);
                sock.Close();
                clientDone.Set();
            }
            else
            {
                throw new SocketException((int)e.SocketError);
            }
        }


        /// <summary>
        /// Called when a SendAsync operation completes
        /// </summary>
        private static void ProcessSend(SocketAsyncEventArgs e)
        {
            if (e.SocketError == SocketError.Success)
            {
                Console.WriteLine("Sent 'Hello World' to the server");

                //Read data sent from the server
                Socket sock = e.UserToken as Socket;
                bool willRaiseEvent = sock.ReceiveAsync(e);
                if (!willRaiseEvent)
                {
                    ProcessReceive(e);
                }
            }
            else
            {
                throw new SocketException((int)e.SocketError);
            }
        }
    }
}
