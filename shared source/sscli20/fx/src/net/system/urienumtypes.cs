//------------------------------------------------------------------------------
// <copyright file="UriEnumTypes.cs" company="Microsoft">
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

    Defines enum types used by System.Uri class

Author:
    Alexei Vopilov    Nov 21 2003

Revision History:

--*/
namespace System
{

    // Used to control whether absolu
    public enum UriKind {
        RelativeOrAbsolute  = 0,
        Absolute            = 1,
        Relative            = 2
    }

    [Flags]
    public enum UriComponents
    {
        // Generic parts.
        // ATTN: The values must stay in sync with Uri.Flags.xxxNotCanonical
        Scheme      = 0x1,
        UserInfo    = 0x2,
        Host        = 0x4,
        Port        = 0x8,
        Path        = 0x10,
        Query       = 0x20,
        Fragment    = 0x40,

        StrongPort  = 0x80,

        // This will also return respective delimiters for scheme, userinfo or port
        // Valid only for a single componet requests.
        KeepDelimiter = 0x40000000,

        // This is used by GetObjectData and can also be used directly.
        // Works for both absolute and relaitve Uris
        SerializationInfoString = unchecked((int)0x80000000),

        // Shortcuts for general cases
        AbsoluteUri = Scheme | UserInfo | Host | Port | Path | Query | Fragment,
        HostAndPort = Host | StrongPort,                //includes port even if default
        StrongAuthority = UserInfo | Host | StrongPort, //includes port even if default
        SchemeAndServer = Scheme | Host | Port,
        HttpRequestUrl = Scheme | Host | Port | Path | Query,
        PathAndQuery = Path | Query,
    }
    public enum UriFormat
    {
        UriEscaped = 1,
        Unescaped = 2,      // Completely unescaped.
        SafeUnescaped = 3

    }
}
