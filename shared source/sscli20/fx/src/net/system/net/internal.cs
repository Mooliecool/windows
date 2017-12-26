//------------------------------------------------------------------------------
// <copyright file="Internal.cs" company="Microsoft">
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
    using System.IO;
    using System.Reflection;
    using System.Collections;
    using System.Collections.Generic;
    using System.Collections.Specialized;
    using System.Globalization;
    using System.Net.Sockets;
    using System.Runtime.InteropServices;
    using System.Runtime.Versioning;
    using System.Security.Cryptography.X509Certificates;
    using System.Text;
    using System.Text.RegularExpressions;
    using System.Security.Permissions;
    using System.Diagnostics;
    using System.Threading;
    using System.Security.Principal;
    using System.Security;
    using System.Net.Security;
    using System.Net.NetworkInformation;
    using System.Runtime.Serialization;

    internal static class IntPtrHelper {
        /*
        //                                    
        internal static IntPtr Add(IntPtr a, IntPtr b) {
            return (IntPtr) ((long)a + (long)b);
        }
        */
        internal static IntPtr Add(IntPtr a, int b) {
            return (IntPtr) ((long)a + (long)b);
        }
    }

    internal class InternalException : SystemException
    {
        internal InternalException()
        {
            GlobalLog.Assert("InternalException thrown.");
        }

        internal InternalException(SerializationInfo serializationInfo, StreamingContext streamingContext) :
            base(serializationInfo, streamingContext)
        { }
    }

    internal static class NclUtilities
    {
        /// <devdoc>
        ///    <para>
        ///       Indicates true if the threadpool is low on threads,
        ///       in this case we need to refuse to start new requests,
        ///       and avoid blocking.
        ///    </para>
        /// </devdoc>
        internal static bool IsThreadPoolLow()
        {

            int workerThreads, completionPortThreads;
            ThreadPool.GetAvailableThreads(out workerThreads, out completionPortThreads);

            return workerThreads < 2 || completionPortThreads < 2;
        }

        internal static bool HasShutdownStarted
        {
            get
            {
                return Environment.HasShutdownStarted || AppDomain.CurrentDomain.IsFinalizingForUnload();
            }
        }


        // ContextRelativeDemand
        // Allows easily demanding a permission against a given ExecutionContext.
        // Have requested the CLR to provide this method on ExecutionContext.
        private static ContextCallback s_ContextRelativeDemandCallback;

        internal static ContextCallback ContextRelativeDemandCallback
        {
            get
            {
                if (s_ContextRelativeDemandCallback == null)
                    s_ContextRelativeDemandCallback = new ContextCallback(DemandCallback);
                return s_ContextRelativeDemandCallback;
            }
        }

        private static void DemandCallback(object state)
        {
            ((CodeAccessPermission) state).Demand();
        }

        // This is for checking if a hostname probably refers to this machine without going to DNS.
        internal static bool GuessWhetherHostIsLoopback(string host)
        {
            string hostLower = host.ToLowerInvariant();
            if (hostLower == "localhost" || hostLower == "loopback")
            {
                return true;
            }

            return false;
        }

        internal static bool IsFatal(Exception exception)
        {
            return exception != null && (exception is OutOfMemoryException || exception is StackOverflowException || exception is ThreadAbortException);
        }

        // Need a fast cached list of local addresses for internal use.
        private static IPAddress[] _LocalAddresses;
        private static object _LocalAddressesLock;

        private const int HostNameBufferLength = 256;
        internal static string _LocalDomainName;

        // Copied from the old version of DNS.cs
        // Returns a list of our local addresses by calling gethostbyname with null.
        //
        private static IPHostEntry GetLocalHost()
        {
            //
            // IPv6 Changes: If IPv6 is enabled, we can't simply use the
            //               old IPv4 gethostbyname(null). Instead we need
            //               to do a more complete lookup.
            //
            if (Socket.SupportsIPv6)
            {
                //
                // IPv6 enabled: use getaddrinfo() of the local host name
                // to obtain this information. Need to get the machines
                // name as well - do that here so that we don't need to
                // Assert DNS permissions.
                //
                StringBuilder hostname = new StringBuilder(HostNameBufferLength);
                SocketError errorCode =
                    UnsafeNclNativeMethods.OSSOCK.gethostname(
                    hostname,
                    HostNameBufferLength);

                if (errorCode != SocketError.Success)
                {
                    throw new SocketException();
                }

                return Dns.GetHostByName(hostname.ToString());
            }
            else
            {
                //
                // IPv6 disabled: use gethostbyname() to obtain information.
                //
                IntPtr nativePointer =
                    UnsafeNclNativeMethods.OSSOCK.gethostbyname(
                    null);

                if (nativePointer == IntPtr.Zero)
                {
                    throw new SocketException();
                }

                return Dns.NativeToHostEntry(nativePointer);
            }

        } // GetLocalHost

        internal static IPAddress[] LocalAddresses
        {
            get
            {
                IPAddress[] local = _LocalAddresses;
                if (local != null)
                {
                    return local;
                }

                lock (LocalAddressesLock)
                {
                    local = _LocalAddresses;
                    if (local != null)
                    {
                        return local;
                    }

                    List<IPAddress> localList = new List<IPAddress>();

                        try
                        {
                            IPHostEntry hostEntry = GetLocalHost();
                            if (hostEntry != null)
                            {
                                if (hostEntry.HostName != null)
                                {
                                    int dot = hostEntry.HostName.IndexOf('.');
                                    if (dot != -1)
                                    {
                                        _LocalDomainName = hostEntry.HostName.Substring(dot);
                                    }
                                }

                                IPAddress[] ipAddresses = hostEntry.AddressList;
                                if (ipAddresses != null)
                                {
                                    foreach (IPAddress ipAddress in ipAddresses)
                                    {
                                        localList.Add(ipAddress);
                                    }
                                }
                            }
                        }
                        catch
                        {
                        }

                    local = new IPAddress[localList.Count];
                    int index = 0;
                    foreach (IPAddress ipAddress in localList)
                    {
                        local[index] = ipAddress;
                        index++;
                    }
                    _LocalAddresses = local;

                    return local;
                }
            }
        }

        internal static bool IsAddressLocal(IPAddress ipAddress) {
            IPAddress[] localAddresses = NclUtilities.LocalAddresses;
            for (int i = 0; i < localAddresses.Length; i++)
            {
                if (ipAddress.Equals(localAddresses[i], false))
                {
                    return true;
                }
            }
            return false;
        }

        private static object LocalAddressesLock
        {
            get
            {
                if (_LocalAddressesLock == null)
                {
                    Interlocked.CompareExchange(ref _LocalAddressesLock, new object(), null);
                }
                return _LocalAddressesLock;
            }
        }
    }

    internal static class NclConstants
    {
        internal static readonly object Sentinel = new object();
        internal static readonly object[] EmptyObjectArray = new object[0];
        internal static readonly Uri[] EmptyUriArray = new Uri[0];

        internal static readonly byte[] CRLF = new byte[] {(byte) '\r', (byte) '\n'};
        internal static readonly byte[] ChunkTerminator = new byte[] {(byte) '0', (byte) '\r', (byte) '\n', (byte) '\r', (byte) '\n'};
    }

    //
    // A simple sync point, useful for deferring work.  Just an int value with helper methods.
    //
    // The two events being synchronized are the "Triggering" event and the "Completing" event.  The Triggering event
    // marks the gate as being active; the first subsequent Completing event handles the action.
    //
    // First, a thread calls Trigger() to set the trigger on the gate.  This means it needs some work to be done, but only
    // after another operation (the Completing event) finishes.  If Trigger() returns false, the Completing event already
    // happened.  When the Completing event occurs, that thread calls Complete().  It returns true if the gate has been
    // previously triggered and the caller is the first one to complete it.  The caller should then handle the pending work item.
    //
    // StartTrigger()/FinishTrigger() can be used instead of Trigger() if the triggering thread needs to set up some state
    // (e.g. the pending work item).  It will block Complete() in a spin-lock.
    //
    internal struct InterlockedGate
    {
        private int m_State;

        internal const int Open = 0;        // Initial state of gate.
        internal const int Held = 1;        // Gate is being actively held by a thread - indeterminate state.
        internal const int Triggered = 2;   // The triggering event has occurred.
        internal const int Closed = 3;      // The gated event is done.

#if DEBUG
        /* Consider removing
        internal int State
        {
            get
            {
                return m_State;
            }
        }
        */
#endif

        // Only call when all threads are guaranteed to be done with the gate.
        internal void Reset()
        {
            m_State = Open;
        }

        // Returns false if the gate is already closed or triggered.  If exclusive is true, throws if the gate is already
        // triggered.
        internal bool Trigger(bool exclusive)
        {
            int gate = Interlocked.CompareExchange(ref m_State, Triggered, Open);
            if (exclusive && (gate == Held || gate == Triggered))
            {
                GlobalLog.Assert("InterlockedGate::Trigger", "Gate already triggered.");
                throw new InternalException();
            }
            return gate == Open;
        }

        // Use StartTrigger() and FinishTrigger() to trigger the gate as a two step operation.  This is useful to set up an invariant
        // that must be ready by the time another thread closes the gate.  Do not block between StartTrigger() and FinishTrigger(), just
        // set up your state to be consistent.  If this method returns true, FinishTrigger() *must* be called to avoid deadlock - do
        // it in a finally.
        //
        // Returns false if the gate is already closed or triggered.  If exclusive is true, throws if the gate is already
        // triggered.
        internal bool StartTrigger(bool exclusive)
        {
            int gate = Interlocked.CompareExchange(ref m_State, Held, Open);
            if (exclusive && (gate == Held || gate == Triggered))
            {
                GlobalLog.Assert("InterlockedGate::StartTrigger", "Gate already triggered.");
                throw new InternalException();
            }
            return gate == Open;
        }

        // Gate must be held by StartTrigger().
        internal void FinishTrigger()
        {
            int gate = Interlocked.CompareExchange(ref m_State, Triggered, Held);
            if (gate != Held)
            {
                GlobalLog.Assert("InterlockedGate::FinishTrigger", "Gate not held.");
                throw new InternalException();
            }
        }

        // Returns false if the gate had never been triggered or is already closed.
        internal bool Complete()
        {
            int gate;

            // Spin while the gate is being held, allowing the other thread to set invariants up.
            while ((gate = Interlocked.CompareExchange(ref m_State, Closed, Triggered)) != Triggered)
            {
                if (gate == Closed)
                {
                    return false;
                }

                if (gate == Open)
                {
                    if (Interlocked.CompareExchange(ref m_State, Closed, Open) == Open)
                    {
                        return false;
                    }

                    continue;
                }

                // gate == Held
                Thread.SpinWait(1);
            }

            return true;
        }
    }






    //
    // support class for Validation related stuff.
    //
    internal static class ValidationHelper {

        public static string [] EmptyArray = new string[0];

        internal static readonly char[]  InvalidMethodChars =
                new char[]{
                ' ',
                '\r',
                '\n',
                '\t'
                };

        // invalid characters that cannot be found in a valid method-verb or http header
        internal static readonly char[]  InvalidParamChars =
                new char[]{
                '(',
                ')',
                '<',
                '>',
                '@',
                ',',
                ';',
                ':',
                '\\',
                '"',
                '\'',
                '/',
                '[',
                ']',
                '?',
                '=',
                '{',
                '}',
                ' ',
                '\t',
                '\r',
                '\n'};

        public static string [] MakeEmptyArrayNull(string [] stringArray) {
            if ( stringArray == null || stringArray.Length == 0 ) {
                return null;
            } else {
                return stringArray;
            }
        }

        public static string MakeStringNull(string stringValue) {
            if ( stringValue == null || stringValue.Length == 0) {
                return null;
            } else {
                return stringValue;
            }
        }

        /*
        //                                    
        public static string MakeStringEmpty(string stringValue) {
            if ( stringValue == null || stringValue.Length == 0) {
                return String.Empty;
            } else {
                return stringValue;
            }
        }
        */


        public static string ExceptionMessage(Exception exception) {
            if (exception==null) {
                return string.Empty;
            }
            if (exception.InnerException==null) {
                return exception.Message;
            }
            return exception.Message + " (" + ExceptionMessage(exception.InnerException) + ")";
        }

        public static string ToString(object objectValue) {
            if (objectValue == null) {
                return "(null)";
            } else if (objectValue is string && ((string)objectValue).Length==0) {
                return "(string.empty)";
            } else if (objectValue is Exception) {
                return ExceptionMessage(objectValue as Exception);
            } else if (objectValue is IntPtr) {
                return "0x" + ((IntPtr)objectValue).ToString("x");
            } else {
                return objectValue.ToString();
            }
        }
        public static string HashString(object objectValue) {
            if (objectValue == null) {
                return "(null)";
            } else if (objectValue is string && ((string)objectValue).Length==0) {
                return "(string.empty)";
            } else {
                return objectValue.GetHashCode().ToString(NumberFormatInfo.InvariantInfo);
            }
        }

        public static bool IsInvalidHttpString(string stringValue) {
            return stringValue.IndexOfAny(InvalidParamChars)!=-1;
        }

        public static bool IsBlankString(string stringValue) {
            return stringValue==null || stringValue.Length==0;
        }

        /*
        //                                    
        public static bool ValidateUInt32(long address) {
            // on false, API should throw new ArgumentOutOfRangeException("address");
            return address>=0x00000000 && address<=0xFFFFFFFF;
        }
        */

        public static bool ValidateTcpPort(int port) {
            // on false, API should throw new ArgumentOutOfRangeException("port");
            return port>=IPEndPoint.MinPort && port<=IPEndPoint.MaxPort;
        }

        public static bool ValidateRange(int actual, int fromAllowed, int toAllowed) {
            // on false, API should throw new ArgumentOutOfRangeException("argument");
            return actual>=fromAllowed && actual<=toAllowed;
        }

        /*
        //                                    
        public static bool ValidateRange(long actual, long fromAllowed, long toAllowed) {
            // on false, API should throw new ArgumentOutOfRangeException("argument");
            return actual>=fromAllowed && actual<=toAllowed;
        }
        */
    }

    internal static class ExceptionHelper
    {
        internal static readonly WebPermission WebPermissionUnrestricted = new WebPermission(NetworkAccess.Connect);
        internal static readonly SecurityPermission UnmanagedPermission = new SecurityPermission(SecurityPermissionFlag.UnmanagedCode);
        internal static readonly SocketPermission UnrestrictedSocketPermission = new SocketPermission(PermissionState.Unrestricted);
        internal static readonly SecurityPermission InfrastructurePermission = new SecurityPermission(SecurityPermissionFlag.Infrastructure);
        internal static readonly SecurityPermission ControlPolicyPermission = new SecurityPermission(SecurityPermissionFlag.ControlPolicy);
        internal static readonly SecurityPermission ControlPrincipalPermission = new SecurityPermission(SecurityPermissionFlag.ControlPrincipal);

        internal static NotImplementedException MethodNotImplementedException {
            get {
                return new NotImplementedException(SR.GetString(SR.net_MethodNotImplementedException));
            }
        }

        internal static NotImplementedException PropertyNotImplementedException {
            get {
                return new NotImplementedException(SR.GetString(SR.net_PropertyNotImplementedException));
            }
        }

        internal static NotSupportedException MethodNotSupportedException {
            get {
                return new NotSupportedException(SR.GetString(SR.net_MethodNotSupportedException));
            }
        }

        internal static NotSupportedException PropertyNotSupportedException {
            get {
                return new NotSupportedException(SR.GetString(SR.net_PropertyNotSupportedException));
            }
        }

        internal static WebException IsolatedException {
            get {
                return new WebException(NetRes.GetWebStatusString("net_requestaborted", WebExceptionStatus.KeepAliveFailure),WebExceptionStatus.KeepAliveFailure, WebExceptionInternalStatus.Isolated, null);
            }
        }

        internal static WebException RequestAbortedException {
            get {
                return new WebException(NetRes.GetWebStatusString("net_requestaborted", WebExceptionStatus.RequestCanceled), WebExceptionStatus.RequestCanceled);
            }
        }

        internal static UriFormatException BadSchemeException {
            get {
                return new UriFormatException(SR.GetString(SR.net_uri_BadScheme));
            }
        }

        internal static UriFormatException BadAuthorityException {
            get {
                return new UriFormatException(SR.GetString(SR.net_uri_BadAuthority));
            }
        }

        internal static UriFormatException EmptyUriException {
            get {
                return new UriFormatException(SR.GetString(SR.net_uri_EmptyUri));
            }
        }

        internal static UriFormatException SchemeLimitException {
            get {
                return new UriFormatException(SR.GetString(SR.net_uri_SchemeLimit));
            }
        }

        internal static UriFormatException SizeLimitException {
            get {
                return new UriFormatException(SR.GetString(SR.net_uri_SizeLimit));
            }
        }

        internal static UriFormatException MustRootedPathException {
            get {
                return new UriFormatException(SR.GetString(SR.net_uri_MustRootedPath));
            }
        }

        internal static UriFormatException BadHostNameException {
            get {
                return new UriFormatException(SR.GetString(SR.net_uri_BadHostName));
            }
        }

        internal static UriFormatException BadPortException {
            get {
                return new UriFormatException(SR.GetString(SR.net_uri_BadPort));
            }
        }

        internal static UriFormatException BadAuthorityTerminatorException {
            get {
                return new UriFormatException(SR.GetString(SR.net_uri_BadAuthorityTerminator));
            }
        }

        internal static UriFormatException BadFormatException {
            get {
                return new UriFormatException(SR.GetString(SR.net_uri_BadFormat));
            }
        }

        internal static UriFormatException CannotCreateRelativeException {
            get {
                return new UriFormatException(SR.GetString(SR.net_uri_CannotCreateRelative));
            }
        }

        internal static WebException CacheEntryNotFoundException {
            get {
                return new WebException(NetRes.GetWebStatusString("net_requestaborted", WebExceptionStatus.CacheEntryNotFound), WebExceptionStatus.CacheEntryNotFound);
            }
        }

        internal static WebException RequestProhibitedByCachePolicyException {
            get {
                return new WebException(NetRes.GetWebStatusString("net_requestaborted", WebExceptionStatus.RequestProhibitedByCachePolicy), WebExceptionStatus.RequestProhibitedByCachePolicy);
            }
        }
    }


    //
    // WebRequestPrefixElement
    //
    // This is an element of the prefix list. It contains the prefix and the
    // interface to be called to create a request for that prefix.
    //

    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    // internal class WebRequestPrefixElement {
    internal class WebRequestPrefixElement  {

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public    string              Prefix;
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        internal    IWebRequestCreate   creator;
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        internal    Type   creatorType;

        public IWebRequestCreate Creator {
            get {
                if (creator == null && creatorType != null) {
                    lock(this) {
                        if (creator == null) {
                            creator = (IWebRequestCreate)Activator.CreateInstance(
                                                        creatorType,
                                                        BindingFlags.CreateInstance
                                                        | BindingFlags.Instance
                                                        | BindingFlags.NonPublic
                                                        | BindingFlags.Public,
                                                        null,          // Binder
                                                        new object[0], // no arguments
                                                        CultureInfo.InvariantCulture
                                                        );
                        }
                    }
                }

                return creator;
            }

            set {
                creator = value;
            }
        }

        public WebRequestPrefixElement(string P, Type creatorType) {
            // verify that its of the proper type of IWebRequestCreate
            if (!typeof(IWebRequestCreate).IsAssignableFrom(creatorType))
            {
                throw new InvalidCastException(SR.GetString(SR.net_invalid_cast,
                                                                creatorType.AssemblyQualifiedName,
                                                                "IWebRequestCreate"));
            }

            Prefix = P;
            this.creatorType = creatorType;
        }

        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public WebRequestPrefixElement(string P, IWebRequestCreate C) {
            Prefix = P;
            Creator = C;
        }

    } // class PrefixListElement


    //
    // HttpRequestCreator.
    //
    // This is the class that we use to create HTTP and HTTPS requests.
    //

    internal class HttpRequestCreator : IWebRequestCreate {

        /*++

         Create - Create an HttpWebRequest.

            This is our method to create an HttpWebRequest. We register
            for HTTP and HTTPS Uris, and this method is called when a request
            needs to be created for one of those.


            Input:
                    Uri             - Uri for request being created.

            Returns:
                    The newly created HttpWebRequest.

         --*/

        public WebRequest Create( Uri Uri ) {
            //
            // Note, DNS permissions check will not happen on WebRequest
            //
            return new HttpWebRequest(Uri, null);
        }

    } // class HttpRequestCreator

    //
    //  CoreResponseData - Used to store result of HTTP header parsing and
    //      response parsing.  Also Contains new stream to use, and
    //      is used as core of new Response
    //
    internal class CoreResponseData {

        // Status Line Response Values
        public HttpStatusCode m_StatusCode;
        public string m_StatusDescription;
        public bool m_IsVersionHttp11;

        // Content Length needed for semantics, -1 if chunked
        public long m_ContentLength;

        // Response Headers
        public WebHeaderCollection m_ResponseHeaders;

        // ConnectStream - for reading actual data
        public Stream m_ConnectStream;

        internal CoreResponseData Clone() {
            CoreResponseData cloneResponseData = new CoreResponseData();
            cloneResponseData.m_StatusCode        = m_StatusCode;
            cloneResponseData.m_StatusDescription = m_StatusDescription;
            cloneResponseData.m_IsVersionHttp11   = m_IsVersionHttp11;
            cloneResponseData.m_ContentLength     = m_ContentLength;
            cloneResponseData.m_ResponseHeaders   = m_ResponseHeaders;
            cloneResponseData.m_ConnectStream     = m_ConnectStream;
            return cloneResponseData;
        }

    }


    /*++

    StreamChunkBytes - A class to read a chunk stream from a ConnectStream.

    A simple little value class that implements the IReadChunkBytes
    interface.

    --*/
    internal class StreamChunkBytes : IReadChunkBytes {

        public  ConnectStream   ChunkStream;
        public  int             BytesRead = 0;
        public  int             TotalBytesRead = 0;
        private byte            PushByte;
        private bool            HavePush;

        public StreamChunkBytes(ConnectStream connectStream) {
            ChunkStream = connectStream;
            return;
        }

        public int NextByte {
            get {
                if (HavePush) {
                    HavePush = false;
                    return PushByte;
                }

                return ChunkStream.ReadSingleByte();
            }
            set {
                PushByte = (byte)value;
                HavePush = true;
            }
        }

    } // class StreamChunkBytes


    internal delegate bool HttpAbortDelegate(HttpWebRequest request, WebException webException);

    //
    // this class contains known header names
    //

    internal static class HttpKnownHeaderNames {

        public const string CacheControl = "Cache-Control";
        public const string Connection = "Connection";
        public const string Date = "Date";
        public const string KeepAlive = "Keep-Alive";
        public const string Pragma = "Pragma";
        public const string ProxyConnection = "Proxy-Connection";
        public const string Trailer = "Trailer";
        public const string TransferEncoding = "Transfer-Encoding";
        public const string Upgrade = "Upgrade";
        public const string Via = "Via";
        public const string Warning = "Warning";
        public const string ContentLength = "Content-Length";
        public const string ContentType = "Content-Type";
        public const string ContentEncoding = "Content-Encoding";
        public const string ContentLanguage = "Content-Language";
        public const string ContentLocation = "Content-Location";
        public const string ContentRange = "Content-Range";
        public const string Expires = "Expires";
        public const string LastModified = "Last-Modified";
        public const string Age = "Age";
        public const string Location = "Location";
        public const string ProxyAuthenticate = "Proxy-Authenticate";
        public const string RetryAfter = "Retry-After";
        public const string Server = "Server";
        public const string SetCookie = "Set-Cookie";
        public const string SetCookie2 = "Set-Cookie2";
        public const string Vary = "Vary";
        public const string WWWAuthenticate = "WWW-Authenticate";
        public const string Accept = "Accept";
        public const string AcceptCharset = "Accept-Charset";
        public const string AcceptEncoding = "Accept-Encoding";
        public const string AcceptLanguage = "Accept-Language";
        public const string Authorization = "Authorization";
        public const string Cookie = "Cookie";
        public const string Cookie2 = "Cookie2";
        public const string Expect = "Expect";
        public const string From = "From";
        public const string Host = "Host";
        public const string IfMatch = "If-Match";
        public const string IfModifiedSince = "If-Modified-Since";
        public const string IfNoneMatch = "If-None-Match";
        public const string IfRange = "If-Range";
        public const string IfUnmodifiedSince = "If-Unmodified-Since";
        public const string MaxForwards = "Max-Forwards";
        public const string ProxyAuthorization = "Proxy-Authorization";
        public const string Referer = "Referer";
        public const string Range = "Range";
        public const string UserAgent = "User-Agent";
        public const string ContentMD5 = "Content-MD5";
        public const string ETag = "ETag";
        public const string TE = "TE";
        public const string Allow = "Allow";
        public const string AcceptRanges = "Accept-Ranges";
        public const string P3P = "P3P";
        public const string XPoweredBy = "X-Powered-By";
        public const string XAspNetVersion = "X-AspNet-Version";
    }

    /// <devdoc>
    ///    <para>
    ///       Represents the method that will notify callers when a continue has been
    ///       received by the client.
    ///    </para>
    /// </devdoc>
    // Delegate type for us to notify callers when we receive a continue
    public delegate void HttpContinueDelegate(int StatusCode, WebHeaderCollection httpHeaders);

    //
    // HttpWriteMode - used to control the way in which an entity Body is posted.
    //
    enum HttpWriteMode {
        Unknown         = 0,
        ContentLength   = 1,
        Chunked         = 2,
        Buffer          = 3,
        None            = 4,
    }

    // Used by Request to notify Connection that we are no longer holding the Connection (for NTLM connection sharing)
    delegate void UnlockConnectionDelegate();

    enum HttpBehaviour : byte {
        Unknown                     = 0,
        HTTP10                      = 1,
        HTTP11PartiallyCompliant    = 2,
        HTTP11                      = 3,
    }

    internal enum HttpProcessingResult {
        Continue  = 0,
        ReadWait  = 1,
        WriteWait = 2,
    }

    //
    // HttpVerb - used to define various per Verb Properties
    //

    //
    // Note - this is a place holder for Verb properties,
    //  the following two bools can most likely be combined into
    //  a single Enum type.  And the Verb can be incorporated.
    //
    class KnownHttpVerb {
        internal string Name; // verb name

        internal bool RequireContentBody; // require content body to be sent
        internal bool ContentBodyNotAllowed; // not allowed to send content body
        internal bool ConnectRequest; // special semantics for a connect request
        internal bool ExpectNoContentResponse; // response will not have content body

        internal KnownHttpVerb(string name, bool requireContentBody, bool contentBodyNotAllowed, bool connectRequest, bool expectNoContentResponse) {
            Name = name;
            RequireContentBody = requireContentBody;
            ContentBodyNotAllowed = contentBodyNotAllowed;
            ConnectRequest = connectRequest;
            ExpectNoContentResponse = expectNoContentResponse;
        }

        // Force an an init, before we use them
        private static ListDictionary NamedHeaders;

        // known verbs
        internal static KnownHttpVerb Get;
        internal static KnownHttpVerb Connect;
        internal static KnownHttpVerb Head;
        internal static KnownHttpVerb Put;
        internal static KnownHttpVerb Post;
        internal static KnownHttpVerb MkCol;

        //
        // InitializeKnownVerbs - Does basic init for this object,
        //  such as creating defaultings and filling them
        //
        static KnownHttpVerb() {
            NamedHeaders = new ListDictionary(CaseInsensitiveAscii.StaticInstance);
            Get = new KnownHttpVerb("GET", false, true, false, false);
            Connect = new KnownHttpVerb("CONNECT", false, true, true, false);
            Head = new KnownHttpVerb("HEAD", false, true, false, true);
            Put = new KnownHttpVerb("PUT", true, false, false, false);
            Post = new KnownHttpVerb("POST", true, false, false, false);
            MkCol = new KnownHttpVerb("MKCOL",false,false,false,false);
            NamedHeaders[Get.Name] = Get;
            NamedHeaders[Connect.Name] = Connect;
            NamedHeaders[Head.Name] = Head;
            NamedHeaders[Put.Name] = Put;
            NamedHeaders[Post.Name] = Post;
            NamedHeaders[MkCol.Name] = MkCol;
        }

        public bool Equals(KnownHttpVerb verb) {
            return this==verb || string.Compare(Name, verb.Name, StringComparison.OrdinalIgnoreCase)==0;
        }

        public static KnownHttpVerb Parse(string name) {
            KnownHttpVerb knownHttpVerb = NamedHeaders[name] as KnownHttpVerb;
            if (knownHttpVerb==null) {
                // unknown verb, default behaviour
                knownHttpVerb = new KnownHttpVerb(name, false, false, false, false);
            }
            return knownHttpVerb;
        }
    }


    //
    // HttpProtocolUtils - A collection of utility functions for HTTP usage.
    //

    internal class HttpProtocolUtils {

        private HttpProtocolUtils() {
        }

        //
        // extra buffers for build/parsing, recv/send HTTP data,
        //  at some point we should consolidate
        //


        // parse String to DateTime format.
        internal static DateTime string2date(String S) {
            DateTime dtOut;
            if (HttpDateParse.ParseHttpDate(S,out dtOut)) {
                return dtOut;
            }
            else {
                throw new ProtocolViolationException(SR.GetString(SR.net_baddate));
            }

        }

        // convert Date to String using RFC 1123 pattern
        internal static string date2string(DateTime D) {
            DateTimeFormatInfo dateFormat = new DateTimeFormatInfo();
            return D.ToUniversalTime().ToString("R", dateFormat);
        }
    }


    internal enum TriState {
        Unspecified = -1,
        False = 0,
        True = 1
    }

    internal enum DefaultPorts {
        DEFAULT_FTP_PORT = 21,
        DEFAULT_GOPHER_PORT = 70,
        DEFAULT_HTTP_PORT = 80,
        DEFAULT_HTTPS_PORT = 443,
        DEFAULT_NNTP_PORT = 119,
        DEFAULT_SMTP_PORT = 25,
        DEFAULT_TELNET_PORT = 23
    }

    [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Unicode)]
    internal struct hostent {
        public IntPtr   h_name;
        public IntPtr   h_aliases;
        public short    h_addrtype;
        public short    h_length;
        public IntPtr   h_addr_list;
    }


    [StructLayout(LayoutKind.Sequential)]
    internal struct Blob {
        public int cbSize;
        public int pBlobData;
    }


    // This is only for internal code path i.e. TLS stream.
    // See comments on GetNextBuffer() method below.
    //
    internal class SplitWritesState
    {
        private const int c_SplitEncryptedBuffersSize = 64*1024;
        private BufferOffsetSize[] _UserBuffers;
        private int _Index;
        private int _LastBufferConsumed;
        private BufferOffsetSize[] _RealBuffers;

        //
        internal SplitWritesState(BufferOffsetSize[] buffers)
        {
            _UserBuffers    = buffers;
            _LastBufferConsumed = 0;
            _Index          = 0;
            _RealBuffers = null;
        }
        //
        // Everything was handled
        //
        internal bool IsDone {
            get {
                if (_LastBufferConsumed != 0)
                    return false;

                for (int index = _Index ;index < _UserBuffers.Length; ++index)
                    if (_UserBuffers[index].Size != 0)
                        return false;

                return true;
            }
        }
        // Encryption takes CPU and if the input is large (like 10 mb) then a delay may
        // be 30 sec or so. Hence split the ecnrypt and write operations in smaller chunks
        // up to c_SplitEncryptedBuffersSize total.
        // Note that upon return from here EncryptBuffers() may additonally split the input
        // into chunks each <= chkSecureChannel.MaxDataSize (~16k) yet it will complete them all as a single IO.
        //
        //  Returns null if done, returns the _buffers reference if everything is handled in one shot (also done)
        //
        //  Otheriwse returns subsequent BufferOffsetSize[] to encrypt and pass to base IO method
        //
        internal BufferOffsetSize[] GetNextBuffers()
        {
            int curIndex = _Index;
            int currentTotalSize = 0;
            int lastChunkSize = 0;

            int  firstBufferConsumed = _LastBufferConsumed;

            for ( ;_Index < _UserBuffers.Length; ++_Index)
            {
                lastChunkSize = _UserBuffers[_Index].Size-_LastBufferConsumed;

                currentTotalSize += lastChunkSize;

                if (currentTotalSize > c_SplitEncryptedBuffersSize)
                {
                    lastChunkSize -= (currentTotalSize - c_SplitEncryptedBuffersSize);
                    currentTotalSize = c_SplitEncryptedBuffersSize;
                    break;
                }

                lastChunkSize = 0;
                _LastBufferConsumed = 0;
            }

            // Are we done done?
            if (currentTotalSize == 0)
                return null;

             // Do all buffers fit the limit?
            if (firstBufferConsumed == 0 && curIndex == 0 && _Index == _UserBuffers.Length)
                return _UserBuffers;

            // We do have something to split and send out
            int buffersCount = lastChunkSize == 0? _Index-curIndex: _Index-curIndex+1;

            if (_RealBuffers == null || _RealBuffers.Length != buffersCount)
                _RealBuffers = new BufferOffsetSize[buffersCount];

            int j = 0;
            for (; curIndex < _Index; ++curIndex)
            {
                _RealBuffers[j++] = new BufferOffsetSize(_UserBuffers[curIndex].Buffer, _UserBuffers[curIndex].Offset + firstBufferConsumed, _UserBuffers[curIndex].Size-firstBufferConsumed, false);
                firstBufferConsumed = 0;
            }

            if (lastChunkSize != 0)
            {
                _RealBuffers[j] = new BufferOffsetSize(_UserBuffers[curIndex].Buffer, _UserBuffers[curIndex].Offset + _LastBufferConsumed, lastChunkSize, false);
                if ((_LastBufferConsumed += lastChunkSize) == _UserBuffers[_Index].Size)
                {
                    ++_Index;
                    _LastBufferConsumed = 0;
                }
            }

            return _RealBuffers;

        }
    }
}
