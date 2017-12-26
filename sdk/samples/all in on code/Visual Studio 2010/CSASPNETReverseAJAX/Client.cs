/****************************** Module Header ******************************\
* Module Name:    Client.cs
* Project:        CSASPNETReverseAJAX
* Copyright (c) Microsoft Corporation
*
* Client class is used to synchronize the message sending and the message receiving.
* When DequeueMessage method is called, the method will wait until a new message 
* is inserted by calling EnqueueMessage method. This class benefits ManualResetEvent
* class to achieve the synchronism. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System.Collections.Generic;
using System.Threading;

namespace CSASPNETReverseAJAX
{
    /// <summary>
    /// This class represents a web client which can receive messages.
    /// </summary>
    public class Client
    {
        private ManualResetEvent messageEvent = new ManualResetEvent(false);
        private Queue<Message> messageQueue = new Queue<Message>();

        /// <summary>
        /// This method is called by a sender to send a message to this client.
        /// </summary>
        /// <param name="message">the new message</param>
        public void EnqueueMessage(Message message)
        {
            lock (messageQueue)
            {
                messageQueue.Enqueue(message);

                // Set a new message event.
                messageEvent.Set();
            }
        }

        /// <summary>
        /// This method is called by the client to receive messages from the message queue.
        /// If no message, it will wait until a new message is inserted.
        /// </summary>
        /// <returns>the unread message</returns>
        public Message DequeueMessage()
        {
            // Wait until a new message.
            messageEvent.WaitOne();

            lock (messageQueue)
            {
                if (messageQueue.Count == 1)
                {
                    messageEvent.Reset();
                }
                return messageQueue.Dequeue();
            }
        }
    }
}