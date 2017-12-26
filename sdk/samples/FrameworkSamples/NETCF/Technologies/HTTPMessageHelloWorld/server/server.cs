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

namespace Microsoft.Samples.WCF
{ 
    class Server
    {
        static void Main(string[] args)
        {
            // Creating the Binding
            BasicHttpBinding binding = new BasicHttpBinding();

            BindingParameterCollection parameters = 
                new BindingParameterCollection();

            IChannelListener<IReplyChannel> listener = 
                binding.BuildChannelListener<IReplyChannel>
                (new Uri("http://<<ServerAddress>>:<<PortNumber>>"), 
                parameters);

            listener.Open();

            XmlSerializerWrapper wrapper = 
                new XmlSerializerWrapper(typeof(TransmittedObject));

            // Opening the Channel
            IReplyChannel channel = listener.AcceptChannel();
            channel.Open(TimeSpan.MaxValue);

            // Waiting for and receiving the Message
            RequestContext r = 
                channel.ReceiveRequest(TimeSpan.MaxValue);

            TransmittedObject to = 
                r.RequestMessage.GetBody<TransmittedObject>(wrapper);

            // Processing the Message
            to.str = to.str + " World";
            to.i = to.i + 1;

            // Creating the return Message
            Message m = Message.CreateMessage
                (MessageVersion.Soap11, "urn:test", to, wrapper);

            r.Reply(m, TimeSpan.MaxValue);

            // Cleaning up
            channel.Close();
        }
    }

    // Message wrapper class for sending and receiving
    [System.ServiceModel.XmlSerializerFormat]
    [System.SerializableAttribute()]
    [System.Xml.Serialization.XmlTypeAttribute
        (Namespace = "http://Microsoft.ServiceModel.Samples")]
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
