//------------------------------------------------------------------------------
// <copyright file="UnsafeNativeMethods.cs" company="Microsoft">
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

namespace System.Net {
    using System.Runtime.InteropServices;
    using System.Runtime.CompilerServices;
    using System.Security.Permissions;
    using System.Text;
    using System.Net.Sockets;
    using System.Net.Cache;
    using System.Threading;
    using System.ComponentModel;
    using System.Collections;
    using System.Globalization;
    using System.Runtime.ConstrainedExecution;
    using System.Security;
    using Microsoft.Win32.SafeHandles;

    [System.Security.SuppressUnmanagedCodeSecurityAttribute()]
    internal static class UnsafeNclNativeMethods {

 #if !PLATFORM_UNIX
        internal const String DLLPREFIX = "";
        internal const String DLLSUFFIX = ".dll";
 #else // !PLATFORM_UNIX
  #if __APPLE__
        internal const String DLLPREFIX = "lib";
        internal const String DLLSUFFIX = ".dylib";
  #else
        internal const String DLLPREFIX = "lib";
        internal const String DLLSUFFIX = ".so";
  #endif
 #endif // !PLATFORM_UNIX

        internal const String ROTOR_PAL   = DLLPREFIX + "rotor_pal" + DLLSUFFIX;
        internal const String ROTOR_PALRT = DLLPREFIX + "rotor_palrt" + DLLSUFFIX;
        private const String KERNEL32 = ROTOR_PAL;

        private const string WS2_32 = ExternDll.Kernel32; // Resolves to rotor_pal

        private const string SECUR32 = "secur32.dll";
        private const string CRYPT32 = "crypt32.dll";
        private const string ADVAPI32 = "advapi32.dll";
        private const string HTTPAPI = "httpapi.dll";
        private const string SCHANNEL = "schannel.dll";
        private const string SECURITY = "security.dll";
        private const string RASAPI32 = "rasapi32.dll";
        private const string WININET = "wininet.dll";
        private const string WINHTTP = "winhttp.dll";

        /*
        //                                    
        [DllImport(KERNEL32)]
        internal static extern IntPtr CreateSemaphore([In] IntPtr lpSemaphoreAttributes, [In] int lInitialCount, [In] int lMaximumCount, [In] string lpName);
        */

        /* Consider removing.
        [DllImport(KERNEL32)]
        internal static extern IntPtr CreateEvent([In] IntPtr lpEventAttributes, [In] bool manualResetEvent, [In] bool initalState, [In] IntPtr lpName);
        */

        [DllImport(KERNEL32)]
        internal static extern IntPtr CreateSemaphore([In] IntPtr lpSemaphoreAttributes, [In] int lInitialCount, [In] int lMaximumCount, [In] IntPtr lpName);

#if DEBUG
        [DllImport(KERNEL32)]
        internal static extern bool ReleaseSemaphore([In] IntPtr hSemaphore, [In] int lReleaseCount, [Out] out int lpPreviousCount);

#else
        [DllImport(KERNEL32)]
        internal static extern bool ReleaseSemaphore([In] IntPtr hSemaphore, [In] int lReleaseCount, [In] IntPtr lpPreviousCount);
#endif

        internal static class ErrorCodes
        {
            internal const uint ERROR_SUCCESS               = 0;
            internal const uint ERROR_HANDLE_EOF            = 38;
            internal const uint ERROR_NOT_SUPPORTED         = 50;
            internal const uint ERROR_INVALID_PARAMETER     = 87;
            internal const uint ERROR_ALREADY_EXISTS        = 183;
            internal const uint ERROR_MORE_DATA             = 234;
            internal const uint ERROR_OPERATION_ABORTED     = 995;
            internal const uint ERROR_IO_PENDING            = 997;
            internal const uint ERROR_NOT_FOUND             = 1168;
        }

        [DllImport(KERNEL32, ExactSpelling=true, CallingConvention=CallingConvention.StdCall, SetLastError=true)]
        internal static extern uint GetCurrentThreadId();

#if STRESS || !DEBUG
        [DllImport(KERNEL32, ExactSpelling = true)]
        internal static extern void DebugBreak();
#endif

        [DllImport(ROTOR_PALRT, CharSet=CharSet.Unicode, SetLastError=true, EntryPoint="PAL_FetchConfigurationStringW")]
        internal static extern bool FetchConfigurationString(bool perMachine, String parameterName, StringBuilder parameterValue, int parameterValueLength);


        // Because the regular SafeNetHandles has a LocalAlloc with a different return type.
        [System.Security.SuppressUnmanagedCodeSecurityAttribute()]
        internal static class SafeNetHandlesSafeOverlappedFree {
            [DllImport(ExternDll.Kernel32, ExactSpelling=true, SetLastError=true)]
            internal static extern SafeOverlappedFree LocalAlloc(int uFlags, UIntPtr sizetdwBytes);
        }


        [System.Security.SuppressUnmanagedCodeSecurityAttribute()]
        internal static class SafeNetHandles {


            [DllImport(ExternDll.Kernel32, ExactSpelling=true, SetLastError=true)]
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            internal static extern bool CloseHandle(IntPtr handle);

            [DllImport(ExternDll.Kernel32, ExactSpelling=true, SetLastError=true)]
            internal static extern SafeLocalFree LocalAlloc(int uFlags, UIntPtr sizetdwBytes);

            [DllImport(ExternDll.Kernel32, ExactSpelling=true, SetLastError=true)]
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            internal static extern IntPtr LocalFree(IntPtr handle);



            [DllImport(KERNEL32, ExactSpelling=true, SetLastError=true)]
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            internal static extern unsafe bool FreeLibrary([In] IntPtr hModule);


            [DllImport(ExternDll.Kernel32, ExactSpelling=true, SetLastError=true)]
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            internal static extern IntPtr GlobalFree(IntPtr handle);

            // Blocking call - requires IntPtr instead of SafeCloseSocket.
            [DllImport(WS2_32, ExactSpelling=true, SetLastError=true)]
            internal static extern SafeCloseSocket.InnerSafeCloseSocket accept(
                                                  [In] IntPtr socketHandle,
                                                  [Out] byte[] socketAddress,
                                                  [In, Out] ref int socketAddressSize
                                                  );

            [DllImport(WS2_32, ExactSpelling=true, SetLastError=true)]
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            internal static extern SocketError closesocket(
                                                  [In] IntPtr socketHandle
                                                  );

            [DllImport(WS2_32, ExactSpelling=true, SetLastError=true)]
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            internal static extern SocketError ioctlsocket(
                                                [In] IntPtr handle,
                                                [In] int cmd,
                                                [In, Out] ref int argp
                                                );

            [DllImport(WS2_32, ExactSpelling=true, SetLastError=true)]
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            internal static extern SocketError WSAEventSelect(
                                                     [In] IntPtr handle,
                                                     [In] IntPtr Event,
                                                     [In] AsyncEventBits NetworkEvents
                                                     );

            [DllImport(WS2_32, ExactSpelling=true, SetLastError=true)]
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            internal static extern SocketError setsockopt(
                                               [In] IntPtr handle,
                                               [In] SocketOptionLevel optionLevel,
                                               [In] SocketOptionName optionName,
                                               [In] ref Linger linger,
                                               [In] int optionLength
                                               );

            /* Consider removing
            [DllImport(WS2_32, ExactSpelling=true, SetLastError=true)]
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
            internal static extern SocketError setsockopt(
                                               [In] IntPtr handle,
                                               [In] SocketOptionLevel optionLevel,
                                               [In] SocketOptionName optionName,
                                               [In] ref int optionValue,
                                               [In] int optionLength
                                               );
            */

        }

        //
        // UnsafeNclNativeMethods.OSSOCK class contains all Unsafe() calls and should all be protected
        // by the appropriate SocketPermission() to connect/accept to/from remote
        // peers over the network and to perform name resolution.
        // te following calls deal mainly with:
        // 1) socket calls
        // 2) DNS calls
        //

        //
        // here's a brief explanation of all possible decorations we use for PInvoke.
        // these are used in such a way that we hope to gain maximum performance from the
        // unmanaged/managed/unmanaged transition we need to undergo when calling into winsock:
        //
        // [In] (Note: this is similar to what msdn will show)
        // the managed data will be marshalled so that the unmanaged function can read it but even
        // if it is changed in unmanaged world, the changes won't be propagated to the managed data
        //
        // [Out] (Note: this is similar to what msdn will show)
        // the managed data will not be marshalled so that the unmanaged function will not see the
        // managed data, if the data changes in unmanaged world, these changes will be propagated by
        // the marshaller to the managed data
        //
        // objects are marshalled differently if they're:
        //
        // 1) structs
        // for structs, by default, the whole layout is pushed on the stack as it is.
        // in order to pass a pointer to the managed layout, we need to specify either the ref or out keyword.
        //
        //      a) for IN and OUT:
        //      [In, Out] ref Struct ([In, Out] is optional here)
        //
        //      b) for IN only (the managed data will be marshalled so that the unmanaged
        //      function can read it but even if it changes it the change won't be propagated
        //      to the managed struct)
        //      [In] ref Struct
        //
        //      c) for OUT only (the managed data will not be marshalled so that the
        //      unmanaged function cannot read, the changes done in unmanaged code will be
        //      propagated to the managed struct)
        //      [Out] out Struct ([Out] is optional here)
        //
        // 2) array or classes
        // for array or classes, by default, a pointer to the managed layout is passed.
        // we don't need to specify neither the ref nor the out keyword.
        //
        //      a) for IN and OUT:
        //      [In, Out] byte[]
        //
        //      b) for IN only (the managed data will be marshalled so that the unmanaged
        //      function can read it but even if it changes it the change won't be propagated
        //      to the managed struct)
        //      [In] byte[] ([In] is optional here)
        //
        //      c) for OUT only (the managed data will not be marshalled so that the
        //      unmanaged function cannot read, the changes done in unmanaged code will be
        //      propagated to the managed struct)
        //      [Out] byte[]
        //
        [System.Security.SuppressUnmanagedCodeSecurityAttribute()]
        internal static class OSSOCK {

            private const string WS2_32 = ROTOR_PAL;

            //
            // IPv6 Changes: These are initialized in InitializeSockets - don't set them here or
            //               there will be an ordering problem with the call above that will
            //               result in both being set to false !
            //

            [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto)]
            internal struct WSAPROTOCOLCHAIN {
                internal int ChainLen;                                 /* the length of the chain,     */
                [MarshalAs(UnmanagedType.ByValArray, SizeConst=7)]
                internal uint[] ChainEntries;       /* a list of dwCatalogEntryIds */
            }

            [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto)]
            internal struct WSAPROTOCOL_INFO {
                internal uint dwServiceFlags1;
                internal uint dwServiceFlags2;
                internal uint dwServiceFlags3;
                internal uint dwServiceFlags4;
                internal uint dwProviderFlags;
                Guid ProviderId;
                internal uint dwCatalogEntryId;
                WSAPROTOCOLCHAIN ProtocolChain;
                internal int iVersion;
                internal AddressFamily iAddressFamily;
                internal int iMaxSockAddr;
                internal int iMinSockAddr;
                internal int iSocketType;
                internal int iProtocol;
                internal int iProtocolMaxOffset;
                internal int iNetworkByteOrder;
                internal int iSecurityScheme;
                internal uint dwMessageSize;
                internal uint dwProviderReserved;
                [MarshalAs(UnmanagedType.ByValTStr, SizeConst=256)]
                internal string szProtocol;
            }

            [StructLayout(LayoutKind.Sequential)]
            internal struct ControlData {
                internal UIntPtr length;
                internal uint level;
                internal uint type;
                internal uint address;
                internal uint index;
            }

            [StructLayout(LayoutKind.Sequential)]
            internal struct ControlDataIPv6 {
                internal UIntPtr length;
                internal uint level;
                internal uint type;
                [MarshalAs(UnmanagedType.ByValArray,SizeConst=16)]
                internal byte[] address;
                internal uint index;
            }


            [StructLayout(LayoutKind.Sequential)]
            internal struct WSAMsg {
                internal IntPtr socketAddress;
                internal uint addressLength;
                internal IntPtr buffers;
                internal uint count;
                internal WSABuffer controlBuffer;
                internal SocketFlags flags;
            }


            // CharSet=Auto here since WSASocket has A and W versions. We can use Auto cause the method is not used under constrained execution region
            [DllImport(WS2_32, CharSet=CharSet.Auto, SetLastError=true)]
            internal static extern SafeCloseSocket.InnerSafeCloseSocket WSASocket(
                                                    [In] AddressFamily addressFamily,
                                                    [In] SocketType socketType,
                                                    [In] ProtocolType protocolType,
                                                    [In] IntPtr protocolInfo, // will be WSAProtcolInfo protocolInfo once we include QOS APIs
                                                    [In] uint group,
                                                    [In] SocketConstructorFlags flags
                                                    );

            [DllImport(WS2_32, CharSet=CharSet.Auto, SetLastError=true)]
            internal unsafe static extern SafeCloseSocket.InnerSafeCloseSocket WSASocket(
                                        [In] AddressFamily addressFamily,
                                        [In] SocketType socketType,
                                        [In] ProtocolType protocolType,
                                        [In] byte* pinnedBuffer, // will be WSAProtcolInfo protocolInfo once we include QOS APIs
                                        [In] uint group,
                                        [In] SocketConstructorFlags flags
                                        );


            [DllImport(WS2_32, CharSet=CharSet.Ansi, BestFitMapping=false, ThrowOnUnmappableChar=true, SetLastError=true)]
            internal static extern SocketError WSAStartup(
                                               [In] short wVersionRequested,
                                               [Out] out WSAData lpWSAData
                                               );

            [DllImport(WS2_32, SetLastError=true)]
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
            internal static extern SocketError ioctlsocket(
                                                [In] SafeCloseSocket socketHandle,
                                                [In] int cmd,
                                                [In, Out] ref int argp
                                                );

            [DllImport(WS2_32, CharSet=CharSet.Ansi, BestFitMapping=false, ThrowOnUnmappableChar=true, SetLastError=true)]
            internal static extern IntPtr gethostbyname(
                                                  [In] string host
                                                  );

            [DllImport(WS2_32, SetLastError=true)]
            internal static extern IntPtr gethostbyaddr(
                                                  [In] ref int addr,
                                                  [In] int len,
                                                  [In] ProtocolFamily type
                                                  );

            [DllImport(WS2_32, CharSet=CharSet.Ansi, BestFitMapping=false, ThrowOnUnmappableChar=true, SetLastError=true)]
            internal static extern SocketError gethostname(
                                                [Out] StringBuilder hostName,
                                                [In] int bufferLength
                                                );

            // this should belong to SafeNativeMethods, but it will not for simplicity
            [DllImport(WS2_32, CharSet=CharSet.Ansi, BestFitMapping=false, ThrowOnUnmappableChar=true, SetLastError=true)]
            internal static extern int inet_addr(
                                              [In] string cp
                                              );

            [DllImport(WS2_32, SetLastError=true)]
            internal static extern SocketError getpeername(
                                                [In] SafeCloseSocket socketHandle,
                                                [Out] byte[] socketAddress,
                                                [In, Out] ref int socketAddressSize
                                                );

            [DllImport(WS2_32, SetLastError=true)]
            internal static extern SocketError getsockopt(
                                               [In] SafeCloseSocket socketHandle,
                                               [In] SocketOptionLevel optionLevel,
                                               [In] SocketOptionName optionName,
                                               [Out] out int optionValue,
                                               [In, Out] ref int optionLength
                                               );

            [DllImport(WS2_32, SetLastError=true)]
            internal static extern SocketError getsockopt(
                                               [In] SafeCloseSocket socketHandle,
                                               [In] SocketOptionLevel optionLevel,
                                               [In] SocketOptionName optionName,
                                               [Out] byte[] optionValue,
                                               [In, Out] ref int optionLength
                                               );

            [DllImport(WS2_32, SetLastError=true)]
            internal static extern SocketError getsockopt(
                                               [In] SafeCloseSocket socketHandle,
                                               [In] SocketOptionLevel optionLevel,
                                               [In] SocketOptionName optionName,
                                               [Out] out Linger optionValue,
                                               [In, Out] ref int optionLength
                                               );

            [DllImport(WS2_32, SetLastError=true)]
            internal static extern SocketError getsockopt(
                                               [In] SafeCloseSocket socketHandle,
                                               [In] SocketOptionLevel optionLevel,
                                               [In] SocketOptionName optionName,
                                               [Out] out IPMulticastRequest optionValue,
                                               [In, Out] ref int optionLength
                                               );

            //
            // IPv6 Changes: need to receive and IPv6MulticastRequest from getsockopt
            //
            [DllImport(WS2_32, SetLastError=true)]
            internal static extern SocketError getsockopt(
                                               [In] SafeCloseSocket socketHandle,
                                               [In] SocketOptionLevel optionLevel,
                                               [In] SocketOptionName optionName,
                                               [Out] out IPv6MulticastRequest optionValue,
                                               [In, Out] ref int optionLength
                                               );

            [DllImport(WS2_32, SetLastError=true)]
            internal static extern SocketError setsockopt(
                                               [In] SafeCloseSocket socketHandle,
                                               [In] SocketOptionLevel optionLevel,
                                               [In] SocketOptionName optionName,
                                               [In] ref int optionValue,
                                               [In] int optionLength
                                               );

            [DllImport(WS2_32, SetLastError=true)]
            internal static extern SocketError setsockopt(
                                               [In] SafeCloseSocket socketHandle,
                                               [In] SocketOptionLevel optionLevel,
                                               [In] SocketOptionName optionName,
                                               [In] byte[] optionValue,
                                               [In] int optionLength
                                               );

            [DllImport(WS2_32, SetLastError=true)]
            internal static extern SocketError setsockopt(
                                               [In] SafeCloseSocket socketHandle,
                                               [In] SocketOptionLevel optionLevel,
                                               [In] SocketOptionName optionName,
                                               [In] ref IntPtr pointer,
                                               [In] int optionLength
                                               );

            [DllImport(WS2_32, SetLastError=true)]
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
            internal static extern SocketError setsockopt(
                                               [In] SafeCloseSocket socketHandle,
                                               [In] SocketOptionLevel optionLevel,
                                               [In] SocketOptionName optionName,
                                               [In] ref Linger linger,
                                               [In] int optionLength
                                               );

            [DllImport(WS2_32, SetLastError=true)]
            internal static extern SocketError setsockopt(
                                               [In] SafeCloseSocket socketHandle,
                                               [In] SocketOptionLevel optionLevel,
                                               [In] SocketOptionName optionName,
                                               [In] ref IPMulticastRequest mreq,
                                               [In] int optionLength
                                               );

            //
            // IPv6 Changes: need to pass an IPv6MulticastRequest to setsockopt
            //
            [DllImport(WS2_32, SetLastError=true)]
            internal static extern SocketError setsockopt(
                                               [In] SafeCloseSocket socketHandle,
                                               [In] SocketOptionLevel optionLevel,
                                               [In] SocketOptionName optionName,
                                               [In] ref IPv6MulticastRequest mreq,
                                               [In] int optionLength
                                               );


            // This method is always blocking, so it uses an IntPtr.
            [DllImport(WS2_32, SetLastError = true)]
            internal unsafe static extern int send(
                                         [In] IntPtr      socketHandle,
                                         [In] byte*       pinnedBuffer,
                                         [In] int         len,
                                         [In] SocketFlags socketFlags
                                         );

            // This method is always blocking, so it uses an IntPtr.
            [DllImport(WS2_32, SetLastError = true)]
            internal unsafe static extern int recv(
                                         [In] IntPtr      socketHandle,
                                         [In] byte*       pinnedBuffer,
                                         [In] int         len,
                                         [In] SocketFlags socketFlags
                                         );

            [DllImport(WS2_32, SetLastError=true)]
            internal static extern SocketError listen(
                                           [In] SafeCloseSocket socketHandle,
                                           [In] int backlog
                                           );

            [DllImport(WS2_32, SetLastError=true)]
            internal static extern SocketError bind(
                                         [In] SafeCloseSocket socketHandle,
                                         [In] byte[] socketAddress,
                                         [In] int socketAddressSize
                                         );

            [DllImport(WS2_32, SetLastError=true)]
            internal static extern SocketError shutdown(
                                             [In] SafeCloseSocket socketHandle,
                                             [In] int how
                                             );

            // This method is always blocking, so it uses an IntPtr.
            [DllImport(WS2_32, SetLastError = true)]
            internal unsafe static extern int sendto(
                                           [In] IntPtr      socketHandle,
                                           [In] byte*       pinnedBuffer,
                                           [In] int         len,
                                           [In] SocketFlags socketFlags,
                                           [In] byte[]      socketAddress,
                                           [In] int         socketAddressSize
                                           );

            // This method is always blocking, so it uses an IntPtr.
            [DllImport(WS2_32, SetLastError = true)]
            internal unsafe static extern int recvfrom(
                                             [In] IntPtr        socketHandle,
                                             [In] byte*         pinnedBuffer,
                                             [In] int           len,
                                             [In] SocketFlags   socketFlags,
                                             [Out] byte[]       socketAddress,
                                             [In, Out] ref int  socketAddressSize
                                             );

            [DllImport(WS2_32, SetLastError=true)]
            internal static extern SocketError getsockname(
                                                [In] SafeCloseSocket socketHandle,
                                                [Out] byte[] socketAddress,
                                                [In, Out] ref int socketAddressSize
                                                );

            [DllImport(WS2_32, SetLastError=true)]
            internal static extern int select(
                                           [In] int ignoredParameter,
                                           [In, Out] IntPtr[] readfds,
                                           [In, Out] IntPtr[] writefds,
                                           [In, Out] IntPtr[] exceptfds,
                                           [In] ref TimeValue timeout
                                           );

            [DllImport(WS2_32, SetLastError=true)]
            internal static extern int select(
                                           [In] int ignoredParameter,
                                           [In, Out] IntPtr[] readfds,
                                           [In, Out] IntPtr[] writefds,
                                           [In, Out] IntPtr[] exceptfds,
                                           [In] IntPtr nullTimeout
                                           );

            // This function is always potentially blocking so it uses an IntPtr.
            [DllImport(WS2_32, SetLastError = true)]
            internal static extern SocketError WSAConnect(
                                              [In] IntPtr socketHandle,
                                              [In] byte[] socketAddress,
                                              [In] int    socketAddressSize,
                                              [In] IntPtr inBuffer,
                                              [In] IntPtr outBuffer,
                                              [In] IntPtr sQOS,
                                              [In] IntPtr gQOS
                                              );


            [DllImport(WS2_32, SetLastError=true)]
            internal static extern SocketError WSASend(
                                              [In] SafeCloseSocket socketHandle,
                                              [In] ref WSABuffer buffer,
                                              [In] int bufferCount,
                                              [Out] out int bytesTransferred,
                                              [In] SocketFlags socketFlags,
                                              [In] IntPtr overlapped,
                                              [In] IntPtr completionRoutine
                                              );

            [DllImport(WS2_32, SetLastError=true)]
            internal static extern SocketError WSASend(
                                              [In] SafeCloseSocket socketHandle,
                                              [In] WSABuffer[] buffersArray,
                                              [In] int bufferCount,
                                              [Out] out int bytesTransferred,
                                              [In] SocketFlags socketFlags,
                                              [In] IntPtr overlapped,
                                              [In] IntPtr completionRoutine
                                              );

            /* Consider removing
            [DllImport(WS2_32, SetLastError = true, EntryPoint = "WSASend")]
            internal static extern SocketError WSASend_Blocking(
                                              [In] IntPtr socketHandle,
                                              [In] ref WSABuffer buffer,
                                              [In] int bufferCount,
                                              [Out] out int bytesTransferred,
                                              [In] SocketFlags socketFlags,
                                              [In] IntPtr overlapped,
                                              [In] IntPtr completionRoutine
                                              );
            */

            [DllImport(WS2_32, SetLastError = true, EntryPoint = "WSASend")]
            internal static extern SocketError WSASend_Blocking(
                                              [In] IntPtr socketHandle,
                                              [In] WSABuffer[] buffersArray,
                                              [In] int bufferCount,
                                              [Out] out int bytesTransferred,
                                              [In] SocketFlags socketFlags,
                                              [In] IntPtr overlapped,
                                              [In] IntPtr completionRoutine
                                              );

            [DllImport(WS2_32, SetLastError = true)]
            internal static extern SocketError WSASendTo(
                                                [In] SafeCloseSocket socketHandle,
                                                [In] ref WSABuffer buffer,
                                                [In] int bufferCount,
                                                [Out] out int bytesTransferred,
                                                [In] SocketFlags socketFlags,
                                                [In] IntPtr socketAddress,
                                                [In] int socketAddressSize,
                                                [In] IntPtr overlapped,
                                                [In] IntPtr completionRoutine
                                                );

            [DllImport(WS2_32, SetLastError=true)]
            internal static extern SocketError WSARecv(
                                              [In] SafeCloseSocket socketHandle,
                                              [In, Out] ref WSABuffer buffer,
                                              [In] int bufferCount,
                                              [Out] out int bytesTransferred,
                                              [In, Out] ref SocketFlags socketFlags,
                                              [In] IntPtr overlapped,
                                              [In] IntPtr completionRoutine
                                              );

            [DllImport(WS2_32, SetLastError = true)]
            internal static extern SocketError WSARecv(
                                              [In] SafeCloseSocket socketHandle,
                                              [In, Out] WSABuffer[] buffers,
                                              [In] int bufferCount,
                                              [Out] out int bytesTransferred,
                                              [In, Out] ref SocketFlags socketFlags,
                                              [In] IntPtr overlapped,
                                              [In] IntPtr completionRoutine
                                              );

            /* Consider removing
            [DllImport(WS2_32, SetLastError = true, EntryPoint = "WSARecv")]
            internal static extern SocketError WSARecv_Blocking(
                                              [In] IntPtr socketHandle,
                                              [In, Out] ref WSABuffer buffer,
                                              [In] int bufferCount,
                                              [Out] out int bytesTransferred,
                                              [In, Out] ref SocketFlags socketFlags,
                                              [In] IntPtr overlapped,
                                              [In] IntPtr completionRoutine
                                              );
            */

            [DllImport(WS2_32, SetLastError = true, EntryPoint = "WSARecv")]
            internal static extern SocketError WSARecv_Blocking(
                                              [In] IntPtr socketHandle,
                                              [In, Out] WSABuffer[] buffers,
                                              [In] int bufferCount,
                                              [Out] out int bytesTransferred,
                                              [In, Out] ref SocketFlags socketFlags,
                                              [In] IntPtr overlapped,
                                              [In] IntPtr completionRoutine
                                              );

            [DllImport(WS2_32, SetLastError=true)]
            internal static extern SocketError WSARecvFrom(
                                                  [In] SafeCloseSocket socketHandle,
                                                  [In, Out] ref WSABuffer buffer,
                                                  [In] int bufferCount,
                                                  [Out] out int bytesTransferred,
                                                  [In, Out] ref SocketFlags socketFlags,
                                                  [In] IntPtr socketAddressPointer,
                                                  [In] IntPtr socketAddressSizePointer,
                                                  [In] IntPtr overlapped,
                                                  [In] IntPtr completionRoutine
                                                  );


            [DllImport(WS2_32, SetLastError=true)]
            internal static extern SocketError WSAEventSelect(
                                                     [In] SafeCloseSocket socketHandle,
                                                     [In] SafeHandle Event,
                                                     [In] AsyncEventBits NetworkEvents
                                                     );

            [DllImport(WS2_32, SetLastError=true)]
            internal static extern SocketError WSAEventSelect(
                                         [In] SafeCloseSocket socketHandle,
                                         [In] IntPtr Event,
                                         [In] AsyncEventBits NetworkEvents
                                         );


            // Used with SIOGETEXTENSIONFUNCTIONPOINTER - we're assuming that will never block.
            [DllImport(WS2_32, SetLastError=true)]
            internal static extern SocketError WSAIoctl(
                                                [In] SafeCloseSocket socketHandle,
                                                [In] int ioControlCode,
                                                [In,Out] ref Guid guid,
                                                [In] int guidSize,
                                                [Out] out IntPtr funcPtr,
                                                [In]  int funcPtrSize,
                                                [Out] out int bytesTransferred,
                                                [In] IntPtr shouldBeNull,
                                                [In] IntPtr shouldBeNull2
                                                );

            [DllImport(WS2_32, SetLastError = true, EntryPoint = "WSAIoctl")]
            internal static extern SocketError WSAIoctl_Blocking(
                                                [In] IntPtr socketHandle,
                                                [In] int ioControlCode,
                                                [In] byte[] inBuffer,
                                                [In] int inBufferSize,
                                                [Out] byte[] outBuffer,
                                                [In] int outBufferSize,
                                                [Out] out int bytesTransferred,
                                                [In] IntPtr overlapped,
                                                [In] IntPtr completionRoutine
                                                );

            [DllImport(WS2_32,SetLastError=true)]
            internal static extern SocketError WSAEnumNetworkEvents(
                                                     [In] SafeCloseSocket socketHandle,
                                                     [In] SafeWaitHandle Event,
                                                     [In, Out] ref NetworkEvents networkEvents
                                                     );


            [DllImport(WS2_32, SetLastError=true)]
            internal static extern bool WSAGetOverlappedResult(
                                                     [In] SafeCloseSocket socketHandle,
                                                     [In] IntPtr overlapped,
                                                     [Out] out uint bytesTransferred,
                                                     [In] bool wait,
                                                     [In] IntPtr ignored
                                                     );

        }; // class UnsafeNclNativeMethods.OSSOCK


    }
}
