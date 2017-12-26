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
** Class:  ConsoleStream
**
**
** Purpose: Exposes a separate Stream for Console IO and 
** handles WinCE appropriately.  Also keeps us from using the
** ThreadPool for all Console output.
**
**
===========================================================*/

using System;
using System.Runtime.InteropServices;
using System.Security;
using Microsoft.Win32;
using Microsoft.Win32.SafeHandles;
using System.Runtime.CompilerServices;
using System.Runtime.Versioning;

namespace System.IO {

    internal sealed class __ConsoleStream : Stream
    {
        internal const int DefaultBufferSize = 128;

        // From winerror.h
        private const int ERROR_BROKEN_PIPE = 109;
        // ERROR_NO_DATA ("The pipe is being closed") is returned when we write to
        // a console that is closing.
        private const int ERROR_NO_DATA = 232; 

        private SafeFileHandle _handle;
        private bool _canRead;
        private bool _canWrite;

        [ResourceExposure(ResourceScope.Process)]
        internal __ConsoleStream(SafeFileHandle handle, FileAccess access)
        {
            BCLDebug.Assert(handle != null && !handle.IsInvalid, "__ConsoleStream expects a valid handle!");
            _handle = handle;
            _canRead = access == FileAccess.Read;
            _canWrite = access == FileAccess.Write;
        }
    
        public override bool CanRead {
            get { return _canRead; }
        }

        public override bool CanWrite {
            get { return _canWrite; }
        }

        public override bool CanSeek {
            get { return false; }
        }

        public override long Length {
            get {
                __Error.SeekNotSupported();
                return 0; // compiler appeasement
            }
        }

        public override long Position {
            get { 
                __Error.SeekNotSupported();
                return 0; // compiler appeasement
            }
            set {
                __Error.SeekNotSupported();
            }
        }

        protected override void Dispose(bool disposing)
        {
            // We're probably better off not closing the OS handle here.  First,
            // we allow a program to get multiple instances of __ConsoleStreams
            // around the same OS handle, so closing one handle would invalidate
            // them all.  Additionally, we want a second AppDomain to be able to 
            // write to stdout if a second AppDomain quits.
            if (_handle != null) {
                _handle = null;
            }
            _canRead = false;
            _canWrite = false;
            base.Dispose(disposing);
        }

        public override void Flush()
        {
            if (_handle == null) __Error.FileNotOpen();
            if (!CanWrite) __Error.WriteNotSupported();
        }

        public override void SetLength(long value)
        {
            __Error.SeekNotSupported();
        }

        [ResourceExposure(ResourceScope.None)]
        [ResourceConsumption(ResourceScope.Process, ResourceScope.Process)]
        public override int Read([In, Out] byte[] buffer, int offset, int count) {
            if (buffer==null)
                throw new ArgumentNullException("buffer");
            if (offset < 0 || count < 0)
                throw new ArgumentOutOfRangeException((offset < 0 ? "offset" : "count"), Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
            if (buffer.Length - offset < count)
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidOffLen"));
            if (!_canRead) __Error.ReadNotSupported();

            int errorCode = 0;
            int result = ReadFileNative(_handle, buffer, offset, count, 0, out errorCode);
            if (result == -1) {
                __Error.WinIOError(errorCode, String.Empty);
            }
            return result;
        }

        public override long Seek(long offset, SeekOrigin origin) {
            __Error.SeekNotSupported();
            return 0; // compiler appeasement
        }

        [ResourceExposure(ResourceScope.None)]
        [ResourceConsumption(ResourceScope.Process, ResourceScope.Process)]
        public override void Write(byte[] buffer, int offset, int count) {
            if (buffer==null)
                throw new ArgumentNullException("buffer");
            if (offset < 0 || count < 0)
                throw new ArgumentOutOfRangeException((offset < 0 ? "offset" : "count"), Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
            if (buffer.Length - offset < count)
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidOffLen"));
            if (!_canWrite) __Error.WriteNotSupported();

            int errorCode = 0;
            int result = WriteFileNative(_handle, buffer, offset, count, 0, out errorCode);
            if (result == -1) {
                //BCLDebug.ConsoleError("__ConsoleStream::Write: throwing on error.  Error code: "+errorCode+"  0x"+errorCode.ToString("x")+"  handle: "+_handle.ToString());
                __Error.WinIOError(errorCode, String.Empty);
            }
            return;
        }

        // P/Invoke wrappers for writing to and from a file, nearly identical
        // to the ones on FileStream.  These are duplicated to save startup/hello
        // world working set.
        [ResourceExposure(ResourceScope.Process)]
        [ResourceConsumption(ResourceScope.Process)]
        private unsafe static int ReadFileNative(SafeFileHandle hFile, byte[] bytes, int offset, int count, int mustBeZero, out int errorCode)
        {
            BCLDebug.Assert(offset >= 0, "offset >= 0");
            BCLDebug.Assert(count >= 0, "count >= 0");
            BCLDebug.Assert(bytes != null, "bytes != null");

            // Don't corrupt memory when multiple threads are erroneously writing
            // to this stream simultaneously.
            if (bytes.Length - offset < count)
                throw new IndexOutOfRangeException(Environment.GetResourceString("IndexOutOfRange_IORaceCondition"));

            // You can't use the fixed statement on an array of length 0.
            if (bytes.Length==0) {
                errorCode = 0;
                return 0;
            }

            int r;
            int numBytesRead;
            fixed(byte* p = bytes) {
                r = ReadFile(hFile, p + offset, count, out numBytesRead, Win32Native.NULL);
            }
            if (r==0) {
                errorCode = Marshal.GetLastWin32Error();
                if (errorCode == ERROR_BROKEN_PIPE) {
                    // A pipe into stdin was closed.  Not an error, but EOF.
                    return 0;
                }
                return -1;
            }
            else
                errorCode = 0;
            return numBytesRead;
        }

        [ResourceExposure(ResourceScope.Process)]
        [ResourceConsumption(ResourceScope.Process)]
        private static unsafe int WriteFileNative(SafeFileHandle hFile, byte[] bytes, int offset, int count, int mustBeZero, out int errorCode) {
            BCLDebug.Assert(offset >= 0, "offset >= 0");
            BCLDebug.Assert(count >= 0, "count >= 0");
            BCLDebug.Assert(bytes != null, "bytes != null");
            BCLDebug.Assert(bytes.Length >= offset + count, "bytes.Length >= offset + count");

            // You can't use the fixed statement on an array of length 0.
            if (bytes.Length==0) {
                errorCode = 0;
                return 0;
            }

            int numBytesWritten = 0;
            int r;
            fixed(byte* p = bytes) {
                r = WriteFile(hFile, p + offset, count, out numBytesWritten, Win32Native.NULL);
            }
            if (r==0) {
                errorCode = Marshal.GetLastWin32Error();
                if (errorCode==ERROR_NO_DATA || errorCode==ERROR_BROKEN_PIPE)
                    return 0;
                return -1;
            }
            else {
                errorCode = 0;
            }
            return numBytesWritten;          
        }

        // The P/Invoke declarations for ReadFile and WriteFile are here for a reason!  This prevents us from loading several classes
        // in the trivial hello world case. 
        [DllImport(Win32Native.KERNEL32, SetLastError=true), SuppressUnmanagedCodeSecurityAttribute]
        [ResourceExposure(ResourceScope.Process)]
        unsafe private static extern int ReadFile(SafeFileHandle handle, byte* bytes, int numBytesToRead, out int numBytesRead, IntPtr mustBeZero);
        
        [DllImport(Win32Native.KERNEL32, SetLastError=true), SuppressUnmanagedCodeSecurityAttribute]
        [ResourceExposure(ResourceScope.Process)]
        internal static unsafe extern int WriteFile(SafeFileHandle handle, byte* bytes, int numBytesToWrite, out int numBytesWritten, IntPtr mustBeZero);
    }
}
