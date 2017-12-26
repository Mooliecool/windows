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
** Class:  FileStream
**
**
** Purpose: Exposes a Stream around a file, with full 
** synchronous and asychronous support, and buffering.
**
**
===========================================================*/
using System;
using Microsoft.Win32;
using Microsoft.Win32.SafeHandles;
using System.Security;
using System.Security.Permissions;
using System.Threading;
using System.Runtime.InteropServices;
using System.Runtime.Remoting.Messaging;
using System.Runtime.CompilerServices;
using System.Globalization;
using System.Runtime.Versioning;


/*
 * FileStream supports different modes of accessing the disk - async mode
 * and sync mode.  They are two completely different codepaths in the
 * sync & async methods (ie, Read/Write vs. BeginRead/BeginWrite).  File
 * handles in NT can be opened in only sync or overlapped (async) mode,
 * and we have to deal with this pain.  Stream has implementations of
 * the sync methods in terms of the async ones, so we'll
 * call through to our base class to get those methods when necessary.
 *
 * Also buffering is added into FileStream as well. Folded in the
 * code from BufferedStream, so all the comments about it being mostly
 * aggressive (and the possible perf improvement) apply to FileStream as 
 * well.  Also added some buffering to the async code paths.
 *
 * Class Invariants:
 * The class has one buffer, shared for reading & writing.  It can only be
 * used for one or the other at any point in time - not both.  The following
 * should be true:
 *   0 <= _readPos <= _readLen < _bufferSize
 *   0 <= _writePos < _bufferSize
 *   _readPos == _readLen && _readPos > 0 implies the read buffer is valid, 
 *     but we're at the end of the buffer.
 *   _readPos == _readLen == 0 means the read buffer contains garbage.
 *   Either _writePos can be greater than 0, or _readLen & _readPos can be
 *     greater than zero, but neither can be greater than zero at the same time.
 *
 */

namespace System.IO {
    // This is an internal object implementing IAsyncResult with fields
    // for all of the relevant data necessary to complete the IO operation.
    // This is used by AsyncFSCallback and all of the async methods.
    unsafe internal sealed class FileStreamAsyncResult : IAsyncResult
    {
        // README:
        // If you modify the order of these fields, make sure to update 
        // the native VM definition of this class as well!!! 

        // User code callback
        internal AsyncCallback _userCallback;
        internal Object _userStateObject;
        internal ManualResetEvent _waitHandle;
        internal SafeFileHandle _handle;      // For cancellation support.
        internal int _EndXxxCalled;   // Whether we've called EndXxx already.
        internal int _numBytes;     // number of bytes read OR written
        internal int _errorCode;
        internal int _numBufferedBytes;

        internal bool _isWrite;     // Whether this is a read or a write
        internal bool _isComplete;  // Value for IsCompleted property        
        internal bool _completedSynchronously;


        public Object AsyncState
        {
            get { return _userStateObject; }
        }

        public bool IsCompleted
        {
            get { return _isComplete; }
        }

        public WaitHandle AsyncWaitHandle
        {
            get { return null; }
        }

        // Returns true iff the user callback was called by the thread that 
        // called BeginRead or BeginWrite.  If we use an async delegate or
        // threadpool thread internally, this will be false.  This is used
        // by code to determine whether a successive call to BeginRead needs 
        // to be done on their main thread or in their callback to avoid a
        // stack overflow on many reads or writes.
        public bool CompletedSynchronously
        {
            get { return _completedSynchronously; }
        }

        internal static FileStreamAsyncResult CreateBufferedReadResult(int numBufferedBytes, AsyncCallback userCallback, Object userStateObject) {
            FileStreamAsyncResult asyncResult = new FileStreamAsyncResult();
            asyncResult._userCallback = userCallback;
            asyncResult._userStateObject = userStateObject;
            asyncResult._isWrite = false;
            asyncResult._numBufferedBytes = numBufferedBytes;
            return asyncResult;
        }

        private void CallUserCallbackWorker(Object callbackState)
        {
            _isComplete = true;
            if (_waitHandle != null)
                _waitHandle.Set();
            _userCallback(this);
        }

        internal void CallUserCallback()
        {
            // Convenience method for me, since I have to do this in a number 
            // of places in the buffering code for fake IAsyncResults.   
            // AsyncFSCallback intentionally does not use this method.
            
            if (_userCallback != null) {
                // Call user's callback on a threadpool thread.  
                // Set completedSynchronously to false, since it's on another 
                // thread, not the main thread.
                _completedSynchronously = false;
                ThreadPool.QueueUserWorkItem(new WaitCallback(CallUserCallbackWorker));
            }
            else {
                _isComplete = true;
                if (_waitHandle != null)
                    _waitHandle.Set();
            }
        }


    }

    [ComVisible(true)]
    public class FileStream : Stream
    {
        internal const int DefaultBufferSize = 4096;


        private static readonly bool _canUseAsync = false;

        private byte[] _buffer;   // Shared read/write buffer.  Alloc on first use.
        private String _fileName; // Fully qualified file name.
        private bool _isAsync;    // Whether we opened the handle for overlapped IO
        private bool _canRead;
        private bool _canWrite;
        private bool _canSeek;
        private bool _exposedHandle; // Could other code be using this handle?
        private bool _isPipe;     // Whether to disable async buffering code.
        private int _readPos;     // Read pointer within shared buffer.
        private int _readLen;     // Number of bytes read in buffer from file.
        private int _writePos;    // Write pointer within shared buffer.
        private int _bufferSize;  // Length of internal buffer, if it's allocated.
        private SafeFileHandle _handle;
        private long _pos;        // Cache current location in the file.
        private long _appendStart;// When appending, prevent overwriting file.


        //This exists only to support IsolatedStorageFileStream.
        //Any changes to FileStream must include the corresponding changes in IsolatedStorage.
        internal FileStream() { 
            _fileName = null;
            _handle = null;
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileStream(String path, FileMode mode) 
            : this(path, mode, (mode == FileMode.Append ? FileAccess.Write : FileAccess.ReadWrite), FileShare.Read, DefaultBufferSize, FileOptions.None, Path.GetFileName(path), false) {
        }
    
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileStream(String path, FileMode mode, FileAccess access) 
            : this(path, mode, access, FileShare.Read, DefaultBufferSize, FileOptions.None, Path.GetFileName(path), false) {
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileStream(String path, FileMode mode, FileAccess access, FileShare share) 
            : this(path, mode, access, share, DefaultBufferSize, FileOptions.None, Path.GetFileName(path), false) {
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileStream(String path, FileMode mode, FileAccess access, FileShare share, int bufferSize) 
            : this(path, mode, access, share, bufferSize, FileOptions.None, Path.GetFileName(path), false)
        {
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileStream(String path, FileMode mode, FileAccess access, FileShare share, int bufferSize, FileOptions options)
            : this(path, mode, access, share, bufferSize, options, Path.GetFileName(path), false)
        {
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileStream(String path, FileMode mode, FileAccess access, FileShare share, int bufferSize, bool useAsync) 
            : this(path, mode, access, share, bufferSize, (useAsync ? FileOptions.Asynchronous : FileOptions.None), Path.GetFileName(path), false)
        {
        }


        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        internal FileStream(String path, FileMode mode, FileAccess access, FileShare share, int bufferSize, FileOptions options, String msgPath, bool bFromProxy)
        {
            Win32Native.SECURITY_ATTRIBUTES secAttrs = GetSecAttrs(share);
            Init(path, mode, access, 0, false, share, bufferSize, options, secAttrs, msgPath, bFromProxy);
        
        }

        // AccessControl namespace is not defined in Rotor
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        internal void Init(String path, FileMode mode, FileAccess access, int rights, bool useRights, FileShare share, int bufferSize, FileOptions options, Win32Native.SECURITY_ATTRIBUTES secAttrs, String msgPath, bool bFromProxy)
        {
            // msgPath must be safe to hand back to untrusted code.

            _fileName = msgPath;  // To handle odd cases of finalizing partially constructed objects.
            _exposedHandle = false;

            if (path == null)
                throw new ArgumentNullException("path", Environment.GetResourceString("ArgumentNull_Path"));
            if (path.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyPath"));

            BCLDebug.Assert(!useRights, "Specifying FileSystemRights is not supported on this platform!");
          
            // don't include inheritable in our bounds check for share
            FileShare tempshare = share & ~FileShare.Inheritable;
            String badArg = null;

            if (mode < FileMode.CreateNew || mode > FileMode.Append)
                badArg = "mode";
            else if (!useRights && (access < FileAccess.Read || access > FileAccess.ReadWrite))
                badArg = "access";
            else if (tempshare < FileShare.None || tempshare > (FileShare.ReadWrite | FileShare.Delete))
                badArg = "share";
            
            if (badArg != null)
                throw new ArgumentOutOfRangeException(badArg, Environment.GetResourceString("ArgumentOutOfRange_Enum"));
            
            // NOTE: any change to FileOptions enum needs to be matched here in the error validation
            if (options != FileOptions.None && (options & ~(FileOptions.WriteThrough | FileOptions.Asynchronous | FileOptions.RandomAccess | FileOptions.DeleteOnClose | FileOptions.SequentialScan | FileOptions.Encrypted | (FileOptions)0x20000000 /* NoBuffering */)) != 0)
                throw new ArgumentOutOfRangeException("options", Environment.GetResourceString("ArgumentOutOfRange_Enum"));

            if (bufferSize <= 0)
                throw new ArgumentOutOfRangeException("bufferSize", Environment.GetResourceString("ArgumentOutOfRange_NeedPosNum"));

            // Write access validation
            if (!useRights && (access & FileAccess.Write) == 0) { 
                if (mode==FileMode.Truncate || mode==FileMode.CreateNew || mode==FileMode.Create || mode==FileMode.Append) {
                    // No write access
                    if (!useRights)
                        throw new ArgumentException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("Argument_InvalidFileMode&AccessCombo"), mode, access));
                }
            }


            int fAccess;
            if (!useRights) {
                fAccess = access == FileAccess.Read? GENERIC_READ:
                access == FileAccess.Write? GENERIC_WRITE:
                GENERIC_READ | GENERIC_WRITE;
            }
            else {
                fAccess = rights;
            }
            
            // Get absolute path - Security needs this to prevent something
            // like trying to create a file in c:\tmp with the name 
            // "..\WinNT\System32\ntoskrnl.exe".  Store it for user convenience.
            String filePath = Path.GetFullPathInternal(path);

            _fileName = filePath;

            // Prevent access to your disk drives as raw block devices.
            if (filePath.StartsWith("\\\\.\\", StringComparison.Ordinal))
                throw new ArgumentException(Environment.GetResourceString("Arg_DevicesNotSupported"));

            // Build up security permissions required, as well as validate we
            // have a sensible set of parameters.  IE, creating a brand new file
            // for reading doesn't make much sense.
            FileIOPermissionAccess secAccess = FileIOPermissionAccess.NoAccess;
            if (!useRights && (access & FileAccess.Read) != 0) {
                if (mode==FileMode.Append)
                    throw new ArgumentException(Environment.GetResourceString("Argument_InvalidAppendMode"));
                else
                    secAccess = secAccess | FileIOPermissionAccess.Read;
            }
            // I can't think of any combos of FileMode we should disallow if we
            // don't have read access.  Writing would pretty much always be valid
            // in those cases.

            // For any FileSystemRights other than ReadAndExecute, demand Write permission
            // This is probably bit overkill for TakeOwnership etc but we don't have any 
            // matching FileIOPermissionAccess to demand. It is better that we ask for Write permission.

            if (!useRights && (access & FileAccess.Write) != 0) { 
                if (mode==FileMode.Append)
                    secAccess = secAccess | FileIOPermissionAccess.Append;
                else
                    secAccess = secAccess | FileIOPermissionAccess.Write;
            }

            new FileIOPermission(secAccess, new String[] { filePath }, false, false).Demand();
            // Our Inheritable bit was stolen from Windows, but should be set in
            // the security attributes class.  Don't leave this bit set.
            share &= ~FileShare.Inheritable;

            bool seekToEnd = (mode==FileMode.Append);
            // Must use a valid Win32 constant here...
            if (mode == FileMode.Append)
                mode = FileMode.OpenOrCreate;

            // WRT async IO, do the right thing for whatever platform we're on.
            // This way, someone can easily write code that opens a file 
            // asynchronously no matter what their platform is.  
            if (_canUseAsync && (options & FileOptions.Asynchronous) != 0)
                _isAsync = true;
            else
                options &= ~FileOptions.Asynchronous;

            int flagsAndAttributes = (int) options;
            
            // Don't pop up a dialog for reading from an emtpy floppy drive
            int oldMode = Win32Native.SetErrorMode(Win32Native.SEM_FAILCRITICALERRORS);
            try {
                _handle = Win32Native.SafeCreateFile(filePath, fAccess, share, secAttrs, mode, flagsAndAttributes, Win32Native.NULL);
				
                if (_handle.IsInvalid) {
                    // Return a meaningful exception, using the RELATIVE path to
                    // the file to avoid returning extra information to the caller
                    // unless they have path discovery permission, in which case
                    // the full path is fine & useful.
                    
                    // NT5 oddity - when trying to open "C:\" as a FileStream,
                    // we usually get ERROR_PATH_NOT_FOUND from the OS.  We should
                    // probably be consistent w/ every other directory.
                    int errorCode = Marshal.GetLastWin32Error();
                    if (errorCode==__Error.ERROR_PATH_NOT_FOUND && filePath.Equals(Directory.InternalGetDirectoryRoot(filePath)))
                        errorCode = __Error.ERROR_ACCESS_DENIED;
    
                    // We need to give an exception, and preferably it would include
                    // the fully qualified path name.  Do security check here.  If
                    // we fail, give back the msgPath, which should not reveal much.
                    // While this logic is largely duplicated in 
                    // __Error.WinIOError, we need this for 
                    // IsolatedStorageFileStream.
                    bool canGiveFullPath = false;
    
                    if (!bFromProxy)
                    {
                        try {
                            new FileIOPermission(FileIOPermissionAccess.PathDiscovery, new String[] { _fileName }, false, false ).Demand();
                            canGiveFullPath = true;
                        }
                        catch(SecurityException) {}
                    }
    
                    if (canGiveFullPath)
                        __Error.WinIOError(errorCode, _fileName);
                    else
                        __Error.WinIOError(errorCode, msgPath);
                }
            }
            finally {
                Win32Native.SetErrorMode(oldMode);
            }
                
            // Disallow access to all non-file devices from the FileStream
            // constructors that take a String.  Everyone else can call 
            // CreateFile themselves then use the constructor that takes an 
            // IntPtr.  Disallows "con:", "com1:", "lpt1:", etc.
            int fileType = Win32Native.GetFileType(_handle);
            if (fileType != Win32Native.FILE_TYPE_DISK) {
                _handle.Close();
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_FileStreamOnNonFiles"));
            }

            if (!useRights) {
                _canRead = (access & FileAccess.Read) != 0;
                _canWrite = (access & FileAccess.Write) != 0;
            }

            _canSeek = true;
            _isPipe = false;
            _pos = 0;
            _bufferSize = bufferSize;
            _readPos = 0;
            _readLen = 0;
            _writePos = 0;

            // For Append mode...
            if (seekToEnd) {
                _appendStart = SeekCore(0, SeekOrigin.End);
            }
            else {
                _appendStart = -1;
            }
        }

        [Obsolete("This constructor has been deprecated.  Please use new FileStream(SafeFileHandle handle, FileAccess access) instead.  http://go.microsoft.com/fwlink/?linkid=14202")]
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileStream(IntPtr handle, FileAccess access) 
            : this(handle, access, true, DefaultBufferSize, false) {
        }
        
        [Obsolete("This constructor has been deprecated.  Please use new FileStream(SafeFileHandle handle, FileAccess access) instead, and optionally make a new SafeFileHandle with ownsHandle=false if needed.  http://go.microsoft.com/fwlink/?linkid=14202")]
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileStream(IntPtr handle, FileAccess access, bool ownsHandle) 
            : this(handle, access, ownsHandle, DefaultBufferSize, false) {
        }

        [Obsolete("This constructor has been deprecated.  Please use new FileStream(SafeFileHandle handle, FileAccess access, int bufferSize) instead, and optionally make a new SafeFileHandle with ownsHandle=false if needed.  http://go.microsoft.com/fwlink/?linkid=14202")]
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileStream(IntPtr handle, FileAccess access, bool ownsHandle, int bufferSize)
            : this(handle, access, ownsHandle, bufferSize, false) {
        }

        // We explicitly do a Demand, not a LinkDemand here.
        [Obsolete("This constructor has been deprecated.  Please use new FileStream(SafeFileHandle handle, FileAccess access, int bufferSize, bool isAsync) instead, and optionally make a new SafeFileHandle with ownsHandle=false if needed.  http://go.microsoft.com/fwlink/?linkid=14202")]
        [SecurityPermissionAttribute(SecurityAction.Demand, Flags=SecurityPermissionFlag.UnmanagedCode)]
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileStream(IntPtr handle, FileAccess access, bool ownsHandle, int bufferSize, bool isAsync) 
            : this(new SafeFileHandle(handle, ownsHandle), access, bufferSize, isAsync) {
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileStream(SafeFileHandle handle, FileAccess access)
            : this(handle, access, DefaultBufferSize, false) {
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public FileStream(SafeFileHandle handle, FileAccess access, int bufferSize)
            : this(handle, access, bufferSize, false) {
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        [SecurityPermissionAttribute(SecurityAction.Demand, Flags=SecurityPermissionFlag.UnmanagedCode)]
        public FileStream(SafeFileHandle handle, FileAccess access, int bufferSize, bool isAsync) {
            // To ensure we don't leak a handle, put it in a SafeFileHandle first
            if (handle.IsInvalid)
                throw new ArgumentException(Environment.GetResourceString("Arg_InvalidHandle"), "handle");

            _handle = handle;
            _exposedHandle = true;

            // Now validate arguments.
            if (access < FileAccess.Read || access > FileAccess.ReadWrite)
                throw new ArgumentOutOfRangeException("access", Environment.GetResourceString("ArgumentOutOfRange_Enum"));
            if (bufferSize <= 0)
                throw new ArgumentOutOfRangeException("bufferSize", Environment.GetResourceString("ArgumentOutOfRange_NeedPosNum"));

            int handleType = Win32Native.GetFileType(_handle);
            BCLDebug.Assert(handleType == Win32Native.FILE_TYPE_DISK || handleType == Win32Native.FILE_TYPE_PIPE || handleType == Win32Native.FILE_TYPE_CHAR, "FileStream was passed an unknown file type!");
            _isAsync = isAsync && _canUseAsync;  // On Win9x, just do the right thing.
            _canRead = 0 != (access & FileAccess.Read);
            _canWrite = 0 != (access & FileAccess.Write);
            _canSeek = handleType == Win32Native.FILE_TYPE_DISK;
            _bufferSize = bufferSize;
            _readPos = 0;
            _readLen = 0;
            _writePos = 0;
            _fileName = null;
            _isPipe = handleType == Win32Native.FILE_TYPE_PIPE;

                if (handleType != Win32Native.FILE_TYPE_PIPE)
                    VerifyHandleIsSync();
            if (_canSeek)
                SeekCore(0, SeekOrigin.Current);
            else
                _pos = 0;
        }

        private static Win32Native.SECURITY_ATTRIBUTES GetSecAttrs(FileShare share)
        {
            Win32Native.SECURITY_ATTRIBUTES secAttrs = null;
            if ((share & FileShare.Inheritable) != 0) {
                secAttrs = new Win32Native.SECURITY_ATTRIBUTES();
                secAttrs.nLength = (int)Marshal.SizeOf(secAttrs);

                secAttrs.bInheritHandle = 1;
            }
            return secAttrs;
        }


        // Verifies that this handle supports synchronous IO operations (unless you
        // didn't open it for either reading or writing).
        private unsafe void VerifyHandleIsSync()
        {
            // Do NOT use this method on pipes.  Reading or writing to a pipe may
            // cause an app to block incorrectly, introducing a deadlock (depending
            // on whether a write will wake up an already-blocked thread or this
            // FileStream's thread).

            // Do NOT change this to use a byte[] of length 0, or test test won't
            // work.  Our ReadFile & WriteFile methods are special cased to return
            // for arrays of length 0, since we'd get an IndexOutOfRangeException 
            // while using C#'s fixed syntax.
            byte[] bytes = new byte[1];
            int hr = 0;
            int r = 0;
            
            // If the handle is a pipe, ReadFile will block until there
            // has been a write on the other end.  We'll just have to deal with it,
            // For the read end of a pipe, you can mess up and 
            // accidentally read synchronously from an async pipe.
            if (CanRead) {
                r = ReadFileNative(_handle, bytes, 0, 0, null, out hr);
            }
            else if (CanWrite) {
                r = WriteFileNative(_handle, bytes, 0, 0, null, out hr);
            }

            if (hr==ERROR_INVALID_PARAMETER)
                throw new ArgumentException(Environment.GetResourceString("Arg_HandleNotSync"));
            if (hr == Win32Native.ERROR_INVALID_HANDLE)
                __Error.WinIOError(hr, "<OS handle>");
        }


        public override bool CanRead {
            get { return _canRead; }
        }

        public override bool CanWrite {
            get { return _canWrite; }
        }

        public override bool CanSeek {
            get { return _canSeek; }
        }

        public virtual bool IsAsync {
            get { return _isAsync; }
        }

        public override long Length {
            get {
                if (_handle.IsClosed) __Error.FileNotOpen();
                if (!CanSeek) __Error.SeekNotSupported();
                int hi = 0, lo = 0;

                lo = Win32Native.GetFileSize(_handle, out hi);

                if (lo==-1) {  // Check for either an error or a 4GB - 1 byte file.
                    int hr = Marshal.GetLastWin32Error();
                    if (hr != 0)
                        __Error.WinIOError(hr, String.Empty);
                }
                long len = (((long)hi) << 32) | ((uint) lo);
                // If we're writing near the end of the file, we must include our
                // internal buffer in our Length calculation.  Don't flush because
                // we use the length of the file in our async write method.
                if (_writePos > 0 && _pos + _writePos > len)
                    len = _writePos + _pos;
                return len;
            }
        }

        public String Name {
            get {
                if (_fileName == null)
                    return Environment.GetResourceString("IO_UnknownFileName");
                new FileIOPermission(FileIOPermissionAccess.PathDiscovery, new String[] { _fileName }, false, false ).Demand();
                return _fileName;
            }
        }

        internal String NameInternal {
            get {
                if (_fileName == null)
                    return "<UnknownFileName>";
                return _fileName;
            }
        }

        public override long Position {
            get { 
                if (_handle.IsClosed) __Error.FileNotOpen();
                if (!CanSeek) __Error.SeekNotSupported();
                BCLDebug.Assert((_readPos==0 && _readLen==0 && _writePos >= 0) || (_writePos==0 && _readPos <= _readLen), "We're either reading or writing, but not both.");

                // Verify that internal position is in sync with the handle
                if (_exposedHandle)
                    VerifyOSHandlePosition();

                // Compensate for buffer that we read from the handle (_readLen) Vs what the user
                // read so far from the internel buffer (_readPos). Of course add any unwrittern  
                // buffered data
                return _pos + (_readPos - _readLen + _writePos);
            }
            set {
                if (value < 0) throw new ArgumentOutOfRangeException("value", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
                if (_writePos > 0) FlushWrite(false);
                _readPos = 0;
                _readLen = 0;
                Seek(value, SeekOrigin.Begin);
            }
        }



        protected override void Dispose(bool disposing)
        {
            // Nothing will be done differently based on whether we are 
            // disposing vs. finalizing.  This is taking advantage of the
            // weak ordering between normal finalizable objects & critical
            // finalizable objects, which I included in the SafeHandle 
            // design for FileStream, which would often "just work" when 
            // finalized.
            try {
                if (_handle != null && !_handle.IsClosed) {
                    // Flush data to disk iff we were writing.  After 
                    // thinking about this, we also don't need to flush
                    // our read position, regardless of whether the handle
                    // was exposed to the user.  They probably would NOT 
                    // want us to do this.
                    if (_writePos > 0) {
                        FlushWrite(!disposing);
                    }
                }
            }
            finally {
                if (_handle != null && !_handle.IsClosed)
                    _handle.Dispose();
                
                _canRead = false;
                _canWrite = false;
                _canSeek = false;
                // Don't set the buffer to null, to avoid a NullReferenceException
                // when users have a race condition in their code (ie, they call
                // Close when calling another method on Stream like Read).
                //_buffer = null;
                base.Dispose(disposing);
            }
        }

        ~FileStream()
        {
            if (_handle != null) {
                BCLDebug.Correctness(_handle.IsClosed, "You didn't close a FileStream & it got finalized.  Name: \""+_fileName+"\"");
                Dispose(false);
            }
        }

        public override void Flush()
        {
            // This code is duplicated in Dispose
            if (_handle.IsClosed) __Error.FileNotOpen();
            if (_writePos > 0) {
                FlushWrite(false);
            }
            else if (_readPos < _readLen && CanSeek) {
                FlushRead();
            }
        }

        // Reading is done by blocks from the file, but someone could read
        // 1 byte from the buffer then write.  At that point, the OS's file
        // pointer is out of sync with the stream's position.  All write 
        // functions should call this function to preserve the position in the file.
        private void FlushRead() {
            BCLDebug.Assert(_writePos == 0, "FileStream: Write buffer must be empty in FlushRead!");
            if (_readPos - _readLen != 0) {
                BCLDebug.Assert(CanSeek, "FileStream will lose buffered read data now.");
                SeekCore(_readPos - _readLen, SeekOrigin.Current);
            }
            _readPos = 0;
            _readLen = 0;
        }

        // Writes are buffered.  Anytime the buffer fills up 
        // (_writePos + delta > _bufferSize) or the buffer switches to reading
        // and there is left over data (_writePos > 0), this function must be called.
        private void FlushWrite(bool calledFromFinalizer) {
            BCLDebug.Assert(_readPos == 0 && _readLen == 0, "FileStream: Read buffer must be empty in FlushWrite!");

            WriteCore(_buffer, 0, _writePos);
            _writePos = 0;
        }


        [Obsolete("This property has been deprecated.  Please use FileStream's SafeFileHandle property instead.  http://go.microsoft.com/fwlink/?linkid=14202")]
        public virtual IntPtr Handle {
            [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.UnmanagedCode)]
            [SecurityPermissionAttribute(SecurityAction.InheritanceDemand, Flags=SecurityPermissionFlag.UnmanagedCode)]
            [ResourceExposure(ResourceScope.Machine)]
            [ResourceConsumption(ResourceScope.Machine)]
            get { 
                Flush();
                // Explicitly dump any buffered data, since the user could move our
                // position or write to the file.
                _readPos = 0;
                _readLen = 0;
                _writePos = 0;
                _exposedHandle = true;

                return _handle.DangerousGetHandle();
            }
        }

        public virtual SafeFileHandle SafeFileHandle {
            [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.UnmanagedCode)]
            [SecurityPermissionAttribute(SecurityAction.InheritanceDemand, Flags=SecurityPermissionFlag.UnmanagedCode)]
            get { 
                Flush();
                // Explicitly dump any buffered data, since the user could move our
                // position or write to the file.
                _readPos = 0;
                _readLen = 0;
                _writePos = 0;
                _exposedHandle = true;

                return _handle;
            }
        }

        public override void SetLength(long value)
        {
            if (value < 0)
                throw new ArgumentOutOfRangeException("value", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
            if (_handle.IsClosed) __Error.FileNotOpen();
            if (!CanSeek) __Error.SeekNotSupported();
            if (!CanWrite) __Error.WriteNotSupported();
            // Handle buffering updates.
            if (_writePos > 0) {
                FlushWrite(false);
            }
            else if (_readPos < _readLen) {
                FlushRead();
            }
            if (_appendStart != -1 && value < _appendStart)
                throw new IOException(Environment.GetResourceString("IO.IO_SetLengthAppendTruncate"));
            SetLengthCore(value);
        }

        private void SetLengthCore(long value)
        {
            BCLDebug.Assert(value >= 0, "value >= 0");
            long origPos = _pos;

            if (_exposedHandle)
                VerifyOSHandlePosition();
            if (_pos != value)
                SeekCore(value, SeekOrigin.Begin);
            if (!Win32Native.SetEndOfFile(_handle)) {
                int hr = Marshal.GetLastWin32Error();
                if (hr==__Error.ERROR_INVALID_PARAMETER)
                    throw new ArgumentOutOfRangeException("value", Environment.GetResourceString("ArgumentOutOfRange_FileLengthTooBig"));
                __Error.WinIOError(hr, String.Empty);
            }
            // Return file pointer to where it was before setting length
            if (origPos != value) {
                if (origPos < value)
                    SeekCore(origPos, SeekOrigin.Begin);
                else
                    SeekCore(0, SeekOrigin.End);
            }
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
            
            if (_handle.IsClosed) __Error.FileNotOpen();
            
            BCLDebug.Assert((_readPos==0 && _readLen==0 && _writePos >= 0) || (_writePos==0 && _readPos <= _readLen), "We're either reading or writing, but not both.");

            bool isBlocked = false;
            int n = _readLen - _readPos;
            // if the read buffer is empty, read into either user's array or our
            // buffer, depending on number of bytes user asked for and buffer size.
            if (n == 0) {
                if (!CanRead) __Error.ReadNotSupported();
                if (_writePos > 0) FlushWrite(false);
                if (!CanSeek || (count >= _bufferSize)) {
                    n = ReadCore(array, offset, count);
                    // Throw away read buffer.
                    _readPos = 0;
                    _readLen = 0;
                    return n;
                }
                if (_buffer == null) _buffer = new byte[_bufferSize];
                n = ReadCore(_buffer, 0, _bufferSize);
                if (n == 0) return 0;
                isBlocked = n < _bufferSize;
                _readPos = 0;
                _readLen = n;
            }
            // Now copy min of count or numBytesAvailable (ie, near EOF) to array.
            if (n > count) n = count;
            Buffer.InternalBlockCopy(_buffer, _readPos, array, offset, n);
            _readPos += n;


            if (!_isPipe) {
                // If we hit the end of the buffer and didn't have enough bytes, we must
                // read some more from the underlying stream.  However, if we got
                // fewer bytes from the underlying stream than we asked for (ie, we're 
                // probably blocked), don't ask for more bytes.
                if (n < count && !isBlocked) {
                    BCLDebug.Assert(_readPos == _readLen, "Read buffer should be empty!");                
                    int moreBytesRead = ReadCore(array, offset + n, count - n);
                    n += moreBytesRead;
                    // We've just made our buffer inconsistent with our position 
                    // pointer.  We must throw away the read buffer.
                    _readPos = 0;
                    _readLen = 0;
                }
            }

            return n;
        }

        private unsafe int ReadCore(byte[] buffer, int offset, int count) {
            BCLDebug.Assert(!_handle.IsClosed, "!_handle.IsClosed");
            BCLDebug.Assert(CanRead, "CanRead");

            BCLDebug.Assert(buffer != null, "buffer != null");
            BCLDebug.Assert(_writePos == 0, "_writePos == 0");
            BCLDebug.Assert(offset >= 0, "offset is negative");
            BCLDebug.Assert(count >= 0, "count is negative");

            // Make sure we are reading from the right spot
            if (_exposedHandle)
                VerifyOSHandlePosition();

            int hr = 0;
            int r = ReadFileNative(_handle, buffer, offset, count, null, out hr);
            if (r == -1) {
                if (hr == ERROR_BROKEN_PIPE) {
                    r = 0;
                }
                else {
                    if (hr == ERROR_INVALID_PARAMETER)
                        throw new ArgumentException(Environment.GetResourceString("Arg_HandleNotSync"));
                    
                    __Error.WinIOError(hr, String.Empty);
                }
            }
            BCLDebug.Assert(r >= 0, "FileStream's ReadCore is likely broken.");
            _pos += r;

            return r;
        }

        public override long Seek(long offset, SeekOrigin origin) {
            if (origin<SeekOrigin.Begin || origin>SeekOrigin.End)
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidSeekOrigin"));
            if (_handle.IsClosed) __Error.FileNotOpen();
            if (!CanSeek) __Error.SeekNotSupported();

            BCLDebug.Assert((_readPos==0 && _readLen==0 && _writePos >= 0) || (_writePos==0 && _readPos <= _readLen), "We're either reading or writing, but not both.");

            // If we've got bytes in our buffer to write, write them out.
            // If we've read in and consumed some bytes, we'll have to adjust
            // our seek positions ONLY IF we're seeking relative to the current
            // position in the stream.  This simulates doing a seek to the new
            // position, then a read for the number of bytes we have in our buffer.
            if (_writePos > 0) {
                FlushWrite(false);
            }
            else if (origin == SeekOrigin.Current) {
                // Don't call FlushRead here, which would have caused an infinite
                // loop.  Simply adjust the seek origin.  This isn't necessary
                // if we're seeking relative to the beginning or end of the stream.
                offset -= (_readLen - _readPos);
            }

            // Verify that internal position is in sync with the handle
            if (_exposedHandle)
                VerifyOSHandlePosition();

            long oldPos = _pos + (_readPos - _readLen);
            long pos = SeekCore(offset, origin);

            // Prevent users from overwriting data in a file that was opened in
            // append mode.
            if (_appendStart != -1 && pos < _appendStart) {
                SeekCore(oldPos, SeekOrigin.Begin);
                throw new IOException(Environment.GetResourceString("IO.IO_SeekAppendOverwrite"));
            }

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
                        SeekCore(_readLen, SeekOrigin.Current);
                }
                else if (oldPos - _readPos < pos && pos < oldPos + _readLen - _readPos) {
                    int diff = (int)(pos - oldPos);
                    //Console.WriteLine("Seek: diff was "+diff+", readpos was "+_readPos+"  adjusting buffer - shrinking by "+ (_readPos + diff));
                    Buffer.InternalBlockCopy(_buffer, _readPos+diff, _buffer, 0, _readLen - (_readPos + diff));
                    _readLen -= (_readPos + diff);
                    _readPos = 0;
                    if (_readLen > 0)
                        SeekCore(_readLen, SeekOrigin.Current);
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

        // This doesn't do argument checking.  Necessary for SetLength, which must
        // set the file pointer beyond the end of the file. This will update the 
        // internal position
        private long SeekCore(long offset, SeekOrigin origin) {
            BCLDebug.Assert(!_handle.IsClosed && CanSeek, "!_handle.IsClosed && CanSeek");
            BCLDebug.Assert(origin>=SeekOrigin.Begin && origin<=SeekOrigin.End, "origin>=SeekOrigin.Begin && origin<=SeekOrigin.End");
            int hr = 0;
            long ret = 0;
            
            ret = Win32Native.SetFilePointer(_handle, offset, origin, out hr);
            if (ret == -1) {
                // For invalid handles, detect the error and mark our handle
                // as closed to give slightly better error messages.  Also
                // help ensure we avoid handle recycling bugs.
                if (hr == Win32Native.ERROR_INVALID_HANDLE)
                    _handle.SetHandleAsInvalid();
                __Error.WinIOError(hr, String.Empty);
            }
            
            _pos = ret;
            return ret;
        }

        // Checks the position of the OS's handle equals what we expect it to.
        // This will fail if someone else moved the FileStream's handle or if
        // we've hit a bug in FileStream's position updating code.
        private void VerifyOSHandlePosition()
        {
            if (!CanSeek)
                return;

            // SeekCore will override the current _pos, so save it now
            long oldPos = _pos;
            long curPos = SeekCore(0, SeekOrigin.Current);
            
            if (curPos != oldPos) {
                // For reads, this is non-fatal but we still could have returned corrupted 
                // data in some cases. So discard the internal buffer. Potential MDA 
                _readPos = 0;  
                _readLen = 0;
                if(_writePos > 0) {
                    // Discard the buffer and let the user know!
                    _writePos = 0;
                    throw new IOException(Environment.GetResourceString("IO.IO_FileStreamHandlePosition"));
                }
            }
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

            if (_handle.IsClosed) __Error.FileNotOpen();
            if (_writePos==0) {
                // Ensure we can write to the stream, and ready buffer for writing.
                if (!CanWrite) __Error.WriteNotSupported();
                if (_readPos < _readLen) FlushRead();
                _readPos = 0;
                _readLen = 0;
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
                WriteCore(_buffer, 0, _writePos);
                _writePos = 0;
            }
            // If the buffer would slow writes down, avoid buffer completely.
            if (count >= _bufferSize) {
                BCLDebug.Assert(_writePos == 0, "FileStream cannot have buffered data to write here!  Your stream will be corrupted.");
                WriteCore(array, offset, count);
                return;
            }
            else if (count == 0)
                return;  // Don't allocate a buffer then call memcpy for 0 bytes.
            if (_buffer==null) _buffer = new byte[_bufferSize];
            // Copy remaining bytes into buffer, to write at a later date.
            Buffer.InternalBlockCopy(array, offset, _buffer, _writePos, count);
            _writePos = count;
            return;
        }

        private unsafe void WriteCore(byte[] buffer, int offset, int count) {
            BCLDebug.Assert(!_handle.IsClosed, "!_handle.IsClosed");
            BCLDebug.Assert(CanWrite, "CanWrite");

            BCLDebug.Assert(buffer != null, "buffer != null");
            BCLDebug.Assert(_readPos == _readLen, "_readPos == _readLen");
            BCLDebug.Assert(offset >= 0, "offset is negative");
            BCLDebug.Assert(count >= 0, "count is negative");
            
            // Make sure we are writing to the position that we think we are
            if (_exposedHandle)
                VerifyOSHandlePosition();
            
            int hr = 0;
            int r = WriteFileNative(_handle, buffer, offset, count, null, out hr);
            if (r == -1) {
                // For pipes, ERROR_NO_DATA is not an error, but the pipe is closing.
                if (hr == ERROR_NO_DATA) {
                    r = 0;
                }
                else {
                    // ERROR_INVALID_PARAMETER may be returned for writes
                    // where the position is too large (ie, writing at Int64.MaxValue 
                    // on Win9x) OR for synchronous writes to a handle opened 
                    // asynchronously.
                    if (hr == ERROR_INVALID_PARAMETER)
                        throw new IOException(Environment.GetResourceString("IO.IO_FileTooLongOrHandleNotSync"));
                    __Error.WinIOError(hr, String.Empty);
                }
            }
            BCLDebug.Assert(r >= 0, "FileStream's WriteCore is likely broken.");
            _pos += r;
            return;
        }

        [HostProtection(ExternalThreading=true)]
        public override IAsyncResult BeginRead(byte[] array, int offset, int numBytes, AsyncCallback userCallback, Object stateObject)
        {
            if (array==null)
                throw new ArgumentNullException("array");
            if (offset < 0)
                throw new ArgumentOutOfRangeException("offset", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
            if (numBytes < 0)
                throw new ArgumentOutOfRangeException("numBytes", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
            if (array.Length - offset < numBytes)
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidOffLen"));

            if (_handle.IsClosed) __Error.FileNotOpen();
            return base.BeginRead(array, offset, numBytes, userCallback, stateObject);
        }


        public unsafe override int EndRead(IAsyncResult asyncResult)
        {
            // There are 3 significantly different IAsyncResults we'll accept
            // here.  One is from Stream::BeginRead.  The other two are variations
            // on our FileStreamAsyncResult.  One is from BeginReadCore,
            // while the other is from the BeginRead buffering wrapper.
            if (asyncResult==null)
                throw new ArgumentNullException("asyncResult");

            return base.EndRead(asyncResult);
        }

        // Reads a byte from the file stream.  Returns the byte cast to an int
        // or -1 if reading from the end of the stream.
        public override int ReadByte() {
            if (_handle.IsClosed) __Error.FileNotOpen();
            if (_readLen==0 && !CanRead) __Error.ReadNotSupported();
            BCLDebug.Assert((_readPos==0 && _readLen==0 && _writePos >= 0) || (_writePos==0 && _readPos <= _readLen), "We're either reading or writing, but not both.");
            if (_readPos == _readLen) {
                if (_writePos > 0) FlushWrite(false);
                BCLDebug.Assert(_bufferSize > 0, "_bufferSize > 0");
                if (_buffer == null) _buffer = new byte[_bufferSize];
                _readLen = ReadCore(_buffer, 0, _bufferSize);
                _readPos = 0;
            }
            if (_readPos == _readLen)
                return -1;

            int result = _buffer[_readPos];
            _readPos++;
            return result;
        }


        [HostProtection(ExternalThreading=true)]
        public override IAsyncResult BeginWrite(byte[] array, int offset, int numBytes, AsyncCallback userCallback, Object stateObject)
        {
            if (array==null)
                throw new ArgumentNullException("array");
            if (offset < 0)
                throw new ArgumentOutOfRangeException("offset", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
            if (numBytes < 0)
                throw new ArgumentOutOfRangeException("numBytes", Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));
            if (array.Length - offset < numBytes)
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidOffLen"));

            if (_handle.IsClosed) __Error.FileNotOpen();


           return base.BeginWrite(array, offset, numBytes, userCallback, stateObject);
        }


        public unsafe override void EndWrite(IAsyncResult asyncResult)
        {
            if (asyncResult==null)
                throw new ArgumentNullException("asyncResult");

            base.EndWrite(asyncResult);
        }

        public override void WriteByte(byte value)
        {
            if (_handle.IsClosed) __Error.FileNotOpen();
            if (_writePos==0) {
                if (!CanWrite) __Error.WriteNotSupported();
                if (_readPos < _readLen) FlushRead();
                _readPos = 0;
                _readLen = 0;
                BCLDebug.Assert(_bufferSize > 0, "_bufferSize > 0");
                if (_buffer==null) _buffer = new byte[_bufferSize];
            }
            if (_writePos == _bufferSize)
                FlushWrite(false);

            _buffer[_writePos] = value;
            _writePos++;
        }

        public virtual void Lock(long position, long length) {
            if (_handle.IsClosed) __Error.FileNotOpen();
            if (position < 0 || length < 0)
                throw new ArgumentOutOfRangeException((position < 0 ? "position" : "length"), Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));

            int positionLow     = unchecked((int)(position      ));
            int positionHigh    = unchecked((int)(position >> 32));
            int lengthLow       = unchecked((int)(length        ));
            int lengthHigh      = unchecked((int)(length   >> 32));
            
            if (!Win32Native.LockFile(_handle, positionLow, positionHigh, lengthLow, lengthHigh))
                __Error.WinIOError();
        }

        public virtual void Unlock(long position, long length) {
            if (_handle.IsClosed) __Error.FileNotOpen();
            if (position < 0 || length < 0)
                throw new ArgumentOutOfRangeException((position < 0 ? "position" : "length"), Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum"));

            int positionLow     = unchecked((int)(position      ));
            int positionHigh    = unchecked((int)(position >> 32));
            int lengthLow       = unchecked((int)(length        ));
            int lengthHigh      = unchecked((int)(length   >> 32));

            if (!Win32Native.UnlockFile(_handle, positionLow, positionHigh, lengthLow, lengthHigh))
                __Error.WinIOError();
        }

        // Windows API definitions, from winbase.h and others
        
        private const int FILE_ATTRIBUTE_NORMAL = 0x00000080;
        private const int FILE_ATTRIBUTE_ENCRYPTED = 0x00004000;
        private const int FILE_FLAG_OVERLAPPED = 0x40000000;
        internal const int GENERIC_READ = unchecked((int)0x80000000);
        private const int GENERIC_WRITE = 0x40000000;
    
        private const int FILE_BEGIN = 0;
        private const int FILE_CURRENT = 1;
        private const int FILE_END = 2;

        // Error codes (not HRESULTS), from winerror.h
        private const int ERROR_BROKEN_PIPE = 109;
        private const int ERROR_NO_DATA = 232;
        private const int ERROR_HANDLE_EOF = 38;
        private const int ERROR_INVALID_PARAMETER = 87;
        private const int ERROR_IO_PENDING = 997;


        // __ConsoleStream also uses this code. 
        private unsafe int ReadFileNative(SafeFileHandle handle, byte[] bytes, int offset, int count, NativeOverlapped* overlapped, out int hr)
        {
            BCLDebug.Assert(handle != null, "handle != null");
            BCLDebug.Assert(offset >= 0, "offset >= 0");
            BCLDebug.Assert(count >= 0, "count >= 0");
            BCLDebug.Assert(bytes != null, "bytes != null");

            BCLDebug.Assert((_isAsync && overlapped != null) || (!_isAsync && overlapped == null), "Async IO parameter screwup in call to ReadFileNative.");

            // Don't corrupt memory when multiple threads are erroneously writing
            // to this stream simultaneously.
            if (bytes.Length - offset < count)
                throw new IndexOutOfRangeException(Environment.GetResourceString("IndexOutOfRange_IORaceCondition"));

            // You can't use the fixed statement on an array of length 0.
            if (bytes.Length==0) {
                hr = 0;
                return 0;
            }

            int r = 0;
            int numBytesRead = 0;

            fixed(byte* p = bytes) {
                if (_isAsync)
                    r = Win32Native.ReadFile(handle, p + offset, count, IntPtr.Zero, overlapped);
                else
                    r = Win32Native.ReadFile(handle, p + offset, count, out numBytesRead, IntPtr.Zero);
            }

            if (r==0) {
                hr = Marshal.GetLastWin32Error();
                // We should never silently swallow an error here without some
                // extra work.  We must make sure that BeginReadCore won't return an 
                // IAsyncResult that will cause EndRead to block, since the OS won't
                // call AsyncFSCallback for us.  
                if (hr == ERROR_BROKEN_PIPE || hr == Win32Native.ERROR_PIPE_NOT_CONNECTED) {
                    // This handle was a pipe, and it's done. Not an error, but EOF.
                    // However, the OS will not call AsyncFSCallback!
                    // Let the caller handle this, since BeginReadCore & ReadCore 
                    // need to do different things.
                    return -1;
                }

                // For invalid handles, detect the error and mark our handle
                // as closed to give slightly better error messages.  Also
                // help ensure we avoid handle recycling bugs.
                if (hr == Win32Native.ERROR_INVALID_HANDLE)
                    _handle.SetHandleAsInvalid();

                return -1;
            }
            else
                hr = 0;
            return numBytesRead;
        }

        private unsafe int WriteFileNative(SafeFileHandle handle, byte[] bytes, int offset, int count, NativeOverlapped* overlapped, out int hr) {
            BCLDebug.Assert(handle != null, "handle != null");
            BCLDebug.Assert(offset >= 0, "offset >= 0");
            BCLDebug.Assert(count >= 0, "count >= 0");
            BCLDebug.Assert(bytes != null, "bytes != null");

            BCLDebug.Assert((_isAsync && overlapped != null) || (!_isAsync && overlapped == null), "Async IO parameter screwup in call to WriteFileNative.");

            // Don't corrupt memory when multiple threads are erroneously writing
            // to this stream simultaneously.  (the OS is reading from
            // the array we pass to WriteFile, but if we read beyond the end and
            // that memory isn't allocated, we could get an AV.)
            if (bytes.Length - offset < count)
                throw new IndexOutOfRangeException(Environment.GetResourceString("IndexOutOfRange_IORaceCondition"));

            // You can't use the fixed statement on an array of length 0.
            if (bytes.Length==0) {
                hr = 0;
                return 0;
            }

            int numBytesWritten = 0;
            int r = 0;
            
            fixed(byte* p = bytes) {
                if (_isAsync)
                    r = Win32Native.WriteFile(handle, p + offset, count, IntPtr.Zero, overlapped);
                else
                    r = Win32Native.WriteFile(handle, p + offset, count, out numBytesWritten, IntPtr.Zero);
            }

            if (r==0) {
                hr = Marshal.GetLastWin32Error();
                // We should never silently swallow an error here without some
                // extra work.  We must make sure that BeginWriteCore won't return an 
                // IAsyncResult that will cause EndWrite to block, since the OS won't
                // call AsyncFSCallback for us.  

                if (hr==ERROR_NO_DATA) {
                    // This handle was a pipe, and the pipe is being closed on the 
                    // other side.  Let the caller handle this, since BeginWriteCore 
                    // & WriteCore need to do different things.
                    return -1;
                }
                
                // For invalid handles, detect the error and mark our handle
                // as closed to give slightly better error messages.  Also
                // help ensure we avoid handle recycling bugs.
                if (hr == Win32Native.ERROR_INVALID_HANDLE)
                    _handle.SetHandleAsInvalid();

                return -1;
            }
            else
                hr = 0;
            return numBytesWritten;          
        }
    }
}
