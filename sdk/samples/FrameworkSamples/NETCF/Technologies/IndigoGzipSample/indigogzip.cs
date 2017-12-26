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
using System.IO;
using System.IO.Compression;
using System.ServiceModel.Channels;

// This is an Indigo Extensibility Sample that uses the Gzip Encoder 
// in the System.IO.Compression Namespace to provide an Indigo Channel 
// that Compresses Indigo Messages passed to it.

namespace Microsoft.Samples.Indigo.GzipEncoder
{
    //This class is used to create the custom encoder (CompressionMessageEncoder)
    public class CompressionMessageEncoderFactory : MessageEncoderFactory
    {
        #region Private fields
        //The factory will always return the same encoder
        MessageEncoder encoder;
        #endregion

        #region Constructor
        //The compression encoder wraps an inner encoder
        //We require a factory to be passed in that will create this inner encoder
        public CompressionMessageEncoderFactory(MessageEncoderFactory messageEncoderFactory)
        {
            if (messageEncoderFactory == null)
                throw new ArgumentNullException("messageEncoderFactory", "A valid message encoder factory must be passed to the CompressionEncoder");
            encoder = new CompressionMessageEncoder(messageEncoderFactory.Encoder);

        }
        #endregion

        #region Properties
        //The service framework uses this property to obtain an encoder from this encoder factory
        public override MessageEncoder Encoder
        {
            get { return encoder; }
        }

        public override MessageVersion MessageVersion
        {
            get { return encoder.MessageVersion; }
        }
        #endregion
    }

    //This is the actual compression encoder
    class CompressionMessageEncoder :MessageEncoder 
    {
        #region Private fields
        static string compressionContentType = "application/x-gzip";

        //This implementation wraps an inner encoder that actually converts a WCF Message
        //into textual XML, binary XML or some other format. This implementation then compresses the results.
        //The opposite happens when reading messages.
        //This member stores this inner encoder.
        MessageEncoder innerEncoder;
        #endregion

        #region Constructor
        //We require an inner encoder to be supplied (see comment above)
        internal CompressionMessageEncoder(MessageEncoder messageEncoder)
            : base()
        {
            if (messageEncoder == null)
                throw new ArgumentNullException("messageEncoder", "A valid message encoder must be passed to the CompressionEncoder");
            innerEncoder = messageEncoder;
        }
        #endregion

        #region Properties

        public override string ContentType
        {
            get { return compressionContentType; }
        }

        public override string MediaType
        {
            get { return compressionContentType; }
        }

        //SOAP version to use - we delegate to the inner encoder for this
        public override MessageVersion MessageVersion
        {
            get { return innerEncoder.MessageVersion; }
        }
        #endregion

        #region Buffer compression and decompression
        
        //Helper method to compress an array of bytes
        static ArraySegment<byte> CompressBuffer(ArraySegment<byte> buffer, BufferManager bufferManager, int messageOffset)
        {
            MemoryStream memoryStream = new MemoryStream();
            memoryStream.Write(buffer.Array, 0, messageOffset);

            using (GZipStream gzStream = new GZipStream(memoryStream, CompressionMode.Compress, true))
            {
                gzStream.Write(buffer.Array, messageOffset, buffer.Count);
            }


            byte[] compressedBytes = memoryStream.ToArray();
            byte[] bufferedBytes = bufferManager.TakeBuffer(compressedBytes.Length);

            Array.Copy(compressedBytes, 0, bufferedBytes, 0, compressedBytes.Length);

            bufferManager.ReturnBuffer(buffer.Array);
            ArraySegment<byte> byteArray = new ArraySegment<byte>(bufferedBytes, messageOffset, bufferedBytes.Length - messageOffset);

            return byteArray;
        }

        //Helper method to decompress an array of bytes
        static ArraySegment<byte> DecompressBuffer(ArraySegment<byte> buffer, BufferManager bufferManager)
        {
            MemoryStream memoryStream = new MemoryStream(buffer.Array, buffer.Offset, buffer.Count - buffer.Offset);
            MemoryStream decompressedStream = new MemoryStream();
            int totalRead = 0;
            int blockSize = 1024;
            byte[] tempBuffer = bufferManager.TakeBuffer(blockSize);
            using (GZipStream gzStream = new GZipStream(memoryStream, CompressionMode.Decompress))
            {
                while (true)
                {
                    int bytesRead = gzStream.Read(tempBuffer, 0, blockSize);
                    if (bytesRead == 0)
                        break;
                    decompressedStream.Write(tempBuffer, 0, bytesRead);
                    totalRead += bytesRead;
                }
            }
            bufferManager.ReturnBuffer(tempBuffer);

            byte[] decompressedBytes = decompressedStream.ToArray();
            byte[] bufferManagerBuffer = bufferManager.TakeBuffer(decompressedBytes.Length + buffer.Offset);
            Array.Copy(buffer.Array, 0, bufferManagerBuffer, 0, buffer.Offset);
            Array.Copy(decompressedBytes, 0, bufferManagerBuffer, buffer.Offset, decompressedBytes.Length);

            ArraySegment<byte> byteArray = new ArraySegment<byte>(bufferManagerBuffer, buffer.Offset, decompressedBytes.Length);
            bufferManager.ReturnBuffer(buffer.Array);

            return byteArray;
        }
        #endregion

        
        #region Buffered message handling
        //One of the two main entry points into the encoder. Called by WCF to decode a buffered byte array into a Message.
        public override Message ReadMessage(ArraySegment<byte> buffer, BufferManager bufferManager, string contentType)
        {
            //Decompress the buffer
            ArraySegment<byte> decompressedBuffer = DecompressBuffer(buffer, bufferManager);

            //Use the inner encoder to decode the decompressed buffer
            Message returnMessage = innerEncoder.ReadMessage(decompressedBuffer, bufferManager);

            //Display the Compressed and uncompressed sizes
            Console.WriteLine("GZipCompressed message is {0} bytes", buffer.Count);
            Console.WriteLine("Decompressed message is {0} bytes", decompressedBuffer.Count);
            
            returnMessage.Properties.Encoder = this;
            return returnMessage;
        }

        //One of the two main entry points into the encoder. Called by WCF to encode a Message into a buffered byte array.
        public override ArraySegment<byte> WriteMessage(Message message, int maxMessageSize, BufferManager bufferManager, int messageOffset)
        {
            //Use the inner encoder to encode a Message into a buffered byte array
            ArraySegment<byte> buffer = innerEncoder.WriteMessage(message, maxMessageSize, bufferManager, messageOffset);
            //Compress the resulting byte array
            ArraySegment<byte> compressedBuffer = CompressBuffer(buffer, bufferManager, messageOffset);

            //Display the Compressed and uncompressed sizes
            Console.WriteLine("Original message is {0} bytes", buffer.Count);
            Console.WriteLine("GZipCompressed message is {0} bytes", compressedBuffer.Count);


            return compressedBuffer;
        }
        #endregion

        //Streaming equivalents of the above methods are not implemented
        #region Streaming message handling
       public override Message ReadMessage(System.IO.Stream stream, int maxSizeOfHeaders, string contentType)
        {
           throw new NotSupportedException();
        }


        public override void WriteMessage(Message message, System.IO.Stream stream)
        {
            throw new NotSupportedException();
        }
        #endregion

    }


    //This is the binding element that, when plugged into a custom binding, will enable the compression encoder
    public sealed class CompressionMessageEncodingBindingElement : MessageEncodingBindingElement 
    {
        #region Private fields
        //We will use an inner binding element to store information required for the inner encoder
        MessageEncodingBindingElement innerBindingElement;
        #endregion

        #region Constructor
        //By default, use the default text encoder as the inner encoder
        public CompressionMessageEncodingBindingElement()
            : this(new TextMessageEncodingBindingElement()) { }

        public CompressionMessageEncodingBindingElement(MessageEncodingBindingElement messageEncoderBindingElement)
        {
            this.innerBindingElement = messageEncoderBindingElement;
        }
        #endregion

        #region Properties
        public MessageEncodingBindingElement InnerMessageEncodingBindingElement
        {
            get { return innerBindingElement; }
            set { innerBindingElement = value; }
        }
        #endregion

        #region MessageEncodingBindingElement methods
        //Main entry point into the encoder binding element. Called by WCF to get the factory that will create the
        //message encoder
        public override MessageEncoderFactory CreateMessageEncoderFactory()
        {
            return new CompressionMessageEncoderFactory(innerBindingElement.CreateMessageEncoderFactory());
        }
       
        public override MessageVersion MessageVersion
        {
            get { return innerBindingElement.MessageVersion; }
            set { innerBindingElement.MessageVersion = value; }
        }
        #endregion

        #region BindingElement overrides
        public override BindingElement Clone()
        {
            return new CompressionMessageEncodingBindingElement(this.innerBindingElement);
        }
        public override T GetProperty<T>(BindingContext context)
        {
            return context.GetInnerProperty<T>();
        }

        public override IChannelFactory<TChannel> BuildChannelFactory<TChannel>(BindingContext context)
        {
            if (context == null)
                throw new ArgumentNullException("context");

            context.BindingParameters.Add(this);
            return context.BuildInnerChannelFactory<TChannel>();
        }

        public override IChannelListener<TChannel> BuildChannelListener<TChannel>(BindingContext context)
        {
            if (context == null)
                throw new ArgumentNullException("context");

            context.BindingParameters.Add(this);
            return context.BuildInnerChannelListener<TChannel>();
        }

        public override bool CanBuildChannelListener<TChannel>(BindingContext context)
        {
            if (context == null)
                throw new ArgumentNullException("context");

            context.BindingParameters.Add(this);
            return context.CanBuildInnerChannelListener<TChannel>();
        }
        #endregion
      
    }


}
