using System;
using System.Collections.Generic;
using System.Text;
using System.Net;


//This project implements an echo socket server.
//The socket server requires four command line parameters:  
//Usage: AsyncSocketServer.exe <#connections> <Receive Size In Bytes> <address family: ipv4 | ipv6> <Local Port Number>

//# Connections: The maximum number of connections the server will accept simultaneously.
//Receive Size in Bytes: The buffer size used by the server for each receive operation.  
//Address family: The address family of the socket the server will use to listen for incoming connections.  Supported values are ‘ipv4’ and ‘ipv6’.
//Local Port Number: The port the server will bind to.

//Example: AsyncSocketServer.exe 500 1024 ipv4 8000



namespace AsyncSocketSample
{
    class Program
    {
        static void Main(string[] args)
        {
            int numConnections;
            int receiveSize;
            IPEndPoint localEndPoint;
            int port;

            // parse command line parameters
            //format: #connections, receive size per connection, address family, port num
            if (args.Length < 4)
            {
                Console.WriteLine("Usage: AsyncSocketServer.exe <#connections> <receiveSizeInBytes> <address family: ipv4 | ipv6> <Local Port Number>");
                return;
            }

            try
            {
                numConnections = int.Parse(args[0]);
                receiveSize = int.Parse(args[1]);
                string addressFamily = args[2].ToLower();
                port = int.Parse(args[3]);


                if (numConnections <= 0)
                {
                    throw new ArgumentException("The number of connections specified must be greater than 0");
                }
                if (receiveSize <= 0)
                {
                    throw new ArgumentException("The receive size specified must be greater than 0");
                }
                if (port <= 0)
                {
                    throw new ArgumentException("The port specified must be greater than 0");
                }

                // This sample supports two address family types: ipv4 and ipv6 
                if (addressFamily.Equals("ipv4"))
                {
                    localEndPoint = new IPEndPoint(IPAddress.Any, port);
                }
                else if (addressFamily.Equals("ipv6"))
                {
                    localEndPoint = new IPEndPoint(IPAddress.IPv6Any, port);
                }
                else
                {
                    throw new ArgumentException("Invalid address family specified");
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                Console.WriteLine("Usage: AsyncSocketServer.exe <#connections> <receiveSizeInBytes> <address family: ipv4 | ipv6> <Local Port Number>");
                return;
            }

            Console.WriteLine("Press any key to start the server ...");
            Console.ReadKey();

            // Start the server listening for incoming connection requests
            Server server = new Server(numConnections, receiveSize);
            server.Init();
            server.Start(localEndPoint);


        }
    }
}
