//------------------------------------------------------------------------------
// <copyright file="_ConnectStream.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>
//------------------------------------------------------------------------------

namespace System.Net {
    using System.Diagnostics;
    using System.IO;
    using System.Net.Sockets;
    using System.Runtime.InteropServices;
    using System.Threading;
    using System.Security.Permissions;
    using System.ComponentModel;

    internal struct WriteHeadersCallbackState{
        internal HttpWebRequest request;
        internal ConnectStream stream;

        internal WriteHeadersCallbackState(HttpWebRequest request, ConnectStream stream){
            this.request = request;
            this.stream = stream;
        }
    }

    /*++

        ConnectStream  - a stream interface to a Connection object.

        This class implements the Stream interface, as well as a
        WriteHeaders call. Inside this stream we handle details like
        chunking, dechunking and tracking of ContentLength. To write
        or read data, we call a method on the connection object. The
        connection object is responsible for protocol stuff that happens
        'below' the level of the HTTP request, for example MUX or SSL

    --*/

    internal class ConnectStream : Stream, ICloseEx
    {
#if DEBUG
        internal IAsyncResult       _PendingResult;
#endif
        
        private static class Nesting {
            public const int Idle           = 0;
            public const int IoInProgress   = 1;    // we are doing read or write
            public const int Closed         = 2;    // stream was closed if that is done in IoInProgress on write, the write will resume delayed close part.
            public const int InError        = 3;    // IO is not allowed due to error stream state
            public const int InternalIO     = 4;    // stream is used by us, this is internal error if public IO sees that value
        }

        private     int             m_CallNesting;          // see Nesting enum for details
        private     ScatterGatherBuffers
                                    m_BufferedData;        // list of sent buffers in case of resubmit (redirect/authentication).
        private     bool            m_SuppressWrite;           // don't write data to the connection, only buffer it
        private     bool            m_BufferOnly;           // don't write data to the connection, only buffer it
        private     long            m_BytesLeftToWrite;     // Total bytes left to be written.
        private     int             m_BytesAlreadyTransferred;  // Bytes already read/written in the current operation.
        private     Connection      m_Connection;           // Connection for I/O.
        private     byte[]          m_ReadBuffer;           // Read buffer for read stream.
        private     int             m_ReadOffset;           // Offset into m_ReadBuffer.
        private     int             m_ReadBufferSize;       // Bytes left in m_ReadBuffer.
        private     long            m_ReadBytes;            // Total bytes to read on stream, -1 for read to end.
        private     bool            m_Chunked;              // True if we're doing chunked read.
        private     int             m_DoneCalled;           // 0 at init, 1 after we've called Read/Write Done
        private     int             m_ShutDown;             // 0 at init, 1 after we've called Complete
        private     Exception       m_ErrorException;       // non-null if we've seen an error on this connection.
        private     bool            m_ChunkEofRecvd;        // True, if we've seen an EOF, or reached a EOF state for no more reads
        private     int             m_ChunkSize;            // Number of bytes in current chunk.
        private     byte[]          m_TempBuffer;           // A temporary buffer.
        private     bool            m_ChunkedNeedCRLFRead;  // true - when we need to read a /r/n before a chunk size
        private     bool            m_Draining;             // true - when we're draining. needed to handle chunked draining.

        private     HttpWriteMode   m_HttpWriteMode;

        private     int             m_ReadTimeout;          // timeout in ms for reads
        private     int             m_WriteTimeout;         // timeout in ms for writes

        private const long c_MaxDrainBytes = 64 * 1024; // 64 K - greater than, we should just close the connection

        private static readonly WaitCallback  m_ReadChunkedCallbackDelegate = new WaitCallback(ReadChunkedCallback);
        private static readonly AsyncCallback m_ReadCallbackDelegate = new AsyncCallback(ReadCallback);
        private static readonly AsyncCallback m_WriteCallbackDelegate = new AsyncCallback(WriteCallback);
        private static readonly AsyncCallback m_WriteHeadersCallback = new AsyncCallback(WriteHeadersCallback);

        // Special value indicating that an asynchronous read operation is intentionally zero-length.
        private static readonly object ZeroLengthRead = new object();

        private HttpWebRequest m_Request;

        //
        // Timeout - timeout in ms for sync reads & writes, passed in HttpWebRequest
        //

        public override bool CanTimeout {
            get {return true;}
        }

        public override int ReadTimeout {
            get {
                return m_ReadTimeout;
            }
            set {
                if (value<=0 && value!=System.Threading.Timeout.Infinite) {
                    throw new ArgumentOutOfRangeException(SR.GetString(SR.net_io_timeout_use_gt_zero));
                }
                m_ReadTimeout = value;
            }
        }

        public override int WriteTimeout {
            get {
                return m_WriteTimeout;

            }
            set {
                if (value<=0 && value!=System.Threading.Timeout.Infinite) {
                    throw new ArgumentOutOfRangeException(SR.GetString(SR.net_io_timeout_use_gt_zero));
                }
                m_WriteTimeout = value;
            }
        }

        //
        // If IgnoreSocketErrors==true then no data will be sent to the wire
        //
        private bool m_IgnoreSocketErrors;
        internal bool IgnoreSocketErrors {
            get {
                return m_IgnoreSocketErrors;
            }
        }

        //
        // If the KeepAlive=true then we  must be prepares for a write socket errors trying to flush the body
        // If the KeepAlive=false then we should cease body writing as the connection is probably dead
        // If fatal=true then the connection is dead due to IO fault (discovered during read), throw IO exception
        //
        // m_IgnoreSocketErrors and m_ThrowSocketError are mostly for a write type of streams.
        // However a response read stream may have this member set when draning a response on resubmit.
        //
        // This this isn't synchronized, we also check after receiving an exception from the transport whether these have been set
        // and take them into account if they have (on writes).
        private bool m_ErrorResponseStatus;
        internal void ErrorResponseNotify(bool isKeepAlive) {
            m_ErrorResponseStatus = true;
            m_IgnoreSocketErrors |= !isKeepAlive;
            GlobalLog.Print((WriteStream?"Write-":"Read-") + "ConnectStream#"+ ValidationHelper.HashString(this) + "::Got notification on an Error Response, m_IgnoreSocketErrors:" + m_IgnoreSocketErrors);
        }

        // This means we should throw a connection closed exception from now on (write only).
        // It's unclear whether this needs to be better synchronized with m_ErrorResponseStatus, such as if ErrorResponseNotify
        // were called (asynchronously) while a m_ErrorException was already set.
        internal void FatalResponseNotify()
        {
            if (m_ErrorException == null)
            {
                Interlocked.CompareExchange<Exception>(ref m_ErrorException, new IOException(SR.GetString(SR.net_io_readfailure, SR.GetString(SR.net_io_connectionclosed))), null);
            }
            m_ErrorResponseStatus = false;
            GlobalLog.Print((WriteStream ? "Write-" : "Read-") + "ConnectStream#" + ValidationHelper.HashString(this) + "::Got notification on a Fatal Response");
        }

        /*++
            Write Constructor for this class. This is the write constructor;
            it takes as a parameter the amount of entity body data to be written,
            with a value of -1 meaning to write it out as chunked. The other
            parameter is the Connection of which we'll be writing.

            Right now we use the DefaultBufferSize for the stream. In
            the future we'd like to pass a 0 and have the stream be
            unbuffered for write.

            Input:

                Conn            - Connection for this stream.
                BytesToWrite    - Total bytes to be written, or -1
                                    if we're doing chunked encoding.

            Returns:

                Nothing.

        --*/

        public ConnectStream(Connection connection, HttpWebRequest request) {
            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::.ctor(Write)");
            m_Connection = connection;
            m_ReadTimeout = m_WriteTimeout = System.Threading.Timeout.Infinite;
            //
            // we need to save a reference to the request for two things
            // 1. In case of buffer-only we kick in actual submition when the stream is closed by a user
            // 2. In case of write stream abort() we notify the request so the response stream is handled properly
            //
            m_Request = request;
            m_HttpWriteMode = request.HttpWriteMode;

            GlobalLog.Assert(m_HttpWriteMode != HttpWriteMode.Unknown, "ConnectStream#{0}::.ctor()|HttpWriteMode:{1}", ValidationHelper.HashString(this), m_HttpWriteMode);
            m_BytesLeftToWrite = m_HttpWriteMode==HttpWriteMode.ContentLength ? request.ContentLength : -1;
            if (request.HttpWriteMode==HttpWriteMode.Buffer) {
                m_BufferOnly = true;
                EnableWriteBuffering();
            }
            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::.ctor() Connection:" + ValidationHelper.HashString(m_Connection) + " BytesToWrite:" + BytesLeftToWrite);
        }

        /*++

            Read constructor for this class. This constructor takes in
            the connection and some information about a buffer that already
            contains data. Reads from this stream will read first from the
            buffer, and after that is exhausted will read from the connection.

            We also take in a size of bytes to read, or -1 if we're to read
            to connection close, and a flag indicating whether or now
            we're chunked.

            Input:

                Conn                - Connection for this stream.
                buffer              - Initial buffer to read from.
                offset              - offset into buffer to start reading.
                size               - number of bytes in buffer to read.
                readSize            - Number of bytes allowed to be read from
                                        the stream, -1 for read to connection
                                        close.
                chunked             - True if we're doing chunked decoding.

            Returns:

                Nothing.

        --*/

        public ConnectStream(Connection connection, byte[] buffer, int offset, int bufferCount, long readCount, bool chunked, HttpWebRequest request)
        {
            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::.ctor(Read)");
            if(Logging.On)Logging.PrintInfo(Logging.Web, this, "ConnectStream", SR.GetString(SR.net_log_buffered_n_bytes, readCount));

            m_ReadBuffer = buffer;
            m_ReadOffset = offset;
            m_ReadBufferSize = bufferCount;
            m_ReadBytes = readCount;
            m_ReadTimeout = m_WriteTimeout = System.Threading.Timeout.Infinite;
            m_Chunked = chunked;
            m_Connection = connection;
            m_TempBuffer = new byte[2];
            //
            // A request reference is used to verify (by the connection class) that this request should start a next one on Close.
            //
            m_Request = request;
            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::.ctor() Connection:" + ValidationHelper.HashString(m_Connection) +
                            " m_ReadOffset:"   + m_ReadOffset + " m_ReadBufferSize: " + m_ReadBufferSize +
                            " ContentLength: " + m_ReadBytes  + " m_Chunked:" + m_Chunked.ToString());
        }

        internal void SwitchToContentLength(){
            m_HttpWriteMode = HttpWriteMode.ContentLength;
        }

        internal bool SuppressWrite {
            /* Consider Removing 
            get {
                return m_SuppressWrite;
            }
            */
            set{
                m_SuppressWrite = value;
            }
        }
        
        internal Connection Connection {
            get {
                return m_Connection;
            }
        }

        internal bool BufferOnly {
            get {
                return m_BufferOnly;
            }
        }

        internal ScatterGatherBuffers BufferedData {
            get {
                return m_BufferedData;
            }
            set {
                m_BufferedData = value;
            }
        }

        private bool WriteChunked {
            get {
                return m_HttpWriteMode==HttpWriteMode.Chunked;
            }
        }

        internal long BytesLeftToWrite {
            get {
                return m_BytesLeftToWrite;
            }
        }

        // True if this is a write stream.
        bool WriteStream {
            get {
                return m_HttpWriteMode != HttpWriteMode.Unknown;
            }
        }

        internal bool IsPostStream {
            get {
                return m_HttpWriteMode != HttpWriteMode.None;
            }
        }

        /*++

            ErrorInStream - indicates an exception was caught
            internally due to a stream error, and that I/O
            operations should not continue

            Input: Nothing.

            Returns: True if there is an error

         --*/

        private bool ErrorInStream {
            get {
                return m_ErrorException!=null;
            }
        }

        /*++

            CallDone - calls out to the Connection that spawned this
            Stream (using the DoneRead/DoneWrite method).
            If the Connection specified that we don't need to
            do this, or if we've already done this already, then
            we return silently.

            Input: Nothing.

            Returns: Nothing.

         --*/
        internal void CallDone()
        {
            CallDone(null);
        }
        private void CallDone(ConnectionReturnResult returnResult)
        {
            GlobalLog.Enter("ConnectStream#" + ValidationHelper.HashString(this) + "::CallDone");
            if ( Interlocked.Increment( ref m_DoneCalled) == 1 )
            {
                if (!WriteStream)
                {
#if DEBUG
                    GlobalLog.DebugRemoveRequest(m_Request);
#endif
                    if (returnResult == null) {
                        //readstartnextrequest will call setresponses internally.
                        m_Connection.ReadStartNextRequest(m_Request, ref returnResult);
                    }
                    else{
                        ConnectionReturnResult.SetResponses(returnResult);
                    }
                }
                else
                {
                    m_Request.WriteCallDone(this, returnResult);
                }
            }
            GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(this) + "::CallDone");
        }

        internal void ProcessWriteCallDone(ConnectionReturnResult returnResult)
        {
            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::ProcessWriteCallDone()");

            try {
                if (returnResult == null) {
                    m_Connection.WriteStartNextRequest(m_Request, ref returnResult);

                    // If the request is Sync, then we do our Read here for data
                    if (!m_Request.Async)
                    {
                        object syncReaderResult = m_Request.ConnectionReaderAsyncResult.InternalWaitForCompletion();

                        //we should only do a syncread if we didn't already read the response
                        //via poll when we handed back the request stream
                        if (syncReaderResult == null && !m_Request.SawInitialResponse)
#if DEBUG
                            // Remove once mixed sync/async requests are supported.
                            using (GlobalLog.SetThreadKind(ThreadKinds.Sync))
#endif
                        {
                            m_Connection.SyncRead(m_Request, true, false);
                        }
                    }

                    m_Request.SawInitialResponse = false;
                }

                ConnectionReturnResult.SetResponses(returnResult);
            }
            finally {
                // This will decrement the response window on the write side AND may
                // result in either immediate or delayed processing of a response for the m_Request instance
                if (IsPostStream || m_Request.Async)
                    m_Request.CheckWriteSideResponseProcessing();
            }
        }

        internal bool IsClosed {
            get {
                return m_ShutDown != 0;
            }
        }

        /*++

            Read property for this class. We return the readability of
            this stream. This is a read only property.

            Input: Nothing.

            Returns: True if this is a read stream, false otherwise.

         --*/

        public override bool CanRead {
            get {
                return !WriteStream && !IsClosed;
            }
        }

        /*++

            Seek property for this class. Since this stream is not
            seekable, we just return false. This is a read only property.

            Input: Nothing.

            Returns: false

         --*/

        public override bool CanSeek {
            get {
                return false;
            }
        }

        /*++

            CanWrite property for this class. We return the writeability of
            this stream. This is a read only property.

            Input: Nothing.

            Returns: True if this is a write stream, false otherwise.

         --*/

        public override bool CanWrite {
            get {
                return WriteStream && !IsClosed;
            }
        }


        /*++

            DataAvailable property for this class. This property check to see
            if at least one byte of data is currently available. This is a read
            only property.

            Input: Nothing.

            Returns: True if data is available, false otherwise.

         --*/
        /*
        //                                    
        public bool DataAvailable {
            get {
                //
                // Data is available if this is not a write stream and either
                // we have data buffered or the underlying connection has
                // data.
                //
                return !WriteStream && (m_ReadBufferSize != 0 || m_Connection.DataAvailable);
            }
        }
        */

        /*++

            Length property for this class. Since we don't support seeking,
            this property just throws a NotSupportedException.

            Input: Nothing.

            Returns: Throws exception.

         --*/

        public override long Length {
            get {
                throw new NotSupportedException(SR.GetString(SR.net_noseek));
            }
        }

        /*++

            Position property for this class. Since we don't support seeking,
            this property just throws a NotSupportedException.

            Input: Nothing.

            Returns: Throws exception.

         --*/

        public override long Position {
            get {
                throw new NotSupportedException(SR.GetString(SR.net_noseek));
            }

            set {
                throw new NotSupportedException(SR.GetString(SR.net_noseek));
            }
        }


        /*++

            Eof property to indicate when the read is no longer allowed,
            because all data has been already read from socket.

            Input: Nothing.

            Returns: true/false depending on whether we are complete

         --*/

        private bool Eof {
            get {
                if (ErrorInStream) {
                    return true;
                }
                else if (m_Chunked) {
                    return m_ChunkEofRecvd;
                }
                else if (m_ReadBytes == 0) {
                    return true;
                }
                else if (m_ReadBytes == -1) {
                    return(m_DoneCalled > 0 && m_ReadBufferSize <= 0);
                }
                else {
                    return false;
                }
            }
        }

        /* Consider Removing
        internal bool IsEverythingBuffered {
            get {
                return m_ReadBytes >= 0 && m_ReadBufferSize == m_ReadBytes;
            }
        }
        */

        /*++
            Uses an old Stream to resubmit buffered data using the current
             stream, this is used in cases of POST, or authentication,
             where we need to buffer upload data so that it can be resubmitted

            Input:

                OldStream - Old Stream that was previously used

            Returns:

                Nothing.

        --*/

        internal void ResubmitWrite(ConnectStream oldStream, bool suppressWrite) {
            GlobalLog.Enter("ConnectStream#" + ValidationHelper.HashString(this) + "::ResubmitWrite", ValidationHelper.HashString(oldStream));
            GlobalLog.ThreadContract(ThreadKinds.Sync, "ConnectStream#" + ValidationHelper.HashString(this) + "::ResubmitWrite");

            //

            //
            try {
                Interlocked.CompareExchange(ref m_CallNesting, Nesting.InternalIO, Nesting.Idle);
                GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::ResubmitWrite() Inc: " + m_CallNesting.ToString());

                GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::ResubmitWrite(), callNesting : " + m_CallNesting.ToString() + " IsClosed = " + IsClosed);
                //
                // no need to buffer here:
                // we're already resubmitting buffered data give it to the connection to put it on the wire again
                // we set BytesLeftToWrite to 0 'cause even on failure there can be no recovery,
                // so just leave it to IOError() to clean up and don't call ResubmitWrite()
                //
                ScatterGatherBuffers bufferedData = oldStream.BufferedData;
                SafeSetSocketTimeout(SocketShutdown.Send);
                if (!WriteChunked) {
                    if (!suppressWrite)
                        m_Connection.Write(bufferedData);
                }
                else {
                    // we have the data buffered, but we still want to chunk.

                    // first set this to disable Close() from sending a chunk terminator.
                    GlobalLog.Assert(m_HttpWriteMode != HttpWriteMode.None, "ConnectStream#{0}::ResubmitWrite()|m_HttpWriteMode == HttpWriteMode.None", ValidationHelper.HashString(this));
                    m_HttpWriteMode = HttpWriteMode.ContentLength;

                    if (bufferedData.Length==0) {
                        m_Connection.Write(NclConstants.ChunkTerminator, 0, NclConstants.ChunkTerminator.Length);
                    }
                    else {
                        int chunkHeaderOffset = 0;
                        byte[] chunkHeaderBuffer = GetChunkHeader(bufferedData.Length, out chunkHeaderOffset);
                        BufferOffsetSize[] dataBuffers = bufferedData.GetBuffers();
                        BufferOffsetSize[] buffers = new BufferOffsetSize[dataBuffers.Length + 3];
                        buffers[0] = new BufferOffsetSize(chunkHeaderBuffer, chunkHeaderOffset, chunkHeaderBuffer.Length - chunkHeaderOffset, false);
                        int index = 0;
                        foreach (BufferOffsetSize buffer in dataBuffers) {
                            buffers[++index] = buffer;
                        }
                        buffers[++index] = new BufferOffsetSize(NclConstants.CRLF, 0, NclConstants.CRLF.Length, false);
                        buffers[++index] = new BufferOffsetSize(NclConstants.ChunkTerminator, 0, NclConstants.ChunkTerminator.Length, false);

                        SplitWritesState splitState = new SplitWritesState(buffers);

                        BufferOffsetSize[] sendBuffers = splitState.GetNextBuffers();
                        while(sendBuffers != null){
                            m_Connection.MultipleWrite(sendBuffers);
                            sendBuffers = splitState.GetNextBuffers();
                        }
                    }
                }
                if(Logging.On && bufferedData.GetBuffers() != null) {
                    foreach (BufferOffsetSize bufferOffsetSize in bufferedData.GetBuffers()) {
                        if (bufferOffsetSize == null) {
                            Logging.Dump(Logging.Web, this, "ResubmitWrite", null, 0, 0);
                        }
                        else {
                            Logging.Dump(Logging.Web, this, "ResubmitWrite", bufferOffsetSize.Buffer, bufferOffsetSize.Offset, bufferOffsetSize.Size);
                        }
                    }
                }
                GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::ResubmitWrite() sent:" + bufferedData.Length.ToString() );
            }
            catch (Exception exception)
            {
                if (NclUtilities.IsFatal(exception)) throw;

                // A Fatal error
                WebException we = new WebException(NetRes.GetWebStatusString("net_connclosed", WebExceptionStatus.SendFailure),
                                               WebExceptionStatus.SendFailure,
                                               WebExceptionInternalStatus.RequestFatal,
                                               exception);
                IOError(we, false);
            }
            finally {
                Interlocked.CompareExchange(ref m_CallNesting, Nesting.Idle, Nesting.InternalIO);
                GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::ResubmitWrite(), callNesting : " + m_CallNesting.ToString() + " IsClosed = " + IsClosed);
            }
            m_BytesLeftToWrite = 0;
            CallDone();
            GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(this) + "::ResubmitWrite", BytesLeftToWrite.ToString());
        }


        //
        // called by HttpWebRequest if AllowWriteStreamBuffering is true on that instance
        //
        internal void EnableWriteBuffering() {
            GlobalLog.Assert(WriteStream, "ConnectStream#{0}::EnableWriteBuffering()|!WriteStream", ValidationHelper.HashString(this));
            if (BufferedData==null) {
                // create stream on demand, only if needed
                GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::EnableWriteBuffering() Write() creating ScatterGatherBuffers WriteChunked:" + WriteChunked.ToString() + " BytesLeftToWrite:" + BytesLeftToWrite.ToString());
                if (WriteChunked)
                {
                    BufferedData = new ScatterGatherBuffers();
                }
                else
                {
                    BufferedData = new ScatterGatherBuffers(BytesLeftToWrite);
                }
            }
        }

        /*++
            FillFromBufferedData - This fills in a buffer from data that we have buffered.

            This method pulls out the buffered data that may have been provided as
            excess actual data from the header parsing

            Input:

                buffer          - Buffer to read into.
                offset          - Offset in buffer to read into.
                size           - Size in bytes to read.

            Returns:
                Number of bytes read.

        --*/
        private int FillFromBufferedData(byte [] buffer, ref int offset, ref int size ) {
            //
            // if there's no stuff in our read buffer just return 0
            //
            if (m_ReadBufferSize == 0) {
                return 0;
            }

            //
            // There's stuff in our read buffer. Figure out how much to take,
            // which is the minimum of what we have and what we're to read,
            // and copy it out.
            //
            int BytesTransferred = Math.Min(size, m_ReadBufferSize);

            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::FillFromBufferedData() Filling bytes: " + BytesTransferred.ToString());

            Buffer.BlockCopy(
                m_ReadBuffer,
                m_ReadOffset,
                buffer,
                offset,
                BytesTransferred);

            // Update our internal read buffer state with what we took.

            m_ReadOffset += BytesTransferred;
            m_ReadBufferSize -= BytesTransferred;

            // If the read buffer size has gone to 0, null out our pointer
            // to it so maybe it'll be garbage-collected faster.

            if (m_ReadBufferSize == 0) {
                m_ReadBuffer = null;
            }

            // Update what we're to read and the caller's offset.

            size -= BytesTransferred;
            offset += BytesTransferred;

            return BytesTransferred;
        }

        /*++
            Write

            This function write data to the network. If we were given a definite
            content length when constructed, we won't write more than that amount
            of data to the network. If the caller tries to do that, we'll throw
            an exception. If we're doing chunking, we'll chunk it up before
            sending to the connection.


            Input:

                buffer          - buffer to write.
                offset          - offset in buffer to write from.
                size           - size in bytes to write.

            Returns:
                Nothing.

        --*/
        public override void Write(byte[] buffer, int offset, int size) {
#if DEBUG
            using (GlobalLog.SetThreadKind(ThreadKinds.User | ThreadKinds.Sync)) {
#endif
            if (Logging.On) Logging.Enter(Logging.Web, this, "Write", "");
            //
            // Basic parameter validation
            //
            if (!WriteStream) {
                throw new NotSupportedException(SR.GetString(SR.net_readonlystream));
            }
            if (buffer==null) {
                throw new ArgumentNullException("buffer");
            }
            if (offset<0 || offset>buffer.Length) {
                throw new ArgumentOutOfRangeException("offset");
            }
            if (size<0 || size>buffer.Length-offset) {
                throw new ArgumentOutOfRangeException("size");
            }

            InternalWrite(false, buffer, offset, size, null, null );
            if(Logging.On)Logging.Dump(Logging.Web, this, "Write", buffer, offset, size);

            if(Logging.On)Logging.Exit(Logging.Web, this, "Write", "");
#if DEBUG
            }
#endif
        }



        /*++
            BeginWrite - Does async write to the Stream

            Splits the operation into two outcomes, for the
            non-chunking case, we calculate size to write,
            then call BeginWrite on the Connection directly,
            and then we're finish, for the Chunked case,
            we procede with use two callbacks to continue the
            chunking after the first write, and then second write.
            In order that all of the Chunk data/header/terminator,
            in the correct format are sent.

            Input:

                buffer          - Buffer to write into.
                offset          - Offset in buffer to write into.
                size           - Size in bytes to write.
                callback        - the callback to be called on result
                state           - object to be passed to callback

            Returns:
                IAsyncResult    - the async result

        --*/


        [HostProtection(ExternalThreading=true)]
        public override IAsyncResult BeginWrite(byte[] buffer, int offset, int size, AsyncCallback callback, object state ) {
#if DEBUG
            using (GlobalLog.SetThreadKind(ThreadKinds.User | ThreadKinds.Async)) {
#endif
            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::BeginWrite " + ValidationHelper.HashString(m_Connection) + ", " + offset.ToString() + ", " + size.ToString());
            if(Logging.On)Logging.Enter(Logging.Web, this, "BeginWrite", "");
            //
            // Basic parameter validation
            //
            if (!WriteStream) {
                throw new NotSupportedException(SR.GetString(SR.net_readonlystream));
            }
            if (buffer==null) {
                throw new ArgumentNullException("buffer");
            }
            if (offset<0 || offset>buffer.Length) {
                throw new ArgumentOutOfRangeException("offset");
            }
            if (size<0 || size>buffer.Length-offset) {
                throw new ArgumentOutOfRangeException("size");
            }

            IAsyncResult result = InternalWrite(true, buffer, offset, size, callback, state);
            if(Logging.On)Logging.Exit(Logging.Web, this, "BeginWrite", result);
            return result;
#if DEBUG
            }
#endif
        }

        //
        // Handles either async or sync Writing for *public* stream API
        //
        private IAsyncResult InternalWrite(bool async, byte[] buffer, int offset, int size, AsyncCallback callback, object state ) {
            //
            // if we have a stream error, or we've already shut down this socket
            //  then we must prevent new BeginRead/BeginWrite's from getting
            //  submited to the socket, since we've already closed the stream.
            //
            if (ErrorInStream) {
                GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::InternalWrite() throwing:" + m_ErrorException.ToString());
                throw m_ErrorException;
            }

            if (IsClosed && !IgnoreSocketErrors) {
                GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::InternalWrite() throwing");
                throw new WebException(
                            NetRes.GetWebStatusString("net_requestaborted", WebExceptionStatus.ConnectionClosed),
                            WebExceptionStatus.ConnectionClosed);
            }
            
            if (m_Request.Aborted && !IgnoreSocketErrors) {
                GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::InternalWrite() throwing");
                throw new WebException(
                    NetRes.GetWebStatusString("net_requestaborted", WebExceptionStatus.RequestCanceled),
                    WebExceptionStatus.RequestCanceled);
            }             

            int nesting = Interlocked.CompareExchange(ref m_CallNesting, Nesting.IoInProgress, Nesting.Idle);
            GlobalLog.Print((async?"Async ":"") + "InternalWrite() In: callNesting : " + nesting.ToString());
            if (nesting != Nesting.Idle && nesting != Nesting.Closed)
            {
                throw new NotSupportedException(SR.GetString(SR.net_no_concurrent_io_allowed));
            }

            //
            // buffer data to the ScatterGatherBuffers
            // regardles of chunking, we buffer the data as if we were not chunking
            // and on resubmit, we don't bother chunking.
            //
            if (BufferedData!=null && size != 0 && (m_Request.ContentLength != 0 || !IsPostStream || !m_Request.NtlmKeepAlive)) {
                //
                // if we don't need to, we shouldn't send data on the wire as well
                // but in this case we gave a stream to the user so we have transport
                //
                BufferedData.Write(buffer, offset, size);
            }

            LazyAsyncResult asyncResult = null;
            bool completeSync = false;
            try
            {
                if (size == 0 || BufferOnly || m_SuppressWrite || IgnoreSocketErrors)
                {
                    //
                    // We're not putting this data on the wire, then we're done
                    //
                    if(m_SuppressWrite && m_BytesLeftToWrite > 0 && size > 0)
                    {
                        m_BytesLeftToWrite -= size;
                    }

                    GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::InternalWrite() swallowing: size==0 || BufferOnly || IgnoreSocketErrors= " + (size==0) + BufferOnly + IgnoreSocketErrors);
                    if (async) {
                        asyncResult = new LazyAsyncResult(this, state, callback);
                        completeSync = true;
                    }
                    return asyncResult;
                }
                else if (WriteChunked) {
                    //
                    // We're chunking. Write the chunk header out first,
                    // then the data, then a CRLF.
                    // for this we'll use BeginMultipleSend();
                    //
                    int chunkHeaderOffset = 0;
                    byte[] chunkHeaderBuffer = GetChunkHeader(size, out chunkHeaderOffset);

                    BufferOffsetSize[] buffers;
                    GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::InternalWrite() m_ErrorResponseStatus:" + m_ErrorResponseStatus);

                    if (m_ErrorResponseStatus) {
                        //if we already got a (>200) response, then just terminate chunking and
                        //switch to simple buffering (if any)
                        GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::InternalWrite() setting m_IgnoreSocketErrors to True (was:" + m_IgnoreSocketErrors + ") sending chunk terminator");
                        m_IgnoreSocketErrors = true;
                        buffers = new BufferOffsetSize[1];
                        buffers[0] = new BufferOffsetSize(NclConstants.ChunkTerminator, 0, NclConstants.ChunkTerminator.Length, false);
                    }
                    else {
                        buffers = new BufferOffsetSize[3];
                        buffers[0] = new BufferOffsetSize(chunkHeaderBuffer, chunkHeaderOffset, chunkHeaderBuffer.Length - chunkHeaderOffset, false);
                        buffers[1] = new BufferOffsetSize(buffer, offset, size, false);
                        buffers[2] = new BufferOffsetSize(NclConstants.CRLF, 0, NclConstants.CRLF.Length, false);
                    }

                    asyncResult = (async) ? new NestedMultipleAsyncResult(this, state, callback, buffers) : null;

                    //
                    // after setting up the buffers and error checking do the async Write Call
                    //

                    try {
                        if (async) {
                            m_Connection.BeginMultipleWrite(buffers, m_WriteCallbackDelegate, asyncResult);
                        }
                        else {
                            SafeSetSocketTimeout(SocketShutdown.Send);
                            m_Connection.MultipleWrite(buffers);
                        }
                    }

                    catch (Exception exception) {
                        // IgnoreSocketErrors can be set at any time - need to check it again.
                        if (IgnoreSocketErrors && !NclUtilities.IsFatal(exception))
                        {
                            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::InternalWrite() swallowing: IgnoreSocketErrors set after throw.");
                            if (async)
                            {
                                completeSync = true;
                            }
                            return asyncResult;
                        }

                        if (m_Request.Aborted && (exception is IOException || exception is ObjectDisposedException)) {
                            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::InternalWrite() throwing");
                            throw new WebException(
                                NetRes.GetWebStatusString("net_requestaborted", WebExceptionStatus.RequestCanceled),
                                WebExceptionStatus.RequestCanceled);
                        }

                        nesting = Nesting.InError;

                        if (NclUtilities.IsFatal(exception))
                        {
                            m_ErrorResponseStatus = false;
                            IOError(exception);
                            throw;
                        }

                        if (m_ErrorResponseStatus) {
                            // We already got a error response, hence server could drop the connection,
                            // Here we are recovering for future (optional) resubmit ...
                            m_IgnoreSocketErrors = true;
                            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::InternalWrite() IGNORE write fault");
                            if (async)
                            {
                                completeSync = true;
                            }
                        }
                        else {
                            // Note we could swallow this since receive callback is already posted and
                            // should give us similar failure
                            IOError(exception);
                            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::InternalWrite() throwing:" + exception.ToString());
                            throw;
                        }
                    }
                    GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::InternalWrite chunked");
                    return asyncResult;
                }
                else {
                    //
                    // We're not chunking. See if we're sending too much; if not,
                    // go ahead and write it.
                    //
                    asyncResult = (async) ? new NestedSingleAsyncResult(this, state, callback, buffer, offset, size) : null;

                    if (BytesLeftToWrite != -1) {
                        //
                        // but only check if we aren't writing to an unknown content-length size,
                        // as we can be buffering.
                        //
                        if (BytesLeftToWrite < (long)size) {
                            //
                            // writing too much data.
                            //
                            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::InternalWrite()");
                            throw new ProtocolViolationException(SR.GetString(SR.net_entitytoobig));
                        }

                        if (!async) {
                            //
                            // Otherwise update our bytes left to send and send it.
                            //
                            m_BytesLeftToWrite -= (long)size;
                        }
                    }

                    //
                    // After doing, the m_WriteByte size calculations, and error checking
                    //  here doing the async Write Call
                    //

                    try {
                        if (async) {
                            if(m_Request.ContentLength == 0 && IsPostStream) {
                                m_BytesLeftToWrite -=size;
                                completeSync = true;
                            }
                           else{
                                m_BytesAlreadyTransferred = size;
                                m_Connection.BeginWrite(buffer, offset, size, m_WriteCallbackDelegate, asyncResult);
                           }
                        }
                        else {
                            SafeSetSocketTimeout(SocketShutdown.Send);
                            //If we are doing the ntlm handshake,  contentlength
                            //could be 0 for the first part, even if there is data
                            //to write.
                            if (m_Request.ContentLength != 0 || !IsPostStream || !m_Request.NtlmKeepAlive) {
                                m_Connection.Write(buffer, offset, size);
                            }
                        }
                    }
                    catch (Exception exception) {
                        // IgnoreSocketErrors can be set at any time - need to check it again.
                        if (IgnoreSocketErrors && !NclUtilities.IsFatal(exception))
                        {
                            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::InternalWrite() swallowing: IgnoreSocketErrors set after throw.");
                            if (async)
                            {
                                completeSync = true;
                            }
                            return asyncResult;
                        }

                        if (m_Request.Aborted && (exception is IOException || exception is ObjectDisposedException)) {
                            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::InternalWrite() throwing");
                            throw new WebException(
                                NetRes.GetWebStatusString("net_requestaborted", WebExceptionStatus.RequestCanceled),
                                WebExceptionStatus.RequestCanceled);
                        }

                        nesting = Nesting.InError;

                        if (NclUtilities.IsFatal(exception))
                        {
                            m_ErrorResponseStatus = false;
                            IOError(exception);
                            throw;
                        }

                        if (m_ErrorResponseStatus) {
                            // We already got a error response, hence server could drop the connection,
                            // Here we are recovering for future (optional) resubmit ...
                            m_IgnoreSocketErrors = true;
                            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::InternWrite() IGNORE write fault");
                            if (async)
                            {
                                completeSync = true;
                            }
                        }
                        else {
                            // Note we could swallow this since receive callback is already posted and
                            // should give us similar failure
                            IOError(exception);
                            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::InternalWrite() throwing:" + exception.ToString());
                            throw;
                        }
                    }
                    GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::InternalWrite");
                    return asyncResult;
                }
            }
            finally {
                if (!async || nesting == Nesting.InError || completeSync)
                {
                    nesting = Interlocked.CompareExchange(ref m_CallNesting, (nesting == Nesting.InError? Nesting.InError: Nesting.Idle), Nesting.IoInProgress);
                    GlobalLog.Print("InternalWrite() Out callNesting: " + nesting.ToString());
                    if (nesting == Nesting.Closed)
                    {
                        //send closing bytes
                        ResumeInternalClose(asyncResult);
                    }
                    else if (completeSync && asyncResult != null)
                    {
                        asyncResult.InvokeCallback();
                    }
                }
            }
        }


        /*++
            WriteDataCallback

            This is a callback, that is part of the main BeginWrite
            code, this is part of the normal transfer code.

            Input:

               asyncResult - IAsyncResult generated from BeginWrite

            Returns:

               None

        --*/
        private static void WriteCallback(IAsyncResult asyncResult)
        {
            LazyAsyncResult userResult = (LazyAsyncResult) asyncResult.AsyncState;
            ((ConnectStream) userResult.AsyncObject).ProcessWriteCallback(asyncResult, userResult);
        }

        private void ProcessWriteCallback(IAsyncResult asyncResult, LazyAsyncResult userResult)
        {
            Exception userException = null;

            try {
                NestedSingleAsyncResult castedSingleAsyncResult = userResult as NestedSingleAsyncResult;
                if (castedSingleAsyncResult != null)
                {
                    try {
                        m_Connection.EndWrite(asyncResult);
                        if (BytesLeftToWrite != -1) {
                            // Update our bytes left to send.
                            m_BytesLeftToWrite -= m_BytesAlreadyTransferred;
                            m_BytesAlreadyTransferred = 0;
                        }

                        if (Logging.On) Logging.Dump(Logging.Web, this, "WriteCallback", castedSingleAsyncResult.Buffer, castedSingleAsyncResult.Offset, castedSingleAsyncResult.Size);
                    }
                    catch (Exception exception) {

                        userException = exception;

                        if (NclUtilities.IsFatal(exception))
                        {
                            m_ErrorResponseStatus = false;
                            IOError(exception);
                            throw;
                        }
                        if (m_ErrorResponseStatus) {
                            // We already got a error response, hence server could drop the connection,
                            // Here we are recovering for future (optional) resubmit ...
                            m_IgnoreSocketErrors = true;
                            userException = null;
                            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::EndWrite() IGNORE write fault");
                        }
                    }
                }
                else {
                    NestedMultipleAsyncResult castedMultipleAsyncResult = (NestedMultipleAsyncResult) userResult;
                    try {
                        m_Connection.EndMultipleWrite(asyncResult);
                        if(Logging.On) {
                            foreach (BufferOffsetSize bufferOffsetSize in castedMultipleAsyncResult.Buffers) {
                                Logging.Dump(Logging.Web, castedMultipleAsyncResult, "WriteCallback", bufferOffsetSize.Buffer, bufferOffsetSize.Offset, bufferOffsetSize.Size);
                            }
                        }
                    }
                    catch (Exception exception) {

                        userException = exception;

                        if (NclUtilities.IsFatal(exception))
                        {
                            m_ErrorResponseStatus = false;
                            IOError(exception);
                            throw;
                        }
                        if (m_ErrorResponseStatus) {
                            // We already got a error response, hence server could drop the connection,
                            // Here we are recovering for future (optional) resubmit ...
                            m_IgnoreSocketErrors = true;
                            userException = null;
                            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::EndWrite() IGNORE write fault");
                        }
                    }
                }
            }
            finally {

                if (Nesting.Closed == ExchangeCallNesting((userException == null? Nesting.Idle: Nesting.InError), Nesting.IoInProgress))
                {
                    if (userException != null && m_ErrorException == null)
                    {
                        Interlocked.CompareExchange<Exception>(ref m_ErrorException, userException, null);
                    }
                    ResumeInternalClose(userResult);
                }
                else
                {
                    userResult.InvokeCallback(userException);
                }
            }
        }
        //I need this because doing this within the static w/ "ref stream.m_Callnesting is getting an error.
        private int  ExchangeCallNesting(int value, int comparand) {
            int result = Interlocked.CompareExchange(ref m_CallNesting, value, comparand);
            GlobalLog.Print("an AsyncCallback Out callNesting: " + m_CallNesting.ToString());
            return result;
        }

        /*++
            EndWrite - Finishes off async write of data, just calls into
                m_Connection.EndWrite to get the result.

            Input:

                asyncResult     - The AsyncResult returned by BeginWrite


        --*/
        public override void EndWrite(IAsyncResult asyncResult) {
#if DEBUG
            using (GlobalLog.SetThreadKind(ThreadKinds.User)) {
#endif
            GlobalLog.Enter("ConnectStream#" + ValidationHelper.HashString(this) + "::EndWrite");
            if(Logging.On)Logging.Enter(Logging.Web, this, "EndWrite", "");
            //
            // parameter validation
            //
            if (asyncResult==null) {
                throw new ArgumentNullException("asyncResult");
            }
            LazyAsyncResult castedAsyncResult = asyncResult as LazyAsyncResult;

            if (castedAsyncResult==null || castedAsyncResult.AsyncObject!=this) {
                throw new ArgumentException(SR.GetString(SR.net_io_invalidasyncresult), "asyncResult");
            }
            if (castedAsyncResult.EndCalled) {
                throw new InvalidOperationException(SR.GetString(SR.net_io_invalidendcall, "EndWrite"));
            }

            castedAsyncResult.EndCalled = true;

            //
            // wait & then check for errors
            //

            object returnValue = castedAsyncResult.InternalWaitForCompletion();

            if (ErrorInStream) {
                GlobalLog.LeaveException("ConnectStream#" + ValidationHelper.HashString(this) + "::EndWrite", m_ErrorException);
                throw m_ErrorException;
            }

            Exception exception = returnValue as Exception;
            if (exception!=null) {

                if (exception is IOException && m_Request.Aborted) {
                    GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::InternalWrite() throwing");
                    throw new WebException(
                        NetRes.GetWebStatusString("net_requestaborted", WebExceptionStatus.RequestCanceled),
                        WebExceptionStatus.RequestCanceled);
                }

                IOError(exception);
                GlobalLog.LeaveException("ConnectStream#" + ValidationHelper.HashString(this) + "::EndWrite", exception);
                throw exception;
            }

            GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(this) + "::EndWrite");
            if(Logging.On)Logging.Exit(Logging.Web, this, "EndWrite", "");
#if DEBUG
            }
#endif
        }


        /*++
            Read - Read from the connection.
            ReadWithoutValidation

            This method reads from the network, or our internal buffer if there's
            data in that. If there's not, we'll read from the network. If we're

            doing chunked decoding, we'll decode it before returning from this
            call.


            Input:

                buffer          - Buffer to read into.
                offset          - Offset in buffer to read into.
                size           - Size in bytes to read.

            Returns:
                Nothing.

        --*/
        public override int Read([In, Out] byte[] buffer, int offset, int size) {
#if DEBUG
            using (GlobalLog.SetThreadKind(ThreadKinds.User | ThreadKinds.Sync)) {
#endif
            if (Logging.On) Logging.Enter(Logging.Web, this, "Read", "");
            if (WriteStream) {
                throw new NotSupportedException(SR.GetString(SR.net_writeonlystream));
            }
            if (buffer==null) {
                throw new ArgumentNullException("buffer");
            }
            if (offset<0 || offset>buffer.Length) {
                throw new ArgumentOutOfRangeException("offset");
            }
            if (size<0 || size>buffer.Length-offset) {
                throw new ArgumentOutOfRangeException("size");
            }
            if (ErrorInStream) {
                throw m_ErrorException;
            }

            if (IsClosed) {
                throw new WebException(
                            NetRes.GetWebStatusString("net_requestaborted", WebExceptionStatus.ConnectionClosed),
                            WebExceptionStatus.ConnectionClosed);
            }

            if (m_Request.Aborted) {
                GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::InternalWrite() throwing");
                throw new WebException(
                            NetRes.GetWebStatusString("net_requestaborted", WebExceptionStatus.RequestCanceled),
                            WebExceptionStatus.RequestCanceled);
            }

            //
            // if we fail/hang this call for some reason,
            // this Nesting count we be non-0, so that when we
            // close this stream, we will abort the socket.
            //
            int nesting = Interlocked.CompareExchange(ref m_CallNesting, Nesting.IoInProgress, Nesting.Idle);
            GlobalLog.Print("Read() In: callNesting : " + m_CallNesting.ToString());

            if (nesting != Nesting.Idle)
            {
                throw new NotSupportedException(SR.GetString(SR.net_no_concurrent_io_allowed));
            }

            int bytesRead = -1;
            try
            {
                SafeSetSocketTimeout(SocketShutdown.Receive);
            }
            catch (Exception exception)
            {
                IOError(exception);
                throw;
            }
            try {
                bytesRead = ReadWithoutValidation(buffer, offset, size);
            }
            catch (Exception exception)
            {
                Win32Exception win32Exception = exception.InnerException as Win32Exception;
                if (win32Exception != null && win32Exception.NativeErrorCode == (int)SocketError.TimedOut)
                    exception = new WebException(SR.GetString(SR.net_timeout), WebExceptionStatus.Timeout);
                throw exception;
            }

            Interlocked.CompareExchange(ref m_CallNesting, Nesting.Idle, Nesting.IoInProgress);
            GlobalLog.Print("Read() Out: callNesting: " + m_CallNesting.ToString());

            if(Logging.On && bytesRead>0)Logging.Dump(Logging.Web, this, "Read", buffer, offset, bytesRead);
            if(Logging.On)Logging.Exit(Logging.Web, this, "Read", bytesRead);
            return bytesRead;
#if DEBUG
            }
#endif
        }


        /*++
            ReadWithoutValidation - Read from the connection.

            Sync version of BeginReadWithoutValidation

            This method reads from the network, or our internal buffer if there's
            data in that. If there's not, we'll read from the network. If we're
            doing chunked decoding, we'll decode it before returning from this
            call.

        --*/
        private int ReadWithoutValidation(byte[] buffer, int offset, int size)
        {
            return ReadWithoutValidation(buffer, offset, size, true);
        }

        //
        // abortOnError parameter is set to false when called from CloseInternal
        //
        private int ReadWithoutValidation([In, Out] byte[] buffer, int offset, int size, bool abortOnError)
        {
            GlobalLog.Print("int ConnectStream::ReadWithoutValidation()");
            GlobalLog.Print("(start)m_ReadBytes = "+m_ReadBytes);


// ********** WARNING - updating logic below should also be updated in BeginReadWithoutValidation *****************

            //
            // Figure out how much we should really read.
            //

            int bytesToRead = 0;

            if (m_Chunked) {
                if (!m_ChunkEofRecvd) {

                    // See if we have more left from a previous
                    // chunk.
                    if (m_ChunkSize != 0) {
                        bytesToRead = Math.Min(size, m_ChunkSize);
                    }
                    else {
                        // read size of next chunk
                        try {
                            bytesToRead = ReadChunkedSync(buffer, offset, size);
                            m_ChunkSize -= bytesToRead;
                        }
                        catch (Exception exception) {

                            if (abortOnError)
                            {
                                IOError(exception);
                            }
                            throw;
                        }
                        return bytesToRead;
                    }
                }
            }
            else {

                //
                // Not doing chunked, so don't read more than is left.
                //

                if (m_ReadBytes != -1) {
                    bytesToRead = (int)Math.Min(m_ReadBytes, (long)size);
                }
                else {
                    bytesToRead = size;
                }
            }

            // If we're not going to read anything, either because they're
            // not asking for anything or there's nothing left, bail
            // out now.

            if (bytesToRead == 0 || this.Eof) {
                return 0;
            }

            try {
                bytesToRead = InternalRead(buffer, offset, bytesToRead);
            }
            catch (Exception exception) {
                if (abortOnError)
                {
                    IOError(exception);
                }
                throw;
            }

            GlobalLog.Print("bytesTransferred = "+bytesToRead);
            int bytesTransferred = bytesToRead;

            if (m_Chunked) {
                m_ChunkSize -= bytesTransferred;
            }
            else {

                bool doneReading = false;

                if (bytesTransferred <= 0)
                {
                    bytesTransferred = 0;

                    //
                    // We read 0 bytes from the connection, or got an error. This is OK if we're
                    // reading to end, it's an error otherwise.
                    //
                    if (m_ReadBytes != -1) {
                        // A Fatal error
                        if (abortOnError)
                        {
                            IOError(null, false);   // request will be aborted but the user will see EOF on that stream read call
                        }
                        else
                        {
                            throw m_ErrorException; // CloseInternal will process this case as abnormal
                        }
                    }
                    else {
                        //
                        // We're reading to end, and we found the end, by reading 0 bytes
                        //
                        doneReading = true;
                    }
                }

                //
                // Not chunking. Update our read bytes state and return what we've read.
                //

                if (m_ReadBytes != -1) {
                    m_ReadBytes -= bytesTransferred;

                    GlobalLog.Assert(m_ReadBytes >= 0, "ConnectStream: Attempting to read more bytes than available.|m_ReadBytes < 0");

                    GlobalLog.Print("m_ReadBytes = "+m_ReadBytes);

                    if (m_ReadBytes < 0)
                        throw new InternalException();

                }

                if (m_ReadBytes == 0 || doneReading) {
                    // We're all done reading, tell the connection that.
                    m_ReadBytes = 0;

                    //
                    // indicate to cache that read completed OK
                    //

                    CallDone();
                }
            }
            GlobalLog.Print("bytesTransferred = "+bytesToRead);
            GlobalLog.Print("(end)m_ReadBytes = "+m_ReadBytes);
// ********** WARNING - updating logic above should also be updated in BeginReadWithoutValidation and EndReadWithoutValidation *****************
            return bytesTransferred;
        }



        /*++
            BeginRead - Read from the connection.
            BeginReadWithoutValidation

            This method reads from the network, or our internal buffer if there's
            data in that. If there's not, we'll read from the network. If we're
            doing chunked decoding, we'll decode it before returning from this
            call.


            Input:

                buffer          - Buffer to read into.
                offset          - Offset in buffer to read into.
                size           - Size in bytes to read.

            Returns:
                Nothing.

        --*/


        [HostProtection(ExternalThreading=true)]
        public override IAsyncResult BeginRead(byte[] buffer, int offset, int size, AsyncCallback callback, object state) {
#if DEBUG
            using (GlobalLog.SetThreadKind(ThreadKinds.User | ThreadKinds.Async)) {
#endif
            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::BeginRead() " + ValidationHelper.HashString(m_Connection) + ", " + offset.ToString() + ", " + size.ToString());
            if(Logging.On)Logging.Enter(Logging.Web, this, "BeginRead", "");

            //
            // parameter validation
            //
            if (WriteStream) {
                throw new NotSupportedException(SR.GetString(SR.net_writeonlystream));
            }
            if (buffer==null) {
                throw new ArgumentNullException("buffer");
            }
            if (offset<0 || offset>buffer.Length) {
                throw new ArgumentOutOfRangeException("offset");
            }
            if (size<0 || size>buffer.Length-offset) {
                throw new ArgumentOutOfRangeException("size");
            }

            //
            // if we have a stream error, or we've already shut down this socket
            //  then we must prevent new BeginRead/BeginWrite's from getting
            //  submited to the socket, since we've already closed the stream.
            //

            if (ErrorInStream) {
                throw m_ErrorException;
            }

            if (IsClosed) {
                throw new WebException(
                            NetRes.GetWebStatusString("net_requestaborted", WebExceptionStatus.ConnectionClosed),
                            WebExceptionStatus.ConnectionClosed);
            }

            if (m_Request.Aborted) {
                GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::InternalWrite() throwing");
                throw new WebException(
                            NetRes.GetWebStatusString("net_requestaborted", WebExceptionStatus.RequestCanceled),
                            WebExceptionStatus.RequestCanceled);
            }         


            //
            // if we fail/hang this call for some reason,
            // this Nesting count we be non-0, so that when we
            // close this stream, we will abort the socket.
            //

            int nesting = Interlocked.CompareExchange(ref m_CallNesting, Nesting.IoInProgress, Nesting.Idle);
            GlobalLog.Print("BeginRead() In: callNesting : " + m_CallNesting.ToString());

            if (nesting != 0)
            {
                throw new NotSupportedException(SR.GetString(SR.net_no_concurrent_io_allowed));
            }

            IAsyncResult result =
                BeginReadWithoutValidation(
                        buffer,
                        offset,
                        size,
                        callback,
                        state);

            if(Logging.On)Logging.Exit(Logging.Web, this, "BeginRead", result);
            return result;
#if DEBUG
            }
#endif
        }


        /*++
            BeginReadWithoutValidation - Read from the connection.

            internal version of BeginRead above, without validation

            This method reads from the network, or our internal buffer if there's
            data in that. If there's not, we'll read from the network. If we're
            doing chunked decoding, we'll decode it before returning from this
            call.


            Input:

                buffer          - Buffer to read into.
                offset          - Offset in buffer to read into.
                size           - Size in bytes to read.

            Returns:
                Nothing.

        --*/

        private IAsyncResult BeginReadWithoutValidation(byte[] buffer, int offset, int size, AsyncCallback callback, object state) {
            GlobalLog.Enter("ConnectStream#" + ValidationHelper.HashString(this) + "::BeginReadWithoutValidation", ValidationHelper.HashString(m_Connection) + ", " + offset.ToString() + ", " + size.ToString());
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "ConnectStream#" + ValidationHelper.HashString(this) + "::BeginReadWithoutValidation");

            //
            // Figure out how much we should really read.
            //

            int bytesToRead = 0;

            if (m_Chunked) {

                if (!m_ChunkEofRecvd) {

                    // See if we have more left from a previous
                    // chunk.

                    if (m_ChunkSize != 0) {
                        bytesToRead = Math.Min(size, m_ChunkSize);
                    }
                    else {
                        NestedSingleAsyncResult castedAsyncResult = new NestedSingleAsyncResult(this, state, callback, buffer, offset, size);

                        ThreadPool.QueueUserWorkItem(m_ReadChunkedCallbackDelegate, castedAsyncResult);

                        GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(this) + "::BeginReadWithoutValidation() called QueueUserWorkItem(m_ReadChunkedCallbackDelegate)");
                        return castedAsyncResult;
                    }

                }
            }
            else {

                //
                // Not doing chunked, so don't read more than is left.
                //

                if (m_ReadBytes != -1) {
                    bytesToRead = (int)Math.Min(m_ReadBytes, (long)size);
                }
                else {
                    bytesToRead = size;
                }
            }

            // If we're not going to read anything, either because they're
            // not asking for anything or there's nothing left, bail
            // out now.

            if (bytesToRead == 0 || this.Eof) {
                NestedSingleAsyncResult completedResult = new NestedSingleAsyncResult(this, state, callback, ZeroLengthRead);
                GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(this) + "::BeginReadWithoutValidation() completed, bytesToRead: " + bytesToRead + " Eof: " + this.Eof.ToString());
                return completedResult;
            }

            try
            {
                int bytesAlreadyRead = 0;
                if (m_ReadBufferSize > 0)
                {
                    bytesAlreadyRead = FillFromBufferedData(buffer, ref offset, ref bytesToRead);
                    if (bytesToRead == 0)
                    {
                        NestedSingleAsyncResult completedResult = new NestedSingleAsyncResult(this, state, callback, bytesAlreadyRead);
                        GlobalLog.Leave("ConnectStream::BeginReadWithoutValidation");
                        return completedResult;
                    }
                }

                if (ErrorInStream)
                {
                    GlobalLog.LeaveException("ConnectStream::BeginReadWithoutValidation", m_ErrorException);
                    throw m_ErrorException;
                }

                GlobalLog.Assert(m_DoneCalled == 0 || m_ReadBytes != -1, "BeginRead: Calling BeginRead after ReadDone.|m_DoneCalled > 0 && m_ReadBytes == -1");

                // Keep track of this during the read so it can be added back at the end.
                m_BytesAlreadyTransferred = bytesAlreadyRead;

                IAsyncResult asyncResult = m_Connection.BeginRead(buffer, offset, bytesToRead, callback, state);

                // a null return indicates that the connection was closed underneath us.
                if (asyncResult == null)
                {
                    m_BytesAlreadyTransferred = 0;
                    m_ErrorException = new WebException(
                        NetRes.GetWebStatusString("net_requestaborted", WebExceptionStatus.RequestCanceled),
                        WebExceptionStatus.RequestCanceled);

                    GlobalLog.LeaveException("ConnectStream::BeginReadWithoutValidation", m_ErrorException);
                    throw m_ErrorException;
                }

                GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(this) + "::BeginReadWithoutValidation() called BeginRead");
                return asyncResult;
            }
            catch (Exception exception) {
                IOError(exception);
                GlobalLog.LeaveException("ConnectStream#" + ValidationHelper.HashString(this) + "::BeginReadWithoutValidation", exception);
                throw;
            }
        }


        /*++
            InternalRead

            This is an interal version of Read without validation,
             that is called from the Chunked code as well the normal codepaths.

        --*/

        private int InternalRead(byte[] buffer, int offset, int size) {
            GlobalLog.ThreadContract(ThreadKinds.Sync, "ConnectStream#" + ValidationHelper.HashString(this) + "::InternalRead");

            // Read anything first out of the buffer
            int bytesToRead = FillFromBufferedData(buffer, ref offset, ref size);
            if (bytesToRead>0) {
                return bytesToRead;
            }

            // otherwise, we need to read more data from the connection.
            if (ErrorInStream) {
                GlobalLog.LeaveException("ConnectStream::InternalBeginRead", m_ErrorException);
                throw m_ErrorException;
            }

            bytesToRead = m_Connection.Read(
                    buffer,
                    offset,
                    size);

            return bytesToRead;
        }


        /*++
            ReadCallback

            This callback is only used by chunking as the last step of its multi-phase async operation.

            Input:

               asyncResult - IAsyncResult generated from BeginWrite

            Returns:

               None

        --*/
        private static void ReadCallback(IAsyncResult asyncResult) {
            GlobalLog.Enter("ConnectStream::ReadCallback", "asyncResult=#"+ValidationHelper.HashString(asyncResult));
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "ConnectStream::ReadCallback");

            //
            // we called m_Connection.BeginRead() previously that call
            // completed and called our internal callback
            // we passed the NestedSingleAsyncResult (that we then returned to the user)
            // as the state of this call, so build it back:
            //
            NestedSingleAsyncResult castedAsyncResult = (NestedSingleAsyncResult)asyncResult.AsyncState;
            ConnectStream thisConnectStream = (ConnectStream)castedAsyncResult.AsyncObject;

            try {
                int bytesTransferred = thisConnectStream.m_Connection.EndRead(asyncResult);
                if(Logging.On)Logging.Dump(Logging.Web, thisConnectStream, "ReadCallback", castedAsyncResult.Buffer, castedAsyncResult.Offset, Math.Min(castedAsyncResult.Size, bytesTransferred));
                //
                // call the user's callback, with success
                //
                castedAsyncResult.InvokeCallback(bytesTransferred);
            }
            catch (Exception exception) {
                if (NclUtilities.IsFatal(exception)) throw;

                //
                // call the user's callback, with exception
                //
                castedAsyncResult.InvokeCallback(exception);
            }
            GlobalLog.Leave("ConnectStream::ReadCallback");
        }


        /*++
            EndRead - Finishes off the Read for the Connection
            EndReadWithoutValidation

            This method completes the async call created from BeginRead,
            it attempts to determine how many bytes were actually read,
            and if any errors occured.

            Input:
                asyncResult - created by BeginRead

            Returns:
                int - size of bytes read, or < 0 on error

        --*/

        public override int EndRead(IAsyncResult asyncResult) {
#if DEBUG
            using (GlobalLog.SetThreadKind(ThreadKinds.User)) {
#endif
            if (Logging.On) Logging.Enter(Logging.Web, this, "EndRead", "");

            //
            // parameter validation
            //
            if (asyncResult==null) {
                throw new ArgumentNullException("asyncResult");
            }

            int bytesTransferred;
            bool zeroLengthRead = false;
            if (asyncResult.GetType() == typeof(NestedSingleAsyncResult))
            {
                NestedSingleAsyncResult castedAsyncResult = (NestedSingleAsyncResult) asyncResult;
                if (castedAsyncResult.AsyncObject != this)
                {
                    throw new ArgumentException(SR.GetString(SR.net_io_invalidasyncresult), "asyncResult");
                }
                if (castedAsyncResult.EndCalled)
                {
                    throw new InvalidOperationException(SR.GetString(SR.net_io_invalidendcall, "EndRead"));
                }
                castedAsyncResult.EndCalled = true;

                if (ErrorInStream)
                {
                    GlobalLog.LeaveException("ConnectStream::EndRead", m_ErrorException);
                    throw m_ErrorException;
                }

                object result = castedAsyncResult.InternalWaitForCompletion();

                Exception errorException = result as Exception;
                if (errorException != null)
                {
                    IOError(errorException, false);
                    bytesTransferred = -1;
                }
                else
                {
                    // If it's a NestedSingleAsyncResult, we completed it ourselves with our own result.
                    if (result == null)
                    {
                        bytesTransferred = 0;
                    }
                    else if (result == ZeroLengthRead)
                    {
                        bytesTransferred = 0;
                        zeroLengthRead = true;
                    }
                    else
                    {
                        try
                        {
                            bytesTransferred = (int) result;
                        }
                        catch (InvalidCastException)
                        {
                            bytesTransferred = -1;
                        }
                    }
                }
            }
            else
            {
                // If it's not a NestedSingleAsyncResult, we forwarded directly to the Connection and need to call EndRead.
                try
                {
                    bytesTransferred = m_Connection.EndRead(asyncResult);
                }
                catch (Exception exception)
                {
                    if (NclUtilities.IsFatal(exception)) throw;

                    IOError(exception, false);
                    bytesTransferred = -1;
                }
            }

            bytesTransferred = EndReadWithoutValidation(bytesTransferred, zeroLengthRead);

            Interlocked.CompareExchange(ref m_CallNesting, Nesting.Idle, Nesting.IoInProgress);
            GlobalLog.Print("EndRead() callNesting: " + m_CallNesting.ToString());

            if(Logging.On)Logging.Exit(Logging.Web, this, "EndRead", bytesTransferred);
            return bytesTransferred;
#if DEBUG
            }
#endif
        }


        /*++
            EndReadWithoutValidation - Finishes off the Read for the Connection
                Called internally by EndRead.

            This method completes the async call created from BeginRead,
            it attempts to determine how many bytes were actually read,
            and if any errors occured.

            Input:
                asyncResult - created by BeginRead

            Returns:
                int - size of bytes read, or < 0 on error

        --*/
        private int EndReadWithoutValidation(int bytesTransferred, bool zeroLengthRead)
        {
            GlobalLog.Enter("ConnectStream#" + ValidationHelper.HashString(this) + "::EndReadWithoutValidation", bytesTransferred.ToString());

            int bytesAlreadyTransferred = m_BytesAlreadyTransferred;
            m_BytesAlreadyTransferred = 0;

            if (m_Chunked)
            {
                if (bytesTransferred < 0)
                {
                    IOError(null, false);
                    bytesTransferred = 0;
                }

                bytesTransferred += bytesAlreadyTransferred;
                m_ChunkSize -= bytesTransferred;
            }
            else {

                //
                // we're not chunking, a note about error
                //  checking here, in some cases due to 1.0
                //  servers we need to read until 0 bytes,
                //  or a server reset, therefore, we may need
                //  ignore sockets errors
                //

                bool doneReading = false;

                // if its finished without async, just use what was read already from the buffer,
                // otherwise we call the Connection's EndRead to find out
                if (bytesTransferred <= 0)
                {
                    //
                    // We read 0 bytes from the connection, or it had an error. This is OK if we're
                    // reading to end, it's an error otherwise.
                    //
                    if (m_ReadBytes != -1 && (bytesTransferred < 0 || !zeroLengthRead))
                    {
                        IOError(null, false);
                    }
                    else {
                        //
                        // We're reading to end, and we found the end, by reading 0 bytes
                        //
                        doneReading = true;
                        bytesTransferred = 0;
                    }
                }

                bytesTransferred += bytesAlreadyTransferred;

                //
                // Not chunking. Update our read bytes state and return what we've read.
                //
                if (m_ReadBytes != -1) {
                    m_ReadBytes -= bytesTransferred;

                    GlobalLog.Assert(m_ReadBytes >= 0, "ConnectStream: Attempting to read more bytes than available.|m_ReadBytes < 0");

                    GlobalLog.Print("m_ReadBytes = "+m_ReadBytes);
                }

                if (m_ReadBytes == 0 || doneReading) {
                    // We're all done reading, tell the connection that.
                    m_ReadBytes = 0;

                    //
                    // indicate to cache that read completed OK
                    //

                    CallDone();
                }
            }

            GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(this) + "::EndRead", bytesTransferred);
            return bytesTransferred;
        }


        /*++
            ReadSingleByte - Read a single byte from the stream.

            A utility function to read a single byte from the stream. Could be
            done via ReadCoreNormal, but this is slightly more efficient.

            Input:


            Returns:
                The byte read as an int, or -1 if we couldn't read.

        --*/
        internal int ReadSingleByte() {
            if (ErrorInStream) {
                return -1;
            }

            if (m_ReadBufferSize != 0) {
                m_ReadBufferSize--;
                return (int)m_ReadBuffer[m_ReadOffset++];
            }
            else {
                int bytesTransferred = m_Connection.Read(m_TempBuffer, 0, 1);

                if (bytesTransferred <= 0) {
                    return -1;
                }

                return (int)m_TempBuffer[0];
            }
        }


        /*++
           ReadCRLF

            A utility routine that tries to read the CRLF at the end of a
            chunk.


            Input:

                buffer          - buffer to read into

            Returns:
                int - number of bytes read

        --*/
        private int ReadCRLF(byte[] buffer) {
            GlobalLog.Enter("ConnectStream#" + ValidationHelper.HashString(this) + "::ReadCRLF");
            int offset = 0;
            int size = NclConstants.CRLF.Length;

            int BytesRead = FillFromBufferedData(buffer, ref offset, ref size);

            if (BytesRead >= 0 && BytesRead != NclConstants.CRLF.Length)
            {
                do {
                    int bytesTransferred = m_Connection.Read(buffer, offset, size);

                    if (bytesTransferred <= 0) {
                        GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(this) + "::ReadCRLF", bytesTransferred);
                        throw new IOException(SR.GetString(SR.net_io_readfailure, SR.GetString(SR.net_io_connectionclosed)));
                    }
                    else {
                        size   -= bytesTransferred;
                        offset += bytesTransferred;
                    }
                } while ( size > 0 );
            }

            GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(this) + "::ReadCRLF", BytesRead);
            return BytesRead;
        }


        /*++
            ReadChunkedCallback

            This is callback, that parses and does a chunked read.
            It is here that we attempt to Read enough bytes
            to determine the size of the next chunk of data,
            and parse through any headers/control information
            asscoiated with that chunk.

            Input:

               asyncResult - IAsyncResult generated from ConnectStream.BeginRead

        --*/
        private static void ReadChunkedCallback(object state) {
#if DEBUG
            GlobalLog.SetThreadSource(ThreadKinds.Worker);
            using (GlobalLog.SetThreadKind(ThreadKinds.System | ThreadKinds.Sync)) {
#endif

// ********** WARNING - updating logic below should also be updated in ReadChunkedSync *****************

            NestedSingleAsyncResult castedAsyncResult = state as NestedSingleAsyncResult;
            ConnectStream thisConnectStream = castedAsyncResult.AsyncObject as ConnectStream;

            GlobalLog.Enter("ConnectStream#" + ValidationHelper.HashString(thisConnectStream) + "::ReadChunkedCallback", ValidationHelper.HashString(castedAsyncResult));

            try {
                if (!thisConnectStream.m_Draining && thisConnectStream.IsClosed) {
                    // throw on shutdown only if we're not draining the socket.
                    Exception exception =
                        new WebException(
                            NetRes.GetWebStatusString("net_requestaborted", WebExceptionStatus.ConnectionClosed),
                            WebExceptionStatus.ConnectionClosed);

                    castedAsyncResult.InvokeCallback(exception);
                    GlobalLog.LeaveException("ReadChunkedCallback", exception);
                    return;
                }
                else if (thisConnectStream.m_ErrorException!=null) {
                    // throw on IO error even if we're draining the socket.
                    castedAsyncResult.InvokeCallback(thisConnectStream.m_ErrorException);
                    GlobalLog.LeaveException("ReadChunkedCallback", thisConnectStream.m_ErrorException);
                    return;
                }
                if (thisConnectStream.m_ChunkedNeedCRLFRead) {
                    thisConnectStream.ReadCRLF(thisConnectStream.m_TempBuffer);
                    thisConnectStream.m_ChunkedNeedCRLFRead = false;
                }

                StreamChunkBytes ReadByteBuffer = new StreamChunkBytes(thisConnectStream);

                // We need to determine size of next chunk,
                // by carefully reading, byte by byte

                thisConnectStream.m_ChunkSize = thisConnectStream.ProcessReadChunkedSize(ReadByteBuffer);

                // If this isn't a zero length chunk, read it.
                if (thisConnectStream.m_ChunkSize != 0) {
                    thisConnectStream.m_ChunkedNeedCRLFRead = true;

                    int bytesToRead = Math.Min(castedAsyncResult.Size, thisConnectStream.m_ChunkSize);

                    //
                    // Attempt to fill in our entired read from,
                    //  data previously buffered, if this completely
                    //  satisfies us, then we are done, complete sync
                    //
                    int bytesAlreadyRead = 0;
                    if (thisConnectStream.m_ReadBufferSize > 0)
                    {
                        bytesAlreadyRead = thisConnectStream.FillFromBufferedData(castedAsyncResult.Buffer, ref castedAsyncResult.Offset, ref bytesToRead);
                        if (bytesToRead == 0)
                        {
                            castedAsyncResult.InvokeCallback(bytesAlreadyRead);
                            GlobalLog.Leave("ConnectStream::ReadChunkedCallback");
                            return;
                        }
                    }

                    //
                    // otherwise, we need to read more data from the connection.
                    //
                    if (thisConnectStream.ErrorInStream)
                    {
                        GlobalLog.LeaveException("ConnectStream::ReadChunkedCallback", thisConnectStream.m_ErrorException);
                        throw thisConnectStream.m_ErrorException;
                    }

                    GlobalLog.Assert(thisConnectStream.m_DoneCalled == 0 || thisConnectStream.m_ReadBytes != -1, "ConnectStream::ReadChunkedCallback|Calling BeginRead after ReadDone.");

                    // Keep track of this during the read so it can be added back at the end.
                    thisConnectStream.m_BytesAlreadyTransferred = bytesAlreadyRead;

                    IAsyncResult asyncResult = thisConnectStream.m_Connection.BeginRead(castedAsyncResult.Buffer, castedAsyncResult.Offset, bytesToRead, m_ReadCallbackDelegate, castedAsyncResult);

                    // a null return indicates that the connection was closed underneath us.
                    if (asyncResult == null)
                    {
                        thisConnectStream.m_BytesAlreadyTransferred = 0;
                        thisConnectStream.m_ErrorException = new WebException(
                            NetRes.GetWebStatusString("net_requestaborted", WebExceptionStatus.RequestCanceled),
                            WebExceptionStatus.RequestCanceled);

                        GlobalLog.LeaveException("ConnectStream::ReadChunkedCallback", thisConnectStream.m_ErrorException);
                        throw thisConnectStream.m_ErrorException;
                    }
                }
                else {
                    // We've found the terminating 0 length chunk. We may be very well looking
                    // at an extension footer or the very final CRLF.

                    thisConnectStream.ReadCRLF(thisConnectStream.m_TempBuffer);
                    thisConnectStream.RemoveTrailers(ReadByteBuffer);

                    // Remember that we've found this, so we don't try and dechunk
                    // more.

                    thisConnectStream.m_ReadBytes = 0;
                    thisConnectStream.m_ChunkEofRecvd = true;

                    thisConnectStream.CallDone();

                    // we're done reading, return 0 bytes
                    castedAsyncResult.InvokeCallback(0);
                }
                GlobalLog.Leave("ReadChunkedCallback");
            }
            catch (Exception exception)
            {
                if (NclUtilities.IsFatal(exception)) throw;

                castedAsyncResult.InvokeCallback(exception);
                GlobalLog.LeaveException("ConnectStream::ReadChunkedCallback", exception);
            }

// ********** WARNING - updating logic above should also be updated in ReadChunkedSync *****************
#if DEBUG
            }
#endif
        }

        /*++
            ReadChunkedSync

            Parses and does a chunked read.
            It is here that we attempt to Read enough bytes
            to determine the size of the next chunk of data,
            and parse through any headers/control information
            asscoiated with that chunk.

            Returns:

               None

        --*/
        private int ReadChunkedSync(byte[] buffer, int offset, int size) {
            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::ReadChunkedSync");

// ********** WARNING - updating logic below should also be updated in ReadChunkedCallback *****************

            if (!m_Draining && IsClosed) {
                // throw on shutdown only if we're not draining the socket.
                Exception exception =
                    new WebException(
                        NetRes.GetWebStatusString("net_requestaborted", WebExceptionStatus.ConnectionClosed),
                        WebExceptionStatus.ConnectionClosed);

                throw exception;
            }
            else if (m_ErrorException!=null) {
                // throw on IO error even if we're draining the socket.
                throw m_ErrorException;
            }
            if (m_ChunkedNeedCRLFRead) {
                ReadCRLF(m_TempBuffer);
                m_ChunkedNeedCRLFRead = false;
            }

            StreamChunkBytes ReadByteBuffer = new StreamChunkBytes(this);

            // We need to determine size of next chunk,
            // by carefully reading, byte by byte

            m_ChunkSize = ProcessReadChunkedSize(ReadByteBuffer);

            // If this isn't a zero length chunk, read it.
            if (m_ChunkSize != 0) {
                m_ChunkedNeedCRLFRead = true;
                return InternalRead(buffer, offset, Math.Min(size, m_ChunkSize));
            }
            else {
                // We've found the terminating 0 length chunk. We may be very well looking
                // at an extension footer or the very final CRLF.

                ReadCRLF(m_TempBuffer);
                RemoveTrailers(ReadByteBuffer);

                // Remember that we've found this, so we don't try and dechunk
                // more.

                m_ReadBytes = 0;
                m_ChunkEofRecvd = true;

                CallDone();

                // we're done reading, return 0 bytes
                return 0;
            }

 // ********** WARNING - updating logic above should also be updated in ReadChunkedAsync *****************
        }


        /*++
            ProcessReadChunkedSize

            This is a continuation of the ReadChunkedCallback,
            and is used to parse out the size of a chunk

            Input:

               TheByteRead - single byte read from wire to process

               castedAsyncResult - Async Chunked State information

            Returns:

               None

        --*/
        private int ProcessReadChunkedSize(StreamChunkBytes ReadByteBuffer) {
            GlobalLog.Enter("ConnectStream#" + ValidationHelper.HashString(this) + "::ProcessReadChunkedSize");

            // now get the chunk size.
            int chunkSize;
            int BytesRead = ChunkParse.GetChunkSize(ReadByteBuffer, out chunkSize);

            if (BytesRead <= 0) {
                GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(this) + "::ProcessReadChunkedSize - error");
                throw new IOException(SR.GetString(SR.net_io_readfailure, SR.GetString(SR.net_io_connectionclosed)));
            }

            // Now skip past and extensions and the CRLF.
            BytesRead = ChunkParse.SkipPastCRLF(ReadByteBuffer);

            if (BytesRead <= 0) {
                GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(this) + "::ProcessReadChunkedSize - error");
                throw new IOException(SR.GetString(SR.net_io_readfailure, SR.GetString(SR.net_io_connectionclosed)));
            }

            GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(this) + "::ProcessReadChunkedSize", chunkSize);
            return chunkSize;
        }


        /*++
            RemoveTrailers

            This handles possible trailer headers that are found after the
            last chunk.  Currently we throw them away for this version.

            Input:

               ReadByteBuffer -

            Returns:

               None - throws on exception

        --*/
        private void RemoveTrailers(StreamChunkBytes ReadByteBuffer) {
            while (m_TempBuffer[0] != '\r' && m_TempBuffer[1] != '\n') {
                int BytesRead = ChunkParse.SkipPastCRLF(ReadByteBuffer);

                if (BytesRead <= 0) {
                    throw new IOException(SR.GetString(SR.net_io_readfailure, SR.GetString(SR.net_io_connectionclosed)));
                }

                ReadCRLF(m_TempBuffer);
            }
        }

        private static void WriteHeadersCallback(IAsyncResult ar)
        {
            if(ar.CompletedSynchronously){
                return;
            }

            WriteHeadersCallbackState state = (WriteHeadersCallbackState)ar.AsyncState;
            ConnectStream stream = state.stream;
            HttpWebRequest request = state.request;
            WebExceptionStatus error = WebExceptionStatus.SendFailure;

            //m_Request.writebuffer may be set to null on resubmit before method exits
            byte[] writeBuffer = request.WriteBuffer;

            GlobalLog.Enter("ConnectStream#" + ValidationHelper.HashString(stream) + "::WriteHeadersCallback", "Connection#" + ValidationHelper.HashString(stream.m_Connection) + ", " + writeBuffer.Length.ToString());

            try{
                stream.m_Connection.EndWrite(ar);
                stream.m_Connection.CheckStartReceive(request);
                error = WebExceptionStatus.Success;
            }
            catch (Exception e){
                if (NclUtilities.IsFatal(e)) throw;

                GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(stream) + "::WriteHeaders Exception: "+e.ToString());

                if (e is IOException || e is ObjectDisposedException)
                {
                    //new connection but reset from server on inital send
                    if(!stream.m_Connection.AtLeastOneResponseReceived && !request.BodyStarted){
                        e = new WebException(
                            NetRes.GetWebStatusString("net_connclosed", error),
                            error,
                            WebExceptionInternalStatus.Recoverable,
                            e);
                    }
                    else{
                        e = new WebException(
                            NetRes.GetWebStatusString("net_connclosed", error),
                            error,
                            stream.m_Connection.AtLeastOneResponseReceived ? WebExceptionInternalStatus.Isolated : WebExceptionInternalStatus.RequestFatal,
                            e);
                    }
                }

                stream.IOError(e, false);
            }

            stream.ExchangeCallNesting(Nesting.Idle, Nesting.InternalIO);

            request.WriteHeadersCallback(error, stream, true);
            GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(stream) + "::WriteHeadersCallback",writeBuffer.Length.ToString());
        }


        /*++
            WriteHeaders

            This function writes header data to the network. Headers are special
            in that they don't have any non-header transforms applied to them,
            and are not subject to content-length constraints. We just write them
            through, and if we're done writing headers we tell the connection that.

            Returns:
                WebExceptionStatus.Pending      - we don't have a stream yet.
                WebExceptionStatus.SendFailure  - there was an error while writing to the wire.
                WebExceptionStatus.Success      - success.

        --*/
        internal void WriteHeaders(bool async) {
            GlobalLog.Enter("ConnectStream#" + ValidationHelper.HashString(this) + "::WriteHeaders", "Connection#" + ValidationHelper.HashString(m_Connection) + ", headers buffer size = " + m_Request.WriteBuffer.Length.ToString());

            WebExceptionStatus error =  WebExceptionStatus.SendFailure;

            if (!ErrorInStream)
            {
                //m_Request.WriteBuffer may be set to null on resubmit before method exits
                byte[] writeBuffer = m_Request.WriteBuffer;
                
                try
                {
                    Interlocked.CompareExchange(ref m_CallNesting, Nesting.InternalIO, Nesting.Idle);
                    GlobalLog.Print("WriteHeaders() callNesting: " + m_CallNesting.ToString());

                    if(async)
                    {
                        WriteHeadersCallbackState state = new WriteHeadersCallbackState(m_Request, this);
                        IAsyncResult ar = m_Connection.UnsafeBeginWrite(writeBuffer,0,writeBuffer.Length, m_WriteHeadersCallback, state);
                        if (ar.CompletedSynchronously) {
                            m_Connection.EndWrite(ar);
                            m_Connection.CheckStartReceive(m_Request);
                            error = WebExceptionStatus.Success;
                        }
                        else {
                            error = WebExceptionStatus.Pending;
#if DEBUG
                            _PendingResult = ar;
#endif
                        }
                    }
                    else
                    {
                        SafeSetSocketTimeout(SocketShutdown.Send);
                        m_Connection.Write(writeBuffer, 0, writeBuffer.Length);
                        m_Connection.CheckStartReceive(m_Request);
                        error = WebExceptionStatus.Success;
                    }

                    if (Logging.On) Logging.PrintInfo(Logging.Web, this, SR.GetString(SR.net_log_sending_headers, m_Request.Headers.ToString(true)));
                }
                catch (Exception e) {

                    if (NclUtilities.IsFatal(e)) throw;

                    GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::WriteHeaders Exception: "+e.ToString());
                    
                    if (e is IOException || e is ObjectDisposedException)
                    {
                        //new connection but reset from server on inital send
                        if(!m_Connection.AtLeastOneResponseReceived && !m_Request.BodyStarted){
                            e = new WebException(
                                NetRes.GetWebStatusString("net_connclosed", error),
                                error,
                                WebExceptionInternalStatus.Recoverable,
                                e);
                        }
                        else{
                            e = new WebException(
                                            NetRes.GetWebStatusString("net_connclosed", error),
                                            error,
                                            m_Connection.AtLeastOneResponseReceived ? WebExceptionInternalStatus.Isolated : WebExceptionInternalStatus.RequestFatal,
                                            e);
                        }
                    }

                    IOError(e, false);
                }
                finally {
                    if(error != WebExceptionStatus.Pending) {
                        Interlocked.CompareExchange(ref m_CallNesting, Nesting.Idle, Nesting.InternalIO);
                        GlobalLog.Print("WriteHeaders() callNesting: " + m_CallNesting.ToString());
                    }
                }

            }
            else
            {
                GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::WriteHeaders() ignoring since ErrorInStream = true");
            }

            if(error != WebExceptionStatus.Pending) {
                //if error is Pending, and this is async, the request callback will be invoked from the stream callback.
                m_Request.WriteHeadersCallback(error, this, async);
            }

            GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(this) + "::WriteHeaders", error.ToString());
        }

        // Wrapper for Connection
        internal void PollAndRead(bool userRetrievedStream) {
            m_Connection.PollAndRead(m_Request, userRetrievedStream);
        }

        private void SafeSetSocketTimeout(SocketShutdown mode) {

            if(Eof){
                return;
            }

            int timeout;
            if (mode == SocketShutdown.Receive) {
                timeout = ReadTimeout;
            } else /*if (mode == SocketShutdown.Send)*/ {
                timeout = WriteTimeout;
            }
            Connection connection = m_Connection;
            if (connection!=null) {
                NetworkStream networkStream = connection.NetworkStream;
                if (networkStream!=null) {
                    networkStream.SetSocketTimeoutOption(mode, timeout, false);
                }
            }
        }


        /*++
            Close - Close the stream

            Called when the stream is closed. We close our parent stream first.
            Then if this is a write stream, we'll send the terminating chunk
            (if needed) and call the connection DoneWriting() method.

            Input:

                Nothing.

            Returns:

                Nothing.

        --*/

        protected override void Dispose(bool disposing) {
#if DEBUG
            using (GlobalLog.SetThreadKind(ThreadKinds.User | ThreadKinds.Sync)) {
#endif
                try {
                    GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::Close()");
                    if(Logging.On)Logging.Enter(Logging.Web, this, "Close", "");
                    ((ICloseEx)this).CloseEx(CloseExState.Normal);
                    if(Logging.On)Logging.Exit(Logging.Web, this, "Close", "");
                }
                finally {
                    base.Dispose(disposing);
                }
#if DEBUG
            }
#endif
        }

        internal void CloseInternal(bool internalCall) {
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "ConnectStream#" + ValidationHelper.HashString(this) + "::Abort");
            ((ICloseEx)this).CloseEx((internalCall ? CloseExState.Silent : CloseExState.Normal));
        }

        void ICloseEx.CloseEx(CloseExState closeState) {
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "ConnectStream#" + ValidationHelper.HashString(this) + "::Abort");
            CloseInternal(
                          (closeState & CloseExState.Silent) != 0,
                          (closeState & CloseExState.Abort) != 0
                          );
            GC.SuppressFinalize(this);
        }

        //
        // Optionally sends chunk terminator and proceeds with close that was collided with pending user write IO
        //
        void ResumeInternalClose(LazyAsyncResult userResult)
        {
            GlobalLog.Print("ConnectStream##" + ValidationHelper.HashString(this) + "::ResumeInternalClose(), userResult:" + userResult);
            //
            // write stream. terminate our chunking if needed.
            //
            if (WriteChunked && !ErrorInStream && !m_IgnoreSocketErrors)
            {
                m_IgnoreSocketErrors = true;
                try {
                    if (userResult == null)
                    {
                        SafeSetSocketTimeout(SocketShutdown.Send);
                        m_Connection.Write(NclConstants.ChunkTerminator, 0, NclConstants.ChunkTerminator.Length);
                    }
                    else
                    {
                        m_Connection.BeginWrite(NclConstants.ChunkTerminator, 0, NclConstants.ChunkTerminator.Length, new AsyncCallback(ResumeClose_Part2_Wrapper), userResult);
                        return;
                    }
                }
                catch (Exception exception) {
                    GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::CloseInternal() exceptionOnWrite:" + exception.Message);
                }
            }
            ResumeClose_Part2(userResult); //never throws
        }
        
        void ResumeClose_Part2_Wrapper(IAsyncResult ar)
        {
            try {
                m_Connection.EndWrite(ar);
            }
            catch (Exception exception) {
                GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::ResumeClose_Part2_Wrapper() ignoring exceptionOnWrite:" + exception.Message);
            }
            ResumeClose_Part2((LazyAsyncResult)ar.AsyncState);
        }

        private void ResumeClose_Part2(LazyAsyncResult userResult)
        {
            try
            {
                try
                {
                    if (ErrorInStream)
                    {
                        GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::ResumeClose_Part2() Aborting the connection");
                        m_Connection.AbortSocket(true);
                    }
                }
                finally
                {
                    CallDone();
                    GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(this) + "::ResumeClose_Part2", "Done");
                }
            }
            catch { }
            finally
            {
                if (userResult != null)
                {
                    userResult.InvokeCallback();
                }
            }
        }

        // The number should be reasonalbly large
        private const int AlreadyAborted = 777777;
        private void CloseInternal(bool internalCall, bool aborting) {
            GlobalLog.Enter("ConnectStream#" + ValidationHelper.HashString(this) + "::CloseInternal", internalCall.ToString());
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "ConnectStream#" + ValidationHelper.HashString(this) + "::Abort");

            bool normalShutDown = !aborting;
            Exception exceptionOnWrite = null;

            //
            // We have to prevent recursion, because we'll call our parents, close,
            // which might try to flush data. If we're in an error situation, that
            // will cause an error on the write, which will cause Close to be called
            // again, etc.
            //
            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::CloseInternal() m_ShutDown:" + m_ShutDown.ToString() + " m_CallNesting:" + m_CallNesting.ToString() + " m_DoneCalled:" + m_DoneCalled.ToString());

            //If this is an abort (aborting == true) of a write stream then we will call request.Abort()
            //that will call us again. To prevent a recursion here, only one abort is allowed.
            //However, Abort must still override previous normal close if any.
            if (aborting) {
                if (Interlocked.Exchange(ref m_ShutDown, AlreadyAborted) >= AlreadyAborted) {
                    GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(this) + "::CloseInternal", "already has been Aborted");
                    return;
                }
            }
            else {
                //If m_ShutDown != 0, then this method has been already called before,
                //Hence disregard this (presumably normal) extra close
                if (Interlocked.Increment(ref m_ShutDown) > 1) {
                    GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(this) + "::CloseInternal", "already has been closed");
                    return;
                }
            }

            //
            // Since this should be the last call made, we should be at 0
            //  If not on the read side then it's an error so we should close the socket
            //  If not on the write side then MAY BE we want this write stream to ignore all
            //  further writes and optionally send chunk terminator.
            //
            int nesting = (IsPostStream  && internalCall && !IgnoreSocketErrors && !BufferOnly && normalShutDown && !NclUtilities.HasShutdownStarted)? Nesting.Closed: Nesting.InError;
            if (Interlocked.Exchange(ref m_CallNesting, nesting) == Nesting.IoInProgress)
            {
                if (nesting == Nesting.Closed)
                {
                    GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::CloseInternal() PostStream, Internal call and m_CallNesting==1, defer closing until user write completes");
                    return;
                }
                normalShutDown &= !NclUtilities.HasShutdownStarted;
            }
            GlobalLog.Print("Close m_CallNesting: " + m_CallNesting.ToString());

            // Questionable: Thsi is to avoid throwing on public Close() when IgnoreSocketErrors==true
            if (IgnoreSocketErrors && IsPostStream && !internalCall)
            {
                m_BytesLeftToWrite = 0;
            }

            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::CloseInternal() normalShutDown:" + normalShutDown.ToString() + " m_CallNesting:" + m_CallNesting.ToString() + " m_DoneCalled:" + m_DoneCalled.ToString());


            if (IgnoreSocketErrors || !normalShutDown) {
                GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::CloseInternal() don't read/write on this, dead connection stream.");
            }
            else if (!WriteStream) {
                //
                // read stream
                //
                GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::CloseInternal() callNesting: " + m_CallNesting.ToString());
                GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::CloseInternal() read stream, calling DrainSocket()");
#if DEBUG
            using (GlobalLog.SetThreadKind(ThreadKinds.Sync)) {
#endif
                normalShutDown = DrainSocket();
#if DEBUG
            }
#endif
            }
            else {
                //
                // write stream. terminate our chunking if needed.
                //
                try {
                    if (!ErrorInStream) {
                        //
                        // if not error already, then...
                        // first handle chunking case
                        //
                        if (WriteChunked) {
                            //
                            // no need to buffer here:
                            // on resubmit, we won't be chunking anyway this will send 5 bytes on the wire
                            //
                            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::CloseInternal() Chunked, writing ChunkTerminator");
                            try {
                                // The idea behind is that closed stream must not write anything to the wire
                                // Still if we are chunking, the now buffering and future resubmit is possible
                                if (!m_IgnoreSocketErrors) {
                                    m_IgnoreSocketErrors = true;
                                    SafeSetSocketTimeout(SocketShutdown.Send);

#if DEBUG
                                    // Until there is an async version of this, we have to assert Sync privileges here.
                                    using (GlobalLog.SetThreadKind(ThreadKinds.Sync)) {
#endif
                                    m_Connection.Write(NclConstants.ChunkTerminator, 0, NclConstants.ChunkTerminator.Length);
#if DEBUG
                                    }
#endif
                                }
                            }
                            catch {
                                GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::CloseInternal() IGNORE chunk write fault");
                            }
                            m_BytesLeftToWrite = 0;
                        }
                        else if (BytesLeftToWrite>0) {
                            //
                            // not enough bytes written to client
                            //
                            GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(this) + "::CloseInternal() BytesLeftToWrite:" + BytesLeftToWrite.ToString() + " throwing not enough bytes written");
                            throw new IOException(SR.GetString(SR.net_io_notenoughbyteswritten));
                        }
                        else if (BufferOnly) {
                            //
                            // now we need to use the saved reference to the request the client
                            // closed the write stream. we need to wake up the request, so that it
                            // sends the headers and kick off resubmitting of buffered entity body
                            //
                            GlobalLog.Assert(m_Request != null, "ConnectStream#{0}::CloseInternal|m_Request == null", ValidationHelper.HashString(this));
                            m_BytesLeftToWrite = BufferedData.Length;
                            m_Request.SwitchToContentLength();
                            //
                            // writing the headers will kick off the whole request submission process
                            // (including waiting for the 100 Continue and writing the whole entity body)
                            //
                            SafeSetSocketTimeout(SocketShutdown.Send);
                            m_Request.NeedEndSubmitRequest();
                            GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(this) + "::CloseInternal", "Done");
                            return;
                        }
                    }
                    else {
                        normalShutDown = false;
                    }
                }
                catch (Exception exception) {
                    normalShutDown = false;

                    if (NclUtilities.IsFatal(exception))
                    {
                        m_ErrorException = exception;
                        throw;
                    }

                    exceptionOnWrite = new WebException(
                        NetRes.GetWebStatusString("net_requestaborted", WebExceptionStatus.RequestCanceled),
                        exception,
                        WebExceptionStatus.RequestCanceled,
                        null);

                    GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::CloseInternal() exceptionOnWrite:" + exceptionOnWrite.Message);
                }
            }

            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::CloseInternal() normalShutDown:" + normalShutDown.ToString() + " m_CallNesting:" + m_CallNesting.ToString() + " m_DoneCalled:" + m_DoneCalled.ToString());

            if (!normalShutDown && m_DoneCalled==0) {
                // If a normal Close (aborting == false) has turned into Abort _inside_ this method,
                // then check if another abort has been charged from other thread
                if (!aborting && Interlocked.Exchange(ref m_ShutDown, AlreadyAborted) >= AlreadyAborted){
                    GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(this) + "::CloseInternal", "other thread has charged Abort(), canceling that one");
                    return;
                }
                //
                // then abort the connection if we finished in error
                //   note: if m_DoneCalled != 0, then we no longer have
                //   control of the socket, so closing would cause us
                //   to close someone else's socket/connection.
                //
                m_ErrorException =
                    new WebException(
                        NetRes.GetWebStatusString("net_requestaborted", WebExceptionStatus.RequestCanceled),
                        WebExceptionStatus.RequestCanceled);

                GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::CloseInternal() Aborting the connection");

                m_Connection.AbortSocket(true);
                // For write stream Abort() we depend on either of two, i.e:
                // 1. The connection BeginRead is curently posted (means there are no response headers received yet)
                // 2. The response (read) stream must be closed as well if aborted this (write) stream.
                // Next block takes care of (2) since otherwise, (1) is true.
                if (WriteStream) {
                    HttpWebRequest req = m_Request;
                    if (req != null) {
                        req.Abort();
                    }
                }

                if (exceptionOnWrite != null) {
                    GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::CloseInternal() calling CallDone() on exceptionOnWrite:" + exceptionOnWrite.Message);

                    CallDone();

                    if (!internalCall) {
                        GlobalLog.LeaveException("ConnectStream#" + ValidationHelper.HashString(this) + "::CloseInternal() throwing:", exceptionOnWrite);
                        throw exceptionOnWrite;
                    }
                }
            }
            //
            // Let the connection know we're done writing or reading.
            //
            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::CloseInternal() calling CallDone()");

            CallDone();

            GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(this) + "::CloseInternal", "Done");
        }


        /*++
            Flush - Flush the stream

            Called when the user wants to flush the stream. This is meaningless to
            us, so we just ignore it.

            Input:

                Nothing.

            Returns:

                Nothing.



        --*/
        public override void Flush() {
        }

        /*++
            Seek - Seek on the stream

            Called when the user wants to seek the stream. Since we don't support
            seek, we'll throw an exception.

            Input:

                offset      - offset to see
                origin      - where to start seeking

            Returns:

                Throws exception



        --*/
        public override long Seek(long offset, SeekOrigin origin) {
            throw new NotSupportedException(SR.GetString(SR.net_noseek));
        }

        /*++
            SetLength - Set the length on the stream

            Called when the user wants to set the stream length. Since we don't
            support seek, we'll throw an exception.

            Input:

                value       - length of stream to set

            Returns:

                Throws exception



        --*/
        public override void SetLength(long value) {
            throw new NotSupportedException(SR.GetString(SR.net_noseek));
        }

        /*++
            DrainSocket - Reads data from the connection, till we'll ready
                to finish off the stream, or close the connection for good.


            returns - bool true on success, false on failure

        --*/
        private bool DrainSocket() {
            GlobalLog.Enter("ConnectStream::DrainSocket");
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "ConnectStream#" + ValidationHelper.HashString(this) + "::DrainSocket");

            if (IgnoreSocketErrors)
            {
                GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(this) + "::DrainSocket() IgnoreSocketErrors == true, stream is dead.", true);
                return true;
            }

            //
            // If its not chunked and we have a read buffer, don't waste time coping the data
            //  around againg, just pretend its gone, i.exception. make it die
            //
            long ReadBytes = m_ReadBytes;

            if (!m_Chunked) {

                GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::DrainSocket() m_ReadBytes:" + m_ReadBytes.ToString() + " m_ReadBufferSize:" + m_ReadBufferSize.ToString());

                if (m_ReadBufferSize != 0) {
                    //
                    // There's stuff in our read buffer.
                    // Update our internal read buffer state with what we took.
                    //
                    m_ReadOffset += m_ReadBufferSize;

                    GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::DrainSocket() m_ReadBytes:" + m_ReadBytes.ToString() + " m_ReadOffset:" + m_ReadOffset.ToString());

                    if (m_ReadBytes != -1) {

                        m_ReadBytes -= m_ReadBufferSize;

                        GlobalLog.Print("m_ReadBytes = "+m_ReadBytes);

                        // error handling, we shouldn't hang here if trying to drain, and there
                        //  is a mismatch with Content-Length and actual bytes.
                        //
                        //  Note: I've seen this often happen with some sites where they return 204
                        //  in violation of HTTP/1.1 with a Content-Length > 0

                        if (m_ReadBytes < 0) {
                            GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::DrainSocket() m_ReadBytes:" + m_ReadBytes.ToString() + " incorrect Content-Length? setting m_ReadBytes to 0 and returning false.");
                            m_ReadBytes = 0;
                            GlobalLog.Leave("ConnectStream::DrainSocket", false);
                            return false;
                        }
                    }
                    m_ReadBufferSize = 0;

                    // If the read buffer size has gone to 0, null out our pointer
                    // to it so maybe it'll be garbage-collected faster.
                    m_ReadBuffer = null;
                }

                // exit out of drain Socket when there is no connection-length,
                // it doesn't make sense to drain a possible empty socket,
                // when we're just going to close it.
                if (ReadBytes == -1) {
                    GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(this) + "::DrainSocket() ReadBytes==-1, returning true");
                    return true;
                }
            }

            //
            // in error or Eof, we may be in a weird state
            //  so we need return if we as if we don't have any more
            //  space to read, note Eof is true when there is an error
            //

            if (this.Eof) {
                GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(this) + "::DrainSocket() Eof, returning true");
                return true;
            }


            //
            // If we're draining more than 64K, then we should
            //  just close the socket, since it would be costly to
            //  do this.
            //

            if (m_ReadBytes > c_MaxDrainBytes) {
                GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::DrainSocket() m_ReadBytes:" + m_ReadBytes.ToString() + " too large, Closing the Connection");
                m_Connection.AbortSocket(false);
                GlobalLog.Leave("ConnectStream::DrainSocket", true);
                return true;
            }

            //
            // Now drain the socket the old, slow way by reading or pasing Chunked stuff
            //
            m_Draining = true;
            int bytesRead;
            for (;;) {
                try {
                    bytesRead = ReadWithoutValidation(s_DrainingBuffer, 0, s_DrainingBuffer.Length, false);
                    GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(this) + "::DrainSocket() drained bytesRead:" + bytesRead.ToString() + " bytes");
                    if (bytesRead<=0) {
                        break;
                    }
                }
                catch (Exception exception) {
                    if (NclUtilities.IsFatal(exception)) throw;

                    GlobalLog.Print("exception" + exception.ToString());
                    bytesRead = -1;
                    break;
                }
            }
            GlobalLog.Leave("ConnectStream::DrainSocket", true);
            return bytesRead > 0;
        }

        internal static byte[] s_DrainingBuffer = new byte[4096];

        /*++
            IOError - Handle an IOError on the stream.

            Input:

                exception       - optional Exception that will be later thrown

            Returns:

                Nothing or may throw

        --*/
        private void IOError(Exception exception) {
            IOError(exception, true);
        }

        // willThrow means that the code calling IOError is going to throw the exception.  This can throw a different exception in that case.
        private void IOError(Exception exception, bool willThrow)
        {
            GlobalLog.Enter("ConnectStream#" + ValidationHelper.HashString(this) + "::IOError", "Connection# " + ValidationHelper.HashString(m_Connection));
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "ConnectStream#" + ValidationHelper.HashString(this) + "::IOError");

            string Msg;

            if (m_ErrorException == null)
            {
                if ( exception == null ) {
                    if ( !WriteStream ) {
                        Msg = SR.GetString(SR.net_io_readfailure, SR.GetString(SR.net_io_connectionclosed));
                    }
                    else {
                        Msg = SR.GetString(SR.net_io_writefailure, SR.GetString(SR.net_io_connectionclosed));
                    }

                    Interlocked.CompareExchange<Exception>(ref m_ErrorException, new IOException(Msg), null);
                }
                else
                {
                    willThrow &= Interlocked.CompareExchange<Exception>(ref m_ErrorException, exception, null) != null;
                }
            }

            m_ChunkEofRecvd = true;

            ConnectionReturnResult returnResult = null;

            if (WriteStream)
                m_Connection.HandleConnectStreamException(true, false, WebExceptionStatus.SendFailure, ref returnResult, m_ErrorException);
            else
                m_Connection.HandleConnectStreamException(false, true, WebExceptionStatus.ReceiveFailure, ref returnResult, m_ErrorException);


            CallDone(returnResult);

            GlobalLog.Leave("ConnectStream#" + ValidationHelper.HashString(this) + "::IOError");

            if (willThrow)
            {
                throw m_ErrorException;
            }
        }


        /*++

            GetChunkHeader

            A private utility routine to convert an integer to a chunk header,
            which is an ASCII hex number followed by a CRLF. The header is retuned
            as a byte array.

            Input:

                size        - Chunk size to be encoded
                offset      - Out parameter where we store offset into buffer.

            Returns:

                A byte array with the header in int.

        --*/

        internal static byte[] GetChunkHeader(int size, out int offset) {
            GlobalLog.Enter("ConnectStream::GetChunkHeader", "size:" + size.ToString());

            uint Mask = 0xf0000000;
            byte[] Header = new byte[10];
            int i;
            offset = -1;

            //
            // Loop through the size, looking at each nibble. If it's not 0
            // convert it to hex. Save the index of the first non-zero
            // byte.
            //
            for (i = 0; i < 8; i++, size <<= 4) {
                //
                // offset == -1 means that we haven't found a non-zero nibble
                // yet. If we haven't found one, and the current one is zero,
                // don't do anything.
                //
                if (offset == -1) {
                    if ((size & Mask) == 0) {
                        continue;
                    }
                }

                //
                // Either we have a non-zero nibble or we're no longer skipping
                // leading zeros. Convert this nibble to ASCII and save it.
                //
                uint Temp = (uint)size >> 28;

                if (Temp < 10) {
                    Header[i] = (byte)(Temp + '0');
                }
                else {
                    Header[i] = (byte)((Temp - 10) + 'A');
                }

                //
                // If we haven't found a non-zero nibble yet, we've found one
                // now, so remember that.
                //
                if (offset == -1) {
                    offset = i;
                }
            }

            Header[8] = (byte)'\r';
            Header[9] = (byte)'\n';

            GlobalLog.Leave("ConnectStream::GetChunkHeader");
            return Header;
        }
    }
    //
    // Base Memory stream does not overide BeginXXX and that will cause base Stream
    // to do async delegates and that is not thread safe on async Stream.Close()
    //
    // This class will always complete async requests synchronously
    //
    internal sealed class SyncMemoryStream: MemoryStream {
        private int m_ReadTimeout;
        private int m_WriteTimeout;

        internal SyncMemoryStream(byte[] bytes): base(bytes, false)
        {
            m_ReadTimeout = m_WriteTimeout = System.Threading.Timeout.Infinite;
        }
        //
        internal SyncMemoryStream(int initialCapacity): base(initialCapacity)
        {
            m_ReadTimeout = m_WriteTimeout = System.Threading.Timeout.Infinite;
        }
        public override IAsyncResult BeginRead(byte[] buffer, int offset, int count, AsyncCallback callback, Object state)
        {
            int result = Read(buffer, offset, count);
            return new LazyAsyncResult(null, state, callback, result);
        }
        //
        public override int EndRead(IAsyncResult asyncResult)
        {
            LazyAsyncResult lazyResult = (LazyAsyncResult)asyncResult;
            return (int) lazyResult.InternalWaitForCompletion();
        }
        public override IAsyncResult BeginWrite(byte[] buffer, int offset, int count, AsyncCallback callback, Object state)
        {
            Write(buffer, offset, count);
            return new LazyAsyncResult(null, state, callback, null);
        }
        //
        public override void EndWrite(IAsyncResult asyncResult)
        {
            LazyAsyncResult lazyResult = (LazyAsyncResult)asyncResult;
            lazyResult.InternalWaitForCompletion();
        }
        //
        public override bool CanTimeout {
            get {
                return true;
            }
        }
        public override int ReadTimeout {
            get {
                return m_ReadTimeout;
            }
            set {
                m_ReadTimeout = value;
            }
        }
        public override int WriteTimeout {
            get {
                return m_WriteTimeout;
            }
            set {
                m_WriteTimeout = value;
            }
        }
    }
}
