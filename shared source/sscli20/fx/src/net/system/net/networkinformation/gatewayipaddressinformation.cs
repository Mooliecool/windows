//------------------------------------------------------------------------------
// <copyright file="GatewayIPAddressInformation.cs" company="Microsoft">
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

namespace System.Net.NetworkInformation
{
           
    /// Provides information about a network interface address.
    public abstract class GatewayIPAddressInformation
    {
        /// Gets the Internet Protocol (IP) address.
        public abstract IPAddress Address {get;}
    }
}

