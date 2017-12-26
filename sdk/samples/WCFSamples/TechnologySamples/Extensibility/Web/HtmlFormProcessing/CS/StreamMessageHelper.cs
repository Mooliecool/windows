//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Generic;
using System.IO;
using System.ServiceModel.Channels;
using System.Text;
using System.Xml;

namespace Microsoft.WebProgrammingModel.Samples
{
    public class StreamMessageHelper
    {
        public static Message CreateMessage(MessageVersion version, string action, string contentType, StreamWriterDelegate writer)
        {
            DelegateBodyWriter bodyWriter = new DelegateBodyWriter(writer);

            Message message = Message.CreateMessage(version, action, bodyWriter);
            message.Properties.Add(WebBodyFormatMessageProperty.Name, new WebBodyFormatMessageProperty(WebContentFormat.Raw));

            HttpResponseMessageProperty response = new HttpResponseMessageProperty();
            response.Headers[System.Net.HttpResponseHeader.ContentType] = contentType;  
            message.Properties.Add(HttpResponseMessageProperty.Name, response);

            return message;
        }

        public static Message CreateMessage(MessageVersion version, string action, string contentType, Stream stream)
        {
            StreamBodyWriter bodyWriter = new StreamBodyWriter(stream);

            Message message = Message.CreateMessage(version, action, bodyWriter);
            message.Properties.Add(WebBodyFormatMessageProperty.Name, new WebBodyFormatMessageProperty(WebContentFormat.Raw));

            HttpResponseMessageProperty response = new HttpResponseMessageProperty();
            response.Headers[System.Net.HttpResponseHeader.ContentType] = contentType;
            message.Properties.Add(HttpResponseMessageProperty.Name, response);


            return message;
        }

        public static Stream GetStream(Message message)
        {
            XmlDictionaryReader reader = message.GetReaderAtBodyContents();
            return new XmlReaderStream(reader);
        }
    }

    class DelegateBodyWriter : BodyWriter
    {
        StreamWriterDelegate writer;

        public DelegateBodyWriter(StreamWriterDelegate writer)
            : base(false)
        {
            this.writer = writer;
        }

        protected override void OnWriteBodyContents(XmlDictionaryWriter writer)
        {
            writer.WriteStartElement("Binary");

            XmlWriterStream stream = new XmlWriterStream(writer);
            this.writer(stream);
            stream.Close();

            writer.WriteEndElement();
        }
    }

    class StreamBodyWriter : BodyWriter
    {
        public const string StreamElementName = "Binary";
        const int BufferSize = 1024;

        Stream stream;

        public StreamBodyWriter(Stream stream)
            : base(false)
        {
            this.stream = stream;
        }

        protected override void OnWriteBodyContents(XmlDictionaryWriter writer)
        {
            writer.WriteStartElement(StreamBodyWriter.StreamElementName);

            byte[] buffer = new byte[BufferSize];
            while (true)
            {
                int actual = this.stream.Read(buffer, 0, buffer.Length);
                if (actual == 0)
                {
                    break;
                }
                writer.WriteBase64(buffer, 0, actual);
            }

            //this.stream.Close();

            writer.WriteEndElement();
        }
    }

    public delegate void StreamWriterDelegate(Stream output);

    internal class XmlWriterStream : Stream
    {
        XmlDictionaryWriter innerWriter;

        internal XmlWriterStream(XmlDictionaryWriter xmlWriter)
        {
            this.innerWriter = xmlWriter;
        }

        public override bool CanRead
        {
            get { return false; }
        }

        public override bool CanSeek
        {
            get { return false; }
        }

        public override bool CanWrite
        {
            get { return true; }
        }

        public override long Length
        {
            get { throw new NotSupportedException(); }
        }

        public override long Position
        {
            get { throw new NotSupportedException(); }
            set { throw new NotSupportedException(); }
        }

        public override void Flush()
        {
            //noop
        }

        public override int Read(byte[] buffer, int offset, int count)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public override long Seek(long offset, SeekOrigin origin)
        {
            throw new NotSupportedException();
        }

        public override void SetLength(long value)
        {
            throw new NotSupportedException();
        }

        public override void Write(byte[] buffer, int offset, int count)
        {
            this.innerWriter.WriteBase64(buffer, offset, count);
        }
    }

    internal class XmlReaderStream : Stream
    {
        XmlDictionaryReader innerReader;

        internal XmlReaderStream(XmlDictionaryReader xmlReader)
        {
            this.innerReader = xmlReader;
            this.innerReader.ReadStartElement("Binary");
        }

        public override bool CanRead
        {
            get { return true; }
        }

        public override bool CanSeek
        {
            get { return false; }
        }

        public override bool CanWrite
        {
            get { return false; }
        }

        public override long Length
        {
            get { throw new NotSupportedException(); }
        }

        public override long Position
        {
            get { throw new NotSupportedException(); }
            set { throw new NotSupportedException(); }
        }

        public override void Flush()
        {
            //noop
        }

        public override int Read(byte[] buffer, int offset, int count)
        {
            return this.innerReader.ReadContentAsBase64(buffer, offset, count);
        }

        public override long Seek(long offset, SeekOrigin origin)
        {
            throw new NotSupportedException();
        }

        public override void SetLength(long value)
        {
            throw new NotSupportedException();
        }

        public override void Write(byte[] buffer, int offset, int count)
        {
            throw new Exception("The method or operation is not implemented.");
        }
    }
}
