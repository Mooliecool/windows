//------------------------------------------------------------------------------
// <copyright file="HttpWebRequest.cs" company="Microsoft">
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
    using System.Collections.Specialized;
    using System.Globalization;
    using System.IO;
    using System.IO.Compression;
    using System.Net.Cache;
    using System.Net.Configuration;
    using System.Runtime.Serialization;
    using System.Security;
    using System.Security.Cryptography.X509Certificates;
    using System.Security.Permissions;
    using System.Text;
    using System.Text.RegularExpressions;
    using System.Threading;
    using System.Net.Security;
    using System.Reflection;
    using System.ComponentModel;

    /// <devdoc>
    /// <para>
    /// <see cref='System.Net.HttpWebRequest'/> is an HTTP-specific implementation of the <see cref='System.Net.WebRequest'/> class.
    ///
    ///  Performs the major body of HTTP request processing. Handles
    ///    everything between issuing the HTTP header request to parsing the
    ///    the HTTP response.  At that point, we hand off the request to the response
    ///    object, where the programmer can query for headers or continue reading, usw.
    ///  </para>
    /// </devdoc>


    [Flags]
    public enum DecompressionMethods{
        None = 0,
        GZip = 1,
        Deflate = 2
    }


    [Serializable]
    public class HttpWebRequest : WebRequest, ISerializable {

        //these could have race conditions
        bool m_Saw100Continue;
        bool m_KeepAlive = true;
        bool m_LockConnection;
        bool m_NtlmKeepAlive;
        bool m_PreAuthenticate;
        DecompressionMethods m_AutomaticDecompression;

        private static class AbortState {
            public const int Public     = 1;
            public const int Internal = 2;
        }
        // interlocked
        private int m_Aborted;

        //
        // This is set on failure of the request before reattempting a retransmit,
        // we do this on write/read failures so that we can recover from certain failures
        // that may be caused by intermittent server/network failures.
        // Returns: true, if we have seen some kind of failure.
        //
        // set to true if this request failed during a connect

        bool m_OnceFailed;

        bool m_Pipelined = true;
        bool m_Retry = true;
        bool m_HeadersCompleted;
        bool m_IsCurrentAuthenticationStateProxy;
        bool m_SawInitialResponse;
        bool m_BodyStarted;
        bool m_RequestSubmitted;
        bool m_OriginallyBuffered;
        bool m_Extra401Retry;

        //these should be safe.
        [Flags]
        private enum Booleans : uint {
            AllowAutoRedirect                       = 0x00000001,
            AllowWriteStreamBuffering               = 0x00000002,
            ExpectContinue                          = 0x00000004,

            ProxySet                                = 0x00000010,

            UnsafeAuthenticatedConnectionSharing    = 0x00000040,
            IsVersionHttp10                         = 0x00000080,
            SendChunked                             = 0x00000100,
            EnableDecompression                     = 0x00000200,
            IsTunnelRequest                         = 0x00000400,
            Default = AllowAutoRedirect | AllowWriteStreamBuffering | ExpectContinue
        }


        internal const HttpStatusCode MaxOkStatus = (HttpStatusCode)299;
        private const HttpStatusCode MaxRedirectionStatus = (HttpStatusCode)399;
        private const int RequestLineConstantSize = 12;
        private const string ContinueHeader = "100-continue";
        internal const string ChunkedHeader = "chunked";
        internal const string GZipHeader    = "gzip";
        internal const string DeflateHeader    = "deflate";


        // default delay on the Stream.Read and Stream.Write operations
        private const int DefaultReadWriteTimeout = 5 * 60 * 1000; // 5 minutes

        // time delay that we would wait for continue
        internal const int DefaultContinueTimeout = 350; // ms

        // Size of '  HTTP/x.x\r\n'
        // which are the
        // unchanging pieces of the
        // request line.
        private static readonly byte[]  HttpBytes = new byte[]{(byte)'H', (byte)'T', (byte)'T', (byte)'P', (byte)'/'};

        // Statics used in the 100 Continue timeout mechanism.
        private static readonly WaitCallback s_EndWriteHeaders_Part2Callback = new WaitCallback(EndWriteHeaders_Part2Wrapper);
        private static readonly TimerThread.Callback s_ContinueTimeoutCallback = new TimerThread.Callback(ContinueTimeoutCallback);
        private static readonly TimerThread.Queue s_ContinueTimerQueue = TimerThread.GetOrCreateQueue(DefaultContinueTimeout);
        private static readonly TimerThread.Callback s_TimeoutCallback = new TimerThread.Callback(TimeoutCallback);
        private static readonly WaitCallback s_AbortWrapper = new WaitCallback(AbortWrapper);

        private static int s_UniqueGroupId;

        private Booleans                _Booleans = Booleans.Default;

        private DateTime                _CachedIfModifedSince = DateTime.MinValue;

        // Used in the 100 Continue timeout mechanism.
        private TimerThread.Timer       m_ContinueTimer;
        private InterlockedGate         m_ContinueGate;

        // Holds a WriteStream result to be processed by GetResponse.
        private object                  m_PendingReturnResult;

        // Read and Write async results - corspond to BeginGetResponse(read), and BeginGetRequestStream(write)
        private LazyAsyncResult         _WriteAResult;
        private LazyAsyncResult         _ReadAResult;

        // Used by our Connection to block the Request on getting a Connection
        private LazyAsyncResult         _ConnectionAResult;
        // Used by our Connection to block on being able to Read from our Connection
        private LazyAsyncResult         _ConnectionReaderAResult;

        // Once set, the Request either works Async or Sync internally
        private TriState                _RequestIsAsync;

        // Delegate that can be called on Continue Response
        private HttpContinueDelegate    _ContinueDelegate;

        // Link back to the server point used for this request.
        internal ServicePoint           _ServicePoint;

        // this is generated by SetResponse
        internal HttpWebResponse        _HttpResponse;


        // set by Connection code upon completion (can be either CoreResponseData or an Exception)
        private object                  _CoreResponse;
        private int                     _NestedWriteSideCheck; //To keep track nested responses for Sync case

        // request values
        private KnownHttpVerb           _Verb;
        // the actual verb set by caller or default
        private KnownHttpVerb           _OriginVerb;

        // our HTTP header response, request, parsing and storage objects
        private WebHeaderCollection     _HttpRequestHeaders;

        // send buffer for output request with headers.
        private byte[]                  _WriteBuffer;

        // Property to set whether writes can be handled
        private HttpWriteMode           _HttpWriteMode;

        // the host, port, and path
        private Uri                     _Uri;
        // the origin Uri host, port and path that never changes
        private Uri                     _OriginUri;

        // for which response ContentType we will look for and parse the CharacterSet
        private string                  _MediaType;

        // content length
        private long                    _ContentLength;

        // proxy that we are using...
        private IWebProxy               _Proxy;
        private ProxyChain              _ProxyChain;

        private string                  _ConnectionGroupName;
        private bool                    m_InternalConnectionGroup;

        private AuthenticationState     _ProxyAuthenticationState;
        private AuthenticationState     _ServerAuthenticationState;

        private ICredentials            _AuthInfo;
        private HttpAbortDelegate       _AbortDelegate;

        //
        // used to prevent Write Buffering,
        //  used otherwise for reposting POST, and PUTs in redirects
        //
        private ConnectStream           _SubmitWriteStream;
        private ConnectStream           _OldSubmitWriteStream;
        private int                     _MaximumAllowedRedirections;
        private int                     _AutoRedirects;

        //
        // generic version of _AutoRedirects above
        // used to count the number of requests made off this WebRequest
        //
        private int                     _RerequestCount;

        //
        // Timeout in milliseconds, if a synchronous request takes longer
        // than timeout, a WebException is thrown
        //
        private int                     _Timeout;

        //
        // Used to track relative time out across the use of the request
        //
        private TimerThread.Timer       _Timer;

        //
        // Timer factory, tied to the _Timeout time.
        //
        private TimerThread.Queue       _TimerQueue;

        private int                     _RequestContinueCount;

        //
        // Timeout for Read & Write on the Stream that we return through
        //  GetResponse().GetResponseStream() && GetRequestStream()
        //
        private int                     _ReadWriteTimeout;

        private CookieContainer         _CookieContainer;

        private int                     _MaximumResponseHeadersLength;

        private UnlockConnectionDelegate _UnlockDelegate;



        // size of post data, that needs to be greater, before we wait for a continue response
        // private static int  DefaultRequireWaitForContinueSize = 2048; // bytes



        //
        // Properties
        //

        internal TimerThread.Timer RequestTimer
        {
            get
            {
                return _Timer;
            }
        }

        internal bool Aborted {
            get {
                return m_Aborted != 0;
            }
        }


        /// <devdoc>
        ///    <para>
        ///       Enables or disables automatically following redirection responses.
        ///    </para>
        /// </devdoc>
        public bool AllowAutoRedirect {
            get {
                return (_Booleans&Booleans.AllowAutoRedirect)!=0;
            }
            set {
                if (value) {
                    _Booleans |= Booleans.AllowAutoRedirect;
                }
                else {
                    _Booleans &= ~Booleans.AllowAutoRedirect;
                }
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Enables or disables buffering the data stream sent to the server.
        ///    </para>
        /// </devdoc>
        public bool AllowWriteStreamBuffering {
            get {
                return (_Booleans&Booleans.AllowWriteStreamBuffering)!=0;
            }
            set {
                if (value) {
                    _Booleans |= Booleans.AllowWriteStreamBuffering;
                }
                else {
                    _Booleans &= ~Booleans.AllowWriteStreamBuffering;
                }
            }
        }

        private bool ExpectContinue {
            get {
                return (_Booleans&Booleans.ExpectContinue)!=0;
            }
            set {
                if (value) {
                    _Booleans |= Booleans.ExpectContinue;
                }
                else {
                    _Booleans &= ~Booleans.ExpectContinue;
                }
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Returns <see langword='true'/> if a response has been received from the
        ///       server.
        ///    </para>
        /// </devdoc>
        public bool HaveResponse {
            get {
                return _ReadAResult != null && _ReadAResult.InternalPeekCompleted;
            }
        }

        // this overrides the public KeepAlive setting.
        // we use this override for NTLM only
        internal bool NtlmKeepAlive {
            get {
                return m_NtlmKeepAlive;
            }
            set {
                m_NtlmKeepAlive = value;
            }
        }



        internal bool SawInitialResponse {
            get {
                GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SawInitialResponse_get() :" + m_SawInitialResponse);
                return m_SawInitialResponse;
            }
            set {
                GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SawInitialResponse_set() :" + value);
                m_SawInitialResponse = value;
            }
        }

        internal bool BodyStarted {
            get {
                return m_BodyStarted;
            }
        }

        /*
            Accessor:   KeepAlive

            To the app, this means "I want to use a persistent connection if
            available" if set to true, or "I don't want to use a persistent
            connection", if set to false.

            This accessor allows the application simply to register its
            desires so far as persistence is concerned. We will act on this
            and the pipelined requirement (below) at the point that we come
            to choose or create a connection on the application's behalf

            Read:       returns the sense of the keep-alive request switch

            Write:      set the sense of the keep-alive request switch
        */

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the value of the Keep-Alive header.
        ///    </para>
        /// </devdoc>
        public bool KeepAlive {
            get {
                return m_KeepAlive;
            }
            set {
                m_KeepAlive = value;
            }
        }

        //
        // LockConnection - set to true when the
        //  request needs exclusive access to the Connection
        //
        internal bool LockConnection {
            get {
                return m_LockConnection;
            }
            set {
                m_LockConnection = value;
            }
        }


        /*
            Accessor:   Pipelined

            To the app, this means "I want to use pipelining if available" if
            set to true, or "I don't want to use pipelining", if set to false.
            We could infer the state of the keep-alive flag from this setting
            too, but we will decide that only at connection-initiation time.
            If the application sets pipelining but resets keep-alive then we
            will generate a non-pipelined, non-keep-alive request

            Read:       returns the sense of the pipelined request switch

            Write:      sets the sense of the pipelined request switch
        */

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the value of Pipelined property.
        ///    </para>
        /// </devdoc>
        public bool Pipelined {
            get {
                return m_Pipelined;
            }
            set {
                m_Pipelined = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Enables or disables pre-authentication.
        ///    </para>
        /// </devdoc>
        public override bool PreAuthenticate {
            get {
                return m_PreAuthenticate;
            }
            set {
                m_PreAuthenticate = value;
            }
        }

        private bool ProxySet {
            get {
                return (_Booleans&Booleans.ProxySet)!=0;
            }
            set {
                if (value) {
                    _Booleans |= Booleans.ProxySet;
                }
                else {
                    _Booleans &= ~Booleans.ProxySet;
                }
            }
        }

        private bool RequestSubmitted {
            get {
                return m_RequestSubmitted;
            }
        }

        // Call under lock.
        private bool SetRequestSubmitted()
        {
            bool ret = RequestSubmitted;
            m_RequestSubmitted = true;
            GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetRequestSubmitted() returning:" + ret.ToString());
            return ret;
        }


        //
        // if the 100 Continue comes in around 350ms there might be race conditions
        // that make us set Understands100Continue to true when parsing the 100 response
        // in the Connection object and later make us set it to false if the
        // thread that is waiting comes in later than 350ms. to solve this we save info
        // on wether we did actually see the 100 continue. In that case, even if there's a
        // timeout, we won't set it to false.
        //
        internal bool Saw100Continue {
            get {
                return m_Saw100Continue;
            }
            set {
                m_Saw100Continue = value;
            }
        }

        /// <devdoc>
        ///    <para>Allows hi-speed NTLM connection sharing with keep-alive</para>
        /// </devdoc>
        public bool UnsafeAuthenticatedConnectionSharing {
            get {
                return (_Booleans&Booleans.UnsafeAuthenticatedConnectionSharing)!=0;
            }
            set {
                ExceptionHelper.WebPermissionUnrestricted.Demand();
                if (value) {
                    _Booleans |= Booleans.UnsafeAuthenticatedConnectionSharing;
                }
                else {
                    _Booleans &= ~Booleans.UnsafeAuthenticatedConnectionSharing;
                }
            }
        }
        //
        // When authenticating TO THE proxy we create a shared connection
        //
        internal  bool UnsafeOrProxyAuthenticatedConnectionSharing {
            get {
                return m_IsCurrentAuthenticationStateProxy || UnsafeAuthenticatedConnectionSharing;
            }
        }


        // HTTP version of the request
        private bool IsVersionHttp10 {
            get {
                return (_Booleans&Booleans.IsVersionHttp10)!=0;
            }
            set {
                if (value) {
                    _Booleans |= Booleans.IsVersionHttp10;
                }
                else {
                    _Booleans &= ~Booleans.IsVersionHttp10;
                }
            }
        }

        //
        // SendChunked - set/gets the state of chunk transfer send mode,
        //  if true, we will attempt to upload/write bits using chunked property
        //

        /// <devdoc>
        ///    <para>
        ///       Enable and disable sending chunked data to the server.
        ///    </para>
        /// </devdoc>
        public bool SendChunked {
            get {
                return (_Booleans&Booleans.SendChunked)!=0;
            }
            set {
                if (RequestSubmitted) {
                    throw new InvalidOperationException(SR.GetString(SR.net_writestarted));
                }
                if (value) {
                    _Booleans |= Booleans.SendChunked;
                }
                else {
                    _Booleans &= ~Booleans.SendChunked;
                }
            }
        }

        public DecompressionMethods AutomaticDecompression {
            get {
                return m_AutomaticDecompression;
            }
            set {
                if (RequestSubmitted) {
                    throw new InvalidOperationException(SR.GetString(SR.net_writestarted));
                }
                m_AutomaticDecompression = value;
            }
        }


        /*
        //                           
        internal DecompressionMethods DecompressionMethod{
            get{
                DecompressionMethods method = DecompressionMethods.None;

                string acceptEncoding = Headers[HttpKnownHeaderNames.AcceptEncoding];
                if (acceptEncoding != null){
                    if((AutomaticDecompression & DecompressionMethods.GZip) != 0){
                        method = DecompressionMethods.GZip;
                    }
                    //was it set directly?
                    else if(acceptEncoding.IndexOf(HttpWebRequest.GZipHeader) != -1) {
                        method=DecompressionMethods.GZip;
                    }
                    if (acceptEncoding.IndexOf(HttpWebRequest.DeflateHeader) != -1){
                        method |= DecompressionMethods.Deflate;
                    }
                }
                return method;
            }
         }
         */


        // This property holds our actual behaviour.
        // In some cases we might have to disable chunking even if the user has requested it.
        // In this case SendChunked returns true, but HttpWriteMode is != HttpWriteMode.Chunked
        internal HttpWriteMode HttpWriteMode {
            get {
                return _HttpWriteMode;
            }
            set {
                _HttpWriteMode = value;
            }
        }


        internal string AuthHeader(HttpResponseHeader header) {
            if (_HttpResponse==null) {
                return null;
            }
                //            currently if'd out in HttpWebRequest.cs
            return _HttpResponse.Headers[(int)header];
        }

        // This is a shortcut that would set the default policy for HTTP/HTTPS.
        // The default policy is overridden by any prefix-registered policy.
        // Will demand permission for set{}
        public static new RequestCachePolicy DefaultCachePolicy {
            get {
#if DEBUG
                using (GlobalLog.SetThreadKind(ThreadKinds.User | ThreadKinds.Async)) {
#endif
                RequestCachePolicy policy = RequestCacheManager.GetBinding(Uri.UriSchemeHttp).Policy;
                if (policy == null)
                    return WebRequest.DefaultCachePolicy;
                return policy;
#if DEBUG
                }
#endif
            }
            set {
#if DEBUG
                using (GlobalLog.SetThreadKind(ThreadKinds.User | ThreadKinds.Async)) {
#endif
                // This is a replacement of RequestCachePermission demand since we are not including the latest in the product.
                ExceptionHelper.WebPermissionUnrestricted.Demand();

                RequestCacheBinding binding = RequestCacheManager.GetBinding(Uri.UriSchemeHttp);
                RequestCacheManager.SetBinding(Uri.UriSchemeHttp, new RequestCacheBinding(binding.Cache, binding.Validator, value));
#if DEBUG
                }
#endif
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the default for the MaximumResponseHeadersLength property.
        ///    </para>
        ///    <remarks>
        ///       This value can be set in the config file, the default can be overridden using the MaximumResponseHeadersLength property.
        ///    </remarks>
        /// </devdoc>
        public static int DefaultMaximumResponseHeadersLength {
            get {
#if DEBUG
                using (GlobalLog.SetThreadKind(ThreadKinds.User | ThreadKinds.Async)) {
#endif
                return SettingsSectionInternal.Section.MaximumResponseHeadersLength;
#if DEBUG
                }
#endif
            }
            set {
#if DEBUG
                using (GlobalLog.SetThreadKind(ThreadKinds.User | ThreadKinds.Async)) {
#endif
                ExceptionHelper.WebPermissionUnrestricted.Demand();
                if (value<0 && value!=-1) {
                    throw new ArgumentOutOfRangeException(SR.GetString(SR.net_toosmall));
                }
                SettingsSectionInternal.Section.MaximumResponseHeadersLength = value;
#if DEBUG
                }
#endif
            }
        }

        public static int DefaultMaximumErrorResponseLength {
            get {
                return SettingsSectionInternal.Section.MaximumErrorResponseLength;
            }
            set {
                ExceptionHelper.WebPermissionUnrestricted.Demand();
                if (value<0 && value!=-1) {
                    throw new ArgumentOutOfRangeException(SR.GetString(SR.net_toosmall));
                }
                SettingsSectionInternal.Section.MaximumErrorResponseLength = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the maximum allowed length of the response headers.
        ///    </para>
        ///    <remarks>
        ///       The length is measured in kilobytes (1024 bytes) and it includes the response status line and the response
        ///       headers as well as all extra control characters received as part of the HTTP protocol. A value of -1 means
        ///       no such limit will be imposed on the response headers, a value of 0 means that all requests will fail.
        ///    </remarks>
        /// </devdoc>
        public int MaximumResponseHeadersLength {
            get {
                return _MaximumResponseHeadersLength;
            }
            set {
                if (RequestSubmitted) {
                    throw new InvalidOperationException(SR.GetString(SR.net_reqsubmitted));
                }
                if (value<0 && value!=-1) {
                    throw new ArgumentOutOfRangeException(SR.GetString(SR.net_toosmall));
                }
                _MaximumResponseHeadersLength = value;
            }
        }

        //
        // AbortDelegate - set by ConnectionGroup when
        //  the request is blocked waiting for a Connection
        //
        internal HttpAbortDelegate AbortDelegate {
            set {
                _AbortDelegate = value;
            }
        }

        // This is invoked by the Connection whenever we get access to Write to the Connection
        internal LazyAsyncResult ConnectionAsyncResult {
            get {
                return _ConnectionAResult;
            }
        }

        // This is invoked by the Connection whenever we get to Read from the Connection
        internal LazyAsyncResult ConnectionReaderAsyncResult {
            get {
                return _ConnectionReaderAResult;
            }
        }

        private bool UserRetrievedWriteStream {
            get {
                return _WriteAResult != null && _WriteAResult.InternalPeekCompleted;
            }
        }

        // True if we're being used in an async manner
        internal bool Async {
            get {
                // So Long as we've not been set to false, we operate as an async request
                return _RequestIsAsync!=TriState.False;
            }
            set {
                if (_RequestIsAsync == TriState.Unspecified) {
                    _RequestIsAsync = value ? TriState.True : TriState.False;
                }
            }
        }

        //
        // UnlockConnectionDelegate - set by the Connection
        //  iff the Request is asking for exclusive access (ie LockConnection == true)
        //  in this case UnlockConnectionDelegate must be called when the Request
        //  has finished authentication.
        //
        internal UnlockConnectionDelegate UnlockConnectionDelegate {
            get {
                return _UnlockDelegate;
            }
            set {
                _UnlockDelegate = value;
            }
        }

        private bool UsesProxy {
            get {
                return ServicePoint.InternalProxyServicePoint;
            }
        }

        internal HttpStatusCode ResponseStatusCode {
            get {
                return _HttpResponse.StatusCode;
            }
        }

        internal bool UsesProxySemantics {
            get {
                return ServicePoint.InternalProxyServicePoint && (((object)_Uri.Scheme != (object)Uri.UriSchemeHttps) || (IsTunnelRequest));
            }
        }

        internal Uri ChallengedUri {
            get {
                return CurrentAuthenticationState.ChallengedUri;
            }
        }

        internal AuthenticationState ProxyAuthenticationState {
            get {
                if (_ProxyAuthenticationState==null) {
                    _ProxyAuthenticationState = new AuthenticationState(true);
                }
                return _ProxyAuthenticationState;
            }
        }

        internal AuthenticationState ServerAuthenticationState {
            get {
                if (_ServerAuthenticationState==null) {
                    _ServerAuthenticationState = new AuthenticationState(false);
                }
                return _ServerAuthenticationState;
            }
            set {
                _ServerAuthenticationState = value;
            }
        }

        // the AuthenticationState we're using for authentication (proxy/server)
        // used to match entries in the Hashtable in NtlmClient & NegotiateClient
        internal AuthenticationState CurrentAuthenticationState {
            get {
                return m_IsCurrentAuthenticationStateProxy ? _ProxyAuthenticationState : _ServerAuthenticationState;
            }
            set {
                m_IsCurrentAuthenticationStateProxy = (object)_ProxyAuthenticationState==(object)value;
            }
        }


        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public CookieContainer CookieContainer {
            get {
                return _CookieContainer;
            }
            set {
                _CookieContainer = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets the original Uri of the request.
        ///       This read-only propery returns the Uri for this request. The
        ///       Uri object was created by the constructor and is always non-null.
        ///
        ///       Note that it will always be the base Uri, and any redirects,
        ///       or such will not be indicated.
        ///    </para>
        /// </devdoc>
        public override Uri RequestUri {                                   // read-only
            get {
                return _OriginUri;
            }
        }

        /*
            Accessor:   ContentLength

            The property that controls the Content-Length of the request entity
            body. Getting this property returns the last value set, or -1 if
            no value has been set. Setting it sets the content length, and
            the application must write that much data to the stream.
            Setting this property has a side effect on InternalSendChunked, it sets it to false.

            Input:
                Content length

            Returns: The value of the content length on get.

        */

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the Content-Length header of the request.
        ///    </para>
        /// </devdoc>
        public override long ContentLength {
            get {
                return _ContentLength;
            }
            set {
                if (RequestSubmitted) {
                    throw new InvalidOperationException(SR.GetString(SR.net_writestarted));
                }
                if (value < 0) {
                    throw new ArgumentOutOfRangeException(SR.GetString(SR.net_clsmall));
                }
                GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::ContentLength_set() was:" + _ContentLength + " now:" + value);
                _ContentLength = value;
            }
        }

        /// <devdoc>
        ///    <para>Timeout is set to 100 seconds by default</para>
        /// </devdoc>
        public override int Timeout {
            get {
                return _Timeout;
            }
            set {
                if (value<0 && value!=System.Threading.Timeout.Infinite) {
                    throw new ArgumentOutOfRangeException(SR.GetString(SR.net_io_timeout_use_ge_zero));
                }
                if (_Timeout != value)
                {
                    _Timeout = value;
                    _TimerQueue = null;
                }
            }
        }

        private TimerThread.Queue TimerQueue
        {
            get
            {
                TimerThread.Queue queue = _TimerQueue;
                if (queue == null)
                {
                    queue = TimerThread.GetOrCreateQueue(_Timeout == 0 ? 1 : _Timeout);
                    _TimerQueue = queue;
                }
                return queue;
            }
        }

        /// <devdoc>
        ///    <para>Used to control the Timeout when calling Stream.Read (AND) Stream.Write.
        ///         Effects Streams returned from GetResponse().GetResponseStream() (AND) GetRequestStream().
        ///         Default is 5 mins.
        ///    </para>
        /// </devdoc>
        public int ReadWriteTimeout {
            get {
                return _ReadWriteTimeout;
            }
            set {
                if (RequestSubmitted) {
                    throw new InvalidOperationException(SR.GetString(SR.net_reqsubmitted));
                }
                if (value<=0 && value!=System.Threading.Timeout.Infinite) {
                    throw new ArgumentOutOfRangeException(SR.GetString(SR.net_io_timeout_use_gt_zero));
                }
                _ReadWriteTimeout = value;
            }
        }

        
        internal long SwitchToContentLength() {
            //prevents clearing of transferencoding if we can't resubmit.
            if(HaveResponse)
            {
                return -1;
            }


            //we should only switch to content length if the server is unknown of less than IIS6.0
            //we only do this if it was orginially chunked.  If it was bufferonly, we should allow the switch
            if( HttpWriteMode == HttpWriteMode.Chunked){
                ConnectStream stream = _OldSubmitWriteStream;
                if(stream == null){
                    stream = _SubmitWriteStream;
                }
                if(stream.Connection != null && stream.Connection.IISVersion >= 6){
                    return -1;
                }
            }

            GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SwitchToContentLength() Switching to HttpWriteMode.ContentLength from:" + HttpWriteMode.ToString());

            long returnValue = -1;
            long oldContentLength = _ContentLength;

            if (HttpWriteMode != HttpWriteMode.None){
                
                if(HttpWriteMode == HttpWriteMode.Buffer){
                    _ContentLength = _SubmitWriteStream.BufferedData.Length;
                    m_OriginallyBuffered = true;
                    HttpWriteMode = HttpWriteMode.ContentLength;
                    return -1;
                }

                //1st ntlm leg w/ preauthenticated connection 
                // we don't have data yet
                if (NtlmKeepAlive && _OldSubmitWriteStream == null) {
                    _ContentLength = 0;
                    _SubmitWriteStream.SuppressWrite = true;

                    //if this was previously a bufferonly request, we won't set the contentlength
                    //back after header serialization
                    if(_SubmitWriteStream.BufferOnly != true)
                    {
                        returnValue = oldContentLength;
                    }

                    if(HttpWriteMode == HttpWriteMode.Chunked){
                        HttpWriteMode = HttpWriteMode.ContentLength;
                        _SubmitWriteStream.SwitchToContentLength();
                        //restore request settings after headers are serialized
                        returnValue = -2;                      
                        _HttpRequestHeaders.RemoveInternal(HttpKnownHeaderNames.TransferEncoding);
                    }
                }

                //this is a resubmit w/ data already available
                if(_OldSubmitWriteStream != null){
                    //first ntlm leg after noauth
                    if(NtlmKeepAlive){
                        _ContentLength = 0;
                    }
                    //2nd leg of ntlm, or resubmit for chunked on <IIS6.0
                    else if(_ContentLength == 0 || HttpWriteMode == HttpWriteMode.Chunked){
                        _ContentLength = _OldSubmitWriteStream.BufferedData.Length;
                    }

                    //Chunked rerequests should always be contentlength
                    if(HttpWriteMode == HttpWriteMode.Chunked){
                        HttpWriteMode = HttpWriteMode.ContentLength;
                        _SubmitWriteStream.SwitchToContentLength();
                        _HttpRequestHeaders.RemoveInternal(HttpKnownHeaderNames.TransferEncoding);
                    }
                }
            }
            GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SwitchToContentLength() ContentLength was:" + oldContentLength + " now:" + _ContentLength.ToString() + " returning:" + returnValue);
            return returnValue;
        }


        void PostSwitchToContentLength(long value){
            if(value > -1){
                _ContentLength = value;
            }
            if(value == -2){
                _ContentLength = -1;
                HttpWriteMode = HttpWriteMode.Chunked; 
            }
        }

        /// <devdoc>
        ///    <para>
        ///     Removes the ConnectionGroup and the Connection resources
        ///     held open usually do to NTLM operations.
        ///     </para>
        /// </devdoc>
        private void ClearAuthenticatedConnectionResources() {
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::ClearAuthenticatedConnectionResources()");

            if (ProxyAuthenticationState.UniqueGroupId != null || ServerAuthenticationState.UniqueGroupId != null)
            {
                GlobalLog.Assert(!UnsafeAuthenticatedConnectionSharing, "Created a unique connection AND UnsafeConnectionNtlmAuthentication is true.");
                // A unique connection was create so we must tear it down or it gets leaked
                ServicePoint.ReleaseConnectionGroup(GetConnectionGroupLine());
            }

            UnlockConnectionDelegate unlockConnectionDelegate = this.UnlockConnectionDelegate;
            try {
                if (unlockConnectionDelegate != null) {
                    unlockConnectionDelegate();
                }
                this.UnlockConnectionDelegate = null;
            }
            catch (Exception exception) {
                if (NclUtilities.IsFatal(exception)) throw;
            }
            catch {
            }

            ProxyAuthenticationState.ClearSession(this);
            ServerAuthenticationState.ClearSession(this);
        }

        //
        // Is set to true under the _connection_ lock to indicate that header are written
        // Is set to false when we start submitting
        //
        internal bool HeadersCompleted {
            get {
                return m_HeadersCompleted;
            }
            set {
                m_HeadersCompleted = value;
            }
        }

        // This method checks the consistency of the protocol usage.
        // It also might adjust the upload behaviour to comply with what the server supports.
        private void CheckProtocol(bool onRequestStream) {
            GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckProtocol(" + onRequestStream + ") HttpWriteMode:" + HttpWriteMode + " SendChunked:" + SendChunked + " ContentLength:" + ContentLength);
            if (!CanGetRequestStream) {
                if (onRequestStream) {
                    // prevent someone from getting a request stream, if the protocol verb/method doesn't support it
                    throw new ProtocolViolationException(SR.GetString(SR.net_nouploadonget));
                }
                else {
                    // prevent someone from setting ContentLength/Chunked, and then doing a Get
                    if (HttpWriteMode!=HttpWriteMode.Unknown && HttpWriteMode!=HttpWriteMode.None) {
                        throw new ProtocolViolationException(SR.GetString(SR.net_nocontentlengthonget));
                    }
                    // GET & HEAD requests will end up here
                    HttpWriteMode = HttpWriteMode.None;
                }
            }
            else {
                if (HttpWriteMode==HttpWriteMode.Unknown) {
                    if (SendChunked) {
                        // prevent someone from sending chunked to a HTTP/1.0 server
                        if (ServicePoint.HttpBehaviour==HttpBehaviour.HTTP11 || ServicePoint.HttpBehaviour==HttpBehaviour.Unknown) {
                            HttpWriteMode = HttpWriteMode.Chunked;
                        }
                        else {
                            if (AllowWriteStreamBuffering) {
                                // change this request to buffer instead of using chunking
                                HttpWriteMode = HttpWriteMode.Buffer;
                            }
                            else {
                                throw new ProtocolViolationException(SR.GetString(SR.net_nochunkuploadonhttp10));
                            }
                        }
                    }
                    else {
                        // we need to do some fixups if we don't have a HttpWriteMode
                        // unknown verbs (like TRACE) will also end up here. since we don't know if we need to send
                        // a content we'll assume we won't and upload will work only if ContentLength or Chunked
                        // were set. note that this means that we won't ever buffer for unknown verbs unless
                        // you call [Begin]GetRequestStream before [Begin]GetResponse.
                        HttpWriteMode = ContentLength>=0 ? HttpWriteMode.ContentLength : onRequestStream ? HttpWriteMode.Buffer : HttpWriteMode.None;
                    }
                }
            }
            if (HttpWriteMode!=HttpWriteMode.Chunked) {
                if (onRequestStream && ContentLength==-1 && !AllowWriteStreamBuffering && KeepAlive) {
                    // Missing Entity Body Delimiter:
                    // prevent someone from trying to send data without setting
                    // a ContentLength or InternalSendChunked when buffering is disabled and on a KeepAlive connection
                    throw new ProtocolViolationException(SR.GetString(SR.net_contentlengthmissing));
                }
                if (!ValidationHelper.IsBlankString(TransferEncoding)) {
                    // Transfer Encoding Without Chunked
                    // prevent someone from setting a Transfer Encoding without having InternalSendChunked==true
                    throw new InvalidOperationException(SR.GetString(SR.net_needchunked));
                }
            }
            GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckProtocol(" + onRequestStream + ") no error, returning null. HttpWriteMode:" + HttpWriteMode + " SendChunked:" + SendChunked + " ContentLength:" + ContentLength);
            return;
        }


        /// <devdoc>
        ///    <para>
        ///    Retreives the Request Stream from an HTTP Request uses an Async
        ///      operation to do this, and the result is retrived async.
        ///
        ///    Async operations include work in progess, this call is used to retrieve
        ///    results by pushing the async operations to async worker thread on the callback.
        ///    There are many open issues involved here including the handling of possible blocking
        ///    within the bounds of the async worker thread or the case of Write and Read stream
        ///    operations still blocking.
        ///   </para>
        /// </devdoc>
        [HostProtection(ExternalThreading=true)]
        public override IAsyncResult BeginGetRequestStream(AsyncCallback callback, object state) {
#if DEBUG
            using (GlobalLog.SetThreadKind(ThreadKinds.User | ThreadKinds.Async)) {
#endif
            GlobalLog.Enter("HttpWebRequest#" + ValidationHelper.HashString(this) + "::BeginGetRequestStream");
            if(Logging.On)Logging.Enter(Logging.Web, this, "BeginGetRequestStream", "");

            CheckProtocol(true);

            ContextAwareResult asyncResult = new ContextAwareResult(false, true, this, state, callback);


            lock (asyncResult.StartPostingAsyncOp())
            {
                // and have a result (weird but was supported in V1.X as repeated calls for the submit stream.
                if (_WriteAResult != null && _WriteAResult.InternalPeekCompleted)
                {
                    if (_WriteAResult.Result is Exception)
                    {
                        throw (Exception)_WriteAResult.Result;
                    }

                    try
                    {
                        asyncResult.InvokeCallback(_WriteAResult.Result);
                    }
                    catch (Exception e)
                    {
                        Abort(e, AbortState.Public);
                        throw;
                    }
                }
                else
                {
                    // prevent new requests when low on resources
                    if (!RequestSubmitted && NclUtilities.IsThreadPoolLow())
                    {
                        Exception exception = new InvalidOperationException(SR.GetString(SR.net_needmorethreads));
                        Abort(exception, AbortState.Public);
                        throw exception;
                    }

                    lock(this)
                    {
                        if (_WriteAResult != null)
                        {
                            throw new InvalidOperationException(SR.GetString(SR.net_repcall));
                        }

                        // See if we're already submitted a request (e.g. via GetResponse).
                        if (SetRequestSubmitted())
                        {
                            // Not completed write stream, this is an application error.
                            throw new InvalidOperationException(SR.GetString(SR.net_reqsubmitted));
                        }

                        // If there's already been a _ReadAResult completed, it better have been with an exception, like an abort.
                        // We need to check within this lock.  Before the lock, _WriteAResult didn't exist so won't have been notified.
                        // BeginSubmitRequest() will fail silently if we go ahead and call it after an abort.  Since we know this is the
                        // first call to any of the [Begin]GetRe... methods by the above checks, we know ProcessResponse can't have been
                        // called or any other valid _ReadAResult created yet.
                        if (_ReadAResult != null)
                        {
                            GlobalLog.Assert(_ReadAResult.InternalPeekCompleted, "HttpWebRequest#{0}::BeginGetRequestStream()|Incomplete _ReadAResult present on request.", ValidationHelper.HashString(this));
                            GlobalLog.Assert(_ReadAResult.Result is Exception, "HttpWebRequest#{0}::BeginGetRequestStream()|_ReadAResult with successful completion already present on request.", ValidationHelper.HashString(this));
                            throw (Exception) _ReadAResult.Result;
                        }

                        // get async going
                        _WriteAResult = asyncResult;
                        Async = true;
                    }

                    // OK, we haven't submitted the request yet, so do so now
                    // save off verb from origin Verb
                    GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::BeginGetRequestStream() resetting CurrentMethod to " + _OriginVerb);
                    CurrentMethod = _OriginVerb;
                    BeginSubmitRequest();
                }

                asyncResult.FinishPostingAsyncOp();
            }

            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::BeginGetRequestStream", ValidationHelper.HashString(asyncResult));
            if(Logging.On)Logging.Exit(Logging.Web, this, "BeginGetRequestStream", asyncResult);
            return asyncResult;
#if DEBUG
            }
#endif
        }

        /// <devdoc>
        ///  <para>Retreives the Request Stream after an Async operation has completed </para>
        /// </devdoc>
        public override Stream EndGetRequestStream(IAsyncResult asyncResult) {
#if DEBUG
            using (GlobalLog.SetThreadKind(ThreadKinds.User)) {
#endif
            GlobalLog.Enter("HttpWebRequest#" + ValidationHelper.HashString(this) + "::EndGetRequestStream", ValidationHelper.HashString(asyncResult));
            if(Logging.On)Logging.Enter(Logging.Web, this, "EndGetRequestStream", "");

            //
            // parameter validation
            //
            if (asyncResult == null) {
                throw new ArgumentNullException("asyncResult");
            }
            LazyAsyncResult castedAsyncResult = asyncResult as LazyAsyncResult;
            if (castedAsyncResult==null || castedAsyncResult.AsyncObject!=this) {
                throw new ArgumentException(SR.GetString(SR.net_io_invalidasyncresult), "asyncResult");
            }
            if (castedAsyncResult.EndCalled) {
                throw new InvalidOperationException(SR.GetString(SR.net_io_invalidendcall, "EndGetRequestStream"));
            }

            ConnectStream connectStream = castedAsyncResult.InternalWaitForCompletion() as ConnectStream;
            castedAsyncResult.EndCalled = true;

            if (connectStream == null)
            {
                if (Logging.On) Logging.Exception(Logging.Web, this, "EndGetRequestStream", castedAsyncResult.Result as Exception);
                throw (Exception) castedAsyncResult.Result;
            }

            // Otherwise it worked, so return the HttpWebResponse.
            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::EndGetRequestStream", ValidationHelper.HashString(connectStream));
            if(Logging.On)Logging.Exit(Logging.Web, this, "EndGetRequestStream", connectStream);
            return connectStream;
#if DEBUG
            }
#endif
        }

        /// <devdoc>
        /// <para>Gets a <see cref='System.IO.Stream'/> that the application can use to write request data.
        ///    This property returns a stream that the calling application can write on.
        ///    This property is not settable.  Getting this property may cause the
        ///    request to be sent, if it wasn't already. Getting this property after
        ///    a request has been sent that doesn't have an entity body causes an
        ///    exception to be thrown.
        ///</para>
        /// </devdoc>
        public override Stream GetRequestStream() {
#if DEBUG
            using (GlobalLog.SetThreadKind(ThreadKinds.User | ThreadKinds.Sync)) {
#endif
            GlobalLog.Enter("HttpWebRequest#" + ValidationHelper.HashString(this) + "::GetRequestStream");
            if(Logging.On)Logging.Enter(Logging.Web, this, "GetRequestStream", "");

            CheckProtocol(true);

            // See if we're already submitted a request and have a result cached.
            if (_WriteAResult == null || !_WriteAResult.InternalPeekCompleted)
            {
                lock(this)
                {
                    if (_WriteAResult != null)
                    {
                        throw new InvalidOperationException(SR.GetString(SR.net_repcall));
                    }

                    // See if we're already submitted a request (e.g. via GetResponse).
                    if (SetRequestSubmitted())
                    {
                        // Not completed write stream, this is an application error.
                        throw new InvalidOperationException(SR.GetString(SR.net_reqsubmitted));
                    }

                    // If there's already been a _ReadAResult completed, it better have been with an exception, like an abort.
                    // We need to check within this lock.  Before the lock, _WriteAResult didn't exist so won't have been notified.
                    // BeginSubmitRequest() will fail silently if we go ahead and call it after an abort.  Since we know this is the
                    // first call to any of the [Begin]GetRe... methods by the above checks, we know ProcessResponse can't have been
                    // called or any other valid _ReadAResult created yet.
                    if (_ReadAResult != null)
                    {
                        GlobalLog.Assert(_ReadAResult.InternalPeekCompleted, "HttpWebRequest#{0}::GetRequestStream()|Incomplete _ReadAResult present on request.", ValidationHelper.HashString(this));
                        GlobalLog.Assert(_ReadAResult.Result is Exception, "HttpWebRequest#{0}::GetRequestStream()|_ReadAResult with successful completion already present on request.", ValidationHelper.HashString(this));
                        throw (Exception) _ReadAResult.Result;
                    }

                    // use the AsyncResult to return our Stream
                    _WriteAResult = new LazyAsyncResult(this, null, null);
                    Async = false;
                }

                // OK, we haven't submitted the request yet, so do so now
                // save off verb from origin Verb
                GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "GetRequestStream() resetting CurrentMethod to " + _OriginVerb);
                CurrentMethod = _OriginVerb;

                // Submit the Request, causes us to queue ourselves to a Connection and may block
                // It has happened that Sync path uses this loop the Retry memeber for handling resubmissions.
                while (m_Retry && !_WriteAResult.InternalPeekCompleted) {
                    _OldSubmitWriteStream = null;
                    _SubmitWriteStream = null;
                    BeginSubmitRequest();
                }

                while(Aborted && !_WriteAResult.InternalPeekCompleted)
                {
                    // spin untill the _CoreResponse is set
                    if (!(_CoreResponse is Exception))
                        Thread.SpinWait(1);
                    else
                        CheckWriteSideResponseProcessing();
                }
            }

            ConnectStream connectStream = _WriteAResult.InternalWaitForCompletion() as ConnectStream;
            _WriteAResult.EndCalled = true;
            if (connectStream == null)
            {
                if (Logging.On) Logging.Exception(Logging.Web, this, "EndGetRequestStream", _WriteAResult.Result as Exception);
                throw (Exception) _WriteAResult.Result;
            }

            if(Logging.On)Logging.Exit(Logging.Web, this, "GetRequestStream", connectStream);
            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::GetRequestStream", ValidationHelper.HashString(connectStream));
            return connectStream;
#if DEBUG
            }
#endif
        }


        //
        // This read-only propery does a test against the object to verify that
        // we're not sending data with a GET or HEAD, these are dissallowed by the HTTP spec.
        // Returns: true if we allow sending data for this request, false otherwise
        //
        private bool CanGetRequestStream {
            get {
                bool result = !CurrentMethod.ContentBodyNotAllowed;
                GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CanGetRequestStream(" + _OriginVerb + "): " + result);
                return result;
            }
        }

        //
        // This read-only propery does a test against the object to verify if
        // we're allowed to get a Response Stream to read,
        // this is dissallowed per the HTTP spec for a HEAD request.
        // Returns: true if we allow sending data for this request, false otherwise
        //
        internal bool CanGetResponseStream {
            get {
                bool result = !CurrentMethod.ExpectNoContentResponse;
                GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CanGetResponseStream(" + CurrentMethod + "): " + result);
                return result;
            }
        }

        //
        // This read-only propery describes whether we can
        // send this verb without content data.
        // Assumes Method is already set.
        // Returns: true, if we must send some kind of content
        //
        internal bool RequireBody {
            get {
                bool result = CurrentMethod.RequireContentBody;
                GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::RequireBody(" + CurrentMethod + "): " + result);
                return result;
            }
        }

        /*++

            CheckBuffering - Determine if we need buffering based on having no contentlength


                consider the case in which we have no entity body delimiters:
                    RequireBody && ContentLength==-1 && !SendChunked == true

                now we need to consider 3 cases:

                AllowWriteStreamBuffering (1)
                    - buffer internally all the data written to the request stream when the user
                      closes the request stream send it on the wire with a ContentLength

                !AllowWriteStreamBuffering
                    - can't buffer internally all the data written to the request stream
                      so the data MUST go to the wire and the server sees it: 2 cases

                    !KeepAlive (2)
                        - send a "Connection: close" header to the server. the server SHOULD
                          1) send a final response
                          2) read all the data on the connection and treat it as being part of the
                             entity body of this request.

                    KeepAlive (3)
                        - throw, we can't do this, 'cause the server wouldn't know when the data is over.


            Input:
                    None.

            Returns:
                    true if we need buffering, false otherwise.

        --*/

        /*
        //                                    
        internal bool CheckBuffering {
            //
            // ContentLength is not set, and user is not chunking, buffering is on
            // so force the code into writing (give the user a stream to write to)
            // and we'll buffer for him
            //
            get {
                bool checkBuffering = RequireBody && ContentLength==-1 && HttpWriteMode!=HttpWriteMode.Chunked && AllowWriteStreamBuffering && ServicePoint.HttpBehaviour!=HttpBehaviour.HTTP11;
                GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckBuffering() returns:" + checkBuffering.ToString());
                return checkBuffering;
            }
        }
        */

        private bool HasEntityBody {
            get {
                return HttpWriteMode==HttpWriteMode.Chunked || HttpWriteMode==HttpWriteMode.Buffer || (HttpWriteMode==HttpWriteMode.ContentLength && ContentLength>0);
            }
        }


        // This is a notify from the connection ReadCallback about
        // - error response received and
        // - the KeepAlive status agreed by both sides
        internal void ErrorStatusCodeNotify(Connection connection, bool isKeepAlive, bool fatal) {
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::ErrorStatusCodeNotify");
            GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::ErrorStatusCodeNotify() Connection has reported Error Response Status" + (fatal ? " (fatal)" : ""));
            ConnectStream submitStream = _SubmitWriteStream;
            if (submitStream != null && submitStream.Connection == connection) {
                if (!fatal)
                {
                    submitStream.ErrorResponseNotify(isKeepAlive);
                }
                else
                {
                    if (!Aborted)
                    {
                        submitStream.FatalResponseNotify();
                    }
                }
            }
            else {
                GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::ErrorStatusCodeNotify() IGNORE connection is not used");
            }
        }

        /*
            Method: DoSubmitRequestProcessing

            Does internal processing of redirect and request retries for authentication
            Assumes that it cannot block, this returns a state var indicating when it
            needs to block

            Assumes that we are never called with a null response

            Input:
                none

            Returns:
                HttpProcessingResult -

        */

        //
        // ASSUMPTION: If this method throws the exception must be caught and handled
        // appropriatelly by the caller.
        //
        private HttpProcessingResult DoSubmitRequestProcessing(ref Exception exception)
        {
            GlobalLog.Enter("HttpWebRequest#" + ValidationHelper.HashString(this) + "::DoSubmitRequestProcessing");
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::ErrorStatusCodeNotify");
            HttpProcessingResult result = HttpProcessingResult.Continue;

            m_Retry = false;

            try
            {
                //
                // We have a response of some sort, see if we need to resubmit
                // it do to authentication, redirection or something
                // else, then handle clearing out state and draining out old response.
                //
                if (_HttpResponse != null) {
                    // give apps the chance to examine the headers of the new response
                    if (_CookieContainer != null) {
                        CookieModule.OnReceivedHeaders(this);
                    }

                    ProxyAuthenticationState.Update(this);
                    ServerAuthenticationState.Update(this);
                }

                bool resubmit = false;
                bool checkNextProxy = true;
                if (_HttpResponse == null)
                {
                    resubmit = true;
                }
                else if (CheckResubmitForCache(ref exception) || CheckResubmit(ref exception))
                {
                    resubmit = true;
                    checkNextProxy = false;
                }

                ServicePoint servicePoint = null;
                if (checkNextProxy)
                {
                    WebException webException = exception as WebException;
                    if (webException != null && webException.InternalStatus == WebExceptionInternalStatus.ServicePointFatal)
                    {
                        ProxyChain chain = _ProxyChain;
                        if (chain != null)
                        {
                            servicePoint = ServicePointManager.FindServicePoint(chain);
                        }
                        resubmit = servicePoint != null;
                    }
                }

                if (resubmit)
                {
                    GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::DoSubmitRequestProcessing() resubmiting this request.");

                    if (CacheProtocol != null && _HttpResponse != null)
                        CacheProtocol.Reset();

                    ClearRequestForResubmit();

                    // In these cases, we don't know whether to keep the credentials for the follow-up request or not.
                    // Different auth schemes have different requirements.  The typical case is to keep them - NTLM
                    // requires this path in order to work at all.  But Kerberos for example can give a 401 here.
                    // Set a flag saying that if we get a 401, start over without credentials.
                    WebException e = exception as WebException;
                    if(e != null){
                        if (e.Status == WebExceptionStatus.PipelineFailure || e.Status == WebExceptionStatus.KeepAliveFailure) {
                            m_Extra401Retry = true;
                        }
                    }

                    if (servicePoint == null)
                    {
                        servicePoint = FindServicePoint(true);
                    }
                    else
                    {
                        _ServicePoint = servicePoint;
                    }

                    if (Async) {
                        SubmitRequest(servicePoint);
                    }
                    else {
                        // under sync conditions, we let GetResponse() loop calling BeginSubmitRequest() until we're done
                        m_Retry = true;
                    }
                    result = HttpProcessingResult.WriteWait;
                }
            }
            finally
            {
                if (result == HttpProcessingResult.Continue)
                {
                    ClearAuthenticatedConnectionResources();
                }
            }

            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::DoSubmitRequestProcessing", result.ToString());
            return result;
        }



        /// <devdoc>
        ///    <para>Used to query for the Response of an HTTP Request using Async</para>
        /// </devdoc>
        [HostProtection(ExternalThreading=true)]
        public override IAsyncResult BeginGetResponse(AsyncCallback callback, object state) {
#if DEBUG
            using (GlobalLog.SetThreadKind(ThreadKinds.User | ThreadKinds.Async)) {
#endif
            GlobalLog.Enter("HttpWebRequest#" + ValidationHelper.HashString(this) + "::BeginGetResponse");
            if(Logging.On)Logging.Enter(Logging.Web, this, "BeginGetResponse", "");

            CheckProtocol(false);

            ConnectStream stream = _OldSubmitWriteStream != null ? _OldSubmitWriteStream : _SubmitWriteStream;

            // Close the request stream if the user forgot to do that and all data is written.
            if(stream != null && !stream.IsClosed && stream.BytesLeftToWrite == 0)
            {
                stream.Close();
            }

            ContextAwareResult asyncResult = new ContextAwareResult(false, true, this, state, callback);

            if (!RequestSubmitted && NclUtilities.IsThreadPoolLow())
            {
                // prevent new requests when low on resources
                Exception exception = new InvalidOperationException(SR.GetString(SR.net_needmorethreads));
                Abort(exception, AbortState.Public);
                throw exception;
            }

            // Need to lock the context until it's created (if necessary) in Returning().
            lock (asyncResult.StartPostingAsyncOp())
            {
                bool gotResponse = false;
                bool requestSubmitted;
                lock (this)
                {
                    requestSubmitted = SetRequestSubmitted();

                    if (HaveResponse)
                    {
                        gotResponse = true;
                    }
                    else
                    {
                        if (_ReadAResult != null)
                        {
                            throw new InvalidOperationException(SR.GetString(SR.net_repcall));
                        }

                        _ReadAResult = asyncResult;
                        Async = true;
                    }
                }

                // Must check this after setting _ReadAResult, which holds the context which may be used for permission checks etc.
                // See if we need to do the call-done processing here.
                CheckDeferredCallDone(stream);

                if (gotResponse)
                {
                    if (Logging.On) Logging.Exit(Logging.Web, this, "BeginGetResponse", _ReadAResult.Result);
                    GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::BeginGetResponse", "Already Completed, response = " + ValidationHelper.HashString(_ReadAResult.Result));
                    Exception e = _ReadAResult.Result as Exception;
                    if (e != null)
                    {
                        throw e;
                    }

                    try
                    {
                        asyncResult.InvokeCallback(_ReadAResult.Result);
                    }
                    catch (Exception exception)
                    {
                        Abort(exception, AbortState.Public);
                        throw;
                    }
                }
                else
                {
                    // If we're here it's because we don't have the response yet. We may have
                    // already submitted the request, but if not do so now.
                    if (!requestSubmitted)
                    {
                        // Save Off verb, and use it to make the request
                        GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + ": resetting CurrentMethod to " + _OriginVerb);
                        CurrentMethod = _OriginVerb;
                    }

                    // If we're here it's because we don't have the response yet. We may have
                    //  already submitted the request, but if not do so now.
                    if (_RerequestCount > 0 || !requestSubmitted) {
                        while (m_Retry) {
                            // Keep looping in case there are redirects, auth re-requests, etc
                            BeginSubmitRequest();
                        }
                    }
                }
                asyncResult.FinishPostingAsyncOp();
            }

            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::BeginGetResponse", ValidationHelper.HashString(asyncResult));
            if(Logging.On)Logging.Exit(Logging.Web, this, "BeginGetResponse", asyncResult);
            return asyncResult;
#if DEBUG
            }
#endif
        }

        /// <devdoc>
        ///  <para>Retreives the Response Result from an HTTP Result after an Async operation has completed</para>
        /// </devdoc>
        public override WebResponse EndGetResponse(IAsyncResult asyncResult) {
#if DEBUG
            using (GlobalLog.SetThreadKind(ThreadKinds.User)) {
#endif
            GlobalLog.Enter("HttpWebRequest#" + ValidationHelper.HashString(this) + "::EndGetResponse", ValidationHelper.HashString(asyncResult));
            if(Logging.On)Logging.Enter(Logging.Web, this, "EndGetResponse", "");

            //
            // parameter validation
            //
            if (asyncResult==null) {
                throw new ArgumentNullException("asyncResult");
            }

            LazyAsyncResult castedAsyncResult = asyncResult as LazyAsyncResult;
            if (castedAsyncResult==null || castedAsyncResult.AsyncObject!=this) {
                throw new ArgumentException(SR.GetString(SR.net_io_invalidasyncresult), "asyncResult");
            }
            if (castedAsyncResult.EndCalled) {
                throw new InvalidOperationException(SR.GetString(SR.net_io_invalidendcall, "EndGetResponse"));
            }

            HttpWebResponse httpWebResponse = castedAsyncResult.InternalWaitForCompletion() as HttpWebResponse;
            castedAsyncResult.EndCalled = true;

            if (httpWebResponse == null)
            {
                if (Logging.On) Logging.Exception(Logging.Web, this, "EndGetResponse", castedAsyncResult.Result as Exception);
                throw (Exception) castedAsyncResult.Result;
            }

            GlobalLog.Assert(httpWebResponse.ResponseStream != null, "HttpWebRequest#{0}::EndGetResponse()|httpWebResponse.ResponseStream == null", ValidationHelper.HashString(this));
            // Otherwise it worked, so return the HttpWebResponse.
            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::EndGetResponse", ValidationHelper.HashString(httpWebResponse));
            if(Logging.On)Logging.Exit(Logging.Web, this, "EndGetResponse", httpWebResponse);
            return httpWebResponse;
#if DEBUG
            }
#endif
        }

        private void CheckDeferredCallDone(ConnectStream stream)
        {
            object returnResult = Interlocked.Exchange(ref m_PendingReturnResult, DBNull.Value);
            if (returnResult == NclConstants.Sentinel)
            {
#if DEBUG
                if (!Async)
                {
                    using (GlobalLog.SetThreadKind(ThreadKinds.Sync)) {
                        EndSubmitRequest();
                    }
                }
                else
#endif
                EndSubmitRequest();
            }
            else if (returnResult != null && returnResult != DBNull.Value)
            {
                // It could still be Missing.Value, which indicates ProcessWriteCallDone() should be called with null.
                stream.ProcessWriteCallDone(returnResult as ConnectionReturnResult);
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Returns a response from a request to an Internet resource.
        ///    The response property. This property returns the WebResponse for this
        ///    request. This may require that a request be submitted first.
        ///
        ///     The idea is that we look and see if a request has already been
        ///    submitted. If one has, we'll just return the existing response
        ///    (if it's not null). If we haven't submitted a request yet, we'll
        ///    do so now, possible multiple times while we handle redirects
        ///    etc.
        ///    </para>
        /// </devdoc>
        public override WebResponse GetResponse() {
#if DEBUG
            using (GlobalLog.SetThreadKind(ThreadKinds.User | ThreadKinds.Sync)) {
#endif
            GlobalLog.Enter("HttpWebRequest#" + ValidationHelper.HashString(this) + "::GetResponse");
            if(Logging.On)Logging.Enter(Logging.Web, this, "GetResponse", "");

            CheckProtocol(false);

            // Many of these logics require GetResponse() to be called after all write-stream activity is done.  You can't call it
            // simultaneously on another thread and expect it to block until it can run.  Doing that can cause the request to
            // hang.

            ConnectStream stream = _OldSubmitWriteStream != null ? _OldSubmitWriteStream : _SubmitWriteStream;

            // Close the request stream if the user forgot to do that and all data is written.
            if (stream != null && !stream.IsClosed && stream.BytesLeftToWrite == 0)
                stream.Close();


            // return response, if the response is already set
            bool gotResponse = false;
            HttpWebResponse httpWebResponse = null;
            bool requestSubmitted;
            lock (this)
            {
                requestSubmitted = SetRequestSubmitted();
                if (HaveResponse)
                {
                    gotResponse = true;
                    httpWebResponse = _ReadAResult.Result as HttpWebResponse;
                }
                else
                {
                    if (_ReadAResult != null)
                    {
                        throw new InvalidOperationException(SR.GetString(SR.net_repcall));
                    }

                    Async = false;

                    // Since we don't really allow switching between sync and async, if the request is already async, this needs to
                    // capture context for use in the ongoing async operations as if it were BeginGetResponse().
                    if (Async)
                    {
                        ContextAwareResult readResult = new ContextAwareResult(false, true, this, null, null);
                        readResult.StartPostingAsyncOp(false);
                        readResult.FinishPostingAsyncOp();
                        _ReadAResult = readResult;
                    }
                    else
                    {
                        _ReadAResult = new LazyAsyncResult(this, null, null);
                    }
                }
            }

            // See if we need to do the call-done processing here.
            CheckDeferredCallDone(stream);

            if (!gotResponse)
            {
                //The previous call may have been async.  If we are now doing a sync call, we should
                //use the timeout
                if (_Timer == null){
                    _Timer = TimerQueue.CreateTimer(s_TimeoutCallback, this);
                }
                
                
                // Save Off verb, and use it to make the request
                if (!requestSubmitted) {
                    GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + ": resetting CurrentMethod to " + _OriginVerb);
                    CurrentMethod = _OriginVerb;
                }

                // If we're here it's because we don't have the response yet. We may have
                //  already submitted the request, but if not do so now.
                while (m_Retry) {
                    // Keep looping in case there are redirects, auth re-requests, etc
                    BeginSubmitRequest();
                }

                while(!Async && Aborted && !_ReadAResult.InternalPeekCompleted)
                {
                    // spin untill the _CoreResponse is set
                    if (!(_CoreResponse is Exception))
                        Thread.SpinWait(1);
                    else
                        CheckWriteSideResponseProcessing();
                }

                httpWebResponse = _ReadAResult.InternalWaitForCompletion() as HttpWebResponse;
                _ReadAResult.EndCalled = true;
            }

            if (httpWebResponse == null)
            {
                if (Logging.On) Logging.Exception(Logging.Web, this, "EndGetResponse", _ReadAResult.Result as Exception);
                throw (Exception) _ReadAResult.Result;
            }

            GlobalLog.Assert(httpWebResponse.ResponseStream != null, "HttpWebRequest#{0}::GetResponse()|httpWebResponse.ResponseStream == null", ValidationHelper.HashString(this));
            if(Logging.On)Logging.Exit(Logging.Web, this, "GetResponse", httpWebResponse);
            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::GetResponse", ValidationHelper.HashString(httpWebResponse));
            return httpWebResponse;
#if DEBUG
            }
#endif
        }

        internal void WriteCallDone(ConnectStream stream, ConnectionReturnResult returnResult)
        {
            GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::WriteCallDone()");

            // Make sure this is the user stream.
            if (!object.ReferenceEquals(stream, _OldSubmitWriteStream != null ? _OldSubmitWriteStream : _SubmitWriteStream))
            {
                GlobalLog.Assert(object.ReferenceEquals(stream, _SubmitWriteStream), "HttpWebRequest#{0}::CallDone|Called from invalid stream.", ValidationHelper.HashString(this));
                GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::WriteCallDone() - called for resubmit stream");
                stream.ProcessWriteCallDone(returnResult);
                return;
            }

            // If we're still writing headers in GetRequestStream, don't delay, or GetRequestStream will hang.
            if (!UserRetrievedWriteStream)
            {
                GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::WriteCallDone() - called during headers");
                stream.ProcessWriteCallDone(returnResult);
                return;
            }

            object pendResult = returnResult == null ? (object) Missing.Value : returnResult;
            object oldResult = Interlocked.CompareExchange(ref m_PendingReturnResult, pendResult, null);
            if (oldResult == DBNull.Value)
            {
                stream.ProcessWriteCallDone(returnResult);
            }
        }

        internal void NeedEndSubmitRequest()
        {
            GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::NeedEndSubmitRequest()");

            object oldResult = Interlocked.CompareExchange(ref m_PendingReturnResult, NclConstants.Sentinel, null);
            if (oldResult == DBNull.Value)
            {
                EndSubmitRequest();
            }
            else
            {
                GlobalLog.Assert(oldResult == null, "HttpWebRequest#{0}::NeedEndSubmitRequest()|Duplicate call.  typeof(oldResult):", ValidationHelper.HashString(this), oldResult != null ? oldResult.GetType().ToString() : "null");
            }
        }

        /*
            Accessor:   Address

            This is just a simple Uri that is returned, indicating the end result
            of the request, after any possible Redirects, etc, that may transpire
            during the request.  This was added to handle this case since RequestUri
            will not change from the moment this Request is created.

            Input:

            Returns: The Uri for this request..


        */
        /// <devdoc>
        ///    <para>
        ///       Gets the Uri that actually responded to the request.
        ///    </para>
        /// </devdoc>
        public Uri Address {
            get {
                return _Uri;
            }
        }


        /// <devdoc>
        ///    <para>Gets/Sets Deletegate used to signal us on Continue callback</para>
        /// </devdoc>
        public HttpContinueDelegate ContinueDelegate {
            get {
                return _ContinueDelegate;
            }
            set {
                _ContinueDelegate = value;
            }
        }

        internal void CallContinueDelegateCallback(object state)
        {
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::CallContinueDelegateCallback");
            CoreResponseData response = (CoreResponseData) state;
            ContinueDelegate((int) response.m_StatusCode, response.m_ResponseHeaders);
        }


        /// <devdoc>
        ///    <para>
        ///       Gets the service point used for this request.  Looks up the ServicePoint for given Uri,
        ///         one isn't already created and assigned to this HttpWebRequest.
        ///    </para>
        /// </devdoc>
        public ServicePoint ServicePoint {
            get {
#if DEBUG
                using (GlobalLog.SetThreadKind(ThreadKinds.User | ThreadKinds.Async)) {
#endif
                return FindServicePoint(false);
#if DEBUG
                }
#endif
            }
        }

        /// <devdoc>
        /// </devdoc>
        public int MaximumAutomaticRedirections {
            get {
                return _MaximumAllowedRedirections;
            }
            set {
                if (value <= 0) {
                    throw new ArgumentException(SR.GetString(SR.net_toosmall), "value");
                }
                _MaximumAllowedRedirections = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the request method.
        ///       This method represents the initial origin Verb, this is unchanged/uneffected by redirects
        ///    </para>
        /// </devdoc>
        public override string Method {
            get {
                return _OriginVerb.Name;
            }
            set {
                if (ValidationHelper.IsBlankString(value)) {
                    throw new ArgumentException(SR.GetString(SR.net_badmethod), "value");
                }

                if (ValidationHelper.IsInvalidHttpString(value)) {
                    throw new ArgumentException(SR.GetString(SR.net_badmethod), "value");
                }
                _OriginVerb = KnownHttpVerb.Parse(value);
            }
        }

        internal KnownHttpVerb CurrentMethod {
            get {
                return _Verb != null ? _Verb : _OriginVerb;
            }
            set{
                _Verb = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Provides authentication information for the request.
        ///    </para>
        /// </devdoc>
        public override ICredentials Credentials {
            get {
                return _AuthInfo;
            }
            set {
                _AuthInfo = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Allows us to use generic default credentials.
        ///    </para>
        /// </devdoc>
        public override bool UseDefaultCredentials {
            get {
                return (Credentials is SystemNetworkCredential) ? true : false;
            }
            set {
                if (RequestSubmitted) {
                    throw new InvalidOperationException(SR.GetString(SR.net_writestarted));
                }
                _AuthInfo = value ? CredentialCache.DefaultCredentials : null;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       True if we're tunneling SSL through a proxy
        ///    </para>
        /// </devdoc>
        internal bool IsTunnelRequest {
            get {
                return (_Booleans&Booleans.IsTunnelRequest)!=0;
            }
            set {
                if (value) {
                    _Booleans |= Booleans.IsTunnelRequest;
                }
                else {
                    _Booleans &= ~Booleans.IsTunnelRequest;
                }
            }
        }

        //
        // ConnectionGroupName - used to control which group
        //  of connections we use, by default should be null
        //
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public override string ConnectionGroupName {
            get {
                return _ConnectionGroupName;
            }
            set {
                _ConnectionGroupName = value;
            }
        }

        internal bool InternalConnectionGroup
        {
            /* Consider removing
            get
            {
                return m_InternalConnectionGroup;
            }
            */

            set
            {
                m_InternalConnectionGroup = value;
            }
        }


        /// <devdoc>
        ///    <para>
        ///       A collection of HTTP headers stored as name value pairs.
        ///    </para>
        /// </devdoc>
        public override WebHeaderCollection Headers {
            get {
                return _HttpRequestHeaders;
            }
            set {

                // we can't change headers after they've already been sent
                if ( RequestSubmitted ) {
                    throw new InvalidOperationException(SR.GetString(SR.net_reqsubmitted));
                }

                WebHeaderCollection webHeaders = value;
                WebHeaderCollection newWebHeaders = new WebHeaderCollection(WebHeaderCollectionType.HttpWebRequest);

                // Copy And Validate -
                // Handle the case where their object tries to change
                //  name, value pairs after they call set, so therefore,
                //  we need to clone their headers.
                //

                foreach (String headerName in webHeaders.AllKeys ) {
                    newWebHeaders.Add(headerName,webHeaders[headerName]);
                }

                _HttpRequestHeaders = newWebHeaders;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the proxy information for a request.
        ///    </para>
        /// </devdoc>
        public override IWebProxy Proxy {
            get {
#if DEBUG
                using (GlobalLog.SetThreadKind(ThreadKinds.User | ThreadKinds.Async)) {
#endif
                ExceptionHelper.WebPermissionUnrestricted.Demand();
                return _Proxy;
#if DEBUG
                }
#endif
            }
            set {
#if DEBUG
                using (GlobalLog.SetThreadKind(ThreadKinds.User | ThreadKinds.Async)) {
#endif
                ExceptionHelper.WebPermissionUnrestricted.Demand();
                // we can't change the proxy, while the request is already fired
                if ( RequestSubmitted ) {
                    throw new InvalidOperationException(SR.GetString(SR.net_reqsubmitted));
                }
                InternalProxy = value;
#if DEBUG
                }
#endif
            }
        }

        internal IWebProxy InternalProxy {
            get {
                return _Proxy;
            }
            set {
                ProxySet = true;
                _Proxy = value;
                if (_ProxyChain != null)
                {
                    _ProxyChain.Dispose();
                }
                _ProxyChain = null;

                // since the service point is based on our proxy, make sure,
                // that we reresolve it
                ServicePoint servicePoint = FindServicePoint(true);
            }
        }


        // HTTP Version
        /// <devdoc>
        ///    <para>
        ///       Gets and sets
        ///       the HTTP protocol version used in this request.
        ///    </para>
        /// </devdoc>
        public Version ProtocolVersion {
            get {
                return IsVersionHttp10 ? HttpVersion.Version10 : HttpVersion.Version11;
            }
            set {
                if (value.Equals(HttpVersion.Version11)) {
                    IsVersionHttp10 = false;
                }
                else if (value.Equals(HttpVersion.Version10)) {
                    IsVersionHttp10 = true;
                }
                else {
                    throw new ArgumentException(SR.GetString(SR.net_wrongversion), "value");
                }
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets and sets the value of the Content-Type header. Null clears it out.
        ///    </para>
        /// </devdoc>
        public override String ContentType {
            get {
                return _HttpRequestHeaders[HttpKnownHeaderNames.ContentType];
            }
            set {
                SetSpecialHeaders(HttpKnownHeaderNames.ContentType, value);
            }
        }

        /// <devdoc>
        ///    <para>Sets the media type header</para>
        /// </devdoc>
        public string MediaType {
            get {
                return _MediaType;
            }
            set {
                _MediaType = value;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the value of the Transfer-Encoding header. Setting null clears it out.
        ///    </para>
        /// </devdoc>
        public string TransferEncoding {
            get {
                return _HttpRequestHeaders[HttpKnownHeaderNames.TransferEncoding];
            }
            set {
#if DEBUG
                using (GlobalLog.SetThreadKind(ThreadKinds.User | ThreadKinds.Async)) {
#endif
                bool fChunked;
                //
                // on blank string, remove current header
                //
                if (ValidationHelper.IsBlankString(value)) {
                    //
                    // if the value is blank, then remove the header
                    //
                    _HttpRequestHeaders.RemoveInternal(HttpKnownHeaderNames.TransferEncoding);
                    return;
                }

                //
                // if not check if the user is trying to set chunked:
                //
                string newValue = value.ToLower(CultureInfo.InvariantCulture);
                fChunked = (newValue.IndexOf(ChunkedHeader) != -1);

                //
                // prevent them from adding chunked, or from adding an Encoding without
                //  turing on chunked, the reason is due to the HTTP Spec which prevents
                //  additional encoding types from being used without chunked
                //
                if (fChunked) {
                    throw new ArgumentException(SR.GetString(SR.net_nochunked), "value");
                }
                else if (!SendChunked) {
                    throw new InvalidOperationException(SR.GetString(SR.net_needchunked));
                }
                else {
                    _HttpRequestHeaders.CheckUpdate(HttpKnownHeaderNames.TransferEncoding, value);
                }
#if DEBUG
                }
#endif
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets and sets the value of the Connection header. Setting null clears the header out.
        ///    </para>
        /// </devdoc>
        public string Connection {
            get {
                return _HttpRequestHeaders[HttpKnownHeaderNames.Connection];
            }
            set {
#if DEBUG
                using (GlobalLog.SetThreadKind(ThreadKinds.User | ThreadKinds.Async)) {
#endif
                bool fKeepAlive;
                bool fClose;

                //
                // on blank string, remove current header
                //
                if (ValidationHelper.IsBlankString(value)) {
                    _HttpRequestHeaders.Remove(HttpKnownHeaderNames.Connection);
                    return;
                }

                string newValue = value.ToLower(CultureInfo.InvariantCulture);

                fKeepAlive = (newValue.IndexOf("keep-alive") != -1) ;
                fClose =  (newValue.IndexOf("close") != -1) ;

                //
                // Prevent keep-alive and close from being added
                //

                if (fKeepAlive ||
                    fClose) {
                    throw new ArgumentException(SR.GetString(SR.net_connarg), "value");
                }
                else {
                    _HttpRequestHeaders.CheckUpdate(HttpKnownHeaderNames.Connection, value);
                }
#if DEBUG
                }
#endif
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the value of the Accept header.
        ///    </para>
        /// </devdoc>
        public string Accept {
            get {
                return _HttpRequestHeaders[HttpKnownHeaderNames.Accept];
            }
            set {
                SetSpecialHeaders(HttpKnownHeaderNames.Accept, value);
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the value of the Referer header.
        ///    </para>
        /// </devdoc>
        public string Referer {
            get {
                return _HttpRequestHeaders[HttpKnownHeaderNames.Referer];
            }
            set {
                SetSpecialHeaders(HttpKnownHeaderNames.Referer, value);
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the value of the User-Agent header.
        ///    </para>
        /// </devdoc>
        public string UserAgent {
            get {
                return _HttpRequestHeaders[HttpKnownHeaderNames.UserAgent];
            }
            set {
                SetSpecialHeaders(HttpKnownHeaderNames.UserAgent, value);
            }
        }


        /*
            Accessor:   Expect

            The property that controls the Expect header

            Input:
                string Expect, null clears the Expect except for 100-continue value

            Returns: The value of the Expect on get.

        */

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the value of the Expect header.
        ///    </para>
        /// </devdoc>
        public string Expect {
            get {
                return _HttpRequestHeaders[HttpKnownHeaderNames.Expect];
            }
            set {
#if DEBUG
                using (GlobalLog.SetThreadKind(ThreadKinds.User | ThreadKinds.Async)) {
#endif
                // only remove everything other than 100-cont
                bool fContinue100;

                //
                // on blank string, remove current header
                //

                if (ValidationHelper.IsBlankString(value)) {
                    _HttpRequestHeaders.RemoveInternal(HttpKnownHeaderNames.Expect);
                    return;
                }

                //
                // Prevent 100-continues from being added
                //

                string newValue = value.ToLower(CultureInfo.InvariantCulture);

                fContinue100 = (newValue.IndexOf(ContinueHeader) != -1) ;

                if (fContinue100) {
                    throw new ArgumentException(SR.GetString(SR.net_no100), "value");
                }
                else {
                    _HttpRequestHeaders.CheckUpdate(HttpKnownHeaderNames.Expect, value);
                }
#if DEBUG
                }
#endif
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Gets or sets the value of the If-Modified-Since header.
        ///    </para>
        /// </devdoc>
        public DateTime IfModifiedSince {
            get {
#if DEBUG
                using (GlobalLog.SetThreadKind(ThreadKinds.User | ThreadKinds.Async)) {
#endif
                string ifmodHeaderValue = _HttpRequestHeaders[HttpKnownHeaderNames.IfModifiedSince];

                if (ifmodHeaderValue == null) {
                    return DateTime.Now;
                }
                else if (_CachedIfModifedSince != DateTime.MinValue) {
                    return _CachedIfModifedSince;
                }

                return HttpProtocolUtils.string2date(ifmodHeaderValue);
#if DEBUG
                }
#endif
            }
            set {
#if DEBUG
                using (GlobalLog.SetThreadKind(ThreadKinds.User | ThreadKinds.Async)) {
#endif
                SetSpecialHeaders(HttpKnownHeaderNames.IfModifiedSince, HttpProtocolUtils.date2string(value));
                _CachedIfModifedSince = value;
#if DEBUG
                }
#endif
            }
        }

        internal byte[] WriteBuffer {
            get {
                return _WriteBuffer;
            }
        }

        //
        // Frequently usedheaders made available as properties,
        //   the following headers add or remove headers taking care
        //   of special cases due to their unquie qualities within
        //   the net handlers,
        //
        //  Note that all headers supported here will be disallowed,
        //    and not accessable through the normal Header objects.
        //

        /*
            Accessor:   SetSpecialHeaders

            Private method for removing duplicate code which removes and
              adds headers that are marked private

            Input:  HeaderName, value to set headers

            Returns: none


        */

        private void SetSpecialHeaders(string HeaderName, string value) {
            value = WebHeaderCollection.CheckBadChars(value, true);
            _HttpRequestHeaders.RemoveInternal(HeaderName);
            if (value.Length != 0) {
                _HttpRequestHeaders.AddInternal(HeaderName, value);
            }
        }

        /*
            Abort - Attempts to abort pending request,

            This calls into the delegate, and then closes any pending streams.

            Input: none

            Returns: none

        */
        public override void Abort()
        {
#if DEBUG
            using (GlobalLog.SetThreadKind(ThreadKinds.User /* | ThreadKinds.Async would be nice */)) {
#endif
            Abort(null, AbortState.Public);
#if DEBUG
            }
#endif
        }

        private void Abort(Exception exception, int abortState)
        {
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::Abort()");
            if (Logging.On) Logging.Enter(Logging.Web, this, "Abort", (exception == null? "" :  exception.Message));

            if(Interlocked.CompareExchange(ref m_Aborted, abortState, 0) == 0) // public abort will never drain streams
            {
                GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::Abort() - " + exception);

                m_OnceFailed = true;
                CancelTimer();

                WebException webException = exception as WebException;
                if (exception == null)
                {
                    webException = new WebException(NetRes.GetWebStatusString("net_requestaborted", WebExceptionStatus.RequestCanceled), WebExceptionStatus.RequestCanceled);
                }
                else if (webException == null)
                {
                    webException = new WebException(NetRes.GetWebStatusString("net_requestaborted", WebExceptionStatus.RequestCanceled), exception, WebExceptionStatus.RequestCanceled, _HttpResponse);
                }

                try
                {
#if DEBUG
                bool setResponseCalled = false;
                try
                {
#endif
                    // Want to make sure that other threads see that we're aborted before they set an abort delegate, or that we see
                    // the delegate if they might have missed that we're aborted.
                    Thread.MemoryBarrier();
                    HttpAbortDelegate abortDelegate = _AbortDelegate;
#if DEBUG
                    m_AbortDelegateUsed = abortDelegate == null ? (object) DBNull.Value : abortDelegate;
#endif
                    if (abortDelegate == null || abortDelegate(this, webException))
                    {
                        // We don't have a connection associated with this request

                        LazyAsyncResult chkConnectionAsyncResult = Async? null: ConnectionAsyncResult;
                        LazyAsyncResult chkReaderAsyncResult = Async? null: ConnectionReaderAsyncResult;

#if DEBUG
                        setResponseCalled = true;
#endif
                        SetResponse(webException);

                        if (chkConnectionAsyncResult != null)
                            chkConnectionAsyncResult.InvokeCallback(webException);
                        if (chkReaderAsyncResult != null)
                            chkReaderAsyncResult.InvokeCallback(webException);
                    }
#if DEBUG
                }
                catch (Exception stressException)
                {
                    t_LastStressException = stressException;
                    if (!NclUtilities.IsFatal(stressException)){
                        GlobalLog.Assert(setResponseCalled, "HttpWebRequest#{0}::Abort|{1}", ValidationHelper.HashString(this), stressException.Message);
                    }
                    throw;
                }
#endif
                }
                catch (Exception ex)
                {
                    if (NclUtilities.IsFatal(ex)) throw;
                }
                catch { }
            }

            if(Logging.On)Logging.Exit(Logging.Web, this, "Abort", "");
        }

        // Cancel any pending timer.
        private void CancelTimer()
        {
            TimerThread.Timer timer = _Timer;
            if (timer != null)
            {
                timer.Cancel();
            }
        }

        // TimeoutCallback - Called by the TimerThread to abort a request.  This just posts ThreadPool work item - Abort() does too
        // much to be done on the timer thread (timer thread should never block or call user code).
        private static void TimeoutCallback(TimerThread.Timer timer, int timeNoticed, object context)
        {
            ThreadPool.UnsafeQueueUserWorkItem(s_AbortWrapper, context);
        }

        private static void AbortWrapper(object context)
        {
#if DEBUG
            GlobalLog.SetThreadSource(ThreadKinds.Worker);
            using (GlobalLog.SetThreadKind(ThreadKinds.System)) {
#endif
            ((HttpWebRequest) context).Abort(new WebException(NetRes.GetWebStatusString(WebExceptionStatus.Timeout), WebExceptionStatus.Timeout), AbortState.Public);
#if DEBUG
            }
#endif
        }

        /*
            FindServicePoint - Finds the ServicePoint for this request

            This calls the FindServicePoint off of the ServicePointManager
            to determine what ServicePoint to use.  When our proxy changes,
            or there is a redirect, this should be recalled to determine it.

            Input:  forceFind        - regardless of the status, always call FindServicePoint

            Returns: ServicePoint

        */
        private ServicePoint FindServicePoint(bool forceFind) {
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::FindServicePoint");

            ServicePoint servicePoint = _ServicePoint;
            if ( servicePoint == null || forceFind ) {
                lock(this) {
                    //
                    // we will call FindServicePoint - iff
                    //  - there is no service point ||
                    //  - we are forced to find one, usually due to changes of the proxy or redirection
                    //

                    if ( _ServicePoint == null || forceFind ) {

                        if (!ProxySet) {
                            _Proxy = WebRequest.InternalDefaultWebProxy;
                        }
                        if (_ProxyChain != null)
                        {
                            _ProxyChain.Dispose();
                        }
                        _ServicePoint = ServicePointManager.FindServicePoint(_Uri, _Proxy, out _ProxyChain, ref _AbortDelegate, ref m_Aborted);
                        if(Logging.On)Logging.Associate(Logging.Web, this, _ServicePoint);
                    }
                }
                servicePoint = _ServicePoint;
            }

            return servicePoint;
        }

        /*
            InvokeGetRequestStreamCallback - Notify our GetRequestStream caller

            This is needed to tell our caller that we're finished,
            and he can go ahead and write to the stream.
        */
        private void InvokeGetRequestStreamCallback()
        {
            GlobalLog.Enter("HttpWebRequest#" + ValidationHelper.HashString(this) + "::InvokeGetRequestStreamCallback");
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::InvokeGetRequestStreamCallback");

            LazyAsyncResult asyncResult = _WriteAResult;
            GlobalLog.Assert(asyncResult == null || this == (HttpWebRequest)asyncResult.AsyncObject, "HttpWebRequest#{0}::InvokeGetRequestStreamCallback()|this != asyncResult.AsyncObject", ValidationHelper.HashString(this));

            if (asyncResult != null) {
                try {
                    asyncResult.InvokeCallback(_SubmitWriteStream);
                }
                catch (Exception exception) {
                    if (NclUtilities.IsFatal(exception)) throw;

                    Abort(exception, AbortState.Public);
                    GlobalLog.LeaveException("HttpWebRequest#" + ValidationHelper.HashString(this) + "::InvokeGetRequestStreamCallback", exception);
                    throw;
                }
            }
            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::InvokeGetRequestStreamCallback", "success");
        }

        /*
            RequestSubmitDone - Handle submit done callback.

            This is our submit done handler, called by the underlying connection
            code when a stream is available for our use. We save the stream for
            later use and signal the wait event.

            We also handle the continuation/termination of a BeginGetRequestStream,
            by saving out the result and calling its callback if needed.

            Input:  SubmitStream        - The stream we may write on.
                    Status              - The status of the submission.

            Returns: Nothing.

        */
        internal void SetRequestSubmitDone(ConnectStream submitStream) {
            GlobalLog.Enter("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetRequestSubmitDone", ValidationHelper.HashString(submitStream));
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetRequestSubmitDone");

            if (!Async) {
                ConnectionAsyncResult.InvokeCallback();
            }

            if (AllowWriteStreamBuffering) {
                submitStream.EnableWriteBuffering();
            }

            if (submitStream.CanTimeout) {
                submitStream.ReadTimeout = ReadWriteTimeout;
                submitStream.WriteTimeout = ReadWriteTimeout;
            }
            if(Logging.On)Logging.Associate(Logging.Web, this, submitStream);
            _SubmitWriteStream = submitStream;

            //
            // This line is needed ONLY if we got a connect failure (Abort can still happen at random time)
            // CallDone will check for the write side response processing and this is what we want.
            // Note that Sync case already has a separate path to check for the response
            //
            if (Async && _CoreResponse != null && (object)_CoreResponse != (object)DBNull.Value)
            {
                GlobalLog.Assert(_CoreResponse is Exception, "SetRequestSubmitDone()|Found offensive response right after getting connection ({0}).", _CoreResponse);
                submitStream.CallDone();
                GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetRequestSubmitDone() - already have a core response", _CoreResponse.GetType().FullName);
                return;
            }

            EndSubmitRequest();
            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetRequestSubmitDone");
        }


        internal void WriteHeadersCallback(WebExceptionStatus errorStatus, ConnectStream stream, bool async)
        {
            GlobalLog.Enter("HttpWebRequest#" + ValidationHelper.HashString(this) + "::WriteHeadersCallback", ValidationHelper.HashString(stream));
            if(errorStatus == WebExceptionStatus.Success)
            {
                bool completed = EndWriteHeaders(async);
                if (!completed) {
                    errorStatus = WebExceptionStatus.Pending;
                }
                else {
                    GlobalLog.Print("ConnectStream#" + ValidationHelper.HashString(stream) + "::WriteHeaders completed:true BytesLeftToWrite:" + stream.BytesLeftToWrite.ToString());
                    if (stream.BytesLeftToWrite == 0) {
                        //
                        // didn't go pending, no data to write. we're done.
                        //
                        stream.CallDone();
                    }
                }
            }

            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::WriteHeadersCallback", errorStatus.ToString());
        }

        /*
            SetRequestContinue - Handle 100-continues on Posting

            This indicates to us that we need to continue posting,
            and there is no need to buffer because we are now
        */
        internal void SetRequestContinue()
        {
            SetRequestContinue(null);
        }

        internal void SetRequestContinue(CoreResponseData continueResponse)
        {
            GlobalLog.Enter("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetRequestContinue");
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetRequestContinue");

            _RequestContinueCount++;

            if (HttpWriteMode == HttpWriteMode.None)
            {
                GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetRequestContinue - not a POST type request, return");
                return;
            }

            GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetRequestContinue() _RequestContinueCount:" + _RequestContinueCount);

            if (m_ContinueGate.Complete())
            {
                // Generally, in Sync mode there will not be a timer (instead PollAndRead times out).  However, in mixed mode
                // there can be.  If there is a timer, whether or not to call EndWriteHeaders_Part2 depends on whether
                // we can successfully cancel it here.  Otherwise, the timeout callback should call it.
                //
                // m_ContinueGate guards the synchronization of m_ContinueTimer.
                TimerThread.Timer timer = m_ContinueTimer;
                m_ContinueTimer = null;

                // In the case there was no timer, just call EndWriteHeaders_Part2.
                if (timer == null || timer.Cancel())
                {
                    // Invoke the 100 continue delegate if the user supplied one and we received a 100 Continue.
                    if (continueResponse != null && ContinueDelegate != null)
                    {
                        GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetRequestContinue() calling ContinueDelegate()");
                        ExecutionContext x = null;
                        if (x == null)
                        {
                            ContinueDelegate((int) continueResponse.m_StatusCode, continueResponse.m_ResponseHeaders);
                        }
                        else
                        {
                            ExecutionContext.Run(x, new ContextCallback(CallContinueDelegateCallback), continueResponse);
                        }
                    }
                    
                    EndWriteHeaders_Part2();
                }
           }

            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetRequestContinue");
        }

        //
        // Used to keep us looping and parsing until we get a 100 continue.
        // Since we can get more than one 100 continue during redirects or auth, we need to keep it on a counter.
        //
        internal int RequestContinueCount {
            get {
                return _RequestContinueCount;
            }
        }
        //
        // This will open the write side response window, so any response processing
        // coming before the window is closed will be delayed.
        // CheckWriteSideResponseProcessing() becomes a must do call sometime after this method was invoked.
        //
        internal void OpenWriteSideResponseWindow()
        {
            //
            // Multithreading: This method is called from the connection and under the connection lock
            // It should not be subject to a race condition. Note that request.Abort() also ends up with the connection lock.
            GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::OpenWriteSideResponseWindow()");

            _CoreResponse = DBNull.Value;
            _NestedWriteSideCheck = 0;
        }
        //
        // If the response is already received this method starts its processing.
        // Otheriwse the window for write side response processing is closed
        //
        // We do not clear current response if any hence SetAndOrProcessResponse() will not go into concurent
        // reponse processing
        //
        internal void CheckWriteSideResponseProcessing()
        {
            // In Sync case never close the write side window
            object responseData = Async? Interlocked.CompareExchange(ref _CoreResponse, null, DBNull.Value): _CoreResponse;

            if (responseData == DBNull.Value)
            {
                GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckWriteSideResponseProcessing() - responseData = DBNull means no response yet, " +  (Async?"async: closing write side window": "sync: leaving write side window opened"));
                return;
            }

            if (responseData == null)
            {
                // change later on responseData = new a generic Abort WebException
                throw new InternalException(); // we set it to DBNull.Value and want to find it now as either that value or as the real response
            }

            if (!Async && ++_NestedWriteSideCheck != 1)
            {
                GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckWriteSideResponseProcessing() - Repeated SYNC check _NestedWriteSideCheck = " + _NestedWriteSideCheck + ", response =" + responseData.GetType().FullName);
                return;
            }

            GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckWriteSideResponseProcessing() - Response is ready, process here: " + responseData.GetType().FullName);

            // The response has been already set and the processing was deferred until now
            Exception exception  = responseData as Exception;
            if (exception != null)
                SetResponse(exception);
            else
                SetResponse(responseData as CoreResponseData);
        }
        //
        //
        // Some kind of response is ready and this method will ether process it or defer until the write side response check.
        //
        // Note, when an exception comes here then the request is already taken off the list and the connection is BEING closed.
        internal void SetAndOrProcessResponse(object responseOrException)
        {
            GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetAndOrProcessResponse() - Entered with responseOrException: " + responseOrException);

            if (responseOrException == null)
                throw new InternalException(); //Consider making an Assert later. If the condtion is met then this request may hang.

            CoreResponseData newResponse  = responseOrException as CoreResponseData;
            WebException    newException = responseOrException as WebException;

            object responseData = Interlocked.CompareExchange(ref _CoreResponse,  responseOrException, DBNull.Value);

            //
            // (1) This method cannot be called with two exceptions in a row or with two responses in a row.
            // (2) If a response comes then an exception means a fatal exception overrides the response or otherwise ignored
            // (3) If an exception comes then a response means the exception takes precedence so the response can be ignored
            //

            if (responseData != null)
            {
                if (responseData.GetType() == typeof(CoreResponseData))
                {
                     if (newResponse != null)
                         throw new InternalException();// make an assert later

                     if (newException != null && newException.InternalStatus != WebExceptionInternalStatus.ServicePointFatal &&
                        newException.InternalStatus != WebExceptionInternalStatus.RequestFatal)
                         return;
                     // Else the exception will override the response
                }
                else if (responseData.GetType() != typeof(DBNull))
                {
                    // Here responseData == Exception so newResponse must not be an exception
                    if (newResponse  == null)
                        throw new InternalException();// make an assert later that will be ignored in retail

                    //We have an exception and now getting a response
                    // Release that response stream to unblock the connection
                    ICloseEx closeEx = newResponse.m_ConnectStream as ICloseEx;
                    if (closeEx != null)
                    {
                        closeEx.CloseEx(CloseExState.Silent);
                    }
                    else
                    {
                        newResponse.m_ConnectStream.Close();
                    }

                    return;
                }
            }

            //
            // Can the write side process that response?
            //
            if (responseData == DBNull.Value)
            {
                GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetAndOrProcessResponse() - Write Thread will procees the response.");
                //
                // Note for a sync request a write side window is always open
                //
                if (!Async)
                {
                    LazyAsyncResult chkConnectionAsyncResult = ConnectionAsyncResult;
                    LazyAsyncResult chkReaderAsyncResult = ConnectionReaderAsyncResult;

                    chkConnectionAsyncResult.InvokeCallback(responseOrException); // ref "responseOrException": could be anything except for AsyncTriState or stream
                    chkReaderAsyncResult.InvokeCallback(responseOrException);     // ref "responseOrException": could be anything except for null
                }
                return;
            }
            else if (responseData != null)
            {
                Exception e = responseOrException as Exception;
                if (e != null)
                {
                    // This may happen if we failed while trying to process a response.
                    GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetAndOrProcessResponse() - Fatal Exception -> Override old response = " + responseData);
                    SetResponse(e);
                }
                else
                {
                    GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetAndOrProcessResponse() - Error!!! Unexpected (1) Response already set response = " + responseData);
                    throw new InternalException();// make an assert later
                }
                return;
            }

            //
            // Can that thread process the response?
            //
            responseData = Interlocked.CompareExchange(ref _CoreResponse,  responseOrException, null);
            if (responseData != null)
            {
                GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetAndOrProcessResponse() - Warning!!! Unexpected (2) SetResponse, already set response = " + responseData);

                // This is a race with Abort, if we got an exception then re-enter SetResponse, otherwise return
                if (newResponse != null)
                {
                    GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetAndOrProcessResponse() - Ignoring current response while SetResponse() is processing an exception");

                    // We have an exception and now getting a response
                    // Release that response stream to unblock the connection
                    ICloseEx closeEx = newResponse.m_ConnectStream as ICloseEx;
                    if (closeEx != null)
                    {
                        closeEx.CloseEx(CloseExState.Silent);
                    }
                    else
                    {
                        newResponse.m_ConnectStream.Close();
                    }

                    return;
                }
                GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetAndOrProcessResponse() - Forcing new exception to re-enter SetResponse() that is being processed with CoreResponseData");
            }

            if (!Async)
                throw new InternalException(); //Consider making an Assert later. If the condition is met it _might_ process a sync request on non submitting thread.

            // For this request the write side response window was already closed so we have to procees the response
            if (newResponse != null)
                SetResponse(newResponse);
            else
                SetResponse(responseOrException as Exception);
        }
        //
        // When we got a live response, this method will construct the response object
        // and then consult with caching protocol on the appropriate action.
        // On return the response can be re-created.
        // Under some cases this method may initate retrying of the current request.
        //
        private void SetResponse(CoreResponseData coreResponseData) {
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetResponse");
            GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetResponse - coreResponseData=" + ValidationHelper.HashString(coreResponseData));

            try {
                if (!Async)
                {
                    LazyAsyncResult chkConnectionAsyncResult = ConnectionAsyncResult;
                    LazyAsyncResult chkReaderAsyncResult = ConnectionReaderAsyncResult;

                    chkConnectionAsyncResult.InvokeCallback(coreResponseData); // ref "coreResponseData": could be anything except for AsyncTriState or stream
                    chkReaderAsyncResult.InvokeCallback(coreResponseData);     // ref "coreResponseData": could be anything except for null
                }

                if (coreResponseData != null)
                {
                    if (coreResponseData.m_ConnectStream.CanTimeout)
                    {
                        coreResponseData.m_ConnectStream.WriteTimeout = ReadWriteTimeout;
                        coreResponseData.m_ConnectStream.ReadTimeout = ReadWriteTimeout;
                    }
                    _HttpResponse = new HttpWebResponse(_Uri, CurrentMethod, coreResponseData, _MediaType, UsesProxySemantics, AutomaticDecompression);

                    if(Logging.On)Logging.Associate(Logging.Web, this, coreResponseData.m_ConnectStream);
                    if(Logging.On)Logging.Associate(Logging.Web, this, _HttpResponse);
                    ProcessResponse();
                }
                else
                {
                    GlobalLog.Assert("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetResponse()", "coreResponseData == null");
                    Abort(null, AbortState.Public);
                    GlobalLog.LeaveException("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetResponse", new InternalException());
                }
            }
            catch (Exception exception) {
                Abort(exception, AbortState.Internal);
                GlobalLog.LeaveException("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetResponse", exception);
            }
            return;
        }

        /*++

        Routine Description:

            Wakes up blocked threads, so they can read response object,
              from the result

            We also handle the continuation/termination of a BeginGetResponse,
            by saving out the result and calling its callback if needed.


        --*/
        private void ProcessResponse() {
            GlobalLog.Enter("HttpWebRequest#" + ValidationHelper.HashString(this) + "::ProcessResponse", "From Cache = " + _HttpResponse.IsFromCache);
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::ProcessResponse");

            HttpProcessingResult httpResult = HttpProcessingResult.Continue;
            Exception exception = null;

            // handle redirects, authentication, and such
            httpResult = DoSubmitRequestProcessing(ref exception);

            if (httpResult == HttpProcessingResult.Continue)
            {
                CancelTimer();

                object result = exception != null ? (object) exception : (object) _HttpResponse;

                if (_ReadAResult == null)
                {
                    lock (this)
                    {
                        if (_ReadAResult == null)
                        {
                            _ReadAResult = new LazyAsyncResult(null, null, null, result);  //never throws
                        }
                    }
                }

                try
                {
                    FinishRequest(_HttpResponse, exception); // never throws

                    _ReadAResult.InvokeCallback(result);

                    try
                    {
                        SetRequestContinue();
                    }
                    catch { }
                }
                catch (Exception e)
                {
                    Abort(e, AbortState.Public);
                    throw;
                }
                finally
                {
                    // If request was already aborted the response will not be set on asyncResult, hence abort it now
                    if (exception == null && _ReadAResult.Result != (object) _HttpResponse)
                    {
                        WebException webException = _ReadAResult.Result as WebException;
                        if (webException != null && webException.Response != null)
                        {
                            GlobalLog.Assert(object.ReferenceEquals(webException.Response, _HttpResponse), "HttpWebRequset#{0}::ProcessResponse|Different HttpWebResponse in exception versus _HttpResponse.", ValidationHelper.HashString(this));
                            _HttpResponse.Abort();  // never throws
                        }
                    }
                }
            }
            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::ProcessResponse");
        }

        //
        // Process an exception and optionally set request for retrying
        //
        private void SetResponse(Exception E) {
#if DEBUG
            bool callbackInvoked = false;
            try
            {
#endif
            GlobalLog.Enter("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetResponse", E.ToString() + "/*** SETRESPONSE IN ERROR ***");
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetResponse");

            HttpProcessingResult httpResult = HttpProcessingResult.Continue;

            //
            // Preserve the very first web exception occured if it was fatal
            //
            WebException webException = HaveResponse ? _ReadAResult.Result as WebException : null;
            WebException newWebException = E as WebException;
            if (webException != null && (webException.InternalStatus == WebExceptionInternalStatus.RequestFatal ||
                webException.InternalStatus == WebExceptionInternalStatus.ServicePointFatal) &&
                (newWebException == null || newWebException.InternalStatus != WebExceptionInternalStatus.RequestFatal))
            {
                E = webException;
            }
            else
            {
                webException = newWebException;
            }

            if (E != null) 
            {
                if (Logging.On) Logging.Exception(Logging.Web, this, "", webException);
            }

            try {
                if ( webException != null &&
                     (webException.InternalStatus == WebExceptionInternalStatus.Isolated ||
                     webException.InternalStatus == WebExceptionInternalStatus.ServicePointFatal ||
                     (webException.InternalStatus == WebExceptionInternalStatus.Recoverable && !m_OnceFailed)))
                {

                    if (webException.InternalStatus == WebExceptionInternalStatus.Recoverable)
                        m_OnceFailed = true;

                    Pipelined = false;

                    if (_SubmitWriteStream != null && _OldSubmitWriteStream == null && _SubmitWriteStream.BufferOnly) {
                        _OldSubmitWriteStream = _SubmitWriteStream;
                    }

                    httpResult = DoSubmitRequestProcessing(ref E);
                }
            }
            catch (Exception unexpectedException)
            {
                if (NclUtilities.IsFatal(unexpectedException)) throw;

                // This is highly unexpected but if happens would result into Aborted exception with caught one as an inner exception
                httpResult = HttpProcessingResult.Continue;
                E = new WebException(NetRes.GetWebStatusString("net_requestaborted", WebExceptionStatus.RequestCanceled), unexpectedException, WebExceptionStatus.RequestCanceled, _HttpResponse);
            }
            finally
            {
                if (httpResult == HttpProcessingResult.Continue)
                {
                    CancelTimer();

                    if (!(E is WebException) && !(E is SecurityException))
                    {
                        if (_HttpResponse==null) {
                            E = new WebException(E.Message, E);
                        }
                        else {
                            E = new WebException(
                                SR.GetString(
                                    SR.net_servererror,
                                    NetRes.GetWebStatusCodeString(
                                        ResponseStatusCode,
                                        _HttpResponse.StatusDescription)),
                                E,
                                WebExceptionStatus.ProtocolError,
                                _HttpResponse );
                        }
                    }

                    LazyAsyncResult writeAResult;
                    LazyAsyncResult readAResult = null;

                    // Async Abort may happen at any time including when the request is being cleared
                    // and resubmitted hence using checked response.
                    HttpWebResponse chkResponse = _HttpResponse;

                    lock (this)
                    {
                        writeAResult = _WriteAResult;

                        if (_ReadAResult == null)
                        {
                            _ReadAResult = new LazyAsyncResult(null, null, null, E); //never throws
                        }
                        else
                        {
                            readAResult = _ReadAResult;
                        }
                    }

                    try
                    {
                        FinishRequest(chkResponse, E); //never throws

                        try
                        {
                            if (writeAResult != null) {
                                writeAResult.InvokeCallback(E);
                            }
                        }
                        finally
                        {
                            if (readAResult != null) {
#if DEBUG
                                callbackInvoked = true;
#endif
                                readAResult.InvokeCallback(E);
                            }
                        }
                    }
                    finally
                    {
                        chkResponse = _ReadAResult.Result as HttpWebResponse;

                        // If the response was already set that exception closes it.
                        if (chkResponse != null)
                        {
                            chkResponse.Abort();    //never throws
                        }

                        if (CacheProtocol != null)
                        {
                            CacheProtocol.Abort();  //never throws
                        }
                    }
                }
#if DEBUG
                else
                {
                    callbackInvoked = true;
                }
#endif
            }
            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SetResponse", httpResult.ToString());
#if DEBUG
            }
            catch (Exception exception)
            {
                t_LastStressException = exception;

                if (!NclUtilities.IsFatal(exception)){
                    GlobalLog.Assert(callbackInvoked, "HttpWebRequest#{0}::SetResponse|{1}", ValidationHelper.HashString(this), exception.Message);
                }
                throw;
            }
#endif
        }

#if DEBUG
        [ThreadStatic]
        private static Exception t_LastStressException;
        private object m_AbortDelegateUsed;
#endif


        /*++

            BeginSubmitRequest: Begins Submit off a request to the network.

            This is called when we need to transmit an Async Request, but
            this function only servers to activate the submit, and does not
            actually block

            Called when we want to submit a request to the network. We do several
            things here - look for a proxy, find the service point, etc. In the
            end we call the service point to get access (via a stream) to the
            underlying connection, then serialize our headers onto that connection.
            The actual submission request to the service point is async, so we
            submit the request and then return, to allow the async to run its course.

            Input:
                forceFind - insures that always get a new ServicePoint,
                    needed on redirects, or where the ServicePoint may have changed

            Returns: Nothing

        --*/

        private void BeginSubmitRequest()
        {
            GlobalLog.Enter("HttpWebRequest#" + ValidationHelper.HashString(this) + "::BeginSubmitRequest");
            GlobalLog.ThreadContract(ThreadKinds.User, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::BeginSubmitRequest");
            GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::BeginSubmitRequest() Method:" + Method + " Address:" + Address);

            SubmitRequest(FindServicePoint(false));

            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::BeginSubmitRequest");
        }

        private void SubmitRequest(ServicePoint servicePoint)
        {
            GlobalLog.Enter("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SubmitRequest");
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::SubmitRequest");
            GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SubmitRequest() HaveResponse:" + HaveResponse + " Saw100Continue:" + Saw100Continue);

            if (!Async)
            {
                _ConnectionAResult = new LazyAsyncResult(this, null, null);
                _ConnectionReaderAResult = new LazyAsyncResult(this, null, null);
                OpenWriteSideResponseWindow();
            }

            if (_Timer == null && !Async){
                _Timer = TimerQueue.CreateTimer(s_TimeoutCallback, this);
            }

            try {

                if (_SubmitWriteStream != null && _SubmitWriteStream.IsPostStream)
                {
                    // _OldSubmitWriteStream is the stream that holds real user data
                    // In no case it can be overwritten.
                    // For multiple resubmits the ContentLength was set already, so no need call it again.
                    // on first resubmission the real user data hasn't been saved, so _OldSubmitWriteStream is null
                    GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SubmitRequest() (resubmit) firstResubmission:" + (_OldSubmitWriteStream == null) + " NtlmKeepAlive:" + NtlmKeepAlive);
                    if (_OldSubmitWriteStream == null)
                    {
                        // save the real user data.
                        GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SubmitRequest() (resubmit) save the real user data _OldSubmitWriteStream#" + ValidationHelper.HashString(_OldSubmitWriteStream));
                        _OldSubmitWriteStream = _SubmitWriteStream;
                    }
                    // make sure we reformat the headers before resubmitting
                    _WriteBuffer = null;
                }

                m_Retry = false;

                // If pre-authentication is requested call the AuthenticationManager
                // and add authorization header if there is response
                if (PreAuthenticate) {
                    if (UsesProxySemantics && _Proxy != null && _Proxy.Credentials != null)
                        ProxyAuthenticationState.PreAuthIfNeeded(this, _Proxy.Credentials);
                    if (Credentials != null)
                        ServerAuthenticationState.PreAuthIfNeeded(this, Credentials);
                }

                if (WriteBuffer == null) {
                    UpdateHeaders();
                }

                if (CheckCacheRetrieveBeforeSubmit()) {
                    // We are done and internal Response processing is kicked in
                    GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SubmitRequest CACHED RESPONSE");
                    return;
                }

                // At this point we are going to send a live request
                // _AbortDelegate is set on submission process.
                servicePoint.SubmitRequest(this, GetConnectionGroupLine());
            }
            finally {
                if (!Async)
                    CheckWriteSideResponseProcessing();
            }
            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SubmitRequest");
        }


        //
        // This method may be invoked as part of the request submission but
        // before the response is received
        // Return:
        // - True       = response is ready
        // - False      = Proceed with the request submission
        private bool CheckCacheRetrieveBeforeSubmit() {
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckCacheRetrieveBeforeSubmit");

            if (CacheProtocol == null) {
                return false;
            }

            try
            {
                Uri cacheUri = _Uri;
                if (cacheUri.Fragment.Length != 0)
                    cacheUri = new Uri(cacheUri.GetParts(UriComponents.AbsoluteUri & ~UriComponents.Fragment, UriFormat.SafeUnescaped));

                CacheProtocol.GetRetrieveStatus(cacheUri, this);

                if (CacheProtocol.ProtocolStatus == CacheValidationStatus.Fail) {
                    throw CacheProtocol.ProtocolException;
                }

                if (CacheProtocol.ProtocolStatus != CacheValidationStatus.ReturnCachedResponse) {
                    return false;
                }

                if (HttpWriteMode!=HttpWriteMode.None) {
                    throw new NotSupportedException(SR.GetString(SR.net_cache_not_supported_body));
                }

                // If we take it from cache, we have to kick in response processing
                // The _CacheStream is good to return as the response stream.
                HttpRequestCacheValidator ctx = (HttpRequestCacheValidator) CacheProtocol.Validator;
                CoreResponseData responseData = new CoreResponseData();
                responseData.m_IsVersionHttp11  = ctx.CacheHttpVersion.Equals(HttpVersion.Version11);
                responseData.m_StatusCode       = ctx.CacheStatusCode;
                responseData.m_StatusDescription= ctx.CacheStatusDescription;
                responseData.m_ResponseHeaders  = ctx.CacheHeaders;
                responseData.m_ContentLength    = CacheProtocol.ResponseStreamLength;
                responseData.m_ConnectStream    = CacheProtocol.ResponseStream;
                _HttpResponse = new HttpWebResponse(_Uri, CurrentMethod, responseData, _MediaType, UsesProxySemantics, AutomaticDecompression);
                _HttpResponse.InternalSetFromCache = true;
                _HttpResponse.InternalSetIsCacheFresh = (ctx.CacheFreshnessStatus != CacheFreshnessStatus.Stale);
                ProcessResponse();
                return true;
            }
            catch (Exception exception)
            {
                Abort(exception, AbortState.Public);
                throw;
            }
        }

        //
        // This method has to be invoked as part of the wire response processing.
        // The wire response can be replaced on return
        //
        // ATTN: If the method returns false, the response is invalid and should be retried
        //
        private bool CheckCacheRetrieveOnResponse() {
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckCacheRetrieveOnResponse");

            if (CacheProtocol == null) {
                return true;
            }

            if (CacheProtocol.ProtocolStatus == CacheValidationStatus.Fail) {
                throw CacheProtocol.ProtocolException;
            }

            Stream oldResponseStream = _HttpResponse.ResponseStream;

            CacheProtocol.GetRevalidateStatus(_HttpResponse, _HttpResponse.ResponseStream);

            if (CacheProtocol.ProtocolStatus == CacheValidationStatus.RetryResponseFromServer)
            {
                // Try to resubmit or fail
                return false;
            }

            if (CacheProtocol.ProtocolStatus != CacheValidationStatus.ReturnCachedResponse &&
                CacheProtocol.ProtocolStatus != CacheValidationStatus.CombineCachedAndServerResponse)
            {
                // Take current response
                return true;
            }

            if (HttpWriteMode!=HttpWriteMode.None) {
                // This should never happen in real life
                throw new NotSupportedException(SR.GetString(SR.net_cache_not_supported_body));
            }

            CoreResponseData responseData = new CoreResponseData();
            HttpRequestCacheValidator ctx = (HttpRequestCacheValidator) CacheProtocol.Validator;
            // If we take it from cache, we have to replace the live response if any
            responseData.m_IsVersionHttp11  = ctx.CacheHttpVersion.Equals(HttpVersion.Version11);
            responseData.m_StatusCode       = ctx.CacheStatusCode;
            responseData.m_StatusDescription= ctx.CacheStatusDescription;

            responseData.m_ResponseHeaders  = CacheProtocol.ProtocolStatus == CacheValidationStatus.CombineCachedAndServerResponse
                                                ? new WebHeaderCollection(ctx.CacheHeaders)
                                                : ctx.CacheHeaders;

            responseData.m_ContentLength    = CacheProtocol.ResponseStreamLength;
            responseData.m_ConnectStream    = CacheProtocol.ResponseStream;

            _HttpResponse = new HttpWebResponse(_Uri, CurrentMethod, responseData, _MediaType, UsesProxySemantics, AutomaticDecompression);

            if (CacheProtocol.ProtocolStatus == CacheValidationStatus.ReturnCachedResponse)
            {
                _HttpResponse.InternalSetFromCache = true;
                _HttpResponse.InternalSetIsCacheFresh = CacheProtocol.IsCacheFresh;

                // can only dispose the response stream when not combining the streams
                // Note the response itself may still be needed for cache update call.
                if (oldResponseStream != null)
                {
                    try {
                        oldResponseStream.Close();
                    }
                    catch {
                    }
                }
            }
            return true;
        }

        //
        // This will decide on cache update and construct the effective response
        //
        // Possible CacheStatus on Input:
        // - Bypass     = exit from the method, now
        // - FromCache  = Take from cache, do not update the headers (see Bypass))
        // - Proceed    = Take the cached response, ask for cache Context update
        // - NoCache    = Take the live response, ask for cache update
        //
        private void CheckCacheUpdateOnResponse() {
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckCacheUpdateOnResponse");

            if (CacheProtocol == null) {
                return;
            }

            if (CacheProtocol.GetUpdateStatus(_HttpResponse, _HttpResponse.ResponseStream) == CacheValidationStatus.UpdateResponseInformation)
            {
                _HttpResponse.ResponseStream = CacheProtocol.ResponseStream;
            }
            else if (CacheProtocol.ProtocolStatus == CacheValidationStatus.Fail)
                throw CacheProtocol.ProtocolException;
        }

        /*++

        EndSubmitRequest: End Submit off a request

        This function is invoked by a connection ready callback and continues the request by submitting its headers.

        --*/
        private void EndSubmitRequest()
        {
            GlobalLog.Enter("HttpWebRequest#" + ValidationHelper.HashString(this) + "::EndSubmitRequest");
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::EndSubmitRequest");

            try {

                //
                // check to see if we need to buffer the headers and send them at
                // a later time, when we know content length
                //
                if (HttpWriteMode==HttpWriteMode.Buffer) {
                    InvokeGetRequestStreamCallback();
                    GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::EndSubmitRequest", "InvokeGetRequestStreamCallback(HttpWriteMode==HttpWriteMode.Buffer)");
                    return;
                }

                // gather header bytes and write them to the stream
                if (WriteBuffer==null)
                {
                    long result = SwitchToContentLength();
                    SerializeHeaders();
                    PostSwitchToContentLength(result);
                }

                GlobalLog.Assert(WriteBuffer != null && WriteBuffer[0] < 0x80 && WriteBuffer[0] != 0x0, "HttpWebRequest#{0}::EndSubmitRequest()|Invalid WriteBuffer generated.", ValidationHelper.HashString(this));

                _SubmitWriteStream.WriteHeaders(Async);
            }
            catch {
                // We depend on this to unblock possible response processing in case of unexpected failure
                ConnectStream chkStream = _SubmitWriteStream;
                if (chkStream != null)
                    chkStream.CallDone();

                throw;
            }
            finally {
                if (!Async)
                    CheckWriteSideResponseProcessing();
            }

            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::EndSubmitRequest");
        }


        /*++

            EndWriteHeaders: End write of headers

            This Typically called by a callback that wishes to proceed
             with the finalization of writing headers

            Input: Nothing.

            Returns: bool - true if success, false if we need to go pending

        --*/
        internal bool EndWriteHeaders(bool async)
        {
            GlobalLog.Enter("HttpWebRequest#" + ValidationHelper.HashString(this) + "::EndWriteHeaders async:" + async.ToString());
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::EndWriteHeaders");

            try {
                //
                // if sending data and we sent a 100-continue to a 1.1 or better
                // server then synchronize with the 100-continue intermediate
                // response (or a final failure response)
                //
                GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::EndWriteHeaders() ContentLength:" + ContentLength + " HttpWriteMode:" + HttpWriteMode + " _ServicePoint.Understands100Continue:" + _ServicePoint.Understands100Continue + " ExpectContinue:" + ExpectContinue);
                if ((ContentLength>0 || HttpWriteMode==HttpWriteMode.Chunked) && ExpectContinue && _ServicePoint.Understands100Continue) {
                    if (async ? m_ContinueGate.StartTrigger(true) : m_ContinueGate.Trigger(true))
                    {
                        if (async)
                        {
                            try
                            {
                                // If we haven't already received the continue, set a timer.
                                GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::EndWriteHeaders() starting timer for 100Continue, timeout:" + DefaultContinueTimeout.ToString());
                                GlobalLog.Assert(m_ContinueTimer == null, "HttpWebRequest#{0}::EndWriteHeaders()|Timer already set.", ValidationHelper.HashString(this));
                                m_ContinueTimer = s_ContinueTimerQueue.CreateTimer(s_ContinueTimeoutCallback, this);
                            }
                            finally
                            {
                                m_ContinueGate.FinishTrigger();
                            }

                            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::EndWriteHeaders", false);
                            return false;
                        }
                        else {
                            GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::EndWriteHeaders() calling PollAndRead()");
                            // On the Sync Path, we need to poll the Connection to see if there is any Data
                            _SubmitWriteStream.PollAndRead(UserRetrievedWriteStream);
                            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::EndWriteHeaders", true);
                            return true;
                        }
                    }
                }

                // No continue expected or it was already received.  Move forward.
                EndWriteHeaders_Part2();
            }
            catch {
                // We depend on this to unblock possible response processing in case of unexpected failure
                ConnectStream chkStream = _SubmitWriteStream;
                if (chkStream != null)
                    chkStream.CallDone();

                throw;
            }

            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::EndWriteHeaders", true);
            return true;
        }

        // This can never call into user code or block, because it's called by the TimerThread.
        private static void ContinueTimeoutCallback(TimerThread.Timer timer, int timeNoticed, object context)
        {
            GlobalLog.Enter("HttpWebRequest#" + ValidationHelper.HashString(context) + "::ContinueTimeoutCallback");
            GlobalLog.ThreadContract(ThreadKinds.Unknown, ThreadKinds.SafeSources | ThreadKinds.Timer, "HttpWebRequest#" + ValidationHelper.HashString(context) + "::ContinueTimeoutCallback");

            HttpWebRequest thisHttpWebRequest = (HttpWebRequest) context;

            if (thisHttpWebRequest.HttpWriteMode == HttpWriteMode.None)
            {
                GlobalLog.Assert("HttpWebRequest#" + ValidationHelper.HashString(thisHttpWebRequest) + "::CompleteContinueGate()", "Not a POST type request, must not come here.");
                return;
            }

            // Complete the 100 Continue gate.  If this completes it, clean up the timer.
            if (thisHttpWebRequest.CompleteContinueGate())
            {
                GlobalLog.Assert(thisHttpWebRequest.m_ContinueTimer == timer, "HttpWebRequest#{0}::ContinueTimeoutCallback|Timers don't match.", ValidationHelper.HashString(thisHttpWebRequest));
                thisHttpWebRequest.m_ContinueTimer = null;
            }

            // Always call this.  Otherwise the timer should have been successfully cancelled.
            //
            // We have to put it on a threadpool thread since it may call user code.  This is not
            // a critical path for perf.
            ThreadPool.UnsafeQueueUserWorkItem(s_EndWriteHeaders_Part2Callback, thisHttpWebRequest);
            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(thisHttpWebRequest) + "::ContinueTimeoutCallback");
        }

        //
        private bool CompleteContinueGate()
        {
            return m_ContinueGate.Complete();
        }
        private static void EndWriteHeaders_Part2Wrapper(object state)
        {
#if DEBUG
            GlobalLog.SetThreadSource(ThreadKinds.Worker);
            using (GlobalLog.SetThreadKind(ThreadKinds.System)) {
#endif
            GlobalLog.Enter("HttpWebRequest#" + ValidationHelper.HashString(state) + "::EndWriteHeaders_Part2Wrapper");
            ((HttpWebRequest)state).EndWriteHeaders_Part2();
            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(state) + "::EndWriteHeaders_Part2Wrapper");
#if DEBUG
            }
#endif
        }

        internal void EndWriteHeaders_Part2()
        {
            GlobalLog.Enter("HttpWebRequest#" + ValidationHelper.HashString(this) + "::EndWriteHeaders_Part2");
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::EndWriteHeaders_Part2");

            try {
                ConnectStream submitWriteStream = _SubmitWriteStream;
                GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::EndWriteHeaders_Part2() ConnectStream#" + ValidationHelper.HashString(submitWriteStream) + " HttpWriteMode:" + HttpWriteMode + " HaveResponse:" + HaveResponse);
                if (HttpWriteMode != HttpWriteMode.None)
                {
                    m_BodyStarted = true;

                    GlobalLog.Assert(submitWriteStream != null, "HttpWebRequest#{0}::EndWriteHeaders_Part2()|submitWriteStream == null", ValidationHelper.HashString(this));
                    //
                    // We always need to buffer because some servers send
                    // 100 Continue even when they mean to redirect,
                    // so we waste the cycles with buffering
                    //
                    GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::EndWriteHeaders_Part2() AllowWriteStreamBuffering:" + AllowWriteStreamBuffering);
                    if (AllowWriteStreamBuffering) {
                        GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::EndWriteHeaders_Part2() BufferOnly:" + submitWriteStream.BufferOnly + " _OldSubmitWriteStream#" + ValidationHelper.HashString(_OldSubmitWriteStream) + " submitWriteStream#" + ValidationHelper.HashString(submitWriteStream));
                        if (submitWriteStream.BufferOnly) {
                            //
                            // if the ConnectStream was buffering the headers then
                            // there will not be an OldSubmitWriteStream. set it
                            // now to the newly created one.
                            //
                            _OldSubmitWriteStream = submitWriteStream;
                        }
                        if (_OldSubmitWriteStream != null)
                        {
                            GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::EndWriteHeaders_Part2() _OldSubmitWriteStream#" + ValidationHelper.HashString(_OldSubmitWriteStream) + " NtlmKeepAlive:" + NtlmKeepAlive);
    #if DEBUG
                            using (GlobalLog.SetThreadKind(ThreadKinds.Sync)) {
    #endif
                            submitWriteStream.ResubmitWrite(_OldSubmitWriteStream, (NtlmKeepAlive && ContentLength == 0));
    #if DEBUG
                            }
    #endif
                            submitWriteStream.CloseInternal(true);
                        }
                    }
                }
                else { // if (HttpWriteMode == HttpWriteMode.None) {
                    if (submitWriteStream != null) {
                        // close stream so the headers get sent
                        submitWriteStream.CloseInternal(true);
                        // disable write stream
                        submitWriteStream = null;
                    }
                    _OldSubmitWriteStream = null;
                }

                // callback processing - notify our caller that we're done
                InvokeGetRequestStreamCallback();
            }
            catch {
                // We depend on this to unblock possible response processing in case of unexpected failure
                ConnectStream chkStream = _SubmitWriteStream;
                if (chkStream != null)
                    chkStream.CallDone();

                //
                // Here we don't expect any exceptions and if got some from the user code then propagate it up.
                //
                throw;
            }

            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::EndWriteHeaders_Part2");
        }

        /*++

        Routine Description:

            Assembles the status line for an HTTP request
             specifically for CONNECT style verbs, that create a pipe

        Arguments:

            headersSize - size of the Header string that we send after
                this request line

        Return Value:

            int - number of bytes written out

        --*/
        private int GenerateConnectRequestLine(int headersSize) {
            int offset = 0;
            string host = ConnectHostAndPort;

            //
            // Handle Connect Case, i.e. "CONNECT hostname.domain.edu:999"
            //

            int writeBufferLength = CurrentMethod.Name.Length +
                                    host.Length +
                                    RequestLineConstantSize +
                                    headersSize;

            _WriteBuffer = new byte[writeBufferLength];
            offset = Encoding.ASCII.GetBytes(CurrentMethod.Name, 0, CurrentMethod.Name.Length, WriteBuffer, 0);
            WriteBuffer[offset++] = (byte)' ';
            offset += Encoding.ASCII.GetBytes(host, 0, host.Length, WriteBuffer, offset);
            WriteBuffer[offset++] = (byte)' ';
            return offset;
        }

        internal string ConnectHostAndPort {
            get {
                if (!IsTunnelRequest) {
                    return _Uri.GetParts(UriComponents.HostAndPort, UriFormat.UriEscaped);
                } else {
                    return _OriginUri.GetParts(UriComponents.HostAndPort, UriFormat.UriEscaped);
                }
            }
        }

        /*++

        Routine Description:

            Assembles the status line for an HTTP request
             specifically to a proxy...

        Arguments:

            headersSize - size of the Header string that we send after
                this request line

        Return Value:

            int - number of bytes written out

        --*/
        private int GenerateProxyRequestLine(int headersSize) {
            int offset = 0;

            //
            string requestUrl;

            if ((object)_Uri.Scheme == (object)Uri.UriSchemeFtp) {
                string userInfo = _Uri.GetParts(UriComponents.UserInfo
                                                | UriComponents.KeepDelimiter,
                                                UriFormat.UriEscaped);
                if (userInfo != "") {
                    requestUrl = _Uri.GetParts(UriComponents.HttpRequestUrl
                                               | UriComponents.UserInfo,
                                               UriFormat.UriEscaped);
                } else {
                    string username = null;
                    string password = null;
                    NetworkCredential networkCreds = Credentials.GetCredential(_Uri, "basic");
                    if (networkCreds != null
                        && (object)networkCreds
                           != (object)FtpWebRequest.DefaultNetworkCredential)
                    {
                        username = networkCreds.InternalGetDomainUserName();
                        password = networkCreds.InternalGetPassword();
                        password = (password == null) ? string.Empty : password;
                    }
                    if (username != null) {
                        string scheme = _Uri.GetParts(UriComponents.Scheme
                                                      | UriComponents.KeepDelimiter,
                                                      UriFormat.UriEscaped);
                        string urlMinusScheme = _Uri.GetParts(UriComponents.HttpRequestUrl
                                                              & ~UriComponents.Scheme,
                                                              UriFormat.UriEscaped);

                        // For FTP proxy we don't escape the username and password strings
                        // Since some servers don't seem to support it
                        // Only escape the absolute minimum that is required for
                        // a valid Uri which is (: \ / ? # %)
                        username = username.Replace(":",  "%3A");
                        password = password.Replace(":",  "%3A");
                        username = username.Replace("\\", "%5C");
                        password = password.Replace("\\", "%5C");
                        username = username.Replace("/",  "%2F");
                        password = password.Replace("/",  "%2F");
                        username = username.Replace("?",  "%3F");
                        password = password.Replace("?",  "%3F");
                        username = username.Replace("#",  "%23");
                        password = password.Replace("#",  "%23");
                        username = username.Replace("%",  "%25");
                        password = password.Replace("%",  "%25");
                        username = username.Replace("@",  "%40");
                        password = password.Replace("@",  "%40");
                        requestUrl = scheme+username+":"+password+"@"+urlMinusScheme;
                        requestUrl = new Uri(requestUrl).AbsoluteUri;
                    } else {
                        requestUrl = _Uri.GetParts(UriComponents.HttpRequestUrl,
                                                   UriFormat.UriEscaped);
                    }
                }
            }
            else {
                requestUrl = _Uri.GetParts(UriComponents.HttpRequestUrl,
                                           UriFormat.UriEscaped);
            }

            int writeBufferLength = CurrentMethod.Name.Length +
                                    requestUrl.Length +
                                    RequestLineConstantSize +
                                    headersSize;

            _WriteBuffer = new byte[writeBufferLength];
            offset = Encoding.ASCII.GetBytes(CurrentMethod.Name, 0, CurrentMethod.Name.Length, WriteBuffer, 0);
            WriteBuffer[offset++] = (byte)' ';
            offset += Encoding.ASCII.GetBytes(requestUrl, 0, requestUrl.Length, WriteBuffer, offset);
            WriteBuffer[offset++] = (byte)' ';
            return offset;
        }

        /*++

        Routine Description:

            Assembles the status/request line for the request.

        Arguments:

            headersSize - size of the Header string that we send after
                this request line

        Return Value:

            int - number of bytes written

        --*/
        private int GenerateRequestLine(int headersSize) {
            int offset = 0;
            string pathAndQuery = _Uri.PathAndQuery;

            int writeBufferLength =
                CurrentMethod.Name.Length +
                pathAndQuery.Length +
                RequestLineConstantSize +
                headersSize;

            _WriteBuffer = new byte[writeBufferLength];
            offset = Encoding.ASCII.GetBytes(CurrentMethod.Name, 0, CurrentMethod.Name.Length, WriteBuffer, 0);
            WriteBuffer[offset++] = (byte)' ';
            offset += Encoding.ASCII.GetBytes(pathAndQuery, 0, pathAndQuery.Length, WriteBuffer, offset);
            WriteBuffer[offset++] = (byte)' ';
            return offset;
        }

        /*++

        Routine Description:

            Assembles the data/headers for an HTTP request
             into a buffer

        Arguments:

            none.

        Return Value:

            none.

        --*/
        internal void UpdateHeaders() {
            GlobalLog.Enter("HttpWebRequest#" + ValidationHelper.HashString(this) + "::UpdateHeaders");
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::UpdateHeaders");

            // Set HostName Header
            _HttpRequestHeaders.ChangeInternal( HttpKnownHeaderNames.Host, (_Uri.IsDefaultPort ? _Uri.Host : ConnectHostAndPort));
            // about to create the headers we're going to send. Check if any
            // modules want to inspect or modify them
            if (_CookieContainer != null) {
                CookieModule.OnSendingHeaders(this);
            }
            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::UpdateHeaders");
        }

        //
        // The method is called right before sending headers to the wire
        // The result is updated internal _WriteBuffer
        //
        internal void SerializeHeaders() {
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::UpdateHeaders");

            //
            // If we have content-length, use it, if we don't check for chunked
            //  sending mode, otherwise, if -1, then see to closing the connection.
            // There's one extra case in which the user didn't set the ContentLength and is
            //  not chunking either. In this case we buffer the data, but to the time of that
            //  method call the content length is already known.
            //
            GlobalLog.Assert(HttpWriteMode!=HttpWriteMode.Unknown, "HttpWebRequest#{0}::SerializeHeaders()|HttpWriteMode:{1}", ValidationHelper.HashString(this), HttpWriteMode);
            if (HttpWriteMode!=HttpWriteMode.None) {
                if (HttpWriteMode==HttpWriteMode.Chunked) {
                    GlobalLog.Assert(!CurrentMethod.ContentBodyNotAllowed, "HttpWebRequest#{0}::SerializeHeaders()|!ContentBodyNotAllowed CurrentMethod:{1} HttpWriteMode:{2} ContentLength:{3}", ValidationHelper.HashString(this), CurrentMethod, HttpWriteMode, ContentLength);
                    _HttpRequestHeaders.AddInternal(HttpKnownHeaderNames.TransferEncoding, ChunkedHeader);
                }
                else if (ContentLength>=0) {
                    GlobalLog.Assert(HttpWriteMode == HttpWriteMode.ContentLength, "HttpWebRequest#{0}::SerializeHeaders()|HttpWriteMode:{1}", ValidationHelper.HashString(this), HttpWriteMode);
                    GlobalLog.Assert(!CurrentMethod.ContentBodyNotAllowed, "HttpWebRequest#{0}::SerializeHeaders()|!ContentBodyNotAllowed CurrentMethod:{1} HttpWriteMode:{2} ContentLength:{3}", ValidationHelper.HashString(this), CurrentMethod, HttpWriteMode, ContentLength);
                    _HttpRequestHeaders.ChangeInternal(HttpKnownHeaderNames.ContentLength, _ContentLength.ToString(NumberFormatInfo.InvariantInfo));
                }
                ExpectContinue = ExpectContinue && !IsVersionHttp10 && ServicePoint.Expect100Continue;
                if ((ContentLength>0 || HttpWriteMode==HttpWriteMode.Chunked) && ExpectContinue) {
                    _HttpRequestHeaders.AddInternal(HttpKnownHeaderNames.Expect, ContinueHeader);
                }
            }

            if((AutomaticDecompression & DecompressionMethods.GZip) != 0){

                if((AutomaticDecompression & DecompressionMethods.Deflate) != 0){
                    _HttpRequestHeaders.AddInternal(HttpKnownHeaderNames.AcceptEncoding, GZipHeader + ", " + DeflateHeader);
                }
                else{
                    _HttpRequestHeaders.AddInternal(HttpKnownHeaderNames.AcceptEncoding, GZipHeader);
                }
            }
            else if((AutomaticDecompression & DecompressionMethods.Deflate) != 0){
                _HttpRequestHeaders.AddInternal(HttpKnownHeaderNames.AcceptEncoding, DeflateHeader);
            }

            //
            // Behavior from Wininet, on Uris with Proxies, send Proxy-Connection: instead
            //  of Connection:
            //
            string connectionString = HttpKnownHeaderNames.Connection;
            if (UsesProxySemantics) {
                _HttpRequestHeaders.RemoveInternal(HttpKnownHeaderNames.Connection);
                connectionString = HttpKnownHeaderNames.ProxyConnection;
                if (!ValidationHelper.IsBlankString(Connection)) {
                    _HttpRequestHeaders.AddInternal(HttpKnownHeaderNames.ProxyConnection, _HttpRequestHeaders[HttpKnownHeaderNames.Connection]);
                }
            }
            else {
                _HttpRequestHeaders.RemoveInternal(HttpKnownHeaderNames.ProxyConnection);
            }

            if (KeepAlive || NtlmKeepAlive) {
                GlobalLog.Assert(_ServicePoint != null, "HttpWebRequest#{0}::SerializeHeaders()|_ServicePoint == null", ValidationHelper.HashString(this));
                if (IsVersionHttp10 || ServicePoint.HttpBehaviour<=HttpBehaviour.HTTP10) {
                    _HttpRequestHeaders.AddInternal((UsesProxySemantics? HttpKnownHeaderNames.ProxyConnection : HttpKnownHeaderNames.Connection), "Keep-Alive");
                }
            }
            else if (!IsVersionHttp10) {
                _HttpRequestHeaders.AddInternal(connectionString, "Close");
            }


            //
            // Now create our headers by calling ToString, and then
            //   create a HTTP Request Line to go with it.
            //

            int offset;
            string requestHeadersString = _HttpRequestHeaders.ToString();
            int requestHeadersSize = WebHeaderCollection.HeaderEncoding.GetByteCount(requestHeadersString);

            // NOTE: Perhaps we should cache this on this-object in the future?
            if (CurrentMethod.ConnectRequest) {
                // for connect verbs we need to specially handle it.
                offset = GenerateConnectRequestLine(requestHeadersSize);
            }
            else if (UsesProxySemantics) {
                // depending on whether, we have a proxy, generate a proxy or normal request
                offset = GenerateProxyRequestLine(requestHeadersSize);
            }
            else {
                // default case for normal HTTP requests
                offset = GenerateRequestLine(requestHeadersSize);
            }

            Buffer.BlockCopy(HttpBytes, 0, WriteBuffer, offset, HttpBytes.Length);
            offset += HttpBytes.Length;

            WriteBuffer[offset++] = (byte)'1';
            WriteBuffer[offset++] = (byte)'.';
            WriteBuffer[offset++] = IsVersionHttp10 ? (byte)'0' : (byte)'1';
            WriteBuffer[offset++] = (byte)'\r';
            WriteBuffer[offset++] = (byte)'\n';
            if (Logging.On) Logging.PrintInfo(Logging.Web, this, "Request: "+Encoding.ASCII.GetString(WriteBuffer, 0, offset));
            //
            // Serialze the headers out to the byte Buffer,
            //   by converting them to bytes from UNICODE
            //
            WebHeaderCollection.HeaderEncoding.GetBytes(requestHeadersString, 0, requestHeadersString.Length, WriteBuffer, offset);
            GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::SerializeHeaders(), bytesCount = " + offset.ToString());
        }


        //
        // PERF:
        // removed some double initializations.
        // perf went from:
        // clocks per instruction CPI: 9,098.72 to 1,301.14
        // %app exclusive time: 2.92 to 0.43
        //
        /// <devdoc>
        ///    <para>
        ///       Basic Constructor for HTTP Protocol Class, Initializes to basic header state.
        ///    </para>
        /// </devdoc>
        internal HttpWebRequest(Uri uri, ServicePoint servicePoint) {
            if(Logging.On)Logging.Enter(Logging.Web, this, "HttpWebRequest", uri);

            (new WebPermission(NetworkAccess.Connect, uri)).Demand();

            // OOPS, This ctor can also be called with FTP scheme but then it should only allowed if going through the proxy
            // Something to think about...
            //if ((object)uri.Scheme != (object)Uri.UriSchemeHttp && (object)uri.Scheme != (object)Uri.UriSchemeHttps)
                //throw new ArgumentOutOfRangeException("uri");

            GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::.ctor(" + uri.ToString() + ")");
            //
            // internal constructor, HttpWebRequest cannot be created directly
            // but only through WebRequest.Create() method
            // set defaults
            //
            _HttpRequestHeaders         = new WebHeaderCollection(WebHeaderCollectionType.HttpWebRequest);
            _Proxy                      = WebRequest.InternalDefaultWebProxy;
            _HttpWriteMode              = HttpWriteMode.Unknown;
            _MaximumAllowedRedirections = 50;
            _Timeout                    = WebRequest.DefaultTimeout;
            _TimerQueue                 = WebRequest.DefaultTimerQueue;
            _ReadWriteTimeout           = DefaultReadWriteTimeout;
            _MaximumResponseHeadersLength = DefaultMaximumResponseHeadersLength;
            _ContentLength              = -1;
            _OriginVerb                 = KnownHttpVerb.Get;
            _OriginUri                  = uri;
            _Uri                        = _OriginUri;
            _ServicePoint               = servicePoint;
            _RequestIsAsync             = TriState.Unspecified;

            SetupCacheProtocol(_OriginUri);


            if(Logging.On)Logging.Exit(Logging.Web, this, "HttpWebRequest", null);
        }

        internal HttpWebRequest(Uri proxyUri, Uri requestUri, HttpWebRequest orginalRequest) : this(proxyUri, null) {

            GlobalLog.Enter("HttpWebRequest::HttpWebRequest",
                            "proxyUri="+proxyUri+", requestUri="+requestUri
                            );

            _OriginVerb = KnownHttpVerb.Parse("CONNECT");

            //
            // CONNECT requests cannot be pipelined
            //

            Pipelined = false;

            //
            // each CONNECT request has a unique connection group name to avoid
            // non-CONNECT requests being made over the same connection
            //

            _OriginUri = requestUri;
            IsTunnelRequest = true;

            _ConnectionGroupName = ServicePointManager.SpecialConnectGroupName + "(" + UniqueGroupId + ")";
            m_InternalConnectionGroup = true;

            //
            // the CONNECT request must respond to a 407 as if it were a 401.
            // So we set up the server authentication state as if for a proxy
            //
            ServerAuthenticationState = new AuthenticationState(true);

            // CONNECT request is not suitable for caching
            CacheProtocol = null;

            GlobalLog.Leave("HttpWebRequest::HttpWebRequest");
        }

        /// <devdoc>
        ///    <para>ISerializable constructor</para>
        /// </devdoc>
        [Obsolete("Serialization is obsoleted for this type.  http://go.microsoft.com/fwlink/?linkid=14202")]
        [SecurityPermissionAttribute(SecurityAction.Demand, SerializationFormatter =true)]
        /*private*/ protected HttpWebRequest(SerializationInfo serializationInfo, StreamingContext streamingContext):base(serializationInfo, streamingContext) {
#if DEBUG
            using (GlobalLog.SetThreadKind(ThreadKinds.User)) {
#endif
            ExceptionHelper.WebPermissionUnrestricted.Demand();
            if(Logging.On)Logging.Enter(Logging.Web, this, "HttpWebRequest", serializationInfo);

            _HttpRequestHeaders         = (WebHeaderCollection)serializationInfo.GetValue("_HttpRequestHeaders", typeof(WebHeaderCollection));
            _Proxy                      = (IWebProxy)serializationInfo.GetValue("_Proxy", typeof(IWebProxy));
            KeepAlive                   = serializationInfo.GetBoolean("_KeepAlive");
            Pipelined                   = serializationInfo.GetBoolean("_Pipelined");
            AllowAutoRedirect           = serializationInfo.GetBoolean("_AllowAutoRedirect");
            AllowWriteStreamBuffering   = serializationInfo.GetBoolean("_AllowWriteStreamBuffering");
            HttpWriteMode               = (HttpWriteMode)serializationInfo.GetInt32("_HttpWriteMode");
            _MaximumAllowedRedirections = serializationInfo.GetInt32("_MaximumAllowedRedirections");
            _AutoRedirects              = serializationInfo.GetInt32("_AutoRedirects");
            _Timeout                    = serializationInfo.GetInt32("_Timeout");
            try {
                _ReadWriteTimeout       = serializationInfo.GetInt32("_ReadWriteTimeout");
            }
            catch {
                _ReadWriteTimeout       = DefaultReadWriteTimeout;
            }
            try {
                _MaximumResponseHeadersLength = serializationInfo.GetInt32("_MaximumResponseHeadersLength");
            }
            catch {
                _MaximumResponseHeadersLength = DefaultMaximumResponseHeadersLength;
            }
            _ContentLength              = serializationInfo.GetInt64("_ContentLength");
            _MediaType                  = serializationInfo.GetString("_MediaType");
            _OriginVerb                 = KnownHttpVerb.Parse(serializationInfo.GetString("_OriginVerb"));
            _ConnectionGroupName        = serializationInfo.GetString("_ConnectionGroupName");
            ProtocolVersion             = (Version)serializationInfo.GetValue("_Version", typeof(Version));
            _OriginUri                  = (Uri)serializationInfo.GetValue("_OriginUri", typeof(Uri));

            SetupCacheProtocol(_OriginUri);
            if(Logging.On)Logging.Exit(Logging.Web, this, "HttpWebRequest", null);
#if DEBUG
            }
#endif
        }

        /// <devdoc>
        ///    <para>ISerializable method</para>
        /// </devdoc>
        [SecurityPermission(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter, SerializationFormatter=true)]
        void ISerializable.GetObjectData(SerializationInfo serializationInfo, StreamingContext streamingContext)
        {
#if DEBUG
            using (GlobalLog.SetThreadKind(ThreadKinds.User)) {
#endif
            GetObjectData(serializationInfo, streamingContext);
#if DEBUG
            }
#endif
        }

        //
        // FxCop: Need this in addition to the above in order to allow derived classes to access the base implementation.
        //
        [SecurityPermissionAttribute(SecurityAction.Demand, SerializationFormatter =true)]
        protected override void GetObjectData(SerializationInfo serializationInfo, StreamingContext streamingContext)
        {
#if DEBUG
            using (GlobalLog.SetThreadKind(ThreadKinds.User)) {
#endif
            //
            // for now disregard streamingContext.
            // just Add all the members we need to deserialize to construct
            // the object at deserialization time
            //
            // the following runtime types already support serialization:
            // Boolean, Char, SByte, Byte, Int16, UInt16, Int32, UInt32, Int64, UInt64, Single, Double, DateTime
            // for the others we need to provide our own serialization
            //
            serializationInfo.AddValue("_HttpRequestHeaders", _HttpRequestHeaders, typeof(WebHeaderCollection));
            serializationInfo.AddValue("_Proxy", _Proxy, typeof(IWebProxy));
            serializationInfo.AddValue("_KeepAlive", KeepAlive);
            serializationInfo.AddValue("_Pipelined", Pipelined);
            serializationInfo.AddValue("_AllowAutoRedirect", AllowAutoRedirect);
            serializationInfo.AddValue("_AllowWriteStreamBuffering", AllowWriteStreamBuffering);
            serializationInfo.AddValue("_HttpWriteMode", HttpWriteMode);
            serializationInfo.AddValue("_MaximumAllowedRedirections", _MaximumAllowedRedirections);
            serializationInfo.AddValue("_AutoRedirects", _AutoRedirects);
            serializationInfo.AddValue("_Timeout", _Timeout);
            serializationInfo.AddValue("_ReadWriteTimeout", _ReadWriteTimeout);
            serializationInfo.AddValue("_MaximumResponseHeadersLength", _MaximumResponseHeadersLength);
            serializationInfo.AddValue("_ContentLength", ContentLength);
            serializationInfo.AddValue("_MediaType", _MediaType);
            serializationInfo.AddValue("_OriginVerb", _OriginVerb);
            serializationInfo.AddValue("_ConnectionGroupName", _ConnectionGroupName);
            serializationInfo.AddValue("_Version", ProtocolVersion, typeof(Version));
            serializationInfo.AddValue("_OriginUri", _OriginUri, typeof(Uri));
            base.GetObjectData(serializationInfo, streamingContext);
#if DEBUG
            }
#endif
        }

        /// <devdoc>
        ///    <para>Used by ServicePoint code to find the right connection Group</para>
        /// </devdoc>
        internal static StringBuilder GenerateConnectionGroup(string connectionGroupName, bool unsafeConnectionGroup, bool isInternalGroup) {
            StringBuilder connectionLine = new StringBuilder(connectionGroupName);

            connectionLine.Append(unsafeConnectionGroup ? "U>" : "S>");

            if (isInternalGroup)
            {
                connectionLine.Append("I>");
            }

            return connectionLine;
        }


        /// <devdoc>
        ///    <para>Generates a string that
        ///     allows a Connection to remain unique for a given NTLM auth
        ///     user, this is needed to prevent multiple users from
        ///     using the same sockets after they are authenticated.</para>
        /// </devdoc>
        internal string GetConnectionGroupLine() {
            GlobalLog.Enter("HttpWebRequest#" + ValidationHelper.HashString(this) + "::GetConnectionGroupLine");
            StringBuilder connectionLine = GenerateConnectionGroup(_ConnectionGroupName, UnsafeAuthenticatedConnectionSharing, m_InternalConnectionGroup);


            //

            if ((_Uri.Scheme == Uri.UriSchemeHttps) || IsTunnelRequest) {
                if (UsesProxy) {
                    connectionLine.Append(ConnectHostAndPort);
                    connectionLine.Append("$");
                }
            }
            if (ProxyAuthenticationState.UniqueGroupId != null) {
                connectionLine.Append(ProxyAuthenticationState.UniqueGroupId);
            }
            else if (ServerAuthenticationState.UniqueGroupId != null) {
                connectionLine.Append(ServerAuthenticationState.UniqueGroupId);
            }

            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::GetConnectionGroupLine", connectionLine.ToString());
            return connectionLine.ToString();
        }


        /// <devdoc>
        ///    <para>CheckResubmitForAuth - Determines if a HTTP request needs to be
        ///     resubmitted due to HTTP authenication
        ///
        ///     true  - if we should reattempt submitting the request
        ///     false - if the request is complete
        ///     </para>
        /// </devdoc>
        private bool CheckResubmitForAuth() {
            GlobalLog.Enter("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckResubmitForAuth");
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckResubmitForAuth");


            bool result = false;
            bool authenticated = false;
            bool skip = false;
            if (UsesProxySemantics && _Proxy != null && _Proxy.Credentials!=null) {
                try
                {
                    result |= ProxyAuthenticationState.AttemptAuthenticate(this, _Proxy.Credentials);
                }
                catch (Win32Exception)
                {
                    if (!m_Extra401Retry)
                    {
                        throw;
                    }
                    skip = true;
                }
                authenticated = true;
                GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckResubmitForAuth() ProxyAuthenticationState.AttemptAuthenticate() returns result:" + result.ToString());
            }
            if (Credentials != null && !skip)
            {
                try
                {
                    result |= ServerAuthenticationState.AttemptAuthenticate(this, Credentials);
                }
                catch (Win32Exception)
                {
                    if (!m_Extra401Retry)
                    {
                        throw;
                    }
                    result = false;
                }
                authenticated = true;
                GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckResubmitForAuth() ServerAuthenticationState.AttemptAuthenticate() returns result:" + result.ToString());
            }
            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckResubmitForAuth", result);

            if (!result && authenticated && m_Extra401Retry)
            {
                ClearAuthenticatedConnectionResources();
                m_Extra401Retry = false;
                result = true;
            }

            return result;
        }

        /// <devdoc>
        ///    <para>Determines whether we need to resubmit the request due to the cache settings
        ///     here it is imperitive that this is not recalled when we already are receiving a cached
        ///     response</para>
        /// </devdoc>
        private bool CheckResubmitForCache(ref Exception e) {
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckResubmitForCache");

            // Here, we can go into retrying the request
            if (!CheckCacheRetrieveOnResponse()) {

                // NB: We don't have a flag that would control a generic retries
                // So far we used _AllowAutoRedirect when doing Auth resubmits so the cache intiated retries are put into same bucket
                if (AllowAutoRedirect) {
                    // Resubmit the request
                    if(Logging.On)Logging.PrintWarning(Logging.Web, this, "", SR.GetString(SR.net_log_cache_validation_failed_resubmit));
                    return true;
                }

                //fail terribly
                if(Logging.On)Logging.PrintError(Logging.Web, this, "", SR.GetString(SR.net_log_cache_refused_server_response));
                e = new InvalidOperationException(SR.GetString(SR.net_cache_not_accept_response));
                return false;
            }

            // This may change _HttpResponse memeber based on the CacheProtocol feedback.
            CheckCacheUpdateOnResponse();
            return false;
        }

        /// <devdoc>
        ///    <para>Determines if a HTTP request needs to be
        ///     resubmitted to a server point, this is called in Response
        ///     Parsing to handle cases such as server Redirects and Authentications</para>
        ///
        ///     true  - if we should reattempt submitting the request
        ///     false - if the request is complete
        /// </devdoc>
        private bool CheckResubmit(ref Exception e) {
            GlobalLog.Enter("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckResubmit");
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckResubmit");

            bool authResubmit = false;

            if (ResponseStatusCode==HttpStatusCode.Unauthorized                 || // 401
                ResponseStatusCode==HttpStatusCode.ProxyAuthenticationRequired)    // 407
            {
                try {
                    //
                    // Check for Authentication
                    //
                    if (!(authResubmit = CheckResubmitForAuth()))
                    {
                        e = new WebException(SR.GetString(SR.net_servererror,
                                            NetRes.GetWebStatusCodeString(ResponseStatusCode, _HttpResponse.StatusDescription)),
                                            null,
                                            WebExceptionStatus.ProtocolError,
                                            _HttpResponse);
                        GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckResubmit", "false");
                        return false;
                    }
                }
                catch (System.ComponentModel.Win32Exception w32Exception) {
                    throw new WebException(
                                            SR.GetString(SR.net_servererror,
                                                NetRes.GetWebStatusCodeString(ResponseStatusCode, _HttpResponse.StatusDescription)),
                                            w32Exception,
                                            WebExceptionStatus.ProtocolError,
                                            _HttpResponse);

                }
            }
            else {
                if (ServerAuthenticationState != null && ServerAuthenticationState.Authorization != null)
                {
                    HttpWebResponse response = _HttpResponse;
                    if (response != null)
                    {
                        response.InternalSetIsMutuallyAuthenticated = ServerAuthenticationState.Authorization.MutuallyAuthenticated;
                    }
                }

                // This is a workaround for resubmitting a failed chunked POST throw the proxy that does not support chunked POST
                if (ResponseStatusCode==HttpStatusCode.BadRequest && this.SendChunked && this.ServicePoint.InternalProxyServicePoint)
                {
                    ClearAuthenticatedConnectionResources();
                    return true;
                }


                //
                // Check for Redirection
                //
                // Table View:
                // Method            301             302             303             307
                //    *                *               *             GET               *
                // POST              GET             GET             GET            POST
                //
                // Put another way:
                //  301 & 302  - All methods are redirected to the same method but POST. POST is redirected to a GET.
                //  303 - All methods are redirected to GET
                //  307 - All methods are redirected to the same method.
                //
                else if (AllowAutoRedirect && (
                    ResponseStatusCode==HttpStatusCode.Ambiguous          || // 300
                    ResponseStatusCode==HttpStatusCode.Moved              || // 301
                    ResponseStatusCode==HttpStatusCode.Redirect           || // 302
                    ResponseStatusCode==HttpStatusCode.RedirectMethod     || // 303
                    ResponseStatusCode==HttpStatusCode.RedirectKeepVerb ))   // 307
                {

                    _AutoRedirects++;

                    if (_AutoRedirects>_MaximumAllowedRedirections)
                    {

                        e = new WebException(SR.GetString(SR.net_tooManyRedirections),
                                             null,
                                             WebExceptionStatus.ProtocolError,
                                             _HttpResponse);
                        GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckResubmit", "false");
                        return false;
                    }

                    string location = _HttpResponse.Headers.Location;
                    if (location==null)
                    {
                        e = new WebException(SR.GetString(SR.net_servererror,
                                            NetRes.GetWebStatusCodeString(ResponseStatusCode, _HttpResponse.StatusDescription)),
                                            null,
                                            WebExceptionStatus.ProtocolError,
                                            _HttpResponse);
                        GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckResubmit", "false");
                        return false;
                    }
                    GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckResubmit() Location:" + location);
                    Uri newUri;
                    try {
                        newUri = new Uri(_Uri, location);
                    }
                    catch (UriFormatException exception) {
                        e = new WebException(SR.GetString(SR.net_resubmitprotofailed), exception, WebExceptionStatus.ProtocolError, _HttpResponse);
                        GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckResubmit", "false");
                        return false;
                    }
                    if (newUri.Scheme != Uri.UriSchemeHttp && newUri.Scheme != Uri.UriSchemeHttps)
                    {
                        e = new WebException(SR.GetString(SR.net_resubmitprotofailed), null, WebExceptionStatus.ProtocolError, _HttpResponse);
                        GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckResubmit", "false");
                        return false;
                    }
                    try {
                        // Check for permissions against redirect Uri
                        ExecutionContext x = null;
                        CodeAccessPermission permission = (new WebPermission(NetworkAccess.Connect, newUri));
                        if (x == null)
                        {
                            permission.Demand();
                        }
                        else
                        {
                            ExecutionContext.Run(x, NclUtilities.ContextRelativeDemandCallback, permission);
                        }
                    }
                    catch (SecurityException exception) {
                        e = new SecurityException(SR.GetString(SR.net_redirect_perm),
                                                new WebException(SR.GetString(SR.net_resubmitcanceled), exception, WebExceptionStatus.ProtocolError, _HttpResponse));

                        GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckResubmit", "false");
                        return false;
                    }
                    
                    _Uri = newUri;

                    bool disableUpload = false;
                    if (ResponseStatusCode>MaxOkStatus) {
                        if(Logging.On)Logging.PrintWarning(Logging.Web, this, "", SR.GetString(SR.net_log_server_response_error_code, ((int)ResponseStatusCode).ToString(NumberFormatInfo.InvariantInfo)));
                    }

                    switch (ResponseStatusCode) {
                        case HttpStatusCode.Moved:
                        case HttpStatusCode.Redirect:
                            if (CurrentMethod.Equals(KnownHttpVerb.Post)) {
                                disableUpload = true;
                            }
                            break;
                        case HttpStatusCode.RedirectKeepVerb:
                            break;
                        default:
                            disableUpload = true;
                            break;
                    }

                    // set new Method
                    if (disableUpload) {
                        GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckResubmit() disabling upload HttpWriteMode:" + HttpWriteMode + " SubmitWriteStream#" + ValidationHelper.HashString(_SubmitWriteStream));
                        GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckResubmit() changing Verb from " + CurrentMethod + " to " + KnownHttpVerb.Get);
                        CurrentMethod = KnownHttpVerb.Get;
                        ExpectContinue = false;
                        HttpWriteMode = HttpWriteMode.None;
                    }

                    //
                    // make sure we're not sending over credential information to an evil redirection
                    // URI. this will set our credentials to null unless the user is using DefaultCredentials
                    // or a CredentialCache, in which case he is responsible for binding the credentials to
                    // the proper Uri and AuthenticationScheme.
                    //

                    Credentials = null;

                    //
                    // do the necessary cleanup on the Headers involved in the
                    // Authentication handshake.
                    //
                    ProxyAuthenticationState.ClearAuthReq(this);
                    ServerAuthenticationState.ClearAuthReq(this);

                    //strip referer if coming from an https site
                    if(_OriginUri.Scheme == Uri.UriSchemeHttps) {
                        _HttpRequestHeaders.RemoveInternal(HttpKnownHeaderNames.Referer);
                    }

                    // resubmit
                    GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckResubmit() CurrentMethod=" + CurrentMethod);
                }
                else if (ResponseStatusCode > MaxRedirectionStatus)          // > 399 = fatal
                {
                    e = new WebException(SR.GetString(SR.net_servererror,
                                        NetRes.GetWebStatusCodeString(ResponseStatusCode, _HttpResponse.StatusDescription)),
                                        null,
                                        WebExceptionStatus.ProtocolError,
                                        _HttpResponse);
                    GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckResubmit", "false");
                    return false;
                }
                else if (AllowAutoRedirect && ResponseStatusCode > MaxOkStatus) //some of 3XX and AllowAutoRedirect==true will result into an exceptional respone
                {
                    e = new WebException(SR.GetString(SR.net_servererror,
                                        NetRes.GetWebStatusCodeString(ResponseStatusCode, _HttpResponse.StatusDescription)),
                                        null,
                                        WebExceptionStatus.ProtocolError,
                                        _HttpResponse);
                    GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckResubmit", "false");
                    return false;
                }
                else // SUCCESS Status is <= 299 or (<=399 && AllowAutoRedirect==false) will result into a normal (non exceptional) response
                {
                    GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckResubmit", "false");
                    return false;
                }
            }

            GlobalLog.Assert(HttpWriteMode != HttpWriteMode.Unknown, "HttpWebRequest#{0}::CheckResubmit()|HttpWriteMode:{1}", ValidationHelper.HashString(this), HttpWriteMode);
            if (HttpWriteMode!=HttpWriteMode.None && !AllowWriteStreamBuffering && (HttpWriteMode!=HttpWriteMode.ContentLength || ContentLength!=0))
            {
                e = new WebException(SR.GetString(SR.net_need_writebuffering), null, WebExceptionStatus.ProtocolError, _HttpResponse);
                GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckResubmit", "false");
                return false;
            }

            if (!authResubmit){
                ClearAuthenticatedConnectionResources();
            }
            
            if(Logging.On)Logging.PrintWarning(Logging.Web, this, "", SR.GetString(SR.net_log_resubmitting_request));
            GlobalLog.Leave("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckResubmit", "true");
            return true;
        }

        /*++

        Routine Description:

            ClearRequestForResubmit - prepares object for resubmission and recall
                of submit request, for redirects, authentication, and other uses
                This is needed to prevent duplicate headers being added, and other
                such things.

        Arguments:

            None.

        Return Value:

            None.

        --*/
        private void ClearRequestForResubmit() {
            GlobalLog.ThreadContract(ThreadKinds.Unknown, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::ClearRequestForResubmit");

            _HttpRequestHeaders.RemoveInternal(HttpKnownHeaderNames.Host);
            _HttpRequestHeaders.RemoveInternal(HttpKnownHeaderNames.Connection);
            _HttpRequestHeaders.RemoveInternal(HttpKnownHeaderNames.ProxyConnection);
            _HttpRequestHeaders.RemoveInternal(HttpKnownHeaderNames.ContentLength);
            _HttpRequestHeaders.RemoveInternal(HttpKnownHeaderNames.TransferEncoding);
            _HttpRequestHeaders.RemoveInternal(HttpKnownHeaderNames.Expect);

            if (_HttpResponse != null && _HttpResponse.ResponseStream != null) {
                //
                // We just drain the response data, and throw them away since we're redirecting or authenticating.
                //
                GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::ClearRequestForResubmit() draining ResponseStream");
                if (!_HttpResponse.KeepAlive) {
                    ConnectStream liveStream = _HttpResponse.ResponseStream as ConnectStream;
                    if (liveStream != null ) {
                        // The response stream may be closed at any time by the server.
                        // At this point we don't want to drain such a ConnectStream.
                        liveStream.ErrorResponseNotify(false);
                    }
                }
                ICloseEx icloseEx = _HttpResponse.ResponseStream as ICloseEx;
                if (icloseEx != null) {
                    icloseEx.CloseEx(CloseExState.Silent);
                }
                else {
                    _HttpResponse.ResponseStream.Close();
                }
            }

            _AbortDelegate = null;

            if (_SubmitWriteStream != null) {
                //
                // We're uploading and need to resubmit for Authentication or Redirect.
                // if the response wants to keep alive the connection we shouldn't be closing
                // it (this would also brake connection-oriented authentication schemes such as NTLM).
                // so we need to flush all the data to the wire.
                // if the server is closing the connection, instead, we can just close our side as well.
                if ((_HttpResponse != null && _HttpResponse.KeepAlive) || _SubmitWriteStream.IgnoreSocketErrors)
                {
                    //
                    // the server wants to keep the connection alive.
                    // if we're uploading data, we need to make sure that we upload all
                    // of it before we start resubmitting.
                    // give the stream to the user if he didn't get it yet.
                    // if the user has set ContentLength to a big number, then we might be able
                    // to just decide to close the connection, but we need to be careful to NTLM.
                    //
                    GlobalLog.Assert(HttpWriteMode != HttpWriteMode.Unknown, "HttpWebRequest#{0}::ClearRequestForResubmit()|HttpWriteMode:{1}", ValidationHelper.HashString(this), HttpWriteMode);
                    if (HasEntityBody)
                    {
                        //
                        // we're uploading
                        //
                        GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::ClearRequestForResubmit() _WriteAResult:" + ValidationHelper.ToString(_WriteAResult));

                        //
                        // the user didn't get the stream yet, give it to him
                        //
                        GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::ClearRequestForResubmit() calling SetRequestContinue()");
                        SetRequestContinue();

                        // We should only close the stream if we're not giving it back to the app.
                        if (!Async && UserRetrievedWriteStream)
                            _SubmitWriteStream.CallDone();
                    }
                }

                if ((Async || UserRetrievedWriteStream) && _OldSubmitWriteStream != null && _OldSubmitWriteStream != _SubmitWriteStream) {
                    GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::ClearRequestForResubmit() closing RequestStream");
                    _SubmitWriteStream.CloseInternal(true);
                }
            }

            m_ContinueGate.Reset();
            _RerequestCount += 1;

            m_BodyStarted = false;
            HeadersCompleted = false;
            _WriteBuffer  = null;
            m_Extra401Retry = false;

            _HttpResponse = null;

            if (!Aborted && Async)
                _CoreResponse = null;
        }

        //
        //  Oprionally:
        //  1) copies off the response stream
        //  2) closes SubmitStream
        //  3) updates ServicePoint 100-Continue expectation for future requests
        //
        //  Should never throw
        //
        private void FinishRequest(HttpWebResponse response, Exception errorException)
        {

            GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::FinishRequest()");

            if (!_ReadAResult.InternalPeekCompleted && m_Aborted != AbortState.Public) // otherwise it's too late
            {
                if (response != null && errorException != null)
                {
                    // if this is a protocol exception copy off stream, don't just close it
                    GlobalLog.Print("HttpWebRequest#" + ValidationHelper.HashString(this) + "::CheckFinalStatus() - status" + (int)ResponseStatusCode);
                    response.ResponseStream = MakeMemoryStream(response.ResponseStream); // Never throws
                }
            }

            if (errorException != null && _SubmitWriteStream != null && !_SubmitWriteStream.IsClosed)
            {
                _SubmitWriteStream.ErrorResponseNotify(false);
            }

            //
            // turn off expectation of 100 continue (we'll keep sending an
            // "Expect: 100-continue" header to a 1.1 server though)
            //
            if ( errorException == null && _HttpResponse != null &&
                 ( _HttpWriteMode == HttpWriteMode.Chunked || _ContentLength > 0 ) &&
                 ExpectContinue && !Saw100Continue  &&
                 _ServicePoint.Understands100Continue &&
                 !IsTunnelRequest &&
                 ResponseStatusCode <= MaxOkStatus)
            {
                _ServicePoint.Understands100Continue = false;
            }
        }

        //
        // Never throws
        //
        private Stream MakeMemoryStream(Stream stream) {
           // GlobalLog.ThreadContract(ThreadKinds.Sync, "HttpWebRequest#" + ValidationHelper.HashString(this) + "::MakeMemoryStream");

            if (stream == null || stream is SyncMemoryStream)
                return stream;

            SyncMemoryStream memoryStream = new SyncMemoryStream(0);      // buffered Stream to save off data
            try {
                //
                // Now drain the Stream
                //
                if (stream.CanRead) {
                    byte [] buffer = new byte[1024];
                    int bytesTransferred = 0;

                    int maxBytesToBuffer = (HttpWebRequest.DefaultMaximumErrorResponseLength == -1)?buffer.Length:HttpWebRequest.DefaultMaximumErrorResponseLength*1024;
                    while ((bytesTransferred = stream.Read(buffer, 0, Math.Min(buffer.Length, maxBytesToBuffer))) > 0)
                    {
                        memoryStream.Write(buffer, 0, bytesTransferred);
                        if(HttpWebRequest.DefaultMaximumErrorResponseLength != -1)
                            maxBytesToBuffer -= bytesTransferred;
                    }
                }
                memoryStream.Position = 0;
            }
            catch {
            }
            finally
            {
                try {
                    ICloseEx icloseEx = stream as ICloseEx;
                    if (icloseEx != null) {
                        icloseEx.CloseEx(CloseExState.Silent);
                    }
                    else {
                        stream.Close();
                    }
                }
                catch {
                }
            }
            return memoryStream;
        }

        /// <devdoc>
        ///    <para>
        ///       Adds a range header to the request for a specified range.
        ///    </para>
        /// </devdoc>
        public void AddRange(int from, int to) {
            AddRange("bytes", from, to);
        }


        /// <devdoc>
        ///    <para>
        ///       Adds a range header to a request for a specific
        ///       range from the beginning or end
        ///       of the requested data.
        ///       To add the range from the end pass negative value
        ///       To add the range from the some offset to the end pass positive value
        ///    </para>
        /// </devdoc>
        public void AddRange(int range) {
            AddRange("bytes", range);
        }

        public void AddRange(string rangeSpecifier, int from, int to) {

            //
            // Do some range checking before assembling the header
            //

            if (rangeSpecifier == null) {
                throw new ArgumentNullException("rangeSpecifier");
            }
            if ((from < 0) || (to < 0)) {
                throw new ArgumentOutOfRangeException(SR.GetString(SR.net_rangetoosmall));
            }
            if (from > to) {
                throw new ArgumentOutOfRangeException(SR.GetString(SR.net_fromto));
            }
            if (!WebHeaderCollection.IsValidToken(rangeSpecifier)) {
                throw new ArgumentException(SR.GetString(SR.net_nottoken), "rangeSpecifier");
            }
            if (!AddRange(rangeSpecifier, from.ToString(NumberFormatInfo.InvariantInfo), to.ToString(NumberFormatInfo.InvariantInfo))) {
                throw new InvalidOperationException(SR.GetString(SR.net_rangetype));
            }
        }

        public void AddRange(string rangeSpecifier, int range) {
            if (rangeSpecifier == null) {
                throw new ArgumentNullException("rangeSpecifier");
            }
            if (!WebHeaderCollection.IsValidToken(rangeSpecifier)) {
                throw new ArgumentException(SR.GetString(SR.net_nottoken), "rangeSpecifier");
            }
            if (!AddRange(rangeSpecifier, range.ToString(NumberFormatInfo.InvariantInfo), (range >= 0) ? "" : null)) {
                throw new InvalidOperationException(SR.GetString(SR.net_rangetype));
            }
        }

        //
        // bool AddRange(rangeSpecifier, from, to)
        //
        //  Add or extend a range header. Various range types can be specified
        //  via rangeSpecifier, but only one type of Range request will be made
        //  e.g. a byte-range request, or a row-range request. Range types
        //  cannot be mixed
        //
        private bool AddRange(string rangeSpecifier, string from, string to) {

            string curRange = _HttpRequestHeaders[HttpKnownHeaderNames.Range];

            if ((curRange == null) || (curRange.Length == 0)) {
                curRange = rangeSpecifier + "=";
            }
            else {
                if (String.Compare(curRange.Substring(0, curRange.IndexOf('=')), rangeSpecifier, StringComparison.OrdinalIgnoreCase) != 0) {
                    return false;
                }
                curRange = string.Empty;
            }
            curRange += from.ToString();
            if (to != null) {
                curRange += "-" + to;
            }
            _HttpRequestHeaders.SetAddVerified(HttpKnownHeaderNames.Range, curRange);
            return true;
        }

        private static string UniqueGroupId {
            get {
                return (Interlocked.Increment(ref s_UniqueGroupId)).ToString(NumberFormatInfo.InvariantInfo);
            }
        }


    }
}
