//------------------------------------------------------------------------------
// <copyright file="webproxy.cs" company="Microsoft">
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
    using System.Net.NetworkInformation;
    using System.Globalization;
    using System.Security.Permissions;
    using System.Text;
    using System.Text.RegularExpressions;
    using System.Collections;
    using System.Collections.Specialized;
    using System.Runtime.Serialization;
    using System.ComponentModel;
    using System.Threading;

    class WebProxyData {
        internal bool bypassOnLocal;
        internal bool automaticallyDetectSettings;
        internal Uri proxyAddress;
        internal Uri scriptLocation;
        internal ArrayList bypassList;
    }

    /// <devdoc>
    ///    <para>
    ///       Handles default proxy setting implimentation for the Http proxy.
    ///    </para>
    /// </devdoc>
    [Serializable]
    public class WebProxy : IAutoWebProxy, ISerializable {
        // these are settable by the user
        private bool _UseRegistry;    // This is just around for serialization. Can we get rid of it?
        private bool _BypassOnLocal;
        private bool m_EnableAutoproxy;
        private Uri _ProxyAddress;
        private ArrayList _BypassList;
        private ICredentials _Credentials;

        // these are computed on the fly
        private Regex[] _RegExBypassList;
        private Hashtable _ProxyHostAddresses;

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public WebProxy()
            : this((Uri) null, false, null, null) {
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public WebProxy(Uri Address)
            : this(Address, false, null, null) {
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public WebProxy(Uri Address, bool BypassOnLocal)
            : this(Address, BypassOnLocal, null, null) {
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public WebProxy(Uri Address, bool BypassOnLocal, string[] BypassList)
            : this(Address, BypassOnLocal, BypassList, null) {
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        internal WebProxy(Hashtable proxyHostAddresses, bool BypassOnLocal, string[] BypassList)
            : this((Uri) null, BypassOnLocal, BypassList, null) {
            _ProxyHostAddresses = proxyHostAddresses;
            if (_ProxyHostAddresses != null ) {
                _ProxyAddress = (Uri) proxyHostAddresses["http"];
            }
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public WebProxy(Uri Address, bool BypassOnLocal, string[] BypassList, ICredentials Credentials) {
            _ProxyAddress = Address;
            _BypassOnLocal = BypassOnLocal;
            if (BypassList != null) {
                _BypassList = new ArrayList(BypassList);
                UpdateRegExList(true);
            }
            _Credentials = Credentials;
            m_EnableAutoproxy = true;
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public WebProxy(string Host, int Port)
            : this(new Uri("http://" + Host + ":" + Port.ToString(CultureInfo.InvariantCulture)), false, null, null) {
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public WebProxy(string Address)
            : this(CreateProxyUri(Address), false, null, null) {
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public WebProxy(string Address, bool BypassOnLocal)
            : this(CreateProxyUri(Address), BypassOnLocal, null, null) {
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public WebProxy(string Address, bool BypassOnLocal, string[] BypassList)
            : this(CreateProxyUri(Address), BypassOnLocal, BypassList, null) {
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public WebProxy(string Address, bool BypassOnLocal, string[] BypassList, ICredentials Credentials)
            : this(CreateProxyUri(Address), BypassOnLocal, BypassList, Credentials) {
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public Uri Address {
            get {
               return _ProxyAddress;
            }
            set {
                _UseRegistry = false;
                DeleteScriptEngine();
               _ProxyHostAddresses = null;  // hash list of proxies                                                
               _ProxyAddress = value;
            }
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        internal bool AutoDetect
        {
            set {
                GlobalLog.Assert(_UseRegistry == false, "Cannot set AutoDetect if we are using registry for proxy settings");
                GlobalLog.Assert(m_EnableAutoproxy, "WebProxy#{0}::.ctor()|Cannot set AutoDetect if usesystemdefault is set.", ValidationHelper.HashString(this));

                if (ScriptEngine == null)
                {
                    ScriptEngine = new AutoWebProxyScriptEngine(this, false);
                }
                ScriptEngine.AutomaticallyDetectSettings = value;
            }
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        internal Uri ScriptLocation {
            set {
                GlobalLog.Assert(value != null, "Cannot set ScriptLocation to null");
                GlobalLog.Assert(_UseRegistry == false, "Cannot set AutoDetect if we are using registry for proxy settings");
                GlobalLog.Assert(m_EnableAutoproxy, "WebProxy#{0}::.ctor()|Cannot set ScriptLocation if usesystemdefault is set.", ValidationHelper.HashString(this));

                if (ScriptEngine == null)
                {
                    ScriptEngine = new AutoWebProxyScriptEngine(this, false);
                }
                ScriptEngine.AutomaticConfigurationScript = value;
            }
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool BypassProxyOnLocal {
            get {
                return _BypassOnLocal;
            }
            set {
                _UseRegistry = false;
                DeleteScriptEngine();
                _BypassOnLocal = value;
            }
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string[] BypassList {
            get {
                if (_BypassList == null) {
                    _BypassList = new ArrayList();
                }
                return (string[])_BypassList.ToArray(typeof(string));
            }
            set {
                _UseRegistry = false;
                DeleteScriptEngine();
                _BypassList = new ArrayList(value);
                UpdateRegExList(true);
            }
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public ICredentials Credentials {
            get {
                return _Credentials;
            }
            set {
                _Credentials = value;
            }
        }

        /// <devdoc>
        ///    <para>Sets Credentials to CredentialCache.DefaultCredentials</para>
        /// </devdoc>
        public bool UseDefaultCredentials  {
            get {
                return (Credentials is SystemNetworkCredential) ? true : false;
            }
            set {
                _Credentials = value ? CredentialCache.DefaultCredentials : null;
            }
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public ArrayList BypassArrayList {
            get {
                if ( _BypassList == null ) {
                    _BypassList = new ArrayList();
                }
                return _BypassList;
            }
        }

        internal void CheckForChanges() {
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public Uri GetProxy(Uri destination) {
            GlobalLog.Print("WebProxy#" + ValidationHelper.HashString(this) + "::GetProxy() destination:" + ValidationHelper.ToString(destination));
            if (destination == null)
            {
                throw new ArgumentNullException("destination");
            }

            AutoWebProxyState autoWebProxyState;
            Uri result = GetProxyAuto(destination, out autoWebProxyState);
            if (autoWebProxyState==AutoWebProxyState.ExecutionSuccess) {
                return result;
            }
            if (IsBypassedManual(destination)) {
                return destination;
            }
            Hashtable proxyHostAddresses = _ProxyHostAddresses;
            Uri proxy = proxyHostAddresses!=null ? proxyHostAddresses[destination.Scheme] as Uri : _ProxyAddress;
            return proxy!=null? proxy : destination;
        }

        //
        // CreateProxyUri - maps string to Uri
        //

        private static Uri CreateProxyUri(string address) {
            if (address == null) {
                return null;
            }
            if (address.IndexOf("://") == -1) {
                address = "http://" + address;
            }
            return new Uri(address);
        }

        //
        // UpdateRegExList - Update internal _RegExBypassList
        //  warning - can throw if the RegEx doesn't parse??
        //
        private void UpdateRegExList(bool canThrow) {
            Regex[] regExBypassList = null;
            ArrayList bypassList = _BypassList;
            try {
                if ( bypassList != null && bypassList.Count > 0 ) {
                    regExBypassList = new Regex[bypassList.Count];
                    for (int i = 0; i < bypassList.Count; i++ ) {
                        regExBypassList[i] = new Regex((string)bypassList[i], RegexOptions.IgnoreCase | RegexOptions.CultureInvariant);
                    }
                }
            }
            catch {
                if (!canThrow) {
                    _RegExBypassList = null;
                    return;
                }
                throw;
            }
            // only update here, cause it could throw earlier in the loop
            _RegExBypassList = regExBypassList;
        }

        //
        // IsMatchInBypassList - match input against _RegExBypassList
        //
        private bool IsMatchInBypassList(Uri input) {
            UpdateRegExList(false);
            if ( _RegExBypassList == null ) {
                return false;
            }
            string matchUriString = input.Scheme + "://" + input.Host + (!input.IsDefaultPort ? (":"+input.Port) : "" );
            for (int i = 0; i < _BypassList.Count; i++ ) {
                if (_RegExBypassList[i].IsMatch(matchUriString)) {
                    return true;
                }
            }
            return false;
        }

        /// <devdoc>
        /// Determines if the host Uri should be routed locally or go through the proxy.
        /// </devdoc>
        private bool IsLocal(Uri host) {
            string hostString = host.Host;

            // If and only if the host has a colon or is all numbers and dots, it is an IP address.
            // If no dot, it is local.  (If it has both a dot and a colon, undefined.)
            int dot = -1;
            bool isIp4 = true;
            bool isIp6 = false;
            for (int i = 0; i < hostString.Length; i++)
            {
                if (hostString[i] == '.')
                {
                    if (dot == -1)
                    {
                        dot = i;
                        if (!isIp4)
                        {
                            break;
                        }
                    }

                    continue;
                }

                if (hostString[i] == ':')
                {
                    isIp6 = true;
                    isIp4 = false;
                    break;
                }

                if (hostString[i] < '0' || hostString[i] > '9')
                {
                    isIp4 = false;
                    if (dot != -1)
                    {
                        break;
                    }
                }
            }

            // No dot?  Local.
            // The case of all digits is a little weird.  Technically it could be a v4 IP, but practically it can only
            // be interpreted as an intranet name.
            if (dot == -1 && !isIp6)
            {
                return true;
            }

            if (isIp4 || isIp6)
            {
                // I don't see why this would fail, but in case it does, it's not an IP.
                try
                {
                    IPAddress hostAddress = IPAddress.Parse(hostString);
                    if (IPAddress.IsLoopback(hostAddress))
                    {
                        return true;
                    }
                    return NclUtilities.IsAddressLocal(hostAddress);
                }
                catch (FormatException) { }
            }

            // If it matches the primary domain, it's local.  (Whether or not the hostname matches.)
            string local = "." + IPGlobalProperties.InternalGetIPGlobalProperties().DomainName;
            if (local !=  null && local.Length == (hostString.Length - dot) &&
                string.Compare(local, 0, hostString, dot, local.Length, StringComparison.OrdinalIgnoreCase ) == 0) {
                return true;
            }
            return false;
        }

        /// <devdoc>
        /// Determines if the host Uri should be routed locally or go through a proxy.
        /// </devdoc>
        private bool IsLocalInProxyHash(Uri host) {
            Hashtable proxyHostAddresses = _ProxyHostAddresses;
            if (proxyHostAddresses != null) {
                Uri proxy = (Uri) proxyHostAddresses[host.Scheme];
                if (proxy == null) {
                    return true; // no proxy entry for this scheme, then bypass
                }
            }
            return false;
        }


        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool IsBypassed(Uri host) {
            GlobalLog.Print("WebProxy#" + ValidationHelper.HashString(this) + "::IsBypassed() destination:" + ValidationHelper.ToString(host));
            if (host == null)
            {
                throw new ArgumentNullException("host");
            }

            AutoWebProxyState autoWebProxyState;
            bool result = IsBypassedAuto(host, out autoWebProxyState);
            if (autoWebProxyState==AutoWebProxyState.ExecutionSuccess) {
                return result;
            }
            return IsBypassedManual(host);
        }

        private bool IsBypassedManual(Uri host) {
            if (host.IsLoopback) {
                return true; // bypass localhost from using a proxy.
            }
            return (_ProxyAddress==null && _ProxyHostAddresses==null) || (_BypassOnLocal && IsLocal(host)) || IsMatchInBypassList(host) || IsLocalInProxyHash(host);
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [Obsolete("This method has been deprecated. Please use the proxy selected for you by default. http://go.microsoft.com/fwlink/?linkid=14202")]
        public static WebProxy GetDefaultProxy() {
            ExceptionHelper.WebPermissionUnrestricted.Demand();
            return new WebProxy(true);
        }

        //
        // ISerializable constructor
        //
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected WebProxy(SerializationInfo serializationInfo, StreamingContext streamingContext) {
            // first check for useRegistry on the serialized proxy
            bool useRegistry = false;
            try {
                useRegistry = serializationInfo.GetBoolean("_UseRegistry");
            }
            catch {
            }
            if (useRegistry) {
                // just make the proxy advanced, don't populate with any settings
                // note - this will happen in the context of the user performing the deserialization (their proxy settings get read)
                ExceptionHelper.WebPermissionUnrestricted.Demand();
                UnsafeUpdateFromRegistry();
                return;
            }
            // normal proxy
            _ProxyAddress   = (Uri)serializationInfo.GetValue("_ProxyAddress", typeof(Uri));
            _BypassOnLocal  = serializationInfo.GetBoolean("_BypassOnLocal");
            _BypassList     = (ArrayList)serializationInfo.GetValue("_BypassList", typeof(ArrayList));
            try {
                UseDefaultCredentials = serializationInfo.GetBoolean("_UseDefaultCredentials");
            }
            catch {
            }
        }

        //
        // ISerializable method
        //
        /// <internalonly/>
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [SecurityPermission(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter, SerializationFormatter=true)]
        void ISerializable.GetObjectData(SerializationInfo serializationInfo, StreamingContext streamingContext)
        {
            GetObjectData(serializationInfo, streamingContext);
        }

        //
        // FxCop: provide a way for derived classes to access this method even if they reimplement ISerializable.
        //
        [SecurityPermission(SecurityAction.LinkDemand, SerializationFormatter=true)]
        protected virtual void GetObjectData(SerializationInfo serializationInfo, StreamingContext streamingContext)
        {
            serializationInfo.AddValue("_BypassOnLocal", _BypassOnLocal);
            serializationInfo.AddValue("_ProxyAddress", _ProxyAddress);
            serializationInfo.AddValue("_BypassList", _BypassList);
            serializationInfo.AddValue("_UseDefaultCredentials", UseDefaultCredentials);
            if (_UseRegistry) {
                serializationInfo.AddValue("_UseRegistry", true);
            }
        }


        /// <summary>
        ///     Handles proxy settings by using Internet Explorer based settings,
        ///     keep in mind the security implications when downloading and running
        ///     script from any network source configured in Internet Explorer.
        /// </summary>

        private AutoWebProxyScriptEngine m_ScriptEngine;

        internal AutoWebProxyScriptEngine ScriptEngine {
            get {
                return m_ScriptEngine;
            }
            set {
                m_ScriptEngine = value;
            }
        }

        internal WebProxy(bool enableAutoproxy)
        {
            m_EnableAutoproxy = enableAutoproxy;
            UnsafeUpdateFromRegistry();
        }

        internal void DeleteScriptEngine() {
            if (ScriptEngine != null) {
                ScriptEngine.Close();
                ScriptEngine = null;
            }
        }

        internal void UnsafeUpdateFromRegistry() {
            GlobalLog.Assert(!_UseRegistry, "WebProxy#{0}::UnsafeUpdateFromRegistry()|_UseRegistry ScriptEngine#{1}", ValidationHelper.HashString(this), ValidationHelper.HashString(m_ScriptEngine));
            _UseRegistry = true;
        }

        internal void Update(WebProxyData webProxyData) {
            // update needs to happen atomically
            lock (this) {
                _BypassOnLocal = webProxyData.bypassOnLocal;
                _ProxyAddress = webProxyData.proxyAddress;
                _BypassList = webProxyData.bypassList;

                ScriptEngine.AutomaticallyDetectSettings = m_EnableAutoproxy && webProxyData.automaticallyDetectSettings;
                ScriptEngine.AutomaticConfigurationScript = m_EnableAutoproxy ? webProxyData.scriptLocation : null;
            }
        }

        ProxyChain IAutoWebProxy.GetProxies(Uri destination) {
            GlobalLog.Print("WebProxy#" + ValidationHelper.HashString(this) + "::GetProxies() destination:" + ValidationHelper.ToString(destination));
            if (destination == null)
            {
                throw new ArgumentNullException("destination");
            }
            return new ProxyScriptChain(this, destination);
        }


        //
        // IWebProxy implementation
        //

        // Get proxies can never return null in the case of ExecutionSuccess.
        private Uri GetProxyAuto(Uri destination, out AutoWebProxyState autoWebProxyState) {
            GlobalLog.Print("WebProxy#" + ValidationHelper.HashString(this) + "::GetProxyAuto() destination:" + ValidationHelper.ToString(destination));
            if (ScriptEngine == null) {
                autoWebProxyState = AutoWebProxyState.Uninitialized;
                return null;
            }
            StringCollection proxies = ScriptEngine.GetProxies(destination, true, out autoWebProxyState);
            if (autoWebProxyState!=AutoWebProxyState.ExecutionSuccess) {
                return null;
            }
            if (proxies.Count == 0)
            {
                // Null here means, no proxy available (incl. DIRECT), the request is prohibited.
                return null;
            }
            if (AreAllBypassed(proxies, true)) {
                return destination;
            }
            return ProxyUri(proxies[0]);
        }

        private bool IsBypassedAuto(Uri destination, out AutoWebProxyState autoWebProxyState) {
            GlobalLog.Print("WebProxy#" + ValidationHelper.HashString(this) + "::IsBypassedAuto() destination:" + ValidationHelper.ToString(destination));
            if (ScriptEngine == null) {
                autoWebProxyState = AutoWebProxyState.Uninitialized;
                return true;
            }
            StringCollection proxies = ScriptEngine.GetProxies(destination, true, out autoWebProxyState);
            if (autoWebProxyState!=AutoWebProxyState.ExecutionSuccess) {
                return true;
            }
            if (proxies.Count == 0)
            {
                return false;
            }
            return AreAllBypassed(proxies, true);
        }

        internal Uri[] GetProxiesAuto(Uri destination, out AutoWebProxyState autoWebProxyState, ref int syncStatus)
        {
            GlobalLog.Print("WebProxy#" + ValidationHelper.HashString(this) + "::GetProxiesAuto() destination:" + ValidationHelper.ToString(destination));
            if (ScriptEngine == null) {
                autoWebProxyState = AutoWebProxyState.Uninitialized;
                return null;
            }
            StringCollection proxies = ScriptEngine.GetProxies(destination, false, out autoWebProxyState, ref syncStatus);
            if (autoWebProxyState!=AutoWebProxyState.ExecutionSuccess) {
                return null;
            }
            if (proxies.Count == 0)
            {
                return new Uri[] { };
            }
            if (AreAllBypassed(proxies, false))
            {
                return new Uri[] { null };
            }
            Uri[] proxyUris = new Uri[proxies.Count];
            for (int i=0; i<proxies.Count; i++) {
                proxyUris[i] = ProxyUri(proxies[i]);
            }
            return proxyUris;
        }

        internal void AbortGetProxiesAuto(ref int syncStatus)
        {
            if (ScriptEngine != null)
            {
                ScriptEngine.Abort(ref syncStatus);
            }
        }

        internal Uri GetProxyAutoFailover(Uri destination)
        {
            if (IsBypassedManual(destination))
            {
                return null;
            }

            Uri proxy = _ProxyAddress;
            Hashtable proxyHostAddresses = _ProxyHostAddresses;
            if (proxyHostAddresses != null)
            {
                proxy = proxyHostAddresses[destination.Scheme] as Uri;
            }
            return proxy;
        }

        private static bool AreAllBypassed(StringCollection proxies, bool checkFirstOnly) {
            bool isBypassed = true;
            for (int i=0; isBypassed && i<proxies.Count; i++) {
                isBypassed = ValidationHelper.IsBlankString(proxies[i]);
                if (checkFirstOnly) {
                    break;
                }
            }
            return isBypassed;
        }

        private static Uri ProxyUri(string proxyName) {
            return proxyName==null || proxyName.Length==0 ? null : new Uri("http://" + proxyName);
        }
    }
}
