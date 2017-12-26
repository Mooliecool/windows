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
namespace Microsoft.Samples.WCF.UptimeMonitor {

    using System;
    using System.Runtime.Serialization;
    using System.Xml.Serialization;
    using System.Diagnostics.CodeAnalysis;

    /// <summary>
    /// Represents a query for the server's uptime information.
    /// </summary>
    [SuppressMessage("Microsoft.Design", "CA1053:StaticHolderTypesShouldNotHaveConstructors",
        Justification = "The request is empty now, but it could have members added, so it's not inherently a static class.")]
    public class UptimeRequest {
        /// <summary>
        /// The port that the server will listen for requests on,
        /// and that the client will send those requests to.
        /// </summary>
        public const int ListeningPort = 50388;
    }

    /// <summary>
    /// Represents the server's response to the uptime request.
    /// </summary>
    public class UptimeResponse {
        DateTime timestamp = DateTime.Now;
        /// <summary>
        /// The time this response was sent.  
        /// Its field is automatically initialized the first time.
        /// </summary>
        public DateTime Timestamp {
            get { return timestamp; }
            set { timestamp = value; }
        }
        /// <summary>
        /// The duration that the server has been running uninterrupted.
        /// </summary>
        /// <remarks>
        /// We use the XmlIgnoreAttribute here because TimeSpan's do not 
        /// serialize correctly under the XmlSerializer.  Instead,
        /// we store the TimeSpan value as a number of seconds
        /// for serialization purposes.
        /// </remarks>
        [XmlIgnore]
        public TimeSpan Uptime {
            get { return TimeSpan.FromSeconds(UptimeInSeconds); }
            set { UptimeInSeconds = value.TotalSeconds; }
        }
        double uptimeInSeconds;
        /// <summary>
        /// The total number of seconds that the server has been running.  
        /// </summary>
        /// <remarks>
        /// This acts as the data store for the <see cref="Uptime"/> property
        /// as a System.Double will serialize to xml whereas a System.TimeSpan
        /// will not.
        /// </remarks>
        [XmlElement]
        public double UptimeInSeconds {
            get { return uptimeInSeconds; }
            set { uptimeInSeconds = value; }
        }
    }

    /// <summary>
    /// The WCF serialization adapter for the XmlSerializer.
    /// </summary>
    /// <remarks>
    /// This class is largely unimplemented, because WCF really only needs
    /// the ReadObject and WriteObject methods for how we use it here.
    /// </remarks>
    class MessageSerializer : XmlObjectSerializer {
        /// <summary>
        /// The serializer to use for creating uptime request messages.
        /// </summary>
        [SuppressMessage("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal static MessageSerializer RequestSerializer =
            new MessageSerializer(typeof(UptimeRequest));
        /// <summary>
        /// The serializer to use for creating uptime response messages.
        /// </summary>
        [SuppressMessage("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal static MessageSerializer ResponseSerializer =
            new MessageSerializer(typeof(UptimeResponse));

        XmlSerializer serializer;

        /// <summary>
        /// Constructs a WCF serializer.
        /// </summary>
        /// <remarks>
        /// We leave it as a private constructor here because we expose static members
        /// for the only two instance we'll need of this class.
        /// </remarks>
        MessageSerializer(Type type) {
            serializer = new XmlSerializer(type);
        }

        public override bool IsStartObject(System.Xml.XmlDictionaryReader reader) {
            throw new NotImplementedException();
        }

        public override object ReadObject(System.Xml.XmlDictionaryReader reader, bool verifyObjectName) {
            return serializer.Deserialize(reader);
        }

        public override void WriteObject(System.Xml.XmlDictionaryWriter writer, object graph) {
            serializer.Serialize(writer, graph);
        }

        public override void WriteEndObject(System.Xml.XmlDictionaryWriter writer) {
            throw new NotImplementedException();
        }

        public override void WriteObjectContent(System.Xml.XmlDictionaryWriter writer, object graph) {
            throw new NotImplementedException();
        }

        public override void WriteStartObject(System.Xml.XmlDictionaryWriter writer, object graph) {
            throw new NotImplementedException();
        }
    }
}