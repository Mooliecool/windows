//------------------------------------------------------------------------------
// <copyright file="_CacheStreams.cs" company="Microsoft">
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

/*++    

Abstract:
    The file contains two streams used in conjunction with caching.
    The first class will combine two streams for reading into just one continued stream.
    The second class will forward (as writes) to external stream all reads issued on a "this" stream.

Author:

    Alexei Vopilov    21-Dec-2002

Revision History:

--*/
namespace System.Net.Cache {
using System;
using System.Net;
using System.IO;
using System.Threading;
using System.Collections.Specialized;


    //
    // This stream will take two Streams (head and tail) and combine them into a single stream
    // Only read IO is supported!
    //
    internal class CombinedReadStream: Stream, ICloseEx {
        private Stream  m_HeadStream;
        private Stream  m_TailStream;
        private bool    m_HeadEOF;
        private long    m_HeadLength;
        private int     m_ReadNesting;
        private AsyncCallback m_ReadCallback;   //lazy initialized


        internal CombinedReadStream(Stream headStream, Stream tailStream) {
            m_HeadStream = headStream;
            m_TailStream = tailStream;
            m_HeadEOF = headStream == Stream.Null;
        }

        public override bool CanRead {
            get {return m_HeadEOF? m_TailStream.CanRead: m_HeadStream.CanRead;}
        }

        // If CanSeek is false, Position, Seek, Length, and SetLength should throw.
        public override bool CanSeek {
            get {return false;}
        }

        public override bool CanWrite {
            get {return false;}
        }

        public override long Length {
            get {
                return m_TailStream.Length + (m_HeadEOF? m_HeadLength: m_HeadStream.Length);
            }
        }

        public override long Position {
            get {
                return m_TailStream.Position + (m_HeadEOF? m_HeadLength: m_HeadStream.Position);
            }

            set {
                throw new NotSupportedException(SR.GetString(SR.net_noseek));
            }
        }

        public override long Seek(long offset, SeekOrigin origin) {
            throw new NotSupportedException(SR.GetString(SR.net_noseek));
        }

        public override void SetLength(long value) {
            throw new NotSupportedException(SR.GetString(SR.net_noseek));
        }

        public override void Write(byte[] buffer, int offset, int count) {
            throw new NotSupportedException(SR.GetString(SR.net_noseek));
        }

        public override IAsyncResult BeginWrite(byte[] buffer, int offset, int count, AsyncCallback callback, Object state) {
            throw new NotSupportedException(SR.GetString(SR.net_noseek));
        }

        public override void EndWrite(IAsyncResult asyncResult) {
            throw new NotSupportedException(SR.GetString(SR.net_noseek));
        }

        public override void Flush() {
        }

        public override int Read(byte[] buffer, int offset, int count) {

            try {
                if (Interlocked.Increment(ref m_ReadNesting) != 1) {
                    throw new NotSupportedException(SR.GetString(SR.net_io_invalidnestedcall, "Read", "read"));
                }

                if (m_HeadEOF) {
                    return m_TailStream.Read(buffer, offset, count);
                }
                else {
                    int result = m_HeadStream.Read(buffer, offset, count);
                    m_HeadLength += result;
                    if (result == 0 && count != 0) {
                        m_HeadEOF = true;
                        m_HeadStream.Close();
                        result = m_TailStream.Read(buffer, offset, count);
                    }
                    return result;
                }
            }
            finally {
                Interlocked.Decrement(ref m_ReadNesting);
            }

        }


        //
        // This is a wrapper result used to substitue the AsyncResult returned from m_HeadStream IO
        // Note that once seen a EOF on m_HeadStream we will stop using this wrapper.
        //
        private class InnerAsyncResult: LazyAsyncResult {
            public byte[] Buffer;
            public int    Offset;
            public int    Count;

            public InnerAsyncResult(object userState, AsyncCallback userCallback, byte[] buffer, int offset, int count)
            :base (null, userState, userCallback) {

                Buffer = buffer;
                Offset = offset;
                Count  = count;
            }

        }

        private void ReadCallback(IAsyncResult transportResult) {
            GlobalLog.Assert(transportResult.AsyncState is InnerAsyncResult, "InnerAsyncResult::ReadCallback|The state expected to be of type InnerAsyncResult, received {0}.", transportResult.GetType().FullName);
            if (transportResult.CompletedSynchronously)
            {
                return;
            }

            InnerAsyncResult userResult = transportResult.AsyncState as InnerAsyncResult;
            try {
                // Complete transport IO, in this callback that is always the head stream
                int count;
                if (!m_HeadEOF) {
                    count = m_HeadStream.EndRead(transportResult);
                    m_HeadLength += count;
                }
                else {
                    count = m_TailStream.EndRead(transportResult);
                }


                //check on EOF condition
                if (!m_HeadEOF && count == 0 && userResult.Count != 0) {
                    //Got a first stream EOF
                    m_HeadEOF = true;
                    m_HeadStream.Close();
                    IAsyncResult ar = m_TailStream.BeginRead(userResult.Buffer, userResult.Offset, userResult.Count, m_ReadCallback, userResult);
                    if (!ar.CompletedSynchronously) {
                        return;
                    }
                    count = m_TailStream.EndRead(ar);
                }
                // just complete user IO
                userResult.Buffer = null;
                userResult.InvokeCallback(count);
            }
            catch (Exception e) {
                //ASYNC: try to swallow even serious exceptions (nothing to loose?)
                if (userResult.InternalPeekCompleted)
                    throw;

                userResult.InvokeCallback(e);
            }
            catch {
                //ASYNC: try to swallow even serious exceptions (nothing to loose?)
                if (userResult.InternalPeekCompleted)
                    throw;

                userResult.InvokeCallback(new Exception(SR.GetString(SR.net_nonClsCompliantException)));
            }
        }

        public override IAsyncResult BeginRead(byte[] buffer, int offset, int count, AsyncCallback callback, Object state) {
            try {
                if (Interlocked.Increment(ref m_ReadNesting) != 1) {
                    throw new NotSupportedException(SR.GetString(SR.net_io_invalidnestedcall, "BeginRead", "read"));
                }

                if (m_ReadCallback == null) {
                    m_ReadCallback = new AsyncCallback(ReadCallback);
                }

                if (m_HeadEOF) {
                    return m_TailStream.BeginRead(buffer, offset, count, callback, state);
                }
                else {
                    InnerAsyncResult userResult = new InnerAsyncResult(state, callback, buffer, offset, count);
                    IAsyncResult ar = m_HeadStream.BeginRead(buffer, offset, count, m_ReadCallback, userResult);

                    if (!ar.CompletedSynchronously)
                    {
                        return userResult;
                    }

                    int bytes = m_HeadStream.EndRead(ar);
                    m_HeadLength += bytes;

                    //check on EOF condition
                    if (bytes == 0 && userResult.Count != 0) {
                        //Got a first stream EOF
                        m_HeadEOF = true;
                        m_HeadStream.Close();
                        return m_TailStream.BeginRead(buffer, offset, count, callback, state);
                    }
                    else {
                        // just complete user IO
                        userResult.Buffer = null;
                        userResult.InvokeCallback(count);
                        return userResult;
                    }

                }
            }
            catch {
                Interlocked.Decrement(ref m_ReadNesting);
                throw;
            }
        }

        public override int EndRead(IAsyncResult asyncResult) {

            if (Interlocked.Decrement(ref m_ReadNesting) != 0) {
                Interlocked.Increment(ref m_ReadNesting);
                throw new InvalidOperationException(SR.GetString(SR.net_io_invalidendcall, "EndRead"));
            }

            if (asyncResult == null) {
                throw new ArgumentNullException("asyncResult");
            }

            InnerAsyncResult myResult = asyncResult as InnerAsyncResult;

            if (myResult == null) {
                // We are just passing IO down, although m_HeadEOF should be always true here.
                GlobalLog.Assert(m_HeadEOF, "CombinedReadStream::EndRead|m_HeadEOF is false and asyncResult is not of InnerAsyncResult type {0).", asyncResult.GetType().FullName);
                return m_HeadEOF? m_TailStream.EndRead(asyncResult): m_HeadStream.EndRead(asyncResult);
            }

            // this is our wrapped AsyncResult
            myResult.InternalWaitForCompletion();

            // Exception?
            if (myResult.Result is Exception) {
                throw (Exception)(myResult.Result);
            }

            // Report the count read
            return (int)myResult.Result;
        }

        // Subclasses should use Dispose(bool, CloseExState)
        protected override sealed void Dispose(bool disposing) {
            Dispose(disposing, CloseExState.Normal);
            GC.SuppressFinalize(this);
        }

        void ICloseEx.CloseEx(CloseExState closeState) {
            Dispose(true, closeState);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing, CloseExState closeState) {

            // All below calls should already be idempotent

            try {
                if (!m_HeadEOF) {
                    ICloseEx icloseEx = m_HeadStream as ICloseEx;
                    if (icloseEx != null) {
                        icloseEx.CloseEx(closeState);
                    }
                    else {
                        m_HeadStream.Close();
                    }
                }
            }
            finally {
                ICloseEx icloseEx = m_TailStream as ICloseEx;
                if (icloseEx != null) {
                    icloseEx.CloseEx(closeState);
                }
                else {
                    m_TailStream.Close();
                }
            }

            if (!disposing) {
                m_HeadStream = null;
                m_TailStream = null;
            }
        }

        public override bool CanTimeout {
            get {
                return m_TailStream.CanTimeout && m_HeadStream.CanTimeout;
            }
        }

        public override int ReadTimeout {
            get {
                return (m_HeadEOF) ? m_TailStream.ReadTimeout : m_HeadStream.ReadTimeout;
            }
            set {
                m_TailStream.ReadTimeout = m_HeadStream.ReadTimeout = value;
            }
        }

        public override int WriteTimeout {
            get {
                return (m_HeadEOF) ? m_TailStream.WriteTimeout : m_HeadStream.WriteTimeout;
            }
            set {
                m_TailStream.WriteTimeout = m_HeadStream.WriteTimeout = value;
            }
        }
    }

    //
    // This stream will plug into a stream and listen for all reads on it
    // It is also constructed with yet another stream used for multiplexing IO to
    //
    // When it sees a read on this stream the result gets forwarded as write to a shadow stream.
    // ONLY READ IO is supported!
    //
    internal class ForwardingReadStream: Stream, ICloseEx {
        private Stream  m_OriginalStream;
        private Stream  m_ShadowStream;
        private int     m_ReadNesting;
        private bool    m_ShadowStreamIsDead;
        private AsyncCallback m_ReadCallback;   // lazy initialized
        private long    m_BytesToSkip;       // suppress from the read first number of bytes
        private bool    m_ThrowOnWriteError;
        private bool    m_SeenReadEOF;


        internal ForwardingReadStream(Stream originalStream, Stream shadowStream, long bytesToSkip, bool throwOnWriteError) {
            if (!shadowStream.CanWrite) {
                throw new ArgumentException(SR.GetString(SR.net_cache_shadowstream_not_writable), "shadowStream");
            }
            m_OriginalStream = originalStream;
            m_ShadowStream = shadowStream;
            m_BytesToSkip = bytesToSkip;
            m_ThrowOnWriteError = throwOnWriteError;
        }

        public override bool CanRead {
            get {return m_OriginalStream.CanRead;}
        }

        // If CanSeek is false, Position, Seek, Length, and SetLength should throw.
        public override bool CanSeek {
            get {return false;}
        }

        public override bool CanWrite {
            get {return false;}
        }

        public override long Length {
            get {
                return m_OriginalStream.Length - m_BytesToSkip;
            }
        }

        public override long Position {
            get {
                return m_OriginalStream.Position - m_BytesToSkip;
            }

            set {
                throw new NotSupportedException(SR.GetString(SR.net_noseek));
            }
        }

        public override long Seek(long offset, SeekOrigin origin) {
            throw new NotSupportedException(SR.GetString(SR.net_noseek));
        }

        public override void SetLength(long value) {
            throw new NotSupportedException(SR.GetString(SR.net_noseek));
        }

        public override void Write(byte[] buffer, int offset, int count) {
            throw new NotSupportedException(SR.GetString(SR.net_noseek));
        }

        public override IAsyncResult BeginWrite(byte[] buffer, int offset, int count, AsyncCallback callback, Object state) {
            throw new NotSupportedException(SR.GetString(SR.net_noseek));
        }

        public override void EndWrite(IAsyncResult asyncResult) {
            throw new NotSupportedException(SR.GetString(SR.net_noseek));
        }

        public override void Flush() {
        }

        public override int Read(byte[] buffer, int offset, int count) {

            bool isDoingWrite = false;
            int result = -1;
            if (Interlocked.Increment(ref m_ReadNesting) != 1) {
                throw new NotSupportedException(SR.GetString(SR.net_io_invalidnestedcall, "Read", "read"));
            }

            try {

                if (m_BytesToSkip != 0L) {
                    // Sometime we want to combine cached + live stream AND the user requested explicit range starts from not 0
                    byte[] tempBuffer = new byte[4096];
                    while (m_BytesToSkip != 0L) {
                        int bytes = m_OriginalStream.Read(tempBuffer, 0, (m_BytesToSkip < (long)tempBuffer.Length? (int)m_BytesToSkip: tempBuffer.Length));
                        if (bytes == 0)
                            m_SeenReadEOF = true;

                        m_BytesToSkip -= bytes;
                        if (!m_ShadowStreamIsDead)
                            m_ShadowStream.Write(tempBuffer, 0, bytes);
                    }
                }

                result = m_OriginalStream.Read(buffer, offset, count);
                if (result == 0)
                    m_SeenReadEOF = true;

                if (m_ShadowStreamIsDead) {
                    return result;
                }
                isDoingWrite = true;
                m_ShadowStream.Write(buffer, offset, result);
                return result;
            }
            catch (Exception e) {
                if (e is ThreadAbortException || e is StackOverflowException || e is OutOfMemoryException)
                    throw;

                GlobalLog.Print("ShadowReadStream::Read() Got Exception, disabling the shadow stream, stack trace = " + e.ToString());
                if (!m_ShadowStreamIsDead) {
                    // try to swallow even serious exception, since got nothing to loose?
                    m_ShadowStreamIsDead = true;
                    try {
                        if (m_ShadowStream is ICloseEx)
                            ((ICloseEx)m_ShadowStream).CloseEx(CloseExState.Abort | CloseExState.Silent);
                        else
                            m_ShadowStream.Close();
                    }
                    catch (Exception ee) {
                        if (e is ThreadAbortException || e is StackOverflowException || e is OutOfMemoryException)
                            throw;
                        GlobalLog.Print("ShadowReadStream::Read() Got (ignoring) Exception, on shadow stream.Close, stack trace = " + ee.ToString());
                    }
                    catch {
                        GlobalLog.Print("ShadowReadStream::Read() Got (ignoring) Exception, on shadow stream.Close, stack trace = Non-CLS Compliant Exception");
                    }
                }
                if (!isDoingWrite || m_ThrowOnWriteError)
                    throw;

                return result;
            }
            catch {
                GlobalLog.Print("ShadowReadStream::Read() Got Exception, disabling the shadow stream, stack trace =  Non-CLS Compliant Exception");
                if (!m_ShadowStreamIsDead) {
                    // try to swallow even serious exception, since got nothing to loose?
                    m_ShadowStreamIsDead = true;
                    try {
                        if (m_ShadowStream is ICloseEx)
                            ((ICloseEx)m_ShadowStream).CloseEx(CloseExState.Abort | CloseExState.Silent);
                        else
                            m_ShadowStream.Close();
                    }
                    catch (Exception ee) {
                        if (NclUtilities.IsFatal(ee)) throw;
                        GlobalLog.Print("ShadowReadStream::Read() Got (ignoring) Exception, on shadow stream.Close, stack trace = " + ee.ToString());
                    }
                    catch {
                        GlobalLog.Print("ShadowReadStream::Read() Got (ignoring) Exception, on shadow stream.Close, stack trace =  Non-CLS Compliant Exception");
                    }
                }
                if (!isDoingWrite || m_ThrowOnWriteError)
                    throw;

                return result;
            }
            finally {
                Interlocked.Decrement(ref m_ReadNesting);
            }
        }


        //
        // This is a wrapper result used to substitue the AsyncResult returned from m_OriginalStream IO
        // Note that once seen a m_ShadowStream error we will stop using this wrapper.
        //
        private class InnerAsyncResult: LazyAsyncResult {
            public byte[] Buffer;
            public int    Offset;
            public int    Count;
            public bool   IsWriteCompletion;

            public InnerAsyncResult(object userState, AsyncCallback userCallback, byte[] buffer, int offset, int count)
            :base (null, userState, userCallback) {

                Buffer = buffer;
                Offset = offset;
                Count  = count;
            }

        }

        private void ReadCallback(IAsyncResult transportResult) {
            GlobalLog.Assert(transportResult.AsyncState is InnerAsyncResult, "InnerAsyncResult::ReadCallback|The state expected to be of type InnerAsyncResult, received {0}.", transportResult.GetType().FullName);
            if (transportResult.CompletedSynchronously)
            {
                return;
            }

            // Recover our asyncResult
            InnerAsyncResult userResult = transportResult.AsyncState as InnerAsyncResult;

            ReadComplete(transportResult);
        }

        private void ReadComplete(IAsyncResult transportResult)
        {
            while(true)
            {
                // Recover our asyncResult
                InnerAsyncResult userResult = transportResult.AsyncState as InnerAsyncResult;

                try
                {
                    if (!userResult.IsWriteCompletion)
                    {
                        userResult.Count = m_OriginalStream.EndRead(transportResult);
                        if (userResult.Count == 0)
                            m_SeenReadEOF = true;


                        if (!m_ShadowStreamIsDead) {
                            userResult.IsWriteCompletion = true;
                            //Optionally charge notification write IO
                            transportResult = m_ShadowStream.BeginWrite(userResult.Buffer, userResult.Offset, userResult.Count, m_ReadCallback, userResult);
                            if (transportResult.CompletedSynchronously)
                            {
                                continue;
                            }
                            return;
                        }
                    }
                    else
                    {
                        GlobalLog.Assert(!m_ShadowStreamIsDead, "ForwardingReadStream::ReadComplete|ERROR: IsWriteCompletion && m_ShadowStreamIsDead");

                        m_ShadowStream.EndWrite(transportResult);
                        userResult.IsWriteCompletion = false;
                    }
                }
                catch (Exception e)
                {
                    //ASYNC: try to swallow even serious exceptions (nothing to loose?)
                    if (userResult.InternalPeekCompleted)
                    {
                        GlobalLog.Print("ShadowReadStream::ReadComplete() Rethrowing Exception (end), userResult.IsCompleted, stack trace = " + e.ToString());
                        throw;
                    }

                    try
                    {
                        m_ShadowStreamIsDead = true;
                        if (m_ShadowStream is ICloseEx)
                            ((ICloseEx)m_ShadowStream).CloseEx(CloseExState.Abort | CloseExState.Silent);
                        else
                            m_ShadowStream.Close();
                    }
                    catch (Exception ee)
                    {
                        //ASYNC: Again try to swallow even serious exceptions
                        GlobalLog.Print("ShadowReadStream::ReadComplete() Got (ignoring) Exception, on shadow stream.Close, stack trace = " + ee.ToString());
                    }
                    catch
                    {
                        //ASYNC: Again try to swallow even serious exceptions
                        GlobalLog.Print("ShadowReadStream::ReadComplete() Got (ignoring) Exception, on shadow stream.Close, stack trace =  Non-CLS Compliant Exception");
                    }

                    if (!userResult.IsWriteCompletion || m_ThrowOnWriteError)
                    {
                        if (transportResult.CompletedSynchronously)
                        {
                            throw;
                        }

                        userResult.InvokeCallback(e);
                        return;
                    }
                }
                catch {
                    //ASYNC: try to swallow even serious exceptions (nothing to loose?)
                    if (userResult.InternalPeekCompleted)
                    {
                        GlobalLog.Print("ShadowReadStream::ReadComplete() Rethrowing Exception (end), userResult.IsCompleted, stack trace =  Non-CLS Compliant Exception");
                        throw;
                    }

                    try
                    {
                        m_ShadowStreamIsDead = true;
                        if (m_ShadowStream is ICloseEx)
                            ((ICloseEx)m_ShadowStream).CloseEx(CloseExState.Abort | CloseExState.Silent);
                        else
                            m_ShadowStream.Close();
                    }
                    catch (Exception ee)
                    {
                        //ASYNC: Again try to swallow even serious exceptions
                        GlobalLog.Print("ShadowReadStream::ReadComplete() Got (ignoring) Exception, on shadow stream.Close, stack trace = " + ee.ToString());
                    }
                    catch
                    {
                        //ASYNC: Again try to swallow even serious exceptions
                        GlobalLog.Print("ShadowReadStream::ReadComplete() Got (ignoring) Exception, on shadow stream.Close, stack trace =  Non-CLS Compliant Exception");
                    }

                    if (!userResult.IsWriteCompletion || m_ThrowOnWriteError)
                    {
                        if (transportResult.CompletedSynchronously)
                        {
                            throw;
                        }

                        userResult.InvokeCallback(new Exception(SR.GetString(SR.net_nonClsCompliantException)));
                        return;
                    }
                }

                // Need to process, re-issue the read.
                try
                {
                    if (m_BytesToSkip != 0L) {
                        m_BytesToSkip -= userResult.Count;
                        userResult.Count = m_BytesToSkip < (long)userResult.Buffer.Length? (int)m_BytesToSkip: userResult.Buffer.Length;
                        if (m_BytesToSkip == 0L) {
                            // we did hide the original IO request in the outer iaresult state.
                            // charge the real user operation now
                            transportResult = userResult;
                            userResult = userResult.AsyncState as InnerAsyncResult;
                            GlobalLog.Assert(userResult != null, "ForwardingReadStream::ReadComplete|ERROR: Inner IAResult is null after stream FastForwarding.");
                        }
                        transportResult = m_OriginalStream.BeginRead(userResult.Buffer, userResult.Offset, userResult.Count, m_ReadCallback, userResult);
                        if (transportResult.CompletedSynchronously)
                        {
                            continue;
                        }
                        return;
                    }
                    //if came to here, complete original user IO
                    userResult.InvokeCallback(userResult.Count);
                    return;
                }
                catch (Exception e)
                {
                    //ASYNC: try to swallow even serious exceptions (nothing to loose?)
                    if (userResult.InternalPeekCompleted)
                    {
                        GlobalLog.Print("ShadowReadStream::ReadComplete() Rethrowing Exception (begin), userResult.IsCompleted, stack trace = " + e.ToString());
                        throw;
                    }

                    try
                    {
                        m_ShadowStreamIsDead = true;
                        if (m_ShadowStream is ICloseEx)
                            ((ICloseEx)m_ShadowStream).CloseEx(CloseExState.Abort | CloseExState.Silent);
                        else
                            m_ShadowStream.Close();
                    }
                    catch (Exception ee)
                    {
                        //ASYNC: Again try to swallow even serious exceptions
                        GlobalLog.Print("ShadowReadStream::ReadComplete() Got (ignoring) Exception, on shadow stream.Close (after begin), stack trace = " + ee.ToString());
                    }
                    catch
                    {
                        //ASYNC: Again try to swallow even serious exceptions
                        GlobalLog.Print("ShadowReadStream::ReadComplete() Got (ignoring) Exception, on shadow stream.Close (after begin), stack trace = Non-CLS Compliant Exception");
                    }

                    if (transportResult.CompletedSynchronously)
                    {
                        throw;
                    }

                    // This will set the exception result first then try to execute a user callback
                    userResult.InvokeCallback(e);
                    return;
                }
                catch
                {
                    //ASYNC: try to swallow even serious exceptions (nothing to loose?)
                    if (userResult.InternalPeekCompleted)
                    {
                        GlobalLog.Print("ShadowReadStream::ReadComplete() Rethrowing Exception (begin), userResult.IsCompleted, stack trace = Non-CLS Compliant Exception");
                        throw;
                    }

                    try
                    {
                        m_ShadowStreamIsDead = true;
                        if (m_ShadowStream is ICloseEx)
                            ((ICloseEx)m_ShadowStream).CloseEx(CloseExState.Abort | CloseExState.Silent);
                        else
                            m_ShadowStream.Close();
                    }
                    catch (Exception ee)
                    {
                        //ASYNC: Again try to swallow even serious exceptions
                        GlobalLog.Print("ShadowReadStream::ReadComplete() Got (ignoring) Exception, on shadow stream.Close (after begin), stack trace = " + ee.ToString());
                    }
                    catch
                    {
                        //ASYNC: Again try to swallow even serious exceptions
                        GlobalLog.Print("ShadowReadStream::ReadComplete() Got (ignoring) Exception, on shadow stream.Close (after begin), stack trace = Non-CLS Compliant Exception");
                    }

                    if (transportResult.CompletedSynchronously)
                    {
                        throw;
                    }

                    // This will set the exception result first then try to execute a user callback
                    userResult.InvokeCallback(new Exception(SR.GetString(SR.net_nonClsCompliantException)));
                    return;
                }
            }
        }

        public override IAsyncResult BeginRead(byte[] buffer, int offset, int count, AsyncCallback callback, Object state) {
            if (Interlocked.Increment(ref m_ReadNesting) != 1) {
                throw new NotSupportedException(SR.GetString(SR.net_io_invalidnestedcall, "BeginRead", "read"));
            }

            try {

                if (m_ReadCallback == null) {
                    m_ReadCallback = new AsyncCallback(ReadCallback);
                }

                if (m_ShadowStreamIsDead && m_BytesToSkip == 0L) {
                    return m_OriginalStream.BeginRead(buffer, offset, count, callback, state);
                }
                else {
                    InnerAsyncResult userResult = new InnerAsyncResult(state, callback, buffer, offset, count);
                    if (m_BytesToSkip != 0L) {
                        InnerAsyncResult temp = userResult;
                        userResult = new InnerAsyncResult(temp, null, new byte[4096],
                                                          0, m_BytesToSkip < (long) buffer.Length? (int)m_BytesToSkip: buffer.Length);
                    }
                    IAsyncResult result = m_OriginalStream.BeginRead(userResult.Buffer, userResult.Offset, userResult.Count, m_ReadCallback, userResult);
                    if (result.CompletedSynchronously)
                    {
                        ReadComplete(result);
                    }
                    return userResult;
                }
            }
            catch {
                Interlocked.Decrement(ref m_ReadNesting);
                throw;
            }
        }

        public override int EndRead(IAsyncResult asyncResult) {

            if (Interlocked.Decrement(ref m_ReadNesting) != 0) {
                Interlocked.Increment(ref m_ReadNesting);
                throw new InvalidOperationException(SR.GetString(SR.net_io_invalidendcall, "EndRead"));
            }

            if (asyncResult == null) {
                throw new ArgumentNullException("asyncResult");
            }

            InnerAsyncResult myResult = asyncResult as InnerAsyncResult;

            if (myResult == null) {
                // We are just passing IO down, although the shadow stream should be dead for now.
                GlobalLog.Assert(m_ShadowStreamIsDead, "ForwardingReadStream::EndRead|m_ShadowStreamIsDead is false and asyncResult is not of InnerAsyncResult type {0}.", asyncResult.GetType().FullName);
                int bytes = m_OriginalStream.EndRead(asyncResult);
                if (bytes == 0)
                    m_SeenReadEOF = true;
            }

            // this is our wrapped AsyncResult
            bool suceess = false;
            try {
                myResult.InternalWaitForCompletion();
                // Exception?
                if (myResult.Result is Exception)
                    throw (Exception)(myResult.Result);
                suceess = true;
            }
            finally {
                if (!suceess && !m_ShadowStreamIsDead) {
                    m_ShadowStreamIsDead = true;
                    if (m_ShadowStream is ICloseEx)
                        ((ICloseEx)m_ShadowStream).CloseEx(CloseExState.Abort | CloseExState.Silent);
                    else
                        m_ShadowStream.Close();
                }
            }

            // Report the read count
            return (int)myResult.Result;
        }

        // Subclasses should use Dispose(bool, CloseExState)
        protected sealed override void Dispose(bool disposing) {
            Dispose(disposing, CloseExState.Normal);
            GC.SuppressFinalize(this);
        }

        private int _Disposed;
        void ICloseEx.CloseEx(CloseExState closeState) {

            if (Interlocked.Increment(ref _Disposed) == 1) {
                // This would allow us to cache the response stream that user throws away
                // Next time the cached version could save us from an extra roundtrip
                if (closeState == CloseExState.Silent) {
                    try {
                        int total = 0;
                        int bytesRead;
                        while (total < ConnectStream.s_DrainingBuffer.Length && (bytesRead = Read(ConnectStream.s_DrainingBuffer, 0, ConnectStream.s_DrainingBuffer.Length)) > 0) {
                            total += bytesRead;
                        }
                    }
                    catch (Exception exception) {
                        //ATTN: this path will swalow errors regardless of m_IsThrowOnWriteError setting
                        //      A "Silent" close is for an intermediate response that is to be ignored anyway
                        if (exception is ThreadAbortException || exception is StackOverflowException || exception is OutOfMemoryException) {
                            throw;
                        }
                    }
                    catch {
                        //ATTN: this path will swalow errors regardless of m_IsThrowOnWriteError setting
                        //      A "Silent" close is for an intermediate response that is to be ignored anyway
                    }
                }

                Dispose(true, closeState);
                GC.SuppressFinalize(this);
            }
        }

        protected virtual void Dispose(bool disposing, CloseExState closeState) {

            // All below calls should already be idempotent

            try {
                ICloseEx icloseEx = m_OriginalStream as ICloseEx;
                if (icloseEx != null) {
                    icloseEx.CloseEx(closeState);
                }
                else {
                    m_OriginalStream.Close();
                }
            }
            finally {

                // Notify the wirte stream on a partial response if did not see EOF on read
                if (!m_SeenReadEOF)
                    closeState |= CloseExState.Abort;

                //
                // We don't want to touch m_ShadowStreamIsDead because Close() can be called from other thread while IO is in progress.
                // We assume that all streams used by this class are thread safe on Close().
                // m_ShadowStreamIsDead = true;

                if (m_ShadowStream is ICloseEx)
                    ((ICloseEx)m_ShadowStream).CloseEx(closeState);
                else
                    m_ShadowStream.Close();
            }

            if (!disposing) {
                // no IO can be in progress because it a GC thread.
                m_OriginalStream = null;
                m_ShadowStream = null;
            }
            base.Dispose(disposing);
        }

        public override bool CanTimeout {
            get {
                return m_OriginalStream.CanTimeout && m_ShadowStream.CanTimeout;
            }
        }

        public override int ReadTimeout {
            get {
                return m_OriginalStream.ReadTimeout;
            }
            set {
                m_OriginalStream.ReadTimeout = m_ShadowStream.ReadTimeout = value;
            }
        }

        public override int WriteTimeout {
            get {
                return m_ShadowStream.WriteTimeout;
            }
            set {
                m_OriginalStream.WriteTimeout = m_ShadowStream.WriteTimeout = value;
            }
        }
    }

    //
    // This stream will listen on the parent stream Close.
    // Assuming the parent stream represents a READ stream such as CombinedReadStream or a response stream.
    // When the paretn stream is closed this wrapper will update the metadata associated with the entry.
    internal class MetadataUpdateStream: Stream, ICloseEx {
        private Stream m_ParentStream;
        private RequestCache m_Cache;
        private string      m_Key;
        private DateTime    m_Expires;
        private DateTime    m_LastModified;
        private DateTime    m_LastSynchronized;
        private TimeSpan    m_MaxStale;
        private StringCollection m_EntryMetadata;
        private StringCollection m_SystemMetadata;
        private bool        m_CacheDestroy;
        private bool        m_IsStrictCacheErrors;


        internal MetadataUpdateStream(  Stream parentStream,
                                        RequestCache cache,
                                        string      key,
                                        DateTime    expiresGMT,
                                        DateTime    lastModifiedGMT,
                                        DateTime    lastSynchronizedGMT,
                                        TimeSpan    maxStale,
                                        StringCollection entryMetadata,
                                        StringCollection systemMetadata,
                                        bool        isStrictCacheErrors): base()
        {
            if (parentStream == null)
                throw new ArgumentNullException("parentStream");

            m_ParentStream      = parentStream;
            m_Cache             = cache;
            m_Key               = key;
            m_Expires           = expiresGMT;
            m_LastModified      = lastModifiedGMT;
            m_LastSynchronized  = lastSynchronizedGMT;
            m_MaxStale          = maxStale;
            m_EntryMetadata     = entryMetadata;
            m_SystemMetadata    = systemMetadata;
            m_IsStrictCacheErrors = isStrictCacheErrors;
        }

        //
        // This constructor will result in removing a cache entry upon closure
        //
        private MetadataUpdateStream( Stream parentStream, RequestCache cache, string key, bool isStrictCacheErrors):base() {
            if (parentStream == null)
                throw new ArgumentNullException("parentStream");

            m_ParentStream      = parentStream;
            m_Cache             = cache;
            m_Key               = key;
            m_CacheDestroy      = true;
            m_IsStrictCacheErrors = isStrictCacheErrors;
        }
        //
        //
        //
        /*
        //                                    
        public static Stream CreateEntryRemovalStream(  Stream parentStream, RequestCache cache, string key, bool isStrictCacheErrors)
        {
            return new MetadataUpdateStream(parentStream, cache, key, isStrictCacheErrors);
        }
        */
        //
        public override bool CanRead {
            get {return m_ParentStream.CanRead;}
        }
        //
        // If CanSeek is false, Position, Seek, Length, and SetLength should throw.
        public override bool CanSeek {
            get {return m_ParentStream.CanSeek;}
        }
        //
        public override bool CanWrite {
            get {return m_ParentStream.CanWrite;}
        }
        //
        public override long Length {
            get {return m_ParentStream.Length;}
        }
        //
        public override long Position {
            get {return m_ParentStream.Position;}

            set {m_ParentStream.Position = value;}
        }

        public override long Seek(long offset, SeekOrigin origin) {
            return m_ParentStream.Seek(offset, origin);
        }

        public override void SetLength(long value) {
            m_ParentStream.SetLength(value);
        }

        public override void Write(byte[] buffer, int offset, int count) {
            m_ParentStream.Write(buffer, offset, count);
        }

        public override IAsyncResult BeginWrite(byte[] buffer, int offset, int count, AsyncCallback callback, Object state) {
            return m_ParentStream.BeginWrite(buffer, offset, count, callback, state);
        }

        public override void EndWrite(IAsyncResult asyncResult) {
            m_ParentStream.EndWrite(asyncResult);
        }

        public override void Flush() {
            m_ParentStream.Flush();
        }

        public override int Read(byte[] buffer, int offset, int count) {
            return m_ParentStream.Read(buffer, offset,  count);
        }

        public override IAsyncResult BeginRead(byte[] buffer, int offset, int count, AsyncCallback callback, Object state) {
            return m_ParentStream.BeginRead(buffer, offset, count, callback, state);
        }

        public override int EndRead(IAsyncResult asyncResult) {
            return m_ParentStream.EndRead(asyncResult);
        }

        // Subclasses should use Dispose(bool, CloseExState)
        protected sealed override void Dispose(bool disposing) {
            Dispose(disposing, CloseExState.Normal);
            GC.SuppressFinalize(this);
        }

        void ICloseEx.CloseEx(CloseExState closeState) {
            Dispose(true, closeState);
        }

        public override bool CanTimeout {
            get {
                return m_ParentStream.CanTimeout;
            }
        }

        public override int ReadTimeout {
            get {
                return m_ParentStream.ReadTimeout;
            }
            set {
                m_ParentStream.ReadTimeout = value;
            }
        }

        public override int WriteTimeout {
            get {
                return m_ParentStream.WriteTimeout;
            }
            set {
                m_ParentStream.WriteTimeout = value;
            }
        }

        private int _Disposed;
        protected virtual void Dispose(bool disposing, CloseExState closeState) {

            if (Interlocked.Increment(ref _Disposed) == 1) {
                ICloseEx icloseEx = m_ParentStream as ICloseEx;

                if (icloseEx != null) {
                    icloseEx.CloseEx(closeState);
                }
                else {
                    m_ParentStream.Close();
                }

                if (m_CacheDestroy)
                {
                    if (m_IsStrictCacheErrors)
                    {
                        m_Cache.Remove(m_Key);
                    }
                    else
                    {
                        m_Cache.TryRemove(m_Key);
                    }
                }
                else
                {
                    if (m_IsStrictCacheErrors)
                    {
                        m_Cache.Update(m_Key, m_Expires, m_LastModified, m_LastSynchronized, m_MaxStale, m_EntryMetadata, m_SystemMetadata);
                    }
                    else
                    {
                        m_Cache.TryUpdate(m_Key, m_Expires, m_LastModified, m_LastSynchronized, m_MaxStale, m_EntryMetadata, m_SystemMetadata);
                    }

                }

                if (!disposing) {
                    m_Cache             = null;
                    m_Key               = null;
                    m_EntryMetadata     = null;
                    m_SystemMetadata    = null;
                }
            }
            base.Dispose(disposing);
        }
    }

    //
    // This stream is for Partial responses.
    // It will scroll to the given position and limit the original stream windows to given size
    internal class RangeStream: Stream, ICloseEx {
        private Stream m_ParentStream;
        long    m_Offset;
        long    m_Size;
        long    m_Position;

        internal RangeStream (Stream parentStream, long offset, long size): base() {
            m_ParentStream      = parentStream;
            m_Offset            = offset;
            m_Size              = size;
            if (m_ParentStream.CanSeek) {
                m_ParentStream.Position = offset;
                m_Position = offset;
            }
            else {
                // for now we expect a FileStream that is seekable.
                throw new NotSupportedException(SR.GetString(SR.net_cache_non_seekable_stream_not_supported));
            }
        }

        public override bool CanRead {
            get {return m_ParentStream.CanRead;}
        }

        // If CanSeek is false, Position, Seek, Length, and SetLength should throw.
        public override bool CanSeek {
            get {return m_ParentStream.CanSeek;}
        }

        public override bool CanWrite {
            get {return m_ParentStream.CanWrite;}
        }

        public override long Length {
            get {
                long dummy = m_ParentStream.Length;
                return m_Size;
            }
        }

        public override long Position {
            get {return m_ParentStream.Position-m_Offset;}

            set {
                value += m_Offset;
                if (value > m_Offset + m_Size) {
                    value = m_Offset + m_Size;
                }
                m_ParentStream.Position = value;
            }
        }

        public override long Seek(long offset, SeekOrigin origin) {
            switch (origin) {
            case SeekOrigin.Begin:
                        offset += m_Offset;
                        if (offset > m_Offset+m_Size) {
                            offset = m_Offset+m_Size;
                        }
                        if (offset < m_Offset) {
                            offset = m_Offset;
                        }
                        break;
            case SeekOrigin.End:
                        offset -= (m_Offset+m_Size);
                        if (offset > 0) {
                            offset = 0;
                        }
                        if (offset < -m_Size) {
                            offset = -m_Size;
                        }
                        break;
            default:
                        if (m_Position+offset > m_Offset+m_Size) {
                            offset = (m_Offset+m_Size) - m_Position;
                        }
                        if (m_Position+offset < m_Offset) {
                            offset = m_Offset-m_Position;
                        }
                        break;
            }
            m_Position=m_ParentStream.Seek(offset, origin);
            return m_Position-m_Offset;
        }

        public override void SetLength(long value) {
            throw new NotSupportedException(SR.GetString(SR.net_cache_unsupported_partial_stream));
        }

        public override void Write(byte[] buffer, int offset, int count) {
            if (m_Position + count > m_Offset+m_Size) {
                throw new NotSupportedException(SR.GetString(SR.net_cache_unsupported_partial_stream));
            }
            m_ParentStream.Write(buffer, offset, count);
            m_Position += count;
        }

        public override IAsyncResult BeginWrite(byte[] buffer, int offset, int count, AsyncCallback callback, Object state) {
            if (m_Position+offset > m_Offset+m_Size) {
                throw new NotSupportedException(SR.GetString(SR.net_cache_unsupported_partial_stream));
            }
            return m_ParentStream.BeginWrite(buffer, offset, count, callback, state);
        }

        public override void EndWrite(IAsyncResult asyncResult) {
            m_ParentStream.EndWrite(asyncResult);
            m_Position = m_ParentStream.Position;
        }

        public override void Flush() {
            m_ParentStream.Flush();
        }

        public override int Read(byte[] buffer, int offset, int count) {
            if (m_Position >= m_Offset+m_Size) {
                return 0;
            }
            if (m_Position + count > m_Offset+m_Size) {
                count = (int)(m_Offset + m_Size - m_Position);
            }
            int result = m_ParentStream.Read(buffer, offset,  count);
            m_Position += result;
            return result;
        }

        public override IAsyncResult BeginRead(byte[] buffer, int offset, int count, AsyncCallback callback, Object state) {
            if (m_Position >= m_Offset+m_Size) {
                count = 0;
            }
            else if (m_Position + count > m_Offset+m_Size) {
                count = (int)(m_Offset + m_Size - m_Position);
            }
            return m_ParentStream.BeginRead(buffer, offset, count, callback, state);
        }

        public override int EndRead(IAsyncResult asyncResult) {
            int result = m_ParentStream.EndRead(asyncResult);
            m_Position += result;
            return result;
        }

        // Subclasses should use Dispose(bool, CloseExState)
        protected sealed override void Dispose(bool disposing) {
            Dispose(disposing, CloseExState.Normal);
            GC.SuppressFinalize(this);
        }

        void ICloseEx.CloseEx(CloseExState closeState) {
            Dispose(true, closeState);
            GC.SuppressFinalize(this);
        }

        public override bool CanTimeout {
            get {
                return m_ParentStream.CanTimeout;
            }
        }

        public override int ReadTimeout {
            get {
                return m_ParentStream.ReadTimeout;
            }
            set {
                m_ParentStream.ReadTimeout = value;
            }
        }

        public override int WriteTimeout {
            get {
                return m_ParentStream.WriteTimeout;
            }
            set {
                m_ParentStream.WriteTimeout = value;
            }
        }

        protected virtual void Dispose(bool disposing, CloseExState closeState) {

            // All calls below should already be idempotent.

            ICloseEx icloseEx = m_ParentStream as ICloseEx;

            if (icloseEx != null) {
                icloseEx.CloseEx(closeState);
            }
            else {
                m_ParentStream.Close();
            }
            base.Dispose(disposing);
        }
    }

}
