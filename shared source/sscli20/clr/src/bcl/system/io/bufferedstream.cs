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
** Class:  BufferedStream
**
** Purpose: A composable Stream that buffers reads & writes 
** to the underlying stream.
**
**
===========================================================*/
using System;
using System.Runtime.InteropServices;
using System.Globalization;

namespace System.IO {
    //
    //
    //
    [ComVisible(true)]
    public sealed class BufferedStream : Stream {
        private Stream _s;         // Underlying stream.  Close sets _s to null.
        private byte[] _buffer;    // Shared read/write buffer.  Alloc on first use.
        private int _readPos;      // Read pointer within shared buffer.
        private int _readLen;      // Number of bytes read in buffer from _s.
        private int _writePos;     // Write pointer within shared buffer.
        private int _bufferSize;   // Length of internal buffer, if it's allocated.


        private const int _DefaultBufferSize = 4096;

        private BufferedStream() {}

        public BufferedStream(Stream stream) : this(stream, _DefaultBufferSize)
        {
        }

        public BufferedStream(Stream stream, int bufferSize)
        {
            if (stream==null)
                throw new ArgumentNullException("stream");
            if (bufferSize <= 0)
                throw new ArgumentOutOfRangeException("bufferSize", String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("ArgumentOutOfRange_MustBePositive"), "bufferSize"));
            BCLDebug.Perf(!(stream is FileStream), "FileStream is buffered - don't wrap it in a BufferedStream");
            BCLDebug.Perf(!(stream is MemoryStream), "MemoryStream shouldn't be wrapped in a BufferedStream!");

            _s = stream;
            _bufferSize = bufferSize;
            // Allocate _buffer on its first use - it will not be used if all reads
            // & writes are greater than or equal to buffer size.
            if (!_s.CanRead && !_s.CanWrite) __Error.StreamIsClosed();
        }

        public override bool CanRead {
            get { return _s != null && _s.CanRead; }
        }

        public override bool CanWrite {
            get { return _s != null && _s.CanWrite; }
        }

        public override bool CanSeek {
            get { return _s != null && _s.CanSeek; }
        }

        public override long Length {
            get {
                if (_s==null) __Error.StreamIsClosed();
                if (_writePos > 0) FlushWrite();
                return _s.Length;
            }
        }

        public override long Position {
            get {
                if (_s==null) __Error.StreamIsClosed();
                if (!_s.CanSeek) __Error.SeekNotSupported();
                //              return _s.Seek(0, SeekOrigin.Current) + (_readPos + _writePos - _readLen);
                return _s.Position + (_readPos - _readLen + _writePos);
            }
            set {
                if (value < 0) throw new ArgumentOutOfRangeException("value", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
                if (_s==null) __Error.StreamIsClosed();
                if (!_s.CanSeek) __Error.SeekNotSupported();
                if (_writePos > 0) FlushWrite();
                _readPos = 0;
                _readLen = 0;
                _s.Seek(value, SeekOrigin.Begin);
            }
        }

        protected override void Dispose(bool disposing)
        {
            try {
                if (disposing && _s != null) {
                    try {
                        Flush();
                    }
                    finally {
                        _s.Close();
                    }
                }
            }
            finally {
                _s = null;
                _buffer = null;

                // Call base.Dispose(bool) to cleanup async IO resources
                base.Dispose(disposing);
            }
        }

        public override void Flush() {
            if (_s==null) __Error.StreamIsClosed();
            if (_writePos > 0) {
                FlushWrite();
            }
            else if (_readPos < _readLen && _s.CanSeek) {
                FlushRead();
            }
        }

        // Reading is done by blocks from the file, but someone could read
        // 1 byte from the buffer then write.  At that point, the OS's file
        // pointer is out of sync with the stream's position.  All write 
        // functions should call this function to preserve the position in the file.
        private void FlushRead() {
            BCLDebug.Assert(_writePos == 0, "BufferedStream: Write buffer must be empty in FlushRead!");
            if (_readPos - _readLen != 0)
                _s.Seek(_readPos - _readLen, SeekOrigin.Current);
            _readPos = 0;
            _readLen = 0;
        }
    
        // Writes are buffered.  Anytime the buffer fills up 
        // (_writePos + delta > _bufferSize) or the buffer switches to reading
        // and there is dirty data (_writePos > 0), this function must be called.
        private void FlushWrite() {
            BCLDebug.Assert(_readPos == 0 && _readLen == 0, "BufferedStream: Read buffer must be empty in FlushWrite!");
            _s.Write(_buffer, 0, _writePos);
            _writePos = 0;
            _s.Flush();
        }

        public override int Read([In, Out] byte[] array, int offset, int count) {
            if (array==null)
                throw new ArgumentNullException("array", Environment.GetResourceString("ArgumentNull_Buffer"));
            if (offset < 0)
                throw new ArgumentOutOfRangeException("offset", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
            if (count < 0)
                throw new ArgumentOutOfRangeException("count", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
            if (array.Length - offset < count)
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidOffLen"));
            
            if (_s==null) __Error.StreamIsClosed();

            int n = _readLen - _readPos;
            // if the read buffer is empty, read into either user's array or our
            // buffer, depending on number of bytes user asked for and buffer size.
            if (n == 0) {
                if (!_s.CanRead) __Error.ReadNotSupported();
                if (_writePos > 0) FlushWrite();
                if (count >= _bufferSize) {
                    n = _s.Read(array, offset, count);
                    // Throw away read buffer.
                    _readPos = 0;
                    _readLen = 0;
                    return n;
                }
                if (_buffer == null) _buffer = new byte[_bufferSize];
                n = _s.Read(_buffer, 0, _bufferSize);
                if (n == 0) return 0;
                _readPos = 0;
                _readLen = n;
            }
            // Now copy min of count or numBytesAvailable (ie, near EOF) to array.
            if (n > count) n = count;
            Buffer.InternalBlockCopy(_buffer, _readPos, array, offset, n);
            _readPos += n;


            if (n < count) {
                int moreBytesRead = _s.Read(array, offset + n, count - n);
                n += moreBytesRead;
                _readPos = 0;
                _readLen = 0;
            }

            return n;
        }

        // Reads a byte from the underlying stream.  Returns the byte cast to an int
        // or -1 if reading from the end of the stream.
        public override int ReadByte() {
            if (_s==null) __Error.StreamIsClosed();
            if (_readLen==0 && !_s.CanRead) __Error.ReadNotSupported();
            if (_readPos == _readLen) {
                if (_writePos > 0) FlushWrite();
                if (_buffer == null) _buffer = new byte[_bufferSize];
                _readLen = _s.Read(_buffer, 0, _bufferSize);
                _readPos = 0;
            }
            if (_readPos == _readLen) return -1;

            return _buffer[_readPos++];
        }

        public override void Write(byte[] array, int offset, int count) {
            if (array==null)
                throw new ArgumentNullException("array", Environment.GetResourceString("ArgumentNull_Buffer"));
            if (offset < 0)
                throw new ArgumentOutOfRangeException("offset", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
            if (count < 0)
                throw new ArgumentOutOfRangeException("count", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
            if (array.Length - offset < count)
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidOffLen"));

            if (_s==null) __Error.StreamIsClosed();
            if (_writePos==0) {
                // Ensure we can write to the stream, and ready buffer for writing.
                if (!_s.CanWrite) __Error.WriteNotSupported();
                if (_readPos < _readLen)
                    FlushRead();
                else {
                    _readPos = 0;
                    _readLen = 0;
                }
            }

            // If our buffer has data in it, copy data from the user's array into
            // the buffer, and if we can fit it all there, return.  Otherwise, write
            // the buffer to disk and copy any remaining data into our buffer.
            // The assumption here is memcpy is cheaper than disk (or net) IO.
            // (10 milliseconds to disk vs. ~20-30 microseconds for a 4K memcpy)
            // So the extra copying will reduce the total number of writes, in 
            // non-pathological cases (ie, write 1 byte, then write for the buffer 
            // size repeatedly)
            if (_writePos > 0) {
                int numBytes = _bufferSize - _writePos;   // space left in buffer
                if (numBytes > 0) {
                    if (numBytes > count)
                        numBytes = count;
                    Buffer.InternalBlockCopy(array, offset, _buffer, _writePos, numBytes);
                    _writePos += numBytes;
                    if (count==numBytes) return;
                    offset += numBytes;
                    count -= numBytes;
                }
                // Reset our buffer.  We essentially want to call FlushWrite
                // without calling Flush on the underlying Stream.
                _s.Write(_buffer, 0, _writePos);
                _writePos = 0;
            }
            // If the buffer would slow writes down, avoid buffer completely.
            if (count >= _bufferSize) {
                BCLDebug.Assert(_writePos == 0, "BufferedStream cannot have buffered data to write here!  Your stream will be corrupted.");
                _s.Write(array, offset, count);
                return;
            } 
            else if (count == 0)
				return;  // Don't allocate a buffer then call memcpy for 0 bytes.
            if (_buffer==null) _buffer = new byte[_bufferSize];
            // Copy remaining bytes into buffer, to write at a later date.
            Buffer.InternalBlockCopy(array, offset, _buffer, 0, count);
            _writePos = count;
        }

        public override void WriteByte(byte value) {
            if (_s==null) __Error.StreamIsClosed();
            if (_writePos==0) {
                if (!_s.CanWrite) __Error.WriteNotSupported();
                if (_readPos < _readLen) 
                    FlushRead();
                else {
                    _readPos = 0;
                    _readLen = 0;
                }
                if (_buffer==null) _buffer = new byte[_bufferSize];
            }
            if (_writePos == _bufferSize)
                FlushWrite();

            _buffer[_writePos++] = value;
        }


        public override long Seek(long offset, SeekOrigin origin)
        {
            if (_s==null) __Error.StreamIsClosed();
            if (!_s.CanSeek) __Error.SeekNotSupported();
            // If we've got bytes in our buffer to write, write them out.
            // If we've read in and consumed some bytes, we'll have to adjust
            // our seek positions ONLY IF we're seeking relative to the current
            // position in the stream.
            BCLDebug.Assert(_readPos <= _readLen, "_readPos <= _readLen");
            if (_writePos > 0) {
                FlushWrite();
            }
            else if (origin == SeekOrigin.Current) {
                // Don't call FlushRead here, which would have caused an infinite
                // loop.  Simply adjust the seek origin.  This isn't necessary
                // if we're seeking relative to the beginning or end of the stream.
                BCLDebug.Assert(_readLen - _readPos >= 0, "_readLen ("+_readLen+") - _readPos ("+_readPos+") >= 0");
                offset -= (_readLen - _readPos);
            }
            /*
            _readPos = 0;
            _readLen = 0;
            return _s.Seek(offset, origin);
            */
            long oldPos = _s.Position + (_readPos - _readLen);
            long pos = _s.Seek(offset, origin);

            // We now must update the read buffer.  We can in some cases simply
            // update _readPos within the buffer, copy around the buffer so our 
            // Position property is still correct, and avoid having to do more 
            // reads from the disk.  Otherwise, discard the buffer's contents.
            if (_readLen > 0) {
                // We can optimize the following condition:
                // oldPos - _readPos <= pos < oldPos + _readLen - _readPos
                if (oldPos == pos) {
                    if (_readPos > 0) {
                        //Console.WriteLine("Seek: seeked for 0, adjusting buffer back by: "+_readPos+"  _readLen: "+_readLen);
                        Buffer.InternalBlockCopy(_buffer, _readPos, _buffer, 0, _readLen - _readPos);
                        _readLen -= _readPos;
                        _readPos = 0;
                    }
                    // If we still have buffered data, we must update the stream's 
                    // position so our Position property is correct.
                    if (_readLen > 0)
                        _s.Seek(_readLen, SeekOrigin.Current);
                }
                else if (oldPos - _readPos < pos && pos < oldPos + _readLen - _readPos) {
                    int diff = (int)(pos - oldPos);
                    //Console.WriteLine("Seek: diff was "+diff+", readpos was "+_readPos+"  adjusting buffer - shrinking by "+ (_readPos + diff));
                    Buffer.InternalBlockCopy(_buffer, _readPos+diff, _buffer, 0, _readLen - (_readPos + diff));
                    _readLen -= (_readPos + diff);
                    _readPos = 0;
                    if (_readLen > 0)
                        _s.Seek(_readLen, SeekOrigin.Current);
                }
                else {
                    // Lose the read buffer.
                    _readPos = 0;
                    _readLen = 0;
                }
                BCLDebug.Assert(_readLen >= 0 && _readPos <= _readLen, "_readLen should be nonnegative, and _readPos should be less than or equal _readLen");
                BCLDebug.Assert(pos == Position, "Seek optimization: pos != Position!  Buffer math was mangled.");
            }
            return pos;
        }

        public override void SetLength(long value) {
            if (value < 0) throw new ArgumentOutOfRangeException("value", Environment.GetResourceString("ArgumentOutOfRange_NegFileSize"));
            if (_s==null) __Error.StreamIsClosed();
            if (!_s.CanSeek) __Error.SeekNotSupported();
            if (!_s.CanWrite) __Error.WriteNotSupported();
            if (_writePos > 0) {
                FlushWrite();
            }
            else if (_readPos < _readLen) {
                FlushRead();
            }
            _s.SetLength(value);
        }
    }
}
