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

using System;
using System.Linq;
using System.Collections.Generic;
using System.Text;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.Xml.Serialization;
using System.Xml;
using System.Runtime.Serialization;
using System.Diagnostics;
using Microsoft.ServiceModel.Channels.Mail.ExchangeWebService;
using Microsoft.ServiceModel.Channels.Mail;

namespace Microsoft.Samples.WCF
{
    class Server
    {
        static void Main(string[] args)
        {
            // Creating the Channel
            string channelName = "EMailHelloWorld";

            string serverAddress = "server@example.com";
            string serverPWD = "MyPassword";
            string clientAddress = "device@example.com";
            string exchangeServerLocation = "http://example.com";

            ExchangeWebServiceMailBinding binding = 
                new ExchangeWebServiceMailBinding(
                    new Uri(exchangeServerLocation),
                    new System.Net.NetworkCredential(serverAddress, 
                        serverPWD));
            BindingParameterCollection parameters = 
                new BindingParameterCollection();

            IChannelListener<IInputChannel> listener = 
                binding.BuildChannelListener<IInputChannel>
                (MailUriHelper.CreateUri(channelName, ""), parameters);
            listener.Open();

            // Opening the Channel
            IInputChannel inputChannel = listener.AcceptChannel();
            inputChannel.Open(TimeSpan.MaxValue);
            Console.WriteLine("Channel Open");
            
            // Waiting and receiving the Message
            Message reply = inputChannel.Receive(TimeSpan.MaxValue);
            Console.WriteLine("Message Recieved");
            XmlSerializerWrapper wrapper = new XmlSerializerWrapper(
                typeof(TransmittedObject));
            TransmittedObject to = reply.GetBody<TransmittedObject>(wrapper);

            // Processing the Message
            to.str = to.str + " World";
            to.i = to.i + 1;

            Console.WriteLine("Response: " + to.str + " " + to.i.ToString());

            // Creating and returning the Message
            Message m = Message.CreateMessage(binding.MessageVersion, 
                "urn:test", to, wrapper);

            IChannelFactory<IOutputChannel> channelFactory = 
                binding.BuildChannelFactory<IOutputChannel>(parameters);

            channelFactory.Open();

            IOutputChannel outChannel = channelFactory.CreateChannel(
                new EndpointAddress(
                    MailUriHelper.CreateUri(channelName, clientAddress)));
            outChannel.Open();

            Console.WriteLine("Out Channel Open");

            // Sending the Message over the OutChannel
            outChannel.Send(m);

            Console.WriteLine("Message Sent");

            // Cleaning up
            outChannel.Close();
            channelFactory.Close();

            listener.Close();
            inputChannel.Close();
            binding.Close();
        }
    }

    // Message wrapper class for sending and receiving
    [System.ServiceModel.XmlSerializerFormat]
    [System.SerializableAttribute()]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace = 
        "http://Microsoft.ServiceModel.Samples")]
    public class TransmittedObject
    {
        [System.Xml.Serialization.XmlElementAttribute(Order = 0)]
        public string str;

        [System.Xml.Serialization.XmlElementAttribute(Order = 1)]
        public int i;
    }

    // XML Serializer wrapper required for [de]serializing the custom Message
    public sealed class XmlSerializerWrapper : XmlObjectSerializer
    {
        XmlSerializer serializer;
        string defaultNS;
        Type objectType;

        public XmlSerializerWrapper(Type type)
            : this(type, null, null)
        {
        }

        public XmlSerializerWrapper(Type type, string name, string ns)
        {
            this.objectType = type;
            if (!String.IsNullOrEmpty(ns))
            {
                this.defaultNS = ns;
                this.serializer = new XmlSerializer(type, ns);
            }
            else
            {
                this.defaultNS = "";
                this.serializer = new XmlSerializer(type);
            }
        }

        public override bool IsStartObject(XmlDictionaryReader reader)
        {
            throw new NotImplementedException();
        }

        public override object ReadObject(XmlDictionaryReader reader, 
            bool verifyObjectName)
        {
            throw new NotImplementedException();
        }
        public override void WriteEndObject(XmlDictionaryWriter writer)
        {
            throw new NotImplementedException();
        }

        public override void WriteObjectContent(XmlDictionaryWriter writer, 
            object graph)
        {
            throw new NotImplementedException();
        }

        public override void WriteStartObject(XmlDictionaryWriter writer, 
            object graph)
        {
            throw new NotImplementedException();
        }

        public override void WriteObject(XmlDictionaryWriter writer, 
            object graph)
        {
            this.serializer.Serialize(writer, graph);
        }

        public override object ReadObject(XmlDictionaryReader reader)
        {
            string readersNS;

            readersNS = (String.IsNullOrEmpty(reader.NamespaceURI)) ? 
                "" : reader.NamespaceURI;
            if (String.Compare(this.defaultNS, readersNS) != 0)
            {
                this.serializer = new XmlSerializer(this.objectType, 
                    readersNS);
                this.defaultNS = readersNS;
            }

            return (this.serializer.Deserialize(reader));
        }
    }
}
