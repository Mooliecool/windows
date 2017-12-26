//------------------------------------------------------------------------------
// <copyright file="GenericUriParser.cs" company="Microsoft">
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

    This is a public sealed class that exposes various Uri parsing options
    suported by built in Uri parser

Author:
    Alexei Vopilov    Jul 26 2004

Revision History:
--*/



namespace System {

    using System.Globalization;
    using System.Collections;
    using System.Security.Permissions;

    //
    // This enum specifies the public options used to customize a hierarchical built-in parser.
    //
    [Flags]
    public enum GenericUriParserOptions
    {
        // A hierarchical URI, allows a userinfo, non empty Inet-based authority, path, query and fragment
        // The URI path gets agressively compressed means dots, slashes and backslashes are unescaped,
        // backslashesare converted, and then it compresses the path. It also removes trailing dots,
        // empty segments and dots-only segments
        Default                         = 0x0,

        // Allows a free style authority that would terminate with '/'
        GenericAuthority           = 0x1,

        // Allows an empty authority foo:///
        AllowEmptyAuthority        = 0x2,

        // Disables a user info component, it implied in the case of GenericAuthority flag
        NoUserInfo                 = 0x4,

        // Disables a port component, it is implied in the case of GenericAuthority flag
        NoPort                     = 0x8,

        // Disables a query. A ? char is considered as part of the path and is escaped
        NoQuery                    = 0x10,

        // Disables a fragment. A # char is considered as part of the path or query and is escaped
        NoFragment                 = 0x20,

        // if false then converta \ to /, otheriwse does this conversion for the Path component.
        DontConvertPathBackslashes = 0x40,

        // if false, then a/./b or a/.../b becomes a/b and /a/../b becomes /b
        DontCompressPath           = 0x80,

        // if false  then a/%2e./b  becomes a/../b and then usually compressed
        DontUnescapePathDotsAndSlashes= 0x100
    }
    //
    // A hierachical Uri parser that supports various customization options
    //
    // ATTN: This type must be compile-time registered with UriParser.CheckSetIsSimpleFlag() method
    //      to avoid calling into the user code if there is no one.
    //
    public class GenericUriParser: UriParser
    {
        //
        // The only availabe .ctor.
        //
        public GenericUriParser(GenericUriParserOptions options)
            : base(MapGenericParserOptions(options))
        {
        }
        //
        private static UriSyntaxFlags MapGenericParserOptions(GenericUriParserOptions options)
        {
            //
            // Here we map public flags to internal ones
            // Note an instacne of this parser is always a "simple parser" since the class is sealed.
            //
            UriSyntaxFlags flags = DefaultGenericUriParserFlags;

            if ((options & GenericUriParserOptions.GenericAuthority) != 0)
            {
                // Disable some options that are not compatible with generic authority
                flags &= ~(UriSyntaxFlags.MayHaveUserInfo | UriSyntaxFlags.MayHavePort | UriSyntaxFlags.AllowUncHost | UriSyntaxFlags.AllowAnInternetHost);
                flags |= UriSyntaxFlags.AllowAnyOtherHost;
            }

            if ((options & GenericUriParserOptions.AllowEmptyAuthority) != 0)
            {
                flags |= UriSyntaxFlags.AllowEmptyHost;
            }

            if ((options & GenericUriParserOptions.NoUserInfo) != 0)
            {
                flags &= ~UriSyntaxFlags.MayHaveUserInfo;
            }

            if ((options & GenericUriParserOptions.NoPort) != 0)
            {
                flags &= ~UriSyntaxFlags.MayHavePort;
            }

            if ((options & GenericUriParserOptions.NoQuery) != 0)
            {
                flags &= ~UriSyntaxFlags.MayHaveQuery;
            }

            if ((options & GenericUriParserOptions.NoFragment) != 0)
            {
                flags &= ~UriSyntaxFlags.MayHaveFragment;
            }

            if ((options & GenericUriParserOptions.DontConvertPathBackslashes) != 0)
            {
                flags &= ~UriSyntaxFlags.ConvertPathSlashes;
            }

            if ((options & GenericUriParserOptions.DontCompressPath) != 0)
            {
                flags &= ~(UriSyntaxFlags.CompressPath | UriSyntaxFlags.CanonicalizeAsFilePath);
            }

            if ((options & GenericUriParserOptions.DontUnescapePathDotsAndSlashes) != 0)
            {
                flags &= ~UriSyntaxFlags.UnEscapeDotsAndSlashes;
            }

            return flags;
        }

        private const UriSyntaxFlags DefaultGenericUriParserFlags =
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


    }
}

