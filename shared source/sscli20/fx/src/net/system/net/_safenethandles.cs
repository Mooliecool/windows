//------------------------------------------------------------------------------
// <copyright file="_SafeNetHandles.cs" company="Microsoft">
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
        The file contains _all_ SafeHandles implementations for System.Net namespace.
        These handle wrappers do guarantee that OS resources get cleaned up when the app domain dies.

        All PInvoke declarations that do freeing  the  OS resources  _must_ be in this file
        All PInvoke declarations that do allocation the OS resources _must_ be in this file


Details:

        The protection from leaking OF the OS resources is based on two technologies
        1) SafeHandle class
        2) Non interuptible regions using Constrained Execution Region (CER) technology

        For simple cases SafeHandle class does all the job. The Prerequisites are:
        - A resource is able to be represented by IntPtr type (32 bits on 32 bits platforms).
        - There is a PInvoke availble that does the creation of the resource.
          That PInvoke either returns the handle value or it writes the handle into out/ref parameter.
        - The above PInvoke as part of the call does NOT free any OS resource.

        For those "simple" cases we desinged SafeHandle-derived classes that provide
        static methods to allocate a handle object.
        Each such derived class provides a handle release method that is run as non-interrupted.

        For more complicated cases we employ the support for non-interruptible methods (CERs).
        Each CER is a tree of code rooted at a catch or finally clause for a specially marked exception
        handler (preceded by the RuntimeHelpers.PrepareConstrainedRegions() marker) or the Dispose or
        ReleaseHandle method of a SafeHandle derived class. The graph is automatically computed by the
        runtime (typically at the jit time of the root method), but cannot follow virtual or interface
        calls (these must be explicitly prepared via RuntimeHelpers.PrepareMethod once the definite target
        method is known). Also, methods in the graph that must be included in the CER must be marked with
        a reliability contract stating guarantees about the consistency of the system if an error occurs
        while they are executing. Look for ReliabilityContract for examples (a full explanation of the
        semantics of this contract is beyond the scope of this comment).

        An example of the top-level of a CER:

            RuntimeHelpers.PrepareConstrainedRegions();
            try
            {
                // Normal code
            }
            finally
            {
                // Guaranteed to get here even in low memory scenarios. Thread abort will not interrupt
                // this clause and we won't fail because of a jit allocation of any method called (modulo
                // restrictions on interface/virtual calls listed above and further restrictions listed
                // below).
            }

        Another common pattern is an empty-try (where you really just want a region of code the runtime
        won't interrupt you in):

            RuntimeHelpers.PrepareConstrainedRegions();
            try {} finally
            {
                // Non-interruptible code here
            }

        This ugly syntax will be supplanted with compiler support at some point.

        While within a CER region certain restrictions apply in order to avoid having the runtime inject
        a potential fault point into your code (and of course you're are responsible for ensuring your
        code doesn't inject any explicit fault points of its own unless you know how to tolerate them).

        A quick and dirty guide to the possible causes of fault points in CER regions:
        - Explicit allocations (though allocating a value type only implies allocation on the stack,
          which may not present an issue).
        - Boxing a value type (C# does this implicitly for you in many cases, so be careful).
        - Use of Monitor.Enter or the lock keyword.
        - Accessing a multi-dimensional array.
        - Calling any method outside your control that doesn't make a guarantee (e.g. via a
          ReliabilityAttribute) that it doesn't introduce failure points.
        - Making P/Invoke calls with non-blittable parameters types. Blittable types are:
            - SafeHandle when used as an [in] parameter
            - NON BOXED base types that fit onto a machine word
            - ref struct with blittable fields
            - class type with blittable fields
            - pinned Unicode strings using "fixed" statement
            - pointers of any kind
            - IntPtr type
        - P/Invokes should not have any CharSet attribute on it's declaration.
          Obvioulsy string types should not appear in the parameters.
        - String type MUST not appear in a field of a marshaled ref struct or class in a P?Invoke

Author:

    Alexei Vopilov    04-Sept-2002

Revision History:

--*/

namespace System.Net {
    using System.Net.Cache;
    using System.Net.Sockets;
    using System.Security;
    using System.Runtime.InteropServices;
    using System.Runtime.CompilerServices;
    using System.Threading;
    using System.Security.Permissions;
    using System.ComponentModel;
    using System.Text;
    using System.Globalization;
    using Microsoft.Win32.SafeHandles;
    using System.Runtime.ConstrainedExecution;

#if DEBUG
    //
    // This is a helper class for debugging GC-ed handles that we define.
    // As a general rule normal code path should always destroy handles explicitly
    //
    internal abstract class DebugSafeHandle: SafeHandleZeroOrMinusOneIsInvalid {
        string m_Trace;

        protected DebugSafeHandle(bool ownsHandle): base(ownsHandle) {
            Trace();
        }

        protected DebugSafeHandle(IntPtr invalidValue, bool ownsHandle): base(ownsHandle) {
            SetHandle(invalidValue);
            Trace();
        }

        [EnvironmentPermission(SecurityAction.Assert, Unrestricted=true)]
        private void Trace() {
            m_Trace = "WARNING! GC-ed  >>" + this.GetType().FullName + "<< (should be excplicitly closed) \r\n";
        }

        [ReliabilityContract(Consistency.MayCorruptAppDomain, Cer.None)]
        ~DebugSafeHandle() {
            GlobalLog.SetThreadSource(ThreadKinds.Finalization);
            GlobalLog.Print(m_Trace);
        }
    }

    //
    // This is a helper class for debugging GC-ed handles that we define.
    // As a general rule normal code path should always destroy handles explicitly
    //
    internal abstract class DebugCriticalHandleMinusOneIsInvalid : CriticalHandleMinusOneIsInvalid {
        string m_Trace;

        protected DebugCriticalHandleMinusOneIsInvalid(): base() {
            Trace();
        }

        [EnvironmentPermission(SecurityAction.Assert, Unrestricted=true)]
        private void Trace() {
            m_Trace = "WARNING! GC-ed  >>" + this.GetType().FullName + "<< (should be excplicitly closed) \r\n";
            GlobalLog.Print("Creating SafeHandle, type = " + this.GetType().FullName);
        }

        ~DebugCriticalHandleMinusOneIsInvalid() {
            GlobalLog.SetThreadSource(ThreadKinds.Finalization);
            GlobalLog.Print(m_Trace);
        }
    }

    //
    // This is a helper class for debugging GC-ed handles that we define.
    // As a general rule normal code path should always destroy handles explicitly
    //
    internal abstract class DebugSafeHandleMinusOneIsInvalid : SafeHandleMinusOneIsInvalid {
        string m_Trace;

        protected DebugSafeHandleMinusOneIsInvalid(bool ownsHandle): base(ownsHandle) {
            Trace();
        }

        [EnvironmentPermission(SecurityAction.Assert, Unrestricted=true)]
        private void Trace() {
            m_Trace = "WARNING! GC-ed  >>" + this.GetType().FullName + "<< (should be excplicitly closed) \r\n";
            GlobalLog.Print("Creating SafeHandle, type = " + this.GetType().FullName);
        }

        ~DebugSafeHandleMinusOneIsInvalid() {
            GlobalLog.SetThreadSource(ThreadKinds.Finalization);
            GlobalLog.Print(m_Trace);
        }
    }

    //
    // This is a helper class for debugging GC-ed handles that we define.
    // As a general rule normal code path should always destroy handles explicitly
    //
    internal abstract class DebugCriticalHandleZeroOrMinusOneIsInvalid : CriticalHandleZeroOrMinusOneIsInvalid {
        string m_Trace;

        protected DebugCriticalHandleZeroOrMinusOneIsInvalid(): base() {
            Trace();
        }

        [EnvironmentPermission(SecurityAction.Assert, Unrestricted=true)]
        private void Trace() {
            m_Trace = "WARNING! GC-ed  >>" + this.GetType().FullName + "<< (should be excplicitly closed) \r\n";
            GlobalLog.Print("Creating SafeHandle, type = " + this.GetType().FullName);
        }

        ~DebugCriticalHandleZeroOrMinusOneIsInvalid() {
            GlobalLog.SetThreadSource(ThreadKinds.Finalization);
            GlobalLog.Print(m_Trace);
        }
    }
#endif // DEBUG



    ///////////////////////////////////////////////////////////////
    //
    // This is safe handle factory for any object that depends on
    // KERNEL32 CloseHandle as the handle disposal method.
    //
    ///////////////////////////////////////////////////////////////
    [SuppressUnmanagedCodeSecurity]
#if DEBUG
    internal sealed class SafeCloseHandle : DebugCriticalHandleZeroOrMinusOneIsInvalid {
#else
    internal sealed class SafeCloseHandle : CriticalHandleZeroOrMinusOneIsInvalid {
#endif
        private const string SECURITY = "security.dll";
        private const string ADVAPI32 = "advapi32.dll";
        private const string HTTPAPI = "httpapi.dll";

        private int _disposed;

        private SafeCloseHandle() : base() {
        }

        internal IntPtr DangerousGetHandle() {
            return handle;
        }

        protected override bool ReleaseHandle() {
            if (!IsInvalid) {
                if (Interlocked.Increment(ref _disposed) == 1) {
                    return UnsafeNclNativeMethods.SafeNetHandles.CloseHandle(handle);
                }
            }
            return true;
        }


    }


    ///////////////////////////////////////////////////////////////
    //
    // This is implementaion of Safe AllocHGlobal which is turned out
    // to be LocalAlloc down in CLR
    //
    ///////////////////////////////////////////////////////////////
    [SuppressUnmanagedCodeSecurity]
#if DEBUG
    internal sealed class SafeLocalFree : DebugSafeHandle {
#else
    internal sealed class SafeLocalFree : SafeHandleZeroOrMinusOneIsInvalid {
#endif
        private const int LMEM_FIXED = 0;
        private const int NULL = 0;

        // This returned handle cannot be modified by the application.
        public static SafeLocalFree Zero = new SafeLocalFree(false);

        private SafeLocalFree() : base(true) {}

        private SafeLocalFree(bool ownsHandle) : base(ownsHandle) {}

        public static SafeLocalFree LocalAlloc(int cb) {
            SafeLocalFree result = UnsafeNclNativeMethods.SafeNetHandles.LocalAlloc(LMEM_FIXED, (UIntPtr) cb);
            if (result.IsInvalid) {
                result.SetHandleAsInvalid();
                throw new OutOfMemoryException();
            }
            return result;
        }

        override protected bool ReleaseHandle()
        {
            return UnsafeNclNativeMethods.SafeNetHandles.LocalFree(handle) == IntPtr.Zero;
        }
    }

    ///////////////////////////////////////////////////////////////
    //
    // A few Win32 APIs return pointers to blobs that need GlobalFree().
    //
    ///////////////////////////////////////////////////////////////
    [SuppressUnmanagedCodeSecurity]
#if DEBUG
    internal sealed class SafeGlobalFree : DebugSafeHandle {
#else
    internal sealed class SafeGlobalFree : SafeHandleZeroOrMinusOneIsInvalid
    {
#endif
        private SafeGlobalFree() : base(true) { }
        private SafeGlobalFree(bool ownsHandle) : base(ownsHandle) { }

        override protected bool ReleaseHandle()
        {
            return UnsafeNclNativeMethods.SafeNetHandles.GlobalFree(handle) == IntPtr.Zero;
        }
    }

    [ComVisible(false)]
#if DEBUG
    internal sealed class SafeOverlappedFree : DebugSafeHandle {
#else
    internal sealed class SafeOverlappedFree : SafeHandleZeroOrMinusOneIsInvalid {
#endif
        private const int LPTR = 0x0040;

        private SafeCloseSocket             _socketHandle;

        private SafeOverlappedFree() : base(true) {}
        private SafeOverlappedFree(bool ownsHandle) : base(ownsHandle) {}

        public static SafeOverlappedFree Alloc() {
            SafeOverlappedFree result = UnsafeNclNativeMethods.SafeNetHandlesSafeOverlappedFree.LocalAlloc(LPTR, (UIntPtr) Win32.OverlappedSize);
            if (result.IsInvalid) {
                result.SetHandleAsInvalid();
                throw new OutOfMemoryException();
            }
            return result;
        }

        public static SafeOverlappedFree Alloc(SafeCloseSocket socketHandle) {
            SafeOverlappedFree result = Alloc();
            result._socketHandle = socketHandle;
            return result;
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        public void Close (bool resetOwner)
        {
            RuntimeHelpers.PrepareConstrainedRegions();
            try {} finally {
                if (resetOwner) {
                    _socketHandle = null;
                }
                Close();
            }
        }

        unsafe override protected bool ReleaseHandle()
        {
            SafeCloseSocket socketHandle = _socketHandle;
            if (socketHandle != null && !socketHandle.IsInvalid)
            {
                // We are being finalized while the I/O operation associated
                // with the current overlapped is still pending (e.g. on app
                // domain shutdown). The socket has to be closed first to
                // avoid reuse after delete of the native overlapped structure.
                socketHandle.Dispose();
            }
            // Release the native overlapped structure
            return UnsafeNclNativeMethods.SafeNetHandles.LocalFree(handle) == IntPtr.Zero;
        }
    }



    ///////////////////////////////////////////////////////////////
    //
    // This class implements a safe socket handle.
    // It uses an inner and outer SafeHandle to do so.  The inner
    // SafeHandle holds the actual socket, but only ever has one
    // reference to it.  The outer SafeHandle guards the inner
    // SafeHandle with real ref counting.  When the outer SafeHandle
    // is cleaned up, it releases the inner SafeHandle - since
    // its ref is the only ref to the inner SafeHandle, it deterministically
    // gets closed at that point - no races with concurrent IO calls.
    // This allows Close() on the outer SafeHandle to deterministically
    // close the inner SafeHandle, in turn allowing the inner SafeHandle
    // to block the user thread in case a graceful close has been
    // requested.  (It's not legal to block any other thread - such closes
    // are always abortive.)
    //
    ///////////////////////////////////////////////////////////////
    [SuppressUnmanagedCodeSecurity]
#if DEBUG
    internal class SafeCloseSocket : DebugSafeHandleMinusOneIsInvalid
#else
    internal class SafeCloseSocket : SafeHandleMinusOneIsInvalid
#endif
    {
        protected SafeCloseSocket() : base(true) { }

        private InnerSafeCloseSocket m_InnerSocket;
        private volatile bool m_Released;
#if DEBUG
        private InnerSafeCloseSocket m_InnerSocketCopy;
#endif

        public override bool IsInvalid {
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            get {
                return IsClosed || base.IsInvalid;
            }
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        private void SetInnerSocket(InnerSafeCloseSocket socket)
        {
            m_InnerSocket = socket;
            SetHandle(socket.DangerousGetHandle());
#if DEBUG
            m_InnerSocketCopy = socket;
#endif
        }

        private static SafeCloseSocket CreateSocket(InnerSafeCloseSocket socket)
        {
            SafeCloseSocket ret = new SafeCloseSocket();
            CreateSocket(socket, ret);
            return ret;
        }

        protected static void CreateSocket(InnerSafeCloseSocket socket, SafeCloseSocket target)
        {
            if (socket!=null && socket.IsInvalid) {
                target.SetHandleAsInvalid();
                return;
            }

            bool b = false;
            RuntimeHelpers.PrepareConstrainedRegions();
            try
            {
                socket.DangerousAddRef(ref b);
            }
            catch
            {
                if (b)
                {
                    socket.DangerousRelease();
                    b = false;
                }
            }
            finally
            {
                if (b)
                {
                    target.SetInnerSocket(socket);
                    socket.Close();
                }
                else
                {
                    target.SetHandleAsInvalid();
                }
            }
        }

        internal unsafe static SafeCloseSocket CreateWSASocket(byte* pinnedBuffer)
        {
            return CreateSocket(InnerSafeCloseSocket.CreateWSASocket(pinnedBuffer));
        }

        internal static SafeCloseSocket CreateWSASocket(AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType)
        {
            return CreateSocket(InnerSafeCloseSocket.CreateWSASocket(addressFamily, socketType, protocolType));
        }

        internal static SafeCloseSocket Accept(
                                            SafeCloseSocket socketHandle,
                                            byte[] socketAddress,
                                            ref int socketAddressSize
                                            )
        {
            return CreateSocket(InnerSafeCloseSocket.Accept(socketHandle, socketAddress, ref socketAddressSize));
        }

        protected override bool ReleaseHandle()
        {
            m_Released = true;
            InnerSafeCloseSocket innerSocket = m_InnerSocket == null ? null : Interlocked.Exchange<InnerSafeCloseSocket>(ref m_InnerSocket, null);
            if (innerSocket != null)
            {
                innerSocket.DangerousRelease();
            }
            return true;
        }

        internal void CloseAsIs()
        {
            RuntimeHelpers.PrepareConstrainedRegions();
            try { }
            finally
            {
#if DEBUG
                // If this throws it could be very bad.
                try
                {
#endif
                InnerSafeCloseSocket innerSocket = m_InnerSocket == null ? null : Interlocked.Exchange<InnerSafeCloseSocket>(ref m_InnerSocket, null);
                Close();
                if (innerSocket != null)
                {
                    // Wait until it's safe.
                    while (!m_Released)
                    {
                        Thread.SpinWait(1);
                    }

                    // Now free it with blocking.
                    innerSocket.BlockingRelease();
                }
#if DEBUG
                }
                catch (Exception exception)
                {
                    if (!NclUtilities.IsFatal(exception)){
                        GlobalLog.Assert("SafeCloseSocket::CloseAsIs(handle:" + handle.ToString("x") + ")", exception.Message);
                    }
                    throw;
                }
#endif
            }
        }

        internal class InnerSafeCloseSocket : SafeHandleMinusOneIsInvalid
        {
            protected InnerSafeCloseSocket() : base(true) { }

            private static readonly byte [] tempBuffer = new byte[1];
            private bool m_Blockable;

            public override bool IsInvalid {
                [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
                get {
                    return IsClosed || base.IsInvalid;
                }
            }

            // This method is implicitly reliable and called from a CER.
            protected override bool ReleaseHandle()
            {
                bool ret = false;

#if DEBUG
                try
                {
#endif
                GlobalLog.Print("SafeCloseSocket::ReleaseHandle(handle:" + handle.ToString("x") + ")");

                SocketError errorCode;

                // If m_Blockable was set in BlockingRelease, it's safe to block here, which means
                // we can honor the linger options set on the socket.  It also means closesocket() might return WSAEWOULDBLOCK, in which
                // case we need to do some recovery.
                if (m_Blockable)
                {
                    GlobalLog.Print("SafeCloseSocket::ReleaseHandle(handle:" + handle.ToString("x") + ") Following 'blockable' branch.");

                    errorCode = UnsafeNclNativeMethods.SafeNetHandles.closesocket(handle);
#if DEBUG
                    m_CloseSocketHandle = handle;
                    m_CloseSocketResult = errorCode;
#endif
                    if (errorCode == SocketError.SocketError) errorCode = (SocketError) Marshal.GetLastWin32Error();
                    GlobalLog.Print("SafeCloseSocket::ReleaseHandle(handle:" + handle.ToString("x") + ") closesocket()#1:" + errorCode.ToString());

                    // If it's not WSAEWOULDBLOCK, there's no more recourse - we either succeeded or failed.
                    if (errorCode != SocketError.WouldBlock)
                    {
                        return ret = errorCode == SocketError.Success;
                    }

                    // The socket must be non-blocking with a linger timeout set.
                    // We have to set the socket to blocking.
                    int nonBlockCmd = 0;
                    errorCode = UnsafeNclNativeMethods.SafeNetHandles.ioctlsocket(
                        handle,
                        IoctlSocketConstants.FIONBIO,
                        ref nonBlockCmd);
                    if (errorCode == SocketError.SocketError) errorCode = (SocketError) Marshal.GetLastWin32Error();
                    GlobalLog.Print("SafeCloseSocket::ReleaseHandle(handle:" + handle.ToString("x") + ") ioctlsocket()#1:" + errorCode.ToString());

                    // This can fail if there's a pending WSAEventSelect.  Try canceling it.
                    if (errorCode == SocketError.InvalidArgument)
                    {
                        errorCode = UnsafeNclNativeMethods.SafeNetHandles.WSAEventSelect(
                            handle,
                            IntPtr.Zero,
                            AsyncEventBits.FdNone);
                        GlobalLog.Print("SafeCloseSocket::ReleaseHandle(handle:" + handle.ToString("x") + ") WSAEventSelect():" + (errorCode == SocketError.SocketError ? (SocketError)Marshal.GetLastWin32Error() : errorCode).ToString());

                        // Now retry the ioctl.
                        errorCode = UnsafeNclNativeMethods.SafeNetHandles.ioctlsocket(
                            handle,
                            IoctlSocketConstants.FIONBIO,
                            ref nonBlockCmd);
                        GlobalLog.Print("SafeCloseSocket::ReleaseHandle(handle:" + handle.ToString("x") + ") ioctlsocket#2():" + (errorCode == SocketError.SocketError ? (SocketError)Marshal.GetLastWin32Error() : errorCode).ToString());
                    }

                    // If that succeeded, try again.
                    if (errorCode == SocketError.Success)
                    {
                        errorCode = UnsafeNclNativeMethods.SafeNetHandles.closesocket(handle);
#if DEBUG
                        m_CloseSocketHandle = handle;
                        m_CloseSocketResult = errorCode;
#endif
                        if (errorCode == SocketError.SocketError) errorCode = (SocketError) Marshal.GetLastWin32Error();
                        GlobalLog.Print("SafeCloseSocket::ReleaseHandle(handle:" + handle.ToString("x") + ") closesocket#2():" + errorCode.ToString());

                        // If it's not WSAEWOULDBLOCK, there's no more recourse - we either succeeded or failed.
                        if (errorCode != SocketError.WouldBlock)
                        {
                            return ret = errorCode == SocketError.Success;
                        }
                    }

                    // It failed.  Fall through to the regular abortive close.
                }

                // By default or if CloseAsIs() path failed, set linger timeout to zero to get an abortive close (RST).
                Linger lingerStruct;
                lingerStruct.OnOff = 1;
                lingerStruct.Time = (short) 0;

                errorCode = UnsafeNclNativeMethods.SafeNetHandles.setsockopt(
                    handle,
                    SocketOptionLevel.Socket,
                    SocketOptionName.Linger,
                    ref lingerStruct,
                    Linger.Size);
#if DEBUG
                m_CloseSocketLinger = errorCode;
#endif
                if (errorCode == SocketError.SocketError) errorCode = (SocketError) Marshal.GetLastWin32Error();
                GlobalLog.Print("SafeCloseSocket::ReleaseHandle(handle:" + handle.ToString("x") + ") setsockopt():" + errorCode.ToString());

                if (errorCode != SocketError.Success && errorCode != SocketError.InvalidArgument && errorCode != SocketError.ProtocolOption)
                {
                    // Too dangerous to try closesocket() - it might block!
                    return ret = false;
                }

                errorCode = UnsafeNclNativeMethods.SafeNetHandles.closesocket(handle);
#if DEBUG
                m_CloseSocketHandle = handle;
                m_CloseSocketResult = errorCode;
#endif
                GlobalLog.Print("SafeCloseSocket::ReleaseHandle(handle:" + handle.ToString("x") + ") closesocket#3():" + (errorCode == SocketError.SocketError ? (SocketError)Marshal.GetLastWin32Error() : errorCode).ToString());

                return ret = errorCode == SocketError.Success;
#if DEBUG
                }
                catch (Exception exception)
                {
                    if (!NclUtilities.IsFatal(exception)){
                        GlobalLog.Assert("SafeCloseSocket::ReleaseHandle(handle:" + handle.ToString("x") + ")", exception.Message);
                    }
                    ret = true;  // Avoid a second assert.
                    throw;
                }
                finally
                {
                    m_CloseSocketThread = Thread.CurrentThread.ManagedThreadId;
                    m_CloseSocketTick = Environment.TickCount;
                    GlobalLog.Assert(ret, "SafeCloseSocket::ReleaseHandle(handle:{0:x})|ReleaseHandle failed.", handle);
                }
#endif
            }

#if DEBUG
            private IntPtr m_CloseSocketHandle;
            private SocketError m_CloseSocketResult = unchecked((SocketError) 0xdeadbeef);
            private SocketError m_CloseSocketLinger = unchecked((SocketError) 0xdeadbeef);
            private int m_CloseSocketThread;
            private int m_CloseSocketTick;
#endif

            // Use this method to close the socket handle using the linger options specified on the socket.
            // Guaranteed to only be called once, under a CER, and not if regular DangerousRelease is called.
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            internal void BlockingRelease()
            {
                m_Blockable = true;
                DangerousRelease();
            }

            internal unsafe static InnerSafeCloseSocket CreateWSASocket(byte* pinnedBuffer)
            {
                //-1 is the value for FROM_PROTOCOL_INFO
                InnerSafeCloseSocket result = UnsafeNclNativeMethods.OSSOCK.WSASocket((AddressFamily) (-1),(SocketType) (-1),(ProtocolType) (-1), pinnedBuffer, 0, SocketConstructorFlags.WSA_FLAG_OVERLAPPED);
                if (result.IsInvalid) {
                    result.SetHandleAsInvalid();
                }
                return result;
            }

            internal static InnerSafeCloseSocket CreateWSASocket(AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType)
            {
                InnerSafeCloseSocket result = UnsafeNclNativeMethods.OSSOCK.WSASocket(addressFamily, socketType, protocolType, IntPtr.Zero, 0, SocketConstructorFlags.WSA_FLAG_OVERLAPPED);
                if (result.IsInvalid) {
                    result.SetHandleAsInvalid();
                }
                return result;
            }

            internal static InnerSafeCloseSocket Accept(SafeCloseSocket socketHandle, byte[] socketAddress, ref int socketAddressSize)
            {
                InnerSafeCloseSocket result = UnsafeNclNativeMethods.SafeNetHandles.accept(socketHandle.DangerousGetHandle(), socketAddress, ref socketAddressSize);
                if (result.IsInvalid) {
                    result.SetHandleAsInvalid();
                }
                return result;
            }
        }
    }


    [SuppressUnmanagedCodeSecurity]
    internal sealed class SafeCloseSocketAndEvent: SafeCloseSocket {
        internal SafeCloseSocketAndEvent() : base() {}
        private AutoResetEvent waitHandle;

        override protected bool ReleaseHandle()
        {
            bool result = base.ReleaseHandle();
            DeleteEvent();
            return result;
         }

        internal static SafeCloseSocketAndEvent CreateWSASocketWithEvent(AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType, bool autoReset, bool signaled){
            SafeCloseSocketAndEvent result = new SafeCloseSocketAndEvent();
            CreateSocket(InnerSafeCloseSocket.CreateWSASocket(addressFamily, socketType, protocolType), result);
            if (result.IsInvalid) {
                throw new SocketException();
            }

            result.waitHandle = new AutoResetEvent(false);
            CompleteInitialization(result);
            return result;
        }

        internal static void CompleteInitialization(SafeCloseSocketAndEvent socketAndEventHandle){
            SafeWaitHandle handle = socketAndEventHandle.waitHandle.SafeWaitHandle;
            bool b = false;
            RuntimeHelpers.PrepareConstrainedRegions();
            try
            {
                handle.DangerousAddRef(ref b);
            }
            catch
            {
                if (b)
                {
                    handle.DangerousRelease();
                    socketAndEventHandle.waitHandle = null;
                    b = false;
                }
            }
            finally
            {
                if (b)
                {
                    handle.Dispose();
                }
            }
        }


        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        private void DeleteEvent(){
            try{
                if(waitHandle != null){
                    waitHandle.SafeWaitHandle.DangerousRelease();
                }
            }
            catch{
            }
        }

        internal WaitHandle GetEventHandle(){
            return waitHandle;
        }
    }



}
