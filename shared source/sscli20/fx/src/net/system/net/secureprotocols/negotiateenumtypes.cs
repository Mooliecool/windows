//------------------------------------------------------------------------------
// <copyright file="NegoEnumProperties.cs" company="Microsoft">
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

namespace System.Net.Security {


    
    //
    // WebRequest - specific authentication flags
    //
    public enum AuthenticationLevel {
        None = 0,
        MutualAuthRequested = 1,    // default setting
        MutualAuthRequired  = 2
    }

    // This will request security properties of a NegotiateStream
    public enum ProtectionLevel
    {
        // Used only with Negotiate on Win9x platform
        None = 0,

        // Data integrity only
        Sign = 1,

        // Both data confidentiality and integrity
        EncryptAndSign = 2
    }

}

