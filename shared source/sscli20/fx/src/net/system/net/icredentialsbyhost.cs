//------------------------------------------------------------------------------
// <copyright file="ICredentials.cs" company="Microsoft">
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

    //using System;
    //using System.Net;
    using System.Runtime.InteropServices;

    //
    // This is an extensible interface that authenticators
    // must implement to support credential lookup.
    // During execution of the protocol, if authentication
    // information is needed the GetCredential function will
    // be called with the host and realm information.
    //

    /// <devdoc>
    ///    <para>Provides the base authentication interface for Web client authentication.</para>
    /// </devdoc>
    public interface ICredentialsByHost {
        /// <devdoc>
        ///    <para>
        ///       Returns a NetworkCredential object that
        ///       is associated with the supplied host, realm, and authentication type.
        ///    </para>
        /// </devdoc>

        //
        // CONVENTION:
        // returns null if no information is available
        // for the specified host&realm
        //
        NetworkCredential GetCredential(string host, int port, string authenticationType);

    } // interface ICredentials


} // namespace System.Net
