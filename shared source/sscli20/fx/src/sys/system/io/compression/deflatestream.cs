//------------------------------------------------------------------------------
// <copyright file="DeflateStream.cs" company="Microsoft">
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

namespace System.IO.Compression {
    using System.IO;
    using System.Diagnostics;
    using System.Threading;
    using System.Security.Permissions;

    public class DeflateStream : Stream {
        private const int bufferSize = 4096;

        internal delegate void AsyncWriteDelegate(byte[] array, int offset, int count, bool isAsync);

        private Stream _stream;
        private CompressionMode _mode;
        private bool _leaveOpen;
        private Inflater inflater;
        private Deflater deflater;
        private byte[] buffer;
        
        private int asyncOperations; 
        private readonly AsyncCallback m_CallBack;
        private readonly AsyncWriteDelegate m_AsyncWriterDelegate;

        public DeflateStream(Stream stream, CompressionMode mode) : this( stream, mode, false, false) {
        }

        public DeflateStream(Stream stream, CompressionMode mode, bool leaveOpen) : this(stream, mode, leaveOpen, false){
        }
        
        internal DeflateStream(Stream stream, CompressionMode mode, bool leaveOpen, bool usingGZip){                                                                                          
            _stream = stream;
            _mode = mode;
            _leaveOpen = leaveOpen;

            if(_stream == null ) {
                throw new ArgumentNullException("stream");
            }

            switch (_mode) {
            case CompressionMode.Decompress:
                if (!(_stream.CanRead)) {
                    throw new ArgumentException(SR.GetString(SR.NotReadableStream), "stream");
                }
                inflater = new Inflater(usingGZip);
                m_CallBack = new AsyncCallback(ReadCallback); 
                break;

            case CompressionMode.Compress:
                if (!(_stream.CanWrite)) {
                    throw new ArgumentException(SR.GetString(SR.NotWriteableStream), "stream");
                }
                deflater = new Deflater(usingGZip);
                m_AsyncWriterDelegate = new AsyncWriteDelegate(this.InternalWrite);
                m_CallBack = new AsyncCallback(WriteCallback); 
                break;    

            default:
                throw new ArgumentException(SR.GetString(SR.ArgumentOutOfRange_Enum), "mode");
            }           
            buffer = new byte[bufferSize];
        }

        public override bool CanRead { 
            get {
                if( _stream == null) {
                    return false;      
                }

                return (_mode == CompressionMode.Decompress && _stream.CanRead);
            }
        }

        public override bool CanWrite { 
            get {
                if( _stream == null) {
                    return false;                
                }
                
                return (_mode == CompressionMode.Compress && _stream.CanWrite);
            }
        }

        public override bool CanSeek { 
            get {                
                return false;
            }
        }

        public override long Length { 
            get {
                throw new NotSupportedException(SR.GetString(SR.NotSupported));            
            }
        }

        public override long Position { 
            get {
                throw new NotSupportedException(SR.GetString(SR.NotSupported));            
            } 
            
            set {
                throw new NotSupportedException(SR.GetString(SR.NotSupported));            
            }
        }
        
        public override void Flush() {
            if( _stream == null) {
                throw new ObjectDisposedException(null, SR.GetString(SR.ObjectDisposed_StreamClosed));
            }
            return;
        }

        public override long Seek(long offset, SeekOrigin origin) {
            throw new NotSupportedException(SR.GetString(SR.NotSupported));            
        }

        public override void SetLength(long value) {
            throw new NotSupportedException(SR.GetString(SR.NotSupported));            
        }

        public override int Read(byte[] array, int offset, int count) {
            EnsureDecompressionMode();
            ValidateParameters(array, offset, count);

            int bytesRead;
            int currentOffest = offset;
            int remainingCount = count;

            while(true) {
                bytesRead = inflater.Inflate(array, currentOffest, remainingCount); 
                currentOffest += bytesRead;
                remainingCount -= bytesRead;

                if( remainingCount == 0) {
                    break;
                }
               
                if (inflater.Finished() ) { 
                    // if we finished decompressing, we can't have anything left in the outputwindow.
                    Debug.Assert(inflater.AvailableOutput == 0, "We should have copied all stuff out!");
                    break;                    
                }                

                Debug.Assert(inflater.NeedsInput(), "We can only run into this case if we are short of input");

                int bytes = _stream.Read( buffer, 0, buffer.Length);
                if( bytes == 0) {
                    break;      //Do we want to throw an exception here?
                }

                inflater.SetInput(buffer, 0 , bytes);
            }

            return count - remainingCount;
        }

        private void ValidateParameters(byte[] array, int offset, int count) {
            if (array==null) {
                throw new ArgumentNullException("array");
            }
            
            if (offset < 0) {
                throw new ArgumentOutOfRangeException("offset");
            }

            if (count < 0) {
                throw new ArgumentOutOfRangeException("count");
            }

            if (array.Length - offset < count) {
                throw new ArgumentException(SR.GetString(SR.InvalidArgumentOffsetCount));       
            }
            
            if (_stream == null ) {
                throw new ObjectDisposedException(null, SR.GetString(SR.ObjectDisposed_StreamClosed));
            }
        }

        private void EnsureDecompressionMode() {
            if( _mode != CompressionMode.Decompress) {
                throw new InvalidOperationException(SR.GetString(SR.CannotReadFromDeflateStream));
            }
        }

        private void EnsureCompressionMode() {
            if( _mode != CompressionMode.Compress) {
                throw new InvalidOperationException(SR.GetString(SR.CannotWriteToDeflateStream));
            }
        }

        [HostProtection(ExternalThreading=true)]
        public override IAsyncResult BeginRead(byte[] array, int offset, int count, AsyncCallback asyncCallback, object asyncState) {
            EnsureDecompressionMode();
            if (asyncOperations != 0 ) {
                throw new InvalidOperationException(SR.GetString(SR.InvalidBeginCall));
            }
            Interlocked.Increment(ref asyncOperations);

            try {
                ValidateParameters(array, offset, count);

                DeflateStreamAsyncResult userResult = new DeflateStreamAsyncResult(
                        this, asyncState, asyncCallback, array, offset, count);
                userResult.isWrite = false;

                // Try to read decompressed data in output buffer
                int bytesRead = inflater.Inflate(array, offset, count);
                if( bytesRead != 0) {
                    // If decompression output buffer is not empty, return immediately.
                    // 'true' means we complete synchronously.
                    userResult.InvokeCallback(true, (object) bytesRead);                      
                    return userResult;
                }

                if (inflater.Finished() ) {  
                    // end of compression stream
                    userResult.InvokeCallback(true, (object) 0);  
                    return userResult;
                }
                    
                // If there is no data on the output buffer and we are not at 
                // the end of the stream, we need to get more data from the base stream
                _stream.BeginRead(buffer, 0, buffer.Length, m_CallBack, userResult);   
                userResult.m_CompletedSynchronously &= userResult.IsCompleted;            
                
                return userResult;
            }
            catch {
                Interlocked.Decrement( ref asyncOperations);
                throw;
            }
        }

        // callback function for asynchrous reading on base stream
        private void ReadCallback(IAsyncResult baseStreamResult) {
            DeflateStreamAsyncResult outerResult = (DeflateStreamAsyncResult) baseStreamResult.AsyncState;
            outerResult.m_CompletedSynchronously &= baseStreamResult.CompletedSynchronously;
            int bytesRead = 0;

            try {
                bytesRead = _stream.EndRead(baseStreamResult);
            }
            catch (Exception exc) {
                // Defer throwing this until EndXxx where we are ensured of user code on the stack.
                outerResult.InvokeCallback(exc); 
                return;
            }

            if (bytesRead <= 0 ) {
                // This indicates the base stream has received EOF
                outerResult.InvokeCallback((object) 0);  
                return;
            }

            // Feed the data from base stream into decompression engine
            inflater.SetInput(buffer, 0 , bytesRead);
            bytesRead = inflater.Inflate(outerResult.buffer, outerResult.offset, outerResult.count); 
            if( bytesRead == 0 && !inflater.Finished()) {  
                // We could have read in head information and didn't get any data.
                // Read from the base stream again.   
                // Need to solve recusion.
                _stream.BeginRead(buffer, 0, buffer.Length, m_CallBack, outerResult);                   
            }
            else {
                outerResult.InvokeCallback((object) bytesRead);              
            }
        }

        public override int EndRead(IAsyncResult asyncResult) {
            EnsureDecompressionMode();

            if (asyncOperations != 1) {
                throw new InvalidOperationException(SR.GetString(SR.InvalidEndCall));
            }

            if (asyncResult == null) {
                throw new ArgumentNullException("asyncResult");
            }

            if (_stream == null ) {
                throw new InvalidOperationException(SR.GetString(SR.ObjectDisposed_StreamClosed));
            }

            DeflateStreamAsyncResult myResult = asyncResult as DeflateStreamAsyncResult;

            if (myResult == null) {
                throw new ArgumentNullException("asyncResult");
            }

            try {
                if (!myResult.IsCompleted) {
                    myResult.AsyncWaitHandle.WaitOne();
                }
            }
            finally {
                Interlocked.Decrement(ref asyncOperations); 
                // this will just close the wait handle
                myResult.Close();
            }

            if (myResult.Result is Exception) {
                throw (Exception)(myResult.Result);
            }

            return (int)myResult.Result;
        }


        public override void Write(byte[] array, int offset, int count) {
            EnsureCompressionMode();
            ValidateParameters(array, offset, count);
            InternalWrite(array, offset, count, false);
        }

        internal void InternalWrite(byte[] array, int offset, int count, bool isAsync) {
            int currentOffest = offset;
            int remainingCount = count;
            int bytesCompressed;

            // compressed the bytes we already passed to the deflater
            while(!deflater.NeedsInput()) {
                bytesCompressed = deflater.GetDeflateOutput(buffer);
                if( bytesCompressed != 0) {
                    if (isAsync) {
                        IAsyncResult result = _stream.BeginWrite(buffer, 0,  bytesCompressed, null, null);
                        _stream.EndWrite(result);
                    }
                    else 
                        _stream.Write(buffer, 0,  bytesCompressed);
                }
            }

            deflater.SetInput(array, offset, count);

            // compressed the new input
            while(!deflater.NeedsInput()) {
                bytesCompressed = deflater.GetDeflateOutput(buffer);
                if( bytesCompressed != 0) {
                    if (isAsync) {
                        IAsyncResult result = _stream.BeginWrite(buffer, 0,  bytesCompressed, null, null);
                        _stream.EndWrite(result);
                    }
                    else 
                        _stream.Write(buffer, 0,  bytesCompressed);
                }
            }
        }

        protected override void Dispose(bool disposing) {
            try {
                // Flush on the underlying stream can throw (ex., low disk space)
                if (disposing && _stream != null) {
                    Flush();

                    // Need to do close the output stream in compression mode
                    if( _mode == CompressionMode.Compress && _stream != null) {
                        int bytesCompressed;
                        // compress any bytes left.
                        while(!deflater.NeedsInput()) {
                            bytesCompressed = deflater.GetDeflateOutput(buffer);
                            if( bytesCompressed != 0) {
                                _stream.Write(buffer, 0,  bytesCompressed);
                            }
                        }
                        
                        // Write the end of compressed stream data.
                        // We can safely do this since the buffer is large enough.
                        bytesCompressed = deflater.Finish(buffer);  

                        if (bytesCompressed > 0)
                            _stream.Write(buffer, 0,  bytesCompressed);
                    }
                }
            }
            finally {
                try {
                    // Attempt to close the stream even if there was an IO error from Flushing.
                    // Note that Stream.Close() can potentially throw here (may or may not be
                    // due to the same Flush error). In this case, we still need to ensure 
                    // cleaning up internal resources, hence the finally block.  
                    if(disposing && !_leaveOpen && _stream != null) {
                        _stream.Close();
                    }
                }
                finally {
                    _stream = null;
                    base.Dispose(disposing);
                }
            }
        }


        [HostProtection(ExternalThreading=true)]
        public override IAsyncResult BeginWrite(byte[] array, int offset, int count, AsyncCallback asyncCallback, object asyncState) {
            EnsureCompressionMode();
            if (asyncOperations != 0 ) {
                throw new InvalidOperationException(SR.GetString(SR.InvalidBeginCall));
            }
            Interlocked.Increment(ref asyncOperations);

            try {
                ValidateParameters(array, offset, count);

                DeflateStreamAsyncResult userResult = new DeflateStreamAsyncResult(
                        this, asyncState, asyncCallback, array, offset, count);
                userResult.isWrite = true;

                m_AsyncWriterDelegate.BeginInvoke(array, offset, count, true, m_CallBack, userResult);
                userResult.m_CompletedSynchronously &= userResult.IsCompleted;            
                                
                return userResult;
            }
            catch {
                Interlocked.Decrement( ref asyncOperations);
                throw;
            }
        }

        // callback function for asynchrous reading on base stream
        private void WriteCallback(IAsyncResult asyncResult) {
            DeflateStreamAsyncResult outerResult = (DeflateStreamAsyncResult) asyncResult.AsyncState;
            outerResult.m_CompletedSynchronously &= asyncResult.CompletedSynchronously;            

            try {
                m_AsyncWriterDelegate.EndInvoke(asyncResult);
            }
            catch (Exception exc) {
                // Defer throwing this until EndXxx where we are ensured of user code on the stack.
                outerResult.InvokeCallback(exc); 
                return;
            }
            outerResult.InvokeCallback(null);              
        }

        public override void EndWrite(IAsyncResult asyncResult) {
            EnsureCompressionMode();

            if (asyncOperations != 1) {
                throw new InvalidOperationException(SR.GetString(SR.InvalidEndCall));
            }

            if (asyncResult == null) {
                throw new ArgumentNullException("asyncResult");
            }

            if (_stream == null ) {
                throw new InvalidOperationException(SR.GetString(SR.ObjectDisposed_StreamClosed));
            }

            DeflateStreamAsyncResult myResult = asyncResult as DeflateStreamAsyncResult;

            if (myResult == null) {
                throw new ArgumentNullException("asyncResult");
            }

            try {
                if (!myResult.IsCompleted) {
                    myResult.AsyncWaitHandle.WaitOne();
                }
            }
            finally {
                Interlocked.Decrement(ref asyncOperations); 
                // this will just close the wait handle
                myResult.Close();
            }

            if (myResult.Result is Exception) {
                throw (Exception)(myResult.Result);
            }
        }


        public Stream BaseStream { 
            get {
                return _stream;
            }
        }
    }
    

    internal class DeflateStreamAsyncResult : IAsyncResult {
        public byte[]   buffer;
        public int      offset;
        public int      count;
        public bool     isWrite;
        
        private object m_AsyncObject;               // Caller's async object.
        private object m_AsyncState;                // Caller's state object.
        private AsyncCallback m_AsyncCallback;      // Caller's callback method.

        private object m_Result;                     // Final IO result to be returned byt the End*() method.
        internal bool m_CompletedSynchronously;      // true if the operation completed synchronously.
        private int m_InvokedCallback;               // 0 is callback is not called
        private int m_Completed;                     // 0 if not completed >0 otherwise.
        private object m_Event;                      // lazy allocated event to be returned in the IAsyncResult for the client to wait on

        public DeflateStreamAsyncResult(object asyncObject, object asyncState,
                                   AsyncCallback asyncCallback,
                                   byte[] buffer, int offset, int count) {

                this.buffer      = buffer;
                this.offset      = offset;
                this.count     = count;
                m_CompletedSynchronously = true;
                m_AsyncObject = asyncObject;
                m_AsyncState = asyncState;
                m_AsyncCallback = asyncCallback;
        }

        // Interface method to return the caller's state object.
        public object AsyncState {
            get {
                return m_AsyncState;
            }
        }

        // Interface property to return a WaitHandle that can be waited on for I/O completion.
        // This property implements lazy event creation.
        // the event object is only created when this property is accessed,
        // since we're internally only using callbacks, as long as the user is using
        // callbacks as well we will not create an event at all.
        public WaitHandle AsyncWaitHandle {
            get {
                // save a copy of the completion status
                int savedCompleted = m_Completed;
                if (m_Event == null) {
                    // lazy allocation of the event:
                    // if this property is never accessed this object is never created
                    Interlocked.CompareExchange(ref m_Event, new ManualResetEvent(savedCompleted != 0), null);
                }

                ManualResetEvent castedEvent = (ManualResetEvent)m_Event;
                if (savedCompleted == 0 && m_Completed != 0) {
                    // if, while the event was created in the reset state,
                    // the IO operation completed, set the event here.
                    castedEvent.Set();
                }
                return castedEvent;
            }
        }

        // Interface property, returning synchronous completion status.
        public bool CompletedSynchronously {
            get {
                return m_CompletedSynchronously;
            }
        }

        // Interface property, returning completion status.
        public bool IsCompleted {
            get {
                return m_Completed != 0;
            }
        }

        // Internal property for setting the IO result.
        internal object Result {
            get {
                return m_Result;
            }
        }

        internal void Close() {
            if (m_Event != null) {
                ((ManualResetEvent)m_Event).Close();
            }            
        }

        internal void InvokeCallback(bool completedSynchronously, object result) {
            Complete(completedSynchronously, result);
        }

        internal void InvokeCallback(object result) {
            Complete(result);
        }

        // Internal method for setting completion.
        // As a side effect, we'll signal the WaitHandle event and clean up.
        private void Complete(bool completedSynchronously, object result) {
            m_CompletedSynchronously = completedSynchronously;
            Complete(result);
        }

        private void Complete(object result) {
            m_Result = result;
            
            // Set IsCompleted and the event only after the usercallback method. 
            Interlocked.Increment( ref m_Completed );
            
            if (m_Event != null) {
                ((ManualResetEvent)m_Event).Set();
            }

            if (Interlocked.Increment(ref m_InvokedCallback)==1) {
                if( m_AsyncCallback != null) {
                    m_AsyncCallback(this);
                }
            }
        }

    }
}



