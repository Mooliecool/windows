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
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.ServiceModel;
using System.ServiceModel.Description;
using System.ServiceModel.Channels;
using System.Xml;
using System.Xml.Serialization;
using System.Runtime.Serialization;

namespace Microsoft.Samples.WCF
{

    public partial class Client : Form
    {
        public Client()
        {
            InitializeComponent();
        }

        private void sendButton_Click(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;

            // Creating the message to send
            TransmittedObject to = new TransmittedObject();
            to.str = "Hello";
            to.i = 5;

            XmlSerializerWrapper wrapper =
                new XmlSerializerWrapper(typeof(TransmittedObject));

            Message m = Message.CreateMessage
                (MessageVersion.Soap11, "urn:test", to, wrapper);

            // Creating the channel to send the message
            BasicHttpBinding binding = new BasicHttpBinding();
            BindingParameterCollection parameters =
                new BindingParameterCollection();
            IChannelFactory<IRequestChannel> channelFactory =
                binding.BuildChannelFactory<IRequestChannel>(parameters);

            channelFactory.Open();

            // Opening the channel
            IRequestChannel outChannel =
                channelFactory.CreateChannel
                (new EndpointAddress
                (new Uri("http://<<ServerAddress>>:<<PortNumber>>")));

            outChannel.Open(TimeSpan.MaxValue);

            // Sending the Message and waiting for the reply
            Message reply = outChannel.Request(m, TimeSpan.MaxValue);

            // Deserializing and using the Message
            TransmittedObject to1 =
                reply.GetBody<TransmittedObject>
                (new XmlSerializerWrapper(typeof(TransmittedObject)));

            MessageBox.Show(to1.str + " " + to1.i.ToString());

            // Cleaning up
            outChannel.Close();
            channelFactory.Close();

            Cursor.Current = Cursors.Default;
        }

        private void menuItemExit_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }
    }

    // Message wrapper class for sending and receiving
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