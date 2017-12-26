//------------------------------------------------------------------------------
// <copyright file="SocketOptionLevel.cs" company="Microsoft">
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

namespace System.Net.Sockets {
    using System;

    //
    // Option flags per-socket.
    //

    /// <devdoc>
    ///    <para>
    ///       Defines socket option levels for the <see cref='System.Net.Sockets.Socket'/> class.
    ///    </para>
    /// </devdoc>
    //UEUE
    public enum SocketOptionLevel {

        /// <devdoc>
        ///    <para>
        ///       Indicates socket options apply to the socket itself.
        ///    </para>
        /// </devdoc>
        Socket = 0xffff,

        /// <devdoc>
        ///    <para>
        ///       Indicates socket options apply to IP sockets.
        ///    </para>
        /// </devdoc>
        IP = ProtocolType.IP,

        /// <devdoc>
        /// <para>
        /// Indicates socket options apply to IPv6 sockets.
        /// </para>
        /// </devdoc>
        IPv6 = ProtocolType.IPv6,

        /// <devdoc>
        ///    <para>
        ///       Indicates socket options apply to Tcp sockets.
        ///    </para>
        /// </devdoc>
        Tcp = ProtocolType.Tcp,

        /// <devdoc>
        /// <para>
        /// Indicates socket options apply to Udp sockets.
        /// </para>
        /// </devdoc>
        //UEUE
        Udp = ProtocolType.Udp,

    }; // enum SocketOptionLevel


} // namespace System.Net.Sockets
