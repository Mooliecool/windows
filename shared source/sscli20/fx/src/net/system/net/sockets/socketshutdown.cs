//------------------------------------------------------------------------------
// <copyright file="SocketShutdown.cs" company="Microsoft">
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

    /// <devdoc>
    ///    <para>
    ///       Defines constants used by the <see cref='System.Net.Sockets.Socket.Shutdown'/> method.
    ///    </para>
    /// </devdoc>
    public enum SocketShutdown {
        /// <devdoc>
        ///    <para>
        ///       Shutdown sockets for receive.
        ///    </para>
        /// </devdoc>
        Receive   = 0x00,
        /// <devdoc>
        ///    <para>
        ///       Shutdown socket for send.
        ///    </para>
        /// </devdoc>
        Send      = 0x01,
        /// <devdoc>
        ///    <para>
        ///       Shutdown socket for both send and receive.
        ///    </para>
        /// </devdoc>
        Both      = 0x02,

    }; // enum SocketShutdown


} // namespace System.Net.Sockets
