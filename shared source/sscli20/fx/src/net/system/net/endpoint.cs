//------------------------------------------------------------------------------
// <copyright file="EndPoint.cs" company="Microsoft">
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

using System;
using System.Runtime.InteropServices;
using System.Net.Sockets;

namespace System.Net {

    // Generic abstraction to identify network addresses

    /// <devdoc>
    ///    <para>
    ///       Identifies a network address.
    ///    </para>
    /// </devdoc>
    [Serializable]
    public abstract class EndPoint {
        /// <devdoc>
        ///    <para>
        ///       Returns the Address Family to which the EndPoint belongs.
        ///    </para>
        /// </devdoc>

        public virtual AddressFamily AddressFamily {
            get {
                throw ExceptionHelper.PropertyNotImplementedException;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Serializes EndPoint information into a SocketAddress structure.
        ///    </para>
        /// </devdoc>
        public virtual SocketAddress Serialize() {
            throw ExceptionHelper.MethodNotImplementedException;
        }

        /// <devdoc>
        ///    <para>
        ///       Creates an EndPoint instance from a SocketAddress structure.
        ///    </para>
        /// </devdoc>
        public virtual EndPoint Create(SocketAddress socketAddress) {
            throw ExceptionHelper.MethodNotImplementedException;
        }

    }; // abstract class EndPoint


} // namespace System.Net

