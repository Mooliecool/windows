//-----------------------------------------------------------------
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.
//-----------------------------------------------------------------

using System;
using System.Messaging;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.Transactions;

namespace Microsoft.Samples.RoutingAdvancedErrorHandling
{

    //extremely basic System.Messaging (MSMQ) utility class
    class QueueUtils
    {

        [ServiceContract(SessionMode = SessionMode.NotAllowed)]
        public interface IQueueContract
        {
            [OperationContract(Action = "*", IsOneWay = true)]
            void OneWayMessage(System.ServiceModel.Channels.Message msg);
        }

        public interface IQueueContractClient : IQueueContract, IClientChannel { }

        public static void CreateQueue(string queuePath, bool isTransactional)
        {
            if (MessageQueue.Exists(queuePath))
            {
                DeleteQueue(queuePath);
            }

            System.Console.WriteLine("Creating Queue '{0}'", queuePath);
            using (MessageQueue wcfQueue = MessageQueue.Create(queuePath, isTransactional))
            {
                wcfQueue.UseJournalQueue = true;
                System.Console.WriteLine("Queue created successfully");
            }
        }

        public static void DeleteQueue(string queuePath)
        {
            if (MessageQueue.Exists(queuePath))
            {
                try
                {
                    MessageQueue.Delete(queuePath);
                }
                catch (Exception)
                {
                }
            }
        }

        public static void PurgeQueue(string queuePath)
        {

            try
            {
                using (MessageQueue queue = new MessageQueue(queuePath))
                {
                    queue.Purge();
                }

            }
            catch (Exception)
            {
            }

        }

        public static void InsertMessageIntoQueue(string queuePath)
        {
            MsmqTransportBindingElement msmqbe = new MsmqTransportBindingElement();
            msmqbe.MsmqTransportSecurity.MsmqAuthenticationMode = MsmqAuthenticationMode.None;
            msmqbe.MsmqTransportSecurity.MsmqProtectionLevel = System.Net.Security.ProtectionLevel.None;

            Binding QueueBinding = new CustomBinding(new BinaryMessageEncodingBindingElement(), msmqbe);

            IQueueContractClient client = ChannelFactory<IQueueContractClient>.CreateChannel(QueueBinding, new EndpointAddress(queuePath));
            System.ServiceModel.Channels.Message msg = System.ServiceModel.Channels.Message.CreateMessage(QueueBinding.MessageVersion, "MSMQMessage", (object)"bodyData");

            using (TransactionScope ts = new TransactionScope())
            {
                
                client.OneWayMessage(msg);
                ts.Complete();
            }

            client.Close();
            System.Console.WriteLine("Inserted one message into the  queue at {0}", queuePath);
        }

        public static int GetMessageCount(string queuePath)
        {
            try
            {
                using (MessageQueue queue = new MessageQueue(queuePath))
                {
                    queue.MessageReadPropertyFilter.SetAll();

                    System.Messaging.Message[] messageArray = queue.GetAllMessages();
                    foreach (System.Messaging.Message m in messageArray)
                    {
                        m.Dispose();
                    }
                    return messageArray.Length;
                }
            }
            catch (MessageQueueException)
            {
                return -1;
            }
        }

        public static bool QueueExists(string queuePath)
        {
            try
            {
                return MessageQueue.Exists(queuePath);
            }
            catch (Exception)
            {
                return false;
            }
            
        }
    }
}
