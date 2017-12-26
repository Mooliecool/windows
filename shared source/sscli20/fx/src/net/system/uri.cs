//------------------------------------------------------------------------------
// <copyright file="URI.cs" company="Microsoft">
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

namespace System {
    using System.Configuration;
    using System.Runtime.InteropServices;
    using System.Text;
    using System.Globalization;
    using System.Runtime.Serialization;
    using System.ComponentModel;
    using System.Security.Permissions;
//  using System.Threading;

    [Serializable]
    [TypeConverter(typeof(UriTypeConverter))]
    public partial class Uri : ISerializable {

        public static readonly string UriSchemeFile = UriParser.FileUri.SchemeName;
        public static readonly string UriSchemeFtp = UriParser.FtpUri.SchemeName;
        public static readonly string UriSchemeGopher = UriParser.GopherUri.SchemeName;
        public static readonly string UriSchemeHttp = UriParser.HttpUri.SchemeName;
        public static readonly string UriSchemeHttps = UriParser.HttpsUri.SchemeName;
        public static readonly string UriSchemeMailto = UriParser.MailToUri.SchemeName;
        public static readonly string UriSchemeNews = UriParser.NewsUri.SchemeName;
        public static readonly string UriSchemeNntp = UriParser.NntpUri.SchemeName;
        public static readonly string UriSchemeNetTcp = UriParser.NetTcpUri.SchemeName;
        public static readonly string UriSchemeNetPipe = UriParser.NetPipeUri.SchemeName;
        public static readonly string SchemeDelimiter = "://";


        private const int c_Max16BitUtf8SequenceLength = 3+3+3+3; //each unicode byte takes 3 escaped chars
        private const int c_MaxUriBufferSize = 0xFFF0;
        private const int c_MaxUriSchemeName = 1024;

        private string      m_String;   // untouched user string
        private UriParser   m_Syntax;   // This is a whole Uri syntax, not only the scheme name

        private enum ParsingError {
            // looks good
            None            = 0,

            // Could be only Relative
            BadFormat           = 1,
            BadScheme           = 2,
            BadAuthority        = 3,
            EmptyUriString      = 4,
            LastRelativeUriOkErrIndex   = 4,

            // Fatal
            SchemeLimit         = 5,
            SizeLimit           = 6,
            MustRootedPath      = 7,
            LastFatalErrIndex           = 7,

            // derived class controlled
            BadHostName         = 8,
            NonEmptyHost        = 9,    //unix only
            BadPort             = 10,
            BadAuthorityTerminator=11,

            CannotCreateRelative = 12
        }

        [Flags]
        private enum Flags {
            Zero                = 0x00000000,

            SchemeNotCanonical     = 0x1,
            UserNotCanonical       = 0x2,
            HostNotCanonical       = 0x4,
            PortNotCanonical       = 0x8,
            PathNotCanonical       = 0x10,
            QueryNotCanonical      = 0x20,
            FragmentNotCanonical   = 0x40,
            CannotDisplayCanonical = 0x7F,

            E_UserNotCanonical      = 0x80,
            E_HostNotCanonical      = 0x100,
            E_PortNotCanonical      = 0x200,
            E_PathNotCanonical      = 0x400,
            E_QueryNotCanonical     = 0x800,
            E_FragmentNotCanonical  = 0x1000,
            E_CannotDisplayCanonical = 0x1F80,


            ShouldBeCompressed      = 0x2000,
            FirstSlashAbsent        = 0x4000,
            BackslashInPath         = 0x8000,

            IndexMask           = 0x0000FFFF,
            HostTypeMask        = 0x00070000,
            HostNotParsed   = 0x00000000,
            IPv6HostType    = 0x00010000,
            IPv4HostType    = 0x00020000,
            DnsHostType     = 0x00030000,
#if !PLATFORM_UNIX
            UncHostType     = 0x00040000,
#endif // !PLATFORM_UNIX
            BasicHostType   = 0x00050000,
            UnusedHostType  = 0x00060000,
            UnknownHostType = 0x00070000,

            UserEscaped         = 0x00080000,
            AuthorityFound      = 0x00100000,
            HasUserInfo         = 0x00200000,
            LoopbackHost        = 0x00400000,
            NotDefaultPort      = 0x00800000,

            UserDrivenParsing   = 0x01000000,
            CanonicalDnsHost    = 0x02000000,
            ErrorOrParsingRecursion = 0x04000000,   // Used to signal a default parser error and alsoe to confirm Port and Host values in case of a custom user Parser
#if !PLATFORM_UNIX
            DosPath             = 0x08000000,
            UncPath             = 0x10000000,
#endif // !PLATFORM_UNIX
            ImplicitFile        = 0x20000000,
            MinimalUriInfoSet   = 0x40000000,
            AllUriInfoSet       = unchecked((int)0x80000000),
        }

        private Flags       m_Flags;
        private UriInfo     m_Info;

        private class UriInfo {
            public string   Host;
            public string   ScopeId;        //only IP v6 may need this
            public string   String;
            public Offset   Offset;
            public MoreInfo MoreInfo;       //Multi-threading: This field must be always accessed through a _local_ stack copy of m_Info.
        };

        [StructLayout(LayoutKind.Sequential, Pack=1)]
        private struct Offset {
            public ushort  Scheme;
            public ushort  User;
            public ushort  Host;
            public ushort  PortValue;
            public ushort  Path;
            public ushort  Query;
            public ushort  Fragment;
            public ushort  End;
        };

        private class MoreInfo {
            public string   Path;
            public string   Query;
            public string   Fragment;
            public string   AbsoluteUri;
            public int      Hash;
            public string   RemoteUrl;
        };

        private bool IsImplicitFile {
            get {return (m_Flags & Flags.ImplicitFile) != 0;}
        }

        private bool IsUncOrDosPath {
#if !PLATFORM_UNIX
            get {return (m_Flags & (Flags.UncPath|Flags.DosPath)) != 0;}
#else
            get {return false;}
#endif // !PLATFORM_UNIX
        }

        private bool IsDosPath {
#if !PLATFORM_UNIX
            get {return (m_Flags & Flags.DosPath) != 0;}
#else
            get {return false;}
#endif // !PLATFORM_UNIX
        }

        private bool IsUncPath {
#if !PLATFORM_UNIX
            get {return (m_Flags & Flags.UncPath) != 0;}
#else
            get {return false;}
#endif // !PLATFORM_UNIX
        }

        private Flags HostType {
            get {return m_Flags & Flags.HostTypeMask;}
        }

        private UriParser Syntax {
            get {
                return m_Syntax;
            }
        }

        private bool IsNotAbsoluteUri {
            get {return (object) m_Syntax == null;}
        }

        internal bool UserDrivenParsing
        {
            get {
                return (m_Flags & Flags.UserDrivenParsing) != 0;
            }
        }
        private void SetUserDrivenParsing()
        {
            // we use = here to clear all parsing flags for a uri that we think is invalid.
            m_Flags = Flags.UserDrivenParsing | (m_Flags & Flags.UserEscaped);
        }

        private ushort SecuredPathIndex {
            get {
                // This is one more trouble with a Dos Path.
                // This property gets "safe" first path slash that is not the first if path = c:\
                if (IsDosPath) {
                    char ch = m_String[m_Info.Offset.Path];
                    return (ushort)((ch == '/' || ch == '\\')? 3 :2);
                }
                return (ushort)0;
            }
        }

        private bool NotAny(Flags flags) {
            return (m_Flags & flags) == 0;
        }

        private bool InFact(Flags flags) {
            return (m_Flags & flags) != 0;
        }

        private static bool StaticNotAny(Flags allFlags, Flags checkFlags) {
            return (allFlags & checkFlags) == 0;
        }

        private static bool StaticInFact(Flags allFlags, Flags checkFlags) {
            return (allFlags & checkFlags) != 0;
        }

        //
        //
        private UriInfo EnsureUriInfo() {
            Flags cF = m_Flags;
            if (NotAny(Flags.MinimalUriInfoSet)) {
                CreateUriInfo(cF);
            }
            return m_Info;
        }
        //
        //
        private void EnsureParseRemaining() {
            if (NotAny(Flags.AllUriInfoSet)) {
                ParseRemaining();
            }
        }
        //
        //
        private void EnsureHostString(bool allowDnsOptimization) {
            EnsureUriInfo();
            if ((object)m_Info.Host == null) {
                if (allowDnsOptimization && InFact(Flags.CanonicalDnsHost)) {
                    /* Optimization for a canonical DNS name
                    *  ATTN: the host string won't be created,
                    *  Hence ALL m_Info.Host callers first call EnsureHostString(false)
                    *  For example IsLoopBack property is one of such callers.
                    */
                    return;
                }
                CreateHostString();
            }
        }

        //
        // Uri(string)
        //
        //  We expect to create a Uri from a display name - e.g. that was typed by
        //  a user, or that was copied & pasted from a document. That is, we do not
        //  expect already encoded URI to be supplied.
        //
        public Uri(string uriString){
            if ((object)uriString == null)
                throw new ArgumentNullException("uriString");

            CreateThis(uriString, false, UriKind.Absolute);
        }


        //
        // Uri(string, bool)
        //
        //  Uri constructor. Assumes that input string is canonically escaped
        //
        [Obsolete("The constructor has been deprecated. Please use new Uri(string). The dontEscape parameter is deprecated and is always false. http://go.microsoft.com/fwlink/?linkid=14202")]
        public Uri(string uriString, bool dontEscape) {
            if ((object)uriString == null)
                throw new ArgumentNullException("uriString");

            CreateThis(uriString, dontEscape, UriKind.Absolute);
        }

        
        //
        // Uri(string, UriKind);
        //
        public Uri(string uriString, UriKind uriKind)
        {
            if ((object)uriString == null)
                throw new ArgumentNullException("uriString");

            CreateThis(uriString, false, uriKind);
        }


        //
        // Uri(Uri, string)
        //
        //  Construct a new Uri from a base and relative URI. The relative URI may
        //  also be an absolute URI, in which case the resultant URI is constructed
        //  entirely from it
        //
        public Uri(Uri baseUri, string relativeUri){
            if ((object)baseUri == null)
                throw new ArgumentNullException("baseUri");

            if (!baseUri.IsAbsoluteUri)
                throw new ArgumentOutOfRangeException("baseUri");

            CreateUri(baseUri, relativeUri, false);
        }

        //
        // Uri(Uri, string, bool)
        //
        //  Uri combinatorial constructor. Do not perform character escaping if
        //  DontEscape is true
        //
        [Obsolete("The constructor has been deprecated. Please new Uri(Uri, string). The dontEscape parameter is deprecated and is always false. http://go.microsoft.com/fwlink/?linkid=14202")]
        public Uri(Uri baseUri, string relativeUri, bool dontEscape){
            if ((object)baseUri == null)
                throw new ArgumentNullException("baseUri");

            if (!baseUri.IsAbsoluteUri)
                throw new ArgumentOutOfRangeException("baseUri");

            CreateUri(baseUri, relativeUri, dontEscape);
        }

        private void CreateUri(Uri baseUri, string relativeUri, bool dontEscape) 
        {
            // Parse relativeUri and populate Uri internal data.
            CreateThis(relativeUri, dontEscape, UriKind.RelativeOrAbsolute);

            UriFormatException e;
            if (baseUri.Syntax.IsSimple)
            {
                // Resolve Uris if possible OR get merged Uri String to re-parse below
                Uri uriResult = ResolveHelper(baseUri, this, ref relativeUri, ref dontEscape, out e);

                if (e != null)
                    throw e;

                // If resolved into a Uri then we build from that Uri
                if (uriResult != null)
                {
                    if ((object)uriResult != (object)this)
                        CreateThisFromUri(uriResult);

                    return;
                }
            }
            else
            {
                dontEscape = false;
                relativeUri = baseUri.Syntax.InternalResolve(baseUri, this, out e);
                if (e != null)
                    throw e;
            }

            m_Flags = Flags.Zero;
            m_Info = null;
            m_Syntax = null;
            // If not resolved, we reparse modified Uri string and populate Uri internal data.
            CreateThis(relativeUri, dontEscape, UriKind.Absolute);
        }

        //
        // Uri(Uri , Uri )
        // Note: a static Create() method should be used by users, not this .ctor
        //
        public Uri(Uri baseUri, Uri relativeUri)
        {
            if ((object)baseUri == null)
                throw new ArgumentNullException("baseUri");

            if (!baseUri.IsAbsoluteUri)
                throw new ArgumentOutOfRangeException("baseUri");

            CreateThisFromUri(relativeUri);

            string newUriString = null;
            UriFormatException e;
            bool dontEscape;

            if (baseUri.Syntax.IsSimple)
            {
                dontEscape = InFact(Flags.UserEscaped);
                relativeUri = ResolveHelper(baseUri, this, ref newUriString, ref dontEscape, out e);

                if (e != null)
                    throw e;

                if (relativeUri != null)
                {
                    if ((object)relativeUri != (object)this)
                        CreateThisFromUri(relativeUri);

                    return;
                }
            }
            else
            {
                dontEscape = false;
                newUriString = baseUri.Syntax.InternalResolve(baseUri, this, out e);
                if (e != null)
                    throw e;
            }

            m_Flags = Flags.Zero;
            m_Info = null;
            m_Syntax = null;
            CreateThis(newUriString, dontEscape, UriKind.Absolute);
        }

        //
        // ISerializable constructor
        //
        protected Uri(SerializationInfo serializationInfo, StreamingContext streamingContext)
        {
            string uriString = serializationInfo.GetString("AbsoluteUri");

            if (uriString.Length != 0)
            {
                CreateThis(uriString, false, UriKind.Absolute);
                return;
            }

            uriString = serializationInfo.GetString("RelativeUri");
            if ((object)uriString == null)
                throw new ArgumentNullException("uriString");

            CreateThis(uriString, false, UriKind.Relative);
        }
        //
        // This method is shared by base+relative Uris constructors and is only called from them.
        // The assumptions:
        //  - baseUri is a valid absolute Uri
        //  - relative part is not null and not empty
        private unsafe static ParsingError GetCombinedString(Uri baseUri, string relativeStr, bool dontEscape, ref string result)
        {
            // NB: This is not RFC2396 compliant although it is inline with w3c.org recommendations
            // This parser will allow the relativeStr to be an absolute Uri with the different scheme
            // In fact this is strict violation of RFC2396
            //
            for (int i=0; i < relativeStr.Length; ++i)
            {
                if (relativeStr[i] == '/' || relativeStr[i] == '\\' || relativeStr[i] == '?' || relativeStr[i] == '#')
                {
                    break;
                }
                else if (relativeStr[i] == ':')
                {
                    if (i < 2)
                    {
                        // Note we don't support one-letter Uri schemes.
                        // Hence anything like x:sdsd is a relative path and be added to the baseUri Path
                        break;
                    }
                    string scheme =  relativeStr.Substring(0, i);
                    fixed (char* sptr = scheme) {
                        UriParser syntax = null;
                        if (CheckSchemeSyntax(sptr, (ushort) scheme.Length, ref syntax) == ParsingError.None) {
                            if (baseUri.Syntax == syntax) {
                                //Remove the scheme for backward Uri parsers compatibility
                                if (i+1 < relativeStr.Length) {
                                    relativeStr = relativeStr.Substring(i+1);
                                }
                                else {
                                    relativeStr = string.Empty;
                                }
                            }
                            else {
                                // This is the place where we switch the scheme.
                                // Return relative part as the result Uri.
                                result = relativeStr;
                                return ParsingError.None;
                            }
                        }
                    }
                    break;
                }
            }

            if (relativeStr.Length == 0) {
                result = baseUri.OriginalString;
                return ParsingError.None;
            }

            result = CombineUri(baseUri, relativeStr, dontEscape? UriFormat.UriEscaped: UriFormat.SafeUnescaped);
            return ParsingError.None;
        }
        //
        private static UriFormatException GetException(ParsingError err)
        {
            switch (err)
            {
                case ParsingError.None:
                    return null;
                // Could be OK for Relative Uri
                case ParsingError.BadFormat:
                    return System.Net.ExceptionHelper.BadFormatException;
                case ParsingError.BadScheme:
                    return System.Net.ExceptionHelper.BadSchemeException;
                case ParsingError.BadAuthority:
                    return System.Net.ExceptionHelper.BadAuthorityException;
                case ParsingError.EmptyUriString:
                    return System.Net.ExceptionHelper.EmptyUriException;
                // Fatal
                case ParsingError.SchemeLimit:
                    return System.Net.ExceptionHelper.SchemeLimitException;
                case ParsingError.SizeLimit:
                    return System.Net.ExceptionHelper.SizeLimitException;
                case ParsingError.MustRootedPath:
                    return System.Net.ExceptionHelper.MustRootedPathException;
                // Derived class controllable
                case ParsingError.BadHostName:
                    return System.Net.ExceptionHelper.BadHostNameException;
                case ParsingError.NonEmptyHost:
                    return System.Net.ExceptionHelper.BadFormatException;   //unix-only
                case ParsingError.BadPort:
                    return System.Net.ExceptionHelper.BadPortException;
                case ParsingError.BadAuthorityTerminator:
                    return System.Net.ExceptionHelper.BadAuthorityTerminatorException;
                case ParsingError.CannotCreateRelative:
                    return System.Net.ExceptionHelper.CannotCreateRelativeException;
                default:
                    break;
            }
            return System.Net.ExceptionHelper.BadFormatException;
        }
        //
        // ISerializable method
        //
        /// <internalonly/>
        [SecurityPermission(SecurityAction.LinkDemand, SerializationFormatter=true)]
        void ISerializable.GetObjectData(SerializationInfo serializationInfo, StreamingContext streamingContext)
        {
            GetObjectData(serializationInfo, streamingContext);
        }

        //
        // FxCop: provide some way for derived classes to access GetObjectData even if the derived class
        // explicitly re-inherits ISerializable.
        //
        [SecurityPermission(SecurityAction.LinkDemand, SerializationFormatter=true)]
        protected void GetObjectData(SerializationInfo serializationInfo, StreamingContext streamingContext)
        {

            if (IsAbsoluteUri)
                serializationInfo.AddValue("AbsoluteUri", GetParts(UriComponents.SerializationInfoString, UriFormat.UriEscaped));
            else
            {
                serializationInfo.AddValue("AbsoluteUri", string.Empty);
                serializationInfo.AddValue("RelativeUri", GetParts(UriComponents.SerializationInfoString, UriFormat.UriEscaped));
            }
        }

        //
        //
        //
        public string AbsolutePath {
            get {
                if (IsNotAbsoluteUri) {
                    throw new InvalidOperationException(SR.GetString(SR.net_uri_NotAbsolute));
                }

                string path = PrivateAbsolutePath;
                if (IsDosPath && path[0] == '/') {
                    path =  path.Substring(1);
                }
                return path;
            }
        }
        //
        private string PrivateAbsolutePath {
            get {
                UriInfo info = EnsureUriInfo();
                if ((object) info.MoreInfo == null) {
                    info.MoreInfo = new MoreInfo();
                }
                string result = info.MoreInfo.Path;
                if ((object) result == null) {
                    result = GetParts(UriComponents.Path | UriComponents.KeepDelimiter, UriFormat.UriEscaped);
                    info.MoreInfo.Path = result;
                }
                return result;
            }
        }
        //
        //
        //
        public string AbsoluteUri {
            get {
                if (IsNotAbsoluteUri) {
                    throw new InvalidOperationException(SR.GetString(SR.net_uri_NotAbsolute));
                }

                UriInfo info = EnsureUriInfo();
                if ((object) info.MoreInfo == null) {
                    info.MoreInfo = new MoreInfo();
                }
                string result = info.MoreInfo.AbsoluteUri;
                if ((object) result == null) {
                    result = GetParts(UriComponents.AbsoluteUri, UriFormat.UriEscaped);
                    info.MoreInfo.AbsoluteUri = result;
                }
                return result;
            }
        }
        //
        //
        // The result is of the form "hostname[:port]" Port is omitted if default
        //
        public string Authority {
            get {
                if (IsNotAbsoluteUri) {
                    throw new InvalidOperationException(SR.GetString(SR.net_uri_NotAbsolute));
                }

                return GetParts(UriComponents.Host | UriComponents.Port, UriFormat.UriEscaped);
            }
        }
        //
        //
        //Gets a hostname part (special formatting for IPv6 form)
        public string Host {
            get {
                if (IsNotAbsoluteUri) {
                    throw new InvalidOperationException(SR.GetString(SR.net_uri_NotAbsolute));
                }

                return GetParts(UriComponents.Host, UriFormat.UriEscaped);
            }
        }
        //
        //
        public UriHostNameType HostNameType {
            get {
                if (IsNotAbsoluteUri) {
                    throw new InvalidOperationException(SR.GetString(SR.net_uri_NotAbsolute));
                }

                if (m_Syntax.IsSimple)
                    EnsureUriInfo();
                else
                {
                    // For a custom parser we request HostString creation to confirm HostType
                    EnsureHostString(false);
                }

                switch (HostType) {
                    case Flags.DnsHostType:   return UriHostNameType.Dns;
                    case Flags.IPv4HostType:  return UriHostNameType.IPv4;
                    case Flags.IPv6HostType:  return UriHostNameType.IPv6;
                    case Flags.BasicHostType: return UriHostNameType.Basic;
#if !PLATFORM_UNIX
                    case Flags.UncHostType:   return UriHostNameType.Basic; //return (UriHostNameType)(UriHostNameType.Basic+10);
#endif // !PLATFORM_UNIX
                    case Flags.UnknownHostType: return UriHostNameType.Unknown;
                    default:
                        break;
                }
                return UriHostNameType.Unknown;
            }
        }
        //
        //
        public bool IsDefaultPort {
            get {
                if (IsNotAbsoluteUri) {
                    throw new InvalidOperationException(SR.GetString(SR.net_uri_NotAbsolute));
                }
                if (m_Syntax.IsSimple)
                    EnsureUriInfo();
                else
                {
                    // For a custom parser we request HostString creation that will aso set the port
                    EnsureHostString(false);
                }

                return NotAny(Flags.NotDefaultPort);
            }
        }
        //
        //
        public bool IsFile {
            get {
                if (IsNotAbsoluteUri) {
                    throw new InvalidOperationException(SR.GetString(SR.net_uri_NotAbsolute));
                }

                return (object)m_Syntax.SchemeName == (object)UriSchemeFile;
            }
        }
        private static bool StaticIsFile(UriParser syntax)
        {
            return syntax.InFact(UriSyntaxFlags.FileLikeUri);
        }
        //
        //
        public bool IsLoopback {
            get {
                if (IsNotAbsoluteUri) {
                    throw new InvalidOperationException(SR.GetString(SR.net_uri_NotAbsolute));
                }

                EnsureHostString(false);

                return InFact(Flags.LoopbackHost);
            }
        }
        //
        //
        public bool IsUnc {
            get {
                if (IsNotAbsoluteUri) {
                    throw new InvalidOperationException(SR.GetString(SR.net_uri_NotAbsolute));
                }
                return IsUncPath;
            }
        }
        //
        // LocalPath
        //
        //  Returns a 'local' version of the path. This is mainly for file: URI
        //  such that DOS and UNC paths are returned with '/' converted back to
        //  '\', and any escape sequences converted
        //
        //  The form of the returned path is in NOT Escaped
        //
        public string LocalPath {
            get {
                if (IsNotAbsoluteUri) {
                    throw new InvalidOperationException(SR.GetString(SR.net_uri_NotAbsolute));
                }
                return GetLocalPath();
            }
        }

        private string GetLocalPath() {
            EnsureParseRemaining();

            //Other cases will get a Unix-style path
            if (IsUncOrDosPath)
            {
                EnsureHostString(false);
                int start;

                // Do we have a valid local path right in m_string?
                if (NotAny(Flags.HostNotCanonical|Flags.PathNotCanonical|Flags.ShouldBeCompressed)) {

                    start = IsUncPath? m_Info.Offset.Host-2 :m_Info.Offset.Path;


                    string str =
                        (IsImplicitFile && m_Info.Offset.Host == (IsDosPath?0:2) && m_Info.Offset.Query == m_Info.Offset.End)
                            ? m_String
                            : (IsDosPath && (m_String[start] == '/' || m_String[start] == '\\'))
                                ? m_String.Substring(start + 1, m_Info.Offset.Query - start - 1)
                                : m_String.Substring(start, m_Info.Offset.Query - start);

                    // Should be a rare case, convert c|\ into c:\
                    if (IsDosPath && str[1] == '|') {
                        // Sadly, today there is no method for replacong just one occurrence
                        str = str.Remove(1, 1);
                        str = str.Insert(1, ":");
                    }

                    // check for all back slashes (though may be string.Replace is smart?)
                    for (int i = 0; i < str.Length; ++i) {
                        if (str[i] == '/') {
                            str = str.Replace('/', '\\');
                            break;
                        }
                    }

                    return str;
                }

                // Not everything went well, going hardcore

                char[] result;
                int count = 0;
                start = m_Info.Offset.Path;

                string host = m_Info.Host;
                result = new char [host.Length + 3 + m_Info.Offset.Fragment - m_Info.Offset.Path ];

                if (IsUncPath)
                {
                    result[0] = '\\';
                    result[1] = '\\';
                    count = 2;

                    UnescapeString(host, 0, host.Length, result, ref count, c_DummyChar, c_DummyChar, c_DummyChar, UnescapeMode.CopyOnly);

                }
                else {
                    // Dos path
                    if(m_String[start] == '/' ||  m_String[start] == '\\') {
                        // Skip leading slash for a DOS path
                        ++start;
                    }
                }


                ushort pathStart = (ushort)count; //save for optional Compress() call

                UnescapeMode mode = (InFact(Flags.PathNotCanonical) && !IsImplicitFile) ? (UnescapeMode.Unescape | UnescapeMode.UnescapeAll): UnescapeMode.CopyOnly;
                UnescapeString(m_String, start, m_Info.Offset.Query, result, ref count, c_DummyChar, c_DummyChar, c_DummyChar, mode);

                // Possibly convert c|\ into c:\
                if (result[1] == '|')
                    result[1] = ':';

                if (InFact(Flags.ShouldBeCompressed)) {
                    // suspecting not compressed path
                    // For a dos path we won't compress the "x:" part if found /../ sequences
                    result = Compress(result, (ushort)(IsDosPath? pathStart + 2: pathStart), ref count, m_Syntax);
                }

                // We don't know whether all slashes were the back ones
                // Plus going through Compress will turn them into / anyway
                // Converting / back into \
                for (ushort i = 0; i < (ushort) count; ++i) {
                    if (result[i] == '/') {
                        result[i] = '\\';
                    }
                }

                return new string(result, 0, count);

            }
            else {
                // Return unescaped canonical path
                // Note we cannot call GetParts here because it has circular dependancy on GelLocalPath method
                return GetUnescapedParts(UriComponents.Path | UriComponents.KeepDelimiter, UriFormat.Unescaped);
            }
        }

        //
        //
        //  Gets the escaped Uri.AbsolutePath and Uri.Query
        //  properties separated by a "?" character.
        public string PathAndQuery {
            get {
                if (IsNotAbsoluteUri) {
                    throw new InvalidOperationException(SR.GetString(SR.net_uri_NotAbsolute));
                }

                string result = GetParts(UriComponents.PathAndQuery, UriFormat.UriEscaped);
                if (IsDosPath && result[0] == '/')
                {
                    result = result.Substring(1);
                }
                return result;
            }
        }
        //
        //
        //
        //
        public int Port {
            get {
                if (IsNotAbsoluteUri) {
                    throw new InvalidOperationException(SR.GetString(SR.net_uri_NotAbsolute));
                }

                if (m_Syntax.IsSimple)
                    EnsureUriInfo();
                else
                {
                    // For a custom parser we request HostString creation that will aso set the port
                    EnsureHostString(false);
                }

                if (InFact(Flags.NotDefaultPort)) {
                    return (int)m_Info.Offset.PortValue;
                }
                return m_Syntax.DefaultPort;
            }
        }
        //
        //
        //
        //  Gets the escaped query.
        public string Query {
            get {
                if (IsNotAbsoluteUri) {
                    throw new InvalidOperationException(SR.GetString(SR.net_uri_NotAbsolute));
                }

                UriInfo info = EnsureUriInfo();
                if ((object)info.MoreInfo == null) {
                    info.MoreInfo = new MoreInfo();
                }
                string result = info.MoreInfo.Query;
                if ((object)result == null) {
                    result = GetParts(UriComponents.Query | UriComponents.KeepDelimiter, UriFormat.UriEscaped);
                    info.MoreInfo.Query = result;
                }
                return result;
            }
        }
        //
        //
        //
        //    Gets the escaped fragment.
        public string Fragment {
            get {
                if (IsNotAbsoluteUri) {
                    throw new InvalidOperationException(SR.GetString(SR.net_uri_NotAbsolute));
                }

                UriInfo info = EnsureUriInfo();
                if ((object)info.MoreInfo == null) {
                    info.MoreInfo = new MoreInfo();
                }
                string result = info.MoreInfo.Fragment;
                if ((object)result == null) {
                    result = GetParts(UriComponents.Fragment | UriComponents.KeepDelimiter, UriFormat.UriEscaped);
                    info.MoreInfo.Fragment = result;
                }
                return result;
            }
        }

        //
        //  Gets the Scheme string of this Uri
        //
        //
        public string Scheme {
            get {
                if (IsNotAbsoluteUri) {
                    throw new InvalidOperationException(SR.GetString(SR.net_uri_NotAbsolute));
                }

                return m_Syntax.SchemeName;
            }
        }

        //
        //    Gets the exact string passed by a user.
        public String OriginalString {
            get {
                return m_String;
            }
        }

        //
        //    Gets the host string that is unescaped and if it's Ipv6 host,
        //    then the returned string is suitable for DNS lookup.
        //
        //    For Ipv6 this will strip [] and add ScopeId if was found in the original string
        public string DnsSafeHost {
            get {

                if (IsNotAbsoluteUri) {
                    throw new InvalidOperationException(SR.GetString(SR.net_uri_NotAbsolute));
                }

                EnsureHostString(false);

                // Special case, will include ScopeID and strip [] around IPv6
                // This will also unescape the host string
                string ret = m_Info.Host;

                if (HostType == Flags.IPv6HostType)
                {
                    ret = ret.Substring(1, ret.Length - 2);
                    if ((object)m_Info.ScopeId != null)
                    {
                        ret += m_Info.ScopeId;
                    }
                }
                // We do not unescape anything but Basic host
                else if (HostType == Flags.BasicHostType && InFact(Flags.HostNotCanonical | Flags.E_HostNotCanonical))
                {
                    char[] dest = new char[ret.Length];
                    int count = 0;
                    UnescapeString(ret, 0, ret.Length, dest, ref count, c_DummyChar, c_DummyChar, c_DummyChar, UnescapeMode.CopyOnly);
                    ret = new string(dest, 0, count);
                }

                return ret;
            }
        }

        //
        //  Returns false if the string passed in the constructor cannot be parsed as
        //  valid AbsoluteUri. This could be a relative Uri instead.
        //
        public bool IsAbsoluteUri {
            get {
                return m_Syntax != null;
            }
        }

        //
        //
        //  Gets an array of the segments that make up a URI.
        public string[] Segments {
            get {
                if (IsNotAbsoluteUri) {
                    throw new InvalidOperationException(SR.GetString(SR.net_uri_NotAbsolute));
                }


                string[] segments = null; // used to be a class cached result
                if (segments == null) {

                    string path = PrivateAbsolutePath;

                    if (path.Length == 0) {
                        segments = new string[0];
                    }
                    else {
                        System.Collections.ArrayList pathSegments = new System.Collections.ArrayList();
                        int current = 0;
                        while (current < path.Length) {
                            int next = path.IndexOf('/', current);
                            if (next == -1) {
                                next = path.Length - 1;
                            }
                            pathSegments.Add(path.Substring(current, (next - current) + 1));
                            current = next + 1;
                        }
                        segments = (string[]) (pathSegments.ToArray(typeof(string)));
                    }
                }
                return segments;
            }
        }
        //
        //
        //  Returns 'true' if the 'dontEscape' parameter was set to 'true ' when the Uri instance was created.
        public bool UserEscaped {
            get {
                return InFact(Flags.UserEscaped);
            }
        }
        //
        //
        //  Gets the user name, password, and other user specific information associated
        //  with the Uniform Resource Identifier (URI).
        public string UserInfo {
            get {
                if (IsNotAbsoluteUri) {
                    throw new InvalidOperationException(SR.GetString(SR.net_uri_NotAbsolute));
                }

                return GetParts(UriComponents.UserInfo, UriFormat.UriEscaped);
            }
        }

        //
        // CheckHostName
        //
        //  Determines whether a host name authority is a valid Host name according
        //  to DNS naming rules
        //
        // Returns:
        //  true if <name> is valid else false
        //
        // Throws:
        //  Nothing
        //
        public static UriHostNameType CheckHostName(string name) {

            if ((object)name == null || name.Length == 0 || name.Length > short.MaxValue) {
                return UriHostNameType.Unknown;
            }
            int end = name.Length;
            unsafe {
                fixed (char* fixedName = name) {

                    if (name[0] == '[' && name[name.Length-1] == ']') {
                        // we require that _entire_ name is recognized as ipv6 address
                        if (IPv6AddressHelper.IsValid(fixedName, 1, ref end) && end == name.Length) {
                            return UriHostNameType.IPv6;
                        }
                    }
                    end = name.Length;
                    if (IPv4AddressHelper.IsValid(fixedName, 0 , ref end, false, false) && end == name.Length) {
                        return UriHostNameType.IPv4;
                    }
                    end = name.Length;
                    bool dummyBool = false;
                    if (DomainNameHelper.IsValid(fixedName, 0, ref end, ref dummyBool, false) && end == name.Length) {
                        return UriHostNameType.Dns;
                    }
                }

                //This checks the form without []
                end = name.Length+2;
                // we require that _entire_ name is recognized as ipv6 address
                name = "["+name+"]";
                fixed (char* newFixedName = name) {
                    if (IPv6AddressHelper.IsValid(newFixedName, 1, ref end) && end == name.Length) {
                        return UriHostNameType.IPv6;
                    }
                }
            }
            return UriHostNameType.Unknown;
        }
        //
        // CheckSchemeName
        //
        //  Determines whether a string is a valid scheme name according to RFC 2396.
        //  Syntax is:
        //      scheme = alpha *(alpha | digit | '+' | '-' | '.')
        //
        public static bool CheckSchemeName(string schemeName) {
            if (((object)schemeName == null)
                || (schemeName.Length == 0)
                || !IsAsciiLetter(schemeName[0])) {
                return false;
            }
            for (int i = schemeName.Length - 1; i > 0; --i) {
                if (!(IsAsciiLetterOrDigit(schemeName[i])
                    || (schemeName[i] == '+')
                    || (schemeName[i] == '-')
                    || (schemeName[i] == '.'))) {
                    return false;
                }
            }
            return true;
        }
        //
        // Returns:
        //  Number in the range 0..15
        //
        // Throws:
        //  ArgumentException
        //
        public static int FromHex(char digit) {
            if (((digit >= '0') && (digit <= '9'))
                || ((digit >= 'A') && (digit <= 'F'))
                || ((digit >= 'a') && (digit <= 'f'))) {
                return  (digit <= '9')
                    ? ((int)digit - (int)'0')
                    : (((digit <= 'F')
                    ? ((int)digit - (int)'A')
                    : ((int)digit - (int)'a'))
                    + 10);
            }
            throw new ArgumentException("digit");
        }
        //
        // GetHashCode
        //
        //  Overrides default function (in Object class)
        //
        //
        [SecurityPermission(SecurityAction.InheritanceDemand, Flags=SecurityPermissionFlag.Infrastructure)]
        public override int GetHashCode() {
            if (IsNotAbsoluteUri)
            {
                return CalculateCaseInsensitiveHashCode(OriginalString);
            }

            UriInfo info = EnsureUriInfo();
            if ((object)info.MoreInfo == null) {
                info.MoreInfo = new MoreInfo();
            }
            int tempHash = info.MoreInfo.Hash;
            if (tempHash == 0) {
                string chkString = info.MoreInfo.RemoteUrl;
                if ((object) chkString == null)
                    chkString = GetParts(UriComponents.HttpRequestUrl, UriFormat.SafeUnescaped);
                tempHash = CalculateCaseInsensitiveHashCode(chkString);
                if (tempHash == 0) {
                    tempHash = 0x1000000;   //making it not zero still large enough to be maped to zero by a hashtable
                }
                info.MoreInfo.Hash = tempHash;
            }
            return tempHash;
        }

        //
        // ToString
        //
        // The better implementation would be just
        //
        private const UriFormat V1ToStringUnescape = (UriFormat)0x7FFF;

        [SecurityPermission(SecurityAction.InheritanceDemand, Flags=SecurityPermissionFlag.Infrastructure)]
        public override string ToString()
        {
            if (IsNotAbsoluteUri) {
                return OriginalString;
            }

            EnsureUriInfo();
            if ((object)m_Info.String == null)
            {

                // V1.1 compat unless #353711 is appoved, otheriwse it should be just a call into GetParts() as shown below
                // m_Info.String = GetParts(UriComponents.AbsoluteUri, UriFormat.SafeUnescaped);

                if (Syntax.IsSimple)
                    m_Info.String = GetComponentsHelper(UriComponents.AbsoluteUri, V1ToStringUnescape);
                else
                    m_Info.String = GetParts(UriComponents.AbsoluteUri, UriFormat.SafeUnescaped);

            }
            return m_Info.String;
        }

        //
        //
        //  A static shortcut to Uri.Equals
        //
        [SecurityPermission(SecurityAction.InheritanceDemand, Flags=SecurityPermissionFlag.Infrastructure)]
        public static bool operator == (Uri uri1, Uri uri2) {
            if ((object)uri1 == (object)uri2) {
                return true;
            }
            if ((object)uri1 == null || (object)uri2 == null) {
                return false;
            }
            return uri2.Equals(uri1);
        }

        //
        //
        //  A static shortcut to !Uri.Equals
        //
        [SecurityPermission(SecurityAction.InheritanceDemand, Flags=SecurityPermissionFlag.Infrastructure)]
        public static bool operator != (Uri uri1, Uri uri2) {
            if ((object)uri1 == (object)uri2) {
                return false;
            }

            if ((object)uri1 == null || (object)uri2 == null) {
                return true;
            }

            return !uri2.Equals(uri1);
        }



        //
        // Equals
        //
        //  Overrides default function (in Object class)
        //
        // Assumes:
        //  <comparand> is an object of class Uri
        //
        // Returns:
        //  true if objects have the same value, else false
        //
        // Throws:
        //  Nothing
        //
        [SecurityPermission(SecurityAction.InheritanceDemand, Flags=SecurityPermissionFlag.Infrastructure)]
        public override bool Equals(object comparand) {
            if ((object) comparand == null) {
                return false;
            }

            if ((object)this == (object)comparand) {
                return true;
            }

            Uri obj = comparand as Uri;

            //
            // we allow comparisons of Uri and String objects only. If a string
            // is passed, convert to Uri. This is inefficient, but allows us to
            // canonicalize the comparand, making comparison possible
            //
            if ((object)obj == null) {
                string s = comparand as string;

                if ((object)s == null)
                    return false;

                if (!TryCreate(s, UriKind.RelativeOrAbsolute, out obj))
                    return false;
            }

            // Since v1.0 two Uris are equal if everything but fragment and UserInfo does match

            // This check is for a case where we already fixed up the equal references
            if ((object)this.m_String == (object)obj.m_String) {
                return true;
            }

            if (IsAbsoluteUri != obj.IsAbsoluteUri)
                return false;

            if (IsNotAbsoluteUri)
                return OriginalString.Equals(obj.OriginalString);

            if (NotAny(Flags.AllUriInfoSet) || obj.NotAny(Flags.AllUriInfoSet)) {
                // Try raw compare for m_Strings as the last chance to keep the working set small
                if (!IsUncOrDosPath ) {
                    if (m_String.Length == obj.m_String.Length) {
                        unsafe {
                            // Try case sensitive compare on m_Strings
                            fixed (char* pMe = m_String) {
                                fixed (char* pShe = obj.m_String) {
                                    // This will never go negative since m_String is checked to be a valid URI
                                    int i = (m_String.Length-1);
                                    for ( ;i >= 0 ; --i) {
                                        if (*(pMe+i) != *(pShe+i)) {
                                            break;
                                        }
                                    }
                                    if (i == -1) {
                                        return true;
                                    }
                                }
                            }
                        }
                    }
                }
                else if (String.Compare(m_String, obj.m_String, StringComparison.OrdinalIgnoreCase) == 0) {
                    return true;
                }
            }

            // Note that equality test will bring the working set of both
            // objects up to creation of m_Info.MoreInfo member
            EnsureUriInfo();
            obj.EnsureUriInfo();

            if (!UserDrivenParsing && !obj.UserDrivenParsing && Syntax.IsSimple && obj.Syntax.IsSimple)
            {
                // Optimization of canonical DNS names by avoiding host string creation.
                // Note there could be explicit ports specified that would invalidate path offsets
                if (InFact(Flags.CanonicalDnsHost) && obj.InFact(Flags.CanonicalDnsHost)) {
                    ushort i1 = m_Info.Offset.Host;
                    ushort end1 = m_Info.Offset.Path;

                    ushort i2 = obj.m_Info.Offset.Host;
                    ushort end2 = obj.m_Info.Offset.Path;
                    string str = obj.m_String;
                    //Taking the shortest part
                    if (end1-i1 > end2-i2) {
                        end1 = (ushort)(i1 + end2-i2);
                    }
                    // compare and break on ':' if found
                    while (i1 < end1) {
                        if (m_String[i1] != str[i2]) {
                            return false;
                        }
                        if (str[i2] == ':') {
                            // The other must have ':' too to have equal host
                            break;
                        }
                        ++i1;++i2;
                    }

                    // The longest host must have ':' or be of the same size
                    if (i1 < m_Info.Offset.Path && m_String[i1] != ':') {
                        return false;
                    }
                    if (i2 < end2 && str[i2] != ':') {
                        return false;
                    }
                    //hosts are equal!
                }
                else {
                    EnsureHostString(false);
                    obj.EnsureHostString(false);
                    if (!m_Info.Host.Equals(obj.m_Info.Host)) {
                        return false;
                    }
                }

                if (Port != obj.Port) {
                    return false;
                }
            }


            UriInfo meInfo  = m_Info;
            UriInfo sheInfo = obj.m_Info;
            if ((object)meInfo.MoreInfo == null) {
                meInfo.MoreInfo = new MoreInfo();
            }
            if ((object)sheInfo.MoreInfo == null) {
                sheInfo.MoreInfo = new MoreInfo();
            }

            // NB: To avoid a race condition when creating MoreInfo field
            // "meInfo" and "sheInfo" shall remain as local copies.
            string me = meInfo.MoreInfo.RemoteUrl;
            if ((object)me == null) {
                me = GetParts(UriComponents.HttpRequestUrl, UriFormat.SafeUnescaped);
                meInfo.MoreInfo.RemoteUrl = me;
            }
            string she = sheInfo.MoreInfo.RemoteUrl;
            if ((object)she == null) {
                she = obj.GetParts(UriComponents.HttpRequestUrl, UriFormat.SafeUnescaped);
                sheInfo.MoreInfo.RemoteUrl = she;
            }

            if (!IsUncOrDosPath ) {
                if (me.Length != she.Length) {
                    return false;
                }
                unsafe {
                    // Try case sensitive compare on m_Strings
                    fixed (char* pMe = me) {
                        fixed (char* pShe = she) {
                            char *endMe  = pMe  + me.Length;
                            char *endShe = pShe + me.Length;
                            while (endMe != pMe) {
                                if (*--endMe != *--endShe) {
                                    return false;
                                }
                            }
                            return true;
                        }
                    }
                }
            }


            // if IsUncOrDosPath is true then we ignore case in the path comparison
            // Get Unescaped form as most safe for the comparison
            // Fragment AND UserInfo are ignored
            //
            return (String.Compare(meInfo.MoreInfo.RemoteUrl,
                                   sheInfo.MoreInfo.RemoteUrl,
                                   IsUncOrDosPath ? StringComparison.OrdinalIgnoreCase : StringComparison.Ordinal ) == 0);
        }
        //
        // GetLeftPart
        //
        //  Returns part of the URI based on the parameters:
        //
        // Inputs:
        //  <argument>  part
        //      Which part of the URI to return
        //
        // Returns:
        //  The requested substring
        //
        // Throws:
        //  UriFormatException if URI type doesn't have host-port or authority parts
        //
        public string GetLeftPart(UriPartial part) {
            if (IsNotAbsoluteUri) {
                throw new InvalidOperationException(SR.GetString(SR.net_uri_NotAbsolute));
            }

            EnsureUriInfo();
            const UriComponents NonPathPart = (UriComponents.Scheme | UriComponents.UserInfo | UriComponents.Host | UriComponents.Port);

            switch (part) {
                case UriPartial.Scheme:

                    return GetParts(UriComponents.Scheme | UriComponents.KeepDelimiter, UriFormat.UriEscaped);

                case UriPartial.Authority:

                    if (NotAny(Flags.AuthorityFound) || IsDosPath) {




                        return String.Empty;
                    }
                    return GetParts(NonPathPart, UriFormat.UriEscaped);

                case UriPartial.Path:
                    return GetParts(NonPathPart | UriComponents.Path, UriFormat.UriEscaped);

                case UriPartial.Query:
                    return GetParts(NonPathPart | UriComponents.Path | UriComponents.Query, UriFormat.UriEscaped);

            }
            throw new ArgumentException("part");
        }

        //
        //
        /// Transforms a character into its hexadecimal representation.
        public static string HexEscape(char character) {
            if (character > '\xff') {
                throw new ArgumentOutOfRangeException("character");
            }
            char[] chars = new char[3];
            int pos = 0;
            EscapeAsciiChar(character, chars, ref pos);
            return new string(chars);
        }

        //
        // HexUnescape
        //
        //  Converts a substring of the form "%XX" to the single character represented
        //  by the hexadecimal value XX. If the substring s[Index] does not conform to
        //  the hex encoding format then the character at s[Index] is returned
        //
        // Inputs:
        //  <argument>  pattern
        //      String from which to read the hexadecimal encoded substring
        //
        //  <argument>  index
        //      Offset within <pattern> from which to start reading the hexadecimal
        //      encoded substring
        //
        // Outputs:
        //  <argument>  index
        //      Incremented to the next character position within the string. This
        //      may be EOS if this was the last character/encoding within <pattern>
        //
        // Returns:
        //  Either the converted character if <pattern>[<index>] was hex encoded, or
        //  the character at <pattern>[<index>]
        //
        // Throws:
        //  ArgumentOutOfRangeException
        //

        public static char HexUnescape(string pattern, ref int index) {
            if ((index < 0) || (index >= pattern.Length)) {
                throw new ArgumentOutOfRangeException("index");
            }
            if ((pattern[index] == '%')
                && (pattern.Length - index >= 3)) {
                char ret = EscapedAscii(pattern[index + 1], pattern[index + 2]);
                if (ret != c_DummyChar) {
                    index += 3;
                    return ret;
                }
            }
            return pattern[index++];
        }

        //
        // IsHexDigit
        //
        //  Determines whether a character is a valid hexadecimal digit in the range
        //  [0..9] | [A..F] | [a..f]
        //
        // Inputs:
        //  <argument>  character
        //      Character to test
        //
        // Returns:
        //  true if <character> is a hexadecimal digit character
        //
        // Throws:
        //  Nothing
        //
        public static bool IsHexDigit(char character) {
            return ((character >= '0') && (character <= '9'))
                || ((character >= 'A') && (character <= 'F'))
                || ((character >= 'a') && (character <= 'f'));
        }

        //
        // IsHexEncoding
        //
        //  Determines whether a substring has the URI hex encoding format of '%'
        //  followed by 2 hexadecimal characters
        //
        // Inputs:
        //  <argument>  pattern
        //      String to check
        //
        //  <argument>  index
        //      Offset in <pattern> at which to check substring for hex encoding
        //
        // Assumes:
        //  0 <= <index> < <pattern>.Length
        //
        // Returns:
        //  true if <pattern>[<index>] is hex encoded, else false
        //
        // Throws:
        //  Nothing
        //
        public static bool IsHexEncoding(string pattern, int index) {
            if ((pattern.Length - index) < 3) {
                return false;
            }
            if ((pattern[index] == '%') && EscapedAscii(pattern[index + 1], pattern[index + 1]) != c_DummyChar) {
                return true;
            }
            return false;
        }

        //
        // MakeRelative (toUri)
        //
        //  Return a relative path which when applied to this Uri would create the
        //  resulting Uri <toUri>
        //
        // Inputs:
        //  <argument>  toUri
        //      Uri to which we calculate the transformation from this Uri
        //
        // Returns:
        //  If the 2 Uri are common except for a relative path difference, then that
        //  difference, else the display name of this Uri
        //
        // Throws:
        //  Nothing
        //
        [Obsolete("The method has been deprecated. Please use MakeRelativeUri(Uri uri). http://go.microsoft.com/fwlink/?linkid=14202")]
        public string MakeRelative(Uri toUri)
        {
            if (IsNotAbsoluteUri || toUri.IsNotAbsoluteUri)
                throw new InvalidOperationException(SR.GetString(SR.net_uri_NotAbsolute));

            if ((Scheme == toUri.Scheme) && (Host == toUri.Host) && (Port == toUri.Port))
                return PathDifference(AbsolutePath, toUri.AbsolutePath, !IsUncOrDosPath);

            return toUri.ToString();
        }
        //
        public Uri MakeRelativeUri(Uri uri)
        {
            if (IsNotAbsoluteUri || uri.IsNotAbsoluteUri)
                throw new InvalidOperationException(SR.GetString(SR.net_uri_NotAbsolute));

            // Note that the UserInfo part is ignored when computing a relative Uri.
            if ((Scheme == uri.Scheme) && (Host == uri.Host) && (Port == uri.Port))
                return new Uri(
                            PathDifference(AbsolutePath, uri.AbsolutePath, !IsUncOrDosPath) + uri.GetParts(UriComponents.Query | UriComponents.Fragment, UriFormat.UriEscaped),
                            UriKind.Relative
                            );

            return uri;
        }

        //
        // http://host/Path/Path/File?Query is the base of
        //      - http://host/Path/Path/File/ ...    (those "File" words may be different in semantic but anyway)
        //      - http://host/Path/Path/#Fragment
        //      - http://host/Path/Path/?Query
        //      - http://host/Path/Path/MoreDir/ ...
        //      - http://host/Path/Path/OtherFile?Query
        //      - http://host/Path/Path/Fl
        //      - http://host/Path/Path/
        //
        //  It is not a base for
        //      - http://host/Path/Path         (that last "Path" is not considered as a directory)
        //      - http://host/Path/Path?Query
        //      - http://host/Path/Path#Fragment
        //      - http://host/Path/Path2/
        //      - http://host/Path/Path2/MoreDir
        //      - http://host/Path/File
        //
        // ASSUMES that strings like http://host/Path/Path/MoreDir/../../  have been canonicalized before going to this method.
        // ASSUMES that back slashes already have been converted if applicable.
        //
        private static unsafe bool TestForSubPath(  char* pMe,  ushort meLength,
                                                    char* pShe, ushort sheLength,
                                                    bool ignoreCase) {
            ushort i = 0;
            char chMe;
            char chShe;

            bool AllSameBeforeSlash = true;

            for( ;i < meLength && i < sheLength; ++i)
            {
                chMe  = *(pMe+i);
                chShe = *(pShe+i);

                if (chMe == '?' || chMe == '#') {
                    // survived so far and pMe does not have any more path segments
                    return true;
                }

                // If pMe terminates a path segment, so must pShe
                if (chMe == '/') {
                    if (chShe != '/') {
                        // comparison has falied
                        return false;
                    }
                    // plus the segments must be the same
                    if (!AllSameBeforeSlash) {
                        // comparison has falied
                        return false;
                    }
                    //so far so good
                    AllSameBeforeSlash = true;
                    continue;
                }

                // if pShe terminates then pMe must not have any more path segments
                if (chShe == '?' || chShe == '#') {
                    break;
                }

                if (!ignoreCase) {
                    if (chMe != chShe) {
                        AllSameBeforeSlash = false;
                    }
                }
                else {
                    if (Char.ToLower(chMe, CultureInfo.InvariantCulture) != Char.ToLower(chShe, CultureInfo.InvariantCulture)) {
                        AllSameBeforeSlash = false;
                    }
                }
            }

            // If me is longer then it must not have any more path segments
            for (; i < meLength; ++i) {
                if ((chMe = *(pMe+i)) == '?' || chMe == '#') {
                    return true;
                }
                if (chMe == '/') {
                    return false;
                }
            }
            //survived by getting to the end of pMe
            return true;
        }
        internal unsafe static string InternalEscapeString(string rawString) {
            if ((object)rawString == null)
                return String.Empty;

            int position = 0;
            char[] dest = EscapeString(rawString, 0, rawString.Length, null, ref position, true, '?', '#', '%');
            if ((object)dest == null)
                return rawString;

            return new string(dest, 0, position);
        }

        //
        //  This method is called first to figure out the scheme or a simple file path
        //  Is called only at the .ctor time
        //
        private static unsafe ParsingError ParseScheme(string uriString, ref Flags flags, ref UriParser syntax)
        {
            if (uriString.Length == 0)
                return ParsingError.EmptyUriString;

            // we don;t work with >= 64k Uris
            if (uriString.Length >= c_MaxUriBufferSize)
                return ParsingError.SizeLimit;

            //STEP1: parse scheme, lookup this Uri Syntax or create one using UnknownV1SyntaxFlags uri syntax template
            fixed (char* pUriString = uriString)
            {
                ParsingError err = ParsingError.None;
                ushort idx = ParseSchemeCheckImplicitFile(pUriString, (ushort)uriString.Length, ref err, ref flags, ref syntax);

                if (err != ParsingError.None)
                    return err;

                flags |= (Flags)idx;
             }
             return ParsingError.None;
        }

        //
        // A wrapper for ParseMinimal() called from a user parser
        // It signals back that the call has been done
        // plus it communicates back a flag for an error if any
        //
        internal UriFormatException ParseMinimal()
        {
            ParsingError result = PrivateParseMinimal();
            if (result == ParsingError.None)
                return null;

            // Means the we think the Uri is invalid, bu that can be later overriden by a user parser
            m_Flags |= Flags.ErrorOrParsingRecursion;

            return GetException(result);
        }
        //
        //
        //  This method tries to parse the minimal information needed to certify the valifity
        //  of a uri string
        //
        //      scheme://userinfo@host:Port/Path?Query#Fragment
        //
        //  The method must be called only at the .ctor time
        //
        //  Returns ParsingError.None if the Uri syntax is valid, an error otheriwse
        //
        private unsafe ParsingError PrivateParseMinimal()
        {
            ushort idx = (ushort) (m_Flags & Flags.IndexMask);
            ushort length = (ushort) m_String.Length;

            // Means a custom UriParser did call "base" InitializeAndValidate()
            m_Flags &= ~(Flags.IndexMask | Flags.UserDrivenParsing);

            //STEP2: Parse up to the port
            fixed (char* pUriString = m_String)
            {
                // Cut trailing spaces in m_String
                if (length > idx && IsLWS(pUriString[length-1]))
                {
                    --length;
                    while (length != idx && IsLWS(pUriString[--length]))
                        ;
                    ++length;
                }

                //
#if !PLATFORM_UNIX
                if (m_Syntax.IsAllSet(UriSyntaxFlags.AllowEmptyHost|UriSyntaxFlags.AllowDOSPath) && NotAny(Flags.ImplicitFile) && (idx+1 < length)) {

                    char c;
                    ushort i = (ushort) idx;

                    for (; i < length; ++i) {
                        if (!((c=pUriString[i])== '\\' || c == '/'))
                            break;
                    }

                    if (m_Syntax.InFact(UriSyntaxFlags.FileLikeUri) || i-idx <= 3) {
                        // if more than one slash after the scheme, the authority is present
                        if (i-idx >= 2) {
                            m_Flags |= Flags.AuthorityFound;
                        }
                        // DOS-like path?
                        if (i+1 < (ushort) length  && ((c=pUriString[i+1]) == ':' || c == '|') && IsAsciiLetter(pUriString[i])) {

                            if (i+2 >= (ushort) length || ((c=pUriString[i+2]) != '\\' && c != '/'))
                            {
                                // report an error but only for a file: scheme
                                if (m_Syntax.InFact(UriSyntaxFlags.FileLikeUri))
                                    return ParsingError.MustRootedPath;
                            }
                            else
                            {
                                // This will set IsDosPath
                                m_Flags |= Flags.DosPath;

                                if (m_Syntax.InFact(UriSyntaxFlags.MustHaveAuthority)) {
                                    // when DosPath found and Authority is required, set this flag even if Authority is empty
                                    m_Flags |= Flags.AuthorityFound;
                                }
                                if (i != idx && i-idx != 2) {
                                    //This will remember that DosPath is rooted
                                    idx = (ushort)(i-1);
                                }
                                else {
                                    idx = i;
                                }
                            }
                        }
                        else if (m_Syntax.InFact(UriSyntaxFlags.FileLikeUri) && (i-idx >= 2 && i-idx != 3 && i < length && pUriString[i] != '?' && pUriString[i] != '#'))
                        {
                            m_Flags |= Flags.UncPath;
                            idx = i;
                        }
                    }
                }
#endif // !PLATFORM_UNIX
                //
                //STEP 1.5 decide on the Authority component
                //
#if !PLATFORM_UNIX
                if ((m_Flags & (Flags.UncPath|Flags.DosPath)) != 0) {
                }
#else
                if ((m_Flags & Flags.ImplicitFile) != 0) {
                    // Already parsed up to the path
                }
#endif // !PLATFORM_UNIX
                else if ((idx+2) <= length) {
                    char first  = pUriString[idx];
                    char second = pUriString[idx+1];

                    if (m_Syntax.InFact(UriSyntaxFlags.MustHaveAuthority)) {
                        // (V1.0 compatiblity) This will allow http:\\ http:\/ http:/\
#if !PLATFORM_UNIX
                        if ((first == '/' || first == '\\') && (second == '/' || second == '\\'))
#else
                        if (first == '/' && second == '/')
#endif // !PLATFORM_UNIX
                        {
                            m_Flags |= Flags.AuthorityFound;
                            idx+=2;
                        }
                        else {
                            return ParsingError.BadAuthority;
                        }
                    }
                    else if (m_Syntax.InFact(UriSyntaxFlags.OptionalAuthority) && (InFact(Flags.AuthorityFound) || (first == '/' && second == '/'))) {
                        m_Flags |= Flags.AuthorityFound;
                        idx+=2;
                    }
                    else if (m_Syntax.NotAny(UriSyntaxFlags.MailToLikeUri)) {
                        m_Flags |= ((Flags)idx | Flags.UnknownHostType);
                        return ParsingError.None;
                    }
                }
                else if (m_Syntax.InFact(UriSyntaxFlags.MustHaveAuthority)) {
                    return ParsingError.BadAuthority;
                }
                else if (m_Syntax.NotAny(UriSyntaxFlags.MailToLikeUri)) {
                    m_Flags |= ((Flags)idx | Flags.UnknownHostType);
                    return ParsingError.None;
                }

#if !PLATFORM_UNIX
                // The following sample taken from the original parser comments makes the whole story sad
                // vsmacros://c:\path\file
                // Note that two slashes say there must be an Authority but instead the path goes
                // Fro V1 compat the next block allow this case but not for schemes like http
                if (InFact(Flags.DosPath)) {

                    m_Flags |= (((m_Flags & Flags.AuthorityFound)!= 0)? Flags.BasicHostType :Flags.UnknownHostType);
                    m_Flags |= (Flags)idx;
                    return ParsingError.None;
                }
#endif // !PLATFORM_UNIX

                //STEP 2: Check the syntax of authority expecting at least one character in it
                //
                // Note here we do know that there is an authority in the string OR it's a DOS path

                // We may find a userInfo and the port when parsing an authority
                // Also we may find a registry based authority.
                // We must ensure that known schemes do use a server-based authority
            {
                ParsingError err = ParsingError.None;
                idx = CheckAuthorityHelper(pUriString, idx, (ushort)length, ref err, ref m_Flags, m_Syntax);
                if (err != ParsingError.None)
                    return err;

                // This will disallow '\' as the host terminator for any scheme that is not implicitFile or cannot have a Dos Path
                if ((idx < (ushort)length && pUriString[idx] == '\\') && NotAny(Flags.ImplicitFile) && m_Syntax.NotAny(UriSyntaxFlags.AllowDOSPath))
                    return ParsingError.BadAuthorityTerminator;

            }

                // The Path (or Port) parsing index is reloaded on demand in CreateUriInfo when accessing a Uri property
                m_Flags |= (Flags)idx;

                // The rest of the string will be parsed on demand
                // The Host/Authorty is all checked, the type is known but the host value string
                // is not created/canonicalized at this point.
            }
            return ParsingError.None;
        }

        //
        //
        // The method is called when we have to access m_Info members
        // This will create the m_Info based on the copied parser context
        // Under milti-threading race this method may do duplicated yet harmless work
        //
        private void CreateUriInfo(Flags cF) {

            UriInfo info = new UriInfo();
            // This will be revisited in ParseRemaining but for now just have it at least m_String.Length
            info.Offset.End = (ushort)m_String.Length;

            if (UserDrivenParsing)
                goto Done;

            ushort idx;
            bool notCanonicalScheme = false;

            // The m_String may have leading spaces, figure that out
            // plus it will set idx value for next steps
            if (StaticInFact(cF, Flags.ImplicitFile)) {
                idx = (ushort)0;
                while (IsLWS(m_String[idx])) {
                    ++idx;
                    ++info.Offset.Scheme;
                }

#if !PLATFORM_UNIX
                if (StaticInFact(cF, Flags.UncPath)) {
                    // For implicit file AND Unc only
                    idx += 2;
                    //skip any other slashes (compatibility with V1.0 parser)
                    while(idx < (ushort)(cF & Flags.IndexMask) && (m_String[idx] == '/' || m_String[idx] == '\\')) {
                        ++idx;
                    }
                }
#endif // !PLATFORM_UNIX
            }
            else {
                // This is NOT an ImplicitFile uri
                idx = (ushort)m_Syntax.SchemeName.Length;

                while (m_String[idx++] != ':') {
                    ++info.Offset.Scheme;
                }

                if (StaticInFact(cF, Flags.AuthorityFound))
                {
                    if (m_String[idx] == '\\' || m_String[idx+1] == '\\')
                        notCanonicalScheme = true;

                    idx+=2;
#if !PLATFORM_UNIX
                    if (StaticInFact(cF, Flags.UncPath|Flags.DosPath)) {
                        // Skip slashes if it was allowed during ctor time
                        // NB: Today this is only allowed if a Unc or DosPath was found after the scheme
                        while( idx < (ushort)(cF & Flags.IndexMask) && (m_String[idx] == '/' || m_String[idx] == '\\')) {
                            notCanonicalScheme = true;
                            ++idx;
                        }
                    }
#endif // !PLATFORM_UNIX
                }
            }

            // This is weird but some schemes (mailto) do not have Authority-based syntax, still they do have a port
            if (m_Syntax.DefaultPort != UriParser.NoDefaultPort)
                info.Offset.PortValue = (ushort)m_Syntax.DefaultPort;

            //Here we set the indexes for already parsed components
            if ((cF & Flags.HostTypeMask) == Flags.UnknownHostType
#if !PLATFORM_UNIX
                || StaticInFact(cF, Flags.DosPath)
#endif // !PLATFORM_UNIX
                ) {
                //there is no Authotity component defined
                info.Offset.User  = (ushort) (cF & Flags.IndexMask);
                info.Offset.Host = info.Offset.User;
                info.Offset.Path = info.Offset.User;
                cF &= ~Flags.IndexMask;
                if (notCanonicalScheme) {
                    cF |= Flags.SchemeNotCanonical;
                }
                goto Done;
            }

            info.Offset.User = idx;

            //Basic Host Type does not have userinfo and port
            if (HostType == Flags.BasicHostType) {
                info.Offset.Host = idx;
                info.Offset.Path = (ushort) (cF & Flags.IndexMask);
                cF &= ~Flags.IndexMask;
                goto Done;
            }

            if (StaticInFact(cF, Flags.HasUserInfo)) {
                // we previously found a userinfo, get it again
                while (m_String[idx] != '@') {
                    ++idx;
                }
                ++idx;
                info.Offset.Host = idx;
            }
            else {
                info.Offset.Host = idx;
            }

            //Now reload the end of the parsed host
            idx = (ushort) (cF & Flags.IndexMask);

            //From now on we do not need IndexMask bits, and reuse the space for X_NotCanonical flags
            //clear them now
            cF &= ~Flags.IndexMask;

            // If this is not canonical, don't count on user input to be good
            if (notCanonicalScheme) {
                cF |= Flags.SchemeNotCanonical;
            }

            //Guessing this is a path start
            info.Offset.Path = idx;

            // parse Port if any. The new spec allows a port after ':' to be empty (assuming default?)
            bool notEmpty = false;
            // Note we already checked on general port syntax in ParseMinimal()
            if (idx < info.Offset.End && m_String[idx] == ':')
            {
                int port = 0;

                //Check on some noncanonical cases http://host:0324/, http://host:03, http://host:0, etc
                if (++idx < info.Offset.End) {
                    port = (ushort)(m_String[idx] - '0');
                    if (!(port == unchecked((ushort)('/' - '0')) || port == (ushort)('?' - '0') || port == unchecked((ushort)('#' - '0')))) {
                        notEmpty = true;
                        if(port == 0) {
                            cF |= (Flags.PortNotCanonical | Flags.E_PortNotCanonical);
                        }
                        for (++idx; idx < info.Offset.End; ++idx) {
                            ushort val = (ushort)((ushort)m_String[idx] - (ushort)'0');
                            if (val == unchecked((ushort)('/' - '0')) || val == (ushort)('?' - '0') || val == unchecked((ushort)('#' - '0'))) {
                                break;
                            }
                            port = (port * 10 + val);
                        }
                    }
                }
                if (notEmpty && info.Offset.PortValue != (ushort)port) {
                    info.Offset.PortValue = (ushort)port;
                    cF |= Flags.NotDefaultPort;
                }
                else {
                    //This will tell that we do have a ':' but the port value does
                    //not follow to canonical rules
                    cF |= (Flags.PortNotCanonical | Flags.E_PortNotCanonical);
                }
                info.Offset.Path = (ushort)idx;
            }

        Done:
            cF |= Flags.MinimalUriInfoSet;
/*********
            //                                                                                                   
            int copyF = m_Flags;
            while ((copyF & Flags.MinimalUriInfoSet) == 0)
            {
               if (copyF != (copyF = Interlocked.CompareExchange(ref m_Flags, cF | (copyF & ~Flags.IndexMask), copyF))
                   continue;
               m_Info  = info;
            }
*********/

            lock (m_String)
            {
                if (NotAny(Flags.MinimalUriInfoSet))
                {
                    m_Info  = info;
                    m_Flags = (m_Flags & ~Flags.IndexMask) | cF;
                }
            }

        }

        //
        // This will create a Host string. The validity has been already checked
        //
        // Assuming: UriInfo memeber is already set at this point
        private unsafe void CreateHostString() {
            //
            // Mutlithrreading!
            //
            if (!m_Syntax.IsSimple)
            {
                lock (m_Info)
                {
                    // ATTN: Avoid possible recursion through CreateHostString->Syntax.GetComponents->Uri.GetComponentsHelper->CreateHostString
                    if (NotAny(Flags.ErrorOrParsingRecursion))
                    {
                        m_Flags |= Flags.ErrorOrParsingRecursion;
                        // Need to get host string through the derived type
                        GetHostViaCustomSyntax();
                        m_Flags &= ~Flags.ErrorOrParsingRecursion;
                        return;
                    }
                }
            }
            Flags flags = m_Flags;
            string host = CreateHostStringHelper(m_String, m_Info.Offset.Host, m_Info.Offset.Path, ref flags, ref m_Info.ScopeId);

            // now check on canonical host representation
            if (host.Length != 0)
            {
                // An Authority may need escaping except when it's an inet server address
                //
                // We do not escape UNC names and will get rid of this type when switching to IDN spec
                //
                if (HostType == Flags.BasicHostType) {
                    ushort idx = 0;
                    Check result;
                    fixed (char* pHost = host) {
                        result = CheckCanonical(pHost, ref idx, (ushort)host.Length, c_DummyChar);
                    }

                    if ((result & Check.DisplayCanonical) == 0) {
                        // For implicit file the user string must be in perfect display format,
                        // Hence, ignoring complains from CheckCanonical()
                        if (NotAny(Flags.ImplicitFile) || (result & Check.ReservedFound) != 0) {
                            flags |= Flags.HostNotCanonical;
                        }
                    }

                    if (InFact(Flags.ImplicitFile) && (result & (Check.ReservedFound | Check.EscapedCanonical)) != 0) {
                        // need to re-escape this host if any escaped sequence was found
                        result &= ~Check.EscapedCanonical;
                    }

                    if ((result & (Check.EscapedCanonical|Check.BackslashInPath)) != Check.EscapedCanonical) {
                        flags |= Flags.E_HostNotCanonical;
                        if (NotAny(Flags.UserEscaped))
                        {
                            int position = 0;
                            char[] dest = EscapeString(host, 0, host.Length, null, ref position, true, '?', '#', IsImplicitFile? c_DummyChar: '%');
                            if ((object)dest != null)
                                host = new string(dest, 0, position);
                        }
                        else {
                        }
                    }
                }
                else if (NotAny(Flags.CanonicalDnsHost)){
                    // Check to see if we can take the canonical host string out of m_String
                    if ((object)m_Info.ScopeId != null) {
                        // IPv6 ScopeId is included when serializing a Uri
                        flags |= (Flags.HostNotCanonical | Flags.E_HostNotCanonical);
                    }
                    else for (ushort i=0 ; i < host.Length; ++i) {
                             if ((m_Info.Offset.Host + i) >= m_Info.Offset.End || host[i] != m_String[m_Info.Offset.Host + i]) {
                                 flags |= (Flags.HostNotCanonical | Flags.E_HostNotCanonical);
                                 break;
                             }
                         }
                }
            }

            m_Info.Host = host;
            lock (m_Info)
            {
                m_Flags |= flags;
            }
        }
        //
        private static string CreateHostStringHelper(string str, ushort idx, ushort end, ref Flags flags, ref string scopeId)
        {
            bool loopback = false;
            string host;
            switch (flags & Flags.HostTypeMask) {

                case Flags.DnsHostType:
                    host = DomainNameHelper.ParseCanonicalName(str, idx, end, ref loopback);
                    break;

                case Flags.IPv6HostType:
                    host = IPv6AddressHelper.ParseCanonicalName(str, idx, ref loopback, ref scopeId);
                    break;

                case Flags.IPv4HostType:
                    host = IPv4AddressHelper.ParseCanonicalName(str, idx, end, ref loopback);
                    break;

#if !PLATFORM_UNIX
                case Flags.UncHostType:
                    host = UncNameHelper.ParseCanonicalName(str, idx, end, ref loopback);
                    break;
#endif // !PLATFORM_UNIX

                case Flags.BasicHostType:
#if !PLATFORM_UNIX
                    if (StaticInFact(flags, Flags.DosPath)) {
                        host = string.Empty;
                    }
                    else
#endif // !PLATFORM_UNIX
                    {
                        // This is for a registry-based authority, not relevant for known schemes
                        host = str.Substring(idx, end-idx);
                    }
                    // A empty host would count for a loopback
                    if (host.Length == 0) {
                        loopback = true;
                    }
                    //there will be no port
                    break;

                case Flags.UnknownHostType:
                    //means the host is *not expected* for this uri type
                    host = string.Empty;
                    break;

                default: //it's a bug
                    throw GetException(ParsingError.BadHostName);
            }

            if (loopback) {
                flags |= Flags.LoopbackHost;
            }
            return host;
        }
        //
        // Called under lock()
        //
        private unsafe void GetHostViaCustomSyntax()
        {
            // A multithreading check
            if (m_Info.Host != null)
                return;

            string host = m_Syntax.InternalGetComponents(this, UriComponents.Host, UriFormat.UriEscaped);

            // ATTN: Check on whether recursion has not happened
            if ((object)m_Info.Host == null)
            {
                if (host.Length >= c_MaxUriBufferSize)
                    throw GetException(ParsingError.SizeLimit);

                ParsingError err = ParsingError.None;
                Flags flags = m_Flags & ~Flags.HostTypeMask;

                fixed (char *pHost = host)
                {
                    if (CheckAuthorityHelper(pHost, 0, (ushort)host.Length, ref err, ref flags, m_Syntax) != (ushort)host.Length)
                    {
                        // We cannot parse the entire host string
                        flags &= ~Flags.HostTypeMask;
                        flags |= Flags.UnknownHostType;
                    }
                }

                if (err != ParsingError.None || (flags & Flags.HostTypeMask) == Flags.UnknownHostType)
                {
                    // Well, custom parser has returned a not known host type, take it as Basic then.
                    m_Flags = (m_Flags & ~Flags.HostTypeMask) | Flags.BasicHostType;
                }
                else
                {
                    host = CreateHostStringHelper(host, 0, (ushort)host.Length, ref flags, ref m_Info.ScopeId);
                    for (ushort i=0 ; i < host.Length; ++i) {
                             if ((m_Info.Offset.Host + i) >= m_Info.Offset.End || host[i] != m_String[m_Info.Offset.Host + i]) {
                                 m_Flags |= (Flags.HostNotCanonical | Flags.E_HostNotCanonical);
                                 break;
                             }
                         }
                    m_Flags = (m_Flags & ~Flags.HostTypeMask) | (flags & Flags.HostTypeMask);
                }
            }
            //
            // This is a chance for a custom parser to report a different port value
            //
            string portStr = m_Syntax.InternalGetComponents(this, UriComponents.StrongPort, UriFormat.UriEscaped);
            int port = 0;
            if ((object)portStr == null || portStr.Length == 0)
            {
                // It's like no port
                m_Flags &= ~Flags.NotDefaultPort;
                m_Flags |= (Flags.PortNotCanonical|Flags.E_PortNotCanonical);
                m_Info.Offset.PortValue = 0;
            }
            else
            {
                for (int idx=0; idx < portStr.Length; ++idx)
                {
                    int val = portStr[idx] - '0';
                    if (val < 0 || val > 9 || (port = (port * 10 + val)) > 0xFFFF)
                        throw new UriFormatException(SR.GetString(SR.net_uri_PortOutOfRange, m_Syntax.GetType().FullName, portStr));
                }
                if (port != m_Info.Offset.PortValue)
                {
                    if (port == m_Syntax.DefaultPort)
                        m_Flags &= ~Flags.NotDefaultPort;
                    else
                        m_Flags |= Flags.NotDefaultPort;

                    m_Flags |= (Flags.PortNotCanonical|Flags.E_PortNotCanonical);
                    m_Info.Offset.PortValue = (ushort) port;
                }
            }
            // This must be done as the last thing in this method
            m_Info.Host = host;
        }
        //
        // An internal shortcut into Uri extenisiblity API
        //
        internal string GetParts(UriComponents uriParts, UriFormat formatAs)
        {
            return GetComponents(uriParts, formatAs);
        }

        //
        //
        //
        private string GetEscapedParts(UriComponents uriParts) {
            // Which Uri parts are not escaped canonically ?
            // Notice that public UriPart and private Flags must me in Sync so below code can work
            //
            ushort  nonCanonical = (ushort)(((ushort)m_Flags & ((ushort)Flags.CannotDisplayCanonical<<7)) >> 6);
            if (InFact(Flags.SchemeNotCanonical)) {
                nonCanonical |= (ushort)Flags.SchemeNotCanonical;
            }

            // We keep separate flags for some of path canonicalization facts
            if ((uriParts & UriComponents.Path) != 0) {
                if (InFact(Flags.ShouldBeCompressed|Flags.FirstSlashAbsent|Flags.BackslashInPath)) {
                    nonCanonical |= (ushort)Flags.PathNotCanonical;
                }
                else if (IsDosPath && m_String[m_Info.Offset.Path + SecuredPathIndex - 1] == '|') {
                    // A rare case of c|\
                    nonCanonical |= (ushort)Flags.PathNotCanonical;
                }
            }

            if (((ushort)uriParts & nonCanonical) == 0) {
                string ret = GetUriPartsFromUserString(uriParts);
                if ((object)ret != null) {
                    return ret;
                }
            }

            return ReCreateParts(uriParts, nonCanonical, UriFormat.UriEscaped);
        }

        private string GetUnescapedParts(UriComponents uriParts, UriFormat formatAs) {
            // Which Uri parts are not escaped canonically ?
            // Notice that public UriComponents and private Uri.Flags must me in Sync so below code can work
            //
            ushort  nonCanonical = (ushort)((ushort)m_Flags & (ushort)Flags.CannotDisplayCanonical);

            // We keep separate flags for some of path canonicalization facts
            if ((uriParts & UriComponents.Path) != 0) {
                if (InFact(Flags.ShouldBeCompressed|Flags.FirstSlashAbsent|Flags.BackslashInPath)) {
                    nonCanonical |= (ushort)Flags.PathNotCanonical;
                }
                else if (IsDosPath && m_String[m_Info.Offset.Path + SecuredPathIndex - 1] == '|') {
                    // A rare case of c|\
                    nonCanonical |= (ushort)Flags.PathNotCanonical;
                }

            }

            if (((ushort)uriParts & nonCanonical) == 0) {
                string ret = GetUriPartsFromUserString(uriParts);
                if ((object)ret != null) {
                    return ret;
                }
            }

            return ReCreateParts(uriParts, nonCanonical, formatAs);
        }

        //
        //
        //
        private string ReCreateParts(UriComponents parts, ushort nonCanonical, UriFormat formatAs)
        {
            // going hard core
            EnsureHostString(false);
            string stemp = (parts & UriComponents.Host) == 0? string.Empty: m_Info.Host;
            // we reserve more space than required because a canonical Ipv6 Host
            // may take more characteres than in original m_String
            // Also +3 is for :// and +1 is for absent first slash
            // Also we may escape every character, hence multiplying by 6
            int count = (m_Info.Offset.End-m_Info.Offset.User) * (formatAs == UriFormat.UriEscaped?6:1);
            char[] chars = new char[stemp.Length + count + m_Syntax.SchemeName.Length + 3 + 1];
            count = 0;

            //Scheme and slashes
            if ((parts & UriComponents.Scheme) != 0) {
                m_Syntax.SchemeName.CopyTo(0, chars, count, m_Syntax.SchemeName.Length);
                count += m_Syntax.SchemeName.Length;
                if (parts != UriComponents.Scheme) {
                    chars[count++] = ':';
                    if (InFact(Flags.AuthorityFound)) {
                        chars[count++] = '/';
                        chars[count++] = '/';
                    }
                }
            }

            //UserInfo
            if ((parts & UriComponents.UserInfo) != 0 && InFact(Flags.HasUserInfo))
            {
                if ((nonCanonical & (ushort)UriComponents.UserInfo) != 0) {
                    switch (formatAs) {
                        case UriFormat.UriEscaped:
                            if (NotAny(Flags.UserEscaped))
                            {
                                chars = EscapeString(m_String, m_Info.Offset.User, m_Info.Offset.Host, chars, ref count, true, '?', '#', '%');
                            }
                            else {
                                if (InFact(Flags.E_UserNotCanonical)) {
                                }
                                m_String.CopyTo(m_Info.Offset.User, chars, count, m_Info.Offset.Host - m_Info.Offset.User);
                                count += (m_Info.Offset.Host - m_Info.Offset.User);
                            }
                            break;

                        case UriFormat.SafeUnescaped:
                            chars = UnescapeString(m_String, m_Info.Offset.User, m_Info.Offset.Host-1, chars, ref count, '@', '/', '\\', InFact(Flags.UserEscaped)? UnescapeMode.Unescape: UnescapeMode.EscapeUnescape);
                            chars[count++] = '@';
                            break;

                        case UriFormat.Unescaped:
                            chars = UnescapeString(m_String, m_Info.Offset.User, m_Info.Offset.Host, chars, ref count, c_DummyChar, c_DummyChar, c_DummyChar, UnescapeMode.Unescape | UnescapeMode.UnescapeAll);
                            break;

                        default: //V1ToStringUnescape
                            chars = UnescapeString(m_String, m_Info.Offset.User, m_Info.Offset.Host, chars, ref count, c_DummyChar, c_DummyChar, c_DummyChar, UnescapeMode.CopyOnly);
                            break;
                    }
                }
                else {
                    UnescapeString(m_String, m_Info.Offset.User, m_Info.Offset.Host, chars, ref count, c_DummyChar, c_DummyChar, c_DummyChar, UnescapeMode.CopyOnly);
                }
                if (parts == UriComponents.UserInfo)
                {
                    //strip '@' delimiter
                    --count;
                }
            }

            // Host
            if ((parts & UriComponents.Host) != 0 && stemp.Length != 0)
            {
                UnescapeMode mode;
                if (formatAs != UriFormat.UriEscaped && HostType == Flags.BasicHostType && (nonCanonical & (ushort)UriComponents.Host) != 0) {
                    // only Basic host could be in the escaped form
                    mode = formatAs == UriFormat.Unescaped? (UnescapeMode.Unescape | UnescapeMode.UnescapeAll):
                                            (InFact(Flags.UserEscaped)? UnescapeMode.Unescape: UnescapeMode.EscapeUnescape);

                }
                else {
                    mode = UnescapeMode.CopyOnly;
                }
                chars = UnescapeString(stemp, 0, stemp.Length, chars, ref count, '/', '?', '#', mode);


                // A fix up only for SerializationInfo and IpV6 host with a scopeID
                if ((parts & UriComponents.SerializationInfoString) != 0 && HostType == Flags.IPv6HostType && (object)m_Info.ScopeId != null)
                {
                    m_Info.ScopeId.CopyTo(0, chars, count-1, m_Info.ScopeId.Length);
                    count += m_Info.ScopeId.Length;
                    chars[count-1] = ']';
                }
            }

            //Port (always wants a ':' delimiter if got to this method)
            if ((parts & UriComponents.Port) != 0)
            {
                if ((nonCanonical & (ushort)UriComponents.Port) == 0) {
                    //take it from m_String
                    if (InFact(Flags.NotDefaultPort)) {
                        ushort start = m_Info.Offset.Path;
                        while (m_String[--start] != ':') {
                            ;
                        }
                        m_String.CopyTo(start, chars, count, m_Info.Offset.Path - start);
                        count += (m_Info.Offset.Path - start);
                    }
                    else if ((parts & UriComponents.StrongPort) != 0 && m_Syntax.DefaultPort != UriParser.NoDefaultPort) {
                        chars[count++]= ':';
                        stemp = m_Info.Offset.PortValue.ToString(CultureInfo.InvariantCulture);
                        stemp.CopyTo(0, chars, count, stemp.Length);
                        count += stemp.Length;
                    }
                }
                else if (InFact(Flags.NotDefaultPort) || ((parts & UriComponents.StrongPort) != 0 && m_Syntax.DefaultPort != UriParser.NoDefaultPort)) {
                    // recreate string from port value
                    chars[count++]= ':';
                    stemp = m_Info.Offset.PortValue.ToString(CultureInfo.InvariantCulture);
                    stemp.CopyTo(0, chars, count, stemp.Length);
                    count += stemp.Length;
                }
            }

            ushort delimiterAwareIndex;

            //Path
            if ((parts & UriComponents.Path) != 0)
            {
                chars = GetCanonicalPath(chars, ref count, formatAs);

                // (possibly strip the leading '/' delimiter)
                if (parts == UriComponents.Path)
                {
                    if (InFact(Flags.AuthorityFound) && count !=0 && chars[0] == '/')
                    {
                        delimiterAwareIndex = 1; --count;
                    }
                    else
                    {
                        delimiterAwareIndex = 0;
                    }
                    return count == 0? string.Empty: new string(chars, delimiterAwareIndex, count);
                }
            }

            //Query (possibly strip the '?' delimiter)
            if ((parts & UriComponents.Query) != 0 && m_Info.Offset.Query < m_Info.Offset.Fragment)
            {
                delimiterAwareIndex = (ushort)(m_Info.Offset.Query+1);
                if(parts != UriComponents.Query)
                    chars[count++] = '?';   //see Fragment+1 below

                if ((nonCanonical & (ushort)UriComponents.Query) != 0)
                {
                    switch (formatAs)
                    {
                    case UriFormat.UriEscaped:
                        //Can Assert IsImplicitfile == false
                        if (NotAny(Flags.UserEscaped))
                            chars = EscapeString(m_String, delimiterAwareIndex, m_Info.Offset.Fragment, chars, ref count, true,  '#', c_DummyChar, '%');
                        else
                        {
                            UnescapeString(m_String, delimiterAwareIndex, m_Info.Offset.Fragment, chars, ref count, c_DummyChar, c_DummyChar, c_DummyChar, UnescapeMode.CopyOnly);
                        }
                        break;

                    case V1ToStringUnescape:

                        chars = UnescapeString(m_String, delimiterAwareIndex, m_Info.Offset.Fragment, chars, ref count, '#', c_DummyChar, c_DummyChar,
                                               (InFact(Flags.UserEscaped)? UnescapeMode.Unescape: UnescapeMode.EscapeUnescape) | UnescapeMode.V1ToStringFlag);
                        break;

                    case UriFormat.Unescaped:

                        chars = UnescapeString(m_String, delimiterAwareIndex, m_Info.Offset.Fragment, chars, ref count, '#', c_DummyChar, c_DummyChar,
                                               (UnescapeMode.Unescape| UnescapeMode.UnescapeAll));
                        break;

                    default: // UriFormat.SafeUnescaped

                        chars = UnescapeString(m_String, delimiterAwareIndex, m_Info.Offset.Fragment, chars, ref count, '#', c_DummyChar, c_DummyChar,
                                               (InFact(Flags.UserEscaped)? UnescapeMode.Unescape: UnescapeMode.EscapeUnescape));
                        break;
                    }
                }
                else
                {
                    UnescapeString(m_String, delimiterAwareIndex, m_Info.Offset.Fragment, chars, ref count, c_DummyChar, c_DummyChar, c_DummyChar, UnescapeMode.CopyOnly);
                }
            }

            //Fragment (possibly strip the '#' delimiter)
            if ((parts & UriComponents.Fragment) != 0 && m_Info.Offset.Fragment < m_Info.Offset.End)
            {
                delimiterAwareIndex = (ushort)(m_Info.Offset.Fragment+1);
                if(parts != UriComponents.Fragment)
                    chars[count++] = '#';   //see Fragment+1 below

                if ((nonCanonical & (ushort)UriComponents.Fragment) != 0)
                {
                    switch (formatAs) {
                    case UriFormat.UriEscaped:
                            if (NotAny(Flags.UserEscaped))
                                chars = EscapeString(m_String, delimiterAwareIndex, m_Info.Offset.End, chars, ref count, true, '#', c_DummyChar, '%');
                            else
                            {
                                UnescapeString(m_String, delimiterAwareIndex, m_Info.Offset.End, chars, ref count, c_DummyChar, c_DummyChar, c_DummyChar, UnescapeMode.CopyOnly);
                            }
                            break;

                    case V1ToStringUnescape:

                        chars = UnescapeString(m_String, delimiterAwareIndex, m_Info.Offset.End, chars, ref count, '#', c_DummyChar, c_DummyChar,
                                               (InFact(Flags.UserEscaped)? UnescapeMode.Unescape: UnescapeMode.EscapeUnescape) | UnescapeMode.V1ToStringFlag);
                        break;
                    case UriFormat.Unescaped:

                        chars = UnescapeString(m_String, delimiterAwareIndex, m_Info.Offset.End, chars, ref count, '#', c_DummyChar, c_DummyChar,
                                               UnescapeMode.Unescape | UnescapeMode.UnescapeAll);
                        break;

                    default: // UriFormat.SafeUnescaped

                        chars = UnescapeString(m_String, delimiterAwareIndex, m_Info.Offset.End, chars, ref count, '#', c_DummyChar, c_DummyChar,
                                               (InFact(Flags.UserEscaped)? UnescapeMode.Unescape: UnescapeMode.EscapeUnescape));
                        break;
                    }
                }
                else
                {
                    UnescapeString(m_String, delimiterAwareIndex, m_Info.Offset.End, chars, ref count, c_DummyChar, c_DummyChar, c_DummyChar, UnescapeMode.CopyOnly);
                }
            }

            return new string(chars, 0, count);
        }

        //
        // This method is called only if the user string has a canonical representation
        // of requested parts
        //
        private string GetUriPartsFromUserString(UriComponents uriParts) {

            ushort delimiterAwareIdx;

            switch (uriParts & ~UriComponents.KeepDelimiter) {
                    // For FindServicePoint perf
                case UriComponents.Scheme | UriComponents.Host | UriComponents.Port:
                    if (!InFact(Flags.HasUserInfo))
                        return m_String.Substring(m_Info.Offset.Scheme, m_Info.Offset.Path - m_Info.Offset.Scheme);

                    return m_String.Substring(m_Info.Offset.Scheme, m_Info.Offset.User - m_Info.Offset.Scheme)
                         + m_String.Substring(m_Info.Offset.Host, m_Info.Offset.Path - m_Info.Offset.Host);

                    // For HttpWebRequest.ConnectHostAndPort perf
                case UriComponents.HostAndPort:  //Host|StrongPort

                    if (!InFact(Flags.HasUserInfo))
                        goto case UriComponents.StrongAuthority;

                    if (InFact(Flags.NotDefaultPort) || m_Syntax.DefaultPort == UriParser.NoDefaultPort)
                        return m_String.Substring(m_Info.Offset.Host, m_Info.Offset.Path - m_Info.Offset.Host);

                    return m_String.Substring(m_Info.Offset.Host, m_Info.Offset.Path - m_Info.Offset.Host)
                        + ':' + m_Info.Offset.PortValue.ToString(CultureInfo.InvariantCulture);

                    // For an obvious common case perf
                case UriComponents.AbsoluteUri:     //Scheme|UserInfo|Host|Port|Path|Query|Fragment,
                    if (m_Info.Offset.Scheme == 0 && m_Info.Offset.End == m_String.Length)
                        return m_String;

                    return m_String.Substring(m_Info.Offset.Scheme, m_Info.Offset.End - m_Info.Offset.Scheme);

                    // For Uri.Equals() and HttpWebRequest through a proxy perf
                case UriComponents.HttpRequestUrl:   //Scheme|Host|Port|Path|Query,
                    if (InFact(Flags.HasUserInfo)) {
                        return m_String.Substring(m_Info.Offset.Scheme, m_Info.Offset.User - m_Info.Offset.Scheme)
                            + m_String.Substring(m_Info.Offset.Host, m_Info.Offset.Fragment - m_Info.Offset.Host);
                    }
                    if (m_Info.Offset.Scheme == 0 && m_Info.Offset.Fragment == m_String.Length)
                        return m_String;

                    return m_String.Substring(m_Info.Offset.Scheme, m_Info.Offset.Fragment - m_Info.Offset.Scheme);

                    // For CombineUri() perf
                case UriComponents.SchemeAndServer|UriComponents.UserInfo:
                    return m_String.Substring(m_Info.Offset.Scheme, m_Info.Offset.Path - m_Info.Offset.Scheme);

                    // For Cache perf
                case (UriComponents.AbsoluteUri & ~UriComponents.Fragment):
                    if (m_Info.Offset.Scheme == 0 && m_Info.Offset.Fragment == m_String.Length)
                        return m_String;

                    return m_String.Substring(m_Info.Offset.Scheme, m_Info.Offset.Fragment - m_Info.Offset.Scheme);


                // Strip scheme delimiter if was not requested
                case UriComponents.Scheme:
                        if (uriParts != UriComponents.Scheme)
                            return m_String.Substring(m_Info.Offset.Scheme, m_Info.Offset.User - m_Info.Offset.Scheme);

                        return m_Syntax.SchemeName;

                // KeepDelimiter makes no sense for this component
                case UriComponents.Host:
                    ushort idx = m_Info.Offset.Path;
                    if (InFact(Flags.NotDefaultPort|Flags.PortNotCanonical)) {
                        //Means we do have ':' after the host
                        while (m_String[--idx] != ':')
                            ;
                    }
                    return (idx - m_Info.Offset.Host == 0)? string.Empty: m_String.Substring(m_Info.Offset.Host, idx - m_Info.Offset.Host);

                case UriComponents.Path:

                    // Strip the leading '/' for a hierarchical URI if no delimiter was requested
                    if (uriParts == UriComponents.Path && InFact(Flags.AuthorityFound) && m_Info.Offset.End > m_Info.Offset.Path &&  m_String[m_Info.Offset.Path] == '/')
                        delimiterAwareIdx = (ushort)(m_Info.Offset.Path + 1);
                    else
                        delimiterAwareIdx = m_Info.Offset.Path;

                    if (delimiterAwareIdx >= m_Info.Offset.Query)
                        return string.Empty;


                    return m_String.Substring(delimiterAwareIdx, m_Info.Offset.Query - delimiterAwareIdx);

                case UriComponents.Query:
                    // Strip the '?' if no delimiter was requested
                    if (uriParts == UriComponents.Query)
                        delimiterAwareIdx = (ushort)(m_Info.Offset.Query + 1);
                    else
                        delimiterAwareIdx = m_Info.Offset.Query;

                    if (delimiterAwareIdx >= m_Info.Offset.Fragment)
                        return string.Empty;

                    return m_String.Substring(delimiterAwareIdx, m_Info.Offset.Fragment - delimiterAwareIdx);

                case UriComponents.Fragment:
                    // Strip the '#' if no delimiter was requested
                    if (uriParts == UriComponents.Fragment)
                        delimiterAwareIdx = (ushort)(m_Info.Offset.Fragment + 1);
                    else
                        delimiterAwareIdx = m_Info.Offset.Fragment;

                    if (delimiterAwareIdx >= m_Info.Offset.End)
                        return string.Empty;

                    return m_String.Substring(delimiterAwareIdx, m_Info.Offset.End - delimiterAwareIdx);

                case UriComponents.UserInfo | UriComponents.Host | UriComponents.Port:
                    return (m_Info.Offset.Path - m_Info.Offset.User == 0)? string.Empty: m_String.Substring(m_Info.Offset.User, m_Info.Offset.Path - m_Info.Offset.User);

                case UriComponents.StrongAuthority:  //UserInfo|Host|StrongPort
                    if (InFact(Flags.NotDefaultPort) || m_Syntax.DefaultPort == UriParser.NoDefaultPort)
                        goto case UriComponents.UserInfo | UriComponents.Host | UriComponents.Port;

                    return m_String.Substring(m_Info.Offset.User, m_Info.Offset.Path - m_Info.Offset.User)
                        + ':' + m_Info.Offset.PortValue.ToString(CultureInfo.InvariantCulture);

                case UriComponents.PathAndQuery:        //Path|Query,
                    return m_String.Substring(m_Info.Offset.Path, m_Info.Offset.Fragment - m_Info.Offset.Path);

                case UriComponents.HttpRequestUrl|UriComponents.Fragment: //Scheme|Host|Port|Path|Query|Fragment,
                    if (InFact(Flags.HasUserInfo)) {
                        return m_String.Substring(m_Info.Offset.Scheme, m_Info.Offset.User - m_Info.Offset.Scheme)
                            + m_String.Substring(m_Info.Offset.Host, m_Info.Offset.End - m_Info.Offset.Host);
                    }
                    if (m_Info.Offset.Scheme == 0 && m_Info.Offset.End == m_String.Length)
                        return m_String;

                    return m_String.Substring(m_Info.Offset.Scheme, m_Info.Offset.End - m_Info.Offset.Scheme);

                case UriComponents.PathAndQuery|UriComponents.Fragment:  //LocalUrl|Fragment
                    return m_String.Substring(m_Info.Offset.Path, m_Info.Offset.End - m_Info.Offset.Path);

                case UriComponents.UserInfo:
                        // Strip the '@' if no delimiter was requested

                    if (NotAny(Flags.HasUserInfo))
                        return string.Empty;

                    if (uriParts == UriComponents.UserInfo)
                        delimiterAwareIdx = (ushort)(m_Info.Offset.Host - 1);
                    else
                        delimiterAwareIdx = m_Info.Offset.Host;

                    if (m_Info.Offset.User >= delimiterAwareIdx)
                        return string.Empty;

                    return m_String.Substring(m_Info.Offset.User, delimiterAwareIdx - m_Info.Offset.User);

                default:
                    return null;
            }
        }


        //
        //This method does:
        //  - Creates m_Info member
        //  - checks all componenets up to path on their canonical representation
        //  - continues parsing starting the path position
        //  - Sets the offsets of remaining components
        //  - Sets the Canonicalization flags if applied
        //  - Will NOT create MoreInfo members
        //
        private unsafe void ParseRemaining() {

            // ensure we parsed up to the path
            EnsureUriInfo();

            Flags cF = Flags.Zero;

            if (UserDrivenParsing)
                goto Done;

            // Multithreading!
            // m_Info.Offset values may be parsed twice but we lock only on m_Flags update.

            fixed (char* str = m_String) {
                ushort idx = m_Info.Offset.Scheme;
                ushort length = (ushort) m_String.Length;

                // Cut trailing spaces in m_String
                if (length > idx && IsLWS(str[length-1]))
                {
                    --length;
                    while (length != idx && IsLWS(str[--length]))
                        ;
                    ++length;
                }

                if (IsImplicitFile) {
                    cF |= Flags.SchemeNotCanonical;
                }
                else {
                    ushort i = 0;
                    for (; i < (ushort)m_Syntax.SchemeName.Length; ++i)
                    {
                        if (m_Syntax.SchemeName[i] != str[idx + i])
                            cF |= Flags.SchemeNotCanonical;
                    }
                    // For an authority Uri only // after the scheme would be canonical
                    // (compatibility bug http:\\host)
                    if (InFact(Flags.AuthorityFound) && (idx+i+3 >= length || str[idx+i+1] != '/' || str[idx+i+2] != '/')) {
                        cF |= Flags.SchemeNotCanonical;
                    }
                }

                Check result = Check.None;

                //Check the form of the user info
                if (InFact(Flags.HasUserInfo)) {
                    idx = m_Info.Offset.User;
                    result = CheckCanonical(str, ref idx, m_Info.Offset.Host, '@');
                    if ((result & Check.DisplayCanonical) == 0) {
                        cF |= Flags.UserNotCanonical;
                    }
                    if ((result & (Check.EscapedCanonical|Check.BackslashInPath)) != Check.EscapedCanonical) {
                        cF |= Flags.E_UserNotCanonical;
                    }
                }

                //
                // Delay canonical Host checking to avoid creation of a host string
                // Will do that on demand.
                //


                //
                //We have already checked on the port in EnsureUriInfo() that calls CreateUriInfo
                //

                //
                // Parsing the Path if any
                //
                idx = m_Info.Offset.Path;
                //Some uris do not have a query
                //    When '?' is passed as delimiter, then it's special case
                //    so both '?' and '#' will work as delimiters
                if (IsImplicitFile || !m_Syntax.InFact(UriSyntaxFlags.MayHaveQuery|UriSyntaxFlags.MayHaveFragment)) {
                    result = CheckCanonical(str, ref idx, length, c_DummyChar);
                }
                else {
                    result = CheckCanonical(str, ref idx, length, (m_Syntax.InFact(UriSyntaxFlags.MayHaveQuery) ? '?': m_Syntax.InFact(UriSyntaxFlags.MayHaveFragment)? '#': c_EOL));
                }

                // ATTN:
                // This may render problems for unknown schemes, but in general for an authority based Uri
                // (that has slashes) a path should start with "/"
                // This becomes more interesting knowning how a file uri is used in "file://c:/path"
                // It will be converted to file:///c:/path
                //
                // However, even more interesting is that vsmacros://c:\path will not add the third slash in the _canoical_ case
                // (vsmacros inventors have violated the RFC)
                //
                // We use special syntax flag to check if the path is rooted, i.e. has a first slash
                //
                if (InFact(Flags.AuthorityFound) && m_Syntax.InFact(UriSyntaxFlags.PathIsRooted)
                    && (m_Info.Offset.Path == length || (str[m_Info.Offset.Path] != '/' && str[m_Info.Offset.Path] != '\\'))) {
                    cF |= Flags.FirstSlashAbsent;
                }

                // Check the need for compression or backslashes conversion
                // we included IsDosPath since it may come with other than FILE uri, for ex. scheme://C:\path
                // (This is very unfortunate that the original design has included that feature)
                if (IsDosPath || (InFact(Flags.AuthorityFound) && m_Syntax.InFact(UriSyntaxFlags.CompressPath|UriSyntaxFlags.ConvertPathSlashes|UriSyntaxFlags.UnEscapeDotsAndSlashes)))
                {
                    if (m_Syntax.InFact(UriSyntaxFlags.UnEscapeDotsAndSlashes) && (result & Check.DotSlashEscaped) != 0)
                        cF |= (Flags.E_PathNotCanonical|Flags.PathNotCanonical);

                    if (m_Syntax.InFact(UriSyntaxFlags.ConvertPathSlashes) && (result & Check.BackslashInPath) != 0)
                        cF |= (Flags.E_PathNotCanonical|Flags.PathNotCanonical);

                    if (m_Syntax.InFact(UriSyntaxFlags.CompressPath) && ((cF & Flags.E_PathNotCanonical) != 0 || (result & Check.DotSlashAttn) != 0))
                        cF |= Flags.ShouldBeCompressed;

                    if ((result & Check.BackslashInPath) != 0)
                        cF |= Flags.BackslashInPath;
                }
                else if ((result & Check.BackslashInPath) != 0) {
                    // for a "generic" path '\' should be escaped
                    cF |= Flags.E_PathNotCanonical;
                }

                if ((result & Check.DisplayCanonical) == 0) {
                    if (NotAny(Flags.ImplicitFile) || InFact(Flags.UserEscaped) || (result & Check.ReservedFound) != 0) {
                        //means it's found as escaped or has unescaped Reserved Characters
                        cF |= Flags.PathNotCanonical;
                    }
                }

                if (InFact(Flags.ImplicitFile) && (result & (Check.ReservedFound | Check.EscapedCanonical)) != 0) {
                    // need to escape reserved chars or re-escape '%' if an "escaped sequence" was found
                    result &= ~Check.EscapedCanonical;
                }

                if ((result & Check.EscapedCanonical) == 0) {
                    //means it's found as not completely escaped
                    cF |= Flags.E_PathNotCanonical;
                }

                //
                //Now we've got to parse the Query if any. Note that Query requires the presense of '?'
                //
                m_Info.Offset.Query = idx;
                if (idx < length && str[idx] == '?') {
                    ++idx; // This is to exclude first '?' character from checking
                    result = CheckCanonical(str, ref idx, length, m_Syntax.InFact(UriSyntaxFlags.MayHaveFragment)? '#': c_EOL);
                    if ((result & Check.DisplayCanonical) == 0) {
                        cF |= Flags.QueryNotCanonical;
                    }

                    if ((result & (Check.EscapedCanonical|Check.BackslashInPath)) != Check.EscapedCanonical) {
                        cF |= Flags.E_QueryNotCanonical;
                    }
                }

                //
                //Now we've got to parse the Fragment if any. Note that Fragment requires the presense of '#'
                //

                m_Info.Offset.Fragment = idx;
                if (idx < length && str[idx] == '#') {
                    ++idx; // This is to exclude first '#' character from checking
                    result = CheckCanonical(str, ref idx, length, c_EOL);     //We don't using c_DummyChar since want to allow '?' and '#' as unescaped
                    if ((result & Check.DisplayCanonical) == 0) {
                        cF |= Flags.FragmentNotCanonical;
                    }

                    if ((result & (Check.EscapedCanonical|Check.BackslashInPath)) != Check.EscapedCanonical) {
                        cF |= Flags.E_FragmentNotCanonical;
                    }
                }

                m_Info.Offset.End = idx;
            }
        Done:
            cF |= Flags.AllUriInfoSet;
            lock (m_Info)
            {
                m_Flags |= cF;
            }
        }

        //
        //
        // verifies the syntax of the scheme part
        // Checks on implicit File: scheme due to simple Dos/Unc path passed
        // returns the start of the next component  position
        // throws UriFormatException if invalid scheme
        //
        unsafe static private ushort ParseSchemeCheckImplicitFile(char *uriString, ushort length, ref ParsingError err, ref Flags flags, ref UriParser syntax) {

            ushort idx = 0;

            //skip whitespaces
            while(idx < length && IsLWS(uriString[idx])) {
                ++idx;
            }


            ushort end = idx;
            while (end < length && uriString[end] != ':') {
                ++end;
            }

            // NB: On 64-bits we will use less optimized code from CheckSchemeSyntax()
            //
            if (IntPtr.Size == 4) {
                // long = 4chars: The minimal size of a known scheme is 3 + ':'
                if (end != length && end >= idx+3 && CheckKnownSchemes((long*) (uriString + idx), (ushort)(end-idx), ref syntax)) {
                    return (ushort)(end+1);
                }
            }

            //NB: A string must have at least 3 characters and at least 1 before ':'
            if (idx+2 >= length || end == idx) {
                err = ParsingError.BadFormat;
                return 0;
            }

            //Check for supported special cases like a DOS file path OR a UNC share path
            //NB: A string may not have ':' if this is a UNC path
        {
            char c;
            if ((c=uriString[idx+1]) == ':' || c == '|') {
#if !PLATFORM_UNIX
                //DOS-like path?
                if (IsAsciiLetter(uriString[idx])) {
                    if((c=uriString[idx+2]) == '\\' || c== '/') {
                        flags |= (Flags.DosPath|Flags.ImplicitFile|Flags.AuthorityFound);
                        syntax = UriParser.FileUri;
                        return idx;
                    }
                    err = ParsingError.MustRootedPath;
                    return 0;
                }
#endif // !PLATFORM_UNIX
                err = ParsingError.BadFormat;
                return 0;
            }
#if !PLATFORM_UNIX
            else if ((c=uriString[idx]) == '/' || c == '\\') {
                //UNC share ?
                if ((c=uriString[idx+1]) == '\\' || c == '/') {
                    flags |= (Flags.UncPath|Flags.ImplicitFile|Flags.AuthorityFound);
                    syntax = UriParser.FileUri;
                    idx+=2;
                    // V1.1 compat this will simply eat any slashes prepended to a UNC path
                    while (idx < length && ((c=uriString[idx]) == '/' ||  c == '\\'))
                        ++idx;

                    return idx;
                }
                err = ParsingError.BadFormat;
                return 0;
            }
#else
            else if (uriString[idx] == '/') {
                // On UNIX an implicit file has the form /<path> or scheme:///<path>
                if (idx == 0 || uriString[idx-1] != ':' ) {
                    // No scheme present; implicit /<path> starting at idx
                    flags |= (Flags.ImplicitFile|Flags.AuthorityFound);
                    syntax = UriParser.FileUri;
                    return idx;
                } else if (uriString[idx+1] == '/' && uriString[idx+2] == '/') {
                    // scheme present; rooted path starts at idx + 2
                    flags |= (Flags.ImplicitFile|Flags.AuthorityFound);
                    syntax = UriParser.FileUri;
                    idx+=2;
                    return idx;
                }
            }
            else if (uriString[idx] == '\\') {
                err = ParsingError.BadFormat;
                return 0;
            }
#endif // !PLATFORM_UNIX
        }

            if (end == length) {
                err = ParsingError.BadFormat;
                return 0;
            }

            // Here could be a possibly valid, and not well-known scheme
            // Finds the scheme delimiter
            // we don;t work with the schemes names > c_MaxUriSchemeName (should be ~1k)
            if ((end-idx) > c_MaxUriSchemeName) {
                err = ParsingError.SchemeLimit;
                return 0;
            }

            //Check the syntax, canonicalize  and avoid a GC call
            char* schemePtr = stackalloc char[end-idx];
            for (length = 0; idx < end; ++idx) {
                schemePtr[length++] = uriString[idx];
            }
            err = CheckSchemeSyntax(schemePtr, length, ref syntax);
            if (err != ParsingError.None) {
                return 0;
            }
            return (ushort)(end+1);
        }
        //
        // Quickly parses well known schemes.
        // nChars does not include the last ':'. Assuming there is one at the end of passed buffer
        unsafe static private bool CheckKnownSchemes(long *lptr, ushort nChars, ref UriParser syntax) {
            //NOTE beware of too short input buffers!

            const long _HTTP_Mask0   = 'h'|('t'<<16)|((long)'t'<<32)|((long)'p'<<48);
            const char _HTTPS_Mask1  = 's';
            const long _FTP_Mask     = 'f'|('t'<<16)|((long)'p'<<32)|((long)':'<<48);
            const long _FILE_Mask0   = 'f'|('i'<<16)|((long)'l'<<32)|((long)'e'<<48);
            const long _GOPHER_Mask0 = 'g'|('o'<<16)|((long)'p'<<32)|((long)'h'<<48);
            const int  _GOPHER_Mask1 = 'e'|('r'<<16);
            const long _MAILTO_Mask0 = 'm'|('a'<<16)|((long)'i'<<32)|((long)'l'<<48);
            const int  _MAILTO_Mask1 = 't'|('o'<<16);
            const long _NEWS_Mask0   = 'n'|('e'<<16)|((long)'w'<<32)|((long)'s'<<48);
            const long _NNTP_Mask0   = 'n'|('n'<<16)|((long)'t'<<32)|((long)'p'<<48);
            const long _UUID_Mask0   = 'u'|('u'<<16)|((long)'i'<<32)|((long)'d'<<48);

            const long _TELNET_Mask0 = 't'|('e'<<16)|((long)'l'<<32)|((long)'n'<<48);
            const int  _TELNET_Mask1 = 'e'|('t'<<16);
            
            const long _NETXXX_Mask0 = 'n'|('e'<<16)|((long)'t'<<32)|((long)'.'<<48);
            const long _NETTCP_Mask1 = 't'|('c'<<16)|((long)'p'<<32)|((long)':'<<48);
            const long _NETPIPE_Mask1 = 'p'|('i'<<16)|((long)'p'<<32)|((long)'e'<<48);
            
            const long _LDAP_Mask0   = 'l'|('d'<<16)|((long)'a'<<32)|((long)'p'<<48);


            const long _LOWERCASE_Mask = 0x0020002000200020L;
            const int  _INT_LOWERCASE_Mask = 0x00200020;


            //Map to a known scheme if possible
            //upgrade 4 letters to ASCII lower case, keep a false case to stay false
            switch (*lptr | _LOWERCASE_Mask) {
                case _HTTP_Mask0:
                    if (nChars == 4) {
                        syntax = UriParser.HttpUri;
                        return true;
                    }
                    if (nChars == 5 && ((*(char*)(lptr+1))|0x20) == _HTTPS_Mask1) {
                        syntax = UriParser.HttpsUri;
                        return true;
                    }
                    break;

                case _FILE_Mask0:
                    if (nChars == 4) {
                        syntax = UriParser.FileUri;
                        return true;
                    }
                    break;
                case _FTP_Mask:
                    if (nChars == 3) {
                        syntax = UriParser.FtpUri;
                        return true;
                    }
                    break;

                case _NEWS_Mask0:
                    if (nChars == 4) {
                        syntax = UriParser.NewsUri;
                        return true;
                    }
                    break;

                case _NNTP_Mask0:
                    if (nChars == 4) {
                        syntax = UriParser.NntpUri;
                        return true;
                    }
                    break;

                case _UUID_Mask0:
                    if (nChars == 4) {
                        syntax = UriParser.UuidUri;
                        return true;
                    }
                    break;

                case _GOPHER_Mask0:
                    if (nChars == 6 && (*(int*)(lptr+1)|_INT_LOWERCASE_Mask) == _GOPHER_Mask1) {
                        syntax = UriParser.GopherUri;
                        return true;
                    }
                    break;
                case _MAILTO_Mask0:
                    if (nChars == 6 && (*(int*)(lptr+1)|_INT_LOWERCASE_Mask) == _MAILTO_Mask1) {
                        syntax = UriParser.MailToUri;
                        return true;
                    }
                    break;

                case _TELNET_Mask0:
                    if (nChars == 6 && (*(int*)(lptr+1)|_INT_LOWERCASE_Mask) == _TELNET_Mask1) {
                        syntax = UriParser.TelnetUri;
                        return true;
                    }
                    break;    

                case _NETXXX_Mask0:
                    if (nChars == 8 && (*(lptr+1)|_LOWERCASE_Mask) == _NETPIPE_Mask1) {
                        syntax = UriParser.NetPipeUri;
                        return true;
                    }
                    else if (nChars == 7 && (*(lptr+1)|_LOWERCASE_Mask) == _NETTCP_Mask1) {
                        syntax = UriParser.NetTcpUri;
                        return true;
                    }
                    break;

                case _LDAP_Mask0:
                    if (nChars == 4) {
                        syntax = UriParser.LdapUri;
                        return true;
                    }
                    break;
                default:    break;
            }
            return false;
        }

        //
        //
        // This will check whether a scheme string follows the rules
        //
        unsafe static private ParsingError CheckSchemeSyntax(char* ptr, ushort length, ref UriParser syntax) {
            //First character must be an alpha
        {
            char c = *ptr;
            if (c >= 'a' && c <= 'z') {
                ;
            } else if (c >= 'A' && c <= 'Z') {
                *ptr = (char)(c | 0x20);    //make it lowercase
            } else {
                return ParsingError.BadScheme;
            }
        }

            for (ushort i = 1; i < length; ++i) {
                char c = ptr[i];
                if (c >= 'a' && c <= 'z') {
                    ;
                } else if (c >= 'A' && c <= 'Z') {
                    ptr[i] = (char)(c | 0x20);    //make it lowercase
                } else if (c >= '0' && c <= '9') {
                    ;
                } else if (c == '+' || c == '-' || c == '.') {
                    ;
                } else {
                    return ParsingError.BadScheme;
                }
            }
            // A not well-known scheme, needs string creation
            // Note it is already in the lower case as required.
            string str  =  new string(ptr, 0, length);
            syntax = UriParser.FindOrFetchAsUnknownV1Syntax(str);
            return ParsingError.None;
        }
        //
        //
        // Checks the syntax of an authority component. It may also get a userInfo if present
        // Returns an error if no/mailformed authority found
        // Does not NOT touch m_Info
        // Returns position of the Path component
        //
        // Must be called in the ctor only
        private static unsafe ushort CheckAuthorityHelper(char* pString, ushort idx, ushort length, ref ParsingError err, ref Flags flags, UriParser syntax) {
            int end = length;
            char ch;

            //Special case is an empty authority
            if (idx == length || ((ch=pString[idx]) == '/' || (ch == '\\' && StaticIsFile(syntax)) || ch == '#' || ch == '?'))
            {
                if (syntax.InFact(UriSyntaxFlags.AllowEmptyHost))
                {
                    flags &= ~Flags.UncPath;    //UNC cannot have an empty hostname
                    if (StaticInFact(flags, Flags.ImplicitFile))
                        err = ParsingError.BadHostName;
                    else
                        flags |= Flags.BasicHostType;
                }
                else
                    err = ParsingError.BadHostName;

                return idx;
            }

#if PLATFORM_UNIX
            if (StaticIsFile(syntax) && ch != '/') {
                // On UNIX a file URL may only have an empty authority
                err = ParsingError.NonEmptyHost;
                return idx;
            }
#endif // PLATFORM_UNIX

            ushort start = idx;

            // Attempt to parse user info first
            if (syntax.InFact(UriSyntaxFlags.MayHaveUserInfo))
            {
                for (; start < end; ++start)
                {
                    if (start == end -1 || pString[start] == '?' || pString[start] == '#' || pString[start] == '\\' || pString[start] == '/')
                    {
                        start = idx;
                        break;
                    }
                    else if (pString[start] == '@')
                    {
                        flags |= Flags.HasUserInfo;
                        ++start;
                        ch = pString[start];
                        break;
                    }
                }
            }

            // DNS name only optimization
            // Fo an overriden parsing the optimization is suppressed since hostname can be changed to anything
            bool dnsNotCanonical = !syntax.IsSimple;

            if (ch == '[' && syntax.InFact(UriSyntaxFlags.AllowIPv6Host) && IPv6AddressHelper.IsValid(pString, (int)start+1, ref end))
            {
                flags |= Flags.IPv6HostType;
            }
            else if ( ch <= '9' && ch >= '0' && syntax.InFact(UriSyntaxFlags.AllowIPv4Host) &&
                IPv4AddressHelper.IsValid(pString, (int) start, ref end, false, StaticNotAny(flags, Flags.ImplicitFile)))
            {
                flags |= Flags.IPv4HostType;
            }
            else if (syntax.InFact(UriSyntaxFlags.AllowDnsHost) &&
                DomainNameHelper.IsValid(pString, start, ref end, ref dnsNotCanonical, StaticNotAny(flags, Flags.ImplicitFile)))
            {
                flags |= Flags.DnsHostType;
                if (!dnsNotCanonical) {
                    flags |= Flags.CanonicalDnsHost;
                }
            }
#if !PLATFORM_UNIX
            else if (syntax.InFact(UriSyntaxFlags.AllowUncHost))
            {
                //
                // This must remain as the last check befor BasicHost type
                //
                if (UncNameHelper.IsValid(pString, start, ref end, StaticNotAny(flags, Flags.ImplicitFile)))
                {
                    if (end-start <= UncNameHelper.MaximumInternetNameLength)
                        flags |= Flags.UncHostType;
                }
            }
#endif // !PLATFORM_UNIX

            // The deal here is that we won't allow '\' host terminator except for the File scheme
            // If we see '\' we try to make it a part of of a Basic host
            if (end < length && pString[end] == '\\' && (flags & Flags.HostTypeMask) != Flags.HostNotParsed && !StaticIsFile(syntax))
            {
                if (syntax.InFact(UriSyntaxFlags.V1_UnknownUri))
                {
                    err = ParsingError.BadHostName;
                    flags |= Flags.UnknownHostType;
                    return (ushort) end;
                }
                flags &= ~Flags.HostTypeMask;
            }
            // Here we have checked the syntax up to the end of host
            // The only thing that can cause an exception is the port value
            // Spend some (duplicated) cycles on that.
            else if (end < length && pString[end] == ':')
            {
                if (syntax.InFact(UriSyntaxFlags.MayHavePort))
                {
                    int port = 0;
                    for (idx = (ushort)(end+1); idx < length; ++idx) {
                        ushort val = (ushort)((ushort)pString[idx] - (ushort)'0');
                        if ((val >= 0) && (val <= 9))
                        {
                            if ((port = (port * 10 + val)) > 0xFFFF)
                                break;
                        }
                        else if (val == unchecked((ushort)('/' - '0')) || val == (ushort)('?' - '0') || val == unchecked((ushort)('#' - '0')))
                        {
                            break;
                        }
                        else
                        {
                            // The second check is to keep compatibility with V1 until the UriParser is registered
                            if(syntax.InFact(UriSyntaxFlags.AllowAnyOtherHost) && syntax.NotAny(UriSyntaxFlags.V1_UnknownUri))
                            {
                                flags &= ~Flags.HostTypeMask;
                                break;
                            }
                            else
                            {
                                err = ParsingError.BadPort;
                                return idx;
                            }
                        }
                    }
                    // check on 0-ffff range
                    if (port > 0xFFFF)
                    {
                        if (syntax.InFact(UriSyntaxFlags.AllowAnyOtherHost))
                        {
                            flags &= ~Flags.HostTypeMask;
                        }
                        else
                        {
                            err = ParsingError.BadPort;
                            return idx;
                        }
                    }
                }
                else
                {
                    flags &= ~Flags.HostTypeMask;
                }
            }

            // check on whether nothing has worked out
            if ((flags & Flags.HostTypeMask) == Flags.HostNotParsed)
            {
                //No user info for a Basic hostname
                flags &= ~Flags.HasUserInfo;
                // Some schemes do not allow HostType = Basic (plus V1 almost never understands this cause of a bug)
                //
                if(syntax.InFact(UriSyntaxFlags.AllowAnyOtherHost))
                {
                    flags |= Flags.BasicHostType;
                    for (end = idx; end < length; ++end) {
                        if (pString[end] == '/' || (pString[end] == '?' || pString[end] == '#')) {
                            break;
                        }
                    }
                }
                else
                {
                    if (syntax.InFact(UriSyntaxFlags.V1_UnknownUri))
                    {
                        // Can assert here that the host is not empty so we will set dotFound
                        // at least once or fail before exiting the loop
                        bool dotFound = false;
                        for (end = idx; end < length; ++end)
                        {
                            if (dotFound && (pString[end] == '/' || pString[end] == '?' || pString[end] == '#'))
                                break;
                            else if (end < (idx + 2) && pString[end] == '.')
                            {
                                // allow one or two dots
                                dotFound = true;
                            }
                            else
                            {
                                //failure
                                err = ParsingError.BadHostName;
                                flags |= Flags.UnknownHostType;
                                return idx;
                            }
                        }
                        //success
                        flags |= Flags.BasicHostType;
                    }
                    else if(syntax.InFact(UriSyntaxFlags.MustHaveAuthority))
                    {
                        err = ParsingError.BadHostName;
                        flags |= Flags.UnknownHostType;
                        return idx;
                    }
                }
            }
            return (ushort) end;
        }


        //
        //
        // The method checks whether a string needs transformation before going to display or wire
        //
        // Parameters:
        // - escaped   true = treat all valid escape sequences as escaped sequences, false = escape all %
        // - delim     a character signalling the termination of the component being checked
        //
        // When delim=='?', then '#' character is also considered as delimiter additionally to passed '?'.
        //
        // The method pays attention to the dots and slashes so to signal potential Path compression action needed.
        // Even that is not required for other components, the cycles are still spent (little inefficiency)
        //

        const char c_DummyChar = (char) 0xFFFF;     //An Invalid Unicode character used as a dummy char passed into the parameter
        const char c_EOL       = (char) 0xFFFE;     //An Invalid Unicode character used by CheckCanonical as "no delimiter condition"
        [Flags]
        private enum Check {
            None            = 0x0,
            EscapedCanonical= 0x1,
            DisplayCanonical= 0x2,
            DotSlashAttn    = 0x4,
            DotSlashEscaped = 0x80,
            BackslashInPath = 0x10,
            ReservedFound   = 0x20
        }

        //
        // Used by ParseRemaining as well by InternalIsWellFormedOriginalString
        //
        private unsafe Check CheckCanonical(char* str, ref ushort idx, ushort end, char delim) {
            Check res = Check.None;
            bool needsEscaping = false;
            bool foundEscaping = false;

            char c = c_DummyChar;
            ushort i=idx;
            for (; i < end; ++i)
            {
                c = str[i];
                // Control chars usually should be escaped in any case
                if (c <= '\x1F' || (c >= '\x7F' && c <= '\x9F'))
                {
                    needsEscaping = true;
                    foundEscaping = true;
                    res |= Check.ReservedFound;
                }
                else if (c > 'z' && c != '~') {
                    if (!needsEscaping) needsEscaping = true;
                }
                else if (c == delim) {
                    break;
                }
                else if (delim == '?' && c == '#' && (m_Syntax != null && m_Syntax.InFact(UriSyntaxFlags.MayHaveFragment))) {
                    // this is a special case when deciding on Query/Fragment
                    break;
                }
                else if (c == '?') {
                    if (IsImplicitFile || (m_Syntax != null && !m_Syntax.InFact(UriSyntaxFlags.MayHaveQuery) && delim != c_EOL))
                    {
                        res |= Check.ReservedFound;
                        foundEscaping = true;
                        needsEscaping = true;
                    }
                }
                else if (c == '#') {
                    needsEscaping = true;
                    if (IsImplicitFile || (m_Syntax != null && !m_Syntax.InFact(UriSyntaxFlags.MayHaveFragment))) {
                        res |= Check.ReservedFound;
                        foundEscaping = true;
                    }
                }
                else if (c == '/' || c == '\\') {
                    if ((res & Check.BackslashInPath) == 0 && c == '\\') {
                        res |= Check.BackslashInPath;
                    }
                    if ((res & Check.DotSlashAttn) == 0 && i+1 != end && (str[i+1] == '/' || str[i+1] == '\\' )) {
                        res |= Check.DotSlashAttn;
                    }
                }
                else if (c == '.') {
                    if ((res & Check.DotSlashAttn) == 0 && i+1 == end || str[i+1] == '.' || str[i+1] == '/' || str[i+1] == '\\' || str[i+1] == '?' || str[i+1] == '#') {
                        res |= Check.DotSlashAttn;
                    }
                }
                else if (!needsEscaping && ((c <= '"' && c != '!') || (c >= '[' && c <= '^') || c == '>' || c == '<' || c == '`')) {
                    needsEscaping = true;
                }
                else if (c == '%') {
                    if (!foundEscaping) foundEscaping = true;
                    //try unescape a byte hex escaping
                    if (i+2 < end && (c = EscapedAscii(str[i+1], str[i+2])) != c_DummyChar)
                    {
                        if (c == '.' || c == '/' || c == '\\') {
                            res |= Check.DotSlashEscaped;
                        }
                        i+=2;
                        continue;
                    }
                    // otherwise we follow to non escaped case
                    if (!needsEscaping) {
                        needsEscaping = true;
                    }
                }
            }

            if (foundEscaping) {
                if (!needsEscaping) {
                    res |= Check.EscapedCanonical;
                }
            }
            else {
                res |= Check.DisplayCanonical;
                if (!needsEscaping) {
                    res |= Check.EscapedCanonical;
                }
            }
            idx = i;
            return res;
        }

        //
        // Returns the escaped and canonicalized path string
        // the passed array must be long enough to hold at least
        // canonical unescaped path representation (allocated by the caller)
        //
        private unsafe char[] GetCanonicalPath(char[] dest, ref int pos, UriFormat formatAs)
        {

            if (InFact(Flags.FirstSlashAbsent))
                dest[pos++] = '/';

            if (m_Info.Offset.Path == m_Info.Offset.Query)
                return dest;

            int end = pos;

            int dosPathIdx = SecuredPathIndex;

            if (formatAs == UriFormat.UriEscaped)
            {
                if (InFact(Flags.ShouldBeCompressed))
                {
                    m_String.CopyTo(m_Info.Offset.Path, dest, end, m_Info.Offset.Query - m_Info.Offset.Path);
                    end += (m_Info.Offset.Query - m_Info.Offset.Path);

                    // If the path was found as needed compression and contains escaped characters, unescape only interesting characters (safe)

                    if (m_Syntax.InFact(UriSyntaxFlags.UnEscapeDotsAndSlashes) && InFact(Flags.PathNotCanonical) && !IsImplicitFile)
                    {
                        fixed (char* pdest = dest)
                            UnescapeOnly(pdest, pos, ref end, '.', '/', m_Syntax.InFact(UriSyntaxFlags.ConvertPathSlashes)? '\\': c_DummyChar) ;
                    }
                }
                else
                {
                    if (InFact(Flags.E_PathNotCanonical) && NotAny(Flags.UserEscaped)) {
                        string str = m_String;

                        // Check on not canonical disk designation like C|\, should be rare, rare case
                        if (dosPathIdx != 0 && str[dosPathIdx + m_Info.Offset.Path -1] == '|')
                        {
                            str = str.Remove(dosPathIdx + m_Info.Offset.Path -1, 1);
                            str = str.Insert(dosPathIdx + m_Info.Offset.Path -1, ":");
                        }
                        dest = EscapeString(str, m_Info.Offset.Path, m_Info.Offset.Query, dest, ref end, true, '?', '#', IsImplicitFile? c_DummyChar: '%');
                    }
                    else {
                        m_String.CopyTo(m_Info.Offset.Path, dest, end, m_Info.Offset.Query - m_Info.Offset.Path);
                        end += (m_Info.Offset.Query - m_Info.Offset.Path);
                    }
                }
            }
            else
            {
                m_String.CopyTo(m_Info.Offset.Path, dest, end, m_Info.Offset.Query - m_Info.Offset.Path);
                end += (m_Info.Offset.Query - m_Info.Offset.Path);

                if (InFact(Flags.ShouldBeCompressed))
                {
                    // If the path was found as needed compression and contains escaped characters, unescape only interesting characters (safe)

                    if (m_Syntax.InFact(UriSyntaxFlags.UnEscapeDotsAndSlashes) && InFact(Flags.PathNotCanonical) && !IsImplicitFile)
                    {
                        fixed (char* pdest = dest)
                            UnescapeOnly(pdest, pos, ref end, '.', '/', m_Syntax.InFact(UriSyntaxFlags.ConvertPathSlashes)? '\\': c_DummyChar) ;
                    }
                }
            }

            // Here we already got output data as copied into dest array
            // We just may need more processing of that data

            //
            // if this URI is using 'non-proprietary' disk drive designation, convert to MS-style
            //
            // (path is already  >= 3 chars if recognized as a DOS-like)
            //
            if (dosPathIdx != 0 && dest[dosPathIdx + pos - 1] == '|')
                dest[dosPathIdx + pos - 1] = ':';

            if (InFact(Flags.ShouldBeCompressed))
            {
                // It will also convert back slashes if needed
                dest = Compress(dest, (ushort)(pos + dosPathIdx), ref end, m_Syntax);
                if (dest[pos] == '\\')
                    dest[pos] = '/';

                // Escape path if requested and found as not fully escaped
                if (formatAs == UriFormat.UriEscaped && NotAny(Flags.UserEscaped) && InFact(Flags.E_PathNotCanonical)) {
                    string srcString = new string(dest, pos, end-pos);
                    dest = EscapeString(srcString ,  0, end-pos, dest, ref pos, true, '?', '#', IsImplicitFile? c_DummyChar: '%');
                    end = pos;
                }
            }
            else if (m_Syntax.InFact(UriSyntaxFlags.ConvertPathSlashes) && InFact(Flags.BackslashInPath))
            {
                for (int i = pos; i < end; ++i)
                    if (dest[i] == '\\') dest[i] = '/';
            }

            if (formatAs != UriFormat.UriEscaped && InFact(Flags.PathNotCanonical))
            {
                UnescapeMode mode;
                if (InFact(Flags.PathNotCanonical))
                {
                    switch (formatAs)
                    {
                    case V1ToStringUnescape:

                        mode = (InFact(Flags.UserEscaped)? UnescapeMode.Unescape: UnescapeMode.EscapeUnescape) | UnescapeMode.V1ToStringFlag;
                        if (IsImplicitFile)
                            mode &= ~UnescapeMode.Unescape;
                        break;

                    case UriFormat.Unescaped:
                        mode = IsImplicitFile? UnescapeMode.CopyOnly : UnescapeMode.Unescape | UnescapeMode.UnescapeAll;
                        break;

                    default: // UriFormat.SafeUnescaped

                        mode = InFact(Flags.UserEscaped)? UnescapeMode.Unescape: UnescapeMode.EscapeUnescape;
                        if (IsImplicitFile)
                            mode &= ~UnescapeMode.Unescape;
                        break;
                    }
                }
                else {
                    mode = UnescapeMode.CopyOnly;
                }

                char[] dest1 = new char[dest.Length];
                Buffer.BlockCopy(dest, 0, dest1, 0, end<<1);
                fixed (char *pdest = dest1)
                {
                    dest = UnescapeString(pdest, pos, end, dest, ref pos, '?', '#', c_DummyChar, mode);
                }
            }
            else
            {
                pos = end;
            }

            return dest;
        }

        // works only with ASCII characters, used to partially unescape path before compressing
        private unsafe static void UnescapeOnly(char* pch, int start, ref int end, char ch1, char ch2, char ch3) {
            if (end - start < 3) {
                //no chance that something is escaped
                return;
            }

            char *pend = pch + end-2;
            pch += start;
            char *pnew = null;

            over:

            // Just looking for a interested escaped char
            if (pch >= pend)    goto done;
            if(*pch++ != '%')   goto over;

            char ch = EscapedAscii(*pch++, *pch++);
            if (!(ch == ch1 || ch == ch2 || ch == ch3)) goto over;

            // Here we found something and now start copying the scanned chars
            pnew = pch-2;
            *(pnew-1) = ch;

            over_new:

            if (pch >= pend)                goto done;
            if((*pnew++ = *pch++) != '%')   goto over_new;

            ch = EscapedAscii((*pnew++ = *pch++), (*pnew++ = *pch++));
            if (!(ch == ch1 || ch == ch2 || ch == ch3)) {
                goto over_new;
            }

            pnew -= 2;
            *(pnew-1) = ch;

            goto over_new;

            done:
            pend+=2;

            if (pnew == null) {
                //nothing was found
                return;
            }

            //the tail may be already processed
            if(pch == pend) {
                end -= (int) (pch-pnew);
                return;
            }

            *pnew++ = *pch++;
            if(pch == pend) {
                end -= (int) (pch-pnew);
                return;
            }
            *pnew++ = *pch++;
            end -= (int) (pch-pnew);
        }
        //
        //
        //
        //
        //
        private static char EscapedAscii(char digit, char next) {
            if (!(((digit >= '0') && (digit <= '9'))
                || ((digit >= 'A') && (digit <= 'F'))
                || ((digit >= 'a') && (digit <= 'f')))) {
                return c_DummyChar;
            }

            int res= (digit <= '9')
                ? ((int)digit - (int)'0')
                : (((digit <= 'F')
                ? ((int)digit - (int)'A')
                : ((int)digit - (int)'a'))
                   + 10);

            if (!(((next >= '0') && (next <= '9'))
                || ((next >= 'A') && (next <= 'F'))
                || ((next >= 'a') && (next <= 'f')))) {
                return c_DummyChar;
            }

            return  (char) ((res << 4) + ((next <= '9')
                    ? ((int)next - (int)'0')
                    : (((next <= 'F')
                        ? ((int)next - (int)'A')
                        : ((int)next - (int)'a'))
                       + 10)));
        }


        //
        //
        // This will compress any "\" "/../" "/./" "///" "/..../" /XXX.../, etc found in the input
        //
        // The passed syntax controls whether to use agressive compression or the one specified in RFC 2396
        //
        //
        private static char[] Compress(char[] dest, ushort start, ref int destLength, UriParser syntax)
        {
            ushort  slashCount      = 0;
            ushort  lastSlash       = 0;
            ushort  dotCount        = 0;
            ushort  removeSegments  = 0;

            unchecked {
                //ushort i == -1 and start == -1 overflow is ok here
                ushort  i = (ushort)((ushort)destLength - (ushort)1);
                start = (ushort)(start-1);

                for (; i != start ; --i) {
                    char ch = dest[i];
                    if (ch == '\\' && syntax.InFact(UriSyntaxFlags.ConvertPathSlashes)) {
                        dest[i] = ch = '/';
                    }

                    //
                    // compress multiple '/' for file URI
                    //
                    if (ch == '/') {
                        ++slashCount;
                        if (slashCount > 1) {
                            continue;
                        }
                    }
                    else {
                        if (slashCount > 1) {
                            if (syntax.InFact(UriSyntaxFlags.CanonicalizeAsFilePath))
                            {
                                // We saw > 1 slashes so remove all but the last one
                                // dest.Remove(i+1, slashCount -1);
                                Buffer.BlockCopy(dest, (i+slashCount)<<1, dest, (i+1)<<1, (destLength - (i + slashCount))<<1);
                                destLength -= (slashCount-1);
                            }
                            // else preserve repeated slashes
                            lastSlash = (ushort)(i + 1);
                        }
                        slashCount = 0;
                    }

                    if (ch == '.') {
                        ++dotCount;
                        continue;
                    }
                    else if (dotCount != 0) {

                        bool skipSegment = syntax.NotAny(UriSyntaxFlags.CanonicalizeAsFilePath) && (dotCount > 2 || ch != '/' || i == start);

                        //
                        // Cases:
                        // /./ or /...[....]/   = remove this segment as invalid
                        // /../                 = remove this segment, mark next for removal
                        // /....x               = DO NOT TOUCH, leave as is
                        // x.../                = remove trailing dots
                        //
                        if (!skipSegment && ch == '/') {
                            if (lastSlash == i+dotCount+1 || (lastSlash == 0 && i+dotCount+1 == destLength)) {
                                //
                                //  /./ or  /...[....]/ or /.<eos> or  /...[....]<eos>
                                //  Remove this segment (note that /.../ is an invalid segment, remove it since
                                //  we should not throw at this parsing point.
                                //
                                // just reusing a variable slot we perform //dest.Remove(i+1, dotCount + (lastSlash==0?0:1));
                                lastSlash = (ushort)(i + 1 + dotCount + (lastSlash==0?0:1));
                                Buffer.BlockCopy(dest, lastSlash<<1, dest, (i+1)<<1, (destLength - lastSlash)<<1);
                                destLength -= (lastSlash-i-1);

                                lastSlash = i;
                                if (dotCount == 2) {
                                    //
                                    // We have 2 dots in between like /../ or /..<eos>,
                                    // Mark next segment for removal and remove this /../ or /..
                                    //
                                    ++removeSegments;
                                }
                                dotCount = 0;
                                continue;
                            }
                        }
                        // Note if removeSegments!=0, then ignore and remove the whole segment later
                        else if (!skipSegment && removeSegments == 0 && (lastSlash == i+dotCount+1 || (lastSlash == 0 && i+dotCount+1 == destLength))) {
                            //
                            // x.../  or  x...<eos>
                            // remove trailing dots
                            //
                            //
                            // just reusing a variable slot we perform //dest.Remove(i+1, dotCount);
                            dotCount = (ushort)(i + 1 + dotCount);
                            Buffer.BlockCopy(dest, dotCount<<1, dest, (i+1)<<1, (destLength - dotCount)<<1);
                            destLength -= (dotCount-i-1);
                            lastSlash = 0;  //the other dots in this segment will stay intact
                            dotCount = 0;
                            continue;
                        }
                        dotCount = 0;
                        //
                        // Here all other cases go such as
                        // x.[..]y or /.[..]x or (/x.[...][/] && removeSegments !=0)
                    }

                    //
                    // Now we may want to remove a segment because of previous /../
                    //
                    if (ch == '/') {
                        if (removeSegments  != 0) {
                            --removeSegments;

                            // just reusing a variable slot we perform //dest.Remove(i+1, lastSlash - i);
                            lastSlash = (ushort)(lastSlash + 1);
                            Buffer.BlockCopy(dest, lastSlash<<1, dest, (i+1)<<1, (destLength - lastSlash)<<1);
                            destLength -= (lastSlash-i-1);
                        }
                    lastSlash = i;
                    }
                }

               start = (ushort)((ushort)start + (ushort)1);
            } //end of unchecked

            if ((ushort)destLength > start && syntax.InFact(UriSyntaxFlags.CanonicalizeAsFilePath))
            {
                if (slashCount > 1) {
                    Buffer.BlockCopy(dest, lastSlash<<1, dest, start<<1, (destLength - lastSlash)<<1);
                    destLength -= (slashCount-1);
                }
                else if (removeSegments != 0 && dest[start] != '/') {
                    //remove first not rooted segment
                    // dest.Remove(i+1, lastSlash - i);
                    lastSlash = (ushort)(lastSlash + 1);
                    Buffer.BlockCopy(dest, lastSlash<<1, dest, start<<1, (destLength - lastSlash)<<1);
                    destLength -= lastSlash;
                }
                else if (dotCount != 0) {
                    // If final string starts with a segment looking like .[...]/ or .[...]<eos>
                    // then we remove this fisrt segment
                    if (lastSlash == dotCount+1 || (lastSlash == 0 && dotCount + 1 == destLength)) {
                        //dest.Remove(0, dotCount + (lastSlash==0?0:1));
                        dotCount = (ushort)(dotCount + (lastSlash==0?0:1));
                        Buffer.BlockCopy(dest, dotCount<<1, dest, start<<1, (destLength - dotCount)<<1);
                        destLength -= dotCount;
                    }
                }
            }
            return dest;
        }
        //
        //
        //
        //
        private static readonly char[] HexUpperChars = {
                                   '0', '1', '2', '3', '4', '5', '6', '7',
                                   '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
                                   };

        //used by DigestClient
        internal static readonly char[] HexLowerChars = {
                                   '0', '1', '2', '3', '4', '5', '6', '7',
                                   '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
                                   };


        //
        //
        //
        //
        private static void EscapeAsciiChar(char ch, char[] to, ref int pos) {
            to[pos++] = '%';
            to[pos++] = HexUpperChars[(ch & 0xf0) >> 4];
            to[pos++] = HexUpperChars[ch & 0xf];
        }

        internal static int CalculateCaseInsensitiveHashCode(string text)
        {
            return StringComparer.InvariantCultureIgnoreCase.GetHashCode(text);
        }
        //
        // CombineUri
        //
        //  Given 2 URI strings, combine them into a single resultant URI string
        //
        // Inputs:
        //  <argument>  basePart
        //      Base URI to combine with
        //
        //  <argument>  relativePart
        //      String expected to be relative URI
        //
        // Assumes:
        //  <basePart> is in canonic form
        //
        // Returns:
        //  Resulting combined URI string
        //
        private static string CombineUri(Uri basePart, string relativePart, UriFormat uriFormat) {
           //NB: relativePart is ensured as not empty by the caller
           //    Another assumption is that basePart is an AbsoluteUri

           // This method was not optimized for efficiency
           // Means a relative Uri ctor may be relatively slow plus it increases the footprint of the baseUri

           char c1 = relativePart[0];

#if !PLATFORM_UNIX
           //check a special case for the base as DOS path and a rooted relative string
           if ( basePart.IsDosPath &&
               (c1 == '/' || c1 == '\\') &&
               (relativePart.Length == 1 || (relativePart[1] != '/' && relativePart[1] != '\\')))
           {
               // take relative part appended to the base string after the drive letter
               int idx = basePart.OriginalString.IndexOf(':');
               if (basePart.IsImplicitFile) {
                   return basePart.OriginalString.Substring(0, idx+1 ) + relativePart;
               }
               // The basePart has explicit scheme (could be not file:), take the DOS drive ':' position
               idx = basePart.OriginalString.IndexOf(':', idx+1);
               return basePart.OriginalString.Substring(0, idx+1 ) + relativePart;
           }
#endif // !PLATFORM_UNIX

           // Check special case for Unc or absolute path in relativePart when base is FILE
           if (StaticIsFile(basePart.Syntax))
           {

                if (c1 == '\\' || c1 == '/') {

                    if(relativePart.Length >= 2 && (relativePart[1] == '\\' || relativePart[1] == '/')) {
                        //Assuming relative is a Unc path and base is a file uri.
                        return basePart.IsImplicitFile? relativePart: "file:" + relativePart;
                    }

                    // here we got an absolute path in relativePart,
                    // For compatibility with V1.0 parser we restrict the compression scope to Unc Share, i.e. \\host\share\
                    if (basePart.IsUnc) {
                        string share = basePart.GetParts(UriComponents.Path | UriComponents.KeepDelimiter, UriFormat.Unescaped);
                        for (int i = 1; i < share.Length; ++i) {
                            if (share[i] == '/') {
                                share = share.Substring(0, i);
                                break;
                            }
                        }
                        if (basePart.IsImplicitFile) {
                            return  @"\\"
                                    + basePart.GetParts(UriComponents.Host, UriFormat.Unescaped)
                                    + share
                                    + relativePart;
                        }
                        return  "file://"
                                + basePart.GetParts(UriComponents.Host, uriFormat)
                                + share
                                + relativePart;

                    }
                    // It's not obvious but we've checked (for this relativePart format) that baseUti is nor UNC nor DOS path
                    //
                    // Means base is a Unix style path and, btw, IsImplicitFile cannot be the case either
                    return "file://" + relativePart;
               }
           }

           // If we are here we did not recognize absolute DOS/UNC path for a file: base uri
           // Note that DOS path may still happen in the relativePart and if so it may override the base uri scheme.

           bool convBackSlashes =  basePart.Syntax.InFact(UriSyntaxFlags.ConvertPathSlashes);

           string left = null;

           // check for network or local absolute path
           if (c1 == '/' || (c1 == '\\' && convBackSlashes)) {
               if (relativePart.Length >= 2 && relativePart[1] == '/') {
                   // got an authority in relative path and the base scheme is not file (checked)
                   return basePart.Scheme + ':' + relativePart;
               }

               // Got absolute relative path, and the base is nor FILE nor a DOS path (checked at the method start)
               if (basePart.HostType == Flags.IPv6HostType) {
                   left =  basePart.GetParts(UriComponents.Scheme|UriComponents.UserInfo, uriFormat)
                                    + '[' + basePart.DnsSafeHost + ']'
                                    + basePart.GetParts(UriComponents.KeepDelimiter|UriComponents.Port, uriFormat);
               }
               else {
                   left =  basePart.GetParts(UriComponents.SchemeAndServer|UriComponents.UserInfo, uriFormat);
               }
               if (convBackSlashes && c1 == '\\')
                   relativePart = '/' + relativePart.Substring(1);

               return left + relativePart;
           }

           // Here we got a relative path or just a query+[fragment]
           // Need to run path Compression because this is how relative Uri combining works

           // Take the base part path up to and including the last slash
           left = basePart.GetParts(UriComponents.Path | UriComponents.KeepDelimiter, basePart.IsImplicitFile? UriFormat.Unescaped: uriFormat);
           int length = left.Length;
           char[] path = new char[length + relativePart.Length];

           if (length > 0) {
               left.CopyTo(0, path, 0, length);
               while(length > 0) {
                   if (path[--length] == '/') {
                       ++length;
                       break;
                   }
               }
           }

           //Append relative path to the result
           relativePart.CopyTo(0, path, length, relativePart.Length);

           // Split relative on path and extra (for compression)
           c1 = basePart.Syntax.InFact(UriSyntaxFlags.MayHaveQuery)? '?': c_DummyChar;

           char c2 = (!basePart.IsImplicitFile && basePart.Syntax.InFact(UriSyntaxFlags.MayHaveFragment))? '#': c_DummyChar;
           string extra = String.Empty;

           // assuming c_DummyChar may not happen in an unicode uri string
           if (!(c1 == c_DummyChar && c2 == c_DummyChar)) {
               int i=0;
               for (;i < relativePart.Length; ++i) {
                   if (path[length + i] == c1 || path[length + i] == c2) {
                       break;
                   }
               }
               if (i == 0) {
                   extra = relativePart;
               }
               else if (i < relativePart.Length) {
                   extra = relativePart.Substring(i);
               }
               length += i;
           }
           else {
               length += relativePart.Length;
           }

           // Take the base part up to the path
           if (basePart.HostType == Flags.IPv6HostType) {
               if (basePart.IsImplicitFile) {
                   left =  @"\\[" + basePart.DnsSafeHost + ']';
               }
               else {
                   left =  basePart.GetParts(UriComponents.Scheme|UriComponents.UserInfo, uriFormat)
                           + '[' + basePart.DnsSafeHost + ']'
                           + basePart.GetParts(UriComponents.KeepDelimiter|UriComponents.Port, uriFormat);
               }
           }
           else {
               if (basePart.IsImplicitFile) {
#if !PLATFORM_UNIX
                   if (basePart.IsDosPath) {
                       // The FILE DOS path comes as /c:/path, we have to exclude first 3 chars from compression
                       path = Compress(path, 3, ref length, basePart.Syntax);
                       return new string(path, 1, length-1) + extra;
                   }
                   else {
                       left =  @"\\" + basePart.GetParts(UriComponents.Host, UriFormat.Unescaped);
                   }
#else
                   left =  basePart.GetParts(UriComponents.Host, UriFormat.Unescaped);
#endif // !PLATFORM_UNIX

               }
               else {
                   left =  basePart.GetParts(UriComponents.SchemeAndServer|UriComponents.UserInfo, uriFormat);
               }
           }
           //compress the path
           path = Compress(path, basePart.SecuredPathIndex, ref length, basePart.Syntax);
           return left + new string(path, 0, length) + extra;
        }

        //
        //
        //
        //
        //
        private static string PathDifference(string path1, string path2, bool compareCase) {

            int i;
            int si = -1;

            for (i = 0; (i < path1.Length) && (i < path2.Length); ++i) {
                if ((path1[i] != path2[i]) && (compareCase || (Char.ToLower(path1[i], CultureInfo.InvariantCulture) != Char.ToLower(path2[i], CultureInfo.InvariantCulture))))
                {
                    break;

                } else if (path1[i] == '/') {
                    si = i;
                }
            }

            if (i == 0) {
                return path2;
            }
            if ((i == path1.Length) && (i == path2.Length)) {
                return String.Empty;
            }

            StringBuilder relPath = new StringBuilder();

            for (; i < path1.Length; ++i) {
                if (path1[i] == '/') {
                    relPath.Append("../");
                }
            }
            return relPath.ToString() + path2.Substring(si + 1);
        }

        //Used by Uribuilder
        internal bool HasAuthority {
            get {
                return InFact(Flags.AuthorityFound);
            }
        }

        private static readonly char[] _WSchars = new char[] {' ', '\n', '\r', '\t'};
        private static bool IsLWS(char ch) {

            return (ch <= ' ') && (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t');
        }

        private static bool IsAsciiLetter(char character) {

            return (character >= 'a' && character <= 'z') ||
                   (character >= 'A' && character <= 'Z');
        }

        private static bool IsAsciiLetterOrDigit(char character) {
            return IsAsciiLetter(character) || (character >= '0' && character <= '9');
        }

        /// <internalonly/>
        [Obsolete("The method has been deprecated. It is not used by the system. http://go.microsoft.com/fwlink/?linkid=14202")]
        protected virtual void Parse()
        {
        }
        /// <internalonly/>
        [Obsolete("The method has been deprecated. It is not used by the system. http://go.microsoft.com/fwlink/?linkid=14202")]
        protected virtual void Canonicalize()
        {
        }
        /// <internalonly/>
        [Obsolete("The method has been deprecated. It is not used by the system. http://go.microsoft.com/fwlink/?linkid=14202")]
        protected virtual void Escape()
        {
        }
        //
        // Unescape
        //
        //  Convert any escape sequences in <path>. Escape sequences can be
        //  hex encoded reserved characters (e.g. %40 == '@') or hex encoded
        //  UTF-8 sequences (e.g. %C4%D2 == 'Latin capital Ligature Ij')
        //
        /// <internalonly/>
        [Obsolete("The method has been deprecated. Please use GetComponents() or static UnescapeDataString() to unescape a Uri component or a string. http://go.microsoft.com/fwlink/?linkid=14202")]
        protected virtual string Unescape(string path) {

            // alexeiv cr: This method is dangerous since it gives path unescaping control
            // to the derived class without any permission demand.
            // Should be deprecated and removed asap.

            char[] dest = new char[path.Length];
            int count = 0;
            dest = UnescapeString(path, 0, path.Length, dest, ref count, c_DummyChar, c_DummyChar, c_DummyChar, UnescapeMode.Unescape | UnescapeMode.UnescapeAll);
            return new string(dest, 0, count);
        }

        [Obsolete("The method has been deprecated. Please use GetComponents() or static EscapeUriString() to escape a Uri component or a string. http://go.microsoft.com/fwlink/?linkid=14202")]
        protected static string EscapeString(string str) {

            // alexeiv cr: This method just does not make sense sa protected
            // It should go public static asap

            if ((object)str == null) {
                return string.Empty;
            }

            int destStart = 0;
            char[] dest = EscapeString(str, 0, str.Length, null, ref destStart, true, '?', '#', '%');
            if ((object)dest == null)
                return str;
            return new string(dest, 0, destStart);
        }

        //
        // CheckSecurity
        //
        //  Check for any invalid or problematic character sequences
        //
        /// <internalonly/>
        [Obsolete("The method has been deprecated. It is not used by the system. http://go.microsoft.com/fwlink/?linkid=14202")]
        protected virtual void CheckSecurity()  {

            // alexeiv cr: This method just does not make sense
            // Should be deprecated and removed asap.

            if (Scheme == "telnet") {

                //
                // remove everything after ';' for telnet
                //

            }
        }

        //
        // IsReservedCharacter
        //
        //  Determine whether a character is part of the reserved set
        //
        // Returns:
        //  true if <character> is reserved else false
        //
        /// <internalonly/>
        [Obsolete("The method has been deprecated. It is not used by the system. http://go.microsoft.com/fwlink/?linkid=14202")]
        protected virtual bool IsReservedCharacter(char character) {

            // alexeiv cr: This method just does not make sense as virtual protected
            // It should go public static asap

            return (character == ';')
                || (character == '/')
                || (character == ':')
                || (character == '@')   // OK FS char
                || (character == '&')
                || (character == '=')
                || (character == '+')   // OK FS char
                || (character == '$')   // OK FS char
                || (character == ',')
                ;
        }

        //
        // IsExcludedCharacter
        //
        //  Determine if a character should be exluded from a URI and therefore be
        //  escaped
        //
        // Returns:
        //  true if <character> should be escaped else false
        //
        /// <internalonly/>
        [Obsolete("The method has been deprecated. It is not used by the system. http://go.microsoft.com/fwlink/?linkid=14202")]
        protected static bool IsExcludedCharacter(char character) {

            // alexeiv cr: This method just does not make sense sa protected
            // It should go public static asap

            //
            // the excluded characters...
            //

            return (character <= 0x20)
                || (character >= 0x7f)
                || (character == '<')
                || (character == '>')
                || (character == '#')
                || (character == '%')
                || (character == '"')

                //
                // the 'unwise' characters...
                //

                || (character == '{')
                || (character == '}')
                || (character == '|')
                || (character == '\\')
                || (character == '^')
                || (character == '[')
                || (character == ']')
                || (character == '`')
                ;
        }

        //
        // IsBadFileSystemCharacter
        //
        //  Determine whether a character would be an invalid character if used in
        //  a file system name. Note, this is really based on NTFS rules
        //
        // Returns:
        //  true if <character> would be a treated as a bad file system character
        //  else false
        //
        [Obsolete("The method has been deprecated. It is not used by the system. http://go.microsoft.com/fwlink/?linkid=14202")]
        protected virtual bool IsBadFileSystemCharacter(char character) {

            // alexeiv cr: This method just does not make sense sa protected virtual
            // It should go public static asap

            return (character < 0x20)
                || (character == ';')
                || (character == '/')
                || (character == '?')
                || (character == ':')
                || (character == '&')
                || (character == '=')
                || (character == ',')
                || (character == '*')
                || (character == '<')
                || (character == '>')
                || (character == '"')
                || (character == '|')
                || (character == '\\')
                || (character == '^')
                ;
        }


    } // class Uri
} // namespace System

