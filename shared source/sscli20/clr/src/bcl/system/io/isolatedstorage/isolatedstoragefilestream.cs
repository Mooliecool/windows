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
 *
 * Class:  IsolatedStorageFileStream
 *
 *
 * Purpose: Provides access to files using the same interface as FileStream
 *
 * Date:  Feb 18, 2000
 *
 ===========================================================*/
namespace System.IO.IsolatedStorage {
    using System;
    using System.IO;
    using Microsoft.Win32;
    using Microsoft.Win32.SafeHandles;
    using System.Security;
    using System.Security.Permissions;
    using System.Threading;
    using System.Runtime.InteropServices;
    using System.Runtime.Versioning;

[System.Runtime.InteropServices.ComVisible(true)]
    public class IsolatedStorageFileStream : FileStream
    {
        private const int    s_BlockSize = 1024;    // Should be a power of 2!
                                                    // see usage before 
                                                    // changing this constant
        // s_BackSlash is initialized in the contructor with Path.DirectorySeparatorChar
        private readonly String s_BackSlash;

        private FileStream m_fs;
        private IsolatedStorageFile m_isf;
        private String m_GivenPath;
        private String m_FullPath;
        private bool   m_OwnedStore;

        private IsolatedStorageFileStream() {}

        [ResourceExposure(ResourceScope.AppDomain | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly, ResourceScope.AppDomain | ResourceScope.Assembly)]
        public IsolatedStorageFileStream(String path, FileMode mode) 
            : this(path, mode, (mode == FileMode.Append ? FileAccess.Write : FileAccess.ReadWrite), FileShare.None, null) {
        }
    
        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly)]
        public IsolatedStorageFileStream(String path, FileMode mode,
                IsolatedStorageFile isf) 
            : this(path, mode, FileAccess.ReadWrite, FileShare.None, isf) {
        }
    
        [ResourceExposure(ResourceScope.AppDomain | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly, ResourceScope.AppDomain | ResourceScope.Assembly)]
        public IsolatedStorageFileStream(String path, FileMode mode, 
                FileAccess access) 
            : this(path, mode, access, access == FileAccess.Read?
                FileShare.Read: FileShare.None, DefaultBufferSize, null) {
        }

        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly)]
        public IsolatedStorageFileStream(String path, FileMode mode, 
                FileAccess access, IsolatedStorageFile isf) 
            : this(path, mode, access, access == FileAccess.Read?
                FileShare.Read: FileShare.None, DefaultBufferSize, isf) {
        }

        [ResourceExposure(ResourceScope.AppDomain | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly, ResourceScope.AppDomain | ResourceScope.Assembly)]
        public IsolatedStorageFileStream(String path, FileMode mode, 
                FileAccess access, FileShare share) 
            : this(path, mode, access, share, DefaultBufferSize, null) {
        }

        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly)]
        public IsolatedStorageFileStream(String path, FileMode mode, 
                FileAccess access, FileShare share, IsolatedStorageFile isf) 
            : this(path, mode, access, share, DefaultBufferSize, isf) {
        }

        [ResourceExposure(ResourceScope.AppDomain | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly, ResourceScope.AppDomain | ResourceScope.Assembly)]
        public IsolatedStorageFileStream(String path, FileMode mode, 
                FileAccess access, FileShare share, int bufferSize) 
            : this(path, mode, access, share, bufferSize, null) {
        }

        // If the isolated storage file is null, then we default to using a file 
        // that is scoped by user, appdomain, and assembly.
        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly)]
        public IsolatedStorageFileStream(String path, FileMode mode, 
            FileAccess access, FileShare share, int bufferSize,  
            IsolatedStorageFile isf) 
        {
            if (path == null)
                throw new ArgumentNullException("path");

            if (s_BackSlash == null)
                s_BackSlash = new String(System.IO.Path.DirectorySeparatorChar,1);

            if ((path.Length == 0) || path.Equals(s_BackSlash))
                throw new ArgumentException(
                    Environment.GetResourceString(
                        "IsolatedStorage_path"));

            ulong oldFileSize=0, newFileSize;
            bool fNewFile = false, fLock=false;
            FileInfo fOld;
 
            if (isf == null)
            {
                m_OwnedStore = true;
                isf = IsolatedStorageFile.GetUserStoreForDomain();
            }

            m_isf = isf;

            FileIOPermission fiop = 
                new FileIOPermission(FileIOPermissionAccess.AllAccess,
                    m_isf.RootDirectory);

            fiop.Assert();
            fiop.PermitOnly();

            m_GivenPath = path;
            m_FullPath  = m_isf.GetFullPath(m_GivenPath);

            try { // for finally Unlocking locked store

                // Cache the old file size if the file size could change
                // Also find if we are going to create a new file.
    
                switch (mode) {
                case FileMode.CreateNew:        // Assume new file
                    fNewFile = true;
                    break;
    
                case FileMode.Create:           // Check for New file & Unreserve
                case FileMode.OpenOrCreate:     // Check for new file
                case FileMode.Truncate:         // Unreserve old file size
                case FileMode.Append:           // Check for new file
    
                    m_isf.Lock();               // oldFileSize needs to be 
                                                // protected
                    fLock = true;               // Lock succeded

                    try {
                        fOld = new FileInfo(m_FullPath);
                        oldFileSize = IsolatedStorageFile.RoundToBlockSize((ulong)fOld.Length);
                    } catch (FileNotFoundException) {
                            fNewFile = true;
                    } catch {
                    }
    
                    break;
    
                case FileMode.Open:             // Open existing, else exception
                    break;
    
                default:
                    throw new ArgumentException(
                        Environment.GetResourceString(
                            "IsolatedStorage_FileOpenMode"));
                }
    
                if (fNewFile)
                    m_isf.ReserveOneBlock();
    
                try {
                    m_fs = new 
                        FileStream(m_FullPath, mode, access, share, bufferSize, 
                            FileOptions.None, m_GivenPath, true);
                } catch {
    
                    if (fNewFile)
                        m_isf.UnreserveOneBlock();
    
                    throw;
                }
    
                // make adjustment to the Reserve / Unreserve state
    
                if ((fNewFile == false) &&
                    ((mode == FileMode.Truncate) || (mode == FileMode.Create)))
                {
                    newFileSize = IsolatedStorageFile.RoundToBlockSize((ulong)m_fs.Length);
        
                    if (oldFileSize > newFileSize)
                        m_isf.Unreserve(oldFileSize - newFileSize);
                    else if (newFileSize > oldFileSize)     // Can this happen ?
                        m_isf.Reserve(newFileSize - oldFileSize);
                }

            } finally {
                if (fLock)
                    m_isf.Unlock();
            }
            CodeAccessPermission.RevertAll();
            
        }


        public override bool CanRead {
            get { return m_fs.CanRead; }
        }

        public override bool CanWrite {
            get { return m_fs.CanWrite; }
        }

        public override bool CanSeek {
            get { return m_fs.CanSeek; }
        }

        public override bool IsAsync {
            get { return m_fs.IsAsync; }
        }

        public override long Length {
            get { return m_fs.Length; }
        }

        public override long Position {

            get { return m_fs.Position; }

            set 
            { 
                if (value < 0) 
                {
                    throw new ArgumentOutOfRangeException("value", 
                        Environment.GetResourceString(
                            "ArgumentOutOfRange_NeedNonNegNum"));
                }

                Seek(value, SeekOrigin.Begin);
            }
        }


        protected override void Dispose(bool disposing)
        {
            if (disposing) {
                if (m_fs != null)
                    m_fs.Close();
                if (m_OwnedStore && m_isf != null)
                    m_isf.Close();
            }
            base.Dispose(disposing);
        }

        public override void Flush() {
            m_fs.Flush();
        }

        [Obsolete("This property has been deprecated.  Please use IsolatedStorageFileStream's SafeFileHandle property instead.  http://go.microsoft.com/fwlink/?linkid=14202")]
        public override IntPtr Handle {
            [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.UnmanagedCode)]
            [SecurityPermissionAttribute(SecurityAction.InheritanceDemand, Flags=SecurityPermissionFlag.UnmanagedCode)]
            get {
                NotPermittedError();
                return Win32Native.INVALID_HANDLE_VALUE;
            }
        }

        public override SafeFileHandle SafeFileHandle {
            [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.UnmanagedCode)]
            [SecurityPermissionAttribute(SecurityAction.InheritanceDemand, Flags=SecurityPermissionFlag.UnmanagedCode)]
            get {
                NotPermittedError();
                return null;
            }
        }

        public override void SetLength(long value) 
        {
            m_isf.Lock(); // oldLen needs to be protected

            try {
                ulong oldLen = (ulong)m_fs.Length;
                ulong newLen = (ulong)value;
    
                // Reserve before the operation.
                m_isf.Reserve(oldLen, newLen);
    
                try {
    
                    ZeroInit(oldLen, newLen);
    
                    m_fs.SetLength(value);
    
                } catch {
    
                    // Undo the reserve
                    m_isf.UndoReserveOperation(oldLen, newLen);
    
                    throw;
                }
    
                // Unreserve if this operation reduced the file size.
                if (oldLen > newLen)
                {
                    // params oldlen, newlength reversed on purpose.
                    m_isf.UndoReserveOperation(newLen, oldLen);
                }

            } finally {
                m_isf.Unlock();
            }
        }

        // 0 out the allocated disk so that 
        // untrusted apps won't be able to read garbage, which
        // is a security  hole, if allowed.
        // This may not be necessary in some file systems ?
        private void ZeroInit(ulong oldLen, ulong newLen)
        {
            if (oldLen >= newLen)
                return;

            ulong    rem  = newLen - oldLen;
            byte[] buffer = new byte[s_BlockSize];  // buffer is zero inited 
                                                    // here by the runtime 
                                                    // memory allocator.

            // back up the current position.
            long pos      = m_fs.Position;

            m_fs.Seek((long)oldLen, SeekOrigin.Begin);

            // If we have a small number of bytes to write, do that and
            // we are done.
            if (rem <= (ulong)s_BlockSize)
            {
                m_fs.Write(buffer, 0, (int)rem);
                m_fs.Position = pos;
                return;
            }

            // Block write is better than writing a byte in a loop
            // or all bytes. The number of bytes to write could
            // be very large.

            // Align to block size
            // allign = s_BlockSize - (int)(oldLen % s_BlockSize);
            // Converting % to & operation since s_BlockSize is a power of 2

            int allign = s_BlockSize - (int)(oldLen & ((ulong)s_BlockSize - 1));

            /* 
                this will never happen since we already handled this case
                leaving this code here for documentation
            if ((ulong)allign > rem)
                allign = (int)rem;
            */

            m_fs.Write(buffer, 0, allign);
            rem -= (ulong)allign;

            int nBlocks = (int)(rem / s_BlockSize);

            // Write out one block at a time.
            for (int i=0; i<nBlocks; ++i)
                m_fs.Write(buffer, 0, s_BlockSize);

            // Write out the remaining bytes.
            // m_fs.Write(buffer, 0, (int) (rem % s_BlockSize));
            // Converting % to & operation since s_BlockSize is a power of 2
            m_fs.Write(buffer, 0, (int) (rem & ((ulong)s_BlockSize - 1)));

            // restore the current position
            m_fs.Position = pos;
        }

        public override int Read(byte[] buffer, int offset, int count) {
            return m_fs.Read(buffer, offset, count);
        }

        public override int ReadByte() {
            return m_fs.ReadByte();
        }

        public override long Seek(long offset, SeekOrigin origin) 
        {
            long  ret;
            
            m_isf.Lock(); // oldLen needs to be protected
            try {
                    
                // Seek operation could increase the file size, make sure
                // that the quota is updated, and file is zeroed out

                ulong oldLen;
                ulong newLen;
                oldLen = (ulong) m_fs.Length;

                switch (origin) {
                case SeekOrigin.Begin:
                    newLen = (ulong)offset;
                    break;
                case SeekOrigin.Current:
                    newLen = (ulong)(m_fs.Position + offset);
                    break;
                case SeekOrigin.End:
                    newLen = oldLen + (ulong)offset;
                    break;
                default:
                    throw new ArgumentException(
                        Environment.GetResourceString(
                            "IsolatedStorage_SeekOrigin"));
                }

                m_isf.Reserve(oldLen, newLen);

                try {

                    ZeroInit(oldLen, newLen);

                    ret = m_fs.Seek(offset, origin);

                } catch {

                    m_isf.UndoReserveOperation(oldLen, newLen);

                    throw;
                }
            }
            finally
            {
                m_isf.Unlock();
            }

            return ret;
        }

        public override void Write(byte[] buffer, int offset, int count) 
        {
            m_isf.Lock(); // oldLen needs to be protected
            try {
                    
                ulong oldLen = (ulong)m_fs.Length;
                ulong newLen = (ulong)(m_fs.Position + count);

                m_isf.Reserve(oldLen, newLen);

                try {

                    m_fs.Write(buffer, offset, count);

                } catch {

                    m_isf.UndoReserveOperation(oldLen, newLen);

                    throw;
                }
            }
            finally
            {
                m_isf.Unlock();
            }
        }

        public override void WriteByte(byte value)
        {
            m_isf.Lock(); // oldLen needs to be protected
            try {
                ulong oldLen = (ulong)m_fs.Length;
                ulong newLen = (ulong)m_fs.Position + 1;

                m_isf.Reserve(oldLen, newLen);

                try {
                    
                    m_fs.WriteByte(value);

                } catch {

                    m_isf.UndoReserveOperation(oldLen, newLen);

                    throw;
                }
            }
            finally {
                m_isf.Unlock(); 
            }
        }

        [HostProtection(ExternalThreading=true)]
        public override IAsyncResult BeginRead(byte[] buffer, int offset, 
            int numBytes, AsyncCallback userCallback, Object stateObject) {
            return m_fs.BeginRead(buffer, offset, numBytes, userCallback, stateObject);
        }

        public override int EndRead(IAsyncResult asyncResult) {
            // try-catch to avoid leaking path info
            return m_fs.EndRead(asyncResult);
                
        }

        [HostProtection(ExternalThreading=true)]
        public override IAsyncResult BeginWrite(byte[] buffer, int offset, 
            int numBytes, AsyncCallback userCallback, Object stateObject) {

            m_isf.Lock(); // oldLen needs to be protected
            try {
                    
                ulong oldLen = (ulong)m_fs.Length;
                ulong newLen = (ulong)m_fs.Position + (ulong)numBytes;
                m_isf.Reserve(oldLen, newLen);

                try {
                    
                    return m_fs.BeginWrite(buffer, offset, numBytes, userCallback, stateObject);

                } catch {

                    m_isf.UndoReserveOperation(oldLen, newLen);

                    throw;
                }
            }
            finally
            {
                m_isf.Unlock();
            }
        }

        public override void EndWrite(IAsyncResult asyncResult) {
            m_fs.EndWrite(asyncResult);
        }

        internal void NotPermittedError(String str) {
            throw new IsolatedStorageException(str);
        }

        internal void NotPermittedError() {
            NotPermittedError(Environment.GetResourceString(
                "IsolatedStorage_Operation"));
        }

    }
}

