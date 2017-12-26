//------------------------------------------------------------------------------
// <copyright file="ServicePointManager.cs" company="Microsoft">
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
    using System.Collections;
    using System.Collections.Generic;
    using System.Configuration;
    using System.Net.Configuration;
    using System.Net.Sockets;
    using System.Net.Security;
    using System.Security.Permissions;
    using System.Security.Cryptography.X509Certificates;
    using System.Threading;
    using System.Globalization;


    // This turned to be a legacy type name that is simply forwarded to System.Security.Authentication.SslProtocols defined values.

    //
    // The ServicePointManager class hands out ServicePoints (may exist or be created
    // as needed) and makes sure they are garbage collected when they expire.
    // The ServicePointManager runs in its own thread so that it never
    //

    /// <devdoc>
    /// <para>Manages the collection of <see cref='System.Net.ServicePoint'/> instances.</para>
    /// </devdoc>
    ///
    public class ServicePointManager {

        /// <devdoc>
        ///    <para>
        ///       The number of non-persistant connections allowed on a <see cref='System.Net.ServicePoint'/>.
        ///    </para>
        /// </devdoc>
        public const int DefaultNonPersistentConnectionLimit = 4;
        /// <devdoc>
        ///    <para>
        ///       The default number of persistent connections allowed on a <see cref='System.Net.ServicePoint'/>.
        ///    </para>
        /// </devdoc>
        public const int DefaultPersistentConnectionLimit = 2;

        /// <devdoc>
        ///    <para>
        ///       The default number of persistent connections when runninger under ASP+.
        ///    </para>
        /// </devdoc>
        private const int DefaultAspPersistentConnectionLimit = 10;


        internal static readonly string SpecialConnectGroupName = "/.NET/NetClasses/HttpWebRequest/CONNECT__Group$$/";
        internal static readonly TimerThread.Callback s_IdleServicePointTimeoutDelegate = new TimerThread.Callback(IdleServicePointTimeoutCallback);


        //
        // data  - only statics used
        //

        //
        // s_ServicePointTable - Uri of ServicePoint is the hash key
        // We provide our own comparer function that knows about Uris
        //

        //also used as a lock object
        private static Hashtable s_ServicePointTable = new Hashtable(10);

        // IIS6 has 120 sec for an idle connection timeout, we should have a little bit less.
        private static TimerThread.Queue s_ServicePointIdlingQueue = TimerThread.GetOrCreateQueue(100 * 1000);
        private static int s_MaxServicePoints = 0;
        private static Hashtable s_ConfigTable = null;
        private static int s_ConnectionLimit = PersistentConnectionLimit;


        //
        // InternalConnectionLimit -
        //  set/get Connection Limit on demand, checking config beforehand
        //

        private static bool s_UserChangedLimit;
        private static int InternalConnectionLimit {
            get {
                if (s_ConfigTable == null) {
                    // init config
                    s_ConfigTable = ConfigTable;
                }
                return s_ConnectionLimit;
            }
            set {
                if (s_ConfigTable == null) {
                    // init config
                    s_ConfigTable = ConfigTable;
                }
                s_UserChangedLimit = true;
                s_ConnectionLimit = value;
            }
        }

        //
        // PersistentConnectionLimit -
        //  Determines the correct connection limit based on whether with running with ASP+
        //  The following order is followed generally for figuring what ConnectionLimit size to use
        //    1.    If ServicePoint.ConnectionLimit is set, then take that value
        //    2.    If ServicePoint has a specific config setting, then take that value
        //    3.    If ServicePoint.DefaultConnectionLimit is set, then take that value
        //    4.    If ServicePoint is localhost, then set to infinite (TO Should we change this value?)
        //    5.    If ServicePointManager has a default config connection limit setting, then take that value
        //    6.    If ServicePoint is running under ASP+, then set value to 10, else set it to 2
        //
        private static int PersistentConnectionLimit {
            get {
                {
                    return DefaultPersistentConnectionLimit;
                }
            }
        }

        /* Consider Removing
        //
        // InternalServicePointCount -
        //  Gets the active number of ServicePoints being used
        //
        internal static int InternalServicePointCount {
            get {
                return s_ServicePointTable.Count;
            }
        }
        */

        [System.Diagnostics.Conditional("DEBUG")]
        internal static void Debug(int requestHash) {
            try {
                foreach (WeakReference servicePointReference in  s_ServicePointTable) {
                    ServicePoint servicePoint;
                    if (servicePointReference != null && servicePointReference.IsAlive) {
                        servicePoint = (ServicePoint)servicePointReference.Target;
                    }
                    else {
                        servicePoint = null;
                    }
                    if (servicePoint!=null) {
                        servicePoint.Debug(requestHash);
                    }
                }
            }
            catch (Exception e) {
                if (e is ThreadAbortException || e is StackOverflowException || e is OutOfMemoryException) {
                    throw;
                }
            }
            catch {
            }
        }

        //
        // ConfigTable -
        // read ConfigTable from Config, or create
        //  a default on failure
        //

        private static Hashtable ConfigTable {
            get {
                if (s_ConfigTable == null) {
                    lock(s_ServicePointTable) {
                        if (s_ConfigTable == null) {
                            Hashtable configTable;
                            configTable = (Hashtable)
                                ConnectionManagementSectionInternal.GetSection().ConnectionManagement;

                            if (configTable == null) {
                                configTable = new Hashtable();
                            }

                            // we piggy back loading the ConnectionLimit here
                            if (configTable.ContainsKey("*") ) {
                                int connectionLimit  = (int) configTable["*"];
                                if ( connectionLimit < 1 ) {
                                    connectionLimit = PersistentConnectionLimit;
                                }
                                s_ConnectionLimit = connectionLimit;
                            }
                            s_ConfigTable = configTable;
                        }
                    }
                }
                return s_ConfigTable;
            }
        }


        internal static TimerThread.Callback IdleServicePointTimeoutDelegate
        {
            get
            {
                return s_IdleServicePointTimeoutDelegate;
            }
        }

        private static void IdleServicePointTimeoutCallback(TimerThread.Timer timer, int timeNoticed, object context)
        {
            ServicePoint servicePoint = (ServicePoint) context;

            lock (s_ServicePointTable)
            {
                s_ServicePointTable.Remove(servicePoint.LookupString);
            }

            servicePoint.ReleaseAllConnectionGroups();
        }


        //
        // constructors
        //

        private ServicePointManager() {
        }


        //
        // accessors
        //

        /// <devdoc>
        /// <para>Gets or sets the maximum number of <see cref='System.Net.ServicePoint'/> instances that should be maintained at any
        ///    time.</para>
        /// </devdoc>
        public static int MaxServicePoints {
            get {
                return s_MaxServicePoints;
            }
            set {
                ExceptionHelper.WebPermissionUnrestricted.Demand();
                if (!ValidationHelper.ValidateRange(value, 0, Int32.MaxValue)) {
                    throw new ArgumentOutOfRangeException("value");
                }
                s_MaxServicePoints = value;
            }
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public static int DefaultConnectionLimit {
            get {
                return InternalConnectionLimit;
            }
            set {
                ExceptionHelper.WebPermissionUnrestricted.Demand();
                if (value > 0) {
                    InternalConnectionLimit = value;

                }
                else {
                    throw new ArgumentOutOfRangeException(SR.GetString(SR.net_toosmall));
                }
            }
        }



        /// <devdoc>
        /// <para>Gets or sets the maximum idle time in seconds of a <see cref='System.Net.ServicePoint'/>.</para>
        /// </devdoc>
        public static int MaxServicePointIdleTime {
            get {
                return s_ServicePointIdlingQueue.Duration;
            }
            set {
                ExceptionHelper.WebPermissionUnrestricted.Demand();
                if ( !ValidationHelper.ValidateRange(value, Timeout.Infinite, Int32.MaxValue)) {
                    throw new ArgumentOutOfRangeException("value");
                }
                if (s_ServicePointIdlingQueue.Duration != value)
                {
                    s_ServicePointIdlingQueue = TimerThread.GetOrCreateQueue(value);
                }
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets indication whether use of the Nagling algorithm is desired.
        ///       Changing this value does not affect existing <see cref='System.Net.ServicePoint'/> instances but only to new ones that are created from that moment on.
        ///    </para>
        /// </devdoc>
        public static bool UseNagleAlgorithm {
            get {
                return SettingsSectionInternal.Section.UseNagleAlgorithm;
            }
            set {
                SettingsSectionInternal.Section.UseNagleAlgorithm = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets indication whether 100-continue behaviour is desired.
        ///       Changing this value does not affect existing <see cref='System.Net.ServicePoint'/> instances but only to new ones that are created from that moment on.
        ///    </para>
        /// </devdoc>
        public static bool Expect100Continue {
            get {
                return SettingsSectionInternal.Section.Expect100Continue;
            }
            set {
                SettingsSectionInternal.Section.Expect100Continue = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///         Enables the use of DNS round robin access, meaning a different IP
        ///         address may be used on each connection, when more than one IP is availble
        ///    </para>
        /// </devdoc>
        public static bool EnableDnsRoundRobin {
            get {
                return SettingsSectionInternal.Section.EnableDnsRoundRobin;
            }
            set {
                SettingsSectionInternal.Section.EnableDnsRoundRobin = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Causes us to go back and reresolve addresses through DNS, even when
        ///       there were no recorded failures.  -1 is infinite.  Time should be in ms
        ///    </para>
        /// </devdoc>
        public static int DnsRefreshTimeout {
            get {
                return SettingsSectionInternal.Section.DnsRefreshTimeout;
            }
            set {
                if(value < -1){
                    SettingsSectionInternal.Section.DnsRefreshTimeout = -1;
                }
                else{
                    SettingsSectionInternal.Section.DnsRefreshTimeout = value;
                }
            }
        }


        public static bool CheckCertificateRevocationList {
            get {
                return SettingsSectionInternal.Section.CheckCertificateRevocationList;
            }
            set {
                //Prevent an applet to override default certificate checking
                ExceptionHelper.UnmanagedPermission.Demand();
                SettingsSectionInternal.Section.CheckCertificateRevocationList = value;
            }
        }

        internal static bool CheckCertificateName {
            get {
                return SettingsSectionInternal.Section.CheckCertificateName;
            }
        }

        //
        // class methods
        //

        //
        // MakeQueryString - Just a short macro to handle creating the query
        //  string that we search for host ports in the host list
        //
        internal static string MakeQueryString(Uri address) {
            if (address.IsDefaultPort)
                return address.Scheme + "://" + address.DnsSafeHost;
            else
                return address.Scheme + "://" + address.DnsSafeHost + ":" + address.Port.ToString();
        }

        internal static string MakeQueryString(Uri address1, bool isProxy) {
           if (isProxy) {
               return MakeQueryString(address1) + "://proxy";
           }
           else {
               return MakeQueryString(address1);
           }
        }

        //
        // FindServicePoint - Query using an Uri string for a given ServerPoint Object
        //

        /// <devdoc>
        /// <para>Finds an existing <see cref='System.Net.ServicePoint'/> or creates a new <see cref='System.Net.ServicePoint'/> to manage communications to the
        ///    specified Uniform Resource Identifier.</para>
        /// </devdoc>
        public static ServicePoint FindServicePoint(Uri address) {
            return FindServicePoint(address, null);
        }


        /// <devdoc>
        /// <para>Finds an existing <see cref='System.Net.ServicePoint'/> or creates a new <see cref='System.Net.ServicePoint'/> to manage communications to the
        ///    specified Uniform Resource Identifier.</para>
        /// </devdoc>
        public static ServicePoint FindServicePoint(string uriString, IWebProxy proxy) {
            Uri uri = new Uri(uriString);
            return FindServicePoint(uri, proxy);
        }


        //
        // FindServicePoint - Query using an Uri for a given server point
        //

        /// <devdoc>
        /// <para>Findes an existing <see cref='System.Net.ServicePoint'/> or creates a new <see cref='System.Net.ServicePoint'/> to manage communications to the specified <see cref='System.Uri'/>
        /// instance.</para>
        /// </devdoc>
        public static ServicePoint FindServicePoint(Uri address, IWebProxy proxy) {
            ProxyChain chain;
            HttpAbortDelegate abortDelegate = null;
            int abortState = 0;
            return FindServicePoint(address, proxy, out chain, ref abortDelegate, ref abortState);
        }

        // If abortState becomes non-zero, the attempt to find a service point has been aborted.
        internal static ServicePoint FindServicePoint(Uri address, IWebProxy proxy, out ProxyChain chain, ref HttpAbortDelegate abortDelegate, ref int abortState)
        {
            if (address==null) {
                throw new ArgumentNullException("address");
            }
            GlobalLog.Enter("ServicePointManager::FindServicePoint() address:" + address.ToString());

            bool isProxyServicePoint = false;
            chain = null;

            //
            // find proxy info, and then switch on proxy
            //
            Uri proxyAddress = null;
            if (proxy!=null  && !address.IsLoopback) {
                IAutoWebProxy autoProxy = proxy as IAutoWebProxy;
                if (autoProxy != null)
                {
                    chain = autoProxy.GetProxies(address);

                    // Set up our ability to abort this MoveNext call.  Note that the current implementations of ProxyChain will only
                    // take time on the first call, so this is the only place we do this.  If a new ProxyChain takes time in later
                    // calls, this logic should be copied to other places MoveNext is called.
                    GlobalLog.Assert(abortDelegate == null, "ServicePointManager::FindServicePoint()|AbortDelegate already set.");
                    abortDelegate = chain.HttpAbortDelegate;
                    try
                    {
                        Thread.MemoryBarrier();
                        if (abortState != 0)
                        {
                            Exception exception = new WebException(NetRes.GetWebStatusString(WebExceptionStatus.RequestCanceled), WebExceptionStatus.RequestCanceled);
                            GlobalLog.LeaveException("ServicePointManager::FindServicePoint() Request aborted before proxy lookup.", exception);
                            throw exception;
                        }

                        if (!chain.Enumerator.MoveNext())
                        {
                            GlobalLog.Assert("ServicePointManager::FindServicePoint()|GetProxies() returned zero proxies.");
/*
                            Exception exception = new WebException(NetRes.GetWebStatusString(WebExceptionStatus.RequestProhibitedByProxy), WebExceptionStatus.RequestProhibitedByProxy);
                            GlobalLog.LeaveException("ServicePointManager::FindServicePoint() Proxy prevented request.", exception);
                            throw exception;
*/
                        }
                        proxyAddress = chain.Enumerator.Current;
                    }
                    finally
                    {
                        abortDelegate = null;
                    }
                }
                else if (!proxy.IsBypassed(address))
                {
                    // use proxy support
                    // rework address
                    proxyAddress = proxy.GetProxy(address);
                }

                // null means DIRECT
                if (proxyAddress!=null) {
                    address = proxyAddress;
                    isProxyServicePoint = true;
                }
            }

            ServicePoint servicePoint = FindServicePointHelper(address, isProxyServicePoint);
            GlobalLog.Leave("ServicePointManager::FindServicePoint() servicePoint#" + ValidationHelper.HashString(servicePoint));
            return servicePoint;
        }

        // Returns null if we get to the end of the chain.
        internal static ServicePoint FindServicePoint(ProxyChain chain)
        {
            GlobalLog.Print("ServicePointManager::FindServicePoint() Calling chained version.");
            if (!chain.Enumerator.MoveNext())
            {
                return null;
            }

            Uri proxyAddress = chain.Enumerator.Current;
            return FindServicePointHelper(proxyAddress == null ? chain.Destination : proxyAddress, proxyAddress != null);
        }

        private static ServicePoint FindServicePointHelper(Uri address, bool isProxyServicePoint)
        {
            GlobalLog.Enter("ServicePointManager::FindServicePointHelper() address:" + address.ToString());

            if (isProxyServicePoint)
            {
                if (address.Scheme != Uri.UriSchemeHttp)
                {
                    Exception exception = new NotSupportedException(SR.GetString(SR.net_proxyschemenotsupported, address.Scheme));
                    GlobalLog.LeaveException("ServicePointManager::FindServicePointHelper() proxy has unsupported scheme:" + address.Scheme.ToString(), exception);
                    throw exception;
                }
            }

            //
            // Search for the correct proxy host,
            //  then match its acutal host by using ConnectionGroups
            //  which are located on the actual ServicePoint.
            //
            string tempEntry = MakeQueryString(address, isProxyServicePoint);

            // lookup service point in the table
            ServicePoint servicePoint = null;
            GlobalLog.Print("ServicePointManager::FindServicePointHelper() locking and looking up tempEntry:[" + tempEntry.ToString() + "]");
            lock (s_ServicePointTable) {
                // once we grab the lock, check if it wasn't already added
                WeakReference servicePointReference =  s_ServicePointTable[tempEntry] as WeakReference;
                GlobalLog.Print("ServicePointManager::FindServicePointHelper() lookup returned WeakReference#" + ValidationHelper.HashString(servicePointReference));
                if ( servicePointReference != null ) {
                    servicePoint = (ServicePoint)servicePointReference.Target;
                    GlobalLog.Print("ServicePointManager::FindServicePointHelper() successful lookup returned ServicePoint#" + ValidationHelper.HashString(servicePoint));
                }
                if (servicePoint==null) {
                    // lookup failure or timeout, we need to create a new ServicePoint
                    if (s_MaxServicePoints<=0 || s_ServicePointTable.Count<s_MaxServicePoints) {
                        // Determine Connection Limit
                        int connectionLimit = InternalConnectionLimit;
                        string schemeHostPort = MakeQueryString(address);
                        bool userDefined = s_UserChangedLimit;
                        if (ConfigTable.ContainsKey(schemeHostPort) ) {
                            connectionLimit = (int) ConfigTable[schemeHostPort];
                            userDefined = true;
                        }
                        servicePoint = new ServicePoint(address, s_ServicePointIdlingQueue, connectionLimit, tempEntry, userDefined, isProxyServicePoint);
                        GlobalLog.Print("ServicePointManager::FindServicePointHelper() created ServicePoint#" + ValidationHelper.HashString(servicePoint));
                        servicePointReference = new WeakReference(servicePoint);
                        s_ServicePointTable[tempEntry] = servicePointReference;
                        GlobalLog.Print("ServicePointManager::FindServicePointHelper() adding entry WeakReference#" + ValidationHelper.HashString(servicePointReference) + " key:[" + tempEntry + "]");
                    }
                    else {
                        Exception exception = new InvalidOperationException(SR.GetString(SR.net_maxsrvpoints));
                        GlobalLog.LeaveException("ServicePointManager::FindServicePointHelper() reached the limit count:" + s_ServicePointTable.Count.ToString() + " limit:" + s_MaxServicePoints.ToString(), exception);
                        throw exception;
                    }
                }
            }

            GlobalLog.Leave("ServicePointManager::FindServicePointHelper() servicePoint#" + ValidationHelper.HashString(servicePoint));
            return servicePoint;
        }

        //
        // FindServicePoint - Query using an Uri for a given server point
        //

        /// <devdoc>
        /// <para>Findes an existing <see cref='System.Net.ServicePoint'/> or creates a new <see cref='System.Net.ServicePoint'/> to manage communications to the specified <see cref='System.Uri'/>
        /// instance.</para>
        /// </devdoc>
        internal static ServicePoint FindServicePoint(string host, int port) {
            if (host==null) {
                throw new ArgumentNullException("address");
            }
            GlobalLog.Enter("ServicePointManager::FindServicePoint() host:" + host.ToString());

            string tempEntry = null;
            bool isProxyServicePoint = false;


            //
            // Search for the correct proxy host,
            //  then match its acutal host by using ConnectionGroups
            //  which are located on the actual ServicePoint.
            //
            tempEntry = "ByHost:"+host+":"+port.ToString(CultureInfo.InvariantCulture);
            // lookup service point in the table
            ServicePoint servicePoint = null;
            GlobalLog.Print("ServicePointManager::FindServicePoint() locking and looking up tempEntry:[" + tempEntry.ToString() + "]");
            lock (s_ServicePointTable) {
                // once we grab the lock, check if it wasn't already added
                WeakReference servicePointReference =  s_ServicePointTable[tempEntry] as WeakReference;
                GlobalLog.Print("ServicePointManager::FindServicePoint() lookup returned WeakReference#" + ValidationHelper.HashString(servicePointReference));
                if ( servicePointReference != null ) {
                    servicePoint = (ServicePoint)servicePointReference.Target;
                    GlobalLog.Print("ServicePointManager::FindServicePoint() successfull lookup returned ServicePoint#" + ValidationHelper.HashString(servicePoint));
                }
                if (servicePoint==null) {
                    // lookup failure or timeout, we need to create a new ServicePoint
                    if (s_MaxServicePoints<=0 || s_ServicePointTable.Count<s_MaxServicePoints) {
                        // Determine Connection Limit
                        int connectionLimit = InternalConnectionLimit;
                        bool userDefined = s_UserChangedLimit;
                        string schemeHostPort =host+":"+port.ToString(CultureInfo.InvariantCulture);

                        if (ConfigTable.ContainsKey(schemeHostPort) ) {
                            connectionLimit = (int) ConfigTable[schemeHostPort];
                            userDefined = true;
                        }
                        servicePoint = new ServicePoint(host, port, s_ServicePointIdlingQueue, connectionLimit, tempEntry, userDefined, isProxyServicePoint);
                        GlobalLog.Print("ServicePointManager::FindServicePoint() created ServicePoint#" + ValidationHelper.HashString(servicePoint));
                        servicePointReference = new WeakReference(servicePoint);
                        s_ServicePointTable[tempEntry] = servicePointReference;
                        GlobalLog.Print("ServicePointManager::FindServicePoint() adding entry WeakReference#" + ValidationHelper.HashString(servicePointReference) + " key:[" + tempEntry + "]");
                    }
                    else {
                        Exception exception = new InvalidOperationException(SR.GetString(SR.net_maxsrvpoints));
                        GlobalLog.LeaveException("ServicePointManager::FindServicePoint() reached the limit count:" + s_ServicePointTable.Count.ToString() + " limit:" + s_MaxServicePoints.ToString(), exception);
                        throw exception;
                    }
                }
            }

            GlobalLog.Leave("ServicePointManager::FindServicePoint() servicePoint#" + ValidationHelper.HashString(servicePoint));
            return servicePoint;
        }
    }
}
