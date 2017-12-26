//------------------------------------------------------------------------------
// <copyright file="GZipStream.cs" company="Microsoft">
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
    using System.Security.Permissions;

    public class GZipStream : Stream {
        private DeflateStream deflateStream;
        
        public GZipStream(Stream stream, CompressionMode mode) : this( stream, mode, false) {
        }

        public GZipStream(Stream stream, CompressionMode mode, bool leaveOpen) {
            deflateStream = new DeflateStream(stream, mode, leaveOpen, true);
        }

        public override bool CanRead { 
            get {
                if( deflateStream == null) {
                    return false;
                }
                
                return deflateStream.CanRead;
            }
        }

        public override bool CanWrite { 
            get {
                if( deflateStream == null) {
                    return false;
                }
                
                return deflateStream.CanWrite;
            }
        }

        public override bool CanSeek { 
            get {
                if( deflateStream == null) {
                   return false;
                }
                
                return deflateStream.CanSeek;
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
            if( deflateStream == null) {
                throw new ObjectDisposedException(null, SR.GetString(SR.ObjectDisposed_StreamClosed));
            }
            deflateStream.Flush();
            return;
        }

        public override long Seek(long offset, SeekOrigin origin) {
            throw new NotSupportedException(SR.GetString(SR.NotSupported));            
        }

        public override void SetLength(long value) {
            throw new NotSupportedException(SR.GetString(SR.NotSupported));            
        }

        [HostProtection(ExternalThreading=true)]
        public override IAsyncResult BeginRead(byte[] array, int offset, int count, AsyncCallback asyncCallback, object asyncState) {
            if( deflateStream == null) {
                throw new InvalidOperationException(SR.GetString(SR.ObjectDisposed_StreamClosed));
            }
            return deflateStream.BeginRead(array, offset, count, asyncCallback, asyncState);
        }

        public override int EndRead(IAsyncResult asyncResult) {
            if( deflateStream == null) {
                throw new InvalidOperationException(SR.GetString(SR.ObjectDisposed_StreamClosed));
            }
            return deflateStream.EndRead(asyncResult);
        }

        [HostProtection(ExternalThreading=true)]
        public override IAsyncResult BeginWrite(byte[] array, int offset, int count, AsyncCallback asyncCallback, object asyncState) {
            if( deflateStream == null) {
                throw new InvalidOperationException(SR.GetString(SR.ObjectDisposed_StreamClosed));
            }
            return deflateStream.BeginWrite(array, offset, count, asyncCallback, asyncState);
        }

        public override void EndWrite(IAsyncResult asyncResult) {
            if( deflateStream == null) {
                throw new InvalidOperationException(SR.GetString(SR.ObjectDisposed_StreamClosed));
            }
            deflateStream.EndWrite(asyncResult);
        }
        public override int Read(byte[] array, int offset, int count) {
            if( deflateStream == null) {
                throw new ObjectDisposedException(null, SR.GetString(SR.ObjectDisposed_StreamClosed));
            }

            return deflateStream.Read(array, offset, count);
        }

        public override void Write(byte[] array, int offset, int count) {
            if( deflateStream == null) {
                throw new ObjectDisposedException(null, SR.GetString(SR.ObjectDisposed_StreamClosed));
            }

            deflateStream.Write(array, offset, count);
        }

        protected override void Dispose(bool disposing) {
            try {
                if (disposing && deflateStream != null) { 
                    deflateStream.Close();
                }
                deflateStream = null;
            }
            finally {
                base.Dispose(disposing);
            }
        }

        public Stream BaseStream { 
            get {
                if( deflateStream != null) {
                    return deflateStream.BaseStream;
                }
                else {
                    return null;
                }
            }
        }
    }
    
}


