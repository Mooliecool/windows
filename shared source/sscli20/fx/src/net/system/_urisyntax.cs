//------------------------------------------------------------------------------
// <copyright file="_UriSyntax.cs" company="Microsoft">
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

    //
    // This file utilizes partial class feature and contains
    // only internal implementation of UriParser type
    //

namespace System {

    using System.Globalization;
    using System.Collections;
    using System.Security.Permissions;

    // This enum specifies the Uri syntax flags that is understood by builtin Uri parser.
    [Flags]
    internal enum UriSyntaxFlags {
        MustHaveAuthority       = 0x1,  // must have "//" after scheme:
        OptionalAuthority       = 0x2,  // used by generic parser due to unknown Uri syntax
        MayHaveUserInfo         = 0x4,
        MayHavePort             = 0x8,
        MayHavePath             = 0x10,
        MayHaveQuery            = 0x20,
        MayHaveFragment         = 0x40,

        AllowEmptyHost          = 0x80,
        AllowUncHost            = 0x100,
        AllowDnsHost            = 0x200,
        AllowIPv4Host           = 0x400,
        AllowIPv6Host           = 0x800,
        AllowAnInternetHost     = AllowDnsHost|AllowIPv4Host|AllowIPv6Host,
        AllowAnyOtherHost       = 0x1000, // Relaxed authority syntax

        FileLikeUri             = 0x2000, //Special case to allow file:\\balbla or file://\\balbla
        MailToLikeUri           = 0x4000, //V1 parser inheritance mailTo:AuthorityButNoSlashes

        V1_UnknownUri           = 0x10000, // a Compatibility with V1 parser for an unknown scheme
        SimpleUserSyntax        = 0x20000, // It is safe to not call virtual UriParser methods
        BuiltInSyntax           = 0x40000, // This is a simple Uri plus it is hardcoded in the product
        ParserSchemeOnly        = 0x80000, // This is a Parser that does only Uri scheme parsing

        AllowDOSPath            = 0x100000,  // will check for "x:\"
        PathIsRooted            = 0x200000,  // For an authority based Uri the first path char is '/'
        ConvertPathSlashes      = 0x400000,  // will turn '\' into '/'
        CompressPath            = 0x800000,  // For an authority based Uri remove/compress /./ /../ in the path
        CanonicalizeAsFilePath  = 0x1000000, // remove/convert sequences /.../ /x../ /x./ dangerous for a DOS path
        UnEscapeDotsAndSlashes  = 0x2000000, // additionally unescape dots and slashes before doing path compression

//      KeepTailLWS             = 0x8000000,
    }
    //
    // Only internal members are included here
    //
    public abstract partial class UriParser {
        private static readonly System.Collections.Hashtable m_Table;
        private static System.Collections.Hashtable  m_TempTable;

        private     UriSyntaxFlags  m_Flags;
        private     int             m_Port;
        private     string          m_Scheme;

        internal const int  NoDefaultPort = -1;
        private  const int  c_InitialTableSize = 25;

        // These are always available without paying hashtable lookup cost
        // Note: see UpdateStaticSyntaxReference()
        internal static UriParser HttpUri;
        internal static UriParser HttpsUri;
        internal static UriParser FtpUri;
        internal static UriParser FileUri;
        internal static UriParser GopherUri;
        internal static UriParser NntpUri;
        internal static UriParser NewsUri;
        internal static UriParser MailToUri;
        internal static UriParser UuidUri;
        internal static UriParser TelnetUri;
        internal static UriParser LdapUri;
        internal static UriParser NetTcpUri;
        internal static UriParser NetPipeUri;

        internal static UriParser VsMacrosUri;

        static UriParser() {

            m_Table = new System.Collections.Hashtable(c_InitialTableSize);
            m_TempTable = new System.Collections.Hashtable(c_InitialTableSize);

            //Now we will call for the instance constructors that will interrupt this static one.

            // Below we simulate calls into FetchSyntax() but avoid using lock() and other things redundant for a .cctor

            HttpUri   = new BuiltInUriParser("http", 80, HttpSyntaxFlags);
            m_Table[HttpUri.SchemeName] = HttpUri;                   //HTTP

            HttpsUri  = new BuiltInUriParser("https", 443, HttpUri.m_Flags);
            m_Table[HttpsUri.SchemeName] = HttpsUri;                  //HTTPS cloned from HTTP

            FtpUri    = new BuiltInUriParser("ftp", 21, FtpSyntaxFlags);
            m_Table[FtpUri.SchemeName] = FtpUri;                    //FTP

            FileUri   = new BuiltInUriParser("file", NoDefaultPort, FileSyntaxFlags);
            m_Table[FileUri.SchemeName] = FileUri;                   //FILE

            GopherUri = new BuiltInUriParser("gopher", 70, GopherSyntaxFlags);
            m_Table[GopherUri.SchemeName] = GopherUri;                 //GOPHER

            NntpUri   = new BuiltInUriParser("nntp", 119, NntpSyntaxFlags);
            m_Table[NntpUri.SchemeName] = NntpUri;                   //NNTP

            NewsUri   = new BuiltInUriParser("news", NoDefaultPort, NewsSyntaxFlags);
            m_Table[NewsUri.SchemeName] = NewsUri;                   //NEWS

            MailToUri = new BuiltInUriParser("mailto", 25, MailtoSyntaxFlags);
            m_Table[MailToUri.SchemeName] = MailToUri;                 //MAILTO

            UuidUri   = new BuiltInUriParser("uuid", NoDefaultPort, NewsUri.m_Flags);
            m_Table[UuidUri.SchemeName] = UuidUri;                   //UUID cloned from NEWS

            TelnetUri = new BuiltInUriParser("telnet", 23, TelnetSyntaxFlags);
            m_Table[TelnetUri.SchemeName] = TelnetUri;                 //TELNET

            LdapUri   = new BuiltInUriParser("ldap", 389, LdapSyntaxFlags);
            m_Table[LdapUri.SchemeName] = LdapUri;                   //LDAP

            NetTcpUri   = new BuiltInUriParser("net.tcp", 808, NetTcpSyntaxFlags);
            m_Table[NetTcpUri.SchemeName] = NetTcpUri;   

            NetPipeUri   = new BuiltInUriParser("net.pipe", NoDefaultPort, NetPipeSyntaxFlags);
            m_Table[NetPipeUri.SchemeName] = NetPipeUri;   

            VsMacrosUri = new BuiltInUriParser("vsmacros", NoDefaultPort, VsmacrosSyntaxFlags);
            m_Table[VsMacrosUri.SchemeName] = VsMacrosUri;               //VSMACROS

        }
        //
        private class BuiltInUriParser: UriParser
        {
            //
            // All BuiltIn parsers use that ctor. They are marked with "simple" and "built-in" flags
            //
            internal BuiltInUriParser(string lwrCaseScheme, int defaultPort, UriSyntaxFlags syntaxFlags)
                : base ((syntaxFlags | UriSyntaxFlags.SimpleUserSyntax | UriSyntaxFlags.BuiltInSyntax))
            {
                m_Scheme = lwrCaseScheme;
                m_Port   = defaultPort;
            }
        }
        //
        internal UriSyntaxFlags Flags {
            get {
                return m_Flags;
            }
        }
        //
        internal bool NotAny(UriSyntaxFlags flags)
        {
            return (m_Flags & flags) == 0;
        }
        //
        internal bool InFact(UriSyntaxFlags flags)
        {
            return (m_Flags & flags) != 0;
        }
        //
        internal bool IsAllSet(UriSyntaxFlags flags)
        {
            return (m_Flags & flags) == flags;
        }
        //
        // Internal .ctor, any ctor eventually goes through this one
        //
        internal UriParser(UriSyntaxFlags flags)
        {
            m_Flags = flags;
            m_Scheme = string.Empty;
        }
        //
        private static void FetchSyntax(UriParser syntax, string lwrCaseSchemeName, int defaultPort)
        {
            if (syntax.SchemeName.Length != 0)
                throw new InvalidOperationException(SR.GetString(SR.net_uri_NeedFreshParser, syntax.SchemeName));

            lock (m_Table)
            {
                syntax.m_Flags &= ~UriSyntaxFlags.V1_UnknownUri;
                UriParser oldSyntax = (UriParser)m_Table[lwrCaseSchemeName];
                if (oldSyntax != null)
                    throw new InvalidOperationException(SR.GetString(SR.net_uri_AlreadyRegistered, oldSyntax.SchemeName));
                
                oldSyntax = (UriParser)m_TempTable[syntax.SchemeName];
                if (oldSyntax != null)
                {
                    // optimization on schemeName, will try to keep the first reference
                    lwrCaseSchemeName = oldSyntax.m_Scheme;
                    m_TempTable.Remove(lwrCaseSchemeName);
                }

                syntax.OnRegister(lwrCaseSchemeName, defaultPort);
                syntax.m_Scheme = lwrCaseSchemeName;
                syntax.CheckSetIsSimpleFlag();
                syntax.m_Port = defaultPort;

                m_Table[syntax.SchemeName] = syntax;
            }
        }
        //
        private const int c_MaxCapacity = 512;
        //schemeStr must be in lower case!
        internal static UriParser FindOrFetchAsUnknownV1Syntax(string lwrCaseScheme) {

            // check may be other thread just added one
            UriParser syntax = (UriParser)m_Table[lwrCaseScheme];
            if (syntax != null) {
                return syntax;
            }
            syntax = (UriParser)m_TempTable[lwrCaseScheme];
            if (syntax != null) {
                return syntax;
            }
            lock (m_Table) {
                if (m_TempTable.Count >= c_MaxCapacity) {
                    m_TempTable = new System.Collections.Hashtable(c_InitialTableSize);
                }
                syntax = new BuiltInUriParser(lwrCaseScheme, NoDefaultPort, UnknownV1SyntaxFlags);
                m_TempTable[lwrCaseScheme] = syntax;
                return syntax;
            }
        }
        //
        internal static UriParser GetSyntax(string lwrCaseScheme) {
            object ret = m_Table[lwrCaseScheme];
            if (ret == null) {
                ret = m_TempTable[lwrCaseScheme];
            }
            return (UriParser)ret;
        }
        //
        // Builtin and User Simple syntaxes do not need custom validation/parsing (i.e. virtual method calls),
        //
        internal bool IsSimple
        {
            get {
                return InFact(UriSyntaxFlags.SimpleUserSyntax);
            }
        }
        //
        internal void CheckSetIsSimpleFlag()
        {
            Type type  = this.GetType();

            if (type == typeof(GenericUriParser)     ||
                type == typeof(HttpStyleUriParser)   ||
                type == typeof(FtpStyleUriParser)    ||
                type == typeof(FileStyleUriParser)   ||
                type == typeof(NewsStyleUriParser)   ||
                type == typeof(GopherStyleUriParser) ||
                type == typeof(NetPipeStyleUriParser) ||
                type == typeof(NetTcpStyleUriParser) ||
                type == typeof(LdapStyleUriParser))
            {
                m_Flags |= UriSyntaxFlags.SimpleUserSyntax;
            }
        }
        //
        // Copied from Uri class
        //
        private static bool CheckSchemeName(string schemeName)
        {
            if (((object)schemeName == null) || (schemeName.Length == 0) || !IsAsciiLetter(schemeName[0]))
            {
                return false;
            }

            for (int i = schemeName.Length - 1; i > 0; --i)
            {
                if (!(IsAsciiLetterOrDigit(schemeName[i]) || (schemeName[i] == '+') || (schemeName[i] == '-') || (schemeName[i] == '.')))
                {
                    return false;
                }
            }

            return true;
        }
        private static bool IsAsciiLetter(char character)
        {
            return (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z');
        }
        private static bool IsAsciiLetterOrDigit(char character)
        {
            return IsAsciiLetter(character) || (character >= '0' && character <= '9');
        }
        //
        // These are simple internal wrappers that will call virtual protected methods
        // (to avoid "protected internal" siganures in the public docs)
        //
        internal UriParser InternalOnNewUri()
        {
            UriParser effectiveParser = OnNewUri();
            if ((object)this != (object)effectiveParser)
            {
                effectiveParser.m_Scheme = m_Scheme;
                effectiveParser.m_Port   = m_Port;
                effectiveParser.m_Flags  = m_Flags;
            }
            return effectiveParser;
        }
        //
        internal void InternalValidate(Uri thisUri, out UriFormatException parsingError)
        {
            InitializeAndValidate(thisUri, out parsingError);
        }
        //
        internal string InternalResolve(Uri thisBaseUri, Uri uriLink, out UriFormatException parsingError)
        {
            return Resolve(thisBaseUri, uriLink, out parsingError);
        }
        //
        internal bool InternalIsBaseOf(Uri thisBaseUri, Uri uriLink)
        {
            return IsBaseOf(thisBaseUri, uriLink);
        }
        //
        internal string InternalGetComponents(Uri thisUri, UriComponents uriComponents, UriFormat uriFormat)
        {
            return GetComponents(thisUri, uriComponents, uriFormat);
        }
        //
        internal bool InternalIsWellFormedOriginalString(Uri thisUri)
        {
            return IsWellFormedOriginalString(thisUri);
        }
        //
        // Various Uri scheme syntax flags
        //
        private const UriSyntaxFlags UnknownV1SyntaxFlags =
                                            UriSyntaxFlags.V1_UnknownUri | // This flag must be always set here
                                            UriSyntaxFlags.OptionalAuthority |
                                            //
                                            UriSyntaxFlags.MayHaveUserInfo |
                                            UriSyntaxFlags.MayHavePort |
                                            UriSyntaxFlags.MayHavePath |
                                            UriSyntaxFlags.MayHaveQuery |
                                            UriSyntaxFlags.MayHaveFragment |
                                            //
                                            UriSyntaxFlags.AllowEmptyHost |
                                            UriSyntaxFlags.AllowUncHost |
                                            UriSyntaxFlags.AllowAnInternetHost |
                                            // UriSyntaxFlags.AllowAnyOtherHost | // V1.1 has a bug and so does not support this case
                                            //
                                            UriSyntaxFlags.PathIsRooted |
                                            UriSyntaxFlags.AllowDOSPath |
                                            UriSyntaxFlags.ConvertPathSlashes |  // V1 compat, it will always convert backslashes
                                            UriSyntaxFlags.CompressPath;         // V1 compat, it will always compress path even for non hierarchical Uris

        private  const UriSyntaxFlags HttpSyntaxFlags =
                                        UriSyntaxFlags.MustHaveAuthority |
                                        //
                                        UriSyntaxFlags.MayHaveUserInfo |
                                        UriSyntaxFlags.MayHavePort |
                                        UriSyntaxFlags.MayHavePath |
                                        UriSyntaxFlags.MayHaveQuery |
                                        UriSyntaxFlags.MayHaveFragment |
                                        //
                                        UriSyntaxFlags.AllowUncHost |
                                        UriSyntaxFlags.AllowAnInternetHost |
                                        //
                                        UriSyntaxFlags.PathIsRooted |
                                        //
                                        UriSyntaxFlags.ConvertPathSlashes |
                                        UriSyntaxFlags.CompressPath |
                                        UriSyntaxFlags.CanonicalizeAsFilePath |
                                        UriSyntaxFlags.UnEscapeDotsAndSlashes;

        private  const UriSyntaxFlags FtpSyntaxFlags =
                                        UriSyntaxFlags.MustHaveAuthority |
                                        //
                                        UriSyntaxFlags.MayHaveUserInfo |
                                        UriSyntaxFlags.MayHavePort |
                                        UriSyntaxFlags.MayHavePath |
                                        UriSyntaxFlags.MayHaveFragment |
                                        //
                                        UriSyntaxFlags.AllowUncHost |
                                        UriSyntaxFlags.AllowAnInternetHost |
                                        //
                                        UriSyntaxFlags.PathIsRooted |
                                        //
                                        UriSyntaxFlags.ConvertPathSlashes |
                                        UriSyntaxFlags.CompressPath |
                                        UriSyntaxFlags.CanonicalizeAsFilePath;

        private  const UriSyntaxFlags FileSyntaxFlags =
                                        UriSyntaxFlags.MustHaveAuthority |
                                        //
                                        UriSyntaxFlags.AllowEmptyHost |
                                        UriSyntaxFlags.AllowUncHost |
                                        UriSyntaxFlags.AllowAnInternetHost |
                                        //
                                        UriSyntaxFlags.MayHavePath |
                                        UriSyntaxFlags.MayHaveFragment |
                                        //
                                        UriSyntaxFlags.FileLikeUri |
                                        //
                                        UriSyntaxFlags.PathIsRooted |
                                        UriSyntaxFlags.AllowDOSPath |
                                        //
                                        UriSyntaxFlags.ConvertPathSlashes |
                                        UriSyntaxFlags.CompressPath |
                                        UriSyntaxFlags.CanonicalizeAsFilePath |
                                        UriSyntaxFlags.UnEscapeDotsAndSlashes;

        private  const UriSyntaxFlags VsmacrosSyntaxFlags =
                                        UriSyntaxFlags.MustHaveAuthority |
                                        //
                                        UriSyntaxFlags.AllowEmptyHost |
                                        UriSyntaxFlags.AllowUncHost |
                                        UriSyntaxFlags.AllowAnInternetHost |
                                        //
                                        UriSyntaxFlags.MayHavePath |
                                        UriSyntaxFlags.MayHaveFragment |
                                        //
                                        UriSyntaxFlags.FileLikeUri |
                                        //
                                        UriSyntaxFlags.AllowDOSPath |
                                        UriSyntaxFlags.ConvertPathSlashes |
                                        UriSyntaxFlags.CompressPath |
                                        UriSyntaxFlags.CanonicalizeAsFilePath |
                                        UriSyntaxFlags.UnEscapeDotsAndSlashes;

        private  const UriSyntaxFlags GopherSyntaxFlags =
                                        UriSyntaxFlags.MustHaveAuthority |
                                        //
                                        UriSyntaxFlags.MayHaveUserInfo |
                                        UriSyntaxFlags.MayHavePort |
                                        UriSyntaxFlags.MayHavePath |
                                        UriSyntaxFlags.MayHaveFragment |
                                        //
                                        UriSyntaxFlags.AllowUncHost |
                                        UriSyntaxFlags.AllowAnInternetHost |
                                        //
                                        UriSyntaxFlags.PathIsRooted;
//                                        UriSyntaxFlags.KeepTailLWS |

        //Note that NNTP and NEWS are quite different in syntax
        private  const UriSyntaxFlags NewsSyntaxFlags =
                                        UriSyntaxFlags.MayHavePath |
                                        UriSyntaxFlags.MayHaveFragment;

        private  const UriSyntaxFlags NntpSyntaxFlags =
                                        UriSyntaxFlags.MustHaveAuthority |
                                        //
                                        UriSyntaxFlags.MayHaveUserInfo|
                                        UriSyntaxFlags.MayHavePort |
                                        UriSyntaxFlags.MayHavePath |
                                        UriSyntaxFlags.MayHaveFragment |
                                        //
                                        UriSyntaxFlags.AllowUncHost |
                                        UriSyntaxFlags.AllowAnInternetHost |
                                        //
                                        UriSyntaxFlags.PathIsRooted;

        private const UriSyntaxFlags TelnetSyntaxFlags =
                                        UriSyntaxFlags.MustHaveAuthority |
                                        //
                                        UriSyntaxFlags.MayHaveUserInfo|
                                        UriSyntaxFlags.MayHavePort |
                                        UriSyntaxFlags.MayHavePath |
                                        UriSyntaxFlags.MayHaveFragment |
                                        //
                                        UriSyntaxFlags.AllowUncHost |
                                        UriSyntaxFlags.AllowAnInternetHost |
                                        //
                                        UriSyntaxFlags.PathIsRooted;

        private const UriSyntaxFlags LdapSyntaxFlags =
                                        UriSyntaxFlags.MustHaveAuthority |
                                        //
                                        UriSyntaxFlags.AllowEmptyHost |
                                        UriSyntaxFlags.AllowUncHost |
                                        UriSyntaxFlags.AllowAnInternetHost |
                                        //
                                        UriSyntaxFlags.MayHaveUserInfo |
                                        UriSyntaxFlags.MayHavePort |
                                        UriSyntaxFlags.MayHavePath |
                                        UriSyntaxFlags.MayHaveQuery |
                                        UriSyntaxFlags.MayHaveFragment |
                                        //
                                        UriSyntaxFlags.PathIsRooted;

        private const UriSyntaxFlags MailtoSyntaxFlags =
                                        //
                                        UriSyntaxFlags.AllowEmptyHost |
                                        UriSyntaxFlags.AllowUncHost |
                                        UriSyntaxFlags.AllowAnInternetHost |
                                        //
                                        UriSyntaxFlags.MayHaveUserInfo |
                                        UriSyntaxFlags.MayHavePort |
                                        UriSyntaxFlags.MayHavePath |
                                        UriSyntaxFlags.MayHaveFragment |
                                        UriSyntaxFlags.MayHaveQuery |
                                        UriSyntaxFlags.MailToLikeUri;

        
        private const UriSyntaxFlags NetPipeSyntaxFlags = 
                                        UriSyntaxFlags.MustHaveAuthority |
                                        UriSyntaxFlags.MayHavePath |
                                        UriSyntaxFlags.MayHaveQuery |
                                        UriSyntaxFlags.MayHaveFragment |
                                        UriSyntaxFlags.AllowAnInternetHost |
                                        UriSyntaxFlags.PathIsRooted |
                                        UriSyntaxFlags.ConvertPathSlashes |
                                        UriSyntaxFlags.CompressPath |
                                        UriSyntaxFlags.CanonicalizeAsFilePath |
                                        UriSyntaxFlags.UnEscapeDotsAndSlashes;

    
        private const UriSyntaxFlags NetTcpSyntaxFlags = NetPipeSyntaxFlags | UriSyntaxFlags.MayHavePort;

    }
}

