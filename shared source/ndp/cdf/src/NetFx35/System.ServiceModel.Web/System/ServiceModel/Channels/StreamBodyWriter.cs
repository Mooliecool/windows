//----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------------------
namespace System.ServiceModel.Channels
{
    using System;
    using System.IO;
    using System.Runtime;
    using System.ServiceModel;
    using System.Xml;
    using DiagnosticUtility = System.ServiceModel.DiagnosticUtility;

    public abstract class StreamBodyWriter : BodyWriter
    {
 
        protected StreamBodyWriter(bool isBuffered)
            : base(isBuffered)
        { }

        internal static StreamBodyWriter CreateStreamBodyWriter(Action<Stream> streamAction)
        {
            if (streamAction == null)
            {
                throw DiagnosticUtility.ExceptionUtility.ThrowHelperArgumentNull("actionOfStream");
            }
            return new ActionOfStreamBodyWriter(streamAction);
        }

        protected abstract void OnWriteBodyContents(Stream stream);

        protected override BodyWriter OnCreateBufferedCopy(int maxBufferSize)
        {
            using (BufferManagerOutputStream bufferedStream = new BufferManagerOutputStream(SR2.MaxReceivedMessageSizeExceeded, maxBufferSize))
            {
                this.OnWriteBodyContents(bufferedStream);
                int size;
                byte[] bytesArray = bufferedStream.ToArray(out size);
                return new BufferedBytesStreamBodyWriter(bytesArray, size);
            }
        }

        protected override void OnWriteBodyContents(XmlDictionaryWriter writer)
        {
            using (XmlWriterBackedStream stream = new XmlWriterBackedStream(writer))
            {
                OnWriteBodyContents(stream);
            }
        }

        class XmlWriterBackedStream : Stream
        {
            private const string StreamElementName = "Binary";

            XmlWriter writer;
            
            public XmlWriterBackedStream(XmlWriter writer)
            {
                if (writer == null)
                {
                    throw DiagnosticUtility.ExceptionUtility.ThrowHelperArgumentNull("writer");
                }
                this.writer = writer;
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

            public override void Flush()
            {
                this.writer.Flush();
            }

            public override long Length
            {
                get
                {
                    throw DiagnosticUtility.ExceptionUtility.ThrowHelperWarning(new InvalidOperationException(SR2.GetString(SR2.XmlWriterBackedStreamPropertyGetNotSupported, "Length")));
                }
            }

            public override long Position
            {
                get
                {
                    throw DiagnosticUtility.ExceptionUtility.ThrowHelperWarning(new InvalidOperationException(SR2.GetString(SR2.XmlWriterBackedStreamPropertyGetNotSupported, "Position")));
                }
                set
                {
                    throw DiagnosticUtility.ExceptionUtility.ThrowHelperWarning(new InvalidOperationException(SR2.GetString(SR2.XmlWriterBackedStreamPropertySetNotSupported, "Position")));
                }
            }

            public override int Read(byte[] buffer, int offset, int count)
            {
                throw DiagnosticUtility.ExceptionUtility.ThrowHelperWarning(new InvalidOperationException(SR2.GetString(SR2.XmlWriterBackedStreamMethodNotSupported, "Read")));
            }

            public override IAsyncResult BeginRead(byte[] buffer, int offset, int count, AsyncCallback callback, object state)
            {
                throw DiagnosticUtility.ExceptionUtility.ThrowHelperWarning(new InvalidOperationException(SR2.GetString(SR2.XmlWriterBackedStreamMethodNotSupported, "BeginRead")));
            }

            public override int EndRead(IAsyncResult asyncResult)
            {
                throw DiagnosticUtility.ExceptionUtility.ThrowHelperWarning(new InvalidOperationException(SR2.GetString(SR2.XmlWriterBackedStreamMethodNotSupported, "EndRead")));
            }

            public override int ReadByte()
            {
                throw DiagnosticUtility.ExceptionUtility.ThrowHelperWarning(new InvalidOperationException(SR2.GetString(SR2.XmlWriterBackedStreamMethodNotSupported, "ReadByte")));
            }

            public override long Seek(long offset, SeekOrigin origin)
            {
                throw DiagnosticUtility.ExceptionUtility.ThrowHelperWarning(new InvalidOperationException(SR2.GetString(SR2.XmlWriterBackedStreamMethodNotSupported, "Seek")));
            }

            public override void SetLength(long value)
            {
                throw DiagnosticUtility.ExceptionUtility.ThrowHelperWarning(new InvalidOperationException(SR2.GetString(SR2.XmlWriterBackedStreamMethodNotSupported, "SetLength")));
            }

            public override void Write(byte[] buffer, int offset, int count)
            {
                if (writer.WriteState == WriteState.Start)
                {
                    writer.WriteStartElement(StreamElementName, string.Empty);
                    this.writer.WriteBase64(buffer, offset, count);
                }
                else if (writer.WriteState == WriteState.Content)
                {
                    this.writer.WriteBase64(buffer, offset, count);
                }
            }
        }

        class BufferedBytesStreamBodyWriter : StreamBodyWriter
        {
            byte[] array;
            int size;

            public BufferedBytesStreamBodyWriter(byte[] array, int size)
                : base(true)
            {
                this.array = array;
                this.size = size;
            }

            protected override void OnWriteBodyContents(Stream stream)
            {
                stream.Write(this.array, 0, this.size);
            }
        }

        class ActionOfStreamBodyWriter : StreamBodyWriter
        {
            Action<Stream> actionOfStream;

            public ActionOfStreamBodyWriter(Action<Stream> actionOfStream)
                : base(false)
            {
                this.actionOfStream = actionOfStream;
            }

            protected override void OnWriteBodyContents(Stream stream)
            {
                actionOfStream(stream);
            }
        }
    }
}
