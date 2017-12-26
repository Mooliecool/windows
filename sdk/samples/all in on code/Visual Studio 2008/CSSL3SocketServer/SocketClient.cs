/****************************** Module Header ******************************\
* Module Name:              SocketClient.cs
* Project:                  CSSL3SocketServer
* Copyright (c) Microsoft Corporation.
* 
* Implement SocketClient class, which encapsulated the socket, and provide a 
* group of methods to receive/send string type message.
* This file is just the same with SocketClient.cs in CSSL3Socket project.
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
using System.Net.Sockets;
using System.Net;

namespace CSSL3SocketServer
{
    public class SocketMessageEventArgs : EventArgs
    {
        public Exception Error { set; get; }
        public string Data { set; get; }
    }

    public class SocketClient
    {
        // Define 3 events for async operation:
        // Open,Receive and Send
        public event EventHandler<SocketMessageEventArgs> MessageReceived;
        public event EventHandler<SocketMessageEventArgs> MessageSended;
        public event EventHandler<SocketMessageEventArgs> ClientConnected;

        // Set receive buffer size
        static readonly int BUFFER_SIZE = 65536;

        // Define the End-of-message char, which is used for separating
        // byte array into string messages
        static readonly char EOM_MARKER = (char)0x7F;

        // The encapsulated socket
        public Socket InnerSocket { private set; get; }

        
        public SocketClient(Socket socket)
        {
            if (socket==null)
                throw new Exception("Socket cannot be null");
            InnerSocket = socket;

            // Initialize string decoder
            encoding = encoding = new UTF8Encoding(false, true);

        }
        public SocketClient(AddressFamily addfamily, SocketType socktype,ProtocolType protype)
        {
            InnerSocket = new Socket(addfamily, socktype, protype);
            encoding = encoding = new UTF8Encoding(false, true);
        }

        #region Socket async connect

        // Get socket connect status
        public bool Connected
        {
            get
            {
                return InnerSocket.Connected;
            }
        }

        // Close socket
        public void Close()
        {
            InnerSocket.Close();
        }

        /// <summary>
        /// Connect socket to endpoint asynchronously.
        /// Possible exception:
        ///  ArgumentException
        ///  ArgumentNullException
        ///  InvalidOperationException
        ///  SocketException
        ///  NotSupportedException
        ///  ObjectDisposedException
        ///  SecurityException
        ///  Details at: http://msdn.microsoft.com/en-us/library/bb538102.aspx
        /// </summary>
        /// <param name="ep">remote endpoint</param>
        public void ConnectAsync(EndPoint ep)
        {
            if (InnerSocket.Connected)
                return;

            // Initialize socketAsyncEventArgs
            // Set remote connect endpoint
            var connectEventArgs = new SocketAsyncEventArgs();
            connectEventArgs.RemoteEndPoint = ep;
            connectEventArgs.Completed += new EventHandler<SocketAsyncEventArgs>(connectEventArgs_Completed);

            // Call ConnectAsync method, if method returned false
            // it means the result has returned synchronously
            if (!InnerSocket.ConnectAsync(connectEventArgs))
            // Call method to handle connect result
                ProcessConnect(connectEventArgs);
        }

        // When connectAsync completed, call method to handle connect result
        void connectEventArgs_Completed(object sender, SocketAsyncEventArgs e)
        {
            ProcessConnect(e);
        }

        // Invoke ClientConnected event to return result 
        void ProcessConnect(SocketAsyncEventArgs e)
        {
            if (e.SocketError == SocketError.Success)
                OnClientConnected(null);
            else
                OnClientConnected(new SocketException((int)e.SocketError));
        }

        void OnClientConnected(Exception error)
        {
            if(ClientConnected!=null)
            {
                ClientConnected(this, new SocketMessageEventArgs
                {
                    Error = error
                });
            }
        }
        #endregion

        #region Socket async Send

        /// <summary>
        /// Use Socket to send string message.
        /// Possible exception:
        ///  FormatException
        ///  ArgumentException
        ///  InvalidOperationException
        ///  NotSupportedException
        ///  ObjectDisposedException
        ///  SocketException
        /// </summary>
        /// <param name="data">message to be sent</param>
        public void SendAsync(string data)
        {
            // If message data contains EOM_MARKER char,
            // throw exception
            if (data.Contains(EOM_MARKER))
                throw new Exception("Unallowed chars existed in message");

            // Add End-of-message char at message end.
            data += EOM_MARKER;

            // Get UTF8 encoded byte array
            var bytesdata = encoding.GetBytes(data);

            // Initialize SendEventArgs
            var sendEventArgs = new SocketAsyncEventArgs();
            sendEventArgs.SetBuffer(bytesdata, 0, bytesdata.Length);
            sendEventArgs.Completed += new EventHandler<SocketAsyncEventArgs>(sendEventArgs_Completed);

            // Call SendAsync method, if method returned false
            // it means the result has returned synchronously
            if (!InnerSocket.SendAsync(sendEventArgs))
                ProcessSend(sendEventArgs);
        }

        // When sendAsync completed, call method to handle send result
        void sendEventArgs_Completed(object sender, SocketAsyncEventArgs e)
        {
            ProcessSend(e);
        }

        // Invoke MessageSended event to return result 
        void ProcessSend(SocketAsyncEventArgs e)
        {
            if (e.SocketError == SocketError.Success)
                OnMessageSended(null);
            else
                OnMessageSended(
                    new SocketException((int)e.SocketError));
        }

        void OnMessageSended(Exception error)
        {
            if (MessageSended != null)
                MessageSended(this, new SocketMessageEventArgs
                {
                    Error = error
                });
        }
        #endregion

        #region Socket async Receive

        // Define flag to indicate receive status
        bool _isReceiving;

        /// <summary>
        /// Start receiving bytes from socket and invoke
        /// MessageReceived event when each message received.
        /// Possible exception:
        ///  ArgumentException
        ///  InvalidOperationException
        ///  NotSupportedException
        ///  ObjectDisposedException
        ///  SocketException
        ///  Details at http://msdn.microsoft.com/en-us/library/system.net.sockets.socket.receiveasync.aspx
        ///  </summary>
        public void StartReceiving()
        {
            
            // Check if socket is started receiving already
            if (!_isReceiving)
                _isReceiving = true;
            else
                return;

            try
            {
                // Initialize receiving buffer
                var buffer = new byte[BUFFER_SIZE];

                // Initialize receive event args
                var receiveEventArgs = new SocketAsyncEventArgs();
                receiveEventArgs.SetBuffer(new byte[BUFFER_SIZE], 0, BUFFER_SIZE);
                receiveEventArgs.Completed += new EventHandler<SocketAsyncEventArgs>(receiveEventArgs_Completed);

                // Call ReceiveAsync method, if method returned false
                // it means the result has returned synchronously
                if (!InnerSocket.ReceiveAsync(receiveEventArgs))
                    ProcessReceive(receiveEventArgs);
            }
            catch (Exception ex)
            {
                StopReceiving();
                throw ex;
            }
        }

        // Stop receiving bytes from socket
        public void StopReceiving()
        {
            _isReceiving = false;
        }

        void receiveEventArgs_Completed(object sender, SocketAsyncEventArgs e)
        {
            ProcessReceive(e);
        }

        // Process receiveAsync complete event
        string receivemessage = "";
        Encoding encoding;
        int taillength;
        private void ProcessReceive(SocketAsyncEventArgs e)
        {
            // When got Error, invoke MessageReceived event
            // to pass the error info to user
            if (e.SocketError != SocketError.Success)
            {
                StopReceiving();
                OnMessageReceived(null,
                    new SocketException((int)e.SocketError));
                return;
            }

            try
            {
                #region String Decoding
                // Decoding bytes to string.
                // Note that UTF-8 is variable-length encode, we need check the byte
                // array tail in case of separating one character into two.
                string receivestr = "";
                // Try decode string
                try
                { 
                    receivestr = encoding.GetString(e.Buffer, 0, taillength + e.BytesTransferred);
                    // If decode successful, reset tail length
                    taillength=0;
                }
                // If got decode exception, remove the array tail and re decode
                catch (DecoderFallbackException ex)
                {
                    try{
                        receivestr = encoding.GetString(e.Buffer,0,taillength+e.BytesTransferred-ex.BytesUnknown.Length);
                        // reset tail length
                        taillength=ex.BytesUnknown.Length;
                        ex.BytesUnknown.CopyTo(e.Buffer,0);
                    }
                    // If still got decode exception, stop receiving.
                    catch(DecoderFallbackException ex2)
                    {
                        throw new Exception("Message decode failed.",ex2);
                    }

                #endregion
                }
                // Check if message ended
                int eompos = receivestr.IndexOf(EOM_MARKER);
                while (eompos != -1)
                {
                    // Compose a complete message
                    receivemessage += receivestr.Substring(0, eompos);

                    // Notify message received
                    OnMessageReceived(receivemessage, null);

                    // Get the remaining string
                    receivemessage = "";
                    receivestr = receivestr.Substring(eompos + 1, receivestr.Length - eompos - 1);

                    // Check if it still has EOM in string
                    eompos = receivestr.IndexOf(EOM_MARKER);
                }
                receivemessage += receivestr;

                // Stop receiving.
                if (!_isReceiving)
                    return;

                // Reset buffer offset
                e.SetBuffer(taillength, BUFFER_SIZE-taillength);

                // Keep receiving
                if (!InnerSocket.ReceiveAsync(e))
                    ProcessReceive(e);
            }
            catch (Exception ex)
            {
                // Return error through MessageReceived event
                OnMessageReceived(null, ex);
                StopReceiving();
            }
        }

        void OnMessageReceived(string data,Exception error)
        {
            if (MessageReceived != null)
                MessageReceived(this, new SocketMessageEventArgs
                {
                    Data = data,
                    Error = error
                });
        }
        #endregion
    }
}
