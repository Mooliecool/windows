/****************************** Module Header ******************************\
* Module Name:              SocketListener.cs
* Project:                  CSSL3SocketServer
* Copyright (c) Microsoft Corporation.
* 
* Implement SocketListener class, which encapsulated the socket, and provide a 
* easy way to listen and return connected sockets.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Threading;
using System.Net.Sockets;

namespace CSSL3SocketServer
{
    public delegate void GetSocketCallBack(Socket sock);
    public class SocketListener
    {
        public void ListenAsync(int port, GetSocketCallBack callback)
        {
            // Run on another thread.
            new Thread(
                new ThreadStart(delegate
                {
                    Listen(port, callback);
                })).Start();
        }
        public void Listen(int port, GetSocketCallBack callback)
        {
            // As a matter of convenience, we use 127.0.0.1 as server socket
            // address. This address is can only be accessed from local.
            // To let server accessible from network, try use machine's network
            // address.

            // 127.0.0.1 address
            IPEndPoint localEP = new IPEndPoint(0x0100007f, port);

            // network ip address.
            //IPHostEntry ipHostInfo = Dns.GetHostEntry(Dns.GetHostName());
            //IPEndPoint localEP = new IPEndPoint(ipHostInfo.AddressList[0], port);

            Socket listener = new Socket(localEP.Address.AddressFamily,
                SocketType.Stream,
                ProtocolType.Tcp);

            try
            {
                listener.Bind(localEP);
                Console.WriteLine("Socket Listener opened: "+localEP);
                while (true)
                {
                    listener.Listen(10);
                    Socket socket = listener.Accept();

                    // Return connected socket through callback function.
                    if (callback != null)
                        callback(socket);
                    else
                    {
                        socket.Close();
                        socket = null;
                    }
                }
            }
            catch (Exception ex)
            {
                Console.Write("Exception occured:" + ex.Message);
            }
            Console.WriteLine("Listener closed: "+localEP);
            listener.Close();
        }
    }
}
