//------------------------------------------------------------------------------
// <copyright file="TransportType.cs" company="Microsoft">
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
    using System;


    /// <devdoc>
    ///    <para>
    ///       Defines the transport type allowed for the socket.
    ///    </para>
    /// </devdoc>
    public  enum TransportType {
        /// <devdoc>
        ///    <para>
        ///       Udp connections are allowed.
        ///    </para>
        /// </devdoc>
        Udp     = 0x1,
        Connectionless = 1,
        /// <devdoc>
        ///    <para>
        ///       TCP connections are allowed.
        ///    </para>
        /// </devdoc>
        Tcp     = 0x2,
        ConnectionOriented = 2,
        /// <devdoc>
        ///    <para>
        ///       Any connection is allowed.
        ///    </para>
        /// </devdoc>
        All     = 0x3

    } // enum TransportType

} // namespace System.Net
