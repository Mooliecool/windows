//------------------------------------------------------------------------------
// <copyright file="Socket.cs" company="Microsoft">
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

namespace System.Net.Sockets {
    using System.Collections;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.Configuration;
    using System.Diagnostics;
    using System.Globalization;
    using System.IO;
    using System.Net;
    using System.Net.Configuration;
    using System.Runtime.InteropServices;
    using System.Security.Permissions;
    using System.Threading;
    using Microsoft.Win32;
    using System.Security;
    using System.Runtime.Versioning;

    /// <devdoc>
    /// <para>The <see cref='Sockets.Socket'/> class implements the Berkeley sockets
    ///    interface.</para>
    /// </devdoc>


    public class Socket : IDisposable
    {
        internal const int DefaultCloseTimeout = -1;

        private object m_AcceptQueueOrConnectResult;

        // the following 8 members represent the state of the socket
        private SafeCloseSocket  m_Handle;
        internal EndPoint   m_RightEndPoint;
        internal EndPoint    m_RemoteEndPoint;
        // this flags monitor if the socket was ever connected at any time and if it still is.
        private bool        m_IsConnected; //  = false;
        private bool        m_IsDisconnected; //  = false;

        // when the socket is created it will be in blocking mode
        // we'll only be able to Accept or Connect, so we only need
        // to handle one of these cases at a time
        private bool        willBlock = true; // desired state of the socket for the user
        private bool        willBlockInternal = true; // actual win32 state of the socket
        private bool        isListening = false;


        // These are constants initialized by constructor
        private AddressFamily   addressFamily;
        private SocketType      socketType;
        private ProtocolType    protocolType;

        // These caches are one degree off of Socket since they're not used in the sync case/when disabled in config.
        private CacheSet m_Caches;

        private class CacheSet
        {
            internal CallbackClosure ConnectClosureCache;
            internal CallbackClosure AcceptClosureCache;
            internal CallbackClosure SendClosureCache;
            internal CallbackClosure ReceiveClosureCache;

            internal OverlappedCache SendOverlappedCache;
            internal OverlappedCache ReceiveOverlappedCache;
        }

        //
        // Overlapped constants.
        //
        // Disable the I/O completion port for Rotor
        internal static bool UseOverlappedIO = true;
        private bool useOverlappedIO;

        // Bool marked true if the native socket m_Handle was bound to the ThreadPool
        private bool        m_BoundToThreadPool; // = false;

        // Event used for async Connect/Accept calls
        private ManualResetEvent m_AsyncEvent;
        private RegisteredWaitHandle m_RegisteredWait;
        private AsyncEventBits m_BlockEventBits = AsyncEventBits.FdNone;

        //These members are to cache permission checks
        private SocketAddress   m_PermittedRemoteAddress;

        private static ConnectExDelegate s_ConnectEx;
        private static DisconnectExDelegate s_DisconnectEx;
        private static DisconnectExDelegate_Blocking s_DisconnectEx_Blocking;
        private static WSARecvMsgDelegate s_WSARecvMsg;
        private static WSARecvMsgDelegate_Blocking s_WSARecvMsg_Blocking;

        private static object s_InternalSyncObject;
        private int m_CloseTimeout = Socket.DefaultCloseTimeout;
        private int m_IntCleanedUp;                 // 0 if not completed >0 otherwise.
        private const int microcnv = 1000000;
        private readonly static int protocolInformationSize = Marshal.SizeOf(typeof(UnsafeNclNativeMethods.OSSOCK.WSAPROTOCOL_INFO));

        internal static bool s_SupportsIPv4;
        internal static bool s_SupportsIPv6;
        internal static bool s_OSSupportsIPv6;
        internal static bool s_Initialized;
        private static WaitOrTimerCallback s_RegisteredWaitCallback;

//************* constructors *************************

        //------------------------------------

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of the <see cref='Sockets.Socket'/> class.
        ///    </para>
        /// </devdoc>
        public Socket(AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType) {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "Socket", addressFamily);
            InitializeSockets();
            m_Handle = SafeCloseSocket.CreateWSASocket(
                    addressFamily,
                    socketType,
                    protocolType);

            if (m_Handle.IsInvalid) {
                //
                // failed to create the win32 socket, throw
                //
                throw new SocketException();
            }

            this.addressFamily = addressFamily;
            this.socketType = socketType;
            this.protocolType = protocolType;

            if(Logging.On)Logging.Exit(Logging.Sockets, this, "Socket", null);
        }


        public Socket(SocketInformation socketInformation) {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "Socket", addressFamily);

            ExceptionHelper.UnrestrictedSocketPermission.Demand();

            InitializeSockets();
            if(socketInformation.ProtocolInformation == null || socketInformation.ProtocolInformation.Length < protocolInformationSize){
                throw new ArgumentException(SR.GetString(SR.net_sockets_invalid_socketinformation), "socketInformation.ProtocolInformation");
            }

            unsafe{
                fixed(byte * pinnedBuffer = socketInformation.ProtocolInformation){
                    m_Handle = SafeCloseSocket.CreateWSASocket(pinnedBuffer);

                    UnsafeNclNativeMethods.OSSOCK.WSAPROTOCOL_INFO protocolInfo = (UnsafeNclNativeMethods.OSSOCK.WSAPROTOCOL_INFO)Marshal.PtrToStructure((IntPtr)pinnedBuffer, typeof(UnsafeNclNativeMethods.OSSOCK.WSAPROTOCOL_INFO));
                    addressFamily = protocolInfo.iAddressFamily;
                    socketType = (SocketType)protocolInfo.iSocketType;
                    protocolType = (ProtocolType)protocolInfo.iProtocol;
                }
            }

            if (m_Handle.IsInvalid) {
                SocketException e = new SocketException();
                if(e.ErrorCode == (int)SocketError.InvalidArgument){
                    throw new ArgumentException(SR.GetString(SR.net_sockets_invalid_socketinformation), "socketInformation");
                }
                else {
                    throw e;
                }
            }

            if (addressFamily != AddressFamily.InterNetwork && addressFamily != AddressFamily.InterNetworkV6) {
                throw new NotSupportedException(SR.GetString(SR.net_invalidversion));
            }

            m_IsConnected = socketInformation.IsConnected;
            willBlock = !socketInformation.IsNonBlocking;
            InternalSetBlocking(willBlock);
            isListening = socketInformation.IsListening;
            UseOnlyOverlappedIO = socketInformation.UseOnlyOverlappedIO;


            //are we bound?  if so, what's the local endpoint?

            EndPoint ep = null;
            if (addressFamily == AddressFamily.InterNetwork ) {
                ep = IPEndPoint.Any;
            }
            else if(addressFamily == AddressFamily.InterNetworkV6) {
                ep = IPEndPoint.IPv6Any;
            }

            SocketAddress socketAddress = ep.Serialize();
            SocketError errorCode;
            try
            {
                errorCode = UnsafeNclNativeMethods.OSSOCK.getsockname(
                    m_Handle,
                    socketAddress.m_Buffer,
                    ref socketAddress.m_Size);
            }
            catch (ObjectDisposedException)
            {
                errorCode = SocketError.NotSocket;
            }

            if (errorCode == SocketError.Success) {
                try {
                    //we're bound
                    m_RightEndPoint = ep.Create(socketAddress);
                }
                catch {
                }
            }

            if(Logging.On)Logging.Exit(Logging.Sockets, this, "Socket", null);
         }


        /// <devdoc>
        ///    <para>
        ///       Called by the class to create a socket to accept an
        ///       incoming request.
        ///    </para>
        /// </devdoc>
        private Socket(SafeCloseSocket fd) {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "Socket", null);
            InitializeSockets();

            //
            if (fd == null || fd.IsInvalid) {
                throw new ArgumentException(SR.GetString(SR.net_InvalidSocketHandle));
            }

            m_Handle = fd;

            addressFamily = Sockets.AddressFamily.Unknown;
            socketType = Sockets.SocketType.Unknown;
            protocolType = Sockets.ProtocolType.Unknown;
            if(Logging.On)Logging.Exit(Logging.Sockets, this, "Socket", null);
        }



//************* properties *************************


        /// <devdoc>
        /// <para>Indicates whether IPv4 support is available and enabled on this machine.</para>
        /// </devdoc>
        public static bool SupportsIPv4 {
            get {
                InitializeSockets();
                return s_SupportsIPv4;
            }
        }

        /// <devdoc>
        /// <para>Indicates whether IPv6 support is available and enabled on this machine.</para>
        /// </devdoc>

        [Obsolete("SupportsIPv6 is obsoleted for this type, please use OSSupportsIPv6 instead. http://go.microsoft.com/fwlink/?linkid=14202")]
        public static bool SupportsIPv6 {
            get {
                InitializeSockets();
                return s_SupportsIPv6;
            }
        }

        internal static bool LegacySupportsIPv6 {
            get {
                InitializeSockets();
                return s_SupportsIPv6;
            }
        }

        public static bool OSSupportsIPv6 {
            get {
                InitializeSockets();
                return s_OSSupportsIPv6;
            }
        }


        /// <devdoc>
        ///    <para>
        ///       Gets the amount of data pending in the network's input buffer that can be
        ///       read from the socket.
        ///    </para>
        /// </devdoc>
        public int Available {
            get {
                if (CleanedUp) {
                    throw new ObjectDisposedException(this.GetType().FullName);
                }

                int argp = 0;

                // This may throw ObjectDisposedException.
                SocketError errorCode = UnsafeNclNativeMethods.OSSOCK.ioctlsocket(
                    m_Handle,
                    IoctlSocketConstants.FIONREAD,
                    ref argp);

                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::Available_get() UnsafeNclNativeMethods.OSSOCK.ioctlsocket returns errorCode:" + errorCode);

                //
                // if the native call fails we'll throw a SocketException
                //
                if (errorCode==SocketError.SocketError) {
                    //
                    // update our internal state after this socket error and throw
                    //
                    SocketException socketException = new SocketException();
                    UpdateStatusAfterSocketError(socketException);
                    if(Logging.On)Logging.Exception(Logging.Sockets, this, "Available", socketException);
                    throw socketException;
                }

                return argp;
            }
         }





        /// <devdoc>
        ///    <para>
        ///       Gets the local end point.
        ///    </para>
        /// </devdoc>
        public EndPoint LocalEndPoint {
            get {
                if (CleanedUp) {
                    throw new ObjectDisposedException(this.GetType().FullName);
                }

                if (m_RightEndPoint==null) {
                    return null;
                }

                SocketAddress socketAddress = m_RightEndPoint.Serialize();

                // This may throw ObjectDisposedException.
                SocketError errorCode = UnsafeNclNativeMethods.OSSOCK.getsockname(
                    m_Handle,
                    socketAddress.m_Buffer,
                    ref socketAddress.m_Size);

                if (errorCode!=SocketError.Success) {
                    //
                    // update our internal state after this socket error and throw
                    //
                    SocketException socketException = new SocketException();
                    UpdateStatusAfterSocketError(socketException);
                    if(Logging.On)Logging.Exception(Logging.Sockets, this, "LocalEndPoint", socketException);
                    throw socketException;
                }

                return m_RightEndPoint.Create(socketAddress);
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets the remote end point
        ///    </para>
        /// </devdoc>
        public EndPoint RemoteEndPoint {
            get {
                if (CleanedUp) {
                    throw new ObjectDisposedException(this.GetType().FullName);
                }

                if (m_RemoteEndPoint==null) {
                    if (m_RightEndPoint==null) {
                        return null;
                    }

                    SocketAddress socketAddress = m_RightEndPoint.Serialize();

                    // This may throw ObjectDisposedException.
                    SocketError errorCode = UnsafeNclNativeMethods.OSSOCK.getpeername(
                        m_Handle,
                        socketAddress.m_Buffer,
                        ref socketAddress.m_Size);

                    if (errorCode!=SocketError.Success) {
                        //
                        // update our internal state after this socket error and throw
                        //
                        SocketException socketException = new SocketException();
                        UpdateStatusAfterSocketError(socketException);
                        if(Logging.On)Logging.Exception(Logging.Sockets, this, "RemoteEndPoint", socketException);
                        throw socketException;
                    }

                    try {
                        m_RemoteEndPoint = m_RightEndPoint.Create(socketAddress);
                    }
                    catch {
                    }
                }

                return m_RemoteEndPoint;
            }
        }

        public IntPtr Handle {
            get {
                ExceptionHelper.UnmanagedPermission.Demand();
                return m_Handle.DangerousGetHandle();
            }
        }
        internal SafeCloseSocket SafeHandle {
            get {
                return m_Handle;
            }
        }


        // Non-blocking I/O control
        /// <devdoc>
        ///    <para>
        ///       Gets and sets the blocking mode of a socket.
        ///    </para>
        /// </devdoc>
        public bool Blocking {
            get {
                //
                // return the user's desired blocking behaviour (not the actual win32 state)
                //
                return willBlock;
            }
            set {
                if (CleanedUp) {
                    throw new ObjectDisposedException(this.GetType().FullName);
                }

                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::set_Blocking() value:" + value.ToString() + " willBlock:" + willBlock.ToString() + " willBlockInternal:" + willBlockInternal.ToString());

                bool current;

                SocketError errorCode = InternalSetBlocking(value, out current);

                if (errorCode!=SocketError.Success) {
                    //
                    // update our internal state after this socket error and throw
                    SocketException socketException = new SocketException(errorCode);
                    UpdateStatusAfterSocketError(socketException);
                    if(Logging.On)Logging.Exception(Logging.Sockets, this, "Blocking", socketException);
                    throw socketException;
                }

                //
                // win32 call succeeded, update desired state
                //
                willBlock = current;
            }
        }

        public bool UseOnlyOverlappedIO{
            get {
                //
                // return the user's desired blocking behaviour (not the actual win32 state)
                //
                return useOverlappedIO;

            }
            set {

                if (m_BoundToThreadPool) {
                    throw new InvalidOperationException(SR.GetString(SR.net_io_completionportwasbound));
                }

                useOverlappedIO = value;
            }
        }


        /// <devdoc>
        ///    <para>
        ///       Gets the connection state of the Socket. This property will return the latest
        ///       known state of the Socket. When it returns false, the Socket was either never connected
        ///       or it is not connected anymore. When it returns true, though, there's no guarantee that the Socket
        ///       is still connected, but only that it was connected at the time of the last IO operation.
        ///    </para>
        /// </devdoc>
        public bool Connected {
            get {
                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::Connected() m_IsConnected:"+m_IsConnected);
                return m_IsConnected;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets the socket's address family.
        ///    </para>
        /// </devdoc>
        public AddressFamily AddressFamily {
            get {
                return addressFamily;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets the socket's socketType.
        ///    </para>
        /// </devdoc>
        public SocketType SocketType {
            get {
                return socketType;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets the socket's protocol socketType.
        ///    </para>
        /// </devdoc>
        public ProtocolType ProtocolType {
            get {
                return protocolType;
            }
        }


        public bool IsBound{
            get{
                return (m_RightEndPoint != null);
            }
        }


        public bool ExclusiveAddressUse{
            get {
                return (int)GetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ExclusiveAddressUse) != 0 ? true : false;
            }
            set {
                if (IsBound) {
                    throw new InvalidOperationException(SR.GetString(SR.net_sockets_mustnotbebound));
                }
                SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ExclusiveAddressUse, value ? 1 : 0);
            }
        }


        public int ReceiveBufferSize {
            get {
                return (int)GetSocketOption(SocketOptionLevel.Socket,
                                     SocketOptionName.ReceiveBuffer);
            }
            set {
                if (value<0) {
                    throw new ArgumentOutOfRangeException("value");
                }

                SetSocketOption(SocketOptionLevel.Socket,
                                  SocketOptionName.ReceiveBuffer, value);
            }
        }

        public int SendBufferSize {
            get {
                return (int)GetSocketOption(SocketOptionLevel.Socket,
                                     SocketOptionName.SendBuffer);
            }

            set {
                if (value<0) {
                    throw new ArgumentOutOfRangeException("value");
                }

                SetSocketOption(SocketOptionLevel.Socket,
                                  SocketOptionName.SendBuffer, value);
            }
        }

        public int ReceiveTimeout {
            get {
                return (int)GetSocketOption(SocketOptionLevel.Socket,
                                     SocketOptionName.ReceiveTimeout);
            }
            set {
                if (value< -1) {
                    throw new ArgumentOutOfRangeException("value");
                }
                if (value == -1) {
                    value = 0;
                }

                SetSocketOption(SocketOptionLevel.Socket,
                                  SocketOptionName.ReceiveTimeout, value);
            }
        }

        public int SendTimeout {
            get {
                return (int)GetSocketOption(SocketOptionLevel.Socket, SocketOptionName.SendTimeout);
            }

            set {
                if (value< -1) {
                    throw new ArgumentOutOfRangeException("value");
                }
                if (value == -1) {
                    value = 0;
                }

                SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.SendTimeout, value);
            }
        }

        public LingerOption LingerState {
            get {
                return (LingerOption)GetSocketOption(SocketOptionLevel.Socket, SocketOptionName.Linger);
            }
            set {
                SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.Linger, value);
            }
        }

        public bool NoDelay {
            get {
                return (int)GetSocketOption(SocketOptionLevel.Tcp, SocketOptionName.NoDelay) != 0 ? true : false;
            }
            set {
                SetSocketOption(SocketOptionLevel.Tcp, SocketOptionName.NoDelay, value ? 1 : 0);
            }
        }

        public short Ttl{
            get {
                if (addressFamily == AddressFamily.InterNetwork) {
                    return (short)(int)GetSocketOption(SocketOptionLevel.IP, SocketOptionName.IpTimeToLive);
                }
                else if (addressFamily == AddressFamily.InterNetworkV6) {
                    return (short)(int)GetSocketOption(SocketOptionLevel.IPv6, SocketOptionName.IpTimeToLive);
                }
                else{
                    throw new NotSupportedException(SR.GetString(SR.net_invalidversion));
                }
            }

            set {

                // -1 is valid only for IPv6 sockets
                // All numbers < -1 are invalid for all sockets
                //
                if (value < -1 || (value == -1 && addressFamily != AddressFamily.InterNetworkV6)) {
                    throw new ArgumentOutOfRangeException("value");
                }

                if (addressFamily == AddressFamily.InterNetwork) {
                    SetSocketOption(SocketOptionLevel.IP, SocketOptionName.IpTimeToLive, value);
                }

                else if (addressFamily == AddressFamily.InterNetworkV6) {
                    SetSocketOption(SocketOptionLevel.IPv6, SocketOptionName.IpTimeToLive, value);
                }
                else{
                    throw new NotSupportedException(SR.GetString(SR.net_invalidversion));
                }
            }
        }

        public bool DontFragment{
            get {
                if (addressFamily == AddressFamily.InterNetwork) {
                    return (int)GetSocketOption(SocketOptionLevel.IP, SocketOptionName.DontFragment) != 0 ? true : false;
                }
                else{
                    throw new NotSupportedException(SR.GetString(SR.net_invalidversion));
                }
            }

            set {
                if (addressFamily == AddressFamily.InterNetwork) {
                    SetSocketOption(SocketOptionLevel.IP, SocketOptionName.DontFragment, value ? 1 : 0);
                }
                else{
                    throw new NotSupportedException(SR.GetString(SR.net_invalidversion));
                }
            }
        }

        public bool MulticastLoopback{
            get {
                if (addressFamily == AddressFamily.InterNetwork) {
                    return (int)GetSocketOption(SocketOptionLevel.IP, SocketOptionName.MulticastLoopback) != 0 ? true : false;
                }
                else if (addressFamily == AddressFamily.InterNetworkV6) {
                    return (int)GetSocketOption(SocketOptionLevel.IPv6, SocketOptionName.MulticastLoopback) != 0 ? true : false;
                }
                else{
                    throw new NotSupportedException(SR.GetString(SR.net_invalidversion));
                }
            }

            set {
                if (addressFamily == AddressFamily.InterNetwork) {
                    SetSocketOption(SocketOptionLevel.IP, SocketOptionName.MulticastLoopback, value ? 1 : 0);
                }

                else if (addressFamily == AddressFamily.InterNetworkV6) {
                    SetSocketOption(SocketOptionLevel.IPv6, SocketOptionName.MulticastLoopback, value ? 1 : 0);
                }
                else{
                    throw new NotSupportedException(SR.GetString(SR.net_invalidversion));
                }
            }
        }


        public bool EnableBroadcast{
            get {
                return (int)GetSocketOption(SocketOptionLevel.Socket, SocketOptionName.Broadcast) != 0 ? true : false;
            }
            set {
                SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.Broadcast, value ? 1 : 0);
            }
        }



//************* public methods *************************





        /// <devdoc>
        ///    <para>Associates a socket with an end point.</para>
        /// </devdoc>
        public void Bind(EndPoint localEP) {

            if(Logging.On)Logging.Enter(Logging.Sockets, this, "Bind", localEP);

            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            //
            // parameter validation
            //
            if (localEP==null) {
                throw new ArgumentNullException("localEP");
            }

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::Bind() localEP:" + localEP.ToString());

            EndPoint endPointSnapshot = localEP;
            IPEndPoint ipSnapshot = localEP as IPEndPoint;

            //
            // for now security is implemented only on IPEndPoint
            // If EndPoint is of other type - unmanaged code permisison is demanded
            //
            if (ipSnapshot != null)
            {
                // Take a snapshot that will make it immutable and not derived.
                ipSnapshot = ipSnapshot.Snapshot();
                endPointSnapshot = ipSnapshot;

                //
                // create the permissions the user would need for the call
                //
                SocketPermission socketPermission
                    = new SocketPermission(
                        NetworkAccess.Accept,
                        Transport,
                        ipSnapshot.Address.ToString(),
                        ipSnapshot.Port);
                //
                // demand for them
                //
                socketPermission.Demand();

                // Here the permission check has succeded.
                // NB: if local port is 0, then winsock will assign some>1024,
                //     so assuming that this is safe. We will not check the
                //     NetworkAccess.Accept permissions in Receive.
            }
            else {

                ExceptionHelper.UnmanagedPermission.Demand();
            }

            //
            // ask the EndPoint to generate a SocketAddress that we
            // can pass down to winsock
            //
            SocketAddress socketAddress = endPointSnapshot.Serialize();
            DoBind(endPointSnapshot, socketAddress);
            if(Logging.On)Logging.Exit(Logging.Sockets, this, "Bind", "");
        }

        internal void InternalBind(EndPoint localEP)
        {
            if (Logging.On) Logging.Enter(Logging.Sockets, this, "InternalBind", localEP);

            if (CleanedUp)
            {
                throw new ObjectDisposedException(GetType().FullName);
            }

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::InternalBind() localEP:" + localEP.ToString());

            //
            // ask the EndPoint to generate a SocketAddress that we
            // can pass down to winsock
            //
            EndPoint endPointSnapshot = localEP;
            SocketAddress socketAddress = SnapshotAndSerialize(ref endPointSnapshot);
            DoBind(endPointSnapshot, socketAddress);

            if (Logging.On) Logging.Exit(Logging.Sockets, this, "InternalBind", "");
        }

        private void DoBind(EndPoint endPointSnapshot, SocketAddress socketAddress)
        {
            // This may throw ObjectDisposedException.
            SocketError errorCode = UnsafeNclNativeMethods.OSSOCK.bind(
                m_Handle,
                socketAddress.m_Buffer,
                socketAddress.m_Size);


            //
            // if the native call fails we'll throw a SocketException
            //
            if (errorCode != SocketError.Success)
            {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException();
                UpdateStatusAfterSocketError(socketException);
                if (Logging.On) Logging.Exception(Logging.Sockets, this, "DoBind", socketException);
                throw socketException;
            }

            if (m_RightEndPoint == null)
            {
                //
                // save a copy of the EndPoint so we can use it for Create()
                //
                m_RightEndPoint = endPointSnapshot;
            }
        }


        /// <devdoc>
        ///    <para>Establishes a connection to a remote system.</para>
        /// </devdoc>
        public void Connect(EndPoint remoteEP) {
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            //
            // parameter validation
            //
            if (remoteEP==null) {
                throw new ArgumentNullException("remoteEP");
            }

            if(m_IsDisconnected){
                throw new InvalidOperationException(SR.GetString(SR.net_sockets_disconnectedConnect));
            }

            if (isListening)
            {
                throw new InvalidOperationException(SR.GetString(SR.net_sockets_mustnotlisten));
            }

            ValidateBlockingMode();
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::Connect() DST:" + ValidationHelper.ToString(remoteEP));

            //This will check the permissions for connect
            EndPoint endPointSnapshot = remoteEP;
            SocketAddress socketAddress = CheckCacheRemote(ref endPointSnapshot, true);

            DoConnect(endPointSnapshot, socketAddress);
        }


        public void Connect(IPAddress address, int port){

            if(Logging.On)Logging.Enter(Logging.Sockets, this, "Connect", address);

            if (CleanedUp){
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            //if address family isn't the socket address family throw
            if (address==null) {
                throw new ArgumentNullException("address");
            }

            if (!ValidationHelper.ValidateTcpPort(port)) {
                throw new ArgumentOutOfRangeException("port");
            }
            if (addressFamily != address.AddressFamily) {
                throw new NotSupportedException(SR.GetString(SR.net_invalidversion));
            }

            IPEndPoint remoteEP = new IPEndPoint(address, port);
            Connect(remoteEP);
            if(Logging.On)Logging.Exit(Logging.Sockets, this, "Connect", null);
        }


        public void Connect(string host, int port){
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "Connect", host);

            if (CleanedUp){
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            if (host==null) {
                throw new ArgumentNullException("host");
            }
            if (!ValidationHelper.ValidateTcpPort(port)){
                throw new ArgumentOutOfRangeException("port");
            }
            if (addressFamily != AddressFamily.InterNetwork && addressFamily != AddressFamily.InterNetworkV6) {
                throw new NotSupportedException(SR.GetString(SR.net_invalidversion));
            }

            IPAddress[] addresses = Dns.GetHostAddresses(host);
            Connect(addresses,port);
            if(Logging.On)Logging.Exit(Logging.Sockets, this, "Connect", null);
        }

        public void Connect(IPAddress[] addresses, int port){
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "Connect", addresses);

            if (CleanedUp){
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            if (addresses==null) {
                throw new ArgumentNullException("addresses");
            }
            if (addresses.Length == 0) {
                throw new ArgumentException(SR.GetString(SR.net_sockets_invalid_ipaddress_length), "addresses");
            }
            if (!ValidationHelper.ValidateTcpPort(port)) {
                throw new ArgumentOutOfRangeException("port");
            }
            if (addressFamily != AddressFamily.InterNetwork && addressFamily != AddressFamily.InterNetworkV6) {
                throw new NotSupportedException(SR.GetString(SR.net_invalidversion));
            }

            Exception   lastex = null;
            foreach ( IPAddress address in addresses ) {
                if ( address.AddressFamily == addressFamily ) {
                    try
                    {
                        Connect(new IPEndPoint(address,port) );
                        lastex = null;
                        break;
                    }
                    catch ( Exception ex )
                    {
                        if (NclUtilities.IsFatal(ex)) throw;
                        lastex = ex;
                    }
                    catch {
                        lastex = new Exception(SR.GetString(SR.net_nonClsCompliantException));
                    }
                }
            }

            if ( lastex != null )
                throw lastex;

            //if we're not connected, then we didn't get a valid ipaddress in the list
            if (!Connected) {
                throw new ArgumentException(SR.GetString(SR.net_invalidAddressList), "addresses");
            }

            if(Logging.On)Logging.Exit(Logging.Sockets, this, "Connect", null);
        }


        /// <devdoc>
        ///    <para>
        ///       Forces a socket connection to close.
        ///    </para>
        /// </devdoc>
        public void Close()
        {
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::Close() timeout = " + m_CloseTimeout);
            if (Logging.On) Logging.Enter(Logging.Sockets, this, "Close", null);
            ((IDisposable)this).Dispose();
            if (Logging.On) Logging.Exit(Logging.Sockets, this, "Close", null);
        }

        public void Close(int timeout)
        {
            if (timeout < -1)
            {
                throw new ArgumentOutOfRangeException("timeout");
            }
            m_CloseTimeout = timeout;
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::Close() timeout = " + m_CloseTimeout);
            ((IDisposable)this).Dispose();
        }


        /// <devdoc>
        ///    <para>
        ///       Places a socket in a listening state.
        ///    </para>
        /// </devdoc>
        public void Listen(int backlog) {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "Listen", backlog);
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::Listen() backlog:" + backlog.ToString());

            // No access permissions are necessary here because
            // the verification is done for Bind

            // This may throw ObjectDisposedException.
            SocketError errorCode = UnsafeNclNativeMethods.OSSOCK.listen(
                m_Handle,
                backlog);


            //
            // if the native call fails we'll throw a SocketException
            //
            if (errorCode!=SocketError.Success) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException();
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "Listen", socketException);
                throw socketException;
            }
            isListening = true;
            if(Logging.On)Logging.Exit(Logging.Sockets, this, "Listen", "");
        }

        /// <devdoc>
        ///    <para>
        ///       Creates a new <see cref='Sockets.Socket'/> instance to handle an incoming
        ///       connection.
        ///    </para>
        /// </devdoc>
        public Socket Accept() {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "Accept", "");

            //
            // parameter validation
            //

            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            if (m_RightEndPoint==null) {
                throw new InvalidOperationException(SR.GetString(SR.net_sockets_mustbind));
            }

            if(!isListening){
                throw new InvalidOperationException(SR.GetString(SR.net_sockets_mustlisten));
            }

            if(m_IsDisconnected){
                throw new InvalidOperationException(SR.GetString(SR.net_sockets_disconnectedAccept));
            }

            ValidateBlockingMode();
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::Accept() SRC:" + ValidationHelper.ToString(LocalEndPoint));

            SocketAddress socketAddress = m_RightEndPoint.Serialize();

            // This may throw ObjectDisposedException.
            SafeCloseSocket acceptedSocketHandle = SafeCloseSocket.Accept(
                m_Handle,
                socketAddress.m_Buffer,
                ref socketAddress.m_Size);

            //
            // if the native call fails we'll throw a SocketException
            //
            if (acceptedSocketHandle.IsInvalid) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException();
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "Accept", socketException);
                throw socketException;
            }

            Socket socket = CreateAcceptSocket(acceptedSocketHandle, m_RightEndPoint.Create(socketAddress), false);
            if(Logging.On)Logging.Exit(Logging.Sockets, this, "Accept", socket);
            return socket;
        }


        /// <devdoc>
        ///    <para>Sends a data buffer to a connected socket.</para>
        /// </devdoc>
        public int Send(byte[] buffer, int size, SocketFlags socketFlags) {
            return Send(buffer, 0, size, socketFlags);
        }
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int Send(byte[] buffer, SocketFlags socketFlags) {
            return Send(buffer, 0, buffer!=null ? buffer.Length : 0, socketFlags);
        }
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int Send(byte[] buffer) {
            return Send(buffer, 0, buffer!=null ? buffer.Length : 0, SocketFlags.None);
        }



        /// <devdoc>
        ///    <para>Sends data to
        ///       a connected socket, starting at the indicated location in the
        ///       data.</para>
        /// </devdoc>


        public int Send(byte[] buffer, int offset, int size, SocketFlags socketFlags) {
            SocketError errorCode;
            int bytesTransferred = Send(buffer, offset, size, socketFlags, out errorCode);
            if(errorCode != SocketError.Success){
                throw new SocketException(errorCode);
            }
            return bytesTransferred;
        }



        public int Send(byte[] buffer, int offset, int size, SocketFlags socketFlags, out SocketError errorCode) {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "Send", "");

            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            //
            // parameter validation
            //
            if (buffer==null) {
                throw new ArgumentNullException("buffer");
            }
            if (offset<0 || offset>buffer.Length) {
                throw new ArgumentOutOfRangeException("offset");
            }
            if (size<0 || size>buffer.Length-offset) {
                throw new ArgumentOutOfRangeException("size");
            }


            errorCode = SocketError.Success;
            ValidateBlockingMode();
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::Send() SRC:" + ValidationHelper.ToString(LocalEndPoint) + " DST:" + ValidationHelper.ToString(RemoteEndPoint) + " size:" + size);

            // This can throw ObjectDisposedException.
            int bytesTransferred;
            unsafe {
                if (buffer.Length == 0)
                    bytesTransferred = UnsafeNclNativeMethods.OSSOCK.send(m_Handle.DangerousGetHandle(), null, 0, socketFlags);
                else{
                    fixed (byte* pinnedBuffer = buffer) {
                        bytesTransferred = UnsafeNclNativeMethods.OSSOCK.send(
                                        m_Handle.DangerousGetHandle(),
                                        pinnedBuffer+offset,
                                        size,
                                        socketFlags);
                    }
                }
            }

            //
            // if the native call fails we'll throw a SocketException
            //
            if ((SocketError)bytesTransferred==SocketError.SocketError) {
                //
                // update our internal state after this socket error and throw
                //
                errorCode = (SocketError)Marshal.GetLastWin32Error();
                UpdateStatusAfterSocketError(errorCode);
                if(Logging.On){
                    Logging.Exception(Logging.Sockets, this, "Send", new SocketException(errorCode));
                    Logging.Exit(Logging.Sockets, this, "Send", 0);
                }
                return 0;
            }


            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::Send() UnsafeNclNativeMethods.OSSOCK.send returns:" + bytesTransferred.ToString());

            if(Logging.On)Logging.Dump(Logging.Sockets, this, "Send", buffer, offset, size);
            if(Logging.On)Logging.Exit(Logging.Sockets, this, "Send", bytesTransferred);
            return bytesTransferred;
        }


        /// <devdoc>
        ///    <para>Sends data to a specific end point, starting at the indicated location in the
        ///       data.</para>
        /// </devdoc>
        public int SendTo(byte[] buffer, int offset, int size, SocketFlags socketFlags, EndPoint remoteEP) {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "SendTo", "");
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            //
            // parameter validation
            //
            if (buffer==null) {
                throw new ArgumentNullException("buffer");
            }
            if (remoteEP==null) {
                throw new ArgumentNullException("remoteEP");
            }
            if (offset<0 || offset>buffer.Length) {
                throw new ArgumentOutOfRangeException("offset");
            }
            if (size<0 || size>buffer.Length-offset) {
                throw new ArgumentOutOfRangeException("size");
            }

            ValidateBlockingMode();
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::SendTo() SRC:" + ValidationHelper.ToString(LocalEndPoint) + " size:" + size + " remoteEP:" + ValidationHelper.ToString(remoteEP));

            //That will check ConnectPermission for remoteEP
            EndPoint endPointSnapshot = remoteEP;
            SocketAddress socketAddress = CheckCacheRemote(ref endPointSnapshot, false);

            // This can throw ObjectDisposedException.
            int bytesTransferred;
            unsafe
            {
                if (buffer.Length == 0)
                {
                    bytesTransferred = UnsafeNclNativeMethods.OSSOCK.sendto(
                        m_Handle.DangerousGetHandle(),
                        null,
                        0,
                        socketFlags,
                        socketAddress.m_Buffer,
                        socketAddress.m_Size);
                }
                else
                {
                    fixed (byte* pinnedBuffer = buffer)
                    {
                        bytesTransferred = UnsafeNclNativeMethods.OSSOCK.sendto(
                            m_Handle.DangerousGetHandle(),
                            pinnedBuffer+offset,
                            size,
                            socketFlags,
                            socketAddress.m_Buffer,
                            socketAddress.m_Size);
                    }
                }
            }

            //
            // if the native call fails we'll throw a SocketException
            //
            if ((SocketError)bytesTransferred==SocketError.SocketError) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException();
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "SendTo", socketException);
                throw socketException;
            }

            if (m_RightEndPoint==null) {
                //
                // save a copy of the EndPoint so we can use it for Create()
                //
                m_RightEndPoint = endPointSnapshot;
            }


            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::SendTo() returning bytesTransferred:" + bytesTransferred.ToString());
            if(Logging.On)Logging.Dump(Logging.Sockets, this, "SendTo", buffer, offset, size);
            if(Logging.On)Logging.Exit(Logging.Sockets, this, "SendTo", bytesTransferred);
            return bytesTransferred;
        }

        /// <devdoc>
        ///    <para>Sends data to a specific end point, starting at the indicated location in the data.</para>
        /// </devdoc>
        public int SendTo(byte[] buffer, int size, SocketFlags socketFlags, EndPoint remoteEP) {
            return SendTo(buffer, 0, size, socketFlags, remoteEP);
        }
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int SendTo(byte[] buffer, SocketFlags socketFlags, EndPoint remoteEP) {
            return SendTo(buffer, 0, buffer!=null ? buffer.Length : 0, socketFlags, remoteEP);
        }
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int SendTo(byte[] buffer, EndPoint remoteEP) {
            return SendTo(buffer, 0, buffer!=null ? buffer.Length : 0, SocketFlags.None, remoteEP);
        }


        /// <devdoc>
        ///    <para>Receives data from a connected socket.</para>
        /// </devdoc>
        public int Receive(byte[] buffer, int size, SocketFlags socketFlags) {
            return Receive(buffer, 0, size, socketFlags);
        }
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int Receive(byte[] buffer, SocketFlags socketFlags) {
            return Receive(buffer, 0, buffer!=null ? buffer.Length : 0, socketFlags);
        }
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int Receive(byte[] buffer) {
            return Receive(buffer, 0, buffer!=null ? buffer.Length : 0, SocketFlags.None);
        }

        /// <devdoc>
        ///    <para>Receives data from a connected socket into a specific location of the receive
        ///       buffer.</para>
        /// </devdoc>

        public int Receive(byte[] buffer, int offset, int size, SocketFlags socketFlags) {
            SocketError errorCode;
            int bytesTransferred = Receive(buffer, offset, size, socketFlags, out errorCode);
            if(errorCode != SocketError.Success){
                throw new SocketException(errorCode);
            }
            return bytesTransferred;
        }


        public int Receive(byte[] buffer, int offset, int size, SocketFlags socketFlags, out SocketError errorCode) {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "Receive", "");
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            //
            // parameter validation
            //

            if (buffer==null) {
                throw new ArgumentNullException("buffer");
            }
            if (offset<0 || offset>buffer.Length) {
                throw new ArgumentOutOfRangeException("offset");
            }
            if (size<0 || size>buffer.Length-offset) {
                throw new ArgumentOutOfRangeException("size");
            }


            ValidateBlockingMode();
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::Receive() SRC:" + ValidationHelper.ToString(LocalEndPoint) + " DST:" + ValidationHelper.ToString(RemoteEndPoint) + " size:" + size);

            // This can throw ObjectDisposedException.
            int bytesTransferred;
            errorCode = SocketError.Success;
            unsafe {
                if (buffer.Length == 0)
                {
                    bytesTransferred = UnsafeNclNativeMethods.OSSOCK.recv(m_Handle.DangerousGetHandle(), null, 0, socketFlags);
                }
                else fixed (byte* pinnedBuffer = buffer) {
                    bytesTransferred = UnsafeNclNativeMethods.OSSOCK.recv(m_Handle.DangerousGetHandle(), pinnedBuffer+offset, size, socketFlags);
                }
            }

            if ((SocketError)bytesTransferred==SocketError.SocketError) {
                //
                // update our internal state after this socket error and throw
                //
                errorCode = (SocketError)Marshal.GetLastWin32Error();
                UpdateStatusAfterSocketError(errorCode);
                if(Logging.On){
                    Logging.Exception(Logging.Sockets, this, "Receive", new SocketException(errorCode));
                    Logging.Exit(Logging.Sockets, this, "Receive", 0);
                }
                return 0;
            }



            if(Logging.On)Logging.Dump(Logging.Sockets, this, "Receive", buffer, offset, bytesTransferred);
            if(Logging.On)Logging.Exit(Logging.Sockets, this, "Receive", bytesTransferred);

            return bytesTransferred;
        }


        public int Receive(IList<ArraySegment<byte>> buffers) {
            return Receive(buffers,SocketFlags.None);
        }


        [CLSCompliant(false)]
        public int Receive(IList<ArraySegment<byte>> buffers, SocketFlags socketFlags) {
            SocketError errorCode;
            int bytesTransferred = Receive(buffers, socketFlags, out errorCode);
            if(errorCode != SocketError.Success){
                throw new SocketException(errorCode);
            }
            return bytesTransferred;
        }

        [CLSCompliant(false)]
        public int Receive(IList<ArraySegment<byte>> buffers, SocketFlags socketFlags, out SocketError errorCode) {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "Receive", "");
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            if (buffers==null) {
               throw new ArgumentNullException("buffers");
            }

            if(buffers.Count == 0){
                throw new ArgumentException(SR.GetString(SR.net_sockets_zerolist,"buffers"), "buffers");
            }


            ValidateBlockingMode();
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::Receive() SRC:" + ValidationHelper.ToString(LocalEndPoint) + " DST:" + ValidationHelper.ToString(RemoteEndPoint));

            //make sure we don't let the app mess up the buffer array enough to cause
            //corruption.
            int count = buffers.Count;
            WSABuffer[] WSABuffers = new WSABuffer[count];
            GCHandle[] objectsToPin = null;
            int bytesTransferred;
            errorCode = SocketError.Success;

            try {
                objectsToPin = new GCHandle[count];
                for (int i = 0; i < count; ++i)
                {
                    ArraySegment<byte> buffer = buffers[i];
                    objectsToPin[i] = GCHandle.Alloc(buffer.Array, GCHandleType.Pinned);
                    WSABuffers[i].Length = buffer.Count;
                    WSABuffers[i].Pointer = Marshal.UnsafeAddrOfPinnedArrayElement(buffer.Array, buffer.Offset);
                }

                // This can throw ObjectDisposedException.
                errorCode = UnsafeNclNativeMethods.OSSOCK.WSARecv_Blocking(
                    m_Handle.DangerousGetHandle(),
                    WSABuffers,
                    buffers.Count,
                    out bytesTransferred,
                    ref socketFlags,
                    IntPtr.Zero,
                    IntPtr.Zero );

                if ((SocketError)errorCode==SocketError.SocketError) {
                    errorCode = (SocketError)Marshal.GetLastWin32Error();
                }
            }
            finally {
                if (objectsToPin != null)
                    for (int i = 0; i < objectsToPin.Length; ++i)
                        if (objectsToPin[i].IsAllocated)
                            objectsToPin[i].Free();
            }

            if (errorCode != SocketError.Success) {
                //
                // update our internal state after this socket error and throw
                //
                UpdateStatusAfterSocketError(errorCode);
                if(Logging.On){
                    Logging.Exception(Logging.Sockets, this, "Receive", new SocketException(errorCode));
                    Logging.Exit(Logging.Sockets, this, "Receive", 0);
                }
                return 0;
            }





            if(Logging.On)Logging.Exit(Logging.Sockets, this, "Receive", bytesTransferred);

            return bytesTransferred;
        }




        /// <devdoc>
        ///    <para>Receives a datagram into a specific location in the data buffer and stores
        ///       the end point.</para>
        /// </devdoc>
        public int ReceiveMessageFrom(byte[] buffer, int offset, int size, ref SocketFlags socketFlags, ref EndPoint remoteEP, out IPPacketInformation ipPacketInformation) {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "ReceiveMessageFrom", "");

            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            if (buffer==null) {
                throw new ArgumentNullException("buffer");
            }
            if (remoteEP==null) {
                throw new ArgumentNullException("remoteEP");
            }
            if (offset<0 || offset>buffer.Length) {
                throw new ArgumentOutOfRangeException("offset");
            }
            if (size<0 || size>buffer.Length-offset) {
                throw new ArgumentOutOfRangeException("size");
            }
            if (m_RightEndPoint==null) {
                throw new InvalidOperationException(SR.GetString(SR.net_sockets_mustbind));
            }


            ValidateBlockingMode();

            // This will check the permissions for connect.
            EndPoint endPointSnapshot = remoteEP;
            SocketAddress socketAddress = CheckCacheRemote(ref endPointSnapshot, false);

            ReceiveMessageOverlappedAsyncResult asyncResult = new ReceiveMessageOverlappedAsyncResult(this,null,null);
            asyncResult.SetUnmanagedStructures(buffer, offset, size, socketAddress, socketFlags);

            // save a copy of the original EndPoint
            SocketAddress socketAddressOriginal = endPointSnapshot.Serialize();

            //setup structure
            int bytesTransfered = 0;
            SocketError errorCode = SocketError.Success;

            if(addressFamily == AddressFamily.InterNetwork) {
                SetSocketOption(SocketOptionLevel.IP,SocketOptionName.PacketInformation,true);
            }
            else if (addressFamily == AddressFamily.InterNetworkV6){
                SetSocketOption(SocketOptionLevel.IPv6,SocketOptionName.PacketInformation,true);
            }

            try
            {
                // This can throw ObjectDisposedException (retrieving the delegate AND resolving the handle).
                if (WSARecvMsg_Blocking(
                    m_Handle.DangerousGetHandle(),
                    Marshal.UnsafeAddrOfPinnedArrayElement(asyncResult.m_MessageBuffer,0),
                    out bytesTransfered,
                    IntPtr.Zero,
                    IntPtr.Zero) == SocketError.SocketError)
                {
                    errorCode =  (SocketError)Marshal.GetLastWin32Error();
                }
            }
            finally {
                asyncResult.SyncReleaseUnmanagedStructures();
            }


            //
            // if the native call fails we'll throw a SocketException
            //
            if (errorCode!=SocketError.Success && errorCode != SocketError.MessageSize) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException(errorCode);
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "ReceiveMessageFrom", socketException);
                throw socketException;
            }


            if (!socketAddressOriginal.Equals(asyncResult.m_SocketAddress))
            {
                try {
                    remoteEP = endPointSnapshot.Create(asyncResult.m_SocketAddress);
                }
                catch {
                }
                if (m_RightEndPoint==null) {
                    //
                    // save a copy of the EndPoint so we can use it for Create()
                    //
                    m_RightEndPoint = endPointSnapshot;
                }
            }

            socketFlags = asyncResult.m_flags;
            ipPacketInformation = asyncResult.m_IPPacketInformation;

            if(Logging.On)Logging.Exit(Logging.Sockets, this, "ReceiveMessageFrom", errorCode);
            return bytesTransfered;
        }

        /// <devdoc>
        ///    <para>Receives a datagram into a specific location in the data buffer and stores
        ///       the end point.</para>
        /// </devdoc>
        public int ReceiveFrom(byte[] buffer, int offset, int size, SocketFlags socketFlags, ref EndPoint remoteEP) {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "ReceiveFrom", "");
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            //
            // parameter validation
            //
            if (buffer==null) {
                throw new ArgumentNullException("buffer");
            }
            if (remoteEP==null) {
                throw new ArgumentNullException("remoteEP");
            }
            if (offset<0 || offset>buffer.Length) {
                throw new ArgumentOutOfRangeException("offset");
            }
            if (size<0 || size>buffer.Length-offset) {
                throw new ArgumentOutOfRangeException("size");
            }
            if (m_RightEndPoint==null) {
                throw new InvalidOperationException(SR.GetString(SR.net_sockets_mustbind));
            }


            ValidateBlockingMode();
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::ReceiveFrom() SRC:" + ValidationHelper.ToString(LocalEndPoint) + " size:" + size + " remoteEP:" + remoteEP.ToString());

            // This will check the permissions for connect.
            // We need this because remoteEP may differ from one used in Connect or
            // there was no Connect called.
            EndPoint endPointSnapshot = remoteEP;
            SocketAddress socketAddress = CheckCacheRemote(ref endPointSnapshot, false);
            SocketAddress socketAddressOriginal = endPointSnapshot.Serialize();

            // This can throw ObjectDisposedException.
            int bytesTransferred;
            unsafe {
                if (buffer.Length == 0)
                    bytesTransferred = UnsafeNclNativeMethods.OSSOCK.recvfrom(m_Handle.DangerousGetHandle(), null, 0, socketFlags, socketAddress.m_Buffer, ref socketAddress.m_Size );
                else fixed (byte* pinnedBuffer = buffer) {
                    bytesTransferred = UnsafeNclNativeMethods.OSSOCK.recvfrom(m_Handle.DangerousGetHandle(), pinnedBuffer+offset, size, socketFlags, socketAddress.m_Buffer, ref socketAddress.m_Size );
                }
            }

            // If the native call fails we'll throw a SocketException.
            // Must do this immediately after the native call so that the SocketException() constructor can pick up the error code.
            SocketException socketException = null;
            if ((SocketError) bytesTransferred == SocketError.SocketError)
            {
                socketException = new SocketException();
                UpdateStatusAfterSocketError(socketException);
                if (Logging.On) Logging.Exception(Logging.Sockets, this, "ReceiveFrom", socketException);
                
                if(socketException.ErrorCode != (int)SocketError.MessageSize){
                    throw socketException;
                }
            }

            if (!socketAddressOriginal.Equals(socketAddress)) {
                try {
                    remoteEP = endPointSnapshot.Create(socketAddress);
                }
                catch {
                }
                if (m_RightEndPoint==null) {
                    //
                    // save a copy of the EndPoint so we can use it for Create()
                    //
                    m_RightEndPoint = endPointSnapshot;
                }
            }

            if(socketException != null){
                throw socketException;
            }



            if(Logging.On)Logging.Dump(Logging.Sockets, this, "ReceiveFrom", buffer, offset, size);
            if(Logging.On)Logging.Exit(Logging.Sockets, this, "ReceiveFrom", bytesTransferred);
            return bytesTransferred;
        }



        /// <devdoc>
        ///    <para>Receives a datagram and stores the source end point.</para>
        /// </devdoc>
        public int ReceiveFrom(byte[] buffer, int size, SocketFlags socketFlags, ref EndPoint remoteEP) {
            return ReceiveFrom(buffer, 0, size, socketFlags, ref remoteEP);
        }
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int ReceiveFrom(byte[] buffer, SocketFlags socketFlags, ref EndPoint remoteEP) {
            return ReceiveFrom(buffer, 0, buffer!=null ? buffer.Length : 0, socketFlags, ref remoteEP);
        }
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int ReceiveFrom(byte[] buffer, ref EndPoint remoteEP) {
            return ReceiveFrom(buffer, 0, buffer!=null ? buffer.Length : 0, SocketFlags.None, ref remoteEP);
        }

        // UE
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int IOControl(int ioControlCode, byte[] optionInValue, byte[] optionOutValue) {
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            if (ioControlCode==IoctlSocketConstants.FIONBIO) {
                throw new InvalidOperationException(SR.GetString(SR.net_sockets_useblocking));
            }

            ExceptionHelper.UnmanagedPermission.Demand();

            int realOptionLength = 0;

            // This can throw ObjectDisposedException.
            SocketError errorCode = UnsafeNclNativeMethods.OSSOCK.WSAIoctl_Blocking(
                m_Handle.DangerousGetHandle(),
                ioControlCode,
                optionInValue,
                optionInValue!=null ? optionInValue.Length : 0,
                optionOutValue,
                optionOutValue!=null ? optionOutValue.Length : 0,
                out realOptionLength,
                IntPtr.Zero,
                IntPtr.Zero);

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::IOControl() UnsafeNclNativeMethods.OSSOCK.WSAIoctl returns errorCode:" + errorCode);

            //
            // if the native call fails we'll throw a SocketException
            //
            if (errorCode==SocketError.SocketError) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException();
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "IOControl", socketException);
                throw socketException;
            }

            return realOptionLength;
        }

        // UE
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int IOControl(IOControlCode ioControlCode, byte[] optionInValue, byte[] optionOutValue) {
            return IOControl(unchecked((int)ioControlCode),optionInValue,optionOutValue);
        }




        /// <devdoc>
        ///    <para>
        ///       Sets the specified option to the specified value.
        ///    </para>
        /// </devdoc>
        public void SetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, int optionValue) {
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            CheckSetOptionPermissions(optionLevel, optionName);
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::SetSocketOption(): optionLevel:" + optionLevel.ToString() + " optionName:" + optionName.ToString() + " optionValue:" + optionValue.ToString());
            SetSocketOption(optionLevel, optionName, optionValue, false);
        }


        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void SetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, byte[] optionValue) {
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            CheckSetOptionPermissions(optionLevel, optionName);

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::SetSocketOption(): optionLevel:" + optionLevel.ToString() + " optionName:" + optionName.ToString() + " optionValue:" + optionValue.ToString());

            // This can throw ObjectDisposedException.
            SocketError errorCode = UnsafeNclNativeMethods.OSSOCK.setsockopt(
                m_Handle,
                optionLevel,
                optionName,
                optionValue,
                optionValue != null ? optionValue.Length : 0);

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::SetSocketOption() UnsafeNclNativeMethods.OSSOCK.setsockopt returns errorCode:" + errorCode);

            //
            // if the native call fails we'll throw a SocketException
            //
            if (errorCode==SocketError.SocketError) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException();
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "SetSocketOption", socketException);
                throw socketException;
            }
        }

        /// <devdoc>
        ///    <para>Sets the specified option to the specified value.</para>
        /// </devdoc>

        public void SetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, bool optionValue) {
            SetSocketOption(optionLevel,optionName,(optionValue?1:0));
        }

        /// <devdoc>
        ///    <para>Sets the specified option to the specified value.</para>
        /// </devdoc>
        public void SetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, object optionValue) {
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            //
            // parameter validation
            //
            if (optionValue==null) {
                throw new ArgumentNullException("optionValue");
            }

            CheckSetOptionPermissions(optionLevel, optionName);

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::SetSocketOption(): optionLevel:" + optionLevel.ToString() + " optionName:" + optionName.ToString() + " optionValue:" + optionValue.ToString());

            if (optionLevel==SocketOptionLevel.Socket && optionName==SocketOptionName.Linger) {
                LingerOption lingerOption = optionValue as LingerOption;
                if (lingerOption==null) {
                    throw new ArgumentException(SR.GetString(SR.net_sockets_invalid_optionValue, "LingerOption"), "optionValue");
                }
                if (lingerOption.LingerTime < 0 || lingerOption.LingerTime>(int)UInt16.MaxValue) {
                    throw new ArgumentException(SR.GetString(SR.ArgumentOutOfRange_Bounds_Lower_Upper, 0, (int)UInt16.MaxValue), "optionValue.LingerTime");
                }
                setLingerOption(lingerOption);
            }
            else if (optionLevel==SocketOptionLevel.IP && (optionName==SocketOptionName.AddMembership || optionName==SocketOptionName.DropMembership)) {
                MulticastOption multicastOption = optionValue as MulticastOption;
                if (multicastOption==null) {
                    throw new ArgumentException(SR.GetString(SR.net_sockets_invalid_optionValue, "MulticastOption"), "optionValue");
                }
                setMulticastOption(optionName, multicastOption);
            }
            //
            // IPv6 Changes: Handle IPv6 Multicast Add / Drop
            //
            else if (optionLevel==SocketOptionLevel.IPv6 && (optionName==SocketOptionName.AddMembership || optionName==SocketOptionName.DropMembership)) {
                IPv6MulticastOption multicastOption = optionValue as IPv6MulticastOption;
                if (multicastOption==null) {
                    throw new ArgumentException(SR.GetString(SR.net_sockets_invalid_optionValue, "IPv6MulticastOption"), "optionValue");
                }
                setIPv6MulticastOption(optionName, multicastOption);
            }
            else {
                throw new ArgumentException(SR.GetString(SR.net_sockets_invalid_optionValue_all), "optionValue");
            }
        }


        /// <devdoc>
        ///    <para>
        ///       Gets the value of a socket option.
        ///    </para>
        /// </devdoc>
        // UE
        public object GetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName) {
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            if (optionLevel==SocketOptionLevel.Socket && optionName==SocketOptionName.Linger) {
                return getLingerOpt();
            }
            else if (optionLevel==SocketOptionLevel.IP && (optionName==SocketOptionName.AddMembership || optionName==SocketOptionName.DropMembership)) {
                return getMulticastOpt(optionName);
            }
            //
            // Handle IPv6 case
            //
            else if (optionLevel==SocketOptionLevel.IPv6 && (optionName==SocketOptionName.AddMembership || optionName==SocketOptionName.DropMembership)) {
                return getIPv6MulticastOpt(optionName);
            }
            else {
                int optionValue = 0;
                int optionLength = 4;

                // This can throw ObjectDisposedException.
                SocketError errorCode = UnsafeNclNativeMethods.OSSOCK.getsockopt(
                    m_Handle,
                    optionLevel,
                    optionName,
                    out optionValue,
                    ref optionLength);

                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::GetSocketOption() UnsafeNclNativeMethods.OSSOCK.getsockopt returns errorCode:" + errorCode);

                //
                // if the native call fails we'll throw a SocketException
                //
                if (errorCode==SocketError.SocketError) {
                    //
                    // update our internal state after this socket error and throw
                    //
                    SocketException socketException = new SocketException();
                    UpdateStatusAfterSocketError(socketException);
                    if(Logging.On)Logging.Exception(Logging.Sockets, this, "GetSocketOption", socketException);
                    throw socketException;
                }

                return optionValue;
            }
        }

        // UE
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void GetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, byte[] optionValue) {
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            int optionLength = optionValue!=null ? optionValue.Length : 0;

            // This can throw ObjectDisposedException.
            SocketError errorCode = UnsafeNclNativeMethods.OSSOCK.getsockopt(
                m_Handle,
                optionLevel,
                optionName,
                optionValue,
                ref optionLength);

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::GetSocketOption() UnsafeNclNativeMethods.OSSOCK.getsockopt returns errorCode:" + errorCode);

            //
            // if the native call fails we'll throw a SocketException
            //
            if (errorCode==SocketError.SocketError) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException();
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "GetSocketOption", socketException);
                throw socketException;
            }
        }

        // UE
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public byte[] GetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, int optionLength) {
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            byte[] optionValue = new byte[optionLength];
            int realOptionLength = optionLength;

            // This can throw ObjectDisposedException.
            SocketError errorCode = UnsafeNclNativeMethods.OSSOCK.getsockopt(
                m_Handle,
                optionLevel,
                optionName,
                optionValue,
                ref realOptionLength);

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::GetSocketOption() UnsafeNclNativeMethods.OSSOCK.getsockopt returns errorCode:" + errorCode);

            //
            // if the native call fails we'll throw a SocketException
            //
            if (errorCode==SocketError.SocketError) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException();
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "GetSocketOption", socketException);
                throw socketException;
            }

            if (optionLength!=realOptionLength) {
                byte[] newOptionValue = new byte[realOptionLength];
                Buffer.BlockCopy(optionValue, 0, newOptionValue, 0, realOptionLength);
                optionValue = newOptionValue;
            }

            return optionValue;
        }


        /// <devdoc>
        ///    <para>
        ///       Determines the status of the socket.
        ///    </para>
        /// </devdoc>
        public bool Poll(int microSeconds, SelectMode mode) {
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            IntPtr handle = m_Handle.DangerousGetHandle();
            IntPtr[] fileDescriptorSet = new IntPtr[2] { (IntPtr) 1, handle };
            TimeValue IOwait = new TimeValue();

            //
            // negative timeout value implies indefinite wait
            //
            int socketCount;
            if (microSeconds != -1) {
                MicrosecondsToTimeValue((long)(uint)microSeconds, ref IOwait);
                socketCount =
                    UnsafeNclNativeMethods.OSSOCK.select(
                        0,
                        mode==SelectMode.SelectRead ? fileDescriptorSet : null,
                        mode==SelectMode.SelectWrite ? fileDescriptorSet : null,
                        mode==SelectMode.SelectError ? fileDescriptorSet : null,
                        ref IOwait);
            }
            else {
                socketCount =
                    UnsafeNclNativeMethods.OSSOCK.select(
                        0,
                        mode==SelectMode.SelectRead ? fileDescriptorSet : null,
                        mode==SelectMode.SelectWrite ? fileDescriptorSet : null,
                        mode==SelectMode.SelectError ? fileDescriptorSet : null,
                        IntPtr.Zero);
            }
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::Poll() UnsafeNclNativeMethods.OSSOCK.select returns socketCount:" + socketCount);

            //
            // if the native call fails we'll throw a SocketException
            //
            if ((SocketError)socketCount==SocketError.SocketError) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException();
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "Poll", socketException);
                throw socketException;
            }
            if ((int)fileDescriptorSet[0]==0) {
                return false;
            }
            return fileDescriptorSet[1] == handle;
        }

        /// <devdoc>
        ///    <para>Determines the status of a socket.</para>
        /// </devdoc>
        public static void Select(IList checkRead, IList checkWrite, IList checkError, int microSeconds) {
            // parameter validation
            if ((checkRead==null || checkRead.Count==0) && (checkWrite==null || checkWrite.Count==0) && (checkError==null || checkError.Count==0)) {
                throw new ArgumentNullException(SR.GetString(SR.net_sockets_empty_select));
            }
            const int MaxSelect = 65536;
            if (checkRead!=null && checkRead.Count>MaxSelect) {
                throw new ArgumentOutOfRangeException(SR.GetString(SR.net_sockets_toolarge_select, "checkRead", MaxSelect.ToString(NumberFormatInfo.CurrentInfo)));
            }
            if (checkWrite!=null && checkWrite.Count>MaxSelect) {
                throw new ArgumentOutOfRangeException(SR.GetString(SR.net_sockets_toolarge_select, "checkWrite", MaxSelect.ToString(NumberFormatInfo.CurrentInfo)));
            }
            if (checkError!=null && checkError.Count>MaxSelect) {
                throw new ArgumentOutOfRangeException(SR.GetString(SR.net_sockets_toolarge_select, "checkError", MaxSelect.ToString(NumberFormatInfo.CurrentInfo)));
            }
            IntPtr[] readfileDescriptorSet   = SocketListToFileDescriptorSet(checkRead);
            IntPtr[] writefileDescriptorSet  = SocketListToFileDescriptorSet(checkWrite);
            IntPtr[] errfileDescriptorSet    = SocketListToFileDescriptorSet(checkError);

            TimeValue IOwait = new TimeValue();

            // if negative, assume infinite
            if (microSeconds != -1) {
                MicrosecondsToTimeValue((long)(uint)microSeconds, ref IOwait);
            }

            int socketCount =
                UnsafeNclNativeMethods.OSSOCK.select(
                    0, // ignored value
                    readfileDescriptorSet,
                    writefileDescriptorSet,
                    errfileDescriptorSet,
                    ref IOwait);

            GlobalLog.Print("Socket::Select() UnsafeNclNativeMethods.OSSOCK.select returns socketCount:" + socketCount);

            //
            // if the native call fails we'll throw a SocketException
            //
            if ((SocketError)socketCount==SocketError.SocketError) {
                throw new SocketException();
            }
            SelectFileDescriptor(checkRead, readfileDescriptorSet);
            SelectFileDescriptor(checkWrite, writefileDescriptorSet);
            SelectFileDescriptor(checkError, errfileDescriptorSet);
        }



        //
        // Async Winsock Support, the following functions use either
        //   the Async Winsock support to do overlapped I/O WSASend/WSARecv
        //   or a WSAEventSelect call to enable selection and non-blocking mode
        //   of otherwise normal Winsock calls.
        //
        //   Currently the following Async Socket calls are supported:
        //      Send, Recv, SendTo, RecvFrom, Connect, Accept
        //

        /*++

        Routine Description:

           BeginConnect - Does a async winsock connect, by calling
           WSAEventSelect to enable Connect Events to signal an event and
           wake up a callback which involkes a callback.

            So note: This routine may go pending at which time,
            but any case the callback Delegate will be called upon completion

        Arguments:

           remoteEP - status line that we wish to parse
           Callback - Async Callback Delegate that is called upon Async Completion
           State - State used to track callback, set by caller, not required

        Return Value:

           IAsyncResult - Async result used to retreive result

        --*/


        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [HostProtection(ExternalThreading=true)]
        public IAsyncResult BeginConnect(EndPoint remoteEP, AsyncCallback callback, object state)
        {
            //
            //  parameter validation
            //
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "BeginConnect", remoteEP);

            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            if (remoteEP==null) {
                throw new ArgumentNullException("remoteEP");
            }

            if (isListening)
            {
                throw new InvalidOperationException(SR.GetString(SR.net_sockets_mustnotlisten));
            }

            if (CanUseConnectEx(remoteEP))
            {
                return BeginConnectEx(remoteEP, true, callback, state);
            }

            // This will check the permissions for connect.
            EndPoint endPointSnapshot = remoteEP;
            SocketAddress socketAddress = CheckCacheRemote(ref endPointSnapshot, true);

            // Flow the context.  No need to lock it since we don't use it until the callback.
            ConnectAsyncResult asyncResult = new ConnectAsyncResult(this, state, callback);
            asyncResult.StartPostingAsyncOp(false);

            // Post the connect.
            DoBeginConnect(endPointSnapshot, socketAddress, asyncResult);

            // We didn't throw, so finish the posting op.  This will call the callback if the operation already completed.
            asyncResult.FinishPostingAsyncOp(ref Caches.ConnectClosureCache);

            if(Logging.On)Logging.Exit(Logging.Sockets, this, "BeginConnect", asyncResult);
            return asyncResult;
        }




        public SocketInformation DuplicateAndClose(int targetProcessId){
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "DuplicateAndClose", null);

            if (CleanedUp)
            {
                throw new ObjectDisposedException(GetType().FullName);
            }

            ExceptionHelper.UnrestrictedSocketPermission.Demand();

            SocketInformation info = new SocketInformation();
            info.ProtocolInformation = new byte[protocolInformationSize];

            // This can throw ObjectDisposedException.
            SocketError errorCode;
            // ROTORTODO
            errorCode = SocketError.SocketError;

            if (errorCode!=SocketError.Success) {
                SocketException socketException = new SocketException();
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "DuplicateAndClose", socketException);
                throw socketException;
            }


            info.IsConnected = Connected;
            info.IsNonBlocking = !Blocking;
            info.IsListening = isListening;
            info.UseOnlyOverlappedIO = UseOnlyOverlappedIO;

            //make sure we don't shutdown, etc.
            Close(-1);

            if(Logging.On)Logging.Exit(Logging.Sockets, this, "DuplicateAndClose", null);
            return info;
        }




        internal IAsyncResult UnsafeBeginConnect(EndPoint remoteEP, AsyncCallback callback, object state)
        {
            if (CanUseConnectEx(remoteEP))
            {
                return BeginConnectEx(remoteEP, false, callback, state);
            }
            EndPoint endPointSnapshot = remoteEP;
            SocketAddress socketAddress = SnapshotAndSerialize(ref endPointSnapshot);

            // No context flow here.  Can use Lazy.
            ConnectAsyncResult asyncResult = new ConnectAsyncResult(this, state, callback);
            DoBeginConnect(endPointSnapshot, socketAddress, asyncResult);
            return asyncResult;
        }

        // Leaving the public logging as "BeginConnect" since that makes sense to the people looking at the output.
        // Private logging can remain "DoBeginConnect".
        private void DoBeginConnect(EndPoint endPointSnapshot, SocketAddress socketAddress, LazyAsyncResult asyncResult)
        {
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::DoBeginConnect() endPointSnapshot:" + endPointSnapshot.ToString());

            EndPoint oldEndPoint = m_RightEndPoint;
            
            // get async going
            if (m_AcceptQueueOrConnectResult != null)
            {
                throw new InvalidOperationException(SR.GetString(SR.net_sockets_no_duplicate_async));
            }

            m_AcceptQueueOrConnectResult = asyncResult;

            if (!SetAsyncEventSelect(AsyncEventBits.FdConnect)){
                m_AcceptQueueOrConnectResult = null;
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            
            // This can throw ObjectDisposedException.
            IntPtr handle = m_Handle.DangerousGetHandle();

            if (m_RightEndPoint == null) {
                  m_RightEndPoint = endPointSnapshot;
            }

            SocketError errorCode = UnsafeNclNativeMethods.OSSOCK.WSAConnect(
                handle,
                socketAddress.m_Buffer,
                socketAddress.m_Size,
                IntPtr.Zero,
                IntPtr.Zero,
                IntPtr.Zero,
                IntPtr.Zero);

            if (errorCode!=SocketError.Success) {
                errorCode = (SocketError)Marshal.GetLastWin32Error();
            }
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::DoBeginConnect() UnsafeNclNativeMethods.OSSOCK.WSAConnect returns errorCode:" + errorCode);

            if (errorCode != SocketError.WouldBlock)
            {
                bool completeSynchronously = true;
                if (errorCode == SocketError.Success)
                {
                    SetToConnected();
                }
                else
                {
                    asyncResult.ErrorCode = (int) errorCode;
                }

                // Using interlocked to avoid a race condition with RegisteredWaitCallback
                // Although UnsetAsyncEventSelect() below should cancel the callback, but 
                // it may already be in progress and therefore resulting in simultaneous
                // registeredWaitCallback calling ConnectCallback() and the synchronous
                // completion here.
                if (Interlocked.Exchange(ref m_RegisteredWait, null) == null)
                    completeSynchronously = false;
                //
                // Cancel async event and go back to blocking mode.
                //
                UnsetAsyncEventSelect();
                InternalSetBlocking(true);

                if (errorCode == SocketError.Success)
                {
                    //
                    // synchronously complete the IO and call the user's callback.
                    //
                    if (completeSynchronously)
                        asyncResult.InvokeCallback();
                }
                else
                {
                    //
                    // if the asynchronous native call fails synchronously
                    // we'll throw a SocketException
                    //
                    m_RightEndPoint = oldEndPoint;
                    SocketException socketException = new SocketException(errorCode);
                    UpdateStatusAfterSocketError(socketException);
                    m_AcceptQueueOrConnectResult = null;
                    if(Logging.On)Logging.Exception(Logging.Sockets, this, "BeginConnect", socketException);
                    throw socketException;
                }
            }

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::DoBeginConnect() to:" + endPointSnapshot.ToString() + " returning AsyncResult:" + ValidationHelper.HashString(asyncResult));
        }

        // Begin ConnectEx is only supported for connection oriented protocols
        // for now this is only supported on win32 platforms.  We need to fix this
        // when the getdelegatefrom function methods are available on 64bit.
        // to use this, the endpoint must either be an IP endpoint, or the
        // socket must already be bound.
        private bool CanUseConnectEx(EndPoint remoteEP)
        {
            return false;
        }

        //
        // This is the internal callback that will be called when
        // the IO we issued for the user to winsock has completed.
        // when this function gets called it must:
        // 1) update the AsyncResult object with the results of the completed IO
        // 2) signal events that the user might be waiting on
        // 3) call the callback function that the user might have specified
        //
        // This method was copied from a ConnectAsyncResult class that became useless.
        private void ConnectCallback()
        {
            LazyAsyncResult asyncResult = (LazyAsyncResult) m_AcceptQueueOrConnectResult;


            GlobalLog.Enter("Socket#" + ValidationHelper.HashString(this) + "::ConnectCallback");
            //
            // If we came here due to a race between BeginConnect and Dispose
            //
            if (asyncResult.InternalPeekCompleted)
            {
                GlobalLog.Assert(CleanedUp, "Socket#{0}::ConnectCallback()|asyncResult is compelted but the socket does not have CleanedUp set.", ValidationHelper.HashString(this));
                GlobalLog.Leave("Socket#" + ValidationHelper.HashString(this) + "::ConnectCallback", "Already completed, socket must be closed");
                return;
            }


            //

            //
            // get async completion
            //
            /*
            int errorCode = (int)socket.GetSocketOption(SocketOptionLevel.Socket, SocketOptionName.Error);
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(socket) + "::ConnectCallback() GetSocketOption() returns errorCode:" + errorCode.ToString());
            */

            NetworkEvents networkEvents = new NetworkEvents();
            networkEvents.Events = AsyncEventBits.FdConnect;

            SocketError errorCode = SocketError.OperationAborted;
            object result = null;

            try
            {
                if (!CleanedUp)
                {
                    try
                    {
                        errorCode = UnsafeNclNativeMethods.OSSOCK.WSAEnumNetworkEvents(
                            m_Handle,
                            m_AsyncEvent.SafeWaitHandle,
                            ref networkEvents);

                        if (errorCode != SocketError.Success)
                        {
                            errorCode = (SocketError) Marshal.GetLastWin32Error();
                            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::ConnectCallback() WSAEnumNetworkEvents() failed with errorCode:" + errorCode.ToString());
                        }
                        else
                        {
                            errorCode = (SocketError) networkEvents.ErrorCodes[(int) AsyncEventBitsPos.FdConnectBit];
                            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::ConnectCallback() ErrorCodes(FdConnect) got errorCode:" + errorCode.ToString());
                        }
                        //
                        // Cancel async event and go back to blocking mode.
                        //
                        UnsetAsyncEventSelect();
                        InternalSetBlocking(true);
                    }
                    catch (ObjectDisposedException)
                    {
                        errorCode = SocketError.OperationAborted;
                    }
                }

                //
                // if the native non-blocking call failed we'll throw a SocketException in EndConnect()
                //
                if (errorCode == SocketError.Success)
                {
                    //
                    // the Socket is connected, update our state and performance counter
                    //
                    SetToConnected();
                }
            }
            catch (Exception exception)
            {
                if (NclUtilities.IsFatal(exception)) throw;

                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::ConnectCallback() caught exception:" + exception.Message + ", CleanedUp:" + CleanedUp);
                result = exception;
            }
            catch
            {
                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::ConnectCallback() caught exception: Non-CLS Compliant Exception" + " CleanedUp:" + CleanedUp);
                result = new Exception(SR.GetString(SR.net_nonClsCompliantException));
            }

            if (!asyncResult.InternalPeekCompleted)
            {
                // A "ErrorCode" concept is questionable, for ex. below lines are subject to a race condition
                asyncResult.ErrorCode = (int) errorCode;
                asyncResult.InvokeCallback(result);
            }

            GlobalLog.Leave("Socket#" + ValidationHelper.HashString(this) + "::ConnectCallback", errorCode.ToString());
        }

        [HostProtection(ExternalThreading=true)]
        public IAsyncResult BeginConnect(string host, int port, AsyncCallback requestCallback, object state){
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "BeginConnect", host);

            if (CleanedUp){
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            if (host==null) {
                throw new ArgumentNullException("host");
            }
            if (!ValidationHelper.ValidateTcpPort(port)){
                throw new ArgumentOutOfRangeException("port");
            }
            if (addressFamily != AddressFamily.InterNetwork && addressFamily != AddressFamily.InterNetworkV6) {
                throw new NotSupportedException(SR.GetString(SR.net_invalidversion));
            }

            if (isListening)
            {
                throw new InvalidOperationException(SR.GetString(SR.net_sockets_mustnotlisten));
            }

            // Here, want to flow the context.  No need to lock.
            MultipleAddressConnectAsyncResult result = new MultipleAddressConnectAsyncResult(null, port, this, state, requestCallback);
            result.StartPostingAsyncOp(false);

            IAsyncResult dnsResult = Dns.UnsafeBeginGetHostAddresses(host, new AsyncCallback(DnsCallback), result);
            if (dnsResult.CompletedSynchronously)
            {
                DoDnsCallback(dnsResult, result);
            }

            // Done posting.
            result.FinishPostingAsyncOp(ref Caches.ConnectClosureCache);

            if(Logging.On)Logging.Exit(Logging.Sockets, this, "BeginConnect", result);
            return result;
        }

/* Uncomment when needed.
        internal IAsyncResult UnsafeBeginConnect(string host, int port, AsyncCallback requestCallback, object state)
        {
            if (Logging.On) Logging.Enter(Logging.Sockets, this, "BeginConnect", host);

            if (CleanedUp)
            {
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            MultipleAddressConnectAsyncResult result = new MultipleAddressConnectAsyncResult(null, port, this, state, requestCallback);

            IAsyncResult dnsResult = Dns.UnsafeBeginGetHostAddresses(host, new AsyncCallback(DnsCallback), result);
            if (dnsResult.CompletedSynchronously)
            {
                DoDnsCallback(dnsResult, result);
            }

            if (Logging.On) Logging.Exit(Logging.Sockets, this, "BeginConnect", result);
            return result;
        }
*/

        [HostProtection(ExternalThreading=true)]
        public IAsyncResult BeginConnect(IPAddress address, int port, AsyncCallback requestCallback, object state){
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "BeginConnect", address);
            if (CleanedUp){
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            if (address==null) {
                throw new ArgumentNullException("address");
            }
            if (!ValidationHelper.ValidateTcpPort(port)){
                throw new ArgumentOutOfRangeException("port");
            }
            //if address family isn't the socket address family throw
            if (addressFamily != address.AddressFamily) {
                throw new NotSupportedException(SR.GetString(SR.net_invalidversion));
            }

            IAsyncResult result = BeginConnect(new IPEndPoint(address,port),requestCallback,state);
            if(Logging.On)Logging.Exit(Logging.Sockets, this, "BeginConnect", result);
            return result;
        }

/* Uncomment when needed.
        internal IAsyncResult UnsafeBeginConnect(IPAddress address, int port, AsyncCallback requestCallback, object state)
        {
            if (Logging.On) Logging.Enter(Logging.Sockets, this, "BeginConnect", address);

            if (CleanedUp)
            {
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            IAsyncResult result = UnsafeBeginConnect(new IPEndPoint(address, port), requestCallback, state);
            if (Logging.On) Logging.Exit(Logging.Sockets, this, "BeginConnect", result);
            return result;
        }
*/

        [HostProtection(ExternalThreading=true)]
        public IAsyncResult BeginConnect(IPAddress[] addresses, int port, AsyncCallback requestCallback, object state)
        {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "BeginConnect", addresses);
            if (CleanedUp){
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            if (addresses==null) {
                throw new ArgumentNullException("addresses");
            }
            if (addresses.Length == 0) {
                throw new ArgumentException(SR.GetString(SR.net_invalidAddressList), "addresses");
            }
            if (!ValidationHelper.ValidateTcpPort(port)) {
                throw new ArgumentOutOfRangeException("port");
            }
            if (addressFamily != AddressFamily.InterNetwork && addressFamily != AddressFamily.InterNetworkV6) {
                throw new NotSupportedException(SR.GetString(SR.net_invalidversion));
            }

            if (isListening)
            {
                throw new InvalidOperationException(SR.GetString(SR.net_sockets_mustnotlisten));
            }

            // Set up the result to capture the context.  No need for a lock.
            MultipleAddressConnectAsyncResult result = new MultipleAddressConnectAsyncResult(addresses, port, this, state, requestCallback);
            result.StartPostingAsyncOp(false);

            DoMultipleAddressConnectCallback(PostOneBeginConnect(result), result);

            // Finished posting async op.  Possibly will call callback.
            result.FinishPostingAsyncOp(ref Caches.ConnectClosureCache);

            if(Logging.On)Logging.Exit(Logging.Sockets, this, "BeginConnect", result);
            return result;
        }

/* Uncomment when needed.
        internal IAsyncResult UnsafeBeginConnect(IPAddress[] addresses, int port, AsyncCallback requestCallback, object state)
        {
            if (Logging.On) Logging.Enter(Logging.Sockets, this, "BeginConnect", addresses);

            if (CleanedUp)
            {
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            // No need to capture context.
            MultipleAddressConnectAsyncResult result = new MultipleAddressConnectAsyncResult(addresses, port, this, state, requestCallback);

            DoMultipleAddressConnectCallback(PostOneBeginConnect(result), result);

            if (Logging.On) Logging.Exit(Logging.Sockets, this, "Connect", result);
            return result;
        }
*/


        // Supports DisconnectEx - this provides completion port IO and support for
        //disconnect and reconnects
        [HostProtection(ExternalThreading=true)]
        public IAsyncResult BeginDisconnect(bool reuseSocket, AsyncCallback callback, object state)
        {
            // Start context-flowing op.  No need to lock - we don't use the context till the callback.
            DisconnectOverlappedAsyncResult asyncResult = new DisconnectOverlappedAsyncResult(this, state, callback);
            asyncResult.StartPostingAsyncOp(false);

            // Post the disconnect.
            DoBeginDisconnect(reuseSocket, asyncResult);

            // Finish flowing (or call the callback), and return.
            asyncResult.FinishPostingAsyncOp();
            return asyncResult;
        }

/* Uncomment when needed.
        internal IAsyncResult UnsafeBeginDisconnect(bool reuseSocket, AsyncCallback callback, object state)
        {
            // No flowing in this version.
            DisconnectOverlappedAsyncResult asyncResult = new DisconnectOverlappedAsyncResult(this, state, callback);
            DoBeginDisconnect(reuseSocket, asyncResult);
            return asyncResult;
        }
*/

        private void DoBeginDisconnect(bool reuseSocket, DisconnectOverlappedAsyncResult asyncResult)
        {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "BeginDisconnect",null);
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::DoBeginDisconnect() ");


            throw new PlatformNotSupportedException(SR.GetString(SR.WinXPRequired));


            asyncResult.SetUnmanagedStructures(null);

            SocketError errorCode=SocketError.Success;

            // This can throw ObjectDisposedException (handle, and retrieving the delegate).
            if (!DisconnectEx(m_Handle,asyncResult.OverlappedHandle, (int)(reuseSocket?TransmitFileOptions.ReuseSocket:0),0)) {
                errorCode = (SocketError)Marshal.GetLastWin32Error();
            }

            if (errorCode == SocketError.Success) {
                SetToDisconnected();
                m_RemoteEndPoint = null;
            }

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::DoBeginDisconnect() UnsafeNclNativeMethods.OSSOCK.DisConnectEx returns:" + errorCode.ToString());

            // if the asynchronous native call fails synchronously
            // we'll throw a SocketException
            //
            errorCode = asyncResult.CheckAsyncCallOverlappedResult(errorCode);

            if (errorCode!= SocketError.Success) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException(errorCode);
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets,this,"BeginDisconnect", socketException);
                throw socketException;
            }

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::DoBeginDisconnect() returning AsyncResult:" + ValidationHelper.HashString(asyncResult));
            if(Logging.On)Logging.Exit(Logging.Sockets, this, "BeginDisconnect", asyncResult);
        }


        // Supports DisconnectEx - this provides support for disconnect and reconnects
        public void Disconnect(bool reuseSocket) {

             if(Logging.On)Logging.Enter(Logging.Sockets, this, "Disconnect",null);
             if (CleanedUp) {
                 throw new ObjectDisposedException(this.GetType().FullName);
             }

            throw new PlatformNotSupportedException(SR.GetString(SR.WinXPRequired));


             GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::Disconnect() ");

             SocketError errorCode = SocketError.Success;

             // This can throw ObjectDisposedException (handle, and retrieving the delegate).
             if (!DisconnectEx_Blocking(m_Handle.DangerousGetHandle(), IntPtr.Zero, (int) (reuseSocket ? TransmitFileOptions.ReuseSocket : 0), 0))
             {
                 errorCode = (SocketError)Marshal.GetLastWin32Error();
             }

             GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::Disconnect() UnsafeNclNativeMethods.OSSOCK.DisConnectEx returns:" + errorCode.ToString());


             if (errorCode!= SocketError.Success) {
                 //
                 // update our internal state after this socket error and throw
                 //
                SocketException socketException = new SocketException(errorCode);
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets,this,"Disconnect", socketException);
                throw socketException;
             }

             SetToDisconnected();
             m_RemoteEndPoint = null;
             
             //set the blocking mode back to what it was
             InternalSetBlocking(willBlockInternal);
             if(Logging.On)Logging.Exit(Logging.Sockets, this, "Disconnect", null);
        }

        /*++

        Routine Description:

           EndConnect - Called addressFamilyter receiving callback from BeginConnect,
            in order to retrive the result of async call

        Arguments:

           AsyncResult - the AsyncResult Returned fron BeginConnect call

        Return Value:

           int - Return code from aync Connect, 0 for success, SocketError.NotConnected otherwise

        --*/
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void EndConnect(IAsyncResult asyncResult) {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "EndConnect", asyncResult);
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            //
            // parameter validation
            //
            if (asyncResult==null) {
                throw new ArgumentNullException("asyncResult");
            }


            LazyAsyncResult castedAsyncResult = asyncResult as ConnectOverlappedAsyncResult;
            if (castedAsyncResult == null) {
                castedAsyncResult = asyncResult as MultipleAddressConnectAsyncResult;
                if (castedAsyncResult == null) {
                    castedAsyncResult = asyncResult as ConnectAsyncResult;
                }
            }

            if (castedAsyncResult == null || castedAsyncResult.AsyncObject!=this) {
                throw new ArgumentException(SR.GetString(SR.net_io_invalidasyncresult), "asyncResult");
            }
            if (castedAsyncResult.EndCalled) {
                throw new InvalidOperationException(SR.GetString(SR.net_io_invalidendcall, "EndConnect"));
            }

            castedAsyncResult.InternalWaitForCompletion();
            castedAsyncResult.EndCalled = true;
            m_AcceptQueueOrConnectResult = null;

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::EndConnect() asyncResult:" + ValidationHelper.HashString(asyncResult));

            if (castedAsyncResult.Result is Exception) {
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "EndConnect", (Exception)castedAsyncResult.Result);
                throw (Exception)castedAsyncResult.Result;
            }
            if ((SocketError)castedAsyncResult.ErrorCode!=SocketError.Success) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException(castedAsyncResult.ErrorCode);
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "EndConnect", socketException);
                throw socketException;
            }
            if(Logging.On)Logging.Exit(Logging.Sockets, this, "EndConnect", "");
        }

        public void EndDisconnect(IAsyncResult asyncResult) {

             if(Logging.On)Logging.Enter(Logging.Sockets, this, "EndDisconnect", asyncResult);
             if (CleanedUp) {
               throw new ObjectDisposedException(this.GetType().FullName);
             }

            throw new PlatformNotSupportedException(SR.GetString(SR.WinNTRequired));

             if (asyncResult==null) {
               throw new ArgumentNullException("asyncResult");
             }


             //get async result and check for errors
             LazyAsyncResult castedAsyncResult = asyncResult as LazyAsyncResult;
             if (castedAsyncResult==null || castedAsyncResult.AsyncObject!=this) {
               throw new ArgumentException(SR.GetString(SR.net_io_invalidasyncresult), "asyncResult");
             }
             if (castedAsyncResult.EndCalled) {
               throw new InvalidOperationException(SR.GetString(SR.net_io_invalidendcall, "EndDisconnect"));
             }

             //wait for completion if it hasn't occured
             castedAsyncResult.InternalWaitForCompletion();
             castedAsyncResult.EndCalled = true;


             GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::EndDisconnect()");

             //
             // if the asynchronous native call failed asynchronously
             // we'll throw a SocketException
             //
             if ((SocketError)castedAsyncResult.ErrorCode!=SocketError.Success) {
                 //
                 // update our internal state after this socket error and throw
                 //
                SocketException socketException = new SocketException(castedAsyncResult.ErrorCode);
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets,this,"EndDisconnect", socketException);
                throw socketException;
             }

             if(Logging.On)Logging.Exit(Logging.Sockets, this, "EndDisconnect", null);
             return;
        }


        /*++

        Routine Description:

           BeginSend - Async implimentation of Send call, mirrored addressFamilyter BeginReceive
           This routine may go pending at which time,
           but any case the callback Delegate will be called upon completion

        Arguments:

           WriteBuffer - status line that we wish to parse
           Index - Offset into WriteBuffer to begin sending from
           Size - Size of Buffer to transmit
           Callback - Delegate function that holds callback, called on completeion of I/O
           State - State used to track callback, set by caller, not required

        Return Value:

           IAsyncResult - Async result used to retreive result

        --*/

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>

        [HostProtection(ExternalThreading=true)]
        public IAsyncResult BeginSend(byte[] buffer, int offset, int size, SocketFlags socketFlags, AsyncCallback callback, object state)
        {
            SocketError errorCode;
            IAsyncResult result = BeginSend(buffer, offset, size, socketFlags, out errorCode, callback, state);
            if(errorCode != SocketError.Success && errorCode !=SocketError.IOPending){
                throw new SocketException(errorCode);
            }
            return result;
        }

        [HostProtection(ExternalThreading=true)]
        public IAsyncResult BeginSend(byte[] buffer, int offset, int size, SocketFlags socketFlags, out SocketError errorCode, AsyncCallback callback, object state)
        {

            if(Logging.On)Logging.Enter(Logging.Sockets, this, "BeginSend", "");

            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            //
            // parameter validation
            //
            if (buffer == null)
            {
                throw new ArgumentNullException("buffer");
            }
            if (offset < 0 || offset > buffer.Length)
            {
                throw new ArgumentOutOfRangeException("offset");
            }
            if (size < 0 || size > buffer.Length - offset)
            {
                throw new ArgumentOutOfRangeException("size");
            }

            // We need to flow the context here.  But we don't need to lock the context - we don't use it until the callback.
            OverlappedAsyncResult asyncResult = new OverlappedAsyncResult(this, state, callback);
            asyncResult.StartPostingAsyncOp(false);

            // Run the send with this asyncResult.
            errorCode = DoBeginSend(buffer, offset, size, socketFlags, asyncResult);

            if(errorCode != SocketError.Success && errorCode !=SocketError.IOPending){
                asyncResult = null;
            }
            else
            {
                // We're not throwing, so finish the async op posting code so we can return to the user.
                // If the operation already finished, the callback will be called from here.
                asyncResult.FinishPostingAsyncOp(ref Caches.SendClosureCache);
            }

            if(Logging.On)Logging.Exit(Logging.Sockets, this, "BeginSend", asyncResult);
            return asyncResult;
        }




        internal
        IAsyncResult UnsafeBeginSend(byte[] buffer, int offset, int size, SocketFlags socketFlags, AsyncCallback callback, object state)
        {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "UnsafeBeginSend", "");

            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            // No need to flow the context.
            OverlappedAsyncResult asyncResult = new OverlappedAsyncResult(this, state, callback);

            SocketError errorCode = DoBeginSend(buffer, offset, size, socketFlags, asyncResult);
            if(errorCode != SocketError.Success && errorCode !=SocketError.IOPending){
                throw new SocketException(errorCode);
            }

            if(Logging.On)Logging.Exit(Logging.Sockets, this, "UnsafeBeginSend", asyncResult);
            return asyncResult;
        }

        private SocketError DoBeginSend(byte[] buffer, int offset, int size, SocketFlags socketFlags, OverlappedAsyncResult asyncResult)
        {

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::BeginSend() SRC:" + ValidationHelper.ToString(LocalEndPoint) + " DST:" + ValidationHelper.ToString(RemoteEndPoint) + " size:" + size.ToString());

            // Guarantee to call CheckAsyncCallOverlappedResult if we call SetUnamangedStructures with a cache in order to
            // avoid a Socket leak in case of error.
            SocketError errorCode = SocketError.SocketError;
            try
            {
                // Set up asyncResult for overlapped WSASend.
                // This call will use completion ports on WinNT and Overlapped IO on Win9x.
                asyncResult.SetUnmanagedStructures(buffer, offset, size, null, false /*don't pin null remoteEP*/, ref Caches.SendOverlappedCache);

                //
                // Get the Send going.
                //
                GlobalLog.Print("BeginSend: asyncResult:" + ValidationHelper.HashString(asyncResult) + " size:" + size.ToString());
                int bytesTransferred;

                // This can throw ObjectDisposedException.
                errorCode = UnsafeNclNativeMethods.OSSOCK.WSASend(
                    m_Handle,
                    ref asyncResult.m_SingleBuffer,
                    1, // only ever 1 buffer being sent
                    out bytesTransferred,
                    socketFlags,
                    asyncResult.OverlappedHandle,
                    IntPtr.Zero);

                if (errorCode!=SocketError.Success) {
                    errorCode = (SocketError)Marshal.GetLastWin32Error();
                }
                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::BeginSend() UnsafeNclNativeMethods.OSSOCK.WSASend returns:" + errorCode.ToString() + " size:" + size.ToString() + " returning AsyncResult:" + ValidationHelper.HashString(asyncResult));
            }
            finally
            {
                errorCode = asyncResult.CheckAsyncCallOverlappedResult(errorCode);
            }

            //
            // if the asynchronous native call fails synchronously
            // we'll throw a SocketException
            //
            if (errorCode != SocketError.Success)
            {
                asyncResult.ExtractCache(ref Caches.SendOverlappedCache);
                UpdateStatusAfterSocketError(errorCode);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "BeginSend", new SocketException(errorCode));
            }
            return errorCode;
        }


        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>


        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [HostProtection(ExternalThreading=true)]
        public IAsyncResult BeginSend(IList<ArraySegment<byte>> buffers, SocketFlags socketFlags, AsyncCallback callback, object state)
        {
            SocketError errorCode;
            IAsyncResult result = BeginSend(buffers, socketFlags, out errorCode, callback, state);
            if(errorCode != SocketError.Success && errorCode !=SocketError.IOPending){
                throw new SocketException(errorCode);
            }
            return result;
        }


        [CLSCompliant(false)]
        [HostProtection(ExternalThreading=true)]
        public IAsyncResult BeginSend(IList<ArraySegment<byte>> buffers, SocketFlags socketFlags, out SocketError errorCode, AsyncCallback callback, object state)
        {

            if(Logging.On)Logging.Enter(Logging.Sockets, this, "BeginSend", "");

            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            //
            // parameter validation
            //
            if (buffers==null) {
                throw new ArgumentNullException("buffers");
            }

            if(buffers.Count == 0){
                throw new ArgumentException(SR.GetString(SR.net_sockets_zerolist,"buffers"), "buffers");
            }

            // We need to flow the context here.  But we don't need to lock the context - we don't use it until the callback.
            OverlappedAsyncResult asyncResult = new OverlappedAsyncResult(this, state, callback);
            asyncResult.StartPostingAsyncOp(false);

            // Run the send with this asyncResult.
            errorCode = DoBeginSend(buffers, socketFlags, asyncResult);

            // We're not throwing, so finish the async op posting code so we can return to the user.
            // If the operation already finished, the callback will be called from here.
            asyncResult.FinishPostingAsyncOp(ref Caches.SendClosureCache);

            if(errorCode != SocketError.Success && errorCode !=SocketError.IOPending){
                asyncResult = null;
            }

            if(Logging.On)Logging.Exit(Logging.Sockets, this, "BeginSend", asyncResult);
            return asyncResult;
        }

/* Uncomment when needed.
        internal IAsyncResult UnsafeBeginSend(IList<ArraySegment<byte>> buffers, SocketFlags socketFlags, AsyncCallback callback, object state)
        {
            // No need to flow the context.
            OverlappedAsyncResult asyncResult = new OverlappedAsyncResult(this, state, callback);
            DoBeginSend(buffers, socketFlags, asyncResult);
            return asyncResult;
        }
*/

        private SocketError DoBeginSend(IList<ArraySegment<byte>> buffers, SocketFlags socketFlags, OverlappedAsyncResult asyncResult)
        {

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::BeginSend() SRC:" + ValidationHelper.ToString(LocalEndPoint) + " DST:" + ValidationHelper.ToString(RemoteEndPoint) + " buffers:" + buffers);

            // Guarantee to call CheckAsyncCallOverlappedResult if we call SetUnamangedStructures with a cache in order to
            // avoid a Socket leak in case of error.
            SocketError errorCode = SocketError.SocketError;
            try
            {
                // Set up asyncResult for overlapped WSASend.
                // This call will use completion ports on WinNT and Overlapped IO on Win9x.
                asyncResult.SetUnmanagedStructures(buffers, ref Caches.SendOverlappedCache);

                GlobalLog.Print("BeginSend: asyncResult:" + ValidationHelper.HashString(asyncResult));

                // This can throw ObjectDisposedException.
                int bytesTransferred;
                errorCode = UnsafeNclNativeMethods.OSSOCK.WSASend(
                    m_Handle,
                    asyncResult.m_WSABuffers,
                    asyncResult.m_WSABuffers.Length,
                    out bytesTransferred,
                    socketFlags,
                    asyncResult.OverlappedHandle,
                    IntPtr.Zero);

                if (errorCode!=SocketError.Success) {
                    errorCode = (SocketError)Marshal.GetLastWin32Error();
                }
                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::BeginSend() UnsafeNclNativeMethods.OSSOCK.WSASend returns:" + errorCode.ToString() + " returning AsyncResult:" + ValidationHelper.HashString(asyncResult));
            }
            finally
            {
                errorCode = asyncResult.CheckAsyncCallOverlappedResult(errorCode);
            }

            //
            // if the asynchronous native call fails synchronously
            // we'll throw a SocketException
            //
            if (errorCode != SocketError.Success)
            {
                asyncResult.ExtractCache(ref Caches.SendOverlappedCache);
                UpdateStatusAfterSocketError(errorCode);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "BeginSend", new SocketException(errorCode));
            }
            return errorCode;
        }


        /*++

        Routine Description:

           EndSend -  Called by user code addressFamilyter I/O is done or the user wants to wait.
                        until Async completion, needed to retrieve error result from call

        Arguments:

           AsyncResult - the AsyncResult Returned fron BeginSend call

        Return Value:

           int - Number of bytes transferred

        --*/
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>


        public int EndSend(IAsyncResult asyncResult) {
            SocketError errorCode;
            int bytesTransferred = EndSend(asyncResult, out errorCode);
            if(errorCode != SocketError.Success){
                throw new SocketException(errorCode);
            }
            return bytesTransferred;
        }


        public int EndSend(IAsyncResult asyncResult, out SocketError errorCode) {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "EndSend", asyncResult);
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            //
            // parameter validation
            //
            if (asyncResult==null) {
                throw new ArgumentNullException("asyncResult");
            }
            OverlappedAsyncResult castedAsyncResult = asyncResult as OverlappedAsyncResult;
            if (castedAsyncResult==null || castedAsyncResult.AsyncObject!=this) {
                throw new ArgumentException(SR.GetString(SR.net_io_invalidasyncresult), "asyncResult");
            }
            if (castedAsyncResult.EndCalled) {
                throw new InvalidOperationException(SR.GetString(SR.net_io_invalidendcall, "EndSend"));
            }

            int bytesTransferred = (int)castedAsyncResult.InternalWaitForCompletion();
            castedAsyncResult.EndCalled = true;
            castedAsyncResult.ExtractCache(ref Caches.SendOverlappedCache);


            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::EndSend() bytesTransferred:" + bytesTransferred.ToString());

            //
            // if the asynchronous native call failed asynchronously
            // we'll throw a SocketException
            //
            errorCode = (SocketError)castedAsyncResult.ErrorCode;
            if (errorCode != SocketError.Success) {
                //
                // update our internal state after this socket error and throw
                //
                UpdateStatusAfterSocketError(errorCode);
                if(Logging.On){
                    Logging.Exception(Logging.Sockets, this, "EndSend", new SocketException(errorCode));
                    Logging.Exit(Logging.Sockets, this, "EndSend", 0);
                }
                return 0;
           }
            if(Logging.On)Logging.Exit(Logging.Sockets, this, "EndSend", bytesTransferred);
            return bytesTransferred;
        }



        /*++

        Routine Description:

           BeginSendTo - Async implimentation of SendTo,

           This routine may go pending at which time,
           but any case the callback Delegate will be called upon completion

        Arguments:

           WriteBuffer - Buffer to transmit
           Index - Offset into WriteBuffer to begin sending from
           Size - Size of Buffer to transmit
           Flags - Specific Socket flags to pass to winsock
           remoteEP - EndPoint to transmit To
           Callback - Delegate function that holds callback, called on completeion of I/O
           State - State used to track callback, set by caller, not required

        Return Value:

           IAsyncResult - Async result used to retreive result

        --*/
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [HostProtection(ExternalThreading=true)]
        public IAsyncResult BeginSendTo(byte[] buffer, int offset, int size, SocketFlags socketFlags, EndPoint remoteEP, AsyncCallback callback, object state)
        {

            if(Logging.On)Logging.Enter(Logging.Sockets, this, "BeginSendTo", "");

            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            //
            // parameter validation
            //
            if (buffer==null) {
                throw new ArgumentNullException("buffer");
            }
            if (remoteEP==null) {
                throw new ArgumentNullException("remoteEP");
            }
            if (offset<0 || offset>buffer.Length) {
                throw new ArgumentOutOfRangeException("offset");
            }
            if (size<0 || size>buffer.Length-offset) {
                throw new ArgumentOutOfRangeException("size");
            }

            // This will check the permissions for connect.
            EndPoint endPointSnapshot = remoteEP;
            SocketAddress socketAddress = CheckCacheRemote(ref endPointSnapshot, false);

            // Set up the async result and indicate to flow the context.
            OverlappedAsyncResult asyncResult = new OverlappedAsyncResult(this, state, callback);
            asyncResult.StartPostingAsyncOp(false);

            // Post the send.
            DoBeginSendTo(buffer, offset, size, socketFlags, endPointSnapshot, socketAddress, asyncResult);

            // Finish, possibly posting the callback.  The callback won't be posted before this point is reached.
            asyncResult.FinishPostingAsyncOp(ref Caches.SendClosureCache);

            if(Logging.On)Logging.Exit(Logging.Sockets, this, "BeginSendTo", asyncResult);
            return asyncResult;
        }

/* Uncomment when needed.
        internal IAsyncResult UnsafeBeginSendTo(byte[] buffer, int offset, int size, SocketFlags socketFlags, EndPoint remoteEP, AsyncCallback callback, object state)
        {
            EndPoint endPointSnapshot = remoteEP;
            SocketAddress socketAddress = SnapshotAndSerialize(ref endPointSnapshot);

            OverlappedAsyncResult asyncResult = new OverlappedAsyncResult(this, state, callback);
            DoBeginSendTo(buffer, offset, size, socketFlags, endPointSnapshot, socketAddress, asyncResult);
            return asyncResult;
        }
*/

        private void DoBeginSendTo(byte[] buffer, int offset, int size, SocketFlags socketFlags, EndPoint endPointSnapshot, SocketAddress socketAddress, OverlappedAsyncResult asyncResult)
        {
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::DoBeginSendTo() size:" + size.ToString());
            EndPoint oldEndPoint = m_RightEndPoint;

            // Guarantee to call CheckAsyncCallOverlappedResult if we call SetUnamangedStructures with a cache in order to
            // avoid a Socket leak in case of error.
            SocketError errorCode = SocketError.SocketError;
            try
            {
                // Set up asyncResult for overlapped WSASendTo.
                // This call will use completion ports on WinNT and Overlapped IO on Win9x.
                asyncResult.SetUnmanagedStructures(buffer, offset, size, socketAddress, false /* don't pin RemoteEP*/, ref Caches.SendOverlappedCache);

                if (m_RightEndPoint == null)
                {
                    m_RightEndPoint = endPointSnapshot;
                }

                int bytesTransferred;
                errorCode = UnsafeNclNativeMethods.OSSOCK.WSASendTo(
                    m_Handle,
                    ref asyncResult.m_SingleBuffer,
                    1, // only ever 1 buffer being sent
                    out bytesTransferred,
                    socketFlags,
                    asyncResult.GetSocketAddressPtr(),
                    asyncResult.SocketAddress.Size,
                    asyncResult.OverlappedHandle,
                    IntPtr.Zero);

                if (errorCode!=SocketError.Success) {
                    errorCode = (SocketError)Marshal.GetLastWin32Error();
                }
                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::DoBeginSendTo() UnsafeNclNativeMethods.OSSOCK.WSASend returns:" + errorCode.ToString() + " size:" + size + " returning AsyncResult:" + ValidationHelper.HashString(asyncResult));
            }
            catch (ObjectDisposedException)
            {
                m_RightEndPoint = oldEndPoint;
                throw;
            }
            finally
            {
                errorCode = asyncResult.CheckAsyncCallOverlappedResult(errorCode);
            }

            //
            // if the asynchronous native call fails synchronously
            // we'll throw a SocketException
            //
            if (errorCode!=SocketError.Success) {
                //
                // update our internal state after this socket error and throw
                //
                m_RightEndPoint = oldEndPoint;
                asyncResult.ExtractCache(ref Caches.SendOverlappedCache);
                SocketException socketException = new SocketException(errorCode);
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "BeginSendTo", socketException);
                throw socketException;
            }

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::DoBeginSendTo() size:" + size.ToString() + " returning AsyncResult:" + ValidationHelper.HashString(asyncResult));
        }

        /*++

        Routine Description:

           EndSendTo -  Called by user code addressFamilyter I/O is done or the user wants to wait.
                        until Async completion, needed to retrieve error result from call

        Arguments:

           AsyncResult - the AsyncResult Returned fron BeginSend call

        Return Value:

           int - Number of bytes transferred

        --*/
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int EndSendTo(IAsyncResult asyncResult) {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "EndSendTo", asyncResult);
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            //
            // parameter validation
            //
            if (asyncResult==null) {
                throw new ArgumentNullException("asyncResult");
            }
            OverlappedAsyncResult castedAsyncResult = asyncResult as OverlappedAsyncResult;
            if (castedAsyncResult==null || castedAsyncResult.AsyncObject!=this) {
                throw new ArgumentException(SR.GetString(SR.net_io_invalidasyncresult), "asyncResult");
            }
            if (castedAsyncResult.EndCalled) {
                throw new InvalidOperationException(SR.GetString(SR.net_io_invalidendcall, "EndSendTo"));
            }

            int bytesTransferred = (int)castedAsyncResult.InternalWaitForCompletion();
            castedAsyncResult.EndCalled = true;
            castedAsyncResult.ExtractCache(ref Caches.SendOverlappedCache);


            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::EndSendTo() bytesTransferred:" + bytesTransferred.ToString());

            //
            // if the asynchronous native call failed asynchronously
            // we'll throw a SocketException
            //
            if ((SocketError)castedAsyncResult.ErrorCode!=SocketError.Success) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException(castedAsyncResult.ErrorCode);
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "EndSendTo", socketException);
                throw socketException;
            }
            if(Logging.On)Logging.Exit(Logging.Sockets, this, "EndSendTo", bytesTransferred);
            return bytesTransferred;
        }


        /*++

        Routine Description:

           BeginReceive - Async implimentation of Recv call,

           Called when we want to start an async receive.
           We kick off the receive, and if it completes synchronously we'll
           call the callback. Otherwise we'll return an IASyncResult, which
           the caller can use to wait on or retrieve the final status, as needed.

           Uses Winsock 2 overlapped I/O.

        Arguments:

           ReadBuffer - status line that we wish to parse
           Index - Offset into ReadBuffer to begin reading from
           Size - Size of Buffer to recv
           Callback - Delegate function that holds callback, called on completeion of I/O
           State - State used to track callback, set by caller, not required

        Return Value:

           IAsyncResult - Async result used to retreive result

        --*/


        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>

        [HostProtection(ExternalThreading=true)]
        public IAsyncResult BeginReceive(byte[] buffer, int offset, int size, SocketFlags socketFlags, AsyncCallback callback, object state)
        {
            SocketError errorCode;
            IAsyncResult result = BeginReceive(buffer, offset, size, socketFlags, out errorCode, callback, state);
            if(errorCode != SocketError.Success && errorCode !=SocketError.IOPending){
                throw new SocketException(errorCode);
            }
            return result;
        }


        [HostProtection(ExternalThreading=true)]
        public IAsyncResult BeginReceive(byte[] buffer, int offset, int size, SocketFlags socketFlags, out SocketError errorCode, AsyncCallback callback, object state)
        {

            if(Logging.On)Logging.Enter(Logging.Sockets, this, "BeginReceive", "");

            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            //
            // parameter validation
            //
            if (buffer==null) {
                throw new ArgumentNullException("buffer");
            }
            if (offset<0 || offset>buffer.Length) {
                throw new ArgumentOutOfRangeException("offset");
            }
            if (size<0 || size>buffer.Length-offset) {
                throw new ArgumentOutOfRangeException("size");
            }

            // We need to flow the context here.  But we don't need to lock the context - we don't use it until the callback.
            OverlappedAsyncResult asyncResult = new OverlappedAsyncResult(this, state, callback);
            asyncResult.StartPostingAsyncOp(false);

            // Run the receive with this asyncResult.
            errorCode = DoBeginReceive(buffer, offset, size, socketFlags, asyncResult);

            if(errorCode != SocketError.Success && errorCode !=SocketError.IOPending){
                asyncResult = null;
            }
            else
            {
                // We're not throwing, so finish the async op posting code so we can return to the user.
                // If the operation already finished, the callback will be called from here.
                asyncResult.FinishPostingAsyncOp(ref Caches.ReceiveClosureCache);
            }

            if(Logging.On)Logging.Exit(Logging.Sockets, this, "BeginReceive", asyncResult);
            return asyncResult;
        }

        internal
        IAsyncResult UnsafeBeginReceive(byte[] buffer, int offset, int size, SocketFlags socketFlags, AsyncCallback callback, object state)
        {

            if(Logging.On)Logging.Enter(Logging.Sockets, this, "UnsafeBeginReceive", "");

            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            // No need to flow the context.
            OverlappedAsyncResult asyncResult = new OverlappedAsyncResult(this, state, callback);
            DoBeginReceive(buffer, offset, size, socketFlags, asyncResult);

            if(Logging.On)Logging.Exit(Logging.Sockets, this, "UnsafeBeginReceive", asyncResult);
            return asyncResult;
        }

        private SocketError DoBeginReceive(byte[] buffer, int offset, int size, SocketFlags socketFlags, OverlappedAsyncResult asyncResult)
        {
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::BeginReceive() size:" + size.ToString());

#if DEBUG
            IntPtr lastHandle = m_Handle.DangerousGetHandle();
#endif
            // Guarantee to call CheckAsyncCallOverlappedResult if we call SetUnamangedStructures with a cache in order to
            // avoid a Socket leak in case of error.
            SocketError errorCode = SocketError.SocketError;
            try
            {
                // Set up asyncResult for overlapped WSARecv.
                // This call will use completion ports on WinNT and Overlapped IO on Win9x.
                asyncResult.SetUnmanagedStructures(buffer, offset, size, null, false /* don't pin null RemoteEP*/, ref Caches.ReceiveOverlappedCache);

                // This can throw ObjectDisposedException.
                int bytesTransferred;
                errorCode = UnsafeNclNativeMethods.OSSOCK.WSARecv(
                    m_Handle,
                    ref asyncResult.m_SingleBuffer,
                    1,
                    out bytesTransferred,
                    ref socketFlags,
                    asyncResult.OverlappedHandle,
                    IntPtr.Zero);

                if (errorCode!=SocketError.Success) {
                    errorCode = (SocketError)Marshal.GetLastWin32Error();
                    GlobalLog.Assert(errorCode != SocketError.Success, "Socket#{0}::DoBeginReceive()|GetLastWin32Error() returned zero.", ValidationHelper.HashString(this));
                }
                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::BeginReceive() UnsafeNclNativeMethods.OSSOCK.WSARecv returns:" + errorCode.ToString() + " bytesTransferred:" + bytesTransferred.ToString() + " returning AsyncResult:" + ValidationHelper.HashString(asyncResult));
            }
            finally
            {
                errorCode = asyncResult.CheckAsyncCallOverlappedResult(errorCode);
            }

            //
            // if the asynchronous native call fails synchronously
            // we'll throw a SocketException
            //
            if (errorCode != SocketError.Success)
            {
                //
                // update our internal state after this socket error and throw
                asyncResult.ExtractCache(ref Caches.ReceiveOverlappedCache);
                UpdateStatusAfterSocketError(errorCode);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "BeginReceive", new SocketException(errorCode));
            }
#if DEBUG
            else
            {
                m_LastReceiveHandle = lastHandle;
                m_LastReceiveThread = Thread.CurrentThread.ManagedThreadId;
                m_LastReceiveTick = Environment.TickCount;
            }
#endif

            return errorCode;
        }


        [CLSCompliant(false)]
        [HostProtection(ExternalThreading=true)]
        public IAsyncResult BeginReceive(IList<ArraySegment<byte>> buffers, SocketFlags socketFlags, AsyncCallback callback, object state)
        {
            SocketError errorCode;
            IAsyncResult result = BeginReceive(buffers, socketFlags, out errorCode, callback, state);
            if(errorCode != SocketError.Success && errorCode !=SocketError.IOPending){
                throw new SocketException(errorCode);
            }
            return result;
        }


        [CLSCompliant(false)]
        [HostProtection(ExternalThreading=true)]
        public IAsyncResult BeginReceive(IList<ArraySegment<byte>> buffers, SocketFlags socketFlags, out SocketError errorCode, AsyncCallback callback, object state)
        {

            if(Logging.On)Logging.Enter(Logging.Sockets, this, "BeginReceive", "");

            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            //
            // parameter validation
            //
            if (buffers==null) {
               throw new ArgumentNullException("buffers");
            }

            if(buffers.Count == 0){
                throw new ArgumentException(SR.GetString(SR.net_sockets_zerolist,"buffers"), "buffers");
            }

            // We need to flow the context here.  But we don't need to lock the context - we don't use it until the callback.
            OverlappedAsyncResult asyncResult = new OverlappedAsyncResult(this, state, callback);
            asyncResult.StartPostingAsyncOp(false);

            // Run the receive with this asyncResult.
            errorCode = DoBeginReceive(buffers, socketFlags, asyncResult);

            if(errorCode != SocketError.Success && errorCode !=SocketError.IOPending){
                asyncResult = null;
            }
            else
            {
                // We're not throwing, so finish the async op posting code so we can return to the user.
                // If the operation already finished, the callback will be called from here.
                asyncResult.FinishPostingAsyncOp(ref Caches.ReceiveClosureCache);
            }

            if(Logging.On)Logging.Exit(Logging.Sockets, this, "BeginReceive", asyncResult);
            return asyncResult;
        }

/* Uncomment when needed.
        internal IAsyncResult UnsafeBeginReceive(IList<ArraySegment<byte>> buffers, SocketFlags socketFlags, AsyncCallback callback, object state)
        {
            // No need to flow the context.
            OverlappedAsyncResult asyncResult = new OverlappedAsyncResult(this, state, callback);
            DoBeginReceive(buffers, socketFlags, asyncResult);
            return asyncResult;
        }
*/

        private SocketError DoBeginReceive(IList<ArraySegment<byte>> buffers, SocketFlags socketFlags, OverlappedAsyncResult asyncResult)
        {
#if DEBUG
            IntPtr lastHandle = m_Handle.DangerousGetHandle();
#endif
            // Guarantee to call CheckAsyncCallOverlappedResult if we call SetUnamangedStructures with a cache in order to
            // avoid a Socket leak in case of error.
            SocketError errorCode = SocketError.SocketError;
            try
            {
                // Set up asyncResult for overlapped WSASend.
                // This call will use completion ports on WinNT and Overlapped IO on Win9x.
                asyncResult.SetUnmanagedStructures(buffers, ref Caches.ReceiveOverlappedCache);

                // This can throw ObjectDisposedException.
                int bytesTransferred;
                errorCode = UnsafeNclNativeMethods.OSSOCK.WSARecv(
                    m_Handle,
                    asyncResult.m_WSABuffers,
                    asyncResult.m_WSABuffers.Length,
                    out bytesTransferred,
                    ref socketFlags,
                    asyncResult.OverlappedHandle,
                    IntPtr.Zero);

                if (errorCode!=SocketError.Success) {
                    errorCode = (SocketError)Marshal.GetLastWin32Error();
                    GlobalLog.Assert(errorCode != SocketError.Success, "Socket#{0}::DoBeginReceive()|GetLastWin32Error() returned zero.", ValidationHelper.HashString(this));
                }
                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::DoBeginReceive() UnsafeNclNativeMethods.OSSOCK.WSARecv returns:" + errorCode.ToString() + " returning AsyncResult:" + ValidationHelper.HashString(asyncResult));
            }
            finally
            {
                errorCode = asyncResult.CheckAsyncCallOverlappedResult(errorCode);
            }

            //
            // if the asynchronous native call fails synchronously
            // we'll throw a SocketException
            //
            if (errorCode != SocketError.Success)
            {
                //
                // update our internal state after this socket error and throw
                asyncResult.ExtractCache(ref Caches.ReceiveOverlappedCache);
                UpdateStatusAfterSocketError(errorCode);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "BeginReceive", new SocketException(errorCode));
            }
#if DEBUG
            else
            {
                m_LastReceiveHandle = lastHandle;
                m_LastReceiveThread = Thread.CurrentThread.ManagedThreadId;
                m_LastReceiveTick = Environment.TickCount;
            }
#endif

            return errorCode;
        }

#if DEBUG
        private IntPtr m_LastReceiveHandle;
        private int m_LastReceiveThread;
        private int m_LastReceiveTick;
#endif

        /*++

        Routine Description:

           EndReceive -  Called when I/O is done or the user wants to wait. If
                     the I/O isn't done, we'll wait for it to complete, and then we'll return
                     the bytes of I/O done.

        Arguments:

           AsyncResult - the AsyncResult Returned fron BeginSend call

        Return Value:

           int - Number of bytes transferred

        --*/

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int EndReceive(IAsyncResult asyncResult) {
            SocketError errorCode;
            int bytesTransferred = EndReceive(asyncResult, out errorCode);
            if(errorCode != SocketError.Success){
                throw new SocketException(errorCode);
            }
            return bytesTransferred;
        }


        public int EndReceive(IAsyncResult asyncResult, out SocketError errorCode) {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "EndReceive", asyncResult);
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            //
            // parameter validation
            //
            if (asyncResult==null) {
                throw new ArgumentNullException("asyncResult");
            }
            OverlappedAsyncResult castedAsyncResult = asyncResult as OverlappedAsyncResult;
            if (castedAsyncResult==null || castedAsyncResult.AsyncObject!=this) {
                throw new ArgumentException(SR.GetString(SR.net_io_invalidasyncresult), "asyncResult");
            }
            if (castedAsyncResult.EndCalled) {
                throw new InvalidOperationException(SR.GetString(SR.net_io_invalidendcall, "EndReceive"));
            }

            int bytesTransferred = (int)castedAsyncResult.InternalWaitForCompletion();
            castedAsyncResult.EndCalled = true;
            castedAsyncResult.ExtractCache(ref Caches.ReceiveOverlappedCache);



            //
            // if the asynchronous native call failed asynchronously
            // we'll throw a SocketException
            //
            errorCode = (SocketError)castedAsyncResult.ErrorCode;
            if (errorCode!=SocketError.Success) {
                //
                // update our internal state after this socket error and throw
                //
                UpdateStatusAfterSocketError(errorCode);
                if(Logging.On){
                    Logging.Exception(Logging.Sockets, this, "EndReceive", new SocketException(errorCode));
                    Logging.Exit(Logging.Sockets, this, "EndReceive", 0);
                }
                return 0;
            }
            if(Logging.On)Logging.Exit(Logging.Sockets, this, "EndReceive", bytesTransferred);
            return bytesTransferred;
        }



        public IAsyncResult BeginReceiveMessageFrom(byte[] buffer, int offset, int size, SocketFlags socketFlags, ref EndPoint remoteEP, AsyncCallback callback, object state) {
            if (Logging.On) Logging.Enter(Logging.Sockets, this, "BeginReceiveMessageFrom", "");
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::BeginReceiveMessageFrom() size:" + size.ToString());

            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            if (buffer==null) {
                throw new ArgumentNullException("buffer");
            }
            if (remoteEP==null) {
                throw new ArgumentNullException("remoteEP");
            }
            if (offset<0 || offset>buffer.Length) {
                throw new ArgumentOutOfRangeException("offset");
            }
            if (size<0 || size>buffer.Length-offset) {
                throw new ArgumentOutOfRangeException("size");
            }
            if (m_RightEndPoint==null) {
                throw new InvalidOperationException(SR.GetString(SR.net_sockets_mustbind));
            }


            // Set up the result and set it to collect the context.
            ReceiveMessageOverlappedAsyncResult asyncResult = new ReceiveMessageOverlappedAsyncResult(this, state, callback);
            asyncResult.StartPostingAsyncOp(false);

            // Start the ReceiveFrom.
            EndPoint oldEndPoint = m_RightEndPoint;

            // This will check the permissions for connect.
            EndPoint endPointSnapshot = remoteEP;
            SocketAddress socketAddress = CheckCacheRemote(ref endPointSnapshot, false);

            // Guarantee to call CheckAsyncCallOverlappedResult if we call SetUnamangedStructures with a cache in order to
            // avoid a Socket leak in case of error.
            SocketError errorCode = SocketError.SocketError;
            try
            {
                asyncResult.SetUnmanagedStructures(buffer, offset, size, socketAddress, socketFlags, ref Caches.ReceiveOverlappedCache);

                // save a copy of the original EndPoint in the asyncResult
                asyncResult.SocketAddressOriginal = endPointSnapshot.Serialize();

                int bytesTransfered;

                // This can throw ObjectDisposedException.
                if (addressFamily == AddressFamily.InterNetwork)
                {
                    SetSocketOption(SocketOptionLevel.IP,SocketOptionName.PacketInformation,true);
                }
                else if (addressFamily == AddressFamily.InterNetworkV6){
                    SetSocketOption(SocketOptionLevel.IPv6,SocketOptionName.PacketInformation,true);
                }

                if (m_RightEndPoint == null)
                {
                    m_RightEndPoint = endPointSnapshot;
                }

                errorCode = (SocketError) WSARecvMsg(
                    m_Handle,
                    Marshal.UnsafeAddrOfPinnedArrayElement(asyncResult.m_MessageBuffer,0),
                    out bytesTransfered,
                    asyncResult.OverlappedHandle,
                    IntPtr.Zero);

                if (errorCode!=SocketError.Success) {
                    errorCode = (SocketError)Marshal.GetLastWin32Error();

                    // I have guarantees from Brad Williamson that WSARecvMsg() will never return WSAEMSGSIZE directly, since a completion
                    // is queued in this case.  We wouldn't be able to handle this easily because of assumptions OverlappedAsyncResult
                    // makes about whether there would be a completion or not depending on the error code.  If WSAEMSGSIZE would have been
                    // normally returned, it returns WSA_IO_PENDING instead.  That same map is implemented here just in case.
                    if (errorCode == SocketError.MessageSize)
                    {
                        GlobalLog.Assert("Socket#" + ValidationHelper.HashString(this) + "::BeginReceiveMessageFrom()|Returned WSAEMSGSIZE!");
                        errorCode = SocketError.IOPending;
                    }
                }

                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::BeginReceiveMessageFrom() UnsafeNclNativeMethods.OSSOCK.WSARecvMsg returns:" + errorCode.ToString() + " size:" + size.ToString() + " returning AsyncResult:" + ValidationHelper.HashString(asyncResult));
            }
            catch (ObjectDisposedException)
            {
                m_RightEndPoint = oldEndPoint;
                throw;
            }
            finally
            {
                errorCode = asyncResult.CheckAsyncCallOverlappedResult(errorCode);
            }

            //
            // if the asynchronous native call fails synchronously
            // we'll throw a SocketException
            //
            if (errorCode!=SocketError.Success)
            {
                //
                // update our internal state after this socket error and throw
                //
                m_RightEndPoint = oldEndPoint;
                asyncResult.ExtractCache(ref Caches.ReceiveOverlappedCache);
                SocketException socketException = new SocketException(errorCode);
                UpdateStatusAfterSocketError(socketException);
                if (Logging.On) Logging.Exception(Logging.Sockets, this, "BeginReceiveMessageFrom", socketException);
                throw socketException;
            }

            // Capture the context, maybe call the callback, and return.
            asyncResult.FinishPostingAsyncOp(ref Caches.ReceiveClosureCache);

            if (asyncResult.CompletedSynchronously && !asyncResult.SocketAddressOriginal.Equals(asyncResult.SocketAddress)) {
                try {
                    remoteEP = endPointSnapshot.Create(asyncResult.SocketAddress);
                }
                catch {
                }
            }

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::BeginReceiveMessageFrom() size:" + size.ToString() + " returning AsyncResult:" + ValidationHelper.HashString(asyncResult));
            if(Logging.On)Logging.Exit(Logging.Sockets, this, "BeginReceiveMessageFrom", asyncResult);
            return asyncResult;
        }


        public int EndReceiveMessageFrom(IAsyncResult asyncResult, ref SocketFlags socketFlags, ref EndPoint endPoint, out IPPacketInformation ipPacketInformation) {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "EndReceiveMessageFrom", asyncResult);
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            if (endPoint==null) {
                throw new ArgumentNullException("endPoint");
            }
            if (asyncResult==null) {
                throw new ArgumentNullException("asyncResult");
            }
            ReceiveMessageOverlappedAsyncResult castedAsyncResult = asyncResult as ReceiveMessageOverlappedAsyncResult;
            if (castedAsyncResult==null || castedAsyncResult.AsyncObject!=this) {
                throw new ArgumentException(SR.GetString(SR.net_io_invalidasyncresult), "asyncResult");
            }
            if (castedAsyncResult.EndCalled) {
                throw new InvalidOperationException(SR.GetString(SR.net_io_invalidendcall, "EndReceiveMessageFrom"));
            }

            int bytesTransferred = (int)castedAsyncResult.InternalWaitForCompletion();
            castedAsyncResult.EndCalled = true;
            castedAsyncResult.ExtractCache(ref Caches.ReceiveOverlappedCache);

            // Update socket address size
            castedAsyncResult.SocketAddress.SetSize(castedAsyncResult.GetSocketAddressSizePtr());

            // pick up the saved copy of the original EndPoint from the asyncResult
            SocketAddress socketAddressOriginal = endPoint.Serialize();
            
            if (!socketAddressOriginal.Equals(castedAsyncResult.SocketAddress)) {
                try {
                    endPoint = endPoint.Create(castedAsyncResult.SocketAddress);
                }
                catch {
                }
            }


            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::EndReceiveMessageFrom() bytesTransferred:" + bytesTransferred.ToString());

            //
            // if the asynchronous native call failed asynchronously
            // we'll throw a SocketException
            //
            if ((SocketError)castedAsyncResult.ErrorCode!=SocketError.Success && (SocketError)castedAsyncResult.ErrorCode != SocketError.MessageSize) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException(castedAsyncResult.ErrorCode);
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "EndReceiveMessageFrom", socketException);
                throw socketException;
            }

            socketFlags = castedAsyncResult.m_flags;
            ipPacketInformation = castedAsyncResult.m_IPPacketInformation;

            if(Logging.On)Logging.Exit(Logging.Sockets, this, "EndReceiveMessageFrom", bytesTransferred);
            return bytesTransferred;
        }



        /*++

        Routine Description:

           BeginReceiveFrom - Async implimentation of RecvFrom call,

           Called when we want to start an async receive.
           We kick off the receive, and if it completes synchronously we'll
           call the callback. Otherwise we'll return an IASyncResult, which
           the caller can use to wait on or retrieve the final status, as needed.

           Uses Winsock 2 overlapped I/O.

        Arguments:

           ReadBuffer - status line that we wish to parse
           Index - Offset into ReadBuffer to begin reading from
           Request - Size of Buffer to recv
           Flags - Additonal Flags that may be passed to the underlying winsock call
           remoteEP - EndPoint that are to receive from
           Callback - Delegate function that holds callback, called on completeion of I/O
           State - State used to track callback, set by caller, not required

        Return Value:

           IAsyncResult - Async result used to retreive result

        --*/

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [HostProtection(ExternalThreading=true)]
        public IAsyncResult BeginReceiveFrom(byte[] buffer, int offset, int size, SocketFlags socketFlags, ref EndPoint remoteEP, AsyncCallback callback, object state) {

            if (Logging.On) Logging.Enter(Logging.Sockets, this, "BeginReceiveFrom", "");

            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            //
            // parameter validation
            //
            if (buffer==null) {
                throw new ArgumentNullException("buffer");
            }
            if (remoteEP==null) {
                throw new ArgumentNullException("remoteEP");
            }
            if (offset<0 || offset>buffer.Length) {
                throw new ArgumentOutOfRangeException("offset");
            }
            if (size<0 || size>buffer.Length-offset) {
                throw new ArgumentOutOfRangeException("size");
            }
            if (m_RightEndPoint==null) {
                throw new InvalidOperationException(SR.GetString(SR.net_sockets_mustbind));
            }

            // This will check the permissions for connect.
            EndPoint endPointSnapshot = remoteEP;
            SocketAddress socketAddress = CheckCacheRemote(ref endPointSnapshot, false);

            // Set up the result and set it to collect the context.
            OverlappedAsyncResult asyncResult = new OverlappedAsyncResult(this, state, callback);
            asyncResult.StartPostingAsyncOp(false);

            // Start the ReceiveFrom.
            DoBeginReceiveFrom(buffer, offset, size, socketFlags, endPointSnapshot, socketAddress, asyncResult);

            // Capture the context, maybe call the callback, and return.
            asyncResult.FinishPostingAsyncOp(ref Caches.ReceiveClosureCache);

            if (asyncResult.CompletedSynchronously && !asyncResult.SocketAddressOriginal.Equals(asyncResult.SocketAddress)) {
                try {
                    remoteEP = endPointSnapshot.Create(asyncResult.SocketAddress);
                }
                catch {
                }
            }


            if(Logging.On)Logging.Exit(Logging.Sockets, this, "BeginReceiveFrom", asyncResult);
            return asyncResult;
        }

/* Uncomment when needed.
        internal IAsyncResult UnsafeBeginReceiveFrom(byte[] buffer, int offset, int size, SocketFlags socketFlags, ref EndPoint remoteEP, AsyncCallback callback, object state)
        {
            EndPoint endPointSnapshot = remoteEP;
            SocketAddress socketAddress = SnapshotAndSerialize(ref endPointSnapshot);

            // Set up the result, no need to collect the context.
            OverlappedAsyncResult asyncResult = new OverlappedAsyncResult(this, state, callback);
            DoBeginReceiveFrom(buffer, offset, size, socketFlags, endPointSnapshot, socketAddress, asyncResult);
            return asyncResult;
        }
*/

        private void DoBeginReceiveFrom(byte[] buffer, int offset, int size, SocketFlags socketFlags, EndPoint endPointSnapshot, SocketAddress socketAddress, OverlappedAsyncResult asyncResult)
        {
            EndPoint oldEndPoint = m_RightEndPoint;
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::DoBeginReceiveFrom() size:" + size.ToString());

            // Guarantee to call CheckAsyncCallOverlappedResult if we call SetUnamangedStructures with a cache in order to
            // avoid a Socket leak in case of error.
            SocketError errorCode = SocketError.SocketError;
            try
            {
                // Set up asyncResult for overlapped WSARecvFrom.
                // This call will use completion ports on WinNT and Overlapped IO on Win9x.
                asyncResult.SetUnmanagedStructures(buffer, offset, size, socketAddress, true /* pin remoteEP*/, ref Caches.ReceiveOverlappedCache);

                // save a copy of the original EndPoint in the asyncResult
                asyncResult.SocketAddressOriginal = endPointSnapshot.Serialize();

                if (m_RightEndPoint == null) {
                    m_RightEndPoint = endPointSnapshot;
                }

                int bytesTransferred;
                errorCode = UnsafeNclNativeMethods.OSSOCK.WSARecvFrom(
                    m_Handle,
                    ref asyncResult.m_SingleBuffer,
                    1,
                    out bytesTransferred,
                    ref socketFlags,
                    asyncResult.GetSocketAddressPtr(),
                    asyncResult.GetSocketAddressSizePtr(),
                    asyncResult.OverlappedHandle,
                    IntPtr.Zero );

                if (errorCode!=SocketError.Success) {
                    errorCode = (SocketError)Marshal.GetLastWin32Error();
                }
                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::DoBeginReceiveFrom() UnsafeNclNativeMethods.OSSOCK.WSARecvFrom returns:" + errorCode.ToString() + " size:" + size.ToString() + " returning AsyncResult:" + ValidationHelper.HashString(asyncResult));
            }
            catch (ObjectDisposedException)
            {
                m_RightEndPoint = oldEndPoint;
                throw;
            }
            finally
            {
                errorCode = asyncResult.CheckAsyncCallOverlappedResult(errorCode);
            }

            //
            // if the asynchronous native call fails synchronously
            // we'll throw a SocketException
            //
            if (errorCode!=SocketError.Success) {
                //
                // update our internal state after this socket error and throw
                //
                m_RightEndPoint = oldEndPoint;
                asyncResult.ExtractCache(ref Caches.ReceiveOverlappedCache);
                SocketException socketException = new SocketException(errorCode);
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "BeginReceiveFrom", socketException);
                throw socketException;
            }

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::DoBeginReceiveFrom() size:" + size.ToString() + " returning AsyncResult:" + ValidationHelper.HashString(asyncResult));
        }


        /*++

        Routine Description:

           EndReceiveFrom -  Called when I/O is done or the user wants to wait. If
                     the I/O isn't done, we'll wait for it to complete, and then we'll return
                     the bytes of I/O done.

        Arguments:

           AsyncResult - the AsyncResult Returned fron BeginReceiveFrom call

        Return Value:

           int - Number of bytes transferred

        --*/

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int EndReceiveFrom(IAsyncResult asyncResult, ref EndPoint endPoint) {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "EndReceiveFrom", asyncResult);
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            //
            // parameter validation
            //
            if (endPoint==null) {
                throw new ArgumentNullException("endPoint");
            }
            if (asyncResult==null) {
                throw new ArgumentNullException("asyncResult");
            }
            OverlappedAsyncResult castedAsyncResult = asyncResult as OverlappedAsyncResult;
            if (castedAsyncResult==null || castedAsyncResult.AsyncObject!=this) {
                throw new ArgumentException(SR.GetString(SR.net_io_invalidasyncresult), "asyncResult");
            }
            if (castedAsyncResult.EndCalled) {
                throw new InvalidOperationException(SR.GetString(SR.net_io_invalidendcall, "EndReceiveFrom"));
            }

            int bytesTransferred = (int)castedAsyncResult.InternalWaitForCompletion();
            castedAsyncResult.EndCalled = true;
            castedAsyncResult.ExtractCache(ref Caches.ReceiveOverlappedCache);

            // Update socket address size
            castedAsyncResult.SocketAddress.SetSize(castedAsyncResult.GetSocketAddressSizePtr());

            SocketAddress socketAddressOriginal = endPoint.Serialize();

            if (!socketAddressOriginal.Equals(castedAsyncResult.SocketAddress)) {
                try {
                    endPoint = endPoint.Create(castedAsyncResult.SocketAddress);
                }
                catch {
                }
            }


            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::EndReceiveFrom() bytesTransferred:" + bytesTransferred.ToString());

            //
            // if the asynchronous native call failed asynchronously
            // we'll throw a SocketException
            //
            if ((SocketError)castedAsyncResult.ErrorCode!=SocketError.Success) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException(castedAsyncResult.ErrorCode);
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "EndReceiveFrom", socketException);
                throw socketException;
            }
            if(Logging.On)Logging.Exit(Logging.Sockets, this, "EndReceiveFrom", bytesTransferred);
            return bytesTransferred;
        }


        /*++

        Routine Description:

           BeginAccept - Does a async winsock accept, creating a new socket on success

            Works by creating a pending accept request the first time,
            and subsequent calls are queued so that when the first accept completes,
            the next accept can be resubmitted in the callback.
            this routine may go pending at which time,
            but any case the callback Delegate will be called upon completion

        Arguments:

           Callback - Async Callback Delegate that is called upon Async Completion
           State - State used to track callback, set by caller, not required

        Return Value:

           IAsyncResult - Async result used to retreive resultant new socket

        --*/

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [HostProtection(ExternalThreading=true)]
        public IAsyncResult BeginAccept(AsyncCallback callback, object state) {

            if(Logging.On)Logging.Enter(Logging.Sockets, this, "BeginAccept", "");
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            // Set up the context flow.
            AcceptAsyncResult asyncResult = new AcceptAsyncResult(this, state, callback);
            asyncResult.StartPostingAsyncOp(false);

            // Do the accept.
            DoBeginAccept(asyncResult);

            // Set up for return.
            asyncResult.FinishPostingAsyncOp(ref Caches.AcceptClosureCache);

            if(Logging.On)Logging.Exit(Logging.Sockets, this, "BeginAccept", asyncResult);
            return asyncResult;
        }

/* Uncomment when needed.
        internal IAsyncResult UnsafeBeginAccept(AsyncCallback callback, object state)
        {
            if (CanUseAcceptEx)
            {
                return UnsafeBeginAccept(0, callback, state);
            }

            // Set up the async result.  Can use Lazy since there's no context flow.
            LazyAsyncResult asyncResult = new LazyAsyncResult(this, state, callback);
            DoBeginAccept(asyncResult);
            return asyncResult;
        }
*/

        private void DoBeginAccept(LazyAsyncResult asyncResult)
        {
            if (m_RightEndPoint==null) {
                throw new InvalidOperationException(SR.GetString(SR.net_sockets_mustbind));
            }

            if(!isListening){
                throw new InvalidOperationException(SR.GetString(SR.net_sockets_mustlisten));
            }

            //
            // We keep a queue, which lists the set of requests that want to
            //  be called when an accept queue completes.  We call accept
            //  once, and then as it completes asyncrounsly we pull the
            //  requests out of the queue and call their callback.
            //
            // We start by grabbing Critical Section, then attempt to
            //  determine if we haven an empty Queue of Accept Sockets
            //  or if its in a Callback on the Callback thread.
            //
            // If its in the callback thread proocessing of the callback, then we
            //  just need to notify the callback by adding an additional request
            //   to the queue.
            //
            // If its an empty queue, and its not in the callback, then
            //   we just need to get the Accept going, make it go async
            //   and leave.
            //
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::DoBeginAccept()");

            bool needFinishedCall = false;
            SocketError errorCode = 0;

            Queue acceptQueue = GetAcceptQueue();
            lock(this)
            {
                if (acceptQueue.Count == 0)
                {
                    SocketAddress socketAddress = m_RightEndPoint.Serialize();

                    GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::BeginAccept() queue is empty calling UnsafeNclNativeMethods.OSSOCK.accept");

                    // Check if a socket is already available.  We need to be non-blocking to do this.
                    InternalSetBlocking(false);

                    SafeCloseSocket acceptedSocketHandle = null;
                    try
                    {
                        acceptedSocketHandle = SafeCloseSocket.Accept(
                            m_Handle,
                            socketAddress.m_Buffer,
                            ref socketAddress.m_Size);
                        errorCode = acceptedSocketHandle.IsInvalid ? (SocketError) Marshal.GetLastWin32Error() : SocketError.Success;
                    }
                    catch (ObjectDisposedException)
                    {
                        errorCode = SocketError.NotSocket;
                    }

                    GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::BeginAccept() UnsafeNclNativeMethods.OSSOCK.accept returns:" + errorCode.ToString());

                    if (errorCode != SocketError.WouldBlock)
                    {
                        if (errorCode == SocketError.Success)
                        {
                            asyncResult.Result = CreateAcceptSocket(acceptedSocketHandle, m_RightEndPoint.Create(socketAddress), false);
                        }
                        else
                        {
                            asyncResult.ErrorCode = (int) errorCode;
                        }

                        // Reset the blocking.
                        InternalSetBlocking(true);

                        // Continue outside the lock.
                        needFinishedCall = true;
                    }
                    else
                    {
                        // It would block.  Start listening for accepts, and add ourselves to the queue.
                        acceptQueue.Enqueue(asyncResult);
                        if (!SetAsyncEventSelect(AsyncEventBits.FdAccept))
                        {
                            acceptQueue.Dequeue();
                            throw new ObjectDisposedException(this.GetType().FullName);
                        }
                    }
                }
                else {
                    acceptQueue.Enqueue(asyncResult);

                    GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::DoBeginAccept() queue is not empty Count:" + acceptQueue.Count.ToString());
                }
            }

            if (needFinishedCall) {
                if (errorCode == SocketError.Success)
                {
                    // Completed synchronously, invoke the callback.
                    asyncResult.InvokeCallback();
                }
                else
                {
                    //
                    // update our internal state after this socket error and throw
                    //
                    SocketException socketException = new SocketException(errorCode);
                    UpdateStatusAfterSocketError(socketException);
                    if(Logging.On)Logging.Exception(Logging.Sockets, this, "BeginAccept", socketException);
                    throw socketException;
                }
            }

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::DoBeginAccept() returning AsyncResult:" + ValidationHelper.HashString(asyncResult));
        }

        //
        // This is a shortcut to AcceptCallback when called from dispose.
        // The only business is lock and complete all results with an error
        //
        private void CompleteAcceptResults(object nullState)
        {
            Queue acceptQueue = GetAcceptQueue();
            bool acceptNeeded = true;
            while (acceptNeeded)
            {
                LazyAsyncResult asyncResult = null;
                lock (this)
                {
                    // If the queue is empty, cancel the select and indicate not to loop anymore.
                    if (acceptQueue.Count == 0)
                        break;
                    asyncResult = (LazyAsyncResult) acceptQueue.Dequeue();

                    if (acceptQueue.Count == 0)
                        acceptNeeded = false;
                }

                // Notify about the completion outside the lock.
                try {
                    asyncResult.InvokeCallback(new SocketException(SocketError.OperationAborted));
                }
                catch {
                    // Exception from the user callback,
                    // If we need to loop, offload to a different thread and re-throw for debugging
                    if (acceptNeeded)
                        ThreadPool.UnsafeQueueUserWorkItem(new WaitCallback(CompleteAcceptResults), null);

                    throw;
                }
            }
        }

        // This method was originally in an AcceptAsyncResult class but that class got useless.
        private void AcceptCallback(object nullState)
        {
            // We know we need to try completing an accept at first.  Keep going until the queue is empty (inside the lock).
            // At that point, BeginAccept() takes control of restarting the pump if necessary.
            bool acceptNeeded = true;
            Queue acceptQueue = GetAcceptQueue();

            while (acceptNeeded)
            {
                LazyAsyncResult asyncResult = null;
                SocketError errorCode = SocketError.OperationAborted;
                SocketAddress socketAddress = null;
                SafeCloseSocket acceptedSocket = null;
                Exception otherException = null;
                object result = null;

                lock (this)
                {
                    //
                    // Accept Callback - called on the callback path, when we expect to release
                    //  an accept socket that winsock says has completed.
                    //
                    //  While we still have items in our Queued list of Accept Requests,
                    //   we recall the Winsock accept, to attempt to gather new
                    //   results, and then match them again the queued items,
                    //   when accept call returns would_block, we reinvoke ourselves
                    //   and rewait for the next asyc callback.
                    //

                    //
                    // We may not have items in the queue because of possible race
                    // between re-entering this callback manually and from the thread pool.
                    //
                    if (acceptQueue.Count == 0)
                        break;

                    // pick an element from the head of the list
                    asyncResult = (LazyAsyncResult) acceptQueue.Peek();

                    if (!CleanedUp)
                    {
                        socketAddress = m_RightEndPoint.Serialize();

                        try
                        {
                            // We know we're in non-blocking because of SetAsyncEventSelect().
                            GlobalLog.Assert(!willBlockInternal, "Socket#{0}::AcceptCallback|Socket should be in non-blocking state.", ValidationHelper.HashString(this));
                            acceptedSocket = SafeCloseSocket.Accept(
                                m_Handle,
                                socketAddress.m_Buffer,
                                ref socketAddress.m_Size);

                            errorCode = acceptedSocket.IsInvalid ? (SocketError) Marshal.GetLastWin32Error() : SocketError.Success;

                            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::AcceptCallback() UnsafeNclNativeMethods.OSSOCK.accept returns:" + errorCode.ToString());
                        }
                        catch (ObjectDisposedException)
                        {
                            // Listener socket was closed.
                            errorCode = SocketError.OperationAborted;
                        }
                        catch (Exception exception)
                        {
                            if (NclUtilities.IsFatal(exception)) throw;

                            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::AcceptCallback() caught exception:" + exception.Message + " CleanedUp:" + CleanedUp);
                            otherException = exception;
                        }
                        catch
                        {
                            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::AcceptCallback() caught exception: Non-CLS Compliant Exception" + " CleanedUp:" + CleanedUp);
                            otherException = new Exception(SR.GetString(SR.net_nonClsCompliantException));
                        }
                    }

                    if (errorCode == SocketError.WouldBlock && otherException == null)
                    {
                        // The accept found no waiting connections, so start listening for more.
                        if (SetAsyncEventSelect(AsyncEventBits.FdAccept))
                            break;
                        otherException = new ObjectDisposedException(this.GetType().FullName);
                    }

                    // CreateAcceptSocket() must be done before InternalSetBlocking() so that the fixup is correct inside
                    // UpdateAcceptSocket().  InternalSetBlocking() must happen in the lock.
                    if (otherException != null)
                    {
                        result = otherException;
                    }
                    else if (errorCode == SocketError.Success)
                    {
                        result = CreateAcceptSocket(acceptedSocket, m_RightEndPoint.Create(socketAddress), true);
                    }
                    else
                    {
                        asyncResult.ErrorCode = (int) errorCode;
                    }

                    // This request completed, so it can be taken off the queue.
                    acceptQueue.Dequeue();

                    // If the queue is empty, cancel the select and indicate not to loop anymore.
                    if (acceptQueue.Count == 0)
                    {
                        if (!CleanedUp)
                            UnsetAsyncEventSelect();
                        InternalSetBlocking(true);
                        acceptNeeded = false;
                    }
                }

                // Notify about the completion outside the lock.
                try {
                    asyncResult.InvokeCallback(result);
                }
                catch {
                    // Exception from the user callback,
                    // If we need to loop, offload to a different thread and re-throw for debugging
                    if (acceptNeeded)
                        ThreadPool.UnsafeQueueUserWorkItem(new WaitCallback(AcceptCallback), nullState);

                    throw;
                }
            }
        }



        /*++

        Routine Description:

           EndAccept -  Called by user code addressFamilyter I/O is done or the user wants to wait.
                        until Async completion, so it provides End handling for aync Accept calls,
                        and retrieves new Socket object

        Arguments:

           AsyncResult - the AsyncResult Returned fron BeginAccept call

        Return Value:

           Socket - a valid socket if successful

        --*/

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>



        public Socket EndAccept(IAsyncResult asyncResult) {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "EndAccept", asyncResult);
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }


            //
            // parameter validation
            //
            if (asyncResult==null) {
                throw new ArgumentNullException("asyncResult");
            }

            AcceptAsyncResult castedAsyncResult = asyncResult as AcceptAsyncResult;
            if (castedAsyncResult==null || castedAsyncResult.AsyncObject!=this) {
                throw new ArgumentException(SR.GetString(SR.net_io_invalidasyncresult), "asyncResult");
            }
            if (castedAsyncResult.EndCalled) {
                throw new InvalidOperationException(SR.GetString(SR.net_io_invalidendcall, "EndAccept"));
            }

            object result = castedAsyncResult.InternalWaitForCompletion();
            castedAsyncResult.EndCalled = true;

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::EndAccept() acceptedSocket:" + ValidationHelper.HashString(result));

            //
            // if the asynchronous native call failed asynchronously
            // we'll throw a SocketException
            //
            Exception exception = result as Exception;
            if (exception != null)
            {
                throw exception;
            }

            if ((SocketError)castedAsyncResult.ErrorCode!=SocketError.Success) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException(castedAsyncResult.ErrorCode);
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "EndAccept", socketException);
                throw socketException;
            }

            if(Logging.On)Logging.Exit(Logging.Sockets, this, "EndAccept", result);
            return (Socket) result;
        }






        /// <devdoc>
        ///    <para>
        ///       Disables sends and receives on a socket.
        ///    </para>
        /// </devdoc>
        public void Shutdown(SocketShutdown how) {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "Shutdown", how);
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::Shutdown() how:" + how.ToString());

            // This can throw ObjectDisposedException.
            SocketError errorCode = UnsafeNclNativeMethods.OSSOCK.shutdown(m_Handle, (int) how);

            //
            // if the native call fails we'll throw a SocketException
            //
            errorCode = errorCode!=SocketError.SocketError ? SocketError.Success : (SocketError)Marshal.GetLastWin32Error();

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::Shutdown() UnsafeNclNativeMethods.OSSOCK.shutdown returns errorCode:" + errorCode);

            //
            // skip good cases: success, socket already closed
            //
            if (errorCode!=SocketError.Success && errorCode!=SocketError.NotSocket) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException(errorCode);
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "Shutdown", socketException );
                throw socketException;
            }

            SetToDisconnected();
            InternalSetBlocking(willBlockInternal);
            if(Logging.On)Logging.Exit(Logging.Sockets, this, "Shutdown", "");
        }



//************* internal and private properties *************************

        private static object InternalSyncObject {
            get {
                if (s_InternalSyncObject == null) {
                    object o = new object();
                    Interlocked.CompareExchange(ref s_InternalSyncObject, o, null);
                }
                return s_InternalSyncObject;
            }
        }

        private CacheSet Caches
        {
            get
            {
                if (m_Caches == null)
                {
                    // It's not too bad if extra of these are created and lost.
                    m_Caches = new CacheSet();
                }
                return m_Caches;
            }
        }

        private DisconnectExDelegate DisconnectEx{
            get{
                //get the disconnect api
                if (s_DisconnectEx == null) {
                    lock(InternalSyncObject) {
                        if (s_DisconnectEx == null) {
                            LoadDisconnectEx();
                        }
                    }
                }
                return s_DisconnectEx;
            }
        }

        private DisconnectExDelegate_Blocking DisconnectEx_Blocking
        {
            get
            {
                //get the disconnect api
                if (s_DisconnectEx_Blocking == null)
                {
                    lock (InternalSyncObject)
                    {
                        if (s_DisconnectEx_Blocking == null)
                        {
                            LoadDisconnectEx();
                        }
                    }
                }
                return s_DisconnectEx_Blocking;
            }
        }

        private void LoadDisconnectEx()
        {
            SocketError errorCode;
            IntPtr ptrDisconnectEx = IntPtr.Zero;

            unsafe
            {
                int length;
                Guid guid = new Guid("{0x7fda2e11,0x8630,0x436f,{0xa0, 0x31, 0xf5, 0x36, 0xa6, 0xee, 0xc1, 0x57}}");

                // This can throw ObjectDisposedException.
                errorCode = UnsafeNclNativeMethods.OSSOCK.WSAIoctl(
                    m_Handle,
                    IoctlSocketConstants.SIOGETEXTENSIONFUNCTIONPOINTER,
                    ref guid,
                    sizeof(Guid),
                    out ptrDisconnectEx,
                    sizeof(IntPtr),
                    out length,
                    IntPtr.Zero,
                    IntPtr.Zero);
            }

            if (errorCode != SocketError.Success)
            {
                throw new SocketException();
            }
            s_DisconnectEx = (DisconnectExDelegate) Marshal.GetDelegateForFunctionPointer(ptrDisconnectEx, typeof(DisconnectExDelegate));
            s_DisconnectEx_Blocking = (DisconnectExDelegate_Blocking)Marshal.GetDelegateForFunctionPointer(ptrDisconnectEx, typeof(DisconnectExDelegate_Blocking));
        }

        private ConnectExDelegate ConnectEx{
            get{
                //get the disconnect api
                if (s_ConnectEx == null) {
                    lock(InternalSyncObject) {
                        if (s_ConnectEx == null) {

                            SocketError errorCode;
                            IntPtr ptrConnectEx = IntPtr.Zero;

                            unsafe {
                                int length;
                                Guid guid = new Guid("{0x25a207b9,0x0ddf3,0x4660,{0x8e,0xe9,0x76,0xe5,0x8c,0x74,0x06,0x3e}}");

                                // This can throw ObjectDisposedException.
                                errorCode = UnsafeNclNativeMethods.OSSOCK.WSAIoctl(
                                    m_Handle,
                                    IoctlSocketConstants.SIOGETEXTENSIONFUNCTIONPOINTER,
                                    ref guid,
                                    sizeof(Guid),
                                    out ptrConnectEx,
                                    sizeof(IntPtr),
                                    out length,
                                    IntPtr.Zero,
                                    IntPtr.Zero);
                            }

                            if (errorCode != SocketError.Success) {
                                throw new SocketException();
                            }
                            s_ConnectEx = (ConnectExDelegate)Marshal.GetDelegateForFunctionPointer(ptrConnectEx, typeof(ConnectExDelegate));
                        }
                    }
                }
                return s_ConnectEx;
            }
        }


        private WSARecvMsgDelegate WSARecvMsg{
            get{
                //get the disconnect api
                if (s_WSARecvMsg == null) {
                    lock(InternalSyncObject) {
                        if (s_WSARecvMsg == null) {
                            LoadWSARecvMsg();
                        }
                    }
                }
                return s_WSARecvMsg;
            }
        }

        private WSARecvMsgDelegate_Blocking WSARecvMsg_Blocking
        {
            get
            {
                //get the disconnect api
                if (s_WSARecvMsg_Blocking == null)
                {
                    lock (InternalSyncObject)
                    {
                        if (s_WSARecvMsg_Blocking == null)
                        {
                            LoadWSARecvMsg();
                        }
                    }
                }
                return s_WSARecvMsg_Blocking;
            }
        }

        private void LoadWSARecvMsg()
        {
            SocketError errorCode;
            IntPtr ptrWSARecvMsg = IntPtr.Zero;

            unsafe
            {
                int length;
                Guid guid = new Guid("{0xf689d7c8,0x6f1f,0x436b,{0x8a,0x53,0xe5,0x4f,0xe3,0x51,0xc3,0x22}}");

                // This can throw ObjectDisposedException.
                errorCode = UnsafeNclNativeMethods.OSSOCK.WSAIoctl(
                    m_Handle,
                    IoctlSocketConstants.SIOGETEXTENSIONFUNCTIONPOINTER,
                    ref guid,
                    sizeof(Guid),
                    out ptrWSARecvMsg,
                    sizeof(IntPtr),
                    out length,
                    IntPtr.Zero,
                    IntPtr.Zero);
            }

            if (errorCode != SocketError.Success)
            {
                throw new SocketException();
            }
            s_WSARecvMsg = (WSARecvMsgDelegate) Marshal.GetDelegateForFunctionPointer(ptrWSARecvMsg, typeof(WSARecvMsgDelegate));
            s_WSARecvMsg_Blocking = (WSARecvMsgDelegate_Blocking) Marshal.GetDelegateForFunctionPointer(ptrWSARecvMsg, typeof(WSARecvMsgDelegate_Blocking));
        }


        private Queue GetAcceptQueue() {
            if (m_AcceptQueueOrConnectResult == null)
                Interlocked.CompareExchange(ref m_AcceptQueueOrConnectResult, new Queue(16), null);
            return (Queue)m_AcceptQueueOrConnectResult;
        }

        internal bool CleanedUp {
            get {
                return (m_IntCleanedUp == 1);
            }
        }

        internal TransportType Transport {
            get {
                return
                    protocolType==Sockets.ProtocolType.Tcp ?
                        TransportType.Tcp :
                        protocolType==Sockets.ProtocolType.Udp ?
                            TransportType.Udp :
                            TransportType.All;
            }
        }


//************* internal and private methods *************************



    private void CheckSetOptionPermissions(SocketOptionLevel optionLevel, SocketOptionName optionName) {
            // freely allow only those below
            if (  !(optionLevel == SocketOptionLevel.Tcp &&
                  (optionName == SocketOptionName.NoDelay   ||
                   optionName == SocketOptionName.BsdUrgent ||
                   optionName == SocketOptionName.Expedited))
                  &&
                  !(optionLevel == SocketOptionLevel.Udp &&
                    (optionName == SocketOptionName.NoChecksum||
                     optionName == SocketOptionName.ChecksumCoverage))
                  &&
                  !(optionLevel == SocketOptionLevel.Socket &&
                  (optionName == SocketOptionName.KeepAlive     ||
                   optionName == SocketOptionName.Linger        ||
                   optionName == SocketOptionName.DontLinger    ||
                   optionName == SocketOptionName.SendBuffer    ||
                   optionName == SocketOptionName.ReceiveBuffer ||
                   optionName == SocketOptionName.SendTimeout   ||
                   optionName == SocketOptionName.ExclusiveAddressUse   ||
                   optionName == SocketOptionName.ReceiveTimeout))
                  &&
                  //ipv6 protection level
                  !(optionLevel == SocketOptionLevel.IPv6 &&
                    optionName == (SocketOptionName)23)){

                ExceptionHelper.UnmanagedPermission.Demand();
            }
        }

        [SuppressUnmanagedCodeSecurity]
        delegate bool ConnectExDelegate(SafeCloseSocket socketHandle,
                        IntPtr  socketAddress,
                        int socketAddressSize,
                        IntPtr      buffer,
                        int dataLength,
                        int bytesSent,
                        IntPtr overlapped);

        [SuppressUnmanagedCodeSecurity]
        delegate bool DisconnectExDelegate(SafeCloseSocket socketHandle, IntPtr overlapped, int flags, int reserved);

        [SuppressUnmanagedCodeSecurity]
        delegate bool DisconnectExDelegate_Blocking(IntPtr socketHandle, IntPtr overlapped, int flags, int reserved);

        [SuppressUnmanagedCodeSecurity]
        unsafe delegate SocketError WSARecvMsgDelegate(SafeCloseSocket socketHandle,
                    IntPtr msg,
                    out int bytesTransferred,
                    IntPtr overlapped,
                    IntPtr completionRoutine
        );

        [SuppressUnmanagedCodeSecurity]
        unsafe delegate SocketError WSARecvMsgDelegate_Blocking(IntPtr socketHandle,
                    IntPtr msg,
                    out int bytesTransferred,
                    IntPtr overlapped,
                    IntPtr completionRoutine
        );

        private SocketAddress SnapshotAndSerialize(ref EndPoint remoteEP)
        {
            IPEndPoint ipSnapshot = remoteEP as IPEndPoint;

            if (ipSnapshot != null)
            {
                ipSnapshot = ipSnapshot.Snapshot();
                remoteEP = ipSnapshot;
            }

            return remoteEP.Serialize();
        }

        //
        // socketAddress must always be the result of remoteEP.Serialize()
        //
        private SocketAddress CheckCacheRemote(ref EndPoint remoteEP, bool isOverwrite)
        {
            IPEndPoint ipSnapshot = remoteEP as IPEndPoint;

            if (ipSnapshot != null)
            {
                ipSnapshot = ipSnapshot.Snapshot();
                remoteEP = ipSnapshot;
            }

            // This doesn't use SnapshotAndSerialize() because we need the ipSnapshot later.
            SocketAddress socketAddress = remoteEP.Serialize();

            // We remember the first peer we have communicated with
            SocketAddress permittedRemoteAddress = m_PermittedRemoteAddress;
            if (permittedRemoteAddress != null && permittedRemoteAddress.Equals(socketAddress))
            {
                return permittedRemoteAddress;
            }

            //
            // for now SocketPermission supports only IPEndPoint
            //
            if (ipSnapshot != null)
            {
                //
                // create the permissions the user would need for the call
                //
                SocketPermission socketPermission
                    = new SocketPermission(
                        NetworkAccess.Connect,
                        Transport,
                        ipSnapshot.Address.ToString(),
                        ipSnapshot.Port);
                //
                // demand for them
                //
                socketPermission.Demand();
            }
            else {
                //
                // for V1 we will demand permission to run UnmanagedCode for
                // an EndPoint that is not an IPEndPoint until we figure out how these fit
                // into the whole picture of SocketPermission
                //

                ExceptionHelper.UnmanagedPermission.Demand();
            }
            //cache only the first peer we communicated with
            if (m_PermittedRemoteAddress == null || isOverwrite) {
                m_PermittedRemoteAddress = socketAddress;
            }

            return socketAddress;
        }


        internal static void InitializeSockets() {
            if (!s_Initialized) {
                lock(InternalSyncObject){
                    if (!s_Initialized) {

                        WSAData wsaData = new WSAData();

                        SocketError errorCode =
                            UnsafeNclNativeMethods.OSSOCK.WSAStartup(
                                (short)0x0202, // we need 2.2
                                out wsaData );

                        if (errorCode!=SocketError.Success) {
                            //
                            // failed to initialize, throw
                            //
                            throw new SocketException();
                        }


                        s_SupportsIPv4 = true;
                        s_SupportsIPv6 = false;


                        s_Initialized = true;
                    }
                }
            }
        }


        internal void InternalConnect(EndPoint remoteEP)
        {
            EndPoint endPointSnapshot = remoteEP;
            SocketAddress socketAddress = SnapshotAndSerialize(ref endPointSnapshot);
            DoConnect(endPointSnapshot, socketAddress);
        }

        private void DoConnect(EndPoint endPointSnapshot, SocketAddress socketAddress)
        {
            if (Logging.On) Logging.Enter(Logging.Sockets, this, "Connect", endPointSnapshot);

            // This can throw ObjectDisposedException.
            SocketError errorCode = UnsafeNclNativeMethods.OSSOCK.WSAConnect(
                m_Handle.DangerousGetHandle(),
                socketAddress.m_Buffer,
                socketAddress.m_Size,
                IntPtr.Zero,
                IntPtr.Zero,
                IntPtr.Zero,
                IntPtr.Zero);


            //
            // if the native call fails we'll throw a SocketException
            //
            if (errorCode!=SocketError.Success) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException();
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "Connect", socketException);
                throw socketException;
            }

            if (m_RightEndPoint==null) {
                //
                // save a copy of the EndPoint so we can use it for Create()
                //
                m_RightEndPoint = endPointSnapshot;
            }

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::DoConnect() connection to:" + endPointSnapshot.ToString());

            //
            // update state and performance counter
            //
            SetToConnected();
            if (Logging.On) Logging.Exit(Logging.Sockets, this, "Connect", "");
        }


        protected virtual void Dispose(bool disposing)
        {
            //we should swallow exceptions in the logging code for Dispose.
            try{
                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::Dispose() disposing:" + disposing.ToString() + " CleanedUp:" + CleanedUp.ToString());
                if(Logging.On)Logging.Enter(Logging.Sockets, this, "Dispose", null);
            }
            catch (Exception exception)
            {
                if (NclUtilities.IsFatal(exception)) throw;
            }

            // For finalization, we should close the handle if it's still alive, and nothing else.  Use abortive close to not block
            // the finalizer thread.
            if (!disposing)
            {
                if (m_Handle != null && !m_Handle.IsInvalid)
                {
                    GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::Dispose() Calling m_Handle.Dispose()");
                    m_Handle.Dispose();
                }
                return;
            }

            // make sure we're the first call to Dispose and no SetAsyncEventSelect is in progress
            int last;
            while ((last = Interlocked.CompareExchange(ref m_IntCleanedUp, 1, 0)) == 2)
            {
                Thread.SpinWait(1);
            }
            if (last == 1)
            {
                try {
                    if(Logging.On)Logging.Exit(Logging.Sockets, this, "Dispose", null);
                }
                catch (Exception exception)
                {
                    if (NclUtilities.IsFatal(exception)) throw;
                }
                return;
            }

            SetToDisconnected();

            AsyncEventBits pendingAsync = AsyncEventBits.FdNone;
            if (m_BlockEventBits != AsyncEventBits.FdNone)
            {
                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::Dispose() Pending nonblocking operations!  m_BlockEventBits:" + m_BlockEventBits.ToString());
                UnsetAsyncEventSelect();
                if (m_BlockEventBits == AsyncEventBits.FdConnect)
                {
                    LazyAsyncResult connectResult = m_AcceptQueueOrConnectResult as LazyAsyncResult;
                    if (connectResult != null && !connectResult.InternalPeekCompleted)
                        pendingAsync = AsyncEventBits.FdConnect;
                }
                else if (m_BlockEventBits == AsyncEventBits.FdAccept)
                {
                    Queue acceptQueue = m_AcceptQueueOrConnectResult as Queue;
                    if (acceptQueue != null && acceptQueue.Count != 0)
                        pendingAsync = AsyncEventBits.FdAccept;
                }
            }

            // Close the handle in one of several ways depending on the timeout.
            // Swallow ObjectDisposedException just in case the handle somehow gets disposed elsewhere.
            try
            {
                int timeout = m_CloseTimeout;
                if (timeout == 0)
                {
                    // Abortive.
                    GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::Dispose() Calling m_Handle.Dispose()");
                    m_Handle.Dispose();
                }
                else
                {
                    SocketError errorCode;

                    // Go to blocking mode.  We know no WSAEventSelect is pending because of the lock and UnsetAsyncEventSelect() above.
                    if (!willBlock || !willBlockInternal)
                    {
                        int nonBlockCmd = 0;
                        errorCode = UnsafeNclNativeMethods.OSSOCK.ioctlsocket(
                            m_Handle,
                            IoctlSocketConstants.FIONBIO,
                            ref nonBlockCmd);
                        GlobalLog.Print("SafeCloseSocket::Dispose(handle:" + m_Handle.DangerousGetHandle().ToString("x") + ") ioctlsocket(FIONBIO):" + (errorCode == SocketError.SocketError ? (SocketError) Marshal.GetLastWin32Error() : errorCode).ToString());
                    }

                    if (timeout < 0)
                    {
                        // Close with existing user-specified linger option.
                        GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::Dispose() Calling m_Handle.CloseAsIs()");
                        m_Handle.CloseAsIs();
                    }
                    else
                    {
                        // Since our timeout is in ms and linger is in seconds, implement our own sortof linger here.
                        errorCode = UnsafeNclNativeMethods.OSSOCK.shutdown(m_Handle, (int) SocketShutdown.Send);
                        GlobalLog.Print("SafeCloseSocket::Dispose(handle:" + m_Handle.DangerousGetHandle().ToString("x") + ") shutdown():" + (errorCode == SocketError.SocketError ? (SocketError) Marshal.GetLastWin32Error() : errorCode).ToString());

                        // This should give us a timeout in milliseconds.
                        errorCode = UnsafeNclNativeMethods.OSSOCK.setsockopt(
                            m_Handle,
                            SocketOptionLevel.Socket,
                            SocketOptionName.ReceiveTimeout,
                            ref timeout,
                            sizeof(int));
                        GlobalLog.Print("SafeCloseSocket::Dispose(handle:" + m_Handle.DangerousGetHandle().ToString("x") + ") setsockopt():" + (errorCode == SocketError.SocketError ? (SocketError) Marshal.GetLastWin32Error() : errorCode).ToString());

                        if (errorCode != SocketError.Success)
                        {
                            m_Handle.Dispose();
                        }
                        else
                        {
                            unsafe
                            {
                                errorCode = (SocketError) UnsafeNclNativeMethods.OSSOCK.recv(m_Handle.DangerousGetHandle(), null, 0, SocketFlags.None);
                            }
                            GlobalLog.Print("SafeCloseSocket::Dispose(handle:" + m_Handle.DangerousGetHandle().ToString("x") + ") recv():" + errorCode.ToString());

                            if (errorCode != (SocketError) 0)
                            {
                                // We got a timeout - abort.
                                m_Handle.Dispose();
                            }
                            else
                            {
                                // We got a FIN or data.  Use ioctlsocket to find out which.
                                int dataAvailable = 0;
                                errorCode = UnsafeNclNativeMethods.OSSOCK.ioctlsocket(
                                    m_Handle,
                                    IoctlSocketConstants.FIONREAD,
                                    ref dataAvailable);
                                GlobalLog.Print("SafeCloseSocket::Dispose(handle:" + m_Handle.DangerousGetHandle().ToString("x") + ") ioctlsocket(FIONREAD):" + (errorCode == SocketError.SocketError ? (SocketError) Marshal.GetLastWin32Error() : errorCode).ToString());

                                if (errorCode != SocketError.Success || dataAvailable != 0)
                                {
                                    // If we have data or don't know, safest thing is to reset.
                                    m_Handle.Dispose();
                                }
                                else
                                {
                                    // We got a FIN.  It'd be nice to block for the remainder of the timeout for the handshake to finsh.
                                    // Since there's no real way to do that, close the socket with the user's preferences.  This lets
                                    // the user decide how best to handle this case via the linger options.
                                    m_Handle.CloseAsIs();
                                }
                            }
                        }
                    }
                }
            }
            catch (ObjectDisposedException)
            {
                GlobalLog.Assert("SafeCloseSocket::Dispose(handle:" + m_Handle.DangerousGetHandle().ToString("x") + ")", "Closing the handle threw ObjectDisposedException.");
            }

#if !DEBUG
            // Clear out the Overlapped caches.
            if (m_Caches != null)
            {
                OverlappedCache.InterlockedFree(ref m_Caches.SendOverlappedCache);
                OverlappedCache.InterlockedFree(ref m_Caches.ReceiveOverlappedCache);
            }
#endif

            if (pendingAsync == AsyncEventBits.FdConnect)
            {
                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::Dispose() QueueUserWorkItem for ConnectCallback");
                // This will try to complete connectResult on a different thread
                ThreadPool.UnsafeQueueUserWorkItem(new WaitCallback(((LazyAsyncResult)m_AcceptQueueOrConnectResult).InvokeCallback), new SocketException(SocketError.OperationAborted));
            }
            else if (pendingAsync == AsyncEventBits.FdAccept)
            {
                // This will try to complete all acceptResults on a different thread
                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::Dispose() QueueUserWorkItem for AcceptCallback");
                ThreadPool.UnsafeQueueUserWorkItem(new WaitCallback(CompleteAcceptResults), null);
            }

            if (m_AsyncEvent != null)
            {
                m_AsyncEvent.Close();
            }
        }

        /// <internalonly/>
        void IDisposable.Dispose() {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        ~Socket() {
#if DEBUG
            GlobalLog.SetThreadSource(ThreadKinds.Finalization);
          //  using (GlobalLog.SetThreadKind(ThreadKinds.System | ThreadKinds.Async)) {
#endif
            Dispose(false);
#if DEBUG
            //}
#endif
        }

        // this version does not throw.
        internal void InternalShutdown(SocketShutdown how) {
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::InternalShutdown() how:" + how.ToString());

            if (CleanedUp || m_Handle.IsInvalid) {
                return;
            }

            try
            {
                UnsafeNclNativeMethods.OSSOCK.shutdown(m_Handle, (int)how);
            }
            catch (ObjectDisposedException) { }
        }


        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        private void DownLevelSendFile(string fileName){
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "SendFile", "");

            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            if (!Connected) {
                throw new NotSupportedException(SR.GetString(SR.net_notconnected));
            }

            ValidateBlockingMode();
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::SendFile() SRC:" + ValidationHelper.ToString(LocalEndPoint) + " DST:" + ValidationHelper.ToString(RemoteEndPoint) + " fileName:" + fileName);

            FileStream fileStream = null;
            if (fileName != null && fileName.Length>0) {
                fileStream = new FileStream(fileName,FileMode.Open,FileAccess.Read,FileShare.Read);
            }

            try
            {
                SocketError errorCode = SocketError.Success;

                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::SendFile() DST:" + ValidationHelper.ToString(RemoteEndPoint) + " UnsafeNclNativeMethods.OSSOCK.TransmitFile returns errorCode:" + errorCode);

                //
                // if the native call fails we'll throw a SocketException
                //
                byte[] buffer = new byte[64000];
                while(true){
                    int read = fileStream.Read(buffer,0,buffer.Length);
                    if (read == 0) {
                        break;
                    }
                    Send(buffer,0,read,SocketFlags.None);
                }
                if(Logging.On)Logging.Exit(Logging.Sockets, this, "SendFile", errorCode);
            }
            finally
            {
                DownLevelSendFileCleanup(fileStream);
            }
            return;
        }

        internal unsafe void SetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, int optionValue, bool silent) {
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::SetSocketOption() optionLevel:" + optionLevel + " optionName:" + optionName + " optionValue:" + optionValue + " silent:" + silent);
            if (silent && (CleanedUp || m_Handle.IsInvalid)) {
                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::SetSocketOption() skipping the call");
                return;
            }
            SocketError errorCode = SocketError.Success;
            try {
                // This can throw ObjectDisposedException.
                errorCode = UnsafeNclNativeMethods.OSSOCK.setsockopt(
                    m_Handle,
                    optionLevel,
                    optionName,
                    ref optionValue,
                    sizeof(int));

                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::SetSocketOption() UnsafeNclNativeMethods.OSSOCK.setsockopt returns errorCode:" + errorCode);
            }
            catch {
                if (silent && m_Handle.IsInvalid) {
                    return;
                }
                throw;
            }
            if (silent) {
                return;
            }

            //
            // if the native call fails we'll throw a SocketException
            //
            if (errorCode==SocketError.SocketError) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException();
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "SetSocketOption", socketException);
                throw socketException;
            }
        }

        private void setMulticastOption(SocketOptionName optionName, MulticastOption MR) {
            IPMulticastRequest ipmr = new IPMulticastRequest();

            ipmr.MulticastAddress = unchecked((int)MR.Group.m_Address);


            if(MR.LocalAddress != null){
                ipmr.InterfaceAddress = unchecked((int)MR.LocalAddress.m_Address);
            }
            else {  //this structure works w/ interfaces as well
                int ifIndex =IPAddress.HostToNetworkOrder(MR.InterfaceIndex);
                ipmr.InterfaceAddress   = unchecked((int)ifIndex);
            }

#if BIGENDIAN
            ipmr.MulticastAddress = (int) (((uint) ipmr.MulticastAddress << 24) |
                                           (((uint) ipmr.MulticastAddress & 0x0000FF00) << 8) |
                                           (((uint) ipmr.MulticastAddress >> 8) & 0x0000FF00) |
                                           ((uint) ipmr.MulticastAddress >> 24));

            if(MR.LocalAddress != null){
                ipmr.InterfaceAddress = (int) (((uint) ipmr.InterfaceAddress << 24) |
                                           (((uint) ipmr.InterfaceAddress & 0x0000FF00) << 8) |
                                           (((uint) ipmr.InterfaceAddress >> 8) & 0x0000FF00) |
                                           ((uint) ipmr.InterfaceAddress >> 24));
            }
#endif  // BIGENDIAN

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::setMulticastOption(): optionName:" + optionName.ToString() + " MR:" + MR.ToString() + " ipmr:" + ipmr.ToString() + " IPMulticastRequest.Size:" + IPMulticastRequest.Size.ToString());

            // This can throw ObjectDisposedException.
            SocketError errorCode = UnsafeNclNativeMethods.OSSOCK.setsockopt(
                m_Handle,
                SocketOptionLevel.IP,
                optionName,
                ref ipmr,
                IPMulticastRequest.Size);

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::setMulticastOption() UnsafeNclNativeMethods.OSSOCK.setsockopt returns errorCode:" + errorCode);

            //
            // if the native call fails we'll throw a SocketException
            //
            if (errorCode==SocketError.SocketError) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException();
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "setMulticastOption", socketException);
                throw socketException;
            }
        }


        /// <devdoc>
        ///     <para>
        ///         IPv6 setsockopt for JOIN / LEAVE multicast group
        ///     </para>
        /// </devdoc>
        private void setIPv6MulticastOption(SocketOptionName optionName, IPv6MulticastOption MR) {
            IPv6MulticastRequest ipmr = new IPv6MulticastRequest();

            ipmr.MulticastAddress = MR.Group.GetAddressBytes();
            ipmr.InterfaceIndex   = unchecked((int)MR.InterfaceIndex);

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::setIPv6MulticastOption(): optionName:" + optionName.ToString() + " MR:" + MR.ToString() + " ipmr:" + ipmr.ToString() + " IPv6MulticastRequest.Size:" + IPv6MulticastRequest.Size.ToString());

            // This can throw ObjectDisposedException.
            SocketError errorCode = UnsafeNclNativeMethods.OSSOCK.setsockopt(
                m_Handle,
                SocketOptionLevel.IPv6,
                optionName,
                ref ipmr,
                IPv6MulticastRequest.Size);

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::setIPv6MulticastOption() UnsafeNclNativeMethods.OSSOCK.setsockopt returns errorCode:" + errorCode);

            //
            // if the native call fails we'll throw a SocketException
            //
            if (errorCode==SocketError.SocketError) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException();
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "setIPv6MulticastOption", socketException);
                throw socketException;
            }
        }

        private void setLingerOption(LingerOption lref) {
            Linger lngopt = new Linger();
            lngopt.OnOff = lref.Enabled ? (short)1 : (short)0;
            lngopt.Time = (short)lref.LingerTime;

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::setLingerOption(): lref:" + lref.ToString());

            // This can throw ObjectDisposedException.
            SocketError errorCode = UnsafeNclNativeMethods.OSSOCK.setsockopt(
                m_Handle,
                SocketOptionLevel.Socket,
                SocketOptionName.Linger,
                ref lngopt,
                Linger.Size);

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::setLingerOption() UnsafeNclNativeMethods.OSSOCK.setsockopt returns errorCode:" + errorCode);

            //
            // if the native call fails we'll throw a SocketException
            //
            if (errorCode==SocketError.SocketError) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException();
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "setLingerOption", socketException);
                throw socketException;
            }
        }

        private LingerOption getLingerOpt() {
            Linger lngopt = new Linger();
            int optlen = Linger.Size;

            // This can throw ObjectDisposedException.
            SocketError errorCode = UnsafeNclNativeMethods.OSSOCK.getsockopt(
                m_Handle,
                SocketOptionLevel.Socket,
                SocketOptionName.Linger,
                out lngopt,
                ref optlen);

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::getLingerOpt() UnsafeNclNativeMethods.OSSOCK.getsockopt returns errorCode:" + errorCode);

            //
            // if the native call fails we'll throw a SocketException
            //
            if (errorCode==SocketError.SocketError) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException();
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "getLingerOpt", socketException);
                throw socketException;
            }

            LingerOption lingerOption = new LingerOption(lngopt.OnOff!=0, (int)lngopt.Time);
            return lingerOption;
        }

        private MulticastOption getMulticastOpt(SocketOptionName optionName) {
            IPMulticastRequest ipmr = new IPMulticastRequest();
            int optlen = IPMulticastRequest.Size;

            // This can throw ObjectDisposedException.
            SocketError errorCode = UnsafeNclNativeMethods.OSSOCK.getsockopt(
                m_Handle,
                SocketOptionLevel.IP,
                optionName,
                out ipmr,
                ref optlen);

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::getMulticastOpt() UnsafeNclNativeMethods.OSSOCK.getsockopt returns errorCode:" + errorCode);

            //
            // if the native call fails we'll throw a SocketException
            //
            if (errorCode==SocketError.SocketError) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException();
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "getMulticastOpt", socketException);
                throw socketException;
            }

#if BIGENDIAN
            ipmr.MulticastAddress = (int) (((uint) ipmr.MulticastAddress << 24) |
                                           (((uint) ipmr.MulticastAddress & 0x0000FF00) << 8) |
                                           (((uint) ipmr.MulticastAddress >> 8) & 0x0000FF00) |
                                           ((uint) ipmr.MulticastAddress >> 24));
            ipmr.InterfaceAddress = (int) (((uint) ipmr.InterfaceAddress << 24) |
                                           (((uint) ipmr.InterfaceAddress & 0x0000FF00) << 8) |
                                           (((uint) ipmr.InterfaceAddress >> 8) & 0x0000FF00) |
                                           ((uint) ipmr.InterfaceAddress >> 24));
#endif  // BIGENDIAN

            IPAddress multicastAddr = new IPAddress(ipmr.MulticastAddress);
            IPAddress multicastIntr = new IPAddress(ipmr.InterfaceAddress);

            MulticastOption multicastOption = new MulticastOption(multicastAddr, multicastIntr);

            return multicastOption;
        }


        /// <devdoc>
        ///     <para>
        ///         IPv6 getsockopt for JOIN / LEAVE multicast group
        ///     </para>
        /// </devdoc>
        private IPv6MulticastOption getIPv6MulticastOpt(SocketOptionName optionName) {
            IPv6MulticastRequest ipmr = new IPv6MulticastRequest();

            int optlen = IPv6MulticastRequest.Size;

            // This can throw ObjectDisposedException.
            SocketError errorCode = UnsafeNclNativeMethods.OSSOCK.getsockopt(
                m_Handle,
                SocketOptionLevel.IP,
                optionName,
                out ipmr,
                ref optlen);

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::getIPv6MulticastOpt() UnsafeNclNativeMethods.OSSOCK.getsockopt returns errorCode:" + errorCode);

            //
            // if the native call fails we'll throw a SocketException
            //
            if (errorCode==SocketError.SocketError) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException();
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "getIPv6MulticastOpt", socketException);
                throw socketException;
            }

            IPv6MulticastOption multicastOption = new IPv6MulticastOption(new IPAddress(ipmr.MulticastAddress),ipmr.InterfaceIndex);

            return multicastOption;
        }

        //
        // this version will ignore failures but it returns the win32
        // error code, and it will update internal state on success.
        //
        private SocketError InternalSetBlocking(bool desired, out bool current) {
            GlobalLog.Enter("Socket#" + ValidationHelper.HashString(this) + "::InternalSetBlocking", "desired:" + desired.ToString() + " willBlock:" + willBlock.ToString() + " willBlockInternal:" + willBlockInternal.ToString());

            if (CleanedUp) {
                GlobalLog.Leave("Socket#" + ValidationHelper.HashString(this) + "::InternalSetBlocking", "ObjectDisposed");
                current = willBlock;
                return SocketError.Success;
            }

            int intBlocking = desired ? 0 : -1;

            SocketError errorCode;
            try
            {
                errorCode = UnsafeNclNativeMethods.OSSOCK.ioctlsocket(
                    m_Handle,
                    IoctlSocketConstants.FIONBIO,
                    ref intBlocking);

                if (errorCode == SocketError.SocketError)
                {
                    errorCode = (SocketError) Marshal.GetLastWin32Error();
                }
            }
            catch (ObjectDisposedException)
            {
                errorCode = SocketError.NotSocket;
            }

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::InternalSetBlocking() UnsafeNclNativeMethods.OSSOCK.ioctlsocket returns errorCode:" + errorCode);

            //
            // we will update only internal state but only on successfull win32 call
            // so if the native call fails, the state will remain the same.
            //
            if (errorCode==SocketError.Success) {
                //
                // success, update internal state
                //
                willBlockInternal = intBlocking==0;
            }

            GlobalLog.Leave("Socket#" + ValidationHelper.HashString(this) + "::InternalSetBlocking", "errorCode:" + errorCode.ToString() + " willBlock:" + willBlock.ToString() + " willBlockInternal:" + willBlockInternal.ToString());
            current = willBlockInternal;
            return errorCode;
        }
        //
        // this version will ignore all failures.
        //
        internal void InternalSetBlocking(bool desired) {
            bool current;
            InternalSetBlocking(desired, out current);
        }

        private static IntPtr[] SocketListToFileDescriptorSet(IList socketList) {
            if (socketList==null || socketList.Count==0) {
                return null;
            }
            IntPtr[] fileDescriptorSet = new IntPtr[socketList.Count + 1];
            fileDescriptorSet[0] = (IntPtr)socketList.Count;
            for (int current=0; current<socketList.Count; current++) {
                if (!(socketList[current] is Socket)) {
                    throw new ArgumentException(SR.GetString(SR.net_sockets_select, socketList[current].GetType().FullName, typeof(System.Net.Sockets.Socket).FullName), "socketList");
                }
                fileDescriptorSet[current + 1] = ((Socket)socketList[current]).m_Handle.DangerousGetHandle();
            }
            return fileDescriptorSet;
        }

        //
        // Transform the list socketList such that the only sockets left are those
        // with a file descriptor contained in the array "fileDescriptorArray"
        //
        private static void SelectFileDescriptor(IList socketList, IntPtr[] fileDescriptorSet) {
            // Walk the list in order
            // Note that the counter is not necessarily incremented at each step;
            // when the socket is removed, advancing occurs automatically as the
            // other elements are shifted down.
            if (socketList==null || socketList.Count==0) {
                return;
            }
            if ((int)fileDescriptorSet[0]==0) {
                // no socket present, will never find any socket, remove them all
                socketList.Clear();
                return;
            }
            lock (socketList) {
                for (int currentSocket=0; currentSocket<socketList.Count; currentSocket++) {
                    Socket socket = socketList[currentSocket] as Socket;
                    // Look for the file descriptor in the array
                    int currentFileDescriptor;
                    for (currentFileDescriptor=0; currentFileDescriptor<(int)fileDescriptorSet[0]; currentFileDescriptor++) {
                        if (fileDescriptorSet[currentFileDescriptor + 1]==socket.m_Handle.DangerousGetHandle()) {
                            break;
                        }
                    }
                    if (currentFileDescriptor==(int)fileDescriptorSet[0]) {
                        // descriptor not found: remove the current socket and start again
                        socketList.RemoveAt(currentSocket--);
                    }
                }
            }
        }


        private static void MicrosecondsToTimeValue(long microSeconds, ref TimeValue socketTime) {
            socketTime.Seconds   = (int) (microSeconds / microcnv);
            socketTime.Microseconds  = (int) (microSeconds % microcnv);
        }
        //Implements ConnectEx - this provides completion port IO and support for
        //disconnect and reconnects

        // Since this is private, the unsafe mode is specified with a flag instead of an overload.
        private IAsyncResult BeginConnectEx(EndPoint remoteEP, bool flowContext, AsyncCallback callback, object state)
        {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "BeginConnectEx", "");

            // This will check the permissions for connect.
            EndPoint endPointSnapshot = remoteEP;
            SocketAddress socketAddress = flowContext ? CheckCacheRemote(ref endPointSnapshot, true) : SnapshotAndSerialize(ref endPointSnapshot);

            //socket must be bound first
            //the calling method BeginConnect will ensure that this method is only
            //called if m_RightEndPoint is not null, of that the endpoint is an ipendpoint
            if (m_RightEndPoint==null){
                GlobalLog.Assert(endPointSnapshot.GetType() == typeof(IPEndPoint), "Socket#{0}::BeginConnectEx()|Socket not bound and endpoint not IPEndPoint.", ValidationHelper.HashString(this));
                if (endPointSnapshot.AddressFamily == AddressFamily.InterNetwork)
                    InternalBind(new IPEndPoint(IPAddress.Any, 0));
                else
                    InternalBind(new IPEndPoint(IPAddress.IPv6Any, 0));
            }

            //
            // Allocate the async result and the event we'll pass to the
            // thread pool.
            //
            ConnectOverlappedAsyncResult asyncResult = new ConnectOverlappedAsyncResult(this, state, callback);

            // If context flowing is enabled, set it up here.  No need to lock since the context isn't used until the callback.
            if (flowContext)
            {
                asyncResult.StartPostingAsyncOp(false);
            }

            // This will pin socketAddress buffer
            asyncResult.SetUnmanagedStructures(socketAddress.m_Buffer);

            EndPoint oldEndPoint = m_RightEndPoint;
            if (m_RightEndPoint == null) {
                  m_RightEndPoint = endPointSnapshot;
            }

            SocketError errorCode=SocketError.Success;

            try
            {
                if (!ConnectEx(
                    m_Handle,
                    Marshal.UnsafeAddrOfPinnedArrayElement(socketAddress.m_Buffer, 0),
                    socketAddress.m_Size,
                    IntPtr.Zero,
                    0,
                    0,
                    asyncResult.OverlappedHandle))
                {
                    errorCode = (SocketError)Marshal.GetLastWin32Error();
                }
            }
            catch (ObjectDisposedException)
            {
                m_RightEndPoint = oldEndPoint;
                throw;
            }

            if (errorCode == SocketError.Success) {
                SetToConnected();
            }

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::BeginConnectEx() UnsafeNclNativeMethods.OSSOCK.connect returns:" + errorCode.ToString());

            errorCode = asyncResult.CheckAsyncCallOverlappedResult(errorCode);

            //
            // if the asynchronous native call fails synchronously
            // we'll throw a SocketException
            //
            if (errorCode != SocketError.Success) {
                //
                // update our internal state after this socket error and throw
                //
                m_RightEndPoint = oldEndPoint;
                SocketException socketException = new SocketException(errorCode);
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "BeginConnectEx", socketException);
                throw socketException;
            }

            // We didn't throw, so indicate that we're returning this result to the user.  This may call the callback.
            // This is a nop if the context isn't being flowed.
            asyncResult.FinishPostingAsyncOp(ref Caches.ConnectClosureCache);

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::BeginConnectEx() to:" + endPointSnapshot.ToString() + " returning AsyncResult:" + ValidationHelper.HashString(asyncResult));
            if(Logging.On)Logging.Exit(Logging.Sockets, this, "BeginConnectEx", asyncResult);
            return asyncResult;
        }


        internal void MultipleSend(BufferOffsetSize[] buffers, SocketFlags socketFlags) {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "MultipleSend", "");
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            //
            // parameter validation
            //
            GlobalLog.Assert(buffers != null, "Socket#{0}::MultipleSend()|buffers == null", ValidationHelper.HashString(this));
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::MultipleSend() buffers.Length:" + buffers.Length.ToString());

            WSABuffer[] WSABuffers = new WSABuffer[buffers.Length];
            GCHandle[] objectsToPin = null;
            int bytesTransferred;
            SocketError errorCode;

            try {
                objectsToPin = new GCHandle[buffers.Length];
                for (int i = 0; i < buffers.Length; ++i)
                {
                    objectsToPin[i] = GCHandle.Alloc(buffers[i].Buffer, GCHandleType.Pinned);
                    WSABuffers[i].Length = buffers[i].Size;
                    WSABuffers[i].Pointer = Marshal.UnsafeAddrOfPinnedArrayElement(buffers[i].Buffer, buffers[i].Offset);
                }

                // This can throw ObjectDisposedException.
                errorCode = UnsafeNclNativeMethods.OSSOCK.WSASend_Blocking(
                    m_Handle.DangerousGetHandle(),
                    WSABuffers,
                    WSABuffers.Length,
                    out bytesTransferred,
                    socketFlags,
                    IntPtr.Zero,
                    IntPtr.Zero);

                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::MultipleSend() UnsafeNclNativeMethods.OSSOCK.WSASend returns:" + errorCode.ToString() + " size:" + buffers.Length.ToString());

            }
            finally {
                if (objectsToPin != null)
                    for (int i = 0; i < objectsToPin.Length; ++i)
                        if (objectsToPin[i].IsAllocated)
                            objectsToPin[i].Free();
            }

            if (errorCode!=SocketError.Success) {
                SocketException socketException = new SocketException();
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "MultipleSend", socketException);
                throw socketException;
            }

            if(Logging.On)Logging.Exit(Logging.Sockets, this, "MultipleSend", "");
        }


        private static void DnsCallback(IAsyncResult result){
            if (result.CompletedSynchronously)
                return;

            MultipleAddressConnectAsyncResult context = (MultipleAddressConnectAsyncResult) result.AsyncState;
            try
            {
                DoDnsCallback(result, context);
            }
            catch (Exception exception)
            {
                if (exception is ThreadAbortException || exception is StackOverflowException || exception is OutOfMemoryException)
                    throw;

                context.InvokeCallback(exception);
            }
        }

        private static void DoDnsCallback(IAsyncResult result, MultipleAddressConnectAsyncResult context)
        {
            IPAddress[] addresses = Dns.EndGetHostAddresses(result);
            context.addresses = addresses;
            DoMultipleAddressConnectCallback(PostOneBeginConnect(context), context);
        }

        private class MultipleAddressConnectAsyncResult : ContextAwareResult
        {
            internal MultipleAddressConnectAsyncResult(IPAddress[] addresses, int port, Socket socket, object myState, AsyncCallback myCallBack) :
                base(socket, myState, myCallBack)
            {
                this.addresses = addresses;
                this.port = port;
                this.socket = socket;
            }

            internal Socket socket;   // Keep this member just to avoid all the casting.
            internal IPAddress[] addresses;
            internal int index;
            internal int port;
            internal Exception lastException;
        }

        private static object PostOneBeginConnect(MultipleAddressConnectAsyncResult context)
        {
            if (context.addresses[context.index].AddressFamily != context.socket.AddressFamily)
            {
                return context.lastException != null ? context.lastException : new ArgumentException(SR.GetString(SR.net_invalidAddressList), "context");
            }

            try
            {
                IAsyncResult connectResult = context.socket.UnsafeBeginConnect(new IPEndPoint(context.addresses[context.index], context.port), new AsyncCallback(MultipleAddressConnectCallback), context);
                if (connectResult.CompletedSynchronously)
                {
                    return connectResult;
                }
            }
            catch (Exception exception)
            {
                if (exception is OutOfMemoryException || exception is StackOverflowException || exception is ThreadAbortException)
                    throw;

                return exception;
            }

            return null;
        }

        private static void MultipleAddressConnectCallback(IAsyncResult result)
        {
            if (result.CompletedSynchronously)
                return;

            MultipleAddressConnectAsyncResult context = (MultipleAddressConnectAsyncResult) result.AsyncState;
            try
            {
                DoMultipleAddressConnectCallback(result, context);
            }
            catch (Exception exception)
            {
                if (exception is ThreadAbortException || exception is StackOverflowException || exception is OutOfMemoryException)
                    throw;

                context.InvokeCallback(exception);
            }
        }

        // This is like a regular async callback worker, except the result can be an exception.  This is a useful pattern when
        // processing should continue whether or not an async step failed.
        private static void DoMultipleAddressConnectCallback(object result, MultipleAddressConnectAsyncResult context)
        {
            while (result != null)
            {
                Exception ex = result as Exception;
                if (ex == null)
                {
                    try
                    {
                        context.socket.EndConnect((IAsyncResult) result);
                    }
                    catch (Exception exception)
                    {
                        if (exception is ThreadAbortException || exception is StackOverflowException || exception is OutOfMemoryException)
                            throw;

                        ex = exception;
                    }
                    catch
                    {
                        ex = new Exception(SR.GetString(SR.net_nonClsCompliantException));
                    }
                }

                if (ex == null)
                {
                    context.InvokeCallback();
                    break;
                }
                else
                {
                    if (++context.index >= context.addresses.Length)
                        throw ex;

                    context.lastException = ex;
                    result = PostOneBeginConnect(context);
                }
            }
        }

        private class DownLevelSendFileAsyncResult : ContextAwareResult
        {
            internal DownLevelSendFileAsyncResult(FileStream stream, Socket socket, object myState, AsyncCallback myCallBack) :
                base(socket, myState, myCallBack)
            {
                this.socket = socket;  // This field just avoids the cast.
                this.fileStream = stream;
                buffer = new byte[64000];
            }

            internal Socket socket;
            internal FileStream fileStream;
            internal byte[] buffer;
            internal bool writing;
        }

        private static void DownLevelSendFileCallback(IAsyncResult result)
        {
            if (result.CompletedSynchronously)
                return;

            DownLevelSendFileAsyncResult context = (DownLevelSendFileAsyncResult) result.AsyncState;
            DoDownLevelSendFileCallback(result, context);
        }

        private static void DoDownLevelSendFileCallback(IAsyncResult result, DownLevelSendFileAsyncResult context)
        {
            try
            {
                while (true)
                {
                    if (!context.writing)
                    {
                        int read = context.fileStream.EndRead(result);

                        if (read > 0)
                        {
                            context.writing = true;
                            result = context.socket.BeginSend(context.buffer, 0, read, SocketFlags.None, new AsyncCallback(DownLevelSendFileCallback), context);
                            if (!result.CompletedSynchronously)
                                break;
                        }
                        else
                        {
                            DownLevelSendFileCleanup(context.fileStream);
                            context.InvokeCallback();
                            break;
                        }
                    }
                    else
                    {
                        int written = context.socket.EndSend(result);
                        context.writing = false;
                        result = context.fileStream.BeginRead(context.buffer, 0, context.buffer.Length, new AsyncCallback(DownLevelSendFileCallback), context);
                        if (!result.CompletedSynchronously)
                            break;
                    }
                }
            }
            catch (Exception exception)
            {
                if (NclUtilities.IsFatal(exception))
                    throw;

                DownLevelSendFileCleanup(context.fileStream);
                context.InvokeCallback(exception);
            }

        }

        //Utility function to cleanup context structures 
        private static void DownLevelSendFileCleanup(FileStream fileStream)
        {
             // Release filestream,  we're done with it
             
             if (fileStream != null ) 
             {
                 fileStream.Close();
                 fileStream = null;
             }
        }

        
        // Since this is private, "Unsafe" is indicated with a flag.
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        private IAsyncResult BeginDownLevelSendFile(
            string fileName,
            bool flowContext,
            AsyncCallback callback,
            object state)
        {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "BeginSendFile", "");

            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }

            if (!Connected) {
                throw new NotSupportedException(SR.GetString(SR.net_notconnected));
            }

            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::BeginDownLevelSendFile() SRC:" + ValidationHelper.ToString(LocalEndPoint) + " DST:" + ValidationHelper.ToString(RemoteEndPoint) + " fileName:" + fileName);

            FileStream fileStream = null;
            if (fileName != null && fileName.Length>0) {
                fileStream = new FileStream(fileName,FileMode.Open,FileAccess.Read,FileShare.Read);
            }

            DownLevelSendFileAsyncResult asyncResult = null;
            IAsyncResult fileResult = null;
            try
            {
                asyncResult = new DownLevelSendFileAsyncResult(fileStream, this, state, callback);

                // If we're flowing context, indicate that here.
                if (flowContext)
                {
                    asyncResult.StartPostingAsyncOp(false);
                }

                fileResult = fileStream.BeginRead(asyncResult.buffer, 0, asyncResult.buffer.Length, new AsyncCallback(DownLevelSendFileCallback), asyncResult);
            }
            catch(Exception e)
            {
                if (!NclUtilities.IsFatal(e))
                {
                    DownLevelSendFileCleanup(fileStream);
                }
                throw;
            } 

            if (fileResult.CompletedSynchronously)
            {
                DoDownLevelSendFileCallback(fileResult, asyncResult);
            }

            // Finished without throwing - seal up the result and give it out.  NOP if we're not flowing.
            asyncResult.FinishPostingAsyncOp(ref Caches.SendClosureCache);
            if(Logging.On)Logging.Exit(Logging.Sockets, this, "BeginSendFile",0);

            return asyncResult;
        }


        internal IAsyncResult BeginMultipleSend(BufferOffsetSize[] buffers, SocketFlags socketFlags, AsyncCallback callback, object state) {
            // Set up the async result and start the flow.
            OverlappedAsyncResult asyncResult = new OverlappedAsyncResult(this, state, callback);
            asyncResult.StartPostingAsyncOp(false);

            // Start the send.
            DoBeginMultipleSend(buffers, socketFlags, asyncResult);

            // Finish it up (capture, complete).
            asyncResult.FinishPostingAsyncOp(ref Caches.SendClosureCache);
            return asyncResult;
        }

        internal IAsyncResult UnsafeBeginMultipleSend(BufferOffsetSize[] buffers, SocketFlags socketFlags, AsyncCallback callback, object state)
        {
            // Unsafe - don't flow.
            OverlappedAsyncResult asyncResult = new OverlappedAsyncResult(this, state, callback);
            DoBeginMultipleSend(buffers, socketFlags, asyncResult);
            return asyncResult;
        }

        private void DoBeginMultipleSend(BufferOffsetSize[] buffers, SocketFlags socketFlags, OverlappedAsyncResult asyncResult)
        {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "BeginMultipleSend", "");
            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            //
            // parameter validation
            //
            GlobalLog.Assert(buffers != null, "Socket#{0}::DoBeginMultipleSend()|buffers == null", ValidationHelper.HashString(this));
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::DoBeginMultipleSend() buffers.Length:" + buffers.Length.ToString());

            // Guarantee to call CheckAsyncCallOverlappedResult if we call SetUnamangedStructures with a cache in order to
            // avoid a Socket leak in case of error.
            SocketError errorCode = SocketError.SocketError;
            try
            {
                // Set up asyncResult for overlapped WSASend.
                // This call will use completion ports on WinNT and Overlapped IO on Win9x.
                asyncResult.SetUnmanagedStructures(buffers, ref Caches.SendOverlappedCache);

                // This can throw ObjectDisposedException.
                int bytesTransferred;
                errorCode = UnsafeNclNativeMethods.OSSOCK.WSASend(
                    m_Handle,
                    asyncResult.m_WSABuffers,
                    asyncResult.m_WSABuffers.Length,
                    out bytesTransferred,
                    socketFlags,
                    asyncResult.OverlappedHandle,
                    IntPtr.Zero);

                if (errorCode!=SocketError.Success) {
                    errorCode = (SocketError)Marshal.GetLastWin32Error();
                }
                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::BeginMultipleSend() UnsafeNclNativeMethods.OSSOCK.WSASend returns:" + errorCode.ToString() + " size:" + buffers.Length.ToString() + " returning AsyncResult:" + ValidationHelper.HashString(asyncResult));
            }
            finally
            {
                errorCode = asyncResult.CheckAsyncCallOverlappedResult(errorCode);
            }

            //
            // if the asynchronous native call fails synchronously
            // we'll throw a SocketException
            //
            if (errorCode!=SocketError.Success) {
                //
                // update our internal state after this socket error and throw
                //
                asyncResult.ExtractCache(ref Caches.SendOverlappedCache);
                SocketException socketException = new SocketException(errorCode);
                UpdateStatusAfterSocketError(socketException);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "BeginMultipleSend", socketException);
                throw socketException;
           }
            if(Logging.On)Logging.Exit(Logging.Sockets, this, "BeginMultipleSend", asyncResult);
        }


       private void EndDownLevelSendFile(IAsyncResult asyncResult) {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "EndSendFile", asyncResult);


            if (CleanedUp) {
                throw new ObjectDisposedException(this.GetType().FullName);
            }
            //
            // parameter validation
            //
            if (asyncResult==null) {
                throw new ArgumentNullException("asyncResult");
            }
            LazyAsyncResult castedAsyncResult = asyncResult as DownLevelSendFileAsyncResult;
            if (castedAsyncResult==null || castedAsyncResult.AsyncObject!=this) {
                throw new ArgumentException(SR.GetString(SR.net_io_invalidasyncresult), "asyncResult");
            }
            if (castedAsyncResult.EndCalled) {
                throw new InvalidOperationException(SR.GetString(SR.net_io_invalidendcall, "EndSendFile"));
            }

            castedAsyncResult.InternalWaitForCompletion();
            castedAsyncResult.EndCalled = true;


            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::EndSendFile()");

            //check for failure
            Exception exception = castedAsyncResult.Result as Exception;
            if(exception != null)
            {
                throw exception;
            }

            if(Logging.On)Logging.Exit(Logging.Sockets, this, "EndSendFile","");
        }

        internal int EndMultipleSend(IAsyncResult asyncResult) {
            if(Logging.On)Logging.Enter(Logging.Sockets, this, "EndMultipleSend", asyncResult);
            //
            // parameter validation
            //
            GlobalLog.Assert(asyncResult != null, "Socket#{0}::EndMultipleSend()|asyncResult == null", ValidationHelper.HashString(this));

            OverlappedAsyncResult castedAsyncResult = asyncResult as OverlappedAsyncResult;

            GlobalLog.Assert(castedAsyncResult != null, "Socket#{0}::EndMultipleSend()|castedAsyncResult == null", ValidationHelper.HashString(this));
            GlobalLog.Assert(castedAsyncResult.AsyncObject == this, "Socket#{0}::EndMultipleSend()|castedAsyncResult.AsyncObject != this", ValidationHelper.HashString(this));
            GlobalLog.Assert(!castedAsyncResult.EndCalled, "Socket#{0}::EndMultipleSend()|castedAsyncResult.EndCalled", ValidationHelper.HashString(this));

            int bytesTransferred = (int)castedAsyncResult.InternalWaitForCompletion();
            castedAsyncResult.EndCalled = true;
            castedAsyncResult.ExtractCache(ref Caches.SendOverlappedCache);


            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::EndMultipleSend() bytesTransferred:" + bytesTransferred.ToString());

            //
            // if the asynchronous native call failed asynchronously
            // we'll throw a SocketException
            //
            if ((SocketError)castedAsyncResult.ErrorCode!=SocketError.Success) {
                //
                // update our internal state after this socket error and throw
                //
                SocketException socketException = new SocketException(castedAsyncResult.ErrorCode);
                if(Logging.On)Logging.Exception(Logging.Sockets, this, "EndMultipleSend", socketException);
                throw socketException;
            }
            if(Logging.On)Logging.Exit(Logging.Sockets, this, "EndMultipleSend", bytesTransferred);
            return bytesTransferred;
        }

         //
        // CreateAcceptSocket - pulls unmanaged results and assembles them
        //   into a new Socket object
        //
        private Socket CreateAcceptSocket(SafeCloseSocket fd, EndPoint remoteEP, bool needCancelSelect) {
            //
            // Internal state of the socket is inherited from listener
            //
            Socket socket           = new Socket(fd);
            return UpdateAcceptSocket(socket,remoteEP, needCancelSelect);
         }

        internal Socket UpdateAcceptSocket(Socket socket, EndPoint remoteEP, bool needCancelSelect) {
            //
            // Internal state of the socket is inherited from listener
            //
            socket.addressFamily    = addressFamily;
            socket.socketType       = socketType;
            socket.protocolType     = protocolType;
            socket.m_RightEndPoint  = m_RightEndPoint;
            socket.m_RemoteEndPoint = remoteEP;
            //
            // the socket is connected
            //
            socket.SetToConnected();
            //
            // if the socket is returned by an Endb), the socket might have
            // inherited the WSAEventSelect() call from the accepting socket.
            // we need to cancel this otherwise the socket will be in non-blocking
            // mode and we cannot force blocking mode using the ioctlsocket() in
            // Socket.set_Blocking(), since it fails returing 10022 as documented in MSDN.
            // (note that the m_AsyncEvent event will not be created in this case.
            //


            if (needCancelSelect)
                socket.UnsetAsyncEventSelect();
            //
            // the new socket will inherit the win32 blocking mode from the accepting socket.
            // if the user desired blocking mode is different from the win32 blocking mode
            // we need to force the desired blocking behaviour.
            //
            socket.willBlock = willBlock;
            if (willBlock!=willBlockInternal) {
                socket.InternalSetBlocking(willBlock);
            }

            return socket;
        }



        //
        // SetToConnected - updates the status of the socket to connected
        //
        internal void SetToConnected() {
            if (m_IsConnected) {
                //
                // socket was already connected
                //
                return;
            }
            //
            // update the status: this socket was indeed connected at
            // some point in time update the perf counter as well.
            //
            m_IsConnected = true;
            m_IsDisconnected = false;
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::SetToConnected() now connected SRC:" + ValidationHelper.ToString(LocalEndPoint) + " DST:" + ValidationHelper.ToString(RemoteEndPoint));
        }

        //
        // SetToDisconnected - updates the status of the socket to disconnected
        //
        internal void SetToDisconnected() {
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::SetToDisconnected()");
            if (!m_IsConnected) {
                //
                // socket was already disconnected
                //
                return;
            }
            //
            // update the status: this socket was indeed disconnected at
            // some point in time, clear any async select bits.
            //
            m_IsConnected = false;
            m_IsDisconnected = true;

            if (!CleanedUp) {
                //
                // if socket is still alive cancel WSAEventSelect()
                //
                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::SetToDisconnected()");

                UnsetAsyncEventSelect();
            }
        }

        //
        // UpdateStatusAfterSocketError(socketException) - updates the status of a connected socket
        // on which a failure occured. it'll go to winsock and check if the connection
        // is still open and if it needs to update our internal state.
        //
        internal void UpdateStatusAfterSocketError(SocketException socketException){
            UpdateStatusAfterSocketError((SocketError) socketException.NativeErrorCode);
        }

        internal void UpdateStatusAfterSocketError(SocketError errorCode)
        {
            //
            // if we already know the socket is disconnected
            // we don't need to do anything else.
            //
            GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::UpdateStatusAfterSocketError(socketException)");

            if (m_IsConnected && (m_Handle.IsInvalid || (errorCode != SocketError.WouldBlock &&
                    errorCode != SocketError.IOPending && errorCode != SocketError.NoBufferSpaceAvailable)))
            {
                //
                //
                // the socket is no longer a valid socket
                //
                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::UpdateStatusAfterSocketError(socketException) Invalidating socket.");
                SetToDisconnected();
            }
        }


        //
        // Does internal initalization before async winsock
        // call to BeginConnect() or BeginAccept().
        //
        private void UnsetAsyncEventSelect()
        {
            GlobalLog.Enter("Socket#" + ValidationHelper.HashString(this) + "::UnsetAsyncEventSelect", "m_BlockEventBits:" + m_BlockEventBits.ToString() + " willBlockInternal:" + willBlockInternal.ToString());

            RegisteredWaitHandle registeredWait = m_RegisteredWait;
            if (registeredWait != null)
            {
                m_RegisteredWait = null;
                registeredWait.Unregister(null);
            }

            SocketError errorCode = SocketError.NotSocket;
            try {
                errorCode = UnsafeNclNativeMethods.OSSOCK.WSAEventSelect(m_Handle, IntPtr.Zero, AsyncEventBits.FdNone);
            }
            catch (Exception e)
            {
                if (NclUtilities.IsFatal(e))
                    throw;
                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::UnsetAsyncEventSelect() !!! (ignoring) Exception: " + e.ToString());
                GlobalLog.Assert(CleanedUp, "Socket#{0}::UnsetAsyncEventSelect|Got exception and CleanedUp not set.", ValidationHelper.HashString(this));
            }
            catch
            {
                GlobalLog.Assert("Socket#" + ValidationHelper.HashString(this) + "::UnsetAsyncEventSelect", "Non-Exception object thrown by WSAEventSelect.");
            }

            //
            // May be re-used in future, reset if the event got signalled after registeredWait.Unregister call
            //
            if (m_AsyncEvent != null)
            {
                try
                {
                    m_AsyncEvent.Reset();
                }
                catch (ObjectDisposedException) { }
            }

            if (errorCode == SocketError.SocketError)
            {
                //
                // update our internal state after this socket error
                // we won't throw since this is an internal method
                //
                UpdateStatusAfterSocketError(errorCode);
            }

            GlobalLog.Leave("Socket#" + ValidationHelper.HashString(this) + "::UnsetAsyncEventSelect", "m_BlockEventBits:" + m_BlockEventBits.ToString() + " willBlockInternal:" + willBlockInternal.ToString());
        }

        private bool SetAsyncEventSelect(AsyncEventBits blockEventBits)
        {
            GlobalLog.Enter("Socket#" + ValidationHelper.HashString(this) + "::SetAsyncEventSelect", "blockEventBits:" + blockEventBits.ToString() + " m_BlockEventBits:" + m_BlockEventBits.ToString() + " willBlockInternal:" + willBlockInternal.ToString());
            GlobalLog.Assert(blockEventBits != AsyncEventBits.FdNone, "Socket#{0}::SetAsyncEventSelect|Use UnsetAsyncEventSelect for FdNone.", ValidationHelper.HashString(this));
            GlobalLog.Assert(m_BlockEventBits == AsyncEventBits.FdNone || m_BlockEventBits == blockEventBits, "Socket#{0}::SetAsyncEventSelect|Can't change from one active wait to another.", ValidationHelper.HashString(this));
            GlobalLog.Assert(m_RegisteredWait == null, "Socket#{0}::SetAsyncEventSelect|Already actively waiting on an op.", ValidationHelper.HashString(this));

            // This check is bogus, too late diggin into a historical reason for it.
            // Make sure the upper level will fail with ObjectDisposedException
            if (m_RegisteredWait != null)
                return false;

            //
            // This will put us into non-blocking mode.  Create the event if it isn't, and register a wait.
            //
            if (m_AsyncEvent == null)
            {
                Interlocked.CompareExchange<ManualResetEvent>(ref m_AsyncEvent, new ManualResetEvent(false), null);
                if (s_RegisteredWaitCallback == null)
                    s_RegisteredWaitCallback = new WaitOrTimerCallback(RegisteredWaitCallback);
            }

            //
            // Try to win over Dispose is there is a race
            //
            if (Interlocked.CompareExchange(ref m_IntCleanedUp, 2, 0) != 0)
            {
                GlobalLog.Leave("Socket#" + ValidationHelper.HashString(this) + "::SetAsyncEventSelect() Already Cleaned up, returning ... ", string.Empty);
                return false;
            }

            m_BlockEventBits = blockEventBits;
            m_RegisteredWait = ThreadPool.UnsafeRegisterWaitForSingleObject(m_AsyncEvent, s_RegisteredWaitCallback, this, Timeout.Infinite, true);

            //
            // Release dispose if any is waiting
            //
            Interlocked.Exchange(ref m_IntCleanedUp, 0);

            SocketError errorCode = SocketError.NotSocket;
            //
            // issue the native call
            //
            try {
                errorCode = UnsafeNclNativeMethods.OSSOCK.WSAEventSelect(m_Handle, m_AsyncEvent.SafeWaitHandle, blockEventBits);
            }
            catch (Exception e)
            {
                if (NclUtilities.IsFatal(e))
                    throw;
                GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::SetAsyncEventSelect() !!! (converting to ObjectDisposed) Exception :" + e.ToString());
                GlobalLog.Assert(CleanedUp, "Socket#{0}::SetAsyncEventSelect|WSAEventSelect got exception and CleanedUp not set.", ValidationHelper.HashString(this));
            }
            catch
            {
                GlobalLog.Assert("Socket#" + ValidationHelper.HashString(this) + "::SetAsyncEventSelect", "WSAEventSelect caught non-Exception object.");
            }

            if (errorCode==SocketError.SocketError) {
                //
                // update our internal state after this socket error
                // we won't throw since this is an internal method
                //
                UpdateStatusAfterSocketError(errorCode);
            }

            //
            // the call to WSAEventSelect might have caused us to change
            // blocking mode, hence we need update internal status
            //
            willBlockInternal = false;

            GlobalLog.Leave("Socket#" + ValidationHelper.HashString(this) + "::SetAsyncEventSelect", "m_BlockEventBits:" + m_BlockEventBits.ToString() + " willBlockInternal:" + willBlockInternal.ToString());
            return errorCode == SocketError.Success;
        }

        private static void RegisteredWaitCallback(object state, bool timedOut)
        {
            GlobalLog.Enter("Socket#" + ValidationHelper.HashString(state) + "::RegisteredWaitCallback", "m_BlockEventBits:" + ((Socket)state).m_BlockEventBits.ToString());
#if DEBUG
            // GlobalLog.SetThreadSource(ThreadKinds.Worker);  Because of change 1077887, need logic to determine thread type here.
            using (GlobalLog.SetThreadKind(ThreadKinds.System)) {
#endif
            Socket me = (Socket)state;

            // Interlocked to avoid a race condition with DoBeginConnect
            if (Interlocked.Exchange(ref me.m_RegisteredWait, null) != null)
            {
                switch (me.m_BlockEventBits)
                {
                    case AsyncEventBits.FdConnect:
                        me.ConnectCallback();
                        break;

                    case AsyncEventBits.FdAccept:
                        me.AcceptCallback(null);
                        break;
                }
            }
#if DEBUG
            }
#endif
            GlobalLog.Leave("Socket#" + ValidationHelper.HashString(state) + "::RegisteredWaitCallback", "m_BlockEventBits:" + ((Socket)state).m_BlockEventBits.ToString());
        }

        //
        // ValidateBlockingMode - called before synchronous calls to validate
        // the fact that we are in blocking mode (not in non-blocking mode) so the
        // call will actually be synchronous
        //
        private void ValidateBlockingMode() {
            if (willBlock && !willBlockInternal) {
                throw new InvalidOperationException(SR.GetString(SR.net_invasync));
            }
        }


        //
        // This Method binds the Socket Win32 Handle to the ThreadPool's CompletionPort
        // (make sure we only bind once per socket)
        //
        [SecurityPermission(SecurityAction.Assert, Flags = SecurityPermissionFlag.UnmanagedCode)]
        internal void BindToCompletionPort()
        {
            //
            // Check to see if the socket native m_Handle is already
            // bound to the ThreadPool's completion port.
            //
            if (!m_BoundToThreadPool && !UseOverlappedIO) {
                lock (this) {
                    if (!m_BoundToThreadPool) {
                        //
                        // bind the socket native m_Handle to the ThreadPool
                        //
                        GlobalLog.Print("Socket#" + ValidationHelper.HashString(this) + "::BindToCompletionPort() calling ThreadPool.BindHandle()");

                        try
                        {
                            ThreadPool.BindHandle(m_Handle);
                            m_BoundToThreadPool = true;
                        }
                        catch (Exception exception)
                        {
                            if (NclUtilities.IsFatal(exception)) throw;
                            Close(0);
                            throw;
                        }
                    }
                }
            }
        }


    }


    internal class ConnectAsyncResult:ContextAwareResult{
        internal ConnectAsyncResult(object myObject, object myState, AsyncCallback myCallBack):base(myObject, myState, myCallBack) {
        }
    }

    internal class AcceptAsyncResult:ContextAwareResult{
        internal AcceptAsyncResult(object myObject, object myState, AsyncCallback myCallBack):base(myObject, myState, myCallBack) {
        }
    }


}
