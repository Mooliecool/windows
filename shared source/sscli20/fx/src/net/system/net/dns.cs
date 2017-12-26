//------------------------------------------------------------------------------
// <copyright file="DNS.cs" company="Microsoft">
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
    using System.Text;
    using System.Collections;
    using System.Net.Sockets;
    using System.Runtime.InteropServices;
    using System.Security.Permissions;
    using System.Threading;
    using System.Security;
    using System.Globalization;

    /// <devdoc>
    ///    <para>Provides simple
    ///       domain name resolution functionality.</para>
    /// </devdoc>

    public static class Dns
    {
        //
        // used by GetHostName() to preallocate a buffer for the call to gethostname.
        //
        private const int HostNameBufferLength = 256;

        //also used as a lock object
        private static DnsPermission s_DnsPermission = new DnsPermission(PermissionState.Unrestricted);
        //
        //
        private const int MaxHostName = 126;


        /*++

        Routine Description:

            Takes a native pointer (expressed as an int) to a hostent structure,
            and converts the information in their to an IPHostEntry class. This
            involves walking through an array of native pointers, and a temporary
            ArrayList object is used in doing this.

        Arguments:

            nativePointer   - Native pointer to hostent structure.



        Return Value:

            An IPHostEntry structure.

        --*/

        internal static IPHostEntry NativeToHostEntry(IntPtr nativePointer) {
            //
            // marshal pointer to struct
            //

            hostent Host = (hostent)Marshal.PtrToStructure(nativePointer, typeof(hostent));
            IPHostEntry HostEntry = new IPHostEntry();

            if (Host.h_name != IntPtr.Zero) {
                HostEntry.HostName = Marshal.PtrToStringAnsi(Host.h_name);
                GlobalLog.Print("HostEntry.HostName: " + HostEntry.HostName);
            }

            // decode h_addr_list to ArrayList of IP addresses.
            // The h_addr_list field is really a pointer to an array of pointers
            // to IP addresses. Loop through the array, and while the pointer
            // isn't NULL read the IP address, convert it to an IPAddress class,
            // and add it to the list.

            ArrayList TempList = new ArrayList();
            int IPAddressToAdd;
            string AliasToAdd;
            IntPtr currentArrayElement;

            //
            // get the first pointer in the array
            //
            currentArrayElement = Host.h_addr_list;
            nativePointer = Marshal.ReadIntPtr(currentArrayElement);

            while (nativePointer != IntPtr.Zero) {
                //
                // if it's not null it points to an IPAddress,
                // read it...
                //
                IPAddressToAdd = Marshal.ReadInt32(nativePointer);
#if BIGENDIAN
                // IP addresses from native code are always a byte array
                // converted to int.  We need to convert the address into
                // a uniform integer value.
                IPAddressToAdd = (int)( ((uint)IPAddressToAdd << 24) |
                                        (((uint)IPAddressToAdd & 0x0000FF00) << 8) |
                                        (((uint)IPAddressToAdd >> 8) & 0x0000FF00) |
                                        ((uint)IPAddressToAdd >> 24) );
#endif

                GlobalLog.Print("currentArrayElement: " + currentArrayElement.ToString() + " nativePointer: " + nativePointer.ToString() + " IPAddressToAdd:" + IPAddressToAdd.ToString());

                //
                // ...and add it to the list
                //
                TempList.Add(new IPAddress(IPAddressToAdd));

                //
                // now get the next pointer in the array and start over
                //
                currentArrayElement = IntPtrHelper.Add(currentArrayElement, IntPtr.Size);
                nativePointer = Marshal.ReadIntPtr(currentArrayElement);
            }

            HostEntry.AddressList = new IPAddress[TempList.Count];
            TempList.CopyTo(HostEntry.AddressList, 0);

            //
            // Now do the same thing for the aliases.
            //

            TempList.Clear();

            currentArrayElement = Host.h_aliases;
            nativePointer = Marshal.ReadIntPtr(currentArrayElement);

            while (nativePointer != IntPtr.Zero) {

                GlobalLog.Print("currentArrayElement: " + ((long)currentArrayElement).ToString() + "nativePointer: " + ((long)nativePointer).ToString());

                //
                // if it's not null it points to an Alias,
                // read it...
                //
                AliasToAdd = Marshal.PtrToStringAnsi(nativePointer);

                //
                // ...and add it to the list
                //
                TempList.Add(AliasToAdd);

                //
                // now get the next pointer in the array and start over
                //
                currentArrayElement = IntPtrHelper.Add(currentArrayElement, IntPtr.Size);
                nativePointer = Marshal.ReadIntPtr(currentArrayElement);

            }

            HostEntry.Aliases = new string[TempList.Count];
            TempList.CopyTo(HostEntry.Aliases, 0);

            return HostEntry;

        } // NativeToHostEntry

        /*****************************************************************************
         Function :    gethostbyname

         Abstract:     Queries DNS for hostname address

         Input Parameters: str (String to query)

         Returns: Void
        ******************************************************************************/

        /// <devdoc>
        /// <para>Retrieves the <see cref='System.Net.IPHostEntry'/>
        /// information
        /// corresponding to the DNS name provided in the host
        /// parameter.</para>
        /// </devdoc>


        [Obsolete("GetHostByName is obsoleted for this type, please use GetHostEntry instead. http://go.microsoft.com/fwlink/?linkid=14202")]
        public static IPHostEntry GetHostByName(string hostName) {
            if (hostName == null) {
                throw new ArgumentNullException("hostName");
            }
            //
            // demand Unrestricted DnsPermission for this call
            //
            s_DnsPermission.Demand();
            return InternalGetHostByName(hostName, false);
        }
        //
        
        
        internal static IPHostEntry InternalGetHostByName(string hostName) {
            return InternalGetHostByName(hostName,true);
        }
        
        internal static IPHostEntry InternalGetHostByName(string hostName, bool includeIPv6) {
            if(Logging.On)Logging.Enter(Logging.Sockets, "DNS", "GetHostByName", hostName);
            IPHostEntry ipHostEntry = null;

            GlobalLog.Print("Dns.GetHostByName: " + hostName);

            if (hostName.Length>MaxHostName) {
                throw new ArgumentOutOfRangeException(SR.GetString(SR.net_toolong, "hostName", MaxHostName.ToString(NumberFormatInfo.CurrentInfo)));
            }

            {
                //
                // IPv6 disabled: use gethostbyname() to obtain DNS information.
                //
                IntPtr nativePointer =
                    UnsafeNclNativeMethods.OSSOCK.gethostbyname(
                        hostName);

                if (nativePointer == IntPtr.Zero) {
                    // This is for compatiblity with NT4/Win2k
                    // Need to do this first since if we wait the last error code might be overwritten.
                    SocketException socketException = new SocketException();

                    IPAddress address;
                    if (IPAddress.TryParse(hostName, out address))
                    {
                        ipHostEntry = new IPHostEntry();
                        ipHostEntry.HostName = address.ToString();
                        ipHostEntry.Aliases = new string[0];
                        ipHostEntry.AddressList = new IPAddress[] { address };
                        if(Logging.On)Logging.Exit(Logging.Sockets, "DNS", "GetHostByName", ipHostEntry);
                        return ipHostEntry;
                    }

                    throw socketException;
                }
                ipHostEntry = NativeToHostEntry(nativePointer);
            }

            if(Logging.On)Logging.Exit(Logging.Sockets, "DNS", "GetHostByName", ipHostEntry);
            return ipHostEntry;

        } // GetHostByName


 
        /*****************************************************************************
         Function :    gethostbyaddr

         Abstract:     Queries IP address string and tries to match with a host name

         Input Parameters: str (String to query)

         Returns: IPHostEntry
        ******************************************************************************/

        /// <devdoc>
        /// <para>Creates an <see cref='System.Net.IPHostEntry'/>
        /// instance from an IP dotted address.</para>
        /// </devdoc>

        [Obsolete("GetHostByAddress is obsoleted for this type, please use GetHostEntry instead. http://go.microsoft.com/fwlink/?linkid=14202")]
        public static IPHostEntry GetHostByAddress(string address) {
            if(Logging.On)Logging.Enter(Logging.Sockets, "DNS", "GetHostByAddress", address);
            //
            // demand Unrestricted DnsPermission for this call
            //
            s_DnsPermission.Demand();

            if (address == null) {
                throw new ArgumentNullException("address");
            }

            GlobalLog.Print("Dns.GetHostByAddress: " + address);

            IPHostEntry ipHostEntry = InternalGetHostByAddress(IPAddress.Parse(address), false, true);
            if(Logging.On)Logging.Exit(Logging.Sockets, "DNS", "GetHostByAddress", ipHostEntry);
            return ipHostEntry;
        } // GetHostByAddress

        /*****************************************************************************
         Function :    gethostbyaddr

         Abstract:     Queries IP address and tries to match with a host name

         Input Parameters: address (address to query)

         Returns: IPHostEntry
        ******************************************************************************/

        /// <devdoc>
        /// <para>Creates an <see cref='System.Net.IPHostEntry'/> instance from an <see cref='System.Net.IPAddress'/>
        /// instance.</para>
        /// </devdoc>

        [Obsolete("GetHostByAddress is obsoleted for this type, please use GetHostEntry instead. http://go.microsoft.com/fwlink/?linkid=14202")]
        public static IPHostEntry GetHostByAddress(IPAddress address) {
            if(Logging.On)Logging.Enter(Logging.Sockets, "DNS", "GetHostByAddress", "");
            //
            // demand Unrestricted DnsPermission for this call
            //
            s_DnsPermission.Demand();

            if (address == null) {
                throw new ArgumentNullException("address");
            }

            IPHostEntry ipHostEntry = InternalGetHostByAddress(address, false, true);
            if(Logging.On)Logging.Exit(Logging.Sockets, "DNS", "GetHostByAddress", ipHostEntry);
            return ipHostEntry;
        } // GetHostByAddress

        internal static IPHostEntry InternalGetHostByAddress(IPAddress address, bool includeIPv6, bool throwOnFailure)
        {
            GlobalLog.Print("Dns.InternalGetHostByAddress: " + address.ToString());

            Exception exception = null;


            //
            // If IPv6 is not enabled (maybe config switch) but we've been
            // given an IPv6 address then we need to bail out now.
            //
            {
                if ( address.AddressFamily == AddressFamily.InterNetworkV6 ) {
                    //
                    // Protocol not supported
                    //
                    throw new SocketException(SocketError.ProtocolNotSupported);
                }
                //
                // Use gethostbyaddr() to try to resolve the IP address
                //
                // End IPv6 Changes
                //
                int addressAsInt = unchecked((int)address.m_Address);
    
#if BIGENDIAN
                addressAsInt = (int)( ((uint)addressAsInt << 24) | (((uint)addressAsInt & 0x0000FF00) << 8) |
                    (((uint)addressAsInt >> 8) & 0x0000FF00) | ((uint)addressAsInt >> 24) );
#endif
    
                IntPtr nativePointer =
                    UnsafeNclNativeMethods.OSSOCK.gethostbyaddr(
                        ref addressAsInt,
                        Marshal.SizeOf(typeof(int)),
                        ProtocolFamily.InterNetwork);
    
    
                if (nativePointer != IntPtr.Zero) {
                    return NativeToHostEntry(nativePointer);
                }
                exception = new SocketException();
            }

            if(throwOnFailure){
                throw exception;
            }

            IPHostEntry ipHostEntry = new IPHostEntry();

            try{
                ipHostEntry.HostName    = address.ToString();
                ipHostEntry.Aliases     = new string[0];
                ipHostEntry.AddressList = new IPAddress[] { address };
            }
            catch{
                throw exception;   //throw the original exception
            }
            return ipHostEntry;
        } // InternalGetHostByAddress


        /*****************************************************************************
         Function :    gethostname

         Abstract:     Queries the hostname from DNS

         Input Parameters:

         Returns: String
        ******************************************************************************/

        /// <devdoc>
        ///    <para>Gets the host name of the local machine.</para>
        /// </devdoc>
        // UEUE: note that this method is not threadsafe!!
        public static string GetHostName() {
            //
            // demand Unrestricted DnsPermission for this call
            //
            s_DnsPermission.Demand();

            GlobalLog.Print("Dns.GetHostName");

            //
            // note that we could cache the result ourselves since you
            // wouldn't expect the hostname of the machine to change during
            // execution, but this might still happen and we would want to
            // react to that change.
            //
            
            Socket.InitializeSockets();
            StringBuilder sb = new StringBuilder(HostNameBufferLength);
            SocketError errorCode =
                UnsafeNclNativeMethods.OSSOCK.gethostname(
                    sb,
                    HostNameBufferLength);

            //
            // if the call failed throw a SocketException()
            //
            if (errorCode!=SocketError.Success) {
                throw new SocketException();
            }
            return sb.ToString();
        }


        /*****************************************************************************
         Function :    resolve

         Abstract:     Converts IP/hostnames to IP numerical address using DNS
                       Additional methods provided for convenience
                       (These methods will resolve strings and hostnames. In case of
                       multiple IP addresses, the address returned is chosen arbitrarily.)

         Input Parameters: host/IP

         Returns: IPAddress
        ******************************************************************************/

        /// <devdoc>
        /// <para>Creates an <see cref='System.Net.IPAddress'/>
        /// instance from a DNS hostname.</para>
        /// </devdoc>
        // UEUE


        [Obsolete("Resolve is obsoleted for this type, please use GetHostEntry instead. http://go.microsoft.com/fwlink/?linkid=14202")]
        public static IPHostEntry Resolve(string hostName) {
            if(Logging.On)Logging.Enter(Logging.Sockets, "DNS", "Resolve", hostName);
            //
            // demand Unrestricted DnsPermission for this call
            //
            s_DnsPermission.Demand();

            if (hostName == null) {
                throw new ArgumentNullException("hostName");
            }

            // See if it's an IP Address.
            IPAddress address;
            IPHostEntry ipHostEntry;

            if (TryParseAsIP(hostName, out address) && (address.AddressFamily != AddressFamily.InterNetworkV6 || Socket.LegacySupportsIPv6))
            {
                ipHostEntry = InternalGetHostByAddress(address, false, false);
            }
            else
            {
                ipHostEntry = InternalGetHostByName(hostName, false);
            }

            if (Logging.On) Logging.Exit(Logging.Sockets, "DNS", "Resolve", ipHostEntry);
            return ipHostEntry;
        }


        //Caution, this will return ipv6 addresses if the OS supports it.  This shouldn't be called by the public apis.
        internal static IPHostEntry InternalResolveFast(string hostName, int timeout, out bool timedOut) {
            GlobalLog.Assert(hostName != null, "hostName == null");
            GlobalLog.Print("Dns.InternalResolveFase: " + hostName);

            //
            // the differences between this method and the previous InternalResolve() are:
            //
            // 1) we don't throw any exceptions
            // 2) we don't do a reverse lookup for address strings, we just use them
            //
            // IPv6 Changes: It is not practical to embed the code for GetAddrInfo here, instead
            //               we call it and catch any exceptions.
            //
            // Timeout: Supports a timeout by offloading work to another thread if necessary.
            //

            // We can't abort a DNS lookup so if we think might need to, run it on another thread.
            // According to MSDN the max time is 17 seconds.  Use 18 and say 20.
            // Also: MSDN describes how one lookup can result in a string of queries.  It's unclear whether
            // those would be run in series, extending the possible time this will take, or whether it will always
            // give up after 17 seconds.  For now assume that 17 seconds is the absolute max.
            bool mightTimeOut = 18000 >= (uint) timeout && timeout != Timeout.Infinite;
            timedOut = false;

            if (hostName.Length > 0 && hostName.Length <= MaxHostName)
            {
                // IP Address?
                IPAddress address;
                if (TryParseAsIP(hostName, out address))
                {
                    IPHostEntry ipHostEntry = new IPHostEntry();
                    ipHostEntry.HostName    = address.ToString();
                    ipHostEntry.Aliases     = new string[0];
                    ipHostEntry.AddressList = new IPAddress[] { address };

                    GlobalLog.Print("Dns::InternalResolveFast() returned address:" + address.ToString());
                    return ipHostEntry;
                }

                // Looks like a hostname (or failed address parsing)
                if (Socket.OSSupportsIPv6)
                {
                    try
                    {
                       // we will no longer offload to a thread, due to the consequence of having a threadpool thread
                       //block on another threadpool thread.  In addition, even w/ the DNS server functioning, we run
                       // the risk of having too many of these queued up, causing requests to fail w/ an unable to resolve
                       //exception.
                        
                       //I'm leaving the code commented out to possibly reuse in our async case.

                       // if (!mightTimeOut)
                       // {
                            return GetAddrInfo(hostName);
                       // }
                       /* else
                        {
                            AsyncDnsContext dnsContext = new AsyncDnsContext(hostName);
                            dnsContext.Offload(new WaitCallback(OffloadedGetAddrInfo));
                            return (IPHostEntry) dnsContext.Wait(timeout, out timedOut);
                        }
                        */
                    }
                    catch ( Exception e )
                    {
                        GlobalLog.Print("Dns::InternalResolveFast() GetAddrInfo() threw: " + e.Message);
                    }
                }
                else {
                    //
                    // we duplicate the code in GetHostByName() to avoid
                    // having to catch the thrown exception
                    //
                    IntPtr nativePointer;
                    //if (!mightTimeOut)
                    //{
                        nativePointer = UnsafeNclNativeMethods.OSSOCK.gethostbyname(hostName);
                    //}
                    /*
                    else
                    {
                        AsyncDnsContext dnsContext = new AsyncDnsContext(hostName);
                        dnsContext.Offload(new WaitCallback(OffloadedGetHostByName));
                        object result = dnsContext.Wait(timeout, out timedOut);
                        nativePointer = result == null ? IntPtr.Zero : (IntPtr) result;
                    }
                    */
                    if (nativePointer != IntPtr.Zero) {
                        GlobalLog.Print("Dns::InternalResolveFast() gethostbyname() returned nativePointer:" + nativePointer.ToString());
                        return NativeToHostEntry(nativePointer);
                    }
                }
            }

            GlobalLog.Print("Dns::InternalResolveFast() returning null");
            if(Logging.On)Logging.Exit(Logging.Sockets, "DNS", "InternalResolveFast", null);
            return null;
        }


        private static WaitCallback resolveCallback = new WaitCallback(ResolveCallback);

        private class ResolveAsyncResult : ContextAwareResult
        {
            internal ResolveAsyncResult(string hostName, object myObject, bool includeIPv6, object myState, AsyncCallback myCallBack) :
                base(myObject, myState, myCallBack)
            {
                this.hostName = hostName;
                this.includeIPv6 = includeIPv6;
            }

            
            internal ResolveAsyncResult(IPAddress address, object myObject, bool includeIPv6, object myState, AsyncCallback myCallBack) :
                base(myObject, myState, myCallBack)
            {
                this.includeIPv6 = includeIPv6;
                this.address = address;
            }
            
            internal readonly string hostName;
            internal bool includeIPv6;
            internal IPAddress address;
        }

        private static void ResolveCallback(object context)
        {
            ResolveAsyncResult result = (ResolveAsyncResult)context;
            IPHostEntry hostEntry;
            try
            {
                if(result.address != null){
                    hostEntry = InternalGetHostByAddress(result.address, result.includeIPv6, false);
                }
                else{
                    hostEntry = InternalGetHostByName(result.hostName, result.includeIPv6);
                }
            }
            catch (Exception exception)
            {
                if (exception is OutOfMemoryException || exception is ThreadAbortException || exception is StackOverflowException)
                    throw;

                result.InvokeCallback(exception);
                return;
            }

            result.InvokeCallback(hostEntry);
        }


        // Helpers for async GetHostByName, ResolveToAddresses, and Resolve - they're almost identical

        private static IAsyncResult HostResolutionBeginHelper(string hostName, bool useGetHostByName, bool flowContext, bool includeIPv6, bool throwOnIPAny, AsyncCallback requestCallback, object state)
        {
            s_DnsPermission.Demand();

            if (hostName == null) {
                throw new ArgumentNullException("hostName");
            }

            GlobalLog.Print("Dns.HostResolutionBeginHelper: " + hostName);

            // See if it's an IP Address.
            IPAddress address;
            ResolveAsyncResult asyncResult;
            if (TryParseAsIP(hostName, out address))
            {
                if (throwOnIPAny && (address.Equals(IPAddress.Any) || address.Equals(IPAddress.IPv6Any)))
                {
                    throw new ArgumentException(SR.GetString(SR.net_invalid_ip_addr), "hostNameOrAddress");
                }

                asyncResult = new ResolveAsyncResult(address, null, includeIPv6, state, requestCallback);

                if (useGetHostByName)
                {
                    IPHostEntry hostEntry = new IPHostEntry();
                    hostEntry.AddressList = new IPAddress[] { address };
                    hostEntry.Aliases = new string[0];
                    hostEntry.HostName = address.ToString();
                    asyncResult.StartPostingAsyncOp(false);
                    asyncResult.InvokeCallback(hostEntry);
                    asyncResult.FinishPostingAsyncOp();
                    return asyncResult;
                }
            }
            else
            {
                asyncResult = new ResolveAsyncResult(hostName, null, includeIPv6, state, requestCallback);
            }

            // Set up the context, possibly flow.
            if (flowContext)
            {
                asyncResult.StartPostingAsyncOp(false);
            }

            // Start the resolve.
            ThreadPool.UnsafeQueueUserWorkItem(resolveCallback, asyncResult);

            // Finish the flowing, maybe it completed?  This does nothing if we didn't initiate the flowing above.
            asyncResult.FinishPostingAsyncOp();
            return asyncResult;
        }


        private static IAsyncResult HostResolutionBeginHelper(IPAddress address, bool flowContext, bool includeIPv6, AsyncCallback requestCallback, object state)
        {
            s_DnsPermission.Demand();

            if (address == null) {
                throw new ArgumentNullException("address");
            }

            if (address.Equals(IPAddress.Any) || address.Equals(IPAddress.IPv6Any))
            {
                throw new ArgumentException(SR.GetString(SR.net_invalid_ip_addr), "address");
            }

            GlobalLog.Print("Dns.HostResolutionBeginHelper: " + address);

            // Set up the context, possibly flow.
            ResolveAsyncResult asyncResult = new ResolveAsyncResult(address, null, includeIPv6, state, requestCallback);
            if (flowContext)
            {
                asyncResult.StartPostingAsyncOp(false);
            }

            // Start the resolve.
            ThreadPool.UnsafeQueueUserWorkItem(resolveCallback, asyncResult);

            // Finish the flowing, maybe it completed?  This does nothing if we didn't initiate the flowing above.
            asyncResult.FinishPostingAsyncOp();
            return asyncResult;
        }


        private static IPHostEntry HostResolutionEndHelper(IAsyncResult asyncResult)
        {
            //
            // parameter validation
            //
            if (asyncResult == null) {
                throw new ArgumentNullException("asyncResult");
            }
            ResolveAsyncResult castedResult = asyncResult as ResolveAsyncResult;
            if (castedResult == null)
            {
                throw new ArgumentException(SR.GetString(SR.net_io_invalidasyncresult), "asyncResult");
            }
            if (castedResult.EndCalled)
            {
                throw new InvalidOperationException(SR.GetString(SR.net_io_invalidendcall, "EndResolve"));
            }

            GlobalLog.Print("Dns.HostResolutionEndHelper");

            castedResult.InternalWaitForCompletion();
            castedResult.EndCalled = true;

            Exception exception = castedResult.Result as Exception;
            if (exception != null)
            {
                throw exception;
            }

            return (IPHostEntry) castedResult.Result;
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [HostProtection(ExternalThreading=true)]
        [Obsolete("BeginGetHostByName is obsoleted for this type, please use BeginGetHostEntry instead. http://go.microsoft.com/fwlink/?linkid=14202")]
        public static IAsyncResult BeginGetHostByName(string hostName, AsyncCallback requestCallback, object stateObject) {
            if(Logging.On)Logging.Enter(Logging.Sockets, "DNS", "BeginGetHostByName", hostName);

            IAsyncResult asyncResult = HostResolutionBeginHelper(hostName, true, true, false, false, requestCallback, stateObject);

            if(Logging.On)Logging.Exit(Logging.Sockets, "DNS", "BeginGetHostByName", asyncResult);
            return asyncResult;
        } // BeginGetHostByName

/* Uncomment when needed.
        internal static IAsyncResult UnsafeBeginGetHostByName(string hostName, AsyncCallback requestCallback, object stateObject)
        {
            if(Logging.On)Logging.Enter(Logging.Sockets, "DNS", "BeginGetHostByName", hostName);

            IAsyncResult asyncResult = HostResolutionBeginHelper(hostName, true, false, false, false, requestCallback, stateObject);

            if(Logging.On)Logging.Exit(Logging.Sockets, "DNS", "BeginGetHostByName", asyncResult);
            return asyncResult;
        } // UnsafeBeginResolve
*/

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>


        [Obsolete("EndGetHostByName is obsoleted for this type, please use EndGetHostEntry instead. http://go.microsoft.com/fwlink/?linkid=14202")]
        public static IPHostEntry EndGetHostByName(IAsyncResult asyncResult) {
            if(Logging.On)Logging.Enter(Logging.Sockets, "DNS", "EndGetHostByName", asyncResult);

            IPHostEntry ipHostEntry = HostResolutionEndHelper(asyncResult);

            if(Logging.On)Logging.Exit(Logging.Sockets, "DNS", "EndGetHostByName", ipHostEntry);
            return ipHostEntry;
        }






        public static IPHostEntry GetHostEntry(string hostNameOrAddress) {
            if(Logging.On)Logging.Enter(Logging.Sockets, "DNS", "GetHostEntry", hostNameOrAddress);
            //
            // demand Unrestricted DnsPermission for this call
            //
            s_DnsPermission.Demand();

            if (hostNameOrAddress == null) {
                throw new ArgumentNullException("hostNameOrAddress");
            }

            // See if it's an IP Address.
            IPAddress address;
            IPHostEntry ipHostEntry;
            if (TryParseAsIP(hostNameOrAddress, out address))
            {
                if (address.Equals(IPAddress.Any) || address.Equals(IPAddress.IPv6Any))
                {
                    throw new ArgumentException(SR.GetString(SR.net_invalid_ip_addr), "hostNameOrAddress");
                }

                ipHostEntry = InternalGetHostByAddress(address, true, false);
            }
            else
            {
                ipHostEntry = InternalGetHostByName(hostNameOrAddress, true);
            }

            if (Logging.On) Logging.Exit(Logging.Sockets, "DNS", "GetHostEntry", ipHostEntry);
            return ipHostEntry;
        }


        public static IPHostEntry GetHostEntry(IPAddress address) {
            if(Logging.On)Logging.Enter(Logging.Sockets, "DNS", "GetHostEntry", "");

            //
            // demand Unrestricted DnsPermission for this call
            //
            s_DnsPermission.Demand();

            if (address == null) {
                throw new ArgumentNullException("address");
            }

            if (address.Equals(IPAddress.Any) || address.Equals(IPAddress.IPv6Any))
            {
                throw new ArgumentException(SR.GetString(SR.net_invalid_ip_addr), "address");
            }

            IPHostEntry ipHostEntry = InternalGetHostByAddress(address, true, false);
            if(Logging.On)Logging.Exit(Logging.Sockets, "DNS", "GetHostEntry", ipHostEntry);
            return ipHostEntry;
        } // GetHostByAddress



        public static IPAddress[] GetHostAddresses(string hostNameOrAddress) {
            if(Logging.On)Logging.Enter(Logging.Sockets, "DNS", "GetHostAddresses", hostNameOrAddress);
            //
            // demand Unrestricted DnsPermission for this call
            //
            s_DnsPermission.Demand();

            if (hostNameOrAddress == null) {
                throw new ArgumentNullException("hostNameOrAddress");
            }

            // See if it's an IP Address.
            IPAddress address;
            IPAddress[] addresses;
            if (TryParseAsIP(hostNameOrAddress, out address))
            {
                if (address.Equals(IPAddress.Any) || address.Equals(IPAddress.IPv6Any))
                {
                    throw new ArgumentException(SR.GetString(SR.net_invalid_ip_addr), "hostNameOrAddress");
                }

                addresses = new IPAddress[] { address };
            }
            else
            {
                // InternalGetHostByName works with IP addresses (and avoids a reverse-lookup), but we need
                // explicit handling in order to do the ArgumentException and guarantee the behavior.
                addresses = InternalGetHostByName(hostNameOrAddress, true).AddressList;
            }

            if(Logging.On)Logging.Exit(Logging.Sockets, "DNS", "GetHostAddresses", addresses);
            return addresses;
        }


        [HostProtection(ExternalThreading=true)]
        public static IAsyncResult BeginGetHostEntry(string hostNameOrAddress, AsyncCallback requestCallback, object stateObject) {
            if(Logging.On)Logging.Enter(Logging.Sockets, "DNS", "BeginGetHostEntry", hostNameOrAddress);

            IAsyncResult asyncResult = HostResolutionBeginHelper(hostNameOrAddress, false, true, true, true, requestCallback, stateObject);

            if(Logging.On)Logging.Exit(Logging.Sockets, "DNS", "BeginGetHostEntry", asyncResult);
            return asyncResult;
        } // BeginResolve



        [HostProtection(ExternalThreading=true)]
        public static IAsyncResult BeginGetHostEntry(IPAddress address, AsyncCallback requestCallback, object stateObject) {
            if(Logging.On)Logging.Enter(Logging.Sockets, "DNS", "BeginGetHostEntry", address);

            IAsyncResult asyncResult = HostResolutionBeginHelper(address, true, true, requestCallback, stateObject);

            if(Logging.On)Logging.Exit(Logging.Sockets, "DNS", "BeginGetHostEntry", asyncResult);
            return asyncResult;
        } // BeginResolve


        public static IPHostEntry EndGetHostEntry(IAsyncResult asyncResult) {
            if(Logging.On)Logging.Enter(Logging.Sockets, "DNS", "EndGetHostEntry", asyncResult);

            IPHostEntry ipHostEntry = HostResolutionEndHelper(asyncResult);

            if(Logging.On)Logging.Exit(Logging.Sockets, "DNS", "EndGetHostEntry", ipHostEntry);
            return ipHostEntry;
        } // EndResolve()


        [HostProtection(ExternalThreading=true)]
        public static IAsyncResult BeginGetHostAddresses(string hostNameOrAddress, AsyncCallback requestCallback, object state) {
            if(Logging.On)Logging.Enter(Logging.Sockets, "DNS", "BeginGetHostAddresses", hostNameOrAddress);

            IAsyncResult asyncResult = HostResolutionBeginHelper(hostNameOrAddress, true, true, true, true, requestCallback, state);

            if(Logging.On)Logging.Exit(Logging.Sockets, "DNS", "BeginGetHostAddresses", asyncResult);
            return asyncResult;
        } // BeginResolve


        public static IPAddress[] EndGetHostAddresses(IAsyncResult asyncResult) {
            if(Logging.On)Logging.Enter(Logging.Sockets, "DNS", "EndGetHostAddresses", asyncResult);

            IPHostEntry ipHostEntry = HostResolutionEndHelper(asyncResult);

            if(Logging.On)Logging.Exit(Logging.Sockets, "DNS", "EndGetHostAddresses", ipHostEntry);
            return ipHostEntry.AddressList;
        } // EndResolveToAddresses


        internal static IAsyncResult UnsafeBeginGetHostAddresses(string hostName, AsyncCallback requestCallback, object state)
        {
            if(Logging.On)Logging.Enter(Logging.Sockets, "DNS", "UnsafeBeginGetHostAddresses", hostName);

            IAsyncResult asyncResult = HostResolutionBeginHelper(hostName, true, false, true, true, requestCallback, state);

            if(Logging.On)Logging.Exit(Logging.Sockets, "DNS", "UnsafeBeginGetHostAddresses", asyncResult);
            return asyncResult;
        } // UnsafeBeginResolveToAddresses


        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [HostProtection(ExternalThreading=true)]
        [Obsolete("BeginResolve is obsoleted for this type, please use BeginGetHostEntry instead. http://go.microsoft.com/fwlink/?linkid=14202")]
        public static IAsyncResult BeginResolve(string hostName, AsyncCallback requestCallback, object stateObject)
        {
            if(Logging.On)Logging.Enter(Logging.Sockets, "DNS", "BeginResolve", hostName);

            IAsyncResult asyncResult = HostResolutionBeginHelper(hostName, false, true, false, false, requestCallback, stateObject);

            if(Logging.On)Logging.Exit(Logging.Sockets, "DNS", "BeginResolve", asyncResult);
            return asyncResult;
        } // BeginResolve


        [Obsolete("EndResolve is obsoleted for this type, please use EndGetHostEntry instead. http://go.microsoft.com/fwlink/?linkid=14202")]
        public static IPHostEntry EndResolve(IAsyncResult asyncResult)
        {
            if(Logging.On)Logging.Enter(Logging.Sockets, "DNS", "EndResolve", asyncResult);

            IPHostEntry ipHostEntry = HostResolutionEndHelper(asyncResult);

            if(Logging.On)Logging.Exit(Logging.Sockets, "DNS", "EndResolve", ipHostEntry);
            return ipHostEntry;
        } // EndResolve()

        /* Uncomment when needed.
                internal static IAsyncResult UnsafeBeginResolve(string hostName, AsyncCallback requestCallback, object stateObject)
                {
                    if(Logging.On)Logging.Enter(Logging.Sockets, "DNS", "BeginResolve", hostName);
        
                    IAsyncResult asyncResult = HostResolutionBeginHelper(hostName, false, false, true, false, requestCallback, stateObject);
        
                    if(Logging.On)Logging.Exit(Logging.Sockets, "DNS", "BeginResolve", asyncResult);
                    return asyncResult;
                } // UnsafeBeginResolve
        */

        //
        // IPv6 Changes: Add getaddrinfo and getnameinfo methods.
        //
        private unsafe static IPHostEntry GetAddrInfo(string name) {
            //
            // Use SocketException here to show operation not supported
            // if, by some nefarious means, this method is called on an
            // unsupported platform.
            //
            throw new SocketException(SocketError.OperationNotSupported);
        }

        internal static string TryGetNameInfo(IPAddress addr, out SocketError errorCode) {
            //
            // Use SocketException here to show operation not supported
            // if, by some nefarious means, this method is called on an
            // unsupported platform.
            //
            throw new SocketException(SocketError.OperationNotSupported);
        }

        private static bool TryParseAsIP(string address, out IPAddress ip)
        {
            return IPAddress.TryParse(address, out ip) &&
                ((ip.AddressFamily == AddressFamily.InterNetwork && Socket.SupportsIPv4) ||
                (ip.AddressFamily == AddressFamily.InterNetworkV6 && Socket.OSSupportsIPv6));
        }
    }
}
