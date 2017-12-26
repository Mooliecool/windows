// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
/*============================================================
**
** Class:  UnmanagedMemoryStreamWrapper
**
** Purpose: Create a Memorystream over an UnmanagedMemoryStream
**
===========================================================*/

using System;
using System.Runtime.InteropServices;
using System.Security.Permissions;

namespace System.IO {
    // Needed for backwards compatibility with V1.x usages of the
    // ResourceManager, where a MemoryStream is now returned as an 
    // UnmanagedMemoryStream from ResourceReader.
    internal sealed class UnmanagedMemoryStreamWrapper : MemoryStream {
        private UnmanagedMemoryStream _unmanagedStream;
        
        internal UnmanagedMemoryStreamWrapper(UnmanagedMemoryStream stream) {
            _unmanagedStream = stream;
        }
        
        public override bool CanRead {
            get { return _unmanagedStream.CanRead; }
        }
        
        public override bool CanSeek {
            get { return _unmanagedStream.CanSeek; }
        }
        
        public override bool CanWrite {
            get { return _unmanagedStream.CanWrite; }
        }
        
        protected override void Dispose(bool disposing)
        {
            try {
                if (disposing)
                    _unmanagedStream.Close();
            }
            finally {
                base.Dispose(disposing);
            }
        }
        
        public override void Flush() {
            _unmanagedStream.Flush();
        }
    
        public override byte[] GetBuffer() {
            throw new UnauthorizedAccessException(Environment.GetResourceString("UnauthorizedAccess_MemStreamBuffer"));
        }
        
        public override int Capacity {
            get { 
                return (int) _unmanagedStream.Capacity;
            }
            set {
                throw new IOException(Environment.GetResourceString("IO.IO_FixedCapacity"));
            }
        }        
        
        public override long Length {
            get {
                return _unmanagedStream.Length;
            }
        }

        public override long Position {
            get { 
                return _unmanagedStream.Position;
            }
            set {
                _unmanagedStream.Position = value;
            }
        }
        
        public override int Read([In, Out] byte[] buffer, int offset, int count) {
            return _unmanagedStream.Read(buffer, offset, count);
        }
    
        public override int ReadByte() {
            return _unmanagedStream.ReadByte();
        }
        
    
        public override long Seek(long offset, SeekOrigin loc) {
            return _unmanagedStream.Seek(offset, loc);
        }

        public unsafe override byte[] ToArray() {
            if (!_unmanagedStream._isOpen) __Error.StreamIsClosed();
            if (!_unmanagedStream.CanRead) __Error.ReadNotSupported();

            byte[] buffer = new byte[_unmanagedStream.Length];
            Buffer.memcpy(_unmanagedStream.Pointer, 0, buffer, 0, (int) _unmanagedStream.Length);
            return buffer;
        }
    
        public override void Write(byte[] buffer, int offset, int count) {
            _unmanagedStream.Write(buffer, offset, count);
        }
    
        public override void WriteByte(byte value) {
            _unmanagedStream.WriteByte(value);
        }
    
        // Writes this MemoryStream to another stream.
        public unsafe override void WriteTo(Stream stream) {
            if (!_unmanagedStream._isOpen) __Error.StreamIsClosed();
            if (!_unmanagedStream.CanRead) __Error.ReadNotSupported();
            if (stream==null)
                throw new ArgumentNullException("stream", Environment.GetResourceString("ArgumentNull_Stream"));

            byte[] buffer = ToArray();
            
            stream.Write(buffer, 0, buffer.Length);
        }
    }
}


