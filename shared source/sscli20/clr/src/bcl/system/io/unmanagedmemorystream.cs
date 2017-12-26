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
** Class:  UnmanagedMemoryStream
**
** Purpose: Create a stream over unmanaged memory, mostly
**          useful for memory-mapped files.
**
** Date:  October 20, 2000 (made public August 4, 2003)
**
===========================================================*/
using System;
using System.Runtime.InteropServices;
using System.Security.Permissions;

namespace System.IO {

    /*
     * This class is used to access a contiguous block of memory, likely outside 
     * the GC heap (or pinned in place in the GC heap, but a MemoryStream may 
     * make more sense in those cases).  It's great if you have a pointer and
     * a length for a section of memory mapped in by someone else and you don't
     * want to copy this into the GC heap.  UnmanagedMemoryStream assumes these 
     * two things:
     *
     * 1) All the memory in the specified block is readable or writable,
     *    depending on the values you pass to the constructor.
     * 2) The lifetime of the block of memory is at least as long as the lifetime
     *    of the UnmanagedMemoryStream.
     * 3) You clean up the memory when appropriate.  The UnmanagedMemoryStream 
     *    currently will do NOTHING to free this memory.
     * 4) All calls to Write and WriteByte may not be threadsafe currently.
     *
     * It may become necessary to add in some sort of 
     * DeallocationMode enum, specifying whether we unmap a section of memory, 
     * call free, run a user-provided delegate to free the memory, etc etc.  
     * We'll suggest user write a subclass of UnmanagedMemoryStream that uses
     * a SafeHandle subclass to hold onto the memory.
     * Check for problems when using this in the negative parts of a 
     * process's address space.  We may need to use unsigned longs internally
     * and change the overflow detection logic.
     */
    [CLSCompliant(false)]
    public class UnmanagedMemoryStream : Stream
    {
        private const long UnmanagedMemStreamMaxLength = Int64.MaxValue;

        private unsafe byte* _mem;
        private long _length;
        private long _capacity;
        private long _position;
        private FileAccess _access;
        internal bool _isOpen;

        // Needed for subclasses that need to map a file, etc.
        protected unsafe UnmanagedMemoryStream()
        {
            _mem = null;
            _isOpen = false;
        }

        public unsafe UnmanagedMemoryStream(byte* pointer, long length)
        {
            Initialize(pointer, length, length, FileAccess.Read, false);
        }

        public unsafe UnmanagedMemoryStream(byte* pointer, long length, long capacity, FileAccess access) 
        {
            Initialize(pointer, length, capacity, access, false);
        }

        // We must create one of these without doing a security check.  This
        // class is created while security is trying to start up.  Plus, doing
        // a Demand from Assembly.GetManifestResourceStream isn't useful.
        internal unsafe UnmanagedMemoryStream(byte* pointer, long length, long capacity, FileAccess access, bool skipSecurityCheck) 
        {
            Initialize(pointer, length, capacity, access, skipSecurityCheck);
        }

        protected unsafe void Initialize(byte* pointer, long length, long capacity, FileAccess access) 
        {
            Initialize(pointer, length, capacity, access, false);
        }

        internal unsafe void Initialize(byte* pointer, long length, long capacity, FileAccess access, bool skipSecurityCheck) 
        {
            if (pointer == null)
                throw new ArgumentNullException("pointer");
            if (length < 0 || capacity < 0)
                throw new ArgumentOutOfRangeException((length < 0) ? "length" : "capacity", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
            if (length > capacity)
                throw new ArgumentOutOfRangeException("length", Environment.GetResourceString("ArgumentOutOfRange_LengthGreaterThanCapacity"));
            // Check for wraparound.
            if (((byte*) ((long)pointer + capacity)) < pointer)
                throw new ArgumentOutOfRangeException("capacity", Environment.GetResourceString("ArgumentOutOfRange_UnmanagedMemStreamWrapAround"));
            if (access < FileAccess.Read || access > FileAccess.ReadWrite)
                throw new ArgumentOutOfRangeException("access", Environment.GetResourceString("ArgumentOutOfRange_Enum"));
            if (_isOpen)
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_CalledTwice"));

            if (!skipSecurityCheck)
                new SecurityPermission(SecurityPermissionFlag.UnmanagedCode).Demand();

            _mem = pointer;
            _length = length;
            _capacity = capacity;
            _access = access;
            _isOpen = true;
        }

        public override bool CanRead {
            get { return _isOpen && (_access & FileAccess.Read) != 0; }
        }

        public override bool CanSeek {
            get { return _isOpen; }
        }

        public override bool CanWrite {
            get { return _isOpen && (_access & FileAccess.Write) != 0; }
        }

        protected override void Dispose(bool disposing)
        {
            _isOpen = false;
            //_mem = null;

            // Stream allocates WaitHandles for async calls. So for correctness 
            // call base.Dispose(disposing) for better perf, avoiding waiting
            // for the finalizers to run on those types.
            base.Dispose(disposing);
        }

        public override void Flush() {
            if (!_isOpen) __Error.StreamIsClosed();
        }

        public override long Length {
            get {
                if (!_isOpen) __Error.StreamIsClosed();
                return _length;
            }
        }

        public long Capacity {
            get {
                if (!_isOpen) __Error.StreamIsClosed();
                return _capacity;
            }
        }

        public unsafe override long Position {
            get { 
                if (!_isOpen) __Error.StreamIsClosed();
                return _position;
            }
            set {
                if (!_isOpen) __Error.StreamIsClosed();
                if (value < 0)
                    throw new ArgumentOutOfRangeException("value", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
                
#if WIN32
                // On 32 bit machines, ensure we don't wrap around.
                if (value > (long) Int32.MaxValue || _mem + value < _mem)
                    throw new ArgumentOutOfRangeException("value", Environment.GetResourceString("ArgumentOutOfRange_MemStreamLength"));
#endif
                _position = value;
            }
        }

        public unsafe byte* PositionPointer {
            [SecurityPermission(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.UnmanagedCode)]
            get {
                long pos = _position;  // Use a temp to avoid a race
                if (pos > _capacity)
                    throw new IndexOutOfRangeException(Environment.GetResourceString("IndexOutOfRange_UMSPosition"));
                byte * ptr = _mem + pos;
                if (!_isOpen) __Error.StreamIsClosed();
                return ptr;
            }
            [SecurityPermission(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.UnmanagedCode)]
            set {
                if (!_isOpen) __Error.StreamIsClosed();
                // Note: subtracting pointers returns an Int64.  Working around
                // to avoid hitting compiler warning CS0652 on this line. 
                if (new IntPtr(value - _mem).ToInt64() > UnmanagedMemStreamMaxLength)
                    throw new ArgumentOutOfRangeException("offset", Environment.GetResourceString("ArgumentOutOfRange_UnmanagedMemStreamLength"));
                if (value < _mem)
                    throw new IOException(Environment.GetResourceString("IO.IO_SeekBeforeBegin"));
                _position = value - _mem;
            }
        }

        internal unsafe byte* Pointer {
            get {
                return _mem;
            }
        }
        
        public override unsafe int Read([In, Out] byte[] buffer, int offset, int count) {
            if (!_isOpen) __Error.StreamIsClosed();
            if ((_access & FileAccess.Read) == 0) __Error.ReadNotSupported();
            if (buffer==null)
                throw new ArgumentNullException("buffer", Environment.GetResourceString("ArgumentNull_Buffer"));
            if (offset < 0)
                throw new ArgumentOutOfRangeException("offset", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
            if (count < 0)
                throw new ArgumentOutOfRangeException("count", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
            if (buffer.Length - offset < count)
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidOffLen"));

            // Use a local variable to avoid a race where another thread 
            // changes our position after we decide we can read some bytes.
            long pos = _position;
            long n = _length - pos;
            if (n > count)
                n = count;
            if (n <= 0)
                return 0;

            int nInt = (int) n; // Safe because n <= count, which is an Int32
            if (nInt < 0)
                nInt = 0;  // _position could be beyond EOF
            BCLDebug.Assert(pos + nInt >= 0, "_position + n >= 0");  // len is less than 2^63 -1.

            Buffer.memcpy(_mem + pos, 0, buffer, offset, nInt);
            _position = pos + n;
            return nInt;
        }

        public override unsafe int ReadByte() {
            if (!_isOpen) __Error.StreamIsClosed();
            if ((_access & FileAccess.Read) == 0) __Error.ReadNotSupported();

            long pos = _position;  // Use a local to avoid a race condition
            if (pos >= _length)
                return -1;
            _position = pos + 1;
            return _mem[pos];
        }

        public override unsafe long Seek(long offset, SeekOrigin loc) {
            if (!_isOpen) __Error.StreamIsClosed();
            if (offset > UnmanagedMemStreamMaxLength)
                throw new ArgumentOutOfRangeException("offset", Environment.GetResourceString("ArgumentOutOfRange_UnmanagedMemStreamLength"));
            switch(loc) {
            case SeekOrigin.Begin:
                if (offset < 0)
                    throw new IOException(Environment.GetResourceString("IO.IO_SeekBeforeBegin"));
                _position = offset;
                break;
                
            case SeekOrigin.Current:
                if (offset + _position < 0)
                    throw new IOException(Environment.GetResourceString("IO.IO_SeekBeforeBegin"));
                _position += offset;
                break;
                
            case SeekOrigin.End:
                if (_length + offset < 0)
                    throw new IOException(Environment.GetResourceString("IO.IO_SeekBeforeBegin"));
                _position = _length + offset;
                break;
                
            default:
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidSeekOrigin"));
            }

            BCLDebug.Assert(_position >= 0, "_position >= 0");
            return _position;
        }

        public unsafe override void SetLength(long value) {
            if (!_isOpen) __Error.StreamIsClosed();
            if ((_access & FileAccess.Write) == 0) __Error.WriteNotSupported();
            if (value < 0)
                throw new ArgumentOutOfRangeException("length", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
            if (value > _capacity)
                throw new IOException(Environment.GetResourceString("IO.IO_FixedCapacity"));

            long len = _length;
            if (value > len)
                Buffer.ZeroMemory(_mem+len, value-len);
            _length = value;
            if (_position > value) _position = value;
        }

        public override unsafe void Write(byte[] buffer, int offset, int count) {
            if (!_isOpen) __Error.StreamIsClosed();
            if ((_access & FileAccess.Write) == 0) __Error.WriteNotSupported();
            if (buffer==null)
                throw new ArgumentNullException("buffer", Environment.GetResourceString("ArgumentNull_Buffer"));
            if (offset < 0)
                throw new ArgumentOutOfRangeException("offset", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
            if (count < 0)
                throw new ArgumentOutOfRangeException("count", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
            if (buffer.Length - offset < count)
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidOffLen"));

            long pos = _position;  // Use a local to avoid a race condition
            long len = _length;
            long n = pos + count;
            // Check for overflow
            if (n < 0)
                throw new IOException(Environment.GetResourceString("IO.IO_StreamTooLong"));

            if (n > len) {
                if (n > _capacity)
                    throw new NotSupportedException(Environment.GetResourceString("IO.IO_FixedCapacity"));
                _length = n;
            }
            // Check to see whether we are now expanding the stream and must 
            // zero any memory in the middle.
            if (pos > len)
                Buffer.ZeroMemory(_mem+len, pos-len);

            Buffer.memcpy(buffer, offset, _mem + pos, 0, count);
            _position = n;
            return;
        }

        public override unsafe void WriteByte(byte value) {
            if (!_isOpen) __Error.StreamIsClosed();
            if ((_access & FileAccess.Write) == 0) __Error.WriteNotSupported();

            long pos = _position;  // Use a local to avoid a race condition
            long len = _length;
            long n = pos + 1;
            if (pos >= len) {
                // Check for overflow
                if (n < 0)
                    throw new IOException(Environment.GetResourceString("IO.IO_StreamTooLong"));
                
                if (n > _capacity)
                    throw new NotSupportedException(Environment.GetResourceString("IO.IO_FixedCapacity"));
                _length = n;

                // Check to see whether we are now expanding the stream and must 
                // zero any memory in the middle.
                if (pos > len) 
                    Buffer.ZeroMemory(_mem+len, pos-len);
            }

            _mem[pos] = value;
            _position = n;
        }
    }
}
