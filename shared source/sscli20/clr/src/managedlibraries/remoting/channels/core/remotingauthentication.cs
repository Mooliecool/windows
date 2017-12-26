// ==++==
//
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
//
// ==--==
//==========================================================================
//  File:          RemotingAuthentication.cs
//
//  Summary:    Defines public types and helpers for remoting channel authentication
//
//  Classes:      
//
//==========================================================================
using System.Net;
using System.Net.Security;
using System.Security.Principal;

namespace System.Runtime.Remoting.Channels
{
    // SocketCachePolicy decides the socketCache timeout behaviour
    public enum SocketCachePolicy
    {
        // Default means v1.0 behaviour (relative timeouts)
        Default = 0,

        // AbsoluteTimeout means sockets would be timedout after absolute time
        AbsoluteTimeout = 1
    }

    // Authorization interface to add auth support for TCP
    public interface IAuthorizeRemotingConnection
    {
        bool IsConnectingEndPointAuthorized(EndPoint endPoint);
        bool IsConnectingIdentityAuthorized(IIdentity identity);
    }

}

